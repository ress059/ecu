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
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor().
 * Otherwise node's next and prev pointers can be garbage non-NULL values,
 * leading to seg faults when accessed.
 * @brief Returns true if node's next and prev pointers are valid. Also
 * checks node's destroy callback and ID are properly assigned. False 
 * otherwise. 
 * 
 * @note This function will return false if list HEAD is supplied since
 * node's destroy callback and ID are checked.
 */
static bool node_valid(const struct ecu_dlist_node *node);

/**
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * Otherwise list head's next and prev pointers can be garbage non-NULL values,
 * leading to seg faults when accessed.
 * @brief Returns true if list HEAD's next and prev pointers are valid.
 * Also checks list HEAD's list pointer, destroy callback, and ID are 
 * properly assigned. False otherwise.
 */
static bool list_valid(const struct ecu_dlist *list);

/**
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor() and
 * @p node previously constructed via call to @ref ecu_dlist_node_ctor(). Otherwise
 * next and prev pointers can be garbage non-NULL values, leading to seg faults
 * when accessed. 
 * @brief Returns true if @p node is HEAD of @p list. False otherwise.
 */
static bool node_is_head(const struct ecu_dlist *list, const struct ecu_dlist_node *node);

/**
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor() and
 * @p node previously constructed via call to @ref ecu_dlist_node_ctor(). Otherwise
 * next and prev pointers can be garbage non-NULL values, leading to seg faults
 * when accessed.
 * @brief Returns true if @p node is in @p list. Also returns true if @p node
 * is list HEAD. False otherwise.
 */
static bool node_in_this_list(const struct ecu_dlist *list, const struct ecu_dlist_node *node);

/**
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor() and
 * @p node previously constructed via call to @ref ecu_dlist_node_ctor(). Otherwise
 * next and prev pointers can be garbage non-NULL values, leading to seg faults
 * when accessed.
 * @brief Returns true if @p node is in another list. Also returns true 
 * if @p node is HEAD of another list. False otherwise.
 * 
 * @param list If node is in this supplied list, function returns false.
 */
static bool node_in_other_list(const struct ecu_dlist *list, const struct ecu_dlist_node *node);

/**
 * @brief Callback assigned to @ref ecu_dlist.head.destroy in 
 * @ref ecu_dlist_ctor(). The HEAD node is solely used as a 
 * dummy delimiter that should never be destroyed. Therefore we
 * simply assert if this callback runs. Also provides additional
 * protection if HEAD node is directly accessed and used in a list.
 */
static void INVALID_DESTROY_CALLBACK(struct ecu_dlist_node *me, ecu_object_id id);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool node_valid(const struct ecu_dlist_node *node)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (node) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );

    if ((node->next->prev == node) && \
        (node->prev->next == node) && \
        (node->destroy != &INVALID_DESTROY_CALLBACK) && \
        (node->id >= ECU_VALID_OBJECT_ID_BEGIN))
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
        (list->head.list == list) && \
        (list->head.destroy == &INVALID_DESTROY_CALLBACK) && \
        (list->head.id == ECU_OBJECT_ID_RESERVED))
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
    /* Do not assert node_valid(node) since node can be HEAD. */

    /* list_valid() already checks all of HEAD's values so we only
    need to check if node == list HEAD. */
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
    #warning "TODO Stopped here. These static functions are kind of a mess rn. \
    Making insert() and remove() function node class functions so we dont need to pass \
    in list. 
    ECU_RUNTIME_ASSERT( (node_valid(node) || node_is_head(list, node)) );
    /* Do not assert node_valid(node) since node can be HEAD. */

    if ((node->list == list) && \
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
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    /* Do not assert node_valid(node) since node can be HEAD. */

    if ((node->list != list) && \
        (node->next->prev == node) && (node->prev->next == node))
    {
        status = true;
    }

    if ((node->list) && (node->list != list))
    {
        ECU_RUNTIME_ASSERT( (list_valid(node->list)) );


    } && node_in_this_list(node->list, node)

    if (node->list) && node->list != list
    {
        ECU_RUNTIME_ASSERT( (list_valid(node->list)) );

    }

    return status;
}

