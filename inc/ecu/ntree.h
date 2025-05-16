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

#ifndef NTREE_H_
#define NTREE_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>

/* ECU. */
#include "ecu/dlist.h"
#include "ecu/object_id.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

#define ECU_NTNODE_DESTROY_UNUSED \
    ((void (*)(struct ecu_ntnode *, ecu_object_id))0)

#define ECU_NTNODE_GET_ENTRY(ptr_, type_, member_) \
    ((type_ *)((uint8_t *)(ptr_) - offsetof(type_, member_)))

#define ECU_NTNODE_GET_CONST_ENTRY(ptr_, type_, member_) \
    ((const type_ *)((const uint8_t *)(ptr_) - offsetof(type_, member_)))

/*------------------------------------------------------------*/
/*---------------------------- NTREE -------------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode
{
    struct ecu_dlist children;
    struct ecu_dnode dnode;
    struct ecu_ntnode *parent;
    void (*destroy)(struct ecu_ntnode *me, ecu_object_id id); // wrap this around dnode destroy.
};

struct ecu_ntree
{
    struct ecu_ntnode root;
};

/*------------------------------------------------------------*/
/*--------------------- NTNODE CHILD ITERATOR ----------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode_child_iterator
{
    struct ecu_dlist_iterator iterator;
};

struct ecu_ntnode_child_citerator
{
    struct ecu_dlist_const_iterator iterator;
};

/*------------------------------------------------------------*/
/*------------------- NTNODE POSTORDER ITERATOR --------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode_postorder_iterator
{

};

struct ecu_ntnode_postorder_citerator
{

};

/*------------------------------------------------------------*/
/*----------------- NTREE NODE MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Ntree Node Constructors
 */
/**@{*/
extern void ecu_ntnode_ctor(struct ecu_ntnode *me,
                            void (*destroy)(struct ecu_ntnode *me, ecu_object_id id),
                            ecu_object_id id);
extern void ecu_ntnode_destroy(struct ecu_ntnode *me);
/**@}*/

/**
 * @name Ntree Node Member Functions
 */
/**@{*/
// NULL returned if no parent (node is root or not in tree).
extern struct ecu_ntnode *ecu_ntnode_parent(struct ecu_ntnode *me);
extern const struct ecu_ntnode *ecu_ntnode_cparent(const struct ecu_ntnode *me);

// return next and prev sibling. NULL returned if no siblings.
extern struct ecu_ntnode *ecu_ntnode_next(struct ecu_ntnode *me);
extern const struct ecu_ntnode *ecu_ntnode_cnext(const struct ecu_ntnode *me);
extern struct ecu_ntnode *ecu_ntnode_prev(struct ecu_ntnode *me);
extern const struct ecu_ntnode *ecu_ntnode_cprev(const struct ecu_ntnode *me);

// return first and last child.
extern struct ecu_ntnode *ecu_ntnode_front(struct ecu_ntnode *me);
extern const struct ecu_ntnode *ecu_ntnode_cfront(const struct ecu_ntnode *me);
extern struct ecu_ntnode *ecu_ntnode_back(struct ecu_ntnode *me);
extern const struct ecu_ntnode *ecu_ntnode_cback(const struct ecu_ntnode *me);

// insert sibling.
extern void ecu_ntnode_insert_before(struct ecu_ntnode *me, struct ecu_ntnode *sibling);
extern void ecu_ntnode_insert_after(struct ecu_ntnode *me, struct ecu_ntnode *sibling);

// insert child
extern void ecu_ntnode_push_front(struct ecu_ntnode *me, struct ecu_ntnode *child);
extern void ecu_ntnode_push_back(struct ecu_ntnode *me, struct ecu_ntnode *child);

// remove node
extern void ecu_ntnode_remove(struct ecu_ntnode *me);

// misc
extern struct ecu_ntnode *ecu_ntnode_lca(struct ecu_ntnode *n1, struct ecu_ntnode *n2);
extern bool ecu_ntnode_in_tree(const struct ecu_ntnode *me);
extern ecu_object_id ecu_ntnode_get_id(const struct ecu_ntnode *me); // can be thought of as key.
extern size_t ecu_ntnode_size(const struct ecu_ntnode *me); // return total number of children, grandchildren, etc.
extern size_t ecu_ntnode_count(const struct ecu_ntnode *me); // return total number of direct children.
/**@}*/

/*------------------------------------------------------------*/
/*-------------------- NTREE MEMBER FUNCTIONS ----------------*/
/*------------------------------------------------------------*/

/**
 * @name Ntree Constructors
 */
/**@{*/
extern void ecu_ntree_ctor(struct ecu_ntree *me);
extern void ecu_ntree_destroy(struct ecu_ntree *me);
/**@}*/

/**
 * @name Ntree Member Functions
 */
/**@{*/
extern void ecu_ntree_clear(struct ecu_ntree *me);
extern struct ecu_ntnode *ecu_ntree_find(ecu_object_id id);

// inserts child below root node.
extern void ecu_ntree_push_front(struct ecu_ntree *me, struct ecu_ntnode *node);
extern void ecu_ntree_push_back(struct ecu_ntree *me, struct ecu_ntnode *node);

// level starts at 0. assert if level > tree depth.
extern void ecu_ntree_insert_front(struct ecu_ntree *me, struct ecu_ntnode *node, size_t level);
extern void ecu_ntree_insert_back(struct ecu_ntree *me, struct ecu_ntnode *node, size_t level);
extern bool ecu_ntree_empty(const struct ecu_ntree *me);
extern size_t ecu_ntree_depth(const struct ecu_ntree *me);
extern size_t ecu_ntree_size(const struct ecu_ntree *me);
/**@}*/

/*------------------------------------------------------------*/
/*----------- NTNODE CHILD ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

/**
 * @name Ntnode Child Iterator Member Functions
 */
/**@{*/
extern struct ecu_ntnode *ecu_ntnode_child_iterator_begin(struct ecu_ntnode_child_iterator *me, 
                                                          struct ecu_ntnode *node);
extern struct ecu_ntnode *ecu_ntnode_child_iterator_end(struct ecu_ntnode_child_iterator *me);
extern struct ecu_ntnode *ecu_ntnode_child_iterator_next(struct ecu_ntnode_child_iterator *me);
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_iterator *me, 
                                                                 const struct ecu_ntnode *node);
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_iterator *me);
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cnext(struct ecu_ntnode_child_iterator *me);
/**@}*/

/**
 * @name Ntnode Postorder Iterator Member Functions
 */
/**@{*/

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* NTREE_H_ */
