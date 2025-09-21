/**
 * @file
 * @brief
 * @rst
 * See :ref:`hsm.h section <hsm_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-01
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/hsm.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("ecu/hsm.c")

/*------------------------------------------------------------*/
/*---------------------- FILE SCOPE TYPES --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Meaning of bits in @ref ecu_hsm.transition bitmap.
 * A set bit means that type of state transition is active.
 */
enum transition_type
{
    HSM_TRANSITION_TYPE_START = -1, /**< Used to prevent type-limits warning when compiling on targets that auto adjust enum types. */
    /************************/
    HSM_SELF_TRANSITION,    /**< State reset. Entry and exit of state runs. */
    HSM_STATE_TRANSITION,   /**< Normal state transition. Entry and exit of LCA not ran. */
    /************************/
    HSM_TRANSITION_TYPE_COUNT
};

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DECLARATIONS - TOP STATE --------*/
/*------------------------------------------------------------*/

/**
 * @brief Handler for default top state. Always returns true
 * to "handle" the event.
 */
static bool top_state_handler(struct ecu_hsm *hsm, const void *event);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if @p parent is a parent, grandparent,
 * great-grandparent, etc of @p state. False otherwise. If
 * @p parent == @p state, then true is also returned.
 * 
 * @param hsm Hsm tree that contains @p parent and @p state.
 * @param parent Parent to check.
 * @param state State to check.
 */
static bool is_parent_of(const struct ecu_hsm *hsm,
                         const struct ecu_hsm_state *parent,
                         const struct ecu_hsm_state *state);

/**
 * @brief Returns child of supplied state. Returned child state 
 * is not checked for validity.
 * 
 * @param hsm Hsm tree that contains @p leaf and @p state.
 * @param leaf Required since only parent points used in state 
 * tree. This leaf is traversed up until current->parent == state.
 * Therefore @p leaf and @p state must be related and not equal.
 * @param state Get child of this state.
 */
static const struct ecu_hsm_state *get_child(const struct ecu_hsm *hsm,
                                             const struct ecu_hsm_state *leaf,
                                             const struct ecu_hsm_state *state);

/**
 * @brief Gets the least common ancestor between the two
 * supplied states. Returned LCA state is not checked for
 * validity. NULL is returned if the two states are 
 * not in the same tree.
 * 
 * @param hsm Hsm tree to check.
 * @param s1 First state to check.
 * @param s2 Second state to check.
 */
static const struct ecu_hsm_state *get_lca(const struct ecu_hsm *hsm,
                                           const struct ecu_hsm_state *s1, 
                                           const struct ecu_hsm_state *s2);

/**
 * @brief Returns true if supplied hsm has been constructed
 * via @ref ecu_hsm_ctor() and is in a valid state. False 
 * otherwise.
 */
static bool hsm_is_valid(const struct ecu_hsm *hsm);

/**
 * @brief Returns true if supplied state has a non-NULL
 * handler and parent, or if the supplied state is the default
 * top state. False otherwise.
 */
static bool state_is_valid(const struct ecu_hsm_state *state);

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DEFINITIONS - TOP STATE ---------*/
/*------------------------------------------------------------*/

static bool top_state_handler(struct ecu_hsm *hsm, const void *event)
{
    (void)hsm;
    (void)event;
    return true;
}

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

#pragma message("TODO: Change from t >= 0 to t > HSM_TRANSITION_TYPE_START after hsm branch merged in. Not doing now to avoid conflicts.")

/* Place here (not in alphabetical order) in attempt to inline function call
when used in other definitions below. */
static bool is_parent_of(const struct ecu_hsm *hsm,
                         const struct ecu_hsm_state *parent,
                         const struct ecu_hsm_state *state) 
{
    ECU_ASSERT( (hsm && parent && state) );
    bool status = false;
    uint8_t height = 0;

    while ((state != ECU_HSM_TOP_STATE.parent) && (height < (hsm->height + 1))) /* +1 since going until top.parent. */
    {
        if (state == parent)
        {
            status = true;
            break;
        }
        else
        {
            height++;
            ECU_ASSERT( (state) );
            state = state->parent;
            /* Do not assert state here since it can equal ECU_HSM_TOP_STATE.parent which may be NULL. */
        }
    }
    ECU_ASSERT( ((status) || (state == ECU_HSM_TOP_STATE.parent)) ); /* Verify loop exited due to state hierarchy being fully traversed and not max height being exceeded. */

    return status;
}

