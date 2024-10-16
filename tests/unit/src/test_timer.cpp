/**
 * @file
 * @brief Unit tests for public API functions in @ref timer.h and @ref i_timer.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-13
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib */
#include <concepts>
#include <cstdint>

/* Files under test. */
#include "ecu/timer.h"

/* Mocks */
#include "mocks/mock_asserter.hpp"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STUB AND MOCK DECLARATIONS --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

template<typename T>
concept valid_tick_width = (sizeof(T) <= sizeof(ecu_max_tick_size_t));


/**
 * @brief Supply interface that stubs the user's hardware timer driver.
 * This is a template so we can force compilation error if supplied tick 
 * width is invalid.
 */
template<typename T>
requires std::unsigned_integral<T> && valid_tick_width<T>
class ITimerStub
{
public:
    ITimerStub() : m_tick_width_bytes(sizeof(T))
    {

    }

    void set_ticks(T ticks)
    {
        m_ticks = ticks;
    }

    static ecu_max_tick_size_t get_ticks(void *object)
    {
        ITimerStub *me = static_cast<ITimerStub *>(object);

        /* It is guaranteed sizeof(ecu_max_tick_size_t) >= sizeof(me->m_ticks) so overflow
        due to downcast will never occur. */
        return static_cast<ecu_max_tick_size_t>(me->m_ticks);
    }

public:
    size_t m_tick_width_bytes;

private:
    T m_ticks;
};


/**
 * @brief Mock for @ref ecu_timer.callback that returns true. Tests can choose 
 * between two mocks: one that returns true and one that returns false. This 
 * is used instead of CppUTest's withOutputParameter to provide an easier 
 * interface to work with since only two possible values can be returned.
 */
static bool timer_callback_mock_true(void *object);


/**
 * @brief Mock for @ref ecu_timer.callback that returns false. Tests can choose 
 * between two mocks: one that returns true and one that returns false. This 
 * is used instead of CppUTest's withOutputParameter to provide an easier 
 * interface to work with since only two possible values can be returned.
 */
static bool timer_callback_mock_false(void *object);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STUB AND MOCK DEFINITIONS ---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static bool timer_callback_mock_true(void *object)
{
    mock().actualCall("timer_callback_mock")
          .withParameter("obj", object);

    return true;
}


static bool timer_callback_mock_false(void *object)
{
    mock().actualCall("timer_callback_mock")
          .withParameter("obj", object);

    return false;  
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

class TimerCollectionBase : public Utest
{
public:
    void setup() override 
    {
        m_assert_call_ok.handler = &AssertCallOk::assert_handler;
        m_assert_call_fail.handler = &AssertCallFail::assert_handler;
    }

    void teardown() override
    {
        ecu_timer_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

public:
    AssertCallOk m_assert_call_ok;
    AssertCallFail m_assert_call_fail;

public:
    struct ecu_timer_collection m_collection;
    struct ecu_timer m_timer1;
    struct ecu_timer m_timer2;
    struct ecu_timer m_timer3;
    struct ecu_timer m_timer4;
};


/**
 * @brief Timer collection using uint8_t hardware timer.
 */
TEST_GROUP_BASE(TimerCollectionUI8, TimerCollectionBase)
{
public:
    ITimerStub<uint8_t> m_itimer;
};


/**
 * @brief Timer collection using uint16_t hardware timer.
 */
TEST_GROUP_BASE(TimerCollectionUI16, TimerCollectionBase)
{
public:
    ITimerStub<uint16_t> m_itimer;
};


/**
 * @brief Timer collection using uint32_t hardware timer.
 */
TEST_GROUP_BASE(TimerCollectionUI32, TimerCollectionBase)
{
public:
    ITimerStub<uint32_t> m_itimer;
};


/**
 * @brief Timer collection using uint64_t hardware timer.
 */
TEST_GROUP_BASE(TimerCollectionUI64, TimerCollectionBase)
{
public:
    ITimerStub<uint64_t> m_itimer;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------- TESTS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Construct timers and collection, add timers to collection, and call
 * collector destructor. Repeat process multiple times in a row. Behavior
 * should be defined.
 */
TEST(TimerCollectionUI32, ConstructorDestructorTest)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));
        m_itimer.set_ticks(0);

        /* Steps 2 and 3: Action and assert. */
        /* First sequence. */
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer2, static_cast<void *>(&m_timer2), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer3, static_cast<void *>(&m_timer3), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer4, static_cast<void *>(&m_timer4), &timer_callback_mock_true);
        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer2, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer3, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer4, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_collection_destroy(&m_collection);

        /* Second sequence. */
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer2, static_cast<void *>(&m_timer2), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer3, static_cast<void *>(&m_timer3), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer4, static_cast<void *>(&m_timer4), &timer_callback_mock_true);
        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer2, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer3, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer4, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_collection_destroy(&m_collection);

        /* Third sequence. */
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer2, static_cast<void *>(&m_timer2), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer3, static_cast<void *>(&m_timer3), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer4, static_cast<void *>(&m_timer4), &timer_callback_mock_true);
        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer2, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer3, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer4, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_collection_destroy(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify via timer callback mock.
 */
TEST(TimerCollectionUI32, SingleTimerTimesOutCorrectly)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectNCalls(2, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));
        m_itimer.set_ticks(0);

        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(0, m_timer1.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        /* Timer should not expire here since ticks not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);

        /* Update ticks. Timer should now expire. */
        m_itimer.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&m_collection);

        /* Call couple more times to verify timer doesn't expire since tick counter not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);

        /* Update ticks. Timer should expire again. */
        m_itimer.set_ticks(TICK_INCREMENT*2);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify via timer callback mocks.
 */
