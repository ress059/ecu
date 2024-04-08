/**
 * @file
 * @author Ian Ress
 * @brief See @ref object_id.h. This file is only used to force a compilation error
 * if @ref ecu_id is declared as unsigned type.
 * @version 0.1
 * @date 2024-04-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* Translation unit. */
#include <ecu/object_id.h>

/* Static assert. */
#include <ecu/asserter.h>



/* Produce compilation error if ecu_id is an unsigned type. */
ECU_STATIC_ASSERT( (((ecu_id)(-1)) < ((ecu_id)(0))) );

ECU_STATIC_ASSERT( (ECU_USER_ID_BEGIN == 0) );
