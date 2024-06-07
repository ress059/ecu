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


// require root to be first member so destroy callback is backwards compatible with
// both tree root and tree node.
// ECU_STATIC_ASSERT( (offsetof(struct ecu_tree, root) == 0) );



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- FILE-SCOPE VARIABLES ----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *TREE_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;

// static const struct ecu_tree_node ROOT_DELIMETER = {0}; // used to differentiate tree root from tree node.


/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DECLARATIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// static struct ecu_tree_node *get_root(struct ecu_tree_node *node); // pointer to nonconst since function can potentially return the parameter.
static struct ecu_tree_node *get_child_leaf(struct ecu_tree_node *node); // pointer to nonconst since function can potentially return the parameter.
// static bool node_is_tree_root(const struct ecu_tree_node *node);
static bool node_in_valid_sibling_list(const struct ecu_tree_node *node);
static bool node_child_head_valid(const struct ecu_tree_node *node);
static bool node_valid(const struct ecu_tree_node *node);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// parameter is not pointer to const since function can return the node itself.
// static struct ecu_tree_node *get_root(struct ecu_tree_node *node)
// {
//     struct ecu_tree_node *root = node;
//     ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );
//     ECU_RUNTIME_ASSERT( (node->parent), TREE_ASSERT_FUNCTOR );

//     while ((root->parent != root) || (root->parent != &ROOT_DELIMETER))
//     {
//         root = root->parent;
//         ECU_RUNTIME_ASSERT( (root), TREE_ASSERT_FUNCTOR );
//     }

//     return root;
// }


static struct ecu_tree_node *get_child_leaf(struct ecu_tree_node *node)
{
    struct ecu_tree_node *leaf = node;
    ECU_RUNTIME_ASSERT( (node_valid(node)), TREE_ASSERT_FUNCTOR );

    while (leaf->child != leaf)
    {
        leaf = leaf->child;
        ECU_RUNTIME_ASSERT( (node_valid(leaf)), TREE_ASSERT_FUNCTOR );
    }

    return leaf;
}


// static bool node_is_tree_root(const struct ecu_tree_node *node)
// {
//     ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );
//     return (node->parent == &ROOT_DELIMETER);

//     // if ((node->parent == node) && \
//     //     (node->next == node) && \
//     //     (node->prev == node) && \
//     //     (node->destroy == &tree_root_destroy_callback) && \
//     //     (node->id == ECU_OBJECT_ID_RESERVED))
//     // {
//     //     is_root = true;
//     // }

//     // return is_root;
// }


// precondition is node has been constructed.
static bool node_in_valid_sibling_list(const struct ecu_tree_node *node)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node->next) && (node->prev)), TREE_ASSERT_FUNCTOR );

    /* Note how this also handles case where node has no siblings. next
    and prev should point to itself. */
    if ((node->next->prev == node) && (node->prev->next == node))
    {
        valid = true;
    }

    return valid;
}


static bool node_child_head_valid(const struct ecu_tree_node *node)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node->child), TREE_ASSERT_FUNCTOR );

    /* Cases handled:
    1. node->child == node. Node has no children so child points to itself. 
    2. node->child->parent == node. Node has children. */
    if ((node->child == node) || (node->child->parent == node))
    {
        valid = true;
    }

    return valid;
}


static bool node_valid(const struct ecu_tree_node *node)
{
    return (node_in_valid_sibling_list(node) && \
            node_child_head_valid(node));
}


// static void tree_root_destroy_callback(struct ecu_tree_node *me)
// {
//     (void)me;
//     ECU_RUNTIME_ASSERT( (false), TREE_ASSERT_FUNCTOR ); // TODO Dont think I should do this. Will go off in iterator?
// }



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: TREE ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// void ecu_tree_ctor(struct ecu_tree *me,
//                    void (*destroy_0)(struct ecu_tree *me),
//                    ecu_object_id id_0)
// {
//     ECU_RUNTIME_ASSERT( ((me) && (id_0 >= ECU_VALID_OBJECT_ID_BEGIN)), 
//                         TREE_ASSERT_FUNCTOR );

