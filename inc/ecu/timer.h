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
 * @date 2025-04-04
 * @copyright Copyright (c) 2025
 */

#ifndef ECU_TIMER_H_
#define ECU_TIMER_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>

/* Linked list of timers. */
#include "ecu/dlist.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Convenience define for @ref ecu_timer_ctor(). Pass to
 * this function if optional callback object is not needed.
 */
#define ECU_TIMER_OBJ_UNUSED \
    ((void *)0)

/**
 * @brief Convenience define for @ref ecu_tlist_ctor(). Pass to
 * this function if optional callback object is not needed.
 */
#define ECU_TLIST_OBJ_UNUSED \
    ((void *)0)

/*------------------------------------------------------------*/
/*------------------------- TIMER TYPES ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Represents a type that can hold the maximum number
 * of hardware timer ticks. This is currently size_t since it
 * is assumed a hardware timer's resolution cannot exceed the
 * the word-size of the target device. This assumption requires
 * the hardware timer to be within the target device. Typedeffed
 * in case this needs to change in the future.
 */
typedef size_t ecu_timer_tick_t;

/**
 * @brief Type of @ref ecu_timer used.
 */
enum ecu_timer_type
{
    ECU_TIMER_TYPE_ONE_SHOT, /**< Once timer expires it is stopped. */
    ECU_TIMER_TYPE_PERIODIC, /**< Once the timer expires it is automatically restarted. */
    /********************/
    ECU_TIMER_TYPES_COUNT
};

/**
 * @brief Tick resolution of user's hardware timer.
 */
enum ecu_timer_resolution
{
    ECU_TIMER_RESOLUTION_8BIT,  /**< Tick count maximum is UINT8_MAX. */
    ECU_TIMER_RESOLUTION_16BIT, /**< Tick count maximum is UINT16_MAX. */
    ECU_TIMER_RESOLUTION_32BIT, /**< Tick count maximum is UINT32_MAX. */
    ECU_TIMER_RESOLUTION_64BIT, /**< Tick count maximum is UINT64_MAX. */
    /**************************/
    ECU_TIMER_RESOLUTIONS_COUNT
};

/*------------------------------------------------------------*/
/*--------------------------- TIMER --------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Timer object represented as node in linked list.
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_timer
{
    /// @brief Linked list node in @ref ecu_tlist.
    struct ecu_dnode dnode;

    /// @brief Timer expires once elapsed ticks exceeds this value.
    ecu_timer_tick_t period;

    /// @brief Absolute timestamp of when timer first started.
    ecu_timer_tick_t starting_ticks;

    /// @brief Single shot, periodic, etc.
    enum ecu_timer_type type;

    /// @brief Mandatory callback that executes when timer expires.
    bool (*callback)(void *obj);

    /// @brief Optional object to pass to @ref ecu_timer.callback.
    void *obj;
};

/**
 * @brief Ordered linked list of timers. "Engine" that
 * runs all timers (@ref ecu_timer).
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_tlist
{
    /// @brief Ordered linked list of timers. Ordered by
    /// time remaining until expiration.
    struct ecu_dlist dlist;

    /// @brief Used to handle tick counter wraparound of
    /// user's hardware timer.
    ecu_timer_tick_t overflow_mask;

    /// @brief Interface between this object and the user's
    /// hardware timer.
    struct
    {
        /// @brief Resolution of hardware timer. 8-bit, 16bit,
        /// 32-bit, etc.
        enum ecu_timer_resolution resolution;

        /// @brief User-defined function that returns RAW number
        /// of ticks from the hardware timer.
        ecu_timer_tick_t (*get_tick_count)(void *obj);

        /// @brief Optional object to pass into @ref ecu_tlist.api.get_tick_count().
        void *obj;
    } api;
};

/*------------------------------------------------------------*/
/*------------------- TIMER MEMBER FUNCTIONS -----------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Timer constructor.
 *
 * @warning @p me must not be an active timer within a list,
 * otherwise behavior is undefined.
 *
 * @param me Timer to construct.
 * @param period Period, in timer ticks. Timer expires once
 * elapsed ticks exceeds this value.
 * @param type See @ref ecu_timer_type enum. Specifies if
 * timer is one-shot, periodic, etc.
 * @param callback Mandatory callback that executes when
 * timer expires.
 * @param obj Optional object to pass to @p callback. Pass
 * @ref ECU_TIMER_OBJ_UNUSED if unused.
 */
