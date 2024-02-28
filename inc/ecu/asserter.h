#warning "TODO: Update file description when run-time asserter is made"
/**
 * @file
 * @brief Compile-time and run-time assert macros.
 * @details Compile-time assert macros uses static_assert variants or symbol redeclaration to produce
 * compilation errors depending on which C/C++ standard is used when compiling.
 * @author Ian Ress
 * 
 * assumptions: for non-safety critical devices. Assert is due to run-time behavior, not a hard fault.
 * Therefore system is still in control and can use stack (function call overhead).
 * 
 */


#ifndef ASSERTER_H_
#define ASSERTER_H_


#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- COMPILE-TIME ASSERT --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @name Compile-time Assert
 * 
 * Macro expands to different variants depending on which C/C++ standard is used
 * when compiling. Serves as a common interface that the Application can use to
 * remain backwards compatible with various standards. Example calls:
 * 
 * @code{.c}
 * COMPILETIME_ASSERT(true); // Passes
 * COMPILETIME_ASSERT(5==5); // Passes
 * 
 * COMPILETIME_ASSERT(false); // Fails
 * COMPILETIME_ASSERT(sizeof(int) >= 2); // Passes
 * @endcode
 * 
 * A message is not passed to this macro in case the implementation allocated
 * memory for it.
 */
/**@{*/
#if defined(ECU_DOXYGEN_)

    /**
     * @brief Produce compilation error if assert fails.
     * 
     * @details Macro expands to different variants depending on which C/C++ standard is 
     * used when compiling. Serves as a common interface that the Application can use to
     * remain backwards compatible with various standards.
     * 
     * 1. Expands to this when compiling with C++11 and greater. Uses static_assert()
     * natively supported by this version.
     * 
     * @code{.c}
     * #define COMPILETIME_ASSERT(check)               static_assert((check_))
     * @endcode
     * 
     * 2. Expands to this when compiling with C11 and greater. Uses _Static_assert() 
     * which is defined in the assert.h header file. Therefore assert.h is also 
     * included in this case.
     * 
     * @code{.c}
     * #include <assert.h>
     * #define COMPILETIME_ASSERT(check_)              _Static_assert((check_))
     * @endcode
     * 
     * 3. Expands to this when compiling with C23 and greater. Uses static_assert() 
     * which is natively supported in this standard.
     * 
     * @code{.c}
     * #define COMPILETIME_ASSERT(check_)              static_assert((check_))
     * @endcode
     * 
     * 4. Expands to this when when using C/C++ standard that does not support
     * static assertions. Macro redeclares symbol to extern char compiletime_assert_[2]
     * if the assertion fails, causing a symbol redeclaration error. This allows the
     * assertion to also be used outisde of functions.
     * 
     * @code{.c}
     * extern char compiletime_assert_[1];
     * #define COMPILETIME_ASSERT(check_)              extern char compiletime_assert[((check_) ? 1 : 2)]
     * @endcode
     * 
     * @param check_ Condition to check. If this is true the assertion passes.
     * If this is false the assertion fails and triggers a compilation error.
     * This must be a literal expression that can be evaluated at compile-time.
     */
    #define COMPILETIME_ASSERT(check_)
// /**@}*/ /* Compile-time Assert */
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
        #define COMPILETIME_ASSERT(check)               static_assert((check_))
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
        #define COMPILETIME_ASSERT(check_)              _Static_assert((check_))
    #elif !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)

        /**
         * @brief Produce compilation error if assert fails. C23 and greater
         * static_assert() which is natively supported.
         * 
         * @param check_ Condition to check. If this is true the assertion passes.
         * If this is false the assertion fails and triggers a compilation error.
         * This must be a literal expression that can be evaluated at compile-time.
         */
        #define COMPILETIME_ASSERT(check_)              static_assert((check_))
    #else
        extern char compiletime_assert[1];

        /**
         * @brief Produce compilation error if assert fails. Using C/C++ standard that
         * does not support static assertions. Macro redeclares symbol to 
         * extern char compiletime_assert_[2] if the assertion fails, causing a symbol 
         * redeclaration error. Allows macro to still be used outside of functions.
         * 
         * @param check_ Condition to check. If this is true the assertion passes.
         * If this is false the assertion fails and triggers a compilation error.
         * This must be a literal expression that can be evaluated at compile-time.
         */
        #define COMPILETIME_ASSERT(check_)              extern char compiletime_assert[((check_) ? 1 : 2)]
    #endif
#endif /* ECU_DOXYGEN_ */



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- RUNTIME ASSERT ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct assert_functor
{
    void (*handler)(struct assert_functor *object, const char *file_name, uint32_t line);
    void *data;
};

/**
 * @name Runtime Assert
 */
/**@{*/
/**
 * @brief Executes a user-defined assert handler callback function if the assert
 * fires. 
 * 
 * @details This callback function should be overwritten by the Application if this
 * module is used. Otherwise the default definition is used which is an infinite
 * while loop. Do not use this for compile-time asserts.
 * 
 * @param check_ Condition to check. If this is true the assertion passes.
 * If this is false the assertion fails and triggers a compilation error.
 * This must be a literal expression that can be evaluated at compile-time.
 */
// #if defined(ECU_RUNTIME_ASSERT)
#define RUNTIME_ASSERT(check_)                      ((check_) ? ((void)0) : (*My_Assert_Handler)())

// #else
// #define RUNTIME_ASSERT(check_) ((void)0)
// #endif


/**
 * @brief User MUST assign this pointer to a function. See file description for more details. 
 * Points to user-defined assert handler function that executes if RUNTIME_ASSERT() macro fires.
 */
extern void (*My_Assert_Handler)(void);


extern void assert_handler(struct assert_functor *object, const char *file_name, uint32_t line);
/**@}*/ /* Run-time Assert */


#endif /* ASSERTER_H_ */
