/**
 * @file
 * @brief Use when ECU library function calls under test could cause an
 * assert to fire. This module provides an assert mock and stub that can
 * be set by calling the relevant ecu_set_assert_functor() function. Both
 * classes throw an exception so control immediately returns to the caller 
 * (the unit test). This prevents library code from running under an assert 
 * condition. @ref AssertCallOk should be used if it is expected for an assert
 * to fire. @ref AssertCallFail should be used if you are not excepting an
 * assert. This calls a mock which will cause the test to fail due to an 
 * unexpected call.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-05-08
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Files under test. */
#include <ecu/asserter.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STUB AND MOCK DECLARATIONS --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

class AssertException
{
};


/**
 * @brief Use when ECU library function calls within the unit test could 
 * cause assert to fire but you wish to ignore it. This throws an exception
 * so control immediately returns to the caller (the unit test). This 
 * prevents library code from running under an assert condition, leading to
 * undefined behavior.
 */
struct AssertCallOk : public ecu_assert_functor 
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)me;
        (void)file;
        (void)line;
        throw AssertException();
    }
};


/**
 * @brief Use when ECU library function calls within the unit test should
 * never fire an assert. Calls a mock which will cause the test to fail
 * due to an unexpected call. Also throws an exception so control immediately
 * returns to the caller (the unit test). This prevents library code from 
 * running under an assert condition, leading to undefined behavior.
 */
struct AssertCallFail : public ecu_assert_functor 
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)me;
        (void)file;
        (void)line;
        mock().actualCall("AssertMock::assert_handler");
        throw AssertException();
    }
};
