/**
 * @file
 * @brief @see fsm.h
 * @author Ian Ress
 * 
 */


/* Translation unit. */
#include <ecu/fsm.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ HELPER DEFINITIONS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static const struct event entry_evt = 
{
    .signal = ENTRY_EVENT
};

static const struct event exit_evt = 
{
    .signal = EXIT_EVENT
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void fsm_ctor(struct fsm *fsm, fsm_func_ptr init_state_0, 
              uint8_t max_state_transitions_0)
{
    RUNTIME_ASSERT( (fsm && init_state_0 && max_state_transitions_0) );
    fsm->state = init_state_0;
    fsm->max_state_transitions = max_state_transitions_0;
}


void fsm_dispatch(struct fsm *fsm, const struct event *event)
{
    enum fsm_status status = FSM_ERROR;
    fsm_func_ptr prev_state;

    /* NULL assertions. Also reject user from dispatching reserved event. */
    RUNTIME_ASSERT( (fsm && event) );
    RUNTIME_ASSERT( ((fsm->max_state_transitions) && (fsm->state) && (event->signal >= IDLE_EVENT)) );

    /* Dispatch event to state. */
    prev_state = fsm->state;
    status = (*fsm->state)(fsm, event);

    /* Handle State Transitions. */
    for (uint8_t i = 0; (status == FSM_EVENT_TRANSITION && fsm->state && i < fsm->max_state_transitions); i++)
    {
        /* Run EXIT_EVENT of current state. Reject user from peforming state transition in EXIT_EVENT. */
        status = (*prev_state)(fsm, &exit_evt);
        RUNTIME_ASSERT( ((status != FSM_EVENT_TRANSITION) && (status != FSM_ERROR)) );

        /* Run ENTRY_EVENT of new state. */
        prev_state = fsm->state;
        status = (*fsm->state)(fsm, &entry_evt);
    }

    /* status != FSM_EVENT_TRANSITION verifies max state transitions were not exceeded. */
    RUNTIME_ASSERT( ((fsm->state) && (status != FSM_EVENT_TRANSITION) && (status != FSM_ERROR)) );
}
