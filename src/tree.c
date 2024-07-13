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



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- FILE-SCOPE VARIABLES ----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *TREE_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DECLARATIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_tree_node *get_child_leaf(struct ecu_tree_node *node); // pointer to nonconst since function can potentially return the parameter.
static bool node_in_valid_sibling_list(const struct ecu_tree_node *node);
static bool node_child_head_valid(const struct ecu_tree_node *node);
static bool node_valid(const struct ecu_tree_node *node);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_tree_node *get_child_leaf(struct ecu_tree_node *node)
{
    /* Do not assert node_valid(node->parent) since it is indirectly asserted
    by node_valid(node) in the first iteration of the while-loop. */
    ECU_RUNTIME_ASSERT( (node_valid(node)), TREE_ASSERT_FUNCTOR );

    while (node->child != node)
    {
        node = node->child;
        ECU_RUNTIME_ASSERT( (node_valid(node)), TREE_ASSERT_FUNCTOR );
    }

    return node;
}


// precondition is node has been constructed.
// WARNING: this function must check @p node for null in order for api using this function
// to remain consistent (api offloads null checking to this function)
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


// WARNING: this function must check @p node for null in order for api using this function
// to remain consistent (api offloads null checking to this function)
static bool node_child_head_valid(const struct ecu_tree_node *node)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node->child) && (node->parent)), TREE_ASSERT_FUNCTOR );

    /* Cases handled:
    1. node->child == node. Node has no children so child points to itself. 
    2. node->child->parent == node. Node has children. */
    if ((node->child == node) || (node->child->parent == node))
    {
        valid = true;
    }

    return valid;
}


