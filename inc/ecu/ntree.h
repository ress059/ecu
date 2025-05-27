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
#include <stdint.h>

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

#define ECU_NTNODE_SIBLING_AT_FOR_EACH(var_, iter_, start_)                     \
    for (struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_at(iter_, start_); \
         var_ != ecu_ntnode_child_iterator_end(iter_);                          \
         var = ecu_ntnode_child_iterator_next(iter_))

#define ECU_NTNODE_CONST_SIBLING_AT_FOR_EACH(var_, citer_, start_)                      \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_cat(citer_, start_); \
         var_ != ecu_ntnode_child_iterator_cend(iter_);                                 \
         var = ecu_ntnode_child_iterator_cnext(iter_))

#define ECU_NTNODE_CHILD_FOR_EACH(var_, iter_, parent_)                             \
    for (struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_begin(iter_, parent_); \
         var_ != ecu_ntnode_child_iterator_end(iter_);                              \
         var_ = end_ntnode_child_iterator_next(iter_))

#define ECU_NTNODE_CONST_CHILD_FOR_EACH(var_, citer_, parent_)                              \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_cbegin(citer_, parent_); \
         var_ != ecu_ntnode_child_iterator_cend(citer_);                                    \
         var_ = end_ntnode_child_iterator_cnext(citer_))

// root is iterated over.
#define ECU_NTNODE_PREORDER_FOR_EACH(var_, iter_, root_)                                \
    for (struct ecu_ntnode *var_ = ecu_ntnode_preorder_iterator_begin(iter_, root_);    \
         var_ != ecu_ntnode_preorder_iterator_end(iter_);                               \
         var_ = end_ntnode_preorder_iterator_next(iter_))

// root is iterated over.
#define ECU_NTNODE_CONST_PREORDER_FOR_EACH(var_, citer_, root_)                                 \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_preorder_iterator_cbegin(citer_, root_);    \
         var_ != ecu_ntnode_preorder_iterator_cend(citer_);                                     \
         var_ = end_ntnode_preorder_iterator_cnext(citer_))

#define ECU_NTNODE_POSTORDER_FOR_EACH(var_, iter_, root_)                               \
    for (struct ecu_ntnode *var_ = ecu_ntnode_postorder_iterator_begin(iter_, root_);   \
         var_ != ecu_ntnode_postorder_iterator_end(iter_);                              \
         var_ = end_ntnode_postorder_iterator_next(iter_))

#define ECU_NTNODE_CONST_POSTORDER_FOR_EACH(var_, citer_, root_)                                \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_postorder_iterator_cbegin(citer_, root_);   \
         var_ != ecu_ntnode_postorder_iterator_cend(citer_);                                    \
         var_ = end_ntnode_postorder_iterator_cnext(citer_))

/*------------------------------------------------------------*/
/*---------------------------- NTREE -------------------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode
{
    struct ecu_dnode dnode;
    struct ecu_dlist children;
    struct ecu_ntnode *parent;
    void (*destroy)(struct ecu_ntnode *me, ecu_object_id id); // wrap this around dnode destroy.
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
    struct ecu_dlist_citerator iterator;
};

/*------------------------------------------------------------*/
/*------------------- NTNODE PREORDER ITERATOR ---------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode_preorder_iterator
{
    struct ecu_ntnode delimiter;
    struct ecu_ntnode *root;
    struct ecu_ntnode *current;
};

struct ecu_ntnode_preorder_citerator
{
    struct ecu_ntnode delimiter;
    const struct ecu_ntnode *root;
    const struct ecu_ntnode *current;
};

/*------------------------------------------------------------*/
/*------------------- NTNODE POSTORDER ITERATOR --------------*/
/*------------------------------------------------------------*/

struct ecu_ntnode_postorder_iterator
{
    struct ecu_ntnode delimiter;
    struct ecu_ntnode *root;
    struct ecu_ntnode *current;
    struct ecu_ntnode *next;
};

