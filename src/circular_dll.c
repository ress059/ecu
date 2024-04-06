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

// TODO: Description
static bool list_valid(const struct ecu_circular_dll *list);


// /**
//  * @brief Returns true if the node is in the list. False otherwise.
//  * The algorithm looks if the node's previous and next nodes point
//  * to it, meaning its apart of a list. Otherwise these will point
//  * to other, unrelated memory.
//  * 
//  * @param node Node to test. Node must have been constructed via
//  * @ref ecu_circular_dll_node_ctor() or 
//  * @ref ECU_CIRCULAR_DLL_NODE_CTOR_COMPILETIME() otherwise behavior
//  * is undefined.
//  */
// static bool node_in_list(struct ecu_circular_dll_node *node);

static bool node_in_list(const struct ecu_circular_dll_node *node);


/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// static bool node_in_list(struct ecu_circular_dll_node *node)
// {
//     bool in_list = false;
//     ECU_RUNTIME_ASSERT( (node), DLL_ASSERT_FUNCTOR );

//     if (node->next && node->prev)
//     {
//         /* Requires node to have been constructed since we are dereferencing node-next and node->prev. */
//         if (node->next != node && node->prev != node && \
//             node->prev->next == node && node->next->prev == node)
//         {
//             in_list = true;
//         }
//     }

//     return in_list;
// }

// Precondition is that node has been constructed.
static bool node_in_list(const struct ecu_circular_dll_node *node)
{
    bool in_list = false;

    ECU_RUNTIME_ASSERT( (node), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node->next) && (node->prev)), DLL_ASSERT_FUNCTOR );

    if ((node->next != node) && (node->prev != node) && \
        (node->next->prev == node) && (node->prev->next == node))
    {
        in_list = true;
    }

    return in_list;
}

// Precondition is that list has been constructed.
static bool list_valid(const struct ecu_circular_dll *list)
{
    bool valid = false;

    ECU_RUNTIME_ASSERT( (list), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((list->head.next) && (list->head.prev)),
                        DLL_ASSERT_FUNCTOR );

    /* Note how this also handles the case where terminal node's next and
    prev pointers point to itself. */
    if ((list->head.next->prev == &list->head) && \
        (list->head.prev->next == &list->head))
    {
        valid = true;
    }

    return valid;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_node_ctor(struct ecu_circular_dll_node *me, uint8_t id_0,
                                void (*destroy_0)(struct ecu_circular_dll_node *me))
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    /* Do NOT do ECU_RUNTIME_ASSERT( (!node_in_list(me)), DLL_ASSERT_FUNCTOR ); 
    since it is valid for next and prev pointers to be initialized to non-NULL 
    garbage values before a constructor call. Otherwise me->next->prev and 
    me->prev->next accesses can lead to Seg faults. It is the user's 
    responsibility to not construct an active node, which is clearly outlined
    in the warning directive of this function description. */

    me->next    = me;
    me->prev    = me;
    me->id      = id_0;      /* Optional. */
    me->destroy = destroy_0; /* Optional callback so do not NULL assert. */
}


void ecu_circular_dll_ctor(struct ecu_circular_dll *me)
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    /* Do NOT do ECU_RUNTIME_ASSERT( (!list_valid(me)), DLL_ASSERT_FUNCTOR ); 
    since it is valid for next and prev pointers to be initialized to non-NULL 
    garbage values before a constructor call. Otherwise me->head.next->prev 
    and me->head.prev->next accesses can lead to Seg faults. It is the user's 
    responsibility to not construct an active list, which is clearly outlined
    in the warning directive of this function description. */

    me->head.next = &me->head;
    me->head.prev = &me->head;
}


void ecu_circular_dll_destroy(struct ecu_circular_dll *me)
{
    struct ecu_circular_dll_iterator iterator;

    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (list_valid(me)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&iterator, me);
         node != ecu_circular_dll_iterator_end(&iterator); 
         node = ecu_circular_dll_iterator_next(&iterator))
    {
        ecu_circular_dll_remove_node(node);

        /* Do not reset node->id so user is able to identify their data type 
        in their destructor callback. */
        if (node->destroy)
        {
            (*node->destroy)(node);
        }
    }

    me->head.next = &me->head;
    me->head.prev = &me->head;
}


void ecu_circular_dll_push_back(struct ecu_circular_dll *me, 
                                struct ecu_circular_dll_node *node)
{
    struct ecu_circular_dll_node *tail = (struct ecu_circular_dll_node *)0;

    ECU_RUNTIME_ASSERT( (me && node), DLL_ASSERT_FUNCTOR );

    /* Functions called in macro so there is no overhead if asserts are disabled. */
    ECU_RUNTIME_ASSERT( (list_valid(me)), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)), DLL_ASSERT_FUNCTOR );

    tail = me->head.prev;
    ECU_RUNTIME_ASSERT( (tail), DLL_ASSERT_FUNCTOR );

    #warning "TODO: Can optimize this? Using old dll branch approach for now."
    tail->next->prev = node;
    node->next = tail->next;
    node->prev = tail;
    tail->next = node;
}


void ecu_circular_dll_remove_node(struct ecu_circular_dll_node *me)
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_in_list(me)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

    #warning "this is the same approach as old dll branch in same order."
    me->next->prev = me->prev;
    me->prev->next = me->next;
    me->next = me;
    me->prev = me;
}


uint32_t ecu_circular_dll_get_size(const struct ecu_circular_dll *me)
{
    uint32_t i = 0;
    struct ecu_circular_dll_iterator iterator;

    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (list_valid(me)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

    for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&iterator, me);
         node != ecu_circular_dll_iterator_end(&iterator); 
         node = ecu_circular_dll_iterator_next(&iterator))
    {
        /* NULL assertions are done within the iterator functions. */
        i++;
    }
    return i;
}


// TODO
bool ecu_circular_dll_is_empty(const struct ecu_circular_dll *me)
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (list_valid(me)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */
    return (me->head.next == &me->head);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- PUBLIC FUNCTIONS: ITERATORS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct ecu_circular_dll_node *ecu_circular_dll_iterator_begin(struct ecu_circular_dll_iterator *me,
                                                              const struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (me && list), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (list_valid(me)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

    me->list = list;
    me->current = list->head.next;
    return (me->current);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_end(struct ecu_circular_dll_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((me->list) && (me->current)), DLL_ASSERT_FUNCTOR );
    return (&me->list->head);
}


struct ecu_circular_dll_node *ecu_circular_dll_iterator_next(struct ecu_circular_dll_iterator *me)
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((me->list) && (me->current)), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (me->current->next), DLL_ASSERT_FUNCTOR );

    me->current = me->current->next;
    return (me->current);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: OTHER --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    DLL_ASSERT_FUNCTOR = functor;
}
