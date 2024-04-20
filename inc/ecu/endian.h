/**
 * @file
 * @brief Byte swapping and endianness macros compatible with any
 * C or C++ standard that supports fixed-width types. Example usage:
 * 
 * @code{.c}
 * #include <ecu/endian.h>
 * 
 * /---------------------------------------------------------------------------/
 * /-------- SEND DATA ACROSS BUS IN SPECIFIC ENDIANNESS (COMPILE TIME) -------/
 * /---------------------------------------------------------------------------/
 * 
 * // val1 = 0x1234 if compiling on Little Endian CPU. Macro does nothing.
 * // val1 = 0x3412 if compiling on Big Endian CPU. Macro swaps bytes.
 * uint16_t val1 = ECU_CPU_TO_LE16_COMPILETIME(0x1234);
 * send(val1);
 * 
 * 
 * /---------------------------------------------------------------------------/
 * /---------- SEND DATA ACROSS BUS IN SPECIFIC ENDIANNESS (RUN TIME) ---------/
 * /---------------------------------------------------------------------------/
 * 
 * static inline uint32_t foo(uint32_t val)
 * {
 *     return ECU_CPU_TO_BE32_RUNTIME(val);
 * }
 * 
 * // send(0x78563412); if compiling on Little Endian CPU. Macro swaps bytes.
 * // send(0x12345678); if compiling on Little Endian CPU. Macro does nothing.
 * send(foo(0x12345678));
 * 
 * 
 * /---------------------------------------------------------------------------/
 * /--------- READ DATA FROM BUS THATS IN SPECIFIC ENDIANNESS (RUN TIME) ------/
 * /---------------------------------------------------------------------------/
 * 
 * // Assume bus is Little Endian in this example. Read some data directly from bus.
 * // Assume le16_data is 0x1234 stored in Little Endian format.
 * uint16_t le16_data = read_from_le_bus();
 * 
 * // Convert it to CPU endianness so CPU can interpret the value correctly.
 * // val2 = 0x1234 if compiling on Little Endian CPU. Macro does nothing.
 * // val2 = 0x1234 if compiling on Big Endian CPU. Macro swaps bytes.
 * uint16_t val2 = ECU_LE16_TO_CPU_RUNTIME(le16_data);
 * @endcode
 * 
 * Endianness of the target is automatically determined by the CMake build 
 * system using CMAKE_C_BYTE_ORDER variable. -DECU_LITTLE_ENDIAN is passed
 * to the compiler for Little Endian targets and -DECU_BIG_ENDIAN is passed
 * for Big Endian targets. This allows appropriate expansion of endianness
 * macros.
 * 
 * Also note that type punning via pointer casts or unions are purposefully 
 * NOT done to swap bytes since this is undefined behavior (violates strict 
 * aliasing). Instead bytes are swapped using appropriate casting and shifting. 
 * Castings done for compile-time swapping macros ensure sufficient storage. 
 * Castings done for run-time swapping functions are for potentially faster 
 * operations while also ensuring sufficient storage.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-17
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_ENDIAN_H_
#define ECU_ENDIAN_H_



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ INCLUDES -----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ COMPILE TIME BYTE SWAPS --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Swap bytes of 16-bit variable at compile-time. 
 * Compatible with any C or C++ standard that supports 
 * fixed-width types since these standards mandate size 
 * of int is at least 2 bytes and uint16_t is defined.
 * 
 * @warning Only use this for compile-time evaluations.
 */
#define ECU_SWAP16_COMPILETIME(x)           (uint16_t)( (((x) & 0xFF00) >> 8) | \
                                                        (((x) & 0x00FF) << 8) )


/**
 * @brief Swap bytes of 32-bit variable at compile-time. 
 * Compatible with any C or C++ standard that supports 
 * fixed-width types since these standards mandate size 
 * of unsigned long (UL) is at least 4 bytes and uint32_t 
 * is defined.
 * 
 * @warning Only use this for compile-time evaluations.
 */
#define ECU_SWAP32_COMPILETIME(x)           (uint32_t)( (((x) & 0xFF000000UL) >> 24) | \
                                                        (((x) & 0x00FF0000UL) >> 8)  | \
                                                        (((x) & 0x0000FF00UL) << 8)  | \
                                                        (((x) & 0x000000FFUL) << 24) )


/**
 * @brief Swap bytes of 64-bit variable at compile-time. 
 * Compatible with any C or C++ standard that supports 
 * fixed-width types since these standards mandate size 
 * of unsigned long long (ULL) is at least 8 bytes and 
 * uint64_t is defined.
 * 
 * @warning Only use this for compile-time evaluations.
 */
