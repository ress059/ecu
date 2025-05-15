/**
 * @file
 * @brief Unit tests for public API functions in @ref hsm.h. 
 * Test Summary:
 * 
 * @ref ecu_hsm_ctor(), @ref ecu_hsm_start(), @ref ecu_hsm_change_state()
 *      - TEST(Hsm, StartStateIsTop)
 *      - TEST(Hsm, StartStateIsLeaf)
 *      - TEST(Hsm, StartTransitionOnEntry)
 * 
 * @ref ecu_hsm_ctor(), @ref ecu_hsm_change_state(), @ref ecu_hsm_dispatch()
 *      - TEST(Hsm, DispatchSelfTransition)
 *      - TEST(Hsm, DispatchInnerTransitionHighToLow)
 *      - TEST(Hsm, DispatchInnerTransitionLowToHigh)
 *      - TEST(Hsm, DispatchInnerTransitionSameLevel)
 *      - TEST(Hsm, DispatchIntraTransitionHighToLow)
 *      - TEST(Hsm, DispatchIntraTransitionLowToHigh)
 *      - TEST(Hsm, DispatchIntraTransitionSameLevel)
 *      - TEST(Hsm, DispatchTransitionToParent)
 *      - TEST(Hsm, DispatchTransitionToGrandParent)
 *      - TEST(Hsm, DispatchTransitionToTop)
 *      - TEST(Hsm, DispatchParentHandlers)
 *      - TEST(Hsm, DispatchParentHandlerInnerTransition)
 *      - TEST(Hsm, DispatchParentHandlerIntraTransition)
 *      - TEST(Hsm, DispatchParentHandlerTransitionToTop)
 *      - TEST(Hsm, DispatchParentHandlerTransitionToSelf)
 *      - TEST(Hsm, DispatchParentHandlerTransitionToCurrentState)
 *      - TEST(Hsm, DispatchAllParentHandlersFalse)
 *      - TEST(Hsm, DispatchTransitionOnEntry)
 *      - TEST(Hsm, DispatchTransitionOnExit)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-12
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <cassert>
#include <type_traits>
#include <utility>

/* Files under test. */
#include "ecu/hsm.h"

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

enum state_id
{
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    /********************/
    TOP_STATE,
    NUMBER_OF_STATE_IDS
};

/* Forward declaration required for handler function definitions. */
struct test_hsm;

struct hsm_base_handler
{
    virtual const SimpleString& name() const = 0;
    virtual ~hsm_base_handler() = default;

    /**
     * @brief Must be ran in state handler function under test.
     * Transitions the HSM into a new state if it was requested
     * by the test by calling @ref to().
     * 
     * @param me HSM in state handler function.
     */
    void process_state_change(test_hsm *me);

    /**
     * @brief Returns true if to() was called. This means
     * the test wants the HSM to change state in its
     * respective handler (entry, exit, handler) function.
     */
    bool state_change_requested() const
    {
        return (m_to < NUMBER_OF_STATE_IDS);
    }

    /**
     * @brief Test should call this if HSM should change state 
     * in its respective handler (entry, exit, handler) function.
     * 
     * @param id State to change to.
     */
    void to(state_id id)
    {
        assert( (id >= 0 && id < NUMBER_OF_STATE_IDS) );
        m_to = id;
    }

    state_id m_to{NUMBER_OF_STATE_IDS};

protected:
    hsm_base_handler() = default;
};

struct hsm_state_entry : public hsm_base_handler
{
    const SimpleString& name() const override
    {
        static const SimpleString m_name{"entry"};
        return m_name;
    }

    /**
     * @brief Each state's entry function, parameterized by a 
     * state ID.
     * 
     * @tparam ID The state's ID.
     * @param hsm This is a test_hsm object upcasted to ecu_hsm
     * base. This is downcasted back into a test_hsm object
     * in order for C++ test code to be compatible with the
     * C ECU library. Having this be a pointer to derived (test_hsm)
     * and upcasting the function pointer is not safe for C++
     * as there are no guarantees &test_hsm == (ecu_hsm *)&test_hsm.
     */
    template<state_id ID>
    static void run(ecu_hsm *hsm);
};

