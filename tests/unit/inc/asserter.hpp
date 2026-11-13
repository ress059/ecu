/**
 * @file
 * @brief Module that allows source code to be tested under an assertion condition. Behavior is
 * undefined if code keeps executing after an assertion. Therefore this module jumps back to the 
 * unit test when this occurs and offers 2 modes of operation:
 * 
 * 1. Default operation where an assertion is not expected to fire. If an assertion fires the test
 * fails and control immediately jumps back to the unit test. No extra code is required from the test. Ie:
 * @code{.cpp}
 * TEST(...)
 * {
 *      // Test fails. Assertion fires here but did not expect it. Prevent ecu_dnode_id() from continuing
 *      // to execute under an assertion condition by having control jump back to the test.
 *      ecu_dnode_id(NULL); 
 * }
 * @endcode
 * 
 * 2. Operation where an assertion is expected. An @ref asserter_exception is thrown and a mock
 * expectation is called when the assertion fires. The accompanying test only passes if the exception
 * is caught and the mock is expected. Ie:
 * @code{.cpp}
 * TEST(...)
 * {
 *      try
 *      {
 *          asserter::expect(); // Expect assertion here.
 * 
 *          // Assertion fires here so test passes. Prevent ecu_dnode_id() from continuing to execute under an 
 *          // assertion condition by having control jump back to the test in the catch clause.
 *          ecu_dnode_id(NULL); 
 *      }
 *      catch (const assert_expection& e)
 *      {
 *          (void)e;
 *      }
 * }
 * @endcode
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2026-11-12
 * @copyright Copyright (c) 2026
 */

#pragma once

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <exception>

/*------------------------------------------------------------*/
/*------------------------- ASSERTER -------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Class representing this module.
 */
class asserter
{
public:
    /// @brief Delete constructor. Keep functions wrapped in class for now in case
    /// private members are needed in the future.
    asserter() = delete;

    /// @brief Sets expectation that an assertion should fire. File where assertion fired
    /// does not matter.
    static void expect();

    /// @brief Sets expectation that an assertion should fire at the specified file.
    /// @param file File where assertion should fire.
    static void expect(const char *file);
};

/**
 * @brief Exception thrown when assertion fires.
 */
class asserter_exception : public std::exception
{
public:
    /// @brief Default constructor.
    asserter_exception() = default;

    const char* what() const noexcept override
    {
        return m_msg;
    }

private:
    static constexpr char m_msg[] = "Expected assertion fired!";
};
