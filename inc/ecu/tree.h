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
 * @private 
 * @brief PRIVATE. Defined so this can easily be changed in the future.
 * Maximum number of tree levels. I.e. if this is uint16_t the max tree
 * levels would be 65535.
 * 
 * @warning This must be an unsigned type since number of tree levels
 * is always positive. A compilation error will occur if this is 
 * declared as a signed type.
 */
typedef uint_fast16_t ecu_tree_max_level_t;


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
    void (*destroy)(struct ecu_tree_node *me);

    /**
     * @private
     * @brief PRIVATE. Optional ID user can assign to each node
     * to identify different types stored in the same tree. See
     * description of @ref object_id.h
     */
    ecu_object_id id;
};


// represents dummy delimeter.
struct ecu_tree
{
    struct ecu_tree_node root;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- ITERATORS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_children_iterator
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

extern void ecu_tree_ctor(struct ecu_tree *me);

extern void ecu_tree_node_ctor(struct ecu_tree_node *me, 
                               void (*destroy_0)(struct ecu_tree_node *me),
                               ecu_object_id id_0);

extern void ecu_tree_destroy(struct ecu_tree *me);
extern void ecu_tree_push_back(struct ecu_tree_node *me, struct ecu_tree_node *parent);
extern void ecu_tree_remove_node(struct ecu_tree_node *me);
extern ecu_tree_max_level_t ecu_tree_node_get_level(const struct ecu_tree_node *me); // me->parent->parent->parent..until you reach root node.
extern struct ecu_tree_node *ecu_tree_find_lca(struct ecu_tree_node *node1, 
                                               struct ecu_tree_node *node2); // return null if two nodes aren't apart of same tree?



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- PUBLIC FUNCTIONS: CHILDREN ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// iterators.
// iterate over all node's children (first level. like a triangle).
// iterate over all node's children and nested children.
// iterate over entire tree.

extern struct ecu_tree_node *ecu_tree_children_iterator_begin(struct ecu_tree_children_iterator *me,
                                                             struct ecu_tree_node *node);
extern struct ecu_tree_node *ecu_tree_children_iterator_end(struct ecu_tree_children_iterator *me);
extern struct ecu_tree_node *ecu_tree_children_iterator_next(struct ecu_tree_children_iterator *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC FUNCTIONS: POSTORDER ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

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
