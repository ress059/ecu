/**
 * @file
 * @brief Allows users to create linked lists (collections) of independent timers that have unique 
 * timeout periods and properties.
 * @image html timer-structure.png "Timer List Structure"
 * \n 
 * 
 * All timers within each list are updated by a single function, @ref ecu_timer_collection_tick(),
 * which must be called periodically by the application. For each timer, a unique user-defined callback
 * is called on timeout. A bare-bones example using this timer module is shown below. This example 
 * assumes a driver has been supplied by the user (explained later in the next code snippet below). 
 * This example adds two one-shot timers and one periodic timer to a timer collection and runs them.
 * @code{.c}
 * #include <ecu/timer.h>
 * 
 * /------------------------------------------------------------/
 * /-------------------- File-scope variables ------------------/
 * /------------------------------------------------------------/
 * 
 * static struct ecu_timer_collection collection;
 * static struct ecu_timer timer1;
 * static struct ecu_timer timer2;
 * static struct ecu_timer timer3;
 * 
 * 
 * /------------------------------------------------------------/
 * /---------------- Static function declarations --------------/
 * /------------------------------------------------------------/
 * 
 * static bool timer1_timeout_callback(void *obj);
 * static bool timer2_timeout_callback(void *obj);
 * static bool timer3_timeout_callback(void *obj);
 * 
 * 
 * /------------------------------------------------------------/
 * /----------------- Static function definitions --------------/
 * /------------------------------------------------------------/
 * // User-defined callbacks that run when timers expire. You can return false to run a 
 * // callback again on the next call to ecu_timer_collection_tick(). This means the callback 
 * // "failed". An example "failure" could be you tried to post to a queue but it was full.
 * 
 * static bool timer1_timeout_callback(void *obj)
 * {
 * }
 * 
 * static bool timer2_timeout_callback(void *obj)
 * {
 * }
 * 
 * static bool timer3_timeout_callback(void *obj)
 * {
 * }
 * 
 * 
 * /------------------------------------------------------------/
 * /---------- Pseudocode. Initialize and use module -----------/
 * /------------------------------------------------------------/
 * 
 * // Construct all timer objects.
 * ecu_timer_collection_ctor(&collection, &user_hardware_timer_driver);
 * ecu_timer_ctor(&timer1, (void *)&obj1_passed_to_callback, &timer1_timeout_callback);
 * ecu_timer_ctor(&timer2, (void *)&obj2_passed_to_callback, &timer2_timeout_callback);
 * ecu_timer_ctor(&timer3, (void *)&obj3_passed_to_callback, &timer3_timeout_callback);
 * 
 * // Arm and start the timers. Timers 1 and 2 are one-shot and expire once after 500
 * // ticks. Timer 3 is periodic and expires periodically after 800 ticks.
 * ecu_timer_arm(&collection, &timer1, false, 500);
 * ecu_timer_arm(&collection, &timer2, false, 500);
 * ecu_timer_arm(&collection, &timer3, true, 800);
 * 
 * void inside_timer_thread_or_main_loop(void)
 * {
 *     // Call this function periodically to update the timers.
 *     ecu_timer_collection_tick(&collection);
 * }
 * @endcode
 * \n 
 * 
 * You'll notice in the example above that @ref ecu_timer_collection_ctor() takes in
 * a pointer to a user-defined timer driver. 
 * @image html timer-user-timer.png "User-defined Timer Driver"
 * \n 
 * 
 * This allows the module to remain harware-agnostic. An interface for @ref i_ecu_timer 
 * struct is provided in @ref itimer.h. The user is pretty much only responsible for defining
 * @ref i_ecu_timer.get_ticks which is a function that returns the raw number of clock
 * ticks. The user does not have to worry about tick counter wraparound - this timer
 * module will handle this automatically and is compatible with hardware timers/tick
 * counters of any byte-width under two conditions:
 * 1. User's tick counter is an unsigned type.
 * 2. User's timer/tick counter has a width (in bytes) less than or equal to @ref ecu_max_tick_size_t.
 * I.e. @ref i_ecu_timer.tick_width_bytes <= sizeof(ecu_max_tick_size_t)
 * \n 
 * 
 * Clock ticks are the units used by this module so the target application 
 * can use a timer with any precision of their choosing (microseconds, milliseconds, etc). 
 * The application is responsible for converting ticks into appropriate units of time since 
 * this is a property of the target hardware. A bare-bones example of a user-defined driver 
 * is shown below. In this example the user's hardware target uses a 32-bit systick. A tick 
 * for this target application represents 1 milisecond. So this application calling @ref ecu_timer_arm(.., 50) 
 * creates a 50ms timer. Calling @ref ecu_timer_arm(.., 100) creates a 100ms timer, etc.
 * @code{.c}
 * #include <ecu/interface/itimer.h>
 * 
 * /------------------------------------------------------------/
 * /-------------------- File-scope variables ------------------/
 * /------------------------------------------------------------/
 * 
 * static volatile uint32_t ticks = 0;
 * static struct i_ecu_timer user_driver;
 * 
 * 
 * /------------------------------------------------------------/
 * /---------------- Static function declarations --------------/
 * /------------------------------------------------------------/
 * 
 * // Systick interrupt that increments tick counter. Fires every 1ms.
 * static void systick_isr(void);
 * 
 * // User-defined function that returns number of ticks. Must have this prototype.
 * static ecu_max_tick_size_t user_get_ticks(struct i_ecu_timer *me);
 * 
 * 
 * /------------------------------------------------------------/
 * /----------------- Static function definitions --------------/
 * /------------------------------------------------------------/
 * 
 * static void systick_isr(void)
 * {
 *     ticks++;
 * }
 * 
 * static ecu_max_tick_size_t user_get_ticks(struct i_ecu_timer *me)
 * {
 *     (void)me;
 *     ecu_max_tick_size_t ticks_copy = 0;
 *     
 *     disable_interrupts();
 *     ticks_copy = (ecu_max_tick_size_t)ticks;
 *     enable_interrupts();
 *     return ticks_copy;
 * }
 * 
 * 
 * /------------------------------------------------------------/
 * /----------------- Pseudocode. Initialize driver ------------/
 * /------------------ and feed into timer module --------------/
 * /------------------------------------------------------------/
 * 
 * i_ecu_timer_ctor(&user_driver, sizeof(ticks), &user_get_ticks);  // Width of user's tick counter = 4 bytes.
 * ecu_timer_collection_ctor(&some_timer_collection, &user_driver); // Feed driver you just created into timer collection. This is the only thing this timer module needs.
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
#include <ecu/circular_dll.h>



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ TIMER -------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @private 
 * @brief PRIVATE. Defined so this can easily be changed in the future.
 * User's hardware timer width must be less than or equal to this.
 * I.e. @ref i_ecu_timer.tick_width_bytes <= sizeof(ecu_max_tick_size_t)
 * 
 * @warning This must be an unsigned type in order to handle tick counter 
 * wraparounds. A compilation error will occur if this is declared as a 
 * signed type.
 */
