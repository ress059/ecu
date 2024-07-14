/**
 * @file
 * @brief TODO
 * 
 * 1. Example of using ECU_TREE_NODE_GET_ENTRY() macro.
 * 2. 
 * 
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
#include <stdbool.h>
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
 * @brief Return pointer to user data that was stored in the tree node.
 * See @ref tree.h file description for an example using this macro. 
 * Users can set @ref ecu_tree_node.id to identify different types in 
 * their tree when using this macro if necessary.
 * 
 * @param ptr Address of tree node within user-defined struct. This will 
 * be of type (struct ecu_tree_node *).
 * @param type User-defined type that contains the tree node. 
 * I.e. (struct user_defined_type).
 * @param member Name of tree node member within the user-defined type.
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
     * @brief PRIVATE. First child.
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
     * defines node's destructor. Called when @ref ecu_tree_node_destroy() 
     * is called and node is apart of the destroyed tree.
     * 
     * @warning Do not call @ref ecu_tree_node_destroy() or directly
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



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- ITERATORS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Non-const iterator that iterates over node's children.
 * 
 * @note Iteration does not include further down levels 
 * (grandchildren, etc).
 */
struct ecu_tree_child_iterator
{
    /**
     * @private
     * @brief PRIVATE. The delimiter node which is the parent
     * node fed into @ref ecu_tree_child_iterator_begin(). Allows
     * iteration over all children using standard for/while loops.
     */
    struct ecu_tree_node *head;

    /**
     * @private
     * @brief PRIVATE. Current node the iteration is on.
     */
    struct ecu_tree_node *current;

    /**
     * @private 
     * @brief PRIVATE. Next node in the iteration. Having this 
     * member allows users to safely remove nodes in the middle
     * of an iteration.
     */
    struct ecu_tree_node *next;
};


/**
 * @brief Non-const postorder iterator. Can use this to iterate
 * over entire tree or any subtree within a tree.
 * 
 * @note Also iterates over the root of the tree.
 */
struct ecu_tree_postorder_iterator
{
    /**
     * @private
     * @brief PRIVATE. Root of the tree being iterated over. Used so
     * any subtree within a tree can also be iterated over.
     */
    struct ecu_tree_node *root;

    /**
     * @private
     * @brief PRIVATE. Current node the iteration is on.
     */
    struct ecu_tree_node *current;

    /**
     * @private
     * @brief PRIVATE. Next node in the iteration. Having this 
     * member allows users to safely remove nodes in the middle
     * of an iteration.
     */
    struct ecu_tree_node *next;

