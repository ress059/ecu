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

#ifndef ECU_NTNODE_H_
#define ECU_NTNODE_H_

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
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Convenience macro passed to API if optional
 * callback objects are unused.
 */
#define ECU_NTNODE_OBJ_UNUSED \
    ((void *)0)
    
/**
 * @brief Convenience macro passed to @ref ecu_ntnode_ctor()
 * if a user-defined node destructor is not needed.
 */
#define ECU_NTNODE_DESTROY_UNUSED \
    ((void (*)(struct ecu_ntnode *, ecu_object_id))0)

/**
 * @brief Retrieves user data from an intrusive @ref ecu_ntnode 
 * by converting the supplied @ref ecu_ntnode back into the 
 * user's node type.
 *
 * @param ptr_ Pointer to intrusive @ref ecu_ntnode.
 * This must be pointer to non-const. I.e. (struct ecu_ntnode *).
 * @param type_ User's node type containing the intrusive
 * @ref ecu_ntnode. Do not use const specifier. I.e. (struct my_type),
 * never (const struct my_type).
 * @param member_ Name of @ref ecu_ntnode member within user's
 * type.
 */
#define ECU_NTNODE_GET_ENTRY(ptr_, type_, member_) \
    ECU_CONTAINER_OF(ptr_, type_, member_)

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_GET_ENTRY().
 * Retrieves user data from an intrusive @ref ecu_ntnode 
 * by converting the supplied @ref ecu_ntnode back into the 
 * user's node type.
 *
 * @param ptr_ Pointer to intrusive @ref ecu_ntnode. This can be
 * pointer to const or non-const. I.e. (struct ecu_ntnode *) or 
 * (const struct ecu_ntnode *).
 * @param type_ User's node type containing the intrusive
 * @ref ecu_ntnode. Do not use const specifier. I.e. (struct my_type),
 * never (const struct my_type).
 * @param member_ Name of @ref ecu_ntnode member within user's
 * type.
 */
#define ECU_NTNODE_GET_CONST_ENTRY(ptr_, type_, member_) \
    ECU_CONST_CONTAINER_OF(ptr_, type_, member_)

/**
 * @brief Iterates (for-loops) over all direct children. Grandchildren, 
 * great-grandchildren, etc are not included in the iteration. 
 * It is safe to remove or destroy the current node in the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_child_iterator.
 * @param parent_ The children of this node will be iterated over.
 * This will be a pointer to @ref ecu_ntnode. 
 */
#define ECU_NTNODE_CHILD_FOR_EACH(var_, iter_, parent_)                             \
    for (struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_begin(iter_, parent_); \
         var_ != ecu_ntnode_child_iterator_end(iter_);                              \
         var_ = ecu_ntnode_child_iterator_next(iter_))

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_CHILD_FOR_EACH().
 * Iterates (for-loops) over all direct children. Grandchildren, 
 * great-grandchildren, etc are not included in the iteration.
 * Returned nodes are read-only.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_child_citerator.
 * @param parent_ The children of this node will be iterated over.
 * This will be a pointer to @ref ecu_ntnode. 
 */
#define ECU_NTNODE_CONST_CHILD_FOR_EACH(var_, citer_, parent_)                              \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_cbegin(citer_, parent_); \
         var_ != ecu_ntnode_child_iterator_cend(citer_);                                    \
         var_ = ecu_ntnode_child_iterator_cnext(citer_))

/**
 * @brief Iterates (for-loops) over all next (right) siblings,
 * including the supplied starting node. Terminates after the 
 * last (rightmost) sibling is reached. It is safe to remove 
 * or destroy the current node in the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_next_sibling_iterator.
 * @param start_ Starting node of the iteration. This node IS
 * included in the iteration and is returned first. This will
 * be a pointer to @ref ecu_ntnode.
 */
#define ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(var_, iter_, start_)                        \
    for (struct ecu_ntnode *var_ = ecu_ntnode_next_sibling_iterator_at(iter_, start_);  \
         var_ != ecu_ntnode_next_sibling_iterator_end(iter_);                           \
         var_ = ecu_ntnode_next_sibling_iterator_next(iter_))

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH().
 * Iterates (for-loops) over all next (right) siblings,
 * including the supplied starting node. Terminates after the 
 * last (rightmost) sibling is reached. Returned nodes are read-only.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_next_sibling_citerator.
 * @param start_ Starting node of the iteration. This node IS
 * included in the iteration and is returned first. This will
 * be a pointer to const @ref ecu_ntnode.
 */
#define ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(var_, citer_, start_)                          \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_next_sibling_iterator_cat(citer_, start_);   \
         var_ != ecu_ntnode_next_sibling_iterator_cend(citer_);                                  \
         var_ = ecu_ntnode_next_sibling_iterator_cnext(citer_))

/**
 * @brief Same as @ref ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH()
 * but excludes the starting node from the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_next_sibling_iterator.
 * @param start_ Starting node of the iteration. This will
 * be a pointer to @ref ecu_ntnode. This node is NOT included 
 * in the iteration. The node next to this one is returned first. 
 */
#define ECU_NTNODE_NEXT_SIBLING_FOR_EACH(var_, iter_, start_)                               \
    for (struct ecu_ntnode *var_ = ecu_ntnode_next_sibling_iterator_begin(iter_, start_);   \
         var_ != ecu_ntnode_next_sibling_iterator_end(iter_);                               \
         var_ = ecu_ntnode_next_sibling_iterator_next(iter_))

/**
 * @brief Same as @ref ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH()
 * but excludes the starting node from the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_next_sibling_citerator.
 * @param start_ Starting node of the iteration. This will
 * be a pointer to const @ref ecu_ntnode. This node is NOT included 
 * in the iteration. The node next to this one is returned first. 
 */     
#define ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(var_, citer_, start_)                                \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_next_sibling_iterator_cbegin(citer_, start_);   \
         var_ != ecu_ntnode_next_sibling_iterator_cend(citer_);                                     \
         var_ = ecu_ntnode_next_sibling_iterator_cnext(citer_))

/**
 * @brief Iterates (for-loops) over all parents, including the supplied 
 * starting node, by traversing up the tree. It is safe to remove or 
 * destroy the current node in the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_parent_iterator.
 * @param start_ Starting node of the iteration. This node IS
 * included in the iteration and is returned first. This will
 * be a pointer to @ref ecu_ntnode.
 */
