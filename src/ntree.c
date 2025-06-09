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

#pragma message("TODO: \
    1. Make sibling iterator a separate entity. do not try to wrap child iterator in it. \
    2. Sibling iterator starts at sibling and goes through all siblings. End == start sibling or maybe delimieter. \
    3. Probably get rid of child_iterator_at() if sibling iterator made.")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/// warning!! dont use for ecu_ntnode_is_root() or ecu_ntnode_in_subtree(). 
static bool node_valid(const struct ecu_ntnode *node);
static void invalid_destroy(struct ecu_ntnode *ntnode, ecu_object_id id);

// wrapper around dnode destroy.
static void dnode_destroy(struct ecu_dnode *dnode, ecu_object_id id);
static bool is_parent_of(const struct ecu_ntnode *parent, const struct ecu_ntnode *child);
static struct ecu_ntnode *get_leaf(struct ecu_ntnode *ntnode);
static const struct ecu_ntnode *get_cleaf(const struct ecu_ntnode *ntnode);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool node_valid(const struct ecu_ntnode *node)
{
    ECU_RUNTIME_ASSERT( (node) );
    bool status = false;

    if ((node->parent) && 
        (ecu_ntnode_in_subtree(node) || ecu_ntnode_is_root(node)))
    {
        status = true;
    }

    return status;
}

static void invalid_destroy(struct ecu_ntnode *ntnode, ecu_object_id id)
{
    (void)ntnode;
    (void)id;
    ECU_RUNTIME_ASSERT( (false) );
}

static void dnode_destroy(struct ecu_dnode *dnode, ecu_object_id id)
{
    ECU_RUNTIME_ASSERT( (dnode) );
    struct ecu_ntnode *me = ECU_DNODE_GET_ENTRY(dnode, struct ecu_ntnode, dnode);
    /* Do not assert node_valid() since internal values will already be reset to NULL. */

    /* Call user's ntnode destroy callback if one was supplied. Otherwise no additional cleanup. */
    if (me->destroy != ECU_NTNODE_DESTROY_UNUSED)
    {
        (*me->destroy)(me, id);
    }
}

static bool is_parent_of(const struct ecu_ntnode *parent, const struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (parent && child) );
    /* node_valid(child) done in for-loop. */
    ECU_RUNTIME_ASSERT( (node_valid(parent)) );
    bool status = false;

    /* Notice how this also handles case where n1 == n2. */
    for (const struct ecu_ntnode *n = child; n; n = ecu_ntnode_cparent(n))
    {
        ECU_RUNTIME_ASSERT( (node_valid(n)) );
        if (n == parent)
        {
            status = true;
            break;
        }
    }

    return status;
}

static struct ecu_ntnode *get_leaf(struct ecu_ntnode *ntnode)
{
    ECU_RUNTIME_ASSERT( (ntnode) );
    ECU_RUNTIME_ASSERT( (node_valid(ntnode)) );
    struct ecu_ntnode *leaf = ntnode;
    struct ecu_ntnode *child = ecu_ntnode_front(leaf);

    /* API asserts returned node is valid (if non-NULL) so do not assert node_valid(). */
    while (child)
    {
        leaf = child;
        child = ecu_ntnode_front(child);
    }

    return leaf;
}

static const struct ecu_ntnode *get_cleaf(const struct ecu_ntnode *ntnode)
{
    ECU_RUNTIME_ASSERT( (ntnode) );
    ECU_RUNTIME_ASSERT( (node_valid(ntnode)) );
    const struct ecu_ntnode *leaf = ntnode;
    const struct ecu_ntnode *child = ecu_ntnode_cfront(leaf);

    /* API asserts returned node is valid (if non-NULL) so do not assert node_valid(). */
    while (child)
    {
        leaf = child;
        child = ecu_ntnode_cfront(child);
    }

    return leaf;
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

    ecu_dnode_ctor(&me->dnode, &dnode_destroy, id);
    ecu_dlist_ctor(&me->children);
    me->parent = me;
    me->destroy = destroy;
}

