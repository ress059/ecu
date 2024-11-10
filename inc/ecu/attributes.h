/**
 * @file
 * @brief 
 * @rst
 * See :ref:`attributes.h section <attributes_h>` in Sphinx documentation. 
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-08-31
 * @copyright Copyright (c) 2024
 */

#ifndef ECU_ATTRIBUTES_H_
#define ECU_ATTRIBUTES_H_

#if defined(ECU_DOXYGEN)
    /**
     * @brief This attribute is assigned to a variable to tell the compiler that it 
     * may be unused. 
     * @details Variable(s) with this attribute will not produce unused warnings
     * even if the unused warning flag is passed to the compiler.
     * 
     * 1. Expands to this if compiling with GCC.
     * @code{.c}
     * #define ECU_ATTRIBUTE_UNUSED             __attribute__((unused))
     * @endcode
     * 
     * 2. If none of the previous options apply to your toolchain then you 
     * are using an unsupported compiler. Non-critical attribute so macro
     * will expand to nothing in this case. Therefore any code using this macro 
     * will still compile.
     * @code{.c}
     * #define ECU_ATTRIBUTE_UNUSED
     * @endcode
     */
#   define ECU_ATTRIBUTE_UNUSED

    /**
     * @brief This attribute is assigned to enum, struct, or union types and 
     * tells the compiler to not produce any padding between members.
     * @details
     * 1. Expands to this if compiling with GCC.
     * @code{.c}
     * #define ECU_ATTRIBUTE_PACKED             __attribute__((packed))
     * @endcode
     * 
     * 2. If none of the previous options apply to your toolchain then you are 
     * using an unsupported compiler. Critical attribute so this macro will NOT be 
     * defined. Any code using this macro will produce a compilation error. 
     */
#   define ECU_ATTRIBUTE_PACKED
#else
#   ifdef __GNUC__
        /**
         * @brief Compiling with GCC so expands to GCC unused attribute.
         */
#       define ECU_ATTRIBUTE_UNUSED                 __attribute__((unused))

        /**
         * @brief Compiling with GCC so expands to GCC packed attribute.
         */
#       define ECU_ATTRIBUTE_PACKED                 __attribute__((packed))
#   else /* Unsupported compilers. Empty-define only non-critical attributes. */
        /**
         * @brief Using unsupported compiler so this macro expands to nothing
         * since it is not critical to the code's functionality.
         */
#       define ECU_ATTRIBUTE_UNUSED
#   endif
#endif /* ECU_DOXYGEN */

#endif /* ECU_ATTRIBUTES_H_ */
