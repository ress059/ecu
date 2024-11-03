/**
 * @file
 * @brief See @ref dlist.h
 * 
 * @author Ian Ress 
 * @version 0.1
 * @date 2024-04-14
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/dlist.h"

/* STDLib. */
#include <stdbool.h>

/* Runtime asserts. */
#include "ecu/asserter.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- DEFINE FILE NAME FOR ASSERTER ----------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/dlist.c")



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- STATIC FUNCTION DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// ALL FUNCTIONS REQUIRE NODE OR LIST TO BE PREVIOUSLY constructed or else next and prev
// pointers can be garbage non-NULL values, leading to seg faults when accessed.

// node is constructed but doesn't have to be apart of a list.
static bool node_valid(const struct ecu_dlist_node *node);

/**
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns true if the list is valid. False otherwise. A valid list
 * means list->head.next->prev == &list->head and list->head.prev->next == &list->head
 */
// list in constructed.
static bool list_valid(const struct ecu_dlist *list);

static bool node_is_head(const struct ecu_dlist *list, const struct ecu_dlist_node *node);

// pre = node and list constructed. Return true if node is in this list.
static bool node_in_this_list(const struct ecu_dlist *list, const struct ecu_dlist_node *node);

// pre = node and list constructed. Return true if node in another list.
static bool node_in_other_list(const struct ecu_dlist *list, const struct ecu_dlist_node *node);

/**
 * @brief Callback assigned to @ref ecu_circular_dll.head.destroy in 
 * @ref ecu_circular_dll_ctor(). This head node is solely used 
 * as a dummy delimiter that should never be destroyed. Therefore we
 * simply assert if this callback runs.
 */
static void list_head_destroy_callback(struct ecu_dlist_node *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool node_valid(const struct ecu_dlist_node *node)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );

    if ((node->next->prev == node) && \
        (node->prev->next == node))
    {
        valid = true;
    }

    return valid;
}

static bool list_valid(const struct ecu_dlist *list)
{
    /* Keep node_valid() and list_valid() completely separate in case check
    for each become very different in the future. */
    bool valid = false;
    ECU_RUNTIME_ASSERT( (list) );
    ECU_RUNTIME_ASSERT( (list->head.next && list->head.prev) );

    if ((list->head.next->prev == &list->head) && \
        (list->head.prev->next == &list->head) && \
        (list->head.list == list))
    {
        valid = true;
    }

    return valid;
}

static bool node_is_head(const struct ecu_dlist *list, const struct ecu_dlist_node *node)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (list && node) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );

    if ((node->list == list) && \
        (&list->head == node))
    {
        status = true;
    }

    return status;
}

static bool node_in_this_list(const struct ecu_dlist *list, const struct ecu_dlist_node *node)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (list && node) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );

    if ((node->list == list) && \
        (node->next != node) && (node->prev != node) && \
        (node->next->prev == node) && (node->prev->next == node))
    {
        status = true;
    }

    return status;
}

static bool node_in_other_list(const struct ecu_dlist *list, const struct ecu_dlist_node *node)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (list && node) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );

    if ((node->list != list) && \
        (node->next != node) && (node->prev != node) && \
        (node->next->prev == node) && (node->prev->next == node))
    {
        status = true;
    }

    return status;
}

static void list_head_destroy_callback(struct ecu_dlist_node *me)
{
    (void)me;
    ECU_RUNTIME_ASSERT( (false) );
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_dlist_ctor(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Do NOT assert if list has nodes in it or call list_valid(me) since it is valid 
    for next and prev pointers to be initialized to non-NULL garbage values before 
    a constructor call. Otherwise me->head.next->.. and me->head.prev->.. accesses 
    can lead to seg faults. It is the user's responsibility to not construct an 
    active list that has nodes in it, which is clearly outlined in the warning 
    directive of this function description. Also do not use ecu_dlist_node_ctor()
    since we use reserved event ID. */

    me->head.next       = &me->head;
    me->head.prev       = &me->head;
    me->head.list       = me;
    me->head.destroy    = &list_head_destroy_callback;
    me->head.id         = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_destroy(struct ecu_dlist *me)
{
    /* Do not use ecu_dlist_node_destroy() in this function since we 
    know for sure that all nodes are within this list. */
    struct ecu_dlist_iterator iterator;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    for (struct ecu_dlist_node *node = ecu_dlist_iterator_begin(&iterator, me);
         node != ecu_dlist_iterator_end(&iterator); 
         node = ecu_dlist_iterator_next(&iterator))
    {
        ecu_dlist_remove(node);

        if (node->destroy != ECU_DLIST_NODE_DESTROY_UNUSED)
        {
            (*node->destroy)(node, node->id);
            node->destroy = ECU_DLIST_NODE_DESTROY_UNUSED;
        }

        /* Yes, some of this is redundant since ecu_dlist_remove(node) is called
        beforehand. However this guarantees that the node is reset and that this
        function is completely independent from ecu_dlist_remove(). Also reset 
        the node ID AFTER the destroy callback is called so user is able to identify 
        their node data type in their callback. */
        node->next  = node;
        node->prev  = node;
        node->list  = (struct ecu_dlist *)0;
        node->id    = ECU_OBJECT_ID_RESERVED;
    }

    me->head.next       = &me->head;
    me->head.prev       = &me->head;
    me->head.list       = (struct ecu_dlist *)0; /* Forces user to have to reconstruct list. */
    me->head.destroy    = &list_head_destroy_callback;
    me->head.id         = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_node_ctor(struct ecu_dlist_node *me,
                         void (*destroy_0)(struct ecu_dlist_node *me, ecu_object_id id),
                         ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (id_0 >= ECU_VALID_OBJECT_ID_BEGIN) );
    /* Do not assert node_in_this_list(me) or node_in_other_list(me) since
    since it is valid for next, prev, and list pointer to be initialized to 
    non-NULL garbage values before a constructor call. It is the user's 
    responsibility to not construct an active node, which is clearly outlined
    in the warning directive of this function description. */

    me->next    = me;
    me->prev    = me;
    me->list    = (struct ecu_dlist *)0;
    me->destroy = destroy_0; /* Optional callback so do not NULL assert. */
    me->id      = id_0;      /* Optional. */
}

# warning "TODO STOPPED HERE. ADDING COMMENTS"
void ecu_dlist_node_destroy(struct ecu_dlist_node *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );

    /* It is valid if the node has been constructed but it is not in a list. */
    if (me->list && node_in_this_list(me->list, me))
    {
        ecu_dlist_remove(me);
    }

    if (me->destroy)
    {
        (*me->destroy)(me, me->id);
    }
    me->next = me;
    me->prev = me;
    me->list = (struct ecu_dlist *)0;
    me->destroy = ECU_DLIST_NODE_DESTROY_UNUSED;

}

void ecu_dlist_insert_before(struct ecu_dlist *me, struct ecu_dlist_node *position, struct ecu_dlist_node *node)
{
    struct ecu_dlist_node *prev = (struct ecu_dlist_node *)0;
    ECU_RUNTIME_ASSERT( (me && position && node) );
    ECU_RUNTIME_ASSERT( (position != node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(me, position)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, node) && !node_in_other_list(me, node)) );

    prev = position->prev;

    position->prev  = node;
    node->next      = position;
    node->prev      = prev;
    prev->next      = node;
    node->list      = me;
}

