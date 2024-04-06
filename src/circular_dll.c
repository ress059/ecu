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

static struct ecu_assert_functor *DLL_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ STATIC FUNCTION DECLARATIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Returns true if the node is in the list. False otherwise.
 * The algorithm looks if the node's previous and next nodes point
 * to it, meaning its apart of a list. Otherwise these will point
 * to other, unrelated memory.
 * 
 * @param node Node to test. Node must have been constructed via
 * @ref ecu_circular_dll_node_ctor() or 
 * @ref ECU_CIRCULAR_DLL_NODE_CTOR_COMPILETIME() otherwise behavior
 * is undefined.
 */
static bool node_in_list(struct ecu_circular_dll_node *node);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool node_in_list(struct ecu_circular_dll_node *node)
{
    bool in_list = false;
    ECU_RUNTIME_ASSERT( (node), DLL_ASSERT_FUNCTOR );

    if (node->next && node->prev)
    {
        /* Requires node to have been constructed since we are dereferencing node-next and node->prev. */
        if (node->next != node && node->prev != node && \
            node->prev->next == node && node->next->prev == node)
        {
            in_list = true;
        }
    }

    return in_list;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_node_ctor(struct ecu_circular_dll_node *node)
{
    ECU_RUNTIME_ASSERT( (node), DLL_ASSERT_FUNCTOR );
    node->next = node;
    node->prev = node;
}


void ecu_circular_dll_ctor(struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (list), DLL_ASSERT_FUNCTOR );
    list->terminal_node.next = &list->terminal_node;
    list->terminal_node.prev = &list->terminal_node;
}


void ecu_circular_dll_destroy(struct ecu_circular_dll *list)
{
    struct ecu_circular_dll_iterator iterator;

    ECU_RUNTIME_ASSERT( (list), DLL_ASSERT_FUNCTOR );

    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(list, &iterator);
         node != ecu_circular_dll_iterator_end(&iterator); 
         node = ecu_circular_dll_iterator_next(&iterator))
    {
        ecu_circular_dll_remove_node(node);
    }

    list->terminal_node.next = &list->terminal_node;
    list->terminal_node.prev = &list->terminal_node;
}


void ecu_circular_dll_push_back(struct ecu_circular_dll *list, 
                                struct ecu_circular_dll_node *node)
{
    ECU_RUNTIME_ASSERT( (list && node), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

    struct ecu_circular_dll_node *tail = list->terminal_node.prev;
    ECU_RUNTIME_ASSERT( (tail), DLL_ASSERT_FUNCTOR );

    tail->next->prev = node;
    node->next = tail->next;
    node->prev = tail;
    tail->next = node;
}


void ecu_circular_dll_remove_node(struct ecu_circular_dll_node *node)
{
    ECU_RUNTIME_ASSERT( (node), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_in_list(node)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next = node;
    node->prev = node;
}


uint32_t ecu_circular_dll_get_size(struct ecu_circular_dll *list)
{
    uint32_t i = 0;
    struct ecu_circular_dll_iterator iterator;

    ECU_RUNTIME_ASSERT( (list), DLL_ASSERT_FUNCTOR );

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
    ECU_RUNTIME_ASSERT( (list && iterator), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (list->terminal_node.next && list->terminal_node.prev), DLL_ASSERT_FUNCTOR );

    iterator->list = list;
    iterator->current = list->terminal_node.next;
    return (iterator->current);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (iterator), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (iterator->list && iterator->current), DLL_ASSERT_FUNCTOR );
    return (&iterator->list->terminal_node);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *iterator)
{
    ECU_RUNTIME_ASSERT( (iterator), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (iterator->list && iterator->current), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (iterator->current->next), DLL_ASSERT_FUNCTOR );

    iterator->current = iterator->current->next;
    return (iterator->current);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: OTHER --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    DLL_ASSERT_FUNCTOR = functor;
}
