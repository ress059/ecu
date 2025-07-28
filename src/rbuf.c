/**
 * @file
 * @brief Standard ring buffer. See
 * @rst
 * See :ref:`rbuf.h section <rbuf_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-26
 * @copyright Copyright (c) 2025
 */

/* Translation unit. */
#include "ecu/rbuf.h"

/* ECU. */
#include "ecu/asserter.h"

ECU_ASSERT_DEFINE_NAME("ecu/rbuf.c")

/**
 * @brief Size (number of bytes) of ring buffer must be at least 
 * this value. Buffer only makes sense if 2+ bytes of data
 * and need one extra byte to differentiate between full and empty.
 */
#define MIN_LEN ((size_t)3)






// todo need to ensure save acecss (isr consumer/app producer or isr producer/app consumer)

void ecu_rbuf_ctor(struct ecu_rbuf *me, uint8_t *buffer, size_t len)
{
    ECU_RUNTIME_ASSERT( (me && buffer) );
    ECU_RUNTIME_ASSERT( (len >= MIN_LEN) );
    me->buffer = buffer;
    me->len = len;
    me->read_index = 0;
    me->write_index = 0;
}

size_t ecu_rbuf_available(const struct ecu_rbuf *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_rbuf_valid(me)) );

    /* Cache read and write index in case they are changed elsewhere. */
    size_t read_index = me->read_index;
    size_t write_index = me->write_index;
    size_t available = 0;

    /* Branch in this specific order. Assumption is empty case is most common. */
    if (read_index == write_index) /* empty? */
    {
        available = me->len - 1; /* -1 since 1 byte reserved for full/empty. */
    }
    else if (read_index > write_index)
    {
        available = read_index - write_index - 1; /* -1 since 1 byte reserved for full/empty. */
    }
    else
    {
        /* (len - write_index) gets available bytes from write_index to end. 
        (len - write_index + read_index) also gets remaining bytes starting at beginning to read_index. 
        Subtract 1 since 1 byte reserved for full/empty. */
        available = me->len - write_index + read_index - 1;
    }

    return available;
}

size_t ecu_rbuf_capacity(const struct ecu_rbuf *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_rbuf_valid(me)) );
    return (me->len - 1); /* -1 since 1 byte reserved for full/empty. */
}

void ecu_rbuf_clear(struct ecu_rbuf *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_rbuf_valid(me)) );
    me->read_index = 0;
    me->write_index = 0;
}

bool ecu_rbuf_full(const struct ecu_rbuf *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_rbuf_valid(me)) );

    /* Cache read and write index in case they are changed elsewhere. */
    size_t read_index = me->read_index;
    size_t next = me->write_index + 1;
    bool full = false;

    if (next >= me->len)
    {
        next = 0;
    }

    if (next == read_index)
    {
        full = true;
    }

    return full;
}

size_t ecu_rbuf_peek(struct ecu_rbuf *me, uint8_t *buffer, size_t len)
{
    ECU_RUNTIME_ASSERT( (me && buffer) );
    ECU_RUNTIME_ASSERT( (ecu_rbuf_valid(me)) );
    ECU_RUNTIME_ASSERT( (len < me->len) ); /* -1 since 1 byte reserved for full/empty. */

    /* Cache read and write index in case they are changed elsewhere. */
    size_t read_index = me->read_index;
    size_t write_index = me->write_index;
    size_t i = 0;

    while ((read_index != write_index) && (i < len))
    {
        buffer[i] = me->buffer[read_index];
        i++;
        read_index++;

        if (read_index >= me->len)
        {
            read_index = 0;
        }
    }

    return i;
}

size_t ecu_rbuf_read(struct ecu_rbuf *me, uint8_t *buffer, size_t len)
{
    ECU_RUNTIME_ASSERT( (me && buffer) );
    ECU_RUNTIME_ASSERT( (ecu_rbuf_valid(me)) );
    ECU_RUNTIME_ASSERT( (len < me->len) ); /* -1 since 1 byte reserved for full/empty. */

    /* Cache read and write index in case they are changed elsewhere. */
    size_t read_index = me->read_index;
    size_t write_index = me->write_index;
    size_t i = 0;

    while ((read_index != write_index) && (i < len))
    {
        buffer[i] = me->buffer[read_index];
        i++;
        read_index++;

        if (read_index >= me->len)
        {
            read_index = 0;
        }
    }

    if (i)
    {
        /* Only update read index if buffer had data. */
        me->read_index = read_index;
    }

    return i;
}

size_t ecu_rbuf_remove(struct ecu_rbuf *me, size_t len)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (ecu_rbuf_valid(me)) );
    ECU_RUNTIME_ASSERT( (len < me->len) ); /* -1 since 1 byte reserved for full/empty. */

    /* Cache read and write index in case they are changed elsewhere. */
    size_t read_index = me->read_index;
    size_t write_index = me->write_index;
    size_t i = 0;

    while ((read_index != write_index) && (i < len))
    {
        i++;
        read_index++;

        if (read_index >= me->len)
        {
            read_index = 0;
        }
    }

    if (i)
    {
        /* Only update read index if buffer had data. */
        me->read_index = read_index;
    }

    return i;
}

bool ecu_rbuf_write_all(struct ecu_rbuf *me, const uint8_t *data, size_t len)
{
    #warning "TODO"
}

size_t ecu_rbuf_write_partial(struct ecu_rbuf *me, const uint8_t *data, size_t len)
{
    #warning "TODO"
}

bool ecu_rbuf_valid(const struct ecu_rbuf *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    /* Cache read and write index in case they are changed elsewhere. */
    size_t read_index = me->read_index;
    size_t write_index = me->write_index;
    bool status = false;

    if ((me->buffer) &&
        (me->len >= MIN_LEN) &&
        (read_index < me->len) &&
        (write_index < me->len))
    {
        status = true;
    }

    return status;
}
