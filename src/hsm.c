/**
 * @file
 * @brief todo
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

ECU_ASSERT_DEFINE_NAME("ecu/hsm.c")

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
 * and height. False otherwise.
 */
static bool hsm_is_valid(const struct ecu_hsm *hsm);

/**
 * @brief Returns true if supplied state has a valid handler 
 * function and parent. False otherwise.
 */
static bool state_is_valid(const struct ecu_hsm_state *state);

/**
 * @brief Returns true if any state transitions are active.
 * False otherwise.
 */
static bool transitions_active(const struct ecu_hsm *hsm);

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
 * great-grandparent, etc of @p state. False otherwise.
 * 
 * @param parent Parent to check.
 * @param state State to check.
 * @param max_height Fail-safe to prevent infinite loop. Starting 
 * at @p state, the state hierarchy is traversed up until the top 
 * state or @p parent is reached. An assert fires if the number of 
 * traversals exceeds this amount.
 */
static bool is_parent_of(const struct ecu_hsm_state *parent,
                         const struct ecu_hsm_state *state,
                         uint8_t max_height);

/**
 * @brief Returns parent of supplied state and asserts it's valid.
 * 
 * @param state Get parent of this state.
 */
static const struct ecu_hsm_state *get_parent(const struct ecu_hsm_state *state);

/**
 * @brief Returns child of supplied state and asserts it's valid.
 * 
 * @param state Get child of this state.
 * @param leaf Required since only parent points used in state 
 * tree. This leaf is traversed up until current->parent == state.
 * Therefore @p leaf and @p state must be related and not equal.
 * @param max_height Fail-safe to prevent infinite loop. Starting 
 * at @p leaf, the state hierarchy is traversed up until 
 * current->parent == @p state. At that point, the child has been
 * reached. An assert fires if the number of traversals exceeds 
 * this amount.
 */
static const struct ecu_hsm_state *get_child(const struct ecu_hsm_state *state,
                                             const struct ecu_hsm_state *leaf,
                                             uint8_t max_height);

/**
 * @brief Gets top state and asserts it's valid.
 * 
 * @param state Starting at this state, the state hierarchy is
 * traversed up until the top state is reached.
 * @param max_height Fail-safe to prevent infinite loop. Starting 
 * at @p state, the state hierarchy is traversed up until the top 
 * state is reached. An assert fires if the number of traversals 
 * exceeds this amount.
 */
static const struct ecu_hsm_state *get_top(const struct ecu_hsm_state *state,
                                           uint8_t max_height);

/**
 * @brief Gets the least common ancestor between the two
 * supplied states. NULL is returned if the two states are 
 * not in the same tree.
 * 
 * @param s1 First state to check.
 * @param s2 Second state to check.
 * @param max_height Fail-safe to prevent infinite loop. LCA
 * algorithm traverses up the state hierarchy until the top 
 * state is reached. An assert fires if the number of traversals 
 * exceeds this amount.
 */
static const struct ecu_hsm_state *get_lca(const struct ecu_hsm_state *s1, 
                                           const struct ecu_hsm_state *s2, 
                                           uint8_t max_height);
#warning "TODO: Decided on following: \
    Level starts at 0. Dont have to assert max_height > 0."

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool hsm_is_valid(const struct ecu_hsm *hsm)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (hsm) );

    if (state_is_valid(hsm->state) && hsm->height > 0)
    {
        status = true;
    }

    return status;
}

static bool state_is_valid(const struct ecu_hsm_state *state)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (state) );

    if (state->handler && state->parent != state)
    {
        status = true;
    }

    return status;
}

static bool transitions_active(const struct ecu_hsm *hsm)
{
    ECU_RUNTIME_ASSERT( (hsm) );
    return (hsm->transition);
}

