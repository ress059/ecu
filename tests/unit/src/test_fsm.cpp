/**
 * @file
 * @brief Unit tests for public API functions in @ref fsm.h. 
 * Test Summary:
 * 
 * @ref ECU_FSM_GET_CONTEXT()
 *      - TEST(Fsm, GetContext)
 * 
 * @ref @ref ecu_fsm_start(), @ref ecu_fsm_change_state()
 *      - TEST(Fsm, StartStateHasNoHandler)
 *      - TEST(Fsm, StartNoTransition)
 *      - TEST(Fsm, StartSingleStateTransition)
 *      - TEST(Fsm, StartConsecutiveStateTransition)
 *      - TEST(Fsm, StartTransitionOnFirstExit)
 *      - TEST(Fsm, StartTransitionOnSecondExit)
 *      - TEST(Fsm, StartTransitionOnThirdExit)
 *      - TEST(Fsm, StartSelfTransitionOnFirstEntry)
 *      - TEST(Fsm, StartSelfTransitionOnSecondEntry)
 *      - TEST(Fsm, StartSelfTransitionOnThirdEntry)
 * 
 * @ref @ref ecu_fsm_change_state(), @ref ecu_fsm_dispatch()
 *      - TEST(Fsm, DispatchStateHasNoHandler)
 *      - TEST(Fsm, DispatchSelfTransition)
 *      - TEST(Fsm, DispatchSelfTransitionThenEntryTransition)
 *      - TEST(Fsm, DispatchSingleStateTransition)
 *      - TEST(Fsm, DispatchConsecutiveStateTransition)
 *      - TEST(Fsm, DispatchSingleStateTransitionThenTransitionOnFirstExit)
 *      - TEST(Fsm, DispatchConsecutiveStateTransitionThenTransitionOnSecondExit)
 *      - TEST(Fsm, DispatchSelfTransitionThenSelfTransitionOnFirstEntry)
 *      - TEST(Fsm, DispatchSingleStateTransitionThenSelfTransitionOnFirstEntry)
 *      - TEST(Fsm, DispatchConsecutiveStateTransitionThenSelfTransitionOnSecondEntry)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-14
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <cassert>
#include <type_traits>
#include <vector>

/* Files under test. */
#include "ecu/fsm.h"

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
 * @brief Each ID represents a unique state.
 * @warning If a new state is added, add the ID here and also
 * call its reset routine TEST_GROUP(Fsm)::setup()!
 */
enum state_id
{
    S0,
    S1,
    S2,
    S3,
    S4,
    /********************/
    NUMBER_OF_STATE_IDS
};

/**
 * @brief C++ wrapper around @ref ecu_fsm_state.
 * The @ref ecu_fsm_state under test is a C structure
 * that takes in function pointers for its state handlers.
 * Therefore handlers in this class must be static.
 * This class is then parametrized by a unique state ID
 * so the compiler creates unique instances of each static
 * handler for every state. Because of this, each state
 * is a singleton.
 */
template<state_id CURRENT_STATE>
struct state : public ecu_fsm_state
{
    /// @brief Delete copy semantics since singleton.
    state(const state& other) = delete;

    /// @brief Delete copy semantics since singleton.
    state& operator=(const state& other) = delete;

    /// @brief Delete move semantics since singleton.
    state(state&& other) = delete;

    /// @brief Delete move semantics since singleton.
    state& operator=(state&& other) = delete;

    /// @brief Returns the singleton instance.
    static state<CURRENT_STATE>& get_instance()
    {
        static state<CURRENT_STATE> me;
        return me;
    }

    /// @brief Returns callable that sets mock expectation when 
    /// it executes. The expectation is that this state's entry
    /// handler must be called. Meant to be supplied as a parameter
    /// to EXPECT_STATE_PATH() within the test group.
    std::function<void()> entered()
    {
        static auto expectation = [](){
            /* It is not guaranteed base class pointer == derived class pointer,
            where base class is C structure under test (ecu_fsm_state). Therefore
            this class must be explicitly upcasted before passing into mock. */
            auto& me = state<CURRENT_STATE>::get_instance();
            ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);
            mock().expectOneCall("entry").withParameter("state", static_cast<const void *>(current_state));
        };

