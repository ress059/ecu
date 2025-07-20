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

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Destructor assigned to dummy delimiter nodes when
 * they are constructed. This function simply asserts since
 * delimiters are not meant to be used by the application.
 * 
 * @param ntnode Unused. 
 * @param id Unused.
 */
static void invalid_destroy(struct ecu_ntnode *ntnode, ecu_object_id id);

/**
 * @brief Do not call this function directly.
 * Linked list node destructor assigned to every tree
 * node that is constructed. Each tree node is a linked list
 * node so the dlist API is utilized, including the custom
 * destroy functionality. This is a wrapper function that
 * is called when the linked list node is destroyed. It simply
 * calls the user's tree node destroy function if one was 
 * supplied to @ref ecu_ntnode_ctor().
 * 
 * @param dnode Linked list node being destroyed. Converted
 * to tree node and passed to user's destroy callback.
 * @param id ID supplied to @ref ecu_ntnode_ctor().
 */
static void dnode_destroy(struct ecu_dnode *dnode, ecu_object_id id);

/**
 * @pre @p parent and @p child previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns true if @p parent is a parent, grandparent,
 * great-grandparent, etc of @p child. False otherwise.
 * 
 * @param parent Function returns true if this node is a parent,
 * grandparent, great-grandparent, etc of @p child.
 * @param child Child node to check. 
 */
static bool is_parent_of(const struct ecu_ntnode *parent, const struct ecu_ntnode *child);

/**
 * @pre @p ntnode previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the left-most leaf of subtree @p ntnode. If @p ntnode
 * is already a leaf, it is returned.
 * 
 * @param ntnode Node to check.
 */
static struct ecu_ntnode *get_leaf(struct ecu_ntnode *ntnode);

/**
 * @brief const-qualified version of @ref get_leaf(). See @ref get_leaf().
 */
static const struct ecu_ntnode *get_cleaf(const struct ecu_ntnode *ntnode);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

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
    /* Do not assert ecu_ntnode_valid() since internal values will already be reset to NULL. */

    /* Call user's ntnode destroy callback if one was supplied. Otherwise no additional cleanup. */
    if (me->destroy != ECU_NTNODE_DESTROY_UNUSED)
    {
        (*me->destroy)(me, id);
    }
}

static bool is_parent_of(const struct ecu_ntnode *parent, const struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (parent && child) );
    /* ecu_ntnode_valid(child) done in for-loop. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    bool status = false;

    /* Notice how this also handles case where n1 == n2. */
    for (const struct ecu_ntnode *n = child; n; n = ecu_ntnode_cparent(n))
    {
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n)) );
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntnode)) );
    struct ecu_ntnode *leaf = ntnode;
    struct ecu_ntnode *child = ecu_ntnode_front_child(leaf);

    /* API asserts returned node is valid (if non-NULL) so do not assert ecu_ntnode_valid(). */
    while (child)
    {
        leaf = child;
        child = ecu_ntnode_front_child(child);
    }

    return leaf;
}

static const struct ecu_ntnode *get_cleaf(const struct ecu_ntnode *ntnode)
{
    ECU_RUNTIME_ASSERT( (ntnode) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntnode)) );
    const struct ecu_ntnode *leaf = ntnode;
    const struct ecu_ntnode *child = ecu_ntnode_front_cchild(leaf);

    /* API asserts returned node is valid (if non-NULL) so do not assert ecu_ntnode_valid(). */
    while (child)
    {
        leaf = child;
        child = ecu_ntnode_front_cchild(child);
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
        (including me) so do not assert ecu_ntnode_valid().
        Destroy objects by setting to NULL values. Forces user to reconstruct
        nodes if they want to use them again, assuming asserts are enabled.
        IMPORTANT: These values are reset before the destroy callback in case
        the user frees their entire node (including ecu_ntnode). Otherwise 
        we would be accessing and writing to freed memory. */
        n->parent = NTNODE_NULL;
        ecu_dnode_destroy(&n->dnode);
    }
}

void ecu_ntnode_clear(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    struct ecu_ntnode_postorder_iterator iterator;

    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iterator, me)
    {
        /* Must be postorder so nodes can be safely removed in the middle of 
        an iteration. Iterator already asserts if returned nodes are valid
        (including me) so do not assert ecu_ntnode_valid(). */
        ecu_ntnode_remove(n);
    }
}

