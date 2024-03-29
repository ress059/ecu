/**
 * @file mock_asserter.hpp
 * @author Ian Ress
 * @brief Provides mock classes to verify @ref ECU_ASSERT() fired. 
 * @details The tester should test the assert under a try-catch block since we throw
 * an exception. For example:
 * @code{.cpp}
 * // Verify assert fired via mock call verification.
 * ECU_MOCK_ASSERTER_EXPECT_NCALLS(1);
 * 
 * try
 * {
 *     ecu_fsm_ctor((struct ecu_fsm *)0, (ecu_fsm_func_ptr)0, 0); // Fires assert since NULL pointers supplied.
 * }
 * catch (const ECUMockAsserterException& e) 
 * {
 *     // File name where assert fired if needed. "/path/to/ecu/fsm.c" in this case.
 *     e.what(); 
 * 
 *     // Line number where assert fired if needed. 
 *     // Appropriate line in ecu_fsm.c -> ecu_fsm_ctor() function in this case.
 *     e.line_number(); 
 * }
 * @endcode
 * 
 * An exception is thrown to make control immediately return to the caller 
 * (the unit test). If this did not happen, the rest of the source code under 
 * test would execute under an assert condition, leading to unpredictable behavior 
 * and potential crashes. I.e. imagine a library function continuing to execute
 * when all NULL pointers are supplied.
 * 
 * When an assert fires the exception thrown is of type ECUMockAsserterException.
 * This allows the tester to see the file and line where the assert fired for 
 * further verification if needed. This also makes the exception as specific
 * as possible.
 * @version 0.1
 * @date 2024-03-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef ECU_MOCK_ASSERTER_HPP_
#define ECU_MOCK_ASSERTER_HPP_


/* STDLib. */
#include <string>
#include <type_traits>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- DEFINES -----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Unit tests should pass this parameter to mock().expectCall() 
 * and similar function variants to verify an assert handler executed.
 * This makes the change point limited to this variable as opposed to
 * all tests if the name ever changes.
 */
inline constexpr char ECU_MOCK_ASSERTER_HANDLER_NAME[] = "ecu_assert_do_not_use";


inline constexpr char ECU_MOCK_ASSERTER_TYPE_NAME[] = "ECUMockAsserterObject";



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- MOCK ASSERTER EXCEPTION CLASS ------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Class of this type is thrown in an exception when an assert fires. 
 * Unit tests should use this in a catch statement when verifying asserts.
 */
class ECUMockAsserterException final 
{
public:
    /**
     * @brief Constructed in our mock assert handler function that is
     * assigned to @ref ECU_RUNTIME_ASSERT(). Interfaces with C library 
     * so converts c-style string into std::string for file name.
     */
    ECUMockAsserterException(const std::string& file, int line) : file_(file), line_(line)
    {
    }

    ~ECUMockAsserterException() = default;
    ECUMockAsserterException(const ECUMockAsserterException& other) = default;
    ECUMockAsserterException& operator=(const ECUMockAsserterException& other) = default;
    ECUMockAsserterException(const ECUMockAsserterException&& other) = default;
    ECUMockAsserterException& operator=(const ECUMockAsserterException&& other) = default;

    /**
     * @brief Returns base file name where @ref ECU_ASSERT()
     * fired. E.g. "fsm.c"
     */
    const std::string& what() const noexcept 
    {
        return file_;
    }

    /**
     * @brief Returns line number where @ref ECU_ASSERT() fired.
     */
    int line_number() const noexcept 
    {
        return line_;
    }

private:
    const std::string file_;
    const int line_;
};



class ECUMockAsserterObject final
{
public:
    ECUAsserterMockObject(const char *file = "")
    {
        base_file_name_ = extract_base_file_name(std::string(file));
    }

    ~ECUAsserterMockObject() = default;

    ECUAsserterMockObject& operator=(const ECUAsserterMockObject& other)
    {
        this->base_file_name_ = other.base_file_name_; /* std::string copy assignment. */
    }

    ECUAsserterMockObject(const ECUAsserterMockObject& other) = delete;
    ECUAsserterMockObject(const ECUAsserterMockObject&& other) = delete;
    ECUAsserterMockObject& operator=(const ECUAsserterMockObject&& other) = delete;

    std::string base_file_name_;

private:
    /**
     * @brief Returns the base file name from the supplied string.
     * 
     * @param file Base type must be std::string. If this is already
     * a base file name a copy of this will be returned.
     */
    template<typename T>
    requires std::is_same_v<std::remove_cvref<T>, std::string>
    static std::string& extract_base_file_name(T&& file);
};



class ECUMockAsserterComparator final : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        bool is_equal = false;

        if (object1 && object2)
        {
            const ECUMockAsserterObject *obj1 = static_cast<const ECUMockAsserterObject *>(object1);
            const ECUMockAsserterObject *obj2 = static_cast<const ECUMockAsserterObject *>(object2);
            is_equal = !(obj1->base_file_name_.compare(obj2->base_file_name_)); /* 0 = equal. */
        }
        
        return is_equal;
    }

    virtual SimpleString valueToString(const void* object)
    {
        static const SimpleString invalidparams("ECUMockAsserterComparator::valueToString() failed to produce error message! NULL object supplied as parameter.");

        if (object)
        {
            const ECUMockAsserterObject *obj = static_cast<const ECUMockAsserterObject *>(object);
            return SimpleString(obj->base_file_name_.c_str()); /* SimpleString(const char *value = ""); */
        }

        return invalidparams;
    }
};


#endif /* ECU_MOCK_ASSERTER_HPP_ */
