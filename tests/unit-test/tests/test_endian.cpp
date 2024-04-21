/**
 * @file
 * @brief Unit tests for public API functions in @ref endian.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-13
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <cstdint>

/* Files under test. */
#include <ecu/endian.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>




/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(Endian)
{
    uint16_t u16_runtime_swap_;
    uint32_t u32_runtime_swap_;
    uint64_t u64_runtime_swap_;
    static constexpr uint16_t U16_COMPILETIME_SWAP_ = ECU_SWAP16_COMPILETIME(0x1234);
    static constexpr uint32_t U32_COMPILETIME_SWAP_ = ECU_SWAP32_COMPILETIME(0x12345678);
    static constexpr uint64_t U64_COMPILETIME_SWAP_ = ECU_SWAP64_COMPILETIME(0x123456789ABCDEF0);
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- TESTS -------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST(Endian, SwappingMacros)
{
    /* Step 1: Arrange. */
    u16_runtime_swap_ = 0x1234;
    u32_runtime_swap_ = 0x12345678;
    u64_runtime_swap_ = 0x123456789ABCDEF0;

    u16_runtime_swap_ = ecu_swap16_runtime(u16_runtime_swap_);
    u32_runtime_swap_ = ecu_swap32_runtime(u32_runtime_swap_);
    u64_runtime_swap_ = ecu_swap64_runtime(u64_runtime_swap_);

    /* Steps 2 and 3: Action and assert. */
    LONGS_EQUAL(0x3412, U16_COMPILETIME_SWAP_);
    LONGS_EQUAL(0x3412, u16_runtime_swap_);
    UNSIGNED_LONGS_EQUAL(0x3412, U16_COMPILETIME_SWAP_);
    UNSIGNED_LONGS_EQUAL(0x3412, u16_runtime_swap_);

    LONGS_EQUAL(0x78563412, U32_COMPILETIME_SWAP_);
    LONGS_EQUAL(0x78563412, u32_runtime_swap_);
    UNSIGNED_LONGS_EQUAL(0x78563412, U32_COMPILETIME_SWAP_);
    UNSIGNED_LONGS_EQUAL(0x78563412, u32_runtime_swap_);

    LONGSLONGS_EQUAL(0xF0DEBC9A78563412, U64_COMPILETIME_SWAP_);
    LONGSLONGS_EQUAL(0xF0DEBC9A78563412, u64_runtime_swap_);
    UNSIGNED_LONGLONGS_EQUAL(0xF0DEBC9A78563412, U64_COMPILETIME_SWAP_);
    UNSIGNED_LONGLONGS_EQUAL(0xF0DEBC9A78563412, u64_runtime_swap_);
}
