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

#define DNODE_CNULL \
    ((const struct ecu_dnode *)0)

#define NTNODE_NULL \
    ((struct ecu_ntnode *)0)

#define NTNODE_CNULL \
    ((const struct ecu_ntnode *)0)

#pragma message("TODO: Stuff decided on: \
    1. me->parent == me if no parent. \
    2. parent function returns null if no parent. \
    3. next functions return null if no siblings or is last sibling. \
    4. prev functions return null if no siblings or is first sibling. \
    5. Root is iterated over for preorder, postorder iterations. \
    6. No separate structure for tree/root. Just nodes.")

#pragma message("TODO: Stuff still being decided: \
    1. Dont know if clear and destroy functionality possible if nodes cant be removed in an iteration. \
        Maybe a breadth-first iterator makes this possible? Or some other iterator type?")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

// wrapper around dnode destroy.
static void invalid_destroy(struct ecu_ntnode *ntnode, ecu_object_id id);
static void ntnode_destroy(struct ecu_dnode *dnode, ecu_object_id id);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void invalid_destroy(struct ecu_ntnode *ntnode, ecu_object_id id)
{
    (void)ntnode;
    (void)id;
    ECU_RUNTIME_ASSERT( (false) );
}

static void ntnode_destroy(struct ecu_dnode *dnode, ecu_object_id id)
{
    ECU_RUNTIME_ASSERT( (dnode) );
    struct ecu_ntnode *me = ECU_DNODE_GET_ENTRY(dnode, struct ecu_ntnode, dnode);

    /* Reset internal values before user destructor called. */
    me->parent = NTNODE_NULL;
    // TODO!!! Destroy all children??? Think this would be recurisve if so!!!

    if (me->destroy != ECU_NTNODE_DESTROY_UNUSED)
    {
        (*me->destroy)(me, id);
    }
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

    ecu_dnode_ctor(&me->dnode, &ntnode_destroy, id);
    ecu_dlist_ctor(&me->children);
    me->parent = me;
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
    struct ecu_ntnode *parent = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (me->parent != me)
    {
        parent = me->parent;
    }

    return parent;
}

const struct ecu_ntnode *ecu_ntnode_cparent(const struct ecu_ntnode *me)
{
    const struct ecu_ntnode *parent = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (me->parent != me)
    {
        parent = me->parent;
    }

    return parent;
}

struct ecu_ntnode *ecu_ntnode_next(struct ecu_ntnode *me)
{
    struct ecu_ntnode_child_iterator iterator;
    struct ecu_ntnode *next = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* Use public API instead of accessing dnode.next directly to remain portable. */
        (void)ecu_ntnode_child_iterator_at(&iterator, &me->dnode);
        next = ecu_ntnode_child_iterator_next(&iterator);

        if (next == ecu_ntnode_child_iterator_end(&iterator)) /* me was last child? */
        {
            next = NTNODE_NULL;
        }
    }

    return next;
}

const struct ecu_ntnode *ecu_ntnode_cnext(const struct ecu_ntnode *me)
{
    struct ecu_ntnode_child_citerator citerator;
    const struct ecu_ntnode *next = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* Use public API instead of accessing dnode.next directly to remain portable. */
        (void)ecu_ntnode_child_iterator_cat(&citerator, &me->dnode);
        next = ecu_ntnode_child_iterator_cnext(&citerator);

        if (next == ecu_ntnode_child_iterator_cend(&citerator)) /* me was last child? */
        {
            next = NTNODE_CNULL;
        }
    }

    return next;
}

struct ecu_ntnode *ecu_ntnode_prev(struct ecu_ntnode *me)
{
    struct ecu_ntnode_child_iterator iterator;
    struct ecu_ntnode *first = NTNODE_NULL;
    struct ecu_ntnode *prev = NTNODE_NULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* Use public API instead of accessing dnode.prev directly to remain portable. Have 
        to treat as singly linked list since there are no reverse iterators at this time. */
        first = ecu_ntnode_child_iterator_begin(&iterator, &me->parent);
        if (first != me) /* me not first child? */
        {
            ECU_NTNODE_CHILD_FOR_EACH(n, &iterator, &me->parent)
            {
                if (ecu_ntnode_next(n) == me)
                {
                    prev = n;
                    break;
                }
            }

            ECU_RUNTIME_ASSERT( (prev) );
        }
    }

    return prev;
}

