/**
 * @file
 * @brief
 * @rst
 * See :ref:`timer.h section <timer_h>` in Sphinx documentation.
 * @endrst
 * 
 * @warning Once started each timer is a shared resource belonging to both the @ref ecu_tlist
 * it was added to, and the application code that created the timer. It is the user's responsibility
 * to ensure exclusive access.
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
 * @brief To remain portable, units of time are measured arbitrarily
 * in ticks. It is the application's responsibility to convert this
 * into concrete units of time. Typedeffed in case this type needs to 
 * change in the future.
 * 
 * @warning Must be unsigned.
 */
typedef unsigned int ecu_tick_t;

/**
 * @brief Type of @ref ecu_timer used.
 */
enum ecu_timer_type_e
{
    ECU_TIMER_TYPE_ONE_SHOT, /**< Once timer expires it is stopped. */
    ECU_TIMER_TYPE_PERIODIC, /**< Once the timer expires it is automatically restarted. */
    /********************/
    ECU_TIMER_TYPES_COUNT /**< Total number of timer types. */
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

    /// @brief Absolute time, in ticks, when this timer expires.
    ecu_tick_t expiration;

    /// @brief Timer expires once elapsed ticks exceeds this value.
    ecu_tick_t period;

    /// @brief Single shot, periodic, etc.
    enum ecu_timer_type_e type;

    /// @brief Mandatory callback that executes when
    /// timer expires. First parameter passed is this timer object.
    /// Callback should return true if successful or return false
    /// if it needs to be retried in the next call to @ref ecu_tlist_service().
    /// An example return false scenario could be a write to a queue
    /// failed due to it being full, so the write needs to be reattempted.
    bool (*callback)(struct ecu_timer *me, void *obj);

    /// @brief Optional object to pass to @ref ecu_timer.callback.
    void *obj;
};

/**
 * @brief Timer linked list that runs all software timers (@ref ecu_timer)
 * added to it. Each list usually holds multiple software timers but maps
 * to a single hardware timer on the user's target device.
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
    bool overflowed;

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
 * Timer operations such as rearming and disarming can be done in this
 * callback as well.
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
 * @brief Returns true if the timer is currently running. False otherwise.
 *
 * @param me Timer to check.
 */
extern bool ecu_timer_active(const struct ecu_timer *me);

/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Stops the timer. Timer can be rearmed without reconstruction.
 * This function can be used on a timer that is already disarmed.
 *
 * @param me Timer to stop.
 */
extern void ecu_timer_disarm(struct ecu_timer *me);

/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Returns the timer's period that was set when it was started.
 * 
 * @param me Timer to check.
 */
extern ecu_tick_t ecu_timer_period(const struct ecu_timer *me);

/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Stops the timer if it was running and reconfigures 
 * it with the newly supplied settings. Timer is not restarted.
 * 
 * @warning @p period is measured in hardware timer ticks, not time.
 *
 * @param me Timer to set.
 * @param period The timer's period, in ticks, to set. Timer
 * expires after this number of ticks has elapsed. Must be
 * between 1 and @ref ECU_TICK_MAX.
 * @param type The timer's type to set. I.e one-shot, periodic, etc.
 */
extern void ecu_timer_set(struct ecu_timer *me,
                          ecu_tick_t period,
                          enum ecu_timer_type_e type);

/**
 * @pre @p me previously constructed via @ref ecu_timer_ctor().
 * @brief Returns the timer's type that was specified when it was started.
 * 
 * @param me Timer to check.
 */
extern enum ecu_timer_type_e ecu_timer_type(const struct ecu_timer *me);
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
 * @warning @p me must not be an active list with timers added
 * to it, otherwise behavior is undefined.
 *
 * @param me Timer list to construct.
 */
extern void ecu_tlist_ctor(struct ecu_tlist *me);
/**@}*/

/**
 * @name Tlist Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @brief Services all software timers (@ref ecu_timer) currently in the
 * list. Servicing involves expiring appropriate timers, handling timer
 * rearming, etc. This must be periodically called by the application at 
 * least once every @ref ECU_TICK_MAX ticks. However the accuracy of the 
 * timers is proportional to how often this function is called.
 * 
 * @param me List to service.
 * @param elapsed Number of ticks that has elapsed since the last time
 * this function was called. The list keeps track of time solely
 * based off of this parameter.
 */
extern void ecu_tlist_service(struct ecu_tlist *me, ecu_tick_t elapsed);

/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @pre @p timer previously constructed via @ref ecu_timer_ctor().
 * @brief Starts a timer with the specified settings. If the timer 
 * is already running it is restarted and reconfigured with the 
 * newly specified settings.
 *
 * @param me List to add timer to. 
 * @param timer Timer to start. It will be serviced in calls
 * to @ref ecu_tlist_service()
 * @param period The timer's period, in ticks, to set. Timer
 * expires after this number of ticks has elapsed. Must be
 * between 1 and @ref ECU_TICK_MAX.
 * @param type The timer's type to set. I.e one-shot, periodic, etc.
 */
extern void ecu_tlist_timer_arm(struct ecu_tlist *me, 
                                struct ecu_timer *timer, 
                                ecu_tick_t period, 
                                enum ecu_timer_type_e type);

/**
 * @pre @p me previously constructed via @ref ecu_tlist_ctor().
 * @pre @p timer previously set via @ref ecu_timer_set() or @ref ecu_tlist_timer_arm().
 * @brief Restarts the timer with its same settings.
 * 
 * @param me List to add timer to. 
 * @param timer Timer to restart. It will be serviced in calls
 * to @ref ecu_tlist_service()
 */
extern void ecu_tlist_timer_rearm(struct ecu_tlist *me, struct ecu_timer *timer);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_TIMER_H_ */
