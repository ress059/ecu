/**
 * @file circular_sll.h
 * @author Ian Ress
 * @brief Circular singly linked list implementation without dynamic memory allocation.
 * All class members are meant to be private but they are exposed to the Application so 
 * memory can easily be allocated at compile-time. I.e. don't have to use memory pool.
 * @version 0.1
 * @date 2023-12-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef CIRCULAR_SLL_H_
#define CIRCULAR_SLL_H_


#include <stdbool.h>
#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- CIRCULAR SINGLY LINKED LIST -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Single node within list. 
 */
struct circular_sll_node
{
    /**
     * @brief PRIVATE. Next node.
     */
    struct circular_sll_node *next;
};


/**
 * @brief Circular singly linked list.
 */
struct circular_sll
{
    /**
     * @brief PRIVATE. Represents HEAD and TAIL node of the list. Dummy node used
     * as delimeter - not apart of user's list.
     */
    struct circular_sll_node terminal_node;
};


/**
 * @brief List iterator. Allows users to construct multiple iterators
 * that operate indepenently of one another.
 */
struct circular_sll_iterator
{
    /**
     * @brief PRIVATE. List that is being iterated.
     */
    struct circular_sll *list;

    /**
     * @brief PRIVATE. Current position in list.
     */
    struct circular_sll_node *current;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC METHODS: LIST ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Constructor.
 * 
 * @param list List to construct. This cannot be NULL - memory must be allocated
 * for list beforehand since dynamic memory allocation is not used. This cannot be 
 * a list that is currently active and has nodes in it. Call destructer then this 
 * constructor if the list needs to be reconstructed.
 */
extern void circular_sll_ctor(struct circular_sll *list);


/**
 * @brief Destructor. Removes all nodes from the list.
 * 
 * @param list List to destroy. This cannot be NULL. This must be a list that
 * was previously constructed via constructor call.
 */
extern void circular_sll_destroy(struct circular_sll *list);


/**
 * @brief Add node to back of the list.
 * 
 * @param list List to add to. This cannot be NULL. Constructor must have been 
 * called on this list beforehand.
 * @param node Node to add. This cannot be NULL - memory must be allocated for 
 * node beforehand since dynamic memory allocation is not used. Node cannot be 
 * within another list.
 */
extern void circular_sll_push_back(struct circular_sll *list, struct circular_sll_node *node);


/**
 * @brief Add node at specific position.
 * 
 * @param position Node is added after this position. For example if your list
 * is [a,b,c] and this parameter is 'b' your new list will be [a,b,new_node,c].
 * Position cannot be NULL and must be in an existing list.
 * @param node Node to add. This cannot be NULL - memory must be allocated for 
 * node beforehand since dynamic memory allocation is not used. Node cannot be 
 * within another list.
 */
extern void circular_sll_add_node(struct circular_sll_node *position, struct circular_sll_node *node);


/**
 * @brief Remove node from list. Previous and next nodes in list are 
 * automatically adjusted.
 * 
 * @param node Node to remove. This cannot be NULL. This must be within
 * a list.
 */
extern void circular_sll_remove_node(struct circular_sll_node *node);


/**
 * @brief Returns number of nodes in a list.
 * 
 * @param list List to get size of. This cannot be NULL. Constructor must 
 * have been called on this list beforehand.
 * @return Number of nodes in the list.
 */
extern uint32_t circular_sll_get_size(struct circular_sll *list);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- PUBLIC METHODS: ITERATORS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Returns first node in the list. If the list has nodes this is the
 * first user-defined node. If the list is empty this returns the terminal 
 * node which is a dummy delimeter that represents HEAD and TAIL of the list.
 * 
 * @param list List to iterate through. This cannot be NULL. Constructor
 * must have been called on this list beforehand.
 * @param iterator Iterator object to construct. This cannot be NULL - 
 * memory must be allocated for iterator beforehand since dynamic memory 
 * allocation is not used.
 */
extern struct circular_sll_node *circular_sll_iterator_begin(struct circular_sll *list,
                                                             struct circular_sll_iterator *iterator);


/**
 * @brief Returns terminal node in the list. This is a dummy delimeter 
 * that represents HEAD and TAIL of the list.
 * 
 * @param iterator Iterator object. This cannot be NULL. This must have
 * been created beforehand via call to the begin function.
 */
extern struct circular_sll_node *circular_sll_iterator_end(struct circular_sll_iterator *iterator);


/**
 * @brief Returns the next node in the iteration.
 * 
 * @param iterator Iterator object. This cannot be NULL. This must have
 * been created beforehand via call to the begin function.
 */
extern struct circular_sll_node *circular_sll_iterator_next(struct circular_sll_iterator *iterator);


#endif /* CIRCULAR_SLL_H_ */