struct ecu_ntnode_postorder_citerator
{
    struct ecu_ntnode delimiter;
    const struct ecu_ntnode *root;
    const struct ecu_ntnode *current;
    const struct ecu_ntnode *next;
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

// destroys all nodes in subtree too.
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
extern void ecu_ntnode_clear(struct ecu_ntnode *me);

// misc
bool ecu_ntnode_is_root(const struct ecu_ntnode *me);
extern bool ecu_ntnode_in_subtree(const struct ecu_ntnode *me);
extern ecu_object_id ecu_ntnode_get_id(const struct ecu_ntnode *me); // can be thought of as key.
extern size_t ecu_ntnode_count(const struct ecu_ntnode *me); // return total number of direct children.
extern size_t ecu_ntnode_level(const struct ecu_ntnode *me); // return which level the node is in. level starts at 0. i.e. root == level 0.
extern size_t ecu_ntnode_size(const struct ecu_ntnode *me); // return total number of children, grandchildren, etc. If node empty returns 1 (just this node).
// return NULL if not found.
extern struct ecu_ntnode *ecu_ntnode_find(struct ecu_ntnode *me, 
                                          bool (*found)(const struct ecu_ntnode *me, void *obj),
                                          void *obj);
extern const struct ecu_ntnode *ecu_ntnode_cfind(struct ecu_ntnode *me, 
                                                 bool (*found)(const struct ecu_ntnode *me, void *obj),
                                                 void *obj);

// return NULL if no LCA.
extern struct ecu_ntnode *ecu_ntnode_lca(struct ecu_ntnode *n1, struct ecu_ntnode *n2);
extern const struct ecu_ntnode *ecu_ntnode_clca(const struct ecu_ntnode *n1, const struct ecu_ntnode *n2);

/**@}*/

/*------------------------------------------------------------*/
/*----------- NTNODE CHILD ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

/**
 * @name Ntnode Child Iterator Member Functions
 */
/**@{*/
extern struct ecu_ntnode *ecu_ntnode_child_iterator_begin(struct ecu_ntnode_child_iterator *me, 
                                                          struct ecu_ntnode *parent);
extern struct ecu_ntnode *ecu_ntnode_child_iterator_at(struct ecu_ntnode_child_iterator *me,
                                                       struct ecu_ntnode *start);
extern struct ecu_ntnode *ecu_ntnode_child_iterator_end(struct ecu_ntnode_child_iterator *me);
extern struct ecu_ntnode *ecu_ntnode_child_iterator_next(struct ecu_ntnode_child_iterator *me);
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_citerator *me, 
                                                                 const struct ecu_ntnode *parent);
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cat(struct ecu_ntnode_child_citerator *me,
                                                              const struct ecu_ntnode *start);
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_citerator *me);
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cnext(struct ecu_ntnode_child_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*-------- NTNODE PREORDER ITERATOR MEMBER FUNCTIONS ---------*/
/*------------------------------------------------------------*/

/**
 * @name Ntnode Preorder Iterator Member Functions
 */
/**@{*/
extern struct ecu_ntnode *ecu_ntnode_preorder_iterator_begin(struct ecu_ntnode_preorder_iterator *me,
                                                             struct ecu_ntnode *root);
extern struct ecu_ntnode *ecu_ntnode_preorder_iterator_end(struct ecu_ntnode_preorder_iterator *me);
extern struct ecu_ntnode *ecu_ntnode_preorder_iterator_next(struct ecu_ntnode_preorder_iterator *me);

extern const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cbegin(struct ecu_ntnode_preorder_citerator *me,
                                                                    const struct ecu_ntnode *root);
extern const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cend(struct ecu_ntnode_preorder_citerator *me);
extern const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cnext(struct ecu_ntnode_preorder_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*-------- NTNODE POSTORDER ITERATOR MEMBER FUNCTIONS --------*/
/*------------------------------------------------------------*/

/**
 * @name Ntnode Postorder Iterator Member Functions
 */
/**@{*/
extern struct ecu_ntnode *ecu_ntnode_postorder_iterator_begin(struct ecu_ntnode_postorder_iterator *me,
                                                              struct ecu_ntnode *root);
extern struct ecu_ntnode *ecu_ntnode_postorder_iterator_end(struct ecu_ntnode_postorder_iterator *me);
extern struct ecu_ntnode *ecu_ntnode_postorder_iterator_next(struct ecu_ntnode_postorder_iterator *me);

extern const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cbegin(struct ecu_ntnode_postorder_citerator *me,
                                                                     const struct ecu_ntnode *root);
extern const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cend(struct ecu_ntnode_postorder_citerator *me);
extern const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cnext(struct ecu_ntnode_postorder_citerator *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* NTREE_H_ */