void ecu_dlist_insert_after(struct ecu_dlist *me, struct ecu_dlist_node *position, struct ecu_dlist_node *node)
{
    ECU_RUNTIME_ASSERT( (me && position && node) );
    ECU_RUNTIME_ASSERT( (position != node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(me, position)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, node) && !node_in_other_list(me, node)) );
    ecu_dlist_insert_before(me, position->next, node);
}

void ecu_dlist_remove(struct ecu_dlist *me, struct ecu_dlist_node *node)
{
    struct ecu_dlist_node *next = (struct ecu_dlist_node *)0;
    struct ecu_dlist_node *prev = (struct ecu_dlist_node *)0;
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(me, node)) );

    next = node->next;
    prev = node->prev;

    next->prev = prev;
    prev->next = next;
    node->next = node;
    node->prev = node;
    node->list = (struct ecu_dlist *)0;
}

void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dlist_node *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, node) && !node_in_other_list(me, node)) );
    ecu_dlist_insert_after(me, &me->head, node);
}

void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dlist_node *node)
{
    ECU_RUNTIME_ASSERT( (me && node ) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, node) && !node_in_other_list(me, node)) );
    ecu_dlist_insert_before(me, &me->head, node);
}

size_t ecu_dlist_get_size(struct ecu_dlist *me)
{
    size_t i = 0;
    struct ecu_dlist_iterator iterator;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    /* Loop through entire list here instead of using a size variable in 
    ecu_dlist to prevent all add and remove functions having to keep track
    of size. Isolate this dependency to only this function. */
    for (struct ecu_dlist_node *node = ecu_dlist_iterator_begin(&iterator, me);
         node != ecu_dlist_iterator_end(&iterator); 
         node = ecu_dlist_iterator_next(&iterator))
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



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_dlist_node *ecu_dlist_iterator_begin(struct ecu_dlist_iterator *me, struct ecu_dlist *list)
{
    ECU_RUNTIME_ASSERT( (me && list) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next) || node_is_head(list, list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next->next) || node_is_head(list, list->head.next->next)) );

    me->list = list;
    me->current = list->head.next;
    me->next = list->head.next->next;
    return (me->current);
}

struct ecu_dlist_node *ecu_dlist_iterator_end(struct ecu_dlist_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    return (&me->list->head);
}

struct ecu_dlist_node *ecu_dlist_iterator_next(struct ecu_dlist_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    ECU_RUNTIME_ASSERT( (nost_in_this_list(me->list, me->next) || node_is_head(me->list, me->next)) );

    me->current = me->next;
    me->next = me->next->next;
    return (me->current);
}

const struct ecu_dlist_node *ecu_dlist_const_iterator_begin(struct ecu_dlist_const_iterator *me, const struct ecu_dlist *list)
{
    ECU_RUNTIME_ASSERT( (me && list) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next) || node_is_head(list, list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next->next) || node_is_head(list, list->head.next->next)) );

    me->list = list;
    me->current = list->head.next;
    me->next = list->head.next->next;
    return (me->current);
}

const struct ecu_dlist_node *ecu_dlist_const_iterator_end(struct ecu_dlist_const_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    return ((const struct ecu_dlist_node *)(&me->list->head));
}

const struct ecu_dlist_node *ecu_dlist_const_iterator_next(struct ecu_dlist_const_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    ECU_RUNTIME_ASSERT( (nost_in_this_list(me->list, me->next) || node_is_head(me->list, me->next)) );

    me->current = me->next;
    me->next = me->next->next;
    return (me->current);
}
