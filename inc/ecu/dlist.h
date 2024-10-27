/**
 * @file
 * @brief Circular doubly linked list without dynamic memory allocation. Provides list addition, 
 * removal, status, and iterators. Private class members denoted by 'PRIVATE' in descriptions 
 * are exposed to the Application so memory can easily be allocated at compile-time. Example use:
 * 
 * @code{.c}
 * #include <ecu/circular_dll.h>
 * 
 * struct user_data
 * {
 *     int x;
 *     struct ecu_dlist_node node;
 *     int y;
 * };
 * 
 * struct user_data data1;
 * struct user_data data2;
 * struct user_data data3;
 * 
 * struct ecu_dlist list;
 * 
 * // Constructing list and nodes. Not using node id or destroy callbacks in this example.
 * ecu_dlist_ctor(&list);
 * ecu_dlist_node_ctor(&data1.node, (void (*)(struct ecu_dlist_node *))0, ECU_OBJECT_ID_UNUSED);
 * ecu_dlist_node_ctor(&data2.node, (void (*)(struct ecu_dlist_node *))0, ECU_OBJECT_ID_UNUSED);
 * ecu_dlist_node_ctor(&data3.node, (void (*)(struct ecu_dlist_node *))0, ECU_OBJECT_ID_UNUSED);
 * 
 * // Adding nodes to list.
 * ecu_dlist_push_back(&list, &data1.node);
 * ecu_dlist_push_back(&list, &data2.node);
 * ecu_dlist_push_back(&list, &data3.node);
 * 
 * // Iterating through a list.
 * struct ecu_dlist_iterator iterator;
 * for (struct ecu_dlist_node *i = ecu_dlist_iterator_begin(&iterator, &list);
 *      i != ecu_dlist_iterator_end(&iterator); 
 *      i = ecu_dlist_iterator_next(&iterator))
 * {
 *      // Retrieving and using user-defined data type stored in list.
 *      struct user_data *element = ecu_dlist_GET_ENTRY(i, struct user_data, node);
 *      element->x = 10;
 *      element->y = 10;
 * }
 * @endcode
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-05
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_DLIST_H_
#define ECU_DLIST_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ INCLUDES ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h> /* offsetof() */
#include <stdint.h>

/* ecu_dlist_node::id */
#include "ecu/object_id.h"



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- MACROS ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Return pointer to user data that was stored in the linked list.
 * @details See @ref circular_dll.h description for an example using 
 * this macro. Users can set @ref ecu_dlist_node.id to identify
 * different types in their list when using this macro.
 * 
 * @param ptr Address of linked list node within user-defined struct. This will be of 
 * type (struct ecu_dlist_node *).
 * @param type User-defined type stored in the list. I.e. (struct user_defined_type).
 * @param member Name of linked list node member within user-defined type.
 */
#define ECU_DLIST_GET_ENTRY(ptr, type, member)   ((type *)((char *)(ptr) - offsetof(type, member)))



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- CIRCULAR DOUBLY LINKED LIST -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Single node within list. 
 */
struct ecu_dlist_node
{
    /**
     * @private
     * @brief PRIVATE. Next node in list.
     */
    struct ecu_dlist_node *next;

    /**
     * @private
     * @brief PRIVATE. Previous node in list.
     */
    struct ecu_dlist_node *prev;

    struct ecu_dlist *list; // list the node is in.

    /**
     * @private
     * @brief PRIVATE. Optional user-defined callback that
     * defines node's destructor. Called when @ref ecu_dlist_destroy() 
     * is called and node is apart of the destroyed list.
     * 
     * @warning Do not call @ref ecu_dlist_destroy() or directly
     * edit members of @ref ecu_dlist_node directly in this callback. 
     * User should only define any additional cleanup necessary for their data type.
     */
    void (*destroy)(struct ecu_dlist_node *me);

    /**
     * @private
     * @brief PRIVATE. Optional ID user can assign to each node
     * to identify different types stored in the same list. See
     * description of @ref object_id.h
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
     * @brief PRIVATE. Dummy node used as delimiter to represent
     * start and end of list. Not apart of user's list.
     */
    struct ecu_dlist_node head;
};


/**
 * @brief Non-const list iterator.
 */
struct ecu_dlist_iterator
{
    /**
     * @private
     * @brief PRIVATE. List that is being iterated. The node
     * in this member is used as the delimiter.
     */
    struct ecu_dlist *list;

    /**
     * @private
     * @brief PRIVATE. Current position in list.
     */
    struct ecu_dlist_node *current;

