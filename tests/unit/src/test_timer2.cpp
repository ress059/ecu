/**
 * @file
 * @brief Unit tests for public API functions in @ref timer.h.
 * Test summary:
 * 
 * @ref ecu_timer_set()
 *      - Set armed timer.
 * 
 * @ref ecu_timer_disarm()
 *      - Disarm already disarmed timer.
 *      - Disarm armed timer.
 * 
 * @ref ecu_timer_is_active()
 *      - Already tested in ecu_timer_set() and ecu_timer_disarm().
 * 
 * @ref ecu_tlist_timer_arm()
 *      - TEST(Timers, TimerArmSameTimestampsAscendingPeriods)
 *      - TEST(Timers, TimerArmDifferentTimestampsAndPeriods)
 *      - TEST(Timers, TimerArmExpiredTimerInList)
 *      - TEST(Timers, TimerArmAddU16TimerToU8ListPeriodInRange)
 *      - TEST(Timers, TimerArmAddU16TimerToU8ListPeriodOutOfRange)
 *      - TEST(Timers, TimerArmTickWraparound)
 *      - TEST(Timers, TimerRearm)
 * 
 * @ref ecu_tlist_service()
 *      - One-shot timeout. 
 *      - Periodic timeout.
 *      - Callback returns false.
 *      - Rearm timer within expire callback.
 *      - Set periodic timer as one-shot within expire callback.
 *      - Tick counter wraparound (UINT8, UINT16, UINT32)
 * 
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-04-07
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib */
#include <cassert>
#include <functional>
#include <vector>
#include <type_traits>
#include <utility>

/* Files under test. */
#include "ecu/timer.h"

/* Stubs. */
#include "stubs/stub_asserter.hpp"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*------------------------- NAMESPACES -----------------------*/
/*------------------------------------------------------------*/

using namespace stubs;

/*------------------------------------------------------------*/
/*------------------- TEMPLATE SPECIALIZATIONS ---------------*/
/*------------------------------------------------------------*/

template<ecu_timer_resolution T>
struct timer_resolution
{
};

template<>
struct timer_resolution<ECU_TIMER_RESOLUTION_8BIT>
{
    typedef std::uint8_t type;
    static constexpr type MAX = UINT8_MAX;
};

template<>
struct timer_resolution<ECU_TIMER_RESOLUTION_16BIT>
{
    typedef std::uint16_t type;
    static constexpr type MAX = UINT16_MAX;
};

template<>
struct timer_resolution<ECU_TIMER_RESOLUTION_32BIT>
{
    typedef std::uint32_t type;
    static constexpr type MAX = UINT32_MAX;
};

template<>
struct timer_resolution<ECU_TIMER_RESOLUTION_64BIT>
{
    typedef std::uint64_t type;
    static constexpr type MAX = UINT64_MAX;
};

/*------------------------------------------------------------*/
/*----------------------- FILE-SCOPE TYPES -------------------*/
/*------------------------------------------------------------*/

struct test_timer : public ecu_timer
{
    test_timer()
    {
        ecu_timer_ctor(this, DEFAULT_PERIOD, DEFAULT_TYPE, &callback, this);
    }

    test_timer(ecu_timer_tick_t timer_period, ecu_timer_type timer_type = ECU_TIMER_TYPE_ONE_SHOT)
    {
        ecu_timer_ctor(this, timer_period, timer_type, &callback, this);
    }

    template<class F, class... Args>
    requires std::invocable<F, Args...> && std::is_void_v<std::invoke_result_t<F, Args...>>
    void inject(F&& func, Args&&... args)
    {
        injection = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
#warning "TODO: Test code...Parmaeters aligned here but misaligned once called in timer callback.."
        // if constexpr(sizeof...(Args) == 4)
        // {
        //     TESTPARAMS(args...);
        // }
    }

    // void TESTPARAMS(ecu_tlist& tlist, test_timer& t, ecu_timer_tick_t period, ecu_timer_type type)
    // {
    //     ;
    // }

    /**
     * @brief Executes when timer expires.
     */
    static bool callback(void *obj)
    {
        assert( (obj) );
        test_timer *me = static_cast<test_timer *>(obj);

        mock().actualCall("timer_expired")
              .withParameter("timer", me);

        /* Allows timer to set, armed, disarmed, etc to be injected within expire callback. */
        if (me->injection)
        {
            me->injection();
        }

        return me->callback_return_val;
    }