static bool transition_is_active(const struct ecu_hsm *hsm, enum transition_type t)
{
    ECU_RUNTIME_ASSERT( (hsm) );
    ECU_RUNTIME_ASSERT( (t >= 0 && t < HSM_TRANSITION_TYPE_COUNT) );
    return (hsm->transition & (1U << t));
}

static void set_transition(struct ecu_hsm *hsm, enum transition_type t)
{
    ECU_RUNTIME_ASSERT( (hsm) );
    ECU_RUNTIME_ASSERT( (t >= 0 && t < HSM_TRANSITION_TYPE_COUNT) );
    hsm->transition |= (1U << t);
}

static void clear_all_transitions(struct ecu_hsm *hsm)
{
    ECU_RUNTIME_ASSERT( (hsm) );
    hsm->transition = 0;
}

static bool is_parent_of(const struct ecu_hsm_state *parent,
                         const struct ecu_hsm_state *state,
                         uint8_t max_height)
{
    bool status = false;
    uint8_t height = 0;
    ECU_RUNTIME_ASSERT( (parent && state) );

    /* Notice how this also handles case where parent == state. */
    for (const struct ecu_hsm_state *s = state; s != ECU_HSM_STATE_NO_PARENT; s = s->parent)
    {
        ECU_RUNTIME_ASSERT( (height < max_height) );
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
    ECU_RUNTIME_ASSERT( (state) );
    ECU_RUNTIME_ASSERT( (state_is_valid(state)) );
    ECU_RUNTIME_ASSERT( (state_is_valid(state->parent)) );
    return (state->parent);
}

static const struct ecu_hsm_state *get_child(const struct ecu_hsm_state *state,
                                             const struct ecu_hsm_state *leaf,
                                             uint8_t max_height)
{
    const struct ecu_hsm_state *s = (const struct ecu_hsm_state *)0;
    ECU_RUNTIME_ASSERT( (state && leaf) );
    ECU_RUNTIME_ASSERT( (state != leaf) );
    ECU_RUNTIME_ASSERT( (is_parent_of(state, leaf, max_height)) );

    for (s = leaf; s->parent != state; s = s->parent)
    {
        /* Purposefully empty. */
    }

    ECU_RUNTIME_ASSERT( (state_is_valid(s)) );
    return s;
}

static const struct ecu_hsm_state *get_top(const struct ecu_hsm_state *state,
                                           uint8_t max_height)
{
    uint8_t height = 0;
    const struct ecu_hsm_state *s = (const struct ecu_hsm_state *)0;
    ECU_RUNTIME_ASSERT( (state) );

    for (s = state; s->parent != ECU_HSM_STATE_NO_PARENT; s = s->parent)
    {
        ECU_RUNTIME_ASSERT( (height < max_height) );
        height++;
    }

    ECU_RUNTIME_ASSERT( (state_is_valid(s)) );
    return s;
}

static const struct ecu_hsm_state *get_lca(const struct ecu_hsm_state *s1, 
                                           const struct ecu_hsm_state *s2, 
                                           uint8_t max_height)
{
    uint8_t height = 0;
    const struct ecu_hsm_state *lca = (const struct ecu_hsm_state *)0;
    ECU_RUNTIME_ASSERT( (s1 && s2) );

    /* Notice how this also handles the case of s1 == s2. */
    for (const struct ecu_hsm_state *s = s1; s != ECU_HSM_STATE_NO_PARENT; s = s->parent)
    {
        ECU_RUNTIME_ASSERT( (height < max_height) );
        height++;

        if (s == s2 || is_parent_of(s, s2, max_height))
        {
            lca = s;
            break;
        }
    }

    ECU_RUNTIME_ASSERT( (state_is_valid(lca)) );
    return lca;
}

/*------------------------------------------------------------*/
/*-------------------- HSM MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void ecu_hsm_ctor(struct ecu_hsm *me, 
                  const struct ecu_hsm_state *state,
                  uint8_t height)
{
    ECU_RUNTIME_ASSERT( (me && state) );
    ECU_RUNTIME_ASSERT( (state_is_valid(state)) );
    ECU_RUNTIME_ASSERT( (height > 0) ); // use fsm if you are using a flat state machine..

    me->state = state;
    me->height = height;
    clear_all_transitions(me);
}

// run entry handlers from top to state.
void ecu_hsm_start(struct ecu_hsm *me)
{
    const struct ecu_hsm_state *trace = (const struct ecu_hsm_state *)0;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (hsm_is_valid(me)) );
    trace = get_top(me->state, me->height);

    /* Run entry handlers from top to current state. Entry handler of current state not ran. */
    while (trace != me->state)
    {
        if (trace->entry)
        {
            (*trace->entry)(me);
            ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
        }

        trace = get_child(trace, me->state, me->height);
    }

    /* Run entry handler of current state. */
    if (me->state->entry)
    {
        (*me->state->entry)(me);
        ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
    }
}

