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


void ecu_dlist_destroy(struct ecu_circular_dll *me)
{
    struct ecu_circular_dll_iterator iterator;

    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

#warning "TODO"
    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&iterator, me);
         node != ecu_circular_dll_iterator_end(&iterator); 
         node = ecu_circular_dll_iterator_next(&iterator))
    {
        ecu_circular_dll_remove_node(node); /* Set next and prev to itself. */

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
    
    /* WARNING: This requires ecu_dlist_insert() insert the new node BEFORE position. */
    ecu_dlist_insert(me, me->head.next, new);
}


void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dlist_node *new)
{
    ECU_RUNTIME_ASSERT( (me && new ) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, new) && !node_in_other_list(me, new)) );

    /* WARNING: This requires ecu_dlist_insert() insert the new node BEFORE position. */
    ecu_dlist_insert(me, &me->head, new);
}


struct ecu_dlist_node *ecu_dlist_pop(struct ecu_dlist *me, const struct ecu_dlist_iterator *position)
{

}



// struct ecu_dlist_node *ecu_dlist_pop_front(struct ecu_dlist *me)
// {
//     struct ecu_dlist_node *front = (struct ecu_dlist_node *)0;
//     ECU_RUNTIME_ASSERT( (me) );
//     ECU_RUNTIME_ASSERT( (list_valid(me)) );
//     ECU_RUNTIME_ASSERT( (!ecu_dlist_is_empty(me)) );

//     front = me->head.next;
//     ecu_dlist_remove(me, front);
//     return front;
// }


// struct ecu_dlist_node *ecu_dlist_pop_back(struct ecu_dlist *me); // call ecu_dlist_remove()




uint32_t ecu_circular_dll_get_size(struct ecu_circular_dll *me)
{
    uint32_t i = 0;
    struct ecu_circular_dll_iterator iterator;

    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) ); /* Function called in macro so there is no overhead if asserts are disabled. */

    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&iterator, me);
         node != ecu_circular_dll_iterator_end(&iterator); 
         node = ecu_circular_dll_iterator_next(&iterator))
    {
        /* NULL assertions are done within the iterator functions. */
        i++;
    }
    return i;
}


bool ecu_circular_dll_is_empty(const struct ecu_circular_dll *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) ); /* Function called in macro so there is no overhead if asserts are disabled. */
    return (me->head.next == &me->head);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_circular_dll_node *ecu_circular_dll_iterator_begin(struct ecu_circular_dll_iterator *me,
                                                              struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (me && list) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) ); /* Function called in macro so there is no overhead if asserts are disabled. */

    me->list = list;
    me->current = list->head.next;      /* list_valid() asserts this node is in list. */
    me->next = list->head.next->next;   /* We assert this node is valid in ecu_circular_dll_iterator_next() call so it's not done here. */
    return (me->current);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) ); /* Function called in macro so there is no overhead if asserts are disabled. */
    return (&me->list->head);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );

    /* Do NOT do ECU_RUNTIME_ASSERT( (node_in_list(me->next)), DLL_ASSERT_FUNCTOR);
    since it is perfectly valid for iterated list to have no nodes in it. 
    Functions called in macro so there is no overhead if asserts are disabled. */
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    ECU_RUNTIME_ASSERT( (node_valid(me->next)) );
    ECU_RUNTIME_ASSERT( (node_valid(me->next->next)) );

    me->current = me->next;
    me->next = me->next->next;
    return (me->current);
}