    std::function<void()> injection;
    bool callback_return_val{true};
    static constexpr ecu_timer_tick_t DEFAULT_PERIOD = 20;
    static constexpr ecu_timer_type DEFAULT_TYPE = ECU_TIMER_TYPE_ONE_SHOT;
};

template<ecu_timer_resolution T>
struct test_tlist : public ecu_tlist
{
    typedef timer_resolution<T>::type tick_t;

    test_tlist()
    {
        ecu_tlist_ctor(this, T, &get_tick_count, this);
    }

    /**
     * @brief Helper around @ref ecu_tlist_timer_arm() that allows
     * multiple timers to be armed at once. Note that function parameter
     * order does not determine timer order. This is handled internally
     * by function under test (@ref ecu_tlist_timer_arm()).
     */
    void arm(test_timer& t)
    {
        ecu_tlist_timer_arm(this, &t);
    }

    /**
     * @brief Helper around @ref ecu_tlist_timer_arm() that allows
     * multiple timers to be armed at once. Note that function parameter
     * order does not determine timer order. This is handled internally
     * by function under test (@ref ecu_tlist_timer_arm()).
     */
    template<typename... Timers>
    requires (std::is_same_v<test_timer, Timers> && ...)
    void arm(test_timer& t1, Timers&... timers)
    {
        arm(t1);
        (arm(timers), ...); /* Never empty since arm(test_timer& t) overload. */
    }

    /**
     * @brief Sets the "hardware timer's" current tick count.
     */
    void set_time(tick_t current_time)
    {
        current_ticks = current_time;
    }

    /**
     * @brief Elapses the "hardware timer" by incrementing its 
     * current tick count by supplied amount. 
     */
    void elapsed(tick_t elapsed_ticks)
    {
        /* Simulate hardware timer incrementing its ticks. Wraparound via unsigned overflow. */
        current_ticks += elapsed_ticks;
    }

    /**
     * @brief Wrapper around @ref ecu_tlist_service() that allows tests
     * to provide elapsed ticks.
     */
    void service(tick_t elapsed_ticks)
    {
        elapsed(elapsed_ticks);

        /* tlist serviced called after "elapsed_ticks" amount of time. */
        ecu_tlist_service(this);
    }

    void CHECK_TIMER_ORDER()
    {
        std::size_t tnumber = 1;
        ecu_dlist_const_iterator citerator;

        ECU_DLIST_CONST_FOR_EACH(tnode, &citerator, &dlist)
        {
            /* CppUTest stores names in memory so we can pass temporary strings. */
            SimpleString name = SimpleString("t") + SimpleString(std::to_string(tnumber).c_str()); /* t1, t2, t3, etc. */
            tnumber++;

            const test_timer *actual = static_cast<const test_timer *>(ECU_DNODE_GET_CONST_ENTRY(tnode, ecu_timer, dnode));
            mock("timer_order").actualCall(name)
                               .withParameter("timer", actual);

        }
    }

    /**
     * @brief Returns tick count from the "hardware timer".
     */
    static ecu_timer_tick_t get_tick_count(void *obj)
    {
        assert( (obj) );
        test_tlist *me = static_cast<test_tlist *>(obj);
        return (ecu_timer_tick_t)me->current_ticks;
    }

