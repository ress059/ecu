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
via calls to ecu_dlist_node_ctor() and ecu_dlist_ctor(). Otherwise
next and prev pointers can be garbage non-NULL values, leading to seg
faults when accessed. */

/**
 * @brief Returns true if node has been properly constructed
 * via @ref ecu_dlist_node_ctor() or @ref ecu_dlist_ctor() if
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
/*------------------ NODE MEMBER FUNCTIONS -------------------*/
/*------------------------------------------------------------*/

void ecu_dlist_node_ctor(struct ecu_dnode *me,
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

void ecu_dlist_node_destroy(struct ecu_dnode *me)
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

ecu_object_id ecu_dlist_node_get_id(const struct ecu_dnode *me)
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
    use ecu_dlist_node_ctor(HEAD) since a reserved destroy callback and ID are
    assigned. */

    me->head.next    = &me->head;
    me->head.prev    = &me->head;
    me->head.destroy = &HEAD_DESTROY_CALLBACK;
    me->head.id      = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_destroy(struct ecu_dlist *me)
{
    /* Do not use ecu_dlist_node_destroy() in this function since we 
    know for sure that all nodes are within this list. */
    struct ecu_dlist_iterator iterator;
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );

    for (struct ecu_dnode *node = ecu_dlist_iterator_begin(&iterator, me);
         node != ecu_dlist_iterator_end(&iterator); 
         node = ecu_dlist_iterator_next(&iterator))
    {
        ecu_dlist_node_destroy(node);
    }

    /* Setting to NULL values forces user to reconstruct the list if they
    want to use it again, assuming asserts are enabled. */
    me->head.next    = (struct ecu_dnode *)0;
    me->head.prev    = (struct ecu_dnode *)0;
    me->head.destroy = &HEAD_DESTROY_CALLBACK;
    me->head.id      = ECU_OBJECT_ID_RESERVED;
}

void ecu_dlist_insert_before(struct ecu_dlist *me, struct ecu_dnode *position, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && position && node) );
    ECU_RUNTIME_ASSERT( (position != node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(position) && node_valid(node)) );
    ECU_RUNTIME_ASSERT( (node_in_list(position) && !node_in_list(node)) );

    node->next           = position;
    node->prev           = position->prev;
    position->prev->next = node;
    position->prev       = node;
}

void ecu_dlist_insert_after(struct ecu_dlist *me, struct ecu_dnode *position, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && position && node) );
    ECU_RUNTIME_ASSERT( (position != node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(position) && node_valid(node)) );
    ECU_RUNTIME_ASSERT( (node_in_list(position) && !node_in_list(node)) );
    ecu_dlist_insert_before(me, position->next, node);
}

void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );
    ecu_dlist_insert_after(me, &me->head, node);
}

void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)) );
    ecu_dlist_insert_before(me, &me->head, node);
}

void ecu_dlist_remove(struct ecu_dlist *me, struct ecu_dnode *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (list_valid(me)) );
    ECU_RUNTIME_ASSERT( (node_valid(node)) );
    ECU_RUNTIME_ASSERT( (node_in_list(node) && !node_is_valid_head(node)) );

    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next       = node;
    node->prev       = node;
    /* Do not reset destroy callback or ID. This is only a remove function,
    not a destroy function. */
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