    /**
     * @private
     * @brief PRIVATE. Dummy delimiter node that is not apart of the
     * user's tree. Allows iteration over entire tree using standard
     * for/while loops.
     */
    struct ecu_tree_node delimiter;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: TREE ---------------------------------------------*/
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
 * @brief Tree node constructor.
 * 
 * @warning @p me cannot be an active node apart of an existing tree. 
 * Otherwise contents of the tree may be lost and behavior is undefined.
 * @warning If @p destroy_0 callback is supplied, do not call @ref ecu_tree_node_destroy(),
 * @ref ecu_tree_remove_node(), or directly edit any members of @ref ecu_tree_node 
 * in your callback. Users should only define any additional cleanup necessary for 
 * their data type. Cleanup of the actual @ref ecu_tree_node type is done automatically 
 * in this module.
 * 
 * @param me Tree node to construct. This cannot be NULL.
 * @param destroy_0 Optional user-defined callback that defines additional
 * cleanup for node's destructor. Called when @ref ecu_tree_node_destroy() 
 * is called and node is apart of the destroyed tree. Supply NULL if unused.
 * @param id_0 Optional ID user can assign to each node to identify different 
 * types stored in the same tree. Supply @ref ECU_OBJECT_ID_UNUSED if unused.
 * See @ref object_id.h description for more details. This value must be 
 * greater than or equal to @ref ECU_VALID_OBJECT_ID_BEGIN
 */
extern void ecu_tree_node_ctor(struct ecu_tree_node *me, 
                               void (*destroy_0)(struct ecu_tree_node *me),
                               ecu_object_id id_0);


/**
 * @pre @p me previously constructed via call to @ref ecu_tree_node_ctor()
 * @brief Tree destructor. Removes supplied node and its entire subtree. Also
 * calls user-supplied destructor @ref ecu_tree_node.destroy for each removed 
 * node if one was supplied in @ref ecu_tree_node_ctor(). See image below for
 * examples.
 * 
 * @param me This node and its subtree will be destroyed. I.e. supplying the
 * tree root will destroy the entire tree. Supplying a node within a tree
 * will destroy the node and its subtree but the remaining tree will still be
 * intact.
 * 
 * @image html tree-ecu-tree-node-destroy.png "Tree Destructor Examples"
 */
extern void ecu_tree_node_destroy(struct ecu_tree_node *me);
/**@}*/


/**
 * @name Tree Manipulation
 */
/**@{*/
/**
 * @pre @p parent and @p new_child previously constructed via call 
 * to @ref ecu_tree_node_ctor().
 * @brief Adds node and its subtree to a parent node within a tree.
 * If the added node is already apart of an existing tree it will
 * be removed and added to this new tree. The added node will be
 * pushed back to the end of the sibling list. See images below for
 * examples.
 * 
 * @warning @p new_child cannot be the same as @p parent. I.e.
 * parent != child
 * @warning You cannot add a parent to one of its children. I.e. @p new_child 
 * cannot be the parent of @p parent.
 * 
 * @param parent Parent node to add child to.
 * @param new_child Node that is being added to the tree. This node's
 * subtree will also be added with its structure still intact.
 * 
 * @image html tree-ecu-tree-add-child-push-back.png "Adding Nodes Examples"
 */
extern void ecu_tree_add_child_push_back(struct ecu_tree_node *parent, 
                                         struct ecu_tree_node *new_child);

#warning "TODO: Stopped here. Still have to add image"
/**
 * @pre @p me previously constructed via call to @ref ecu_tree_node_ctor().
 * @brief Removes a node and its subtree from a tree. The node's
 * subtree will remain unharmed and intact. Therefore calling this on a 
 * tree root or calling this on a node not in a tree does nothing. See 
 * images below for examples.
 * 
 * @param me Node to remove.
 *
 * @image html tree-ecu-tree-remove-node.png "Removing Nodes Examples"
 */
extern void ecu_tree_remove_node(struct ecu_tree_node *me);
/**@}*/


/**
 * @name Tree Info
 */
/**@{*/
/**
 * @pre @p me previously constructed via call to @ref ecu_tree_node_ctor().
 * @brief Returns the level of a node within a tree. This is 0-indexed so the 
 * root node would return 0, a node one level lower in the tree would return 
 * 1, etc.
 * 
 * @param me Node to check.
 */
extern size_t ecu_tree_get_level(const struct ecu_tree_node *me);


#warning "TODO Show example image"
/**
 * @pre @p node1 and @p node2 previously constructed via call 
 * to @ref ecu_tree_node_ctor().
 * @brief Returns the Lowest Common Ancestor (LCA) between two nodes. If
 * the LCA is not found (nodes are in different trees) then NULL is returned.
 * See images below for futher details:
 * @image TODO LCA returns.
 * 
 * @note Parameters are pointers to non-const since this function returns
 * pointer to non-const and can potentially return either @p node1 or @p node2.
 */
extern struct ecu_tree_node *ecu_tree_get_lca(struct ecu_tree_node *node1, 
                                              struct ecu_tree_node *node2);


/**
 * @pre @p node1 and @p node2 previously constructed via call 
 * to @ref ecu_tree_node_ctor().
 * @brief Returns true if both nodes are within the same tree.
 * 
 * @note Parameters are pointers to non-const since this function calls
 * @ref ecu_tree_get_lca() which takes in pointers to non-const.
 */
extern bool ecu_tree_nodes_in_same_tree(struct ecu_tree_node *node1,
                                        struct ecu_tree_node *node2);
/**@}*/



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- PUBLIC FUNCTIONS: CHILDREN ITERATOR -------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/
#warning "TODO ADD message. It is safe to add nodes in the middle of the iteration but it is undefined whether \
these nodes will be iterated over. It is only guaranteed that the next iteratio nwill include all nodes."
#warning "TODO Add image"
/**
 * @name Non-const Children Iterator
 * Iterate over a parent node's children. This parent node is used as 
 * a delimiter. It is safe to remove nodes in the middle of an iteration.
 * Iteration does not include further down levels (grandchildren, etc).
 * Example use:
 * @code {.c}
 * struct ecu_tree_child_iterator iter;
 * 
 * for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iter, &parent_node);
 *      i != ecu_tree_child_iterator_end(&iter);
 *      i = ecu_tree_child_iterator_next(&iter))
 * {
 *      // Can supply 'i' to @ref ECU_TREE_NODE_GET_ENTRY() macro to access your
 *      // user-defined type and its members.
 * }
 * @endcode
 * 
 * Example iteration:
 * @image TODO
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p parent previously constructed via call to @ref ecu_tree_node_ctor().
 * @brief Start the iteration. Initializes iterator and returns the first 
 * child in the iteration. If the parent has no children this returns the 
 * parent node since it is used as a delimiter. It is safe to remove nodes 
 * in the middle of an iteration.
 * 
 * @param me Iterator object to initialize.
 * @param parent Iterate over this node's children. This parent node is also used
 * as a delimiter for the iteration.
 */
extern struct ecu_tree_node *ecu_tree_child_iterator_begin(struct ecu_tree_child_iterator *me,
                                                           struct ecu_tree_node *parent);

/**
 * @pre @p me previously initialized via call to @ref ecu_tree_child_iterator_begin().
 * @brief Returns end node in the iteration which is the parent node supplied to
 * @ref ecu_tree_child_iterator_begin().
 * 
 * @param me Iterator object.
 */
extern struct ecu_tree_node *ecu_tree_child_iterator_end(struct ecu_tree_child_iterator *me);


/**
 * @pre @p me previously initialized via call to @ref ecu_tree_child_iterator_begin()
 * @brief Returns the next node in the iteration.
 * 
 * @param me Iterator object.
 */
extern struct ecu_tree_node *ecu_tree_child_iterator_next(struct ecu_tree_child_iterator *me);
/**@}*/



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- PUBLIC FUNCTIONS: POSTORDER ITERATOR ------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#warning "TODO ADD message. It is safe to add nodes in the middle of the iteration but it is undefined whether \
these nodes will be iterated over. It is only guaranteed that the next iteratio nwill include all nodes."
/**
 * @name Non-const Postorder Iterator
 * Iterate over entire tree or any subtree within a tree in postorder
 * fashion. Also iterates over the root of the tree. It is safe to
 * remove nodes in the middle of an iteration. Example use:
 * @code {.c}
 * struct ecu_tree_postorder_iterator iter;
 * 
 * for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iter, &root_node);
 *      i != ecu_tree_postorder_iterator_end(&iter);
 *      i = ecu_tree_postorder_iterator_next(&iter))
 * {
 *      // Can supply 'i' to @ref ECU_TREE_NODE_GET_ENTRY() macro to access your
 *      // user-defined type and its members.
 * }
 * @endcode
 * 
 * 
 * Example iteration:
 * @image TODO postorder iteration
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p root previously constructed via call to @ref ecu_tree_node_ctor().
 * @brief Start the iteration. Initializes iterator and returns the first 
 * node in the iteration. It is safe to remove nodes in the middle of an 
 * iteration.
 * 
 * @param me Iterator object to initialize.
 * @param root Root of the tree you want to iterate over. This can be
 * the root of an entire tree or any subtree. If this is a root of
 * a tree you would be iterating over the entire tree. If this is
 * the root of a subtree you would only be iterating over the
 * subtree (and not any other nodes contained within the tree).
 */
extern struct ecu_tree_node *ecu_tree_postorder_iterator_begin(struct ecu_tree_postorder_iterator *me,
                                                               struct ecu_tree_node *root);


/**
 * @pre @p me previously initialized via call to @ref ecu_tree_postorder_iterator_begin().
 * @brief Returns the last node in the iteration which is a dummy delimiter
 * contained within @ref ecu_tree_postorder_iterator structure, which is
 * not apart of the user's tree.
 * 
 * @param me Iterator object.
 */
extern struct ecu_tree_node *ecu_tree_postorder_iterator_end(struct ecu_tree_postorder_iterator *me);


/**
 * @pre @p me previously initialized via call to @ref ecu_tree_postorder_iterator_begin()
 * @brief Returns the next node in the iteration.
 * 
 * @param me Iterator object.
 */
extern struct ecu_tree_node *ecu_tree_postorder_iterator_next(struct ecu_tree_postorder_iterator *me);
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
extern void ecu_tree_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_TREE_H_ */
