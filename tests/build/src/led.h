/**
 * @file
 * @brief Standalone LED.
 * They can also be added to an @ref led_strip. For testing 
 * compilation and linkage of ECU.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-07
 * @copyright Copyright (c) 2025
 */

#ifndef LED_H_
#define LED_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* ECU. */
#include "ecu/dlist.h"
#include "ecu/fsm.h"

/*------------------------------------------------------------*/
/*---------------------------- LED ---------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief LED object. Can also be added to 
 * an @ref led_strip.
 */
struct led
{
    /// @brief Linked list node in case the LED is added to 
    /// an @ref led_strip.
    struct ecu_dnode node;

    /// @brief LED is represented by a finite state machine built
    /// using ECU FSM framework.
    struct ecu_fsm fsm;

    /// @brief Dependency injection. Contains user's hardware-specific code.
    struct 
    {
        /// @brief User-defined function that turns the LED on.
        void (*turn_on)(void *obj);

        /// @brief User-defined function that turns the LED off.
        void (*turn_off)(void *obj);

        /// @brief Optional object passed to callbacks in api struct.
        void *obj;
    } api;
};

/*------------------------------------------------------------*/
/*--------------------------- LED EVENTS ---------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Identifies event type dispatched to LED FSM.
 */
enum led_event_id
{
    LED_OFF_EVENT,          /**< Application requested LED to turn off. */
    LED_ON_EVENT,           /**< Application requested LED to turn on. */
    LED_BUTTON_PRESS_EVENT  /**< Button attached to LED was pressed so it should be toggled. */
};

/**
 * @brief Event dispatched to LED FSM.
 */
struct led_event
{
    enum led_event_id id;
};


/*------------------------------------------------------------*/
/*-------------------- LED MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @pre Memory already allocated for @p me.
 * @brief LED constructor.
 * 
 * @param me LED to construct. This cannot be an already active
 * LED otherwise behavior is undefined.
 * @param turn_on Dependency injection. Mandatory callback that 
 * turns the LED on.
 * @param turn_off Dependency injection. Mandatory callback that 
 * turns the LED off.
 * @param obj Optional object to pass to @p turn_on and @p turn_off.
 */
extern void led_ctor(struct led *me,
                     void (*turn_on)(void *obj),
                     void (*turn_off)(void *obj),
                     void *obj);

/**
 * @pre @p me constructed via @ref led_ctor().
 * @brief Destroys the LED. It will be turned off and also 
 * removed if it was in a strip.
 * 
 * @warning This does not free memory since dynamic memory
 * allocation is not used. The destroyed LED object
 * must be reconstructed in order to be used again.
 * 
 * @param me LED to destroy.
 */
extern void led_destroy(struct led *me);

/**
 * @pre @p me constructed via @ref led_ctor().
 * @brief Dispatches event to LED FSM. Wrapper around
 * base class function @ref ecu_fsm_dispatch() so specific
 * event type can be enforced.
 * 
 * @param me LED to update.
 * @param event Event to dispatch.
 */
extern void led_dispatch(struct led *me, const struct led_event *event);

/**
 * @pre @p me constructed via @ref led_ctor().
 * @brief Removes the LED if it was in a strip. Otherwise
 * does nothing.
 * 
 * @param me LED to remove.
 */
extern void led_remove(struct led *me);

#ifdef __cplusplus
}
#endif

#endif /* LED_H_ */
