/**
 * @file
 * @brief Unit tests for public API functions in @ref timer.h and @ref i_timer.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-13
 * @copyright Copyright (c) 2024
 */


/* STDLib */
#include <concepts>
#include <cstdint>

/* Files under test. */
#include <ecu/asserter.h>
#include <ecu/timer.h>
#include <ecu/interface/itimer.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- STUB AND MOCK DECLARATIONS ----------------------------------------*/
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


template<typename T>
concept valid_tick_width = (sizeof(T) <= sizeof(ecu_max_tick_size_t));


template<typename T>
requires std::unsigned_integral<T> && valid_tick_width<T>
struct TimerDriverStub : public i_ecu_timer
{
public:
    TimerDriverStub() : ticks_(0)
    {
       i_ecu_timer_ctor(static_cast<struct i_ecu_timer *>(this), sizeof(T), &get_time);
    }

    void set_ticks(T ticks)
    {
        ticks_ = ticks;
    }

    static constexpr size_t get_size()
    {
        return sizeof(T);
    }

private:
    T ticks_;

    static ecu_max_tick_size_t get_time(struct i_ecu_timer *me)
    {
        /* Return current time then increment afterwards. */
        TimerDriverStub *instance = static_cast<TimerDriverStub *>(me);
        return static_cast<ecu_max_tick_size_t>(instance->ticks_);
    }
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
/*------------------------------------------------------- STUB AND MOCK DEFINITIONS -----------------------------------------*/
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

TEST_GROUP(TimerCollection)
{
    virtual void setup() override 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;
    }

    virtual void teardown() override
    {
        ecu_timer_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        i_ecu_timer_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;

    struct ecu_timer_collection collection_;
    struct ecu_timer timer1_;
    struct ecu_timer timer2_;
    struct ecu_timer timer3_;
    struct ecu_timer timer4_;

    TimerDriverStub<uint8_t> timer_source_ui8_;
    TimerDriverStub<uint16_t> timer_source_ui16_;
    TimerDriverStub<uint32_t> timer_source_ui32_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------- TESTS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Construct timers and collection, add timers to collection, and call
 * collector destructor. Repeat process multiple times in a row. Behavior
 * should be defined.
 */
TEST(TimerCollection, ConstructorDestructorTest)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        
        timer_source_ui32_.set_ticks(0);

        /* Steps 2 and 3: Action and assert. */
        /* First sequence. */
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer2_, &timer2_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer3_, &timer3_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer4_, &timer4_, &timer_callback_mock_true);
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer2_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer3_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer4_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_collection_destroy(&collection_);

