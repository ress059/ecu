/**
 * @file
 * @brief Use when ECU library functions under test could cause an
 * assert to fire. ECU code should not continue running under an assert
 * condition as behavior will now be undefined. To combat this, these stubs 
 * throw an @ref AssertException, which allows control to jump back to the 
 * unit test if an assert fires. Example use:
 * 
 * @code{.cpp}
 * TEST(...)
 * {
 *     // Set stub to fail the test if an assert fires.
 *     set_assert_handler(assert_response::FAIL);
 * 
 *     try
 *     {
 *         test_some_ecu_code();
 *     }
 *     catch (const AssertException& e)
 *     {
 *         // Allows control to jump back here if test code in try 
 *         // block causes an ECU assert to fire.
 *         (void)e;
 *     }
 * }
 * @endcode
 * 
 * If an assert fires, it can be ignored or cause the unit test to fail
 * by using the @ref set_assert_handler() stubs in this module. This should
 * be called at the very beginning of your test. User can also set a custom
 * handler to execute when an assert fires also via @ref set_assert_handler().
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-10-12
 * @copyright Copyright (c) 2024
 */

#ifndef STUB_ASSERTER_HPP_
#define STUB_ASSERTER_HPP_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <cstdint>

/*------------------------------------------------------------*/
/*------------------------- NAMESPACES -----------------------*/
/*------------------------------------------------------------*/

namespace stubs {

/*------------------------------------------------------------*/
/*--------------------------- TYPES --------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Specifies how the unit test should respond if an assert
 * fires.
 */
enum class AssertResponse : std::uint8_t
{
    FAIL,   /**< Test will fail if an assert fires. */
    OK      /**< Test will not fail if an assert fires. */
};

/**
 * @brief Unit tests should catch this exception. This allows
 * control to jump back to the unit test if an assert fires.
 */
class AssertException
{
};

/*------------------------------------------------------------*/
/*----------------------- FREE FUNCTIONS ---------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Specify how the unit test should respond if an
 * assert fires.
 * 
 * @param response The unit test will fail if this is @ref AssertResponse::FAIL.
 * A mock is called which will be unhandled, thus failing the test. No mock
 * is called if this is @ref AssertResponse::OK. In both cases
 * an @ref AssertException is thrown so control jumps back to the unit test
 * if an assert fires.
 */
extern void set_assert_handler(AssertResponse response);

/**
 * @brief Set a custom handler to execute if an assert fires.
 * 
 * @param handler At a minimum this handler should throw an @ref AssertResponse
 * exception so control can jump back to the unit test.
 */
extern void set_assert_handler(void (*handler)(const char *file, int line));

} /* namespace stubs */

#endif /* STUB_ASSERTER_HPP_ */
