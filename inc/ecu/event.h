/**
 * @file
 * @brief Base event class. Serves as an interface that users 
 * inherit from to define their own events that will be dispatched 
 * to library functions.
 * @details Users can define their own events with application-specific
 * data by inheriting this base event struct. For example:
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
 * Library functions only use this base event class so users
 * can upcast back to this base class when calling them. 
 * Using the user_event struct defined earlier as an example:
 * 
 *     void foo(const struct event *event); // Library function.
 * 
 *     struct user_event my_event; 
 * 
 *     foo((const struct event *)&my_event);
 * 
 * This approach also makes it easier for any event struct to be
 * copied over to an asynchronous queue by value. Separating out
 * the event signal from the data would make this more difficult.
 * I.e.
 * 
 *     foo(event_signal sig, const void *data);
 * 
 *     // vs.
 *     struct user_event event;
 *     foo((const void *)&event);
 * 
 * @author Ian Ress
 * 
 */


#ifndef EVENT_BASE_CLASS_H_
#define EVENT_BASE_CLASS_H_


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
 * user-defined event signals. Values less than @ref IDLE_EVENT are 
 * reserved for internal use by the library. The first user-definable 
 * signal starts at @ref USER_EVENT_BEGIN. An example user event declaration
 * would be:
 * 
 *     enum user_event_signals
 *     {
 *         BUTTON_PRESS_EVENT = USER_EVENT_BEGIN,
 *         TIMEOUT_EVENT,
 *         ERROR_EVENT
 *     };
 */
enum reserved_event_signals
{
    ENTRY_EVENT             = -3,   /**< -3. DO NOT USE. Reserved. Run ENTRY_EVENT of state that was entered. */
    EXIT_EVENT              = -2,   /**< -2. DO NOT USE. Reserved. Run EXIT_EVENT of state that was exited. */
    /****************************/
    IDLE_EVENT              = -1,   /**< -1. First signal available to users. Background processing event. */
    USER_EVENT_BEGIN        = 0     /**< 0. Start of user-defined signals. */
};


/**
 * @brief Event signal. Identifies the type of event that was dispatched. 
 * 
 * @details This is a generic type that the library can use to implicitly
 * typecast between this value, @ref reserved_event_signals enumeration, 
 * and user-defined event signal enumerations. This must be a signed integer 
 * type in order to handle @ref reserved_event_signals enumerations less than 0.
 */
typedef int16_t event_signal;



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
struct event
{
    /**
     * @details Identifies the type of event that was dispatched.
     * Must be greater than or equal to @ref IDLE_EVENT
     */
    event_signal signal;
};


#endif /* EVENT_BASE_CLASS_H_ */
