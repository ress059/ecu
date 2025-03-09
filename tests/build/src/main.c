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

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* ECU library. Call some common API functions. */
#include "ecu/asserter.h"
#include "ecu/dlist.h"
#include "ecu/event.h"
// #include "ecu/fsm.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/main.c")

/*------------------------------------------------------------*/
/*----------------------- STATIC ASSERTS ---------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (2==2), "Test static assert." );

/*------------------------------------------------------------*/
/*---------------------- FILE SCOPE TYPES --------------------*/
/*------------------------------------------------------------*/

// enum app_event_signals
// {
//     TIMER_EXPIRED_EVENT = ECU_USER_EVENT_ID_BEGIN
// };

// struct app_fsm
// {
//     struct ecu_fsm super;
//     /********************/
//     uint8_t counter;
// };

// struct app_event
// {
//     struct ecu_event super;
//     /********************/
//     uint8_t data;
// };

struct node
{
    uint16_t x;
    struct ecu_dnode node;
    uint32_t y;
};

/*------------------------------------------------------------*/
/*------------------- FILE SCOPE VARIABLES -------------------*/
/*------------------------------------------------------------*/

// static struct app_fsm app_fsm;
// static struct ecu_fsm_state app_fsm_state;
// static struct app_event app_event;
static struct node node1;
static struct node node2;
static struct ecu_dlist list;

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

// static enum ecu_fsm_status INIT_STATE_ENTRY(struct app_fsm *fsm);
// static enum ecu_fsm_status INIT_STATE(struct app_fsm *fsm, const struct app_event *e);

/*------------------------------------------------------------*/
/*----------------- STATIC FUNCTION DEFINITIONS --------------*/
/*------------------------------------------------------------*/

// static enum ecu_fsm_status INIT_STATE_ENTRY(struct app_fsm *fsm)
// {
//     fsm->counter = 0;
// }

// static enum ecu_fsm_status INIT_STATE(struct app_fsm *fsm, const struct app_event *e)
// {
//     enum ecu_fsm_status status = ECU_FSM_EVENT_HANDLED;

//     switch(e->super.id)
//     {
//         case TIMER_EXPIRED_EVENT:
//         {
//             fsm->counter++;
//             if(fsm->counter > 10)
//             {
//                 /* Self-state transition. */
//                 status = ecu_fsm_transition_to_state((struct ecu_fsm *)&app_fsm, &app_fsm_state);
//             }
//             break;
//         }

//         default:
//         {
//             status = ECU_FSM_EVENT_IGNORED;
//             break;
//         }
//     }

//     return status;
// }

/*------------------------------------------------------------*/
/*--------------------------- MAIN ---------------------------*/
/*------------------------------------------------------------*/

int main(void)
{
    /* Setup. */
    // ecu_fsm_state_ctor(&app_fsm_state, (ecu_fsm_on_entry_handler)&INIT_STATE_ENTRY, 
    //                    (ecu_fsm_on_exit_handler)0, (ecu_fsm_state_handler)&INIT_STATE);
    // ecu_event_ctor((struct ecu_event *)&app_event, TIMER_EXPIRED_EVENT);
    // app_event.data = 5;

    /* Call some API functions */
    ECU_RUNTIME_ASSERT( (3 == 3) );
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    // ecu_fsm_ctor((struct ecu_fsm *)&app_fsm, &app_fsm_state);
    
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_front(&list, &node2.node);
    ecu_dnode_remove(&node2.node);

    // ecu_fsm_dispatch((struct ecu_fsm *)&app_fsm, ECU_EVENT_BASE_CAST(&app_event));

    return 0;
}

/*------------------------------------------------------------*/
/*---------------- ASSERT HANDLER DEFINITION -----------------*/
/*------------------------------------------------------------*/

void ecu_assert_handler(const char *file, int line)
{
    (void)file;
    (void)line;
    while(1)
    {

    }
}
