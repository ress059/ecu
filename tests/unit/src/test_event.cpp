/**
 * @file
 * @brief Unit tests for public API functions in @ref event.h. 
 * Test Summary:
 * 
 * @ref ecu_reserved_event_ids
 *      - TEST(Event, UserEventIDBegin)
 * 
 * @ref ECU_EVENT_IS_BASE_OF()
 *      - TEST(Event, IsBaseOfMacro)
 * 
 * @ref ecu_event_ctor()
 *      - TEST(Event, EventCtorValidID)
 *      - TEST(Event, EventCtorInvalidID)
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
    enum test_event_ids
    {
        INVALID_EVENT_ID = ECU_VALID_EVENT_ID_BEGIN - 1,
        VALID_EVENT_ID = ECU_USER_EVENT_ID_BEGIN
    };

    struct valid_event
    {
        ecu_event base;
        int a;
        int b;
    };

    struct invalid_event
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

    ecu_event m_event;
};

/*------------------------------------------------------------*/
/*----------------------------- TESTS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief @ref ECU_USER_EVENT_ID_BEGIN must be 0.
 */
TEST(Event, UserEventIDBegin)
{
    /* Step 3: Assert. Verify ECU_USER_EVENT_ID_BEGIN is 0. */
    ENUMS_EQUAL_INT(0, ECU_USER_EVENT_ID_BEGIN);
}

/**
 * @brief Only verifies C-style inhertiance. C++ inheritance not
 * supported since type traits can be used instead.
 */
TEST(Event, IsBaseOfMacro)
{
    /* Step 3: Assert. */
    CHECK_TRUE( (ECU_EVENT_IS_BASE_OF(base, valid_event)) );
    CHECK_FALSE( (ECU_EVENT_IS_BASE_OF(base, invalid_event)) );
}

TEST(Event, EventCtorValidID)
{
    try
    {
        /* Step 1: Arrange. */
        m_event.id = INVALID_EVENT_ID;

        /* Step 2: Action. */
        ecu_event_ctor(&m_event, VALID_EVENT_ID);

        /* Step 3: Assert. Verify valid ID was assigned to event. */
        ENUMS_EQUAL_INT(VALID_EVENT_ID, m_event.id);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Event's ID should not be set to the invalid
 * ID value.
 */
TEST(Event, EventCtorInvalidID)
{
    try
    {
        /* Step 1: Arrange. */
        m_event.id = VALID_EVENT_ID;
        set_assert_handler(AssertResponse::OK);

        /* Step 2: Action. */
        ecu_event_ctor(&m_event, INVALID_EVENT_ID);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }

    /* Step 3: Assert. Verify invalid ID was not assigned to event. */
    ENUMS_EQUAL_INT(VALID_EVENT_ID, m_event.id);
}