#define ECU_SWAP64_COMPILETIME(x)           (uint64_t)( (((x) & 0xFF00000000000000ULL) >> 56) | \
                                                        (((x) & 0x00FF000000000000ULL) >> 40) | \
                                                        (((x) & 0x0000FF0000000000ULL) >> 24) | \
                                                        (((x) & 0x000000FF00000000ULL) >> 8)  | \
                                                        (((x) & 0x00000000FF000000ULL) << 8)  | \
                                                        (((x) & 0x0000000000FF0000ULL) << 24) | \
                                                        (((x) & 0x000000000000FF00ULL) << 40) | \
                                                        (((x) & 0x00000000000000FFULL) << 56) )



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- RUN TIME BYTE SWAPS ----------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return byte-swapped copy of 16-bit variable evaluated
 * at run-time.
 * 
 * @warning Only use this for run-time evaluations.
 */
static inline uint16_t ecu_swap16_runtime(uint_fast16_t val)
{
    /* Avoid type punning via (uint8_t *) or using a union so behavior 
    is defined in both C and C++. Compiler will implicitly typecast if 
    uint_fast16_t has higher precedence over int. Otherwise int promotion 
    happens no matter what. */
    ( ((val & 0xFF00) >> 8) | ((val & 0x00FF) << 8) );
}


/**
 * @brief Return byte-swapped copy of 32-bit variable evaluated
 * at run-time.
 * 
 * @warning Only use this for run-time evaluations.
 */
static inline uint32_t ecu_swap32_runtime(uint_fast32_t val)
{
    /* Avoid type punning via (uint8_t *) or using a union so behavior 
    is defined in both C and C++. Explicitly cast in case int cannot hold 
    these raw hexadecimal values and also so faster operation occurs. */
    return ( ((val & (uint_fast32_t)0xFF000000) >> 24) | \
             ((val & (uint_fast32_t)0x00FF0000) >> 8)  | \
             ((val & (uint_fast32_t)0x0000FF00) << 8)  | \
             ((val & (uint_fast32_t)0x000000FF) << 24) );
}


/**
 * @brief Return byte-swapped copy of 64-bit variable evaluated
 * at run-time.
 * 
 * @warning Only use this for run-time evaluations.
 */
static inline uint64_t ecu_swap64_runtime(uint_fast64_t val)
{
    /* Avoid type punning via (uint8_t *) or using a union so behavior 
    is defined in both C and C++. Explicitly cast in case int cannot hold 
    these raw hexadecimal values and also so faster operation occurs. */
    return ( ((val & (uint_fast64_t)0xFF00000000000000) >> 56) | \
             ((val & (uint_fast64_t)0x00FF000000000000) >> 40) | \
             ((val & (uint_fast64_t)0x0000FF0000000000) >> 24) | \
             ((val & (uint_fast64_t)0x000000FF00000000) >> 8)  | \
             ((val & (uint_fast64_t)0x00000000FF000000) << 8)  | \
             ((val & (uint_fast64_t)0x0000000000FF0000) << 24) | \
             ((val & (uint_fast64_t)0x000000000000FF00) << 40) | \
             ((val & (uint_fast64_t)0x00000000000000FF) << 56) );
}

#ifdef __cplusplus
}
#endif



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- ENDIANNESS MACROS ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#if defined(ECU_DOXYGEN)
/**
 * @name Little Endian Data to CPU
 * Compile time and runtime macros that let CPU read 
 * Little Endian encoded data.
 * 
 * Example macro expansion if compiling on Little Endian CPU:
 * @code{.c}
 * #define ECU_LE16_TO_CPU_COMPILETIME(x)   (x)
 * #define ECU_LE16_TO_CPU_RUNTIME(x)       (x)
 * @endcode
 * 
 * Example macro expansion if compiling on Big Endian CPU:
 * @code{.c}
 * #define ECU_LE16_TO_CPU_COMPILETIME(x)   ECU_SWAP16_COMPILETIME(x)
 * #define ECU_LE16_TO_CPU_RUNTIME(x)       ecu_swap16_runtime(x)
 * @endcode
 */
/**@{*/
#define ECU_LE16_TO_CPU_COMPILETIME(x)
#define ECU_LE16_TO_CPU_RUNTIME(x)
#define ECU_LE32_TO_CPU_COMPILETIME(x)
#define ECU_LE32_TO_CPU_RUNTIME(x)
#define ECU_LE64_TO_CPU_COMPILETIME(x)
#define ECU_LE64_TO_CPU_RUNTIME(x)
/**@}*/


