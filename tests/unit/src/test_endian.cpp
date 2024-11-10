/**
 * @file
 * @brief Unit tests for public API functions in @ref endian.h.
 * One of the tests (little endian or big endian) will have to 
 * swap bytes regardless of whatever computer this runs on.
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
#include <cstdint>

/* Files under test. */
#include "ecu/endian.h"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/
/* Make signed integers negative for all test groups. */

TEST_GROUP(CPUToLittleEndian)
{
    std::int16_t m_s16_le_runtime{0};
    std::uint16_t m_u16_le_runtime{0};
    std::int32_t m_s32_le_runtime{0};
    std::uint32_t m_u32_le_runtime{0};
    std::int64_t m_s64_le_runtime{0};
    std::uint64_t m_u64_le_runtime{0};

    static constexpr std::int16_t m_s16{static_cast<std::int16_t>(0xFF12)};
    static constexpr std::uint16_t m_u16{static_cast<std::uint16_t>(0x1234)};
    static constexpr std::int32_t m_s32{static_cast<std::int32_t>(0xFF123456)};
    static constexpr std::uint32_t m_u32{static_cast<std::uint32_t>(0x12345678)};
    static constexpr std::int64_t m_s64{static_cast<std::int64_t>(0xFF11223344556677)};
    static constexpr std::uint64_t m_u64{static_cast<std::uint64_t>(0x1122334455667788)};

    static constexpr std::int16_t m_s16_le_compiletime = ECU_CPU_TO_LE16_COMPILETIME(m_s16);
    static constexpr std::uint16_t m_u16_le_compiletime = ECU_CPU_TO_LE16_COMPILETIME(m_u16);
    static constexpr std::int32_t m_s32_le_compiletime = ECU_CPU_TO_LE32_COMPILETIME(m_s32);
    static constexpr std::uint32_t m_u32_le_compiletime = ECU_CPU_TO_LE32_COMPILETIME(m_u32);
    static constexpr std::int64_t m_s64_le_compiletime = ECU_CPU_TO_LE64_COMPILETIME(m_s64);
    static constexpr std::uint64_t m_u64_le_compiletime = ECU_CPU_TO_LE64_COMPILETIME(m_u64);
};

TEST_GROUP(CPUToBigEndian)
{
    std::int16_t m_s16_be_runtime{0};
    std::uint16_t m_u16_be_runtime{0};
    std::int32_t m_s32_be_runtime{0};
    std::uint32_t m_u32_be_runtime{0};
    std::int64_t m_s64_be_runtime{0};
    std::uint64_t m_u64_be_runtime{0};

    static constexpr std::int16_t m_s16{static_cast<std::int16_t>(0xFF12)};
    static constexpr std::uint16_t m_u16{static_cast<std::uint16_t>(0x1234)};
    static constexpr std::int32_t m_s32{static_cast<std::int32_t>(0xFF123456)};
    static constexpr std::uint32_t m_u32{static_cast<std::uint32_t>(0x12345678)};
    static constexpr std::int64_t m_s64{static_cast<std::int64_t>(0xFF11223344556677)};
    static constexpr std::uint64_t m_u64{static_cast<std::uint64_t>(0x1122334455667788)};

    static constexpr std::int16_t m_s16_be_compiletime = ECU_CPU_TO_BE16_COMPILETIME(m_s16);
    static constexpr std::uint16_t m_u16_be_compiletime = ECU_CPU_TO_BE16_COMPILETIME(m_u16);
    static constexpr std::int32_t m_s32_be_compiletime = ECU_CPU_TO_BE32_COMPILETIME(m_s32);
    static constexpr std::uint32_t m_u32_be_compiletime = ECU_CPU_TO_BE32_COMPILETIME(m_u32);
    static constexpr std::int64_t m_s64_be_compiletime = ECU_CPU_TO_BE64_COMPILETIME(m_s64);
    static constexpr std::uint64_t m_u64_be_compiletime = ECU_CPU_TO_BE64_COMPILETIME(m_u64);
};