const struct ecu_ntnode *ecu_ntnode_cprev(const struct ecu_ntnode *me)
{
    struct ecu_ntnode_child_citerator citerator;
    const struct ecu_ntnode *first = NTNODE_CNULL;
    const struct ecu_ntnode *prev = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    if (ecu_dnode_in_list(&me->dnode))
    {
        /* Use public API instead of accessing dnode.prev directly to remain portable. Have 
        to treat as singly linked list since there are no reverse iterators at this time. */
        first = ecu_ntnode_child_iterator_cbegin(&citerator, &me->parent);
        if (first != me) /* me not first child? */
        {
            ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->parent)
            {
                if (ecu_ntnode_cnext(n) == me)
                {
                    prev = n;
                    break;
                }
            }

            ECU_RUNTIME_ASSERT( (prev) );
        }
    }

    return prev;
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
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(me)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(sibling)) );
    ecu_dnode_insert_before(&me->dnode, &sibling->dnode);
    me->parent = sibling->parent;
}

void ecu_ntnode_insert_after(struct ecu_ntnode *me, struct ecu_ntnode *sibling)
{
    ECU_RUNTIME_ASSERT( (me && sibling) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(me)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(sibling)) );
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
    me->parent = me;
}

bool ecu_ntnode_is_root(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return !ecu_dnode_in_list(&me->dnode);
}

bool ecu_ntnode_in_subtree(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dnode_in_list(&me->dnode);
}

ecu_object_id ecu_ntnode_get_id(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dnode_get_id(&me->dnode);
}

size_t ecu_ntnode_count(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dlist_size(&me->children);
}

size_t ecu_ntnode_level(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    size_t level = 0;
    const struct ecu_ntnode *parent = NTNODE_CNULL;

    for (parent = ecu_ntnode_cparent(me); parent; parent = ecu_ntnode_cparent(me))
    {
        level++;
    }

    return level;
}

size_t ecu_ntnode_size(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    size_t size = 0;
    struct ecu_ntnode_preorder_citerator citerator;

    ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citerator, me)
    {
        (void)n;
        size++;
    }

    return size;
}

struct ecu_ntnode *ecu_ntnode_find(struct ecu_ntnode *me, 
                                   bool (*found)(const struct ecu_ntnode *me, void *obj),
                                   void *obj)
{
    #warning "TODO"
}

const struct ecu_ntnode *ecu_ntnode_cfind(struct ecu_ntnode *me, 
                                          bool (*found)(const struct ecu_ntnode *me, void *obj),
                                          void *obj)
{
    #warning "TODO"
}

struct ecu_ntnode *ecu_ntnode_lca(struct ecu_ntnode *n1, struct ecu_ntnode *n2)
{
    #warning "TODO"
}

const struct ecu_ntnode *ecu_ntnode_clca(const struct ecu_ntnode *n1, const struct ecu_ntnode *n2)
{
    #warning "TODO"
}

