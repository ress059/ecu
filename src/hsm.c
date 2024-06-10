/**
 * @file
 * @brief TODO Description of ecu_hsm_dispatch() algorithm (finding lca etc).
 * 
 * @author Ian Ress 
 * @version 0.1
 * @date 2024-06-02
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include <ecu/hsm.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ FILE-SCOPE VARIABLES -----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *HSM_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- STATIC FUNCTION DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// static bool state_valid(const struct ecu_hsm_state *state);
static enum ecu_hsm_status TOP_STATE_HANDLER(struct ecu_hsm *me, const struct ecu_event *event);

// WARNING: only call this in ecu_hsm_dispatch() and ecu_hsm_begin()
// since this is a file-scope function we assume the state is in the hsm state tree and do
// not assert this condition
// hsm is pointer to nonconst since exit and entry handlers take in non-const hsm.
static void hsm_enter_state(struct ecu_hsm *me, struct ecu_hsm_state *state);
static void hsm_exit_state(struct ecu_hsm *me, struct ecu_hsm_state *state);

// point to nonconst in case it returns itself.
static struct ecu_hsm_state *get_parent_state(struct ecu_hsm_state *state);

// pointer to nonconst because you are returning the same struct just offset...
static struct ecu_hsm_state *convert_node_to_state(struct ecu_tree_node *node);


// static bool is_top_state(const struct ecu_hsm_state *top_state);

// not pointer to const since calls ecu_tree_get_lca() which takes in pointers to non-const.
// composite means state1 is parent of state2 or vise versa.
static bool are_composite_states(struct ecu_hsm_state *state1, 
                                 struct ecu_hsm_state *state2);


// used in case ecu_hsm_status enum changes in the future. Contained within this
// single function instead of using enums directly in code.
static bool is_state_transition_status(enum ecu_hsm_status status);

static bool hsm_valid(const struct ecu_hsm *hsm);

/**
 * @brief Returns true if the supplied state is valid. False otherwise.
 * Valid means a state handler function has been registered and the
 * state is within a valid hsm state tree.
 * 
 * 
 * the state has a state handler function and that isn't the 
 * top state handler. The state's parent node must also point to either
 * a superstate or the top state. It must not point to itselfnot point to 
 * itself - it must point to either a superstate or the top state for the
 * hsm state tree to be valid.
 */
static bool state_valid(const struct ecu_hsm_state *state);
static bool top_state_valid(const struct ecu_hsm_top_state *top_state);




/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static enum ecu_hsm_status TOP_STATE_HANDLER(struct ecu_hsm *me, const struct ecu_event *event)
{
    (void)me;
    (void)event;
    return ECU_HSM_EVENT_IGNORED;
}


static void hsm_enter_state(struct ecu_hsm *me, struct ecu_hsm_state *state)
{
    /* Only allow this function to be called when a state transition takes place. */
    struct ecu_hsm_state *prev_state = (struct ecu_hsm_state *)0;    
    ECU_RUNTIME_ASSERT( (me && state), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((me->current_state) && (me->temp_state) && \
                         (me->current_state != me->temp_state)), HSM_ASSERT_FUNCTOR );

    prev_state = me->temp_state;
    if (state->on_entry)
    {
        /* Run entry handler. Reject state transitions from entry handler. */
        (*state->on_entry)(me);
        ECU_RUNTIME_ASSERT( (me->temp_state == prev_state), HSM_ASSERT_FUNCTOR );
    }
}


static void hsm_exit_state(struct ecu_hsm *me, struct ecu_hsm_state *state)
{
    /* Only allow this function to be called when a state transition takes place. */
    struct ecu_hsm_state *prev_state = (struct ecu_hsm_state *)0;    
    ECU_RUNTIME_ASSERT( (me && state), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((me->current_state) && (me->temp_state) && \
                         (me->current_state != me->temp_state)), HSM_ASSERT_FUNCTOR );

    prev_state = me->temp_state;
    if (state->on_exit)
    {
        /* Run exit handler. Reject state transitions from exit handler. */
        (*state->on_exit)(me);
        ECU_RUNTIME_ASSERT( (me->temp_state == prev_state), HSM_ASSERT_FUNCTOR );
    }
}


