/**
 * @file
 * @brief Unit tests for public API functions in @ref timer.h.
 * Test summary:
 * 
 * @ref ecu_timer_active(), @ref ecu_timer_disarm()
 *      - TEST(Timer, TimerDisarm)
 *      - TEST(Timer, TimerDisarmOnInactiveTimer)
 * 
 * @ref ecu_timer_set()
 *      - TEST(Timer, TimerSetDisarmsTimer)
 * 
 * The remaining tests verify tlist servicing under different conditions.
 * They test the remaining functions under test:
 * @ref ecu_tlist_service(), @ref ecu_tlist_timer_arm(), @ref ecu_tlist_timer_rearm():
 * 
 * TList Service Normal Operation:
 *      - TEST(Timer, ServiceOneShotTimeoutTimesAndOrder)
 *      - TEST(Timer, ServicePeriodicTimeoutTimesAndOrder)
 *      - TEST(Timer, ServiceOneShotAndPeriodicTimeoutTimesAndOrder)
 *      - TEST(Timer, ServiceTimerRearm)
 *      - TEST(Timer, ServiceOneShotTimerCallbackReturnFalse)
 *      - TEST(Timer, ServicePeriodicTimerCallbackReturnFalse)
 *      - TEST(Timer, ServiceRearmTimersInCallbackReturnFalse)
 *      - TEST(Timer, ServiceRearmOneShotTimerInCallback)
 *      - TEST(Timer, ServiceDisarmOneShotTimerInCallback)
 *      - TEST(Timer, ServiceRearmPeriodicTimerInCallback)
 *      - TEST(Timer, ServiceDisarmPeriodicTimerInCallback)
 *      - TEST(Timer, ServiceChangeOneShotTimerInCallback)
 *      - TEST(Timer, ServiceChangePeriodicTimerInCallback)
 *      - TEST(Timer, ServiceConvertOneShotToPeriodicTimerInCallback)
 *      - TEST(Timer, ServiceConvertPeriodicToOneShotTimerInCallback)
 * 
 * TList Service Tick Wraparound:
 *      - TEST(Timer, ServiceTickWraparoundOneShotTimers)
 *      - TEST(Timer, ServiceTickWraparoundPeriodicTimers)
 *      - TEST(Timer, ServiceTickWraparoundCallbackReturnFalse)
 *      - TEST(Timer, ServiceTickWraparoundRearmInCallback)
 *      - TEST(Timer, ServiceTickWraparoundRearmInCallbackReservice)
 *      - TEST(Timer, ServiceTickWraparoundGeneralExpireOrderTest)
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
/*----------------------- FILE-SCOPE TYPES -------------------*/
/*------------------------------------------------------------*/

namespace
{
/**
 * @brief C++ wrapper around C timer structure under test (@ref ecu_timer).
 */
struct test_timer : public ecu_timer
{
    /// @brief Default constructor. Sets timer's period, type, and callback
    /// to default values.
    test_timer()
    {
        ecu_timer_ctor(this, &callback, nullptr);
        ecu_timer_set(this, M_DEFAULT_PERIOD, M_DEFAULT_TYPE);
    }

    /// @brief Constructs timer with specified period and type.
    /// Wrapper that calls C constructor under test.
    test_timer(ecu_tick_t timer_period, ecu_timer_type timer_type = ECU_TIMER_TYPE_ONE_SHOT)
    {
        ecu_timer_ctor(this, &callback, nullptr);
        ecu_timer_set(this, timer_period, timer_type);
    }

    /// @brief Allows unit test to inject function call in timer's
    /// expire callback. This method allows tests to execute custom
    /// code within the timer's callback while maintaining its default
    /// behavior (calling a mock).
    ///
    /// @param func Function to call. Must be callable that returns nothing.
    /// @param args Arguments to pass into @p func. Must match signature
    /// of @p func.
    template<class F, class... Args>
    requires std::invocable<F, Args...> && std::is_void_v<std::invoke_result_t<F, Args...>>
    void inject(F&& func, Args&&... args)
    {
        m_injection = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
    }

