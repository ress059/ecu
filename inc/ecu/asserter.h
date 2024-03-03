/**
 * @file asserter.h
 * @author Ian Ress
 * @brief Compile-time and run-time assert macros. Compile-time assert macro is always enabled.
 * Run-time asserts are enabled by default. They can be disabled by defining 
 * ECU_DISABLE_RUNTIME_ASSERTS. I.e. -DECU_DISABLE_RUNTIME_ASSERTS. Disabling run-time asserts
 * will decrease overhead of this library, but checks (null pointer checks, invalid function
 * parameters, etc) will obviously no longer be done in library API functions. Also note that
 * unused function/parameter warnings will occur when compiling since run-time assert macro
 * calls no longer use any of the parameters passed to it.
 * 
 * @details Compile-time assert macro expands to static_assert variants or negative array 
 * indexing to produce compilation errors depending on which C/C++ standard is used when 
 * compiling. Serves as a common interface that the Application can use to remain backwards 
 * compatible with various standards. Example calls:
 * 
 * @code{.c}
 * ECU_STATIC_ASSERT( (true) ); // Passes
 * ECU_STATIC_ASSERT( (5==5) ); // Passes
 * 
 * ECU_STATIC_ASSERT( (false) ); // Fails
 * ECU_STATIC_ASSERT( (sizeof(int) >= 2) ); // Passes
 * @endcode
 * 
 * A message is not passed to this macro in case the implementation allocates
 * memory for it. The compilation error messages should be enough to locate
 * which assert fired.
 * 
 * Run-time assert macro should be used when the system is still in control. 
 * I.e. no hard faults, stack overflows, any other NMI faults, etc.
 * The following describes run-time assert behavior if it is enabled:
 * 
 * If a run-time assert fires it will execute a user-defined handler function that
 * can be set by calling @ref ecu_asserter_set_handler(). This should be initialized immediately 
 * and only once on startup. This handler is common across all @ref ECU_RUNTIME_ASSERT() calls. 
 * In other words, it is not possible to execute unique handler functions across 
 * different @ref ECU_RUNTIME_ASSERT() calls.
 * 
 * If no user-defined handler is supplied then a default handler will execute instead.
 * This default handler is an infinite while-loop if NDEBUG is not defined. Otherwise
 * the default handler does nothing. I.e.
 * 
 * @code{.c}
 * // Default handler.
 * #if !defined(NDEBUG)
 * while (1)
 * {
 *     // Permanently hang to inspect call stack.
 * }
 * #endif
 * @endcode
 * 
 * An example application using this asserter could look something like this:
 * 
 * @code{.c}
 * /-------------------- main.c ---------------------/
 * #include <ecu/asserter.h>
 * 
 * ECU_STATIC_ASSERT( (sizeof(int) == 4) );
 * 
 * 
 * static void app_assert_handler(const char *file_name, int line);
 * static void app_assert_handler(const char *file_name, int line)
 * {
 *     // STEP 1: Log file_name and line.
 *     // STEP 2: Log register values. I.e. link register and program counter.
 *     // STEP 3: Reset CPU.
 * }
 * 
 * int main(void)
 * {
 *     // Initialize assert handler right on startup.
 *     ecu_asserter_set_handler(&app_assert_handler);
 * 
 *     while (1)
 *     {
 *         uint8_t *mem = allocate_critical_memory();
 *         ECU_RUNTIME_ASSERT( (mem) );
 *     }
 * }
 * @endcode
 * 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef ASSERTER_H_
#define ASSERTER_H_


#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- COMPILE-TIME ASSERT MACROS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#if defined(ECU_DOXYGEN)

    /**
     * @brief Produce compilation error if assert fires.
     * 
     * @details Macro expands to different variants depending on which C/C++ standard is 
     * used when compiling. Serves as a common interface that the Application can use to
     * remain backwards compatible with various standards.
     * 
     * 1. Expands to this when compiling with C++11 and greater. Uses static_assert()
     * natively supported by this version.
     * 
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check)               static_assert((check_))
     * @endcode
     * 
     * 2. Expands to this when compiling with C11 and greater. Uses _Static_assert() 
     * which is defined in the assert.h header file. Therefore assert.h is also 
     * included in this case.
     * 
     * @code{.c}
     * #include <assert.h>
     * #define ECU_STATIC_ASSERT(check_)              _Static_assert((check_))
     * @endcode
     * 
     * 3. Expands to this when compiling with C23 and greater. Uses static_assert() 
     * which is natively supported in this standard.
     * 
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_)              static_assert((check_))
     * @endcode
     * 
     * 4. Expands to this when when using C/C++ standard that does not support
     * static assertions. Macro expands to extern char ecu_static_assert_[-1]
     * if assert fires, thus producing a compilation error.
     * 
     * @code{.c}
     * #define ECU_STATIC_ASSERT(check_)              extern char ecu_static_assert_[(check_) ? 1 : -1]
     * @endcode
     * 
     * @param check_ Condition to check. If this is true the assertion passes.
     * If this is false the assertion fails and triggers a compilation error.
     * This must be a literal expression that can be evaluated at compile-time.
     */
    #define ECU_STATIC_ASSERT(check_)
