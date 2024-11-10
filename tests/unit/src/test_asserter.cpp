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

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/
#warning "TODO: Uncomment files once done."
/* Files under test. */
#include "ecu/asserter.h"
// #include "ecu/dlist.h"
#include "ecu/event.h"
#include "ecu/fsm.h"
#include "ecu/hsm.h"
// #include "ecu/timer.h"
#include "ecu/tree.h"

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

    catch (AssertException& e)
    {
        (void)e;
    }
}

// /**
//  * @brief Verify file name is correct when assert fires
//  * in circular_dll.c
//  */
// TEST(Asserter, ECUCircularDLLAssert)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().expectOneCall("assert_handler").withParameter("p1", "ecu/circular_dll.c");

//         /* Steps 2 and 3: Action and assert. */
//         ecu_circular_dll_ctor((struct ecu_circular_dll *)0);
//     }

//     catch (AssertException& e)
//     {
//         (void)e;
//     }
// }

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
        ecu_event_ctor((struct ecu_event *)0, 0);
    }

    catch (AssertException& e)
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
        ecu_fsm_ctor((struct ecu_fsm *)0, (const struct ecu_fsm_state *)0);
    }

    catch (AssertException& e)
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
    /* Placeholder until HSM module is complete. */
}

// /**
//  * @brief Verify file name is correct when assert fires
//  * in timer.c
//  */
// TEST(Asserter, ECUTimerAssert)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().expectOneCall("assert_handler").withParameter("p1", "ecu/timer.c");

//         /* Steps 2 and 3: Action and assert. */
//         ecu_timer_ctor((struct ecu_timer *)0, (void *)0, (bool (*)(void *))0);
//     }

//     catch (AssertException& e)
//     {
//         (void)e;
//     }
// }

/**
 * @brief Verify file name is correct when assert fires
 * in tree.c
 */
TEST(Asserter, ECUTreeAssert)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("assert_handler").withParameter("p1", "ecu/tree.c");

        /* Steps 2 and 3: Action and assert. */
        ecu_tree_node_ctor((struct ecu_tree_node *)0, (void (*)(struct ecu_tree_node *))0, 0);
    }

    catch (AssertException& e)
    {
        (void)e;
    }
}