void ecu_ntnode_destroy(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    struct ecu_ntnode_postorder_iterator iterator;

    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iterator, me)
    {
        /* Must be postorder so nodes can be safely removed in the middle of 
        an iteration. Iterator already asserts if returned nodes are valid
        (including me) so do not assert node_valid().
        Destroy objects by setting to NULL values. Forces user to reconstruct
        nodes if they want to use them again, assuming asserts are enabled.
        IMPORTANT: These values are reset before the destroy callback in case
        the user frees their entire node (including ecu_ntnode). Otherwise 
        we would be accessing and writing to freed memory. */
        n->parent = NTNODE_NULL;
        ecu_dnode_destroy(&n->dnode);
    }
}

struct ecu_ntnode *ecu_ntnode_parent(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_ntnode *parent = NTNODE_NULL;
    
    if (me->parent != me)
    {
        parent = me->parent;
        ECU_RUNTIME_ASSERT( (node_valid(parent)) );
    }

    return parent;
}

const struct ecu_ntnode *ecu_ntnode_cparent(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    const struct ecu_ntnode *parent = NTNODE_CNULL;

    if (me->parent != me)
    {
        parent = me->parent;
        ECU_RUNTIME_ASSERT( (node_valid(parent)) );
    }

    return parent;
}

struct ecu_ntnode *ecu_ntnode_next(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_ntnode_child_iterator iterator;
    struct ecu_ntnode *next = NTNODE_NULL;

    if (ecu_ntnode_in_subtree(me))
    {
        /* Use public API instead of accessing dnode.next directly to remain portable. */
        (void)ecu_ntnode_child_iterator_at(&iterator, me);
        next = ecu_ntnode_child_iterator_next(&iterator);
        /* Iterator asserts returned node is valid so do not assert. */

        if (next == ecu_ntnode_child_iterator_end(&iterator)) /* me was last child? */
        {
            next = NTNODE_NULL;
        }
    }

    return next;
}

const struct ecu_ntnode *ecu_ntnode_cnext(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_ntnode_child_citerator citerator;
    const struct ecu_ntnode *next = NTNODE_CNULL;

    /* Use public API instead of accessing dnode.next directly to remain portable. */
    if (ecu_ntnode_in_subtree(me))
    {
        (void)ecu_ntnode_child_iterator_cat(&citerator, me);
        next = ecu_ntnode_child_iterator_cnext(&citerator);
        /* Iterator asserts returned node is valid so do not assert. */

        if (next == ecu_ntnode_child_iterator_cend(&citerator)) /* me was last child? */
        {
            next = NTNODE_CNULL;
        }
    }

    return next;
}

struct ecu_ntnode *ecu_ntnode_prev(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_ntnode_child_iterator iterator;
    struct ecu_ntnode *first = NTNODE_NULL; /* First child. */
    struct ecu_ntnode *prev = NTNODE_NULL;

    /* Use public API instead of accessing dnode.prev directly to remain portable. Have 
    to treat as singly linked list since there are no reverse iterators at this time. */
    first = ecu_ntnode_front(me->parent);
    if (first && first != me) /* me has siblings and me not first child? */
    {
        ECU_NTNODE_CHILD_FOR_EACH(n, &iterator, me->parent)
        {
            /* Iterator asserts returned node is valid so do not assert. */
            if (ecu_ntnode_next(n) == me)
            {
                prev = n;
                break;
            }
        }

        ECU_RUNTIME_ASSERT( (prev) );
    }

    return prev;
}

const struct ecu_ntnode *ecu_ntnode_cprev(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_ntnode_child_citerator citerator;
    const struct ecu_ntnode *first = NTNODE_CNULL; /* First child. */
    const struct ecu_ntnode *prev = NTNODE_CNULL;

    /* Use public API instead of accessing dnode.prev directly to remain portable. Have 
    to treat as singly linked list since there are no reverse iterators at this time. */
    first = ecu_ntnode_cfront(me->parent);
    if (first && first != me) /* me has siblings and me not first child? */
    {
        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, me->parent)
        {
            /* Iterator asserts returned node is valid so do not assert. */
            if (ecu_ntnode_cnext(n) == me)
            {
                prev = n;
                break;
            }
        }

        ECU_RUNTIME_ASSERT( (prev) );
    }

    return prev;
}

struct ecu_ntnode *ecu_ntnode_front(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_dnode *dfront = (struct ecu_dnode *)0;
    struct ecu_ntnode *ntfront = NTNODE_NULL;

    dfront = ecu_dlist_front(&me->children);
    if (dfront)
    {
        ntfront = ECU_DNODE_GET_ENTRY(dfront, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (node_valid(ntfront)) );
    }

    return ntfront;
}

