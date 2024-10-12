/**
 * @file
 * @brief Hardware-independent timer module that also handles tick-counter wraparound.
 * For each timer, a user-defined callback executes on timeout. This callback, along with the
 * timeout period and other properties are assigned by calling @ref ecu_timer_ctor(). This module
 * is meant to run multiple timers at once. This is done by adding timers to a linked list, whose
 * HEAD is represented by @ref ecu_timer_collection. Timers can be added or removed from a list
 * by calling @ref ecu_timer_arm() and @ref ecu_timer_disarm() functions respectively.
 * @image html timer-structure.png "Timer List Structure"
 * \n 
 * 
 * All timers within a list are updated by a single function, @ref ecu_timer_collection_tick(), 
 * which must be called periodically by the application. Each timer collection (list) is 
 * independent from one another, so @ref ecu_timer_collection_tick() must be called separately
 * on each @ref ecu_timer_collection that you have. It is also important to note that an
 * @ref ecu_timer object can only be in one collection at any time. It CANNOT be in multiple 
 * collections at the same time.
 * \n 
 * 
 * This module remains hardware-independent by providing an <b>interface</b> for the necessary
 * hardware drivers. These drivers must be supplied to this module by calling the 
 * @ref ecu_timer_collection_ctor() function. This module also only uses <b>clock ticks</b> as 
 * a unit of measurement, NOT seconds, microseconds, milliseconds, etc. This is because conversion 
 * from clock ticks to units of time is a property of the target harware. 
 * @image html timer-hardware-drivers.png "Hardware Driver Interface"
 * \n 
 * 
 * The hardware driver interface consists of:
 * 1. An @ref ecu_timer_collection.get_ticks() function, where the user returns the raw
 * number of clock ticks. You do not have to calculate elapsed ticks or handle tick-counter 
 * wraparound. This module takes care of this automatically.
 * 2. An optional object to pass into the @ref ecu_timer_collection.get_ticks() function.
 * 3. @ref ecu_timer_collection.tick_width_bytes which specifies the width, in bytes, of
 * the timer you are using. For example if you are using a 32-bit timer this would be 4.
 * 4. Optionally define a conversion between clock-ticks and units of time for easier
 * use of this module in your application.
 * 
 * \n
 * Below is a bare-bones example of defining a hardware timer driver, supplying it to 
 * this module, and using this module in an application. A 32-bit systick is used for
 * the hardware driver, which is then supplied to this module. Two one-shot timers and 
 * one periodic timer are then added to a timer collection and ran. For simplicity, no
 * (void *) objects are passed into our timer callbacks or hardware drivers but they can
 * be in order to give the application more flexibility.
 * @code{.c}
 * #include <stdint.h>
 * #include "ecu/timer.h"
 * 
 * 
 * 
 * /------------------------------------------------------------/
 * /--------------------------- Defines ------------------------/
 * /------------------------------------------------------------/
 * 
 * // For this example 1 clock tick = 1 millisecond.
 * #define MS_TO_CLOCK_TICKS(x)                 (x)
 * 
 * 
 * 
 * /------------------------------------------------------------/
 * /-------------------- File-scope variables ------------------/
 * /------------------------------------------------------------/
 * 
 * /----------------------- Harware drivers --------------------/
 * static volatile uint32_t ticks = 0;
 * 
 * 
 * /------------------------- Timer module ---------------------/
 * static struct ecu_timer_collection collection;
 * static struct ecu_timer timer1;
 * static struct ecu_timer timer2;
 * static struct ecu_timer timer3;
 * 
 * 
 * 
 * /------------------------------------------------------------/
 * /---------------- Static function declarations --------------/
 * /------------------------------------------------------------/
 * 
 * /----------------------- Harware drivers --------------------/
 * // Systick interrupt that increments tick counter. Fires every 1ms.
 * static void systick_isr(void);
 * 
 * // Returns number of raw clock ticks of our systick.
 * static ecu_max_tick_size_t get_ticks(void *obj);
 * 
 * 
 * /------------------------- Timer module ---------------------/
 * static bool timer1_timeout_callback(void *obj);
 * static bool timer2_timeout_callback(void *obj);
 * static bool timer3_timeout_callback(void *obj);
 * 
 * 
 * 
 * /------------------------------------------------------------/
 * /----------------- Static function definitions --------------/
 * /------------------------------------------------------------/
 * 
 * /----------------------- Harware drivers --------------------/
 * static void systick_isr(void)
 * {
 *     ticks++;
 * }
 * 
 * static ecu_max_tick_size_t get_ticks(void *obj)
 * {
 *     // NOTE: this would normally be guarded in a critical section since 'ticks' is
 *     // incremented inside an interrupt. This is not done here for simplicity and
 *     // to just get the point across.
 *     (void)obj;
 *     return ticks;
 * }
 * 
 * 
 * /------------------------- Timer module ---------------------/
 * // User-defined callbacks that run when timers expire. You can return false to run a 
 * // callback again on the next call to ecu_timer_collection_tick(). This means the callback 
 * // "failed". An example "failure" could be you tried to post to a queue but it was full.
 * 
 * static bool timer1_timeout_callback(void *obj)
 * {
 *     (void)obj;
 *     return true;
 * }
 * 
 * static bool timer2_timeout_callback(void *obj)
 * {
 *     (void)obj;
 *     return true;
 * }
 * 
 * static bool timer3_timeout_callback(void *obj)
 * {
 *     (void)obj;
 *     return true;
 * }
 * 
 * 
 * 
 * /------------------------------------------------------------/
 * /------------------- Initialize and use module --------------/
 * /------------------------------------------------------------/
 * 
 * // Construct all timer objects.
 * ecu_timer_collection_ctor(&collection, &get_ticks, (void *)0, sizeof(uint32_t));
 * ecu_timer_ctor(&timer1, (void *)0, &timer1_timeout_callback);
 * ecu_timer_ctor(&timer2, (void *)0, &timer2_timeout_callback);
 * ecu_timer_ctor(&timer3, (void *)0, &timer3_timeout_callback);
 * 
 * // Arm and start the timers. Timers 1 and 2 are one-shot and expire once after 500
 * // ms. Timer 3 is periodic and expires periodically after 800 ms.
 * ecu_timer_arm(&collection, &timer1, false, MS_TO_CLOCK_TICKS(500));
 * ecu_timer_arm(&collection, &timer2, false, MS_TO_CLOCK_TICKS(500));
 * ecu_timer_arm(&collection, &timer3, true, MS_TO_CLOCK_TICKS(800));
 * 
 * void inside_timer_thread_or_main_loop(void)
 * {
 *     // Call this function periodically to update the timers.
 *     ecu_timer_collection_tick(&collection);
 * }
 * @endcode
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-04
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_TIMER_H_
#define ECU_TIMER_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- INCLUDES ------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stdint.h>

/* Linked list of timers. */
#include "ecu/circular_dll.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ TIMER -------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @private 
 * @brief PRIVATE. Allows this module to handle wraparound of multiple timer
 * byte widths, up until the size specified here. Defined so this can easily be 
 * changed in the future. User's hardware timer width must be less than or equal 
 * to this. I.e. @ref ecu_timer_collection.tick_width_bytes <= sizeof(@ref ecu_max_tick_size_t)
 * 
 * @warning This must be an unsigned type in order to handle tick counter 
 * wraparounds. A compilation error will occur if this is declared as a 
 * signed type.
 */
