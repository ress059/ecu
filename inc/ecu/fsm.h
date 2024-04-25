/**
 * @file
 * @brief Event-driven finite state machine (fsm). Rather than a case statement, 
 * state is instead represented by a user-defined function:
 * @image html partial-state-definition.png "FSM State Represented By Function"
 * \n
 * 
 * Instead of polling, the application dispatches events to their fsms whenever 
 * required via the @ref ecu_fsm_dispatch() function. Users can also optionally 
 * define exit and entry handlers for each state in case a dispatched event
 * causes a state transition. So the full state definition is comprised of 
 * a user-defined state handler, entry handler, and exit handler. All of this 
 * is contained within the @ref ecu_fsm_state struct.
 * @image html full-state-definition.png "Full FSM State Representation"
 * \n 
 * 
 * This module will automatically update the internal state of your fsm.
 * It will also automatically handle state transitions by executing the
 * proper exit and entry handlers assigned to each state. For example if
 * an event is dispatched that causes a transition from State 1 to State 2
 * the following occurs:
 * @image html state-transition.png "FSM State Transition"
 * \n 
 * 
 * This is all facilitated by the @ref ecu_fsm_dispatch() function. Users
 * indicate a state change by calling @ref ecu_fsm_change_state() within their
 * state handlers. Consecutive state transitions and self-state transitions are
 * also allowed:
 * @image html consecutive-state-transitions.png "FSM Consecutive State Transitions"
 * \n
 * @image html self-state-transition.png "FSM Self State Transition"
 * \n 
 * 
 * Consecutive state transitions are accomplished by calling @ref ecu_fsm_change_state()
 * within a state's entry handler. Self-state transitions are accomplished by
 * passing your current state into @ref ecu_fsm_change_state(). For example
 * if you are in State 1 then ecu_fsm_change_state(&fsm, &state1) would cause
 * a self-transition. <b>IMPORTANT: Do not do a self-transition within your entry 
 * handler. You can see that this will cause an infinite transition loop.</b>
 * 
 * What makes this module useful is that it serves as a base class that
 * users inherit from to define their application-specific fsms. This is 
 * accomplished using C-style inheritance:
 * @code{.c}
 * struct user_fsm
 * {
 *     // Inherit base fsm class. MUST be first member.
 *     struct ecu_fsm fsm;
 * 
 *     // User fsm data
 *     bool state1_led; // true = on, false = off
 *     bool state2_led; // true = on, false = off
 * };
 * @endcode
 * \n 
 * 
 * Custom fsms are passed to this module via upcasting. Using the user_fsm
 * created in the previous code example:
 * @code{.c}
 * struct user_fsm me;
 * 
 * ecu_fsm_ctor((struct ecu_fsm *)&me, ...);
 * ecu_fsm_dispatch((struct ecu_fsm *)&me, ...);
 * @endcode
 * \n 
 * 
 * This exact same approach is used by the application to define custom 
 * events to dispatch. See @ref event.h. This same approach is also done
 * when passing state handlers to this module via casts to @ref ecu_fsm_state_handler,
 * @ref ecu_fsm_on_entry_handler, and @ref ecu_fsm_on_exit_handler. For example:
 * @code{.c}
 * // User defined state handlers. Notice function parameters are pointers
 * // to user-defined fsm and event structs. These types MUST inherit base
 * // fsm and event classes. Function parameters also MUST be pointers to
 * // these user-defined types so we can safely cast back and forth.
 * static enum ecu_fsm_status STATE_1(struct user_fsm *me, const struct user_event *event);
 * static enum ecu_fsm_status STATE_1_ON_ENTRY(struct user_fsm *me);
 * static void STATE_1_ON_EXIT(struct user_fsm *me);
 * 
 * // State 1. Shared across all user_fsm objects that are made.
 * static struct ecu_fsm_state state1;
 * 
 * // Upcast handlers.
 * ecu_fsm_state_ctor(&state1, 
 *                    (ecu_fsm_on_entry_handler)&STATE1_ON_ENTRY, 
 *                    (ecu_fsm_on_exit_handler)&STATE1_ON_EXIT,
 *                    (ecu_fsm_state_handler)&STATE_1));
 * @endcode
 * \n 
 * 
 * For completeness, a bare-bones pseudocode definition of fsm states is shown
 * below that uses the user_fsm struct defined earlier. This is a trivial example 
 * where State 1 to State 2 transitions occur when a button is pressed. 
 * state1_led is on (true) when fsm is in State 1, otherwise it is off (false). 
 * state2_led is on (true) when fsm is in State 2, otherwise it is off (false). 
 * For conciseness the base event class is used but this can be a user-defined 
 * event with more data:
 * @image html fsm-example.png "FSM Code Example (above)"
 * \n
 * 
 * @code{.c}
 * /------------------------------------------------------------/
 * /-------- User-defined State 1 function declarations --------/
 * /------------------------------------------------------------/
 * static enum ecu_fsm_status STATE_1(struct user_fsm *me, const struct ecu_event *event);
 * static enum ecu_fsm_status STATE_1_ON_ENTRY(struct user_fsm *me);
 * static void STATE_1_ON_EXIT(struct user_fsm *me);
 * 
 * // Can also construct state object at compile-time.
 * static const struct ecu_Fsm_state state1 = 
 * {
 *     .on_entry    = (ecu_fsm_on_entry_handler)&STATE_1_ON_ENTRY,
 *     .on_exit     = (ecu_fsm_on_exit_handler)&STATE_1_ON_EXIT,
 *     .handler     = (ecu_fsm_state_handler)&STATE_1
 * };
 * 
 * 
 * /------------------------------------------------------------/
 * /-------- User-defined State 2 function declarations --------/
 * /------------------------------------------------------------/
 * static enum ecu_fsm_status STATE_2(struct user_fsm *me, const struct ecu_event *event);
 * static enum ecu_fsm_status STATE_2_ON_ENTRY(struct user_fsm *me);
 * static void STATE_2_ON_EXIT(struct user_fsm *me);
 * 
 * // Can also construct state object at compile-time.
 * static const struct ecu_Fsm_state state2 = 
 * {
 *     .on_entry    = (ecu_fsm_on_entry_handler)&STATE_2_ON_ENTRY,
 *     .on_exit     = (ecu_fsm_on_exit_handler)&STATE_2_ON_EXIT,
 *     .handler     = (ecu_fsm_state_handler)&STATE_2
 * };
 * 
 * 
 * /------------------------------------------------------------/
 * /--------- User-defined State 1 function definitions --------/
 * /------------------------------------------------------------/
 * static enum ecu_fsm_status STATE_1(struct user_fsm *me, const struct ecu_event *event)
 * {
 *     enum ecu_fsm_status status = ECU_FSM_EVENT_HANDLED;
 *     
 *     switch (event->id)
 *     {
 *         case BUTTON_PRESSED_EVENT:
 *         {
 *             // Transition from State 1 to State 2 on button press.
 *             // IMPORTANT: Ensure 'status' variable is updated from this function call.
 *             status = ecu_fsm_change_state((struct ecu_fsm *)me, &state2);
 *             break;
 *         }
 * 
 *         case SOME_OTHER_EVENT_ID:
 *         {
 *             // Process other events according to current state.
 *             process_event_state1();
 *             break;
 *         }
 * 
 *         default:
 *         {
 *             // You can ignore events if it is not relevant to current state.
 *             status = ECU_FSM_EVENT_IGNORED;
 *             break;
 *         }
 *     }
 *     
 *     // IMPORTANT: Always update and return this variable accordingly within all
 *     // your state handlers so ecu_fsm_dispatch() can properly update your fsm 
 *     // and handle state transitions.
 *     return status;
 * }
 * 
 * 
 * static enum ecu_fsm_status STATE_1_ON_ENTRY(struct user_fsm *me)
 * {
 *     // Turn State 1 LED on when state is entered. Return status to
 *     // ecu_fsm_dispatch(). Note you can call ecu_fsm_change_state()
 *     // within entry events as well for consecutive state transitions.
 *     me->state1_led = true;
 *     return ECU_FSM_EVENT_HANDLED;
 * }
 * 
 * 
 * static void STATE_1_ON_EXIT(struct user_fsm *me)
 * {
 *     me->state1_led = false;
 * }
 * 
 * 
 * /------------------------------------------------------------/
 * /--------- User-defined State 2 function definitions --------/
 * /------------------------------------------------------------/
 * static enum ecu_fsm_status STATE_2(struct user_fsm *me, const struct ecu_event *event)
 * {
 *     enum ecu_fsm_status status = ECU_FSM_EVENT_HANDLED;
 *     
 *     switch (event->id)
 *     {
 *         case BUTTON_PRESSED_EVENT:
 *         {
 *             // Transition from State 2 to State 1 on button press.
 *             // IMPORTANT: Ensure 'status' variable is updated from this function call.
 *             status = ecu_fsm_change_state((struct ecu_fsm *)me, &state1);
 *             break;
 *         }
 * 
 *         case SOME_OTHER_EVENT_ID:
 *         {
 *             // Process other events according to current state.
 *             process_event_state2();
 *             break;
 *         }
 * 
 *         default:
 *         {
 *             // You can ignore events if it is not relevant to current state.
 *             status = ECU_FSM_EVENT_IGNORED;
 *             break;
 *         }
 *     }
 *     
 *     // IMPORTANT: Always update and return this variable accordingly within all
 *     // your state handlers so ecu_fsm_dispatch() can properly update your fsm 
 *     // and handle state transitions.
 *     return status;
 * }
 * 
 * 
 * static enum ecu_fsm_status STATE_2_ON_ENTRY(struct user_fsm *me)
 * {
 *     // Turn State 2 LED on when state is entered. Return status to
 *     // ecu_fsm_dispatch(). Note you can call ecu_fsm_change_state()
 *     // within entry events as well for consecutive state transitions.
 *     me->state2_led = true;
 *     return ECU_FSM_EVENT_HANDLED;
 * }
 * 
 * 
 * static void STATE_2_ON_EXIT(struct user_fsm *me)
 * {
 *     me->state2_led = false;
 * }
 * @endcode
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-14
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_FSM_H_
#define ECU_FSM_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* Runtime asserts. */
#include <ecu/asserter.h>