const struct ecu_ntnode *ecu_ntnode_cfront(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    const struct ecu_dnode *dfront = (struct ecu_dnode *)0;
    const struct ecu_ntnode *ntfront = NTNODE_CNULL;

    dfront = ecu_dlist_cfront(&me->children);
    if (dfront)
    {
        ntfront = ECU_DNODE_GET_CONST_ENTRY(dfront, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (node_valid(ntfront)) );
    }

    return ntfront;
}

struct ecu_ntnode *ecu_ntnode_back(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    struct ecu_dnode *dback = (struct ecu_dnode *)0;
    struct ecu_ntnode *ntback = NTNODE_NULL;

    dback = ecu_dlist_back(&me->children);
    if (dback)
    {
        ntback = ECU_DNODE_GET_ENTRY(dback, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (node_valid(ntback)) );
    }

    return ntback;
}

const struct ecu_ntnode *ecu_ntnode_cback(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    const struct ecu_dnode *dback = (struct ecu_dnode *)0;
    const struct ecu_ntnode *ntback = NTNODE_CNULL;

    dback = ecu_dlist_cback(&me->children);
    if (dback)
    {
        ntback = ECU_DNODE_GET_CONST_ENTRY(dback, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (node_valid(ntback)) );
    }

    return ntback;
}

void ecu_ntnode_insert_before(struct ecu_ntnode *pos, struct ecu_ntnode *sibling)
{
    ECU_RUNTIME_ASSERT( (pos && sibling) );
    ECU_RUNTIME_ASSERT( (pos != sibling) );
    ECU_RUNTIME_ASSERT( (node_valid(pos)) );
    ECU_RUNTIME_ASSERT( (node_valid(sibling)) );
    /* Tree must have a root. Cannot add sibling to a root. 
    Sibling also cannot already be within a tree. */
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(pos)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(sibling)) );

    ecu_dnode_insert_before(&pos->dnode, &sibling->dnode);
    sibling->parent = pos->parent;
}

void ecu_ntnode_insert_after(struct ecu_ntnode *pos, struct ecu_ntnode *sibling)
{
    ECU_RUNTIME_ASSERT( (pos && sibling) );
    ECU_RUNTIME_ASSERT( (pos != sibling) );
    ECU_RUNTIME_ASSERT( (node_valid(pos) && node_valid(sibling)) );
    /* Tree must have a root. Cannot add sibling to a root. 
    Sibling also cannot already be within a tree. */
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(pos)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(sibling)) );

    ecu_dnode_insert_after(&pos->dnode, &sibling->dnode);
    sibling->parent = pos->parent;
}

void ecu_ntnode_push_front(struct ecu_ntnode *parent, struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (parent && child) );
    ECU_RUNTIME_ASSERT( (parent != child) );
    ECU_RUNTIME_ASSERT( (node_valid(parent) && node_valid(child)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(child)) );

    ecu_dlist_push_front(&parent->children, &child->dnode);
    child->parent = parent;
}

void ecu_ntnode_push_back(struct ecu_ntnode *parent, struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (parent && child) );
    ECU_RUNTIME_ASSERT( (parent != child) );
    ECU_RUNTIME_ASSERT( (node_valid(parent) && node_valid(child)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(child)) );

    ecu_dlist_push_back(&parent->children, &child->dnode);
    child->parent = parent;
}

void ecu_ntnode_remove(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    ecu_dnode_remove(&me->dnode);
    me->parent = me;
}

void ecu_ntnode_clear(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    struct ecu_ntnode_postorder_iterator iterator;

    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iterator, me)
    {
        /* Must be postorder so nodes can be safely removed in the middle of 
        an iteration. Iterator already asserts if returned nodes are valid
        (including me) so do not assert node_valid(). */
        ecu_ntnode_remove(n);
    }
}

bool ecu_ntnode_is_root(const struct ecu_ntnode *me)
{
    /* WARNING: Do NOT assert node_valid() since this function is used in node_valid(). */
    ECU_RUNTIME_ASSERT( (me) );
    return (!ecu_dnode_in_list(&me->dnode));
}

bool ecu_ntnode_in_subtree(const struct ecu_ntnode *me)
{
    /* WARNING: Do NOT assert node_valid() since this function is used in node_valid(). */
    ECU_RUNTIME_ASSERT( (me) );
    return (ecu_dnode_in_list(&me->dnode));
}

