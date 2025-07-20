/**
 * @file
 * @brief
 * @rst
 * See :ref:`dlist.h section <dlist_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-05
 * @copyright Copyright (c) 2024
 */

#ifndef ECU_DLIST_H_
#define ECU_DLIST_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ECU. */
#include "ecu/object_id.h"
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Convenience define for @ref ecu_dlist_insert_before()
 * and @ref ecu_dlist_sort(). Pass to these functions if optional
 * callback object is not needed.
 */
#define ECU_DNODE_OBJ_UNUSED \
    ((void *)0)

/**
 * @brief Convenience define for @ref ecu_dnode_ctor().
 * Pass this value to @ref ecu_dnode_ctor() if
 * a user-defined node destructor is not needed.
 */
#define ECU_DNODE_DESTROY_UNUSED \
    ((void (*)(struct ecu_dnode *, ecu_object_id))0)

/**
 * @brief Retrieves user data from an intrusive @ref ecu_dnode 
 * by converting the supplied @ref ecu_dnode back into the 
 * user's node type.
 *
 * @param ptr_ Pointer to intrusive @ref ecu_dnode.
 * This must be pointer to non-const. I.e. (struct ecu_dnode *).
 * @param type_ User's node type containing the intrusive
 * @ref ecu_dnode. Do not use const specifier. I.e. (struct my_type),
 * never (const struct my_type).
 * @param member_ Name of @ref ecu_dnode member within user's
 * type.
 */
#define ECU_DNODE_GET_ENTRY(ptr_, type_, member_) \
    ECU_CONTAINER_OF(ptr_, type_, member_)

/**
 * @brief Const-qualified version of @ref ECU_DNODE_GET_ENTRY().
 * Retrieves user data from an intrusive @ref ecu_dnode 
 * by converting the supplied @ref ecu_dnode back into the 
 * user's node type.
 *
 * @param ptr_ Pointer to intrusive @ref ecu_dnode. This can be
 * pointer to const or non-const. I.e. (struct ecu_dnode *) or 
 * (const struct ecu_dnode *).
 * @param type_ User's node type containing the intrusive
 * @ref ecu_dnode. Do not use const specifier. I.e. (struct my_type),
 * never (const struct my_type).
 * @param member_ Name of @ref ecu_dnode member within user's
 * type.
 */
#define ECU_DNODE_GET_CONST_ENTRY(ptr_, type_, member_) \
    ECU_CONST_CONTAINER_OF(ptr_, type_, member_)

/**
 * @brief Helper macro that iterates over an entire list, starting
 * at the specified position. Use of this macro also protects the
 * application from iterator API changes. It is safe to remove the 
 * current node in the iteration.
 * 
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_dnode.
 * @param iter_ Pointer to @ref ecu_dlist_iterator.
 * @param list_ Pointer to @ref ecu_dlist to iterate over.
 * @param start_ Pointer to @ref ecu_dnode. Iteration is started at this
 * position. This node must be within @p list_.
 */
#define ECU_DLIST_AT_FOR_EACH(var_, iter_, list_, start_)                       \
    for (struct ecu_dnode *var_ = ecu_dlist_iterator_at(iter_, list_, start_);  \
         var_ != ecu_dlist_iterator_end(iter_);                                 \
         var_ = ecu_dlist_iterator_next(iter_))

/**
 * @brief Helper macro that iterates over an entire list, starting
 * at the specified position. Use of this macro also protects the
 * application from iterator API changes.
 *
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_dnode.
 * @param citer_ Pointer to @ref ecu_dlist_citerator.
 * @param list_ Pointer to @ref ecu_dlist to iterate over.
 * @param start_ Pointer to @ref ecu_dnode. Iteration is started at this
 * position. This node must be within @p list_.
 */
#define ECU_DLIST_CONST_AT_FOR_EACH(var_, citer_, list_, start_)                        \
    for (const struct ecu_dnode *var_ = ecu_dlist_iterator_cat(citer_, list_, start_);  \
         var_ != ecu_dlist_iterator_cend(citer_);                                       \
         var_ = ecu_dlist_iterator_cnext(citer_))

/**
 * @brief Helper macro that iterates over an entire list, starting 
 * at HEAD. Use of this macro also protects the application from 
 * iterator API changes. It is safe to remove the current node
 * in the iteration.
 *
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to @ref ecu_dnode.
 * @param iter_ Pointer to @ref ecu_dlist_iterator.
 * @param list_ Pointer to @ref ecu_dlist to iterate over.
 */