size_t ecu_ntnode_count(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    return (ecu_dlist_size(&me->children));
}

bool ecu_ntnode_empty(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    return ((ecu_ntnode_is_root(me)) && (ecu_ntnode_count(me) == 0));
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

const struct ecu_ntnode *ecu_ntnode_cfind(const struct ecu_ntnode *me, 
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

struct ecu_ntnode *ecu_ntnode_front_child(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    struct ecu_dnode *dfront = ecu_dlist_front(&me->children);
    struct ecu_ntnode *ntfront = NTNODE_NULL;

    if (dfront)
    {
        ntfront = ECU_DNODE_GET_ENTRY(dfront, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntfront)) );
    }

    return ntfront;
}

const struct ecu_ntnode *ecu_ntnode_front_cchild(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    const struct ecu_dnode *dfront = ecu_dlist_cfront(&me->children);
    const struct ecu_ntnode *ntfront = NTNODE_CNULL;

    if (dfront)
    {
        ntfront = ECU_DNODE_GET_CONST_ENTRY(dfront, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntfront)) );
    }

    return ntfront;
}

ecu_object_id ecu_ntnode_id(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    return (ecu_dnode_id(&me->dnode));
}

bool ecu_ntnode_in_subtree(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    return (ecu_dnode_in_list(&me->dnode));
}

void ecu_ntnode_insert_sibling_after(struct ecu_ntnode *pos, struct ecu_ntnode *sibling)
{
    ECU_RUNTIME_ASSERT( (pos && sibling) );
    ECU_RUNTIME_ASSERT( (pos != sibling) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(pos)) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(sibling)) );
    /* Tree must have a root. Cannot add sibling to a root. 
    Sibling also cannot already be within a tree. */
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(pos)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(sibling)) );

    ecu_dnode_insert_after(&pos->dnode, &sibling->dnode);
    sibling->parent = pos->parent;
}

void ecu_ntnode_insert_sibling_before(struct ecu_ntnode *pos, struct ecu_ntnode *sibling)
{
    ECU_RUNTIME_ASSERT( (pos && sibling) );
    ECU_RUNTIME_ASSERT( (pos != sibling) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(pos)) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(sibling)) );
    /* Tree must have a root. Cannot add sibling to a root. 
    Sibling also cannot already be within a tree. */
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_is_root(pos)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(sibling)) );

    ecu_dnode_insert_before(&pos->dnode, &sibling->dnode);
    sibling->parent = pos->parent;
}

bool ecu_ntnode_is_root(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    return (!ecu_dnode_in_list(&me->dnode));
}

struct ecu_ntnode *ecu_ntnode_last_child(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    struct ecu_dnode *dback = ecu_dlist_back(&me->children);
    struct ecu_ntnode *ntback = NTNODE_NULL;

    if (dback)
    {
        ntback = ECU_DNODE_GET_ENTRY(dback, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntback)) );
    }

    return ntback;
}

const struct ecu_ntnode *ecu_ntnode_last_cchild(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    const struct ecu_dnode *dback = ecu_dlist_cback(&me->children);
    const struct ecu_ntnode *ntback = NTNODE_CNULL;

    if (dback)
    {
        ntback = ECU_DNODE_GET_CONST_ENTRY(dback, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntback)) );
    }

    return ntback;
}

