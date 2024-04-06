/**
 * @file circular_dll.h
 * @author Ian Ress
 * @brief Circular doubly linked list without dynamic memory allocation.
 * @details Provides list addition, removal, and iterators. All class members are meant 
 * to be private but they are exposed to the Application so memory can easily be allocated 
 * at compile-time. I.e. don't have to use memory pool. Example use:
 * @code{.c}
 * #include <ecu/circular_dll.h>
 * 
 * struct user_data
 * {
 *     int x;
 *     struct ecu_circular_dll_node me;
 *     int y;
 * };
 * 
 * struct user_data data1;
 * struct user_data data2;
 * struct user_data data3;
 * 
 * struct ecu_circular_dll list;
 * 
 * // Constructing and adding nodes to list.
 * ecu_circular_dll_ctor(&list);
 * ecu_circular_dll_push_back(&list, &data1.me);
 * ecu_circular_dll_push_back(&list, &data3.me);
 * ecu_circular_dll_push_back(&list, &data2.me);
 * 
 * // Iterating through a list.
 * struct ecu_circular_dll_iterator iterator;
 * for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&iterator, &list);
 *      node != ecu_circular_dll_iterator_end(&iterator); 
 *      node = ecu_circular_dll_iterator_next(&iterator))
 * {
 *      struct user_data *element = ECU_CIRCULAR_DLL_GET_ENTRY(node, struct user_data, me);
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
 * @brief Return entry containing user data that was stored in the linked list. 
 * @details This will return a pointer to whatever user-defined type was used to 
 * store the node. See @ref circular_dll.h description for an example using 
 * this macro.
 * 
 * @param ptr Address of linked list node. This will be of type (struct ecu_circular_dll_node *).
 * @param type User-defined type of elements in the list. I.e. (struct user_defined_type).
 * @param member Name of linked list node member within user-defined type. This will
 * be of type (struct ecu_circular_dll_node).
 */
#define ECU_CIRCULAR_DLL_GET_ENTRY(ptr, type, member)   ((type *)((char *)(ptr) - offsetof(type, member)))

// #error "Get rid of these macros"
// /**
//  * @brief Use to initialize node at compile-time.
//  * 
//  * @param node This must be of type (struct ecu_circular_dll_node)
//  */
// #define ECU_CIRCULAR_DLL_NODE_CTOR_COMPILETIME(node)    { .next = &(node), .prev = &(node) }


// /**
//  * @brief Use to initialize list at compile-time.
//  * 
//  * @param list This must be of type (struct ecu_circular_dll)
//  */
// #define ECU_CIRCULAR_DLL_LIST_CTOR_COMPILETIME(list)    { .terminal_node = { .next = &(list).terminal_node, .prev = &(list).terminal_node } }



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- CIRCULAR DOUBLY LINKED LIST -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Single node within list. 
 */
struct ecu_circular_dll_node
{
    /**
     * @brief PRIVATE. Next node in list.
     */
    struct ecu_circular_dll_node *next;

    /**
     * @brief PRIVATE. Previous node in list.
     */
    struct ecu_circular_dll_node *prev;

    /**
     * @brief PRIVATE. Optional ID user can assign to each node
     * in case different types are stored in the same list.
     */
    uint8_t id;

    /**
     * @brief PRIVATE. Optional user-defined callback that
     * defines node's destructor. Called when @ref ecu_circular_dll_destroy() 
     * is called.
     * 
     * @warning Do not call @ref ecu_circular_dll_destroy() or directly
     * edit next and prev pointers in this callback. User should only 
     * define any additional cleanup necessary for their data type.
     */
    void (*destroy)(struct ecu_circular_dll_node *me);
};


/**
 * @brief Circular doubly linked list.
 */
struct ecu_circular_dll
{
    /**
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
     * @brief PRIVATE. List that is being iterated.
     */
    const struct ecu_circular_dll *list;

