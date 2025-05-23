/**
 * @file
 * @brief Primarily used for static assertions of code within @ref object_id.h.
 * @rst
 * See :ref:`object_id.h section <object_id_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-07
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/object_id.h"

/* Static assert. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/object_id.c")

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

/* Produce compilation error if ecu_object_id is an unsigned type. */
ECU_STATIC_ASSERT( (((ecu_object_id)(-1)) < ((ecu_object_id)(0))), "ecu_object_id must be a signed type." );

/* The start of object IDs that users can define must always be 0 for future compatibility. */
ECU_STATIC_ASSERT( (ECU_USER_OBJECT_ID_BEGIN == 0), "ECU_USER_OBJECT_ID_BEGIN must equal 0." );
