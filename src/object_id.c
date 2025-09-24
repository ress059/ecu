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

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/object_id.h"

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (ECU_IS_SIGNED(ecu_object_id_t)), "ecu_object_id_t must be a signed type." );

/* The start of object IDs that users can define must always be 0 for future compatibility. */
ECU_STATIC_ASSERT( (ECU_USER_OBJECT_ID_BEGIN == 0), "ECU_USER_OBJECT_ID_BEGIN must equal 0." );
