/**
 * @file
 * @brief See @ref dlist.h
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
 * via @ref ecu_dnode_ctor() or @ref ecu_dlist_ctor() if
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
    bool valid = false;
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );

    if ((node->next->prev == node) && \
        (node->prev->next == node) && \
        (node->destroy != &HEAD_DESTROY_CALLBACK) && \
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
    bool is_head = false;
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );
    /* Do NOT assert node_valid() since node_valid() calls this 
    function. Otherwise stack overflow will occur. */

    if ((node->next->prev == node) && \
        (node->prev->next == node) && \
        (node->destroy == &HEAD_DESTROY_CALLBACK) && \
        (node->id == ECU_OBJECT_ID_RESERVED))
    {
        is_head = true;
    }

    return is_head;
}

static bool node_in_list(const struct ecu_dnode *node)
{
    bool in_list = false;
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );

    if ((node->next != node) && \
        (node->prev != node) && \
        (node->next->prev == node) && \
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
    return node_is_valid_head(&list->head);
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
                    void (*destroy_0)(struct ecu_dnode *me, ecu_object_id id),
                    ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (destroy_0 != &HEAD_DESTROY_CALLBACK) );
    ECU_RUNTIME_ASSERT( (id_0 >= ECU_VALID_OBJECT_ID_BEGIN) );
    /* Do not assert node_valid() or !node_in_list() since next and prev pointers 
    can be initialized to garbage non-NULL values before a constructor call. It is 
    the user's responsibility to not construct an active node, which is clearly outlined
    in the warning directive of this function description. */

    me->next    = me;
    me->prev    = me;
    me->destroy = destroy_0; /* Optional callback so do not NULL assert. */
    me->id      = id_0;      /* Optional. */
}

void ecu_dnode_destroy(struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me) && !node_is_valid_head(me)) );

    /* Remove node from list. OK if node is not in list. */
    me->next->prev = me->prev;
    me->prev->next = me->next;

    if (me->destroy != ECU_DNODE_DESTROY_UNUSED)
    {
        (*me->destroy)(me, me->id);
    }

    /* Setting to NULL values forces user to reconstruct the node if they
    want to use it again, assuming asserts are enabled. Important to reset 
    destroy callback and ID only after the destroy callback executes. */
    me->next    = (struct ecu_dnode *)0;
    me->prev    = (struct ecu_dnode *)0;
    me->destroy = ECU_DNODE_DESTROY_UNUSED;
    me->id      = ECU_OBJECT_ID_RESERVED;
}

void ecu_dnode_insert_before(struct ecu_dnode *me, struct ecu_dnode *position)
{
    ECU_RUNTIME_ASSERT( (me && position) );
    ECU_RUNTIME_ASSERT( (me != position) );
    ECU_RUNTIME_ASSERT( (node_valid(me) && node_valid(position)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(me) && node_in_list(position)) );

    me->next                = position;
    me->prev                = position->prev;
    position->prev->next    = me;
    position->prev          = me;
}

void ecu_dnode_insert_after(struct ecu_dnode *me, struct ecu_dnode *position)
{
    ECU_RUNTIME_ASSERT( (me && position) );
    ECU_RUNTIME_ASSERT( (me != position) );
    ECU_RUNTIME_ASSERT( (node_valid(me) && node_valid(position)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(me) && node_in_list(position)) );
    ecu_dnode_insert_before(me, position->next);
}

