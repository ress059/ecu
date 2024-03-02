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
static bool node_in_list(struct ecu_circular_dll *list, struct ecu_circular_dll_node *node);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool node_in_list(struct ecu_circular_dll *list, struct ecu_circular_dll_node *node)
{
    bool node_in_list = false;
    struct ecu_circular_dll_iterator iterator;

    for (struct ecu_circular_dll_node *current_node = ecu_circular_dll_iterator_begin(list, &iterator);
         current_node != ecu_circular_dll_iterator_end(&iterator); 
         current_node = ecu_circular_dll_iterator_next(&iterator))
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

void ecu_circular_dll_ctor(struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (list) );
    list->terminal_node.next = &list->terminal_node;
    list->terminal_node.prev = &list->terminal_node;
}


void ecu_circular_dll_destroy(struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (list) );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );

    struct ecu_circular_dll_iterator iterator;

    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(list, &iterator);
         node != ecu_circular_dll_iterator_end(&iterator); 
         node = ecu_circular_dll_iterator_next(&iterator))
    {
        ecu_circular_dll_remove_node(list, node);
    }

    list->terminal_node.next = &list->terminal_node;
    list->terminal_node.prev = &list->terminal_node;
}


void ecu_circular_dll_push_back(struct ecu_circular_dll *list, 
                                struct ecu_circular_dll_node *node)
{
    ECU_RUNTIME_ASSERT( (list && node) );
    ECU_RUNTIME_ASSERT( (list->terminal_node.prev) );
    ECU_RUNTIME_ASSERT( (!node_in_list(list, node)) );

    struct ecu_circular_dll_node *tail = list->terminal_node.prev;
    tail->next = node;
    node->prev = tail;
    node->next = &list->terminal_node;
    list->terminal_node.prev = node;
}


void ecu_circular_dll_remove_node(struct ecu_circular_dll *list, 
                                  struct ecu_circular_dll_node *node)
{
    ECU_RUNTIME_ASSERT( (list && node) );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );
    ECU_RUNTIME_ASSERT( (node->next && node->prev) );
    /* Empty list? */
    ECU_RUNTIME_ASSERT( ((list->terminal_node.next != &list->terminal_node) && \
                         (list->terminal_node.prev != &list->terminal_node)) );
    ECU_RUNTIME_ASSERT( (node_in_list(list, node)) );

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}


uint32_t ecu_circular_dll_get_size(struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (list) );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );

    uint32_t i = 0;
    struct ecu_circular_dll_iterator iterator;

    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(list, &iterator);
         node != ecu_circular_dll_iterator_end(&iterator); 
         node = ecu_circular_dll_iterator_next(&iterator))
    {
        /* NULL assertions are done within the iterator functions. */
        i++;
    }
    return i;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- PUBLIC METHODS: ITERATORS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_circular_dll_node *ecu_circular_dll_iterator_begin(struct ecu_circular_dll *list,
                                                              struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (list && iterator) );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev) );

    iterator->list = list;
    iterator->current = list->terminal_node.next;
    return (iterator->current);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (iterator) );
    ECU_RUNTIME_ASSERT( (iterator->list && iterator->current) );
    return (&iterator->list->terminal_node);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (iterator) );
    ECU_RUNTIME_ASSERT( (iterator->list && iterator->current) );

    iterator->current = iterator->current->next;
    ECU_RUNTIME_ASSERT( (iterator->current) );
    return (iterator->current);
}
