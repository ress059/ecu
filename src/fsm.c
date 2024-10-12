/**
 * @file
 * @brief See @ref fsm.h 
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-14
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/fsm.h"

/* Runtime asserts. */
#include "ecu/asserter.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- DEFINE FILE NAME FOR ASSERTER ----------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/fsm.c")



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PUBLIC FUNCTIONS -------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void ecu_fsm_ctor(struct ecu_fsm *me, 
                  const struct ecu_fsm_state *state_0)
{
    ECU_RUNTIME_ASSERT( (me && state_0) );
    ECU_RUNTIME_ASSERT( (state_0->handler) );
    me->state = state_0;
}


void ecu_fsm_state_ctor(struct ecu_fsm_state *me, 
                        ecu_fsm_on_entry_handler on_entry_0,
                        ecu_fsm_on_exit_handler on_exit_0,
                        ecu_fsm_state_handler handler_0)
{
    ECU_RUNTIME_ASSERT( (me && handler_0) );
    me->on_entry    = on_entry_0;   /* Optional so do not NULL assert. */
    me->on_exit     = on_exit_0;    /* Optional so do not NULL assert. */
    me->handler     = handler_0;    /* Mandatory. */
}


void ecu_fsm_dispatch(struct ecu_fsm *me, 
                      const struct ecu_event *event)
{
    enum ecu_fsm_status status = ECU_FSM_EVENT_IGNORED;
    const struct ecu_fsm_state *prev_state = (const struct ecu_fsm_state *)0;

    ECU_RUNTIME_ASSERT( (me && event) );
    ECU_RUNTIME_ASSERT( ((me->state) && (event->id >= ECU_VALID_EVENT_ID_BEGIN)) );
    ECU_RUNTIME_ASSERT( (me->state->handler) );

    /* Dispatch event to state. */
    prev_state = me->state;
    status = (*me->state->handler)(me, event);

    /* Handle State Transitions. */
    while (status == ECU_FSM_STATE_TRANSITION)
    {
        /* State can only be changed when user signals a transition so only assert here. */
        ECU_RUNTIME_ASSERT( (me->state) );
        ECU_RUNTIME_ASSERT( (me->state->handler) );

        /* Exit old state. */
        if (prev_state->on_exit)
        {
            (*prev_state->on_exit)(me);
        }

        /* Enter new state. */
        prev_state = me->state;
        if (me->state->on_entry)
        {
            status = (*me->state->on_entry)(me);

            /* Reject user from self-transitioning into current state 
            within entry handler. Pointless and causes infinite loop. */
            ECU_RUNTIME_ASSERT( ((status != ECU_FSM_STATE_TRANSITION) || \
                                 (prev_state != me->state)) );
        }
    }
}


enum ecu_fsm_status ecu_fsm_transition_to_state(struct ecu_fsm *me, 
                                                const struct ecu_fsm_state *state)
{
    ECU_RUNTIME_ASSERT( (me && state) );
    ECU_RUNTIME_ASSERT( (state->handler) );
    me->state = state;
    return ECU_FSM_STATE_TRANSITION;
}
