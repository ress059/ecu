/**
 * @file mock_asserter.cpp
 * @author Ian Ress
 * @brief See @ref mock_asserter.hpp description.
 * @version 0.1
 * @date 2024-03-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* Translation unit. */
#include <mocks/mock_asserter.hpp>

/* Files under test. */
#include <ecu/asserter.h>

/* CppUTest */
#include <CppUTestExt/MockSupport.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- STATIC FUNCTION DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void mock_asserter_default_handler(const char *file, int line);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------- ECUMOCKASSERTER CLASS MEMBER DEFINITIONS ------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

uint8_t ECUMockAsserter::num_instances = 0;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void mock_asserter_default_handler(const char *file, int line)
{
    /* Throw an exception so we immediately return to the caller (the unit test) when an
    assert fires. This prevents source code from running under an assert condition. */
    mock().actualCall("mock_asserter_default_handler");
    throw ECUMockAsserterException(file, line);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------- ECUMOCKASSERTER CLASS METHOD DEFINITIONS -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ECUMockAsserter::teardown()
{
    mock().clear();
}


ECUMockAsserter::ECUMockAsserter()
{
    if (ECUMockAsserter::num_instances == 0)
    {
        ecu_asserter_set_handler(&mock_asserter_default_handler);
    }
    
    ECUMockAsserter::num_instances++;
}


ECUMockAsserter::~ECUMockAsserter()
{
    if (--ECUMockAsserter::num_instances == 0)
    {
        ecu_asserter_set_handler((void (*)(const char *file, int line))0);
    }
}
