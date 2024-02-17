/**
 * @file
 * @brief Base finite state machine class. State is represented by a user-defined function.
 * Serves as an interface that users inherit from to define their application-specific fsms.
 * @details Users can define their own application-specific fsms by inheriting this base class.
 * For example:
 * 
 *     struct user_fsm
 *     {
 *         // Inherit base fsm class. MUST be first member.
 *         struct fsm fsm;
 * 
 *         // User fsm data
 *         uint8_t switch_statuses;
 *         uint16_t current_time_ms;
 *     };
 * 
 * Library functions only use this base fsm class so users can upcast back to this base class 
 * when calling them. Using the user_fsm struct defined earlier as an example:
 * 
 *     fsm_ctor((struct fsm *)&user_fsm); // Library function.
 * 
 * User-defined events and event data can be dispatched to the fsm using this same inheritance
 * scheme. Using the same user_fsm structure:
 * 
 *     struct user_event
 *     {
 *         // Inherit base event class. MUST be first member.
 *         struct event event;
 *         
 *         // User event data
 *         uint8_t msg[8];
 *         uint16_t index;
 *     };
 * 
 *     fsm_dispatch((struct fsm *)&user_fsm, (const struct event *)&user_event);
 * 
 * See @ref event.h for more details on defining your own events. 
 * 
 * A bare-bones pseudocode example of a user-defined state would follow this template:
 * 
 *     enum fsm_status fsm_operational_state(struct user_fsm *fsm, struct user_event *event)
 *     {
 *         enum fsm_status status = FSM_EVENT_HANDLED;
 * 
 *         switch(event->event.signal)
 *         {
 *             case ENTRY_EVENT:
 *             {
 *                 // User logic when state first enters. Automatically ran by dispatcher.
 *                 // DO NOT DEREFERENCE EVENT STRUCT HERE.
 *                 break;
 *             }
 * 
 *             case EXIT_EVENT:
 *             {
 *                 // User logic when state exits. Automatically ran by dispatcher.
 *                 // DO NOT DEREFERENCE EVENT STRUCT HERE.
 *                 break;
 *             }
 * 
 *             case USER_EVENT_SIGNAL:
 *             {
 *                 if (event->data == data that causes state transition)
 *                 {
 *                     // If this transitions occurs dispatcher would execute: 
 *                     // 1) This current case.
 *                     // 2) EXIT_EVENT of this state.
 *                     // 3) ENTRY_EVENT of "your_new_state".
 *                     status = fsm_change_state(&your_new_state);
 *                 }
 *                 break;
 *             }
 *         }
 * 
 *         return status;
 *     }
 * 
 * @author Ian Ress
 * 
 */


#ifndef FSM_H_
#define FSM_H_


/* STDLib. */
#include <stdint.h>

/* Runtime asserts. */
#include "asserter.h"

/* Base event class. */
#include "event.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- FSM STATE ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Each fsm state handler function must return this type.
 * 
 * @details Dispatcher uses this information to execute state 
 * transitions and verify fsm behavior is valid.
 */
enum fsm_status
{
    FSM_EVENT_TRANSITION,  /**< DO NOT USE. Reserved. Dispatched event caused state transition. */
    FSM_EVENT_HANDLED,     /**< Dispatched event was handled by fsm. Stay in current state. */
    FSM_EVENT_IGNORED,     /**< Dispatched event was ignored. Event is not relevant for current state. Stay in current state. */
    FSM_ERROR              /**< Dispatched event or fsm behavior is invalid. Dispatcher will assert. */
};


/* Forward declaration to define fsm_func_ptr. */
struct fsm;


/**
 * @brief Prototype of all fsm state handler functions.
 */
typedef enum fsm_status (*fsm_func_ptr)(struct fsm *, const struct event *);



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- BASE FSM CLASS ------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Base fsm class. 
 * 
 * @details State is represented by a function. See @ref fsm.h for details.
 */
struct fsm
{
    /**
     * @brief PRIVATE. Current state the fsm is in.
     */
    fsm_func_ptr state;

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
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @name Constructors
 */
/**@{*/
/**
 * @brief Constructor.
 * 
 * @param fsm FSM to construct. This cannot be NULL - memory must be allocated
 * for fsm beforehand since dynamic memory allocation is not used.
 * @param init_state_0 Initial state. This is a function. This cannot be NULL.
 * @param max_state_transitions_0 Maximum number of consecutive state transitions
 * the fsm can take. This must be greater than 0.
 */
extern void fsm_ctor(struct fsm *fsm, fsm_func_ptr init_state_0, 
                     uint8_t max_state_transitions_0);
/**@}*/ /* Constructors */


/**
 * @name FSM Behavior
 */
/**@{*/
/**
 * @brief Dispatch event to fsm.
 * 
 * @details Handles state transitions by automatically executing appropriate 
 * @ref EXIT_EVENT and @ref ENTRY_EVENT cases of user's fsm. Verifies valid 
 * fsm behavior.
 * 
 * @param fsm Dispatch event to this fsm. This cannot be NULL. This must 
 * have been previously constructed via successful constructor call before 
 * using this function.
 * @param event User-defined event to dispatch. This cannot be NULL. 
 * See @ref event.h
 */
extern void fsm_dispatch(struct fsm *fsm, const struct event *event);


/**
 * @brief Transition fsm to a new state.
 * 
 * @details Application calls this inside their user-defined state
 * handler functions to signal that the dispatched event caused a
 * state transition. This function should always be used - fsm 
 * state should never be changed manually.
 * 
 * @warning This cannot be called in the @ref EXIT_EVENT case of your
 * state.
 * 
 * @param fsm Change state of this fsm. This cannot be NULL. This 
 * must have been previously constructed via successful constructor 
 * call before using this function.
 * @param new_state New state to transition into. This is a function.
 * This cannot be NULL.
 * 
 * @return The return value is used internally so the dispatcher knows
 * a state transition occurred.
 */
static inline enum fsm_status fsm_change_state(struct fsm *fsm, fsm_func_ptr new_state)
{
    RUNTIME_ASSERT( (fsm && new_state) );
    fsm->state = new_state;
    return FSM_EVENT_TRANSITION;
}
/**@}*/ /* FSM Behavior */


#endif /* FSM_H_ */
