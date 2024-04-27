/**
 * @file
 * @brief 
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-24
 * @copyright Copyright (c) 2024
 */



#ifndef ECU_TREE_H_
#define ECU_TREE_H_

#include <stdint.h>
/* Siblings are represented by a linked list. */
#include <ecu/circular_dll.h>
#include <ecu/object_id.h>



#define ECU_TREE_NODE_GET_ENTRY(ptr, type, member)   ((type *)((char *)(ptr) - offsetof(type, member)))

typedef uint8_t ecu_tree_level_t;

struct ecu_tree_node
{
    struct ecu_circular_dll_node node; // has id and destroy
    struct ecu_circular_dll children;
    struct ecu_tree_node *parent;

    // ecu_object_id id;
    // void (*destroy)(struct ecu_tree_node *me);
    // first immediate child if (node->child == node) then node has no children
    // dummy node for iterator.
};


struct ecu_tree
{
    struct ecu_tree_node root;
};


struct ecu_tree_sibling_iterator
{
    struct ecu_circular_dll_iterator dll_iterator;
};


// breadth-first. parse all nodes at given depth. Go down one level deeper and repeat.
// fifo.
//       1
//    2    3
// 4 5 6   

// 1. Add node to queue.                    [1]
//      node = 1;
// 2. Add all node's siblings to queue.   [3, 2, 1]
//      next = 2. next = 3. next = 1....node = 1->child = 2;
// 3. Remove node from queue.               [3, 2]
//      node = 1->child = 2;
// Repeat process. Get all next node's siblings. Add to fifo [6, 5, 4, 3, 2] 
//      next = 4. next = 5. next = 6. next = 2...node = 2->next = 3....
// Remove node. [6, 5, 4, 3]
// Get next node's siblings. none so remove node [6, 5, 4]
// Get next node's siblings. None so remove node [6, 5]...[6]....[0]
struct ecu_tree_breadth_iterator
{
    struct ecu_circular_dll_node *head; // current parent.
    struct ecu_circular_dll_iterator dll_iterator;
};


// depth-first. Start at node. Go down branch until you reach leaf node. Go up one level and repeat.
struct ecu_tree_depth_iterator
{

};


#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: TREE ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

extern void ecu_tree_ctor(struct ecu_tree *me);

extern void ecu_tree_node_ctor(struct ecu_tree_node *me, 
                               void (*destroy_0)(struct ecu_circular_dll_node *me),
                               ecu_object_id id_0);

extern void ecu_tree_destroy(struct ecu_tree *me);
extern void ecu_tree_node_add(struct ecu_tree_node *me, struct ecu_tree_node *parent);
extern void ecu_tree_node_remove(struct ecu_tree_node *me);
extern ecu_tree_level_t ecu_tree_node_get_level(struct ecu_tree_node *me); // me->parent->parent->parent..until you reach root node.
extern struct ecu_tree_node *ecu_tree_find_lca(struct ecu_tree_node *node1, struct ecu_tree_node *node2); // return null if two nodes aren't apart of same tree?



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// iterators.
// iterate over all node's children (first level. like a triangle).
// iterate over all node's children and nested children.
// iterate over entire tree.

extern struct ecu_tree_node *ecu_tree_sibling_iterator_begin(struct ecu_tree_sibling_iterator *me,
                                                             struct ecu_tree_node *node);

extern struct ecu_tree_node *ecu_tree_sibling_iterator_end(struct ecu_tree_sibling_iterator *me);
extern struct ecu_tree_node *ecu_tree_sibling_iterator_next(struct ecu_tree_sibling_iterator *me);


extern struct ecu_tree_node *ecu_tree_breadth_iterator_begin(struct ecu_tree_breadth_iterator *me,
                                                             struct ecu_tree *tree);

extern struct ecu_tree_node *ecu_tree_breadth_iterator_end(struct ecu_tree_breadth_iterator *me);
extern struct ecu_tree_node *ecu_tree_breadth_iterator_next(struct ecu_tree_breadth_iterator *me);

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
extern void ecu_tree_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_TREE_H_ */