struct ecu_ntnode *ecu_ntnode_lca(struct ecu_ntnode *n1, struct ecu_ntnode *n2)
{
    ECU_RUNTIME_ASSERT( (n1 && n2) );
    /* ecu_ntnode_valid(n1) done in for-loop. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n2)) );
    struct ecu_ntnode *lca = NTNODE_NULL;

    for (struct ecu_ntnode *n = n1; n; n = ecu_ntnode_parent(n))
    {
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n)) );
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
    /* ecu_ntnode_valid(n1) done in for-loop. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n2)) );
    const struct ecu_ntnode *lca = NTNODE_CNULL;

    for (const struct ecu_ntnode *n = n1; n; n = ecu_ntnode_cparent(n))
    {
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n)) );
        if (n == n2 || is_parent_of(n, n2))
        {
            lca = n;
            break;
        }
    }

    return lca;
}

size_t ecu_ntnode_level(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    size_t level = 0;

    for (const struct ecu_ntnode *parent = ecu_ntnode_cparent(me); 
         parent; 
         parent = ecu_ntnode_cparent(me))
    {
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
        level++;
    }

    return level;
}

struct ecu_ntnode *ecu_ntnode_next(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    struct ecu_dnode *dnext = ecu_dnode_next(&me->dnode);
    struct ecu_ntnode *ntnext = NTNODE_NULL;

    if (dnext)
    {
        ntnext = ECU_DNODE_GET_ENTRY(dnext, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntnext)) );
    }

    return ntnext;
}

const struct ecu_ntnode *ecu_ntnode_cnext(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    const struct ecu_dnode *dnext = ecu_dnode_cnext(&me->dnode);
    const struct ecu_ntnode *ntnext = NTNODE_CNULL;

    if (dnext)
    {
        ntnext = ECU_DNODE_GET_CONST_ENTRY(dnext, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntnext)) );
    }

    return ntnext;
}

struct ecu_ntnode *ecu_ntnode_parent(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    struct ecu_ntnode *parent = NTNODE_NULL;
    
    if (me->parent != me)
    {
        parent = me->parent;
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    }

    return parent;
}

const struct ecu_ntnode *ecu_ntnode_cparent(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    const struct ecu_ntnode *parent = NTNODE_CNULL;

    if (me->parent != me)
    {
        parent = me->parent;
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    }

    return parent;
}

struct ecu_ntnode *ecu_ntnode_prev(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    struct ecu_dnode *dprev = ecu_dnode_prev(&me->dnode);
    struct ecu_ntnode *ntprev = NTNODE_NULL;

    if (dprev)
    {
        ntprev = ECU_DNODE_GET_ENTRY(dprev, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntprev)) );
    }

    return ntprev;
}

const struct ecu_ntnode *ecu_ntnode_cprev(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    const struct ecu_dnode *dprev = ecu_dnode_cprev(&me->dnode);
    const struct ecu_ntnode *ntprev = NTNODE_CNULL;

    if (dprev)
    {
        ntprev = ECU_DNODE_GET_CONST_ENTRY(dprev, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntprev)) );
    }

    return ntprev;
}

void ecu_ntnode_push_child_back(struct ecu_ntnode *parent, struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (parent && child) );
    ECU_RUNTIME_ASSERT( (parent != child) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(child)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(child)) );

    ecu_dlist_push_back(&parent->children, &child->dnode);
    child->parent = parent;
}

void ecu_ntnode_push_child_front(struct ecu_ntnode *parent, struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (parent && child) );
    ECU_RUNTIME_ASSERT( (parent != child) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(child)) );
    ECU_RUNTIME_ASSERT( (!ecu_ntnode_in_subtree(child)) );

    ecu_dlist_push_front(&parent->children, &child->dnode);
    child->parent = parent;
}

void ecu_ntnode_remove(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    ecu_dnode_remove(&me->dnode);
    me->parent = me;
}

size_t ecu_ntnode_size(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Do not assert ecu_ntnode_valid(). Iterator already does this on all returned nodes including me. */
    size_t size = 0;
    struct ecu_ntnode_preorder_citerator citerator;

    ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citerator, me)
    {
        (void)n;
        size++;
    }

    ECU_RUNTIME_ASSERT( (size > 0) ); /* Iteration should have gone at least over me. */
    return (size - 1);
}

bool ecu_ntnode_valid(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    bool status = false;

    if (ecu_dnode_valid(&me->dnode) &&
        ecu_dlist_valid(&me->children) &&
        me->parent)
    {
        status = true; /* me->parent == me if node has no parent.*/
    }

    return status;
}

