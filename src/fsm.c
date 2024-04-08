/**
 * @file
 * @brief @see fsm.h
 * @author Ian Ress
 * 
 */


/* Translation unit. */
#include <ecu/fsm.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static const struct ecu_event entry_evt = 
{
    .id = ECU_ENTRY_EVENT
};

static const struct ecu_event exit_evt = 
{
    .id = ECU_EXIT_EVENT
};

static struct ecu_assert_functor *assert_functor = ECU_DEFAULT_FUNCTOR;



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void ecu_fsm_ctor(struct ecu_fsm *fsm, ecu_fsm_func_ptr init_state_0, 
                  uint8_t max_state_transitions_0)
{
    ECU_RUNTIME_ASSERT( (fsm && init_state_0 && max_state_transitions_0), assert_functor );
    fsm->state = init_state_0;
    fsm->max_state_transitions = max_state_transitions_0;
}


void ecu_fsm_dispatch(struct ecu_fsm *fsm, const struct ecu_event *event)
{
    enum ecu_fsm_status status = ECU_FSM_ERROR;
    ecu_fsm_func_ptr prev_state;

    /* NULL assertions. Also reject user from dispatching reserved event. */
    ECU_RUNTIME_ASSERT( (fsm && event), assert_functor );
    ECU_RUNTIME_ASSERT( ((fsm->max_state_transitions) && (fsm->state) && \
                         (event->id >= ECU_VALID_EVENT_ID_BEGIN)), assert_functor );

    /* Dispatch event to state. */
    prev_state = fsm->state;
    status = (*fsm->state)(fsm, event);

    /* Handle State Transitions. */
    for (uint8_t i = 0; (status == ECU_FSM_STATE_TRANSITION && fsm->state && i < fsm->max_state_transitions); i++)
    {
        /* Run ECU_EXIT_EVENT of current state. Reject user from peforming state transition in ECU_EXIT_EVENT. */
        status = (*prev_state)(fsm, &exit_evt);
        ECU_RUNTIME_ASSERT( ((status != ECU_FSM_STATE_TRANSITION) && \
                             (status != ECU_FSM_ERROR)), assert_functor );

        /* Run ECU_ENTRY_EVENT of new state. */
        prev_state = fsm->state;
        status = (*fsm->state)(fsm, &entry_evt);
    }

    /* status != FSM_EVENT_TRANSITION verifies max state transitions were not exceeded. */
    ECU_RUNTIME_ASSERT( ((fsm->state) && (status != ECU_FSM_STATE_TRANSITION) && \
                         (status != ECU_FSM_ERROR)), assert_functor );
}


void ecu_fsm_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    assert_functor = functor;
}