extern void ecu_timer_ctor(struct ecu_timer *me,
                           ecu_timer_tick_t period,
                           enum ecu_timer_type type,
                           bool (*callback)(void *obj),
                           void *obj);
/**@}*/

/**
 * @name Timer Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Stops the timer and configures it with the newly
 * supplied values. Timer is not restarted.
 *
 * @param me Timer to set.
 * @param period New period, in timer ticks, to set. Timer
 * expires once elapsed ticks exceeds this value.
 * @param type See @ref ecu_timer_type enum. Specifies if
 * timer is one-shot, periodic, etc.
 */
extern void ecu_timer_set(struct ecu_timer *me,
                          ecu_timer_tick_t period,
                          enum ecu_timer_type type);

/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Stops the timer.
 *
 * @param me Timer to stop.
 */
extern void ecu_timer_disarm(struct ecu_timer *me);

/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Returns true if timer is currently running. False otherwise.
 *
 * @param me Timer to check.
 */
extern bool ecu_timer_is_active(const struct ecu_timer *me);
/**@}*/

/*------------------------------------------------------------*/
/*-------------------- TLIST MEMBER FUNCTIONS ----------------*/
/*------------------------------------------------------------*/

/**
 * @name Tlist Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Timer list constructor.
 *
 * @param me Timer list to construct. This is the "engine"
 * that runs all timers (@ref ecu_timer).
 * @param resolution Resolution of user's hardware timer.
 * I.e. if user's timer counts up to 255 ticks this would
 * be @ref ECU_TIMER_RESOLUTION_8BIT. If user's timer counts
 * up to 65,535 this would be @ref ECU_TIMER_RESOLUTION_16BIT,
 * etc.
 * @param get_tick_count Mandatory user-defined function that
 * returns RAW number of ticks from the hardware timer.
 * @param obj Optional object to pass to @p get_tick_count.
 * Pass @ref ECU_TLIST_OBJ_UNUSED if unused.
 */
extern void ecu_tlist_ctor(struct ecu_tlist *me,
                           enum ecu_timer_resolution resolution,
                           ecu_timer_tick_t (*get_tick_count)(void *obj),
                           void *obj);
/**@}*/

/**
 * @name Tlist Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @pre @p timer previously constructed via @ref ecu_timer_ctor().
 * @brief Starts the timer if it is stopped. If the timer is
 * already running it is reset. I.e. if the timer period is 20
 * seconds and is set to expire in 10 seconds, the timer is
 * restarted and will expire 20 seconds later. If the timer is
 * in another tlist it will be removed from the old one and 
 * added to this one.
 *
 * @param me List of timers that are running.
 * @param timer Timer to add.
 */
extern void ecu_tlist_timer_arm(struct ecu_tlist *me, struct ecu_timer *timer);

/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @brief Updates the timers that are currently running. Expired
 * timer callback(s) are called. If expired timer(s) are periodic
 * they are restarted. Otherwise they are stopped. Active timers
 * are stored in an ordered list so this is O(n) where n = the
 * number of EXPIRED timers (NOT the number of elements in the list).
 *
 * @warning Must be called at least once per timer resolution.
 * I.e. if user's hardware timer is 8-bit, this must be called at
 * least once every 255 ticks. The accuracy of the timers
 * depend on how frequently this function is called. I.e. if
 * this routine is called every 1ms, the maximum resolution is 1ms.
 *
 * @param me List of timers to update.
 */
extern void ecu_tlist_service(struct ecu_tlist *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_TIMER_H_ */
