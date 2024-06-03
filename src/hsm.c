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
static void TOP_STATE_EXIT_HANDLER(struct ecu_hsm *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// static bool state_valid(const struct ecu_hsm_state *state)
// {
// 	ECU_RUNTIME_ASSERT( (state), HSM_ASSERT_FUNCTOR );

// 	if (state->handler)
// }


static enum ecu_hsm_status TOP_STATE_HANDLER(struct ecu_hsm *me, const struct ecu_event *event)
{
    (void)me;
    (void)event;
    return ECU_HSM_EVENT_HANDLED;
}


static void TOP_STATE_EXIT_HANDLER(struct ecu_hsm *me)
{
    ECU_RUNTIME_ASSERT( (false), HSM_ASSERT_FUNCTOR );
}



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PUBLIC FUNCTIONS -------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

// entry handler would be a good place to initialize values in your hsm.
void ecu_hsm_ctor(struct ecu_hsm *me,
                  ecu_hsm_on_entry_handler on_entry_0)
{
    /* on_entry_0 optional so do not NULL assert. */
    ECU_RUNTIME_ASSERT( (me), HSM_ASSERT_FUNCTOR );

	/* Do not call ecu_hsm_state_ctor() as helper function since this 
	asserts the input state is not the top state. */
    ecu_tree_node_ctor(&me->top.node, (void (*)(struct ecu_tree_node *))0, ECU_OBJECT_ID_UNUSED);
	me->top.hsm 		= me;
    me->top.on_entry    = on_entry_0;
    me->top.on_exit     = &TOP_STATE_EXIT_HANDLER;
    me->top.handler     = &TOP_STATE_HANDLER;
    me->state           = &me->top;
}


// 1. Not adding super_state here since that would make the constructor order now matter. Higher level nodes (states)
// would have to be constructed before lower level states since ecu_tree_node_ctor() must be called to add nodes to tree.
// 2. undefined behavior if me == &ecu_hsm.top
void ecu_hsm_state_ctor(struct ecu_hsm_state *me,
						const struct ecu_hsm *hsm_0,
                        ecu_hsm_on_entry_handler on_entry_0,
                        ecu_hsm_on_exit_handler on_exit_0,
                        ecu_hsm_state_handler handler_0)
{
    /* on_entry_0 and on_exit_0 optional so do not NULL assert. */
    ECU_RUNTIME_ASSERT( (me && hsm_0 && handler_0), HSM_ASSERT_FUNCTOR );
	ECU_RUNTIME_ASSERT( (me != &hsm_0->top), HSM_ASSERT_FUNCTOR );

	ecu_tree_node_ctor(&me->node, (void (*)(struct ecu_tree_node *))0, ECU_OBJECT_ID_UNUSED);
	me->hsm			= hsm_0;
    me->on_entry    = on_entry_0;
    me->on_exit     = on_exit_0;
    me->handler     = handler_0;
}


// super_state must be in hsm tree.
// pass in &ecu_hsm.top for top state or call ecu_hsm_add_to_top_state()
// me != super_state.
// me != top state
// me and super_state must belong to same hsm.
// if your state has substates those will also be moved over (moving over entire subtree).
void ecu_hsm_add_to_super_state(struct ecu_hsm_state *me, 
								struct ecu_hsm_state *super_state)
{
	/* Note how it is valid for super_state to be equal to &ecu_hsm.top */
	ECU_RUNTIME_ASSERT( ((me) && (super_state) && (me != super_state)), HSM_ASSERT_FUNCTOR );
	ECU_RUNTIME_ASSERT( ((me->hsm) && (super_state->hsm) && (me->hsm == super_state->hsm)), HSM_ASSERT_FUNCTOR );
	ECU_RUNTIME_ASSERT( (me != &me->hsm->top), HSM_ASSERT_FUNCTOR );
	ECU_RUNTIME_ASSERT( ((me->handler) && (super_state->handler)), HSM_ASSERT_FUNCTOR );

	// super state and me must belong to same hsm. can do direct pointer comparison

	ecu_tree_add_child_push_back(&super_state->node, &me->node);
}


void ecu_hsm_add_to_top_state(struct ecu_hsm_state *me, struct ecu_hsm *top_state)
{

}


// start_state != &me->top
// start_state must be in hsm state tree.
void ecu_hsm_begin(struct ecu_hsm *me,
                   const struct ecu_hsm_state *start_state)
{

}


// new_state != &me->top
// new_state must be in hsm state tree.
enum ecu_hsm_status ecu_hsm_change_state(struct ecu_hsm *me,
                                         const struct ecu_hsm_state *new_state)
{
    ECU_RUNTIME_ASSERT( (me && new_state), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (new_state->handler), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (new_state != &me->top), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (ecu_tree_nodes_in_same_tree(&me->top.node, &new_state->node)), HSM_ASSERT_FUNCTOR ); // TODO may be disqualifying const specifier since this function takes in nonconst.

    me->state = new_state;
    return ECU_HSM_STATE_TRANSITION;
}


void ecu_hsm_dispatch(struct ecu_hsm *me,
                      const struct ecu_event *event)
{
    enum ecu_hsm_status status = ECU_HSM_EVENT_HANDLED;
    const struct ecu_hsm_state *start_state = (const struct ecu_hsm_state *)0;
	const struct ecu_hsm_state *current_state = (const struct ecu_hsm_state *)0;
	const struct ecu_tree_node *super_state = (const struct ecu_tree_node *)0;

    ECU_RUNTIME_ASSERT( (me && event), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me->state->handler), HSM_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (event->id >= ECU_VALID_EVENT_ID_BEGIN), HSM_ASSERT_FUNCTOR );

    // /*
    // 1. dispatch event to state.
    // 2. while (status != ECU_HSM_EVENT_TO_SUPER_STATE) {dispatch event to super state}
    // 3. if (status == ECU_HSM_STATE_TRANSITION)
    // {

    // }  
    // */
	start_state = me->state;
	current_state = me->state;
	status = (*me->state->handler)(me, event);

	while ((status == ECU_HSM_EVENT_TO_SUPER_STATE) && \
		   (current_state != &me->top))
	{
		ECU_RUNTIME_ASSERT( (me->state->node.parent), HSM_ASSERT_FUNCTOR );

		

		current_state = ECU_TREE_NODE_GET_ENTRY(state_tree_node, struct ecu_hsm_state, node);
		status = (*current_state->handler)(me, event);
	}
}




void ecu_fsm_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    HSM_ASSERT_FUNCTOR = functor;
}
