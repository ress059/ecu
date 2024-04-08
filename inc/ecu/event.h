/**
 * @file
 * @author Ian Ress
 * @brief Provides a scheme that allows users to define their own event IDs and
 * event objects.
 * @details The event ID scheme allows the library to define reserved event IDs and
 * users to define their own event IDs without conflicts. Event IDs the library
 * reserves will be negative and event IDs the user defines will start at 0 which
 * is always @ref ECU_VALID_EVENT_BEGIN. Example of user defining their own event IDs:
 * @code{.c}
 * enum user_event_ids
 * {
 *     BUTTON_PRESS_EVENT = ECU_USER_EVENT_BEGIN,
 *     TIMEOUT_EVENT,
 *     ERROR_EVENT
 * };
 * @endcode
 * 
 * This scheme also allows library functions to know when an invalid event
 * was used via @ref ECU_VALID_EVENT_BEGIN enumeration.
 * 
 * The event ID is contained within a base event class. This class serves as an
 * interface that users inherit from to add any supplemental event data. For example:
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
 * Library functions only use this base event class so users can upcast back to 
 * this base class when calling them. Using the user_event struct and user_event_ids
 * enumeration defined earlier as an example:
 * @code{.c}
 * // Library function prototype.
 * void ecu_foo(const struct ecu_event *event);
 * 
 * // Initialize your custom event.
 * struct user_event my_event;
 * my_event.event.id = TIMEOUT_EVENT; // ecu_event_id type
 * my_event.index = 5;
 * 
 * // Pass user-defined event to library function.
 * ecu_foo((const struct ecu_event *)&my_event);
 * @endcode
 * @version 0.1
 * @date 2024-04-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef ECU_EVENT_H_
#define ECU_EVENT_H_


#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ EVENT IDS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Scheme that allows the library to define reserved event IDs and
 * users to define their own event IDs without conflicts. Event IDs the library
 * defines will be negative and event IDs the user defines will start at 0 which
 * is always @ref ECU_VALID_EVENT_BEGIN. Example of user defining their own events:
 * @code{.c}
 * enum user_events
 * {
 *     BUTTON_PRESS_EVENT = ECU_USER_EVENT_BEGIN,
 *     TIMEOUT_EVENT,
 *     ERROR_EVENT
 * };
 * @endcode
 * 
 * This scheme also allows library functions to know when an invalid event
 * was used via @ref ECU_VALID_EVENT_BEGIN enumeration.
 */
enum ecu_reserved_event_ids
{
    /*--------------------------------------------------------------*/
    /*------------------ RESERVED EVENT IDS SECTION. ---------------*/
    /*-------- LAST MEMBER MUST EQUAL ECU_VALID_EVENT_BEGIN --------*/
    /*--------------------------------------------------------------*/
    /* ADD FUTURE RESERVED EVENTS HERE */
    ECU_ENTRY_EVENT             = -2,   /**< -2. FOR LIBRARY USE ONLY. Run ECU_ENTRY_EVENT of state that was entered. */
    ECU_EXIT_EVENT              = -1,   /**< -1. FOR LIBRARY USE ONLY. Run ECU_EXIT_EVENT of state that was exited. */
    ECU_VALID_EVENT_BEGIN       = 0,    /**< 0. FOR LIBRARY USE ONLY. Must be last member in this section. Represents end of reserved event IDs. */

    /*--------------------------------------------------------------*/
    /*------------------ AVAILABLE EVENT IDS SECTION. --------------*/
    /*-------- FIRST MEMBER MUST EQUAL ECU_VALID_EVENT_BEGIN -------*/
    /*--------------------------------------------------------------*/   
    ECU_USER_EVENT_BEGIN        = ECU_VALID_EVENT_BEGIN     /**< 0. Start of user-defined event IDs. Must always be 0 for future compatibility. */
};


/**
 * @private 
 * @brief PRIVATE. Generic type that represents an event ID. Used so the library
 * can implicitly typecast between this value, @ref ecu_reserved_event_ids 
 * enumerations, and user-defined event ID enumerations.
 * 
 * @warning This must be a signed integer type in order to 
 * handle @ref ecu_reserved_event_ids enumerations less than 0. A compilation 
 * error will occur if this is declared as an unsigned type.
 */
typedef int16_t ecu_event_id;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- BASE EVENT STRUCTURE ----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Base event class. Only contains the minimum event information 
 * required which is the event ID. Serves as an interface that users 
 * inherit from to add any supplemental event data. See @ref event.h 
 * for details.
 */
struct ecu_event
{
    /**
     * @brief Identifies the type of event that was dispatched.
     * 
     * @warning Must be greater than or equal to @ref ECU_VALID_EVENT_BEGIN.
     * Must follow same mechanism described in @ref ecu_reserved_event_ids
     */
    ecu_event_id id;
};


#endif /* ECU_EVENT_H_ */
