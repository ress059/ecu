/**
 * @file
 * @brief
 * @rst
 * See :ref:`timer.h section <timer_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2025-04-02
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/timer.h"

/* Runtime asserts. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/timer.c")

/*------------------------------------------------------------*/
/*---------------------- FILE SCOPE TYPES --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Timestamp and list information used to sort ordered list.
 */
struct tlist_info
{
    ecu_timer_tick_t current_ticks;
    ecu_timer_tick_t overflow_mask;
};

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if timer type, period, and callback are
 * set.
 *
 * @param timer Timer to check.
 */
static bool timer_is_set(const struct ecu_timer *timer);

/**
 * @brief Returns true if timer list has been constructed via
 * @ref ecu_tlist_ctor(). False otherwise.
 *
 * @param list List to check.
 */
static bool tlist_is_constructed(const struct ecu_tlist *list);

/**
 * @brief Helper function that returns the raw number of clock ticks
 * from the user's hardware timer by calling @ref ecu_tlist.api.get_tick_count().
 * This returned value cannot exceed the maximum resolution of the
 * user's timer. This condition is asserted, which is why this operation
 * is offloaded to this function.
 *
 * @param list Hardware timer to check.
 */
static ecu_timer_tick_t get_ticks(struct ecu_tlist *list);

/**
 * @brief Returns true if @p node should be inserted before @p position
 * node in @ref ecu_tlist. False otherwise. @ref ecu_tlist is
 * an ordered list. It is ordered by time remaining until expiration.
 * For example if the current list is [1, 3, 20] where these values
 * are ticks until timeout and a timer of period 4 is being added,
 * the new list becomes [1, 3, 4, 20].
 *
 * @param node Timer being added.
 * @param position Current node position in @ref ecu_tlist.
 * @param obj Timestamp and list info (@ref tlist_info) required
 * to calculate ticks until timeout.
 */
static bool tlist_insert_condition(const struct ecu_dnode *node,
                                   const struct ecu_dnode *position,
                                   void *obj);

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (((ecu_timer_tick_t)(-1)) > ((ecu_timer_tick_t)0)), 
                    "ecu_timer_tick_t must be an unsigned type so tick wraparound can be handled." );

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool timer_is_set(const struct ecu_timer *timer)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (timer) );

    /* A period if 0 is technically valid. */
    if (timer->type >= 0 &&
        timer->type < ECU_TIMER_TYPES_COUNT &&
        timer->callback)
    {
        status = true;
    }

    return status;
}

static bool tlist_is_constructed(const struct ecu_tlist *list)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (list) );

    if (ecu_dnode_in_list(&list->dlist.head) &&
        list->api.resolution >= 0 &&
        list->api.resolution < ECU_TIMER_RESOLUTIONS_COUNT &&
        list->api.get_tick_count)
    {
        status = true;
    }

    return status;
}

static ecu_timer_tick_t get_ticks(struct ecu_tlist *list)
{
    ecu_timer_tick_t current_ticks = 0;
    ECU_RUNTIME_ASSERT( (list) );
    ECU_RUNTIME_ASSERT( (tlist_is_constructed(list)) );

    /* Get current tick count. It should not be possible for returned
    value to be greater than the maximum resolution of the timer. */
    current_ticks = (*list->api.get_tick_count)(list->api.obj);
    ECU_RUNTIME_ASSERT( (current_ticks <= list->overflow_mask) );
    return current_ticks;
}

