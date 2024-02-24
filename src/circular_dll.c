/**
 * @file
 * @brief See @ref circular_dll.h
 * @author Ian Ress
 * 
 */


/* Translation unit. */
#include <ecu/circular_dll.h>

/* STDLib. */
#include <stdbool.h>

/* Runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ STATIC FUNCTION DECLARATIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Returns true if the node is in the list. False otherwise.
 * 
 * @param list List to test.
 * @param node Node to test.
 */
static bool node_in_list(struct circular_dll *list, struct circular_dll_node *node);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool node_in_list(struct circular_dll *list, struct circular_dll_node *node)
{
    bool node_in_list = false;
    struct circular_dll_iterator iterator;

    for (struct circular_dll_node *current_node = circular_dll_iterator_begin(list, &iterator);
         current_node != circular_dll_iterator_end(&iterator); 
         current_node = circular_dll_iterator_next(&iterator))
    {
        if (current_node == node)
        {
            node_in_list = true;
            break;
        }
    }

    return node_in_list;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC METHODS: LIST ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void circular_dll_ctor(struct circular_dll *list)
{
    RUNTIME_ASSERT( (list) );
    list->terminal_node.next = &list->terminal_node;
    list->terminal_node.prev = &list->terminal_node;
}


void circular_dll_destroy(struct circular_dll *list)
{
    RUNTIME_ASSERT( (list) );
    RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );

    struct circular_dll_iterator iterator;

    for (struct circular_dll_node *node = circular_dll_iterator_begin(list, &iterator);
         node != circular_dll_iterator_end(&iterator); 
         node = circular_dll_iterator_next(&iterator))
    {
        circular_dll_remove_node(list, node);
    }

    list->terminal_node.next = &list->terminal_node;
    list->terminal_node.prev = &list->terminal_node;
}


void circular_dll_push_back(struct circular_dll *list, struct circular_dll_node *node)
{
    RUNTIME_ASSERT( (list && node) );
    RUNTIME_ASSERT( (list->terminal_node.prev) );
    RUNTIME_ASSERT( (!node_in_list(list, node)) );

    struct circular_dll_node *tail = list->terminal_node.prev;
    tail->next = node;
    node->prev = tail;
    node->next = &list->terminal_node;
    list->terminal_node.prev = node;
}


void circular_dll_remove_node(struct circular_dll *list, struct circular_dll_node *node)
{
    RUNTIME_ASSERT( (list && node) );
    RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );
    RUNTIME_ASSERT( (node->next && node->prev) );
    /* Empty list? */
    RUNTIME_ASSERT( ((list->terminal_node.next != &list->terminal_node) && \
                     (list->terminal_node.prev != &list->terminal_node)) );
    RUNTIME_ASSERT( (node_in_list(list, node)) );

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}


uint32_t circular_dll_get_size(struct circular_dll *list)
{
    RUNTIME_ASSERT( (list) );
    RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );

    uint32_t i = 0;
    struct circular_dll_iterator iterator;

    for (struct circular_dll_node *node = circular_dll_iterator_begin(list, &iterator);
         node != circular_dll_iterator_end(&iterator); 
         node = circular_dll_iterator_next(&iterator))
    {
        /* NULL assertions are done within the iterator functions. */
        i++;
    }
    return i;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- PUBLIC METHODS: ITERATORS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct circular_dll_node *circular_dll_iterator_begin(struct circular_dll *list,
                                                      struct circular_dll_iterator *iterator)
{
    RUNTIME_ASSERT( (list && iterator) );
    RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );

    iterator->list = list;
    iterator->current = list->terminal_node.next;
    return (iterator->current);
}


struct circular_dll_node *circular_dll_iterator_end(struct circular_dll_iterator *iterator)
{
    RUNTIME_ASSERT( (iterator) );
    RUNTIME_ASSERT( (iterator->list && iterator->current) );
    return (&iterator->list->terminal_node);
}


struct circular_dll_node *circular_dll_iterator_next(struct circular_dll_iterator *iterator)
{
    RUNTIME_ASSERT( (iterator) );
    RUNTIME_ASSERT( (iterator->list && iterator->current) );

    iterator->current = iterator->current->next;
    RUNTIME_ASSERT( (iterator->current) );
    return (iterator->current);
}