typedef uint64_t ecu_max_tick_size_t;


/**
 * @brief Single timer, represented as a node within a linked list.
 */
struct ecu_timer
{
    /**
     * @private 
     * @brief PRIVATE. Node within timer collection linked list.
     */
    struct ecu_circular_dll_node node;

    /**
     * @private 
     * @brief PRIVATE. Optional object to pass into @ref callback
     */
    void *object;

    /**
     * @private 
     * @brief PRIVATE. User-defined callback that executes when this timer expires. 
     * Return true if callback is successful. Return false if you want to run the 
     * callback again on the next call to @ref ecu_timer_collection_tick(). An example
     * case where you could return false would be you tried to post to a queue but
     * it was full.
     * 
     * @warning Do not always return false otherwise callback will be called indefinately
     * after each call to @ref ecu_timer_collection_tick().
     */
    bool (*callback)(void *object);

    /**
     * @private 
     * @brief PRIVATE. True = timer is running. False = timer is stopped.
     */
    bool armed;

    /**
     * @private 
     * @brief PRIVATE. True = periodic timer. False = one-shot timer.
     */
    bool periodic;

    /**
     * @private 
     * @brief PRIVATE. Number of ticks that must elapse until timer expires.
     */
    ecu_max_tick_size_t timeout_ticks;

    /**
     * @private 
     * @brief PRIVATE. Used to calculate elapsed time.
     */
    ecu_max_tick_size_t starting_ticks;
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ TIMER COLLECTION --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief A linked list of timers. Also provides an interface so user's
 * can link their timer to this module.
 */
struct ecu_timer_collection
{
    /**
     * @private 
     * @brief PRIVATE. User-defined hardware drivers for whatever timer they are using.
     */
    struct 
    {
        /**
         * @private 
         * @brief PRIVATE. Returns the raw number of clock ticks. You do not have to 
         * calculate elapsed ticks or handle tick-counter wraparound. This module takes 
         * care of this automatically.
         */
        ecu_max_tick_size_t (*get_ticks)(void *object);

        /**
         * @private
         * @brief PRIVATE. Optional object to pass into @ref ecu_timer_collection.get_ticks().
         */
        void *object;

        /**
         * @private 
         * @brief PRIVATE. Width, in bytes, of the timer you are using. For example if you 
         * are using a 32-bit timer this would be 4.
         * 
         * @warning This must be less than or equal to @ref ecu_max_tick_size_t.
         * I.e. @ref tick_width_bytes <= sizeof(@ref ecu_max_tick_size_t)
         */
        size_t tick_width_bytes;
    } api;

    /**
     * @private 
     * @brief PRIVATE. List of timers.
     */
    struct ecu_circular_dll list;