//     me->root.child      = &me->root;
//     me->root.parent     = &ROOT_DELIMETER; /* Used to differentiate root from normal tree node. */
//     me->root.next       = &me->root;
//     me->root.prev       = &me->root;
//     me->root.destroy    = destroy_0;
//     me->root.id         = id_0;


//     // make destroy function take in struct ecu_tree_node *me instead?
//     // ecu_tree_node_ctor(me, destroy_0, id_0);
//     // me->parent = &ROOT_DELIMETER;
// }


void ecu_tree_node_ctor(struct ecu_tree_node *me, 
                        void (*destroy_0)(struct ecu_tree_node *me),
                        ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( ((me) && (id_0 >= ECU_VALID_OBJECT_ID_BEGIN)), 
                        TREE_ASSERT_FUNCTOR );

    me->child           = me;
    me->parent          = me;
    me->next            = me;
    me->prev            = me;
    me->destroy         = destroy_0;
    me->id              = id_0;
}


void ecu_tree_node_destroy(struct ecu_tree_node *me)
{
    struct ecu_tree_postorder_iterator iterator;

    ECU_RUNTIME_ASSERT( (node_valid(me)), TREE_ASSERT_FUNCTOR );

    for (struct ecu_tree_node *node = ecu_tree_postorder_iterator_begin(&iterator, me);
         node != ecu_tree_postorder_iterator_end(&iterator); 
         node = ecu_tree_postorder_iterator_next(&iterator))
    {
        ecu_tree_remove_node(node);

        /* Do not reset node->id so user is able to identify their data type 
        in their destructor callback. */
        if (node->destroy)
        {
            (*node->destroy)(node);
            node->destroy = (void (*)(struct ecu_tree_node *))0;
        }
    }
}


void ecu_tree_add_child_push_back(struct ecu_tree_node *parent, 
                                  struct ecu_tree_node *new_child)
{
    struct ecu_tree_node *old_tail = (struct ecu_tree_node *)0;

    /* NULL assertions already done in these functions. Notice it is OK if new_child as previously a root. */
    ECU_RUNTIME_ASSERT( ((new_child != parent) && (node_valid(parent)) && \
                         (node_valid(new_child))), TREE_ASSERT_FUNCTOR );

    ecu_tree_remove_node(new_child);

    /* Need to handle edge case where node we are adding is the parent's first child. */
    if (parent->child == parent)
    {
        /* Parent has no children. This new node is the parent's first child. */
        parent->child   = new_child;
        new_child->next = new_child;
        new_child->prev = new_child;
    }
    else
    {
        /* Parent already has children. */
        old_tail = parent->child->prev;
        old_tail->next->prev = new_child;   /* parent->child->prev = new_child */
        new_child->next = old_tail->next;   /* new_child->next = parent->child */
        new_child->prev = old_tail;
        old_tail->next = new_child;
    }

    new_child->parent = parent;
}


// dont detach node from its children.
void ecu_tree_remove_node(struct ecu_tree_node *me)
{
    /* NULL assertions already done in these functions. Note it is
    OK for the supplied node to be the root of the tree. In this
    case nothing will be done. */
    ECU_RUNTIME_ASSERT( (node_valid(me)), TREE_ASSERT_FUNCTOR );

    /* Need to handle edge case where node we are removing is the first child. */
    if (me->parent->child == me)
    {
        if (me->next == me)
        {
            /* Node's parent only has one child (this node). 0 children after removal. */
            me->parent->child = me->parent;
        }
        else 
        {
            /* Node's parent has other children. Adjust first child pointer to next node in list. */
            me->parent->child = me->next;
        }
    }

    me->next->prev = me->prev;
    me->prev->next = me->next;
    me->next = me;
    me->prev = me;
    me->parent = me;
}




