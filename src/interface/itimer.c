/**
 * @file
 * @brief See @ref itimer.h and @ref timer.h descriptions.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-04
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- INCLUDES ---------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include <ecu/interface/itimer.h>

/* Runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- FILE-SCOPE VARIABLES ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *ITIMER_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- PUBLIC FUNCTIONS -----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void i_ecu_timer_ctor(struct i_ecu_timer *me, 
                      size_t tick_width_bytes_0,
                      ecu_max_tick_size_t (*get_ticks_0)(struct i_ecu_timer *me))
{
    ECU_RUNTIME_ASSERT( (me && get_ticks_0), ITIMER_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (tick_width_bytes_0 > 0 && tick_width_bytes_0 <= sizeof(ecu_max_tick_size_t)),
                        ITIMER_ASSERT_FUNCTOR );

    me->tick_width_bytes = tick_width_bytes_0;
    me->get_ticks = get_ticks_0;
}


void i_ecu_timer_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    ITIMER_ASSERT_FUNCTOR = functor;
}
