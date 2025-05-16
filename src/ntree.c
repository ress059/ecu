/**
 * @file
 * @brief Generic n-ary tree. See
 * @rst
 * See :ref:`ntree.h section <ntree_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-15
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/ntree.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/ntree.c")

/*------------------------------------------------------------*/
/*---------------------------- DEFINES -----------------------*/
/*------------------------------------------------------------*/

#define DNODE_NULL \
    ((struct ecu_dnode *)0)

#define NTNODE_NULL \
    ((struct ecu_ntnode *)0)

#define NTNODE_CNULL \
    ((const struct ecu_ntnode *)0)

#define NTNODE_NO_PARENT \
    ((struct ecu_ntnode *)0)

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

// wrapper around dnode destroy.
static void ntnode_destroy(struct ecu_dnode *dnode, ecu_object_id id);

static bool ntnode_is_root(const struct ecu_ntnode *ntnode);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void ntnode_destroy(struct ecu_dnode *dnode, ecu_object_id id)
{
    ECU_RUNTIME_ASSERT( (dnode) );
    struct ecu_ntnode *me = ECU_DNODE_GET_ENTRY(dnode, struct ecu_ntnode, dnode);

    /* Reset internal values before user destructor called. */
    me->parent = NTNODE_NO_PARENT;
    // TODO!!! Destroy all children??? Think this would be recurisve if so!!!

    if (me->destroy != ECU_NTNODE_DESTROY_UNUSED)
    {
        (*me->destroy)(me, id);
    }
}

static bool ntnode_is_root(const struct ecu_ntnode *ntnode)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (ntnode) );

    if (ecu_dnode_get_id(&ntnode->dnode) == ECU_OBJECT_ID_RESERVED)
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*----------------- NTREE NODE MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

void ecu_ntnode_ctor(struct ecu_ntnode *me, 
                     void (*destroy)(struct ecu_ntnode *me, ecu_object_id id), 
                     ecu_object_id id)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (id >= ECU_VALID_OBJECT_ID_BEGIN) );

    ecu_dlist_ctor(&me->children);
    ecu_dnode_ctor(&me->dnode, &ntnode_destroy, id);
    me->parent = NTNODE_NO_PARENT;
    me->destroy = destroy;
}

void ecu_ntnode_destroy(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    // TODO!!! If I want to just call ecu_dnode_destroy(me) then 
    // all children must be destroyed in ntnode_destroy() callback and this 
    //  would cause recusion. Otherwise we can iterate over entire tree once 
    // iterators are made! Like the 2nd option better!
    // ecu_dnode_destroy(me);
}

struct ecu_ntnode *ecu_ntnode_parent(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->parent);
}

const struct ecu_ntnode *ecu_ntnode_cparent(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->parent);
}

struct ecu_ntnode *ecu_ntnode_next(struct ecu_ntnode *me)
{
    struct ecu_dnode *dnext = DNODE_NULL;
    struct ecu_ntnode *ntnext = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* WARNING: Accesses dnode member directly instead of using 
        public API since there is no API to get next and prev. */
        dnext = me->dnode.next;
        ntnext = ECU_DNODE_GET_ENTRY(dnext, struct ecu_ntnode, dnode);
    }

    return ntnext;
}

const struct ecu_ntnode *ecu_ntnode_cnext(const struct ecu_ntnode *me)
{
    struct ecu_dnode *dnext = DNODE_NULL;
    const struct ecu_ntnode *ntnext = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* WARNING: Accesses dnode member directly instead of using 
        public API since there is no API to get next and prev. */
        dnext = me->dnode.next;
        ntnext = ECU_DNODE_GET_CONST_ENTRY(dnext, struct ecu_ntnode, dnode);
    }

    return ntnext;
}

struct ecu_ntnode *ecu_ntnode_prev(struct ecu_ntnode *me)
{
    struct ecu_dnode *dprev = DNODE_NULL;
    const struct ecu_ntnode *ntprev = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* WARNING: Accesses dnode member directly instead of using 
        public API since there is no API to get next and prev. */
        dprev = me->dnode.prev;
        ntprev = ECU_DNODE_GET_ENTRY(dprev, struct ecu_ntnode, dnode);
    }

    return ntprev;
}

const struct ecu_ntnode *ecu_ntnode_cprev(const struct ecu_ntnode *me)
{
    struct ecu_dnode *dprev = DNODE_NULL;
    const struct ecu_ntnode *ntprev = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* WARNING: Accesses dnode member directly instead of using 
        public API since there is no API to get next and prev. */
        dprev = me->dnode.prev;
        ntprev = ECU_DNODE_GET_CONST_ENTRY(dprev, struct ecu_ntnode, dnode);
    }

    return ntprev;
}

struct ecu_ntnode *ecu_ntnode_front(struct ecu_ntnode *me)
{
    struct ecu_dnode *dfront = (struct ecu_dnode *)0;
    struct ecu_ntnode *ntfront = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    dfront = ecu_dlist_front(&me->children);
    if (dfront)
    {
        ntfront = ECU_DNODE_GET_ENTRY(dfront, struct ecu_ntnode, dnode);
    }

    return ntfront;
}

const struct ecu_ntnode *ecu_ntnode_cfront(const struct ecu_ntnode *me)
{
    const struct ecu_dnode *dfront = (struct ecu_dnode *)0;
    const struct ecu_ntnode *ntfront = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    dfront = ecu_dlist_cfront(&me->children);
    if (dfront)
    {
        ntfront = ECU_DNODE_GET_CONST_ENTRY(dfront, struct ecu_ntnode, dnode);
    }

    return ntfront;
}