// root is level 0.
/* for example:
       0
    1       2
3

node 3 would be at level 2.
*/
size_t ecu_tree_get_level(const struct ecu_tree_node *me)
{
    size_t level = 0;
    const struct ecu_tree_node *root = me;

    ECU_RUNTIME_ASSERT( (root), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (root->parent), TREE_ASSERT_FUNCTOR );

    while (root->parent != root)
    {
        level++;
        root = root->parent;
        ECU_RUNTIME_ASSERT( (root), TREE_ASSERT_FUNCTOR );
    }

    return level;
}


// return null if nodes in different trees.
struct ecu_tree_node *ecu_tree_get_lca(struct ecu_tree_node *node1, 
                                       struct ecu_tree_node *node2)
{
    ECU_RUNTIME_ASSERT( (node1 && node2), TREE_ASSERT_FUNCTOR );

    size_t level1 = 0;
    size_t level2 = 0;
    struct ecu_tree_node *lca = (struct ecu_tree_node *)0;

    if (node1 == node2)
    {
        lca = node1;
    }
    else
    {
        /* We do not have to NULL assert when getting node->parent because
        ecu_tree_get_level() already traverses and NULL asserts parent 
        structure. 
        
        LCA algorithm:
        1. Find whichever node is at a deeper level.

        2. Only for the deeper node, traverse up the tree (get parent) 
        until you are at the same level as the other node. If nodes
        are equal LCA is found. Otherwise proceed to Step #3.
        
        3. Traverse up the tree (get parent) for both nodes. If both
        nodes are equal LCA is found. Otherwise if you reached the root
        of the tree but both nodes are still not equal then the
        nodes are in separate trees. Return null in this case. */
        level1 = ecu_tree_get_level(node1);
        level2 = ecu_tree_get_level(node2);

        /* Steps 1 and 2 of algorithm. */
        if (level1 > level2)
        {
            while (level1 != level2)
            {
                node1 = node1->parent;
                level1--;
            }
        }
        else if (level2 > level1)
        {
            while (level1 != level2)
            {
                node2 = node2->parent;
                level2--;
            }
        }

        /* Step 3 of algorithm. */
        while ((node1 != node2) && (level1 != 0))
        {
            node1 = node1->parent;
            node2 = node2->parent;
            level1--;
        }

        if (node1 == node2)
        {
            lca = node1;
        }
        else
        {
            lca = (struct ecu_tree_node *)0; /* Nodes in different trees. */
        }
    }

    return lca;
}


bool ecu_tree_nodes_in_same_tree(struct ecu_tree_node *node1,
                                 struct ecu_tree_node *node2)
{
    return (ecu_tree_get_lca(node1, node2) != (struct ecu_tree_node *)0);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- PUBLIC FUNCTIONS: CHILDREN ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_node *ecu_tree_child_iterator_begin(struct ecu_tree_child_iterator *me,
                                                    struct ecu_tree_node *parent)
{
    struct ecu_tree_node *start_node = (struct ecu_tree_node *)0;
    struct ecu_tree_node *next_node = (struct ecu_tree_node *)0;
    ECU_RUNTIME_ASSERT( (me && parent), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(parent)), TREE_ASSERT_FUNCTOR );

    start_node = parent->child;
    ECU_RUNTIME_ASSERT( (node_valid(start_node)), TREE_ASSERT_FUNCTOR );

    if (start_node == parent || start_node->next == start_node)
    {
        /* 0 or 1 child. */
        next_node = parent;
    }
    else
    {
        next_node = start_node->next;
        ECU_RUNTIME_ASSERT( (node_valid(next_node)), TREE_ASSERT_FUNCTOR );
    }

    me->head = parent;
    me->current = start_node;
    me->next = next_node;
    return (me->current);
}


