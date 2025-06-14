/**
 * @file
 * @brief
 * @rst
 * See :ref:`dlist.h section <dlist_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-14
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/dlist.h"

/* STDLib. */
#include <stdbool.h>

/* Runtime asserts. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/dlist.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/
/* NOTE: All functions require node and list to be previously constructed
via calls to ecu_dnode_ctor() and ecu_dlist_ctor(). Otherwise
next and prev pointers can be garbage non-NULL values, leading to seg
faults when accessed. */

/**
 * @brief Returns true if node has been properly constructed
 * via @ref ecu_dnode_ctor(), or @ref ecu_dlist_ctor() if
 * supplied node is HEAD. False otherwise.
 */
static bool node_valid(const struct ecu_dnode *node);

/**
 * @brief Returns true if node is HEAD and has been properly
 * constructed via @ref ecu_dlist_ctor(). False otherwise.
 */
static bool node_is_valid_head(const struct ecu_dnode *node);

/**
 * @brief Returns true if node is within any list. This includes
 * the HEAD node. False otherwise.
 */
static bool node_in_list(const struct ecu_dnode *node);

/**
 * @brief Returns true if list has been properly constructed
 * via @ref ecu_dlist_ctor(). False otherwise.
 */
static bool list_valid(const struct ecu_dlist *list);

/**
 * @brief Reserved destroy callback assigned to list's HEAD. HEAD is
 * solely used as a dummy delimiter that should never be destroyed.
 * Therefore this callback just asserts. This callback is also used
 * to help identify the HEAD node in a list.
 */
static void HEAD_DESTROY_CALLBACK(struct ecu_dnode *me, ecu_object_id id);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool node_valid(const struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );
    bool valid = false;

    if ((node->next->prev == node) &&
        (node->prev->next == node) &&
        (node->destroy != &HEAD_DESTROY_CALLBACK) &&
        (node->id >= ECU_VALID_OBJECT_ID_BEGIN))
    {
        valid = true;
    }
    else if (node_is_valid_head(node))
    {
        valid = true;
    }

    return valid;
}

static bool node_is_valid_head(const struct ecu_dnode *node)
{
    /* Do NOT assert node_valid() since node_valid() calls this
    function. Otherwise stack overflow will occur. */
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );
    bool is_head = false;

    if ((node->next->prev == node) &&
        (node->prev->next == node) &&
        (node->destroy == &HEAD_DESTROY_CALLBACK) &&
        (node->id == ECU_OBJECT_ID_RESERVED))
    {
        is_head = true;
    }

    return is_head;
}

static bool node_in_list(const struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    bool in_list = false;

    if ((node->next != node) &&
        (node->prev != node) &&
        (node->next->prev == node) &&
        (node->prev->next == node))
    {
        in_list = true;
    }
    else if (node_is_valid_head(node))
    {
        in_list = true;
    }

    return in_list;
}

static bool list_valid(const struct ecu_dlist *list)
{
    ECU_RUNTIME_ASSERT( (list) );
    return (node_is_valid_head(&list->head));
}

static void HEAD_DESTROY_CALLBACK(struct ecu_dnode *me, ecu_object_id id)
{
    (void)me;
    (void)id;
    ECU_RUNTIME_ASSERT( (false) );
}

/*------------------------------------------------------------*/
/*------------------ DNODE MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void ecu_dnode_ctor(struct ecu_dnode *me,
                    void (*destroy)(struct ecu_dnode *me, ecu_object_id id),
                    ecu_object_id id)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (destroy != &HEAD_DESTROY_CALLBACK) );
    ECU_RUNTIME_ASSERT( (id >= ECU_VALID_OBJECT_ID_BEGIN) );
    /* Do not assert node_valid() or !node_in_list() since next and prev pointers
    can be initialized to garbage non-NULL values before a constructor call. It is
    the user's responsibility to not construct an active node, which is clearly outlined
    in the warning directive of this function description. */

    me->next = me;
    me->prev = me;
    me->destroy = destroy; /* Optional callback so do not NULL assert. */
    me->id = id;           /* Optional. */
}

