/**
 * @file
 * @brief
 * @rst
 * See :ref:`endian.h section <endian_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-17
 * @copyright Copyright (c) 2024
 */

#ifndef ECU_ENDIAN_H_
#define ECU_ENDIAN_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/*------------------------------------------------------------*/
/*------------------ COMPILE TIME BYTE SWAPS -----------------*/
/*------------------------------------------------------------*/

/**
 * @name Compile-time Byte Swapping
 */
/**@{*/
/**
 * @brief Returns byte-swapped value (switches endiannes)
 * of 16-bit input at compile-time. Compatible with any 
 * C or C++ standard that supports fixed-width types since 
 * these standards mandate size of int is at least 2 bytes 
 * and uint16_t is defined.
 */
#define ECU_SWAP16_COMPILETIME(x_)      \
    (uint16_t)((((x_) & 0xFF00) >> 8) | \
               (((x_) & 0x00FF) << 8))

/**
 * @brief Returns byte-swapped value (switches endianness)
 * of 32-bit input at compile-time. Compatible with any C 
 * or C++ standard that supports fixed-width types since 
 * these standards mandate size of unsigned long (UL) is 
 * at least 4 bytes and uint32_t is defined.
 */
#define ECU_SWAP32_COMPILETIME(x_)             \
    (uint32_t)((((x_) & 0xFF000000UL) >> 24) | \
               (((x_) & 0x00FF0000UL) >> 8) |  \
               (((x_) & 0x0000FF00UL) << 8) |  \
               (((x_) & 0x000000FFUL) << 24))

/**
 * @brief Returns byte-swapped value (switches endianness)
 * of 64-bit input at compile-time. Compatible with any C 
 * or C++ standard that supports fixed-width types since 
 * these standards mandate size of unsigned long long (ULL) 
 * is at least 8 bytes and uint64_t is defined.
 */
#define ECU_SWAP64_COMPILETIME(x_)                      \
    (uint64_t)((((x_) & 0xFF00000000000000ULL) >> 56) | \
               (((x_) & 0x00FF000000000000ULL) >> 40) | \
               (((x_) & 0x0000FF0000000000ULL) >> 24) | \
               (((x_) & 0x000000FF00000000ULL) >> 8) |  \
               (((x_) & 0x00000000FF000000ULL) << 8) |  \
               (((x_) & 0x0000000000FF0000ULL) << 24) | \
               (((x_) & 0x000000000000FF00ULL) << 40) | \
               (((x_) & 0x00000000000000FFULL) << 56))
/**@}*/

/*------------------------------------------------------------*/
/*-------------------- RUNTIME BYTE SWAPS --------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Runtime Byte Swapping
 */
/**@{*/
/**
 * @brief Returns byte-swapped value (switches endianness)
 * of 16-bit input at runtime. Can handle signed inputs since
 * implementation only cares about the binary representation
 * of the input, not its actual value.
 */
static inline uint16_t ecu_swap16_runtime(uint_fast16_t val)
{
    /* Avoid type punning via (uint8_t *) or using a union so behavior
    is defined in both C and C++. Compiler will implicitly typecast if
    uint_fast16_t has higher precedence over int. Otherwise int promotion
    happens no matter what. */
    return (((val & 0xFF00) >> 8) | ((val & 0x00FF) << 8));
}

/**
 * @brief Returns byte-swapped value (switches endianness)
 * of 32-bit input at runtime. Can handle signed inputs since
 * implementation only cares about the binary representation
 * of the input, not its actual value.
 */
static inline uint32_t ecu_swap32_runtime(uint_fast32_t val)
{
    /* Avoid type punning via (uint8_t *) or using a union so behavior
    is defined in both C and C++. Explicitly cast in case int cannot hold
    these raw hexadecimal values and also so faster operation occurs. */
    return (((val & (uint_fast32_t)0xFF000000) >> 24) |
            ((val & (uint_fast32_t)0x00FF0000) >> 8) |
            ((val & (uint_fast32_t)0x0000FF00) << 8) |
            ((val & (uint_fast32_t)0x000000FF) << 24));
}

/**
 * @brief Returns byte-swapped value (switches endianness)
 * of 64-bit input at runtime. Can handle signed inputs since
 * implementation only cares about the binary representation
 * of the input, not its actual value.
 */
static inline uint64_t ecu_swap64_runtime(uint_fast64_t val)
{
    /* Avoid type punning via (uint8_t *) or using a union so behavior
    is defined in both C and C++. Explicitly cast in case int cannot hold
    these raw hexadecimal values and also so faster operation occurs. */
    return (((val & (uint_fast64_t)0xFF00000000000000) >> 56) |
            ((val & (uint_fast64_t)0x00FF000000000000) >> 40) |
            ((val & (uint_fast64_t)0x0000FF0000000000) >> 24) |
            ((val & (uint_fast64_t)0x000000FF00000000) >> 8) |
            ((val & (uint_fast64_t)0x00000000FF000000) << 8) |
            ((val & (uint_fast64_t)0x0000000000FF0000) << 24) |
            ((val & (uint_fast64_t)0x000000000000FF00) << 40) |
            ((val & (uint_fast64_t)0x00000000000000FF) << 56));
}
/**@}*/

