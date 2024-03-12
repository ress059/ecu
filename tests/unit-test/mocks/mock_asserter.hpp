/**
 * @file mock_asserter.hpp
 * @author Ian Ress
 * @brief Provides mock classes to verify @ref ECU_ASSERT() fired. 
 * @details The tester should test the assert under a try-catch block since we throw
 * an exception. For example:
 * @code{.cpp}
 * // Verify assert fired via mock call verification.
 * ECU_MOCK_ASSERTER_EXPECT_NCALLS(1);
 * 
 * try
 * {
 *     ecu_fsm_ctor((struct ecu_fsm *)0, (ecu_fsm_func_ptr)0, 0); // Fires assert since NULL pointers supplied.
 * }
 * catch (const ECUMockAsserterException& e) 
 * {
 *     // File name where assert fired if needed. "/path/to/ecu/fsm.c" in this case.
 *     e.what(); 
 * 
 *     // Line number where assert fired if needed. 
 *     // Appropriate line in ecu_fsm.c -> ecu_fsm_ctor() function in this case.
 *     e.line_number(); 
 * }
 * @endcode
 * 
 * An exception is thrown to make control immediately return to the caller 
 * (the unit test). If this did not happen, the rest of the source code under 
 * test would execute under an assert condition, leading to unpredictable behavior 
 * and potential crashes. I.e. imagine a library function continuing to execute
 * when all NULL pointers are supplied.
 * 
 * When an assert fires the exception thrown is of type ECUMockAsserterException.
 * This allows the tester to see the file and line where the assert fired for 
 * further verification if needed. This also makes the exception as specific
 * as possible.
 * @version 0.1
 * @date 2024-03-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef ECU_MOCK_ASSERTER_HPP_
#define ECU_MOCK_ASSERTER_HPP_


/* STDLib. */
#include <string>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppuTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------- MACROS -----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Helper macro to verify assert fired. This macro should be used 
 * within the unit tests in case the mock name passed to mock.expectNCalls() 
 * needs to be changed. The change point would be limited to this macro
 * as opposed to all tests.
 * 
 * @param n Number of function calls to expect. 
 */
#define ECU_MOCK_ASSERTER_EXPECT_NCALLS(n)           mock().expectNCalls((n), "mock_asserter_default_handler")



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- MOCK ASSERTER EXCEPTION CLASS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Class of this type is thrown in an exception when an assert fires. 
 * Unit tests should use this in a catch statement when verifying asserts.
 */
class ECUMockAsserterException final 
{
public:
    /**
     * @brief Constructed in our mock assert handler function that is
     * assigned to @ref ECU_RUNTIME_ASSERT(). Interfaces with C library 
     * so converts c-style string into std::string for file name.
     */
    ECUMockAsserterException(const char *cfile, int line) : file_(cfile), line_(line)
    {
    }

    ~ECUMockAsserterException() = default;
    ECUMockAsserterException(ECUMockAsserterException& other) = default;
    ECUMockAsserterException& operator=(ECUMockAsserterException& other) = default;
    ECUMockAsserterException(ECUMockAsserterException&& other) = default;
    ECUMockAsserterException& operator=(ECUMockAsserterException&& other) = default;

    /**
     * @brief Returns name of file (with full path) where @ref ECU_ASSERT()
     * fired. E.g. "path/to/ecu/fsm.c"
     */
    const std::string& what() const noexcept 
    {
        return file_;
    }

    /**
     * @brief Returns line number where @ref ECU_ASSERT() fired.
     */
    int line_number() const noexcept 
    {
        return line_;
    }

private:
    const std::string file_;
    const int line_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- MOCK ASSERTER BASE CLASS --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Use this class in a unit test whenever asserts need to be handled
 * or mocked. See @ref mock_asserter.hpp description.
 */
class ECUMockAsserter : public Utest
{
public:
    /**
     * @brief Clears the mock test condition.
     */
    virtual void teardown() final override;

protected:
    /**
     * @brief Set assert handler if this is the first class instance being constructed.
     * @details If this is the first class instance being constructed our mock assert
     * handler is assigned to @ref ECU_ASSERT() by calling @ref ecu_asserter_set_handler() 
     * library function. If there are already existing class instances this does not 
     * have to be done since the assigned handler will always be the same.
     */
    ECUMockAsserter();

    /**
     * @brief Clear assert handler if this is the last class instance being destroyed.
     * @details If this is the last class instance being destroyed (num_instances == 0)
     * then the assert handler ran in @ref ECU_ASSERT() is reset back to the default
     * by passing in NULL pointer to @ref ecu_asserter_set_handler() library function. This
     * approach allows multiple ECUMockAsserter class instances to be constructed/destroyed
     * independently of one another, while maintaining automatic cleanup functionality.
     */
    virtual ~ECUMockAsserter();
    ECUMockAsserter(ECUMockAsserter& other) = default;
    ECUMockAsserter& operator=(ECUMockAsserter& other) = default;
    ECUMockAsserter(ECUMockAsserter&& other) = default;
    ECUMockAsserter& operator=(ECUMockAsserter&& other) = default;

private:
    static uint8_t num_instances;
};


#endif /* ECU_MOCK_ASSERTER_HPP_ */