void ecu_dnode_destroy(struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me) && !node_is_valid_head(me)) );

    /* Save entries since they are reset before destroy callback executes. */
    void (*destroy)(struct ecu_dnode *, ecu_object_id) = me->destroy;
    ecu_object_id id = me->id;

    /* Remove node from list. OK if node is not in list. */
    me->next->prev = me->prev;
    me->prev->next = me->next;

    /* Destroy object by setting to NULL values. Forces user to reconstruct
    node if they want to use it again, assuming asserts are enabled.
    IMPORTANT: These values are reset before the destroy callback in case
    the user frees their entire node (including ecu_dnode). Otherwise 
    we would be accessing and writing to freed memory. */
    me->next = (struct ecu_dnode *)0;
    me->prev = (struct ecu_dnode *)0;
    me->destroy = ECU_DNODE_DESTROY_UNUSED;
    me->id = ECU_OBJECT_ID_RESERVED;

    if (destroy != ECU_DNODE_DESTROY_UNUSED)
    {
        (*destroy)(me, id);
    }
}

void ecu_dnode_insert_before(struct ecu_dnode *pos, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (pos && node) );
    ECU_RUNTIME_ASSERT( (pos != node) );
    ECU_RUNTIME_ASSERT( (node_valid(pos) && node_valid(node)) );
    ECU_RUNTIME_ASSERT( (node_in_list(pos) && !node_in_list(node)) );

    node->next = pos;
    node->prev = pos->prev;
    pos->prev->next = node;
    pos->prev = node;
}

void ecu_dnode_insert_after(struct ecu_dnode *pos, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (pos && node) );
    ECU_RUNTIME_ASSERT( (pos != node) );
    ECU_RUNTIME_ASSERT( (node_valid(pos) && node_valid(node)) );
    ECU_RUNTIME_ASSERT( (node_in_list(pos) && !node_in_list(node)) );
    ecu_dnode_insert_before(pos->next, node);
}

void ecu_dnode_remove(struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    ECU_RUNTIME_ASSERT( (!node_is_valid_head(me)) );

    if (ecu_dnode_in_list(me))
    {
        me->next->prev = me->prev;
        me->prev->next = me->next;
        me->next = me;
        me->prev = me;
        /* Do not reset destroy callback or ID. This is only a remove function,
        not a destroy function. */
    }
}

struct ecu_dnode *ecu_dnode_next(struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_dnode *next = (struct ecu_dnode *)0;

    if (ecu_dnode_in_list(me))
    {
        if (!node_is_valid_head(me->next))
        {
            next = me->next;
            ECU_RUNTIME_ASSERT( (node_valid(next)) );
        }
    }

    return next;
}

const struct ecu_dnode *ecu_dnode_cnext(const struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    const struct ecu_dnode *next = (const struct ecu_dnode *)0;

    if (ecu_dnode_in_list(me))
    {
        if (!node_is_valid_head(me->next))
        {
            next = me->next;
            ECU_RUNTIME_ASSERT( (node_valid(next)) );
        }
    }

    return next;
}

struct ecu_dnode *ecu_dnode_prev(struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_dnode *prev = (struct ecu_dnode *)0;

    if (ecu_dnode_in_list(me))
    {
        if (!node_is_valid_head(me->prev))
        {
            prev = me->prev;
            ECU_RUNTIME_ASSERT( (node_valid(prev)) );
        }
    }

    return prev;
}

const struct ecu_dnode *ecu_dnode_cprev(const struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    const struct ecu_dnode *prev = (const struct ecu_dnode *)0;

    if (ecu_dnode_in_list(me))
    {
        if (!node_is_valid_head(me->prev))
        {
            prev = me->prev;
            ECU_RUNTIME_ASSERT( (node_valid(prev)) );
        }
    }

    return prev;
}

bool ecu_dnode_in_list(const struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    return (node_in_list(me));
}

ecu_object_id ecu_dnode_get_id(const struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me) && !node_is_valid_head(me)) );
    return (me->id);
}

