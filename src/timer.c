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

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("ecu/timer.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if @p node should be inserted before
 * @p position. Otherwise returns false. This insertion
 * condition ensures all timer linked lists in @ref ecu_tlist
 * are ordered by the value of @ref ecu_timer.expiration.
 * 
 * @param node Node that is being inserted.
 * @param position List position to check against.
 * @param obj Unused.
 */
static bool insert_here(const struct ecu_dnode *node,
                        const struct ecu_dnode *position,
                        void *obj);

/**
 * @brief Helper function that calls timer's callback. Handles
 * timer rearming logic based on type (one-shot, periodic, etc)
 * and what the user does in their callback.
 * 
 * @param t Timer to expire.
 * @param tlist "Engine" the expired timer was apart of. Required
 * for rearming logic.
 */
static void expire_timer(struct ecu_timer *t, struct ecu_tlist *tlist);

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (ECU_IS_UNSIGNED(ecu_tick_t)), "ecu_tick_t must be an unsigned type." );

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool insert_here(const struct ecu_dnode *node,
                        const struct ecu_dnode *position,
                        void *obj)
{
    ECU_ASSERT( (node && position) );
    (void)obj;
    bool status = false;
    const struct ecu_timer *me = ECU_DNODE_GET_CONST_ENTRY(node, struct ecu_timer, dnode);
    const struct ecu_timer *tposition = ECU_DNODE_GET_CONST_ENTRY(position, struct ecu_timer, dnode);

    /* Use <= instead of < so ordered insertion ends ASAP. No functional difference between the two. */
    if (me->expiration <= tposition->expiration)
    {
        status = true;
    }

    return status;
}

static void expire_timer(struct ecu_timer *timer, struct ecu_tlist *tlist)
{
    ECU_ASSERT( (timer && tlist) );
    ECU_ASSERT( (timer->callback) );

    if (timer->type == ECU_TIMER_TYPE_ONE_SHOT)
    {
        /* Disarming here BEFORE callback allows one-shot timer to be rearmed in user callback. Do 
        NOT disarm periodic timers so we can check if user disarmed periodic timer in their callback. */
        ecu_timer_disarm(timer);
    }

    if ((*timer->callback)(timer, timer->obj)) /* Execute callback. */
    {
        /* Callback successful. Only rearm if timer is periodic and user did NOT disarm it in their callback. */
        if (timer->type == ECU_TIMER_TYPE_PERIODIC && 
            ecu_timer_active(timer))
        {
            ecu_tlist_timer_rearm(tlist, timer);
        }
    }
    else
    {
        /* Callback failed. Retry callback by always expiring the timer on the next service. */
        ecu_timer_disarm(timer);
        timer->expiration = 0;

        if (tlist->overflowed)
        {
            ecu_dlist_push_front(&tlist->wraparounds, &timer->dnode);
        }
        else
        {
            ecu_dlist_push_front(&tlist->timers, &timer->dnode);
        }
    }
}

/*------------------------------------------------------------*/
/*------------------- TIMER MEMBER FUNCTIONS -----------------*/
/*------------------------------------------------------------*/

void ecu_timer_ctor(struct ecu_timer *me,
                    bool (*callback)(struct ecu_timer *me, void *obj),
                    void *obj)
{
    ECU_ASSERT( (me && callback) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    me->expiration = 0;
    me->period = 0;
    me->type = ECU_TIMER_TYPES_COUNT;
    me->callback = callback;
    me->obj = obj;
}

bool ecu_timer_active(const struct ecu_timer *me)
{
    ECU_ASSERT( (me) );
    return (ecu_dnode_in_list(&me->dnode));
}

void ecu_timer_disarm(struct ecu_timer *me)
{
    ECU_ASSERT( (me) );
    ecu_dnode_remove(&me->dnode);
}

void ecu_timer_set(struct ecu_timer *me,
                   ecu_tick_t period,
                   enum ecu_timer_type type)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (period > 0) );
    ECU_ASSERT( (type >= 0 && type < ECU_TIMER_TYPES_COUNT) );

    ecu_timer_disarm(me);
    me->period = period;
    me->type = type;
}

/*------------------------------------------------------------*/
/*-------------------- TLIST MEMBER FUNCTIONS ----------------*/
/*------------------------------------------------------------*/

void ecu_tlist_ctor(struct ecu_tlist *me)
{
    ECU_ASSERT( (me) );

    me->current = 0;
    me->overflowed = false;
    ecu_dlist_ctor(&me->timers);
    ecu_dlist_ctor(&me->wraparounds);
}

