/**
 * @file
 * @brief 
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-26
 * @copyright Copyright (c) 2024
 */



#ifndef ECU_RING_BUFFER_H_
#define ECU_RING_BUFFER_H_

#include <stdbool.h>
#include <stddef.h> /* size_t */
#include <stdint.h>


struct ecu_ring_buffer
{
    void *buffer; // this ring buffer must have exclusive ownership of this buffer
    volatile size_t head; // index
    volatile size_t tail; // index
    volatile bool full;
    size_t element_size;            /* Number of bytes */
    size_t max_number_of_elements;     /* Number of bytes */
};



extern void ecu_ring_buffer_ctor(struct ecu_ring_buffer *me, 
                                 void *buffer_0, 
                                 size_t element_size_0, 
                                 size_t number_of_elements_0);

// only resets head and tail. does not reset buffer contents.
extern void ecu_ring_buffer_clear(struct ecu_ring_buffer *me);

extern bool ecu_ring_buffer_write(struct ecu_ring_buffer *me, 
                                  const void *element, 
                                  size_t size);

extern bool ecu_ring_buffer_read(struct ecu_ring_buffer *me, 
                                 void *element, 
                                 size_t size);

// number of elements currently stored.
extern size_t ecu_ring_buffer_get_size(const struct ecu_ring_buffer *me);

// max number of elements that can be stored.
extern size_t ecu_ring_buffer_get_max_size(const struct ecu_ring_buffer *me);

extern bool ecu_ring_buffer_is_empty(const struct ecu_ring_buffer *me);

extern bool ecu_ring_buffer_is_full(const struct ecu_ring_buffer *me);



/**
 * @name Asserts In This Module
 */
/**@{*/
/**
 * @brief Set a functor to execute if an assert fires within this module. 
 * @details This is optional - if no functor is set a default one will be 
 * used. The default functor hangs in a permanent while loop if NDEBUG is 
 * not defined so users are able to inspect the call stack.
 * 
 * @param functor User-supplied functor. If a NULL value is supplied
 * the default functor will be used.
 */
extern void ecu_ring_buffer_set_assert_functor(struct ecu_assert_functor *functor);
/**@}*/



#endif /* ECU_RING_BUFFER_H_ */