        return std::function<void(void)>(expectation);
    }

    /// @brief Returns callable that sets mock expectation when 
    /// it executes. The expectation is that this state's exit
    /// handler must be called. Meant to be supplied as a parameter
    /// to EXPECT_STATE_PATH() within the test group.
    std::function<void()> exited()
    {
        static auto expectation = [](){
            /* It is not guaranteed base class pointer == derived class pointer,
            where base class is C structure under test (ecu_fsm_state). Therefore
            this class must be explicitly upcasted before passing into mock. */
            auto& me = state<CURRENT_STATE>::get_instance();
            ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);
            mock().expectOneCall("exit").withParameter("state", static_cast<const void *>(current_state));
        };

        return std::function<void(void)>(expectation);
    }

    /// @brief Returns callable that sets mock expectation when 
    /// it executes. The expectation is that this state's main handler
    /// function must be called. Meant to be supplied as a parameter
    /// to EXPECT_STATE_PATH() within the test group.
    std::function<void()> handled()
    {
        static auto expectation = [](){
            /* It is not guaranteed base class pointer == derived class pointer,
            where base class is C structure under test (ecu_fsm_state). Therefore
            this class must be explicitly upcasted before passing into mock. */
            auto& me = state<CURRENT_STATE>::get_instance();
            ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);
            mock().expectOneCall("handler").withParameter("state", static_cast<const void *>(current_state));
        };

        return std::function<void(void)>(expectation);
    }

    /// @brief Resets this object to a known state. This must be
    /// called on startup by the test group since each state is
    /// a singleton.
    void reset()
    {
        entry = &this->with_entry_cb;
        exit = &this->with_exit_cb;
        handler = &this->with_handler_cb;
    }

    /// @brief Builder that sets this state's entry handler.
    /// The handler calls the mock expectation set by @ref entered().
    state<CURRENT_STATE>& with_entry()
    {
        entry = &with_entry_cb;
        return *this;
    }

    /// @brief Builder that sets this state's entry handler.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    template<state_id NEW_STATE>
    state<CURRENT_STATE>& with_entry_no_mock_to()
    {
        entry = &with_entry_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's entry handler.
    /// The handler calls the mock expectation set by @ref entered()
    /// then transitions to the new state specified by the template
    /// parameter.
    template<state_id NEW_STATE>
    state<CURRENT_STATE>& with_entry_to()
    {
        entry = &with_entry_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that makes this state not have an entry handler.
    state<CURRENT_STATE>& with_entry_unused()
    {
        entry = ECU_FSM_STATE_ENTRY_UNUSED;
        return *this;
    }

    /// @brief Builder that sets this state's exit handler.
    /// The handler calls the mock expectation set by @ref exited().
    state<CURRENT_STATE>& with_exit()
    {
        exit = &with_exit_cb;
        return *this;
    }

    /// @brief Builder that sets this state's exit handler.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    template<state_id NEW_STATE>
    state<CURRENT_STATE>& with_exit_no_mock_to()
    {
        exit = &with_exit_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's exit handler.
    /// The handler calls the mock expectation set by @ref exited()
    /// then transitions to the new state specified by the template
    /// parameter.
    template<state_id NEW_STATE>
    state<CURRENT_STATE>& with_exit_to()
    {
        exit = &with_exit_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that makes this state not have an exit handler.
    state<CURRENT_STATE>& with_exit_unused()
    {
        exit = ECU_FSM_STATE_EXIT_UNUSED;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler calls the mock expectation set by @ref handled().
    state<CURRENT_STATE>& with_handler()
    {
        handler = &with_handler_cb;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler is a dummy (non-null) function that does nothing.
    state<CURRENT_STATE>& with_handler_no_mock()
    {
        handler = &with_handler_no_mock_cb;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    template<state_id NEW_STATE>
    state<CURRENT_STATE>& with_handler_no_mock_to()
    {
        handler = &with_handler_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler calls the mock expectation set by @ref handled()
    /// then transitions to the new state specified by the template
    /// parameter.
    template<state_id NEW_STATE>
    state<CURRENT_STATE>& with_handler_to()
    {
        handler = &with_handler_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that makes this state not have a main handler function.
    state<CURRENT_STATE>& with_handler_unused()
    {
        handler = nullptr;
        return *this;
    }

    /// @brief Entry handler that runs if state was built with @ref with_entry().
    static void with_entry_cb(ecu_fsm *fsm)
    {
        assert( (fsm) );

        /* It is not guaranteed base class pointer == derived class pointer,
        where base class is C structure under test (ecu_fsm_state). Therefore
        this class must be explicitly upcasted before passing into mock. */
        auto& me = state<CURRENT_STATE>::get_instance();
        ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);

        mock().actualCall("entry")
              .withParameter("state", static_cast<const void *>(current_state));
    }

    /// @brief Entry handler that runs if state was built with @ref with_entry_no_mock_to().
    template<state_id NEW_STATE>
    static void with_entry_no_mock_to_cb(ecu_fsm *fsm)
    {
        assert( (fsm) );
        auto& new_state = state<NEW_STATE>::get_instance();
        ecu_fsm_change_state(fsm, &new_state);
    }

    /// @brief Entry handler that runs if state was built with @ref with_entry_to().
    template<state_id NEW_STATE>
    static void with_entry_to_cb(ecu_fsm *fsm)
    {
        assert( (fsm) );

        /* It is not guaranteed base class pointer == derived class pointer,
        where base class is C structure under test (ecu_fsm_state). Therefore
        this class must be explicitly upcasted before passing into mock. */
        auto& me = state<CURRENT_STATE>::get_instance();
        ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);

        mock().actualCall("entry")
              .withParameter("state", static_cast<const void *>(current_state));

        auto& new_state = state<NEW_STATE>::get_instance();
        ecu_fsm_change_state(fsm, &new_state);
    }

    /// @brief Exit handler that runs if state was built with @ref with_exit().
    static void with_exit_cb(ecu_fsm *fsm)
    {
        assert( (fsm) );

        /* It is not guaranteed base class pointer == derived class pointer,
        where base class is C structure under test (ecu_fsm_state). Therefore
        this class must be explicitly upcasted before passing into mock. */
        auto& me = state<CURRENT_STATE>::get_instance();
        ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);

        mock().actualCall("exit")
              .withParameter("state", static_cast<const void *>(current_state));
    }

    /// @brief Exit handler that runs if state was built with @ref with_exit_no_mock_to().
    template<state_id NEW_STATE>
    static void with_exit_no_mock_to_cb(ecu_fsm *fsm)
    {
        assert( (fsm) );
        auto& new_state = state<NEW_STATE>::get_instance();
        ecu_fsm_change_state(fsm, &new_state);
    }

    /// @brief Exit handler that runs if state was built with @ref with_exit_to().
    template<state_id NEW_STATE>
    static void with_exit_to_cb(ecu_fsm *fsm)
    {
        assert( (fsm) );

        /* It is not guaranteed base class pointer == derived class pointer,
        where base class is C structure under test (ecu_fsm_state). Therefore
        this class must be explicitly upcasted before passing into mock. */
        auto& me = state<CURRENT_STATE>::get_instance();
        ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);

        mock().actualCall("exit")
              .withParameter("state", static_cast<const void *>(current_state));

        auto& new_state = state<NEW_STATE>::get_instance();
        ecu_fsm_change_state(fsm, &new_state);
    }

    /// @brief Main handler that runs if state was built with @ref with_handler().
    static void with_handler_cb(ecu_fsm *fsm, const void *event)
    {
        assert( (fsm) );
        (void)event;

        /* It is not guaranteed base class pointer == derived class pointer,
        where base class is C structure under test (ecu_fsm_state). Therefore
        this class must be explicitly upcasted before passing into mock. */
        auto& me = state<CURRENT_STATE>::get_instance();
        ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);

        mock().actualCall("handler")
              .withParameter("state", static_cast<const void *>(current_state));
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_no_mock().
    static void with_handler_no_mock_cb(ecu_fsm *fsm, const void *event)
    {
        assert( (fsm) );
        (void)event;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_no_mock_to().
    template<state_id NEW_STATE>
    static void with_handler_no_mock_to_cb(ecu_fsm *fsm, const void *event)
    {
        assert( (fsm) );
        (void)event;
        auto& new_state = state<NEW_STATE>::get_instance();
        ecu_fsm_change_state(fsm, &new_state);
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_to().
    template<state_id NEW_STATE>
    static void with_handler_to_cb(ecu_fsm *fsm, const void *event)
    {
        assert( (fsm) );
        (void)event;

        /* It is not guaranteed base class pointer == derived class pointer,
        where base class is C structure under test (ecu_fsm_state). Therefore
        this class must be explicitly upcasted before passing into mock. */
        auto& me = state<CURRENT_STATE>::get_instance();
        ecu_fsm_state *current_state = static_cast<ecu_fsm_state *>(&me);

        mock().actualCall("handler")
              .withParameter("state", static_cast<const void *>(current_state));

        auto& new_state = state<NEW_STATE>::get_instance();
        ecu_fsm_change_state(fsm, &new_state);
    }

private:
    /// @brief Default constructor. Private since singleton.
    state()
    {

    }
};
}

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Fsm)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
        state<S1>::get_instance().reset();
        state<S2>::get_instance().reset();
        state<S3>::get_instance().reset();
        state<S4>::get_instance().reset();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    /// @brief Sets expectation that finite state machine under test
    /// must have the specified transition path (entry, exit, main handler).
    /// The expectations are set in the order they are supplied.
    template<typename... T>
    requires (std::is_same_v<std::function<void()>, T> && ...)
    static void EXPECT_STATE_PATH(std::function<void()> path, T... n)
    {
        mock().strictOrder();

        path();

        if constexpr(sizeof...(n) > 0)
        {
            (n(), ...);
        }
    }

    /// @brief Dummy event to pass into function under test @ref ecu_fsm_dispatch().
    static constexpr std::uint8_t DUMMY_EVENT{0};

    /// @brief FSM under test.
    ecu_fsm me;
};

/*------------------------------------------------------------*/
/*----------------- TESTS - ECU_FSM_GET_CONTEXT --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Convert intrusive fsm into application fsm type.
 * Verifies returned pointer points to start of user's type.
 */
TEST(Fsm, GetContext)
{
    try
    {
        /* Step 1: Arrange. */
        struct app_fsm_t
        {
            std::uint8_t a;
            ecu_fsm fsm;
            int b;
            std::uint8_t c;
        } app_fsm;

        /* Step 2: Action. */
        app_fsm_t *app_fsm_ptr = ECU_FSM_GET_CONTEXT(&app_fsm.fsm, app_fsm_t, fsm);

        /* Step 3: Assert. */
        POINTERS_EQUAL(&app_fsm, app_fsm_ptr);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------------- TESTS - ECU_FSM_START -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not allowed. All states must have a handler.
 */
TEST(Fsm, StartStateHasNoHandler)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_unused()
                                .with_entry()
                                .with_exit();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief FSM started with no transition.
 * 
 * Expect: entry(S0).
 */
TEST(Fsm, StartNoTransition)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry()
                                .with_exit()
                                .with_handler(); 

        EXPECT_STATE_PATH(state0.entered());

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Transition done in entry function.
 * 
 * Expect: entry(S0) -> exit(S0) -> entry(S1).
 */
TEST(Fsm, StartSingleStateTransition)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_to<S1>()
                                .with_exit()
                                .with_handler();

        auto& state1 = state<S1>::get_instance()
                                .with_entry()
                                .with_exit()
                                .with_handler();

        EXPECT_STATE_PATH(state0.entered(), state0.exited(), state1.entered());

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Transition in both entry functions.
 * 
 * Expect: entry(S0) -> exit(S0) -> entry(S1) -> exit(S1) -> entry(S2).
 */
TEST(Fsm, StartConsecutiveStateTransition)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_to<S1>()
                                .with_exit()
                                .with_handler();

        auto& state1 = state<S1>::get_instance()
                                .with_entry_to<S2>()
                                .with_exit()
                                .with_handler();

        auto& state2 = state<S2>::get_instance()
                                .with_entry()
                                .with_exit()
                                .with_handler();

        EXPECT_STATE_PATH(state0.entered(), state0.exited(), state1.entered(), state1.exited(), state2.entered());

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Transition in exit handler not allowed.
 * 
 * exit(S0) transitions to new state. Not allowed..
 * entry(S0) -> exit(S0) ->...assert
 */
TEST(Fsm, StartTransitionOnFirstExit)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_no_mock_to<S1>()
                                .with_exit_no_mock_to<S2>()
                                .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Transition in exit handler not allowed.
 * 
 * exit(S1) transitions to new state. Not allowed..
 * entry(S0) -> exit(S0) -> entry(S1) -> exit(S1) ->...assert
 */
TEST(Fsm, StartTransitionOnSecondExit)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_no_mock_to<S1>()
                                .with_exit_unused()
                                .with_handler();

        (void)state<S1>::get_instance()
                        .with_entry_no_mock_to<S2>()
                        .with_exit_no_mock_to<S2>()
                        .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Transition in exit handler not allowed.
 * 
 * exit(S2) transitions to new state. Not allowed..
 * entry(S0) -> exit(S0) -> entry(S1) -> exit(S1) -> entry(S2) -> exit(S2)...assert
 */
TEST(Fsm, StartTransitionOnThirdExit)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_no_mock_to<S1>()
                                .with_exit_unused()
                                .with_handler();

        (void)state<S1>::get_instance()
                        .with_entry_no_mock_to<S2>()
                        .with_exit_unused()
                        .with_handler();

        (void)state<S2>::get_instance()
                        .with_entry_no_mock_to<S3>()
                        .with_exit_no_mock_to<S3>()
                        .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Self transition in entry handler not allowed.
 * 
 * entry(S0) transitions to self. Not allowed...
 * entry(S0) ->...assert
 */
TEST(Fsm, StartSelfTransitionOnFirstEntry)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_no_mock_to<S0>()
                                .with_exit()
                                .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Self transition in entry handler not allowed.
 * 
 * entry(S1) transitions to self. Not allowed...
 * entry(S0) -> exit(S0) -> entry(S1) ->...assert
 */
TEST(Fsm, StartSelfTransitionOnSecondEntry)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_no_mock_to<S1>()
                                .with_exit_unused()
                                .with_handler();

        (void)state<S1>::get_instance()
                        .with_entry_no_mock_to<S1>()
                        .with_exit()
                        .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Self transition in entry handler not allowed.
 * 
 * entry(S2) transitions to self. Not allowed...
 * entry(S0) -> exit(S0) -> entry(S1) -> exit(S1) -> entry(S2) ->...assert
 */
TEST(Fsm, StartSelfTransitionOnThirdEntry)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_entry_no_mock_to<S1>()
                                .with_exit_unused()
                                .with_handler();

        (void)state<S1>::get_instance()
                        .with_entry_no_mock_to<S2>()
                        .with_exit_unused()
                        .with_handler();

        (void)state<S2>::get_instance()
                        .with_entry_no_mock_to<S2>()
                        .with_exit()
                        .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_start(&me);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - ECU_FSM_DISPATCH ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not allowed. All states must have a handler.
 */
TEST(Fsm, DispatchStateHasNoHandler)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_unused()
                                .with_entry()
                                .with_exit();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Transition to self within handler function.
 * 
 * Expect: handler(S0) -> exit(S0) -> entry(S0)
 */
TEST(Fsm, DispatchSelfTransition)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_to<S0>()
                                .with_exit()
                                .with_entry();

        EXPECT_STATE_PATH(state0.handled(), state0.exited(), state0.entered());

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Transition to self in handler function. Transition
 * to new state in entry function. Edge case can happen when
 * user first constructs FSM with this state.
 * 
 * Expect: handler(S0) -> exit(S0) -> entry(S0) -> exit(S0) -> entry(S1).
 */
TEST(Fsm, DispatchSelfTransitionThenEntryTransition)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_to<S0>()
                                .with_exit()
                                .with_entry_to<S1>();

        auto& state1 = state<S1>::get_instance()
                                .with_entry()
                                .with_exit()
                                .with_handler();

        EXPECT_STATE_PATH(state0.handled(), state0.exited(), state0.entered(), state0.exited(), state1.entered());

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Normal state transition occurs in handler function.
 * 
 * Expect: handler(S0) -> exit(S0) -> entry(S3)
 */
TEST(Fsm, DispatchSingleStateTransition)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_to<S3>()
                                .with_exit()
                                .with_entry();

        auto& state3 = state<S3>::get_instance()
                                .with_entry()
                                .with_exit()
                                .with_handler();

        EXPECT_STATE_PATH(state0.handled(), state0.exited(), state3.entered());

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief State transition occurs in handler function and
 * new state's entry function.
 * 
 * Expect: handler(S0) -> exit(S0) -> entry(S1) -> exit(S1) -> entry(S2) -> exit(S2) -> entry(S3).
 */
TEST(Fsm, DispatchConsecutiveStateTransition)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_to<S1>()
                                .with_exit()
                                .with_entry();

        auto& state1 = state<S1>::get_instance()
                                .with_entry_to<S2>()
                                .with_exit()
                                .with_handler();

        auto& state2 = state<S2>::get_instance()
                                .with_entry_to<S3>()
                                .with_exit()
                                .with_handler();

        auto& state3 = state<S3>::get_instance()
                                .with_entry()
                                .with_exit()
                                .with_handler();

        EXPECT_STATE_PATH(state0.handled(), state0.exited(), state1.entered(), state1.exited(),
                          state2.entered(), state2.exited(), state3.entered());

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Transition in exit handler not allowed.
 * 
 * exit(S0) transitions to new state. Not allowed..
 * handler(S0) -> exit(S0) ->...assert
 */
TEST(Fsm, DispatchSingleStateTransitionThenTransitionOnFirstExit)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_no_mock_to<S1>()
                                .with_exit_no_mock_to<S2>()
                                .with_entry();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Transition in exit handler not allowed.
 * 
 * exit(S1) transitions to new state. Not allowed..
 * handler(S0) -> exit(S0) -> entry(S1) -> exit(S1) ->...assert
 */
TEST(Fsm, DispatchConsecutiveStateTransitionThenTransitionOnSecondExit)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_no_mock_to<S1>()
                                .with_exit_unused()
                                .with_entry();

        (void)state<S1>::get_instance()
                        .with_entry_no_mock_to<S2>()
                        .with_exit_no_mock_to<S2>()
                        .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Do a self-transition. Self transition in entry handler not allowed.
 * 
 * entry(S0) is to self which is not allowed..
 * handler(S0) -> exit(S0) -> entry(S0) -> ...assert.
 */
TEST(Fsm, DispatchSelfTransitionThenSelfTransitionOnFirstEntry)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_no_mock_to<S0>()
                                .with_exit_unused()
                                .with_entry_no_mock_to<S0>();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief State-to-state transition. Self transition in last
 * entry handler not allowed.
 * 
 * entry(S1) is to self which is not allowed..
 * handler(S0) -> exit(S0) -> entry(S1) -> ...assert.
 */
TEST(Fsm, DispatchSingleStateTransitionThenSelfTransitionOnFirstEntry)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_no_mock_to<S1>()
                                .with_exit_unused()
                                .with_entry();

        (void)state<S1>::get_instance()
                        .with_entry_no_mock_to<S1>()
                        .with_exit()
                        .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Consecutive state-to-state transition. Self transition 
 * in last entry handler not allowed.
 * 
 * entry(S2) is to self which is not allowed..
 * handler(S0) -> exit(S0) -> entry(S1) -> exit(S1) -> entry(S2) ->...assert.
 */
TEST(Fsm, DispatchConsecutiveStateTransitionThenSelfTransitionOnSecondEntry)
{
    try
    {
        /* Step 1: Arrange. */
        auto& state0 = state<S0>::get_instance()
                                .with_handler_no_mock_to<S1>()
                                .with_exit_unused()
                                .with_entry();

        (void)state<S1>::get_instance()
                        .with_entry_no_mock_to<S2>()
                        .with_exit_unused()
                        .with_handler();

        (void)state<S2>::get_instance()
                        .with_entry_no_mock_to<S2>()
                        .with_exit()
                        .with_handler();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_fsm_ctor(&me, &state0);
        ecu_fsm_dispatch(&me, &DUMMY_EVENT);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}
