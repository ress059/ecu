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
#include <stddef.h> /* offsetof() */
#include <stdint.h>

/* ecu_dnode::id */
#include "ecu/object_id.h"

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
 * @details Pass this value to @ref ecu_dnode_ctor() if 
 * a user-defined node destructor is not needed.
 */
#define ECU_DNODE_DESTROY_UNUSED \
    ((void (*)(struct ecu_dnode *, ecu_object_id))0)

/**
 * @brief Access user-defined data stored in a dlist node. Converts 
 * a dlist node into the user-defined type stored in the list.
 * 
 * @param ptr_ Address of dlist node within the user-defined @p type_.
 * This should always be of type (struct ecu_dnode *), never
 * (const struct ecu_dnode *).
 * @param type_ User-defined node type stored in the list.
 * Const specifier should never be supplied. I.e. struct my_type,
 * never const struct my_type.
 * @param member_ Member name of the dlist node within @p type_.
 */
#define ECU_DNODE_GET_ENTRY(ptr_, type_, member_) \
    ((type_ *)((uint8_t *)(ptr_) - offsetof(type_, member_)))

/**
 * @brief Read user-defined data stored in a dlist node. Converts 
 * a dlist node into the user-defined type stored in the list.
 * 
 * @param ptr_ Address of dlist node within the user-defined @p type_.
 * This can be of type (struct ecu_dnode *) or (const struct ecu_dnode *).
 * @param type_ User-defined node type stored in the list.
 * Const specifier should never be supplied. I.e. struct my_type,
 * never const struct my_type.
 * @param member_ Member name of the dlist node within @p type_.
 */
#define ECU_DNODE_GET_CONST_ENTRY(ptr_, type_, member_) \
    ((const type_ *)((const uint8_t *)(ptr_) - offsetof(type_, member_)))

/**
 * @brief Helper macro that iterates over an entire list. Use of
 * this macro also protects the application from iterator API changes.
 * 
 * @param var_ Loop variable name. This will store pointers to @ref ecu_dnode.
 * @param iter_ Pointer to @ref ecu_dlist_iterator.
 * @param list_ Pointer to @ref ecu_dlist to iterate over.
 */
#define ECU_DLIST_FOR_EACH(var_, iter_, list_)                              \
    for (struct ecu_dnode *var_ = ecu_dlist_iterator_begin(iter_, list_);   \
         var_ != ecu_dlist_iterator_end(iter_);                             \
         var_ = ecu_dlist_iterator_next(iter_))

/**
 * @brief Helper macro that iterates over an entire list. Use of
 * this macro also protects the application from iterator API changes.
 * 
 * @param var_ Loop variable name. This will store pointers to const @ref ecu_dnode.
 * @param citer_ Pointer to @ref ecu_dlist_const_iterator.
 * @param list_ Pointer to @ref ecu_dlist to iterate over.
 */
#define ECU_DLIST_CONST_FOR_EACH(var_, citer_, list_)                                   \
    for (const struct ecu_dnode *var_ = ecu_dlist_const_iterator_begin(citer_, list_);  \
         var_ != ecu_dlist_const_iterator_end(citer_);                                  \
         var_ = ecu_dlist_const_iterator_next(citer_))

/*------------------------------------------------------------*/
/*-------------------------- DLIST ---------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Single node within list. User-defined nodes
 * contain this object.
 */
struct ecu_dnode
{
    /**
     * @private
     * @brief PRIVATE. Next node in list.
     */
    struct ecu_dnode *next;

    /**
     * @private
     * @brief PRIVATE. Previous node in list.
     */
    struct ecu_dnode *prev;

    /**
     * @private
     * @brief PRIVATE. Optional user-defined node destructor.
     * @details Executes when @ref ecu_dlist_destroy() or 
     * @ref ecu_dnode_destroy() is called.
     */
    void (*destroy)(struct ecu_dnode *me, ecu_object_id id);

    /**
     * @private
     * @brief PRIVATE. Optional node ID.
     * @details Helps user identify different types stored in the
     * same list.
     */
    ecu_object_id id;
};

/**
 * @brief Intrusive, doubly-linked list.
 */
struct ecu_dlist
{
    /**
     * @private
     * @brief PRIVATE. List HEAD.
     * @details Dummy node used as delimiter to represent
     * start and end of list. Not apart of user's list.
     */
    struct ecu_dnode head;
};

/*------------------------------------------------------------*/
/*--------------------- DLIST ITERATORS ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Non-const list iterator.
 */
