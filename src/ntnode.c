/**
 * @file
 * @brief Generic n-ary tree. See
 * @rst
 * See :ref:`ntnode.h section <ntnode_h>` in Sphinx documentation.
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
#include "ecu/ntnode.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/ntnode.c")

/*------------------------------------------------------------*/
/*---------------------------- DEFINES -----------------------*/
/*------------------------------------------------------------*/

/// @brief NULL @ref ecu_dnode.
#define DNODE_NULL \
    ((struct ecu_dnode *)0)

/// @brief NULL const @ref ecu_dnode.
#define DNODE_CNULL \
    ((const struct ecu_dnode *)0)

/// @brief NULL @ref ecu_ntnode.
#define NTNODE_NULL \
    ((struct ecu_ntnode *)0)

/// @brief NULL const @ref ecu_ntnode.
#define NTNODE_CNULL \
    ((const struct ecu_ntnode *)0)

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Invalidates delimiter such that API asserts if
 * it is passed in.
 */
static void invalidate_delimiter(struct ecu_ntnode *ntnode);

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
 * @brief Const-qualified version of @ref get_leaf(). See @ref get_leaf().
 */
static const struct ecu_ntnode *get_cleaf(const struct ecu_ntnode *ntnode);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void invalidate_delimiter(struct ecu_ntnode *ntnode)
{
    ECU_RUNTIME_ASSERT( (ntnode) );

    ntnode->dnode.next = DNODE_NULL;            /* WARNING: Directly accessing dnode structure. */
    ntnode->dnode.prev = DNODE_NULL;            /* WARNING: Directly accessing dnode structure. */
    ntnode->dnode.id = ECU_OBJECT_ID_RESERVED;  /* WARNING: Directly accessing dnode structure. */
    ntnode->parent = NTNODE_NULL;
    ntnode->destroy = ECU_NTNODE_DESTROY_UNUSED;
}

static bool is_parent_of(const struct ecu_ntnode *parent, const struct ecu_ntnode *child)
{
    ECU_RUNTIME_ASSERT( (parent && child) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(child)) );
    bool status = false;
    struct ecu_ntnode_parent_citerator citer;

    /* Use at iterator to handle the case where child == parent. */
    ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, child)
    {
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
    struct ecu_ntnode *child = ecu_ntnode_first_child(leaf);

    /* API asserts returned node is valid (if non-NULL) so do not assert ecu_ntnode_valid(). */
    while (child)
    {
        leaf = child;
        child = ecu_ntnode_first_child(child);
    }

    return leaf;
}

static const struct ecu_ntnode *get_cleaf(const struct ecu_ntnode *ntnode)
{
    ECU_RUNTIME_ASSERT( (ntnode) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(ntnode)) );
    const struct ecu_ntnode *leaf = ntnode;
    const struct ecu_ntnode *child = ecu_ntnode_first_cchild(leaf);

    /* API asserts returned node is valid (if non-NULL) so do not assert ecu_ntnode_valid(). */
    while (child)
    {
        leaf = child;
        child = ecu_ntnode_first_cchild(child);
    }

    return leaf;
}

/*------------------------------------------------------------*/
/*------------------ NTNODE MEMBER FUNCTIONS -----------------*/
/*------------------------------------------------------------*/

void ecu_ntnode_ctor(struct ecu_ntnode *me, 
                     void (*destroy)(struct ecu_ntnode *me, ecu_object_id id), 
                     ecu_object_id id)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (id >= ECU_VALID_OBJECT_ID_BEGIN) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, id);
    ecu_dlist_ctor(&me->children);
    me->parent = me;
    me->destroy = destroy;
}

void ecu_ntnode_destroy(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    struct ecu_ntnode_postorder_iterator iter;
    ecu_object_id id = ECU_OBJECT_ID_UNUSED;

    /* Must be postorder so nodes can be safely destroyed in the middle of 
    an iteration. Force node to be reconstructed in order to be used again
    by destroying its members (dnode, dlist, etc) and manually invalidating
    ntnode members (parent, etc). Do this before user's destroy callback to
    prevent node from being used in the callback. This must also be done
    BEFORE the callback executes in case user frees the node. Otherwise 
    memory that has already been freed would be accessed. */
    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, me)
    {
        /* Precondition in order to safely destroy children list. Postorder 
        iteratation should guarantee this condition is always met. */
        ECU_RUNTIME_ASSERT( (ecu_ntnode_count(n) == 0) );

        /* Cache ID since it is cleared in dnode destructor. Begin destroying node. */
        id = ecu_ntnode_id(n);
        ecu_dnode_destroy(&n->dnode);
        ecu_dlist_destroy(&n->children);
        n->parent = NTNODE_NULL;

        /* Execute callback AFTER node is invalidated. */
        if (n->destroy != ECU_NTNODE_DESTROY_UNUSED)
        {
            (*n->destroy)(n, id);
        }
    }
}

