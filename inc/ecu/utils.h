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
/*---------------------------- MACROS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns the size of a member within a struct
 * or union at compile-time.
 * 
 * @param type_ Struct or union type containing @p member_.
 * @param member_ Return size of this member.
 */
#define ECU_FIELD_SIZEOF(type_, member_) \
    (sizeof(((type_ *)0)->member_))

#endif /* ECU_UTILS_H_ */
