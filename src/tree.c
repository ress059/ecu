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
/*--------------------------------------------------------- STATIC ASSERTS --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/* Compilation error if ecu_tree_max_level_t is a signed type. Must be unsigned. */
ECU_STATIC_ASSERT( (((ecu_tree_max_level_t)(-1)) > ((ecu_tree_max_level_t)0)) );



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- FILE-SCOPE VARIABLES ----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *TREE_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DECLARATIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_tree_node *get_root(struct ecu_tree_node *node); // pointer to nonconst since function can potentially return the parameter.
static struct ecu_tree_node *get_child_leaf(struct ecu_tree_node *node); // pointer to nonconst since function can potentially return the parameter.
static bool node_is_root(const struct ecu_tree_node *node);
static bool node_in_tree(struct ecu_tree_node *node); // pointer to nonconst since this called get_root() which takes in pointer to nonconst.
static bool node_in_valid_sibling_list(const struct ecu_tree_node *node);
static bool node_child_head_valid(const struct ecu_tree_node *node);
static bool node_valid(const struct ecu_tree_node *node);
static void tree_root_destroy_callback(struct ecu_tree_node *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// parameter is not pointer to const since function can return the node itself.
static struct ecu_tree_node *get_root(struct ecu_tree_node *node)
{
    struct ecu_tree_node *root = node;
    ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node->parent), TREE_ASSERT_FUNCTOR );

    while (root->parent != root)
    {
        root = root->parent;
        ECU_RUNTIME_ASSERT( (root), TREE_ASSERT_FUNCTOR );
    }

    return root;
}


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


static bool node_is_root(const struct ecu_tree_node *node)
{
    bool is_root = false;
    ECU_RUNTIME_ASSERT( (node), TREE_ASSERT_FUNCTOR );

    if ((node->parent == node) && \
        (node->id == ECU_OBJECT_ID_RESERVED) && \
        (node->destroy == &tree_root_destroy_callback))
    {
        is_root = true;
    }

    return is_root;
}


// precondition is node is constructed
// Find root of tree. If it is an actual root then node is apart of tree
// parameter is pointer to non-const since get_root() function takes in
// pointer to non-const.
static bool node_in_tree(struct ecu_tree_node *node)
{
    return node_is_root(get_root(node));
}


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

    if (node->child == node || node->child->parent == node)
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


static void tree_root_destroy_callback(struct ecu_tree_node *me)
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
    me->root.id = ECU_OBJECT_ID_RESERVED;
}


void ecu_tree_node_ctor(struct ecu_tree_node *me, 
                        void (*destroy_0)(struct ecu_tree_node *me),
                        ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( ((me) && (id_0 >= ECU_VALID_OBJECT_ID_BEGIN)), TREE_ASSERT_FUNCTOR );

    me->child           = me;
    me->parent          = me;
    me->next            = me;
    me->prev            = me;
    me->destroy         = destroy_0;
    me->id              = id_0;
}


void ecu_tree_destroy(struct ecu_tree *me)
{
    (void)me;
    // todo. loop through all nodes and call custom destructor for each node.
    // have to handle destructur differently since we iterate through a tree differently. cannot rely on dll iterator.
}


// node with siblings can be added safely i think.
void ecu_tree_push_back(struct ecu_tree_node *me, struct ecu_tree_node *parent)
{
    struct ecu_tree_node *old_tail = (struct ecu_tree_node *)0;

    /* NULL assertions already done in these functions. */
    ECU_RUNTIME_ASSERT( ((!node_in_tree(me)) && (node_in_tree(parent)) && \
                         (node_valid(parent))), TREE_ASSERT_FUNCTOR );
    
    if (parent->child == parent)
    {
        parent->child = me;
        me->next = me;
        me->prev = me;
    }
    else
    {
        old_tail = parent->child->prev;
        old_tail->next->prev = me;      /* parent->child->prev = me */
        me->next = old_tail->next;      /* me->next = parent->child */
        me->prev = old_tail;
        old_tail->next = me;
    }

    me->parent = parent;
}


// dont detach node from its children.
void ecu_tree_remove_node(struct ecu_tree_node *me)
{
    /* NULL assertions already done in these functions. */
    ECU_RUNTIME_ASSERT( (!node_is_root(me)) && (node_in_tree(me)) && \
                        (node_valid(me)), TREE_ASSERT_FUNCTOR );

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
ecu_tree_max_level_t ecu_tree_node_get_level(const struct ecu_tree_node *me)
{
    ecu_tree_max_level_t level = 0;
    const struct ecu_tree_node *root = me;

    /* Do not assert if node is in tree since the same algorithm is done here. */
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
struct ecu_tree_node *ecu_tree_find_lca(struct ecu_tree_node *node1, 
                                        struct ecu_tree_node *node2)
{
    ECU_RUNTIME_ASSERT( (node1 && node2), TREE_ASSERT_FUNCTOR );

    ecu_tree_max_level_t level1 = 0;
    ecu_tree_max_level_t level2 = 0;
    struct ecu_tree_node *lca = (struct ecu_tree_node *)0;

    if (node1 == node2)
    {
        lca = node1;
    }
    else
    {
        /* We do not have to NULL assert when getting node->parent because
        ecu_tree_node_get_level() already traverses and NULL asserts parent 
        structure. 
        
        LCA algorithm:
        1. Find whichever node is at a deeper level.

        2. Only for the deeper node, traverse up the tree (parent) 
        until you are at the same level as the other node.
        
        3. Traverse up the tree (parent) for both nodes. If both
        nodes are equal you found the LCA. If you reached the root
        of the tree but both nodes are still not equal then the
        nodes are in separate trees. Return null in this case. */
        level1 = ecu_tree_node_get_level(node1);
        level2 = ecu_tree_node_get_level(node2);

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



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- PUBLIC FUNCTIONS: CHILDREN ITERATOR -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_tree_node *ecu_tree_children_iterator_begin(struct ecu_tree_children_iterator *me,
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


struct ecu_tree_node *ecu_tree_children_iterator_end(struct ecu_tree_children_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), TREE_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_valid(me->head)), TREE_ASSERT_FUNCTOR );
    return (me->head);
}


struct ecu_tree_node *ecu_tree_children_iterator_next(struct ecu_tree_children_iterator *me)
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
