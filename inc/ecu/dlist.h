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
/*-------------------------- MACROS --------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Retrieve non-const user-defined data stored in a dlist node.
 * 
 * @param ptr Address of dlist node within your user-defined @p type.
 * This should always be of type (struct ecu_dnode *), never
 * (const struct ecu_dnode *).
 * @param type User-defined type that contains the dlist node that @p ptr 
 * points to. This should always be non-const. I.e. struct my_type,
 * never const struct my_type.
 * @param member Member name of the dlist node that @p ptr points to.
 */
#define ECU_DNODE_GET_ENTRY(ptr, type, member)   ((type *)((uint8_t *)(ptr) - offsetof(type, member)))

/**
 * @brief Retrieve const user-defined data stored in a dlist node. 
 * 
 * @param ptr Address of dlist node within your user-defined @p type.
 * This can be of type (struct ecu_dnode *) or (const struct ecu_dnode *).
 * @param type User-defined type that contains the dlist node that @p ptr 
 * points to. This should always be non-const. I.e. struct my_type,
 * never const struct my_type.
 * @param member Member name of the dlist node that @p ptr points to.
 */
#define ECU_DNODE_GET_CONST_ENTRY(ptr, type, member)    ((const type *)((const uint8_t *)(ptr) - offsetof(type, member)))

/**
 * @brief Convience macro for @ref ecu_dnode_ctor().
 * @details Pass this value to @ref ecu_dnode_ctor() if 
 * a user-defined node destructor is not needed.
 */
#define ECU_DNODE_DESTROY_UNUSED            ((void (*)(struct ecu_dnode *, ecu_object_id))0)

/*------------------------------------------------------------*/
/*-------------------------- DLIST ---------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Single node within list. 
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
 * @brief Circular doubly linked list.
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

// ASSERTS IF you try to use invalidated iterator.

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
 * @warning @p me must not be an active node within a list, 
 * otherwise behavior is undefined.
 * @warning Do not call any list API functions (i.e. @ref ecu_dlist_destroy())
 * within your @p destroy_0 callback, otherwise behavior is undefined.
 * This callback should define any <b>additional</b> cleanup needed 
 * to destroy your user-defined data stored in this node. Cleanup of
 * the actual node is automatically done by this module.
 * 
 * @param me Node to construct. This cannot be NULL.
 * @param destroy_0 Optional callback that defines additional cleanup 
 * needed to destroy your user-defined data stored in this node. 
 * @ref ecu_dnode_get_id() and @ref ECU_DNODE_GET_ENTRY() can
 * be used in your callback to retrieve your user-defined data. This
 * function is called when this node is apart of a list that is 
 * destroyed by @ref ecu_dlist_destroy(). This function also executes 
 * when @ref ecu_dnode_destroy() is called on this node. Supply 
 * @ref ECU_DNODE_DESTROY_UNUSED if unused.
 * @param id_0 Optional ID user can assign to each node to identify
 * different types stored in the same list. Supply @ref ECU_OBJECT_ID_UNUSED
 * if unused. This value must be greater than or equal to @ref ECU_VALID_OBJECT_ID_BEGIN
 */
extern void ecu_dnode_ctor(struct ecu_dnode *me,
                           void (*destroy_0)(struct ecu_dnode *me, ecu_object_id id),
                           ecu_object_id id_0);

/**
 * @pre @p me previously constructed via call to @ref ecu_dnode_ctor().
 * @brief Node destructor.
 * @details Removes node if it is in a list. Executes the user-defined 
 * destructor if one was supplied to @ref ecu_dnode_ctor().
 * 
 * @param me Node to destroy. This cannot be HEAD node in @ref ecu_dlist.
 */
extern void ecu_dnode_destroy(struct ecu_dnode *me);
/**@}*/

/**
 * @name Dnode Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p position and @p node previously constructed via calls to @ref ecu_dnode_ctor().
 * @brief Add node before position node.
 * 
 * @param me List to add to.
 * @param position Add @p node before this position node. This must
 * be within the supplied list @p me.
 * @param node Node to add. This cannot already be within the supplied
 * list @p me or apart of another list.
 * 
 * @note First member is the list for API consistency. @ref ecu_dlist_node_get_list(position)
 * can be used if you only have access to the position node.
 * @note It is safe to use this in the middle of an iteration. The
 * newly added node will <b>not</b> be iterated over. 
 */

// inserting before a HEAD node will make the node the new TAIL, not a new HEAD.!
extern void ecu_dnode_insert_before(struct ecu_dnode *me, struct ecu_dnode *position);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p position and @p node previously constructed via calls to @ref ecu_dlist_node_ctor().
 * @brief Add node after position node.
 * 
 * @param me List to add to.
 * @param position Add @p node before this position node. This must
 * be within the supplied list @p me.
 * @param node Node to add. This cannot already be within the supplied
 * list @p me or apart of another list.
 * 
 * @note First member is the list for API consistency. @ref ecu_dlist_node_get_list(position)
 * can be used if you only have access to the position node.
 * @note It is safe to use this in the middle of an iteration. The
 * newly added node will <b>not</b> be iterated over.
 */
