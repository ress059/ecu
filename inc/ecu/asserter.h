/**
 * @file
 * @brief Compile-time and run-time assert macros. Compile-time assert macro is always enabled.
 * Run-time asserts are enabled by default. They can be disabled by defining 
 * ECU_DISABLE_RUNTIME_ASSERTS. I.e. -DECU_DISABLE_RUNTIME_ASSERTS. Example usage:
 * 
 * @code{.c}
 * /-------------------------------- Run-time assert example: -------------------------/
 * /---------------------------------------- file.c -----------------------------------/
 * 
 * // Handler you define if an assert fires. Must have this prototype.
 * static my_assert_handler(struct ecu_assert_functor *me, const char *file, int line);
 * 
 * // User data you can pass into your assert handler.
 * struct my_data_t
 * {
 *     int x;
 *     int y;
 * };
 * 
 * static struct my_data_t my_data = {5, 10};
 * 
 * 
 * // Example assert handler definition.
 * static my_assert_handler(struct ecu_assert_functor *me, const char *file, int line)
 * {
 *     // The ECU_RUNTIME_ASSERT() macro will pass the file name where the assert 
 *     // fired into this parameter. User can choose to use it in their handler.
 *     (void)file;
 * 
 *     // The ECU_RUNTIME_ASSERT() macro will pass the line number where the assert 
 *     // fired into this parameter. User can choose to use it in their handler.
 *     (void)line;
 * 
 *     // User-supplied data.
 *     struct my_data_t *data = (struct my_data_t *)me->data; // User responsible for casting.
 *     data->x.. // 5
 *     data->y.. // 10
 *     
 *     // Define behavior of the system if an assert fires.
 *     perform_system_recovery();
 * }
 *   
 * 
 * // Create your functor. 
 * struct ecu_assert_functor my_functor = 
 * {
 *     .handler = &my_assert_handler,
 *     .data = (void *)&my_data 
 * };
 * 
 * 
 * // Runtime assert usage:
 * void app_function(int *ptr)
 * {
 *     // Run your custom handler my_assert_handler() with your custom data 
 *     // if NULL pointer supplied.
 *     ECU_RUNTIME_ASSERT(ptr, &my_functor);
 * 
 *     // If you don't want the hassle of using your own functor and handler 
 *     // you can supply ECU_DEFAULT_FUNCTOR instead. A default handler will execute 
 *     // instead. The default handler hangs in a permanent while loop if NDEBUG 
 *     // is not defined so users are able to inspect the call stack.
 *     ECU_RUNTIME_ASSERT(ptr, ECU_DEFAULT_FUNCTOR);
 * }
 * @endcode
 * \n
 * 
 * Compile-time assert macro expands to static_assert variants or negative array 
 * indexing to produce compilation errors depending on which C/C++ standard is used when 
 * compiling. Serves as a common interface that the Application can use to remain backwards 
 * compatible with various standards. Text is not passed into this macro in case the 
 * implementation (compiler) allocates memory for it. The compilation error messages should 
 * be enough to locate which static assert fired.
 * 
 * Run-time assert macro should be used when the system is still in control. I.e. no hard 
 * faults, stack overflows, any other NMI faults, etc. A unique, user-defined functor can
 * be supplied to each @ref ECU_RUNTIME_ASSERT() call. If the assert fires, your custom 
 * functor will execute. If you do not wish to create your own functor, @ref ECU_DEFAULT_FUNCTOR 
 * can instead be supplied to @ref ECU_RUNTIME_ASSERT() macro. If an assert fires then a 
 * default functor will execute instead. The default functor hangs in a permanent while loop 
 * if NDEBUG is not defined so users are able to inspect the call stack. See code example
 * above for more details.
 * 
 * @warning Disabling run-time asserts will decrease overhead of this library. However checks 
 * (null pointer checks, invalid function parameters, etc) will obviously no longer be done in 
 * library API functions or in your Application.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_ASSERTER_H_
#define ECU_ASSERTER_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ INCLUDES ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* Unused attribute. */
#include "ecu/attributes.h"



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- COMPILE-TIME ASSERT MACROS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#if defined(ECU_DOXYGEN)
    /**
     * @brief Produce compilation error if assert fires.
     * @details Macro expands to different variants depending on which C/C++ standard is 
     * used when compiling. Serves as a common interface that the Application can use to
     * remain backwards compatible with various standards.
     * 
     * 1. Expands to this when compiling with C++11 and greater or C23 and greater. 
     * Uses static_assert() natively supported by this standard.
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_, msg_)               static_assert((check_), msg_)
     * @endcode
     * 
     * 2. Expands to this when compiling with C11 and greater (before C23). 
     * Uses _Static_assert() which is defined in the assert.h header file. Therefore 
     * assert.h is also included in this case.
     * @code{.c}
     * #include <assert.h> // Included iby asserter.h module.
     * 
     * #define ECU_STATIC_ASSERT(check_, msg_)              _Static_assert((check_), msg_)
     * @endcode
     * 
     * 3. Expands to this when compiling with C23 and greater. Uses static_assert() 
     * which is natively supported by this standard.
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_, msg_)              static_assert((check_), msg_)
     * @endcode
     * 
     * 4. Expands to this when using C/C++ standard that does not support
     * static assertions. If an assert fires, this macro produces a compilation 
     * error by referencing a symbol that is a negative-sized array. @p msg_
     * is not used in this case.
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_, msg_)              extern const char ecu_static_assert_do_not_use_[(check_) ? 1 : -1]
     * @endcode
     * 
     * @param check_ Condition to check. If this is true the assertion passes.
     * If this is false the assertion fails and triggers a compilation error.
     * This must be a literal expression that can be evaluated at compile-time.
     * @param msg_ Message shown in console if static assert fires. Unused if
     * using pre-C11 or pre-C++11 standard but still required by caller to maintain 
     * backwards compatibility.
     */