    /// @brief Make timer's expire callback return either true or false. 
    /// True = callback successful. False = callback retried on next
    /// service. The callback return value will remain the same until
    /// it is altered again by calling this function.
    ///
    /// @param status Timer's expire callback returns this value.
    void callback_successful(bool status)
    {
        m_callback_successul = status;
    }

    /// @brief Default callback that execute when timer expires.
    /// Custom behavior can be injected via @ref inject() and @ref callback_successful().
    static bool callback(ecu_timer *timer, void *obj)
    {
        (void)obj;
        assert( (timer) );
        test_timer *me = static_cast<test_timer *>(timer);

        mock().actualCall("timer_expired")
              .withParameter("timer", static_cast<const void *>(me));

        /* Allows timer to set, armed, disarmed, etc to be injected within expire callback. */
        if (me->m_injection)
        {
            me->m_injection();
        }

        return me->m_callback_successul;
    }

    /// @brief See @ref inject().
    std::function<void()> m_injection;

    /// @brief See @ref callback().
    bool m_callback_successul{true};

    /// @brief Default period assigned to timer if default constructed.
    static constexpr ecu_tick_t M_DEFAULT_PERIOD = 20;

    /// @brief Default type assigned to timer if default constructed.
    static constexpr ecu_timer_type M_DEFAULT_TYPE = ECU_TIMER_TYPE_ONE_SHOT;
};

/**
 * @brief C++ wrapper around C timer list structure under test (@ref ecu_tlist).
 */
struct test_tlist : public ecu_tlist
{
    /// @brief Default constructor.
    test_tlist()
    {
        ecu_tlist_ctor(this);
    }

    /// @brief Helper around @ref ecu_tlist_timer_rearm() that allows
    /// multiple timers to be rearmed at once. Note that function parameter
    /// order does not determine timer order. This is handled internally
    /// by function under test (@ref ecu_tlist_timer_rearm()). 
    ///
    /// @warning Timers must be set using @ref ecu_timer_set() beforehand.
    template<typename... Timers>
    requires (std::is_same_v<test_timer, Timers> && ...)
    void rearm(test_timer& t1, Timers&... tn)
    {
        ecu_tlist_timer_rearm(this, &t1);

        if constexpr(sizeof...(Timers) > 0)
        {
            (ecu_tlist_timer_rearm(this, &tn), ...);
        }
    }
};
}

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Timer)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    /// @brief Sets mock expectation that one timer will expire N times.
    ///
    /// @param t Timer expected to expire.
    /// @param n Number of times timer is expected to expire. 
    static void EXPECT_TIMER_EXPIRED(test_timer& t, unsigned int n)
    {
        mock().expectNCalls(n, "timer_expired")
              .withParameter("timer", static_cast<const void *>(&t));
    }

    /// @brief Sets mock expectation that all supplied timers will
    /// expire in the specified order. I.e. supplying t1, t2, t1, t3 
    /// means t1, t2, t1, t3 will expire in that order.
    template<typename... Timers>
    requires (std::is_same_v<test_timer, Timers> && ...)
    static void EXPECT_TIMER_EXPIRED_IN_ORDER(test_timer &t1, Timers&... tn)
    {
        mock().strictOrder();
        mock().expectOneCall("timer_expired")
              .withParameter("timer", static_cast<const void *>(&t1));

        if constexpr(sizeof...(Timers) > 0)
        {
            (mock().expectOneCall("timer_expired")
                   .withParameter("timer", static_cast<const void *>(&tn)), ...);
        }
    }

    /// @brief Method injected into @ref test_timer.inject for testing purposes.
    /// Runs in timer's callback. Only rearms timer if supplied values are 
    /// different.
    static void rearm_if_different(ecu_tlist *tlist, ecu_timer *t, ecu_tick_t period, ecu_timer_type type)
    {
        assert( (tlist && t) );
        if (t->period != period || t->type != type)
        {
            ecu_tlist_timer_arm(tlist, t, period, type);
        }
    }

    /**@{*/
    /// @brief Test timers and list.
    test_timer t1; /* Start at 1 because that is what CppUTest starts its mock call ordering at. */
    test_timer t2;
    test_timer t3;
    test_timer t4;
    test_timer t5;
    test_tlist tlist;
    /**@}*/

    /// @brief Max value tlist's current tick counter can hold before wrapping around.
    static constexpr ecu_tick_t MAX{ECU_TICK_MAX};
};

