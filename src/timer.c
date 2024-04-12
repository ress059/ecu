/**
 * @file
 * @author Ian Ress
 * @brief TODO. Include logic about overflow_mask calculation.
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* Translation unit. */
#include <ecu/timer.h>

/* Compile-time and runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- STATIC ASSERTS --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/* Compilation error if ecu_max_tick_size_t is a signed type. Must be unsigned. */
ECU_STATIC_ASSERT( (((ecu_max_tick_size_t)(-1)) > ((ecu_max_tick_size_t)0)) );



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *TIMER_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ STATIC FUNCTION DECLARATIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Calculated overflow mask that should be stored in member 'overflow_mask' 
 * of struct @ref ecu_timer_collection. This mask depends on the width of the user's
 * tick counter. See file description of @ref timer.c for more details.
 * 
 * @param tick_width_bytes The width of the user's tick counter, in number of bytes.
 * For example if the user's target uses a 16-bit tick counter this would be 2.
 */
static ecu_max_tick_size_t calculate_overflow_mask(size_t i_tick_width_bytes);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

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
    4) = 0x0000FFFFF
    We use this bitmask to handle overflow for user's 16-bit timer. See timer.c
    file description for more details. */
    return ( ((ecu_max_tick_size_t)(-1)) >> (MAX_NUMBER_OF_BITS - (8U * i_tick_width_bytes)) );
}


/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void ecu_timer_ctor(struct ecu_timer *me, void *object_0, 
                    bool (*callback_0)(void *object))
{
    ECU_RUNTIME_ASSERT( (me && callback_0), TIMER_ASSERT_FUNCTOR );

    // static timer destructor??
    ecu_circular_dll_node_ctor(&me->node, timer destructor???, ECU_OBJECT_ID_UNUSED);
    me->object          = object_0;     /* Optional so do not NULL assert. */
    me->callback        = callback_0;   /* Mandatory. */
    me->armed           = false;
    me->periodic        = false;
    me->timeout_ticks   = 0;
    me->starting_ticks  = 0;
}


void ecu_timer_collection_ctor(struct ecu_timer_collection *me,
                               const struct i_ecu_timer *driver_0)
{
    ECU_RUNTIME_ASSERT( (me && driver_0), TIMER_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (driver_0->get_ticks), TIMER_ASSERT_FUNCTOR );

    /* To handle tick-counter overflow the width of the user's counter 
    must be less than the width of ecu_max_tick_size_t typedef. */
    ECU_RUNTIME_ASSERT( (driver_0->tick_width_bytes > 0) && \
                        (driver_0->tick_width_bytes <= sizeof(ecu_max_tick_size_t)),
                        TIMER_ASSERT_FUNCTOR );
    
    ecu_circular_dll_ctor(&me->list);
    me->overflow_mask = calculate_overflow_mask(driver_0->tick_width_bytes);
}


void ecu_timer_collection_destroy(struct ecu_timer_collection *me)
{

}


void ecu_timer_collection_arm(struct ecu_timer_collection *me, struct ecu_timer *timer, 
                              bool periodic, ecu_max_tick_size_t timeout_ticks);
{
    ECU_RUNTIME_ASSERT( (me && timer) );
    ECU_RUNTIME_ASSERT( (me->driver), TIMER_ASSERT_FUNCTOR ); // offload to is_driver_valid() function.
    ECU_RUNTIME_ASSERT( (me->driver->get_ticks), TIMER_ASSERT_FUNCTOR ); // offload to is_driver_valid() function.
    // ECU_RUNTIME_ASSERT( (me->driver->tick_width > 0 && \
    // me->driver->tick_width <= sizeof(ecu_max_tick_size_t) )) // would rather to overflowmask instead.

    if (driver_0->tick_width == 1)
    {
        me->overflow_mask must be 0xFF
    }
    else if (driver_0->tick_width == 2)
    {
        me->overflow_mask must be 0xFFFF
    }

    ECU_RUNTIME_ASSERT( (timer->callback) && (timer->timeout_ticks > 0) );

    ecu_circular_dll_push_back(&me->list, &timer->node); /* Asserts if node already in list. */
    timer->armed = true;
    timer->starting_ticks = (*me->driver->get_ticks)(me->driver);
}


void ecu_timer_collection_disarm(struct ecu_timer_collection *me, 
                                 struct ecu_timer *timer)
{
    ECU_RUNTIME_ASSERT( (me && timer) );

    // TODO this will be updated. currently takes in 2 parameters - list and node.
    // Most recent is just node and will assert that node is apart of the list.
    ecu_circular_dll_remove_node(&timer->node);
    timer->armed = false;
}


void ecu_timer_collection_tick(struct ecu_timer_collection *me)
{
    bool callback_success = false;
    ecu_max_tick_size_t elapsed_ticks = 0;

    ECU_RUNTIME_ASSERT( (me), TIMER_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me->driver), TIMER_ASSERT_FUNCTOR );
    /* Don't assert if list is empty. */

    for (struct ecu_circular_dll_node *t = ecu_circular_dll_iterator_begin(&me->list, &me->iterator);
         t != ecu_circular_dll_iterator_end(&me->iterator);
         t = ecu_circular_dll_iterator_next(&me->iterator))
    {
        struct ecu_timer *t = ECU_CIRCULAR_DLL_GET_ENTRY(t, struct ecu_timer, node);
        ECU_RUNTIME_ASSERT( (t), TIMER_ASSERT_FUNCTOR );
        ECU_RUNTIME_ASSERT( ((t->callback) && (t->armed) && (t->timeout_ticks > 0)), TIMER_ASSERT_FUNCTOR );

        /* Unsigned overflow and masking automatically handles wraparound,
        even if user's tick counter has smaller width than ecu_max_tick_size_t. */
        elapsed_ticks = ((*me->driver->get_ticks)(me->driver)) - (t->starting_ticks);
        elapsed_ticks = elapsed_ticks & me->overflow_mask;

        if (elapsed_ticks >= t->timeout_ticks)
        {
            callback_success = (*t->callback)(t->object);

            if (callback_success)
            {
                if (t->periodic)
                {
                    t->starting_ticks = (*me->driver->get_ticks)(me->driver);
                }
                else 
                {
                    ecu_timer_collection_disarm(me, t);
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


// /* Guaranteed to always be defined since this is unsigned overflow.
// Do this so we don't have to make another define that depends on 
// the size of ecu_max_tick_size_t (UINT16_MAX, UINT32_MAX,..etc). */
// static const ecu_max_tick_size_t MAX_VALUE = (ecu_max_tick_size_t)(-1);

#warning "TODO: Descriptions for timer.h/.c"