/**
 * @file
 * @brief
 * @rst
 * See :ref:`timer.h section <timer_h>` in Sphinx documentation.
 * @endrst
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
#include <limits.h>
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
 * @brief Maximum value an ecu_tick_t type can hold.
 */
#define ECU_TICK_MAX \
    (UINT_MAX)

/*------------------------------------------------------------*/
/*------------------------- TIMER TYPES ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Represents timer ticks. Must be unsigned. Typedeffed 
 * in case this needs to change in the future.
 */
typedef unsigned int ecu_tick_t;

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
    /// @brief Absolute time, in ticks, when this timer expires.
    ecu_tick_t expiration;

    /// @brief Timer expires once elapsed ticks exceeds this value.
    ecu_tick_t period;

    /// @brief Single shot, periodic, etc.
    enum ecu_timer_type type;

    /// @brief Mandatory callback that executes when
    /// timer expires. First parameter passed is this timer object.
    /// Callback should return true if successful or return false
    /// if it needs to be retried in the next call to @ref ecu_tlist_service().
    /// An example return false scenario could be a write to a queue
    /// failed due to it being full, so the write needs to be reattempted.
    bool (*callback)(struct ecu_timer *me, void *obj);

    /// @brief Optional object to pass to @ref ecu_timer.callback.
    void *obj;

    /// @brief Linked list node in @ref ecu_tlist.
    struct ecu_dnode dnode;
};

/**
 * @brief "Engine" that runs all timers (@ref ecu_timer) added 
 * to this object. This is a software object that represents a 
 * single hardware timer on the target. Each hardware timer in 
 * use should map to a unique @ref ecu_tlist object. 
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_tlist
{
    /// @brief Absolute timestamp, in ticks. Incremented with each call
    /// to @ref ecu_tlist_service(). Overflow is automatically handled.
    ecu_tick_t current;

    /// @brief True if @ref ecu_tlist.current overflowed. Flag set back
    /// to false once overflow handled. Otherwise always false.
    volatile bool overflowed;

    /// @brief Ordered linked list of timers that are running. Ordered 
    /// by @ref ecu_timer.expiration.
    struct ecu_dlist timers;

    /// @brief Ordered linked list of timers that are running, but expire 
    /// after an @ref ecu_tlist.current overflow. Having a separate list 
    /// allows @ref ecu_timer.expiration to still be used as the ordering 
    /// condition.
    struct ecu_dlist wraparounds;
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
 * @param callback Mandatory callback that executes when
 * timer expires. First parameter passed is this timer object.
 * Callback should return true if successful or return false
 * if it needs to be retried in the next call to @ref ecu_tlist_service().
 * An example return false scenario could be a write to a queue
 * failed due to it being full, so the write needs to be reattempted.
 * @param obj Optional object to pass to @p callback. Pass
 * @ref ECU_TIMER_OBJ_UNUSED if unused.
 */
extern void ecu_timer_ctor(struct ecu_timer *me,
                           bool (*callback)(struct ecu_timer *me, void *obj),
                           void *obj);
/**@}*/

/**
 * @name Timer Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Stops the timer if it was running and reconfigures 
 * it with the newly supplied values. Timer is not restarted.
 * 
 * @warning @p period is measured in hardware timer ticks, not time.
 *
 * @param me Timer to set.
 * @param period New period, in timer ticks, to set. Timer
 * expires once elapsed ticks exceeds this value. Must be
 * between 1 and @ref ECU_TICK_MAX.
 * @param type See @ref ecu_timer_type enum. Specifies if
 * timer is one-shot, periodic, etc.
 */
extern void ecu_timer_set(struct ecu_timer *me,
                          ecu_tick_t period,
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
 * @warning @p me must not be an active engine with timers added
 * to it, otherwise behavior is undefined.
 *
 * @param me Timer "engine" to construct. This is the "engine"
 * that runs all timers (@ref ecu_timer) added to it. This is 
 * a software object that represents a single hardware timer 
 * on the target. Each hardware timer in use should map to a 
 * unique @ref ecu_tlist object. 
 */
extern void ecu_tlist_ctor(struct ecu_tlist *me);
/**@}*/

/**
 * @name Tlist Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @pre @p timer previously constructed via @ref ecu_timer_ctor().
 * @brief Starts a timer. If the timer is already running it is restarted
 * with the specified values. This operation is O(N), where N = the total
 * number of timers currently in @p me.
 * 
 * @warning @p period is measured in hardware timer ticks, not time.
 *
 * @param me "Engine" to add @p timer to. 
 * @param timer Timer to start. It will be serviced in calls
 * to @ref ecu_tlist_service(@p me)
 * @param period New period, in timer ticks, to set. Timer
 * expires once elapsed ticks exceeds this value. Must be
 * between 1 and @ref ECU_TICK_MAX.
 * @param type See @ref ecu_timer_type enum. Specifies if
 * timer is one-shot, periodic, etc.
 */
extern void ecu_tlist_timer_arm(struct ecu_tlist *me, 
                                struct ecu_timer *timer, 
                                ecu_tick_t period, 
                                enum ecu_timer_type type);

/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @pre @p timer previously set via @ref ecu_timer_set() or @ref ecu_tlist_timer_arm().
 * @brief Restarts the timer. This operation is O(N), 
 * where N = the total number of timers currently in @p me.
 * 
 * @param me "Engine" to add @p timer to.
 * @param timer Timer to restart. It will be serviced in calls
 * to @ref ecu_tlist_service(@p me)
 */
extern void ecu_tlist_timer_rearm(struct ecu_tlist *me, struct ecu_timer *timer);

/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @brief Services all timers added to the "engine", @p me.
 * If any timers expire, its corresponding @ref ecu_timer.callback
 * is called. This operation is O(N) where N = the number of expired 
 * timers, NOT the total number of timers in @p me.
 * 
 * @warning It is the application's responsibility to ensure exclusive
 * access to all @ref ecu_tlist and @ref ecu_timer objects if this
 * function is called within an ISR.
 * @warning This must be called periodically, at least once every
 * ECU_TICK_MAX ticks. The accuracy of the timers is proportional to
 * how often this function is called.
 * @warning @p elapsed is measured in hardware timer ticks, not time.
 * 
 * @param me "Engine" to service.
 * @param elapsed Number of ticks that elapsed since the last time
 * this function was called. This module keeps track of time solely
 * based off of this parameter.
 */
extern void ecu_tlist_service(struct ecu_tlist *me, ecu_tick_t elapsed);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_TIMER_H_ */
