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
/*-------------------------------------------------- STATIC FUNCTION DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @pre @p list previously constructed via call to @ref ecu_circular_dll_ctor().
 * @brief Returns true if the list is valid. False otherwise. A valid list
 * means @ref ecu_circular_dll.head.next and @ref ecu_circular_dll.head.prev
 * point to @ref ecu_circular_dll_.head 
 */
static bool list_valid(const struct ecu_circular_dll *list);


/**
 * @pre @p node previously constructed via call to @ref ecu_circular_dll_node_ctor().
 * @brief Returns true if node is in a list. False otherwise.
 */
static bool node_in_list(const struct ecu_circular_dll_node *node);


/**
 * @brief Callback assigned to @ref ecu_circular_dll.head.destroy in 
 * @ref ecu_circular_dll_ctor(). This head node is solely used 
 * as a dummy delimeter that should never be destroyed. Therefore we
 * simply assert if this callback runs.
 */
static void list_head_destroy_callback(struct ecu_circular_dll_node *me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool node_in_list(const struct ecu_circular_dll_node *node)
{
    bool in_list = false;

    ECU_RUNTIME_ASSERT( (node), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((node->next) && (node->prev)), DLL_ASSERT_FUNCTOR );

    /* Requires node to have been constructed since we are accesing node->next->prev
    and node->prev->next. Otherwise garbage, non-NULL values of node->next and node->prev 
    can cause seg faults. */
    if ((node->next != node) && (node->prev != node) && \
        (node->next->prev == node) && (node->prev->next == node))
    {
        in_list = true;
    }

    return in_list;
}


static bool list_valid(const struct ecu_circular_dll *list)
{
    bool valid = false;

    ECU_RUNTIME_ASSERT( (list), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( ((list->head.next) && (list->head.prev)),
                        DLL_ASSERT_FUNCTOR );

    /* Note how this also handles the case where head node's next and prev pointers point 
    to itself. Requires list to have been constructed since we are accessing list->head.next->prev
    and list->head.prev->next. Otherwise garbage, non-NULL values of list->head.next and 
    list->head.prev can cause seg faults. */
    if ((list->head.next->prev == &list->head) && \
        (list->head.prev->next == &list->head))
    {
        valid = true;
    }

    return valid;
}


static void list_head_destroy_callback(struct ecu_circular_dll_node *me)
{
    (void)me;
    ECU_RUNTIME_ASSERT( (false), DLL_ASSERT_FUNCTOR );
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS: LIST ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_circular_dll_node_ctor(struct ecu_circular_dll_node *me,
                                void (*destroy_0)(struct ecu_circular_dll_node *me),
                                ecu_object_id id_0)
{
    ECU_RUNTIME_ASSERT( ((me) && (id_0 >= ECU_VALID_OBJECT_ID_BEGIN)), DLL_ASSERT_FUNCTOR );
    /* Do NOT do ECU_RUNTIME_ASSERT( (!node_in_list(me)), DLL_ASSERT_FUNCTOR ); 
    since it is valid for next and prev pointers to be initialized to non-NULL 
    garbage values before a constructor call. Otherwise me->next->prev and 
    me->prev->next accesses can lead to seg faults. It is the user's 
    responsibility to not construct an active node, which is clearly outlined
    in the warning directive of this function description. */

    me->next    = me;
    me->prev    = me;
    me->destroy = destroy_0; /* Optional callback so do not NULL assert. */
    me->id      = id_0;      /* Optional. */
}


void ecu_circular_dll_ctor(struct ecu_circular_dll *me)
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    /* Do NOT assert if list has nodes in it since it is valid for next and prev 
    pointers to be initialized to non-NULL garbage values before a constructor 
    call. Otherwise me->head.next->.. and me->head.prev->.. accesses can lead to 
    seg faults. It is the user's responsibility to not construct an active list
    that has nodes in it, which is clearly outlined in the warning directive of 
    this function description. */

    me->head.next       = &me->head;
    me->head.prev       = &me->head;
    me->head.destroy    = &list_head_destroy_callback;
    me->head.id         = ECU_OBJECT_ID_RESERVED;
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
    struct ecu_circular_dll_node *old_tail = (struct ecu_circular_dll_node *)0;

    ECU_RUNTIME_ASSERT( (me && node), DLL_ASSERT_FUNCTOR );

    /* Functions called in macro so there is no overhead if asserts are disabled. */
    ECU_RUNTIME_ASSERT( (list_valid(me)), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (!node_in_list(node)), DLL_ASSERT_FUNCTOR );

    old_tail = me->head.prev;       /* Do not have to NULL assert because list_valid() check. */
    old_tail->next->prev = node;    /* me->head.prev = node; */
    node->next = old_tail->next;    /* node->next = me->head; */
    node->prev = old_tail;
    old_tail->next = node;
}


void ecu_circular_dll_remove_node(struct ecu_circular_dll_node *me)
{
    ECU_RUNTIME_ASSERT( (me), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (node_in_list(me)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

    me->next->prev = me->prev;
    me->prev->next = me->next;
    me->next = me;
    me->prev = me;
}


uint32_t ecu_circular_dll_get_size(struct ecu_circular_dll *me)
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
                                                              struct ecu_circular_dll *list)
{
    ECU_RUNTIME_ASSERT( (me && list), DLL_ASSERT_FUNCTOR );
    ECU_RUNTIME_ASSERT( (list_valid(list)), DLL_ASSERT_FUNCTOR ); /* Function called in macro so there is no overhead if asserts are disabled. */

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
