/**
 * @file
 * @brief Unit tests for public API functions in @ref endian.h.
 * One of the tests (little endian or big endian) will have to 
 * swap bytes regardless of whatever computer this runs on.
 * Test Summary: 
 * 
 * @ref ECU_CPU_TO_LE16_COMPILETIME(), @ref ECU_CPU_TO_LE32_COMPILETIME(),
 * @ref ECU_CPU_TO_LE64_COMPILETIME(), @ref ECU_SWAP16_COMPILETIME(),
 * @ref ECU_SWAP32_COMPILETIME(), @ref ECU_SWAP64_COMPILETIME()
 *      - TEST(Endian, CPUToLittleEndianCompileTime)
 * 
 * @ref ECU_CPU_TO_LE16_RUNTIME(), @ref ECU_CPU_TO_LE32_RUNTIME(),
 * @ref ECU_CPU_TO_LE64_RUNTIME(), @ref ecu_swap16_runtime(),
 * @ref ecu_swap32_runtime(), @ref ecu_swap64_runtime()
 *      - TEST(Endian, CPUToLittleEndianRuntime)
 * 
 * @ref ECU_CPU_TO_BE16_COMPILETIME(), @ref ECU_CPU_TO_BE32_COMPILETIME(),
 * @ref ECU_CPU_TO_BE64_COMPILETIME(), @ref ECU_SWAP16_COMPILETIME(),
 * @ref ECU_SWAP32_COMPILETIME(), @ref ECU_SWAP64_COMPILETIME()
 *      - TEST(Endian, CPUToBigEndianCompileTime)
 * 
 * @ref ECU_CPU_TO_BE16_RUNTIME(), @ref ECU_CPU_TO_BE32_RUNTIME(),
 * @ref ECU_CPU_TO_BE64_RUNTIME(), @ref ecu_swap16_runtime(),
 * @ref ecu_swap32_runtime(), @ref ecu_swap64_runtime()
 *      - TEST(Endian, CPUToBigEndianRuntime)
 * 
 * @ref ECU_LE16_TO_CPU_COMPILETIME(), @ref ECU_LE32_TO_CPU_COMPILETIME(),
 * @ref ECU_LE64_TO_CPU_COMPILETIME(), @ref ECU_SWAP16_COMPILETIME(),
 * @ref ECU_SWAP32_COMPILETIME(), @ref ECU_SWAP64_COMPILETIME()
 *      - TEST(Endian, LittleEndianToCPUCompileTime)
 * 
 * @ref ECU_LE16_TO_CPU_RUNTIME(), @ref ECU_LE32_TO_CPU_RUNTIME(),
 * @ref ECU_LE64_TO_CPU_RUNTIME(), @ref ecu_swap16_runtime(),
 * @ref ecu_swap32_runtime(), @ref ecu_swap64_runtime()
 *      - TEST(Endian, LittleEndianToCPURunTime)
 * 
 * @ref ECU_BE16_TO_CPU_COMPILETIME(), @ref ECU_BE32_TO_CPU_COMPILETIME(),
 * @ref ECU_BE64_TO_CPU_COMPILETIME(), @ref ECU_SWAP16_COMPILETIME(),
 * @ref ECU_SWAP32_COMPILETIME(), @ref ECU_SWAP64_COMPILETIME()
 *      - TEST(Endian, BigEndianToCPUCompileTime)
 * 
 * @ref ECU_BE16_TO_CPU_RUNTIME(), @ref ECU_BE32_TO_CPU_RUNTIME(),
 * @ref ECU_BE64_TO_CPU_RUNTIME(), @ref ecu_swap16_runtime(),
 * @ref ecu_swap32_runtime(), @ref ecu_swap64_runtime()
 *      - TEST(Endian, BigEndianToCPURunTime)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-13
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <cassert>
#include <cstdint>
#include <type_traits>

/* Files under test. */
#include "ecu/endian.h"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Endian)
{
    /**
     * @brief Returns true if actual stores the expected
     * value in big endian. False if actual stores the
     * expected value in little endian or it stores a completely
     * different value.
     */
    template<typename T>
    requires std::is_integral_v<T>
    static bool is_big_endian(T expected, const T& actual)
    {
        assert( (expected != 0) ); /* Test produces pointless results if test value is 0. */
        bool status = true;
        std::uint8_t expected_byte = 0;
        std::uint8_t actual_byte = 0;

        for (std::size_t i = 0; i < sizeof(T); i++)
        {
            /* Calculate the byte to test for, starting at MSB. I.e. if
            value == 0x1234, first byte == 0x12, next 
            byte (next for-loop iteration) == 0x34, etc. */
            T mask = (0xFFULL << (8ULL * (sizeof(T) - 1 - i)));
            expected_byte = (std::uint8_t)((expected & mask) >> (8U * (sizeof(T) - 1 - i)));

            actual_byte = *((const std::uint8_t *)&actual + i);
            if (expected_byte != actual_byte)
            {
                status = false;
                break;
            }
        }

        return status;
    }

    /**
     * @brief Returns true if actual stores the expected
     * value in little endian. False if actual stores the
     * expected value in big endian or it stores a completely
     * different value.
     */
    template<typename T>
    requires std::is_integral_v<T>
    static bool is_little_endian(T expected, const T& actual)
    {
        assert( (expected != 0) ); /* Test produces pointless results if test value is 0. */

        bool status = true;
        std::uint8_t expected_byte = 0;
        std::uint8_t actual_byte = 0;

        for (std::size_t i = 0; i < sizeof(T); i++)
        {
            /* Calculate the byte to test for, starting at LSB. I.e. if
            value == 0x1234, first byte == 0x34, next 
            byte (next for-loop iteration) == 0x12, etc. */
            T mask = (0xFFULL << (8ULL * i));
            expected_byte = (std::uint8_t)((expected & mask) >> (8U * i));

            actual_byte = *((const std::uint8_t *)&actual + i);
            if (expected_byte != actual_byte)
            {
                status = false;
                break;
            }
        }

        return status;
    }

    /**
     * @brief Stores supplied value in big endian.
     */
    template<typename T>
    requires std::is_integral_v<T>
    static void store_in_big_endian(T value, T& storage)
    {
        assert( (value != 0) ); /* Test produces pointless results if test value is 0. */
        std::uint8_t byte = 0;

        for (std::size_t i = 0; i < sizeof(T); i++)
        {
            /* Calculate the next byte in value to use, starting at MSB. I.e. if
            value == 0x1234, first byte == 0x12, next 
            byte (next for-loop iteration) == 0x34, etc. */
            T mask = (0xFFULL << (8ULL * (sizeof(T) - 1 - i)));
            byte = (std::uint8_t)((value & mask) >> (8U * (sizeof(T) - 1 - i)));
            *((std::uint8_t *)&storage + i) = byte;
        }
    }

    /**
     * @brief Stores supplied value in little endian.
     */
    template<typename T>
    requires std::is_integral_v<T>
    static void store_in_little_endian(T value, T& storage)
    {
        assert( (value != 0) ); /* Test produces pointless results if test value is 0. */
        std::uint8_t byte = 0;

        for (std::size_t i = 0; i < sizeof(T); i++)
        {
            /* Calculate the next byte in value to use, starting at LSB. I.e. if
            value == 0x1234, first byte == 0x34, next 
            byte (next for-loop iteration) == 0x12, etc. */
            T mask = (0xFFULL << (8ULL * i));
            byte = (std::uint8_t)((value & mask) >> (8U * i));
            *((std::uint8_t *)&storage + i) = byte;
        }
    }

    /**
     * @brief For testing ECU_CPU_TO_LE_COMPILETIME() and
     * ECU_LE_TO_CPU_COMPILETIME() macros.
     */
    struct le_compiletime
    {
        /* Raw test values. Make signed integers negative. */
        static constexpr std::int16_t s16_raw = static_cast<std::int16_t>(0xFF12);
        static constexpr std::uint16_t u16_raw =static_cast<std::uint16_t>(0x1234);
        static constexpr std::int32_t s32_raw =static_cast<std::int32_t>(0xFF123456);
        static constexpr std::uint32_t u32_raw =static_cast<std::uint32_t>(0x12345678);
        static constexpr std::int64_t s64_raw =static_cast<std::int64_t>(0xFF11223344556677);
        static constexpr std::uint64_t u64_raw =static_cast<std::uint64_t>(0x1122334455667788);

        /* These should store raw values defined above in little endian. */
        static constexpr std::int16_t s16_cpu_to_le = ECU_CPU_TO_LE16_COMPILETIME(s16_raw);
        static constexpr std::uint16_t u16_cpu_to_le = ECU_CPU_TO_LE16_COMPILETIME(u16_raw);
        static constexpr std::int32_t s32_cpu_to_le = ECU_CPU_TO_LE32_COMPILETIME(s32_raw);
        static constexpr std::uint32_t u32_cpu_to_le = ECU_CPU_TO_LE32_COMPILETIME(u32_raw);
        static constexpr std::int64_t s64_cpu_to_le = ECU_CPU_TO_LE64_COMPILETIME(s64_raw);
        static constexpr std::uint64_t u64_cpu_to_le = ECU_CPU_TO_LE64_COMPILETIME(u64_raw);

        /* These should convert the values stored in little endian back into the original 
        raw values the CPU can understand. 
        WARNING: Requires ECU_CPU_TO_LE_COMPILETIME() macros to be working!! */
        static constexpr std::int16_t s16_le_to_cpu = ECU_LE16_TO_CPU_COMPILETIME(s16_cpu_to_le);
        static constexpr std::uint16_t u16_le_to_cpu = ECU_LE16_TO_CPU_COMPILETIME(u16_cpu_to_le);
        static constexpr std::int32_t s32_le_to_cpu = ECU_LE32_TO_CPU_COMPILETIME(s32_cpu_to_le);
        static constexpr std::uint32_t u32_le_to_cpu = ECU_LE32_TO_CPU_COMPILETIME(u32_cpu_to_le);
        static constexpr std::int64_t s64_le_to_cpu = ECU_LE64_TO_CPU_COMPILETIME(s64_cpu_to_le);
        static constexpr std::uint64_t u64_le_to_cpu = ECU_LE64_TO_CPU_COMPILETIME(u64_cpu_to_le);
    };

    /**
     * @brief For testing ECU_CPU_TO_BE_COMPILETIME() and 
     * ECU_BE_TO_CPU_COMPILETIME() macros.
     */
    struct be_compiletime
    {
        /* Raw test values. Make signed integers negative. */
        static constexpr std::int16_t s16_raw = static_cast<std::int16_t>(0xFF12);
        static constexpr std::uint16_t u16_raw =static_cast<std::uint16_t>(0x1234);
        static constexpr std::int32_t s32_raw =static_cast<std::int32_t>(0xFF123456);
        static constexpr std::uint32_t u32_raw =static_cast<std::uint32_t>(0x12345678);
        static constexpr std::int64_t s64_raw =static_cast<std::int64_t>(0xFF11223344556677);
        static constexpr std::uint64_t u64_raw =static_cast<std::uint64_t>(0x1122334455667788);

        /* These should store raw values defined above in big endian. */
        static constexpr std::int16_t s16_cpu_to_be = ECU_CPU_TO_BE16_COMPILETIME(s16_raw);
        static constexpr std::uint16_t u16_cpu_to_be = ECU_CPU_TO_BE16_COMPILETIME(u16_raw);
        static constexpr std::int32_t s32_cpu_to_be = ECU_CPU_TO_BE32_COMPILETIME(s32_raw);
        static constexpr std::uint32_t u32_cpu_to_be = ECU_CPU_TO_BE32_COMPILETIME(u32_raw);
        static constexpr std::int64_t s64_cpu_to_be = ECU_CPU_TO_BE64_COMPILETIME(s64_raw);
        static constexpr std::uint64_t u64_cpu_to_be = ECU_CPU_TO_BE64_COMPILETIME(u64_raw);

        /* These should convert the values stored in big endian back into the original 
        raw values the CPU can understand. 
        WARNING: Requires ECU_CPU_TO_BE_COMPILETIME() macros to be working!! */
        static constexpr std::int16_t s16_be_to_cpu = ECU_BE16_TO_CPU_COMPILETIME(s16_cpu_to_be);
        static constexpr std::uint16_t u16_be_to_cpu = ECU_BE16_TO_CPU_COMPILETIME(u16_cpu_to_be);
        static constexpr std::int32_t s32_be_to_cpu = ECU_BE32_TO_CPU_COMPILETIME(s32_cpu_to_be);
        static constexpr std::uint32_t u32_be_to_cpu = ECU_BE32_TO_CPU_COMPILETIME(u32_cpu_to_be);
        static constexpr std::int64_t s64_be_to_cpu = ECU_BE64_TO_CPU_COMPILETIME(s64_cpu_to_be);
        static constexpr std::uint64_t u64_be_to_cpu = ECU_BE64_TO_CPU_COMPILETIME(u64_cpu_to_be);
    };

    /* Raw test values for runtime tests. Make signed integers negative. */
    std::int16_t s16_raw = static_cast<std::int16_t>(0xFF12);
    std::uint16_t u16_raw =static_cast<std::uint16_t>(0x1234);
    std::int32_t s32_raw =static_cast<std::int32_t>(0xFF123456);
    std::uint32_t u32_raw =static_cast<std::uint32_t>(0x12345678);
    std::int64_t s64_raw =static_cast<std::int64_t>(0xFF11223344556677);
    std::uint64_t u64_raw =static_cast<std::uint64_t>(0x1122334455667788);
};