        /* Second sequence. */
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer2_, &timer2_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer3_, &timer3_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer4_, &timer4_, &timer_callback_mock_true);
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer2_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer3_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer4_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_collection_destroy(&collection_);

        /* Third sequence. */
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer2_, &timer2_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer3_, &timer3_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer4_, &timer4_, &timer_callback_mock_true);
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer2_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer3_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer4_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_collection_destroy(&collection_);
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
TEST(TimerCollection, SingleTimerTimesOutCorrectly)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectNCalls(2, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer1_));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        timer_source_ui32_.set_ticks(0);

        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(0, timer1_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        /* Timer should not expire here since ticks not updated. */
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);

        /* Update ticks. Timer should now expire. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&collection_);

        /* Call couple more times to verify timer doesn't expire since tick counter not updated. */
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);

        /* Update ticks. Timer should expire again. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT*2);
        ecu_timer_collection_tick(&collection_);
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
TEST(TimerCollection, MultipleTimersTimeoutCorrectly)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectNCalls(2, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer1_));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer2_));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer3_));

        mock().expectNCalls(2, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer4_));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        timer_source_ui32_.set_ticks(0);

        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer2_, &timer2_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer3_, &timer3_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer4_, &timer4_, &timer_callback_mock_true);
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer2_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT*2));
        ecu_timer_arm(&collection_, &timer3_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT*2));
        ecu_timer_arm(&collection_, &timer4_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));

        CHECK_EQUAL(0, timer1_.starting_ticks);
        CHECK_EQUAL(0, timer2_.starting_ticks);
        CHECK_EQUAL(0, timer3_.starting_ticks);
        CHECK_EQUAL(0, timer4_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);
        CHECK_EQUAL((TICK_INCREMENT*2), timer2_.timeout_ticks);
        CHECK_EQUAL((TICK_INCREMENT*2), timer3_.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer4_.timeout_ticks);

        /* Steps 2 and 3: Action and assert. Timers 2 and 3 should still be active. */
        /* No timers should expire here since ticks not updated. */
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);

        /* Update ticks. Only timers 1 and 4 should expire once. Call tick multiple
        times to verify timers only expire once. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);

        /* Update ticks. All timers should expire once. Call tick multiple times
        to verify timers only expire once. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT*2);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
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
TEST(TimerCollection, PeriodicAndNonPeriodicTimers)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        /* Timers 1 and 2 = periodic. Timers 3 and 4 = non-periodic. */
        mock().expectNCalls(5, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer1_));

        mock().expectNCalls(5, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer2_));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer3_));

        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer4_));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        timer_source_ui32_.set_ticks(0);

        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer2_, &timer2_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer3_, &timer3_, &timer_callback_mock_true);
        ecu_timer_ctor(&timer4_, &timer4_, &timer_callback_mock_true);

        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer2_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer3_, false, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        ecu_timer_arm(&collection_, &timer4_, false, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));

        CHECK_EQUAL(0, timer1_.starting_ticks);
        CHECK_EQUAL(0, timer2_.starting_ticks);
        CHECK_EQUAL(0, timer3_.starting_ticks);
        CHECK_EQUAL(0, timer4_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer2_.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer3_.timeout_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer4_.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        /* Timeout 1 */
        timer_source_ui32_.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&collection_);

        /* Timeout 2 */
        timer_source_ui32_.set_ticks(TICK_INCREMENT*2);
        ecu_timer_collection_tick(&collection_);

        /* Timeout 3 */
        timer_source_ui32_.set_ticks(TICK_INCREMENT*3);
        ecu_timer_collection_tick(&collection_);

        /* Timeout 4 */
        timer_source_ui32_.set_ticks(TICK_INCREMENT*4);
        ecu_timer_collection_tick(&collection_);

        /* Timeout 5 */
        timer_source_ui32_.set_ticks(TICK_INCREMENT*5);
        ecu_timer_collection_tick(&collection_);
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
TEST(TimerCollection, U8TimerOverflowHandled)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer1_));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        /* Set the tick counter to max value right before a wraparound. */
        timer_source_ui8_.set_ticks(UINT8_MAX);

        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui8_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(UINT8_MAX, timer1_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        timer_source_ui8_.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&collection_);

        /* Call tick multiple times to verify timer only expires once since ticks not updated. */
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
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
TEST(TimerCollection, U16TimerOverflowHandled)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer1_));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        /* Set the tick counter to max value right before a wraparound. */
        timer_source_ui16_.set_ticks(UINT16_MAX);

        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui16_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(UINT16_MAX, timer1_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        timer_source_ui16_.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&collection_);

        /* Call tick multiple times to verify timer only expires once since ticks not updated. */
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
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
TEST(TimerCollection, U32TimerOverflowHandled)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer1_));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        /* Set the tick counter to max value right before a wraparound. */
        timer_source_ui32_.set_ticks(UINT32_MAX);

        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_true);
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(UINT32_MAX, timer1_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&collection_);

        /* Call tick multiple times to verify timer only expires once since ticks not updated. */
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
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
TEST(TimerCollection, DisarmTimerRightBeforeTimeout)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        timer_source_ui32_.set_ticks(0);
        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_false);
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(0, timer1_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);

        /* Steps 2 and 3: Action and assert. */
        /* 1 tick before timeout. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT-1);
        ecu_timer_collection_tick(&collection_);

        ecu_timer_disarm(&timer1_);

        /* Would timeout if timer it wasn't disarmed. Call tick multiple times to verify this. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT+1);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
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
TEST(TimerCollection, CallbackReturningFalse)
{
    static uint8_t constexpr TICK_INCREMENT = 10; 

    try
    {
        /* Step 1: Arrange. */
        mock().expectNCalls(4, "timer_callback_mock")
              .withParameter("obj", static_cast<void *>(&timer1_));

        ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        i_ecu_timer_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        timer_source_ui32_.set_ticks(0);

        ecu_timer_ctor(&timer1_, &timer1_, &timer_callback_mock_false);
        ecu_timer_collection_ctor(&collection_, static_cast<struct i_ecu_timer *>(&timer_source_ui32_));
        ecu_timer_arm(&collection_, &timer1_, true, static_cast<ecu_max_tick_size_t>(TICK_INCREMENT));
        CHECK_EQUAL(0, timer1_.starting_ticks);
        CHECK_EQUAL(TICK_INCREMENT, timer1_.timeout_ticks);

        /* Steps 2 and 3: Action and asset. */
        /* Should timeout with each tick call since callback returns false. */
        timer_source_ui32_.set_ticks(TICK_INCREMENT);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);

        /* Callback now returns true. Timer callback should only be called on this first tick call. */
        timer1_.callback = &timer_callback_mock_true;
        ecu_timer_collection_tick(&collection_);

        /* Afterwards timer callback should not be called here since ticks are not updated. */
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
        ecu_timer_collection_tick(&collection_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}