void ecu_dnode_remove(struct ecu_dnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_in_list(me) && !node_is_valid_head(me)) );

    me->next->prev = me->prev;
    me->prev->next = me->next;
    me->next       = me;
    me->prev       = me;
    /* Do not reset destroy callback or ID. This is only a remove function,
    not a destroy function. */
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

    me->head.next    = &me->head;
    me->head.prev    = &me->head;
    me->head.destroy = &HEAD_DESTROY_CALLBACK;
    me->head.id      = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_destroy(struct ecu_dlist *me)
{
    struct ecu_dlist_iterator iterator;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    for (struct ecu_dnode *node = ecu_dlist_iterator_begin(&iterator, me);
         node != ecu_dlist_iterator_end(&iterator); 
         node = ecu_dlist_iterator_next(&iterator))
    {
        ecu_dnode_destroy(node);
    }

    /* Setting to NULL values forces user to reconstruct the list if they
    want to use it again, assuming asserts are enabled. */
    me->head.next    = (struct ecu_dnode *)0;
    me->head.prev    = (struct ecu_dnode *)0;
    me->head.destroy = &HEAD_DESTROY_CALLBACK;
    me->head.id      = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );
    ecu_dnode_insert_after(&me->head, node);
}

void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );
    ecu_dnode_insert_before(&me->head, node);
}

// TODO Debating whether condition should take in pointer to const or not. 
void ecu_dlist_insert_before(struct ecu_dlist *me, 
                             struct ecu_dnode *node,
                             bool (*condition)(struct ecu_dnode *current, void *data),
                             void *data)
{
    bool inserted = false;
    struct ecu_dlist_iterator iterator;
    ECU_RUNTIME_ASSERT( (me && node && condition) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );

    for (struct ecu_dnode *i = ecu_dlist_iterator_begin(&iterator, me);
         i = ecu_dlist_iterator_end(&iterator);
         i = ecu_dlist_iterator_next(&iterator))
    {
        if ((*condition)(i, data))
        {
            ecu_dnode_insert_before(node, i);
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
                    bool (*lhs_less_than_rhs)(struct ecu_dnode *lhs, struct ecu_dnode *rhs, void *data),
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
     */
    ECU_RUNTIME_ASSERT( (me && lhs_less_than_rhs) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    bool merge, swap_q = false;
    size_t K = 1;
    size_t qsize, psize, nmerges = 0;
    struct ecu_dnode *e, *p, *q = (struct ecu_dnode *)0;
    const struct ecu_dnode *HEAD = &me->head; /* Must be after list is asserted. */

    while (1)
    {
        p = me->head.next;
        ECU_RUNTIME_ASSERT( (node_valid(p)) );

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
                    ecu_dnode_insert_before(p, q);
                }
            }

            p = q;
        }

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1)   /* Allow for nmerges==0, the empty list case. */
        {
            break;
        }
        else
        {
            K *= 2U;
        }
    }
}

void ecu_dlist_clear(struct ecu_dlist *me)
{
    struct ecu_dlist_iterator iterator;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    for (struct ecu_dnode *node = ecu_dlist_iterator_begin(&iterator, me);
         node != ecu_dlist_iterator_end(&iterator); 
         node = ecu_dlist_iterator_next(&iterator))
    {
        ecu_dnode_remove(node);
    }

    /* Reset list values back to default. */
    ecu_dlist_ctor(me);
}

size_t ecu_dlist_get_size(const struct ecu_dlist *me)
{
    size_t i = 0;
    struct ecu_dlist_const_iterator citerator;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    /* Loop through entire list here instead of using a size variable in 
    ecu_dlist to prevent all add and remove functions having to keep track
    of size. Isolate this dependency to only this function. */
    for (const struct ecu_dnode *node = ecu_dlist_const_iterator_begin(&citerator, me);
         node != ecu_dlist_const_iterator_end(&citerator); 
         node = ecu_dlist_const_iterator_next(&citerator))
    {
        ++i;
    }

    return i;
}

bool ecu_dlist_is_empty(const struct ecu_dlist *me)
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

const struct ecu_dnode *ecu_dlist_const_iterator_begin(struct ecu_dlist_const_iterator *me, const struct ecu_dlist *list)
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

const struct ecu_dnode *ecu_dlist_const_iterator_end(struct ecu_dlist_const_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    return (&me->list->head);
}

const struct ecu_dnode *ecu_dlist_const_iterator_next(struct ecu_dlist_const_iterator *me)
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