void ecu_hsm_change_state(struct ecu_hsm *me, const struct ecu_hsm_state *state)
{
    ECU_RUNTIME_ASSERT( (me && state) );
    ECU_RUNTIME_ASSERT( (hsm_is_valid(me)) );
    ECU_RUNTIME_ASSERT( (state_is_valid(state)) );
    /* Cannot call ecu_hsm_change_state() multiple times in a row. Only one transition per dispatch. */
    ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); 

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
    ECU_RUNTIME_ASSERT( (me && event) );
    /* Only initial state asserted. Remaining state changes have to be done 
    with ecu_hsm_change_state(), which asserts the new state is valid. */
    ECU_RUNTIME_ASSERT( (hsm_is_valid(me)) );
    ECU_RUNTIME_ASSERT( (!transitions_active(me)) );

    const struct ecu_hsm_state *lca = me->state;
    const struct ecu_hsm_state *trace = me->state;
    uint8_t traversals = 0;
    bool handled = false;

    /* Dispatch event to current state. */
    handled = (*me->state->handler)(me, event);

    while (!transitions_active(me) && !handled)
    {
        /* Dispatch event to parents until it is handled or a state transition occurred. */
        ECU_RUNTIME_ASSERT( (traversals < me->height) );
        traversals++;
        trace = get_parent(trace);
        handled = (*trace->handler)(me, event);
    }

    if (transition_is_active(me, HSM_SELF_TRANSITION))
    {
        /* Run exit and entry of LCA (the current state). */
        clear_all_transitions(me);

        if (me->state->exit)
        {
            (*me->state->exit)(me);
            ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in exit handler. */
        }

        if (me->state->entry)
        {
            (*me->state->entry)(me);
            ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
        }
    }
    else if (transitions_active(me)) /* Any other state transition type active? */
    {
        /* Run exit up until LCA. Run entry from LCA to current state. Exit and entry of LCA not ran. */
        clear_all_transitions(me);
        lca = get_lca(trace, me->state, me->height); /* Asserts number of traversals does not exceed me->height. */
        ECU_RUNTIME_ASSERT( (lca) ); /* User cannot transition to state not in HSM hierarchy. */

        /* Run exit handlers from starting state up until LCA. LCA exit not ran. */
        while (trace != lca)
        {
            if (trace->exit)
            {
                (*trace->exit)(me);
                ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in exit handler. */
            }

            trace = get_parent(trace);
        }

        /* Run entry handlers starting from LCA to current state. Entry handlers of LCA and current state not ran. */
        while (trace != me->state)
        {
            trace = get_child(trace, me->state, me->height);

            if (trace->entry)
            {
                (*trace->entry)(me);
                ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
            }
        }

        /* Run entry handler of current state. */
        if (me->state->entry)
        {
            (*me->state->entry)(me);
            ECU_RUNTIME_ASSERT( (no_transitions_active(me)) ); /* No state transitions allowed in entry handler. */
        }
    }
}