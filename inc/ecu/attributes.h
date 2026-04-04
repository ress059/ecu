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
     * @name Critical Attributes
     * These attributes effect the functionality of the program. Critical attributes
     * not supported by the target compiler are NOT defined in order to create a
     * a compilation error (critical attributes used in the program create
     * unresolved symbols).
     */
    /**@{*/
    /**
     * @brief Critical attribute. Attached to a function declaration to inform
     * the compiler the routine never returns.
     *
     * @warning This attribute should be at the start of the function declaration to
     * be fully portable.
     */
    #define ECU_ATTRIBUTE_NORETURN

    /**
     * @brief Critical attribute. If attached to a variable, informs compiler it should
     * have the smallest possible alignment. If attached to a type definition, informs
     * compiler the minimum required memory should be used to represent the type.
     */
    #define ECU_ATTRIBUTE_PACKED

    /**
     * @brief Critical attribute. Places symbol in user-specified program section.
     *
     * @param x_ Name of section. Must be string literal.
     */
    #define ECU_ATTRIBUTE_SECTION(x_)
    /**@}*/

    /**
     * @name Non-critical Attributes
     * These attributes do not effect the functionality of the program. Empty definitions
     * are created for non-critical attributes that are not supported by the target
     * compiler, which allows the program to still compile.
     */
    /**@{*/
    /**
     * @brief Non-critical attribute. Informs compiler that type, variable,
     * or function may be unused so it does not emit an unused warning.
     */
    #define ECU_ATTRIBUTE_UNUSED
    /**@}*/
#else
    /* ECU_ATTRIBUTE_NORETURN - Critical attribute. */
    #if defined(__cplusplus) && (__cplusplus >= 201103L) /* Using C++11 and up. */
        #define ECU_ATTRIBUTE_NORETURN [[noreturn]]
    #elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && (__STDC_VERSION__ < 202311L) /* Using C11 to C17. */
        #define ECU_ATTRIBUTE_NORETURN _Noreturn
    #elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L) /* Using C23 and up. */
        #define ECU_ATTRIBUTE_NORETURN [[noreturn]]
    #elif defined(__GNUC__) && !defined(__STRICT_ANSI__) /* Compiling with GCC with extensions enabled (attributes are compiler extensions not supported in ISO C). */
        #if __GNUC__ > 2 || (__GNUC__ == 2 && (__GNUC_MINOR__ > 95 || (__GNUC_MINOR__ == 95 && __GNUC_PATCHLEVEL__ > 2))) /* GCC >= v2.95.3. Oldest version in GCC docs. */
            #define ECU_ATTRIBUTE_NORETURN __attribute__((noreturn))
        #endif
    #endif

    /* ECU_ATTRIBUTE_PACKED - Critical attribute. */
    #if defined(__GNUC__) && !defined(__STRICT_ANSI__) /* Compiling with GCC with extensions enabled (attributes are compiler extensions not supported in ISO C). */
        #if __GNUC__ > 2 || (__GNUC__ == 2 && (__GNUC_MINOR__ > 95 || (__GNUC_MINOR__ == 95 && __GNUC_PATCHLEVEL__ > 2))) /* GCC >= v2.95.3. Oldest version in GCC docs. */
            #define ECU_ATTRIBUTE_PACKED __attribute__((packed))
        #endif
    #endif

    /* ECU_ATTRIBUTE_SECTION - Critical attribute. */
    #if defined(__GNUC__) && !defined(__STRICT_ANSI__) /* Compiling with GCC with extensions enabled (attributes are compiler extensions not supported in ISO C). */
        #if __GNUC__ > 2 || (__GNUC__ == 2 && (__GNUC_MINOR__ > 95 || (__GNUC_MINOR__ == 95 && __GNUC_PATCHLEVEL__ > 2))) /* GCC >= v2.95.3. Oldest version in GCC docs. */
            #define ECU_ATTRIBUTE_SECTION(x_) __attribute__((section(x_)))
        #endif
    #endif

    /* ECU_ATTRIBUTE_SECTION - Non-critical attribute. */
    #if defined(__GNUC__) && !defined(__STRICT_ANSI__) /* Compiling with GCC with extensions enabled (attributes are compiler extensions not supported in ISO C). */
        #if __GNUC__ > 2 || (__GNUC__ == 2 && (__GNUC_MINOR__ > 95 || (__GNUC_MINOR__ == 95 && __GNUC_PATCHLEVEL__ > 2))) /* GCC >= v2.95.3. Oldest version in GCC docs. */
            #define ECU_ATTRIBUTE_UNUSED __attribute__((unused))
        #endif
    #endif

    /* Non-critical attributes. */
    #ifndef ECU_ATTRIBUTE_UNUSED
        #define ECU_ATTRIBUTE_UNUSED
    #endif
#endif /* ECU_DOXYGEN */

#endif /* ECU_ATTRIBUTES_H_ */