/*------------------------------------------------------------*/
/*------------------------ CHILD ITERATOR --------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_child_iterator_begin(struct ecu_ntnode_child_iterator *me, 
                                                   struct ecu_ntnode *parent)
{
    ECU_RUNTIME_ASSERT( (me && parent) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    struct ecu_dnode *dstart = DNODE_NULL;
    struct ecu_ntnode *ntstart = parent;

    dstart = ecu_dlist_iterator_begin(&me->iterator, &parent->children);
    if (dstart != ecu_dlist_iterator_end(&me->iterator))
    {
        ntstart = ECU_DNODE_GET_ENTRY(dstart, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntstart)) );
    }
    
    return ntstart;
}

struct ecu_ntnode *ecu_ntnode_child_iterator_end(struct ecu_ntnode_child_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    struct ecu_dnode *dend = DNODE_NULL;
    struct ecu_ntnode *ntend = NTNODE_NULL;

    dend = ecu_dlist_iterator_end(&me->iterator);
    ntend = ECU_DNODE_GET_ENTRY(dend, struct ecu_ntnode, children.head);
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntend)) );
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

    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntnext)) );
    return ntnext;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_citerator *me, 
                                                          const struct ecu_ntnode *parent)
{
    ECU_RUNTIME_ASSERT( (me && parent) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    const struct ecu_dnode *dstart = DNODE_CNULL;
    const struct ecu_ntnode *ntstart = parent;

    dstart = ecu_dlist_iterator_cbegin(&me->iterator, &parent->children);
    if (dstart != ecu_dlist_iterator_cend(&me->iterator))
    {
        ntstart = ECU_DNODE_GET_CONST_ENTRY(dstart, struct ecu_ntnode, dnode);
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntstart)) );
    }
    
    return ntstart;
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    const struct ecu_dnode *dend = DNODE_CNULL;
    const struct ecu_ntnode *ntend = NTNODE_CNULL;

    dend = ecu_dlist_iterator_cend(&me->iterator);
    ntend = ECU_DNODE_GET_CONST_ENTRY(dend, struct ecu_ntnode, children.head);
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntend)) );
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

    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntnext)) );
    return ntnext;
}

/*------------------------------------------------------------*/
/*------------------------ PARENT ITERATOR -------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_parent_iterator_begin(struct ecu_ntnode_parent_iterator *me,
                                                    struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start)) );
    return (me->start);
}

struct ecu_ntnode *ecu_ntnode_parent_iterator_next(struct ecu_ntnode_parent_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->start && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start) && ecu_ntnode_valid(me->current) && ecu_ntnode_valid(me->next)) );
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start)) );
    return (me->start);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cnext(struct ecu_ntnode_parent_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->start && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start) && ecu_ntnode_valid(me->current) && ecu_ntnode_valid(me->next)) );
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
/*--------------------- POSTORDER ITERATOR -------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_postorder_iterator_begin(struct ecu_ntnode_postorder_iterator *me,
                                                       struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(root)) );
    struct ecu_ntnode *sibling = NTNODE_NULL;

    /* Guarantee delimiter node is invalidated by assigning reserved ID. API will assert if delimiter 
    is used. ID reassigned after constructor since constructor asserts when a reserved ID is used. */
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_UNUSED);
    me->delimiter.dnode.id = ECU_OBJECT_ID_RESERVED;

    me->root = root;
    me->current = get_leaf(root); /* Asserts returned node is valid since there is always a leaf. */
    sibling = ecu_ntnode_next(me->current); /* Asserts returned node is valid if non-NULL. */

    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert ecu_ntnode_valid(me->next). */
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(&me->delimiter)) );
    return (&me->delimiter);
}

struct ecu_ntnode *ecu_ntnode_postorder_iterator_next(struct ecu_ntnode_postorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(&me->delimiter) && ecu_ntnode_valid(me->root) && ecu_ntnode_valid(me->current) && ecu_ntnode_valid(me->next)) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->next) || me->next == me->root || me->next == &me->delimiter) );

    me->current = me->next;
    struct ecu_ntnode *sibling = ecu_ntnode_next(me->next); /* Asserts returned node is valid if non-NULL. */
    
    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert ecu_ntnode_valid(me->next). */
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(root)) );
    const struct ecu_ntnode *sibling = NTNODE_NULL;

    /* Guarantee delimiter node is invalidated by assigning reserved ID. API will assert if delimiter 
    is used. ID reassigned after constructor since constructor asserts when a reserved ID is used. */
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_UNUSED);
    me->delimiter.dnode.id = ECU_OBJECT_ID_RESERVED;
    
    me->root = root;
    me->current = get_cleaf(root); /* Asserts returned node is valid since there is always a leaf. */
    sibling = ecu_ntnode_cnext(me->current); /* Asserts returned node is valid if non-NULL. */

    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert ecu_ntnode_valid(me->next). */
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(&me->delimiter)) );
    return (&me->delimiter);
}

