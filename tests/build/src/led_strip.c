/**
 * @file
 * @brief See @ref led_strip.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-07
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "led_strip.h"

/* ECU library under test. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("led_strip.c")

/*------------------------------------------------------------*/
/*------------------ LED STRIP MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

void led_strip_ctor(struct led_strip *me)
{
    ECU_ASSERT( (me) );
    ecu_dlist_ctor(&me->leds);
}

void led_strip_destroy(struct led_strip *me)
{
    ECU_ASSERT( (me) );
    ecu_dlist_destroy(&me->leds);
}

void led_strip_add(struct led_strip *me, struct led *led)
{
    ECU_ASSERT( (me && led) );
    ecu_dnode_remove(&led->node);
    ecu_dlist_push_back(&me->leds, &led->node);
}

void led_strip_on(struct led_strip *me)
{
    struct ecu_dlist_iterator iterator;
    static const struct led_event ON_EVENT = {LED_ON_EVENT};
    ECU_ASSERT( (me) );

    ECU_DLIST_FOR_EACH(n, &iterator, &me->leds)
    {
        struct led *led = ECU_DNODE_GET_ENTRY(n, struct led, node);
        led_dispatch(led, &ON_EVENT);
    }
}

void led_strip_off(struct led_strip *me)
{
    struct ecu_dlist_iterator iterator;
    static const struct led_event OFF_EVENT = {LED_OFF_EVENT};
    ECU_ASSERT( (me) );

    ECU_DLIST_FOR_EACH(n, &iterator, &me->leds)
    {
        struct led *led = ECU_DNODE_GET_ENTRY(n, struct led, node);
        led_dispatch(led, &OFF_EVENT);
    }
}