TEST(TimerCollectionUI32, MultipleTimersTimeoutCorrectly)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectNCalls(2, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer2));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer3));

        mock().expectNCalls(2, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer4));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));
        m_itimer.set_ticks(0);

        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer2, static_cast<void *>(&m_timer2), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer3, static_cast<void *>(&m_timer3), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer4, static_cast<void *>(&m_timer4), &timer_callback_mock_true);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer2, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT*2));
        ecu_timer_arm(&m_collection, &m_timer3, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT*2));
        ecu_timer_arm(&m_collection, &m_timer4, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));

        CHECK_EQUAL(0, m_timer1.starting_ticks);
        CHECK_EQUAL(0, m_timer2.starting_ticks);
        CHECK_EQUAL(0, m_timer3.starting_ticks);
        CHECK_EQUAL(0, m_timer4.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);
        CHECK_EQUAL((TICK_INCREMENT*2), m_timer2.timeout_ticks);
        CHECK_EQUAL((TICK_INCREMENT*2), m_timer3.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer4.timeout_ticks);

        /* Steps 2 and 3: Action and assert. Timers 2 and 3 should still be active. */
        /* No timers should expire here since ticks not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);

        /* Update ticks. Only timers 1 and 4 should expire once. Call tick multiple
        times to verify timers only expire once. */
        m_itimer.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);

        /* Update ticks. All timers should expire once. Call tick multiple times
        to verify timers only expire once. */
        m_itimer.set_ticks(TICK_INCREMENT*2);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Periodic timers keep expiring and non-periodic timers
 * only expire once.
 */
TEST(TimerCollectionUI32, PeriodicAndNonPeriodicTimers)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        /* Timers 1 and 2 = periodic. Timers 3 and 4 = non-periodic. */
        mock().expectNCalls(5, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        mock().expectNCalls(5, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer2));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer3));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer4));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));
        m_itimer.set_ticks(0);

        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer2, static_cast<void *>(&m_timer2), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer3, static_cast<void *>(&m_timer3), &timer_callback_mock_true);
        ecu_timer_ctor(&m_timer4, static_cast<void *>(&m_timer4), &timer_callback_mock_true);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer2, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer3, false, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&m_collection, &m_timer4, false, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));

        CHECK_EQUAL(0, m_timer1.starting_ticks);
        CHECK_EQUAL(0, m_timer2.starting_ticks);
        CHECK_EQUAL(0, m_timer3.starting_ticks);
        CHECK_EQUAL(0, m_timer4.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer2.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer3.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer4.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        /* Timeout 1 */
        m_itimer.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&m_collection);

        /* Timeout 2 */
        m_itimer.set_ticks(TICK_INCREMENT*2);
        ecu_timer_collection_tick(&m_collection);

        /* Timeout 3 */
        m_itimer.set_ticks(TICK_INCREMENT*3);
        ecu_timer_collection_tick(&m_collection);

        /* Timeout 4 */
        m_itimer.set_ticks(TICK_INCREMENT*4);
        ecu_timer_collection_tick(&m_collection);

        /* Timeout 5 */
        m_itimer.set_ticks(TICK_INCREMENT*5);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief User's timer driver has 8-bit width and counter wraps around.
 * Verify module able to handle this.
 */