#ifdef __cplusplus
}
#endif

/*------------------------------------------------------------*/
/*--------------------- ENDIANNESS MACROS --------------------*/
/*------------------------------------------------------------*/

#if defined(ECU_DOXYGEN)
    /**
     * @name Reading Big Endian Data
     */
    /**@{*/
    /// @brief Portable method for reading big endian
    /// encoded data at compile-time. Returns byte-swapped 
    /// input if compiling on little endian target. Does 
    /// nothing and returns same input if compiling on big 
    /// endian target.
    #define ECU_BE16_TO_CPU_COMPILETIME(x_)
    #define ECU_BE32_TO_CPU_COMPILETIME(x_)
    #define ECU_BE64_TO_CPU_COMPILETIME(x_)

    /// @brief Portable method for reading big endian
    /// encoded data at runtime. Returns byte-swapped 
    /// input if compiling on little endian target. Does 
    /// nothing and returns same input if compiling on big 
    /// endian target.
    #define ECU_BE32_TO_CPU_RUNTIME(x_)
    #define ECU_BE64_TO_CPU_RUNTIME(x_)
    #define ECU_BE16_TO_CPU_RUNTIME(x_)
    /**@}*/

    /**
     * @name Reading Little Endian Data
     */
    /**@{*/
    /// @brief Portable method for reading little endian
    /// encoded data at compile-time. Does nothing and 
    /// returns same input if compiling on little endian 
    /// target. Returns byte-swapped input if compiling 
    /// on big endian target.
    #define ECU_LE16_TO_CPU_COMPILETIME(x_)
    #define ECU_LE32_TO_CPU_COMPILETIME(x_)
    #define ECU_LE64_TO_CPU_COMPILETIME(x_)

    /// @brief Portable method for reading little endian
    /// encoded data at runtime. Does nothing and 
    /// returns same input if compiling on little endian 
    /// target. Returns byte-swapped input if compiling 
    /// on big endian target.
    #define ECU_LE16_TO_CPU_RUNTIME(x_)
    #define ECU_LE32_TO_CPU_RUNTIME(x_)
    #define ECU_LE64_TO_CPU_RUNTIME(x_)
    /**@}*/

    /**
     * @name Sending Big Endian Data
     */
    /**@{*/
    /// @brief Portable method for sending data in big
    /// endian format at compile-time. Returns byte-swapped 
    /// input if compiling on little endian target. Does 
    /// nothing and returns same input if compiling on big 
    /// endian target. 
    #define ECU_CPU_TO_BE16_COMPILETIME(x_)
    #define ECU_CPU_TO_BE32_COMPILETIME(x_)
    #define ECU_CPU_TO_BE64_COMPILETIME(x_)

    /// @brief Portable method for sending data in big
    /// endian format at runtime. Returns byte-swapped 
    /// input if compiling on little endian target. Does 
    /// nothing and returns same input if compiling on big 
    /// endian target. 
    #define ECU_CPU_TO_BE16_RUNTIME(x_)
    #define ECU_CPU_TO_BE32_RUNTIME(x_)
    #define ECU_CPU_TO_BE64_RUNTIME(x_)
    /**@}*/

    /**
     * @name Sending Little Endian Data
     */
    /**@{*/
    /// @brief Portable method for sending data in little
    /// endian format at compile-time. Does nothing and 
    /// returns same input if compiling on little endian 
    /// target. Returns byte-swapped input if compiling 
    /// on big endian target.
    #define ECU_CPU_TO_LE16_COMPILETIME(x_)
    #define ECU_CPU_TO_LE32_COMPILETIME(x_)
    #define ECU_CPU_TO_LE64_COMPILETIME(x_)

    /// @brief Portable method for sending data in little
    /// endian format at runtime. Does nothing and 
    /// returns same input if compiling on little endian 
    /// target. Returns byte-swapped input if compiling 
    /// on big endian target.
    #define ECU_CPU_TO_LE16_RUNTIME(x_)
    #define ECU_CPU_TO_LE32_RUNTIME(x_)
    #define ECU_CPU_TO_LE64_RUNTIME(x_)
    /**@}*/
