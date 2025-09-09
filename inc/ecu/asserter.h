/**
 * @file
 * @brief
 * @rst
 * See :ref:`asserter.h section <asserter_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */

#ifndef ECU_ASSERTER_H_
#define ECU_ASSERTER_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* ECU. */
#include "ecu/attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------*/
/*-------------------- STATIC ASSERT MACROS ------------------*/
/*------------------------------------------------------------*/

#if defined(ECU_DOXYGEN)
    /**
     * @name Static Assert
     */
    /**@{*/
    /**
     * @brief Portable static assert macro. Expands to appropriate static
     * assert variant based on the C/C++ standard being used.
     *
     * 1. Expands to this when compiling with C++11 and greater.
     * Uses static_assert() which is natively supported by this standard.
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_, msg_) \
     *      static_assert((check_), msg_)
     * @endcode
     *
     * 2. Expands to this when compiling with C11 and greater (before C23).
     * Uses _Static_assert() which is natively supported by this standard.
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_, msg_) \
     *      _Static_assert((check_), msg_)
     * @endcode
     *
     * 3. Expands to this when compiling with C23 and greater. 
     * Uses static_assert() which is natively supported by this standard.
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_, msg_) \
     *      static_assert((check_), msg_)
     * @endcode
     *
     * 4. Expands to this when using C/C++ standard that does not support
     * static asserts. If an assert fires, this macro produces a compilation
     * error by referencing a symbol that is a negative-sized array. @p msg_
     * is not used in this case. Note however that @p msg_ must still be
     * supplied by the caller for backwards compatibility.
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_, msg_) \
     *      extern const char ecu_static_assert_fired_[(check_) ? 1 : -1]
     * @endcode
     *
     * @param check_ Condition to check. If this is true the assertion passes.
     * If this is false the assertion fails and triggers a compilation error.
     * This must be an expression that can be evaluated at compile-time.
     * @param msg_ Message shown in console if static assert fires. Unused if
     * using pre-C11 or pre-C++11 standard but still required by caller to maintain
     * backwards compatibility.
     */
    #define ECU_STATIC_ASSERT(check_, msg_)
    /**@}*/
#else
    #if defined(__cplusplus) && (__cplusplus >= 201103L)
        /// @brief C++11 and greater static_assert().
        #define ECU_STATIC_ASSERT(check_, msg_) \
            static_assert((check_), msg_)
    #elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && (__STDC_VERSION__ < 202311L)
        /// @brief C11 to C23 _Static_assert().
        #define ECU_STATIC_ASSERT(check_, msg_) \
            _Static_assert((check_), msg_)
    #elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
        /// @brief C23 and greater static_assert().
        #define ECU_STATIC_ASSERT(check_, msg_) \
            static_assert((check_), msg_)
    #else
        /// @brief Custom static assert implementation since compiling with standard 
        /// that does not natively support static assert.
        #define ECU_STATIC_ASSERT(check_, msg_) \
            extern const char ecu_static_assert_fired_[(check_) ? 1 : -1]
    #endif
#endif /* ECU_DOXYGEN */

/*------------------------------------------------------------*/
/*-------------------- RUNTIME ASSERT MACROS -----------------*/
/*------------------------------------------------------------*/

#if defined(ECU_DOXYGEN)
    /**
     * @name Runtime Assert
     */
    /**@{*/
    /**
     * @pre @ref ECU_ASSERT_DEFINE_FILE() called at start of file.
     * @brief Assert condition is true at run-time. Expands differently
     * depending on whether asserts are enabled:
     * 
     * 1. If asserts are enabled (ECU_DISBALE_RUNTIME_ASSERTS is NOT defined) 
     * then this macro calls a custom-defined handler when an assertion fires and 
     * expands to: 
     * @code{.c}
     * define ECU_ASSERT(check_) \
     *      ((check_) ? ((void)0) : ecu_assert_handler(&ecu_file_name_[0], __LINE__))
     * @endcode
     *
     * 2. If asserts are disabled (ECU_DISABLE_RUNTIME_ASSERTS is defined)
     * then this macro does nothing:
     * @code{.c}
     * define ECU_ASSERT(check_) \
     *      ((void)0)
     * @endcode
     *
     * @param check_ Condition to check. Assert passes if this is true.
     * Assert fires if this is false.
     */
    #define ECU_ASSERT(check_)

    /**
     * @brief Defines file name @ref ECU_ASSERT() uses if assertion
     * fires. This is a more memory-efficient alternative to the \_\_FILE\_\_ 
     * macro. Expands differently depending on whether asserts are enabled.
     * 
     * 1. If asserts are enabled (ECU_DISBALE_RUNTIME_ASSERTS is NOT defined) 
     * then this expands to:
     * @code{.c}
     * #define ECU_ASSERT_DEFINE_FILE(name_) \
     *      static const char ecu_file_name_[] = name_;
     * @endcode
     * 
     * 2. If asserts are disabled (ECU_DISABLE_RUNTIME_ASSERTS is defined)
     * then this expands to nothing:
     * @code{.c}
     * #define ECU_ASSERT_DEFINE_FILE(name_)
     * @endcode
     * 
     * @warning Do NOT terminate this macro with a semicolon.
     *
     * @param name_ string literal representing source file name.
     */
    #define ECU_ASSERT_DEFINE_FILE(name_)
    /**@}*/
#else
    #if !defined(ECU_DISABLE_RUNTIME_ASSERTS)
        /// @brief Runtime asserts enabled so this macro calls
        /// custom-defined handler if an assertion fires.
        #define ECU_ASSERT(check_) \
            ((check_) ? ((void)0) : ecu_assert_handler(&ecu_file_name_[0], __LINE__))

        /// @brief Runtime asserts enabled so this macro defines the
        /// file name supplied when an assert fires.
        #define ECU_ASSERT_DEFINE_FILE(name_) \
            static const char ecu_file_name_[] ECU_ATTRIBUTE_UNUSED = name_;
    #else
        /// @brief Runtime asserts disabled so this macro does nothing.
        #define ECU_ASSERT(check_) \
            ((void)0)

        /// @brief Runtime asserts disabled so this macro does nothing.
        #define ECU_ASSERT_DEFINE_FILE(name_)
    #endif
#endif /* ECU_DOXYGEN */

/*------------------------------------------------------------*/
/*--------------------- PUBLIC FUNCTIONS ---------------------*/
/*------------------------------------------------------------*/

/**
 * @brief User-defined system response under an assert condition.
 * Called by @ref ECU_ASSERT() when an assertion fires.
 * 
 * @warning This function MUST be defined by the user since system 
 * response is a property of the application. A linker error will 
 * occur if this is not defined.
 *
 * @param file File name the assert fired in. This is the string
 * literal passed into @ref ECU_ASSERT_DEFINE_FILE().
 * @param line Line number where the assert fired.
 */
extern void ecu_assert_handler(const char *file, int line);

#ifdef __cplusplus
}
#endif

#endif /* ECU_ASSERTER_H_ */
