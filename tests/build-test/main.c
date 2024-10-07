/**
 * @file
 * @brief Dummy main.c to test compilation and linkage of ECU library. 
 * Calls and uses common API functions.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-01
 * @copyright Copyright (c) 2024
 */


/* STDLib. */
#include <stdint.h>

/* ECU library. Call some common API functions. */
#include "ecu/asserter.h"
#include "ecu/circular_dll.h"
#include "ecu/event.h"
#include "ecu/fsm.h"



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- STATIC ASSERTS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

ECU_STATIC_ASSERT( (2==2), "Test static assert." );

ECU_STATIC_ASSERT( (2==3), "Test static assert." );


/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- FILE-SCOPE TYPES -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

enum app_event_signals
{
    TIMER_EXPIRED_EVENT = ECU_USER_EVENT_ID_BEGIN
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
static struct ecu_fsm_state app_fsm_state;
static struct app_event app_event;
static struct app_dll_node app_node1;
static struct app_dll_node app_node2;
static struct ecu_circular_dll app_list;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ STATIC FUNCTION DECLARATIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static enum ecu_fsm_status INIT_STATE_ENTRY(struct app_fsm *fsm);
static enum ecu_fsm_status INIT_STATE(struct app_fsm *fsm, const struct app_event *e);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- STATIC FUNCTION DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static enum ecu_fsm_status INIT_STATE_ENTRY(struct app_fsm *fsm)
{
    fsm->counter = 0;
}


static enum ecu_fsm_status INIT_STATE(struct app_fsm *fsm, const struct app_event *e)
{
    enum ecu_fsm_status status = ECU_FSM_EVENT_HANDLED;

    switch(e->super.id)
    {
        case TIMER_EXPIRED_EVENT:
        {
            fsm->counter++;
            if(fsm->counter > 10)
            {
                /* Self-state transition. */
                status = ecu_fsm_transition_to_state((struct ecu_fsm *)&app_fsm, &app_fsm_state);
            }
            break;
        }

        default:
        {
            status = ECU_FSM_EVENT_IGNORED;
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
    ecu_fsm_state_ctor(&app_fsm_state, (ecu_fsm_on_entry_handler)&INIT_STATE_ENTRY, 
                       (ecu_fsm_on_exit_handler)0, (ecu_fsm_state_handler)&INIT_STATE);
    ecu_event_ctor((struct ecu_event *)&app_event, TIMER_EXPIRED_EVENT);
    app_event.data = 5;

    /* Call some API functions */
    ECU_RUNTIME_ASSERT( (3 == 3), ECU_DEFAULT_FUNCTOR );
    ecu_circular_dll_ctor(&app_list);
    ecu_circular_dll_node_ctor(&app_node1.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
    ecu_circular_dll_node_ctor(&app_node2.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);

    ecu_fsm_ctor((struct ecu_fsm *)&app_fsm, &app_fsm_state);
    
    ecu_circular_dll_push_back(&app_list, &app_node1.node);
    ecu_circular_dll_push_back(&app_list, &app_node2.node);
    ecu_circular_dll_remove_node(&app_node2.node);

    ecu_fsm_dispatch((struct ecu_fsm *)&app_fsm, (const struct ecu_event *)&app_event);

    return 0;
}