struct ecu_dlist_iterator
{
    /**
     * @private
     * @brief PRIVATE. List that is being iterated. 
     * @details List's HEAD is used as a delimiter.
     */
    struct ecu_dlist *list;

    /**
     * @private
     * @brief PRIVATE. Current position in list.
     */
    struct ecu_dnode *current;

    /**
     * @private 
     * @brief PRIVATE. Next position in the list. 
     * @details Allows user to safely add and remove nodes 
     * in the middle of an iteration.
     */
    struct ecu_dnode *next;
};

/**
 * @brief Const list iterator.
 */
struct ecu_dlist_const_iterator
{
    /**
     * @private
     * @brief PRIVATE. List that is being iterated. 
     * @details List's HEAD is used as a delimiter.
     */
    const struct ecu_dlist *list;

    /**
     * @private
     * @brief PRIVATE. Current position in list.
     */
    const struct ecu_dnode *current;

    /**
     * @private 
     * @brief PRIVATE. Next position in the list. 
     * @details Allows user to safely add and remove nodes 
     * in the middle of an iteration.
     */
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
 * @param me Node to construct. This cannot be NULL.
 * @param destroy_0 Optional callback. Defines any <b>additional</b> cleanup
 * needed to fully destroy this user-defined node. Do not use dlist API in this callback
 * as cleanup of the @ref ecu_dnode is already done by this module. Doing so is 
 * undefined behavior. Executes when node is destroyed via @ref ecu_dnode_destroy().
 * Also executes when node is in list that is destroyed via @ref ecu_dlist_destroy().
 * Supply @ref ECU_DNODE_DESTROY_UNUSED if unused.
 * @param id_0 Optional ID to assign to node. Used to identify different user-defined
 * types stored in the same list. Supply @ref ECU_OBJECT_ID_UNUSED if unused. This 
 * value must be greater than or equal to @ref ECU_VALID_OBJECT_ID_BEGIN
 */
extern void ecu_dnode_ctor(struct ecu_dnode *me,
                           void (*destroy_0)(struct ecu_dnode *me, ecu_object_id id),
                           ecu_object_id id_0);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Node destructor.
 * @details Removes node if it is in a list. Executes the user-defined 
 * destructor if one was supplied to @ref ecu_dnode_ctor(). Node must
 * be reconstructed via @ref ecu_dnode_ctor() in order to be used again.
 * 
 * @param me Node to destroy. This cannot be @ref ecu_dlist.head
 */
extern void ecu_dnode_destroy(struct ecu_dnode *me);
/**@}*/

/**
 * @name Dnode Member Functions
 */
/**@{*/
/**
 * @pre @p me and @p position previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Insert node before specified position in list.
 * 
 * @param me Node to insert in the list. Node cannot already be in a list. This
 * cannot be @ref ecu_dlist.head.
 * @param position Insert before this position. @p position must be within a list.
 * Node will be new TAIL if this is @ref ecu_dlist.head.
 */
extern void ecu_dnode_insert_before(struct ecu_dnode *me, struct ecu_dnode *position);

/**
 * @pre @p me and @p position previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Insert node after specified position in list.
 * 
 * @param me Node to insert in the list. Node cannot already be in a list. This
 * cannot be @ref ecu_dlist.head.
 * @param position Insert after this position. @p position must be within a list.
 * Node will be new HEAD if this is @ref ecu_dlist.head.
 */
extern void ecu_dnode_insert_after(struct ecu_dnode *me, struct ecu_dnode *position);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Removes node from list. Node can be reused and added to another 
 * list without reconstruction.
 * 
 * @param me Node to remove. this must be within a list. This cannot 
 * be @ref ecu_dlist.head.
 */
extern void ecu_dnode_remove(struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Returns true if node is in list. False otherwise.
 * 
 * @param me Node to check.
 */
extern bool ecu_dnode_in_list(const struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Returns node ID. Used to identity different user-defined types
 * stored in the same list.
 * 
 * @param me Node to check. This cannot be @ref ecu_dlist.head.
 */
extern ecu_object_id ecu_dnode_get_id(const struct ecu_dnode *me);
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
 * @details Destroys list and all nodes within the list. List and nodes
 * must be reconstructed in order to be used again.
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
 * @brief Removes all nodes from the list. List and nodes can be reused
 * without reconstruction.
 * 
 * @param me List to clear.
 */
extern void ecu_dlist_clear(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor().
 * @brief Insert node to front of list.
 * 
 * @param me List to add to.
 * @param node Node to add. This will be the new HEAD. Node cannot already 
 * be within a list. This cannot be @ref ecu_dlist.head.
 */
extern void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dnode *node);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor().
 * @brief Insert node to back of list.
 * 
 * @param me List to add to.
 * @param node Node to add. This will be the new TAIL. Node cannot already
 * be within a list. This cannot be @ref ecu_dlist.head.
 */
extern void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dnode *node);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor().
 * @brief Insert node before position specified by condition function. 
 * @details Starting from HEAD, all nodes within the list are iterated over. Each 
 * node is passed as the position parameter to the condition function. User specifies
 * whether node should be inserted before this position by returning true. Function
 * exits as soon as node is inserted.
 * 
 * @param me List to add to.
 * @param node Node to add. This cannot already be within a list. This cannot 
 * be @ref ecu_dlist.head. 
 * @param condition Mandatory function that returns true if node should be
 * inserted before specified position. False otherwise. @p node will be inserted
 * at TAIL if all conditions return false.
 * @param data Optional object to pass to @p condition. Supply @ref ECU_DNODE_OBJ_UNUSED
 * if unused.
 */
extern void ecu_dlist_insert_before(struct ecu_dlist *me, 
                                    struct ecu_dnode *node,
                                    bool (*condition)(const struct ecu_dnode *node, const struct ecu_dnode *position, void *data),
                                    void *data);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief List merge sort. 
 * @details Sorting condition is specified by user-defined callback.
 * 
 * @param me 
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
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns number of nodes in a list. Returns 0 if list is empty.
 * 
 * @note An empty list means the list only has @ref ecu_dlist.head, which
 * is a dummy delimeter.
 * @note All nodes in list are iterated over in this function, making it O(n).
 * 
 * @param me List to check.
 */
extern size_t ecu_dlist_get_size(const struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns true if list is empty. False otherwise.
 * 
 * @note An empty list means the list only has @ref ecu_dlist.head, which
 * is a dummy delimeter.
 * 
 * @param me List to check.
 */
extern bool ecu_dlist_is_empty(const struct ecu_dlist *me);
/**@}*/

/*------------------------------------------------------------*/
/*----------- NON-CONST ITERATOR MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

/**
 * @name Non-const Iterator Member Functions
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Initializes iterator and returns first node in the list.
 * @details Returns first user-defined node if list is not empty. Otherwise
 * returns the same terminal node as @ref ecu_dlist_iterator_end(). It is 
 * safe to remove nodes in the middle of an iteration. Adding nodes in the 
 * middle of an iteration is undefined.
 * 
 * @param me Non-const iterator to initialize.
 * @param list List to iterate over.
 */
extern struct ecu_dnode *ecu_dlist_iterator_begin(struct ecu_dlist_iterator *me, struct ecu_dlist *list);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin().
 * @brief Returns list's terminal node, which is @ref ecu_dlist.head. This
 * is a dummy delimeter.
 * 
 * @param me Non-const iterator.
 */
extern struct ecu_dnode *ecu_dlist_iterator_end(struct ecu_dlist_iterator *me);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin().
 * @brief Returns the next node in the iteration.
 * 
 * @param me Non-const iterator.
 */
extern struct ecu_dnode *ecu_dlist_iterator_next(struct ecu_dlist_iterator *me);
/**@}*/

/*------------------------------------------------------------*/
/*------------- CONST ITERATOR MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @name Const Iterator Member Functions
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Initializes iterator and returns first node in the list.
 * @details Returns first user-defined node if list is not empty. Otherwise
 * returns the same terminal node as @ref ecu_dlist_const_iterator_end().
 * 
 * @param me Const iterator to initialize.
 * @param list List to iterate over.
 */
extern const struct ecu_dnode *ecu_dlist_const_iterator_begin(struct ecu_dlist_const_iterator *me, const struct ecu_dlist *list);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_const_iterator_begin().
 * @brief Returns list's terminal node, which is @ref ecu_dlist.head. This
 * is a dummy delimeter.
 * 
 * @param me Const iterator.
 */
extern const struct ecu_dnode *ecu_dlist_const_iterator_end(struct ecu_dlist_const_iterator *me);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_const_iterator_begin().
 * @brief Returns the next node in the iteration.
 * 
 * @param me Const iterator.
 */
extern const struct ecu_dnode *ecu_dlist_const_iterator_next(struct ecu_dlist_const_iterator *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_DLIST_H_ */
