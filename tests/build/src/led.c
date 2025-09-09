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

/* ECU library under test. */
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
static void led_on_entry(struct led *me);

/**
 * @brief Handler for LED_ON_STATE.
 * 
 * @param event The @ref led_event that was dispatched.
 */
static void led_on_handler(struct led *me, const void *event);

/**
 * @brief Exit handler for LED_OFF_STATE.
 */
static void led_off_entry(struct led *me);

/**
 * @brief Handler for LED_OFF_STATE.
 * 
 * @param event The @ref led_event that was dispatched.
 */
static void led_off_handler(struct led *me, const void *event);

/*------------------------------------------------------------*/
/*------------------- FILE SCOPE VARIABLES -------------------*/
/*------------------------------------------------------------*/

static const struct ecu_fsm_state LED_ON_STATE = ECU_FSM_STATE_CTOR(
    &led_on_entry, ECU_FSM_STATE_EXIT_UNUSED, &led_on_handler
);

static const struct ecu_fsm_state LED_OFF_STATE = ECU_FSM_STATE_CTOR(
    &led_off_entry, ECU_FSM_STATE_EXIT_UNUSED, &led_off_handler
);

/*------------------------------------------------------------*/
/*------------------------ STATIC ASSERTS --------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (ECU_FSM_IS_BASEOF(fsm, struct led)), "LED must inherit ecu_fsm." ); 

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

static void led_on_entry(struct led *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (me->api.turn_on) );
    (*me->api.turn_on)(me->api.obj);
}

static void led_on_handler(struct led *me, const void *event)
{
    ECU_ASSERT( (me && event) );
    const struct led_event *e = (const struct led_event *)event;

    switch (e->id)
    {
        case LED_OFF_EVENT:
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_OFF_STATE);
            break;
        }

        case LED_BUTTON_PRESS_EVENT:
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_OFF_STATE);
            break;
        }

        default:
        {
            /* Ignore all other events. */
            break;
        }
    }
}

static void led_off_entry(struct led *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (me->api.turn_off) );
    (*me->api.turn_off)(me->api.obj);
}

static void led_off_handler(struct led *me, const void *event)
{
    ECU_ASSERT( (me && event) );
    const struct led_event *e = (const struct led_event *)event;

    switch (e->id)
    {
        case LED_ON_EVENT:
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_ON_STATE);
            break;
        }

        case LED_BUTTON_PRESS_EVENT:
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_ON_STATE);
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

    ecu_fsm_ctor(ECU_FSM_BASE_CAST(me), &LED_OFF_STATE);
    ecu_dnode_ctor(&me->node, &led_cleanup, ECU_OBJECT_ID_UNUSED);
    me->api.turn_on = turn_on;
    me->api.turn_off = turn_off;
    me->api.obj = obj;
}

void led_start(struct led *me)
{
    ECU_ASSERT( (led_is_constructed(me)) );
    ecu_fsm_start(ECU_FSM_BASE_CAST(me));
}

void led_destroy(struct led *me)
{
    ECU_ASSERT( (led_is_constructed(me)) );

    ecu_dnode_destroy(&me->node);
    me->api.turn_on = (void (*)(void *))0;
    me->api.turn_off = (void (*)(void *))0;
    me->api.obj = (void *)0;
}

void led_remove(struct led *me)
{
    ECU_ASSERT( (led_is_constructed(me)) );
    ecu_dnode_remove(&me->node);
}

void led_dispatch(struct led *me, const struct led_event *event)
{
    ECU_ASSERT( (event) );
    ECU_ASSERT( (led_is_constructed(me)) );
    ecu_fsm_dispatch(ECU_FSM_BASE_CAST(me), event);
}