struct hsm_state_exit : public hsm_base_handler
{
    const SimpleString& name() const override
    {
        static const SimpleString m_name{"exit"};
        return m_name;
    }

    /**
     * @brief Each state's exit function, parameterized by a 
     * state ID.
     * 
     * @tparam ID The state's ID.
     * @param hsm This is a test_hsm object upcasted to ecu_hsm
     * base. This is downcasted back into a test_hsm object
     * in order for C++ test code to be compatible with the
     * C ECU library. Having this be a pointer to derived (test_hsm)
     * and upcasting the function pointer is not safe for C++
     * as there are no guarantees &test_hsm == (ecu_hsm *)&test_hsm.
     */
    template<state_id ID>
    static void run(ecu_hsm *hsm);
};

struct hsm_state_handler : public hsm_base_handler
{
    const SimpleString& name() const override
    {
        static const SimpleString m_name{"handler"};
        return m_name;
    }

    /**
     * @brief Controls whether handler should return true or
     * false. Defaults to true. True means the event is handled 
     * in this state. False means the event is not handled and
     * should propogate to parent.
     */
    void handled(bool status)
    {
        ret = status;
    }

    /**
     * @brief Each state's handler function, parameterized by a 
     * state ID.
     * 
     * @tparam ID The state's ID.
     * @param hsm This is a test_hsm object upcasted to ecu_hsm
     * base. This is downcasted back into a test_hsm object
     * in order for C++ test code to be compatible with the
     * C ECU library. Having this be a pointer to derived (test_hsm)
     * and upcasting the function pointer is not safe for C++
     * as there are no guarantees &test_hsm == (ecu_hsm *)&test_hsm.
     * @param event Unused.
     */
    template<state_id ID>
    static bool run(ecu_hsm *hsm, const void *event);

    /// @brief Value returned by run() function.
    bool ret{true};
};

struct hsm_state : public ecu_hsm_state
{
    using entry_t = void (*)(ecu_hsm *);
    using exit_t = void (*)(ecu_hsm *);
    using handler_t = bool (*)(ecu_hsm *, const void *);

    void create(entry_t entry_func, exit_t exit_func, handler_t handler_func)
    {
        entry = entry_func;
        exit = exit_func;
        handler = handler_func;
    }

    hsm_state_entry m_entry;
    hsm_state_exit m_exit;
    hsm_state_handler m_handler;
};

struct test_hsm : public ecu_hsm, public Utest
{
    /**
     * @details Test HSM created:
     * 
     * TOP
     * |
     * S0-------S4------S5
     * |                |
     * S1---S3          S6
     * |
     * S2
     */
    test_hsm()
    {
        for (std::size_t i = 0; i < (std::size_t)NUMBER_OF_STATE_IDS; i++)
        {
            states.at(i).create(entry_functions.at(i), exit_functions.at(i), handler_functions.at(i));
        }

        states.at(TOP_STATE).parent = ECU_HSM_STATE_NO_PARENT;
        states.at(S0).parent = &states.at(TOP_STATE);
        states.at(S1).parent = &states.at(S0);
        states.at(S2).parent = &states.at(S1);
        states.at(S3).parent = &states.at(S0);
        states.at(S4).parent = &states.at(TOP_STATE);
        states.at(S5).parent = &states.at(TOP_STATE);
        states.at(S6).parent = &states.at(S5);
        ecu_hsm_ctor(this, &states.at(TOP_STATE), &states.at(TOP_STATE), height);
    }

    /**
     * @brief Returns state object from supplied ID.
     */
    const hsm_state& state(state_id id)
    {
        return states.at(id);
    }

    /**
     * @brief Returns entry object from supplied ID.
     */
    hsm_state_entry& entry(state_id id)
    {
        return states.at(id).m_entry;
    }

    /**
     * @brief Returns exit object from supplied ID.
     */
    hsm_state_exit& exit(state_id id)
    {
        return states.at(id).m_exit;
    }

    /**
     * @brief Returns handler object from supplied ID.
     */
    hsm_state_handler& handler(state_id id)
    {
        return states.at(id).m_handler;
    }