TEST_GROUP(LittleEndianToCPU)
{
    std::int16_t m_s16_le_runtime{0};
    std::uint16_t m_u16_le_runtime{0};
    std::int32_t m_s32_le_runtime{0};
    std::uint32_t m_u32_le_runtime{0};
    std::int64_t m_s64_le_runtime{0};
    std::uint64_t m_u64_le_runtime{0};

    static constexpr std::int16_t m_s16_le{ECU_CPU_TO_LE16_COMPILETIME(static_cast<std::int16_t>(0xFF12))};
    static constexpr std::uint16_t m_u16_le{ECU_CPU_TO_LE16_COMPILETIME(static_cast<std::uint16_t>(0x1234))};
    static constexpr std::int32_t m_s32_le{ECU_CPU_TO_LE32_COMPILETIME(static_cast<std::int32_t>(0xFF123456))};
    static constexpr std::uint32_t m_u32_le{ECU_CPU_TO_LE32_COMPILETIME(static_cast<std::uint32_t>(0x12345678))};
    static constexpr std::int64_t m_s64_le{ECU_CPU_TO_LE64_COMPILETIME(static_cast<std::int64_t>(0xFF11223344556677))};
    static constexpr std::uint64_t m_u64_le{ECU_CPU_TO_LE64_COMPILETIME(static_cast<std::uint64_t>(0x1122334455667788))};

    static constexpr std::int16_t m_s16_le_compiletime = ECU_LE16_TO_CPU_COMPILETIME(m_s16_le);
    static constexpr std::uint16_t m_u16_le_compiletime = ECU_LE16_TO_CPU_COMPILETIME(m_u16_le);
    static constexpr std::int32_t m_s32_le_compiletime = ECU_LE32_TO_CPU_COMPILETIME(m_s32_le);
    static constexpr std::uint32_t m_u32_le_compiletime = ECU_LE32_TO_CPU_COMPILETIME(m_u32_le);
    static constexpr std::int64_t m_s64_le_compiletime = ECU_LE64_TO_CPU_COMPILETIME(m_s64_le);
    static constexpr std::uint64_t m_u64_le_compiletime = ECU_LE64_TO_CPU_COMPILETIME(m_u64_le);
};

TEST_GROUP(BigEndianToCPU)
{
    std::int16_t m_s16_be_runtime{0};
    std::uint16_t m_u16_be_runtime{0};
    std::int32_t m_s32_be_runtime{0};
    std::uint32_t m_u32_be_runtime{0};
    std::int64_t m_s64_be_runtime{0};
    std::uint64_t m_u64_be_runtime{0};

    static constexpr std::int16_t m_s16_be{ECU_CPU_TO_BE16_COMPILETIME(static_cast<std::int16_t>(0xFF12))};
    static constexpr std::uint16_t m_u16_be{ECU_CPU_TO_BE16_COMPILETIME(static_cast<std::uint16_t>(0x1234))};
    static constexpr std::int32_t m_s32_be{ECU_CPU_TO_BE32_COMPILETIME(static_cast<std::int32_t>(0xFF123456))};
    static constexpr std::uint32_t m_u32_be{ECU_CPU_TO_BE32_COMPILETIME(static_cast<std::uint32_t>(0x12345678))};
    static constexpr std::int64_t m_s64_be{ECU_CPU_TO_BE64_COMPILETIME(static_cast<std::int64_t>(0xFF11223344556677))};
    static constexpr std::uint64_t m_u64_be{ECU_CPU_TO_BE64_COMPILETIME(static_cast<std::uint64_t>(0x1122334455667788))};

    static constexpr std::int16_t m_s16_be_compiletime = ECU_BE16_TO_CPU_COMPILETIME(m_s16_be);
    static constexpr std::uint16_t m_u16_be_compiletime = ECU_BE16_TO_CPU_COMPILETIME(m_u16_be);
    static constexpr std::int32_t m_s32_be_compiletime = ECU_BE32_TO_CPU_COMPILETIME(m_s32_be);
    static constexpr std::uint32_t m_u32_be_compiletime = ECU_BE32_TO_CPU_COMPILETIME(m_u32_be);
    static constexpr std::int64_t m_s64_be_compiletime = ECU_BE64_TO_CPU_COMPILETIME(m_s64_be);
    static constexpr std::uint64_t m_u64_be_compiletime = ECU_BE64_TO_CPU_COMPILETIME(m_u64_be);
};