/* Base event class. */
#include <ecu/event.h>



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- FSM STATE ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Forward declaration for state function typedefs. */
struct ecu_fsm;


/**
 * @brief User's @ref ecu_fsm_state.handler must return one of these
 * values. This allows @ref ecu_fsm_dispatch() to take proper state
 * transitions and verify fsm behavior.
 */
enum ecu_fsm_status
{
    ECU_FSM_STATE_TRANSITION,  /**< Dispatched event caused a state transition. Never return directly. Only return via call to ecu_fsm_change_state() */
    ECU_FSM_EVENT_HANDLED,     /**< Dispatched event was handled by your fsm. Stay in current state. */
    ECU_FSM_EVENT_IGNORED,     /**< Dispatched event was ignored by your fsm. Event is not relevant for current state. Stay in current state. */
};


/**
 * @brief State entry handler in base-class form. This is a typedef 
 * to make it easier for the user to upcast from their handlers to 
 * ones used in this module. See @ref fsm.h for more details.
 */
typedef enum ecu_fsm_status (*ecu_fsm_on_entry_handler)(struct ecu_fsm *me);


/**
 * @brief State exit handler in base-class form. This is a typedef 
 * to make it easier for the user to upcast from their handlers to 
 * ones used in this module. See @ref fsm.h for more details.
 */
