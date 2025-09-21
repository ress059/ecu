/**
 * @file
 * @brief See @ref stub_asserter.hpp
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-10-12
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "stubs/stub_asserter.hpp"

/* STDLib. */
#include <cassert>

/* Define ECU assert handler. */
#include "ecu/asserter.h"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"

/*------------------------------------------------------------*/
/*------------------------- NAMESPACES -----------------------*/
/*------------------------------------------------------------*/

using namespace stubs;

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

static void assert_ok(const char *file, int line);
static void assert_fail(const char *file, int line);

/*------------------------------------------------------------*/
/*--------------------- STATIC VARIABLES ---------------------*/
/*------------------------------------------------------------*/

static void (*current_handler)(const char *file, int line) = &assert_fail;

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void assert_expected(const char *file, int line)
{
    (void)file;
    (void)line;
    mock().actualCall("expected_assertion");
    throw AssertException();
}

static void assert_ok(const char *file, int line)
{
    (void)file;
    (void)line;
    throw AssertException();
}

static void assert_fail(const char *file, int line)
{
    (void)file;
    (void)line;
    mock().actualCall("assert_fired_during_test");
    throw AssertException();
}

/*------------------------------------------------------------*/
/*----------------------- FREE FUNCTIONS ---------------------*/
/*------------------------------------------------------------*/

void stubs::set_assert_handler(AssertResponse response)
{
    switch (response)
    {
        case AssertResponse::OK:
        {
            current_handler = &assert_ok;
            break;
        }

        case AssertResponse::FAIL:
        {
            current_handler = &assert_fail;
            break;
        }

        default:
        {
            assert(false);
            break;
        }
    }
}

void stubs::set_assert_handler(void (*handler)(const char *file, int line))
{
    assert(handler);
    current_handler = handler;
}

void stubs::EXPECT_ASSERTION()
{
    current_handler = &assert_expected;
    mock().expectOneCall("expected_assertion");
}

void ecu_assert_handler(const char *file, int line)
{
    /* Define system response under ECU assert condition. Calls 
    the settable handler. */
    (*current_handler)(file, line);
}
