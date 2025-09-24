/**
 * @file
 * @brief Unit tests for public API functions in @ref event.h. 
 * Test Summary:
 * 
 * @ref ECU_EVENT_CTOR()
 *      - TEST(Event, CompileTimeCtorCreateBaseEvent)
 *      - TEST(Event, CompileTimeCtorCreateDerivedEvent)
 * 
 * @ref ECU_EVENT_IS_BASE_OF()
 *      - TEST(Event, IsBaseOfEventCorrectlyInherited)
 *      - TEST(Event, IsBaseOfEventIncorrectlyInherited)
 * 
 * @ref ecu_event_ctor()
 *      - TEST(Event, RuntimeCtorEventIdIsEcuValidEventIdBegin)
 *      - TEST(Event, RuntimeCtorEventIdIsEcuUserEventIdBegin)
 *      - TEST(Event, RuntimeCtorEventIdIsUserDefinedAndValid)
 *      - TEST(Event, RuntimeCtorEventIdIsUserDefinedAndInvalid)
 * 
 * @author Ian Ress 
 * @version 0.1
 * @date 2024-11-10
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Files under test. */
#include "ecu/event.h"

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
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Event)
{
    /**
     * @brief User-defined event IDs.
     */
    enum test_event_ids
    {
        INVALID_EVENT_ID = ECU_VALID_EVENT_ID_BEGIN - 1,
        VALID_EVENT_ID_0 = ECU_USER_EVENT_ID_BEGIN,
        VALID_EVENT_ID_1,
        VALID_EVENT_ID_2
    };

    /**
     * @brief Correctly inherits base event class.
     * @warning Class must be trivially constructable in 
     * order for tests to produce valid results.
     */
    struct valid_derived_event
    {
        ecu_event base;
        int a;
        int b;
    };

    /**
     * @brief Incorrectly inherits base event class.
     * @warning Class must be trivially constructable in 
     * order for tests to produce valid results.
     */
    struct invalid_derived_event
    {
        int a;
        ecu_event base;
        int b;
    };

    void setup() override 
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override 
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/*------------------------------------------------------------*/
/*----------------- TESTS - ECU_EVENT_CTOR() -----------------*/
/*------------------------------------------------------------*/

/// @brief Test fails if compilation fails.
TEST(Event, CompileTimeCtorCreateBaseEvent)
{
    try
    {
        /* Steps 1 and 2: Arrange and action. */
        ecu_event event = ECU_EVENT_CTOR(
            VALID_EVENT_ID_1, ECU_EVENT_SIZE_UNUSED
        );

        static const ecu_event cevent = ECU_EVENT_CTOR(
            VALID_EVENT_ID_2, ECU_EVENT_SIZE_UNUSED
        );

        /* Step 3: Assert. */
        CHECK_TRUE( (ecu_event_id(&event) == VALID_EVENT_ID_1) );
        CHECK_TRUE( (ecu_event_id(&cevent) == VALID_EVENT_ID_2) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Test fails if compilation fails.
TEST(Event, CompileTimeCtorCreateDerivedEvent)
{
    try
    {
        /* Steps 1 and 2: Arrange and action. */
        valid_derived_event event = 
        {
            .base = ECU_EVENT_CTOR(VALID_EVENT_ID_1, ECU_EVENT_SIZE_UNUSED),
            .a = 1,
            .b = 1
        };

        static const valid_derived_event cevent =
        {
            .base = ECU_EVENT_CTOR(VALID_EVENT_ID_2, ECU_EVENT_SIZE_UNUSED),
            .a = 2,
            .b = 2
        };

        /* Step 3: Assert. */
        CHECK_TRUE( (ecu_event_id(ECU_EVENT_CONST_BASE_CAST(&event)) == VALID_EVENT_ID_1) );
        CHECK_TRUE( (ecu_event_id(ECU_EVENT_CONST_BASE_CAST(&cevent)) == VALID_EVENT_ID_2) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------- TESTS - ECU_EVENT_IS_BASE_OF() --------------*/
/*------------------------------------------------------------*/

/// @brief Macro should return true.
TEST(Event, IsBaseOfEventCorrectlyInherited)
{
    /* Steps 2 and 3: Action and assert. */
    CHECK_TRUE( (ECU_EVENT_IS_BASE_OF(base, valid_derived_event)) );
}

/// @brief Macro should return false.
TEST(Event, IsBaseOfEventIncorrectlyInherited)
{
    /* Steps 2 and 3: Action and assert. */
    CHECK_FALSE( (ECU_EVENT_IS_BASE_OF(base, invalid_derived_event)) );
}

/*------------------------------------------------------------*/
/*----------------- TESTS - ecu_event_ctor() -----------------*/
/*------------------------------------------------------------*/

/// @brief Event should be successfully constructed.
TEST(Event, RuntimeCtorEventIdIsEcuValidEventIdBegin)
{
    try
    {
        /* Step 1: Arrange. */
        valid_derived_event event;

        /* Step 2: Action. */
        ecu_event_ctor(ECU_EVENT_BASE_CAST(&event), ECU_VALID_EVENT_ID_BEGIN, ECU_EVENT_SIZE_UNUSED);

        /* Step 3: Assert. */
        CHECK_TRUE( (ecu_event_id(ECU_EVENT_CONST_BASE_CAST(&event)) == ECU_VALID_EVENT_ID_BEGIN) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Event should be successfully constructed.
TEST(Event, RuntimeCtorEventIdIsEcuUserEventIdBegin)
{
    try
    {
        /* Step 1: Arrange. */
        valid_derived_event event;

        /* Step 2: Action. */
        ecu_event_ctor(ECU_EVENT_BASE_CAST(&event), ECU_USER_EVENT_ID_BEGIN, ECU_EVENT_SIZE_UNUSED);

        /* Step 3: Assert. */
        CHECK_TRUE( (ecu_event_id(ECU_EVENT_CONST_BASE_CAST(&event)) == ECU_USER_EVENT_ID_BEGIN) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Event should be successfully constructed.
TEST(Event, RuntimeCtorEventIdIsUserDefinedAndValid)
{
    try
    {
        /* Step 1: Arrange. */
        valid_derived_event event;

        /* Step 2: Action. */
        ecu_event_ctor(ECU_EVENT_BASE_CAST(&event), VALID_EVENT_ID_2, ECU_EVENT_SIZE_UNUSED);

        /* Step 3: Assert. */
        CHECK_TRUE( (ecu_event_id(ECU_EVENT_CONST_BASE_CAST(&event)) == VALID_EVENT_ID_2) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Not allowed. Assertion should fire.
TEST(Event, RuntimeCtorEventIdIsUserDefinedAndInvalid)
{
    try
    {
        /* Step 1: Arrange. */
        valid_derived_event event;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_event_ctor(ECU_EVENT_BASE_CAST(&event), INVALID_EVENT_ID, ECU_EVENT_SIZE_UNUSED);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}