    /**
     * @private 
     * @brief PRIVATE. Timer list iterator.
     */
    struct ecu_circular_dll_iterator iterator;

    /**
     * @private 
     * @brief PRIVATE. Allows tick counter wraparound to be automatically handled 
     * for a supplied hardware timer of any byte-width.
     */
    ecu_max_tick_size_t overflow_mask;
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me and @p object
 * @brief Timer constructor. Specify timer properties and a callback to execute on timeout.
 * 
 * @warning @p me cannot be an active timer apart of an existing @ref ecu_timer_collection. 
 * Otherwise the timer collection may become corrupted and behavior is undefined.
 * @warning Do not always return false in @p callback_0. Otherwise @p callback_0 will be 
 * called indefiniately after each call to @ref ecu_timer_collection_tick().
 * 
 * @param me Timer to construct. This cannot be NULL.
 * @param object_0 Optional object to supply to @p callback_0. Supply NULL if unused.
 * @param callback_0 User-defined callback that executes when this timer expires. This
 * parameter is mandatory and cannot be NULL. This callback must return true if successful.
 * Return false if you want to run the callback again on the next call to @ref ecu_timer_collection_tick(). 
 * An example case where you could return false would be you tried to post to a queue but 
 * it was full.
 * 
 * @note Once constructed, timers are started by adding them to a @ref ecu_timer_collection 
 * by calling @ref ecu_timer_arm().
 */
extern void ecu_timer_ctor(struct ecu_timer *me, 
                           void *object_0, 
                           bool (*callback_0)(void *object));


/**
 * @pre Memory already allocated for @p me.
 * @brief Timer collection constructor. A timer collection holds a linked list
 * of @ref ecu_timer objects and is used to run multiple timers at once. It also
 * provides an interface to interact with the user's hardware timer. You must supply
 * your hardware timer drivers within this constructor so this module can function. 
 * See @ref timer.h file description for more details.
 * 
 * @warning @p me cannot be an active collection that has timers in it.
 * Otherwise the collection will detach itself from all its timers and 
 * behavior is undefined.
 * @warning @p tick_width_bytes_0 must be less than or equal to @ref ecu_max_tick_size_t.
 * I.e. @p tick_width_bytes_0 <= sizeof(@ref ecu_max_tick_size_t)
 * 
 * @param me Timer collection to construct. This cannot be NULL.
 * @param get_ticks_0 User-defined function that returns the raw number of 
 * clock ticks of your hardware timer source. This function is mandatory 
 * and cannot be NULL. You do not have to calculate elapsed ticks or handle
 * tick-counter wraparound. This module takes care of this automatically.
 * @param object_0 Optional object to pass into @p get_ticks_0 function.
 * Pass in NULL if unused.
 * @param tick_width_bytes_0 Width, in bytes, of your hardware timer. For 
 * example if you are using a 32-bit timer this would be 4.
 */
extern void ecu_timer_collection_ctor(struct ecu_timer_collection *me,
                                      ecu_max_tick_size_t (*get_ticks_0)(void *object),
                                      void *object_0,
                                      size_t tick_width_bytes_0);


/**
 * @pre @p me previously constructed via call to @ref ecu_timer_collection_ctor()
 * @brief Timer collection destructor. Stops and removes all timers from the
 * collection.
 * 
 * @param me Timer collection to destroy.
 */
extern void ecu_timer_collection_destroy(struct ecu_timer_collection *me);
/**@}*/


/**
 * @name Starting Timers, Stopping Timers, and Updating Timers
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_timer_collection_ctor()
 * @pre @p timer previously constructed via call to @ref ecu_timer_ctor()
 * @brief Adds timer to the collection and starts it.
 * 
 * @param me Collection to add timer to.
 * @param timer Timer to start. Timer cannot be active, aleady within this
 * collection, or within another collection.
 * @param periodic True if timer should be periodic. False for one-shot timer.
 * @param timeout_ticks Number of ticks that must elapse before timer expires.
 * This cannot be 0. Application is responsible for converting ticks into appropriate
 * units of time since this is a property of the user's hardware.
 */
extern void ecu_timer_arm(struct ecu_timer_collection *me, 
                          struct ecu_timer *timer, 
                          bool periodic, 
                          ecu_max_tick_size_t timeout_ticks);


/**
 * @pre @p me previously constructed via call to @ref ecu_timer_ctor()
 * @brief Stops the timer and removes it from the collection.
 * 
 * @param me Timer to disarm. This timer must be currently armed/active.
 */
extern void ecu_timer_disarm(struct ecu_timer *me);


/**
 * @pre @p me previously constructed via call to @ref ecu_timer_collection_ctor()
 * @brief This function must be called periodically by the application. Updates 
 * all timers in the collection and calls user-defined callback for each timer 
 * that expires. Expired timers that were one-shot with successful callbacks
 * (returned true) are stopped and removed from the collection. Otherwise timer(s) 
 * remain active and within the collection.
 * 
 * @param me Timer collection to service.
 */
extern void ecu_timer_collection_tick(struct ecu_timer_collection *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_TIMER_H_ */
