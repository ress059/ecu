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

/**
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns true if the list is valid. False otherwise. A valid list
 * means list->head.next->prev == &list->head and list->head.prev->next == &list->head
 */
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

static bool list_valid(const struct ecu_dlist *list)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (list) );
    ECU_RUNTIME_ASSERT( (list->head.next && list->head.prev) );

    /* Note how this also handles the case where head node's next and prev pointers point 
    to itself. Requires list to have been constructed since we are accessing list->head.next->prev
    and list->head.prev->next. Otherwise garbage, non-NULL values of list->head.next and 
    list->head.prev can cause seg faults. */
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

    /* Requires node to have been constructed since we are accesing node->next->prev
    and node->prev->next. Otherwise garbage, non-NULL values of node->next and node->prev 
    can cause seg faults. */
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

    /* Requires node to have been constructed since we are accesing node->next->prev
    and node->prev->next. Otherwise garbage, non-NULL values of node->next and node->prev 
    can cause seg faults. */
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
    struct ecu_dlist_iterator iterator;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    for (struct ecu_dlist_node *node = ecu_dlist_iterator_begin(&iterator, me);
         node != ecu_dlist_iterator_end(&iterator); 
         node = ecu_dlist_iterator_next(&iterator))
    {
        ecu_dlist_remove(me, node); /* Sets next and prev to self. */

        /* Do not reset node->id so user is able to identify their data type 
        in their destructor callback. */
        if (node->destroy)
        {
            (*node->destroy)(node);
            node->destroy = (void (*)(struct ecu_circular_dll_node *))0;
        }
    }

    me->head.next = &me->head;
    me->head.prev = &me->head;
}


void ecu_dlist_node_ctor(struct ecu_dlist_node *me,
                        void (*destroy_0)(struct ecu_dlist_node *me),
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


void ecu_dlist_insert(struct ecu_dlist *me, struct ecu_dlist_node *position, struct ecu_dlist_node *new)
{
    struct ecu_dlist_node *prev = (struct ecu_dlist_node *)0;
    ECU_RUNTIME_ASSERT( (me && position && new) );
    ECU_RUNTIME_ASSERT( (position != new) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(me, position)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, new) && !node_in_other_list(me, new)) );

    prev = position->prev;

    position->prev  = new;
    new->next       = position;
    new->prev       = prev;
    prev->next      = new;
    new->list       = me;
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


void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dlist_node *new)
{
    ECU_RUNTIME_ASSERT( (me && new) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, new) && !node_in_other_list(me, new)) );
    
    /* WARNING: This requires ecu_dlist_insert() to insert the new node BEFORE position. */
    ecu_dlist_insert(me, me->head.next, new);
}


void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dlist_node *new)
{
    ECU_RUNTIME_ASSERT( (me && new ) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, new) && !node_in_other_list(me, new)) );

    /* WARNING: This requires ecu_dlist_insert() to insert the new node BEFORE position. */
    ecu_dlist_insert(me, &me->head, new);
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