    /**
     * @brief Returns ID of supplied object.
     * 
     * @param obj Entry, exit, or handler object stored in
     * this hsm.
     */
    state_id id(hsm_base_handler& obj) const
    {
        state_id id = NUMBER_OF_STATE_IDS;

        for (std::size_t i = 0; i < (std::size_t)NUMBER_OF_STATE_IDS; i++)
        {
            const hsm_base_handler *entry = &states.at(i).m_entry;
            const hsm_base_handler *exit = &states.at(i).m_exit;
            const hsm_base_handler *handler = &states.at(i).m_handler;

            if (entry == &obj || exit == &obj || handler == &obj)
            {
                id = static_cast<state_id>(i);
                break;
            }
        }

        assert( (id >= 0 && id < NUMBER_OF_STATE_IDS) );
        return id;
    }

    /**
     * @brief Wrapper function that calls function under 
     * test: @ref ecu_hsm_start().
     * 
     * @param id State to start HSM in.
     */
    void start(state_id id)
    {
        ecu_hsm_ctor(this, &state(id), &state(TOP_STATE), height);
        ecu_hsm_start(this);
    }

    /**
     * @brief Wrapper function that calls function under 
     * test: @ref ecu_hsm_dispatch().
     * 
     * @param id HSM starts in this specified state. When
     * event is dispatched this state's handler will first
     * run.
     */
    void dispatch(state_id id)
    {
        ecu_hsm_ctor(this, &state(id), &state(TOP_STATE), height);
        ecu_hsm_dispatch(this, &DUMMY_EVENT);
    }

    /**
     * @brief Test should call this function to verify correct
     * state path is taken from hsm under test. 
     * Syntax: EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S1), ...)
     */
    template<typename... Handlers>
    requires (std::is_base_of_v<hsm_base_handler, Handlers> && ...)
    void EXPECT_STATE_PATH(hsm_base_handler& h0, Handlers&... hn)
    {
        mock().expectOneCall(h0.name())
              .withParameter("state", id(h0));

        if constexpr(sizeof...(Handlers) > 0)
        {
            (mock().expectOneCall(hn.name())
                   .withParameter("state", id(hn)), ...);
        }
    }

    /* Instantiates templated functions hsm_state_entry::run, hsm_state_exit::run, and hsm_state_handler::run for all state IDs. */
    template<std::size_t... I>
    static constexpr auto create_entry_functions(std::index_sequence<I...>)
    {
        return std::array<hsm_state::entry_t, sizeof...(I)>{&hsm_state_entry::run<static_cast<state_id>(I)>...};
    }

    template<std::size_t... I>
    static constexpr auto create_exit_functions(std::index_sequence<I...>)
    {
        return std::array<hsm_state::exit_t, sizeof...(I)>{&hsm_state_exit::run<static_cast<state_id>(I)>...};
    }

    template<std::size_t... I>
    static constexpr auto create_handler_functions(std::index_sequence<I...>)
    {
        return std::array<hsm_state::handler_t, sizeof...(I)>{&hsm_state_handler::run<static_cast<state_id>(I)>...};
    }

    static const std::array<hsm_state::entry_t, NUMBER_OF_STATE_IDS> entry_functions;
    static const std::array<hsm_state::exit_t, NUMBER_OF_STATE_IDS> exit_functions;
    static const std::array<hsm_state::handler_t, NUMBER_OF_STATE_IDS> handler_functions;
    static constexpr int DUMMY_EVENT{0};
    static constexpr uint8_t height{3};
    std::array<hsm_state, NUMBER_OF_STATE_IDS> states;
};

/* Class variables defined here so the templates are fully instantiated.... */
const std::array<hsm_state::entry_t, NUMBER_OF_STATE_IDS> test_hsm::entry_functions = test_hsm::create_entry_functions(std::make_index_sequence<NUMBER_OF_STATE_IDS>{});
const std::array<hsm_state::exit_t, NUMBER_OF_STATE_IDS> test_hsm::exit_functions = test_hsm::create_exit_functions(std::make_index_sequence<NUMBER_OF_STATE_IDS>{});
const std::array<hsm_state::handler_t, NUMBER_OF_STATE_IDS> test_hsm::handler_functions = test_hsm::create_handler_functions(std::make_index_sequence<NUMBER_OF_STATE_IDS>{});

