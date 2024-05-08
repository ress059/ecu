/**
 * @file
 * @brief See @ref ring_buffer.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-26
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- INCLUDES ------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include <ecu/ring_buffer.h>

/* STDLib. memset. */
#include <string.h>

/* Runtime asserts. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct ecu_assert_functor *RINGBUF_ASSERT_FUNCTOR = ECU_DEFAULT_FUNCTOR;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DECLARATIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool ring_buffer_valid(const struct ecu_ring_buffer *buf);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool ring_buffer_valid(const struct ecu_ring_buffer *buf)
{
    bool valid = false;
    ECU_RUNTIME_ASSERT( (buf), RINGBUF_ASSERT_FUNCTOR );

    if ((buf->buffer) && (buf->element_size > 0) && \
        (buf->max_number_of_elements > 0))
    {
        valid = true;
    }

    return valid;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PUBLIC FUNCTIONS --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_ring_buffer_ctor(struct ecu_ring_buffer *me, 
                          void *buffer_0, 
                          size_t element_size_0, 
                          size_t max_number_of_elements_0)
{
    #warning "TODO: Add condition (element_size_0 * max_number_of_elements_0) doesn't overflow"
    ECU_RUNTIME_ASSERT( ((me) && (buffer_0) && (element_size_0 > 0) && \
                         (max_number_of_elements_0 > 0)),
                         RINGBUF_ASSERT_FUNCTOR );

    me->buffer                  = buffer_0;
    me->head                    = 0;
    me->tail                    = 0;
    me->full                    = false;
    me->element_size            = element_size_0;
    me->max_number_of_elements  = max_number_of_elements_0;
}


void ecu_ring_buffer_clear(struct ecu_ring_buffer *me)
{
    /* NULL assertion done in function. */
    ECU_RUNTIME_ASSERT( (ring_buffer_valid(me)), RINGBUF_ASSERT_FUNCTOR );

    me->head        = 0;
    me->tail        = 0;
    me->full        = false;
}


bool ecu_ring_buffer_write(struct ecu_ring_buffer *me, 
                           const void *element, 
                           size_t size)
{
    bool success = false;
    uint8_t *destination = (uint8_t *)0;

    ECU_RUNTIME_ASSERT( (ring_buffer_valid(me)), RINGBUF_ASSERT_FUNCTOR ); /* NULL assertion done in function. */
    ECU_RUNTIME_ASSERT( ((element) && (me->element_size == size)), RINGBUF_ASSERT_FUNCTOR );

    if (!me->full)
    {
        destination = (uint8_t *)me->buffer + (me->head * size);
        memcpy((void *)destination, element, size);
        me->head = ++(me->head) % me->max_number_of_elements;

        if (me->head == me->tail)
        {
            me->full = true;
        }

        success = true;
    }

    return success;
}


bool ecu_ring_buffer_read(struct ecu_ring_buffer *me, 
                          void *element, 
                          size_t size)
{
    bool success = false;
    uint8_t *source = (uint8_t *)0;

    ECU_RUNTIME_ASSERT( (ring_buffer_valid(me)), RINGBUF_ASSERT_FUNCTOR ); /* NULL assertion done in function. */
    ECU_RUNTIME_ASSERT( ((element) && (me->element_size == size)), RINGBUF_ASSERT_FUNCTOR );

    if (!ecu_ring_buffer_is_empty(me))
    {
        source = (uint8_t *)me->buffer + (me->tail * size);
        memcpy(element, (const void *)source, size);
        me->tail = ++(me->tail) % me->max_number_of_elements;
        me->full = false; // todo how to avoid having to do this everytime?
        success = true;
    }

    return success;
}


size_t ecu_ring_buffer_get_count(const struct ecu_ring_buffer *me)
{
    size_t num_elements = 0;
    ECU_RUNTIME_ASSERT( (ring_buffer_valid(me)), RINGBUF_ASSERT_FUNCTOR );

    if (me->full)
    {
        num_elements = me->max_number_of_elements;
    }
    else if (me->tail > me->head)
    {
        num_elements = me->tail - me->head;
    }
    else
    {
        num_elements = me->head - me->tail;
    }

    return num_elements;
} 


size_t ecu_ring_buffer_get_max_size(const struct ecu_ring_buffer *me)
{
    ECU_RUNTIME_ASSERT( (ring_buffer_valid(me)), RINGBUF_ASSERT_FUNCTOR );
    return me->max_number_of_elements;
} 


bool ecu_ring_buffer_is_empty(const struct ecu_ring_buffer *me)
{
    bool empty = false;
    ECU_RUNTIME_ASSERT( (ring_buffer_valid(me)), RINGBUF_ASSERT_FUNCTOR );

    if ((me->head == me->tail) && (!me->full))
    {
        empty = true;
    }

    return empty;
}


bool ecu_ring_buffer_is_full(const struct ecu_ring_buffer *me)
{
    ECU_RUNTIME_ASSERT( (ring_buffer_valid(me)), RINGBUF_ASSERT_FUNCTOR );
    return me->full;
}


void ecu_ring_buffer_set_assert_functor(struct ecu_assert_functor *functor)
{
    /* Do not NULL check since setting to NULL means the default assert handler will now be called. */
    RINGBUF_ASSERT_FUNCTOR = functor;
}
