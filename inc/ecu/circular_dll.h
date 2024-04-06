/**
 * @file
 * @author Ian Ress
 * @brief Circular doubly linked list without dynamic memory allocation.
 * @details Provides list addition, removal, status, and iterators. Private class members
 * denoted by 'PRIVATE' in descriptions are exposed to the Application so memory can easily 
 * be allocated at compile-time. Example use:
 * @code{.c}
 * #include <ecu/circular_dll.h>
 * 
 * struct user_data
 * {
 *     int x;
 *     struct ecu_circular_dll_node node;
 *     int y;
 * };
 * 
 * struct user_data data1;
 * struct user_data data2;
 * struct user_data data3;
 * 
 * struct ecu_circular_dll list;
 * 
 * // Constructing list and nodes. Not using node id or destroy callbacks in this example.
 * ecu_circular_dll_ctor(&list);
 * ecu_circular_dll_node_ctor(&data1.node, 0, 0);
 * ecu_circular_dll_node_ctor(&data2.node, 0, 0);
 * ecu_circular_dll_node_ctor(&data3.node, 0, 0);
 * 
 * // Adding nodes to list.
 * ecu_circular_dll_push_back(&list, &data1.node);
 * ecu_circular_dll_push_back(&list, &data3.node);
 * ecu_circular_dll_push_back(&list, &data2.node);
 * 
 * // Iterating through a list.
 * struct ecu_circular_dll_iterator iterator;
 * for (struct ecu_circular_dll_node *i = ecu_circular_dll_iterator_begin(&iterator, &list);
 *      i != ecu_circular_dll_iterator_end(&iterator); 
 *      i = ecu_circular_dll_iterator_next(&iterator))
 * {
 *      // Retrieving and using user-defined data type stored in list.
 *      struct user_data *element = ECU_CIRCULAR_DLL_GET_ENTRY(i, struct user_data, node);
 *      element->x = 10;
 *      element->y = 10;
 * }
 * @endcode
 * @version 0.1
 * @date 2024-04-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef ECU_CIRCULAR_DLL_H_
#define ECU_CIRCULAR_DLL_H_


/* STDLib. */
#include <stdbool.h>
#include <stddef.h> /* offsetof() */
#include <stdint.h>


/* Runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- MACROS ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Return pointer to user data that was stored in the linked list.
 * @details See @ref circular_dll.h description for an example using 
 * this macro. Users can set @ref ecu_circular_dll_node.id to identify
 * different types in their list when using this macro.
 * 
 * @param ptr Address of linked list node within user-defined struct. This will be of 
 * type (struct ecu_circular_dll_node *).
 * @param type User-defined type stored in the list. I.e. (struct user_defined_type).
 * @param member Name of linked list node member within user-defined type.
 */
#define ECU_CIRCULAR_DLL_GET_ENTRY(ptr, type, member)   ((type *)((char *)(ptr) - offsetof(type, member)))



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- CIRCULAR DOUBLY LINKED LIST -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Single node within list. 
 */
struct ecu_circular_dll_node
{
    /**
     * @private
     * @brief PRIVATE. Next node in list.
     */
    struct ecu_circular_dll_node *next;

    /**
     * @private
     * @brief PRIVATE. Previous node in list.
     */
    struct ecu_circular_dll_node *prev;

    /**
     * @private
     * @brief PRIVATE. Optional ID user can assign to each node
     * to identify different types stored in the same list.
     */
    uint8_t id;

    /**
     * @private
     * @brief PRIVATE. Optional user-defined callback that
     * defines node's destructor. Called when @ref ecu_circular_dll_destroy() 
     * is called and node is apart of the destroyed list.
     * 
     * @warning Do not call @ref ecu_circular_dll_destroy() or directly
     * edit members of @ref ecu_circular_dll_node directly in this callback. 
     * User should only define any additional cleanup necessary for their data type.
     */
    void (*destroy)(struct ecu_circular_dll_node *me);
};


/**
 * @brief Circular doubly linked list.
 */
struct ecu_circular_dll
{
    /**
     * @private
     * @brief PRIVATE. Dummy node used as delimeter to represent
     * start and end of list. Not apart of user's list.
     */
    struct ecu_circular_dll_node head;
};


/**
 * @brief List iterator. Allows users to construct multiple iterators
 * that operate indepenently of one another.
 */
struct ecu_circular_dll_iterator
{
    /**
     * @private
     * @brief PRIVATE. List that is being iterated.
     * 
     * @note This is not declared as a pointer to const since iterator functions
     * can return non-const @ref ecu_circular_dll.head
     */
    struct ecu_circular_dll *list;