    /**
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
 * Memory must be allocated for all objects beforehand since dynamic memory
 * allocation is not used.
 */
/**@{*/
/**
 * @brief Node constructor.
 * 
 * @warning If destroy_0 callback is supplied, do not call @ref ecu_circular_dll_destroy()
 * or directly edit node's next and prev pointers in your callback. User
 * should only define any additional cleanup necessary for their data type.
 * Cleanup of the actual ecu_circular_dll_node type is done automatically
 * in this module.
 * 
 * @param me Node to construct. This cannot be NULL.
 * @param id_0 Optional ID user can assign to each node in case different 
 * types are stored in the same list. It is recommended to set this to 0
 * if unused.
 * @param destroy_0 Optional user-defined callback that defines additional
 * cleanup for node's destructor. Called when @ref ecu_circular_dll_destroy() 
 * is called. Set to NULL if unused.
 */
extern void ecu_circular_dll_node_ctor(struct ecu_circular_dll_node *me, uint8_t id_0,
                                       void (*destroy_0)(struct ecu_circular_dll_node *me));


/**
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
 * @brief List destructor. 
 * @details Removes all nodes from the list and calls user-supplied
 * destructor for each node if one was supplied in @ref ecu_circular_dll_node_ctor().
 * 
 * @param me List to destroy. This cannot be NULL. This must be a list that
 * was previously constructed via constructor call.
 */
extern void ecu_circular_dll_destroy(struct ecu_circular_dll *me);
/**@}*/


/**
 * @name List Addition, Removal, and Status
 */
/**@{*/
/**
 * @brief Add node to back of the list.
 * 
 * @param me List to add to. This cannot be NULL. Constructor must have been 
 * called on this list beforehand.
 * @param node Node to add. This cannot be NULL. Constructor must have been
 * called on this node beforehand. Node cannot already be within the supplied 
 * list, or apart of another list.
 */
extern void ecu_circular_dll_push_back(struct ecu_circular_dll *me, 
                                       struct ecu_circular_dll_node *node);


/**
 * @brief Remove node from list. Previous and next nodes in list are 
 * automatically adjusted.
 * 
 * @param me Node to remove. This cannot be NULL. This node must be 
 * within a list.
 */
extern void ecu_circular_dll_remove_node(struct ecu_circular_dll_node *me);


/**
 * @brief Returns number of nodes in a list.
 * 
 * @param me List to get size of. This cannot be NULL. Constructor must 
 * have been called on this list beforehand.
 * 
 * @return Number of nodes in the list.
 * 
 * @note This iterates over the entire list so this is a longer function.
 */
extern uint32_t ecu_circular_dll_get_size(const struct ecu_circular_dll *me);

// TODO: Description
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
 * @brief Initializes iterator and returns first node in the list.
 * @details If the list has nodes this is the first user-defined node. 
 * If the list is empty this returns the terminal node which is a dummy 
 * delimeter that represents HEAD and TAIL of the list.
 * 
 * @param me Iterator object to construct. This cannot be NULL - 
 * memory must be allocated for iterator beforehand since dynamic memory 
 * allocation is not used.
 * @param list List to iterate through. This cannot be NULL. Constructor
 * must have been called on this list beforehand and it must have valid
 * next and prev pointers.
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_begin(struct ecu_circular_dll_iterator *me,
                                                                     const struct ecu_circular_dll *list);


/**
 * @brief Returns terminal node in the list. 
 * @details This is a dummy delimeter that represents HEAD and TAIL of 
 * the list and should never be used directly.
 * 
 * @param me Iterator object. This cannot be NULL. This must have
 * been created beforehand via call to @ref ecu_circular_dll_iterator_begin().
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *me);


/**
 * @brief Returns the next node in the iteration.
 * 
 * @param me Iterator object. This cannot be NULL. This must have
 * been created beforehand via call to @ref ecu_circular_dll_iterator_begin().
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *me);
/**@}*/



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: OTHER --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @name List and Iterator Assert Handler
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
