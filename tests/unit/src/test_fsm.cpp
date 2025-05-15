/**
 * @file
 * @brief Unit tests for public API functions in @ref fsm.h. 
 * Test Summary:
 * 
 * @ref ecu_fsm_start(), @ref ecu_fsm_change_state()
 *      - TEST(Fsm, StartNoTransition)
 *      - TEST(Fsm, StartSingleStateTransition)
 *      - TEST(Fsm, StartConsecutiveStateTransitions)
 *      - TEST(Fsm, StartTransitionOnFirstExit)
 *      - TEST(Fsm, StartTransitionOnSecondExit)
 *      - TEST(Fsm, StartSelfTransitionOnFirstEntry)
 *      - TEST(Fsm, StartSelfTransitionOnSecondEntry)
 * 
 * @ref ecu_fsm_change_state(), @ref ecu_fsm_dispatch()
 *      - TEST(Fsm, DispatchSelfTransition)
 *      - TEST(Fsm, DispatchSelfTransitionThenEntryTransition)
 *      - TEST(Fsm, DispatchSingleStateTransition)
 *      - TEST(Fsm, DispatchConsecutiveStateTransitions)
 *      - TEST(Fsm, DispatchTransitionOnFirstExit)
 *      - TEST(Fsm, DispatchTransitionOnSecondExit)
 *      - TEST(Fsm, DispatchSelfTransitionOnFirstEntry)
 *      - TEST(Fsm, DispatchSelfTransitionOnSecondEntry)
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
#include <utility>

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

/* Forward declaration required for handler function definitions. */
struct test_fsm;

struct fsm_base_handler
{
    virtual const SimpleString& name() const = 0;
    virtual ~fsm_base_handler() = default;

    /**
     * @brief Must be ran in state handler function under test.
     * Transitions the FSM into a new state if it was requested
     * by the test by calling @ref to().
     * 
     * @param me FSM in state handler function.
     */
    void process_state_change(test_fsm *me);

    /**
     * @brief Returns true if to() was called. This means
     * the test wants the FSM to change state in its
     * respective handler (entry, exit, handler) function.
     */
    bool state_change_requested() const
    {
        return (m_to < NUMBER_OF_STATE_IDS);
    }

    /**
     * @brief Test should call this if FSM should change state 
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
    fsm_base_handler() = default;
};

struct fsm_state_entry : public fsm_base_handler
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
     * @param fsm This is a test_fsm object upcasted to ecu_fsm
     * base. This is downcasted back into a test_fsm object
     * in order for C++ test code to be compatible with the
     * C ECU library. Having this be a pointer to derived (test_fsm)
     * and upcasting the function pointer is not safe for C++
     * as there are no guarantees &test_fsm == (ecu_fsm *)&test_fsm.
     */
    template<state_id ID>
    static void run(ecu_fsm *fsm);
};

struct fsm_state_exit : public fsm_base_handler
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
     * @param fsm This is a test_fsm object upcasted to ecu_fsm
     * base. This is downcasted back into a test_fsm object
     * in order for C++ test code to be compatible with the
     * C ECU library. Having this be a pointer to derived (test_fsm)
     * and upcasting the function pointer is not safe for C++
     * as there are no guarantees &test_fsm == (ecu_fsm *)&test_fsm.
     */
    template<state_id ID>
    static void run(ecu_fsm *fsm);
};

struct fsm_state_handler : public fsm_base_handler
{
    const SimpleString& name() const override
    {
        static const SimpleString m_name{"handler"};
        return m_name;
    }

    /**
     * @brief Each state's handler function, parameterized by a 
     * state ID.
     * 
     * @tparam ID The state's ID.
     * @param fsm This is a test_fsm object upcasted to ecu_fsm
     * base. This is downcasted back into a test_fsm object
     * in order for C++ test code to be compatible with the
     * C ECU library. Having this be a pointer to derived (test_fsm)
     * and upcasting the function pointer is not safe for C++
     * as there are no guarantees &test_fsm == (ecu_fsm *)&test_fsm.
     * @param event Unused.
     */
    template<state_id ID>
    static void run(ecu_fsm *fsm, const void *event);
};

struct fsm_state : public ecu_fsm_state
{
    using entry_t = void (*)(ecu_fsm *);
    using exit_t = void (*)(ecu_fsm *);
    using handler_t = void (*)(ecu_fsm *, const void *);

    void create(entry_t entry_func, exit_t exit_func, handler_t handler_func)
    {
        entry = (void (*)(ecu_fsm *))entry_func;
        exit = (void (*)(ecu_fsm *))exit_func;
        handler = (void (*)(ecu_fsm *, const void *))handler_func;
    }

    fsm_state_entry m_entry;
    fsm_state_exit m_exit;
    fsm_state_handler m_handler;
};