static struct ecu_hsm_state *get_parent_state(struct ecu_hsm_state *state)
{
    ECU_RUNTIME_ASSERT( (state), HSM_ASSERT_FUNCTOR );
    return (ECU_TREE_NODE_GET_ENTRY(&state->node.parent, struct ecu_hsm_state, node));
}


static struct ecu_hsm_state *convert_node_to_state(struct ecu_tree_node *node)
{
    ECU_RUNTIME_ASSERT( (node), HSM_ASSERT_FUNCTOR );
    return (ECU_TREE_NODE_GET_ENTRY(node, struct ecu_hsm_state, node));
}


// static bool is_top_state(const struct ecu_hsm_state *state)
// {
//     ECU_RUNTIME_ASSERT( (state), HSM_ASSERT_FUNCTOR );
//     return (state->handler == &TOP_STATE_HANDLER);
// }


static bool are_composite_states(struct ecu_hsm_state *state1, 
                                 struct ecu_hsm_state *state2)
{
    bool status = false;
    struct ecu_tree_node *lca = (struct ecu_tree_node *)0;
    ECU_RUNTIME_ASSERT( (state1 && state2), HSM_ASSERT_FUNCTOR );

    lca = ecu_tree_get_lca(&state1->node, &state2->node);
    if ((lca == &state1->node) || (lca == &state2->node))
    {
        status = true;
    }

    return status;
}


// static bool is_state_transition_status(enum ecu_hsm_status status)
// {
//     return ((status == ECU_HSM_STATE_TRANSITION) || \
//             (status == ECU_HSM_INTRA_STATE_TRANSITION));
// }


// static bool hsm_valid(const struct ecu_hsm *hsm)
// {
//     ECU_RUNTIME_ASSERT( (hsm), HSM_ASSERT_FUNCTOR );
//     return ((top_state_valid(hsm->top_state)) && \
//             (state_valid(hsm->current_state)) && \
//             (state_valid(hsm->temp_state))); 
// }


static bool hsm_active(const struct ecu_hsm *hsm)
{
    /* Have to NULL assert hsm->top_state since we access hsm->top_state->state.
    Rest of functions do all other necessary NULL assertions. */
    bool active = false;
    ECU_RUNTIME_ASSERT( (hsm), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (hsm->top_state), HSM_ASSERT_FUNCTOR ); 

    if ((top_state_valid(hsm->top_state->state)) && \
        (state_active(hsm->current_state)) && \
        (state_active(hsm->temp_state)))
    {
        active = true;
    }

    return active;
}


static bool state_handler_valid(const struct ecu_hsm_state *state)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (state), HSM_ASSERT_FUNCTOR );

    /* Entry and exit handlers are optional so those are not checked. */
    if ((state->handler) && \
        (state->handler != &TOP_STATE_HANDLER))
    {
        valid = true;
    }

    return valid;
}


static bool state_in_tree(const struct ecu_hsm_state *state)
{
    bool in_tree = false;
    ECU_RUNTIME_ASSERT( (state), HSM_ASSERT_FUNCTOR );

    if ((state->node.parent) && \
        (&state->node != state->node.parent))
    {
        in_tree = true;
    }

    return in_tree;
}


static bool state_active(const struct ecu_hsm_state *state)
{
    /* Called functions already NULL assert. */
    return (state_handler_valid(state) && state_in_tree(state));
}