#define ECU_NTNODE_PARENT_AT_FOR_EACH(var_, iter_, start_)                          \
    for (struct ecu_ntnode *var_ = ecu_ntnode_parent_iterator_at(iter_, start_);    \
         var_ != ecu_ntnode_parent_iterator_end(iter_);                             \
         var_ = ecu_ntnode_parent_iterator_next(iter_))

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_PARENT_AT_FOR_EACH().
 * Iterates (for-loops) over all parents, including the supplied 
 * starting node, by traversing up the tree. Returned nodes are 
 * read-only.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_parent_citerator.
 * @param start_ Starting node of the iteration. This node IS
 * included in the iteration and is returned first. This will
 * be a pointer to const @ref ecu_ntnode.
 */
#define ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(var_, citer_, start_)                           \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_parent_iterator_cat(citer_, start_);    \
         var_ != ecu_ntnode_parent_iterator_cend(citer_);                                   \
         var_ = ecu_ntnode_parent_iterator_cnext(citer_))

/**
 * @brief Same as @ref ECU_NTNODE_PARENT_AT_FOR_EACH() but excludes
 * the starting node from the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_parent_iterator.
 * @param start_ Starting node of the iteration. This node is
 * NOT included in the iteration. This will be a pointer to
 * @ref ecu_ntnode.
 */
#define ECU_NTNODE_PARENT_FOR_EACH(var_, iter_, start_)                             \
    for (struct ecu_ntnode *var_ = ecu_ntnode_parent_iterator_begin(iter_, start_); \
         var_ != ecu_ntnode_parent_iterator_end(iter_);                             \
         var_ = ecu_ntnode_parent_iterator_next(iter_))

/**
 * @brief Same as @ref ECU_NTNODE_CONST_PARENT_AT_FOR_EACH() 
 * but excludes the starting node from the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_parent_citerator.
 * @param start_ Starting node of the iteration. This node is
 * NOT included in the iteration. This will be a pointer to
 * const @ref ecu_ntnode.
 */
#define ECU_NTNODE_CONST_PARENT_FOR_EACH(var_, citer_, start_)                              \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_parent_iterator_cbegin(citer_, start_); \
         var_ != ecu_ntnode_parent_iterator_cend(citer_);                                   \
         var_ = ecu_ntnode_parent_iterator_cnext(citer_))

/**
 * @brief Performs a postorder iteration (for-loop) over a tree. 
 * The root node is included in the iteration. It is safe to remove 
 * or destroy the current node in the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_postorder_iterator.
 * @param root_ Tree to iterate over. By design, this will 
 * always be the root of a subtree or the main root. This node 
 * will be included in the iteration, and will be a pointer
 * to @ref ecu_ntnode.
 */
#define ECU_NTNODE_POSTORDER_FOR_EACH(var_, iter_, root_)                               \
    for (struct ecu_ntnode *var_ = ecu_ntnode_postorder_iterator_begin(iter_, root_);   \
         var_ != ecu_ntnode_postorder_iterator_end(iter_);                              \
         var_ = ecu_ntnode_postorder_iterator_next(iter_))

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_POSTORDER_FOR_EACH().
 * Performs a postorder iteration (for-loop) over a tree. 
 * The root node is included in the iteration. Returned nodes 
 * are read-only.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_postorder_citerator.
 * @param root_ Tree to iterate over. By design, this will 
 * always be the root of a subtree or the main root. This node 
 * will be included in the iteration, and will be a pointer
 * to const @ref ecu_ntnode.
 */
#define ECU_NTNODE_CONST_POSTORDER_FOR_EACH(var_, citer_, root_)                                \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_postorder_iterator_cbegin(citer_, root_);   \
         var_ != ecu_ntnode_postorder_iterator_cend(citer_);                                    \
         var_ = ecu_ntnode_postorder_iterator_cnext(citer_))

/**
 * @brief Performs a preorder iteration (for-loop) over a tree. 
 * The root node is included in the iteration. Removing or destroying 
 * the current node in the iteration is NOT allowed since this 
 * is an unsafe operation during preorder traversal.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_preorder_iterator.
 * @param root_ Tree to iterate over. By design, this will 
 * always be the root of a subtree or the main root. This node 
 * will be included in the iteration, and will be a pointer
 * to @ref ecu_ntnode.
 */
#define ECU_NTNODE_PREORDER_FOR_EACH(var_, iter_, root_)                                \
    for (struct ecu_ntnode *var_ = ecu_ntnode_preorder_iterator_begin(iter_, root_);    \
         var_ != ecu_ntnode_preorder_iterator_end(iter_);                               \
         var_ = ecu_ntnode_preorder_iterator_next(iter_))

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_PREORDER_FOR_EACH().
 * Performs a preorder iteration (for-loop) over a tree. 
 * The root node is included in the iteration. Returned 
 * nodes are read-only.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_preorder_citerator.
 * @param root_ Tree to iterate over. By design, this will 
 * always be the root of a subtree or the main root. This node 
 * will be included in the iteration, and will be a pointer
 * to const @ref ecu_ntnode.
 */
#define ECU_NTNODE_CONST_PREORDER_FOR_EACH(var_, citer_, root_)                                 \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_preorder_iterator_cbegin(citer_, root_);    \
         var_ != ecu_ntnode_preorder_iterator_cend(citer_);                                     \
         var_ = ecu_ntnode_preorder_iterator_cnext(citer_))

/**
 * @brief Iterates (for-loops) over all previous (left) siblings, 
 * including the supplied starting node. Terminates after the
 * first (leftmost) sibling is reached. It is safe to remove 
 * or destroy the current node in the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_prev_sibling_iterator.
 * @param start_ Starting node of the iteration. This node IS
 * included in the iteration and is returned first. This will
 * be a pointer to @ref ecu_ntnode.
 */