/*------------------------------------------------------------*/
/*------------------ DLIST MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void ecu_dlist_ctor(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Do not assert list_valid() since HEAD's next and prev pointers can be
    initialized to garbage non-NULL values before a constructor call. It is
    the user's responsibility to not construct an active list, which is clearly
    outlined in the warning directive of this function description. Also do not
    use ecu_dnode_ctor(HEAD) since a reserved destroy callback and ID are
    assigned. */

    me->head.next = &me->head;
    me->head.prev = &me->head;
    me->head.destroy = &HEAD_DESTROY_CALLBACK;
    me->head.id = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_destroy(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    struct ecu_dlist_iterator iterator;

    ECU_DLIST_FOR_EACH(node, &iterator, me)
    {
        ecu_dnode_destroy(node);
    }

    /* Setting to NULL values forces user to reconstruct the list if they
    want to use it again, assuming asserts are enabled. */
    me->head.next = (struct ecu_dnode *)0;
    me->head.prev = (struct ecu_dnode *)0;
    me->head.destroy = &HEAD_DESTROY_CALLBACK;
    me->head.id = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_clear(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    struct ecu_dlist_iterator iterator;

    ECU_DLIST_FOR_EACH(node, &iterator, me)
    {
        ecu_dnode_remove(node);
    }

    /* Reset list values back to default. */
    ecu_dlist_ctor(me);
}

struct ecu_dnode *ecu_dlist_front(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    struct ecu_dnode *front = (struct ecu_dnode *)0;

    if (!ecu_dlist_empty(me))
    {
        front = me->head.next;
        ECU_RUNTIME_ASSERT( (node_valid(front)) );
    }

    return front;
}

const struct ecu_dnode *ecu_dlist_cfront(const struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    const struct ecu_dnode *front = (const struct ecu_dnode *)0;

    if (!ecu_dlist_empty(me))
    {
        front = me->head.next;
        ECU_RUNTIME_ASSERT( (node_valid(front)) );
    }

    return front;
}

void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );
    ecu_dnode_insert_after(&me->head, node);
}

struct ecu_dnode *ecu_dlist_pop_front(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    struct ecu_dnode *front = (struct ecu_dnode *)0;

    if (!ecu_dlist_empty(me))
    {
        front = me->head.next;
        ecu_dnode_remove(me->head.next); /* Do not have to assert node_valid() since it is done in ecu_dnode_remove(). */
    }

    return front;
}

struct ecu_dnode *ecu_dlist_back(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    struct ecu_dnode *tail = (struct ecu_dnode *)0;

    if (!ecu_dlist_empty(me))
    {
        tail = me->head.prev;
        ECU_RUNTIME_ASSERT( (node_valid(tail)) );
    }

    return tail;
}

const struct ecu_dnode *ecu_dlist_cback(const struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    const struct ecu_dnode *tail = (const struct ecu_dnode *)0;

    if (!ecu_dlist_empty(me))
    {
        tail = me->head.prev;
        ECU_RUNTIME_ASSERT( (node_valid(tail)) );
    }

    return tail;
}

void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );
    ecu_dnode_insert_before(&me->head, node);
}

struct ecu_dnode *ecu_dlist_pop_back(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    struct ecu_dnode *tail = (struct ecu_dnode *)0;

    if (!ecu_dlist_empty(me))
    {
        tail = me->head.prev;
        ecu_dnode_remove(me->head.prev); /* Do not have to assert node_valid() since it is done in ecu_dnode_remove(). */
    }

    return tail;
}

void ecu_dlist_insert_before(struct ecu_dlist *me,
                             struct ecu_dnode *node,
                             bool (*condition)(const struct ecu_dnode *node, const struct ecu_dnode *position, void *data),
                             void *data)
{
    ECU_RUNTIME_ASSERT( (me && node && condition) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );
    bool inserted = false;
    struct ecu_dlist_iterator iterator;

    ECU_DLIST_FOR_EACH(i, &iterator, me)
    {
        if ((*condition)(node, i, data))
        {
            ecu_dnode_insert_before(i, node);
            inserted = true;
            break;
        }
    }

    if (!inserted)
    {
        /* Enters if list is empty or condition failed all nodes. This is
        faster and less verbose than checking for empty list at beginning. */
        ecu_dlist_push_back(me, node);
    }
}