    /**
     * @private
     * @brief PRIVATE. Current position in list.
     * 
     * @note This is not declared as a pointer to const since iterator 
     * returns non-const nodes.
     */
    struct ecu_circular_dll_node *current;
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
 * @brief Node constructor.
 * 
 * @warning If @p destroy_0 callback is supplied, do not call @ref ecu_circular_dll_destroy()
 * or directly edit any members of @ref ecu_circular_dll_node in your callback. Users 
 * should only define any additional cleanup necessary for their data type. Cleanup of the 
 * actual @ref ecu_circular_dll_node type is done automatically in this module.
 * 
 * @param me Node to construct. This cannot be NULL.
 * @param id_0 Optional ID user can assign to each node to identify different 
 * types stored in the same list.
 * @param destroy_0 Optional user-defined callback that defines additional
 * cleanup for node's destructor. Called when @ref ecu_circular_dll_destroy() 
 * is called and node is apart of the destroyed list.
 */
extern void ecu_circular_dll_node_ctor(struct ecu_circular_dll_node *me, uint8_t id_0,
                                       void (*destroy_0)(struct ecu_circular_dll_node *me));


/**
 * @pre Memory already allocated for @p me.
 * @brief List constructor.
 * 
 * @warning An active list that has nodes in it cannot be supplied, otherwise
 * list will detach itself from all its nodes. Remaining behavior is undefined 
 * for this case.
 * 
 * @param me List to construct. This cannot be NULL. 
 */
extern void ecu_circular_dll_ctor(struct ecu_circular_dll *me);


/**
 * @pre @p me previously constructed via call to @ref ecu_circular_dll_ctor()
 * @brief List destructor. Removes all nodes from the list and calls user-supplied
 * destructor @ref ecu_circular_dll_node.destroy for each node if one was supplied 
 * in @ref ecu_circular_dll_node_ctor().
 * 
 * @param me List to destroy.
 */
extern void ecu_circular_dll_destroy(struct ecu_circular_dll *me);
/**@}*/


/**
 * @name List Addition, Removal, and Status
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_circular_dll_ctor()
 * @pre @p node previously constructed via call to @ref ecu_circular_dll_node_ctor()
 * @brief Add node to back of the list.
 * 
 * @param me List to add to.
 * @param node Node to add. Node cannot already be within the supplied 
 * list or apart of another active list.
 */
extern void ecu_circular_dll_push_back(struct ecu_circular_dll *me, 
                                       struct ecu_circular_dll_node *node);


/**
 * @pre @p me previously constructed via call to @ref ecu_circular_dll_node_ctor()
 * @brief Remove node from list. Previous and next nodes in list are 
 * automatically adjusted.
 * 
 * @param me Node to remove. This node must be within a list.
 */
extern void ecu_circular_dll_remove_node(struct ecu_circular_dll_node *me);


/**
 * @pre @p me previously constructed via call to @ref ecu_circular_dll_ctor()
 * @brief Returns number of nodes in a list.
 * 
 * @param me List to get size of. This is not declared as pointer to const
 * since non-const iterators are used in this function.
 * 
 * @note This iterates over the entire list so this is a longer function.
 */
extern uint32_t ecu_circular_dll_get_size(struct ecu_circular_dll *me);


/**
 * @pre @p me previously constructed via call to @ref ecu_circular_dll_ctor()
 * @brief Returns true if list is empty. False otherwise.
 * 
 * @param me List to check.
 */
extern bool ecu_circular_dll_is_empty(const struct ecu_circular_dll *me);
/**@}*/



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @name Iterators
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me
 * @pre @p list previously constructed via call to @ref ecu_circular_dll_ctor()
 * @brief Initializes iterator and returns first node in the list.
 * @details If the list has nodes this is the first user-defined node. 
 * If the list is empty this returns the same terminal node returned by
 * @ref ecu_circular_dll_iterator_end().
 * 
 * @param me Iterator object to initialize.
 * @param list List to iterate through. This is not declared as pointer to const
 * since iterator functions can return non-const @ref ecu_circular_dll.head pointers.
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_begin(struct ecu_circular_dll_iterator *me,
                                                                     struct ecu_circular_dll *list);


/**
 * @pre @p me previously initialized via call to ecu_circular_dll_iterator_begin()
 * @brief Returns terminal node in the list which is a dummy delimeter that 
 * represents HEAD and TAIL of the list. Returned node should never be used 
 * directly.
 * 
 * @param me Iterator object.
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *me);


/**
 * @pre @p me previously initialized via call to ecu_circular_dll_iterator_begin()
 * @brief Returns the next node in the iteration.
 * 
 * @param me Iterator object.
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *me);
/**@}*/



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: OTHER --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @name Asserts In This Module
 */
/**@{*/
/**
 * @brief Set a functor to execute if an assert fires within this module. 
 * @details This is optional - if no functor is set a default one will be 
 * used. The default functor hangs in a permanent while loop if NDEBUG is 
 * not defined so users are able to inspect the call stack.
 * 
 * @param functor User-supplied functor. If a NULL value is supplied
 * the default functor will be used.
 */
extern void ecu_circular_dll_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_CIRCULAR_DLL_H_ */
