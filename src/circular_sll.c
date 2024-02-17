/**
 * @file
 * @brief Circular singly linked list implementation without dynamic memory allocation.
 * All class members are meant to be private but they are exposed to the Application so 
 * memory can easily be allocated at compile-time. I.e. don't have to use memory pool.
 * @author Ian Ress
 * 
 */


/* Translation unit. */
#include <ecu/circular_sll.h>

/* STDLib. */
#include <stdbool.h>

/* Runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ HELPER DECLARATIONS ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

enum link_status
{
    LINK_NULL,  /* next points to null pointer. */
    LINK_SELF,  /* next points to self. */
    IN_LIST     /* next points to another node. */
};


/**
 * @brief Returns if the node's next pointer points to nothing, points to itself,
 * or if the node is a list.
 * 
 * @param node Node to test.
 */
static inline enum link_status get_link_status(const struct circular_sll_node *node);


/**
 * @brief Gets the previous node. Since this a singly linked list we must iterate
 * through the entire list to get the previous node.
 * 
 * @param node Function returns node previous to this one.
 */
static struct circular_sll_node *circular_sll_get_prev_node(struct circular_sll_node *node);


/**
 * @brief Returns true if list was constructed. False otherwise.
 */
static inline bool is_constructed(const struct circular_sll *list);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- HELPER DEFINITIONS ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static inline enum link_status get_link_status(const struct circular_sll_node *node)
{
    enum link_status status;
    RUNTIME_ASSERT( (node) );

    if (!node->next)
    {
        status = LINK_NULL;
    }
    else if (node->next == node)
    {
        status = LINK_SELF;
    }
    else
    {
        status = IN_LIST;
    }

    return status;
}


static struct circular_sll_node *circular_sll_get_prev_node(struct circular_sll_node *node)
{
    RUNTIME_ASSERT( (node) );
    RUNTIME_ASSERT( (get_link_status(node) != LINK_NULL) );

    struct circular_sll_node *current_node = node;

    /* Find previous node. Have to loop through entire list since we only have next pointers. */
    while (current_node->next != node)
    {
        current_node = current_node->next;
        RUNTIME_ASSERT( (current_node->next) );
    }

    /* current_node is now the previous node. */
    return current_node;
}


static inline bool is_constructed(const struct circular_sll *list)
{
    RUNTIME_ASSERT( (list) );
    return (list->terminal_node.next);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC METHODS: LIST ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void circular_sll_ctor(struct circular_sll *list)
{
    /* Prevent list from being reconstructed. User must explicitly call destroy. */
    RUNTIME_ASSERT( (list) );
    RUNTIME_ASSERT( (get_link_status(&list->terminal_node) != IN_LIST) );

    list->terminal_node.next = &list->terminal_node;
}


void circular_sll_destroy(struct circular_sll *list)
{
    /* List must have been previously constructed. */
    RUNTIME_ASSERT( (list) );
    RUNTIME_ASSERT( (is_constructed(list)) );

    struct circular_sll_iterator iterator;

    for (struct circular_sll_node *node = circular_sll_iterator_begin(list, &iterator);
         node != circular_sll_iterator_end(&iterator); 
         node = circular_sll_iterator_next(&iterator))
    {
        circular_sll_remove_node(node);
    }
    list->terminal_node.next = (struct circular_sll_node *)0;
}


void circular_sll_push_back(struct circular_sll *list, struct circular_sll_node *node)
{
    RUNTIME_ASSERT( (list && node) );
    RUNTIME_ASSERT( (is_constructed(list)) );
    RUNTIME_ASSERT( (get_link_status(node) != IN_LIST) );

    struct circular_sll_node *tail = circular_sll_get_prev_node(&list->terminal_node);
    RUNTIME_ASSERT( (tail) );

    /* struct circular_sll *list is not declared as pointer to const in function prototype since
    it is possible to edit list->terminal_node here if this is the first node added to the list.
    In other words:
    tail = &list->terminal_node
    We are editing list->terminal_node now. */
    tail->next = node;
    node->next = &list->terminal_node;
}


void circular_sll_add_node(struct circular_sll_node *position, struct circular_sll_node *node)
{
    RUNTIME_ASSERT( (position && node) );
    RUNTIME_ASSERT( (get_link_status(position) == IN_LIST && get_link_status(node) != IN_LIST) );

    node->next = position->next;
    position->next = node;
}


void circular_sll_remove_node(struct circular_sll_node *node)
{
    RUNTIME_ASSERT( (node) );
    RUNTIME_ASSERT( (get_link_status(node) == IN_LIST) );

    struct circular_sll_node *prev_node = circular_sll_get_prev_node(node);
    prev_node->next = node->next;
    node->next = node;
}


uint32_t circular_sll_get_size(struct circular_sll *list)
{
    RUNTIME_ASSERT( (list) );
    RUNTIME_ASSERT( (is_constructed(list)) );

    uint32_t i = 0;
    struct circular_sll_iterator iterator;

    for (struct circular_sll_node *node = circular_sll_iterator_begin(list, &iterator);
         node != circular_sll_iterator_end(&iterator); 
         node = circular_sll_iterator_next(&iterator))
    {
        i++;
    }
    return i;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- PUBLIC METHODS: ITERATORS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct circular_sll_node *circular_sll_iterator_begin(struct circular_sll *list,
                                                      struct circular_sll_iterator *iterator)
{
    RUNTIME_ASSERT( (list && iterator) );
    RUNTIME_ASSERT( (is_constructed(list)) );

    iterator->list = list;
    iterator->current = list->terminal_node.next;
    RUNTIME_ASSERT( (iterator->current) );
    return (iterator->current);
}


struct circular_sll_node *circular_sll_iterator_end(struct circular_sll_iterator *iterator)
{
    RUNTIME_ASSERT( (iterator) );
    RUNTIME_ASSERT( (iterator->list && iterator->current) );
    RUNTIME_ASSERT( (is_constructed(iterator->list)) );

    return (&iterator->list->terminal_node);
}


struct circular_sll_node *circular_sll_iterator_next(struct circular_sll_iterator *iterator)
{
    RUNTIME_ASSERT( (iterator) );
    RUNTIME_ASSERT( (iterator->list && iterator->current) );
    RUNTIME_ASSERT( (is_constructed(iterator->list)) );

    iterator->current = iterator->current->next;
    RUNTIME_ASSERT( (iterator->current) );

    return (iterator->current);
}
