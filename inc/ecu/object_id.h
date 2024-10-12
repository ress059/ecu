/**
 * @file
 * @brief Provides a scheme that allows the library to define reserved object IDs and
 * users to define their own object IDs without conflicts. Object IDs the library
 * reserves will be negative and object IDs the user defines will start at 0 which
 * is always @ref ECU_USER_OBJECT_ID_BEGIN. This scheme also allows library functions 
 * to know when an invalid ID was used via @ref ECU_VALID_OBJECT_ID_BEGIN enumeration.
 * Example of user defining their own IDs:
 * @code{.c}
 * enum user_ids
 * {
 *     OBJECT1_ID = ECU_USER_OBJECT_ID_BEGIN,
 *     OBJECT2_ID,
 *     OBJECT3_ID
 * };
 * @endcode
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-07
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_OBJECT_ID_H_
#define ECU_OBJECT_ID_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- INCLUDES ---------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ OBJECT IDS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Scheme that allows the library to define reserved object IDs and
 * users to define their own object IDs without conflicts. IDs the library
 * defines will be negative and IDs the user defines will start at 0 which is 
 * always @ref ECU_USER_OBJECT_ID_BEGIN. Example of user defining their own IDs:
 * @code{.c}
 * enum user_obj_ids
 * {
 *     OBJECT1_ID = ECU_USER_OBJECT_ID_BEGIN,
 *     OBJECT2_ID,
 *     OBJECT3_ID
 * };
 * @endcode
 * 
 * This scheme also allows library functions to know when an invalid object 
 * ID was used via @ref ECU_VALID_OBJECT_ID_BEGIN enumeration.
 */
enum ecu_reserved_object_ids
{
    /*------------------------------------------------------------------*/
    /*-------------------- RESERVED OBJECT IDS SECTION -----------------*/
    /*-------- LAST MEMBER MUST EQUAL ECU_VALID_OBJECT_ID_BEGIN --------*/
    /*------------------------------------------------------------------*/
    /* ADD FUTURE RESERVED IDS HERE */
    ECU_OBJECT_ID_RESERVED      = -2,   /**< PRIVATE. FOR LIBRARY USE ONLY. */
    /*******************************/
    ECU_VALID_OBJECT_ID_BEGIN   = -1,   /**< PRIVATE. FOR LIBRARY USE ONLY. Represents start of object IDs users can pass to library functions. */

    /*------------------------------------------------------------------*/
    /*-------------------- AVAILABLE OBJECT IDS SECTION ----------------*/
    /*-------- FIRST MEMBER MUST EQUAL ECU_VALID_OBJECT_ID_BEGIN -------*/
    /*------------------------------------------------------------------*/
    ECU_OBJECT_ID_UNUSED        = ECU_VALID_OBJECT_ID_BEGIN,    /**< User sets object ID to this value if they do not want to use object ID mechanism. */
    ECU_USER_OBJECT_ID_BEGIN    = 0                             /**< 0. Start of user-defined IDs. Must always be 0 for future compatibility. */
};


/**
 * @private 
 * @brief PRIVATE. Generic type that represents an object ID. Used so the library
 * can implicitly typecast between this value, @ref ecu_reserved_object_ids 
 * enumerations, and user-defined ID enumerations.
 * 
 * @warning This must be a signed integer type in order to 
 * handle @ref ecu_reserved_object_ids enumerations less than 0. A 
 * compilation error will occur if this is declared as an unsigned type.
 */
typedef int16_t ecu_object_id;


#endif /* ECU_OBJECT_ID_H_ */