/**
 * @name Big Endian Data to CPU
 * Compile time and runtime macros that let CPU read 
 * Big Endian encoded data.
 * 
 * Example macro expansion if compiling on Little Endian CPU:
 * @code{.c}
 * #define ECU_BE16_TO_CPU_COMPILETIME(x)   ECU_SWAP16_COMPILETIME(x)
 * #define ECU_BE16_TO_CPU_RUNTIME(x)       ecu_swap16_runtime(x)
 * @endcode
 * 
 * Example macro expansion if compiling on Big Endian CPU:
 * @code{.c}
 * #define ECU_BE16_TO_CPU_COMPILETIME(x)   (x)
 * #define ECU_BE16_TO_CPU_RUNTIME(x)       (x)
 * @endcode
 */
/**@{*/
#define ECU_BE16_TO_CPU_COMPILETIME(x)
#define ECU_BE16_TO_CPU_RUNTIME(x)
#define ECU_BE32_TO_CPU_COMPILETIME(x)
#define ECU_BE32_TO_CPU_RUNTIME(x)
#define ECU_BE64_TO_CPU_COMPILETIME(x)
#define ECU_BE64_TO_CPU_RUNTIME(x)
/**@}*/


/**
 * @name CPU Data To Little Endian
 * Compile time and runtime macros that convert data 
 * stored in CPU into Little Endian encoded format.
 * 
 * Example macro expansion if compiling on Little Endian CPU:
 * @code{.c}
 * #define ECU_CPU_TO_LE16_COMPILETIME(x)   (x)
 * #define ECU_CPU_TO_LE16_RUNTIME(x)       (x)
 * @endcode
 * 
 * Example macro expansion if compiling on Big Endian CPU:
 * @code{.c}
 * #define ECU_CPU_TO_LE16_COMPILETIME(x)   ECU_SWAP16_COMPILETIME(x)
 * #define ECU_CPU_TO_LE16_RUNTIME(x)       ecu_swap16_runtime(x)
 * @endcode
 */
/**@{*/
#define ECU_CPU_TO_LE16_COMPILETIME(x)
#define ECU_CPU_TO_LE16_RUNTIME(x)
#define ECU_CPU_TO_LE32_COMPILETIME(x)
#define ECU_CPU_TO_LE32_RUNTIME(x)
#define ECU_CPU_TO_LE64_COMPILETIME(x)
#define ECU_CPU_TO_LE64_RUNTIME(x)
/**@}*/


/**
 * @name CPU Data To Big Endian
 * Compile time and runtime macros that convert data 
 * stored in CPU into Big Endian encoded format.
 * 
 * Example macro expansion if compiling on Little Endian CPU:
 * @code{.c}
 * #define ECU_CPU_TO_BE16_COMPILETIME(x)   ECU_SWAP16_COMPILETIME(x)
 * #define ECU_CPU_TO_BE16_RUNTIME(x)       ecu_swap16_runtime(x)
 * @endcode
 * 
 * Example macro expansion if compiling on Big Endian CPU:
 * @code{.c}
 * #define ECU_CPU_TO_LE16_COMPILETIME(x)   (x)
 * #define ECU_CPU_TO_LE16_RUNTIME(x)       (x)
 * @endcode
 */
/**@{*/
#define ECU_CPU_TO_BE16_COMPILETIME(x)
#define ECU_CPU_TO_BE16_RUNTIME(x)
#define ECU_CPU_TO_BE32_COMPILETIME(x)
#define ECU_CPU_TO_BE32_RUNTIME(x)
#define ECU_CPU_TO_BE64_COMPILETIME(x)
#define ECU_CPU_TO_BE64_RUNTIME(x)
/**@}*/
#else
#   if defined(ECU_LITTLE_ENDIAN) /* Defined by CMake build system. */
#       define ECU_LE16_TO_CPU_COMPILETIME(x)           (x)
#       define ECU_LE16_TO_CPU_RUNTIME(x)               (x)
#       define ECU_LE32_TO_CPU_COMPILETIME(x)           (x)
#       define ECU_LE32_TO_CPU_RUNTIME(x)               (x)
#       define ECU_LE64_TO_CPU_COMPILETIME(x)           (x)
#       define ECU_LE64_TO_CPU_RUNTIME(x)               (x)

