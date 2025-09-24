/**
 * @file
 * @brief
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

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("ecu/event.c")

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (ECU_IS_SIGNED(ecu_event_id_t)), "ecu_event_id_t must be a signed type." );

/* The start of event IDs that users can define must always be 0 for future compatibility. */
ECU_STATIC_ASSERT( (ECU_USER_EVENT_ID_BEGIN == 0), "ECU_USER_EVENT_ID_BEGIN must equal 0." );

/*------------------------------------------------------------*/
/*------------------ EVENT MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

void ecu_event_ctor(struct ecu_event *me, ecu_event_id_t id, size_t size)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (id >= ECU_VALID_EVENT_ID_BEGIN) );
    ECU_ASSERT( ((size == ECU_EVENT_SIZE_UNUSED) || (size >= sizeof(struct ecu_event))) );

    me->id = id;
    me->size = size;
}

ecu_event_id_t ecu_event_id(const struct ecu_event *me)
{
    ECU_ASSERT( (me) );
    return (me->id);
}

size_t ecu_event_size(const struct ecu_event *me)
{
    ECU_ASSERT( (me) );
    return (me->size);
}
