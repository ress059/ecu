/**
 * @file
 * @author Ian Ress
 * @brief Provides a scheme that allows the library to define reserved object IDs and
 * users to define their own object IDs without conflicts. Object IDs the library
 * reserves will be negative and object IDs the user defines will start at 0 which
 * is always @ref ECU_USER_ID_BEGIN. Example of user defining their own IDs:
 * @code{.c}
 * enum user_ids
 * {
 *     OBJECT1_ID = ECU_USER_ID_BEGIN,
 *     OBJECT2_ID,
 *     OBJECT3_ID
 * };
 * @endcode
 * 
 * This scheme also allows library functions to know when an invalid ID
 * was used via @ref ECU_VALID_ID_BEGIN enumeration.
 * @version 0.1
 * @date 2024-04-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef ECU_ID_H_
#define ECU_ID_H_


#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ OBJECT IDS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Scheme that allows the library to define reserved IDs and
 * users to define their own object IDs without conflicts. IDs the library
 * defines will be negative and IDs the user defines will start at 0 which
 * is always @ref ECU_USER_ID_BEGIN. Example of user defining their own IDs:
 * @code{.c}
 * enum user_ids
 * {
 *     OBJECT1_ID = ECU_USER_ID_BEGIN,
 *     OBJECT2_ID,
 *     OBJECT3_ID
 * };
 * @endcode
 * 
 * This scheme also allows library functions to know when an invalid ID
 * was used via @ref ECU_VALID_ID_BEGIN enumeration.
 */
enum ecu_reserved_ids
{
    /*-----------------------------------------------------------*/
    /*-------------------- RESERVED IDS SECTION. ----------------*/
    /*--------- LAST MEMBER MUST EQUAL ECU_VALID_ID_BEGIN -------*/
    /*-----------------------------------------------------------*/
    /* ADD FUTURE RESERVED IDS HERE */
    ECU_ID_RESERVED             = -2,   /**< -2. FOR LIBRARY USE ONLY. */
    ECU_VALID_ID_BEGIN          = -1,   /**< -1. FOR LIBRARY USE ONLY. Must be last member in this section. Represents end of reserved IDs. */

    /*-----------------------------------------------------------*/
    /*-------------------- AVAILABLE IDS SECTION. -------------- */
    /*--------- FIRST MEMBER MUST EQUAL ECU_VALID_ID_BEGIN ------*/
    /*-----------------------------------------------------------*/
    ECU_ID_UNUSED               = ECU_VALID_ID_BEGIN,   /**< -1. ID mechanism unused. */
    ECU_USER_ID_BEGIN           = 0                     /**< 0. Start of user-defined IDs. Must always be 0 for future compatibility. */
};


/**
 * @private 
 * @brief PRIVATE. Generic type that represents an object ID. Used so the library
 * can implicitly typecast between this value, @ref ecu_reserved_ids enumerations,
 * and user-defined ID enumerations.
 * 
 * @warning This must be a signed integer type in order to 
 * handle @ref ecu_reserved_ids enumerations less than 0. A compilation 
 * error will occur if this is declared as an unsigned type.
 */
typedef int16_t ecu_id;


#endif /* ECU_ID_H_ */
