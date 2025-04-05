/**
 * @file
 * @brief
 * @rst
 * See :ref:`dlist.h section <dlist_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-04-05
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
    ECU_TIMER_ONE_SHOT, /**< Once timer expires it is stopped. */
    ECU_TIMER_PERIODIC, /**< Once the timer expires it is automatically restarted. */
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

        /// @brief Optional object to pass into get_tick_count() callback.
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
 * restarted and will expire 20 seconds later.
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
