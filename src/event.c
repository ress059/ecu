/**
 * @file
 * @author Ian Ress
 * @brief See @ref event.h. This file is only used for static assertions of
 * code within @ref event.h.
 * @version 0.1
 * @date 2024-04-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* Translation unit. */
#include <ecu/event.h>

/* Static assert. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- STATIC ASSERTS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/* Produce compilation error if ecu_event_id is an unsigned type. */
ECU_STATIC_ASSERT( (((ecu_event_id)(-1)) < ((ecu_event_id)(0))) );

/* The start of event IDs that users can define must always be 0 for future compatibility. */
ECU_STATIC_ASSERT( (ECU_USER_EVENT_ID_BEGIN == 0) );
