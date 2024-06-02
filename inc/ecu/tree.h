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



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ INCLUDES ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <stddef.h> /* offsetof() */
#include <stdint.h>

/* Runtime asserts. */
#include <ecu/asserter.h>

/* ecu_tree_node::id */
#include <ecu/object_id.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- MACROS ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Return pointer to user data that was stored in the tree.
 * 
 * @param ptr Address of tree node within user-defined struct. This will be of 
 * type (struct ecu_tree_node *).
 * @param type User-defined type stored in the list. I.e. (struct user_defined_type).
 * @param member Name of tree node member within user-defined type.
 */
#define ECU_TREE_NODE_GET_ENTRY(ptr, type, member)   ((type *)((char *)(ptr) - offsetof(type, member)))



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- GENERIC TREE --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Single node within tree.
 */
struct ecu_tree_node
{
    /**
     * @private 
     * @brief PRIVATE. Pointer to first child.
     */
    struct ecu_tree_node *child;

    /**
     * @private 
     * @brief PRIVATE. Parent.
     */
    struct ecu_tree_node *parent;

    /**
     * @private
     * @brief PRIVATE. Next sibling. Siblings are represented by
     * a circular linked list.
     */
    struct ecu_tree_node *next;

    /**
     * @private
     * @brief PRIVATE. Previous sibling. Siblings are represented by
     * a circular linked list.
     */
    struct ecu_tree_node *prev;
    
    /**
     * @private
     * @brief PRIVATE. Optional user-defined callback that
     * defines node's destructor. Called when @ref ecu_tree_destroy() 
     * is called and node is apart of the destroyed tree.
     * 
     * @warning Do not call @ref ecu_tree_destroy() or directly
     * edit members of @ref ecu_tree_node directly in this callback. 
     * User should only define any additional cleanup necessary for 
     * their data type.
     */
    // Parameter will either be struct ecu_tree_node * or struct ecu_tree *.
    // User can use the ECU_TREE_NODE_GET_ENTRY() macro in both cases and use
    // the object id to determine their user-defined types that store the ecu_tree
    // and ecu_tree_node.
    // object id can be used for type determination.
    //
    void (*destroy)(struct ecu_tree_node *me);

    /**
     * @private
     * @brief PRIVATE. Optional ID user can assign to each node
     * to identify different types stored in the same tree. See
     * description of @ref object_id.h
     */
    ecu_object_id id;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- ITERATORS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_child_iterator
{
    struct ecu_tree_node *head; // delimiter. the parent is used as head
    struct ecu_tree_node *current;
    struct ecu_tree_node *next;
};



struct ecu_tree_postorder_iterator
{
    struct ecu_tree_node *root;
    struct ecu_tree_node *current;
    struct ecu_tree_node *next;
    struct ecu_tree_node delimiter;
};




/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: TREE ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


extern void ecu_tree_node_ctor(struct ecu_tree_node *me, 
                               void (*destroy_0)(struct ecu_tree_node *me),
                               ecu_object_id id_0);

// destroys node and all nodes in its subtree.
extern void ecu_tree_node_destroy(struct ecu_tree_node *me);


// new_child cannot equal parent.
extern void ecu_tree_add_child_push_back(struct ecu_tree_node *parent, 
                                         struct ecu_tree_node *new_child);

// node's subtree will be unharmed. Therefore calling this on root does nothing.
// calling this on a node not in a tree also does nothing.
extern void ecu_tree_remove_node(struct ecu_tree_node *me);

// 0 indexed. Top level = 0.
extern size_t ecu_tree_get_level(const struct ecu_tree_node *me); // me->parent->parent->parent..until you reach root node.

// return null if lca not found (nodes not in same tree)
extern struct ecu_tree_node *ecu_tree_get_lca(struct ecu_tree_node *node1, 
                                              struct ecu_tree_node *node2); // return null if two nodes aren't apart of same tree?



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- PUBLIC FUNCTIONS: CHILDREN ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// iterators.
// iterate over all node's children (first level. like a triangle).
// iterate over all node's children and nested children.
// iterate over entire tree.
// notice this takes in a ecu_tree_node instead of ecu_tree so you can iterate over subtrees. Pass in
    // ecu_tree_node.root to iterate over an entire tree.

extern struct ecu_tree_node *ecu_tree_child_iterator_begin(struct ecu_tree_child_iterator *me,
                                                           struct ecu_tree_node *node);
extern struct ecu_tree_node *ecu_tree_child_iterator_end(struct ecu_tree_child_iterator *me);
extern struct ecu_tree_node *ecu_tree_child_iterator_next(struct ecu_tree_child_iterator *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC FUNCTIONS: POSTORDER ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// does not iterate over root.
// notice this takes in a ecu_tree_node instead of ecu_tree so you can iterate over subtrees. Pass in
    // ecu_tree_node.root to iterate over an entire tree.
extern struct ecu_tree_node *ecu_tree_postorder_iterator_begin(struct ecu_tree_postorder_iterator *me,
                                                               struct ecu_tree_node *root);
extern struct ecu_tree_node *ecu_tree_postorder_iterator_end(struct ecu_tree_postorder_iterator *me);
extern struct ecu_tree_node *ecu_tree_postorder_iterator_next(struct ecu_tree_postorder_iterator *me);



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