#   define ECU_STATIC_ASSERT(check_, msg_)
#else
#   if (__cplusplus >= 201103L)
        /**
         * @brief Compiling with C++11 and greater so use static_assert().
         */
#       define ECU_STATIC_ASSERT(check_, msg_)          static_assert((check_), msg_)
#   elif !defined(__cplusplus) && (__STDC_VERSION__ >= 201112L) && (__STDC_VERSION__ < 202311L)
#       include <assert.h>

        /**
         * @brief Compiling with C11 and greater (before C23) so use _Static_assert(). 
         * Includes assert.h header.
         */
#       define ECU_STATIC_ASSERT(check_, msg_)          _Static_assert((check_), msg_)
#   elif !defined(__cplusplus) && (__STDC_VERSION__ >= 202311L)

        /**
         * @brief Compiling with C23 and greater so use static_assert().
         */
#       define ECU_STATIC_ASSERT(check_, msg_)          static_assert((check_), msg_)
#   else
#       define ECU_USING_CUSTOM_STATIC_ASSERT_DO_NOT_USE_
        /**
         * @brief Compiling with standard that does not natively support static assertions.
         * Use custom implementation - reference symbol with negative-sized array.
         * 
         * @note msg_ parameter still required by caller to maintain backwards compatibility
         * across all C/C++ standards.
         */
#       define ECU_STATIC_ASSERT(check_, msg_)          extern const char ecu_static_assert_do_not_use_[(check_) ? 1 : -1] ECU_ATTRIBUTE_UNUSED
#   endif
#endif /* ECU_DOXYGEN */



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- RUNTIME ASSERT MACROS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/
#if defined(ECU_DOXYGEN)
    /**
     * @brief Define the file name @ref ECU_RUNTIME_ASSERT() uses for this file.
     * @details Use this macro at the top of each source file if @ref ECU_RUNTIME_ASSERT()
     * is ever used. @p name_ defines the string literal that will be passed 
     * to @ref ECU_RUNTIME_ASSERT() and @ref ecu_assert_handler() if an assert 
     * fires within your source file. This method is a more memory-efficient
     * alternative to the conventional __FILE__ macro. Behavior of this macro
     * depends on if ECU_DISABLE_RUNTIME_ASSERTS is defined or not:
     * 
     * 1. If ECU_DISABLE_RUNTIME_ASSERTS is NOT defined then this macro expands
     * to:
     * @code{.c}
     * #define ECU_ASSERT_DEFINE_NAME(name_)    static const char ecu_file_name_[] = name_;
     * @endcode
     * 
     * 2. If ECU_DISABLE_RUNTIME_ASSERTS is defined then this macro expands to
     * nothing since run-time asserts are disabled:
     * @code{.c}
     * #define ECU_ASSERT_DEFINE_NAME(name_)
     * @endcode
     * 
     * @warning Do **not** terminate this macro with a semicolon.
     * 
     * @param name_ string literal representing source file name.
     */
