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
#include <ecu/tree.h>

/* Runtime asserts. */
#include <ecu/asserter.h>

/* Base event class. */
#include <ecu/event.h>




/* Forward declaration for state function typedefs. */
struct ecu_hsm;

enum ecu_hsm_status
{
    ECU_HSM_STATE_TRANSITION,
    ECU_HSM_EVENT_HANDLED,
    ECU_HSM_EVENT_TO_SUPER_STATE
};


// support transitions in entry event. https://www.youtube.com/watch?v=lUvUNuUMQHo&list=PLPW8O6W-1chxym7TgIPV9k5E8YJtSBToI&index=6
// 20:27. Analagous to INIT_SIG in hsm.
typedef enum ecu_hsm_status (*ecu_hsm_on_entry_handler)(struct ecu_hsm *me);
typedef void (*ecu_hsm_on_exit_handler)(struct ecu_hsm *me);
typedef enum ecu_hsm_status (*ecu_hsm_state_handler)(struct ecu_hsm *me, const struct ecu_event *event);



struct ecu_hsm
{
    struct ecu_hsm_state top;

    const struct ecu_hsm_state *state;
};


struct ecu_hsm_state
{
    struct ecu_tree_node node;
    const struct ecu_hsm *hsm;
    ecu_hsm_on_entry_handler on_entry;
    ecu_hsm_on_exit_handler on_exit;
    ecu_hsm_state_handler handler;
};



#ifdef __cplusplus
extern "C" {
#endif


// entry handler would be a good place to initialize values in your hsm.
extern void ecu_hsm_ctor(struct ecu_hsm *me,
                         ecu_hsm_on_entry_handler on_entry_0);

// 1. Not adding super_state here since that would make the constructor order now matter. Higher level nodes (states)
// would have to be constructed before lower level states since ecu_tree_node_ctor() must be called to add nodes to tree.
// 2. undefined behavior if me == &ecu_hsm.top
extern void ecu_hsm_state_ctor(struct ecu_hsm_state *me,
					           const struct ecu_hsm *hsm_0,
                               ecu_hsm_on_entry_handler on_entry_0,
                               ecu_hsm_on_exit_handler on_exit_0,
                               ecu_hsm_state_handler handler_0);

// start_state != &me->top
// start_state must be in hsm state tree.
extern void ecu_hsm_begin(struct ecu_hsm *me,
                          const struct ecu_hsm_state *start_state);

// new_state != &me->top
// new_state must be in hsm state tree.
extern enum ecu_hsm_status ecu_hsm_change_state(struct ecu_hsm *me,
                                                const struct ecu_hsm_state *new_state);


extern void ecu_hsm_dispatch(struct ecu_hsm *me,
                             const struct ecu_event *event);



/**
 * @name Asserts In This Module
 */
/**@{*/
/**
 * @brief Set a functor to execute if an assert fires within this module. 
 * @details This is optional - if no functor is set a default one will be 
 * used. The default functor hangs in a permanent while loop if NDEBUG is 
 * not defined so users are able to inspect the call stack.
 * 
 * @param functor User-supplied functor. If a NULL value is supplied
 * the default functor will be used. 
 */
extern void ecu_hsm_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_HSM_H_ */