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
#include <memory>
#include <type_traits>

/* Files under test. */
#include <ecu/event.h>
#include <ecu/fsm.h>

/* Mocks */
#include <mocks/mock_asserter.hpp>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- FILE-SCOPE TYPES -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

enum EventTestSignals
{
    INIT_EVENT = ECU_USER_EVENT_ID_BEGIN,
    TEST_STATE_TRANSITION
};


enum class StateIDs : unsigned int
{
    STATE_HANDLER_IDS_START = 0,
    /*************************/
    STATE1_HANDLER,
    STATE2_HANDLER,
    STATE3_HANDLER,
    /*************************/
    STATE_HANDLER_IDS_END,

    STATE_ENTRY_IDS_START,
    /*************************/
    STATE1_ENTRY,
    STATE2_ENTRY,
    STATE3_ENTRY,
    /*************************/
    STATE_ENTRY_IDS_END,

    STATE_EXIT_IDS_START,
    /*************************/
    STATE1_EXIT,
    STATE2_EXIT,
    STATE3_EXIT,
    /*************************/
    STATE_EXIT_IDS_END,

    STATE_TRANSITION_IDS_START,
    /*************************/
    TO_STATE1,
    TO_STATE2,
    TO_STATE3,
    /*************************/
    STATE_TRANSITION_IDS_END
};


struct EventTestBase : public ecu_event 
{
    EventTestBase()
    {
        ecu_event_ctor(static_cast<struct ecu_event *>(this), INIT_EVENT);
        event_data_ = 0;
    }

    /* Dummy additional data. */
    int event_data_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------ TEMPLATE SPECIALIZATIONS AND CONCEPTS ------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

template<class T>
struct is_fsm_state_pointer
{
    static constexpr bool value = false;
};


template<>
struct is_fsm_state_pointer<struct ecu_fsm_state *>
{
    static constexpr bool value = true;
};


template<const StateIDs ID>
concept is_state_handler_id = ((ID > StateIDs::STATE_HANDLER_IDS_START) && \
                               (ID < StateIDs::STATE_HANDLER_IDS_END));


template<const StateIDs ID>
concept is_state_entry_id = ((ID > StateIDs::STATE_ENTRY_IDS_START) && \
                             (ID < StateIDs::STATE_ENTRY_IDS_END));


template<const StateIDs ID>
concept is_state_exit_id = ((ID > StateIDs::STATE_EXIT_IDS_START) && \
                            (ID < StateIDs::STATE_EXIT_IDS_END));


template<const StateIDs ID>
concept is_state_transition_id = ((ID > StateIDs::STATE_TRANSITION_IDS_START) && \
                                  (ID < StateIDs::STATE_TRANSITION_IDS_END));



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ STUB AND MOCK DECLARATIONS -----------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The actual FSM under test. For safe casting the fsm
 * module requires we inherit ecu_fsm. For this reason, our actual
 * test group FsmTestGroupBase contains this class and does not
 * inherit it since it must inherit UTest class. This approach is
 * cleaner than trying to multiply inherit ecu_fsm and UTest.
 */
struct FsmTestBase : public ecu_fsm
{
public:
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
              .withParameter("Fsm", static_cast<void *>(me))
              .withParameter("State", static_cast<unsigned int>(ID));

        return ECU_FSM_EVENT_HANDLED;
    }

    template<const StateIDs NEWSTATE, const StateIDs ID>
    requires is_state_transition_id<NEWSTATE> && is_state_entry_id<ID>
    static enum ecu_fsm_status STATE_ON_ENTRY_TRANSITION_MOCK(FsmTestBase *me)
    {
        enum ecu_fsm_status status;