ecu_object_id ecu_ntnode_get_id(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    return (ecu_dnode_get_id(&me->dnode));
}

size_t ecu_ntnode_count(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    return (ecu_dlist_size(&me->children));
}

size_t ecu_ntnode_level(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me)) );
    size_t level = 0;

    for (const struct ecu_ntnode *parent = ecu_ntnode_cparent(me); 
         parent; 
         parent = ecu_ntnode_cparent(me))
    {
        ECU_RUNTIME_ASSERT( (node_valid(parent)) );
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
        /* Iterator already asserts if returned nodes are valid (including me). */
        (void)n;
        size++;
    }

    ECU_RUNTIME_ASSERT( (size > 0) ); /* Iteration should have gone at least over me. */
    return (size - 1);
}

struct ecu_ntnode *ecu_ntnode_find(struct ecu_ntnode *me, 
                                   bool (*found)(const struct ecu_ntnode *me, void *obj),
                                   void *obj)
{
    ECU_RUNTIME_ASSERT( (me && found) );
    struct ecu_ntnode_preorder_iterator iterator;
    struct ecu_ntnode *target = NTNODE_NULL;

    ECU_NTNODE_PREORDER_FOR_EACH(n, &iterator, me)
    {
        /* Iterator already asserts if returned nodes are valid (including me). */
        if ((*found)(n, obj))
        {
            target = n;
            break;
        }
    }

    return target;
}

const struct ecu_ntnode *ecu_ntnode_cfind(struct ecu_ntnode *me, 
                                          bool (*found)(const struct ecu_ntnode *me, void *obj),
                                          void *obj)
{
    ECU_RUNTIME_ASSERT( (me && found) );
    struct ecu_ntnode_preorder_citerator citerator;
    const struct ecu_ntnode *target = NTNODE_CNULL;

    ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citerator, me)
    {
        /* Iterator already asserts if returned nodes are valid (including me). */
        if ((*found)(n, obj))
        {
            target = n;
            break;
        }
    }

    return target;
}

struct ecu_ntnode *ecu_ntnode_lca(struct ecu_ntnode *n1, struct ecu_ntnode *n2)
{
    ECU_RUNTIME_ASSERT( (n1 && n2) );
    /* node_valid(n1) done in for-loop. */
    ECU_RUNTIME_ASSERT( (node_valid(n2)) );
    struct ecu_ntnode *lca = NTNODE_NULL;

    for (struct ecu_ntnode *n = n1; n; n = ecu_ntnode_parent(n))
    {
        ECU_RUNTIME_ASSERT( (node_valid(n)) );
        if (n == n2 || is_parent_of(n, n2))
        {
            lca = n;
            break;
        }
    }

    return lca;
}

const struct ecu_ntnode *ecu_ntnode_clca(const struct ecu_ntnode *n1, const struct ecu_ntnode *n2)
{
    ECU_RUNTIME_ASSERT( (n1 && n2) );
    /* node_valid(n1) done in for-loop. */
    ECU_RUNTIME_ASSERT( (node_valid(n2)) );
    const struct ecu_ntnode *lca = NTNODE_CNULL;

    for (const struct ecu_ntnode *n = n1; n; n = ecu_ntnode_cparent(n))
    {
        ECU_RUNTIME_ASSERT( (node_valid(n)) );
        if (n == n2 || is_parent_of(n, n2))
        {
            lca = n;
            break;
        }
    }

    return lca;
}

