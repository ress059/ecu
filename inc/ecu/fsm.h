/**
 * @file
 * @brief Base finite state machine class. State is represented by a user-defined function.
 * Serves as an interface that users inherit from to define their application-specific fsms.
 * @details Users can define their own application-specific fsms by inheriting this base class.
 * For example:
 * 
 * @code{.c}
 * struct user_fsm
 * {
 *     // Inherit base fsm class. MUST be first member.
 *     struct ecu_fsm fsm;
 * 
 *     // User fsm data
 *     uint8_t switch_statuses;
 *     uint16_t current_time_ms;
 * };
 * @endcode
 * 
 * Library functions only use this base fsm class so users can upcast back to this base class 
 * when calling them. Using the user_fsm struct defined earlier as an example:
 * 
 * @code{.c}
 * ecu_fsm_ctor((struct ecu_fsm *)&user_fsm); // Library function call.
 * @endcode
 * 
 * User-defined events and event data can be dispatched to the fsm using this same inheritance
 * scheme. Using the same user_fsm structure:
 * 
 * @code{.c}
 * struct user_event
 * {
 *     // Inherit base event class. MUST be first member.
 *     struct ecu_event event;
 *         
 *     // User event data
 *     uint8_t msg[8];
 *     uint16_t index;
 * };
 * 
 * ecu_fsm_dispatch((struct ecu_fsm *)&user_fsm, (const struct ecu_event *)&user_event);
 * @endcode
 * 
 * See @ref event.h for more details on defining your own events. 
 * 
 * A bare-bones pseudocode example of a user-defined state would follow this template:
 * 
 * @code{.c}
 * enum ecu_fsm_status operational_state(struct user_fsm *fsm, struct user_event *event)
 * {
 *     enum ecu_fsm_status status = ECU_FSM_EVENT_HANDLED;
 * 
 *     switch(event->event.signal)
 *     {
 *         case ECU_ENTRY_EVENT:
 *         {
 *             // User logic when state first enters. Automatically ran by dispatcher.
 *             // DO NOT DEREFERENCE EVENT STRUCT HERE.
 *             break;
 *         }
 * 
 *         case ECU_EXIT_EVENT:
 *         {
 *             // User logic when state exits. Automatically ran by dispatcher.
 *             // DO NOT DEREFERENCE EVENT STRUCT HERE.
 *             break;
 *         }
 * 
 *         case USER_EVENT_SIGNAL:
 *         {
 *             if (event->data == data that causes state transition)
 *             {
 *                 // If this transitions occurs dispatcher would execute: 
 *                 // 1) This current case.
 *                 // 2) ECU_EXIT_EVENT of this state.
 *                 // 3) ECU_ENTRY_EVENT of "your_new_state".
 *                 status = ecu_fsm_change_state(&your_new_state);
 *             }
 *             break;
 *         }
 *     }
 *     return status;
 * }
 * @endcode
 * 
 * @author Ian Ress
 * 
 */


#ifndef ECU_FSM_H_
#define ECU_FSM_H_


/* STDLib. */
#include <stdint.h>

/* Runtime asserts. */
#include <ecu/asserter.h>

/* Base event class. */
#include <ecu/event.h>



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- FSM STATE ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Each fsm state handler function must return this type. 
 * 
 * @details @ref ecu_fsm_dispatch() function uses this information 
 * to know which state transitions to execute and verify your fsm's 
 * behavior is valid.
 */
enum ecu_fsm_status
{
    ECU_FSM_STATE_TRANSITION,  /**< DO NOT USE. Reserved. Dispatched event caused state transition. */
    ECU_FSM_EVENT_HANDLED,     /**< Dispatched event was handled by your fsm. Stay in current state. */
    ECU_FSM_EVENT_IGNORED,     /**< Dispatched event was ignored by your fsm. Event is not relevant for current state. Stay in current state. */
    ECU_FSM_ERROR              /**< Dispatched event or fsm behavior is invalid. Dispatcher will assert. */
};


/* Forward declaration to define ecu_fsm_func_ptr. */
struct ecu_fsm;


/**
 * @brief Prototype of all fsm state handler functions.
 */
typedef enum ecu_fsm_status (*ecu_fsm_func_ptr)(struct ecu_fsm *, const struct ecu_event *);



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- BASE FSM CLASS ------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Base fsm class. 
 * 
 * @details State is represented by a function. See @ref fsm.h for details.
 */
struct ecu_fsm
{
    /**
     * @brief PRIVATE. Current state the fsm is in.
     */
    ecu_fsm_func_ptr state;

    /**
     * @brief PRIVATE. Maximum number of consecutive state transitions 
     * fsm can take before an assertion fires.
     * 
     * @details Used to prevent deadlock scenarios where fsm was set 
     * up incorrectly and two states indefinitely transition between 
     * each other.
     */
    uint8_t max_state_transitions;
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- PUBLIC FUNCTIONS -----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructors
 */
/**@{*/
/**
 * @brief Constructor.
 * 
 * @param fsm FSM to construct. This cannot be NULL - memory must be allocated
 * for fsm beforehand since dynamic memory allocation is not used.
 * @param init_state_0 Initial state to put fsm in. This is a function. This cannot 
 * be NULL.
 * @param max_state_transitions_0 Maximum number of consecutive state transitions
 * the fsm is allowed to take before assert fires. This must be greater than 0.
 */
extern void ecu_fsm_ctor(struct ecu_fsm *fsm, ecu_fsm_func_ptr init_state_0, 
                         uint8_t max_state_transitions_0);
/**@}*/


/**
 * @name FSM Behavior
 */
/**@{*/
/**
 * @brief Dispatch event to fsm.
 * 
 * @details Handles state transitions by automatically executing appropriate 
 * @ref ECU_EXIT_EVENT and @ref ECU_ENTRY_EVENT cases of user's fsm. Verifies 
 * valid fsm behavior.
 * 
 * @param fsm Dispatch event to this fsm. This cannot be NULL. This must 
 * have been previously constructed via successful constructor call to
 * @ref ecu_fsm_ctor() before using this function.
 * @param event User-defined event to dispatch. This cannot be NULL. 
 * See @ref event.h
 */
extern void ecu_fsm_dispatch(struct ecu_fsm *fsm, const struct ecu_event *event);


/**
 * @brief Transition fsm to a new state.
 * 
 * @details Application calls this inside their user-defined state
 * handler functions to signal that the dispatched event caused a
 * state transition. This function should always be used - fsm 
 * state should never be changed manually.
 * 
 * @warning This cannot be called in the @ref ECU_EXIT_EVENT case 
 * of your state handler.
 * 
 * @param fsm Change state of this fsm. This cannot be NULL. This must 
 * have been previously constructed via successful constructor call to
 * @ref ecu_fsm_ctor() before using this function.
 * @param new_state New state to transition into. This is a function.
 * This cannot be NULL.
 * 
 * @return The return value is used internally so the dispatcher knows
 * a state transition occurred.
 */
static inline enum ecu_fsm_status ecu_fsm_change_state(struct ecu_fsm *fsm, 
                                                       ecu_fsm_func_ptr new_state)
{
    ECU_RUNTIME_ASSERT( (fsm && new_state), ECU_DEFAULT_FUNCTOR );
    fsm->state = new_state;
    return ECU_FSM_STATE_TRANSITION;
}
/**@}*/


/**
 * @name FSM Assert Handler
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
extern void ecu_fsm_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_FSM_H_ */
