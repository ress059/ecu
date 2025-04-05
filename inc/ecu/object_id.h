/**
 * @file
 * @brief
 * @rst
 * See :ref:`object_id.h section <object_id_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-07
 * @copyright Copyright (c) 2024
 */

#ifndef ECU_OBJECT_ID_H_
#define ECU_OBJECT_ID_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/*------------------------------------------------------------*/
/*------------------------ OBJECT IDS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Object IDs reserved for ECU library.
 * @details Scheme that allows the library and users to uniquely define
 * their own object IDs without conflicts. Object IDs reserved for ECU
 * library will always be negative. Object IDs the user defines will 
 * start at 0 which is always @ref ECU_USER_OBJECT_ID_BEGIN.
 */
enum ecu_reserved_object_ids
{
    /*------------------------------------------------------------------*/
    /*-------------------- RESERVED OBJECT IDS SECTION -----------------*/
    /*-------- LAST MEMBER MUST EQUAL ECU_VALID_OBJECT_ID_BEGIN --------*/
    /*------------------------------------------------------------------*/
    /* ADD FUTURE PRIVATE AND RESERVED IDS HERE. */
    ECU_OBJECT_ID_RESERVED      = -2,   /**< PRIVATE. FOR LIBRARY USE ONLY. */
    /*******************************/
    ECU_VALID_OBJECT_ID_BEGIN   = -1,   /**< PRIVATE. FOR LIBRARY USE ONLY. Start of reserved IDs that the user can assign to objects. */

    /*------------------------------------------------------------------*/
    /*-------------------- AVAILABLE OBJECT IDS SECTION ----------------*/
    /*-------- FIRST MEMBER MUST EQUAL ECU_VALID_OBJECT_ID_BEGIN -------*/
    /*------------------------------------------------------------------*/
    ECU_OBJECT_ID_UNUSED        = ECU_VALID_OBJECT_ID_BEGIN,    /**< Reserved ID. Signifies that object ID mechanism is not used. */
    ECU_USER_OBJECT_ID_BEGIN    = 0                             /**< 0. Start of user-defined IDs. Must always be 0 for future compatibility. */
};

/**
 * @brief PRIVATE. Generic type that represents an object ID. 
 * @details Used so the library can implicitly typecast between this 
 * value, @ref ecu_reserved_object_ids enumerations, and user-defined 
 * object ID enumerations.
 * 
 * @warning This must be a signed integer type in order to 
 * handle @ref ecu_reserved_object_ids enumerations less than 0. A 
 * compilation error will occur if this is declared as an unsigned 
 * type.
 */
typedef int32_t ecu_object_id;

#endif /* ECU_OBJECT_ID_H_ */