/* Functions defined here since they require test_hsm's full definition to be visible... */
void hsm_base_handler::process_state_change(test_hsm *me)
{
    assert( (me) );

    if (state_change_requested())
    {
        ecu_hsm_change_state(me, &me->state(m_to));
    }
}

template<state_id ID>
void hsm_state_entry::run(ecu_hsm *hsm)
{
    /* dynamic_cast cannot be performed because test_hsm
    inherits ecu_hsm which is a non-polymorphic C struct. */
    assert( (hsm) );
    test_hsm *me = static_cast<test_hsm *>(hsm);

    auto& entry_obj = me->entry(ID);
    mock().actualCall(entry_obj.name())
          .withParameter("state", ID);
    
    entry_obj.process_state_change(me);
}

template<state_id ID>
void hsm_state_exit::run(ecu_hsm *hsm)
{
    /* dynamic_cast cannot be performed because test_hsm
    inherits ecu_hsm which is a non-polymorphic C struct. */
    assert( (hsm) );
    test_hsm *me = static_cast<test_hsm *>(hsm);

    auto& exit_obj = me->exit(ID);
    mock().actualCall(exit_obj.name())
          .withParameter("state", ID);

    exit_obj.process_state_change(me);
}

template<state_id ID>
bool hsm_state_handler::run(ecu_hsm *hsm, const void *event)
{
    /* dynamic_cast cannot be performed because test_hsm
    inherits ecu_hsm which is a non-polymorphic C struct. */
    (void)event;
    assert( (hsm) );
    test_hsm *me = static_cast<test_hsm *>(hsm);

    auto& handler_obj = me->handler(ID);
    mock().actualCall(handler_obj.name())
          .withParameter("state", ID);

    handler_obj.process_state_change(me);
    return handler_obj.ret;
}

/*------------------------------------------------------------*/
/*------------------------- TEST GROUPS ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Inherit test_hsm so its member functions are
 * within the test group's namespace.
 */
TEST_GROUP_BASE(Hsm, test_hsm)
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
};

/*------------------------------------------------------------*/
/*---------------------- TESTS - HSM START -------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Expect: entry(TOP_STATE).
 */
TEST(Hsm, StartStateIsTop)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_STATE_PATH(entry(TOP_STATE));

        /* Step 2: Action. */
        start(TOP_STATE);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Expect: entry(TOP_STATE) -> entry(S0) -> entry(S1)
 */
TEST(Hsm, StartStateIsLeaf)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_STATE_PATH(entry(TOP_STATE), entry(S0), entry(S1));

        /* Step 2: Action. */
        start(S1);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Transition in entry handler not allowed.
 */
TEST(Hsm, StartTransitionOnEntry)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        entry(S0).to(S1);
        EXPECT_STATE_PATH(entry(TOP_STATE), entry(S0)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Arrange. */
        start(S0);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - HSM DISPATCH -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Self state transition. TOP -> TOP.
 * 
 * Expect: handler(TOP_STATE) -> exit(TOP_STATE) -> entry(TOP_STATE)
 */
TEST(Hsm, DispatchSelfTransition)
{
    try
    {
        /* Step 1: Arrange. */
        handler(TOP_STATE).to(TOP_STATE);
        EXPECT_STATE_PATH(handler(TOP_STATE), exit(TOP_STATE), entry(TOP_STATE));

        /* Step 2: Action. */
        dispatch(TOP_STATE);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Inner state transition, meaning LCA is top. 
 * Starting state is at deeper level than ending state.
 * S3 -> S5.
 * 
 * Expect: handler(S3) -> exit(S3) -> exit(S0) -> entry(S5)
 */
TEST(Hsm, DispatchInnerTransitionHighToLow)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S3).to(S5);
        EXPECT_STATE_PATH(handler(S3), exit(S3), exit(S0), entry(S5));
        
        /* Step 2: Action. */
        dispatch(S3);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Inner state transition, meaning LCA is top. 
 * Starting state is at shallower level than ending state.
 * S4 -> S2.
 * 
 * Expect: handler(S4) -> exit(S4) -> entry(S0) -> entry(S1) -> entry(S2)
 */
TEST(Hsm, DispatchInnerTransitionLowToHigh)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S4).to(S2);
        EXPECT_STATE_PATH(handler(S4), exit(S4), entry(S0), entry(S1), entry(S2));

        /* Step 2: Action. */
        dispatch(S4);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Inner state transition, meaning LCA is top. 
 * Starting state is at same level as ending state. 
 * S0 -> S4.
 * 
 * Expect: handler(S0) -> exit(S0) -> entry(S4)
 */
TEST(Hsm, DispatchInnerTransitionSameLevel)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S0).to(S4);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S4));

        /* Step 2: Action. */
        dispatch(S0);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Intra state transition, meaning LCA is NOT top. 
 * Starting state is at deeper level than ending state.
 * S2 -> S3. 
 * 
 * Expect: handler(S2) -> exit(S2) -> exit(S1) -> entry(S3)
 */
