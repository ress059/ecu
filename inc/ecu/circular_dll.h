/**
 * @file
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
 * for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&list, &iterator);
 *      node != ecu_circular_dll_iterator_end(&iterator); 
 *      node = ecu_circular_dll_iterator_next(&iterator))
 * {
 *      struct user_data *element = ECU_CIRCULAR_DLL_GET_ENTRY(node, struct user_data, me);
 *      element->x = 10;
 *      element->y = 10;
 * }
 * @endcode
 * @author Ian Ress
 * 
 */


#ifndef ECU_CIRCULAR_DLL_H_
#define ECU_CIRCULAR_DLL_H_


#include <stddef.h> /* offsetof() */
#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- MACROS ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Return entry containing user data that was stored in the linked 
 * list. This will return a pointer to whatever user-defined type was used to 
 * store the node. See @ref circular_dll.h description for an example using 
 * this macro.
 * 
 * @param ptr Address of linked list node. This will be of type (struct ecu_circular_dll_node *).
 * @param type User-defined type of elements in the list. I.e. (struct user_defined_type).
 * @param member Name of linked list node member within user-defined type. This will
 * be of type (struct ecu_circular_dll_node).
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
     * @brief PRIVATE. Next node in list.
     */
    struct ecu_circular_dll_node *next;

    /**
     * @brief PRIVATE. Previous node in list.
     */
    struct ecu_circular_dll_node *prev;
};


/**
 * @brief Circular doubly linked list.
 */
struct ecu_circular_dll
{
    /**
     * @brief PRIVATE. Represents HEAD and TAIL node of the list. 
     * @details Dummy node used as delimeter - not apart of user's list.
     */
    struct ecu_circular_dll_node terminal_node;
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
    struct ecu_circular_dll *list;

    /**
     * @brief PRIVATE. Current position in list.
     */
    struct ecu_circular_dll_node *current;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC METHODS: LIST -----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructors
 */
/**@{*/
/**
 * @brief Constructor.
 * 
 * @param list List to construct. This cannot be NULL - memory must be allocated
 * for list beforehand since dynamic memory allocation is not used. This should
 * not be a list that is currently active and has nodes in it since the list's 
 * next and prev pointers are reset in this constructor.
 */
extern void ecu_circular_dll_ctor(struct ecu_circular_dll *list);


/**
 * @brief Destructor. Removes all nodes from the list. Resets all nodes by
 * having their next and prev pointers point to the nodes themsevles.
 * 
 * @param list List to destroy. This cannot be NULL. This must be a list that
 * was previously constructed via constructor call.
 */
extern void ecu_circular_dll_destroy(struct ecu_circular_dll *list);
/**@}*/ /* Constructors */


/**
 * @name List Addition and Removal
 */
/**@{*/
/**
 * @brief Add node to back of the list.
 * 
 * @param list List to add to. This cannot be NULL. Constructor must have been 
 * called on this list beforehand.
 * @param node Node to add. This cannot be NULL - memory must be allocated for 
 * node beforehand since dynamic memory allocation is not used. Node cannot already
 * be within the supplied list.
 */
extern void ecu_circular_dll_push_back(struct ecu_circular_dll *list, 
                                       struct ecu_circular_dll_node *node);


/**
 * @brief Remove node from list. Previous and next nodes in list are 
 * automatically adjusted.
 * 
 * @param list List to remove from. This cannot be NULL. Constructor 
 * must have been called on this list beforehand.
 * @param node Node to remove. This cannot be NULL. This node must be 
 * within the supplied list.
 */
extern void ecu_circular_dll_remove_node(struct ecu_circular_dll *list, 
                                         struct ecu_circular_dll_node *node);


/**
 * @brief Returns number of nodes in a list.
 * 
 * @param list List to get size of. This cannot be NULL. Constructor must 
 * have been called on this list beforehand.
 * 
 * @return Number of nodes in the list.
 */
extern uint32_t ecu_circular_dll_get_size(struct ecu_circular_dll *list);
/**@}*/ /* List Addition and Removal */



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC METHODS: ITERATORS --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @name Iterators
 */
/**@{*/
/**
 * @brief Returns first node in the list. 
 * @details If the list has nodes this is the first user-defined node. If 
 * the list is empty this returns the terminal node which is a dummy delimeter 
 * that represents HEAD and TAIL of the list.
 * 
 * @param list List to iterate through. This cannot be NULL. Constructor
 * must have been called on this list beforehand and it must have valid
 * next and prev pointers.
 * @param iterator Iterator object to construct. This cannot be NULL - 
 * memory must be allocated for iterator beforehand since dynamic memory 
 * allocation is not used.
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_begin(struct ecu_circular_dll *list,
                                                                     struct ecu_circular_dll_iterator *iterator);


/**
 * @brief Returns terminal node in the list. 
 * @details This is a dummy delimeter that represents HEAD and TAIL of 
 * the list.
 * 
 * @param iterator Iterator object. This cannot be NULL. This must have
 * been created beforehand via call to @ref ecu_circular_dll_iterator_begin().
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *iterator);


/**
 * @brief Returns the next node in the iteration.
 * 
 * @param iterator Iterator object. This cannot be NULL. This must have
 * been created beforehand via call to @ref ecu_circular_dll_iterator_begin().
 */
extern struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *iterator);
/**@}*/ /* Iterators */

#ifdef __cplusplus
}
#endif

#endif /* ECU_CIRCULAR_DLL_H_ */
