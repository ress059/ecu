/**
 * @file
 * @author Ian Ress
 * @brief Timer interface that must be defined by the user to use @ref timer.h. This file encapsulates
 * all hardware dependencies/implementation details so @ref timer.h can remain hardware-agnostic.
 * See @ref timer.h description for a bare-bones example of defining this interface.
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef INTERFACE_ECU_TIMER_H_
#define INTERFACE_ECU_TIMER_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- INCLUDES ---------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* size_t */
#include <stddef.h>

/* ecu_max_tick_size_t */
#include <ecu/timer.h>



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- TIMER SOURCE INTERFACE -------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Timer interface defined by user that can optionally be inherited
 * via C-style inheritance. Can be thought of as a virtual base class.
 */
struct i_ecu_timer
{
    /**
     * @private 
     * @brief PRIVATE. Width, in bytes, of your timer. For example if you 
     * are using a 32-bit timer this would be 4.
     * 
     * @warning This must be less than or equal to @ref ecu_max_tick_size_t.
     * I.e. @ref tick_width_bytes <= sizeof(ecu_max_tick_size_t)
     */
    size_t tick_width_bytes;

    /**
     * @private 
     * @brief PRIVATE. User-defined function that returns the raw number of
     * ticks. See @ref timer.h description for an example.
     */
    ecu_max_tick_size_t (*get_ticks)(struct i_ecu_timer *me);
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- PUBLIC FUNCTIONS -----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @pre Memory already allocated for @p me
 * @brief Interface timer constructor. Must be called before using @ref timer.h
 * 
 * @warning @p tick_width_bytes_0 must be less than or equal to @ref ecu_max_tick_size_t.
 * I.e. @p tick_width_bytes <= sizeof(ecu_max_tick_size_t)
 * 
 * @param me Interface timer to construct. This cannot be NULL.
 * @param tick_width_bytes_0 Width, in bytes, of your timer. For example if you 
 * are using a 32-bit timer this would be 4.
 * @param get_ticks_0 User-defined function that returns the raw number of 
 * ticks. This function is mandatory and cannot be NULL. See @ref timer.h 
 * description for an example.
 */
extern void i_ecu_timer_ctor(struct i_ecu_timer *me, size_t tick_width_bytes_0,
                             ecu_max_tick_size_t (*get_ticks_0)(struct i_ecu_timer *me));


/**
 * @brief Set a functor to execute if an assert fires within this module. 
 * @details This is optional - if no functor is set a default one will be 
 * used. The default functor hangs in a permanent while loop if NDEBUG is 
 * not defined so users are able to inspect the call stack.
 * 
 * @param functor User-supplied functor. If a NULL value is supplied
 * the default functor will be used.
 */
extern void i_ecu_timer_set_assert_functor(struct ecu_assert_functor *functor);

#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_ECU_TIMER_H_ */