/*------------------------------------------------------------*/
/*------ TESTS - ecu_timer_active(), ecu_timer_disarm() ------*/
/*------------------------------------------------------------*/

/**
 * @brief Disarming a timer removes it from the list.
 */
TEST(Timer, TimerDisarm)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tlist_timer_arm(&tlist, &t1, 50, ECU_TIMER_TYPE_PERIODIC);
        CHECK_TRUE( (ecu_timer_active(&t1)) );

        /* Step 2: Action. */
        ecu_timer_disarm(&t1);

        /* Step 3: Assert. */
        CHECK_TRUE( (!ecu_timer_active(&t1)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Disarming a timer that is already disarmed
 * should always be allowed.
 */
TEST(Timer, TimerDisarmOnInactiveTimer)
{
    try
    {
        /* Step 1: Arrange. Precondition must be true to produce useful results. */
        CHECK_FALSE( (ecu_timer_active(&t1)) );

        /* Step 2: Action. */
        ecu_timer_disarm(&t1);

        /* Step 3: Assert. Test also fails if assertion fires. */
        CHECK_FALSE( (ecu_timer_active(&t1)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*---------------------- TESTS - TIMER SET -------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Setting an active timer should always disarm it.
 */
TEST(Timer, TimerSetDisarmsTimer)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        tlist.rearm(t1);

        /* Step 1: Precondition must be true to produce useful results. */
        CHECK_TRUE( (ecu_timer_active(&t1)) );

        /* Step 2: Action. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);

        /* Step 3: Assert. */
        CHECK_FALSE( (ecu_timer_active(&t1)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------- TESTS - TLIST SERVICE NORMAL OPERATION ---------*/
/*------------------------------------------------------------*/

/**
 * @brief One-shot timers only expire once. Expiration
 * order should be based off absolute ticks.
 */
TEST(Timer, ServiceOneShotTimeoutTimesAndOrder)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 15, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 50, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t4, 250, ECU_TIMER_TYPE_ONE_SHOT);
        tlist.rearm(t1, t2, t3, t4);
        EXPECT_TIMER_EXPIRED_IN_ORDER(t1, t2, t3, t4);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 250);
        ecu_tlist_service(&tlist, 250); /* Timers should not expire again. */
        ecu_tlist_service(&tlist, 250); /* Timers should not expire again. */

        /* Step 3: Assert. Test fails if timers expires more than once or wrong expiration order. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Periodic timers should keep expiring. Expiration
 * order should be based off absolute ticks.
 */
TEST(Timer, ServicePeriodicTimeoutTimesAndOrder)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t2, 20, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t3, 250, ECU_TIMER_TYPE_PERIODIC);
        tlist.rearm(t1, t2, t3);
        EXPECT_TIMER_EXPIRED_IN_ORDER(t1, t2, t1, t3, t1, t2, t1, t2);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 15);  /* t1 = expired. Current = 15. t1 = expires @ 25. t2 = expires @ 20. t3 = expires @ 250. */
        ecu_tlist_service(&tlist, 230); /* t2 = expired then t1 = expired. Current = 245. t1 = expires @ 255. t2 = expires @ 265. t3 = expires @ 250. */
        ecu_tlist_service(&tlist, 50);  /* t3 = expired then t1 = expired then t2 = expired. Current = 305. t1 = expires @ 315. t2 = expires @ 325. t3 = expires @ 555. */
        ecu_tlist_service(&tlist, 200); /* t1 = expired then t2 = expired. */

        /* Step 3: Assert. Test fails if wrong expiration order. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief One-shot timers expire only once. Periodic timers
 * keep expiring. Expiration order based off absolute ticks.
 */
TEST(Timer, ServiceOneShotAndPeriodicTimeoutTimesAndOrder)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 25, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t3, 100, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t4, 150, ECU_TIMER_TYPE_PERIODIC);
        tlist.rearm(t1, t2, t3, t4);
        EXPECT_TIMER_EXPIRED_IN_ORDER(t1, t2, t3, t2, t4, t2, t2, t4, t2, t4);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 80);  /* t1 = expired then t2 = expired. Current = 80. t2 = expires @ 105. t3 = expires @ 100. t4 = expires @ 150. */
        ecu_tlist_service(&tlist, 65);  /* t3 = expired then t2 = expired. Current = 145. t2 = expires @ 160. t4 = expires @ 150. */
        ecu_tlist_service(&tlist, 100); /* t4 = expired then t2 = expired. Current = 245. t2 = expires @ 270. t4 = expires @ 395. */
        ecu_tlist_service(&tlist, 500); /* t2 = expired then t4 = expired. Current = 745. t2 = expires @ 770. t4 = expires @ 895. */
        ecu_tlist_service(&tlist, 200); /* t2 = expired then t4 = expired. */

        /* Step 3: Assert. Test fails if wrong expiration order. */
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
TEST(Timer, ServiceTimerRearm)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 30, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 35, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t3, 140, ECU_TIMER_TYPE_ONE_SHOT);
        tlist.rearm(t1, t2, t3);
        EXPECT_TIMER_EXPIRED_IN_ORDER(t2, t2, t1, t3, t2, t2, t1);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 20);  /* Current = 20. t1 = expires @ 30. t2 = expires @ 35. t3 = expires @ 140. */
        tlist.rearm(t1);                /* Current = 20. t1 = expires @ 50. t2 = expires @ 35. t3 = expires @ 140. */
        ecu_tlist_service(&tlist, 20);  /* t2 = expired. Current = 40. t1 = expires @ 50. t2 = expires @ 75. t3 = expires @ 140. */
        tlist.rearm(t1);                /* Current = 40. t1 = expires @ 70. t2 = expires @ 75. t3 = expires @ 140. */
        ecu_tlist_service(&tlist, 25);  /* Current = 65. t1 = expires @ 70. t2 = expires @ 75. t3 = expires @ 140. */
        tlist.rearm(t1);                /* Current = 65. t1 = expires @ 95. t2 = expires @ 75. t3 = expires @ 140. */
        ecu_tlist_service(&tlist, 65);  /* t2 = expired then t1 = expired. Current = 130. t2 = expires @ 165. t3 = expires @ 140. */
        ecu_tlist_service(&tlist, 100); /* t3 = expired then t2 = expired. Current = 230. t2 = expires @ 265. */
        ecu_tlist_service(&tlist, 30);  /* Current = 260. t2 = expires @ 265. */
        tlist.rearm(t2);                /* Current = 260. t2 = expires @ 300. */
        ecu_tlist_service(&tlist, 30);  /* Current = 290. t2 = expires @ 300. */
        tlist.rearm(t1);                /* Current = 290. t1 = expires @ 320. t2 = expires @ 300. */
        ecu_tlist_service(&tlist, 30);  /* t2 = expired then t1 = expired. */

        /* Step 3: Assert. Test fails if wrong expiration order. */
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
 * once callback returns true.
 */