#define ECU_DLIST_FOR_EACH(var_, iter_, list_)                            \
    for (struct ecu_dnode *var_ = ecu_dlist_iterator_begin(iter_, list_); \
         var_ != ecu_dlist_iterator_end(iter_);                           \
         var_ = ecu_dlist_iterator_next(iter_))

/**
 * @brief Helper macro that const iterates over an entire list, 
 * starting at HEAD. Use of this macro also protects the application 
 * from iterator API changes.
 *
 * @param var_ Loop variable name. This variable will store the current 
 * node in the iteration and will be a pointer to const @ref ecu_dnode.
 * @param citer_ Pointer to @ref ecu_dlist_citerator.
 * @param list_ Pointer to @ref ecu_dlist to iterate over.
 */
#define ECU_DLIST_CONST_FOR_EACH(var_, citer_, list_)                               \
    for (const struct ecu_dnode *var_ = ecu_dlist_iterator_cbegin(citer_, list_);   \
         var_ != ecu_dlist_iterator_cend(citer_);                                   \
         var_ = ecu_dlist_iterator_cnext(citer_))

/*------------------------------------------------------------*/
/*-------------------------- DLIST ---------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Single node within list. Intrusive, so
 * user-defined types contain this node as a member.
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_dnode
{
    /// @brief Next node in list.
    struct ecu_dnode *next;

    /// @brief Previous node in list.
    struct ecu_dnode *prev;

    /// @brief Optional user-defined node destructor. Executes
    /// when @ref ecu_dlist_destroy() or @ref ecu_dnode_destroy()
    /// are called.
    void (*destroy)(struct ecu_dnode *me, ecu_object_id id);

    /// @brief Optional node ID. Helps user identify
    /// different types stored in the same list.
    ecu_object_id id;
};

/**
 * @brief Intrusive, doubly-linked list.
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_dlist
{
    /// @brief Dummy node used as delimiter to represent
    /// start and end of list. Not apart of user's list.
    struct ecu_dnode head;
};

/*------------------------------------------------------------*/
/*--------------------- DLIST ITERATORS ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const list iterator.
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_dlist_iterator
{
    /// @brief List that is being iterated. List's
    /// HEAD is used as a delimiter.
    struct ecu_dlist *list;

    /// @brief Current position in list.
    struct ecu_dnode *current;

    /// @brief Next position in the list. Allows user to
    /// safely add and remove nodes in the middle of an
    /// iteration.
    struct ecu_dnode *next;
};

/**
 * @brief Const list iterator.
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_dlist_citerator
{
    /// @brief List that is being iterated. List's
    /// HEAD is used as a delimiter.
    const struct ecu_dlist *list;

    /// @brief Current position in list.
    const struct ecu_dnode *current;

    /// @brief Next position in the list. Allows user to
    /// safely add and remove nodes in the middle of an
    /// iteration.
    const struct ecu_dnode *next;
};

/*------------------------------------------------------------*/
/*------------------ DNODE MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Dnode Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Node constructor.
 *
 * @warning @p me must not be an active node within a list, otherwise behavior
 * is undefined.
 *
 * @param me Node to construct. This cannot be NULL. This cannot
 * be HEAD (@ref ecu_dlist.head).
 * @param destroy Optional callback. Defines any <b>additional</b> cleanup
 * needed to fully destroy this user-defined node. Do not use API calls that edit
 * the ecu_dnode (node insert, remove, etc) within this callback. Doing so is undefined
 * behavior. Executes when node is destroyed via @ref ecu_dnode_destroy(). Also
 * executes when node is in list that is destroyed via @ref ecu_dlist_destroy().
 * Supply @ref ECU_DNODE_DESTROY_UNUSED if unused.
 * @param id Optional ID to assign to node. Used to identify different user-defined
 * types stored in the same list. Supply @ref ECU_OBJECT_ID_UNUSED if unused. This
 * value must be greater than or equal to @ref ECU_VALID_OBJECT_ID_BEGIN
 */
extern void ecu_dnode_ctor(struct ecu_dnode *me,
                           void (*destroy)(struct ecu_dnode *me, ecu_object_id id),
                           ecu_object_id id);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Node destructor.
 * Removes node if it is in a list. Executes the user-defined
 * destructor if one was supplied to @ref ecu_dnode_ctor(). Node must
 * be reconstructed via @ref ecu_dnode_ctor() in order to be used again.
 *
 * @warning Memory is not freed since ECU is meant to be used without
 * dynamic memory allocation. If the supplied node was allocated on the
 * heap user must free it themselves. It is recommended to do this
 * inside the user-defined destroy callback passed to @ref ecu_dnode_ctor().
 *
 * @param me Node to destroy. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern void ecu_dnode_destroy(struct ecu_dnode *me);
