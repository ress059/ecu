/**
 * @file
 * @brief See @ref timer.h description. Explanation of @ref ecu_timer_collection.overflow_mask usage is explained
 * below: 
 * The overflow mask is a property of the user's hardware timer byte-width. It allows tick counter wraparound to 
 * be automatically handled for a supplied hardware timer of any byte-width. The following examples assume 
 * @ref ecu_max_tick_size_t is a uint32_t. Note however that this module is programmed in a way such that 
 * everything will automatically scale if ecu_max_tick_size_t is ever updated in the future.
 * 
 * 1. Example wraparound for 8-bit timer:
 * overflow_mask = 0xFF;
 * starting_ticks = 255; // 0xFF
 * current_ticks = 0;
 * 
 * elapsed_ticks = (current_ticks - starting_ticks) & overflow_mask;
 * elapsed_ticks = (0 - 255) & overflow_mask; 
 * elapsed_ticks = (-255) & overflow_mask; // unsigned overflow. always defined behavior.
 * elapsed_ticks = 0xFFFFFF01 & 0xFF; // Our overflow mask gets rid of the garbage bits resulting from unsigned overflow.
 * elapsed_ticks = 0x01 = 1;
 * 
 * 2. Example wraparound for 16-bit timer:
 * overflow_mask = 0xFFFF;
 * starting_ticks = 65535; // 0xFFFF
 * current_ticks = 0;
 * 
 * elapsed_ticks = (current_ticks - starting_ticks) & overflow_mask;
 * elapsed_ticks = (0 - 65535) & overflow_mask; 
 * elapsed_ticks = (-65535) & overflow_mask; // unsigned overflow. always defined behavior.
 * elapsed_ticks = 0xFFFF0001 & 0xFFFF; // Our overflow mask gets rid of the garbage bits resulting from unsigned overflow.
 * elapsed_ticks = 0x0001 = 1;
 * 
 * This logic is exactly the same for any hardware timers of higher byte widths.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-04
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- INCLUDES ------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/timer.h"

/* Compile-time and runtime asserts. */
#include "ecu/asserter.h"



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- STATIC ASSERTS --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/* Compilation error if ecu_max_tick_size_t is a signed type. Must be unsigned. */
ECU_STATIC_ASSERT( (((ecu_max_tick_size_t)(-1)) > ((ecu_max_tick_size_t)0)), "ecu_max_tick_size_t must be an unsigned type." );



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- FILE-SCOPE VARIABLES ----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *TIMER_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DECLARATIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Additional cleanup of @ref ecu_timer that occurs when
 * @ref ecu_timer_collection_destroy() is called. More specifically
 * this is the function stored in @ref ecu_circular_dll_node.destroy
 */
static void timer_destroy(struct ecu_circular_dll_node *me);


/**
 * @brief Calculates overflow mask that should be stored in 
 * @ref ecu_timer_collection.overflow_mask. This mask is a property of 
 * the user's hardware timer byte-width and allows us to automatically 
 * handle tick counter overflow. See file description of @ref timer.c 
 * for more details.
 * 
 * @param tick_width_bytes Width, in bytes, of user's hardware timer. For 
 * example if user has a 32-bit timer this would be 4. This cannot be 0. 
 * This must be less than or equal to @ref ecu_max_tick_size_t. 
 * I.e. @ref tick_width_bytes <= sizeof(ecu_max_tick_size_t)
 */
static ecu_max_tick_size_t calculate_overflow_mask(size_t i_tick_width_bytes);


/**
 * @brief Returns true if the timer is active. False otherwise. An active timer
 * means that timer->callback exists, timer->armed == true, and timer->timeout_ticks > 0.
 * 
 * @param timer Timer to check. Cannot be NULL.
 */
static bool timer_active(const struct ecu_timer *timer);


/**
 * @brief Returns true if the timer collection is valid. False otherwise. Valid
 * means that collection->api is valid and collection->overflow_mask is set
 * properly.
 * 
 * @param collection Collection to check. Cannot be NULL.
 */
