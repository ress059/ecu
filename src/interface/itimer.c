/**
 * @file
 * @author Ian Ress
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* Translation unit. */
#include <ecu/interface/itimer.h>

/* Runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *ASSERT_FUNCTOR = (struct ecu_assert_functor *)0;



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void i_ecu_timer_ctor(struct i_ecu_timer *me, size_t tick_width_0,
                      max_tick_size_t (*get_ticks_0)(struct i_ecu_timer *me))
{
    ECU_RUNTIME_ASSERT( (me && get_ticks_0), ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (tick_width_0 > 0 && tick_width_0 <= sizeof(max_tick_size_t)),
                        ASSERT_FUNCTOR );

    me->tick_width = tick_width_0;
    me->get_ticks = get_ticks_0;
}


void i_ecu_timer_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    ASSERT_FUNCTOR = functor;
}


#warning "TODO: Descriptions for itimer.h/.c"