static const struct ecu_hsm_state *get_child(const struct ecu_hsm *hsm,
                                             const struct ecu_hsm_state *leaf,
                                             const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (hsm && state && leaf) );
    ECU_ASSERT( (state != leaf) );
    ECU_ASSERT( (is_parent_of(hsm, state, leaf)) );

    uint8_t height = 0;
    const struct ecu_hsm_state *s = leaf;

    while ((s->parent != state) && (height < hsm->height))
    {
        height++;
        s = s->parent;
        ECU_ASSERT( (s) );
    }
    ECU_ASSERT( (s->parent == state) ); /* Verify loop exited due to state hierarchy being fully traversed and not max height being exceeded. */

    return s;
}

static const struct ecu_hsm_state *get_lca(const struct ecu_hsm *hsm,
                                           const struct ecu_hsm_state *s1, 
                                           const struct ecu_hsm_state *s2)
{
    ECU_ASSERT( (hsm && s1 && s2) );
    const struct ecu_hsm_state *lca = (const struct ecu_hsm_state *)0;
    uint8_t height = 0;

    while ((s1 != ECU_HSM_TOP_STATE.parent) && (height < hsm->height + 1)) /* +1 since going until top.parent. */
    {
        if (s1 == s2 || is_parent_of(hsm, s1, s2))
        {
            lca = s1;
            break;
        }
        else
        {
            height++;
            ECU_ASSERT( (s1) );
            s1 = s1->parent;
            /* Do not assert s1 here since it can equal ECU_HSM_TOP_STATE.parent which may be NULL. */
        }
    }
    ECU_ASSERT( ((lca) || (s1 == ECU_HSM_TOP_STATE.parent)) ); /* Verify loop exited due to state hierarchy being fully traversed and not max height being exceeded. */

    return lca;
}

static bool hsm_is_valid(const struct ecu_hsm *hsm)
{
    ECU_ASSERT( (hsm) );
    bool status = false;

    if ((state_is_valid(hsm->state)) && 
        (hsm->height > 0))
    {
        status = true;
    }

    return status;
}

static bool state_is_valid(const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (state) );
    bool status = false;

    if ((state->handler) && 
        (state->parent || state == &ECU_HSM_TOP_STATE))
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (((size_t)HSM_TRANSITION_TYPE_COUNT) <= (ECU_FIELD_SIZEOF(struct ecu_hsm, transition) * 8)),
                    "Max value in transition_type enum exceeds most significant bit of ecu_hsm::transition bitfield." );

/*------------------------------------------------------------*/
/*---------------------- GLOBAL VARIABLES --------------------*/
/*------------------------------------------------------------*/

const struct ecu_hsm_state ECU_HSM_TOP_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &top_state_handler, (const struct ecu_hsm_state *)0
);

/*------------------------------------------------------------*/
/*-------------------- HSM MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void ecu_hsm_ctor(struct ecu_hsm *me, 
                  const struct ecu_hsm_state *state,
                  uint8_t height)
{
    ECU_ASSERT( (me && state) );
    ECU_ASSERT( (state != &ECU_HSM_TOP_STATE) );
    ECU_ASSERT( (state_is_valid(state)) );
    /* Do not assert state->initial == ECU_HSM_STATE_INITIAL since start state does not have to be leaf. */
    ECU_ASSERT( (height > 0) );

    me->state = state;
    me->height = height;
    me->transition = 0;
}

