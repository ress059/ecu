/**
 * @file
 * @author Ian Ress
 * @brief Dummy main.c to test if ECU library compiles. Calls and uses common API functions.
 * @version 0.1
 * @date 2024-03-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* STDLib. */
#include <stdint.h>

/* ECU library. Call some common API functions. */
#include <ecu/asserter.h>
#include <ecu/circular_dll.h>
#include <ecu/fsm.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- STATIC ASSERTS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

ECU_STATIC_ASSERT( (2==2) );



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- FILE-SCOPE TYPES -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

enum app_event_signals
{
    TIMER_EXPIRED_EVENT = ECU_USER_EVENT_BEGIN
};


struct app_fsm
{
    struct ecu_fsm super;
    /********************/
    uint8_t counter;
};


struct app_event
{
    struct ecu_event super;
    /********************/
    uint8_t data;
};


struct app_dll_node
{
    uint16_t x;
    struct ecu_circular_dll_node node;
    uint32_t y;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static struct app_fsm app_fsm;
static struct app_event app_event;
static struct app_dll_node app_node1;
static struct app_dll_node app_node2;
static struct ecu_circular_dll app_list;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ STATIC FUNCTION DECLARATIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static enum ecu_fsm_status app_state_handler(struct app_fsm *fsm, const struct app_event *e);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static enum ecu_fsm_status app_state_handler(struct app_fsm *fsm, const struct app_event *e)
{
    enum ecu_fsm_status status = ECU_FSM_EVENT_HANDLED;

    switch(e->super.signal)
    {
        case ECU_ENTRY_EVENT:
        {
            fsm->counter = 0;
            break;
        }

        case ECU_EXIT_EVENT:
        {
            break;
        }

        case TIMER_EXPIRED_EVENT:
        {
            fsm->counter++;
            if(fsm->counter > 10)
            {
                ecu_fsm_change_state((struct ecu_fsm *)&app_fsm, (ecu_fsm_func_ptr)&app_state_handler);
            }
            break;
        }

        default:
        {
            break;
        }
    }

    return status;
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------- MAIN ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

int main(void)
{
    /* Setup. */
    app_event.super.signal = TIMER_EXPIRED_EVENT;
    app_event.data = 5;

    /* Call some API functions */
    ECU_RUNTIME_ASSERT( (3 == 3), ECU_NO_FUNCTOR );
    ecu_circular_dll_ctor(&app_list);
    ecu_fsm_ctor((struct ecu_fsm *)&app_fsm, (ecu_fsm_func_ptr)&app_state_handler, 1);
    
    ecu_circular_dll_push_back(&app_list, &app_node1.node);
    ecu_circular_dll_push_back(&app_list, &app_node2.node);
    ecu_circular_dll_remove_node(&app_list, &app_node2.node);

    ecu_fsm_dispatch((struct ecu_fsm *)&app_fsm, (const struct ecu_event *)&app_event);

    return 0;
}