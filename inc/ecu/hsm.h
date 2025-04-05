/**
 * @file
 * @brief TODO
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-06-02
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_HSM_H_
#define ECU_HSM_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* HSM represented as generic tree. */
#include "ecu/tree.h"

/* Base event class. */
#include "ecu/event.h"

/* Do not have doxygen parse this file so CI passes. This file is still under construction. */
#ifndef ECU_DOXYGEN 

/* Forward declaration for state function typedefs. */
struct ecu_hsm;

enum ecu_hsm_status
{
    ECU_HSM_EVENT_HANDLED,
    ECU_HSM_EVENT_IGNORED,
    ECU_HSM_EVENT_TO_SUPER_STATE,
    ECU_HSM_STATE_TRANSITION, // normal state transition that can be used in all scenarios. entry and exit condition of lca will NOT be run.
    ECU_HSM_INTRA_STATE_TRANSITION // state transition between two composite states. entry and exit condition of lca (outermost composite state) will run.
};


// support transitions in entry event. https://www.youtube.com/watch?v=lUvUNuUMQHo&list=PLPW8O6W-1chxym7TgIPV9k5E8YJtSBToI&index=6
// 20:27. Analagous to INIT_SIG in hsm.
typedef void (*ecu_hsm_on_entry_handler)(struct ecu_hsm *me);
typedef void (*ecu_hsm_on_exit_handler)(struct ecu_hsm *me);
typedef enum ecu_hsm_status (*ecu_hsm_state_handler)(struct ecu_hsm *me, const struct ecu_event *event);



struct ecu_hsm
{
    // pointer instead of full data member so multiple hsm objects can share
    // the same state tree. Otherwise you would have to construct the top state
    // again for each hsm object.
    const struct ecu_hsm_top_state *top_state;
    const struct ecu_hsm_state *current_state;
    const struct ecu_hsm_state *temp_state; // placeholder so we can change current state at last possible minute.
};


struct ecu_hsm_state
{
    struct ecu_tree_node node;
    ecu_hsm_on_entry_handler on_entry;
    ecu_hsm_on_exit_handler on_exit;
    ecu_hsm_state_handler handler;
};

struct ecu_hsm_top_state
{
    struct ecu_hsm_state state;
};



#ifdef __cplusplus
extern "C" {
#endif


// entry handler would be a good place to initialize your hsm object.
// exit handler good place to deinitialize/destroy your hsm object.
extern void ecu_hsm_ctor(struct ecu_hsm *me,
                         const struct ecu_hsm_top_state *top_state_0);


extern void ecu_hsm_top_state_ctor(struct ecu_hsm_top_state *me,
                                   ecu_hsm_on_entry_handler on_entry_0,
                                   ecu_hsm_on_exit_handler on_exit_0);


// 1. Not adding super_state here since that would make the constructor order now matter. Higher level nodes (states)
// would have to be constructed before lower level states since ecu_tree_node_ctor() must be called to add nodes to tree.
// 2. undefined behavior if you pass in a top state. I.e. &ecu_hsm_top_state.state
extern void ecu_hsm_state_ctor(struct ecu_hsm_state *me,
                               ecu_hsm_on_entry_handler on_entry_0,
                               ecu_hsm_on_exit_handler on_exit_0,
                               ecu_hsm_state_handler handler_0);


// runtime initialization of hsm state tree.
// requires states to be constructed as precondition.
extern void ecu_hsm_add_state_to_top_state(struct ecu_hsm_state *me,
                                           struct ecu_hsm_top_state *top_state);


// runtime initialization of hsm state tree.
// requires states to be constructed as precondition.
extern void ecu_hsm_add_state_to_super_state(struct ecu_hsm_state *me,
                                             struct ecu_hsm_state *super_state);


// start_state != &me->top
// start_state must be in hsm state tree.
extern void ecu_hsm_begin(struct ecu_hsm *me,
                          const struct ecu_hsm_state *start_state);

// requires hsm_begin() to have been called.
extern void ecu_hsm_dispatch(struct ecu_hsm *me,
                             const struct ecu_event *event);


// exit path = from current state to lca (not including lca). entry path = from lca (not including it)
// to new state.
// show images of local and standard state transitions.

// show picture of state A in State B. transition path would be State A entry.
// if you want State B exit -> State B entry -> State A entry call @ref ecu_hsm_transition_to_intra_state() instead.
extern enum ecu_hsm_status ecu_hsm_transition_to_state(struct ecu_hsm *me,
                                                       const struct ecu_hsm_state *new_state);

// same state path of ecu_hsm_change_state() however lca is exited and entered as well.
// show images of local and standard state transitions.
// asserts if states arent composites.
extern enum ecu_hsm_status ecu_hsm_transition_to_intra_state(struct ecu_hsm *me,
                                                             const struct ecu_hsm_state *new_state);
// make function name mention composite/substate and superstate somehow?


static inline enum ecu_hsm_status ecu_hsm_dispatch_event_to_super_state(void)
{
    return ECU_HSM_EVENT_TO_SUPER_STATE;
}

#ifdef __cplusplus
}
#endif

#endif /* ECU_DOXYGEN */

#endif /* ECU_HSM_H_ */