    tick_t current_ticks{0};
};

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Timers)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock("timer_order").checkExpectations();
        mock().checkExpectations();

        mock("timer_order").clear();
        mock().clear();
    }

    /**
     * @brief Expect timers in list to be ordered the same as
     * passed function parameters. For example if t1, t3, t2 is
     * passed it is expected that timers be in the order t1, t3, t2.
     */
    template<typename... Timers>
    requires (std::is_same_v<test_timer, Timers> && ...)
    static void EXPECT_TIMER_ORDER(test_timer& t1, Timers&... timers)
    {
        mock("timer_order").strictOrder();
        EXPECT_TIMER_ORDER(std::index_sequence_for<test_timer, Timers...>{}, t1, timers...);
    }

    template<typename... Timers>
    requires (std::is_same_v<test_timer, Timers> && ...)
    static void EXPECT_TIMER_EXPIRED(test_timer& t, Timers&... timers)
    {
        mock().expectOneCall("timer_expired")
              .withParameter("timer", &t);

        if constexpr(sizeof...(Timers) > 0)
        {
            (EXPECT_TIMER_EXPIRED(timers), ...);
        }
    }

    static void EXPECT_TIMER_EXPIRED(test_timer& t, unsigned int n)
    {
        mock().expectNCalls(n, "timer_expired")
              .withParameter("timer", &t);
    }

    template<typename... Timers>
    requires (std::is_same_v<test_timer, Timers> && ...)
    static void EXPECT_TIMER_EXPIRED_IN_ORDER(test_timer &t, Timers&... timers)
    {
        mock().strictOrder();
        EXPECT_TIMER_EXPIRED(t, timers...);
    }

    template<typename... Timers>
    requires (std::is_same_v<test_timer, Timers> && ...)
    static void set_callback_return_val(bool callback_return_val, test_timer& t1, Timers&... timers)
    {
        t1.callback_return_val = callback_return_val;

        if constexpr(sizeof...(Timers) > 0)
        {
            (set_callback_return_val(callback_return_val, timers), ...);
        }
    }

    static void reset_and_rearm(ecu_tlist *tlist, ecu_timer *t, ecu_timer_tick_t period, ecu_timer_type type)
    {
        ecu_timer_set(t, period, type);
        ecu_tlist_timer_arm(tlist, t);
    }

    test_timer t1; /* Start at 1 because that is what CppUTest starts its mock call ordering at. */
    test_timer t2;
    test_timer t3;
    test_timer t4;
    test_timer t5;
    test_tlist<ECU_TIMER_RESOLUTION_8BIT> u8_list;
    test_tlist<ECU_TIMER_RESOLUTION_16BIT> u16_list;
    test_tlist<ECU_TIMER_RESOLUTION_32BIT> u32_list;

private:
    template<std::size_t... I, typename... Timers>
    static void EXPECT_TIMER_ORDER(std::index_sequence<I...>&&, Timers&... timers)
    {
        (mock("timer_order")
             .expectOneCall(("t" + std::to_string(I + 1)).c_str()) /* t1, t2, t3, etc. */
             .withParameter("timer", static_cast<const void*>(&timers)), ...);
    }
};

/*------------------------------------------------------------*/
/*---------------------- TESTS - TIMER SET -------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Setting an active timer should always disarm it.
 */