extern void ecu_dnode_insert_after(struct ecu_dnode *me, struct ecu_dnode *position);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor().
 * @brief Remove node from list. 
 * 
 * @p me List to remove from.
 * @p node Node to remove. This must be within the supplied
 * list @p me.
 * 
 * @note First member is the list for API consistency. @ref ecu_dlist_node_get_list(node)
 * can be used if you only have access to the node.
 * @note It is safe to use this in the middle of an iteration.
 */
extern void ecu_dnode_remove(struct ecu_dnode *me);

/**
 * @brief TODO
 * 
 * @param me 
 * @return true 
 * @return false 
 */
extern bool ecu_dnode_in_list(const struct ecu_dnode *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_node_ctor().
 * @brief Return node's ID.
 * @details The ID returned is the one set in @ref ecu_dlist_node_ctor().
 * This value can be used to differentiate between multiple types
 * stored in the same list.
 * 
 * @param me Node to check. This cannot be HEAD node in @ref ecu_dlist.
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
 * @details Removes and resets all nodes within the list. User-defined 
 * destructors for each node will execute if they were supplied 
 * in @ref ecu_dlist_node_ctor().
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
 * @brief TODO. Removes all dnodes from list but does not destroy them.
 * 
 * @param me 
 */
extern void ecu_dlist_clear(struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor().
 * @brief Add node to front of the list.
 * 
 * @param me List to add to.
 * @param node Node to add. Node cannot already be within the supplied 
 * list or apart of another active list.
 * 
 * @note This is not meant to be used in the middle of an iteration however 
 * it is safe to do so. If used during an iteration, the newly added node 
 * will <b>not</b> be iterated over.
 */
extern void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dnode *node);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor().
 * @brief Add node to back of the list.
 * 
 * @param me List to add to.
 * @param node Node to add. Node cannot already be within the supplied 
 * list @p me or apart of another list.
 * 
 * @note This is not meant to be used in the middle of an iteration 
 * however it is safe to do so. If used during an iteration, it is 
 * undefined whether the newly added node will be iterated over or not.
 */
extern void ecu_dlist_push_back(struct ecu_dlist *me, struct ecu_dnode *node);

// insert node at position
// Insert a node in a location depending on a external condition. The cond() function 
// checks if the node is to be inserted before the current node against which it is checked.
extern void ecu_dlist_insert_before(struct ecu_dlist *me, 
                                    struct ecu_dnode *node,
                                    bool (*condition)(const struct ecu_dnode *node, const struct ecu_dnode *position, void *data),
                                    void *data);

// TODO Add sort function.
// assert if nodes in list are ever different IDs.
// less_than returns true if lhs < rhs. Otherwise return false if lhs >= rhs
// callback is not pointer to const to give user as much flexibility as possible.
// uses merge sort which is O(nlogn)
// behavior undefined if supplied nodes to compare callback
// are removed/added to list.
extern void ecu_dlist_sort(struct ecu_dlist *me, 
                           bool (*lhs_less_than_rhs)(const struct ecu_dnode *lhs, const struct ecu_dnode *rhs, void *data),
                           void *data);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns number of nodes in a list. Returns 0 if list
 * is empty.
 * 
 * @param me List to check.
 */
// This is an O(n) function.
extern size_t ecu_dlist_get_size(const struct ecu_dlist *me);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor().
 * @brief Returns true if list is empty. False otherwise.
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
 * @details If the list has nodes this returns the first user-defined node. 
 * If the list is empty this returns the same terminal node returned by
 * @ref ecu_dlist_iterator_end().
 * 
 * @param me Non-const iterator to initialize.
 * @param list List to iterate through. This is not declared as pointer to const
 * since iterator functions can return non-const @ref ecu_dlist.head pointers.
 */
extern struct ecu_dnode *ecu_dlist_iterator_begin(struct ecu_dlist_iterator *me, struct ecu_dlist *list);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin().
 * @brief Returns last node in the list.
 * @details The last node is a dummy delimeter that is not apart of the
 * user's list. Therefore it should never be used directly.
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
 * @details If the list has nodes this returns the first user-defined node. 
 * If the list is empty this returns the same terminal node returned by
 * @ref ecu_dlist_const_iterator_end().
 * 
 * @param me Const iterator to initialize.
 * @param list List to iterate through.
 */
extern const struct ecu_dnode *ecu_dlist_const_iterator_begin(struct ecu_dlist_const_iterator *me, const struct ecu_dlist *list);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin().
 * @brief Returns last node in the list.
 * @details The last node is a dummy delimeter that is not apart of the
 * user's list. Therefore it should never be used directly.
 * 
 * @param me Const iterator.
 */
extern const struct ecu_dnode *ecu_dlist_const_iterator_end(struct ecu_dlist_const_iterator *me);

/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin().
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
