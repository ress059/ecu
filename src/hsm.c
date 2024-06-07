/**
 * @file
 * @brief See @ref hsm.h
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


static bool is_top_state(const struct ecu_hsm_state *top_state);

// not pointer to const since calls ecu_tree_get_lca() which takes in pointers to non-const.
static bool are_composite_states(struct ecu_hsm_state *state1, 
                                 struct ecu_hsm_state *state2);

static bool is_state_transition_status(enum ecu_hsm_status status);

static bool hsm_valid(const struct ecu_hsm *hsm);
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

    prev_state = me->current_state;
    if (state->on_entry)
    {
        /* Run entry handler. Reject state transitions from entry handler. */
        (*state->on_entry)(me);
        ECU_RUNTIME_ASSERT( (me->current_state == prev_state), HSM_ASSERT_FUNCTOR );
    }
}


static void hsm_exit_state(struct ecu_hsm *me, struct ecu_hsm_state *state)
{
    /* Only allow this function to be called when a state transition takes place. */
    struct ecu_hsm_state *prev_state = (struct ecu_hsm_state *)0;    
    ECU_RUNTIME_ASSERT( (me && state), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((me->current_state) && (me->temp_state) && \
                         (me->current_state != me->temp_state)), HSM_ASSERT_FUNCTOR );

    prev_state = me->current_state;
    if (state->on_exit)
    {
        /* Run exit handler. Reject state transitions from exit handler. */
        (*state->on_exit)(me);
        ECU_RUNTIME_ASSERT( (me->current_state == prev_state), HSM_ASSERT_FUNCTOR );
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


static bool is_top_state(const struct ecu_hsm_state *state)
{
    ECU_RUNTIME_ASSERT( (state), HSM_ASSERT_FUNCTOR );
    return (state->handler == &TOP_STATE_HANDLER);
}


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


static bool is_state_transition_status(enum ecu_hsm_status status)
{
    return ((status == ECU_HSM_STATE_TRANSITION) || \
            (status == ECU_HSM_LOCAL_STATE_TRANSITION) || \
            (status == ECU_HSM_EXTERNAL_STATE_TRANSITION));
}


static bool hsm_valid(const struct ecu_hsm *hsm)
{
    ECU_RUNTIME_ASSERT( (hsm), HSM_ASSERT_FUNCTOR );
    return ((hsm->top_state) && (hsm->current_state) && (hsm->temp_state)); 
}


static bool state_valid(const struct ecu_hsm_state *state)
{
    ECU_RUNTIME_ASSERT( (state), HSM_ASSERT_FUNCTOR );
    return ((state->handler) && (state->handler != &TOP_STATE_HANDLER));
}


static bool top_state_valid(const struct ecu_hsm_top_state *top_state)
{
    ECU_RUNTIME_ASSERT( (top_state), HSM_ASSERT_FUNCTOR );
    return (top_state->state.handler == &TOP_STATE_HANDLER);
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


void ecu_hsm_state_ctor(struct ecu_hsm_state *me,
                        ecu_hsm_on_entry_handler on_entry_0,
                        ecu_hsm_on_exit_handler on_exit_0,
                        ecu_hsm_state_handler handler_0)
{
    /* on_entry_0 and on_exit_0 are optional so do not NULL assert. */
    ECU_RUNTIME_ASSERT( (me && handler_0), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (!is_top_state(me)), HSM_ASSERT_FUNCTOR );

    ecu_tree_node_ctor(&me->node, (void (*)(struct ecu_tree_node *))0, ECU_OBJECT_ID_UNUSED);
    me->on_entry    = on_entry_0;
    me->on_exit     = on_exit_0;
    me->handler     = handler_0;
}


void ecu_hsm_add_state_to_top_state(struct ecu_hsm_state *me,
                                    struct ecu_hsm_top_state *top_state)
{
    /* These functions NULL assert. */
    ECU_RUNTIME_ASSERT( ((state_valid(me)) && (top_state_valid(top_state))), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me != &top_state->state), HSM_ASSERT_FUNCTOR );

    ecu_tree_add_child_push_back(&top_state->state.node, &me->node);
}


void ecu_hsm_add_state_to_super_state(struct ecu_hsm_state *me,
                                      struct ecu_hsm_state *super_state)
{
    /* These functions NULL assert. */
    ECU_RUNTIME_ASSERT( ((state_valid(me)) && (state_valid(super_state))), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me != super_state), HSM_ASSERT_FUNCTOR );

    ecu_tree_add_child_push_back(&super_state->node, &me->node);
}


void ecu_hsm_begin(struct ecu_hsm *me,
                   const struct ecu_hsm_state *start_state)
{
    #warning "TODO"
}


void ecu_hsm_dispatch(struct ecu_hsm *me,
                      const struct ecu_event *event)
{
    #warning "TODO: Still deciding on if I should have a hsm->temp_state varaible. This decreases variables \
    in this function and allows us to assert me->current_state != me->temp_state for exit_state() and enter_state() functions."
    enum ecu_hsm_status status                  = ECU_HSM_EVENT_IGNORED;
    const struct ecu_hsm_state *exit_trace      = (const struct ecu_hsm_state *)0;
    const struct ecu_hsm_state *entry_trace     = (const struct ecu_hsm_state *)0;
    const struct ecu_hsm_state *parent_trace    = (const struct ecu_hsm_state *)0;
    const struct ecu_hsm_state *lca             = (const struct ecu_hsm_state *)0;

    ECU_RUNTIME_ASSERT( (me && event), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((me->top_state) && (me->current_state) && (me->temp_state) && \
                         (event->id >= ECU_VALID_EVENT_ID_BEGIN)), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me->current_state->handler), HSM_ASSERT_FUNCTOR );

    /* Dispatch event to state. */
    parent_trace    = me->current_state;
    status          = (*me->current_state->handler)(me, event);

    /* Top state handler always returns ECU_HSM_EVENT_IGNORED. */
    while (status == ECU_HSM_EVENT_TO_SUPER_STATE)
    {
        /* Hsm state tree is set up improperly if the parent state is equal to itself. */
        ECU_RUNTIME_ASSERT( (&parent_trace->node != parent_trace->node.parent), HSM_ASSERT_FUNCTOR );
        parent_trace = get_parent_state(parent_trace);
        ECU_RUNTIME_ASSERT( (parent_trace), HSM_ASSERT_FUNCTOR );
        ECU_RUNTIME_ASSERT( (parent_trace->handler), HSM_ASSERT_FUNCTOR );
        status = (*parent_trace->handler)(me, event);
    }
    
    /* State transition taken. Figure out path. See file description for more details on this algorithm. */
    // if (status == ECU_HSM_LOCAL_STATE_TRANSITION || ECU_HSM_EXTERNAL_STATE_TRANSITION)
    if (me->current_state != me->temp_state)
    {
        ECU_RUNTIME_ASSERT( (is_state_transition_status(status)), HSM_ASSERT_FUNCTOR );
        /* lca and entry_trace not asserted since convert_node_to_state() NULL asserts its function argument. */
        // exit_trace          = start_state;
        // parent_trace        = start_state;
        // lca, entry_trace    = convert_node_to_state(ecu_tree_get_lca(&start_state->node, &me->current_state->node));
        exit_trace          = me->current_state;
        entry_trace, lca    = convert_node_to_state(ecu_tree_get_lca(&me->current_state->node, &me->temp_state->node));

        /* Exit trace. See file description for more details. */
        while (exit_trace != lca)
        {
            hsm_exit_state(me, exit_trace);
            ECU_RUNTIME_ASSERT( (&exit_trace->node != exit_trace->node.parent), HSM_ASSERT_FUNCTOR );
            exit_trace = get_parent_state(exit_trace);
            ECU_RUNTIME_ASSERT( (exit_trace), HSM_ASSERT_FUNCTOR );
        }

        /* External transition only appropriate for composite states. LCA will be one of these two states. */
        // if ((status == ECU_HSM_EXTERNAL_STATE_TRANSITION) && \
        //     (are_composite_states(start_state, me->current_state)))
        if (status == ECU_HSM_EXTERNAL_STATE_TRANSITION)
        {
            /* LCA will never be top since this is asserted in change_state() functions. */
            ECU_RUNTIME_ASSERT( (are_composite_states(me->current_state, me->temp_state)), HSM_ASSERT_FUNCTOR );
            hsm_exit_state(me, lca);
            hsm_enter_state(me, lca);
        }
        
        /* Entry trace. See file description for more details. */
        // while (entry_trace != me->current_state)
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
enum ecu_hsm_status ecu_hsm_change_state(struct ecu_hsm *me,
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


enum ecu_hsm_status ecu_hsm_change_state_external(struct ecu_hsm *me,
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


void ecu_hsm_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    HSM_ASSERT_FUNCTOR = functor;
}