#define ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH(var_, iter_, start_)                        \
    for (struct ecu_ntnode *var_ = ecu_ntnode_prev_sibling_iterator_at(iter_, start_);  \
         var_ != ecu_ntnode_prev_sibling_iterator_end(iter_);                           \
         var_ = ecu_ntnode_prev_sibling_iterator_next(iter_))

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH().
 * Iterates (for-loops) over all previous (left) siblings, 
 * including the supplied starting node. Terminates after the
 * first (leftmost) sibling is reached. Returned nodes are read-only.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_prev_sibling_citerator.
 * @param start_ Starting node of the iteration. This node IS
 * included in the iteration and is returned first. This will
 * be a pointer to const @ref ecu_ntnode.
 */
#define ECU_NTNODE_CONST_PREV_SIBLING_AT_FOR_EACH(var_, citer_, start_)                          \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_prev_sibling_iterator_cat(citer_, start_);   \
         var_ != ecu_ntnode_prev_sibling_iterator_cend(citer_);                                  \
         var_ = ecu_ntnode_prev_sibling_iterator_cnext(citer_))

/**
 * @brief Same as @ref ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH()
 * but excludes the starting node from the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_prev_sibling_iterator.
 * @param start_ Starting node of the iteration. This will
 * be a pointer to @ref ecu_ntnode. This node is NOT included 
 * in the iteration. The node previous to this one is returned first. 
 */
#define ECU_NTNODE_PREV_SIBLING_FOR_EACH(var_, iter_, start_)                               \
    for (struct ecu_ntnode *var_ = ecu_ntnode_prev_sibling_iterator_begin(iter_, start_);   \
         var_ != ecu_ntnode_prev_sibling_iterator_end(iter_);                               \
         var_ = ecu_ntnode_prev_sibling_iterator_next(iter_))

/**
 * @brief Same as @ref ECU_NTNODE_CONST_PREV_SIBLING_AT_FOR_EACH()
 * but excludes the starting node from the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer
 * to @ref ecu_ntnode_prev_sibling_citerator.
 * @param start_ Starting node of the iteration. This will
 * be a pointer to const @ref ecu_ntnode. This node is NOT included 
 * in the iteration. The node previous to this one is returned first. 
 */     
#define ECU_NTNODE_CONST_PREV_SIBLING_FOR_EACH(var_, citer_, start_)                                \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_prev_sibling_iterator_cbegin(citer_, start_);   \
         var_ != ecu_ntnode_prev_sibling_iterator_cend(citer_);                                     \
         var_ = ecu_ntnode_prev_sibling_iterator_cnext(citer_))

/**
 * @brief Iterates over all siblings, excluding the starting 
 * node. Performs wraparound if starting sibling is not the 
 * first sibling. It is safe to remove or destroy the current 
 * node in the iteration.
 * 
 * @note If the starting node must be included in the iteration
 * use a child iterator instead. SIBLING_AT() will not be supported
 * since it is impossible to determine when to terminate the
 * iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_ntnode.
 * @param iter_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_sibling_iterator.
 * @param start_ Starting node of the iteration. This node is
 * NOT included in the iteration. This will be a pointer to 
 * @ref ecu_ntnode.
 */
#define ECU_NTNODE_SIBLING_FOR_EACH(var_, iter_, start_)                                \
    for (struct ecu_ntnode *var_ = ecu_ntnode_sibling_iterator_begin(iter_, start_);    \
         var_ != ecu_ntnode_sibling_iterator_end(iter_);                                \
         var_ = ecu_ntnode_sibling_iterator_next(iter_))

/**
 * @brief Const-qualified version of @ref ECU_NTNODE_SIBLING_FOR_EACH().
 * Iterates over all siblings, excluding the starting 
 * node. Performs wraparound if starting sibling is not the 
 * first sibling.
 * 
 * @note If the starting node must be included in the iteration
 * use a child iterator instead. SIBLING_AT() will not be supported
 * since it is impossible to determine when to terminate the
 * iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_ntnode.
 * @param citer_ Iterator to initialize. This will be a pointer 
 * to @ref ecu_ntnode_sibling_citerator.
 * @param start_ Starting node of the iteration. This node is
 * NOT included in the iteration. This will be a pointer to
 * const @ref ecu_ntnode.
 */
#define ECU_NTNODE_CONST_SIBLING_FOR_EACH(var_, citer_, start_)                                 \
    for (const struct ecu_ntnode *var_ = ecu_ntnode_sibling_iterator_cbegin(citer_, start_);    \
         var_ != ecu_ntnode_sibling_iterator_cend(citer_);                                      \
         var_ = ecu_ntnode_sibling_iterator_cnext(citer_))

