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

/* STDLib. */
#include <string>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ FILE-SCOPE VARIABLES -----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static const std::string this_file_full_path(__FILE__);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- STATIC FUNCTION DECLARATIONS -----------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static std::string extract_base_file_name(const std::string& full_path);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- STATIC FUNCTION DEFINITIONS -----------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static std::string extract_base_file_name(const std::string& full_path)
{
    std::string name("INVALID_PATH");
    size_t last_slash = full_path.find_last_of("/\\"); /* '/' or '\' to remain OS-agnostic. */

    if (last_slash != std::string::npos) 
    {
        name = full_path.substr(last_slash + 1);
    }

    return name;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP_BASE(RuntimeAssert, ECUMockAsserter)
{
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- TESTS - ECUMockAsserter Group ----------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Verifies handler is called when assert fires.
 */
TEST(RuntimeAssert, HandlerCall)
{
    /* STEP 1: Arrange. */
    ECU_MOCK_ASSERTER_EXPECT_NCALLS(1);

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


/**
 * @brief Verify file name passed to assert handler is correct.
 * Unit test so don't care about move semantics/optimizations for
 * string instances.
 */
TEST(RuntimeAssert, FileName)
{
    /* STEP 1: Arrange. */
    ECU_MOCK_ASSERTER_EXPECT_NCALLS(1);
    std::string this_file_name = extract_base_file_name(this_file_full_path);
    std::string assert_file_name; /* File name where assert fired. */

    /* STEP 2: Action. */
    try 
    {
        ECU_RUNTIME_ASSERT(false);
    }
    catch (const ECUMockAsserterException& e)
    {
        assert_file_name = extract_base_file_name(e.what());
    }

    /* STEP 3: Assert. */
    CHECK_TRUE( (this_file_name == assert_file_name) );
}