// top state does not have separate functions like state_handler_valid(), 
// state_in_tree(), etc since these should all be set in the constructor and never change.
// takes in ecu_hsm_state instead of ecu_hsm_top_state so functions traversing parent
// tree can easily use this still.
static bool top_state_active(const struct ecu_hsm_state *top_state)
{
    bool active = false;
    ECU_RUNTIME_ASSERT( (top_state), HSM_ASSERT_FUNCTOR );

    /* Entry and exit handlers are optional so those are not checked. 
    Do not call state_handler_valid(), state_in_tree(), or state_active()
    as helpers since the top state is handled differently. */
    if ((top_state->handler == &TOP_STATE_HANDLER) && \
        (top_state->node.parent == &top_state->node))
    {
        active = true;
    }

    return active;
}



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PUBLIC FUNCTIONS -------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void ecu_hsm_ctor(struct ecu_hsm *me,
                  const struct ecu_hsm_top_state *top_state_0)
{
    ECU_RUNTIME_ASSERT( (me && top_state_0), HSM_ASSERT_FUNCTOR );
    me->top_state       = top_state_0;
    me->current_state   = &top_state_0->state;
    me->temp_state      = &top_state_0->state;
}


void ecu_hsm_state_ctor(struct ecu_hsm_state *me,
                        ecu_hsm_on_entry_handler on_entry_0,
                        ecu_hsm_on_exit_handler on_exit_0,
                        ecu_hsm_state_handler handler_0)
{
    /* on_entry_0 and on_exit_0 are optional so do not NULL assert. */
    ECU_RUNTIME_ASSERT( (me && handler_0), HSM_ASSERT_FUNCTOR );

    ecu_tree_node_ctor(&me->node, (void (*)(struct ecu_tree_node *))0, ECU_OBJECT_ID_UNUSED);
    me->on_entry    = on_entry_0;
    me->on_exit     = on_exit_0;
    me->handler     = handler_0;
}


void ecu_hsm_top_state_ctor(struct ecu_hsm_top_state *me,
                            ecu_hsm_on_entry_handler on_entry_0,
                            ecu_hsm_on_exit_handler on_exit_0)
{
    /* on_entry_0 and on_exit_0 are optional so do not NULL assert. */
    ECU_RUNTIME_ASSERT( (me), HSM_ASSERT_FUNCTOR );

    ecu_tree_node_ctor(&me->state.node, (void (*)(struct ecu_tree_node *))0, ECU_OBJECT_ID_UNUSED);
    me->state.on_entry  = on_entry_0;
    me->state.on_exit   = on_exit_0;
    me->state.handler   = &TOP_STATE_HANDLER;
}


void ecu_hsm_add_state_to_top_state(struct ecu_hsm_state *me,
                                    struct ecu_hsm_top_state *top_state)
{
    /* These functions NULL assert. */
    ECU_RUNTIME_ASSERT( ((state_handler_valid(me)) && (top_state_active(top_state))), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me != &top_state->state), HSM_ASSERT_FUNCTOR );

    ecu_tree_add_child_push_back(&top_state->state.node, &me->node);
}


void ecu_hsm_add_state_to_super_state(struct ecu_hsm_state *me,
                                      struct ecu_hsm_state *super_state)
{
    /* These functions NULL assert. */
    ECU_RUNTIME_ASSERT( ((state_handler_valid(me)) && (state_handler_valid(super_state))), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me != super_state), HSM_ASSERT_FUNCTOR );

    ecu_tree_add_child_push_back(&super_state->node, &me->node);
}


void ecu_hsm_begin(struct ecu_hsm *me,
                   const struct ecu_hsm_state *start_state)
{
    #warning "TODO"
    // dont assert hsm_active(). itll be active after this function
}


