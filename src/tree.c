/**
 * @file
 * @brief See @ref tree.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-25
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- INCLUDES ------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include <ecu/tree.h>

/* STDLib. */
#include <stdbool.h>

/* Runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *TREE_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DECLARATIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool node_in_tree(struct ecu_tree_node *node);
static bool node_is_root(struct ecu_tree_node *node);
static void tree_root_destroy_callback(struct ecu_circular_dll_node *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// precondition is node is constructed
static bool node_in_tree(struct ecu_tree_node *node)
{
    bool in_tree = false;
    ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node->child) && (node->parent), TREE_ASSERT_FUNCTOR );

    if ((node->parent != node) && \
        (node->child->parent == node || node->child->parent == node->parent))
    {
        in_tree = true;
    }
    else
    {
        in_tree = node_is_root(node);
    }

    return in_tree;
}


static bool node_is_root(struct ecu_tree_node *node)
{
    bool is_root = false;
    ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );

    if ((node->parent == node) && (node->node.id == ECU_OBJECT_ID_RESERVED) && \
        (node->node.destroy == &tree_root_destroy_callback))
    {
        is_root = true;
    }

    return is_root;
}


static void tree_root_destroy_callback(struct ecu_circular_dll_node *me)
{
    (void)me;
    ECU_RUNTIME_ASSERT( (false), TREE_ASSERT_FUNCTOR );
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: TREE ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_tree_ctor(struct ecu_tree *me)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ecu_tree_node_ctor(&me->root, &tree_root_destroy_callback, ECU_OBJECT_ID_UNUSED);

    /* Have to manually set ID since passing reserved ID to public API is forbidden.*/
    me->root.node.id = ECU_OBJECT_ID_RESERVED;
}


void ecu_tree_node_ctor(struct ecu_tree_node *me, 
                        void (*destroy_0)(struct ecu_circular_dll_node *me),
                        ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );

    ecu_circular_dll_node_ctor(&me->node, destroy_0, id_0);
    ecu_circular_dll_ctor(&me->siblings);
    me->parent      = me;
    me->child       = me;
}


void ecu_tree_destroy(struct ecu_tree *me)
{
    // todo. loop through all nodes and call custom destructor for each node.
}


// node with siblings can be added safely i think.
void ecu_tree_node_add(struct ecu_tree_node *me, struct ecu_tree_node *parent)
{
    /* NULL assertions already done in these functions. */
    ECU_RUNTIME_ASSERT( ((!node_in_tree(me)) && (node_in_tree(parent))), 
                         TREE_ASSERT_FUNCTOR );

    ecu_circular_dll_push_back(&parent->siblings, &me->node); /* Verifies sibling tree is valid. */
    me->parent = parent;
}


// dont detach node from its siblings. just detach node from tree.
void ecu_tree_node_remove(struct ecu_tree_node *me)
{
    ECU_RUNTIME_ASSERT( ((node_in_tree(me)) && \
                         (!node_is_root(me))), TREE_ASSERT_FUNCTOR ); 
    
    ecu_circular_dll_remove_node(&me->node);
    me->parent = me;
}




/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_node *ecu_tree_sibling_iterator_begin(struct ecu_tree_sibling_iterator *me,
                                                      struct ecu_tree_node *node)
{
    ECU_RUNTIME_ASSERT( (me && node), TREE_ASSERT_FUNCTOR );
    struct ecu_circular_dll_node *start = ecu_circular_dll_iterator_begin(&me->dll_iterator, &node->siblings);
    return (ECU_CIRCULAR_DLL_GET_ENTRY(start, struct ecu_tree_node, node));
}


struct ecu_tree_node *ecu_tree_sibling_iterator_end(struct ecu_tree_sibling_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    struct ecu_circular_dll_node *end = ecu_circular_dll_iterator_end(&me->dll_iterator);
    return (ECU_CIRCULAR_DLL_GET_ENTRY(end, struct ecu_tree_node, node));
}


struct ecu_tree_node *ecu_tree_sibling_iterator_next(struct ecu_tree_sibling_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    struct ecu_circular_dll_node *next = ecu_circular_dll_iterator_next(&me->dll_iterator);
    return (ECU_CIRCULAR_DLL_GET_ENTRY(next, struct ecu_tree_node, node));
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: OTHER --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_tree_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    TREE_ASSERT_FUNCTOR = functor;
}