/*------------------------------------------------------------*/
/*----------------------------- TESTS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify CPU puts data into little endian format at compile time.
 */
TEST(CPUToLittleEndian, CompileTime)
{
    /* Step 3: Assert. */
    /* int16_t */
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s16_le_compiletime))));
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s16_le_compiletime) + 1)));

    /* uint16_t */
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_le_compiletime))));
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_le_compiletime) + 1)));

    /* int32_t */
    SIGNED_BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_compiletime))));
    SIGNED_BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_compiletime) + 1)));
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_compiletime) + 2)));
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_compiletime) + 3)));

    /* uint32_t */
    BYTES_EQUAL(0x78, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_compiletime))));
    BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_compiletime) + 1)));
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_compiletime) + 2)));
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_compiletime) + 3)));

    /* int64_t */
    SIGNED_BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime))));
    SIGNED_BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime) + 1)));
    SIGNED_BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime) + 2)));
    SIGNED_BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime) + 3)));
    SIGNED_BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime) + 4)));
    SIGNED_BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime) + 5)));
    SIGNED_BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime) + 6)));
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_compiletime) + 7)));

    /* uint64_t */
    BYTES_EQUAL(0x88, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime))));
    BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime) + 1)));
    BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime) + 2)));
    BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime) + 3)));
    BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime) + 4)));
    BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime) + 5)));
    BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime) + 6)));
    BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_compiletime) + 7)));
}

/**
 * @brief Verify CPU puts data into little endian format at run time.
 */
TEST(CPUToLittleEndian, RunTime)
{
    /* Step 2: Action. Make signed integers negative. */
    m_s16_le_runtime = ECU_CPU_TO_LE16_RUNTIME(0xFF12);
    m_u16_le_runtime = ECU_CPU_TO_LE16_RUNTIME(0x1234);
    m_s32_le_runtime = ECU_CPU_TO_LE32_RUNTIME(0xFF123456);
    m_u32_le_runtime = ECU_CPU_TO_LE32_RUNTIME(0x12345678);
    m_s64_le_runtime = ECU_CPU_TO_LE64_RUNTIME(0xFF11223344556677);
    m_u64_le_runtime = ECU_CPU_TO_LE64_RUNTIME(0x1122334455667788);

    /* Step 3: Assert. */
    /* int16_t */
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s16_le_runtime))));
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s16_le_runtime) + 1)));

    /* uint16_t */
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_le_runtime))));
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_le_runtime) + 1)));

    /* int32_t */
    SIGNED_BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_runtime))));
    SIGNED_BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_runtime) + 1)));
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_runtime) + 2)));
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s32_le_runtime) + 3)));

    /* uint32_t */
    BYTES_EQUAL(0x78, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_runtime))));
    BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_runtime) + 1)));
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_runtime) + 2)));
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_le_runtime) + 3)));

    /* int64_t */
    SIGNED_BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime))));
    SIGNED_BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime) + 1)));
    SIGNED_BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime) + 2)));
    SIGNED_BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime) + 3)));
    SIGNED_BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime) + 4)));
    SIGNED_BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime) + 5)));
    SIGNED_BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime) + 6)));
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s64_le_runtime) + 7)));

    /* uint64_t */
    BYTES_EQUAL(0x88, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime))));
    BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime) + 1)));
    BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime) + 2)));
    BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime) + 3)));
    BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime) + 4)));
    BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime) + 5)));
    BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime) + 6)));
    BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_le_runtime) + 7)));
}

/**
 * @brief Verify CPU puts data into big endian format at compile time.
 */
