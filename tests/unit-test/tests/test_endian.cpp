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
#include <bit>
#include <cstdint>

/* Files under test. */
#include <ecu/endian.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>




/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(EndianMacros)
{
    uint16_t u16_runtime_swap;
    uint32_t u32_runtime_swap;
    uint64_t u64_runtime_swap;
    static constexpr uint16_t U16_COMPILETIME_SWAP = ECU_SWAP16_COMPILETIME(0x1234);
    static constexpr uint32_t U32_COMPILETIME_SWAP = ECU_SWAP32_COMPILETIME(0x12345678);
    static constexpr uint64_t U64_COMPILETIME_SWAP = ECU_SWAP64_COMPILETIME(0x123456789ABCDEF0);
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- TESTS -------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST(EndianMacros, CompileTimeSwappingMacros)
{
    /* Step 1: Arrange. Already done. */
    /* Step 2: */
    static_cast<char *>
}




// test COMPILETIME16 SWAP
// test COMPILETIME32 SWAP
// test COMPILETIME64 SWAP
// test runtime16 swap
// test runtime32 swap
// test runtime64 swap