/*------------------------------------------------------------*/
/*----------- NTNODE CHILD ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_child_iterator_begin(struct ecu_ntnode_child_iterator *me, 
                                                   struct ecu_ntnode *parent)
{
    ECU_RUNTIME_ASSERT( (me && parent) );
    ECU_RUNTIME_ASSERT( (node_valid(parent)) );
    struct ecu_dnode *dstart = DNODE_NULL;
    struct ecu_ntnode *ntstart = parent;

    dstart = ecu_dlist_iterator_begin(&me->iterator, &parent->children);
    if (dstart != ecu_dlist_iterator_end(&me->iterator))
    {
        ntstart = ECU_DNODE_GET_ENTRY(dstart, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (node_valid(ntstart)) );
    }
    
    return ntstart;
}

#pragma message("TODO: Will most likely delete")
struct ecu_ntnode *ecu_ntnode_child_iterator_at(struct ecu_ntnode_child_iterator *me,
                                                struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (node_valid(start)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(start)) );

    (void)ecu_dlist_iterator_at(&me->iterator, &start->parent->children, &start->dnode);
    return start;
}

struct ecu_ntnode *ecu_ntnode_child_iterator_end(struct ecu_ntnode_child_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    struct ecu_dnode *dend = DNODE_NULL;
    struct ecu_ntnode *ntend = NTNODE_NULL;

    dend = ecu_dlist_iterator_end(&me->iterator);
    ntend = ECU_DNODE_GET_ENTRY(dend, struct ecu_ntnode, children.head);
    ECU_RUNTIME_ASSERT( (node_valid(ntend)) );
    return ntend;
}

struct ecu_ntnode *ecu_ntnode_child_iterator_next(struct ecu_ntnode_child_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    struct ecu_dnode *dnext = DNODE_NULL;
    struct ecu_ntnode *ntnext = NTNODE_NULL;

    dnext = ecu_dlist_iterator_next(&me->iterator);
    if (dnext == ecu_dlist_iterator_end(&me->iterator))
    {
        ntnext = ECU_DNODE_GET_ENTRY(dnext, struct ecu_ntnode, children.head);
    }
    else
    {
        ntnext = ECU_DNODE_GET_ENTRY(dnext, struct ecu_ntnode, dnode);
    }

    ECU_RUNTIME_ASSERT( (node_valid(ntnext)) );
    return ntnext;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_citerator *me, 
                                                          const struct ecu_ntnode *parent)
{
    ECU_RUNTIME_ASSERT( (me && parent) );
    ECU_RUNTIME_ASSERT( (node_valid(parent)) );
    const struct ecu_dnode *dstart = DNODE_CNULL;
    const struct ecu_ntnode *ntstart = parent;

    dstart = ecu_dlist_iterator_cbegin(&me->iterator, &parent->children);
    if (dstart != ecu_dlist_iterator_cend(&me->iterator))
    {
        ntstart = ECU_DNODE_GET_CONST_ENTRY(dstart, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (node_valid(ntstart)) );
    }
    
    return ntstart;
}

#pragma message("TODO: Will most likely delete")
const struct ecu_ntnode *ecu_ntnode_child_iterator_cat(struct ecu_ntnode_child_citerator *me,
                                                       const struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (node_valid(start)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(start)) );

    (void)ecu_dlist_iterator_cat(&me->iterator, &start->parent->children, &start->dnode);
    return start;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    const struct ecu_dnode *dend = DNODE_CNULL;
    const struct ecu_ntnode *ntend = NTNODE_CNULL;

    dend = ecu_dlist_iterator_cend(&me->iterator);
    ntend = ECU_DNODE_GET_CONST_ENTRY(dend, struct ecu_ntnode, children.head);
    ECU_RUNTIME_ASSERT( (node_valid(ntend)) );
    return ntend;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cnext(struct ecu_ntnode_child_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    const struct ecu_dnode *dnext = DNODE_CNULL;
    const struct ecu_ntnode *ntnext = NTNODE_CNULL;

    dnext = ecu_dlist_iterator_cnext(&me->iterator);
    if (dnext == ecu_dlist_iterator_cend(&me->iterator))
    {
        ntnext = ECU_DNODE_GET_CONST_ENTRY(dnext, struct ecu_ntnode, children.head);
    }
    else
    {
        ntnext = ECU_DNODE_GET_CONST_ENTRY(dnext, struct ecu_ntnode, dnode);
    }

    ECU_RUNTIME_ASSERT( (node_valid(ntnext)) );
    return ntnext;
}

/*------------------------------------------------------------*/
/*--------- NTNODE PARENT ITERATOR MEMBER FUNCTIONS ----------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_parent_iterator_begin(struct ecu_ntnode_parent_iterator *me,
                                                    struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (node_valid(start)) );
    me->start = start;
    me->current = ecu_ntnode_parent(start);

    if (!me->current)
    {
        me->current = start;
        me->next = start;
    }
    else
    {
        me->next = ecu_ntnode_parent(me->current);
        if (!me->next)
        {
            me->next = start;
        }
    }

    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_parent_iterator_end(struct ecu_ntnode_parent_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me->start)) );
    return (me->start);
}

struct ecu_ntnode *ecu_ntnode_parent_iterator_next(struct ecu_ntnode_parent_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->start && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (node_valid(me->start) && node_valid(me->current) && node_valid(me->next)) );
    /* The current node can be safely removed but removing the next node is not allowed. This is
    not asserted since it is impossible to detect this condition. The next node can be a root, and
    it is unknown what the root is at the start of the iteration. */

    me->current = me->next;
    me->next = ecu_ntnode_parent(me->next);

    if (!me->next)
    {
        me->next = me->start;
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cbegin(struct ecu_ntnode_parent_citerator *me,
                                                           const struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (node_valid(start)) );
    me->start = start;
    me->current = ecu_ntnode_cparent(start);

    if (!me->current)
    {
        me->current = start;
        me->next = start;
    }
    else
    {
        me->next = ecu_ntnode_cparent(me->current);
        if (!me->next)
        {
            me->next = start;
        }
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cend(struct ecu_ntnode_parent_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(me->start)) );
    return (me->start);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cnext(struct ecu_ntnode_parent_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->start && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (node_valid(me->start) && node_valid(me->current) && node_valid(me->next)) );
    /* No nodes can be removed since this is a const iteration. However this is not asserted
    since it is impossible to detect this condition. The next node can be a root, and
    it is unknown what the root is at the start of the iteration. */

    me->current = me->next;
    me->next = ecu_ntnode_cparent(me->next);

    if (!me->next)
    {
        me->next = me->start;
    }

    return (me->current);
}

