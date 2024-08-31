/**
 * @file
 * @brief Wraps some popular, compiler-specific attributes around #define
 * directives. Providing this interface allows all compiler-specific attributes 
 * to be contained within this single module instead of being scattered throughout 
 * application code. Therefore if the toolchain changes, only the macro definitions 
 * in this file would need to be updated (with respect to the compiler attributes). 
 * For example:
 * 
 * @code{.c}
 * // Compiler-specific attributes should never be used directly in application
 * // code like this. If a different compiler needs to be used that does not 
 * // support __attribute__((unused)) then declarations like this must be 
 * // manually changed across the entire application.
 * int var __attribute__((unused)) = 1;
 * 
 * 
 * // Pseudocode for this module
 * #ifdef __GNUC__
 * #    define ECU_ATTRIBUTE_UNUSED            __attribute__((unused))
 * #endif
 * 
 * 
 * // Using this module is a much better approach as this adds an additional
 * // layer of protection. If a different compiler needs to be used that does 
 * // not support __attribute__((unused)) then only the ECU_ATTRIBUTE_UNUSED
 * // definition has to be changed. Any application code referencing this define
 * // can remain the same.
 * int var2 ECU_ATTRIBUTE_UNUSED = 1;
 * @endcode
 * 
 * @warning Only GCC is currently supported. For unsupported compilers, attributes 
 * that do not effect code functionality will be defined, but expand to nothing so 
 * any application using them will still compile. An example of this is @ref ECU_ATTRIBUTE_UNUSED
 * For unsupported compilers, attributes that are critical to code functionality
 * will NOT be defined. Therefore if any of these attributes are used in your code,
 * a compilation error will occur from an unresolved symbol reference. An example of
 * this is @ref ECU_ATTRIBUTE_PACKED
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
     * may be unused. Variable(s) with this attribute will not produce unused warnings
     * even if the unused warning flag is passed to the compiler.
     * 
     * 1. Expands to this if compiling with GCC.
     * @code{.c}
     * #define ECU_ATTRIBUTE_UNUSED             __attribute__((unused))
     * @endcode
     * 
     * 2. If none of the previous options apply to your toolchain (you are using
     * an unsupported compiler), then this expands to nothing since the unused
     * attribute is not critical to code's functionality. Therefore any code
     * using this macro will still compile.
     * @code{.c}
     * #define ECU_ATTRIBUTE_UNUSED
     * @endcode
     */
#   define ECU_ATTRIBUTE_UNUSED

    /**
     * @brief This attribute is assigned to enum, struct, or union types and 
     * tells the compiler to not produce any padding between members.
     * 
     * @warning If you are using an unsupported compiler this macro will NOT be
     * defined. Therefore if this macro is used anywhere in your code but your
     * toolchain does not support it, a compilation error will occur from an unresolved 
     * symbol reference. This is because the packed attribute effects code's functionality 
     * so an error should be emitted if it is used, but not supported by your toolchain.
     * 
     * 1. Expands to this if compiling with GCC.
     * @code{.c}
     * #define ECU_ATTRIBUTE_PACKED             __attribute__((packed))
     * @endcode
     * 
     * 2. If none of the previous options apply to your toolchain (you are using
     * an unsupported compiler), then this will NOT be defined and will produce
     * a compilation error if used. See warning directive for more details.
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
