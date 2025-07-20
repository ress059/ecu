/**
 * @file
 * @brief 
 * @rst
 * See :ref:`utils.h section <utils_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-04-13
 * @copyright Copyright (c) 2025
 */

#ifndef ECU_UTILS_H_
#define ECU_UTILS_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>

/*------------------------------------------------------------*/
/*---------------------------- MACROS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Converts an intrusive member back into the data type 
 * that stores it. See @ref ECU_DNODE_GET_ENTRY() for example 
 * usage.
 *
 * @param ptr_ Pointer to intrusive member. This must be pointer 
 * to non-const.
 * @param type_ Data type containing the intrusive member 
 * that parameter @p ptr_ points to. Do not use const specifier.
 * I.e. struct my_type, never const struct my_type.
 * @param member_ Name of intrusive member within @p type_.
 */
#define ECU_CONTAINER_OF(ptr_, type_, member_) \
    ((type_ *)(void *)((uint8_t *)(ptr_) - offsetof(type_, member_)))

/**
 * @brief Const-qualified version of @ref ECU_CONTAINER_OF().
 * Converts an intrusive member back into the data type 
 * that stores it. See @ref ECU_DNODE_GET_CONST_ENTRY()
 * for example usage.
 *
 * @param ptr_ Pointer to intrusive member. This can be pointer
 * to non-const or const.
 * @param type_ Data type containing the intrusive member 
 * that parameter @p ptr_ points to. Do not use const specifier.
 * I.e. struct my_type, never const struct my_type.
 * @param member_ Name of intrusive member within @p type_.
 */
#define ECU_CONST_CONTAINER_OF(ptr_, type_, member_) \
    ((const type_ *)(const void *)((const uint8_t *)(ptr_) - offsetof(type_, member_)))

/**
 * @brief Returns the size of a member within a struct
 * or union <b>declaration</b> at compile-time.
 * 
 * @param type_ Struct or union type containing @p member_.
 * @param member_ Name of member within @p type_ to get size of.
 */
#define ECU_FIELD_SIZEOF(type_, member_) \
    (sizeof(((type_ *)0)->member_))

/**
 * @brief Returns true if derived class correctly inherits 
 * base class via C-style inheritance. False otherwise.
 * 
 * @param base_ Name of base class member within 
 * user's @p derived_ type.
 * @param derived_ Derived type to check.
 */
#define ECU_IS_BASE_OF(base_, derived_) \
    ((bool)(offsetof(derived_, base_) == (size_t)0))

#endif /* ECU_UTILS_H_ */
