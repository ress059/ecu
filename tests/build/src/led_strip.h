/**
 * @file
 * @brief Collection of LEDs, represented as a linked list. For 
 * testing compilation and linkage of ECU.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-07
 * @copyright Copyright (c) 2025
 */

#ifndef LED_STRIP_H_
#define LED_STRIP_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* ECU library under test. */
#include "ecu/dlist.h"

/* Application. */
#include "led.h"

/*------------------------------------------------------------*/
/*------------------------- LED STRIP ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Collection of LEDs, represented as a linked list.
 */
struct led_strip
{
    struct ecu_dlist leds;
};

/*------------------------------------------------------------*/
/*------------------ LED STRIP MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @pre Memory already allocated for @p me.
 * @brief LED strip constructor.
 * 
 * @param me LED strip to construct. This cannot be
 * an already active LED strip, otherwise behavior is
 * undefined.
 */
extern void led_strip_ctor(struct led_strip *me);

/**
 * @pre @p me constructed via @ref led_strip_ctor().
 * @brief Destroys the LED strip and all LEDs within the
 * strip. All LEDs are turned off.
 * 
 * @warning This does not free memory since dynamic memory
 * allocation is not used. The LED strip and all LED object
 * must be reconstructed in order to be used again.
 * 
 * @param me LED strip to destroy.
 */
extern void led_strip_destroy(struct led_strip *me);

/**
 * @pre @p me constructed via @ref led_strip_ctor() and 
 * @p led constructed via @ref led_ctor().
 * @brief Adds an LED to the strip. 
 * 
 * @param me LED strip to add to.
 * @param led LED to add. If this LED is in another strip,
 * it will be removed and added to this one.
 */
extern void led_strip_add(struct led_strip *me, struct led *led);

/**
 * @pre @p me constructed via @ref led_strip_ctor().
 * @brief Turns all LEDs in the strip on.
 * 
 * @param me LED strip to update.
 */
extern void led_strip_on(struct led_strip *me);

/**
 * @pre @p me constructed via @ref led_strip_ctor().
 * @brief Turns all LEDs in the strip off. 
 * 
 * @param me LED strip to update.
 */
extern void led_strip_off(struct led_strip *me);

#ifdef __cplusplus
}
#endif

#endif /* LED_STRIP_H_*/