struct ecu_ntnode *ecu_ntnode_back(struct ecu_ntnode *me)
{
    struct ecu_dnode *dback = (struct ecu_dnode *)0;
    struct ecu_ntnode *ntback = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    dback = ecu_dlist_back(&me->children);
    if (dback)
    {
        ntback = ECU_DNODE_GET_ENTRY(dback, struct ecu_ntnode, dnode);
    }

    return ntback;
}

const struct ecu_ntnode *ecu_ntnode_cback(const struct ecu_ntnode *me)
{
    const struct ecu_dnode *dback = (struct ecu_dnode *)0;
    const struct ecu_ntnode *ntback = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    dback = ecu_dlist_cback(&me->children);
    if (dback)
    {
        ntback = ECU_DNODE_GET_CONST_ENTRY(dback, struct ecu_ntnode, dnode);
    }

    return ntback;
}

void ecu_ntnode_insert_before(struct ecu_ntnode *me, struct ecu_ntnode *sibling)
{
    ECU_RUNTIME_ASSERT( (me && sibling) );
    ecu_dnode_insert_before(&me->dnode, &sibling->dnode);
    me->parent = sibling->parent;
}

void ecu_ntnode_insert_after(struct ecu_ntnode *me, struct ecu_ntnode *sibling)
{
    ECU_RUNTIME_ASSERT( (me && sibling) );
    ecu_dnode_insert_after(&me->dnode, &sibling->dnode);
    me->parent = sibling->parent;
}

void ecu_ntnode_push_front(struct ecu_ntnode *me, struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (me && child) );
    ecu_dlist_push_front(&me->children, &child->dnode);
    child->parent = me;
}

void ecu_ntnode_push_back(struct ecu_ntnode *me, struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (me && child) );
    ecu_dlist_push_back(&me->children, &child->dnode);
    child->parent = me;
}

void ecu_ntnode_remove(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ecu_dnode_remove(&me->dnode);
    me->parent = NTNODE_NO_PARENT;
}

struct ecu_ntnode *ecu_ntnode_lca(struct ecu_ntnode *n1, struct ecu_ntnode *n2)
{

}

bool ecu_ntnode_in_tree(const struct ecu_ntnode *me)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (me) );

    if ((ntnode_is_root(me)) || 
        (ecu_dnode_in_list(&me->dnode) && me->parent != NTNODE_NO_PARENT))
    {
        status = true;
    }

    return status;
}

ecu_object_id ecu_ntnode_get_id(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dnode_get_id(&me->dnode);
}

size_t ecu_ntnode_size(const struct ecu_ntnode *me)
{

}

size_t ecu_ntnode_count(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dlist_size(&me->children);
}

/*------------------------------------------------------------*/
/*-------------------- NTREE MEMBER FUNCTIONS ----------------*/
/*------------------------------------------------------------*/

void ecu_ntree_ctor(struct ecu_ntree *me)
{

}

void ecu_ntree_destroy(struct ecu_ntree *me)
{

}

void ecu_ntree_clear(struct ecu_ntree *me)
{

}

struct ecu_ntnode *ecu_ntree_find(ecu_object_id id)
{

}

void ecu_ntree_push_front(struct ecu_ntree *me, struct ecu_ntnode *node)
{

}

void ecu_ntree_push_back(struct ecu_ntree *me, struct ecu_ntnode *node)
{

}

void ecu_ntree_insert_front(struct ecu_ntree *me, struct ecu_ntnode *node, size_t level)
{

}

void ecu_ntree_insert_back(struct ecu_ntree *me, struct ecu_ntnode *node, size_t level)
{

}

bool ecu_ntree_empty(const struct ecu_ntree *me)
{

}

size_t ecu_ntree_depth(const struct ecu_ntree *me)
{

}

size_t ecu_ntree_size(const struct ecu_ntree *me)
{

}

/*------------------------------------------------------------*/
/*----------- NTNODE CHILD ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_child_iterator_begin(struct ecu_ntnode_child_iterator *me, 
                                                   struct ecu_ntnode *node)
{
    struct ecu_dnode *dstart = DNODE_NULL;
    struct ecu_ntnode *ntstart = node;
    ECU_RUNTIME_ASSERT( (me && node) );

    if (ecu_ntnode_count(node) > 0U)
    {
        dstart = ecu_dlist_iterator_begin(&me->iterator, &node->children);
        ntstart = ECU_DNODE_GET_ENTRY(dstart, struct ecu_ntnode, dnode);
    }
    
    return ntstart;
}

struct ecu_ntnode *ecu_ntnode_child_iterator_end(struct ecu_ntnode_child_iterator *me)
{
    struct ecu_dnode *dend = DNODE_NULL;
    struct ecu_ntnode *ntend = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    dend = ecu_dlist_iterator_end(&me->iterator);
    ntend = ECU_DNODE_GET_ENTRY(dend, struct ecu_ntnode, children.head);
    return ntend;
}

struct ecu_ntnode *ecu_ntnode_child_iterator_next(struct ecu_ntnode_child_iterator *me)
{
    struct ecu_dnode *dnext = DNODE_NULL;
    struct ecu_ntnode *ntnext = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    dnext = ecu_dlist_iterator_next(&me->iterator);
    if (dnext == ecu_dlist_iterator_end(&me->iterator))
    {
        ntnext = ECU_DNODE_GET_ENTRY(dnext, struct ecu_ntnode, children.head);
    }
    else
    {
        ntnext = ECU_DNODE_GET_ENTRY(dnext, struct ecu_ntnode, dnode);
    }

    return ntnext;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_iterator *me, 
                                                          const struct ecu_ntnode *node)
{

}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_iterator *me)
{

}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cnext(struct ecu_ntnode_child_iterator *me)
{

}
