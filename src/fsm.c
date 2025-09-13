/**
 * @file
 * @brief 
 * @rst
 * See :ref:`fsm.h section <fsm_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-13
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/fsm.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("ecu/fsm.c")

/*------------------------------------------------------------*/
/*---------------------- FILE SCOPE TYPES --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Meaning of bits in @ref ecu_fsm.transition bitmap.
 * A set bit means that type of state transition is active.
 */
enum transition_type
{
    FSM_TRANSITION_TYPE_START = -1, /**< Used to prevent type-limits warning when compiling on targets that auto adjust enum types. */
    /************************/
    FSM_SELF_TRANSITION,
    FSM_STATE_TRANSITION,
    /************************/
    FSM_TRANSITION_TYPE_COUNT
};

/*------------------------------------------------------------*/
/*------------------------ STATIC ASSERTS --------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( ((size_t)FSM_TRANSITION_TYPE_COUNT <= (ECU_FIELD_SIZEOF(struct ecu_fsm, transition) * 8)),
                    "transition bitmap in ecu_fsm is too small. Largest value of "
                    "transition_type exceeds MSB of ecu_fsm transition bitmap." );

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied state has a non-NULL 
 * handler function. False otherwise.
 */
static bool state_is_valid(const struct ecu_fsm_state *state);

/**
 * @brief Returns true if no state transitions have
 * been signalled. False otherwise.
 */
static bool no_transitions_active(const struct ecu_fsm *fsm);

/**
 * @brief Returns true if a specific state transition 
 * type has been signalled. False otherwise.
 *
 * @param t State transition type to check.
 */
static bool transition_is_active(const struct ecu_fsm *fsm, enum transition_type t);

/**
 * @brief Signals that a specific state transition type
 * has been requested. 
 * 
 * @param t State transition type requested.
 */
static void set_transition(struct ecu_fsm *fsm, enum transition_type t);

/**
 * @brief Clears all @ref ecu_fsm transition bits.
 */
static void clear_all_transitions(struct ecu_fsm *fsm);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool state_is_valid(const struct ecu_fsm_state *state)
{
    ECU_ASSERT( (state) );
    return (state->handler);
}

static bool no_transitions_active(const struct ecu_fsm *fsm)
{
    ECU_ASSERT( (fsm) );
    return (fsm->transition == 0);
}

static bool transition_is_active(const struct ecu_fsm *fsm, enum transition_type t)
{
    ECU_ASSERT( (fsm) );
    ECU_ASSERT( ((t > FSM_TRANSITION_TYPE_START) && (t < FSM_TRANSITION_TYPE_COUNT)) );
    return (fsm->transition & (1U << t));
}

static void set_transition(struct ecu_fsm *fsm, enum transition_type t)
{
    ECU_ASSERT( (fsm) );
    ECU_ASSERT( ((t > FSM_TRANSITION_TYPE_START) && (t < FSM_TRANSITION_TYPE_COUNT)) );
    fsm->transition |= (1U << t);
}

static void clear_all_transitions(struct ecu_fsm *fsm)
{
    ECU_ASSERT( (fsm) );
    fsm->transition = 0;
}

/*------------------------------------------------------------*/
/*-------------------- FSM MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void ecu_fsm_ctor(struct ecu_fsm *me, const struct ecu_fsm_state *state)
{
    ECU_ASSERT( (me && state) );
    ECU_ASSERT( (state_is_valid(state)) );
    me->state = state;
    clear_all_transitions(me);
}

void ecu_fsm_change_state(struct ecu_fsm *me, const struct ecu_fsm_state *state)
{
    ECU_ASSERT( (me && state) );
    ECU_ASSERT( (no_transitions_active(me)) ); /* Cannot call ecu_fsm_change_state() multiple times in a row. Only one transition per dispatch. */
    ECU_ASSERT( (state_is_valid(state)) );

    if (me->state == state)
    {
        set_transition(me, FSM_SELF_TRANSITION);
    }
    else
    {
        set_transition(me, FSM_STATE_TRANSITION);
        me->state = state;
    }
}

void ecu_fsm_dispatch(struct ecu_fsm *me, const void *event)
{
    ECU_ASSERT( (me && event) );
    ECU_ASSERT( (no_transitions_active(me)) );
    ECU_ASSERT( (me->state) );
    ECU_ASSERT( (state_is_valid(me->state)) );
    const struct ecu_fsm_state *prev_state = me->state;

    /* Relay event to state. Save previous state in case of transition.
    State transitions are handled by running exit handler of previous state
    and entry handler of current state. Reject user from state transitions
    in exit handler as that makes no sense - we are already exiting the State.
    Reject user from self-transitions in entry handler. Pointless and causes
    infinite loop. */
    (*me->state->handler)(me, event);

    if (transition_is_active(me, FSM_SELF_TRANSITION))
    {
        clear_all_transitions(me);

        if (prev_state->exit)
        {
            (*prev_state->exit)(me);
            ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in exit handler. */
        }

        prev_state = me->state;
        if (me->state->entry)
        {
            (*me->state->entry)(me);
            ECU_ASSERT( (!transition_is_active(me, FSM_SELF_TRANSITION)) ); /* Self-transition not allowed in entry handler. */
        }
    }

    while (transition_is_active(me, FSM_STATE_TRANSITION))
    {
        clear_all_transitions(me);

        if (prev_state->exit)
        {
            (*prev_state->exit)(me);
            ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in exit handler. */
        }

        prev_state = me->state;
        if (me->state->entry)
        {
            (*me->state->entry)(me);
            ECU_ASSERT( (!transition_is_active(me, FSM_SELF_TRANSITION)) ); /* Self-transition not allowed in entry handler. */
        }
    }
}

void ecu_fsm_start(struct ecu_fsm *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (no_transitions_active(me)) );
    ECU_ASSERT( (me->state) );
    ECU_ASSERT( (state_is_valid(me->state)) );
    const struct ecu_fsm_state *prev_state = me->state;

    /* Run entry handler of current state if one was supplied.
    State transitions are handled by running exit handler of previous state
    and entry handler of current state. Reject user from state transitions
    in exit handler as that makes no sense - we are already exiting the State.
    Reject user from self-transitions in entry handler. Pointless and causes
    infinite loop. */
    if (me->state->entry)
    {
        (*me->state->entry)(me);
        ECU_ASSERT( (!transition_is_active(me, FSM_SELF_TRANSITION)) ); /* Self-transition not allowed in entry handler. */

        while (transition_is_active(me, FSM_STATE_TRANSITION))
        {
            clear_all_transitions(me);

            if (prev_state->exit)
            {
                (*prev_state->exit)(me);
                ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in exit handler. */
            }

            prev_state = me->state;
            if (me->state->entry)
            {
                (*me->state->entry)(me);
                ECU_ASSERT( (!transition_is_active(me, FSM_SELF_TRANSITION)) ); /* Self-transition not allowed in entry handler. */
            }
        }
    }
}