struct test_fsm : public ecu_fsm, public Utest
{
    test_fsm()
    {
        for (std::size_t i = 0; i < (std::size_t)NUMBER_OF_STATE_IDS; i++)
        {
            states.at(i).create(entry_functions.at(i), exit_functions.at(i), handler_functions.at(i));
        }

        ecu_fsm_ctor(this, &states.at(S0));
    }

    /**
     * @brief Returns state object from supplied ID.
     */
    const fsm_state& state(state_id id)
    {
        return states.at(id);
    }

    /**
     * @brief Returns entry object from supplied ID.
     */
    fsm_state_entry& entry(state_id id)
    {
        return states.at(id).m_entry;
    }

    /**
     * @brief Returns exit object from supplied ID.
     */
    fsm_state_exit& exit(state_id id)
    {
        return states.at(id).m_exit;
    }

    /**
     * @brief Returns handler object from supplied ID.
     */
    fsm_state_handler& handler(state_id id)
    {
        return states.at(id).m_handler;
    }

    /**
     * @brief Returns ID of supplied object.
     * 
     * @param obj Entry, exit, or handler object stored in
     * this fsm.
     */
    state_id id(fsm_base_handler& obj) const
    {
        state_id id = NUMBER_OF_STATE_IDS;

        for (std::size_t i = 0; i < (std::size_t)NUMBER_OF_STATE_IDS; i++)
        {
            const fsm_base_handler *entry = &states.at(i).m_entry;
            const fsm_base_handler *exit = &states.at(i).m_exit;
            const fsm_base_handler *handler = &states.at(i).m_handler;

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
     * test: @ref ecu_fsm_start().
     */
    void start()
    {
        ecu_fsm_start(this);
    }

    /**
     * @brief Wrapper function that calls function under 
     * test: @ref ecu_fsm_dispatch().
     */
    void dispatch()
    {
        ecu_fsm_dispatch(this, &DUMMY_EVENT);
    }

    /**
     * @brief Test should call this function to verify correct
     * state path is taken from fsm under test. 
     * Syntax: EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S1), ...)
     */
    template<typename... Handlers>
    requires (std::is_base_of_v<fsm_base_handler, Handlers> && ...)
    void EXPECT_STATE_PATH(fsm_base_handler& h0, Handlers&... hn)
    {
        mock().expectOneCall(h0.name())
              .withParameter("state", id(h0));

        if constexpr(sizeof...(Handlers) > 0)
        {
            (mock().expectOneCall(hn.name())
                   .withParameter("state", id(hn)), ...);
        }
    }

    /* Instantiates templated functions fsm_state_entry::run, fsm_state_exit::run, and fsm_state_handler::run for all state IDs. */
    template<std::size_t... I>
    static constexpr auto create_entry_functions(std::index_sequence<I...>)
    {
        return std::array<fsm_state::entry_t, sizeof...(I)>{&fsm_state_entry::run<static_cast<state_id>(I)>...};
    }

    template<std::size_t... I>
    static constexpr auto create_exit_functions(std::index_sequence<I...>)
    {
        return std::array<fsm_state::exit_t, sizeof...(I)>{&fsm_state_exit::run<static_cast<state_id>(I)>...};
    }

    template<std::size_t... I>
    static constexpr auto create_handler_functions(std::index_sequence<I...>)
    {
        return std::array<fsm_state::handler_t, sizeof...(I)>{&fsm_state_handler::run<static_cast<state_id>(I)>...};
    }

    static const std::array<fsm_state::entry_t, NUMBER_OF_STATE_IDS> entry_functions;
    static const std::array<fsm_state::exit_t, NUMBER_OF_STATE_IDS> exit_functions;
    static const std::array<fsm_state::handler_t, NUMBER_OF_STATE_IDS> handler_functions;
    static constexpr int DUMMY_EVENT{0};
    std::array<fsm_state, NUMBER_OF_STATE_IDS> states;
};

/* Class variables defined here so the templates are fully instantiated.... */
const std::array<fsm_state::entry_t, NUMBER_OF_STATE_IDS> test_fsm::entry_functions = test_fsm::create_entry_functions(std::make_index_sequence<NUMBER_OF_STATE_IDS>{});
const std::array<fsm_state::exit_t, NUMBER_OF_STATE_IDS> test_fsm::exit_functions = test_fsm::create_exit_functions(std::make_index_sequence<NUMBER_OF_STATE_IDS>{});
const std::array<fsm_state::handler_t, NUMBER_OF_STATE_IDS> test_fsm::handler_functions = test_fsm::create_handler_functions(std::make_index_sequence<NUMBER_OF_STATE_IDS>{});

/* Functions defined here since they require test_fsm's full definition to be visible... */
void fsm_base_handler::process_state_change(test_fsm *me)
{
    assert( (me) );

    if (state_change_requested())
    {
        ecu_fsm_change_state(me, &me->state(m_to));
    }
}

template<state_id ID>
void fsm_state_entry::run(ecu_fsm *fsm)
{
    /* dynamic_cast cannot be performed because test_fsm
    inherits ecu_fsm which is a non-polymorphic C struct. */
    assert( (fsm) );
    test_fsm *me = static_cast<test_fsm *>(fsm);

    auto& entry_obj = me->entry(ID);
    mock().actualCall(entry_obj.name())
          .withParameter("state", ID);
    
    entry_obj.process_state_change(me);
}

template<state_id ID>
void fsm_state_exit::run(ecu_fsm *fsm)
{
    /* dynamic_cast cannot be performed because test_fsm
    inherits ecu_fsm which is a non-polymorphic C struct. */
    assert( (fsm) );
    test_fsm *me = static_cast<test_fsm *>(fsm);

    auto& exit_obj = me->exit(ID);
    mock().actualCall(exit_obj.name())
          .withParameter("state", ID);

    exit_obj.process_state_change(me);
}

template<state_id ID>
void fsm_state_handler::run(ecu_fsm *fsm, const void *event)
{
    /* dynamic_cast cannot be performed because test_fsm
    inherits ecu_fsm which is a non-polymorphic C struct. */
    (void)event;
    assert( (fsm) );
    test_fsm *me = static_cast<test_fsm *>(fsm);

    auto& handler_obj = me->handler(ID);
    mock().actualCall(handler_obj.name())
          .withParameter("state", ID);

    handler_obj.process_state_change(me);
}

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Inherit test_fsm so its member functions are
 * within the test group's namespace.
 */
TEST_GROUP_BASE(Fsm, test_fsm)
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
/*---------------------- TESTS - FSM START -------------------*/
/*------------------------------------------------------------*/

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
        EXPECT_STATE_PATH(entry(S0));

        /* Step 2: Action. */
        start();

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
        entry(S0).to(S1);
        EXPECT_STATE_PATH(entry(S0), exit(S0), entry(S1));

        /* Step 2: Action. */
        start();

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
TEST(Fsm, StartConsecutiveStateTransitions)
{
    try
    {
        /* Step 1: Arrange. */
        entry(S0).to(S1);
        entry(S1).to(S2);
        EXPECT_STATE_PATH(entry(S0), exit(S0), entry(S1), exit(S1), entry(S2));

        /* Step 2: Action. */
        start();

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
 */
TEST(Fsm, StartTransitionOnFirstExit)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        entry(S0).to(S1);
        exit(S0).to(S1);
        EXPECT_STATE_PATH(entry(S0), exit(S0)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        start();

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
 */
TEST(Fsm, StartTransitionOnSecondExit)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        entry(S0).to(S1);
        entry(S1).to(S2);
        exit(S1).to(S2);
        EXPECT_STATE_PATH(entry(S0), exit(S0), entry(S1), exit(S1)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        start();

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
 */
TEST(Fsm, StartSelfTransitionOnFirstEntry)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        entry(S0).to(S0);
        EXPECT_STATE_PATH(entry(S0)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        start();

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
 */
TEST(Fsm, StartSelfTransitionOnSecondEntry)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        entry(S0).to(S1);
        entry(S1).to(S1);
        EXPECT_STATE_PATH(entry(S0), exit(S0), entry(S1)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        start();

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------------- TESTS - FSM DISPATCH ------------------*/
/*------------------------------------------------------------*/

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
        handler(S0).to(S0);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S0));

        /* Step 2: Action. */
        dispatch();

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
        handler(S0).to(S0);
        entry(S0).to(S1);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S0), exit(S0), entry(S1));

        /* Step 2: Action. */
        dispatch();

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
        handler(S0).to(S3);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S3));

        /* Step 2: Action. */
        dispatch();

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
TEST(Fsm, DispatchConsecutiveStateTransitions)
{
    try
    {
        /* Step 1: Arrange. */
        handler(S0).to(S1);
        entry(S1).to(S2);
        entry(S2).to(S3);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S1), exit(S1), entry(S2), exit(S2), entry(S3));

        /* Step 2: Action. */
        dispatch();

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
 */
TEST(Fsm, DispatchTransitionOnFirstExit)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        handler(S0).to(S1);
        exit(S0).to(S1);
        EXPECT_STATE_PATH(handler(S0), exit(S0)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        dispatch();

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
 */
TEST(Fsm, DispatchTransitionOnSecondExit)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        handler(S0).to(S1);
        entry(S1).to(S2);
        exit(S1).to(S2);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S1), exit(S1)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        dispatch();

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
 */
TEST(Fsm, DispatchSelfTransitionOnFirstEntry)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        handler(S0).to(S1);
        entry(S1).to(S1);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S1)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        dispatch();

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
 */
TEST(Fsm, DispatchSelfTransitionOnSecondEntry)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();
        handler(S0).to(S1);
        entry(S1).to(S2);
        entry(S2).to(S2);
        EXPECT_STATE_PATH(handler(S0), exit(S0), entry(S1), exit(S1), entry(S2)); /* Setup mocks to verify assertion fired in proper location. */

        /* Step 2: Action. */
        dispatch();

        /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}