TEST(CPUToBigEndian, CompileTime)
{
    /* Step 3: Assert. */
    /* int16_t */
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s16_be_compiletime))));
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s16_be_compiletime) + 1)));

    /* uint16_t */
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_be_compiletime))));
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_be_compiletime) + 1)));

    /* int32_t */
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_compiletime))));
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_compiletime) + 1)));
    SIGNED_BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_compiletime) + 2)));
    SIGNED_BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_compiletime) + 3)));

    /* uint32_t */
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_compiletime))));
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_compiletime) + 1)));
    BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_compiletime) + 2)));
    BYTES_EQUAL(0x78, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_compiletime) + 3)));

    /* int64_t */
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime))));
    SIGNED_BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime) + 1)));
    SIGNED_BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime) + 2)));
    SIGNED_BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime) + 3)));
    SIGNED_BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime) + 4)));
    SIGNED_BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime) + 5)));
    SIGNED_BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime) + 6)));
    SIGNED_BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_compiletime) + 7)));

    /* uint64_t */
    BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime))));
    BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime) + 1)));
    BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime) + 2)));
    BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime) + 3)));
    BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime) + 4)));
    BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime) + 5)));
    BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime) + 6)));
    BYTES_EQUAL(0x88, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_compiletime) + 7)));
}

/**
 * @brief Verify CPU puts data into big endian format at run time.
 */
TEST(CPUToBigEndian, RunTime)
{
    /* Step 2: Action. Make signed integers negative. */
    m_s16_be_runtime = ECU_CPU_TO_BE16_RUNTIME(0xFF12);
    m_u16_be_runtime = ECU_CPU_TO_BE16_RUNTIME(0x1234);
    m_s32_be_runtime = ECU_CPU_TO_BE32_RUNTIME(0xFF123456);
    m_u32_be_runtime = ECU_CPU_TO_BE32_RUNTIME(0x12345678);
    m_s64_be_runtime = ECU_CPU_TO_BE64_RUNTIME(0xFF11223344556677);
    m_u64_be_runtime = ECU_CPU_TO_BE64_RUNTIME(0x1122334455667788);

    /* Step 3: Assert. */
    /* int16_t */
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s16_be_runtime))));
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s16_be_runtime) + 1)));

    /* uint16_t */
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_be_runtime))));
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u16_be_runtime) + 1)));

    /* int32_t */
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_runtime))));
    SIGNED_BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_runtime) + 1)));
    SIGNED_BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_runtime) + 2)));
    SIGNED_BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::int8_t*>(&m_s32_be_runtime) + 3)));

    /* uint32_t */
    BYTES_EQUAL(0x12, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_runtime))));
    BYTES_EQUAL(0x34, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_runtime) + 1)));
    BYTES_EQUAL(0x56, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_runtime) + 2)));
    BYTES_EQUAL(0x78, (*(reinterpret_cast<const std::uint8_t*>(&m_u32_be_runtime) + 3)));

    /* int64_t */
    SIGNED_BYTES_EQUAL(0xFF, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime))));
    SIGNED_BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime) + 1)));
    SIGNED_BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime) + 2)));
    SIGNED_BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime) + 3)));
    SIGNED_BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime) + 4)));
    SIGNED_BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime) + 5)));
    SIGNED_BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime) + 6)));
    SIGNED_BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::int8_t*>(&m_s64_be_runtime) + 7)));

    /* uint64_t */
    BYTES_EQUAL(0x11, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime))));
    BYTES_EQUAL(0x22, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime) + 1)));
    BYTES_EQUAL(0x33, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime) + 2)));
    BYTES_EQUAL(0x44, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime) + 3)));
    BYTES_EQUAL(0x55, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime) + 4)));
    BYTES_EQUAL(0x66, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime) + 5)));
    BYTES_EQUAL(0x77, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime) + 6)));
    BYTES_EQUAL(0x88, (*(reinterpret_cast<const std::uint8_t*>(&m_u64_be_runtime) + 7)));
}

/**
 * @brief Verify CPU correctly reads little endian encoded data.
 */
TEST(LittleEndianToCPU, CompileTime)
{
    /* Step 3: Assert. */
    /* int16_t and uint16_t */
    LONGS_EQUAL(static_cast<std::int16_t>(0xFF12), m_s16_le_compiletime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint16_t>(0x1234), m_u16_le_compiletime);

    /* int32_t and uint32_t */
    LONGS_EQUAL(static_cast<std::int32_t>(0xFF123456), m_s32_le_compiletime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint32_t>(0x12345678), m_u32_le_compiletime);

    /* int64_t and uint64_t */
    LONGLONGS_EQUAL(static_cast<std::int64_t>(0xFF11223344556677), m_s64_le_compiletime);
    UNSIGNED_LONGLONGS_EQUAL(static_cast<std::uint64_t>(0x1122334455667788), m_u64_le_compiletime);
}

