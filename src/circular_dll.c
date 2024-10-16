/**
 * @file
 * @brief See @ref circular_dll.h
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
#include "ecu/circular_dll.h"

/* STDLib. */
#include <stdbool.h>

/* Runtime asserts. */
#include "ecu/asserter.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- DEFINE FILE NAME FOR ASSERTER ----------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/circular_dll.c")



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- STATIC FUNCTION DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @pre @p list previously constructed via call to @ref ecu_circular_dll_ctor().
 * @brief Returns true if the list is valid. False otherwise. A valid list
 * means list->head.next->prev == &list->head and list->head.prev->next == &list->head
 */
static bool list_valid(const struct ecu_circular_dll *list);


/**
 * @pre @p node previously constructed via call to @ref ecu_circular_dll_node_ctor().
 * @brief Returns true if the node is valid. False otherwise. A valid node
 * means node->next->prev == node and node->prev->next == node
 */
static bool node_valid(const struct ecu_circular_dll_node *node);


/**
 * @pre @p node previously constructed via call to @ref ecu_circular_dll_node_ctor().
 * @brief Returns true if node is in a list. False otherwise.
 */
static bool node_in_list(const struct ecu_circular_dll_node *node);


/**
 * @brief Callback assigned to @ref ecu_circular_dll.head.destroy in 
 * @ref ecu_circular_dll_ctor(). This head node is solely used 
 * as a dummy delimiter that should never be destroyed. Therefore we
 * simply assert if this callback runs.
 */
static void list_head_destroy_callback(struct ecu_circular_dll_node *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool list_valid(const struct ecu_circular_dll *list)
{
    bool valid = false;

    ECU_RUNTIME_ASSERT( (list) );
    ECU_RUNTIME_ASSERT( ((list->head.next) && (list->head.prev)) );

    /* Note how this also handles the case where head node's next and prev pointers point 
    to itself. Requires list to have been constructed since we are accessing list->head.next->prev
    and list->head.prev->next. Otherwise garbage, non-NULL values of list->head.next and 
    list->head.prev can cause seg faults. */
    if ((list->head.next->prev == &list->head) && \
        (list->head.prev->next == &list->head))
    {
        valid = true;
    }

    return valid;
}


static bool node_valid(const struct ecu_circular_dll_node *node)
{
    bool valid = false;

    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( ((node->next) && (node->prev)) );

    /* Note how this also handles the case where head node's next and prev pointers point 
    to itself. Requires node to have been constructed since we are accessing node->next->prev
    and node->prev->next. Otherwise garbage, non-NULL values of node->next and node->prev
    can cause seg faults. */
    if ((node->next->prev == node) && (node->prev->next == node))
    {
        valid = true;
    }

    return valid;
}


static bool node_in_list(const struct ecu_circular_dll_node *node)
{
    bool in_list = false;

    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( ((node->next) && (node->prev)) );

    /* Requires node to have been constructed since we are accesing node->next->prev
    and node->prev->next. Otherwise garbage, non-NULL values of node->next and node->prev 
    can cause seg faults. */
    if ((node->next != node) && (node->prev != node) && \
        (node->next->prev == node) && (node->prev->next == node))
    {
        in_list = true;
    }

    return in_list;
}


static void list_head_destroy_callback(struct ecu_circular_dll_node *me)
{
    (void)me;
    ECU_RUNTIME_ASSERT( (false) );
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_node_ctor(struct ecu_circular_dll_node *me,
                                void (*destroy_0)(struct ecu_circular_dll_node *me),
                                ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( ((me) && (id_0 >= ECU_VALID_OBJECT_ID_BEGIN)) );
    ECU_RUNTIME_ASSERT( (destroy_0 != (void (*)(struct ecu_circular_dll_node *))&ecu_circular_dll_destroy) );
    /* Do NOT do ECU_RUNTIME_ASSERT( (!node_in_list(me)), DLL_ASSERT_FUNCTOR );
    or ECU_RUNTIME_ASSERT( (node_is_valid(me)), DLL_ASSERT_FUNCTOR);
    since it is valid for next and prev pointers to be initialized to non-NULL 
    garbage values before a constructor call. Otherwise me->next->prev and 
    me->prev->next accesses can lead to seg faults. It is the user's 
    responsibility to not construct an active node, which is clearly outlined
    in the warning directive of this function description. */

    me->next    = me;
    me->prev    = me;
    me->destroy = destroy_0; /* Optional callback so do not NULL assert. */
    me->id      = id_0;      /* Optional. */
}


void ecu_circular_dll_ctor(struct ecu_circular_dll *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Do NOT assert if list has nodes in it or call list_valid(me) since it is valid 
    for next and prev pointers to be initialized to non-NULL garbage values before 
    a constructor call. Otherwise me->head.next->.. and me->head.prev->.. accesses 
    can lead to seg faults. It is the user's responsibility to not construct an 
    active list that has nodes in it, which is clearly outlined in the warning 
    directive of this function description. */

    me->head.next       = &me->head;
    me->head.prev       = &me->head;
    me->head.destroy    = &list_head_destroy_callback;
    me->head.id         = ECU_OBJECT_ID_RESERVED;
}


void ecu_circular_dll_destroy(struct ecu_circular_dll *me)
{
    struct ecu_circular_dll_iterator iterator;

    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) ); /* Function called in macro so there is no overhead if asserts are disabled. */

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


void ecu_circular_dll_push_back(struct ecu_circular_dll *me, 
                                struct ecu_circular_dll_node *node)
{
    struct ecu_circular_dll_node *old_tail = (struct ecu_circular_dll_node *)0;

    ECU_RUNTIME_ASSERT( (me && node) );

    /* Functions called in macro so there is no overhead if asserts are disabled. */
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );

    old_tail = me->head.prev;       /* Do not have to NULL assert because list_valid() check. */
    old_tail->next->prev = node;    /* me->head.prev = node; */
    node->next = old_tail->next;    /* node->next = me->head; */
    node->prev = old_tail;
    old_tail->next = node;
}


void ecu_circular_dll_remove_node(struct ecu_circular_dll_node *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_in_list(me)) ); /* Function called in macro so there is no overhead if asserts are disabled. */

    me->next->prev = me->prev;
    me->prev->next = me->next;
    me->next = me;
    me->prev = me;
}


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