TEST(Timers, TimerSetDisarmsTimer)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        u8_list.arm(t1);

        /* Step 1: Precondition must be true to produce useful results. */
        CHECK_TRUE( (ecu_timer_is_active(&t1)) );

        /* Step 2: Action. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);

        /* Step 3: Assert. */
        CHECK_FALSE( (ecu_timer_is_active(&t1)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - TIMER DISARM -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Disarming a timer that is already disarmed
 * should always be allowed.
 */
TEST(Timers, TimerDisarmOnInactiveTimer)
{
    try
    {
        /* Step 1: Arrange. Precondition must be true to produce useful results. */
        CHECK_FALSE( (ecu_timer_is_active(&t1)) );

        /* Step 2: Action. */
        ecu_timer_disarm(&t1);

        /* Step 3: Assert. Test also fails if assertion fires. */
        CHECK_FALSE( (ecu_timer_is_active(&t1)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - TLIST TIMER ARM ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief List should be ordered based off of ticks
 * remaining until timeout. In this case, the period.
 */
TEST(Timers, TimerArmSameTimestampsAscendingPeriods)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 20, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 30, ECU_TIMER_TYPE_ONE_SHOT);
        EXPECT_TIMER_ORDER(t1, t2, t3);

        /* Step 2: Action. */
        u8_list.arm(t3, t1, t2);

        /* Step 3: Assert. */
        u8_list.CHECK_TIMER_ORDER();
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief List should be ordered based off of ticks
 * remaining until timeout.
 */
TEST(Timers, TimerArmDifferentTimestampsAndPeriods)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 100, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 30, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 50, ECU_TIMER_TYPE_ONE_SHOT);
        EXPECT_TIMER_ORDER(t1, t2, t3);

        /* Step 2: Action. 
        t1 = 10 ticks left.
        t2 = 15 ticks left.
        t3 = 50 ticks left. */
        u8_list.arm(t1);
        u8_list.set_time(75);
        u8_list.arm(t2);
        u8_list.set_time(90);
        u8_list.arm(t3);

        /* Step 3: Assert. */
        u8_list.CHECK_TIMER_ORDER();
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Expired timer should at be at front of list. 
 * Remaining list should be ordered based off of ticks 
 * remaining until timeout.
 */
TEST(Timers, TimerArmExpiredTimerInList)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 70, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 200, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 30, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t4, 5, ECU_TIMER_TYPE_ONE_SHOT);
        EXPECT_TIMER_ORDER(t1, t3, t4, t2);

        /* Step 2: Action. 
        t1 = expired.
        t2 = 95 ticks left.
        t3 = 1 tick left.
        t4 = 5 ticks left. */
        u8_list.arm(t1);
        u8_list.set_time(25);
        u8_list.arm(t2);
        u8_list.set_time(100);
        u8_list.arm(t3);
        u8_list.set_time(129);
        u8_list.arm(t4);

        /* Step 3: Assert. */
        u8_list.CHECK_TIMER_ORDER();
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timers should be successfully added in proper 
 * order to U8 list.
 */
TEST(Timers, TimerArmAddU16TimerToU8ListPeriodInRange)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 20, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 15, ECU_TIMER_TYPE_ONE_SHOT);
        EXPECT_TIMER_ORDER(t1, t3, t2);

        /* Step 1: Arrange. 
        t1 = 10 ticks left.
        t2 = 20 ticks left.
        t3 = 15 ticks left. */
        u8_list.arm(t1, t2);
        u16_list.set_time(60000); /* Ticks > u8_list resolution. */
        u16_list.arm(t3);

        /* Step 2: Action. */
        u8_list.arm(t3);

        /* Step 3: Assert. */
        u8_list.CHECK_TIMER_ORDER();
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timers's period exceed UINT8_MAX so assertion 
 * should fire.
 */
TEST(Timers, TimerArmAddU16TimerToU8ListPeriodOutOfRange)
{
    try
    {
        /* Step 1: Arrange. */
        set_assert_handler(AssertResponse::OK);
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, UINT8_MAX + 1, ECU_TIMER_TYPE_ONE_SHOT); /* Period out of range of 8-bit resolution. */
        EXPECT_TIMER_ORDER(t1); /* t2 should not be added to u8_list. */

        /* Step 1: Arrange. */
        u8_list.arm(t1);
        u16_list.arm(t2);

        /* Step 2: Action. */
        u8_list.arm(t2);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }

    /* Step 3: Assert. Verify t1 was not added to u8_list. Do this outside 
    try-catch since control flow jumps to catch after Step 2. */
    u8_list.CHECK_TIMER_ORDER();
}

/**
 * @brief Timers's current ticks wraps around. List should
 * handle this and maintain proper order.
 */
TEST(Timers, TimerArmTickWraparound)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 50, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 30, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 40, ECU_TIMER_TYPE_ONE_SHOT);
        EXPECT_TIMER_ORDER(t2, t1, t3);

        /* Step 2: Action. 
        t1 = 34 ticks left.
        t2 = 30 ticks left.
        t3 = 40 ticks left. */
        u8_list.set_time(250);
        u8_list.arm(t1);
        u8_list.set_time(10);
        u8_list.arm(t2, t3);

        /* Step 3: Assert. */
        u8_list.CHECK_TIMER_ORDER();
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Rearming should reset the timer.
 */
TEST(Timers, TimerRearm)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 30, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 40, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 45, ECU_TIMER_TYPE_ONE_SHOT);
        EXPECT_TIMER_ORDER(t2, t3, t1);

        /* Step 2: Action. After rearming:
        t1 = 30 ticks left.
        t2 = 20 ticks left.
        t3 = 25 ticks left. */
        u8_list.arm(t1, t2, t3);
        u8_list.set_time(20);
        u8_list.arm(t1); /* Rearming t1 should reset it. */

        /* Step 3: Assert. */
        u8_list.CHECK_TIMER_ORDER();
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - TLIST SERVICE ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief One-shot timers that expire should only 
 * expire once.
 */
TEST(Timers, ServiceOneShotTimeout)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 50, ECU_TIMER_TYPE_ONE_SHOT);
        u8_list.arm(t1, t2);
        EXPECT_TIMER_EXPIRED(t1, t2);

        /* Step 2: Action. 20ms elapsed. 
        t1 = expire only once.
        t2 = expire only once.
        t3 = should not expire. */
        u8_list.service(10); /* Both timers should expire. */
        u8_list.service(10); /* Verify both timer do not expire again since they are one-shot. */

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Periodic timers should keep expiring.
 */
