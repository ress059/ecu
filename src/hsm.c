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

/* Asserts. */
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
    HSM_SELF_TRANSITION,    /**< State reset. Entry and exit of state runs. */
    HSM_STATE_TRANSITION,   /**< Normal state transition. Entry and exit of LCA not ran. */
    /************************/
    HSM_TRANSITION_TYPE_COUNT
};

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied hsm has a valid state 
 * and top state. False otherwise.
 */
static bool hsm_is_valid(const struct ecu_hsm *hsm);

/**
 * @brief Returns true if supplied state has a valid handler 
 * function and parent. False otherwise. Normal states and
 * the top state can be supplied.
 */
static bool state_is_valid(const struct ecu_hsm_state *state);

/**
 * @brief Returns true if any state transitions are active.
 * False otherwise.
 */
static bool any_transitions_active(const struct ecu_hsm *hsm);

/**
 * @brief Returns true if no state transitions are active.
 * False otherwise.
 */
static bool no_transitions_active(const struct ecu_hsm *hsm);

/**
 * @brief Returns true if a specific state transition 
 * type has been signalled. False otherwise.
 *
 * @param t State transition type to check.
 */
static bool transition_is_active(const struct ecu_hsm *hsm, enum transition_type t);

/**
 * @brief Signals that a specific state transition type
 * has been requested. 
 * 
 * @param t State transition type requested.
 */
static void set_transition(struct ecu_hsm *hsm, enum transition_type t);

/**
 * @brief Clears all @ref ecu_hsm.transition bits.
 */
static void clear_all_transitions(struct ecu_hsm *hsm);

/**
 * @brief Returns true if @p parent is a parent, grandparent,
 * great-grandparent, etc of @p state. False otherwise. If
 * @p parent == @p state, then @p state is returned.
 * 
 * @param hsm HSM tree that contains @p parent and @p state.
 * @param parent Parent to check.
 * @param state State to check.
 */
static bool is_parent_of(const struct ecu_hsm *hsm,
                         const struct ecu_hsm_state *parent,
                         const struct ecu_hsm_state *state);

/**
 * @brief Returns parent of supplied state.
 * 
 * @param state Get parent of this state.
 */
static const struct ecu_hsm_state *get_parent(const struct ecu_hsm_state *state);

/**
 * @brief Returns child of supplied state.
 * 
 * @param hsm HSM tree that contains @p leaf and @p state.
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
 * supplied states. NULL is returned if the two states are 
 * not in the same tree.
 * 
 * @param hsm HSM tree to check.
 * @param s1 First state to check.
 * @param s2 Second state to check.
 */
static const struct ecu_hsm_state *get_lca(const struct ecu_hsm *hsm,
                                           const struct ecu_hsm_state *s1, 
                                           const struct ecu_hsm_state *s2);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool hsm_is_valid(const struct ecu_hsm *hsm)
{
    ECU_ASSERT( (hsm) );
    bool status = false;

    if (state_is_valid(hsm->state) && 
        state_is_valid(hsm->top) &&
        hsm->top->parent == ECU_HSM_STATE_NO_PARENT)
    {
        status = true;
    }

    return status;
}

static bool state_is_valid(const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (state) );
    bool status = false;

    if (state->handler && state->parent != state)
    {
        status = true;
    }

    return status;
}

static bool any_transitions_active(const struct ecu_hsm *hsm)
{
    ECU_ASSERT( (hsm) );
    return (hsm->transition);
}

static bool no_transitions_active(const struct ecu_hsm *hsm)
{
    ECU_ASSERT( (hsm) );
    return (hsm->transition == 0U);
}

static bool transition_is_active(const struct ecu_hsm *hsm, enum transition_type t)
{
    ECU_ASSERT( (hsm) );
    ECU_ASSERT( (t >= 0 && t < HSM_TRANSITION_TYPE_COUNT) );
    return (hsm->transition & (1U << t));
}

static void set_transition(struct ecu_hsm *hsm, enum transition_type t)
{
    ECU_ASSERT( (hsm) );
    ECU_ASSERT( (t >= 0 && t < HSM_TRANSITION_TYPE_COUNT) );
    hsm->transition |= (1U << t);
}

static void clear_all_transitions(struct ecu_hsm *hsm)
{
    ECU_ASSERT( (hsm) );
    hsm->transition = 0;
}

static bool is_parent_of(const struct ecu_hsm *hsm,
                         const struct ecu_hsm_state *parent,
                         const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (hsm && parent && state) );
    bool status = false;
    uint8_t height = 0;

    /* Notice how this also handles case where parent == state. */
    for (const struct ecu_hsm_state *s = state; s != ECU_HSM_STATE_NO_PARENT; s = s->parent)
    {
        ECU_ASSERT( (height <= hsm->height) );
        height++;

        if (s == parent)
        {
            status = true;
            break;
        }
    }

    return status;
}

static const struct ecu_hsm_state *get_parent(const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (state) );
    return (state->parent);
}

static const struct ecu_hsm_state *get_child(const struct ecu_hsm *hsm,
                                             const struct ecu_hsm_state *leaf,
                                             const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (hsm && state && leaf) );
    ECU_ASSERT( (state != leaf) );
    ECU_ASSERT( (is_parent_of(hsm, state, leaf)) );
    const struct ecu_hsm_state *s = (const struct ecu_hsm_state *)0;

    for (s = leaf; s->parent != state; s = s->parent)
    {
        /* Purposefully empty. */
    }

    return s;
}

