/**
 * @file timer.h
 * @author Ian Ress
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef ECU_TIMER_H_
#define ECU_TIMER_H_


/* STDLib. */
#include <stdbool.h>
#include <stdint.h>

/* Linked list of timers. */
#include <ecu/circular_dll.h>



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- TIMER ---------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

typedef uint32_t max_tick_size_t; // in case we need to expand to uint64_t in the future. MUST BE UNSIGNED.

struct ecu_timer
{
    struct ecu_circular_dll_node node;
    void *object;
    // Do not edit contents of ecu_timer within your callback.
    bool (*callback)(void *object);
    bool armed;
    bool periodic;
    // Mask bits if counter size < sizeof(uint32_t)
    // uint32_t since these avlues must always be greater than sizeof(user_target's_tick_width);
    // In all cases the tick width supplied by user will always be less than or equal to this value
    // We can mask the corresponding bits to handle overflow.
    max_tick_size_t timeout_ticks; // number of ms that have to elapse until timeout.
    max_tick_size_t starting_ticks;
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ TIMER COLLECTION --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

struct ecu_timer_collection
{
    struct ecu_circular_dll_list list;
    struct ecu_circular_dll_iterator iterator;
    const struct i_ecu_timer *driver; // TODO: May do something differently.
    max_tick_size_t overflow_mask;
    // max elements?
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern void ecu_timer_ctor(struct ecu_timer *me, void *object_0, 
                           bool (*callback_0)(void *object));

extern void ecu_timer_collection_ctor(struct ecu_timer_collection *me,
                                      const struct i_ecu_timer *driver_0);


extern void ecu_timer_collection_arm(struct ecu_timer_collection *me, struct ecu_timer *timer, 
                                     bool periodic, max_tick_size_t timeout_ticks);


extern void ecu_timer_collection_disarm(struct ecu_timer_collection *me, 
                                        struct ecu_timer *timer);


extern void ecu_timer_collection_tick(struct ecu_timer_collection *me); // how to allow one standalone timer.


extern void ecu_timer_set_assert_functor(struct ecu_assert_functor *functor);

#ifdef __cplusplus
}
#endif

#endif /* ECU_TIMER_H_ */