void ecu_ntnode_clear(struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    struct ecu_ntnode_postorder_iterator iter;

    /* Must be postorder so nodes can be safely removed in the middle of an iteration. */
    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, me)
    {
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
    bool status = false;

    if ((ecu_ntnode_is_root(me)) &&
        (ecu_ntnode_count(me) == 0))
    {
        status = true;
    }

    return status;
}

struct ecu_ntnode *ecu_ntnode_first_child(struct ecu_ntnode *me)
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

const struct ecu_ntnode *ecu_ntnode_first_cchild(const struct ecu_ntnode *me)
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

bool ecu_ntnode_is_leaf(const struct ecu_ntnode *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    return (ecu_dlist_empty(&me->children));
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n1)) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n2)) );
    struct ecu_ntnode *lca = NTNODE_NULL;
    struct ecu_ntnode_parent_iterator iter;

    /* Use at iterator to handle case where n1 == n2. */
    ECU_NTNODE_PARENT_AT_FOR_EACH(current, &iter, n1)
    {
        if (current == n2 || is_parent_of(current, n2))
        {
            lca = current;
            break;
        }
    }

    return lca;
}

const struct ecu_ntnode *ecu_ntnode_clca(const struct ecu_ntnode *n1, const struct ecu_ntnode *n2)
{
    ECU_RUNTIME_ASSERT( (n1 && n2) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n1)) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(n2)) );
    const struct ecu_ntnode *lca = NTNODE_CNULL;
    struct ecu_ntnode_parent_citerator citer;

    /* Use at iterator to handle case where n1 == n2. */
    ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(current, &citer, n1)
    {
        if (current == n2 || is_parent_of(current, n2))
        {
            lca = current;
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
    struct ecu_ntnode_parent_citerator citer;

    /* Use normal parent iterator so starting node's level not included in level calculation. */
    ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, me)
    {
        (void)n;
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
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me)) );
    size_t size = 0;
    struct ecu_ntnode_postorder_citerator citer;

    ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, me)
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
        me->parent) /* me->parent == me if node has no parent. Set to NULL when node destroyed. */
    {
        status = true; 
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

    me->current = ecu_ntnode_first_child(parent);
    me->next = NTNODE_NULL;

    if (me->current) /* Parent has children? */
    {
        me->next = ecu_ntnode_next(me->current);
    }

    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_child_iterator_end(struct ecu_ntnode_child_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (NTNODE_NULL);
}

struct ecu_ntnode *ecu_ntnode_child_iterator_next(struct ecu_ntnode_child_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* me->current == NULL once iteration completes.
    Do not allow this function to be called after iteration completes. Force user to restart iteration. 
    Do not valid assert me->current to allow destruction of current node. Also not necessary since
    me->next is valid asserted when it is calculated in previous call. */
    ECU_RUNTIME_ASSERT( (me->current) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. 
    Note in_subtree() valid asserts me->next. IMPORTANT to check for NULL first before using API. */
    ECU_RUNTIME_ASSERT( ((me->next == NTNODE_NULL) || (ecu_ntnode_in_subtree(me->next))) );

    me->current = me->next;

    if (me->next) /* Still have children remaining? */
    {
        me->next = ecu_ntnode_next(me->next);
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_citerator *me, 
                                                          const struct ecu_ntnode *parent)
{
    ECU_RUNTIME_ASSERT( (me && parent) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(parent)) );

    me->current = ecu_ntnode_first_cchild(parent);
    me->next = NTNODE_CNULL;

    if (me->current) /* Parent has children? */
    {
        me->next = ecu_ntnode_cnext(me->current);
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (NTNODE_CNULL);
}

const struct ecu_ntnode *ecu_ntnode_child_iterator_cnext(struct ecu_ntnode_child_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* me->current == NULL once iteration completes.
    Do not allow this function to be called after iteration completes. Force user to restart iteration. 
    Do not valid assert me->current to allow destruction of current node. Also not necessary since
    me->next is valid asserted when it is calculated in previous call. */
    ECU_RUNTIME_ASSERT( (me->current) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. 
    Note in_subtree() valid asserts me->next. IMPORTANT to check for NULL first before using API. */
    ECU_RUNTIME_ASSERT( ((me->next == NTNODE_CNULL) || (ecu_ntnode_in_subtree(me->next))) );

    me->current = me->next;

    if (me->next) /* Still have children remaining? */
    {
        me->next = ecu_ntnode_cnext(me->next);
    }

    return (me->current);
}

/*------------------------------------------------------------*/
/*------------------------ PARENT ITERATOR -------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode *ecu_ntnode_parent_iterator_at(struct ecu_ntnode_parent_iterator *me,
                                                 struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );

    me->current = start;
    me->next = ecu_ntnode_parent(start);
    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_parent_iterator_begin(struct ecu_ntnode_parent_iterator *me,
                                                    struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );

    me->current = ecu_ntnode_parent(start);
    me->next = NTNODE_NULL;

    if (me->current) /* start has parents? */
    {
        me->next = ecu_ntnode_parent(me->current);
    }

    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_parent_iterator_end(struct ecu_ntnode_parent_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (NTNODE_NULL);
}

struct ecu_ntnode *ecu_ntnode_parent_iterator_next(struct ecu_ntnode_parent_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* me->current == NULL once iteration completes.
    Do not allow this function to be called after iteration completes. Force user to restart iteration. 
    Do not valid assert me->current to allow destruction of current node. Also not necessary since
    me->next is valid asserted when it is calculated in previous call. */
    ECU_RUNTIME_ASSERT( (me->current) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed.
    This is not directly checked since it is impossible to detect this condition. The next node can be a root, 
    and it is unknown what the root is at the start of the iteration. Therefore just check if the next node
    is valid when non-NULL. IMPORTANT to check for NULL first before using API. */
    ECU_RUNTIME_ASSERT( ((me->next == NTNODE_NULL) || (ecu_ntnode_valid(me->next))) );

    me->current = me->next;

    if (me->next) /* Still more parents to go? */
    {
        me->next = ecu_ntnode_parent(me->next);
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cat(struct ecu_ntnode_parent_citerator *me,
                                                        const struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );

    me->current = start;
    me->next = ecu_ntnode_cparent(start);
    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cbegin(struct ecu_ntnode_parent_citerator *me,
                                                           const struct ecu_ntnode *start)
{
    ECU_RUNTIME_ASSERT( (me && start) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(start)) );

    me->current = ecu_ntnode_cparent(start);
    me->next = NTNODE_CNULL;

    if (me->current) /* start has parents? */
    {
        me->next = ecu_ntnode_cparent(me->current);
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cend(struct ecu_ntnode_parent_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (NTNODE_CNULL);
}

const struct ecu_ntnode *ecu_ntnode_parent_iterator_cnext(struct ecu_ntnode_parent_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* me->current == NULL once iteration completes.
    Do not allow this function to be called after iteration completes. Force user to restart iteration. 
    Do not valid assert me->current to allow destruction of current node. Also not necessary since
    me->next is valid asserted when it is calculated in previous call. */
    ECU_RUNTIME_ASSERT( (me->current) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed.
    This is not directly checked since it is impossible to detect this condition. The next node can be a root, 
    and it is unknown what the root is at the start of the iteration. Therefore just check if the next node
    is valid when non-NULL. IMPORTANT to check for NULL first before using API. */
    ECU_RUNTIME_ASSERT( ((me->next == NTNODE_CNULL) || (ecu_ntnode_valid(me->next))) );

    me->current = me->next;

    if (me->next) /* Still more parents to go? */
    {
        me->next = ecu_ntnode_cparent(me->next);
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

    /* Guarantee delimiter is invalid to prevent it from being used. API asserts if used. */
    invalidate_delimiter(&me->delimiter);

    me->root = root;
    me->current = get_leaf(root);
    sibling = ecu_ntnode_next(me->current);

    /* Branches MUST be in this order!! Save next node in case current node removed. */
    if (me->current == me->root)
    {
        me->next = &me->delimiter; /* Done with iteration. */
    }
    else if (sibling)
    {
        me->next = get_leaf(sibling);
    }
    else /* else if (me->current != me->root) && !sibling */
    {
        /* It should be impossible for parent to be NULL if this last branch enters. */
        me->next = ecu_ntnode_parent(me->current);
        ECU_RUNTIME_ASSERT( (me->next) );
    }

    return (me->current);
}

struct ecu_ntnode *ecu_ntnode_postorder_iterator_end(struct ecu_ntnode_postorder_iterator *me)
{
    /* Do not valid assert me->delimiter. It is purposefully not valid to prevent it being used. */
    ECU_RUNTIME_ASSERT( (me) );
    return (&me->delimiter);
}

struct ecu_ntnode *ecu_ntnode_postorder_iterator_next(struct ecu_ntnode_postorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Do not assert me->root here to handle edge case of a leaf node being destroyed from other iterations. 
    I.e. see TEST(NtNode, ChildIteratorDestroySome). */
    ECU_RUNTIME_ASSERT( (me->current && me->next) );
    /* Do not allow this function to be called after iteration completes. Force user to restart iteration. 
    Do not valid assert me->current to allow destruction of current node. Also not necessary since
    me->next is either delimiter or valid asserted when it is calculated in previous call. */
    ECU_RUNTIME_ASSERT( (me->current != &me->delimiter) );
    /* Next node can be delimiter. Delimiter purposefully made invalid to prevent it from being used. 
    IMPORTANT to check for delimiter first before using API since delimiter cannot be passed to API. */
    ECU_RUNTIME_ASSERT( ((me->next == &me->delimiter) || (ecu_ntnode_valid(me->next))) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. 
    IMPORTANT to check for delimiter first before using API. */
    ECU_RUNTIME_ASSERT( ((me->next == &me->delimiter) || (ecu_ntnode_in_subtree(me->next)) || (me->next == me->root)) );

    me->current = me->next;

    if (me->next != &me->delimiter)
    {
        /* Only valid assert root node here to handle edge case of a leaf node being destroyed 
        from other iterations. I.e. see TEST(NtNode, ChildIteratorDestroySome). */
        ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->root)) );
        struct ecu_ntnode *sibling = ecu_ntnode_next(me->next); 

        /* Branches MUST be in this order!! Save next node in case current node removed. */
        if (me->next == me->root)
        {
            me->next = &me->delimiter; /* Done with iteration. */
        }
        else if (sibling)
        {
            me->next = get_leaf(sibling);
        }
        else /* else if (me->next != me->root) && !sibling */
        {
            /* It should be impossible for parent to be NULL if this last branch enters. */
            me->next = ecu_ntnode_parent(me->next);
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cbegin(struct ecu_ntnode_postorder_citerator *me,
                                                              const struct ecu_ntnode *root)
{
    ECU_RUNTIME_ASSERT( (me && root) );
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(root)) );
    const struct ecu_ntnode *sibling = NTNODE_CNULL;

    /* Guarantee delimiter is invalid to prevent it from being used. API asserts if used. */
    invalidate_delimiter(&me->delimiter);

    me->root = root;
    me->current = get_cleaf(root);
    sibling = ecu_ntnode_cnext(me->current);

    /* Branches MUST be in this order!! Save next node in case current node removed. */
    if (me->current == me->root)
    {
        me->next = &me->delimiter; /* Done with iteration. */
    }
    else if (sibling)
    {
        me->next = get_cleaf(sibling);
    }
    else /* else if (me->current != me->root) && !sibling */
    {
        /* It should be impossible for parent to be NULL if this last branch enters. */
        me->next = ecu_ntnode_cparent(me->current);
        ECU_RUNTIME_ASSERT( (me->next) );
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cend(struct ecu_ntnode_postorder_citerator *me)
{
    /* Do not valid assert me->delimiter. It is purposefully not valid to prevent it being used. */
    ECU_RUNTIME_ASSERT( (me) );
    return (&me->delimiter);
}

const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cnext(struct ecu_ntnode_postorder_citerator *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->root && me->current && me->next) );
    /* Unlike non-const iterator, me->root can be valid asserted here. Only non-const iterator used for ntnode_destroy(). */
    ECU_RUNTIME_ASSERT( (ecu_ntnode_valid(me->root)) );
    /* Do not allow this function to be called after iteration completes. Force user to restart iteration. 
    Unlike non-const iterator, me->current should be valid asserted since nodes are const and cannot be destroyed. */
    ECU_RUNTIME_ASSERT( ((me->current != &me->delimiter) && (ecu_ntnode_valid(me->current))) );
    /* Next node can be delimiter. Delimiter purposefully made invalid to prevent it from being used. 
    IMPORTANT to check for delimiter first before using API since delimiter cannot be passed to API. */
    ECU_RUNTIME_ASSERT( ((me->next == &me->delimiter) || (ecu_ntnode_valid(me->next))) );
    /* The current node can be safely removed but removing the next node before it's returned is not allowed. 
    IMPORTANT to check for delimiter first before using API. */
    ECU_RUNTIME_ASSERT( ((me->next == &me->delimiter) || (ecu_ntnode_in_subtree(me->next)) || (me->next == me->root)) );

    me->current = me->next;

    if (me->next != &me->delimiter)
    {
        const struct ecu_ntnode *sibling = ecu_ntnode_cnext(me->next); 

        /* Branches MUST be in this order!! Save next node in case current node removed. */
        if (me->next == me->root)
        {
            me->next = &me->delimiter; /* Done with iteration. */
        }
        else if (sibling)
        {
            me->next = get_cleaf(sibling);
        }
        else /* else if (me->next != me->root) && !sibling */
        {
            /* It should be impossible for parent to be NULL if this last branch enters. */
            me->next = ecu_ntnode_cparent(me->next);
            ECU_RUNTIME_ASSERT( (me->next) );
        }
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

    /* Guarantee delimiter is invalid to prevent it from being used. API asserts if used. */
    invalidate_delimiter(&me->delimiter);

    me->root = root;
    me->current = root;
    return root;
}

struct ecu_ntnode *ecu_ntnode_preorder_iterator_end(struct ecu_ntnode_preorder_iterator *me)
{
    /* Do not valid assert me->delimiter. It is purposefully not valid to prevent it being used. */
    ECU_RUNTIME_ASSERT( (me) );
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
        struct ecu_ntnode *child = ecu_ntnode_first_child(me->current);
        struct ecu_ntnode *sibling = ecu_ntnode_next(me->current);
        struct ecu_ntnode *parent = ecu_ntnode_parent(me->current);
        bool traversed_up = true;

        /* API functions assert returned node is valid (if non-NULL), 
        so do not assert ecu_ntnode_valid(child, sibling, parent). */
        while ((!child) && (!sibling) && (parent) && (me->current != me->root))
        {
            traversed_up = true;
            me->current = parent;
            child = ecu_ntnode_first_child(me->current);
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

    /* Guarantee delimiter is invalid to prevent it from being used. API asserts if used. */
    invalidate_delimiter(&me->delimiter);

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

    const struct ecu_ntnode *child = ecu_ntnode_first_cchild(me->current);
    const struct ecu_ntnode *sibling = ecu_ntnode_cnext(me->current);
    const struct ecu_ntnode *parent = ecu_ntnode_cparent(me->current);
    bool traversed_up = true;

    /* API functions assert returned node is valid (if non-NULL), 
    so do not assert ecu_ntnode_valid(child, sibling, parent). */
    while (!child && !sibling && parent)
    {
        traversed_up = true;
        me->current = parent;
        child = ecu_ntnode_first_cchild(me->current);
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

struct ecu_ntnode *ecu_ntnode_sibling_iterator_at(struct ecu_ntnode_sibling_iterator *me,
                                                  struct ecu_ntnode *start)
{
    #warning "TODO"
    return start;
}

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
            me->current = ecu_ntnode_first_child(ecu_ntnode_parent(start));
            ECU_RUNTIME_ASSERT( (me->current) );
        }

        me->next = ecu_ntnode_next(me->current);
        if (!me->next) /* Wraparound. Get first sibling. */
        {
            me->next = ecu_ntnode_first_child(ecu_ntnode_parent(me->current));
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
            me->next = ecu_ntnode_first_child(ecu_ntnode_parent(me->current));
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}

const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cat(struct ecu_ntnode_sibling_citerator *me,
                                                         const struct ecu_ntnode *start)
{
    #warning "TODO"
    return start;
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
            me->current = ecu_ntnode_first_cchild(ecu_ntnode_cparent(start));
            ECU_RUNTIME_ASSERT( (me->current) );
        }

        me->next = ecu_ntnode_cnext(me->current);
        if (!me->next) /* Wraparound. Get first sibling. */
        {
            me->next = ecu_ntnode_first_cchild(ecu_ntnode_cparent(me->current));
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
            me->next = ecu_ntnode_first_cchild(ecu_ntnode_cparent(me->current));
            ECU_RUNTIME_ASSERT( (me->next) );
        }
    }

    return (me->current);
}
