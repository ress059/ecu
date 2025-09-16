/**
 * @file
 * @brief See @ref led.h.
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
#include "led.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("led.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if LED was successfully constructed.
 * False otherwise.
 */
static bool led_is_constructed(const struct led *led);

/**
 * @brief Additional cleanup that happens when LED is destroyed.
 * 
 * @param led LED being destroyed.
 * @param id Unused.
 */
static void led_cleanup(struct ecu_dnode *led, ecu_object_id id);

/**
 * @brief Entry handler for LED_ON_STATE.
 */
static void led_on_entry(struct ecu_fsm *led);

/**
 * @brief Handler for LED_ON_STATE.
 * 
 * @param event The @ref led_event that was dispatched.
 */
static void led_on_handler(struct ecu_fsm *led, const void *event);

/**
 * @brief Exit handler for LED_OFF_STATE.
 */
static void led_off_entry(struct ecu_fsm *led);

/**
 * @brief Handler for LED_OFF_STATE.
 * 
 * @param event The @ref led_event that was dispatched.
 */
static void led_off_handler(struct ecu_fsm *led, const void *event);

/*------------------------------------------------------------*/
/*--------------------- STATIC VARIABLES ---------------------*/
/*------------------------------------------------------------*/

static const struct ecu_fsm_state LED_ON_STATE = ECU_FSM_STATE_CTOR(
    &led_on_entry, ECU_FSM_STATE_EXIT_UNUSED, &led_on_handler
);

static const struct ecu_fsm_state LED_OFF_STATE = ECU_FSM_STATE_CTOR(
    &led_off_entry, ECU_FSM_STATE_EXIT_UNUSED, &led_off_handler
);

/*------------------------------------------------------------*/
/*------------------ STATIC FUNCTION DEFINITIONS -------------*/
/*------------------------------------------------------------*/

static bool led_is_constructed(const struct led *led)
{
    ECU_ASSERT( (led) );
    return (led->api.turn_on && led->api.turn_off);
}

static void led_cleanup(struct ecu_dnode *led, ecu_object_id id)
{
    (void)id;
    ECU_ASSERT( (led) );
    static const struct led_event OFF_EVENT = {LED_OFF_EVENT};
    
    struct led *me = ECU_DNODE_GET_ENTRY(led, struct led, node);
    led_dispatch(me, &OFF_EVENT);
}

static void led_on_entry(struct ecu_fsm *led)
{
    ECU_ASSERT( (led) );
    struct led *me = ECU_FSM_GET_CONTEXT(led, struct led, fsm);

    ECU_ASSERT( (me->api.turn_on) );
    (*me->api.turn_on)(me->api.obj);
}

static void led_on_handler(struct ecu_fsm *led, const void *event)
{
    ECU_ASSERT( (led && event) );
    const struct led_event *e = (const struct led_event *)event;

    switch (e->id)
    {
        case LED_OFF_EVENT:
        {
            ecu_fsm_change_state(led, &LED_OFF_STATE);
            break;
        }

        case LED_BUTTON_PRESS_EVENT:
        {
            ecu_fsm_change_state(led, &LED_OFF_STATE);
            break;
        }

        default:
        {
            /* Ignore all other events. */
            break;
        }
    }
}

static void led_off_entry(struct ecu_fsm *led)
{
    ECU_ASSERT( (led) );
    struct led *me = ECU_FSM_GET_CONTEXT(led, struct led, fsm);

    ECU_ASSERT( (me->api.turn_off) );
    (*me->api.turn_off)(me->api.obj);
}

static void led_off_handler(struct ecu_fsm *led, const void *event)
{
    ECU_ASSERT( (led && event) );
    const struct led_event *e = (const struct led_event *)event;

    switch (e->id)
    {
        case LED_ON_EVENT:
        {
            ecu_fsm_change_state(led, &LED_ON_STATE);
            break;
        }

        case LED_BUTTON_PRESS_EVENT:
        {
            ecu_fsm_change_state(led, &LED_ON_STATE);
            break;
        }
        
        default:
        {
            /* Ignore all other events. */
            break;
        }
    }
}

/*------------------------------------------------------------*/
/*-------------------- LED MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void led_ctor(struct led *me,
              void (*turn_on)(void *obj),
              void (*turn_off)(void *obj),
              void *obj)
{
    ECU_ASSERT( (me && turn_on && turn_off) );

    ecu_dnode_ctor(&me->node, &led_cleanup, ECU_OBJECT_ID_UNUSED);
    ecu_fsm_ctor(&me->fsm, &LED_OFF_STATE);
    me->api.turn_on = turn_on;
    me->api.turn_off = turn_off;
    me->api.obj = obj;
}

void led_start(struct led *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (led_is_constructed(me)) );
    ecu_fsm_start(&me->fsm);
}

void led_destroy(struct led *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (led_is_constructed(me)) );

    ecu_dnode_destroy(&me->node);
    me->api.turn_on = (void (*)(void *))0;
    me->api.turn_off = (void (*)(void *))0;
    me->api.obj = (void *)0;
}

void led_dispatch(struct led *me, const struct led_event *event)
{
    ECU_ASSERT( (me && event) );
    ECU_ASSERT( (led_is_constructed(me)) );
    ecu_fsm_dispatch(&me->fsm, event);
}

void led_remove(struct led *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (led_is_constructed(me)) );
    ecu_dnode_remove(&me->node);
}