void ecu_hsm_dispatch(struct ecu_hsm *me,
                      const struct ecu_event *event)
{
    enum ecu_hsm_status status                  = ECU_HSM_EVENT_IGNORED;
    const struct ecu_hsm_state *exit_trace      = (const struct ecu_hsm_state *)0;
    const struct ecu_hsm_state *entry_trace     = (const struct ecu_hsm_state *)0;
    const struct ecu_hsm_state *parent_trace    = (const struct ecu_hsm_state *)0;
    const struct ecu_hsm_state *lca             = (const struct ecu_hsm_state *)0;

    ECU_RUNTIME_ASSERT( (me && event), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((hsm_active(me)) && (event->id >= ECU_VALID_EVENT_ID_BEGIN)), HSM_ASSERT_FUNCTOR );

    /* Dispatch event to state. */
    parent_trace    = me->current_state;
    status          = (*me->current_state->handler)(me, event);

    /* Top state handler always returns ECU_HSM_EVENT_IGNORED. */
    while (status == ECU_HSM_EVENT_TO_SUPER_STATE)
    {
        /* State can only be added to the tree via add_state() functions. These functions
        require a valid state handler as a precondition and guarantee valid state tree (parent)
        structure so we do not assert this condition. */
        parent_trace = get_parent_state(parent_trace);
        status = (*parent_trace->handler)(me, event);
    }
    
    /* State transition taken. Figure out path. See file description for more details on this algorithm. */
    if (me->current_state != me->temp_state)
    {
        /* lca and entry_trace not asserted since convert_node_to_state() NULL asserts its 
        function argument. Do not assert status == ECU_HSM_STATE_TRANSITION or 
        status == ECU_HSM_INTRA_STATE_TRANSITION since state can only be changed via 
        state_change() functions which always returns one of these statuses. */
        exit_trace          = me->current_state;
        entry_trace, lca    = convert_node_to_state(ecu_tree_get_lca(&me->current_state->node, &me->temp_state->node));

        /* Exit trace. See file description for more details. Exit trace = start state at this point. */
        while (exit_trace != lca)
        {
            /* State can only be added to the tree via add_state() functions. These functions
            guarantee valid state tree (parent) structure so we do not assert this condition. */
            hsm_exit_state(me, exit_trace);
            exit_trace = get_parent_state(exit_trace);
        }

        /* Intra state transition requested. Run exit and entry handler of LCA. */
        if (status == ECU_HSM_INTRA_STATE_TRANSITION)
        {
            /* LCA will never be top since this is asserted in change_state() functions so we do not assert this. */
            ECU_RUNTIME_ASSERT( (are_composite_states(me->current_state, me->temp_state)), HSM_ASSERT_FUNCTOR );
            hsm_exit_state(me, lca);
            hsm_enter_state(me, lca);
        }
        
        /* Entry trace. See file description for more details. Entry trace = LCA at this point. */
        while (entry_trace != me->temp_state)
        {
            parent_trace = me->temp_state;
            while (parent_trace->node.parent != &entry_trace->node)
            {
                parent_trace = get_parent_state(parent_trace);
            }

            entry_trace = parent_trace;
            hsm_enter_state(me, entry_trace);
        }
        me->current_state = me->temp_state;
    }
}


// new_state != &me->top
// new_state must be in hsm state tree.
enum ecu_hsm_status ecu_hsm_transition_to_state(struct ecu_hsm *me,
                                                const struct ecu_hsm_state *new_state)
{
    /* NULL assertions done in functions. */
    ECU_RUNTIME_ASSERT( ((hsm_valid(me)) && (state_valid(new_state))), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (new_state != me->top_state), HSM_ASSERT_FUNCTOR );
    // TODO may be disqualifying const specifier since this function takes in nonconst.
    ECU_RUNTIME_ASSERT( (ecu_tree_nodes_in_same_tree(&me->top_state->state.node, &new_state->node)), 
                        HSM_ASSERT_FUNCTOR );

    me->temp_state = new_state;
    return ECU_HSM_STATE_TRANSITION;
}


enum ecu_hsm_status ecu_hsm_transition_to_intra_state(struct ecu_hsm *me,
                                                      const struct ecu_hsm_state *new_state)
{
    /* NULL assertions done in functions. */
    ECU_RUNTIME_ASSERT( ((hsm_valid(me)) && (state_valid(new_state))), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (new_state != me->top_state), HSM_ASSERT_FUNCTOR );
    // TODO may be disqualifying const specifier since this function takes in nonconst.
    ECU_RUNTIME_ASSERT( (ecu_tree_nodes_in_same_tree(&me->top_state->state.node, &new_state->node)), 
                        HSM_ASSERT_FUNCTOR );

    me->temp_state = new_state;
    return ECU_HSM_INTRA_STATE_TRANSITION;
}


void ecu_hsm_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    HSM_ASSERT_FUNCTOR = functor;
}
