/**
 * @file 
 * @brief See @ref asserter.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/asserter.h"

/* Assert handler setters for each module. */
#include "ecu/circular_dll.h"
#include "ecu/fsm.h"
#include "ecu/hsm.h"
#include "ecu/timer.h"
#include "ecu/tree.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- GLOBAL VARIABLES -------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* When using C/C++ standard that does not natively support static assert, we must implement
a custom mechanism. We make ECU_STATIC_ASSERT() macro uses ecu_static_assert_do_not_use_[] array 
to produce compilation errors via negative-array indexing. Macro expands to ecu_static_assert_do_not_use_[1]
if assert passes. Macro expands to ecu_static_assert_do_not_use_[-1] if assert fails, thus producing
a compilation error. 

Therefore this array must actually be defined somewhere since it will be referenced whenever
ECU_STATIC_ASSERT() is called. Define and allocate memory for it here.*/
#ifdef ECU_USING_CUSTOM_STATIC_ASSERT_DO_NOT_USE_
const char ecu_static_assert_do_not_use_[1] = {0};
#endif



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- STATIC FUNCTION DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void default_assert_handler(void);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void default_assert_handler(void)
{
#if !defined(NDEBUG)
    while (1)
    {
        /* Permanently hang to inspect call stack. */
    }
#endif
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- PUBLIC FUNCTIONS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_set_assert_functor_all(struct ecu_assert_functor *functor)
{
    /* Do not NULL check functor since setting to NULL means the default assert handler will now be called. */
    ecu_circular_dll_set_assert_functor(functor);
    ecu_fsm_set_assert_functor(functor);
    // ecu_hsm_set_assert_functor(functor);
    ecu_timer_set_assert_functor(functor);
    ecu_tree_set_assert_functor(functor);
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PRIVATE FUNCTIONS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_assert_do_not_use(struct ecu_assert_functor *functor, const char *file, int line)
{
    if (functor)
    {
        if (functor->handler)
        {
            (*functor->handler)(functor, file, line); /* user-defined handler. */
        }
        else
        {
            default_assert_handler();
        }
    }
    else
    {
        default_assert_handler();
    }
}