#else
    #if defined(__cplusplus) && (__cplusplus >= 201103L)
        /**
         * @brief Produce compilation error if assert fails. C++11 and greater
         * static_assert() which is natively supported.
         * 
         * @param check_ Condition to check. If this is true the assertion passes.
         * If this is false the assertion fails and triggers a compilation error.
         * This must be a literal expression that can be evaluated at compile-time.
         */
        #define ECU_STATIC_ASSERT(check)        static_assert((check_))
    #elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ == 201112L)
        #include <assert.h>

        /**
         * @brief Produce compilation error if assert fails. C11 and greater
         * _Static_assert(). Includes assert.h header.
         * 
         * @param check_ Condition to check. If this is true the assertion passes.
         * If this is false the assertion fails and triggers a compilation error.
         * This must be a literal expression that can be evaluated at compile-time.
         */
        #define ECU_STATIC_ASSERT(check_)       _Static_assert((check_))
    #elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)

        /**
         * @brief Produce compilation error if assert fails. C23 and greater
         * static_assert() which is natively supported.
         * 
         * @param check_ Condition to check. If this is true the assertion passes.
         * If this is false the assertion fails and triggers a compilation error.
         * This must be a literal expression that can be evaluated at compile-time.
         */
        #define ECU_STATIC_ASSERT(check_)       static_assert((check_))
    #else
        /**
         * @brief Produce compilation error if assert fails. Using C/C++ standard that
         * does not support static assertions. Macro expands to extern char 
         * ecu_static_assert_[-1] if assert fires, thus producing a compilation error.
         * 
         * @param check_ Condition to check. If this is true the assertion passes.
         * If this is false the assertion fails and triggers a compilation error.
         * This must be a literal expression that can be evaluated at compile-time.
         */
        #define ECU_STATIC_ASSERT(check_)       extern char ecu_static_assert_[(check_) ? 1 : -1]
    #endif
#endif /* ECU_DOXYGEN */



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- RUNTIME ASSERT MACROS ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#if defined(ECU_DOXYGEN)

    /**
     * @brief Expands differently depending on if ECU_DISABLE_RUNTIME_ASSERTS
     * is defined.
     * 
     * 1. Expands to this if ECU_DISABLE_RUNTIME_ASSERTS is NOT defined:
     * @code{.c}
     * #define ECU_RUNTIME_ASSERT(check_)      ((check_) ? ((void)0) : ecu_assert_do_not_use(&ecu_file_name_, __LINE__))
     * @endcode
     * 
     * Run-time asserts are active in this case. If an assert fires either 
     * a default handler or user-defined handler will execute. See @ref asserter.h 
     * file description for more details.
     * 
     * 2. Expands to this if ECU_DISABLE_RUNTIME_ASSERTS is defined:
     * @code{.c}
     * #define ECU_RUNTIME_ASSERT(check_)      ((void)0)
     * @endcode
     * 
     * Run-time asserts are disabled in this case so the macro does nothing.
     */
    #define ECU_RUNTIME_ASSERT(check_)
#else
    #if !defined(ECU_DISABLE_RUNTIME_ASSERTS)
        /**
         * @brief Used so memory is only allocated for file name once per file.
         * __FILE_NAME__ macro is not used because it is compiler-dependent.
         */
        static const char ecu_file_name_[] = __FILE__;

        /**
         * @brief Runtime asserts enabled so this macro does checks.
         * 
         * @param check_ Condition to check. If this is true nothing
         * happens. If this is false the assert fires and executes a
         * handler function. See @ref asserter.h file description for
         * more details.
         */
        #define ECU_RUNTIME_ASSERT(check_)      ((check_) ? ((void)0) : ecu_assert_do_not_use(&ecu_file_name_[0], __LINE__))
    #else
        /**
         * @brief Runtime asserts disabled so this macro does nothing.
         */
        #define ECU_RUNTIME_ASSERT(check_)      ((void)0)
    #endif

#endif /* ECU_DOXYGEN */



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PUBLIC FUNCTIONS --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PRIVATE. Do not call this function. This needs to have 
 * external linkage so @ref ECU_RUNTIME_ASSERT() macro can see 
 * and call it.
 */
void ecu_assert_do_not_use(const char *file, int line);


/**
 * @brief Set a user-defined handler to execute if @ref ECU_RUNTIME_ASSERT()
 * fires. Meant to be called once at startup.
 * 
 * @param handler User-defined handler function that executes if @ref ECU_RUNTIME_ASSERT() 
 * fires. @ref ECU_RUNTIME_ASSERT() macro will automatically pass the file name and line number 
 * where the run-time assert fired to your handler. If this is NULL then the assert handler 
 * will be reset back to the default.
 */
extern void ecu_asserter_set_handler(void (*handler)(const char *file, int line));

#ifdef __cplusplus
}
#endif

#endif /* ASSERTER_H_ */
