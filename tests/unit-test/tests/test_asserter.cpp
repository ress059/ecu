/**
 * @file
 * @author Ian Ress
 * @brief Unit tests for public API functions in @ref asserter.h. ECU_DISABLE_RUNTIME_ASSERTS
 * must NOT be defined.
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* Files under test. */
#include <ecu/asserter.h>

/* Mocks. */
#include <mocks/mock_asserter.hpp>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ FILE-SCOPE VARIABLES -----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static const char this_file_full_path[] = __FILE__;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(RuntimeAssert)
{
    virtual void setup() final override 
    {
        ecu_asserter_set_mock_expectation(&this_file_full_path[0]);
    }

    virtual void teardown() final override 
    {
        mock().checkExpectations();
        mock().clear();
    }
};



// TESTS
// 1. Call two macros with different functors. Verify correct functor executes.



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- TESTS - ECUMockAsserter Group ----------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Verifies handler is called with correct arguments when 
 * assert fires.
 */
TEST(RuntimeAssert, TestAssertMacroInThisFile)
{
    /* STEPS 2 AND 3: Action and Assert. */
    try 
    {
        ECU_RUNTIME_ASSERT(false);
    }
    catch (const ECUMockAsserterException& e)
    {
        (void)e;
    }
}