TEST(Timers, ServicePeriodicTimeout)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t2, 20, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t3, 100, ECU_TIMER_TYPE_PERIODIC);
        u8_list.arm(t1, t2, t3);
        EXPECT_TIMER_EXPIRED(t1, 4);
        EXPECT_TIMER_EXPIRED(t2, 2);

        /* Step 2: Action. 40ms elapsed.
        t1 = expire 4 times.
        t2 = expire 2 times.
        t3 = should not expire. */
        u8_list.service(10);
        u8_list.service(10);
        u8_list.service(10);
        u8_list.service(10);

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Verify timeouts are in proper order.
 */
TEST(Timers, ServiceOneShotAndPeriodicTimeoutOrder)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 11, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t3, 15, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t4, 32, ECU_TIMER_TYPE_ONE_SHOT);
        u8_list.arm(t1, t2, t3, t4);
        EXPECT_TIMER_EXPIRED_IN_ORDER(t1, t2, t3, t2, t4, t3, t5, t2, t3);

        /* Step 2: Action. */
        u8_list.service(10); /* t1 = expire. t2 = 1 left. t3 = 5 left. t4 = 22 left. */
        u8_list.service(10); /* t2 = expire then t3 = expire. t4 = 12 left. */
        u8_list.service(10); /* t2 = 1 left. t3 = 5 left. t4 = 2 left. */
        ecu_timer_set(&t5, 10, ECU_TIMER_TYPE_ONE_SHOT);
        u8_list.arm(t5);
        u8_list.service(10); /* t2 = expire then t4 = expire then t3 = expire then t5 = expire. */
        u8_list.service(40); /* t2 = expire then t3 = expire. */

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should always expire on next service if
 * callback returns false. Timer should be removed
 * if callback returns true.
 */
TEST(Timers, ServiceOneShotTimerCallbackReturnValue)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        set_callback_return_val(false, t1);
        u8_list.arm(t1);
        EXPECT_TIMER_EXPIRED(t1, 5);

        /* Step 2: Action. */
        u8_list.service(10);    /* Expire. */
        u8_list.service(0);     /* Expire. */
        u8_list.service(0);     /* Expire. */
        u8_list.service(0);     /* Expire. */
        set_callback_return_val(true, t1);
        u8_list.service(0);     /* Last expiration. */
        u8_list.service(0);     /* t1 should be removed at this point. */
        u8_list.service(0);     /* t1 should be removed at this point. */

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should always expire on next service if
 * callback returns false. Timer should be rearmed (reset)
 * if callback returns true.
 */
TEST(Timers, ServicePeriodicTimerCallbackReturnValue)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        set_callback_return_val(false, t1);
        u8_list.arm(t1);
        EXPECT_TIMER_EXPIRED(t1, 7);

        /* Step 2: Action. */
        u8_list.service(10);    /* Expire. */
        u8_list.service(0);     /* Expire. */
        u8_list.service(0);     /* Expire. */
        u8_list.service(0);     /* Expire. */
        set_callback_return_val(true, t1);
        u8_list.service(0);     /* Expire. */
        u8_list.service(5);
        u8_list.service(5);     /* Expire. */
        u8_list.service(5);
        u8_list.service(5);     /* Expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief API should be able to handle this. Timer rearmed
 * appropriately.
 */