#   define ECU_ASSERT_DEFINE_NAME(name_)

    /**
     * @pre @ref ECU_ASSERT_DEFINE_NAME() called.
     * @brief Assert condition is true at run-time.
     * @details This macro should be used to verify conditions that should
     * always be true unless there is a software bug or processor error.
     * For example: NULL dereferences, out of bounds array access, invalid
     * function parameters, etc. Behavior of this macro depends on if 
     * ECU_DISABLE_RUNTIME_ASSERTS is defined:
     * 
     * 1. If ECU_DISABLE_RUNTIME_ASSERTS is NOT defined then run-time asserts
     * are active. This macro will call a user-defined handler if an assert 
     * fires. The file name and line number where the assert fired will be
     * passed into this handler. It expands to:
     * @code{.c}
     * define ECU_RUNTIME_ASSERT(check_)       ((check_) ? ((void)0) : ecu_assert_handler(&ecu_file_name_[0], __LINE__))
     * @endcode
     * 
     * The file name (&ecu_file_name_[0]) will be the string passed into
     * @ref ECU_ASSERT_DEFINE_NAME() macro, which must be called at the
     * top of each source file before any run-time asserts are used.
     * 
     * 2. If ECU_DISABLE_RUNTIME_ASSERTS is defined then run-time asserts
     * are disabled and this macro does nothing. This reduces overhead at
     * the cost of reliability. It expands to:
     * @code{.c}
     * define ECU_RUNTIME_ASSERT(check_)       ((void)0)
     * @endcode
     * 
     * @param check_ Condition to check. Assert passes if this is true.
     * Assert fires if this is false.
     */
#   define ECU_RUNTIME_ASSERT(check_)
#else
#   if !defined(ECU_DISABLE_RUNTIME_ASSERTS)
        /**
         * @brief Run-time asserts enabled so this macro defines the
         * file name.
         */
#       define ECU_ASSERT_DEFINE_NAME(name_)    static const char ecu_file_name_[] = name_;

        /**
         * @brief Run-time asserts enabled so this macro verifies
         * conditions are true.
         */
#       define ECU_RUNTIME_ASSERT(check_)       ((check_) ? ((void)0) : ecu_assert_handler(&ecu_file_name_[0], __LINE__))
#   else
        /**
         * @brief Runtime asserts disabled so this macro does nothing.
         */
#       define ECU_ASSERT_DEFINE_NAME(name_)

        /**
         * @brief Runtime asserts disabled so this macro does nothing.
         */
#       define ECU_RUNTIME_ASSERT(check_)       ((void)0)
#   endif

#endif /* ECU_DOXYGEN */



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- PUBLIC FUNCTIONS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @pre @ref ECU_ASSERT_DEFINE_NAME() called.
 * @brief Defines system response under an assert condition. 
 * @details This is called if @ref ECU_RUNTIME_ASSERT() fires. It 
 * MUST be defined by the user since system response behavior is a 
 * property of the application. A linker error will occur if this 
 * is not defined.
 * 
 * @param file File name the assert fired in. This is the string
 * literal passed into @ref ECU_ASSERT_DEFINE_NAME().
 * @param line Line number where the assert fired.
 */
extern void ecu_assert_handler(const char *file, int line);

#ifdef __cplusplus
}
#endif

#endif /* ECU_ASSERTER_H_ */
