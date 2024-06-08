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
#include <ecu/fsm.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ FILE-SCOPE VARIABLES -----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *FSM_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PUBLIC FUNCTIONS -------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void ecu_fsm_ctor(struct ecu_fsm *me, 
                  const struct ecu_fsm_state *state_0)
{
    ECU_RUNTIME_ASSERT( (me && state_0), FSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (state_0->handler), FSM_ASSERT_FUNCTOR );
    me->state = state_0;
}


void ecu_fsm_state_ctor(struct ecu_fsm_state *me, 
                        ecu_fsm_on_entry_handler on_entry_0,
                        ecu_fsm_on_exit_handler on_exit_0,
                        ecu_fsm_state_handler handler_0)
{
    ECU_RUNTIME_ASSERT( (me && handler_0), FSM_ASSERT_FUNCTOR );
    me->on_entry    = on_entry_0;   /* Optional so do not NULL assert. */
    me->on_exit     = on_exit_0;    /* Optional so do not NULL assert. */
    me->handler     = handler_0;    /* Mandatory. */
}


void ecu_fsm_dispatch(struct ecu_fsm *me, 
                      const struct ecu_event *event)
{
    enum ecu_fsm_status status = ECU_FSM_EVENT_IGNORED;
    const struct ecu_fsm_state *prev_state = (const struct ecu_fsm_state *)0;

    ECU_RUNTIME_ASSERT( (me && event), FSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((me->state) && (event->id >= ECU_VALID_EVENT_ID_BEGIN)), FSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me->state->handler), FSM_ASSERT_FUNCTOR );

    /* Dispatch event to state. */
    prev_state = me->state;
    status = (*me->state->handler)(me, event);

    /* Handle State Transitions. */
    while (status == ECU_FSM_STATE_TRANSITION)
    {
        /* State can only be changed when user signals a transition so only assert here. */
        ECU_RUNTIME_ASSERT( (me->state), FSM_ASSERT_FUNCTOR );
        ECU_RUNTIME_ASSERT( (me->state->handler), FSM_ASSERT_FUNCTOR );

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
                                 (prev_state != me->state)), FSM_ASSERT_FUNCTOR );
        }
    }
}


enum ecu_fsm_status ecu_fsm_change_state(struct ecu_fsm *me, 
                                         const struct ecu_fsm_state *state)
{
    ECU_RUNTIME_ASSERT( (me && state), FSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (state->handler), FSM_ASSERT_FUNCTOR );
    me->state = state;
    return ECU_FSM_STATE_TRANSITION;
}


void ecu_fsm_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    FSM_ASSERT_FUNCTOR = functor;
}