static bool tlist_insert_condition(const struct ecu_dnode *node,
                                   const struct ecu_dnode *position,
                                   void *obj)
{
    bool status = false;
    ecu_timer_tick_t remaining_ticks = 0;
    ECU_RUNTIME_ASSERT( (node && position && obj) );

    const struct ecu_timer *timer_to_add = ECU_DNODE_GET_CONST_ENTRY(node, struct ecu_timer, dnode);
    const struct ecu_timer *tlist_element = ECU_DNODE_GET_CONST_ENTRY(position, struct ecu_timer, dnode);
    struct tlist_info *info = (struct tlist_info *)obj;

    /* Unsigned overflow automatically handles wraparound.
    ANDing with overflow_mask sets 0xFF overflowed bytes that
    are greater than the timer's resolution back to 0.

    Example: sizeof(ecu_tick_type_t) == sizeof(uint32_t),
    hardware timer = ECU_TIMER_RESOLUTION_8BIT,
    current_ticks = 1, starting_ticks = 255.

    overflow_mask = 0x000000FF
    elapsed_ticks = (current_ticks - starting_ticks) & overflow_mask
    elapsed_ticks = (1 - 255) & 0x000000FF
    elapsed_ticks = 0xFFFFFF02 & 0x000000FF = 2 */
    ecu_timer_tick_t elapsed_ticks = (info->current_ticks - tlist_element->starting_ticks) & info->overflow_mask;

    /* Edge case is elapsed_ticks >= tlist_element->period. Means
    tlist_element timer already expired so obviously return false. */
    if (elapsed_ticks < tlist_element->period)
    {
        remaining_ticks = tlist_element->period - elapsed_ticks;

        if (timer_to_add->period <= remaining_ticks)
        {
            status = true;
        }
    }

    return status;
}

/*------------------------------------------------------------*/
/*------------------- TIMER MEMBER FUNCTIONS -----------------*/
/*------------------------------------------------------------*/

void ecu_timer_ctor(struct ecu_timer *me,
                    ecu_timer_tick_t period,
                    enum ecu_timer_type type,
                    bool (*callback)(void *obj),
                    void *obj)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (type >= 0 && type < ECU_TIMER_TYPES_COUNT) );
    ECU_RUNTIME_ASSERT( (callback) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    me->period = period;
    me->type = type;
    me->callback = callback;
    me->obj = obj;
}

void ecu_timer_set(struct ecu_timer *me,
                   ecu_timer_tick_t period,
                   enum ecu_timer_type type)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (type >= 0 && type < ECU_TIMER_TYPES_COUNT) );

    ecu_timer_disarm(me);
    me->period = period;
    me->type = type;
}

void ecu_timer_disarm(struct ecu_timer *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ecu_dnode_remove(&me->dnode);
}

bool ecu_timer_is_active(const struct ecu_timer *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dnode_in_list(&me->dnode);
}

#pragma message("TODO: Want a timer reset capability without coupling it to tlist. \
    Reset = stop timer but do not restart its timer. When its readded it counts down \
    from its old saved value.")

/*------------------------------------------------------------*/
/*-------------------- TLIST MEMBER FUNCTIONS ----------------*/
/*------------------------------------------------------------*/

void ecu_tlist_ctor(struct ecu_tlist *me,
                    enum ecu_timer_resolution resolution,
                    ecu_timer_tick_t (*get_tick_count)(void *obj),
                    void *obj)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (resolution >= 0 && resolution < ECU_TIMER_RESOLUTIONS_COUNT) );
    ECU_RUNTIME_ASSERT( (get_tick_count) );

    ecu_dlist_ctor(&me->dlist);

    switch (resolution)
    {
        case ECU_TIMER_RESOLUTION_8BIT:
        {
            me->overflow_mask = UINT8_MAX;
            break;
        }

        case ECU_TIMER_RESOLUTION_16BIT:
        {
            /* ecu_timer_tick_t is currently size_t which is never less than
            sizeof(uint16_t). However this condition is still asserted for
            future compatibility in case the type of ecu_timer_tick_t changes. */
            ECU_RUNTIME_ASSERT( (sizeof(ecu_timer_tick_t) >= sizeof(uint16_t)) );
            me->overflow_mask = UINT16_MAX;
            break;
        }

        case ECU_TIMER_RESOLUTION_32BIT:
        {
            ECU_RUNTIME_ASSERT( (sizeof(ecu_timer_tick_t) >= sizeof(uint32_t)) );
            me->overflow_mask = UINT32_MAX;
            break;
        }

        case ECU_TIMER_RESOLUTION_64BIT:
        {
            ECU_RUNTIME_ASSERT( (sizeof(ecu_timer_tick_t) >= sizeof(uint64_t)) );
            me->overflow_mask = UINT64_MAX;
            break;
        }

        default:
        {
            ECU_RUNTIME_ASSERT( (false) );
            break;
        }
    }

    me->api.resolution = resolution;
    me->api.get_tick_count = get_tick_count;
    me->api.obj = obj;
}