static const struct ecu_hsm_state *get_lca(const struct ecu_hsm *hsm,
                                           const struct ecu_hsm_state *s1, 
                                           const struct ecu_hsm_state *s2)
{
    ECU_ASSERT( (hsm && s1 && s2) );
    uint8_t height = 0;
    const struct ecu_hsm_state *lca = (const struct ecu_hsm_state *)0;

    /* Notice how this also handles the case of s1 == s2. */
    for (const struct ecu_hsm_state *s = s1; s != ECU_HSM_STATE_NO_PARENT; s = s->parent)
    {
        ECU_ASSERT( (height <= hsm->height) );
        height++;

        if (s == s2 || is_parent_of(hsm, s, s2))
        {
            lca = s;
            break;
        }
    }

    return lca;
}

/*------------------------------------------------------------*/
/*-------------------- HSM MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void ecu_hsm_ctor(struct ecu_hsm *me, 
                  const struct ecu_hsm_state *state,
                  const struct ecu_hsm_state *top,
                  uint8_t height)
{
    ECU_ASSERT( (me && state && top) );
    ECU_ASSERT( (state_is_valid(state) && state_is_valid(top)) );
    ECU_ASSERT( (top->parent == ECU_HSM_STATE_NO_PARENT) );

    me->state = state;
    me->top = top;
    me->height = height;
    clear_all_transitions(me);
}

void ecu_hsm_start(struct ecu_hsm *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (hsm_is_valid(me)) );
    const struct ecu_hsm_state *trace = me->top;

    /* Run entry handlers from top to current state. Entry handler of current state not ran. */
    while (trace != me->state)
    {
        if (trace->entry)
        {
            (*trace->entry)(me);
            ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
        }

        trace = get_child(me, me->state, trace);
        ECU_ASSERT( (state_is_valid(trace)) );
    }

    /* Run entry handler of current state. */
    if (me->state->entry)
    {
        (*me->state->entry)(me);
        ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
    }
}

void ecu_hsm_change_state(struct ecu_hsm *me, const struct ecu_hsm_state *state)
{
    ECU_ASSERT( (me && state) );
    ECU_ASSERT( (hsm_is_valid(me)) );
    ECU_ASSERT( (state_is_valid(state)) );
    /* Cannot call ecu_hsm_change_state() multiple times in a row. Only one transition per dispatch. */
    ECU_ASSERT( (no_transitions_active(me)) ); 

    if (me->state == state)
    {
        set_transition(me, HSM_SELF_TRANSITION);
    }
    else
    {
        set_transition(me, HSM_STATE_TRANSITION);
        me->state = state;
    }
}

void ecu_hsm_dispatch(struct ecu_hsm *me, const void *event)
{
    ECU_ASSERT( (me && event) );
    ECU_ASSERT( (hsm_is_valid(me)) );
    ECU_ASSERT( (!any_transitions_active(me)) );

    const struct ecu_hsm_state *lca = me->state;
    const struct ecu_hsm_state *start = me->state;
    const struct ecu_hsm_state *trace = me->state;
    uint8_t height = 0;
    bool handled = false;

    /* Dispatch event to current state. */
    handled = (*me->state->handler)(me, event);

    while (!any_transitions_active(me) && !handled)
    {
        /* Dispatch event to parents until it is handled or a state transition occurred. */
        ECU_ASSERT( (height < me->height) ); /* Not a typo. Should be < instead of <=. */
        height++;
        trace = get_parent(trace);
        ECU_ASSERT( (state_is_valid(trace)) ); /* Also rejects get_parent(top) being called. */
        handled = (*trace->handler)(me, event);
    }

    if (transition_is_active(me, HSM_SELF_TRANSITION))
    {
        /* Run exit and entry of LCA (the current state). */
        clear_all_transitions(me);

        if (me->state->exit)
        {
            (*me->state->exit)(me);
            ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in exit handler. */
        }

        if (me->state->entry)
        {
            (*me->state->entry)(me);
            ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
        }
    }
    else if (any_transitions_active(me)) /* Any other state transition type active? */
    {
        /* Run exit up until LCA. Run entry from LCA to current state. Exit and entry of LCA not ran. */
        clear_all_transitions(me);
        trace = start;
        lca = get_lca(me, start, me->state);
        ECU_ASSERT( (lca) ); /* User cannot transition to state not in HSM hierarchy. */

        /* Run exit handlers from starting state up until LCA. LCA exit not ran. */
        while (trace != lca)
        {
            if (trace->exit)
            {
                (*trace->exit)(me);
                ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in exit handler. */
            }

            trace = get_parent(trace);
            ECU_ASSERT( (state_is_valid(trace)) );
        }

        /* Run entry handlers starting from LCA to current state. Entry handlers of LCA not ran. */
        while (trace != me->state)
        {
            trace = get_child(me, me->state, trace);
            ECU_ASSERT( (state_is_valid(trace)) );

            if (trace->entry)
            {
                (*trace->entry)(me);
                ECU_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
            }
        }
    }
}