static void INVALID_DESTROY_CALLBACK(struct ecu_dlist_node *me, ecu_object_id id)
{
    (void)me;
    (void)id;
    ECU_RUNTIME_ASSERT( (false) );
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_dlist_ctor(struct ecu_dlist *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Do not assert if list has nodes in it or assert list_valid(me) since it is
    valid for HEAD's next and prev pointer to be initialized to garbage non-NULL
    values before a constructor call. It is the user's responsibility to not 
    construct an active list, which is clearly outlined in the warning directive 
    of this function description. Also do not use ecu_dlist_node_ctor() in this
    function since we use a reserved event ID. */

    me->head.next       = &me->head;
    me->head.prev       = &me->head;
    me->head.list       = me;
    me->head.destroy    = &INVALID_DESTROY_CALLBACK;
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
        ecu_dlist_remove(me, node);

        if (node->destroy != ECU_DLIST_NODE_DESTROY_UNUSED)
        {
            (*node->destroy)(node, node->id);
        }

        /* Yes, some of this is redundant since ecu_dlist_remove() is called
        beforehand. However this guarantees that the node is reset and that this
        function is completely independent from ecu_dlist_remove(). Also reset 
        the node ID AFTER the destroy callback is called so user is able to identify 
        their node data type in their callback. */
        node->next = node;
        node->prev = node;
        node->list = (struct ecu_dlist *)0;
        /* Forces user to have to reconstruct node. node_valid() checks destroy
        callback and ID. */
        node->destroy = &INVALID_DESTROY_CALLBACK;
        node->id = ECU_OBJECT_ID_RESERVED; 
    }

    me->head.next = &me->head;
    me->head.prev = &me->head;
    me->head.list = (struct ecu_dlist *)0; /* Forces user to have to reconstruct list. list_valid() checks if this is self. */
    me->head.destroy = &INVALID_DESTROY_CALLBACK;
    me->head.id = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_node_ctor(struct ecu_dlist_node *me,
                         void (*destroy_0)(struct ecu_dlist_node *me, ecu_object_id id),
                         ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (destroy_0 != &INVALID_DESTROY_CALLBACK) );
    ECU_RUNTIME_ASSERT( (id_0 >= ECU_VALID_OBJECT_ID_BEGIN) );
    /* Do not assert node_valid(), !node_in_this_list(), or !node_in_other_list() 
    since it is valid for next, prev, and list pointers to be initialized to 
    garbage non-NULL values before a constructor call. It is the user's 
    responsibility to not construct an active node, which is clearly outlined
    in the warning directive of this function description. */

    me->next    = me;
    me->prev    = me;
    me->list    = (struct ecu_dlist *)0;
    me->destroy = destroy_0; /* Optional callback so do not NULL assert. */
    me->id      = id_0;      /* Optional. */
}

void ecu_dlist_node_destroy(struct ecu_dlist_node *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) ); /* Can't be HEAD. */

    /* me->list NULL check MUST be first. */
    if (me->list && node_in_this_list(me->list, me) && !node_is_head(me->list, me))
    {
        ecu_dlist_remove(me->list, me);
    }

    if (me->destroy != ECU_DLIST_NODE_DESTROY_UNUSED)
    {
        (*me->destroy)(me, me->id);
    }

    me->next = me;
    me->prev = me;
    me->list = (struct ecu_dlist *)0;
    /* Forces user to have to reconstruct node. node_valid() checks destroy
    callback and ID. Also reset ID AFTER destroy callback is called so user
    can identify their type. */
    me->destroy = &INVALID_DESTROY_CALLBACK;
    me->id = ECU_OBJECT_ID_RESERVED; 
}

ecu_object_id ecu_dlist_node_get_id(const struct ecu_dlist_node *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    return (me->id);
}

