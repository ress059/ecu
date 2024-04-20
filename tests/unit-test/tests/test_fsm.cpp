/**
 * @file
 * @brief Unit tests for public API functions in @ref fsm.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-15
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Files under test. */
#include <ecu/asserter.h>
#include <ecu/event.h>
#include <ecu/fsm.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STUB AND MOCK DECLARATIONS --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

class AssertException
{
};


/**
 * @brief Use when ECU library function calls within the unit test could 
 * cause assert to fire but you wish to ignore it. This throws an exception
 * so control immediately returns to the caller (the unit test). This 
 * prevents library code from running under an assert condition, leading to
 * undefined behavior.
 */
struct AssertCallOk : public ecu_assert_functor 
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)me;
        (void)file;
        (void)line;
        throw AssertException();
    }
};


/**
 * @brief Use when ECU library function calls within the unit test should
 * never fire an assert. Calls a mock which will cause the test to fail
 * due to an unexpected call. Also throws an exception so control immediately
 * returns to the caller (the unit test). This prevents library code from 
 * running under an assert condition, leading to undefined behavior.
 */
struct AssertCallFail : public ecu_assert_functor 
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)me;
        (void)file;
        (void)line;
        mock().actualCall("AssertMock::assert_handler");
        throw AssertException();
    }
};



enum test_event_ids
{
    TRANSITION_EVENT = ECU_USER_EVENT_ID_BEGIN
};

struct FsmStateMock : public ecu_fsm_state
{

private:
    static enum ecu_fsm_status state1_on_entry(struct ecu_fsm *me)
    {
        mock().actualCall("FsmStateMock::state1_on_entry")
              .withParameter("FsmStateMock", me);
    }

    static void state1_on_exit(struct ecu_fsm *me)
    {
        mock().actualCall("FsmStateMock::state1_on_exit")
              .withParameter("FsmStateMock", me);
    }

    static enum ecu_fsm_status state1_handler(struct ecu_fsm *me, const struct ecu_event *event)
    {
        mock().actualCall("FsmStateMock::state1_handler")
              .withParameter("FsmStateMock", me);

        enum ecu_fsm_status = ECU_FSM_EVENT_HANDLED;

        switch (event->id)
        {
            case TEST_EVENT:
            {
                break;
            }

            default:
            {
                break;
            }
        }
        return status;
    }
};


/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(SingleFsmStateTransition)
{

    struct ecu_event event_; // set id to whatever you want to test.
};




static enum ecu_fsm_status on_entry(struct ecu_fsm *me); // store handler in me that calls actual on_entry method you want to run.
                                                              // whether that's a mock, other behavior etc.


static void on_exit(struct ecu_fsm *me, 
                    ecu_fsm_on_exit_handler handler);


static enum ecu_fsm_status state_handler(struct ecu_fsm *me, 
                                         const struct ecu_event *event);


// normal fsm behavior. verify state transitions via mocks.

// dispatch invalid event to fsm. Verify fsm state handler doesn't run.

// 1. Call ecu_fsm_change_state() outside of state handler. Verify entry and exit events don't run.
// 2. Call ecu_fsm_change_state() multiple times in state handler. Verify only most recent state (and its entry and exit event) is taken
    // and we ignore the past calls.

// 2b. Call ecu_fsm_change_state() multiple times in entry event and also verify above behavior.

// 3. Call ecu_fsm_change_state() in (*on_exit). See what happens.

// 6. Verify max state transitions. Make valid states. State1->State2->State3 but max transitions = 2. Verify
//    State3.on_entry and State2.on_exit were NOT called to verify this.