/*------------------------------------------------------------*/
/*----------- NTNODE CHILD ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_child_iterator_begin(struct ecu_ntnode_child_iterator *me, 
                                                   struct ecu_ntnode *parent)
{
    struct ecu_dnode *dstart = DNODE_NULL;
    struct ecu_ntnode *ntstart = parent;
    ECU_RUNTIME_ASSERT( (me && parent) );

    if (ecu_ntnode_count(parent) > (size_t)0)
    {
        dstart = ecu_dlist_iterator_begin(&me->iterator, &parent->children);
        ntstart = ECU_DNODE_GET_ENTRY(dstart, struct ecu_ntnode, dnode);
    }
    
    return ntstart;
}

struct ecu_ntnode *ecu_ntnode_child_iterator_at(struct ecu_ntnode_child_iterator *me,
                                                struct ecu_ntnode *start)
{
    #warning "TODO"
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

const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_citerator *me, 
                                                          const struct ecu_ntnode *parent)
{
    const struct ecu_dnode *dstart = DNODE_CNULL;
    const struct ecu_ntnode *ntstart = parent;
    ECU_RUNTIME_ASSERT( (me && parent) );

    if (ecu_ntnode_count(parent) > (size_t)0)
    {
        dstart = ecu_dlist_iterator_cbegin(&me->iterator, &parent->children);
        ntstart = ECU_DNODE_GET_CONST_ENTRY(dstart, struct ecu_ntnode, dnode);
    }
    
    return ntstart;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cat(struct ecu_ntnode_child_citerator *me,
                                                       const struct ecu_ntnode *start)
{
    #warning "TODO"
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_citerator *me)
{
    const struct ecu_dnode *dend = DNODE_CNULL;
    const struct ecu_ntnode *ntend = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    dend = ecu_dlist_iterator_end(&me->iterator);
    ntend = ECU_DNODE_GET_CONST_ENTRY(dend, struct ecu_ntnode, children.head);
    return ntend;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cnext(struct ecu_ntnode_child_citerator *me)
{
    const struct ecu_dnode *dnext = DNODE_CNULL;
    const struct ecu_ntnode *ntnext = NTNODE_CNULL;
    ECU_RUNTIME_ASSERT( (me) );

    dnext = ecu_dlist_iterator_next(&me->iterator);
    if (dnext == ecu_dlist_iterator_end(&me->iterator))
    {
        ntnext = ECU_DNODE_GET_CONST_ENTRY(dnext, struct ecu_ntnode, children.head);
    }
    else
    {
        ntnext = ECU_DNODE_GET_CONST_ENTRY(dnext, struct ecu_ntnode, dnode);
    }

    return ntnext;
}

/*------------------------------------------------------------*/
/*-------- NTNODE PREORDER ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_preorder_iterator_begin(struct ecu_ntnode_preorder_iterator *me,
                                                      struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_RESERVED);
    me->root = root;
    me->current = root;
    return root;
}

struct ecu_ntnode *ecu_ntnode_preorder_iterator_end(struct ecu_ntnode_preorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (&me->delimiter);
}

struct ecu_ntnode *ecu_ntnode_preorder_iterator_next(struct ecu_ntnode_preorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current) );
    /* Continuing the iteration after removing a node is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->current) || me->current == me->root || me->current == &me->delimiter) );

    struct ecu_ntnode *child = ecu_ntnode_front(me->current);
    struct ecu_ntnode *sibling = ecu_ntnode_next(me->current);
    struct ecu_ntnode *parent = ecu_ntnode_parent(me->current);
    bool traversed_up = true;

    while (!child && !sibling && parent)
    {
        traversed_up = true;
        me->current = parent;
        child = ecu_ntnode_front(me->current);
        sibling = ecu_ntnode_next(me->current);
        parent = ecu_ntnode_parent(me->current);
    }

    if (!traversed_up && child)
    {
        me->current = child;
    }
    else if (sibling)
    {
        me->current = sibling;
    }
    else if (me->current == me->root)
    {
        /* Reached the end of the iteration. */
        me->current = &me->delimiter;
    }
    else
    {
        /* Iteration restarted after previously ending. */
        ECU_RUNTIME_ASSERT( (me->current == &me->delimiter) );
        me->current = me->root;
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cbegin(struct ecu_ntnode_preorder_citerator *me,
                                                             const struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_RESERVED);
    me->root = root;
    me->current = root;
    return root;
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cend(struct ecu_ntnode_preorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (&me->delimiter);
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cnext(struct ecu_ntnode_preorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current) );
    /* Continuing the iteration after removing a node is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->current) || me->current == me->root || me->current == &me->delimiter) );

    const struct ecu_ntnode *child = ecu_ntnode_cfront(me->current);
    const struct ecu_ntnode *sibling = ecu_ntnode_cnext(me->current);
    const struct ecu_ntnode *parent = ecu_ntnode_cparent(me->current);
    bool traversed_up = true;

    while (!child && !sibling && parent)
    {
        traversed_up = true;
        me->current = parent;
        child = ecu_ntnode_cfront(me->current);
        sibling = ecu_ntnode_cnext(me->current);
        parent = ecu_ntnode_cparent(me->current);
    }

    if (!traversed_up && child)
    {
        me->current = child;
    }
    else if (sibling)
    {
        me->current = sibling;
    }
    else if (me->current == me->root)
    {
        /* Reached the end of the iteration. */
        me->current = &me->delimiter;
    }
    else
    {
        /* Iteration restarted after previously ending. */
        ECU_RUNTIME_ASSERT( (me->current == &me->delimiter) );
        me->current = me->root;
    }

    return (me->current);
}