TEST(Timers, ServiceArmOneShotTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        u8_list.arm(t1);
        t1.inject(&ecu_tlist_timer_arm, &u8_list, &t1);
        EXPECT_TIMER_EXPIRED(t1, 3); /* Timer should be rearmed since user specified that in callback. */

        /* Step 2: Action. */
        u8_list.service(10); /* Expire. */
        u8_list.service(10); /* Expire. */
        u8_list.service(10); /* Expire. */

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Should have no effect.
 */
TEST(Timers, ServiceDisarmOneShotTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.inject(&ecu_timer_disarm, &t1);
        u8_list.arm(t1);
        EXPECT_TIMER_EXPIRED(t1, 1);

        /* Step 2: Action. */
        u8_list.service(10); /* Expire. */
        u8_list.service(10);
        u8_list.service(10);

        /* Step 3: Assert. Test fails if timer not removed. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Should have no effect since timer is periodic.
 * Verify expiration timing is still accurate.
 */
TEST(Timers, ServiceArmPeriodicTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&ecu_tlist_timer_arm, &u8_list, &t1);
        u8_list.arm(t1);
        EXPECT_TIMER_EXPIRED(t1, 4);

        /* Step 2: Action. */
        u8_list.service(10); /* Expire. */
        u8_list.service(10); /* Expire. */
        u8_list.service(10); /* Expire. */
        u8_list.service(10); /* Expire. */

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should not be readded.
 */
TEST(Timers, ServiceDisarmPeriodicTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&ecu_timer_disarm, &t1);
        u8_list.arm(t1);
        EXPECT_TIMER_EXPIRED(t1, 1);

        /* Step 2: Action. */
        u8_list.service(10); /* Expire. */
        u8_list.service(10);
        u8_list.service(10);
        u8_list.service(10);

        /* Step 3: Assert. Test fails if timer readded. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should be successfully reset and readded
 * to list.
 */
TEST(Timers, ServiceResetAndRearmOneShotToPeriodicTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.inject(&reset_and_rearm, &u8_list, &t1, 25, ECU_TIMER_TYPE_PERIODIC);
        u8_list.arm(t1);
        EXPECT_TIMER_EXPIRED(t1, 3);

        /* Step 2: Action. */
        u8_list.service(10); /* Expire. Timer period set to 25. Timer set to periodic. */
        u8_list.service(10);
        u8_list.service(10);
        u8_list.service(10); /* Expire. */
        u8_list.service(10);
        u8_list.service(10);
        u8_list.service(10); /* Expire. */

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should be successfully reset and readded
 * to list.
 */
TEST(Timers, ServiceResetAndRearmPeriodicToOneShotTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&reset_and_rearm, &u8_list, &t1, 25, ECU_TIMER_TYPE_ONE_SHOT);
        u8_list.arm(t1);
        EXPECT_TIMER_EXPIRED(t1, 2);

        /* Step 2: Action. */
        u8_list.service(10); /* Expire. Timer period set to 25. Timer set to one-shot. */
        u8_list.service(10);
        u8_list.service(10);
        u8_list.service(10); /* Expire. Timer removed since now one-shot. */
        u8_list.service(10);
        u8_list.service(10);
        u8_list.service(10);

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}










// timer shouldn't be readded.
TEST(Timers, ServiceSetPeriodicTimerAsOneShotInCallback)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

// timer should be added.
TEST(Timers, ServiceSetOneShotTimerAsPeriodicInCallback)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

TEST(Timers, ServiceTickWraparound)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}



// General order test will be the massive one. combine periodic, one-shot, tick wraparound, bad callbacks, etc. 
// /**
//  * @brief API should be able to handle this. Timer rearmed
//  * in correct order.
//  */
// TEST(Timers, ServiceRearmTimerInCallbackCorrectOrder)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
//         ecu_timer_set(&t2, 21, ECU_TIMER_TYPE_ONE_SHOT); /* Will rearm in callback. */
//         ecu_timer_set(&t3, 31, ECU_TIMER_TYPE_PERIODIC);
//         u8_list.arm(t1, t2, t3);
//         t2.inject(&ecu_tlist_timer_arm, &u8_list, &t2);
//         EXPECT_TIMER_EXPIRED_IN_ORDER(t1, t1, t2, t1, t3, t1, t1, t2, t1);

//         /* Step 2: Action. */
//         u8_list.service(10); /* Elapsed = 10. t1 = expire. t2 = 10 left. t3 = 20 left. */
//         u8_list.service(10); /* Elapsed = 20. t1 = expire. t2 = 1 left. t3 = 11 left. */
//         u8_list.service(10); /* Elapsed = 30. t2 = expire. t1 = expire. t3 = 1 left. */
//         u8_list.service(10); /* Elapsed = 40. t3 = expire. t1 = expire. t2 = 11 left. */
//         u8_list.service(10); /* Elapsed = 50. t1 = expire. t2 = 1 left. t3 = 21 left. */
//         u8_list.service(10); /* Elapsed = 60. t2 = expire. t1 = expire. t3 = 11 left. */

//         /* Step 3: Assert. Test fails if expected timers don't expire. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }
