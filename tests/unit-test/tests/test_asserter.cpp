/**
 * @file
 * @brief Unit tests for public API functions in @ref asserter.h. ECU_DISABLE_RUNTIME_ASSERTS
 * must NOT be defined.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */


/* Files under test. */
#include <ecu/asserter.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ STATIC FUNCTION DECLARATIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Returns the base file name from the supplied string.
 * Uses '/' and '\' as delimeters.
 */
static std::string extract_base_file_name(const char *path);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static std::string extract_base_file_name(const char *path)
{
    std::string file(path);
    return file.substr(file.find_last_of("/\\") + 1); /* '/' or '\' to remain OS-agnostic. */
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- MOCKS -------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Verify functor was called. Do not verify any parameters.
 */
struct AssertMockNoParams : public ecu_assert_functor
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)file;
        (void)line;

        mock()
            .actualCall("AssertMockNoParams::assert_handler")
            .onObject(me);
    }
};


/**
 * @brief Verify functor was called and correct file name was passed. Base
 * file name is always extracted in case full paths are passed. Purposefully
 * do not verify line parameter to avoid false negatives.
 */
struct AssertMockWithParams : public ecu_assert_functor 
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)line;

        /* Has to outlive function call in order for CppuTest mock to use string. */
        static std::string base_file_name;

        if (file)
        {
            base_file_name = extract_base_file_name(file);
        }
        else
        {
            base_file_name = std::string("ERROR: NULL FILE SUPPLIED");
        }

        mock()
            .actualCall("AssertMockWithParams::assert_handler")
            .onObject(me)
            .withParameter("file", base_file_name.c_str());
    }
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(AssertFunctors)
{
    virtual void setup() override 
    {
        assert_mock_no_params_1_.handler = &AssertMockNoParams::assert_handler;
        assert_mock_no_params_2_.handler = &AssertMockNoParams::assert_handler;
        assert_mock_with_params_1_.handler = &AssertMockWithParams::assert_handler;
        assert_mock_with_params_2_.handler = &AssertMockWithParams::assert_handler;
    }

    virtual void teardown() override 
    {
        mock().checkExpectations();
        mock().clear();
    }

    AssertMockNoParams assert_mock_no_params_1_;
    AssertMockNoParams assert_mock_no_params_2_;
    AssertMockWithParams assert_mock_with_params_1_;
    AssertMockWithParams assert_mock_with_params_2_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------- TESTS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST(AssertFunctors, OnlyMockHandler1Called)
{
    /* Step 1: Arrange. */
    mock()
        .expectOneCall("AssertMockNoParams::assert_handler")
        .onObject(static_cast<ecu_assert_functor *>(&assert_mock_no_params_1_));

    /* Steps 2 and 3: Action and assert. */
    ECU_RUNTIME_ASSERT( false, static_cast<ecu_assert_functor *>(&assert_mock_no_params_1_) );
}


TEST(AssertFunctors, OnlyMockHandler2Called)
{
    /* Step 1: Arrange. */
    mock()
        .expectOneCall("AssertMockNoParams::assert_handler")
        .onObject(static_cast<ecu_assert_functor *>(&assert_mock_no_params_2_));

    /* Steps 2 and 3: Action and assert. */
    ECU_RUNTIME_ASSERT( false, static_cast<ecu_assert_functor *>(&assert_mock_no_params_2_) );
}


TEST(AssertFunctors, CorrectFileName)
{
    /* Step 1: Arrange. */
    mock()
        .expectOneCall("AssertMockWithParams::assert_handler")
        .onObject(static_cast<ecu_assert_functor *>(&assert_mock_with_params_1_))
        .withParameter("file", "test_asserter.cpp");

    mock()
        .expectOneCall("AssertMockWithParams::assert_handler")
        .onObject(static_cast<ecu_assert_functor *>(&assert_mock_with_params_2_))
        .withParameter("file", "test_asserter.cpp");

    /* Steps 2 and 3: Action and assert. */
    ECU_RUNTIME_ASSERT( false, static_cast<ecu_assert_functor *>(&assert_mock_with_params_1_) );
    ECU_RUNTIME_ASSERT( false, static_cast<ecu_assert_functor *>(&assert_mock_with_params_2_) );
}
