/**
 * @file
 * @brief Unit tests for public API functions in @ref dlist.h.
 * Test Summary:
 * 
 * @ref ECU_CONTAINER_OF(), @ref ECU_CONST_CONTAINER_OF()
 *      - TEST(Utils, ContainerOfPointerValue)
 *      - TEST(Utils, ContainerOfRead)
 *      - TEST(Utils, ContainerOfWrite)
 *      - TEST(Utils, ConstContainerOfPointerValue)
 *      - TEST(Utils, ConstContainerOf)
 * 
 * @ref ECU_FIELD_SIZEOF()
 *      - TEST(Utils, FieldSizeOf)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-14
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Files under test. */
#include "ecu/utils.h"

/* STDLib. */
#include <cstdint>

/* Stubs and helpers. */
#include "stubs/stub_asserter.hpp"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*------------------------- NAMESPACES -----------------------*/
/*------------------------------------------------------------*/

using namespace stubs;

/*------------------------------------------------------------*/
/*------------------------ TEST GROUPS -----------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Utils)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    /**
     * @brief Test type. Types chosen such that struct
     * padding in the middle of some members.
     */
    struct container_of_type
    {
        /// @brief Default constructor.
        container_of_type() : a(1), b(2), c(3), d(4)
        {

        }

        std::uint8_t a;
        std::uint16_t b;
        std::uint8_t c;
        std::uint32_t d;
    };
};

/*------------------------------------------------------------*/
/*--------------------- TESTS - CONTAINER_OF -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Convert intrusive member into container type.
 * Verifies returned pointer points to start of user's type.
 */
TEST(Utils, ContainerOfPointerValue)
{
    try
    {
        /* Step 1: Arrange. */
        container_of_type me;

        /* Step 2: Action. */
        void *intrusive_member = static_cast<void *>(&me.b);
        container_of_type *ptr = ECU_CONTAINER_OF(intrusive_member, container_of_type, b);

        /* Step 3: Assert. */
        POINTERS_EQUAL(&me, ptr);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Convert intrusive member into container type.
 * Verify container contents are correct.
 */
TEST(Utils, ContainerOfRead)
{
    try
    {
        /* Step 1: Arrange. */
        container_of_type me;
        me.b = 5;
        me.c = 10;

        /* Step 2: Action. */
        void *intrusive_member = static_cast<void *>(&me.b);
        container_of_type *ptr = ECU_CONTAINER_OF(intrusive_member, container_of_type, b);

        /* Step 3: Assert. */
        CHECK_TRUE( (ptr->b == me.b) );
        CHECK_TRUE( (ptr->c == me.c) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Convert intrusive member into container type.
 * Write values. Verify they are written correctly.
 */
TEST(Utils, ContainerOfWrite)
{
    try
    {
        /* Step 1: Arrange. */
        container_of_type me;

        /* Step 2: Action. */
        void *intrusive_member = static_cast<void *>(&me.b);
        container_of_type *ptr = ECU_CONTAINER_OF(intrusive_member, container_of_type, b);
        ptr->b = 5;
        ptr->c = 10;

        /* Step 3: Assert. */
        CHECK_TRUE( (me.b == 5) );
        CHECK_TRUE( (me.c == 10) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Convert intrusive member into container type.
 * Verifies returned pointer points to start of user's type.
 */
TEST(Utils, ConstContainerOfPointerValue)
{
    try
    {
        /* Step 1: Arrange. */
        container_of_type me;

        /* Step 2: Action. */
        const void *intrusive_member = static_cast<const void *>(&me.c);
        const container_of_type *ptr = ECU_CONST_CONTAINER_OF(intrusive_member, container_of_type, c);

        /* Step 3: Assert. */
        POINTERS_EQUAL(&me, ptr);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Convert intrusive member into container type.
 * Verify container contents are correct.
 */
TEST(Utils, ConstContainerOf)
{
    try
    {
        /* Step 1: Arrange. */
        container_of_type me;
        me.a = 5;
        me.d = 10;

        /* Step 2: Action. */
        const void *intrusive_member = static_cast<const void *>(&me.d);
        const container_of_type *ptr = ECU_CONST_CONTAINER_OF(intrusive_member, container_of_type, d);

        /* Step 3: Assert. */
        CHECK_TRUE( (ptr->a == me.a) );
        CHECK_TRUE( (ptr->d == me.d) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - FIELD_SIZEOF -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify correct size returned.
 */
TEST(Utils, FieldSizeOf)
{
    try
    {
        /* Step 1: Arrange. */
        struct field_sizeof_type
        {
            std::uint8_t a;
            std::uint32_t b;
        };

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ECU_FIELD_SIZEOF(field_sizeof_type, a) == sizeof(std::uint8_t)) );
        CHECK_TRUE( (ECU_FIELD_SIZEOF(field_sizeof_type, b) == sizeof(std::uint32_t)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}