void ecu_tlist_service(struct ecu_tlist *me, ecu_tick_t elapsed)
{
    ECU_ASSERT( (me) );
    ecu_tick_t prev = 0;
    struct ecu_dlist_iterator iterator;
    struct ecu_timer *t = (struct ecu_timer *)0;
    struct ecu_dnode *tstart = (struct ecu_dnode *)0;
    
    /* Always update timestamp even if lists empty since time is measured in absolute ticks. */
    prev = me->current;
    me->current += elapsed;

    if (!ecu_dlist_empty(&me->timers) || !ecu_dlist_empty(&me->wraparounds))
    {
        if (me->current < prev)
        {
            /* Edge case that only runs when me->current tick counter has wrapped around.
            All timers in me->timers list will have expired if counter overflows. Expire
            all timers in this list. All timers that are rearmed during this operation 
            (will primarily happen in user callback) are inserted into the me->wraparounds 
            list. Once complete, the me->timers list should be empty. me->timers and 
            me->wraparounds are swapped to "reset" the engine. Finally, any timers that were 
            previously added to the me->wraparounds list before this service have to be 
            checked for expiration. */
            me->overflowed = true; /* Timers rearmed are always added to me->wraparounds. */
            tstart = ecu_dlist_front(&me->wraparounds);
            
            ECU_DLIST_FOR_EACH(node, &iterator, &me->timers)
            {
                /* Everything in me->timers list has expired if me->current counter overflowed. */
                t = ECU_DNODE_GET_ENTRY(node, struct ecu_timer, dnode);
                ECU_ASSERT( (t->callback) );
                expire_timer(t, me);
            }

            /* Reset the "engine". */
            ECU_ASSERT( (ecu_dlist_empty(&me->timers)) ); /* All timers rearmed during this operation should be in me->wraparounds list. */
            ecu_dlist_swap(&me->timers, &me->wraparounds);
            me->overflowed = false; /* Done. Timer rearm logic follows normal conditions. */

            if (tstart) /* Any more timers to check? */
            {
                /* Check if any timers previously in the me->wraparounds list before this service have 
                expired. List is ordered so function can exit as soon as a non-expired timer is reached. */
                ECU_DLIST_AT_FOR_EACH(node, &iterator, &me->timers, tstart)
                {
                    t = ECU_DNODE_GET_ENTRY(node, struct ecu_timer, dnode);

                    if (me->current >= t->expiration)
                    {
                        ECU_ASSERT( (t->callback) );
                        expire_timer(t, me);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            /* This branch runs most of the time. Timeout any expired timers. List is ordered
            so function can exit as soon as a non-expired timer is reached. */
            ECU_DLIST_FOR_EACH(node, &iterator, &me->timers)
            {
                t = ECU_DNODE_GET_ENTRY(node, struct ecu_timer, dnode);

                if (me->current >= t->expiration)
                {
                    ECU_ASSERT( (t->callback) );
                    expire_timer(t, me);
                }
                else
                {
                    break;
                }
            }
        }
    }
}

void ecu_tlist_timer_arm(struct ecu_tlist *me, 
                         struct ecu_timer *timer, 
                         ecu_tick_t period, 
                         enum ecu_timer_type type)
{
    ECU_ASSERT( (me && timer) );
    ECU_ASSERT( (timer->callback) );

    ecu_timer_set(timer, period, type);
    timer->expiration = me->current + period; /* Unsigned overflow OK since we store absolute ticks. */

    if ((timer->expiration < me->current) || (me->overflowed))
    {
        /* Timer expires after me->current wraparound. Or this timer is being rearmed
        in the middle of an ecu_tlist_service() call (in user's callback), where the 
        me->current counter has already wrapped around. */
        ecu_dlist_insert_before(&me->wraparounds, &timer->dnode, &insert_here, ECU_DNODE_OBJ_UNUSED);
    }
    else
    {
        ecu_dlist_insert_before(&me->timers, &timer->dnode, &insert_here, ECU_DNODE_OBJ_UNUSED);
    }
}

void ecu_tlist_timer_rearm(struct ecu_tlist *me, struct ecu_timer *timer)
{
    ECU_ASSERT( (me && timer) );
    ECU_ASSERT( (timer->period > 0) );
    ECU_ASSERT( (timer->type >= 0 && timer->type < ECU_TIMER_TYPES_COUNT) );
    ECU_ASSERT( (timer->callback) );

    ecu_timer_disarm(timer);
    timer->expiration = me->current + timer->period; /* Unsigned overflow OK since we store absolute ticks. */

    if ((timer->expiration < me->current) || (me->overflowed))
    {
        /* Timer expires after me->current wraparound. Or this timer is being rearmed
        in the middle of an ecu_tlist_service() call (in user's callback), where the 
        me->current counter has already wrapped around. */
        ecu_dlist_insert_before(&me->wraparounds, &timer->dnode, &insert_here, ECU_DNODE_OBJ_UNUSED);
    }
    else
    {
        ecu_dlist_insert_before(&me->timers, &timer->dnode, &insert_here, ECU_DNODE_OBJ_UNUSED);
    }
}
