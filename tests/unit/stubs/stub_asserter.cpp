/**
 * @file
 * @brief See @ref stub_asserter.hpp
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-10-12
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include "stubs/stub_asserter.hpp"

/* Define ECU assert handler. */
#include "ecu/asserter.h"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DECLARATIONS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void assert_ok(const char *file, int line);
static void assert_fail(const char *file, int line);



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE SCOPE VARIABLES -----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

static void (*current_handler)(const char *file, int line) = (void (*)(const char *, int)) = &assert_fail;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void assert_ok(const char *file, int line)
{
    (void)file;
    (void)line;
    throw stubs::AssertException();
}


static void assert_fail(const char *file, int line)
{
    (void)file;
    (void)line;
    mock().actualCall("assert_fired_during_test");
    throw stubs::AssertException();
}



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void stubs::set_assert_handler(assert_response response)
{
    switch (response)
    {
        case stubs::assert_response::OK:
        {
            current_handler = &assert_ok;
            break;
        }

        case stubs::assert_response::FAIL:
        {
            current_handler = &assert_fail;
            break;
        }

        default:
        {
            break;
        }
    }
}


void stubs::set_assert_handler(void (*handler)(const char *file, int line))
{
    if (handler)
    {
        current_handler = handler;
    }
}


void ecu_assert_handler(const char *file, int line)
{
    /* Define system response undef ECU assert condition. Calls 
    the settable handler. */
    (*current_handler)(file, line);
}
