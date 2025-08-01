/**
 * @file
 * @brief Unit tests for public API functions in @ref asserter.h. ECU_DISABLE_RUNTIME_ASSERTS
 * must NOT be defined.
 * Test Summary:
 * 
 * @ref ECU_ASSERT_DEFINE_NAME(), @ref ECU_RUNTIME_ASSERT(), @ref ecu_assert_handler()
 *      - TEST(Asserter, DefineNameMacro)
 *      - TEST(Asserter, ECUDListAssert)
 *      - TEST(Asserter, ECUEventAssert)
 *      - TEST(Asserter, ECUFsmAssert)
 *      - TEST(Asserter, ECUHsmAssert)
 *      - TEST(Asserter, ECUNtnodeAssert)
 *      - TEST(Asserter, ECUTimerAssert)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Files under test. */
#include "ecu/asserter.h"
#include "ecu/dlist.h"
#include "ecu/event.h"
#include "ecu/fsm.h"
#include "ecu/hsm.h"
#include "ecu/timer.h"
#include "ecu/ntnode.h"

/* Stubs. */
#include "stubs/stub_asserter.hpp"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*------------------------ NAMESPACES ------------------------*/
/*------------------------------------------------------------*/

using namespace stubs;

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTS ---------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("test_asserter.cpp")

/*------------------------------------------------------------*/
/*--------------- STATIC FUNCTION DECLARATIONS ---------------*/
/*------------------------------------------------------------*/

static void assert_handler(const char *file, int line);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void assert_handler(const char *file, int line)
{
    (void)line;
    mock().actualCall(__func__).withParameter("p1", file);
    throw AssertException();
}

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Asserter)
{
    void setup() override
    {
        set_assert_handler(&assert_handler);
    }

    void teardown() override
    {
        set_assert_handler(AssertResponse::FAIL);
        mock().checkExpectations();
        mock().clear();
    }
};

/*------------------------------------------------------------*/
/*----------------------------- TESTS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify @ref ECU_ASSERT_DEFINE_NAME() macro
 * works correctly in this module.
 */
TEST(Asserter, DefineNameMacro)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "test_asserter.cpp");

        /* Steps 2 and 3: Action and assert. */
        ECU_RUNTIME_ASSERT( (false) );
    }
    catch (const AssertException& e)
    {
        (void)e;
    }
}

/**
 * @brief Verify file name is correct when assert fires
 * in dlist.c
 */
TEST(Asserter, ECUDListAssert)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "ecu/dlist.c");

        /* Steps 2 and 3: Action and assert. */
        ecu_dlist_ctor(nullptr);
    }
    catch (const AssertException& e)
    {
        (void)e;
    }
}

/**
 * @brief Verify file name is correct when assert fires
 * in event.c
 */
TEST(Asserter, ECUEventAssert)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "ecu/event.c");

        /* Steps 2 and 3: Action and assert. */
        ecu_event_ctor(nullptr, 0);
    }
    catch (const AssertException& e)
    {
        (void)e;
    }
}

/**
 * @brief Verify file name is correct when assert fires
 * in fsm.c
 */
TEST(Asserter, ECUFsmAssert)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "ecu/fsm.c");

        /* Steps 2 and 3: Action and assert. */
        ecu_fsm_ctor(nullptr, nullptr);
    }
    catch (const AssertException& e)
    {
        (void)e;
    }
}

/**
 * @brief Verify file name is correct when assert fires
 * in hsm.c
 */
TEST(Asserter, ECUHsmAssert)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "ecu/hsm.c");

        /* Steps 2 and 3: Action and assert. */
        ecu_hsm_ctor(nullptr, nullptr, nullptr, 0);
    }
    catch (const AssertException& e)
    {
        (void)e;
    }
}

/**
 * @brief Verify file name is correct when assert fires
 * in ntnode.c
 */
TEST(Asserter, ECUNtnodeAssert)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "ecu/ntnode.c");

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_ctor(nullptr, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    }
    catch (const AssertException& e)
    {
        (void)e;
    }
}

/**
 * @brief Verify file name is correct when assert fires
 * in timer.c
 */
TEST(Asserter, ECUTimerAssert)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "ecu/timer.c");

        /* Steps 2 and 3: Action and assert. */
        ecu_timer_ctor(reinterpret_cast<ecu_timer *>(0), nullptr, nullptr);
    }
    catch (const AssertException& e)
    {
        (void)e;
    }
}
