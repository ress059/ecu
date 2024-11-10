/**
 * @file 
 * @brief See @ref asserter.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*---------------------- GLOBAL VARIABLES --------------------*/
/*------------------------------------------------------------*/

/* When using C/C++ standard that does not natively support static assert, we must implement
a custom mechanism. We make ECU_STATIC_ASSERT() macro uses ecu_static_assert_fired_[] array 
to produce compilation errors via negative-array indexing. Macro expands to ecu_static_assert_fired_[1]
if assert passes. Macro expands to ecu_static_assert_fired_[-1] if assert fails, thus producing
a compilation error. 

Therefore this array must actually be defined somewhere since it will be referenced whenever
ECU_STATIC_ASSERT() is called. Define and allocate memory for it here.*/
#ifdef ECU_USING_CUSTOM_STATIC_ASSERT_DO_NOT_USE_
const char ecu_static_assert_fired_[1] = {0};
#endif