/**
 * @brief Verify CPU correctly reads little endian encoded data.
 */
TEST(LittleEndianToCPU, RunTime)
{
    /* Step 2: Action. */
    m_s16_le_runtime = ECU_LE16_TO_CPU_RUNTIME(m_s16_le);
    m_u16_le_runtime = ECU_LE16_TO_CPU_RUNTIME(m_u16_le);
    m_s32_le_runtime = ECU_LE32_TO_CPU_RUNTIME(m_s32_le);
    m_u32_le_runtime = ECU_LE32_TO_CPU_RUNTIME(m_u32_le);
    m_s64_le_runtime = ECU_LE64_TO_CPU_RUNTIME(m_s64_le);
    m_u64_le_runtime = ECU_LE64_TO_CPU_RUNTIME(m_u64_le);

    /* Step 3: Assert. */
    /* int16_t and uint16_t */
    LONGS_EQUAL(static_cast<std::int16_t>(0xFF12), m_s16_le_runtime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint16_t>(0x1234), m_u16_le_runtime);

    /* int32_t and uint32_t */
    LONGS_EQUAL(static_cast<std::int32_t>(0xFF123456), m_s32_le_runtime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint32_t>(0x12345678), m_u32_le_runtime);

    /* int64_t and uint64_t */
    LONGLONGS_EQUAL(static_cast<std::int64_t>(0xFF11223344556677), m_s64_le_runtime);
    UNSIGNED_LONGLONGS_EQUAL(static_cast<std::uint64_t>(0x1122334455667788), m_u64_le_runtime);
}

/**
 * @brief Verify CPU correctly reads big endian encoded data.
 */
TEST(BigEndianToCPU, CompileTime)
{
    /* Step 3: Assert. */
    /* int16_t and uint16_t */
    LONGS_EQUAL(static_cast<std::int16_t>(0xFF12), m_s16_be_compiletime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint16_t>(0x1234), m_u16_be_compiletime);

    /* int32_t and uint32_t */
    LONGS_EQUAL(static_cast<std::int32_t>(0xFF123456), m_s32_be_compiletime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint32_t>(0x12345678), m_u32_be_compiletime);

    /* int64_t and uint64_t */
    LONGLONGS_EQUAL(static_cast<std::int64_t>(0xFF11223344556677), m_s64_be_compiletime);
    UNSIGNED_LONGLONGS_EQUAL(static_cast<std::uint64_t>(0x1122334455667788), m_u64_be_compiletime);
}

/**
 * @brief Verify CPU correctly reads big endian encoded data.
 */
TEST(BigEndianToCPU, RunTime)
{
    /* Step 2: Action. */
    m_s16_be_runtime = ECU_BE16_TO_CPU_RUNTIME(m_s16_be);
    m_u16_be_runtime = ECU_BE16_TO_CPU_RUNTIME(m_u16_be);
    m_s32_be_runtime = ECU_BE32_TO_CPU_RUNTIME(m_s32_be);
    m_u32_be_runtime = ECU_BE32_TO_CPU_RUNTIME(m_u32_be);
    m_s64_be_runtime = ECU_BE64_TO_CPU_RUNTIME(m_s64_be);
    m_u64_be_runtime = ECU_BE64_TO_CPU_RUNTIME(m_u64_be);

    /* Step 3: Assert. */
    /* int16_t and uint16_t */
    LONGS_EQUAL(static_cast<std::int16_t>(0xFF12), m_s16_be_runtime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint16_t>(0x1234), m_u16_be_runtime);

    /* int32_t and uint32_t */
    LONGS_EQUAL(static_cast<std::int32_t>(0xFF123456), m_s32_be_runtime);
    UNSIGNED_LONGS_EQUAL(static_cast<std::uint32_t>(0x12345678), m_u32_be_runtime);

    /* int64_t and uint64_t */
    LONGLONGS_EQUAL(static_cast<std::int64_t>(0xFF11223344556677), m_s64_be_runtime);
    UNSIGNED_LONGLONGS_EQUAL(static_cast<std::uint64_t>(0x1122334455667788), m_u64_be_runtime);
}