struct ecu_tree_node *ecu_tree_child_iterator_end(struct ecu_tree_child_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(me->head)), TREE_ASSERT_FUNCTOR );
    return (me->head);
}


struct ecu_tree_node *ecu_tree_child_iterator_next(struct ecu_tree_child_iterator *me)
{
    struct ecu_tree_node *current_node = (struct ecu_tree_node *)0;
    struct ecu_tree_node *next_node = (struct ecu_tree_node *)0;
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node_valid(me->head)) && (node_valid(me->current)) && \
                         (node_valid(me->next))), TREE_ASSERT_FUNCTOR );

    /* Current is what was stored in next from previous call to 
    this function. Update our next value. */
    current_node = me->next;
    next_node = me->next;

    if (current_node->next == me->head->child)
    {
        next_node = me->head;
    }
    else if (current_node == me->head)
    {
        next_node = me->head->child;
    }
    else
    {
        next_node = current_node->next;
    }

    ECU_RUNTIME_ASSERT( (node_valid(next_node)), TREE_ASSERT_FUNCTOR );
    me->current = current_node;
    me->next = next_node;
    return (me->current);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC FUNCTIONS: POSTORDER ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_node *ecu_tree_postorder_iterator_begin(struct ecu_tree_postorder_iterator *me,
                                                        struct ecu_tree_node *root)
{
    struct ecu_tree_node *start_node = (struct ecu_tree_node *)0;
    struct ecu_tree_node *next_node = (struct ecu_tree_node *)0;
    ECU_RUNTIME_ASSERT( (me && root), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(root)), TREE_ASSERT_FUNCTOR );

    ecu_tree_node_ctor(&me->delimiter, 
                       (void (*)(struct ecu_tree_node *))0, 
                       ECU_OBJECT_ID_UNUSED);

    start_node = get_child_leaf(root);

    if (start_node == root)
    {
        next_node = &me->delimiter;
    }
    else
    {
        next_node = start_node->next;
        ECU_RUNTIME_ASSERT( (node_valid(next_node)), TREE_ASSERT_FUNCTOR );

        if (next_node == next_node->parent->child)
        {
            next_node = next_node->parent;
            ECU_RUNTIME_ASSERT( (node_valid(next_node)), TREE_ASSERT_FUNCTOR );
        }
        else
        {
            next_node = get_child_leaf(next_node);
        }
    }

    me->root = root;
    me->current = start_node;
    me->next = next_node;
    return (me->current);
}


struct ecu_tree_node *ecu_tree_postorder_iterator_end(struct ecu_tree_postorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    return (&me->delimiter);
}


struct ecu_tree_node *ecu_tree_postorder_iterator_next(struct ecu_tree_postorder_iterator *me)
{
    struct ecu_tree_node *current_node = (struct ecu_tree_node *)0;
    struct ecu_tree_node *next_node = (struct ecu_tree_node *)0;
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node_valid(me->root)) && (node_valid(me->current)) && \
                         (node_valid(me->next))), TREE_ASSERT_FUNCTOR );

    /* Current is what was stored in next from previous call to 
    this function. Update our next value. */
    current_node = me->next;
    next_node = me->next;

    if (current_node == me->root)
    {
        next_node = &me->delimiter;
    }
    else if (current_node == &me->delimiter)
    {
        next_node = get_child_leaf(me->root);
    }
    else
    {
        next_node = next_node->next;
        ECU_RUNTIME_ASSERT( (node_valid(next_node)), TREE_ASSERT_FUNCTOR );

        if (next_node == next_node->parent->child)
        {
            next_node = next_node->parent;
            ECU_RUNTIME_ASSERT( (node_valid(next_node)), TREE_ASSERT_FUNCTOR );
        }
        else
        {
            next_node = get_child_leaf(next_node);
        }
    }
    
    me->current = current_node;
    me->next = next_node;
    return (me->current);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: OTHER --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_tree_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    TREE_ASSERT_FUNCTOR = functor;
}