#else
    #if defined(ECU_LITTLE_ENDIAN) /* Defined by CMake build system. */
        #define ECU_BE16_TO_CPU_COMPILETIME(x_) ECU_SWAP16_COMPILETIME(x_)
        #define ECU_BE32_TO_CPU_COMPILETIME(x_) ECU_SWAP32_COMPILETIME(x_)
        #define ECU_BE64_TO_CPU_COMPILETIME(x_) ECU_SWAP64_COMPILETIME(x_)
        #define ECU_BE16_TO_CPU_RUNTIME(x_)     ecu_swap16_runtime(x_)
        #define ECU_BE32_TO_CPU_RUNTIME(x_)     ecu_swap32_runtime(x_)
        #define ECU_BE64_TO_CPU_RUNTIME(x_)     ecu_swap64_runtime(x_)

        #define ECU_LE16_TO_CPU_COMPILETIME(x_) (x_)
        #define ECU_LE32_TO_CPU_COMPILETIME(x_) (x_)
        #define ECU_LE64_TO_CPU_COMPILETIME(x_) (x_)
        #define ECU_LE16_TO_CPU_RUNTIME(x_)     (x_)
        #define ECU_LE32_TO_CPU_RUNTIME(x_)     (x_)
        #define ECU_LE64_TO_CPU_RUNTIME(x_)     (x_)

        #define ECU_CPU_TO_BE16_COMPILETIME(x_) ECU_SWAP16_COMPILETIME(x_)
        #define ECU_CPU_TO_BE32_COMPILETIME(x_) ECU_SWAP32_COMPILETIME(x_)
        #define ECU_CPU_TO_BE64_COMPILETIME(x_) ECU_SWAP64_COMPILETIME(x_)
        #define ECU_CPU_TO_BE16_RUNTIME(x_)     ecu_swap16_runtime(x_)
        #define ECU_CPU_TO_BE32_RUNTIME(x_)     ecu_swap32_runtime(x_)
        #define ECU_CPU_TO_BE64_RUNTIME(x_)     ecu_swap64_runtime(x_)

        #define ECU_CPU_TO_LE16_COMPILETIME(x_) (x_)
        #define ECU_CPU_TO_LE32_COMPILETIME(x_) (x_)
        #define ECU_CPU_TO_LE64_COMPILETIME(x_) (x_)
        #define ECU_CPU_TO_LE16_RUNTIME(x_)     (x_)
        #define ECU_CPU_TO_LE32_RUNTIME(x_)     (x_)
        #define ECU_CPU_TO_LE64_RUNTIME(x_)     (x_)
    #elif defined(ECU_BIG_ENDIAN) /* Defined by CMake build system. */
        #define ECU_BE16_TO_CPU_COMPILETIME(x_) (x_)
        #define ECU_BE32_TO_CPU_COMPILETIME(x_) (x_)
        #define ECU_BE64_TO_CPU_COMPILETIME(x_) (x_)
        #define ECU_BE16_TO_CPU_RUNTIME(x_)     (x_)
        #define ECU_BE32_TO_CPU_RUNTIME(x_)     (x_)
        #define ECU_BE64_TO_CPU_RUNTIME(x_)     (x_)

        #define ECU_LE16_TO_CPU_COMPILETIME(x_) ECU_SWAP16_COMPILETIME(x_)
        #define ECU_LE32_TO_CPU_COMPILETIME(x_) ECU_SWAP32_COMPILETIME(x_)
        #define ECU_LE64_TO_CPU_COMPILETIME(x_) ECU_SWAP64_COMPILETIME(x_)
        #define ECU_LE16_TO_CPU_RUNTIME(x_)     ecu_swap16_runtime(x_)
        #define ECU_LE32_TO_CPU_RUNTIME(x_)     ecu_swap32_runtime(x_)
        #define ECU_LE64_TO_CPU_RUNTIME(x_)     ecu_swap64_runtime(x_)
    
        #define ECU_CPU_TO_BE16_COMPILETIME(x_) (x_)
        #define ECU_CPU_TO_BE32_COMPILETIME(x_) (x_)
        #define ECU_CPU_TO_BE64_COMPILETIME(x_) (x_)
        #define ECU_CPU_TO_BE16_RUNTIME(x_)     (x_)
        #define ECU_CPU_TO_BE32_RUNTIME(x_)     (x_)
        #define ECU_CPU_TO_BE64_RUNTIME(x_)     (x_)

        #define ECU_CPU_TO_LE16_COMPILETIME(x_) ECU_SWAP16_COMPILETIME(x_)
        #define ECU_CPU_TO_LE32_COMPILETIME(x_) ECU_SWAP32_COMPILETIME(x_)
        #define ECU_CPU_TO_LE64_COMPILETIME(x_) ECU_SWAP64_COMPILETIME(x_)
        #define ECU_CPU_TO_LE16_RUNTIME(x_)     ecu_swap16_runtime(x_)
        #define ECU_CPU_TO_LE32_RUNTIME(x_)     ecu_swap32_runtime(x_)
        #define ECU_CPU_TO_LE64_RUNTIME(x_)     ecu_swap64_runtime(x_)
    #else
        /* clang-format off */
        #error "CMake build system unable to detect target endianness from CMAKE_C_BYTE_ORDER variable. "
               "Manually define ECU_LITTLE_ENDIAN or ECU_BIG_ENDIAN to use this module."
        /* clang-format on */
    #endif
#endif /* ECU_DOXYGEN */

#endif /* ECU_ENDIAN_H_ */
