/**
 * @file asserter.c
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


/* Translation unit. */
#include "asserter.h"


/**
 * @brief Default assert handler that is assigned if user does not specify one.
 * Executes when a runtime assert fires.
 */
static void ATTRIBUTE_NORETURN_ default_assert_handler(void)
{
    while(1)
    {
        /* Permanently hang to inspect call stack. */
    }
}

void (*My_Assert_Handler)(void) = &default_assert_handler;