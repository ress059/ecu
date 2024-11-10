/**
 * @file
 * @brief Unit tests for public API functions in @ref event.h. 
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

    void setup() override 
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override 
    {
        mock().checkExpectations();
        mock().clear();
    }

    struct ecu_event m_event;
};

/*------------------------------------------------------------*/
/*----------------------------- TESTS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief @ref ECU_USER_EVENT_ID_BEGIN must be 0.
 */
TEST(Event, UserEventIDBegin)
{
    /* Step 3: Assert. */
    ENUMS_EQUAL_INT(0, ECU_USER_EVENT_ID_BEGIN);
}

TEST(Event, EventCtorValidID)
{
    try
    {
        /* Step 1: Arrange. */
        m_event.id = INVALID_EVENT_ID;

        /* Step 2: Action. */
        ecu_event_ctor(&m_event, VALID_EVENT_ID);

        /* Step 3: Assert. */
        ENUMS_EQUAL_INT(VALID_EVENT_ID, m_event.id);
    }
    catch (AssertException& e)
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

        /* Step 2: Action. */
        ecu_event_ctor(&m_event, INVALID_EVENT_ID);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }

    /* Step 3: Assert ID wasn't changed. */
    ENUMS_EQUAL_INT(VALID_EVENT_ID, m_event.id);
}
