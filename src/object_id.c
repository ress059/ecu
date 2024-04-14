/**
 * @file
 * @brief See @ref object_id.h. This file is only used for static assertions of
 * code within @ref object_id.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-07
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ INCLUDES ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include <ecu/object_id.h>

/* Static assert. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- STATIC ASSERTS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/* Produce compilation error if ecu_id is an unsigned type. */
ECU_STATIC_ASSERT( (((ecu_object_id)(-1)) < ((ecu_object_id)(0))) );

/* The start of object IDs that users can define must always be 0 for future compatibility. */
ECU_STATIC_ASSERT( (ECU_USER_OBJECT_ID_BEGIN == 0) );