        mock().actualCall("FsmStateEntryMockStub::STATE_ON_ENTRY_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(me))
              .withParameter("State", static_cast<unsigned int>(ID));

        if constexpr(NEWSTATE == StateIDs::TO_STATE1)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE1_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE2)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE2_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE3)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE3_);
        }

        return status;
    }

    static enum ecu_fsm_status STATE_ON_ENTRY_STUB(FsmTestBase *me)
    {
        (void)me;
        return ECU_FSM_EVENT_HANDLED;
    }

    template<const StateIDs NEWSTATE>
    requires is_state_transition_id<NEWSTATE>
    static enum ecu_fsm_status STATE_ON_ENTRY_TRANSITION_STUB(FsmTestBase *me)
    {
        enum ecu_fsm_status status;

        if constexpr(NEWSTATE == StateIDs::TO_STATE1)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE1_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE2)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE2_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE3)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE3_);
        }

        return status;
    }
};


struct FsmStateExitMockStub
{
public:
    template<const StateIDs ID>
    requires is_state_exit_id<ID>
    static void STATE_ON_EXIT_MOCK(FsmTestBase *me)
    {
        mock().actualCall("FsmStateExitMockStub::STATE_ON_EXIT_MOCK")
              .withParameter("Fsm", static_cast<void *>(me))
              .withParameter("State", static_cast<unsigned int>(ID));
    }

    template<const StateIDs NEWSTATE, const StateIDs ID>
    requires is_state_transition_id<NEWSTATE> && is_state_exit_id<ID>
    static void STATE_ON_EXIT_TRANSITION_MOCK(FsmTestBase *me)
    {
        mock().actualCall("FsmStateExitMockStub::STATE_ON_EXIT_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(me))
              .withParameter("State", static_cast<unsigned int>(ID));

        if constexpr(NEWSTATE == StateIDs::TO_STATE1)
        {
            (void)ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE1_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE2)
        {
            (void)ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE2_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE3)
        {
            (void)ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE3_);
        }
    }

    static void STATE_ON_EXIT_STUB(FsmTestBase *me)
    {
        (void)me;
    }

    template<const StateIDs NEWSTATE>
    requires is_state_transition_id<NEWSTATE>
    static void STATE_ON_EXIT_TRANSITION_STUB(FsmTestBase *me)
    {
        if constexpr(NEWSTATE == StateIDs::TO_STATE1)
        {
            (void)ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE1_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE2)
        {
            (void)ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE2_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE3)
        {
            (void)ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE3_);
        }
    }
};


struct FsmStateHandlerMockStub
{
public:
    static enum ecu_fsm_status STATE_HANDLER_STUB(FsmTestBase *me, const EventTestBase *event)
    {
        (void)me;
        (void)event;
        return ECU_FSM_EVENT_HANDLED;
    }

    template<const StateIDs ID>
    requires is_state_handler_id<ID>
    static enum ecu_fsm_status STATE_HANDLER_MOCK(FsmTestBase *me, const EventTestBase *event)
    {
        mock().actualCall("FsmStateHandlerMockStub::STATE_HANDLER_MOCK")
              .withParameter("Fsm", static_cast<void *>(me))
              .withParameter("Event", static_cast<const void *>(event))
              .withParameter("State", static_cast<unsigned int>(ID));

        return ECU_FSM_EVENT_HANDLED;
    }

    template<const StateIDs NEWSTATE, const StateIDs ID>
    requires is_state_transition_id<NEWSTATE> && is_state_handler_id<ID>
    static enum ecu_fsm_status STATE_HANDLER_TRANSITION_MOCK(FsmTestBase *me, const EventTestBase *event)
    {
        enum ecu_fsm_status status;