void ecu_dlist_sort(struct ecu_dlist *me,
                    bool (*lhs_less_than_rhs)(const struct ecu_dnode *lhs, const struct ecu_dnode *rhs, void *data),
                    void *data)
{
    /**
     * This file is copyright 2001 Simon Tatham.
     *
     * Permission is hereby granted, free of charge, to any person
     * obtaining a copy of this software and associated documentation
     * files (the "Software"), to deal in the Software without
     * restriction, including without limitation the rights to use,
     * copy, modify, merge, publish, distribute, sublicense, and/or
     * sell copies of the Software, and to permit persons to whom the
     * Software is furnished to do so, subject to the following
     * conditions:
     *
     * The above copyright notice and this permission notice shall be
     * included in all copies or substantial portions of the Software.
     *
     * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
     * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
     * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
     * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
     * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
     * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
     * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
     * SOFTWARE.
     *
     * See https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
     * for algorithm used.
     */
    ECU_RUNTIME_ASSERT( (me && lhs_less_than_rhs) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    bool swap_q = false;
    size_t K = 1;
    size_t qsize, psize, nmerges = 0;
    struct ecu_dnode *e, *p, *q = (struct ecu_dnode *)0;
    const struct ecu_dnode *HEAD = &me->head; /* Must be after list is asserted. */

    while (1)
    {
        p = me->head.next;
        ECU_RUNTIME_ASSERT( (node_valid(p)) );
        nmerges = 0;

        while (p != HEAD)
        {
            nmerges++;
            q = p;
            psize = 0;

            /* Step q by K increments or until the end of the list is reached. */
            for (size_t i = 0; i < K; i++)
            {
                psize++;
                q = q->next;
                ECU_RUNTIME_ASSERT( (node_valid(q)) );
                if (q == HEAD)
                {
                    break;
                }
            }
            qsize = K;

            while ((psize > 0) || (qsize > 0 && q != HEAD))
            {
                /* p and/or q are non-empty lists so we have lists to merge
                sort. Decide whether next element of merge comes from p or q. */
                if (psize == 0)
                {
                    /* p is empty. e must come from q. Do not set swap_q here.
                    List doesn't have to be edited if p list is empty. */
                    e = q;
                    q = q->next;
                    ECU_RUNTIME_ASSERT( (node_valid(q)) );
                    qsize--;
                }
                else if (qsize == 0 || q == HEAD) /* qsize is always set to K but qlist can be empty!! Must OR with q == HEAD. */
                {
                    /* q is empty. e must come from p. */
                    e = p;
                    p = p->next;
                    ECU_RUNTIME_ASSERT( (node_valid(p)) );
                    psize--;
                }
                else if ((*lhs_less_than_rhs)(q, p, data))
                {
                    /* First element of q is less than p. e must come from q. */
                    swap_q = true;
                    e = q;
                    q = q->next;
                    ECU_RUNTIME_ASSERT( (node_valid(q)) );
                    qsize--;
                }
                else
                {
                    /* First element of p is less than or equal to q. e must come from p. */
                    e = p;
                    p = p->next;
                    ECU_RUNTIME_ASSERT( (node_valid(p)) );
                    psize--;
                }

                /* We only have to swap elements if the target node e is in
                the qlist. Otherwise the p node can be left untouched. */
                if (swap_q)
                {
                    swap_q = false;
                    ecu_dnode_remove(e); /* Must remove before swapping. API requires added nodes to not be in a list. */
                    ecu_dnode_insert_before(p, e);
                }
            }

            p = q;
        }

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1) /* Allow for nmerges==0, the empty list case. */
        {
            break;
        }
        else
        {
            K *= 2U;
        }
    }
}

