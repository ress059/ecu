/**
 * @file
 * @brief See @ref asserter.hpp.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2026-11-12
 * @copyright Copyright (c) 2026
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit.*/
#include "inc/asserter.hpp"

/* STDLib. */
#include <cassert>

/* Define ECU assert handler. */
#include "ecu/asserter.h"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Calls mock expectation when assert fires but ignores file parameter.
 * 
 * @param file Unused.
 * @param obj Unused. 
 */
static void assert_no_file(const char *file, void *obj);

/**
 * @brief Calls mock expectation when assert fires, supplying file as a parameter.
 * 
 * @param file File where assertion fired.
 * @param obj Unused. 
 */
static void assert_with_file(const char *file, void *obj);

/*------------------------------------------------------------*/
/*--------------------- STATIC VARIABLES ---------------------*/
/*------------------------------------------------------------*/

/**@{*/
/**
 * @brief Defines behavior when an assertion fires.
 */
static void (*assert_func)(const char *, void *) = nullptr;
static void *assert_obj = nullptr;
/**@}*/

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

static void assert_no_file(const char *file, void *obj)
{
    (void)file;
    (void)obj;
    mock("assert").actualCall("ecu_assert_handler");
    throw asserter_exception(); /* Jump back to the unit test. */
}

static void assert_with_file(const char *file, void *obj)
{
    (void)obj;
    mock("assert").actualCall("ecu_assert_handler").withParameter("file", file);
    throw asserter_exception(); /* Jump back to the unit test. */
}

/*------------------------------------------------------------*/
/*------------------------- ASSERTER -------------------------*/
/*------------------------------------------------------------*/

void asserter::expect()
{
    UT_PTR_SET(assert_func, &assert_no_file);
    mock("assert").expectOneCall("ecu_assert_handler");
}

void asserter::expect(const char *file)
{
    UT_PTR_SET(assert_func, &assert_with_file);
    mock("assert").expectOneCall("ecu_assert_handler").withParameter("file", file);
}

void ecu_assert_handler(const char *file, int line)
{
    if (assert_func)
    {
        (*assert_func)(file, assert_obj);
        assert( (false) ); /* Control should never get back to here. Source code should never continue executing under an assertion condition. */
    }

    /* Control gets to here if no custom assert handler ran. Immediately fail the test. */
    char str[100] = {'\0'};
    std::snprintf(str, sizeof(str), "ECU_ASSERT() fired! File: %s, Line: %d", file, line);
    str[sizeof(str)-1] = '\0'; /* Ensure str is NULL terminated. */
    FAIL(str);

    while (1)
    {
        /* Hang forever since handler is declared with noreturn attribute. Control should never get here. */
        assert( (false) );
    }
}