static bool timer_collection_valid(const struct ecu_timer_collection *collection);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void timer_destroy(struct ecu_circular_dll_node *me)
{
    ECU_RUNTIME_ASSERT( (me), TIMER_ASSERT_FUNCTOR );
    struct ecu_timer *t = ECU_CIRCULAR_DLL_GET_ENTRY(me, struct ecu_timer, node);
    ECU_RUNTIME_ASSERT( (t), TIMER_ASSERT_FUNCTOR );

    /* Do not call ecu_circular_dll_remove_node(). Only define
    additional cleanup independent of the node class. */
    t->object          = (void *)0;
    t->callback        = (bool (*)(void *))0;
    t->armed           = false;
    t->periodic        = false;
    t->timeout_ticks   = 0;
    t->starting_ticks  = 0;
}


static ecu_max_tick_size_t calculate_overflow_mask(size_t i_tick_width_bytes)
{
    static const ecu_max_tick_size_t MAX_NUMBER_OF_BITS = 8U * sizeof(ecu_max_tick_size_t);
    
    ECU_RUNTIME_ASSERT( (i_tick_width_bytes > 0) && \
                        (i_tick_width_bytes <= sizeof(ecu_max_tick_size_t)),
                        TIMER_ASSERT_FUNCTOR );

    /* Example: assume ecu_max_tick_size_t = 4 bytes and i_tick_width_bytes = 2 bytes.
    1) UINT32_MAX >> 32 - (8U * i_tick_width_bytes)
    2) 0xFFFFFFFF >> (32 - 16)
    3) 0xFFFFFFFF >> 16 
    4) = 0x0000FFFF
    We use this bitmask to handle overflow for user's 16-bit timer. See timer.c
    file description for more details. */
    return ( ((ecu_max_tick_size_t)(-1)) >> (MAX_NUMBER_OF_BITS - (8U * i_tick_width_bytes)) );
}


static bool timer_active(const struct ecu_timer *timer)
{
    bool active = false;

    ECU_RUNTIME_ASSERT( (timer), TIMER_ASSERT_FUNCTOR );

    if ((timer->armed) && (timer->timeout_ticks > 0) && (timer->callback))
    {
        active = true;
    }

    return active;
}


static bool timer_collection_valid(const struct ecu_timer_collection *me)
{
    bool valid = false;

    ECU_RUNTIME_ASSERT( (me), TIMER_ASSERT_FUNCTOR );

    if ((me->api.get_ticks) && \
        ((me->api.tick_width_bytes > 0) && (me->api.tick_width_bytes <= sizeof(ecu_max_tick_size_t))) && \
        (me->overflow_mask == calculate_overflow_mask(me->api.tick_width_bytes)))
    {
        valid = true;
    }

    return valid;
}



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void ecu_timer_ctor(struct ecu_timer *me, 
                    void *object_0, 
                    bool (*callback_0)(void *object))
{
    ECU_RUNTIME_ASSERT( (me && callback_0), TIMER_ASSERT_FUNCTOR );

    ecu_circular_dll_node_ctor(&me->node, &timer_destroy, ECU_OBJECT_ID_UNUSED);
    me->object          = object_0;     /* Optional so do not NULL assert. */
    me->callback        = callback_0;   /* Mandatory. */
    me->armed           = false;
    me->periodic        = false;
    me->timeout_ticks   = 0;
    me->starting_ticks  = 0;
}


void ecu_timer_collection_ctor(struct ecu_timer_collection *me,
                               ecu_max_tick_size_t (*get_ticks_0)(void *object),
                               void *object_0,
                               size_t tick_width_bytes_0)
{
    ECU_RUNTIME_ASSERT( (me && get_ticks_0), TIMER_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (tick_width_bytes_0 > 0 && tick_width_bytes_0 <= sizeof(ecu_max_tick_size_t)),
                        TIMER_ASSERT_FUNCTOR );
    
    /* Iterator is not "constructed" since it is only used and initialized in ecu_timer_collection_tick(). */
    ecu_circular_dll_ctor(&me->list);
    me->api.get_ticks           = get_ticks_0;
    me->api.object              = object_0;
    me->api.tick_width_bytes    = tick_width_bytes_0;
    me->overflow_mask           = calculate_overflow_mask(tick_width_bytes_0);
}