const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cnext(struct ecu_ntnode_postorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current && me->next) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(&me->delimiter) && ecu_ntnode_valid(me->root) && ecu_ntnode_valid(me->current) && ecu_ntnode_valid(me->next)) );
    /* No nodes can be removed since this is a const iteration. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->current) || me->current == me->root || me->current == &me->delimiter) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->next) || me->next == me->root || me->next == &me->delimiter) );

    me->current = me->next;
    const struct ecu_ntnode *sibling = ecu_ntnode_cnext(me->next); /* Asserts returned node is valid if non-NULL. */
    
    /* Save next node in case user removes current node. API functions assert 
    returned node is valid (if non-NULL), so do not assert ecu_ntnode_valid(me->next). */
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

/*------------------------------------------------------------*/
/*----------------------- PREORDER ITERATOR ------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_preorder_iterator_begin(struct ecu_ntnode_preorder_iterator *me,
                                                      struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(root)) );

    /* Guarantee delimiter node is invalidated by assigning reserved ID. API will assert if delimiter 
    is used. ID reassigned after constructor since constructor asserts when a reserved ID is used. */
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_UNUSED);
    me->delimiter.dnode.id = ECU_OBJECT_ID_RESERVED;

    me->root = root;
    me->current = root;
    return root;
}

struct ecu_ntnode *ecu_ntnode_preorder_iterator_end(struct ecu_ntnode_preorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Do not assert ecu_ntnode_valid() on delimiter. It is purposefully invalid in an 
    attempt to prevent the user from passing it into the API and using it. */
    return (&me->delimiter);
}

struct ecu_ntnode *ecu_ntnode_preorder_iterator_next(struct ecu_ntnode_preorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current) );
    /* Do not assert ecu_ntnode_valid() on delimiter. It is purposefully invalid in an 
    attempt to prevent the user from passing it into the API and using it. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->root) && ecu_ntnode_valid(me->current)) );
    /* Continuing the iteration after removing a node is not allowed. IMPORTANT to
    check if current == delimiter FIRST before passing current to API. */
    ECU_RUNTIME_ASSERT( (me->current == me->root || me->current == &me->delimiter || ecu_ntnode_in_subtree(me->current)) );

    if (me->current == &me->delimiter)
    {
        /* Check for this condition first since delimiter cannot be passed to API. */
        me->current = me->root;
    }
    else
    {
        struct ecu_ntnode *child = ecu_ntnode_front_child(me->current);
        struct ecu_ntnode *sibling = ecu_ntnode_next(me->current);
        struct ecu_ntnode *parent = ecu_ntnode_parent(me->current);
        bool traversed_up = true;

        /* API functions assert returned node is valid (if non-NULL), 
        so do not assert ecu_ntnode_valid(child, sibling, parent). */
        while ((!child) && (!sibling) && (parent) && (me->current != me->root))
        {
            traversed_up = true;
            me->current = parent;
            child = ecu_ntnode_front_child(me->current);
            sibling = ecu_ntnode_next(me->current);
            parent = ecu_ntnode_parent(me->current);
        }

        if (!traversed_up && child)
        {
            me->current = child;
        }
        else if (sibling)
        {
#pragma message("TODO: Think this fails if you do iteration on subtree that is right-most!!")
            me->current = sibling;
        }
        else
        {
            /* Reached the end of the iteration. */
            ECU_RUNTIME_ASSERT( (me->current == me->root) );
            me->current = &me->delimiter;
        }
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cbegin(struct ecu_ntnode_preorder_citerator *me,
                                                             const struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(root)) );

    /* Guarantee delimiter node is invalidated by assigning reserved ID. API will assert if delimiter 
    is used. ID reassigned after constructor since constructor asserts when a reserved ID is used. */
    ecu_ntnode_ctor(&me->delimiter, &invalid_destroy, ECU_OBJECT_ID_UNUSED);
    me->delimiter.dnode.id = ECU_OBJECT_ID_RESERVED;

    me->root = root;
    me->current = root;
    return root;
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cend(struct ecu_ntnode_preorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(&me->delimiter)) );
    return (&me->delimiter);
}