    /**
     * @private 
     * @brief PRIVATE. Next position in the list. This member allows
     * user to safely remove nodes from list while iterating through it.
     */
    struct ecu_dlist_node *next;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructors
 */
/**@{*/



/**
 * @pre Memory already allocated for @p me.
 * @brief List constructor.
 * 
 * @warning @p me cannot be an active list that has nodes in it. Otherwise the
 * list will detach itself from all its nodes and behavior is undefined.
 * 
 * @param me List to construct. This cannot be NULL. 
 */
extern void ecu_dlist_ctor(struct ecu_dlist *me);


/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor()
 * @brief List destructor. Removes all nodes from the list and calls user-supplied
 * destructor @ref ecu_dlist_node.destroy for each node if one was supplied 
 * in @ref ecu_dlist_node_ctor().
 * 
 * @param me List to destroy.
 */
extern void ecu_dlist_destroy(struct ecu_dlist *me);


/**
 * @pre Memory already allocated for @p me.
 * @brief Node constructor.
 * 
 * @warning @p me cannot be an active node apart of an existing list. Otherwise the list 
 * will become corrupted and behavior is undefined.
 * @warning If @p destroy_0 callback is supplied, do not call @ref ecu_dlist_destroy(),
 * @ref ecu_dlist_remove_node(), or directly edit any members of @ref ecu_dlist_node 
 * in your callback. Users should only define any additional cleanup necessary for their data type. 
 * Cleanup of the actual @ref ecu_dlist_node type is done automatically in this module.
 * @warning Read description of @ref object_id.h before supplying @p id_0 parameter.
 * 
 * @param me Node to construct. This cannot be NULL.
 * @param destroy_0 Optional user-defined callback that defines additional
 * cleanup for node's destructor. Called when @ref ecu_dlist_destroy() 
 * is called and node is apart of the destroyed list.
 * @param id_0 Optional ID user can assign to each node to identify different 
 * types stored in the same list. Set to @ref ECU_OBJECT_ID_UNUSED if unused.
 * See @ref object_id.h description for more details. This value must be 
 * greater than or equal to @ref ECU_VALID_OBJECT_ID_BEGIN
 */
extern void ecu_dlist_node_ctor(struct ecu_dlist_node *me,
                                void (*destroy_0)(struct ecu_dlist_node *me),
                                ecu_object_id id_0);

/**@}*/


/**
 * @name List Addition, Removal, and Status
 */
/**@{*/

// insert before node. think just have list for future compatibility but do not use it.
// just do same schem where verify position is in list.
extern void ecu_dlist_insert(struct ecu_dlist *me, struct ecu_dlist_node *position, struct ecu_dlist_node *new);

// general purpose pop function makes no sense since it needs to take in struct ecu_dlist_node *position which 
// will be returned as well.

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_node_ctor()
 * @brief Remove node from list. Previous and next nodes in list are 
 * automatically adjusted.
 * 
 * @param me Node to remove. This node must be within a list.
 */
extern void ecu_dlist_remove(struct ecu_dlist *me, struct ecu_dlist_node *node);


extern void ecu_dlist_push_front(struct ecu_dlist *me, struct ecu_dlist_node *new);

/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor()
 * @pre @p node previously constructed via call to @ref ecu_dlist_node_ctor()
 * @brief Add node to back of the list.
 * 
 * @param me List to add to.
 * @param node Node to add. Node cannot already be within the supplied 
 * list or apart of another active list.
 * 
 * @note If this function is called in the middle of an iteration, the
 * iterator WILL iterate through the newly added nodes.
 */
extern void ecu_dlist_push_back(struct ecu_dlist *me, 
                                struct ecu_dlist_node *new); // call ecu_dlist_insert(list.head)


// assert if empty list.
extern struct ecu_dlist_node *ecu_dlist_pop_front(struct ecu_dlist *me); // call ecu_dlist_remove()

// assert if list empty
extern struct ecu_dlist_node *ecu_dlist_pop_back(struct ecu_dlist *me); // call ecu_dlist_remove()


/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor()
 * @brief Returns number of nodes in a list.
 * 
 * @param me List to get size of. This is not declared as pointer to const
 * since non-const iterators are used in this function.
 * 
 * @note This iterates over the entire list so this is a longer function.
 */
extern size_t ecu_dlist_get_size(struct ecu_dlist *me);


/**
 * @pre @p me previously constructed via call to @ref ecu_dlist_ctor()
 * @brief Returns true if list is empty. False otherwise.
 * 
 * @param me List to check.
 */
extern bool ecu_dlist_is_empty(const struct ecu_dlist *me);

extern bool ecu_dlist_node_in(const struct ecu_dlist *me, const struct ecu_dlist_node *node);
/**@}*/



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @name Iterators
 * You can safely add and remove nodes in the middle of an iteration.
 * If nodes are added via @ref ecu_dlist_push_back() the iterator
 * WILL iterate through the newly added nodes.
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_dlist_ctor()
 * @brief Initializes iterator and returns first node in the list.
 * @details If the list has nodes this is the first user-defined node. 
 * If the list is empty this returns the same terminal node returned by
 * @ref ecu_dlist_iterator_end().
 * 
 * @param me Iterator object to initialize.
 * @param list List to iterate through. This is not declared as pointer to const
 * since iterator functions can return non-const @ref ecu_dlist.head pointers.
 */
extern struct ecu_dlist_node *ecu_dlist_iterator_begin(struct ecu_dlist_iterator *me,
                                                                     struct ecu_dlist *list);


/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin()
 * @brief Returns terminal node in the list which is a dummy delimiter that 
 * represents HEAD and TAIL of the list. Returned node should never be used 
 * directly.
 * 
 * @param me Iterator object.
 */
extern struct ecu_dlist_node *ecu_dlist_iterator_end(struct ecu_dlist_iterator *me);


/**
 * @pre @p me previously initialized via call to @ref ecu_dlist_iterator_begin()
 * @brief Returns the next node in the iteration.
 * 
 * @param me Iterator object.
 */
extern struct ecu_dlist_node *ecu_dlist_iterator_next(struct ecu_dlist_iterator *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_DLIST_H_ */
