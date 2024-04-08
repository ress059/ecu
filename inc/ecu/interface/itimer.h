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


#ifndef INTERFACE_ECU_TIMER_H_
#define INTERFACE_ECU_TIMER_H_


/* max_tick_size_t */
#include <ecu/timer.h>



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ TIMER SOURCE INTERFACE --------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

struct i_ecu_timer
{
    size_t tick_width;
    max_tick_size_t (*get_ticks)(struct i_ecu_timer *me);
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- PUBLIC FUNCTIONS -----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern void i_ecu_timer_ctor(struct i_ecu_timer *me, size_t tick_width_0,
                                     max_tick_size_t (*get_ticks_0)(struct i_ecu_timer *me));


extern void i_ecu_timer_set_assert_functor(struct ecu_assert_functor *functor);


#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_ECU_TIMER_H_ */