/*------------------------------------------------------------*/
/*------------ TESTS - ECU_CPU_TO_LE_COMPILETIME() -----------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify data stored in little endian at compile time.
 */
TEST(Endian, CPUToLittleEndianCompileTime)
{
    /* Step 3: Assert. */
    CHECK_TRUE( (is_little_endian(le_compiletime::s16_raw, le_compiletime::s16_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(le_compiletime::u16_raw, le_compiletime::u16_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(le_compiletime::u32_raw, le_compiletime::u32_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(le_compiletime::s32_raw, le_compiletime::s32_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(le_compiletime::u64_raw, le_compiletime::u64_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(le_compiletime::s64_raw, le_compiletime::s64_cpu_to_le)) );
}

/*------------------------------------------------------------*/
/*-------------- TESTS - ECU_CPU_TO_LE_RUNTIME() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify data stored in little endian at run time.
 */
TEST(Endian, CPUToLittleEndianRuntime)
{
    /* Step 2: Action. */
    std::int16_t s16_cpu_to_le = ECU_CPU_TO_LE16_RUNTIME(s16_raw);
    std::uint16_t u16_cpu_to_le = ECU_CPU_TO_LE16_RUNTIME(u16_raw);
    std::int32_t s32_cpu_to_le = ECU_CPU_TO_LE32_RUNTIME(s32_raw);
    std::uint32_t u32_cpu_to_le = ECU_CPU_TO_LE32_RUNTIME(u32_raw);
    std::int64_t s64_cpu_to_le = ECU_CPU_TO_LE64_RUNTIME(s64_raw);
    std::uint64_t u64_cpu_to_le = ECU_CPU_TO_LE64_RUNTIME(u64_raw);

    /* Step 3: Assert. */
    CHECK_TRUE( (is_little_endian(s16_raw, s16_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(u16_raw, u16_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(s32_raw, s32_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(u32_raw, u32_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(s64_raw, s64_cpu_to_le)) );
    CHECK_TRUE( (is_little_endian(u64_raw, u64_cpu_to_le)) );
}

/*------------------------------------------------------------*/
/*------------ TESTS - ECU_CPU_TO_BE_COMPILETIME() -----------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify data stored in big endian at compile time.
 */
TEST(Endian, CPUToBigEndianCompileTime)
{
    /* Step 3: Assert. */
    CHECK_TRUE( (is_big_endian(be_compiletime::s16_raw, be_compiletime::s16_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(be_compiletime::u16_raw, be_compiletime::u16_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(be_compiletime::u32_raw, be_compiletime::u32_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(be_compiletime::s32_raw, be_compiletime::s32_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(be_compiletime::u64_raw, be_compiletime::u64_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(be_compiletime::s64_raw, be_compiletime::s64_cpu_to_be)) );
}

/*------------------------------------------------------------*/
/*-------------- TESTS - ECU_CPU_TO_BE_RUNTIME() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify data stored in big endian at run time.
 */
TEST(Endian, CPUToBigEndianRuntime)
{
    /* Step 2: Action. */
    std::int16_t s16_cpu_to_be = ECU_CPU_TO_BE16_RUNTIME(s16_raw);
    std::uint16_t u16_cpu_to_be = ECU_CPU_TO_BE16_RUNTIME(u16_raw);
    std::int32_t s32_cpu_to_be = ECU_CPU_TO_BE32_RUNTIME(s32_raw);
    std::uint32_t u32_cpu_to_be = ECU_CPU_TO_BE32_RUNTIME(u32_raw);
    std::int64_t s64_cpu_to_be = ECU_CPU_TO_BE64_RUNTIME(s64_raw);
    std::uint64_t u64_cpu_to_be = ECU_CPU_TO_BE64_RUNTIME(u64_raw);

    /* Step 3: Assert. */
    CHECK_TRUE( (is_big_endian(s16_raw, s16_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(u16_raw, u16_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(s32_raw, s32_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(u32_raw, u32_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(s64_raw, s64_cpu_to_be)) );
    CHECK_TRUE( (is_big_endian(u64_raw, u64_cpu_to_be)) );
}

/*------------------------------------------------------------*/
/*------------ TESTS - ECU_LE_TO_CPU_COMPILETIME() -----------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify CPU correctly reads little endian encoded data.
 * @warning Requires ECU_CPU_TO_LE_COMPILETIME() macros to be working.
 */
TEST(Endian, LittleEndianToCPUCompileTime)
{
    /* Step 3: Assert. */
    CHECK_TRUE( (le_compiletime::s16_le_to_cpu == le_compiletime::s16_raw) );
    CHECK_TRUE( (le_compiletime::u16_le_to_cpu == le_compiletime::u16_raw) );
    CHECK_TRUE( (le_compiletime::s32_le_to_cpu == le_compiletime::s32_raw) );
    CHECK_TRUE( (le_compiletime::u32_le_to_cpu == le_compiletime::u32_raw) );
    CHECK_TRUE( (le_compiletime::s64_le_to_cpu == le_compiletime::s64_raw) );
    CHECK_TRUE( (le_compiletime::u64_le_to_cpu == le_compiletime::u64_raw) );
}

/*------------------------------------------------------------*/
/*-------------- TESTS - ECU_LE_TO_CPU_RUNTIME() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify CPU correctly reads little endian encoded data.
 */
TEST(Endian, LittleEndianToCPURunTime)
{
    /* Step 1: Arrange. */
    std::int16_t s16_cpu_to_le;
    std::uint16_t u16_cpu_to_le;
    std::int32_t s32_cpu_to_le;
    std::uint32_t u32_cpu_to_le;
    std::int64_t s64_cpu_to_le;
    std::uint64_t u64_cpu_to_le;
    store_in_little_endian(s16_raw, s16_cpu_to_le);
    store_in_little_endian(u16_raw, u16_cpu_to_le);
    store_in_little_endian(s32_raw, s32_cpu_to_le);
    store_in_little_endian(u32_raw, u32_cpu_to_le);
    store_in_little_endian(s64_raw, s64_cpu_to_le);
    store_in_little_endian(u64_raw, u64_cpu_to_le);

    /* Step 2: Action. */
    std::int16_t s16_le_to_cpu = ECU_LE16_TO_CPU_RUNTIME(s16_cpu_to_le);
    std::uint16_t u16_le_to_cpu = ECU_LE16_TO_CPU_RUNTIME(u16_cpu_to_le);
    std::int32_t s32_le_to_cpu = ECU_LE32_TO_CPU_RUNTIME(s32_cpu_to_le);
    std::uint32_t u32_le_to_cpu = ECU_LE32_TO_CPU_RUNTIME(u32_cpu_to_le);
    std::int64_t s64_le_to_cpu = ECU_LE64_TO_CPU_RUNTIME(s64_cpu_to_le);
    std::uint64_t u64_le_to_cpu = ECU_LE64_TO_CPU_RUNTIME(u64_cpu_to_le);

    /* Step 3: Assert. */
    CHECK_TRUE( (s16_le_to_cpu == s16_raw) );
    CHECK_TRUE( (u16_le_to_cpu == u16_raw) );
    CHECK_TRUE( (s32_le_to_cpu == s32_raw) );
    CHECK_TRUE( (u32_le_to_cpu == u32_raw) );
    CHECK_TRUE( (s64_le_to_cpu == s64_raw) );
    CHECK_TRUE( (u64_le_to_cpu == u64_raw) );
}

/*------------------------------------------------------------*/
/*------------ TESTS - ECU_BE_TO_CPU_COMPILETIME() -----------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify CPU correctly reads big endian encoded data.
 * @warning Requires ECU_CPU_TO_BE_COMPILETIME() macros to be working.
 */
TEST(Endian, BigEndianToCPUCompileTime)
{
    /* Step 3: Assert. */
    CHECK_TRUE( (be_compiletime::s16_be_to_cpu == be_compiletime::s16_raw) );
    CHECK_TRUE( (be_compiletime::u16_be_to_cpu == be_compiletime::u16_raw) );
    CHECK_TRUE( (be_compiletime::s32_be_to_cpu == be_compiletime::s32_raw) );
    CHECK_TRUE( (be_compiletime::u32_be_to_cpu == be_compiletime::u32_raw) );
    CHECK_TRUE( (be_compiletime::s64_be_to_cpu == be_compiletime::s64_raw) );
    CHECK_TRUE( (be_compiletime::u64_be_to_cpu == be_compiletime::u64_raw) );
}

/*------------------------------------------------------------*/
/*-------------- TESTS - ECU_BE_TO_CPU_RUNTIME() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify CPU correctly reads big endian encoded data.
 */
TEST(Endian, BigEndianToCPURunTime)
{
    /* Step 1: Arrange. */
    std::int16_t s16_cpu_to_be;
    std::uint16_t u16_cpu_to_be;
    std::int32_t s32_cpu_to_be;
    std::uint32_t u32_cpu_to_be;
    std::int64_t s64_cpu_to_be;
    std::uint64_t u64_cpu_to_be;
    store_in_big_endian(s16_raw, s16_cpu_to_be);
    store_in_big_endian(u16_raw, u16_cpu_to_be);
    store_in_big_endian(s32_raw, s32_cpu_to_be);
    store_in_big_endian(u32_raw, u32_cpu_to_be);
    store_in_big_endian(s64_raw, s64_cpu_to_be);
    store_in_big_endian(u64_raw, u64_cpu_to_be);

    /* Step 2: Action. */
    std::int16_t s16_be_to_cpu = ECU_BE16_TO_CPU_RUNTIME(s16_cpu_to_be);
    std::uint16_t u16_be_to_cpu = ECU_BE16_TO_CPU_RUNTIME(u16_cpu_to_be);
    std::int32_t s32_be_to_cpu = ECU_BE32_TO_CPU_RUNTIME(s32_cpu_to_be);
    std::uint32_t u32_be_to_cpu = ECU_BE32_TO_CPU_RUNTIME(u32_cpu_to_be);
    std::int64_t s64_be_to_cpu = ECU_BE64_TO_CPU_RUNTIME(s64_cpu_to_be);
    std::uint64_t u64_be_to_cpu = ECU_BE64_TO_CPU_RUNTIME(u64_cpu_to_be);

    /* Step 3: Assert. */
    CHECK_TRUE( (s16_be_to_cpu == s16_raw) );
    CHECK_TRUE( (u16_be_to_cpu == u16_raw) );
    CHECK_TRUE( (s32_be_to_cpu == s32_raw) );
    CHECK_TRUE( (u32_be_to_cpu == u32_raw) );
    CHECK_TRUE( (s64_be_to_cpu == s64_raw) );
    CHECK_TRUE( (u64_be_to_cpu == u64_raw) );
}