void ecu_hsm_change_state(struct ecu_hsm *me, const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (me && state) );
    ECU_ASSERT( (hsm_is_valid(me)) );
    ECU_ASSERT( (0 == me->transition) ); /* Cannot call ecu_hsm_change_state() multiple times in a row. */
    ECU_ASSERT( (state != &ECU_HSM_TOP_STATE) );
    ECU_ASSERT( (state_is_valid(state)) );
    
    if (me->state == state)
    {
        me->transition = (1U << HSM_SELF_TRANSITION);
    }
    else
    {
        me->transition = (1U << HSM_STATE_TRANSITION);
        me->state = state;
    }
}

void ecu_hsm_dispatch(struct ecu_hsm *me, const void *event)
{
    ECU_ASSERT( (me && event) );
    ECU_ASSERT( (hsm_is_valid(me)) ); /* Also asserts state_is_valid(me->state). */
    ECU_ASSERT( (0 == me->transition) );
    ECU_ASSERT( (me->state->initial == ECU_HSM_STATE_INITIAL_UNUSED) ); /* Must be in leaf state. */
    uint8_t height = 0;
    const struct ecu_hsm_state *source = me->state; /* Store starting state so full state trace can be calculated on transitions. */
    const struct ecu_hsm_state *super = me->state;  /* State that handled the event. */

    /* Dispatch event to current substate. */
    bool handled = (*me->state->handler)(me, event);

    /* If event not handled in current substate, propgate it up to composite states until it is handled or transition occurs. */
    while ((!handled) && (height < me->height))
    {
        ECU_ASSERT( (0 == me->transition) ); /* State transition should mean event has been handled. */
        height++;
        super = super->parent;
        ECU_ASSERT( (state_is_valid(super)) );
        me->state = super; /* Update me->state to detect self state transitions. */
        handled = (*super->handler)(me, event);
    }
    ECU_ASSERT( (handled) ); /* Verify loop exited due to event being handled and not max height being exceeded. */

    if (0 == me->transition)
    {
        /* Finished processing since no state transitions. Restore me->state to original value. */
        me->state = source;
    }
    else
    {
        /* Handle transition by exiting from source state up to the LCA and entering from the LCA
        to the target state. Exit and entry of LCA is only ran if a self-state transition occurred.
        Once done, "drill" into the leaf state if hsm transitioned to a composite state. */
        bool self_state_transition = ((1U << HSM_SELF_TRANSITION) == me->transition);
        me->transition = 0;

        const struct ecu_hsm_state *lca = get_lca(me, super, me->state); /* super == state that handled event. me->state is now target state. */
        const struct ecu_hsm_state *current = source;
        ECU_ASSERT( (lca) ); /* User cannot transition to state not in HSM hierarchy. */

        /* Exit up to the LCA. */
        height = 0;
        while ((current != lca) && (height < me->height))
        {
            ECU_ASSERT( (state_is_valid(current)) );

            if (current->exit != ECU_HSM_STATE_EXIT_UNUSED)
            {
                (*current->exit)(me);
                ECU_ASSERT( (0 == me->transition) );
            }

            current = current->parent;
            height++;
        }
        ECU_ASSERT( (current == lca) ); /* Verify loop exited due to LCA being reached and not max height being exceeded. */

        /* If self-state transition exit and enter LCA. */
        if (self_state_transition)
        {
            if (current->exit != ECU_HSM_STATE_EXIT_UNUSED)
            {
                (*current->exit)(me);
                ECU_ASSERT( (0 == me->transition) );
            }

            if (current->entry != ECU_HSM_STATE_ENTRY_UNUSED)
            {
                (*current->entry)(me);
                ECU_ASSERT( (0 == me->transition) );
            }
        }

        /* Enter from LCA to target state. Target state also entered. */
        height = 0;
        while ((current != me->state) && (height < me->height))
        {
            current = get_child(me, me->state, current);
            ECU_ASSERT( (state_is_valid(current)) );
            height++;

            if (current->entry != ECU_HSM_STATE_ENTRY_UNUSED)
            {
                (*current->entry)(me);
                ECU_ASSERT( (0 == me->transition) );
            }
        }
        ECU_ASSERT( (current == me->state) ); /* Verify loop exited due to target being reached and not max height being exceeded. */

        /* Drill into leaf state. Handle multiple initial transitions in a row. */
        uint8_t drills = 0;
        while ((current->initial != ECU_HSM_STATE_INITIAL_UNUSED) && (drills < me->height))
        {
            height = 0;
            (*current->initial)(me);
            ECU_ASSERT( (((1U << HSM_STATE_TRANSITION) == me->transition) && (is_parent_of(me, current, me->state))) ); /* Initial transition must go down the state hierarchy. */
            me->transition = 0;

            /* Enter until initial state reached. Also enter initial state. */
            while ((current != me->state) && (height < me->height))
            {
                current = get_child(me, me->state, current);
                ECU_ASSERT( (state_is_valid(current)) );
                height++;

                if (current->entry != ECU_HSM_STATE_ENTRY_UNUSED)
                {
                    (*current->entry)(me);
                    ECU_ASSERT( (0 == me->transition) );
                }
            }
            ECU_ASSERT( (current == me->state) ); /* Verify loop exited due to target being reached and not max height being exceeded. */
            drills++;
        }
        ECU_ASSERT( (current->initial == ECU_HSM_STATE_INITIAL_UNUSED) ); /* Verify loop exited due to leaf being reached and not max drills being exceeded. */
    }
}