/*------------------------------------------------------------*/
/*--------------------------- NTNODE -------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Single node within a tree. Intrusive, so
 * user-defined types contain this node as a member.
 * This type represents both a root and regular tree node. 
 * At any point in time, every node will either be a root 
 * of a subtree (normal tree node) or the main root. 
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode
{
    /// @brief This node, represented as a linked list node.
    struct ecu_dnode dnode;

    /// @brief Node's children, represented as a linked list.
    struct ecu_dlist children;

    /// @brief Node's parent.
    struct ecu_ntnode *parent;

    /// @brief Optional user-defined node destructor. Executes
    /// when @ref ecu_ntnode_destroy() is called or if this
    /// node was in a tree that was destroyed by @ref ecu_ntnode_destroy().
    void (*destroy)(struct ecu_ntnode *me, ecu_object_id id);
};

/*------------------------------------------------------------*/
/*--------------------- NTNODE CHILD ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const child iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_child_iterator
{
    /// @brief Current node in the iteration.
    struct ecu_ntnode *current;

    /// @brief Next node in the iteration to allow safe removal
    /// of current node.
    struct ecu_ntnode *next;
};

/**
 * @brief Const child iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_child_citerator
{
    /// @brief Current node in the iteration.
    const struct ecu_ntnode *current;

    /// @brief Next node in the iteration. Kept to remain 
    /// consistent with non-const iterator implementation.
    const struct ecu_ntnode *next;
};

/*------------------------------------------------------------*/
/*-------------------- NTNODE PARENT ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const parent iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_parent_iterator
{
    /// @brief Current node in the iteration.
    struct ecu_ntnode *current;

    /// @brief Next node in the iteration to allow safe removal
    /// of current node.
    struct ecu_ntnode *next;
};

/**
 * @brief Const parent iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_parent_citerator
{
    /// @brief Current node in the iteration.
    const struct ecu_ntnode *current;

    /// @brief Next node in the iteration. Kept to remain 
    /// consistent with non-const iterator implementation.
    const struct ecu_ntnode *next;
};

/*------------------------------------------------------------*/
/*--------------- NTNODE NEXT SIBLING ITERATOR ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const next sibling iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_next_sibling_iterator
{
    /// @brief Current node in the iteration.
    struct ecu_ntnode *current;

    /// @brief Next node in the iteration to allow safe removal
    /// of current node.
    struct ecu_ntnode *next;
};

/**
 * @brief Const next sibling iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_next_sibling_citerator
{
    /// @brief Current node in the iteration.
    const struct ecu_ntnode *current;

    /// @brief Next node in the iteration. Kept to remain 
    /// consistent with non-const iterator implementation.
    const struct ecu_ntnode *next;
};

/*------------------------------------------------------------*/
/*------------------- NTNODE POSTORDER ITERATOR --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const postorder iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_postorder_iterator
{
    /// @brief Dummy delimiter returned at the end of the iteration.
    /// Not apart of user's tree. Allows iteration over entire tree.
    struct ecu_ntnode delimiter;

    /// @brief Root of tree being iterated over. Included
    /// in iteration.
    struct ecu_ntnode *root;

    /// @brief Current node in the iteration.
    struct ecu_ntnode *current;

    /// @brief Next node in the iteration to allow safe removal
    /// of current node.
    struct ecu_ntnode *next;
};

/**
 * @brief Const postorder iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_postorder_citerator
{
    /// @brief Dummy delimiter returned at the end of the iteration.
    /// Not apart of user's tree. Allows iteration over entire tree.
    struct ecu_ntnode delimiter;

    /// @brief Root of tree being iterated over. Included
    /// in iteration.
    const struct ecu_ntnode *root;

    /// @brief Current node in the iteration.
    const struct ecu_ntnode *current;

    /// @brief Next node in the iteration. Kept to remain 
    /// consistent with non-const iterator implementation.
    const struct ecu_ntnode *next;
};

/*------------------------------------------------------------*/
/*------------------- NTNODE PREORDER ITERATOR ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const preorder iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_preorder_iterator
{
    /// @brief Dummy delimiter returned at the end of the iteration.
    /// Not apart of user's tree. Allows iteration over entire tree.
    struct ecu_ntnode delimiter;

    /// @brief Root of tree being iterated over. Included
    /// in iteration.
    struct ecu_ntnode *root;

    /// @brief Current node in the iteration.
    struct ecu_ntnode *current;
};

/**
 * @brief Const preorder iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_preorder_citerator
{
    /// @brief Dummy delimiter returned at the end of the iteration.
    /// Not apart of user's tree. Allows iteration over entire tree.
    struct ecu_ntnode delimiter;

    /// @brief Root of tree being iterated over. Included
    /// in iteration.
    const struct ecu_ntnode *root;

    /// @brief Current node in the iteration.
    const struct ecu_ntnode *current;
};

/*------------------------------------------------------------*/
/*--------------- NTNODE PREV SIBLING ITERATOR ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const previous sibling iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_prev_sibling_iterator
{
    /// @brief Current node in the iteration.
    struct ecu_ntnode *current;

    /// @brief Next node in the iteration to allow safe removal
    /// of current node.
    struct ecu_ntnode *next;
};

/**
 * @brief Const previous sibling iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_prev_sibling_citerator
{
    /// @brief Current node in the iteration.
    const struct ecu_ntnode *current;

    /// @brief Next node in the iteration. Kept to remain 
    /// consistent with non-const iterator implementation.
    const struct ecu_ntnode *next;
};

/*------------------------------------------------------------*/
/*-------------------- NTNODE SIBLING ITERATOR ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const sibling iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_sibling_iterator
{
    /// @brief Starting node of iteration which is also the 
    /// ending node. Not included in iteration to allow safe 
    /// removal of current node.
    struct ecu_ntnode *end;

    /// @brief Current node in the iteration.
    struct ecu_ntnode *current;

    /// @brief Next node in the iteration to allow safe removal
    /// of current node.
    struct ecu_ntnode *next;
};

/**
 * @brief Const sibling iterator.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_ntnode_sibling_citerator
{
    /// @brief Starting node of iteration which is also the 
    /// ending node. Not included in iteration to allow safe 
    /// removal of current node.
    const struct ecu_ntnode *end;

    /// @brief Current node in the iteration.
    const struct ecu_ntnode *current;

    /// @brief Next node in the iteration. Kept to remain 
    /// consistent with non-const iterator implementation.
    const struct ecu_ntnode *next;
};

/*------------------------------------------------------------*/
/*------------------ NTNODE MEMBER FUNCTIONS -----------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Node constructor.
 *
 * @warning @p me must not be an active node within a tree, otherwise behavior
 * is undefined.
 *
 * @param me Node to construct. This cannot be NULL.
 * @param destroy Optional callback. Defines any <b>additional</b> cleanup
 * needed to fully destroy this user-defined node. Do not use API calls that edit
 * the ecu_ntnode (insert, remove, etc) within this callback. Doing so is undefined
 * behavior. Executes when node is destroyed via @ref ecu_ntnode_destroy(), or
 * if this node was in a tree that was destroyed by @ref ecu_ntnode_destroy().
 * Supply @ref ECU_NTNODE_DESTROY_UNUSED if unused.
 * @param id Optional ID to assign to node. Used to identify different user-defined
 * types stored in the same tree. Supply @ref ECU_OBJECT_ID_UNUSED if unused. 
 * Otherwise this value must be greater than or equal to @ref ECU_VALID_OBJECT_ID_BEGIN
 */
extern void ecu_ntnode_ctor(struct ecu_ntnode *me,
                            void (*destroy)(struct ecu_ntnode *me, ecu_object_id id),
                            ecu_object_id id);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Node destructor. Destroys the supplied node and
 * all of its descendants (children, grandchildren, etc).
 * All destroyed nodes must be reconstructed in order to be
 * used again.
 *
 * @warning Memory is not freed since ECU is meant to be used without
 * dynamic memory allocation. If the destroyed nodes were allocated
 * on the heap, user is responsible for freeing memory. It is
 * recommended to free the nodes inside the destroy callbacks passed to
 * @ref ecu_ntnode_ctor().
 *
 * @param me Node to destroy. This node's subtree will also
 * be destroyed.
 */