const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cnext(struct ecu_ntnode_preorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(&me->delimiter) && ecu_ntnode_valid(me->root) && ecu_ntnode_valid(me->current)) );
    /* Continuing the iteration after removing a node is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->current) || me->current == me->root || me->current == &me->delimiter) );

    const struct ecu_ntnode *child = ecu_ntnode_front_cchild(me->current);
    const struct ecu_ntnode *sibling = ecu_ntnode_cnext(me->current);
    const struct ecu_ntnode *parent = ecu_ntnode_cparent(me->current);
    bool traversed_up = true;

    /* API functions assert returned node is valid (if non-NULL), 
    so do not assert ecu_ntnode_valid(child, sibling, parent). */
    while (!child && !sibling && parent)
    {
        traversed_up = true;
        me->current = parent;
        child = ecu_ntnode_front_cchild(me->current);
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
/*----------------------- SIBLING ITERATOR -------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_sibling_iterator_begin(struct ecu_ntnode_sibling_iterator *me,
                                                     struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );
    me->start = start;

    if (ecu_ntnode_is_root(start))
    {
        me->current = start;
        me->next = start;
    }
    else
    {
        me->current = ecu_ntnode_next(start);
        if (!me->current) /* Wraparound. Get first sibling. */
        {
            me->current = ecu_ntnode_front_child(ecu_ntnode_parent(start));
            ECU_RUNTIME_ASSERT( (me->current) );
        }

        me->next = ecu_ntnode_next(me->current);
        if (!me->next) /* Wraparound. Get first sibling. */
        {
            me->next = ecu_ntnode_front_child(ecu_ntnode_parent(me->current));
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_sibling_iterator_end(struct ecu_ntnode_sibling_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start)) );
    return (me->start);
}

struct ecu_ntnode *ecu_ntnode_sibling_iterator_next(struct ecu_ntnode_sibling_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start) && ecu_ntnode_valid(me->current) && ecu_ntnode_valid(me->next)) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->next)) );

    me->current = me->next;

    /* Do nothing for edge case where ending node removed and next called explicitly on expired iterator. */
    if (!ecu_ntnode_is_root(me->next)) 
    {
        me->next = ecu_ntnode_next(me->current);
        if (!me->next) /* Wraparound. Get first sibling. */
        {
            me->next = ecu_ntnode_front_child(ecu_ntnode_parent(me->current));
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cbegin(struct ecu_ntnode_sibling_citerator *me,
                                                            const struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );
    me->start = start;

    if (ecu_ntnode_is_root(start))
    {
        me->current = start;
        me->next = start;
    }
    else
    {
        me->current = ecu_ntnode_cnext(start);
        if (!me->current) /* Wraparound. Get first sibling. */
        {
            me->current = ecu_ntnode_front_cchild(ecu_ntnode_cparent(start));
            ECU_RUNTIME_ASSERT( (me->current) );
        }

        me->next = ecu_ntnode_cnext(me->current);
        if (!me->next) /* Wraparound. Get first sibling. */
        {
            me->next = ecu_ntnode_front_cchild(ecu_ntnode_cparent(me->current));
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cend(struct ecu_ntnode_sibling_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start)) );
    return (me->start);
}

const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cnext(struct ecu_ntnode_sibling_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->start) && ecu_ntnode_valid(me->current) && ecu_ntnode_valid(me->next)) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_in_subtree(me->next) || me->next == me->start) );

    me->current = me->next;

    /* Do nothing for edge case where ending node removed and next called explicitly on expired iterator. */
    if (!ecu_ntnode_is_root(me->next)) 
    {
        me->next = ecu_ntnode_cnext(me->current);
        if (!me->next) /* Wraparound. Get first sibling. */
        {
            me->next = ecu_ntnode_front_cchild(ecu_ntnode_cparent(me->current));
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}