TEST(Timer, ServiceOneShotTimerCallbackReturnFalse)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.callback_successful(false);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 8);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10);    /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        ecu_tlist_service(&tlist, 100);   /* Expire. */
        ecu_tlist_service(&tlist, 50);    /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        t1.callback_successful(true);
        ecu_tlist_service(&tlist, 1);     /* Last expiration. */
        ecu_tlist_service(&tlist, 100);   /* t1 should be removed at this point. */
        ecu_tlist_service(&tlist, 100);   /* t1 should be removed at this point. */

        /* Step 3: Assert. Test fails if timer does not expire properly. */
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
 * once callback returns true.
 */
TEST(Timer, ServicePeriodicTimerCallbackReturnFalse)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.callback_successful(false);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 10);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10);    /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        ecu_tlist_service(&tlist, 50);    /* Expire. */
        ecu_tlist_service(&tlist, 100);   /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        ecu_tlist_service(&tlist, 1);     /* Expire. */
        t1.callback_successful(true);
        ecu_tlist_service(&tlist, 1);     /* Expire. Rearmed. */
        ecu_tlist_service(&tlist, 5);
        ecu_tlist_service(&tlist, 5);     /* Expire. Rearmed. */
        ecu_tlist_service(&tlist, 5);
        ecu_tlist_service(&tlist, 5);     /* Expire. Rearmed. */

        /* Step 3: Assert. Test fails if timer does not expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Rearming should have no effect. Timers should 
 * always expire on next service if callbacks returns false.
 */