extern void ecu_ntnode_destroy(struct ecu_ntnode *me);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Removes the supplied node and all of its descendants from
 * a tree. All nodes can be reused and added to another tree without 
 * reconstruction.
 * 
 * @param me Node to remove. Its entire subtree will also be removed.
 */
extern void ecu_ntnode_clear(struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the number of direct children the supplied node
 * has. Grandchildren, great-granchildren, etc are not counted.
 * Returns 0 if the node has no children.
 * 
 * @param me Node to check.
 */
extern size_t ecu_ntnode_count(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the node's first (leftmost) child. NULL is returned
 * if the node has no children.
 * 
 * @param me Node to check.
 */
extern struct ecu_ntnode *ecu_ntnode_first_child(struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_first_child().
 * Returns the node's first (leftmost) child. NULL is returned
 * if the node has no children.
 * 
 * @param me Node to check.
 */
extern const struct ecu_ntnode *ecu_ntnode_first_cchild(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the node's ID assigned when it was constructed
 * in @ref ecu_ntnode_ctor().
 * 
 * @param me Node to check.
 */
extern ecu_object_id ecu_ntnode_id(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns true if the node is in a tree. False otherwise.
 * Note that this returns true for a root node that has children.
 * 
 * @param me Node to check.
 */
extern bool ecu_ntnode_in_tree(const struct ecu_ntnode *me);

/**
 * @pre @p pos and @p sibling previously constructed via @ref ecu_ntnode_ctor().
 * @brief Inserts a sibling node into the tree after the specified position.
 * @p sibling becomes @p pos's next sibling.
 * 
 * @param pos Position node. Node is inserted after this position. 
 * This cannot be a root.
 * @param sibling Node to insert. This cannot already be within a tree
 * unless this is a root node. If this is a root of an existing tree,
 * its tree remains intact.
 */
extern void ecu_ntnode_insert_sibling_after(struct ecu_ntnode *pos, struct ecu_ntnode *sibling);

/**
 * @pre @p pos and @p sibling previously constructed via @ref ecu_ntnode_ctor().
 * @brief Inserts a sibling node into the tree before the specified position.
 * @p sibling becomes @p pos's previous sibling.
 * 
 * @param pos Position node. Node is inserted before this position. 
 * This cannot be a root.
 * @param sibling Node to insert. This cannot already be within a tree
 * unless this is a root node. If this is a root of an existing tree,
 * its tree remains intact.
 */
extern void ecu_ntnode_insert_sibling_before(struct ecu_ntnode *pos, struct ecu_ntnode *sibling);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns true if the node is in a tree and is not the
 * root. False otherwise.
 * 
 * @param me Node to check.
 */
extern bool ecu_ntnode_is_descendant(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns true if the node is a leaf, meaning it has no 
 * children. False otherwise. Note that this returns true for
 * an empty node since that is technically a leaf.
 * 
 * @param me Node to check.
 */
extern bool ecu_ntnode_is_leaf(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns true if the node is a root, meaning it has no
 * parent. False otherwise. Note that this returns true for an
 * empty node since that is techincally a root.
 * 
 * @param me Node to check.
 */
extern bool ecu_ntnode_is_root(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the node's last (rightmost) child. NULL is returned
 * if the node has no children.
 * 
 * @param me Node to check.
 */
extern struct ecu_ntnode *ecu_ntnode_last_child(struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_last_child().
 * Returns the node's last (rightmost) child. NULL is returned
 * if the node has no children.
 * 
 * @param me Node to check.
 */
extern const struct ecu_ntnode *ecu_ntnode_last_cchild(const struct ecu_ntnode *me);

/**
 * @pre @p n1 and @p n2 previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the least common ancestor of @p n1 and @p n2.
 * Returns NULL if nodes are in separate trees and do not have
 * an LCA.
 * 
 * @param n1 First node to check.
 * @param n2 Second node to check.
 */
extern struct ecu_ntnode *ecu_ntnode_lca(struct ecu_ntnode *n1, struct ecu_ntnode *n2);

/**
 * @pre @p n1 and @p n2 previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_lca().
 * Returns the least common ancestor of @p n1 and @p n2.
 * Returns NULL if nodes are in separate trees and do not have
 * an LCA.
 * 
 * @param n1 First node to check.
 * @param n2 Second node to check.
 */
extern const struct ecu_ntnode *ecu_ntnode_clca(const struct ecu_ntnode *n1, const struct ecu_ntnode *n2);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns which level of the tree the supplied node is in. 
 * Returns 0 if the supplied node is a root.
 * 
 * @param me Node to check.
 */
extern size_t ecu_ntnode_level(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the node's next (right) sibling. NULL is returned
 * if the node is the last (rightmost) sibling or has no siblings.
 * 
 * @param me Node to check.
 */
extern struct ecu_ntnode *ecu_ntnode_next(struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_next().
 * Returns the node's next (right) sibling. NULL is returned
 * if the node is the last (rightmost) sibling or has no siblings.
 * 
 * @param me Node to check.
 */
extern const struct ecu_ntnode *ecu_ntnode_cnext(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the supplied node's parent. NULL is returned
 * if the node is a root.
 * 
 * @param me Node to check.
 */
extern struct ecu_ntnode *ecu_ntnode_parent(struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_parent().
 * Returns the supplied node's parent. NULL is returned
 * if the node is a root.
 * 
 * @param me Node to check.
 */
extern const struct ecu_ntnode *ecu_ntnode_cparent(const struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the node's previous (left) sibling. NULL is returned
 * if the node is the first (leftmost) sibling or has no siblings.
 * 
 * @param me Node to check.
 */
extern struct ecu_ntnode *ecu_ntnode_prev(struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_prev().
 * Returns the node's previous (left) sibling. NULL is returned
 * if the node is the first (leftmost) sibling or has no siblings.
 * 
 * @param me Node to check.
 */
extern const struct ecu_ntnode *ecu_ntnode_cprev(const struct ecu_ntnode *me);

/**
 * @pre @p parent and @p child previously constructed via @ref ecu_ntnode_ctor().
 * @brief Inserts a rightmost child node into the tree. @p child 
 * becomes @p parent's last (rightmost) child.
 * 
 * @param parent Parent node to add child to.
 * @param child Node to insert. This will become @p parent's last child.
 * This cannot already be within a tree unless this is a root node.
 */
extern void ecu_ntnode_push_child_back(struct ecu_ntnode *parent, struct ecu_ntnode *child);

/**
 * @pre @p parent and @p child previously constructed via @ref ecu_ntnode_ctor().
 * @brief Inserts a leftmost child node into the tree. @p child 
 * becomes @p parent's first (leftmost) child.
 * 
 * @param parent Parent node to add child to.
 * @param child Node to insert. This will become @p parent's first child.
 * This cannot already be within a tree unless this is a root node.
 */
extern void ecu_ntnode_push_child_front(struct ecu_ntnode *parent, struct ecu_ntnode *child);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Removes node from a tree. If the removed node has descendants, 
 * the node becomes a new root with its tree intact. The tree remains 
 * unchanged if an empty or root node is supplied.
 * 
 * @param me Node to remove.
 */
extern void ecu_ntnode_remove(struct ecu_ntnode *me);

/**
 * @pre @p me previously constructed via @ref ecu_ntnode_ctor().
 * @brief Returns the total number of descendants (children, grandchildren,
 * etc) the node has. Returns 0 if the node has no descendants.
 * 
 * @param me Node to check.
 */
extern size_t ecu_ntnode_size(const struct ecu_ntnode *me);

/**
 * @brief Returns true if supplied node has been constructed
 * via @ref ecu_ntnode_ctor() and can be used. False otherwise.
 * 
 * @param me Node to check.
 */
extern bool ecu_ntnode_valid(const struct ecu_ntnode *me);
/**@}*/

/*------------------------------------------------------------*/
/*------------------------ CHILD ITERATOR --------------------*/
/*------------------------------------------------------------*/

/**
 * @name Child Iterator
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p parent previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over the supplied node's direct children.
 * Grandchildren, great-grandchildren, etc are not iterated over.
 * If the starting node has children its first (leftmost) child is
 * returned. Otherwise NULL is returned, signifying the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CHILD_FOR_EACH()
 * instead.
 * 
 * @param me Non-const child iterator to initialize.
 * @param parent Parent containing children to iterate over.
 */
extern struct ecu_ntnode *ecu_ntnode_child_iterator_begin(struct ecu_ntnode_child_iterator *me, 
                                                          struct ecu_ntnode *parent);

/**
 * @pre @p me started via @ref ecu_ntnode_child_iterator_begin().
 * @brief Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CHILD_FOR_EACH()
 * instead.
 * 
 * @param me Non-const child iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_child_iterator_end(struct ecu_ntnode_child_iterator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_child_iterator_begin().
 * @brief Returns the next child in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CHILD_FOR_EACH()
 * instead.
 * 
 * @param me Non-const child iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_child_iterator_next(struct ecu_ntnode_child_iterator *me);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p parent previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_child_iterator_begin().
 * Starts an iteration over the supplied node's direct children.
 * Grandchildren, great-grandchildren, etc are not iterated over.
 * If the starting node has children its first (leftmost) child is
 * returned. Otherwise NULL is returned, signifying the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_CHILD_FOR_EACH()
 * instead.
 * 
 * @param me Const child iterator to initialize.
 * @param parent Parent containing children to iterate over.
 */
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cbegin(struct ecu_ntnode_child_citerator *me, 
                                                                 const struct ecu_ntnode *parent);

/**
 * @pre @p me started via @ref ecu_ntnode_child_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_child_iterator_end().
 * Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_CHILD_FOR_EACH()
 * instead.
 * 
 * @param me Const child iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cend(struct ecu_ntnode_child_citerator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_child_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_child_iterator_next().
 * Returns the next child in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_CHILD_FOR_EACH()
 * instead.
 * 
 * @param me Const child iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_child_iterator_cnext(struct ecu_ntnode_child_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*------------------- NEXT SIBLING ITERATOR ------------------*/
/*------------------------------------------------------------*/

/**
 * @name Next Sibling Iterator
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over all next (right) siblings, including
 * the supplied starting node. The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH()
 * instead.
 * 
 * @param me Non-const next sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_at(struct ecu_ntnode_next_sibling_iterator *me,
                                                              struct ecu_ntnode *start);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over all next (right) siblings, excluding
 * the supplied starting node. If the starting node has siblings the next
 * (right) sibling to it is returned. Otherwise NULL is returned, signifying
 * the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_NEXT_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Non-const next sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_begin(struct ecu_ntnode_next_sibling_iterator *me,
                                                                 struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_next_sibling_iterator_at() or @ref ecu_ntnode_next_sibling_iterator_begin().
 * @brief Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_NEXT_SIBLING_FOR_EACH() instead.
 * 
 * @param me Non-const next sibling iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_end(struct ecu_ntnode_next_sibling_iterator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_next_sibling_iterator_at() or @ref ecu_ntnode_next_sibling_iterator_begin().
 * @brief Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_NEXT_SIBLING_FOR_EACH() instead.
 * 
 * @param me Non-const next sibling iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_next(struct ecu_ntnode_next_sibling_iterator *me);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_next_sibling_iterator_at().
 * Starts an iteration over all next (right) siblings, including
 * the supplied starting node. The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH()
 * instead.
 * 
 * @param me Const next sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_cat(struct ecu_ntnode_next_sibling_citerator *me,
                                                                     const struct ecu_ntnode *start);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_next_sibling_iterator_begin().
 * Starts an iteration over all next (right) siblings, excluding
 * the supplied starting node. If the starting node has siblings the next
 * (right) sibling to it is returned. Otherwise NULL is returned, signifying
 * the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Const next sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_cbegin(struct ecu_ntnode_next_sibling_citerator *me,
                                                                        const struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_next_sibling_iterator_cat() or @ref ecu_ntnode_next_sibling_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_next_sibling_iterator_end().
 * Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH() instead.
 * 
 * @param me Const next sibling iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_cend(struct ecu_ntnode_next_sibling_citerator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_next_sibling_iterator_cat() or @ref ecu_ntnode_next_sibling_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_next_sibling_iterator_next().
 * Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH() instead.
 * 
 * @param me Const next sibling iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_next_sibling_iterator_cnext(struct ecu_ntnode_next_sibling_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*------------------------ PARENT ITERATOR -------------------*/
/*------------------------------------------------------------*/

/**
 * @name Parent Iterator
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over all parents, including the supplied
 * starting node. The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PARENT_AT_FOR_EACH()
 * instead.
 * 
 * @param me Non-const parent iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_parent_iterator_at(struct ecu_ntnode_parent_iterator *me,
                                                        struct ecu_ntnode *start);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over all parents, excluding the supplied
 * starting node. If the starting node has parents, its parent is returned.
 * Otherwise NULL is returned, signifying the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PARENT_FOR_EACH()
 * instead.
 * 
 * @param me Non-const parent iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_parent_iterator_begin(struct ecu_ntnode_parent_iterator *me,
                                                           struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_parent_iterator_at() or @ref ecu_ntnode_parent_iterator_begin().
 * @brief Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PARENT_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_PARENT_FOR_EACH() instead.
 * 
 * @param me Non-const parent iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_parent_iterator_end(struct ecu_ntnode_parent_iterator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_parent_iterator_at() or @ref ecu_ntnode_parent_iterator_begin().
 * @brief Returns the next parent in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PARENT_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_PARENT_FOR_EACH() instead.
 * 
 * @param me Non-const parent iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_parent_iterator_next(struct ecu_ntnode_parent_iterator *me);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_parent_iterator_at().
 * Starts an iteration over all parents, including the supplied
 * starting node. The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PARENT_AT_FOR_EACH()
 * instead.
 * 
 * @param me Const parent iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_parent_iterator_cat(struct ecu_ntnode_parent_citerator *me,
                                                               const struct ecu_ntnode *start);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_parent_iterator_begin().
 * Starts an iteration over all parents, excluding the supplied
 * starting node. If the starting node has parents, its parent is returned.
 * Otherwise NULL is returned, signifying the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PARENT_FOR_EACH()
 * instead.
 * 
 * @param me Const parent iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_parent_iterator_cbegin(struct ecu_ntnode_parent_citerator *me,
                                                                  const struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_parent_iterator_cat() or @ref ecu_ntnode_parent_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_parent_iterator_end().
 * Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PARENT_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_CONST_PARENT_FOR_EACH() instead.
 * 
 * @param me Const parent iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_parent_iterator_cend(struct ecu_ntnode_parent_citerator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_parent_iterator_cat() or @ref ecu_ntnode_parent_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_parent_iterator_next().
 * Returns the next parent in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PARENT_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_CONST_PARENT_FOR_EACH() instead.
 * 
 * @param me Const parent iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_parent_iterator_cnext(struct ecu_ntnode_parent_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*--------------------- POSTORDER ITERATOR -------------------*/
/*------------------------------------------------------------*/

/**
 * @name Postorder Iterator
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p root previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts a postorder iteration over the supplied node's tree.
 * Returns the first node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_POSTORDER_FOR_EACH()
 * instead.
 * 
 * @param me Non-const postorder iterator to initialize.
 * @param root Root of tree to iterate over. This root is
 * included in the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_postorder_iterator_begin(struct ecu_ntnode_postorder_iterator *me,
                                                              struct ecu_ntnode *root);

/**
 * @pre @p me started via @ref ecu_ntnode_postorder_iterator_begin().
 * @brief Returns the ending node in the iteration, which is a
 * dummy delimiter.
 * 
 * @warning The node returned by this function should never be used
 * since it is a dummy delimiter.
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_POSTORDER_FOR_EACH()
 * instead.
 * 
 * @param me Non-const postorder iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_postorder_iterator_end(struct ecu_ntnode_postorder_iterator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_postorder_iterator_begin().
 * @brief Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_POSTORDER_FOR_EACH()
 * instead.
 * 
 * @param me Non-const postorder iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_postorder_iterator_next(struct ecu_ntnode_postorder_iterator *me);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p root previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_postorder_iterator_begin().
 * Starts a postorder iteration over the supplied node's tree.
 * Returns the first node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_POSTORDER_FOR_EACH()
 * instead.
 * 
 * @param me Const postorder iterator to initialize.
 * @param root Root of tree to iterate over. This root is
 * included in the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cbegin(struct ecu_ntnode_postorder_citerator *me,
                                                                     const struct ecu_ntnode *root);

/**
 * @pre @p me started via @ref ecu_ntnode_postorder_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_postorder_iterator_end().
 * Returns the ending node in the iteration, which is a
 * dummy delimiter.
 * 
 * @warning The node returned by this function should never be used
 * since it is a dummy delimiter.
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_POSTORDER_FOR_EACH()
 * instead.
 * 
 * @param me Const postorder iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cend(struct ecu_ntnode_postorder_citerator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_postorder_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_postorder_iterator_next().
 * Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_POSTORDER_FOR_EACH()
 * instead.
 * 
 * @param me Const postorder iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_postorder_iterator_cnext(struct ecu_ntnode_postorder_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*----------------------- PREORDER ITERATOR ------------------*/
/*------------------------------------------------------------*/

/**
 * @name Preorder Iterator
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p root previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts a preorder iteration over the supplied node's tree.
 * The supplied @p root is returned, which is the first node in the 
 * iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PREORDER_FOR_EACH()
 * instead.
 * 
 * @param me Non-const preorder iterator to initialize.
 * @param root Root of tree to iterate over. This root is
 * included in the iteration and is returned in this function.
 */
extern struct ecu_ntnode *ecu_ntnode_preorder_iterator_begin(struct ecu_ntnode_preorder_iterator *me,
                                                             struct ecu_ntnode *root);

/**
 * @pre @p me started via @ref ecu_ntnode_preorder_iterator_begin().
 * @brief Returns the ending node in the iteration, which is a
 * dummy delimiter.
 * 
 * @warning The node returned by this function should never be used
 * since it is a dummy delimiter.
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PREORDER_FOR_EACH()
 * instead.
 * 
 * @param me Non-const preorder iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_preorder_iterator_end(struct ecu_ntnode_preorder_iterator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_preorder_iterator_begin().
 * @brief Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PREORDER_FOR_EACH()
 * instead.
 * 
 * @param me Non-const preorder iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_preorder_iterator_next(struct ecu_ntnode_preorder_iterator *me);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p root previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_preorder_iterator_begin().
 * Starts a preorder iteration over the supplied node's tree.
 * The supplied @p root is returned, which is the first node in the 
 * iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PREORDER_FOR_EACH()
 * instead.
 * 
 * @param me Const preorder iterator to initialize.
 * @param root Root of tree to iterate over. This root is
 * included in the iteration and is returned in this function.
 */
extern const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cbegin(struct ecu_ntnode_preorder_citerator *me,
                                                                    const struct ecu_ntnode *root);

/**
 * @pre @p me started via @ref ecu_ntnode_preorder_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_preorder_iterator_end().
 * Returns the ending node in the iteration, which is a
 * dummy delimiter.
 * 
 * @warning The node returned by this function should never be used
 * since it is a dummy delimiter.
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PREORDER_FOR_EACH()
 * instead.
 * 
 * @param me Const preorder iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cend(struct ecu_ntnode_preorder_citerator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_preorder_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_preorder_iterator_next().
 * Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PREORDER_FOR_EACH()
 * instead.
 * 
 * @param me Const preorder iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_preorder_iterator_cnext(struct ecu_ntnode_preorder_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*------------------- PREV SIBLING ITERATOR ------------------*/
/*------------------------------------------------------------*/

/**
 * @name Previous Sibling Iterator
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over all previous (left) siblings, including
 * the supplied starting node. The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH()
 * instead.
 * 
 * @param me Non-const prev sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_at(struct ecu_ntnode_prev_sibling_iterator *me,
                                                              struct ecu_ntnode *start);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over all previous (left) siblings, excluding
 * the supplied starting node. If the starting node has siblings the previous
 * (left) sibling to it is returned. Otherwise NULL is returned, signifying
 * the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PREV_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Non-const prev sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_begin(struct ecu_ntnode_prev_sibling_iterator *me,
                                                                 struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_prev_sibling_iterator_at() or @ref ecu_ntnode_prev_sibling_iterator_begin().
 * @brief Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_PREV_SIBLING_FOR_EACH() instead.
 * 
 * @param me Non-const prev sibling iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_end(struct ecu_ntnode_prev_sibling_iterator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_prev_sibling_iterator_at() or @ref ecu_ntnode_prev_sibling_iterator_begin().
 * @brief Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_PREV_SIBLING_FOR_EACH() instead.
 * 
 * @param me Non-const prev sibling iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_next(struct ecu_ntnode_prev_sibling_iterator *me);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_prev_sibling_iterator_at().
 * Starts an iteration over all previous (left) siblings, including
 * the supplied starting node. The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PREV_SIBLING_AT_FOR_EACH()
 * instead.
 * 
 * @param me Const prev sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_cat(struct ecu_ntnode_prev_sibling_citerator *me,
                                                                     const struct ecu_ntnode *start);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_prev_sibling_iterator_begin().
 * Starts an iteration over all previous (left) siblings, excluding
 * the supplied starting node. If the starting node has siblings the previous
 * (left) sibling to it is returned. Otherwise NULL is returned, signifying
 * the end of the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PREV_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Const prev sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_cbegin(struct ecu_ntnode_prev_sibling_citerator *me,
                                                                        const struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_prev_sibling_iterator_cat() or @ref ecu_ntnode_prev_sibling_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_prev_sibling_iterator_end().
 * Returns the ending node in the iteration, which is NULL.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PREV_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_CONST_PREV_SIBLING_FOR_EACH() instead.
 * 
 * @param me Const prev sibling iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_cend(struct ecu_ntnode_prev_sibling_citerator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_prev_sibling_iterator_cat() or @ref ecu_ntnode_prev_sibling_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_prev_sibling_iterator_next().
 * Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_PREV_SIBLING_AT_FOR_EACH() 
 * or @ref ECU_NTNODE_CONST_PREV_SIBLING_FOR_EACH() instead.
 * 
 * @param me Const prev sibling iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_prev_sibling_iterator_cnext(struct ecu_ntnode_prev_sibling_citerator *me);
/**@}*/

/*------------------------------------------------------------*/
/*----------------------- SIBLING ITERATOR -------------------*/
/*------------------------------------------------------------*/

/**
 * @name Sibling Iterator
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Starts an iteration over all siblings, excluding the supplied
 * starting node.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Non-const sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern struct ecu_ntnode *ecu_ntnode_sibling_iterator_begin(struct ecu_ntnode_sibling_iterator *me,
                                                            struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_sibling_iterator_begin().
 * @brief Returns the ending node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Non-const sibling iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_sibling_iterator_end(struct ecu_ntnode_sibling_iterator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_sibling_iterator_begin().
 * @brief Returns the next sibling in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Non-const sibling iterator.
 */
extern struct ecu_ntnode *ecu_ntnode_sibling_iterator_next(struct ecu_ntnode_sibling_iterator *me);

/**
 * @pre Memory already allocated for @p me.
 * @pre @p start previously constructed via @ref ecu_ntnode_ctor().
 * @brief Const-qualified version of @ref ecu_ntnode_sibling_iterator_begin().
 * Starts an iteration over all siblings, excluding the supplied
 * starting node.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Const sibling iterator to initialize.
 * @param start Starting node of the iteration.
 */
extern const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cbegin(struct ecu_ntnode_sibling_citerator *me,
                                                                   const struct ecu_ntnode *start);

/**
 * @pre @p me started via @ref ecu_ntnode_sibling_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_sibling_iterator_end().
 * Returns the ending node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Const sibling iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cend(struct ecu_ntnode_sibling_citerator *me);

/**
 * @pre @p me started via @ref ecu_ntnode_sibling_iterator_cbegin().
 * @brief Const-qualified version of @ref ecu_ntnode_sibling_iterator_next().
 * Returns the next sibling in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_NTNODE_CONST_SIBLING_FOR_EACH()
 * instead.
 * 
 * @param me Const sibling iterator.
 */
extern const struct ecu_ntnode *ecu_ntnode_sibling_iterator_cnext(struct ecu_ntnode_sibling_citerator *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_NTNODE_H_ */
