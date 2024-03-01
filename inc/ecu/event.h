/**
 * @file
 * @brief Base event class. Serves as an interface that users 
 * inherit from to define their own events that will be dispatched 
 * to library functions.
 * @details Users can define their own events with application-specific
 * data by inheriting this base event struct. For example:
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
 * @endcode
 * 
 * Library functions only use this base event class so users
 * can upcast back to this base class when calling them. 
 * Using the user_event struct defined earlier as an example:
 * 
 * @code{.c}
 * void ecu_foo(const struct ecu_event *event); // Library function prototype.
 * 
 * struct user_event my_event; 
 * 
 * ecu_foo((const struct ecu_event *)&my_event);
 * @endcode
 * 
 * This approach also makes it easier for any event struct to be
 * copied over to an asynchronous queue by value. Separating out
 * the event signal from the data would make this more difficult.
 * I.e.
 * 
 * @code{.c}
 * foo(ecu_event_signal sig, const void *data);
 * 
 * // vs.
 * struct user_event event;
 * foo((const void *)&event);
 * @endcode
 * 
 * @author Ian Ress
 * 
 */


#ifndef ECU_EVENT_H_
#define ECU_EVENT_H_


#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- EVENT SIGNALS --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Template that users should follow to define their own event 
 * signals.
 * 
 * @details Event signals identify the type of event that was dispatched.
 * This template ensures there is no conflict between reserved and 
 * user-defined event signals. Values less than @ref ECU_IDLE_EVENT are 
 * reserved for internal use by the library. The first user-definable 
 * signal starts at @ref ECU_USER_EVENT_BEGIN. An example user event 
 * declaration would be:
 * @code{.c}
 * enum user_event_signals
 * {
 *     BUTTON_PRESS_EVENT = ECU_USER_EVENT_BEGIN,
 *     TIMEOUT_EVENT,
 *     ERROR_EVENT
 * };
 * @endcode
 */
enum ecu_reserved_event_signals
{
    ECU_ENTRY_EVENT             = -3,   /**< -3. DO NOT USE. Reserved. Run ECU_ENTRY_EVENT of state that was entered. */
    ECU_EXIT_EVENT              = -2,   /**< -2. DO NOT USE. Reserved. Run ECU_EXIT_EVENT of state that was exited. */
    /****************************/
    ECU_IDLE_EVENT              = -1,   /**< -1. First signal available to users. Background processing event. */
    ECU_USER_EVENT_BEGIN        = 0     /**< 0. Start of user-defined signals. */
};


/**
 * @brief Event signal. Identifies the type of event that was dispatched. 
 * 
 * @details This is a generic type that the library can use to implicitly
 * typecast between this value, @ref ecu_reserved_event_signals enumeration, 
 * and user-defined event signal enumerations. This must be a signed integer 
 * type in order to handle @ref ecu_reserved_event_signals enumerations less 
 * than 0.
 */
typedef int16_t ecu_event_signal;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- BASE EVENT STRUCTURE ----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Base event class. 
 * 
 * @details Serves as an interface that users inherit from to 
 * define their own events that will be dispatched to library functions. 
 * See @ref event.h for details.
 */
struct ecu_event
{
    /**
     * @brief Identifies the type of event that was dispatched.
     * Must be greater than or equal to @ref ECU_IDLE_EVENT
     */
    ecu_event_signal signal;
};


#endif /* ECU_EVENT_H_ */