#       define ECU_BE16_TO_CPU_COMPILETIME(x)           ECU_SWAP16_COMPILETIME(x)
#       define ECU_BE16_TO_CPU_RUNTIME(x)               ecu_swap16_runtime(x)
#       define ECU_BE32_TO_CPU_COMPILETIME(x)           ECU_SWAP32_COMPILETIME(x)
#       define ECU_BE32_TO_CPU_RUNTIME(x)               ecu_swap32_runtime(x)
#       define ECU_BE64_TO_CPU_COMPILETIME(x)           ECU_SWAP64_COMPILETIME(x)
#       define ECU_BE64_TO_CPU_RUNTIME(x)               ecu_swap64_runtime(x)

#       define ECU_CPU_TO_LE16_COMPILETIME(x)           (x)
#       define ECU_CPU_TO_LE16_RUNTIME(x)               (x)
#       define ECU_CPU_TO_LE32_COMPILETIME(x)           (x)
#       define ECU_CPU_TO_LE32_RUNTIME(x)               (x)
#       define ECU_CPU_TO_LE64_COMPILETIME(x)           (x)
#       define ECU_CPU_TO_LE64_RUNTIME(x)               (x)

#       define ECU_CPU_TO_BE16_COMPILETIME(x)           ECU_SWAP16_COMPILETIME(x)
#       define ECU_CPU_TO_BE16_RUNTIME(x)               ecu_swap16_runtime(x)
#       define ECU_CPU_TO_BE32_COMPILETIME(x)           ECU_SWAP32_COMPILETIME(x)
#       define ECU_CPU_TO_BE32_RUNTIME(x)               ecu_swap32_runtime(x)
#       define ECU_CPU_TO_BE64_COMPILETIME(x)           ECU_SWAP64_COMPILETIME(x)
#       define ECU_CPU_TO_BE64_RUNTIME(x)               ecu_swap64_runtime(x)
#   elif defined(ECU_BIG_ENDIAN) /* Defined by CMake build system. */
#       define ECU_LE16_TO_CPU_COMPILETIME(x)           ECU_SWAP16_COMPILETIME(x)
#       define ECU_LE16_TO_CPU_RUNTIME(x)               ecu_swap16_runtime(x)
#       define ECU_LE32_TO_CPU_COMPILETIME(x)           ECU_SWAP32_COMPILETIME(x)
#       define ECU_LE32_TO_CPU_RUNTIME(x)               ecu_swap32_runtime(x)
#       define ECU_LE64_TO_CPU_COMPILETIME(x)           ECU_SWAP64_COMPILETIME(x)
#       define ECU_LE64_TO_CPU_RUNTIME(x)               ecu_swap64_runtime(x)

#       define ECU_BE16_TO_CPU_COMPILETIME(x)           (x)
#       define ECU_BE16_TO_CPU_RUNTIME(x)               (x)
#       define ECU_BE32_TO_CPU_COMPILETIME(x)           (x)
#       define ECU_BE32_TO_CPU_RUNTIME(x)               (x)
#       define ECU_BE64_TO_CPU_COMPILETIME(x)           (x)
#       define ECU_BE64_TO_CPU_RUNTIME(x)               (x)

#       define ECU_CPU_TO_LE16_COMPILETIME(x)           ECU_SWAP16_COMPILETIME(x)
#       define ECU_CPU_TO_LE16_RUNTIME(x)               ecu_swap16_runtime(x)
#       define ECU_CPU_TO_LE32_COMPILETIME(x)           ECU_SWAP32_COMPILETIME(x)
#       define ECU_CPU_TO_LE32_RUNTIME(x)               ecu_swap32_runtime(x)
#       define ECU_CPU_TO_LE64_COMPILETIME(x)           ECU_SWAP64_COMPILETIME(x)
#       define ECU_CPU_TO_LE64_RUNTIME(x)               ecu_swap64_runtime(x)

#       define ECU_CPU_TO_BE16_COMPILETIME(x)           (x)
#       define ECU_CPU_TO_BE16_RUNTIME(x)               (x)
#       define ECU_CPU_TO_BE32_COMPILETIME(x)           (x)
#       define ECU_CPU_TO_BE32_RUNTIME(x)               (x)
#       define ECU_CPU_TO_BE64_COMPILETIME(x)           (x)
#       define ECU_CPU_TO_BE64_RUNTIME(x)               (x)
#   else 
#       error "CMake build system unable to detect target endianness. "
              "Looking at CMake variable CMAKE_C_BYTE_ORDER"
#   endif
#endif



#endif /* ECU_ENDIAN_H_ */
