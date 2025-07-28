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

#ifndef ECU_RBUF_H_
#define ECU_RBUF_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


/// tries to guarantee thread-safety but it is not guaranteed since C11 atomics
/// cannot be used. Steps taken to try to be thread-safe:
///     no shared variables from producer and consumer (ie no full flag)
///     a copy of read and write index are stored at the beginning of relevnat functions in case they are changed elsewhere.

#define ECU_RBUF_CTOR_COMPILETIME(buffer_, len_)    \
    {                                               \
        .buffer = buffer_,                          \
        .len = len_,                                \
        .read_index = 0,                            \
        .write_index = 0                            \
    }

struct ecu_rbuf
{
    uint8_t *buffer;
    size_t len; // total size, including reserved byte.
    volatile size_t read_index;
    volatile size_t write_index;

    // no full flag so a variable is not shared bewteen consumer and producer.
};



extern void ecu_rbuf_ctor(struct ecu_rbuf *me, uint8_t *buffer, size_t len);

extern size_t ecu_rbuf_available(const struct ecu_rbuf *me); // return num bytes available.
extern size_t ecu_rbuf_capacity(const struct ecu_rbuf *me); // return max size
extern void ecu_rbuf_clear(struct ecu_rbuf *me); // read_index == write_index
extern bool ecu_rbuf_full(const struct ecu_rbuf *me); // if write_index + 1 == read_index
extern size_t ecu_rbuf_peek(struct ecu_rbuf *me, uint8_t *buffer, size_t len); // look at contents but dont remove.
extern size_t ecu_rbuf_read(struct ecu_rbuf *me, uint8_t *buffer, size_t len); // first cache write index in case its changed.
extern size_t ecu_rbuf_remove(struct ecu_rbuf *me, size_t len); // remove len bytes of data. return num bytes removed.
extern size_t ecu_rbuf_size(const struct ecu_rbuf *me);
extern bool ecu_rbuf_write_all(struct ecu_rbuf *me, const uint8_t *data, size_t len); // dont think i need to cache read index.
extern size_t ecu_rbuf_write_partial(struct ecu_rbuf *me, const uint8_t *data, size_t len);
extern bool ecu_rbuf_valid(const struct ecu_rbuf *me);

#endif /* ECU_RBUF_H_ */