        mock().actualCall("FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(me))
              .withParameter("Event", static_cast<const void *>(event))
              .withParameter("State", static_cast<unsigned int>(ID));

        if constexpr(NEWSTATE == StateIDs::TO_STATE1)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE1_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE2)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE2_);
        }
        else if constexpr(NEWSTATE == StateIDs::TO_STATE3)
        {
            status = ecu_fsm_change_state(static_cast<struct ecu_fsm *>(me), &me->STATE3_);
        }

        return status;
    }
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(FsmTestGroupBase)
{
    void setup()
    {
        me_.assert_call_ok_.handler = &AssertCallOk::assert_handler;
        me_.assert_call_fail_.handler = &AssertCallFail::assert_handler;
    }

    void teardown()
    {
        ecu_fsm_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    FsmTestBase me_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------- TESTS ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Transition into State 2 from State 1's handler. Path
 * should be: State 1 Handler -> State 1 Exit -> State 2 Entry.
 */
TEST(FsmTestGroupBase, SingleStateTransition)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_fsm_set_assert_functor(static_cast<struct ecu_assert_functor *>(&me_.assert_call_fail_));
        mock().strictOrder();

        /* State 1 handler. */
        mock().expectOneCall("FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("Event", static_cast<const void *>(&me_.event_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_HANDLER));

        /* State 1 exit. */
        mock().expectOneCall("FsmStateExitMockStub::STATE_ON_EXIT_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_EXIT));        

        /* State 2 entry. */
        mock().expectOneCall("FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE2_ENTRY));

        /* Set up fsm. */
        ecu_fsm_state_ctor(&me_.STATE1_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE1_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE1_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK<StateIDs::TO_STATE2, StateIDs::STATE1_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE2_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE2_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE2_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE2_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE3_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE3_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE3_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE3_HANDLER>);

        ecu_fsm_ctor(static_cast<struct ecu_fsm *>(&me_),
                     &me_.STATE1_);

        /* Steps 2 and 3: Action and assert. */
        ecu_fsm_dispatch(static_cast<struct ecu_fsm *>(&me_), &me_.event_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Transition into State 2 from State 1's handler. Transition into
 * State 3 from State 2's entry handler. Transition into State 1 from
 * State 3's entry handler. Path should be: State 1 Handler -> 
 * State 1 Exit -> State 2 Entry -> State 2 Exit -> State 3 Entry ->
 * State 3 Exit -> State 1 Entry.
 */
TEST(FsmTestGroupBase, ConsecutiveStateTransitions)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_fsm_set_assert_functor(static_cast<struct ecu_assert_functor *>(&me_.assert_call_fail_));
        mock().strictOrder();

        /* State 1 handler. */
        mock().expectOneCall("FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("Event", static_cast<const void *>(&me_.event_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_HANDLER));

        /* State 1 exit. */
        mock().expectOneCall("FsmStateExitMockStub::STATE_ON_EXIT_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_EXIT));        

        /* State 2 entry. */
        mock().expectOneCall("FsmStateEntryMockStub::STATE_ON_ENTRY_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE2_ENTRY));

        /* State 2 exit. */
        mock().expectOneCall("FsmStateExitMockStub::STATE_ON_EXIT_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE2_EXIT));

        /* State 3 entry. */
        mock().expectOneCall("FsmStateEntryMockStub::STATE_ON_ENTRY_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE3_ENTRY));

        /* State 3 exit. */
        mock().expectOneCall("FsmStateExitMockStub::STATE_ON_EXIT_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE3_EXIT));

        /* State 1 entry. */
        mock().expectOneCall("FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_ENTRY));

        /* Set up fsm. */
        ecu_fsm_state_ctor(&me_.STATE1_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE1_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE1_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK<StateIDs::TO_STATE2, StateIDs::STATE1_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE2_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_TRANSITION_MOCK<StateIDs::TO_STATE3, StateIDs::STATE2_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE2_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE2_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE3_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_TRANSITION_MOCK<StateIDs::TO_STATE1, StateIDs::STATE3_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE3_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE3_HANDLER>);

        ecu_fsm_ctor(static_cast<struct ecu_fsm *>(&me_),
                     &me_.STATE1_);

        /* Steps 2 and 3: Action and assert. */
        ecu_fsm_dispatch(static_cast<struct ecu_fsm *>(&me_), &me_.event_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Self transition back into State 1. Path should be:
 * State 1 Handler -> State 1 Exit -> State 1 Entry.
 */
TEST(FsmTestGroupBase, SelfStateTransition)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_fsm_set_assert_functor(static_cast<struct ecu_assert_functor *>(&me_.assert_call_fail_));
        mock().strictOrder();

        /* State 1 handler. */
        mock().expectOneCall("FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("Event", static_cast<const void *>(&me_.event_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_HANDLER));

        /* State 1 exit. */
        mock().expectOneCall("FsmStateExitMockStub::STATE_ON_EXIT_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_EXIT));        

        /* State 1 entry. */
        mock().expectOneCall("FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_ENTRY));

        /* Set up fsm. */
        ecu_fsm_state_ctor(&me_.STATE1_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE1_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE1_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK<StateIDs::TO_STATE1, StateIDs::STATE1_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE2_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE2_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE2_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE2_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE3_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE3_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE3_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE3_HANDLER>);

        ecu_fsm_ctor(static_cast<struct ecu_fsm *>(&me_),
                     &me_.STATE1_);

        /* Steps 2 and 3: Action and assert. */
        ecu_fsm_dispatch(static_cast<struct ecu_fsm *>(&me_), &me_.event_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Transition into State 2 from State 1's handler. Transition
 * into State 3 from State 1's exit handler. Path should be: 
 * State 1 Handler -> State 1 exit -> State 3 entry.
 * 
 * @note Library strongly discourages changing state in exit handler
 * since you already transitioning into another state at that point.
 * However it is still supported.
 */
TEST(FsmTestGroupBase, StateChangeInExitHandler)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_fsm_set_assert_functor(static_cast<struct ecu_assert_functor *>(&me_.assert_call_fail_));
        mock().strictOrder();

        /* State 1 handler. */
        mock().expectOneCall("FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("Event", static_cast<const void *>(&me_.event_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_HANDLER));

        /* State 1 exit. */
        mock().expectOneCall("FsmStateExitMockStub::STATE_ON_EXIT_TRANSITION_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE1_EXIT));     

        /* State 3 entry. */
        mock().expectOneCall("FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK")
              .withParameter("Fsm", static_cast<void *>(&me_))
              .withParameter("State", static_cast<unsigned int>(StateIDs::STATE3_ENTRY));

        /* Set up fsm. */
        /* State 1 Handler transitions to State 2. */
        /* State 1 exit transitions to State 3. */
        ecu_fsm_state_ctor(&me_.STATE1_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE1_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_TRANSITION_MOCK<StateIDs::TO_STATE3, StateIDs::STATE1_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_TRANSITION_MOCK<StateIDs::TO_STATE2, StateIDs::STATE1_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE2_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE2_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE2_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE2_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE3_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE3_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE3_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE3_HANDLER>);

        ecu_fsm_ctor(static_cast<struct ecu_fsm *>(&me_),
                     &me_.STATE1_);

        /* Steps 2 and 3: Action and assert. */
        ecu_fsm_dispatch(static_cast<struct ecu_fsm *>(&me_), &me_.event_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief FSM should not run and reject the invalid event ID.
 */
TEST(FsmTestGroupBase, InvalidEventIdDispatched)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_fsm_set_assert_functor(static_cast<struct ecu_assert_functor *>(&me_.assert_call_ok_));

        /* Set up fsm. */
        ecu_fsm_state_ctor(&me_.STATE1_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE1_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE1_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE1_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE2_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE2_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE2_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE2_HANDLER>);

        ecu_fsm_state_ctor(&me_.STATE3_, 
                           (ecu_fsm_on_entry_handler)&FsmStateEntryMockStub::STATE_ON_ENTRY_MOCK<StateIDs::STATE3_ENTRY>,
                           (ecu_fsm_on_exit_handler)&FsmStateExitMockStub::STATE_ON_EXIT_MOCK<StateIDs::STATE3_EXIT>,
                           (ecu_fsm_state_handler)&FsmStateHandlerMockStub::STATE_HANDLER_MOCK<StateIDs::STATE3_HANDLER>);

        ecu_fsm_ctor(static_cast<struct ecu_fsm *>(&me_),
                     &me_.STATE1_);

        /* Create invalid (reserved) event ID. */
        me_.event_.id = ECU_VALID_EVENT_ID_BEGIN - 1;

        /* Steps 2 and 3: Action and assert. */
        /* Verify fsm does not run. States are all mocks so mock will go off if it does, thus failing the test. */
        ecu_fsm_dispatch(&me_, &me_.event_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* OK */
    }
}