typedef uint32_t ecu_max_tick_size_t;


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
 * @brief A linked list of timers.
 */
struct ecu_timer_collection
{
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
     * @brief PRIVATE. User-defined timer driver that contains hardware dependencies.
     * See @ref timer.h description for more details.
     * 
     * @note Not const qualified since @ref i_ecu_timer.get_ticks takes in non-const
     * @ref i_ecu_timer
     */
    struct i_ecu_timer *driver;

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
 * @brief Timer constructor.
 * 
 * @warning @p me cannot be an active timer apart of an existing @ref ecu_timer_collection. 
 * Otherwise the timer collection will become corrupted and behavior is undefined.
 * @warning Do not always return false in @p callback_0 otherwise callback will be 
 * called indefinately after each call to @ref ecu_timer_collection_tick().
 * 
 * @param me Timer to construct. This cannot be NULL.
 * @param object_0 Optional object to supply to @p callback_0. Supply NULL if unused.
 * @param callback_0 User-defined callback that executes when this timer expires. This
 * parameter is mandatory and cannot be NULL. This callback returns true if successful.
 * Return false if you want to run the callback again on the next call to 
 * @ref ecu_timer_collection_tick(). An example case where you could return false would 
 * be you tried to post to a queue but it was full.
 */
extern void ecu_timer_ctor(struct ecu_timer *me, 
                           void *object_0, 
                           bool (*callback_0)(void *object));


/**
 * @pre Memory already allocated for @p me.
 * @pre @p driver_0 previously constructed via call to @ref i_ecu_timer_ctor()
 * @brief Timer collection constructor.
 * 
 * @warning @p me cannot be an active collection that has timers in it.
 * Otherwise the collection will detach itself from all its timers and 
 * behavior is undefined.
 * 
 * @param me Timer collection to construct. This cannot be NULL.
 * @param driver_0 User-defined timer driver. See @ref timer.h description
 * for more details.
 */
extern void ecu_timer_collection_ctor(struct ecu_timer_collection *me,
                                      struct i_ecu_timer *driver_0);


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
 * This cannot be 0. Application is responsible for converting ticks to appropriate
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
 * @param me Timer to disarm. This must be currently armed/active.
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
 * @warning Do not preempt this function with calls to @ref ecu_timer_arm() 
 * or @ref ecu_timer_disarm().
 * 
 * @param me Timer collection to service.
 */
extern void ecu_timer_collection_tick(struct ecu_timer_collection *me);
/**@}*/


/**
 * @name Asserts In This Module
 */
/**@{*/
/**
 * @brief Set a functor to execute if an assert fires within this module. 
 * @details This is optional - if no functor is set a default one will be 
 * used. The default functor hangs in a permanent while loop if NDEBUG is 
 * not defined so users are able to inspect the call stack.
 * 
 * @param functor User-supplied functor. If a NULL value is supplied
 * the default functor will be used.
 */
extern void ecu_timer_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_TIMER_H_ */