/**@}*/

/**
 * @name Dnode Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Returns node ID. Used to identity different user-defined types
 * stored in the same list.
 *
 * @param me Node to check. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern ecu_object_id ecu_dnode_id(const struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Returns true if node is in list. False otherwise.
 *
 * @param me Node to check. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern bool ecu_dnode_in_list(const struct ecu_dnode *me);

/**
 * @pre @p pos and @p node previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Insert node after specified position in list.
 * 
 * @param pos Position node. Node is inserted after this position. This
 * must be within a list and cannot be HEAD (@ref ecu_dlist.head).
 * @param node Node to insert in the list. This cannot already be within
 * a list and cannot be HEAD (@ref ecu_dlist.head).
 */
extern void ecu_dnode_insert_after(struct ecu_dnode *pos, struct ecu_dnode *node);

/**
 * @pre @p pos and @p node previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Insert node before specified position in list.
 *
 * @param pos Position node. Node is inserted before this position. This
 * must be within a list and cannot be HEAD (@ref ecu_dlist.head).
 * @param node Node to insert. This cannot already be within a list.
 * and cannot be HEAD (@ref ecu_dlist.head).
 */
extern void ecu_dnode_insert_before(struct ecu_dnode *pos, struct ecu_dnode *node);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Returns the node next to (right of) @p me. NULL is returned 
 * if @p me is the last node in the list or if @p me is not in 
 * a list.
 * 
 * @param me Node to check. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern struct ecu_dnode *ecu_dnode_next(struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Const-qualified version of @ref ecu_dnode_next().
 * Returns the node next to (right of) @p me. NULL is returned 
 * if @p me is the last node in the list or if @p me is not in 
 * a list.
 * 
 * @param me Node to check. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern const struct ecu_dnode *ecu_dnode_cnext(const struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Returns the node previous to (left of) @p me. NULL is 
 * returned if @p me is the first node in the list (one after HEAD) or 
 * if @p me is not in a list.
 * 
 * @param me Node to check. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern struct ecu_dnode *ecu_dnode_prev(struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Const-qualified version of @ref ecu_dnode_prev().
 * Returns the node previous to (left of) @p me. NULL is 
 * returned if @p me is the first node in the list (one after HEAD) or 
 * if @p me is not in a list.
 * 
 * @param me Node to check. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern const struct ecu_dnode *ecu_dnode_cprev(const struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief If node is in list, it is removed. Node can be reused and
 * added to another list without reconstruction.
 *
 * @param me Node to remove. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern void ecu_dnode_remove(struct ecu_dnode *me);

/**
 * @brief Returns true if supplied node has been constructed
 * via @ref ecu_dnode_ctor(). Returns false if supplied node
 * has not been constructed or if supplied node is HEAD (@ref ecu_dlist.head).
 * 
 * @param me Node to check.
 */
extern bool ecu_dnode_valid(const struct ecu_dnode *me);
/**@}*/

/*------------------------------------------------------------*/
/*------------------ DLIST MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

/**
 * @name DList Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief List constructor.
 *
 * @warning @p me must not be an active list, otherwise
 * behavior is undefined.
 *
 * @param me List to construct. This cannot be NULL.
 */
extern void ecu_dlist_ctor(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief List destructor.
 * Destroys list and all nodes within the list. List and nodes
 * must be reconstructed in order to be used again.
 *
 * @warning Memory is not freed since ECU is meant to be used without
 * dynamic memory allocation. If the supplied list or any nodes within the
 * list were allocated on the heap, user must free it themselves. It is
 * recommended to free the nodes inside the destroy callbacks passed to
 * @ref ecu_dnode_ctor(). The list must be freed elsewhere.
 *
 * @param me List to destroy.
 */
extern void ecu_dlist_destroy(struct ecu_dlist *me);
/**@}*/

/**
 * @name DList Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief If list is not empty, returns a pointer to the tail node.
 * If list is empty, returns NULL.
 *
 * @param me List to check.
 */
extern struct ecu_dnode *ecu_dlist_back(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Const-qualified version of @ref ecu_dlist_back().
 * If list is not empty, returns a pointer to the tail node.
 * If list is empty, returns NULL.
 *
 * @param me List to check.
 */
extern const struct ecu_dnode *ecu_dlist_cback(const struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Removes all nodes from the list. List and nodes can be reused
 * without reconstruction.
 *
 * @param me List to clear.
 */
extern void ecu_dlist_clear(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns true if list is empty. False otherwise.
 *
 * @note An empty list means the list only has HEAD (@ref ecu_dlist.head), 
 * which is a dummy delimeter.
 *
 * @param me List to check.
 */
extern bool ecu_dlist_empty(const struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief If list is not empty, returns a pointer to the front node.
 * If list is empty, returns NULL.
 *
 * @param me List to check.
 */
extern struct ecu_dnode *ecu_dlist_front(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Const-qualified version of @ref ecu_dlist_front().
 * If list is not empty, returns a pointer to the front node.
 * If list is empty, returns NULL.
 *
 * @param me List to check.
 */
extern const struct ecu_dnode *ecu_dlist_cfront(const struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Insert node before position specified by condition function.
 * Starting from HEAD, all nodes within the list are iterated over. Each
 * node is passed as the position parameter to the condition function. User specifies
 * whether node should be inserted before this position by returning true. Function
 * exits as soon as node is inserted.
 *
 * @param me List to add to.
 * @param node Node to add. This cannot already be within a list. This cannot
 * be HEAD (@ref ecu_dlist.head).
 * @param condition Mandatory function that returns true if node should be
 * inserted before specified position. False otherwise. @p node will become
 * the new TAIL if all conditions return false.
 * @param data Optional object to pass to @p condition. Supply @ref ECU_DNODE_OBJ_UNUSED
 * if unused.
 */
extern void ecu_dlist_insert_before(struct ecu_dlist *me,
                                    struct ecu_dnode *node,
                                    bool (*condition)(const struct ecu_dnode *node, const struct ecu_dnode *position, void *data),
                                    void *data);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Insert node to back of list.
 *
 * @param me List to add to.
 * @param node Node to add. This will be the new TAIL. Node cannot already
 * be within a list. This cannot be HEAD (@ref ecu_dlist.head).
 */
extern void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dnode *node);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Insert node to front of list.
 *
 * @param me List to add to.
 * @param node Node to add. This cannot already be within a list.
 * This cannot be HEAD (@ref ecu_dlist.head).
 */
extern void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dnode *node);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief If list is not empty, removes the tail node and returns
 * a pointer to it. If list is empty, returns NULL.
 *
 * @param me List to pop.
 */
extern struct ecu_dnode *ecu_dlist_pop_back(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief If list is not empty, removes the front node and returns
 * a pointer to it. If list is empty, returns NULL.
 *
 * @param me List to pop.
 */
extern struct ecu_dnode *ecu_dlist_pop_front(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns number of nodes in a list. Returns 0 if list is empty.
 *
 * @note An empty list means the list only has HEAD(@ref ecu_dlist.head), 
 * which is a dummy delimeter.
 * @note All nodes in list are iterated over in this function, making it O(n).
 *
 * @param me List to check.
 */
extern size_t ecu_dlist_size(const struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief List merge sort. Sorting condition is specified by user-defined callback.
 *
 * @param me List to sort.
 * @param lhs_less_than_rhs Mandatory function that defines sorting condition.
 * Return true if lhs node is less than rhs node. Return false if rhs node is
 * greater than or equal to lhs node.
 * @param data Optional object to pass to @p lhs_less_than_rhs. Supply
 * @ref ECU_DNODE_OBJ_UNUSED if unused.
 */
extern void ecu_dlist_sort(struct ecu_dlist *me,
                           bool (*lhs_less_than_rhs)(const struct ecu_dnode *lhs, const struct ecu_dnode *rhs, void *data),
                           void *data);

/**
 * @pre @p me and @p other previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Swaps the two lists. I.e. if @p me == [0, 1] and @p other == [2, 3], 
 * then @p me == [2, 3] and @p other == [0, 1] after this operation.
 * If one list is empty and one list is nonempty, they will also be swapped.
 * I.e. if @p me == [] and @p other == [4, 5], then @p me == [4, 5] and
 * @p other == [] after this operation. If both lists are empty this
 * function does nothing.
 * 
 * @param me Swap this list with @p other. This cannot equal @p other.
 * @param other Swap this list with @p me. This cannot equal @p me.
 */
extern void ecu_dlist_swap(struct ecu_dlist *me, struct ecu_dlist *other);

/**
 * @brief Returns true if supplied list has been constructed
 * via @ref ecu_dlist_ctor(). False otherwise.
 * 
 * @param me List to check.
 */
extern bool ecu_dlist_valid(const struct ecu_dlist *me);
/**@}*/

/*------------------------------------------------------------*/
/*----------- NON-CONST ITERATOR MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

/**
 * @name Iterators
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p start node is in @p list.
 * @brief Initializes iterator at the supplied starting node's position. 
 * The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_AT_FOR_EACH()
 * instead.
 * 
 * @param me Non-const iterator to initialize.
 * @param list List to iterate over.
 * @param start Starting position of the iteration. This node must be
 * within @p list and cannot be HEAD (@ref ecu_dlist.head).
 */
extern struct ecu_dnode *ecu_dlist_iterator_at(struct ecu_dlist_iterator *me, 
                                               struct ecu_dlist *list, 
                                               struct ecu_dnode *start);

/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Initializes iterator and returns first node in the list.
 * Returns first user-defined node if list is not empty. Otherwise
 * returns the same terminal node as @ref ecu_dlist_iterator_end().
 * 
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_FOR_EACH()
 * instead.
 *
 * @param me Non-const iterator to initialize.
 * @param list List to iterate over.
 */
extern struct ecu_dnode *ecu_dlist_iterator_begin(struct ecu_dlist_iterator *me, struct ecu_dlist *list);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin().
 * @brief Returns list's terminal node, which is HEAD (@ref ecu_dlist.head).
 * 
 * @warning The node returned by this function should never be used
 * since it is HEAD(@ref ecu_dlist.head) which is a dummy delimiter.
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_FOR_EACH()
 * instead.
 *
 * @param me Non-const iterator.
 */
extern struct ecu_dnode *ecu_dlist_iterator_end(struct ecu_dlist_iterator *me);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin().
 * @brief Returns the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_FOR_EACH()
 * instead.
 *
 * @param me Non-const iterator.
 */
extern struct ecu_dnode *ecu_dlist_iterator_next(struct ecu_dlist_iterator *me);

/*------------------------------------------------------------*/
/*------------- CONST ITERATOR MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p start node is in @p list.
 * @brief Const-qualified version of @ref ecu_dlist_iterator_at().
 * Initializes iterator at the supplied starting node's position. 
 * The supplied starting node is returned.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_CONST_AT_FOR_EACH()
 * instead.
 * 
 * @param me Const iterator to initialize.
 * @param list List to iterate over.
 * @param start Starting position of the iteration. This node must be
 * within @p list and cannot be HEAD (@ref ecu_dlist.head).
 */
extern const struct ecu_dnode *ecu_dlist_iterator_cat(struct ecu_dlist_citerator *me, 
                                                      const struct ecu_dlist *list, 
                                                      const struct ecu_dnode *start);
                                                      
/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Const-qualified version of @ref ecu_dlist_iterator_begin().
 * Initializes iterator and returns first node in the list.
 * Returns first user-defined node if list is not empty. Otherwise
 * returns the same terminal node as @ref ecu_dlist_iterator_cend().
 * 
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_CONST_FOR_EACH()
 * instead.
 *
 * @param me Const iterator to initialize.
 * @param list List to iterate over.
 */
extern const struct ecu_dnode *ecu_dlist_iterator_cbegin(struct ecu_dlist_citerator *me, 
                                                         const struct ecu_dlist *list);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_cbegin()
 * or @ref ecu_dlist_iterator_cat().
 * @brief Const-qualified version of @ref ecu_dlist_iterator_end().
 * Returns list's terminal node, which is HEAD (@ref ecu_dlist.head).
 * 
 * @warning The node returned by this function should never be used
 * since it is HEAD(@ref ecu_dlist.head) which is a dummy delimiter.
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_CONST_FOR_EACH()
 * instead.
 *
 * @param me Const iterator.
 */
extern const struct ecu_dnode *ecu_dlist_iterator_cend(struct ecu_dlist_citerator *me);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_cbegin()
 * or @ref ecu_dlist_iterator_cat().
 * @brief Const-qualified version of @ref ecu_dlist_iterator_next(). Returns 
 * the next node in the iteration.
 * 
 * @warning Not meant to be used directly. Use @ref ECU_DLIST_CONST_FOR_EACH()
 * instead.
 *
 * @param me Const iterator.
 */
extern const struct ecu_dnode *ecu_dlist_iterator_cnext(struct ecu_dlist_citerator *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_DLIST_H_ */
