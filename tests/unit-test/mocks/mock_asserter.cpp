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

/* STDLib. */
#include <string>

/* CppUTest */
#include <CppUTestExt/MockSupport.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- STATIC FUNCTION DEFINITIONS -----------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

template<typename T>
requires std::is_same_v<std::remove_cvref<T>, std::string>
static std::string& ECUMockAsserterObject::extract_base_file_name(T&& file)
{
    size_t last_slash = file.find_last_of("/\\"); /* '/' or '\' to remain OS-agnostic. */

    if (last_slash != std::string::npos) 
    {
        return file.substr(last_slash + 1);
    }

    /* Base file name already supplied. */
    return file;
}



void ecu_assert_do_not_use(const char *file, int line)
{
    static ECUMockAsserterObject obj;
    obj = ECUMockAsserterObject(file);

    mock()
        .actualCall(ECU_ASSERT_HANDLER_NAME)
        .withParameterOfType("ECUMockAsserterObject", "base_file_name_", &obj); /* Do not pass in line parameter since it is much more prone to false test failures. */

    /* Throw an exception so we immediately return to the caller (the unit test) when an
    assert fires. This prevents source code from running under an assert condition. */
    throw ECUMockAsserterException(obj.base_file_name_, line);



    // can i construct a class in here? and use that as a virtual interface?
}