/*------------------------------------------------------------*/
/*--------- NTNODE SIBLING ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_sibling_iterator_begin(struct ecu_ntnode_sibling_iterator *me,
                                                     struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (node_valid(start)) );
    me->current = ecu_ntnode_next(start);

    if (!me->current)
    {
        if (ecu_ntnode_is_root(start))
        {
            me->current = start;
            me->next = start;
        }
        else
        {
            me->current = ecu_ntnode_front(ecu_ntnode_parent(start)); /* Wraparound. Get first sibling. */
            ECU_RUNTIME_ASSERT( (me->current) );
            me->next = ecu_ntnode_next(me->current);

            if (!me->next)
            {
                me->next = start;
                ECU_RUNTIME_ASSERT( (me->current == start) );
            }
        }
    }
    else
    {
        me->next = ecu_ntnode_next(me->current);
        if (!me->next)
        {
            me->next = ecu_ntnode_front(ecu_ntnode_parent(me->current)); /* Wraparound. Get first sibling. */
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_sibling_iterator_end(struct ecu_ntnode_sibling_iterator *me)
{

}

struct ecu_ntnode *ecu_ntnode_sibling_iterator_next(struct ecu_ntnode_sibling_iterator *me)
{

}

const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cbegin(struct ecu_ntnode_sibling_citerator *me,
                                                            const struct ecu_ntnode *start)
{

}

const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cend(struct ecu_ntnode_sibling_citerator *me)
{

}

const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cnext(struct ecu_ntnode_sibling_citerator *me)
{

}

/*------------------------------------------------------------*/
/*-------- NTNODE PREORDER ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_preorder_iterator_begin(struct ecu_ntnode_preorder_iterator *me,
                                                      struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ECU_RUNTIME_ASSERT( (node_valid(root)) );
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_RESERVED);
    me->root = root;
    me->current = root;
    return root;
}

struct ecu_ntnode *ecu_ntnode_preorder_iterator_end(struct ecu_ntnode_preorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter)) );
    return (&me->delimiter);
}

struct ecu_ntnode *ecu_ntnode_preorder_iterator_next(struct ecu_ntnode_preorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter) && node_valid(me->root) && node_valid(me->current)) );
    /* Continuing the iteration after removing a node is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->current) || me->current == me->root || me->current == &me->delimiter) );

    struct ecu_ntnode *child = ecu_ntnode_front(me->current);
    struct ecu_ntnode *sibling = ecu_ntnode_next(me->current);
    struct ecu_ntnode *parent = ecu_ntnode_parent(me->current);
    bool traversed_up = true;

    /* API functions assert returned node is valid (if non-NULL), 
    so do not assert node_valid(child, sibling, parent). */
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
    ECU_RUNTIME_ASSERT( (node_valid(root)) );
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_RESERVED);
    me->root = root;
    me->current = root;
    return root;
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cend(struct ecu_ntnode_preorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter)) );
    return (&me->delimiter);
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cnext(struct ecu_ntnode_preorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter) && node_valid(me->root) && node_valid(me->current)) );
    /* Continuing the iteration after removing a node is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->current) || me->current == me->root || me->current == &me->delimiter) );

    const struct ecu_ntnode *child = ecu_ntnode_cfront(me->current);
    const struct ecu_ntnode *sibling = ecu_ntnode_cnext(me->current);
    const struct ecu_ntnode *parent = ecu_ntnode_cparent(me->current);
    bool traversed_up = true;

    /* API functions assert returned node is valid (if non-NULL), 
    so do not assert node_valid(child, sibling, parent). */
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