void ecu_hsm_start(struct ecu_hsm *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (hsm_is_valid(me)) ); /* Also asserts state_is_valid(me->state). */
    ECU_ASSERT( (0 == me->transition) );
    /* Do not assert state->initial == ECU_HSM_STATE_INITIAL_UNUSED since start state does not have to be leaf. */

    uint8_t height = 0;
    const struct ecu_hsm_state *current = me->state;

    /* Find state one below top state. */
    while ((current->parent != &ECU_HSM_TOP_STATE) && (height < me->height))
    {
        height++;
        current = current->parent;
        ECU_ASSERT( (current) );
    }
    ECU_ASSERT( (current->parent == &ECU_HSM_TOP_STATE) ); /* Verify loop exited due to source being reached and not max height being exceeded. */

    /* Run entry of current (one below top). */
    if (current->entry != ECU_HSM_STATE_ENTRY_UNUSED)
    {
        (*current->entry)(me);
        ECU_ASSERT( (0 == me->transition) );
    }

    /* Enter until target state is reached. Also enter target state. */
    height = 0;
    while ((current != me->state) && (height < me->height))
    {
        current = get_child(me, me->state, current);
        ECU_ASSERT( (state_is_valid(current)) );
        height++;

        if (current->entry != ECU_HSM_STATE_ENTRY_UNUSED)
        {
            (*current->entry)(me);
            ECU_ASSERT( (0 == me->transition) );
        }
    }
    ECU_ASSERT( (current == me->state) ); /* Verify loop exited due to target being reached and not max height being exceeded. */

    /* Drill into leaf state. Handle multiple initial transitions in a row. */
    uint8_t drills = 0;
    while ((current->initial != ECU_HSM_STATE_INITIAL_UNUSED) && (drills < me->height))
    {
        height = 0;
        (*current->initial)(me);
        ECU_ASSERT( (((1U << HSM_STATE_TRANSITION) == me->transition) && (is_parent_of(me, current, me->state))) ); /* Initial transition must go down the state hierarchy. */
        me->transition = 0;

        /* Enter until initial state reached. Also enter initial state. */
        while ((current != me->state) && (height < me->height))
        {
            current = get_child(me, me->state, current);
            ECU_ASSERT( (state_is_valid(current)) );
            height++;

            if (current->entry != ECU_HSM_STATE_ENTRY_UNUSED)
            {
                (*current->entry)(me);
                ECU_ASSERT( (0 == me->transition) );
            }
        }
        ECU_ASSERT( (current == me->state) ); /* Verify loop exited due to target being reached and not max height being exceeded. */
        drills++;
    }
    ECU_ASSERT( (current->initial == ECU_HSM_STATE_INITIAL_UNUSED) ); /* Verify loop exited due to leaf being reached and not max drills being exceeded. */
}
