/**
 * @file
 * @brief Primarily used for static assertions of code within @ref event.h.
 * @rst
 * See :ref:`event.h section <event_h>` in Sphinx documentation.
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
#include "ecu/event.h"

/* Asserts. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/event.c")

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

/* Produce compilation error if ecu_event_id is an unsigned type. */
ECU_STATIC_ASSERT( (((ecu_event_id)(-1)) < ((ecu_event_id)(0))), "ecu_event_id must be an unsigned type." );

/* The start of event IDs that users can define must always be 0 for future compatibility. */
ECU_STATIC_ASSERT( (ECU_USER_EVENT_ID_BEGIN == 0), "ECU_USER_EVENT_ID_BEGIN must equal 0." );

/*------------------------------------------------------------*/
/*--------------------- PUBLIC FUNCTIONS ---------------------*/
/*------------------------------------------------------------*/

void ecu_event_ctor(struct ecu_event *me, 
                    ecu_event_id id_0)
{
    ECU_RUNTIME_ASSERT( ((me) && (id_0 >= ECU_VALID_EVENT_ID_BEGIN)) );
    me->id = id_0;
}
