/**
 * @file
 * @brief Tests for public API functions in @ref fsm.h. This is more of an
 * integration tests than unit test.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-15
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib */
#include <concepts>

/* Files under test. */
#include <ecu/asserter.h>
#include <ecu/event.h>
#include <ecu/fsm.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- FILE-SCOPE TYPES -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

enum EventTestSignals
{
    TEST_STATE_TRANSITION = ECU_USER_EVENT_ID_BEGIN
};


enum class StateIDs : unsigned int
{
    STATE_ENTRY_IDS_START = 0,
    /*****************/
    STATE1_ENTRY,
    STATE2_ENTRY,
    STATE3_ENTRY,
    /*****************/
    STATE_ENTRY_IDS_END,

    STATE_EXIT_IDS_START,
    /*****************/
    STATE1_EXIT,
    STATE2_EXIT,
    STATE3_EXIT,
    /*****************/
    STATE_EXIT_IDS_END
};


struct EventTestBase : public ecu_event 
{
    /* Dummy additional data. */
    int event_data_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ TEMPLATE SPECIALIZATIONS AND CONCEPTS ------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

template<class T>
struct is_ecu_fsm_state_pointer
{
    static constexpr bool value = false;
};


template<>
struct is_ecu_fsm_state_pointer<ecu_fsm_state *>
{
    static constexpr bool value = true;
};


template<const StateIDs ID>
concept is_state_entry_id = ((ID > StateIDs::STATE_ENTRY_IDS_START) && \
                             (ID < StateIDs::STATE_ENTRY_IDS_END));

template<const StateIDs ID>
concept is_state_exit_id = ((ID > StateIDs::STATE_EXIT_IDS_START) && \
                            (ID < StateIDs::STATE_EXIT_IDS_END));



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ STUB AND MOCK DECLARATIONS -----------------------------------------*/
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
public:
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
public:
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)me;
        (void)file;
        (void)line;
        mock().actualCall("AssertMock::assert_handler");
        throw AssertException();
    }
};


struct FsmTestBase : public ecu_fsm
{
public:
    void setup() 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;
    }

    void teardown()
    {
        ecu_fsm_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;

    EventTestBase event_;
    struct ecu_fsm_state STATE1_;
    struct ecu_fsm_state STATE2_;
    struct ecu_fsm_state STATE3_;
};


struct FsmStateEntryMockStub
{
public:
    template<const StateIDs ID>
    requires is_state_entry_id<ID>
    static enum ecu_fsm_status STATE_ON_ENTRY_MOCK(FsmTestBase *me)
    {
        mock().actualCall("FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK")
              .withParameter("Fsm", me)
              .withParameter("State", static_cast<unsigned int>(ID));

        return ECU_FSM_EVENT_HANDLED;
    }

    template<class T*, const StateIDs ID>
    requires is_ecu_fsm_state_pointer<T>::value && is_state_entry_id<ID>
    static enum ecu_fsm_status STATE_ON_ENTRY_TRANSITION_MOCK(FsmTestBase *me)
    {
        mock().actualCall("FsmStateEntryMockStub::STATE_ON_ENTRY_TRANSITION_MOCK")
              .withParameter("Fsm", me)
              .withParameter("State", static_cast<unsigned int>(ID));

        return ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), T);
    }

    static enum ecu_fsm_status STATE_ON_ENTRY_STUB(FsmTestBase *me)
    {
        (void)me;
        return ECU_FSM_EVENT_HANDLED;
    }

    template<class T*>
    requires is_ecu_fsm_state_pointer<T>::value
    static enum ecu_fsm_status STATE_ON_ENTRY_TRANSITION_STUB(FsmTestBase *me)
    {
        return ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), T);
    }
};


struct FsmStateExitMockStub
{
public:
    template<const StateIDs ID>
    requires is_state_exit_id<ID>
    static void STATE_ON_EXIT_MOCK(struct FsmTestBase *me)
    {
        mock().actualCall("FsmStateExitMockStub::STATE_ON_EXIT_MOCK")
              .withParameter("Fsm", me)
              .withParameter("State", static_cast<unsigned int>(ID));
    }

    static void STATE_ON_EXIT_STUB(struct FsmTestBase *me)
    {
        (void)me;
    }
};


struct FsmStateHandlerStub
{
public:
    static enum ecu_fsm_status STATE_HANDLER_STUB(FsmTestBase *me, const struct EventTestBase *event)
    {
        (void)me;
        (void)event;
        return ECU_FSM_EVENT_HANDLED;
    }
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/


TEST_GROUP_BASE(FsmState1ToState2Transition, FsmTestBase)
{
    static enum ecu_fsm_status STATE1_HANDLER(FsmTestBase *me, const struct EventTestBase *event)
    {
        enum ecu_fsm_status status = ECU_FSM_EVENT_HANDLED;

        switch (static_cast<const struct ecu_event *>(event)->id)
        {
            case TEST_STATE_TRANSITION:
            {
                status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE2_);
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


#error "TODO: Stopped here. Need to use TEST_BASE() macro on FsmTestBase class"
#define TEST_GROUP_BASE(testGroup, baseclass) \
  extern int externTestGroup##testGroup; \
  int externTestGroup##testGroup = 0; \
  struct TEST_GROUP_##CppUTestGroup##testGroup : public baseclass

#define TEST_BASE(testBaseClass) \
  struct testBaseClass : public Utest

#define TEST_GROUP(testGroup) \
  TEST_GROUP_BASE(testGroup, Utest)

/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------- TESTS ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST(FsmState1ToState2Transition, SingleStateTransition)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_fsm_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        ecu_fsm_state_ctor(&STATE1_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE1_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE1_EXIT>,
                           (ecu_fsm_state_handler)&STATE1_HANDLER);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


// normal fsm behavior. verify state transitions via mocks.



// dispatch invalid event to fsm. Verify fsm state handler doesn't run.

// 1. Call ecu_fsm_change_state() outside of state handler. Verify entry and exit events don't run.
// 2. Call ecu_fsm_change_state() multiple times in state handler. Verify only most recent state (and its entry and exit event) is taken
    // and we ignore the past calls.

// 2b. Call ecu_fsm_change_state() multiple times in entry event and also verify above behavior.

// 3. Call ecu_fsm_change_state() in (*on_exit). Verify behavior is invalid. how? state will already be changed.

// 6. Verify max state transitions. Make valid states. State1->State2->State3 but max transitions = 2. Verify
//    State3.on_entry and State2.on_exit were NOT called to verify this.