TEST(Hsm, DispatchIntraTransitionHighToLow)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S2).to(S3);
        EXPECT_STATE_PATH(handler(S2), exit(S2), exit(S1), entry(S3));

        /* Step 2: Action. */
        dispatch(S2);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Intra state transition, meaning LCA is NOT top.
 * Starting state is at shallower level than ending state. 
 * S3 -> S2.
 * 
 * Expect: handler(S3) -> exit(S3) -> entry(S1) -> entry(S2)
 */
TEST(Hsm, DispatchIntraTransitionLowToHigh)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S3).to(S2);
        EXPECT_STATE_PATH(handler(S3), exit(S3), entry(S1), entry(S2));

        /* Step 2: Action. */
        dispatch(S3);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Intra state transition, meaning LCA is NOT top.
 * Starting state is at same level as ending state. 
 * S1 -> S3. 
 * 
 * Expect: handler(S1) -> exit(S1) -> entry(S3)
 */
TEST(Hsm, DispatchIntraTransitionSameLevel)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S1).to(S3);
        EXPECT_STATE_PATH(handler(S1), exit(S1), entry(S3));

        /* Step 2: Action. */
        dispatch(S1);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief S6 -> S5. 
 * 
 * Expect: handler(S6) -> exit(S6)
 */
TEST(Hsm, DispatchTransitionToParent)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S6).to(S5);
        EXPECT_STATE_PATH(handler(S6), exit(S6));

        /* Step 2: Action. */
        dispatch(S6);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief S2 -> S0. 
 * 
 * Expect: handler(S2) -> exit(S2) -> exit(S1)
 */
TEST(Hsm, DispatchTransitionToGrandParent)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S2).to(S0);
        EXPECT_STATE_PATH(handler(S2), exit(S2), exit(S1));

        /* Step 2: Action. */
        dispatch(S2);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief S2 -> TOP. 
 * 
 * Expect: handler(S2) -> exit(S2) -> exit(S1) -> exit(S0)
 */
TEST(Hsm, DispatchTransitionToTop)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S2).to(TOP_STATE);
        EXPECT_STATE_PATH(handler(S2), exit(S2), exit(S1), exit(S0));

        /* Step 2: Action. */
        dispatch(S2);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Verify parent handers called if user returns false.
 * Start at S6. Handled at TOP.
 */
TEST(Hsm, DispatchParentHandlers)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S6).handled(false);
        handler(S5).handled(false);
        handler(TOP_STATE).handled(true);
        EXPECT_STATE_PATH(handler(S6), handler(S5), handler(TOP_STATE));

        /* Step 2: Action. */
        dispatch(S6);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief State transition signaled by a parent
 * handler. Inner state transition, meaning LCA is top.
 * S4 -> S1. Transition done in TOP. 
 * 
 * Expect: handler(S4) -> handler(TOP) -> exit(S4) -> entry(S0) -> entry(S1)
 */