void ecu_timer_collection_destroy(struct ecu_timer_collection *me)
{
    ECU_RUNTIME_ASSERT( (me), TIMER_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (timer_collection_valid(me)), TIMER_ASSERT_FUNCTOR );

    ecu_circular_dll_destroy(&me->list); /* Automatically calls timer_destroy() for each ecu_timer node. */
    me->api.get_ticks           = (ecu_max_tick_size_t (*)(void *))0;
    me->api.object              = (void *)0;
    me->api.tick_width_bytes    = 0;
    me->overflow_mask           = 0;
}


void ecu_timer_arm(struct ecu_timer_collection *me, 
                   struct ecu_timer *timer, 
                   bool periodic, 
                   ecu_max_tick_size_t timeout_ticks)
{
    ECU_RUNTIME_ASSERT( ((me) && (timer) && (timeout_ticks > 0)), TIMER_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (timer_collection_valid(me)), TIMER_ASSERT_FUNCTOR );

    ecu_circular_dll_push_back(&me->list, &timer->node); /* Asserts if node already in a list. */
    timer->armed            = true;
    timer->periodic         = periodic;
    timer->timeout_ticks    = timeout_ticks;
    timer->starting_ticks   = (*me->api.get_ticks)(me->api.object);
}


void ecu_timer_disarm(struct ecu_timer *me)
{
    ECU_RUNTIME_ASSERT( (me), TIMER_ASSERT_FUNCTOR );

    ecu_circular_dll_remove_node(&me->node); /* Asserts node is in a list. */
    me->armed            = false;
    me->periodic         = false;
    me->timeout_ticks    = 0;
    me->starting_ticks   = 0;
}


void ecu_timer_collection_tick(struct ecu_timer_collection *me)
{
    bool callback_success = false;
    ecu_max_tick_size_t elapsed_ticks = 0;

    ECU_RUNTIME_ASSERT( (me), TIMER_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (timer_collection_valid(me)), TIMER_ASSERT_FUNCTOR );
    /* Don't assert if list is empty. */

    /* Currently servicing all timers in single function call to avoid scenario 
    where periodic timer with very low timeout period takes precedence over all other
    timers in the collection. This would be an issue if you break after only one timer
    expires. */
    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&me->iterator, &me->list);
         node != ecu_circular_dll_iterator_end(&me->iterator);
         node = ecu_circular_dll_iterator_next(&me->iterator))
    {
        ECU_RUNTIME_ASSERT( (node), TIMER_ASSERT_FUNCTOR );
        struct ecu_timer *t = ECU_CIRCULAR_DLL_GET_ENTRY(node, struct ecu_timer, node);
        ECU_RUNTIME_ASSERT( (t), TIMER_ASSERT_FUNCTOR );
        ECU_RUNTIME_ASSERT( (timer_active(t)), TIMER_ASSERT_FUNCTOR );

        /* Use unsigned overflow to automatically handle tick counter wraparound. Guaranteed to 
        always be unsigned overflow since we static assert ecu_max_tick_size_t is unsigned. See 
        timer.c file description for details. */
        elapsed_ticks = ((*me->api.get_ticks)(me->api.object)) - (t->starting_ticks);
        elapsed_ticks = elapsed_ticks & me->overflow_mask;

        if (elapsed_ticks >= t->timeout_ticks)
        {
            callback_success = (*t->callback)(t->object);

            if (callback_success)
            {
                if (t->periodic)
                {
                    t->starting_ticks = (*me->api.get_ticks)(me->api.object);
                }
                else 
                {
                    ecu_timer_disarm(t);
                }
            }
            else 
            {
                /* Don't update starting ticks so timer expires on next tick call so
                we can reattempt user's callback. */
            }
        }
    }
}


void ecu_timer_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    TIMER_ASSERT_FUNCTOR = functor;
}