TEST(TimerCollectionUI8, UI8TimerOverflowHandled)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));

        /* Set the tick counter to max value right before a wraparound. */
        m_itimer.set_ticks(UINT8_MAX);
        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(UINT8_MAX, m_timer1.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        m_itimer.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&m_collection);

        /* Call tick multiple times to verify timer only expires once since ticks not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief User's timer driver has 16-bit width and counter wraps around.
 * Verify module able to handle this.
 */
TEST(TimerCollectionUI16, UI16TimerOverflowHandled)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));

        /* Set the tick counter to max value right before a wraparound. */
        m_itimer.set_ticks(UINT16_MAX);
        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(UINT16_MAX, m_timer1.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        m_itimer.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&m_collection);

        /* Call tick multiple times to verify timer only expires once since ticks not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief User's timer driver has 32-bit width and counter wraps around.
 * Verify module able to handle this.
 */
TEST(TimerCollectionUI32, UI32TimerOverflowHandled)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));

        /* Set the tick counter to max value right before a wraparound. */
        m_itimer.set_ticks(UINT32_MAX);
        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(UINT32_MAX, m_timer1.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        m_itimer.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&m_collection);

        /* Call tick multiple times to verify timer only expires once since ticks not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief User's timer driver has 64-bit width and counter wraps around.
 * Verify module able to handle this.
 */
TEST(TimerCollectionUI64, UI64TimerOverflowHandled)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));

        /* Set the tick counter to max value right before a wraparound. */
        m_itimer.set_ticks(UINT64_MAX);
        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_true);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);
                                  
        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(UINT64_MAX, m_timer1.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        m_itimer.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&m_collection);

        /* Call tick multiple times to verify timer only expires once since ticks not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify timeout callback isn't called.
 */
TEST(TimerCollectionUI32, DisarmTimerRightBeforeTimeout)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));
        m_itimer.set_ticks(0);
        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_false);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(0, m_timer1.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        /* 1 tick before timeout. */
        m_itimer.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&m_collection);

        ecu_timer_disarm(&m_timer1);

        /* Would timeout if timer it wasn't disarmed. Call tick multiple times to verify this. */
        m_itimer.set_ticks(TICK_INCREMENT+1);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify timer keeps expiring if user callback returns false.
 * Verify timer stops once callback returns true.
 */
TEST(TimerCollectionUI32, CallbackReturningFalse)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectNCalls(4, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&m_timer1));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&m_assert_call_fail));
        m_itimer.set_ticks(0);
        ecu_timer_ctor(&m_timer1, static_cast<void *>(&m_timer1), &timer_callback_mock_false);
        ecu_timer_collection_ctor(&m_collection, 
                                  &m_itimer.get_ticks, 
                                  static_cast<void *>(&m_itimer), 
                                  m_itimer.m_tick_width_bytes);

        ecu_timer_arm(&m_collection, &m_timer1, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(0, m_timer1.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, m_timer1.timeout_ticks);

        /* Steps 2 and 3: Action and asset. */
        /* Should timeout with each tick call since callback returns false. */
        m_itimer.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);

        /* Callback now returns true. Timer callback should only be called on this first tick call. */
        m_timer1.callback = &timer_callback_mock_true;
        ecu_timer_collection_tick(&m_collection);

        /* Afterwards timer callback should not be called here since ticks are not updated. */
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
        ecu_timer_collection_tick(&m_collection);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}