typedef void (*ecu_fsm_on_exit_handler)(struct ecu_fsm *me);


/**
 * @brief State handler in base-class form. This is a typedef 
 * to make it easier for the user to upcast from their handlers to 
 * ones used in this module. See @ref fsm.h for more details.
 */
typedef enum ecu_fsm_status (*ecu_fsm_state_handler)(struct ecu_fsm *me, const struct ecu_event *event);


/**
 * @brief Single state within fsm.
 */
struct ecu_fsm_state
{
    /**
     * @private 
     * @brief PRIVATE. Optional handler that executes when state is 
     * first entered.
     */
    ecu_fsm_on_entry_handler on_entry;

    /**
     * @private 
     * @brief PRIVATE. Optional handler that executes when state is 
     * exited.
     */
    ecu_fsm_on_exit_handler on_exit;

    /**
     * @private 
     * @brief PRIVATE. Handler that defines the state's behavior.
     * This is mandatory.
     */
    ecu_fsm_state_handler handler;
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- BASE FSM CLASS ------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Base fsm class that users inherit from to define
 * their own fsms. See @ref fsm.h for more details.
 */
struct ecu_fsm
{
    /**
     * @private 
     * @brief PRIVATE. Current state the fsm is in.
     */
    const struct ecu_fsm_state *state;
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
 * @pre Memory already allocated for @p me.
 * @brief State constructor.
 * 
 * @param me State to construct. This cannot be NULL.
 * @param on_entry_0 @ref ecu_fsm_state.on_entry. User-defined handler that executes 
 * when this state is first entered. Optional. Set to NULL if unused.
 * @param on_exit_0 @ref ecu_fsm_state.on_exit. User-defined handler that executes 
 * when this state is exited. Optional. Set to NULL if unused.
 * @param handler_0 @ref ecu_fsm_state.handler. User-defined handler that defines
 * this state's behavior. This is mandatory and cannot be NULL.
 */
extern void ecu_fsm_state_ctor(struct ecu_fsm_state *me, 
                               ecu_fsm_on_entry_handler on_entry_0,
                               ecu_fsm_on_exit_handler on_exit_0,
                               ecu_fsm_state_handler handler_0);


/**
 * @pre Memory already allocated for @p me.
 * @pre @p state_0 previously constructed via call to @ref ecu_fsm_state_ctor().
 * @brief FSM constructor.
 * 
 * @param me FSM to construct. This cannot be NULL.
 * @param state_0 Initial state to put fsm in. This cannot be NULL.
 */
extern void ecu_fsm_ctor(struct ecu_fsm *me, 
                         const struct ecu_fsm_state *state_0);
/**@}*/


/**
 * @name External Application Interaction with FSM
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_fsm_ctor().
 * @brief Dispatch event to fsm. Handles state transitions signaled 
 * by @ref ecu_fsm_change_state() calls by updating the fsm's state,
 * executing the proper exit handlers ( @ref ecu_fsm_state.on_exit ), 
 * and executing the proper entry handlers ( @ref ecu_fsm_state.on_entry ).
 * See @ref fsm.h for more details.
 * 
 * @param me FSM to dispatch event to.
 * @param event User-defined event to dispatch. This cannot be NULL.
 * @ref ecu_event.id must be greater than or equal to @ref ECU_VALID_EVENT_ID_BEGIN.
 */
extern void ecu_fsm_dispatch(struct ecu_fsm *me, const struct ecu_event *event);
/**@}*/


/**
 * @name Functions Called Only In Your State Handlers
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_fsm_ctor().
 * @pre @p state previously constructed via call to @ref ecu_fsm_state_ctor().
 * @brief Transition fsm into a new state.
 * 
 * @warning Only call this function within your @ref ecu_fsm_state.handler 
 * and @ref ecu_fsm_state.on_entry handlers.
 * @warning Do NOT do a self-transition within your @ref ecu_fsm_state.on_entry
 * handler or else an infinite transition loop occurs. See @ref fsm.h.
 * @warning Ensure the return value of this function is returned by your
 * handlers so @ref ecu_fsm_dispatch() controls your fsm properly.
 * 
 * @param me Change state of this fsm.
 * @param state New state to transition into.
 * 
 * @return PRIVATE. Used internally by @ref ecu_fsm_dispatch() to determine 
 * proper fsm state transitions and behavior.
 */
extern enum ecu_fsm_status ecu_fsm_change_state(struct ecu_fsm *me, 
                                                const struct ecu_fsm_state *state);
/**@}*/


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
extern void ecu_fsm_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_FSM_H_ */