void ecu_dlist_swap(struct ecu_dlist *me, struct ecu_dlist *other)
{
    ECU_RUNTIME_ASSERT( (me && other) );
    ECU_RUNTIME_ASSERT( (me != other) );
    ECU_RUNTIME_ASSERT( (list_valid(me) && list_valid(other)) );
    struct ecu_dnode *temp = me->head.next;

    if (!ecu_dlist_empty(me) || !ecu_dlist_empty(other))
    {
        me->head.next           = other->head.next;
        other->head.next        = temp;
        me->head.next->prev     = &me->head;
        other->head.next->prev  = &other->head;

        temp                    = me->head.prev;
        me->head.prev           = other->head.prev;
        other->head.prev        = temp;
        me->head.prev->next     = &me->head;
        other->head.prev->next  = &other->head;
    }
}

size_t ecu_dlist_size(const struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    size_t i = 0;
    struct ecu_dlist_citerator citerator;

    /* Loop through entire list here instead of using a size variable in
    ecu_dlist to prevent all add and remove functions having to keep track
    of size. Isolate this dependency to only this function. */
    ECU_DLIST_CONST_FOR_EACH(n, &citerator, me)
    {
        ++i;
    }

    return i;
}

bool ecu_dlist_empty(const struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    return (me->head.next == &me->head);
}

/*------------------------------------------------------------*/
/*----------- NON-CONST ITERATOR MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

struct ecu_dnode *ecu_dlist_iterator_begin(struct ecu_dlist_iterator *me, struct ecu_dlist *list)
{
    ECU_RUNTIME_ASSERT( (me && list) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next->next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(list->head.next->next)) );

    me->list = list;
    me->current = list->head.next;
    me->next = list->head.next->next;
    return (me->current);
}

struct ecu_dnode *ecu_dlist_iterator_at(struct ecu_dlist_iterator *me, 
                                        struct ecu_dlist *list, 
                                        struct ecu_dnode *start)
{
    ECU_RUNTIME_ASSERT( (me && list && start) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_valid(start)) );
    ECU_RUNTIME_ASSERT( (node_in_list(start)) );
    ECU_RUNTIME_ASSERT( (node_valid(start->next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(start->next)) );

    me->list = list;
    me->current = start;
    me->next = start->next;
    return (me->current);
}

struct ecu_dnode *ecu_dlist_iterator_end(struct ecu_dlist_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    return (&me->list->head);
}

struct ecu_dnode *ecu_dlist_iterator_next(struct ecu_dlist_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    ECU_RUNTIME_ASSERT( (node_valid(me->next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(me->next)) );
    /* me->next->next does not have to be asserted here. It will be checked in
    the me->next asserts when this function is called again. */

    me->current = me->next;
    me->next = me->next->next;
    return (me->current);
}

/*------------------------------------------------------------*/
/*------------- CONST ITERATOR MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

const struct ecu_dnode *ecu_dlist_iterator_cbegin(struct ecu_dlist_citerator *me, 
                                                  const struct ecu_dlist *list)
{
    ECU_RUNTIME_ASSERT( (me && list) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next->next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(list->head.next->next)) );

    me->list = list;
    me->current = list->head.next;
    me->next = list->head.next->next;
    return (me->current);
}

const struct ecu_dnode *ecu_dlist_iterator_cat(struct ecu_dlist_citerator *me, 
                                               const struct ecu_dlist *list, 
                                               const struct ecu_dnode *start)
{
    ECU_RUNTIME_ASSERT( (me && list && start) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_valid(start)) );
    ECU_RUNTIME_ASSERT( (node_in_list(start)) );
    ECU_RUNTIME_ASSERT( (node_valid(start->next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(start->next)) );

    me->list = list;
    me->current = start;
    me->next = start->next;
    return (me->current);
}

const struct ecu_dnode *ecu_dlist_iterator_cend(struct ecu_dlist_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    return (&me->list->head);
}

const struct ecu_dnode *ecu_dlist_iterator_cnext(struct ecu_dlist_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    ECU_RUNTIME_ASSERT( (node_valid(me->next)) );
    ECU_RUNTIME_ASSERT( (node_in_list(me->next)) );
    /* me->next->next does not have to be asserted here. It will be checked in
    the me->next asserts when this function is called again. */

    me->current = me->next;
    me->next = me->next->next;
    return (me->current);
}