void ecu_dlist_node_insert_before(struct ecu_dlist_node *position, struct ecu_dlist_node *node)
{
    struct ecu_dlist_node *prev = (struct ecu_dlist_node *)0;
    ECU_RUNTIME_ASSERT( (position && node) );
    ECU_RUNTIME_ASSERT( (position != node) );
#warning "TODO: Stopped editing the node class functions here. Still ahve to do this one and all the others."
    ECU_RUNTIME_ASSERT( (list_valid(position->list)) ); /* Do not assert list_valid(node->list) since we are adding this node to a list. */
    ECU_RUNTIME_ASSERT( (node_valid(node)) ); /* Do not assert node_valid(position) since position can be HEAD. */
    ECU_RUNTIME_ASSERT( (node_in_this_list(position->list, position)) );
    ECU_RUNTIME_ASSERT( (!node_in_this_list(position->list, node) && !node_in_other_list(position->list, node)) );

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
    ECU_RUNTIME_ASSERT( (node_valid(node)) ); /* Do not assert node_valid(position) since position can be HEAD. */
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
    ECU_RUNTIME_ASSERT( (node_valid(node)) ); /* node can't be HEAD. */
    ECU_RUNTIME_ASSERT( (node_in_this_list(me, node)) );

    next = node->next;
    prev = node->prev;

    next->prev = prev;
    prev->next = next;
    node->next = node;
    node->prev = node;
    node->list = (struct ecu_dlist *)0;
    /* Do not reset destroy callback or ID. This is only a remove function,
    not a destroy function. */
}

void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dlist_node *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) ); /* node can't be HEAD. */
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, node) && !node_in_other_list(me, node)) );
    ecu_dlist_insert_after(me, &me->head, node);
}

void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dlist_node *node)
{
    ECU_RUNTIME_ASSERT( (me && node ) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) ); /* node can't be HEAD. */
    ECU_RUNTIME_ASSERT( (!node_in_this_list(me, node) && !node_in_other_list(me, node)) );
    ecu_dlist_insert_before(me, &me->head, node);
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
    for (const struct ecu_dlist_node *node = ecu_dlist_const_iterator_begin(&citerator, me);
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



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_dlist_node *ecu_dlist_iterator_begin(struct ecu_dlist_iterator *me, struct ecu_dlist *list)
{
    ECU_RUNTIME_ASSERT( (me && list) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next) || node_is_head(list, list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next->next)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next->next) || node_is_head(list, list->head.next->next)) );

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
    ECU_RUNTIME_ASSERT( (node_in_this_list(me->list, me->next)) );
    ECU_RUNTIME_ASSERT( (node_valid(me->next) || node_is_head(me->list, me->next)) );
    /* me->next->next does not have to be asserted here. It will be checked in
    the me->next asserts when this function is called again. */

    me->current = me->next;
    me->next = me->next->next;
    return (me->current);
}

const struct ecu_dlist_node *ecu_dlist_const_iterator_begin(struct ecu_dlist_const_iterator *me, const struct ecu_dlist *list)
{
    ECU_RUNTIME_ASSERT( (me && list) );
    ECU_RUNTIME_ASSERT( (list_valid(list)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next) || node_is_head(list, list->head.next)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(list, list->head.next->next)) );
    ECU_RUNTIME_ASSERT( (node_valid(list->head.next->next) || node_is_head(list, list->head.next->next)) );

    me->list = list;
    me->current = list->head.next;
    me->next = list->head.next->next;
    return (me->current);
}

const struct ecu_dlist_node *ecu_dlist_const_iterator_end(struct ecu_dlist_const_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    return (&me->list->head);
}

const struct ecu_dlist_node *ecu_dlist_const_iterator_next(struct ecu_dlist_const_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me->list)) );
    ECU_RUNTIME_ASSERT( (node_in_this_list(me->list, me->next)) );
    ECU_RUNTIME_ASSERT( (node_valid(me->next) || node_is_head(me->list, me->next)) );
    /* me->next->next does not have to be asserted here. It will be checked in
    the me->next asserts when this function is called again. */

    me->current = me->next;
    me->next = me->next->next;
    return (me->current);
}