/*------------------------------------------------------------*/
/*-------- NTNODE POSTORDER ITERATOR MEMBER FUNCTIONS --------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_postorder_iterator_begin(struct ecu_ntnode_postorder_iterator *me,
                                                       struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ECU_RUNTIME_ASSERT( (node_valid(root)) );
    struct ecu_ntnode *sibling = NTNODE_NULL;

    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_RESERVED);
    me->root = root;
    me->current = get_leaf(root); /* Asserts returned node is valid since there is always a leaf. */
    sibling = ecu_ntnode_next(me->current); /* Asserts returned node is valid if non-NULL. */

    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert node_valid(me->next). */
    if (sibling)
    {
        me->next = get_leaf(sibling);
    }
    else if (!sibling && me->current != root)
    {
        me->next = ecu_ntnode_parent(me->current);
    }
    else
    {
        me->next = &me->delimiter;
    }

    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_postorder_iterator_end(struct ecu_ntnode_postorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter)) );
    return (&me->delimiter);
}

struct ecu_ntnode *ecu_ntnode_postorder_iterator_next(struct ecu_ntnode_postorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter) && node_valid(me->root) && node_valid(me->current) && node_valid(me->next)) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->next) || me->next == me->root || me->next == &me->delimiter) );

    me->current = me->next;
    struct ecu_ntnode *sibling = ecu_ntnode_next(me->next); /* Asserts returned node is valid if non-NULL. */
    
    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert node_valid(me->next). */
    if (sibling)
    {
        me->next = get_leaf(sibling);
    }
    else if (!sibling && me->next != me->root)
    {
        me->next = ecu_ntnode_parent(me->next);
    }
    else if (me->next == me->root)
    {
        me->next = &me->delimiter;
    }
    else
    {
        ECU_RUNTIME_ASSERT( (me->next == &me->delimiter) );
        me->next = get_leaf(me->root);
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cbegin(struct ecu_ntnode_postorder_citerator *me,
                                                              const struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ECU_RUNTIME_ASSERT( (node_valid(root)) );
    const struct ecu_ntnode *sibling = NTNODE_NULL;

    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_RESERVED);
    me->root = root;
    me->current = get_cleaf(root); /* Asserts returned node is valid since there is always a leaf. */
    sibling = ecu_ntnode_cnext(me->current); /* Asserts returned node is valid if non-NULL. */

    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert node_valid(me->next). */
    if (sibling)
    {
        me->next = get_cleaf(sibling);
    }
    else if (!sibling && me->current != root)
    {
        me->next = ecu_ntnode_cparent(me->current);
    }
    else
    {
        me->next = &me->delimiter;
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cend(struct ecu_ntnode_postorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter)) );
    return (&me->delimiter);
}

const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cnext(struct ecu_ntnode_postorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (node_valid(&me->delimiter) && node_valid(me->root) && node_valid(me->current) && node_valid(me->next)) );
    /* No nodes can be removed since this is a const iteration. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->current) || me->current == me->root || me->current == &me->delimiter) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->next) || me->next == me->root || me->next == &me->delimiter) );

    me->current = me->next;
    const struct ecu_ntnode *sibling = ecu_ntnode_cnext(me->next); /* Asserts returned node is valid if non-NULL. */
    
    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert node_valid(me->next). */
    if (sibling)
    {
        me->next = get_cleaf(sibling);
    }
    else if (!sibling && me->next != me->root)
    {
        me->next = ecu_ntnode_cparent(me->next);
    }
    else if (me->next == me->root)
    {
        me->next = &me->delimiter;
    }
    else
    {
        ECU_RUNTIME_ASSERT( (me->next == &me->delimiter) );
        me->next = get_cleaf(me->root);
    }

    return (me->current);
}