TEST(Timer, ServiceRearmTimersInCallbackReturnFalse)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 20, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&ecu_tlist_timer_rearm, &tlist, &t1);
        t2.inject(&ecu_tlist_timer_rearm, &tlist, &t2);
        t1.callback_successful(false);
        t2.callback_successful(false);
        tlist.rearm(t1, t2);
        EXPECT_TIMER_EXPIRED(t1, 10);
        EXPECT_TIMER_EXPIRED(t2, 12);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 20);  /* Both should expire. */
        ecu_tlist_service(&tlist, 1);   /* Both should expire. */
        ecu_tlist_service(&tlist, 1);   /* Both should expire. */
        ecu_tlist_service(&tlist, 20);  /* Both should expire. */
        ecu_tlist_service(&tlist, 1);   /* Both should expire. */
        ecu_tlist_service(&tlist, 1);   /* Both should expire. */
        t1.callback_successful(true);
        ecu_tlist_service(&tlist, 1);   /* Both should expire. */
        ecu_tlist_service(&tlist, 1);   /* Only t2 expires. */
        ecu_tlist_service(&tlist, 1);   /* Only t2 expires. */
        ecu_tlist_service(&tlist, 1);   /* Only t2 expires. */
        t2.callback_successful(true);
        ecu_tlist_service(&tlist, 10);  /* Both expire. */
        ecu_tlist_service(&tlist, 10);  /* Only t1 expires. */
        ecu_tlist_service(&tlist, 10);  /* Both t1 and t2 expire. */

        /* Step 3: Assert. Test fails if timers do not expire properly. */
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
TEST(Timer, ServiceRearmOneShotTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        tlist.rearm(t1);
        t1.inject(&ecu_tlist_timer_rearm, &tlist, &t1);
        EXPECT_TIMER_EXPIRED(t1, 3); /* Timer should be rearmed since user specified that in callback. */

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10); /* Expire. */

        /* Step 3: Assert. Test fails if timer does not expire properly. */
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
TEST(Timer, ServiceDisarmOneShotTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.inject(&ecu_timer_disarm, &t1);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 1);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);

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
TEST(Timer, ServiceRearmPeriodicTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&ecu_tlist_timer_rearm, &tlist, &t1);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 4);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10); /* Expire. */

        /* Step 3: Assert. Test fails if timer does not expire properly. */
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
TEST(Timer, ServiceDisarmPeriodicTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&ecu_timer_disarm, &t1);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 1);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);

        /* Step 3: Assert. Test fails if timer readded. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Rearm timer with different period inside callback.
 * API should handle this.
 */
TEST(Timer, ServiceChangeOneShotTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.inject(&ecu_tlist_timer_arm, &tlist, &t1, 20, ECU_TIMER_TYPE_ONE_SHOT);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 3);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10); /* Expire. */

        /* Step 3: Assert. Test fails if timer does not expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Rearm timer with different period inside callback.
 * API should handle this.
 */
TEST(Timer, ServiceChangePeriodicTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&ecu_tlist_timer_arm, &tlist, &t1, 20, ECU_TIMER_TYPE_PERIODIC);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 3);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10); /* Expire. */

        /* Step 3: Assert. Test fails if timer does not expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should be successfully converted.
 */
TEST(Timer, ServiceConvertOneShotToPeriodicTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.inject(&rearm_if_different, &tlist, &t1, 25, ECU_TIMER_TYPE_PERIODIC);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 3);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. Timer period set to 25. Timer set to periodic. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10); /* Expire. */

        /* Step 3: Assert. Test fails if timer does not expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should be successfully converted.
 */
TEST(Timer, ServiceConvertPeriodicToOneShotTimerInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        t1.inject(&rearm_if_different, &tlist, &t1, 25, ECU_TIMER_TYPE_ONE_SHOT);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 2);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 10); /* Expire. Timer period set to 25. Timer set to one-shot. */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10); /* Expire. Timer removed since now one-shot !!if handled correctly in callback!! */
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);
        ecu_tlist_service(&tlist, 10);

        /* Step 3: Assert. Test fails if timer does not expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------- TESTS - TLIST SERVICE TICK WRAPAROUND ----------*/
/*------------------------------------------------------------*/

/**
 * @brief Some one-shot timers set to expire after tick
 * counter wraparound. Expiration order should still be
 * correct and API should handle this.
 */
TEST(Timer, ServiceTickWraparoundOneShotTimers)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t2, 20, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t3, 50, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_timer_set(&t4, 100, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_service(&tlist, MAX-20); /* About to wraparound. */
        tlist.rearm(t1, t2, t3, t4);
        EXPECT_TIMER_EXPIRED_IN_ORDER(t1, t2, t3, t4);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 500);

        /* Step 3: Assert. Test fails if wrong expiration order. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Some periodic timers are set to expire after tick
 * counter wraparound. Expiration order should still be
 * correct and API should handle this.
 */
TEST(Timer, ServiceTickWraparoundPeriodicTimers)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 20, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t2, 40, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t3, 60, ECU_TIMER_TYPE_PERIODIC);
        ecu_tlist_service(&tlist, MAX-150);
        tlist.rearm(t1, t2, t3);
        EXPECT_TIMER_EXPIRED(t1, 18);
        EXPECT_TIMER_EXPIRED(t2, 9);
        EXPECT_TIMER_EXPIRED(t3, 6);

        /* Step 2: Action. Ensure wraparound. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t3 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired and t3 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t3 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired and t3 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t3 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired. */
        ecu_tlist_service(&tlist, 20); /* t1 = expired and t2 = expired and t3 = expired. */

        /* Step 3: Assert. Test fails if timers did expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief API should handle this. Callback should be 
 * reattempted once after every service until it returns true.
 */
TEST(Timer, ServiceTickWraparoundCallbackReturnFalse)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.callback_successful(false);
        ecu_tlist_service(&tlist, MAX-10);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 8);

        /* Step 2: Arrange. */
        ecu_tlist_service(&tlist, 10); /* Expire. */
        ecu_tlist_service(&tlist, 5);  /* Wraparound. Expire. */
        ecu_tlist_service(&tlist, 1);  /* Expire. */
        ecu_tlist_service(&tlist, 1);  /* Expire. */
        ecu_tlist_service(&tlist, 20); /* Expire. */
        ecu_tlist_service(&tlist, 1);  /* Expire. */
        ecu_tlist_service(&tlist, 1);  /* Expire. */
        t1.callback_successful(true);
        ecu_tlist_service(&tlist, 1);  /* Last expiration. */

        /* Step 3: Assert. Test fails if timers did expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should be rearmed such that is only expires once
 * on the service call.
 */
TEST(Timer, ServiceTickWraparoundRearmInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.inject(&ecu_tlist_timer_rearm, &tlist, &t1);
        ecu_tlist_service(&tlist, MAX-10);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 1);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 500); /* Wraparound. t1 should only expire once. */

        /* Step 3: Assert. Test fails if timers did expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Timer should be rearmed such that is only expires once
 * on the service call. Verify it was rearmed correctly by
 * reservicing at exactly the timeout time.
 */
TEST(Timer, ServiceTickWraparoundRearmInCallbackReservice)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.inject(&ecu_tlist_timer_rearm, &tlist, &t1);
        ecu_tlist_service(&tlist, MAX-10);
        tlist.rearm(t1);
        EXPECT_TIMER_EXPIRED(t1, 2);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 500); /* Wraparound. t1 should only expire once. */
        ecu_tlist_service(&tlist, 1);   /* Should not expire. */
        ecu_tlist_service(&tlist, 5);   /* Should not expire. */
        ecu_tlist_service(&tlist, 4);   /* Expire. */

        /* Step 3: Assert. Test fails if timers did expire properly. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Combine periodic, one-shot, tick wraparound, 
 * false callbacks, and rearming in service in single test.
 */
TEST(Timer, ServiceTickWraparoundGeneralExpireOrderTest)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tlist_service(&tlist, MAX-100);
        ecu_timer_set(&t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        t1.callback_successful(false);
        ecu_timer_set(&t2, 23, ECU_TIMER_TYPE_ONE_SHOT);
        t2.inject(&ecu_tlist_timer_arm, &tlist, &t2, 45, ECU_TIMER_TYPE_PERIODIC);
        ecu_timer_set(&t3, 72, ECU_TIMER_TYPE_PERIODIC);
        tlist.rearm(t1, t2, t3);
        EXPECT_TIMER_EXPIRED_IN_ORDER(t1, t2, t1, t3, t2, t1, t2, t3, t2, t1, t3, t2, t3, t2, t3, t3);

        /* Step 2: Action. */
        ecu_tlist_service(&tlist, 40);  /* t1 = expired then t2 = expired. Current = 40. t1 = always expires. t2 = expires @ 85. t3 = expires @ 72. */
        ecu_tlist_service(&tlist, 45);  /* t1 = expired then t3 = expired then t2 = expired. Current = 85. t1 = always expires. t2 = expires @ 130. t3 = expires @ 157. */
        t1.callback_successful(true);
        t1.inject(&rearm_if_different, &tlist, &t1, 100, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_service(&tlist, 30);  /* Wraparound. t1 = expired. Current = 115. t1 = expires @ 215. t2 = expires @ 130. t3 = expires @ 157. */
        ecu_tlist_service(&tlist, 50);  /* t2 = expired then t3 = expired. Current = 165. t1 = expires @ 215. t2 = expires @ 210. t3 = expires @ 237. */
        ecu_tlist_service(&tlist, 45);  /* t2 = expired. Current = 210. t1 = expires @ 215. t2 = expires @ 255. t3 = expires @ 237. */
        ecu_tlist_service(&tlist, 5);   /* t1 = expired. Current = 215. t2 = expires @ 255. t3 = expires @ 237. */
        t2.inject(&rearm_if_different, &tlist, &t2, 125, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_service(&tlist, MAX); /* Wraparound. t3 = expired then t2 = expired. */
        ecu_tlist_service(&tlist, 125); /* t3 = expired then t2 = expired. */
        ecu_tlist_service(&tlist, 72);  /* t3 = expired. */
        ecu_tlist_service(&tlist, 72);  /* t3 = expired. */

        /* Step 3: Assert. Test fails if expected timers don't expire. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}
