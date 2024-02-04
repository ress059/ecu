/**
 * @file asserter.h
 * @author Ian Ress
 * @brief General Asserter for compile-time and run-time asserts. One of the following Toolchains MUST be used in order
 * to use this module:
 * 
 * - GCC v2.5 and up, OR
 * - C++11 and up, OR
 * - C11 and up.
 * 
 * Compile-time asserter uses static_assert variants or symbol redeclaration to produce compilation errors depending on
 * the target toolchain. Run-time asserter calls an assert handler callback function via My_Assert_Handler function 
 * pointer. Therefore the Application MUST assign a function to this pointer if this module is used. For example:
 * 
 * /----------------------------------- user_file.h ---------------------------/
 * #include "asserter.h"
 * 
 * void error_handler(void) ATTRIBUTE_NORETURN_;
 * 
 * (void)(*My_Assert_Handler)(void) ATTRIBUTE_NORETURN_ = &error_handler;
 * 
 * void error_handler(void)
 * {
 *      // User-defined runtime assert handler.
 * }
 * 
 * General Guidelines for Assertions:
 * 1) Always use assertions for bugs. I.e. NULL pointer dereference, out-of-bounds access, invalid 
 * function parameters.
 * 
 * 2) Never use assertions for undesired run-time behavior that can occur. I.e. out-of-range temperature
 * reading, communication loss, emergency message received from other device.
 * 
 * 3) Always keep assertions in production builds. Preprocessor define can be used to differentiate
 * between debug and production assert handler definitions.
 * 
 * 4) Thouroughly test assert handler definition under multiple failure conditions since this is the
 * last line of defence if your program fails (fault injection). I.e. Stack Overflow, misaligned access, 
 * preemption.
 * 
 * 5) Assert handler definition should handle BOTH hardware (NMI faults) and software bugs. An example 
 * definition could be resetting the Stack Pointer, performing Application-specific shutdown behavior, 
 * and resetting the CPU. For Unit Tests this function definition could be a Mock where it is verified 
 * this is called under the appropriate conditions (fault injection testing).
 *
 * @version 0.1
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef ASSERTER_H_
#define ASSERTER_H_



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- ASSERT MACROS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#if defined(__cplusplus) && (__cplusplus >= 201103L)

    /**
     * @brief Expands to C++11 and greater static_assert() macro which is natively 
     * supported in this standard. Do not use this for run-time asserts.
     * 
     * @param check_ Assertion to check. If this is True the assertion passes. If this 
     * is False the assertion fails and triggers a compilation error. This expression must
     * be a constant expression that can be evaluated at compile-time.
     */
    #define COMPILETIME_ASSERT(check_)              static_assert((check_), "COMPILETIME_ASSERT macro fired!")
#elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ == 201112L)
    #include <assert.h>

    /**
     * @brief Expands to C11 and greater _Static_assert() macro which is defined in
     * the assert.h header file in this standard. Do not use this for run-time asserts.
     * 
     * @param check_ Assertion to check. If this is True the assertion passes. If this 
     * is False the assertion fails and triggers a compilation error. This expression must
     * be a constant expression that can be evaluated at compile-time.
     */
    #define COMPILETIME_ASSERT(check_)              _Static_assert((check_), "COMPILETIME_ASSERT macro fired!")

#elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)

    /**
     * @brief Expands to C23 and greater static_assert() macro which is natively
     * supported in this standard. Do not use this for run-time asserts.
     * 
     * @param check_ Assertion to check. If this is True the assertion passes. If this 
     * is False the assertion fails and triggers a compilation error. This expression must
     * be a constant expression that can be evaluated at compile-time.
     */
    #define COMPILETIME_ASSERT(check_)              static_assert((check_), "COMPILETIME_ASSERT macro fired!")
#else
    extern char compiletime_assert[1];

    /**
     * @brief Using C or C++ standard that does not support static_assert or any of
     * its variants. Produces a compilation error by triggering a symbol redeclaration
     * error if the Assertion fires. I.e. extern char compiletime_assert[1] and
     * extern char compiletime_assert[2] will both be declared, causing a compilation error.
     * Do not use this for run-time asserts.
     * 
     * @param check_ Assertion to check. If this is True the assertion passes. If this
     * if False the assertion fails and triggers a compilation error. This expression must 
     * be able to be evaluated at compile-time.
     */
    #define COMPILETIME_ASSERT(check_)              extern char compiletime_assert[((check_) ? 1 : 2)]
#endif


/**
 * @brief Executes a user-defined assert handler callback function if the assert
 * fires. This callback function should be overwritten by the Application if this
 * module is used. Otherwise the default definition is used which is an infinite
 * while loop. Do not use this for compile-time asserts.
 * 
 * @param check_ Assertion to check. This must be a run-time expression. If this 
 * is True the assertion passes. If this is False the assertion fails and the 
 * user-defined assert handler executes.
 */
#define RUNTIME_ASSERT(check_)                      ((check_) ? ((void)0) : (*My_Assert_Handler)())



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ COMPILER-SPECIFIC ATTRIBUTES APPLIED TO ASSERT HANDLER FUNCTION ----------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#if defined(__cplusplus) && (__cplusplus >= 201103L)

    /**
     * @brief Assigned to Assert Handler so the Compiler knows this function never returns.
     * Using C++11 and greater so expands to [[noreturn]] attribute which is natively supported
     * in this Standard.
     */
    #define ATTRIBUTE_NORETURN_                 [[noreturn]]

#elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)

    /**
     * @brief Assigned to Assert Handler so the Compiler knows this function never returns.
     * Using C23 and greater so expands to [[noreturn]] attribute which is natively supported
     * in this Standard.
     */
    #define ATTRIBUTE_NORETURN_                 [[noreturn]]

#elif defined(__GNUC__) && ( (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5) )

    /**
     * @brief Assigned to Assert Handler so the Compiler knows this function never returns. 
     * Using GCC v2.5 and up so expands to GCC noreturn attribute.
     */
    #define ATTRIBUTE_NORETURN_                 __attribute__((noreturn))
#else
    #error "Noreturn and weak function attributes not defined. Currently support GCC v2.5 and greater."
#endif


/**
 * @brief User MUST assign this pointer to a function. See file description for more details. 
 * Points to user-defined assert handler function that executes if RUNTIME_ASSERT() macro fires.
 */
extern void (*My_Assert_Handler)(void) ATTRIBUTE_NORETURN_;


#endif /* ASSERTER_H_ */