TEST(Hsm, DispatchParentHandlerInnerTransition)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S4).handled(false);
        handler(TOP_STATE).handled(true);
        handler(TOP_STATE).to(S1);
        EXPECT_STATE_PATH(handler(S4), handler(TOP_STATE), exit(S4), entry(S0), entry(S1));

        /* Step 2: Action. */
        dispatch(S4);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief State transition signaled by a parent handler. 
 * Intra state transition, meaning LCA is NOT top.
 * S2 -> S3. Transition done in S0. 
 * 
 * Expect: handler(S2) -> handler(S1) -> handler(S0) -> exit(S2) -> 
 * exit(S1) -> entry(S3) 
 */
TEST(Hsm, DispatchParentHandlerIntraTransition)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S2).handled(false);
        handler(S1).handled(false);
        handler(S0).handled(true);
        handler(S0).to(S3);
        EXPECT_STATE_PATH(handler(S2), handler(S1), handler(S0), exit(S2), exit(S1), entry(S3));

        /* Step 2: Action. */
        dispatch(S2);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief State transition to top signaled by a parent handler.
 * S3 -> TOP. Transition done in S0.
 * 
 * Expect: handler(S3) -> handler(S0) -> exit(S3) -> exit(S0)
 */
TEST(Hsm, DispatchParentHandlerTransitionToTop)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S3).handled(false);
        handler(S0).handled(true);
        handler(S0).to(TOP_STATE);
        EXPECT_STATE_PATH(handler(S3), handler(S0), exit(S3), exit(S0));

        /* Step 2: Action. */
        dispatch(S3);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Parent handler signals state transition to itself.
 * S2 -> S1. Transition to S1 done in S1.
 * 
 * Expect: handler(S2) -> handler(S1) -> exit(S2)
 */
TEST(Hsm, DispatchParentHandlerTransitionToSelf)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S2).handled(false);
        handler(S1).handled(true);
        handler(S1).to(S1);
        EXPECT_STATE_PATH(handler(S2), handler(S1), exit(S2));

        /* Step 2: Action. */
        dispatch(S2);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Parent handler signals state transition to 
 * the current state.
 * S2 -> S1. Transition to S2 done in S1.
 * 
 * Expect: handler(S2) -> handler(S1) -> exit(S2) -> entry(S2)
 */
TEST(Hsm, DispatchParentHandlerTransitionToCurrentState)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S2).handled(false);
        handler(S1).handled(true);
        handler(S1).to(S2);
        EXPECT_STATE_PATH(handler(S2), handler(S1), exit(S2), entry(S2));

        /* Step 2: Action. */
        dispatch(S2);

        /* Step 3: Assert. Fails if State Path was incorrect. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. At least one parent handler must 
 * return true in state hierachy. Start at S6.
 * 
 * Expect: handler(S6) -> handler(S5) -> handler(TOP) -> assert
 */
TEST(Hsm, DispatchAllParentHandlersFalse)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        handler(S6).handled(false);
        handler(S5).handled(false);
        handler(TOP_STATE).handled(false);
        /* Setup mocks to verify assertion fired in proper location. */
        EXPECT_STATE_PATH(handler(S6), handler(S5), handler(TOP_STATE));

        /* Step 2: Action. */
        dispatch(S6);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Transition in entry handler not allowed.
 * S4 -> S5. Transition in S5 entry. 
 * 
 * Expect: handler(S4) -> exit(S4) -> entry(S5) -> assert
 */
TEST(Hsm, DispatchTransitionOnEntry)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        handler(S4).to(S5);
        entry(S5).to(S6);
        /* Setup mocks to verify assertion fired in proper location. */
        EXPECT_STATE_PATH(handler(S4), exit(S4), entry(S5));

        /* Step 2: Action. */
        dispatch(S4);

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
 * S3 -> S0. Transition in S3 exit.
 * 
 * Expect: handler(S3) -> exit(S3) -> assert
 */
TEST(Hsm, DispatchTransitionOnExit)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        handler(S3).to(S0);
        exit(S3).to(TOP_STATE);
        EXPECT_STATE_PATH(handler(S3), exit(S3)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        dispatch(S3);

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}