// check if node is in valid sibling list and has valid child structure.
// WARNING: this function must check @p node for null in order for api using this function
// to remain consistent (api offloads null checking to this function)
static bool node_valid(const struct ecu_tree_node *node)
{
    return (node_in_valid_sibling_list(node) && \
            node_child_head_valid(node));
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: TREE ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

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

    /* Notice it is OK if new_child was previously a root. */
    ECU_RUNTIME_ASSERT( ((parent != new_child) && (node_valid(parent)) && \
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
        old_tail                = parent->child->prev;
        old_tail->next->prev    = new_child;            /* parent->child->prev = new_child */
        new_child->next         = old_tail->next;       /* new_child->next = parent->child */
        new_child->prev         = old_tail;
        old_tail->next          = new_child;
    }

    new_child->parent = parent;
}


// dont detach node from its children.
void ecu_tree_remove_node(struct ecu_tree_node *me)
{
    /* Note it is OK for the supplied node to be the root of the tree. 
    In this case nothing will be done. */
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

    me->next->prev  = me->prev;
    me->prev->next  = me->next;
    me->next        = me;
    me->prev        = me;
    me->parent      = me;
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

    /* Do not assert node_valid(me->parent) since it is indirectly asserted
    by node_valid(me) in the first iteration of the while-loop. */
    ECU_RUNTIME_ASSERT( (node_valid(me)), TREE_ASSERT_FUNCTOR );

    while (me->parent != me)
    {
        ++level;
        me = me->parent;
        ECU_RUNTIME_ASSERT( (node_valid(me)), TREE_ASSERT_FUNCTOR );
    }

    return level;
}


// return null if nodes in different trees.
struct ecu_tree_node *ecu_tree_get_lca(struct ecu_tree_node *node1, 
                                       struct ecu_tree_node *node2)
{
    size_t level = 0;
    size_t level_node1 = 0;
    size_t level_node2 = 0;
    struct ecu_tree_node *lca = (struct ecu_tree_node *)0;
    ECU_RUNTIME_ASSERT( ((node_valid(node1)) && (node_valid(node2))), TREE_ASSERT_FUNCTOR );

    if (node1 == node2)
    {
        lca = node1;
    }
    else
    {
        /* LCA algorithm:
        1. Find whichever node is at a deeper level.

        2. Only for the deeper node, traverse up the tree (get parent) 
        until you are at the same level as the other node. If nodes
        are equal LCA is found. Otherwise proceed to Step #3.
        
        3. Traverse up the tree (get parent) for both nodes. If both
        nodes are equal LCA is found. If you reached the root of the tree 
        but both nodes are still not equal then the nodes are in separate 
        trees. Return null in this case. Otherwise keep repeating Step #3. */
        level_node1 = ecu_tree_get_level(node1);
        level_node2 = ecu_tree_get_level(node2);

        /* Steps 1 and 2 of algorithm. */
        if (level_node1 > level_node2)
        {
            while (level_node1 != level_node2)
            {
                node1 = node1->parent;
                --level_node1;
                ECU_RUNTIME_ASSERT( (node_valid(node1)), TREE_ASSERT_FUNCTOR );
            }
        }
        else if (level_node2 > level_node1)
        {
            while (level_node1 != level_node2)
            {
                node2 = node2->parent;
                --level_node2;
                ECU_RUNTIME_ASSERT( (node_valid(node2)), TREE_ASSERT_FUNCTOR );
            }
        }
        level = level_node1; /* level_node1 == level_node2 at this point. Use variable 'level' for easier reading. */

        /* Step 3 of algorithm. */
        while ((node1 != node2) && (level != 0))
        {
            node1 = node1->parent;
            node2 = node2->parent;
            --level;
            ECU_RUNTIME_ASSERT( ((node_valid(node1)) && (node_valid(node2))), TREE_ASSERT_FUNCTOR );
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
    /* WARNING: This function requires that ecu_tree_get_lca(node1, node2) returns 
    NULL if node1 and node2 are in different trees. This function must be refactored 
    if this ever changes. */
    bool in_same_tree = false;
    ECU_RUNTIME_ASSERT( ((node_valid(node1)) && (node_valid(node2))), TREE_ASSERT_FUNCTOR );

    if (ecu_tree_get_lca(node1, node2))
    {
        in_same_tree = true;
    }

    return in_same_tree;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- PUBLIC FUNCTIONS: CHILDREN ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_node *ecu_tree_child_iterator_begin(struct ecu_tree_child_iterator *me,
                                                    struct ecu_tree_node *parent)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(parent)), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(parent->child)), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(parent->child->next)), TREE_ASSERT_FUNCTOR );

    me->head    = parent;
    me->current = parent->child;

    if ((parent->child == parent) || (parent->child->next == parent->child))
    {
        /* parent has 0 or 1 child. Next node is the parent (delimiter). */
        me->next = parent;
    }
    else
    {
        /* parent has more than 1 child. Next node is the next child in the linked list. */
        me->next = parent->child->next;
    }

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
    /* Don't assert me->current since it is just set to me->next. */
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node_valid(me->head)) && (node_valid(me->next))), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node_valid(me->next->next)) && (node_valid(me->head->child))), TREE_ASSERT_FUNCTOR );

    /* Current is what was stored in next from previous call to this function. Update our next value. */
    me->current = me->next;
    
    if (me->current->next == me->head->child)
    {
        /* One node before end of iteration. Next node is the parent (delimiter). */
        me->next = me->head;
    }
    else if (me->current == me->head)
    {
        /* Reached end of iteration. Restarting the iteration so next node is parent's first child. */
        me->next = me->head->child;
    }
    else
    {
        /* Still in middle of child list. Return next child in linked list. */
        me->next = me->next->next;
    }

    return (me->current);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC FUNCTIONS: POSTORDER ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_node *ecu_tree_postorder_iterator_begin(struct ecu_tree_postorder_iterator *me,
                                                        struct ecu_tree_node *root)
{
    ECU_RUNTIME_ASSERT( (me && root), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(root)), TREE_ASSERT_FUNCTOR );

    /* Not using a static delimiter node in this constructor for now in case
    user messes up delimiter node. If this happens it would be contained in 
    their single iterator object. Messing up a static delimiter node would
    effect ALL iterator objects since it would be shared. Cannot fix this by
    making delimiter const since ecu_tree_postorder_iterator_end() and 
    ecu_tree_postorder_iterator_next() return pointers to non-const. */
    ecu_tree_node_ctor(&me->delimiter, 
                       (void (*)(struct ecu_tree_node *))0, 
                       ECU_OBJECT_ID_UNUSED);

    me->root    = root;
    me->current = get_child_leaf(root);
    ECU_RUNTIME_ASSERT( (node_valid(me->current)), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(me->current->next)), TREE_ASSERT_FUNCTOR );

    if (me->current == me->root)
    {
        /* Root has no nodes so next node is delimiter. Cannot combine everything into one 
        if (me->current->next == me->current) statement since root can be a SUBTREE. me->current 
        would be root so me->current->next would be root's sibling which is incorrect. */
        me->next = &me->delimiter;
    }
    else
    {
        if (me->current->next == me->current)
        {
            /* First leaf (current node) has no siblings. Next node is parent of first leaf. 
            Don't assert me->current->parent is valid since me->next is asserted at end. 
            me->current->next was asserted beforehand since it is used in if statement.
            NOTE: we can do if (me->current->next == me->current) instead of 
            if (me->current->next == me->current->parent->child) since me->current will 
            ALWAYS be the first child since we are just starting the iteration. I.e. me->current
            will always be me->current->parent->child. This is NOT the case for 
            ecu_tree_postorder_iterator_next() which is why that uses me->current->parent->child. */
            me->next = me->current->parent;
        }
        else
        {
            /* First leaf (current node) has siblings. Next node is next sibling's leaf. */
            me->next = get_child_leaf(me->current->next);
        }
        ECU_RUNTIME_ASSERT( (node_valid(me->next)), TREE_ASSERT_FUNCTOR );
    }

    return (me->current);
}


struct ecu_tree_node *ecu_tree_postorder_iterator_end(struct ecu_tree_postorder_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    return (&me->delimiter);
}


struct ecu_tree_node *ecu_tree_postorder_iterator_next(struct ecu_tree_postorder_iterator *me)
{
    /* Don't assert me->current since it is just set to me->next */
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node_valid(me->root)) && (node_valid(me->next))), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(me->next->next)), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(me->next->parent)), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(me->next->parent->child)), TREE_ASSERT_FUNCTOR );

    /* Current is what was stored in next from previous call to this function. Update our next value. */       
    me->current = me->next;

    if (me->current == me->root)
    {
        /* One node before end of iteration. Next node is dummy delimiter. */
        me->next = &me->delimiter;
    }
    else if (me->current == &me->delimiter)
    {
        /* Reached end of iteration. Restarting the iteration so next node is root's first leaf. */
        me->next = get_child_leaf(me->root);
    }
    else
    {
        if (me->current->next == me->current->parent->child)
        {
            /* Reached end of sibling list. Next node is parent. */
            me->next = me->current->parent;
        }
        else
        {
            /* In the middle of the sibling list. Next node is next sibling's leaf. */
            me->next = get_child_leaf(me->current->next);
        }
        ECU_RUNTIME_ASSERT( (node_valid(me->next)), TREE_ASSERT_FUNCTOR );
    }

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
