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



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *assert_functor = (struct ecu_assert_functor *)0;



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
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_ctor(struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (list), assert_functor );
    list->terminal_node.next = &list->terminal_node;
    list->terminal_node.prev = &list->terminal_node;
}


void ecu_circular_dll_destroy(struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (list), assert_functor );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev),
                        assert_functor );

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
    ECU_RUNTIME_ASSERT( (list && node), assert_functor );
    ECU_RUNTIME_ASSERT( (!node_in_list(list, node)), assert_functor ); /* Function called in macro so there is no overhead if asserts are disabled. */

    struct ecu_circular_dll_node *tail = list->terminal_node.prev;
    ECU_RUNTIME_ASSERT( (tail), assert_functor );

    tail->next = node;
    node->prev = tail;
    node->next = &list->terminal_node;
    list->terminal_node.prev = node;
}


void ecu_circular_dll_remove_node(struct ecu_circular_dll *list, 
                                  struct ecu_circular_dll_node *node)
{
    ECU_RUNTIME_ASSERT( (list && node), assert_functor );

    /* Empty list? */
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev), assert_functor );
    ECU_RUNTIME_ASSERT( ((list->terminal_node.next != &list->terminal_node) && \
                         (list->terminal_node.prev != &list->terminal_node)), assert_functor );

    ECU_RUNTIME_ASSERT( (node->next && node->prev), assert_functor );
    ECU_RUNTIME_ASSERT( (node_in_list(list, node)), assert_functor ); /* Function called in macro so there is no overhead if asserts are disabled. */

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}


uint32_t ecu_circular_dll_get_size(struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (list), assert_functor );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev), assert_functor );

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
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_circular_dll_node *ecu_circular_dll_iterator_begin(struct ecu_circular_dll *list,
                                                              struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (list && iterator), assert_functor );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev), assert_functor );

    iterator->list = list;
    iterator->current = list->terminal_node.next;
    return (iterator->current);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (iterator), assert_functor );
    ECU_RUNTIME_ASSERT( (iterator->list && iterator->current), assert_functor );
    return (&iterator->list->terminal_node);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (iterator), assert_functor );
    ECU_RUNTIME_ASSERT( (iterator->list && iterator->current), assert_functor );

    iterator->current = iterator->current->next;
    ECU_RUNTIME_ASSERT( (iterator->current), assert_functor );
    return (iterator->current);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: OTHER --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    assert_functor = functor;
}