void ecu_tlist_timer_arm(struct ecu_tlist *me, struct ecu_timer *timer)
{
    struct tlist_info info;
    ECU_RUNTIME_ASSERT( (me && timer) );
    ECU_RUNTIME_ASSERT( (tlist_is_constructed(me)) );
    ECU_RUNTIME_ASSERT( (timer_is_set(timer)) );

    ecu_timer_disarm(timer); /* If user's timer was previously running it is rearmed. */
    timer->starting_ticks = get_ticks(me);
    info.current_ticks = timer->starting_ticks;
    info.overflow_mask = me->overflow_mask;

    /* Ordered list insertion. Ordered by time remaining until expiration. O(n). */
    ecu_dlist_insert_before(&me->dlist, &timer->dnode, &tlist_insert_condition, (void *)&info);
}

void ecu_tlist_service(struct ecu_tlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (tlist_is_constructed(me)) );
    static const bool CALLBACK_SUCCESSFUL = true;
    struct ecu_dlist_iterator iterator;
    ecu_timer_tick_t current_ticks = 0;
    ecu_timer_tick_t elapsed_ticks = 0;
    ecu_timer_tick_t prev_period = 0;
    ecu_timer_tick_t prev_starting_ticks = 0;

    if (!ecu_dlist_is_empty(&me->dlist))
    {
        current_ticks = get_ticks(me);

        ECU_DLIST_FOR_EACH(node, &iterator, &me->dlist)
        {
            /* Unsigned overflow automatically handles wraparound. ANDing with overflow_mask
            sets 0xFF overflowed bytes that are greater than the timer's resolution back to 0.

            Example: sizeof(ecu_tick_type_t) == sizeof(uint32_t),
            hardware timer = ECU_TIMER_RESOLUTION_8BIT,
            current_ticks = 1, starting_ticks = 255.

            overflow_mask = 0x000000FF
            elapsed_ticks = (current_ticks - starting_ticks) & overflow_mask
            elapsed_ticks = (1 - 255) & 0x000000FF
            elapsed_ticks = 0xFFFFFF02 & 0x000000FF = 2 */
            struct ecu_timer *t = ECU_DNODE_GET_ENTRY(node, struct ecu_timer, dnode);
            prev_period = t->period;                 /* Save to re-expire timer in case callback returns false. */
            prev_starting_ticks = t->starting_ticks; /* Save to re-expire timer in case callback returns false. */
            elapsed_ticks = (current_ticks - t->starting_ticks) & me->overflow_mask;

            if (elapsed_ticks >= t->period)
            {
                /* Timer expired. */
                ECU_RUNTIME_ASSERT( (t->callback) );
                ecu_timer_disarm(t); /* Disarm here to allow one-shot timer to be readded in user callback. */

                if ((*t->callback)(t->obj) == CALLBACK_SUCCESSFUL)
                {
                    if (t->type == ECU_TIMER_PERIODIC)
                    {
                        ecu_timer_disarm(t); /* Disarm here in case user armed periodic timer in their callback for some reason. */
                        ecu_tlist_timer_arm(me, t);
                    }
                }
                else
                {
                    /* User callback failed so we need to reattempt it on next service by re-expiring
                    the timer. Add timer to front since it has already expired, and will expire on next
                    service. Ensure timer's period and starting ticks are their original values in case
                    user set them in their callback for some reason. This guarantees the timer will
                    re-expire on the next service. */
                    ecu_timer_disarm(t); /* Disarm in case user armed timer in their callback for some reason. */
                    t->period = prev_period;
                    t->starting_ticks = prev_starting_ticks;
                    ecu_dlist_push_front(&me->dlist, &t->dnode);
                }
            }
            else
            {
                /* This list is ordered by time remaining until expiration. If timer we are checking
                has not timed out, then break since the remaining ones will also not be timed out. */
                break;
            }
        }
    }
}