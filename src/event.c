/**
 * @file
 * @author Ian Ress
 * @brief See @ref event.h. This file is only used to force a compilation error
 * if @ref ecu_event_signal is declared as unsigned type.
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



/* Produce compilation error if ecu_event_signal is an unsigned type. */
ECU_STATIC_ASSERT( (((ecu_event_signal)(-1)) < ((ecu_event_signal)(0))) );

ECU_STATIC_ASSERT( (ECU_USER_EVENT_BEGIN == 0) );
