/**
 * @file
 * @brief Unit tests for public API functions in @ref fsm.h. 
 * Test Summary:
 * 
 * TODO!!!!
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
#include <functional>
#include <type_traits>

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

template<state_id ID>
struct test_state : public ecu_fsm_state
{
    test_state()
    {
        static constexpr std::string S{"S"};
        static constexpr std::string entry{"entry"};
        static constexpr std::string exit{"exit"};
        static constexpr std::string handler{"handler"};
         SimpleString(std::to_string(ID).c_str())
    }

    ~test_state()
    {
        // reset std::func to null
    }

    static SimpleString m_entry_name;
    static SimpleString m_exit_name;
    static SimpleString m_handler_name;
};



struct test_fsm;

struct test_state : public ecu_fsm_state
{
    test_state(test_fsm *fsm, SimpleString name) : 
        m_fsm{fsm},
        m_entry{std::move(name + SimpleString("_entry"))}, 
        m_exit{std::move(name + SimpleString("_exit"))}, 
        m_handler{std::move(name + SimpleString("_handler"))}
    {
        entry = m_entry.get();
        exit = m_exit.get();
        handler = m_handler.get();
    }

    template<class T>
    struct state_function;

    template<class Ret, class... Args>
    struct state_function<Ret(Args...)>
    {
        typedef Ret (*handler_t)(Args...);

        state_function(SimpleString name) : 
            m_name{std::move(name)}
        {
            static_assert( (sizeof...(Args) <= 2), "Number of function arguments too large.");

            if constexpr(sizeof...(Args) == 0)
            {
                m_run = std::bind(&run, *this);
            }
            else if constexpr(sizeof...(Args) == 1)
            {
                m_run = std::bind(&run, *this, std::placeholders::_1);
            }
            else
            {
                m_run = std::bind(&run, *this, std::placeholders::_1, std::placeholders::_2);
            }
        }

        static Ret run(state_function& me, Args... args)
        {
            (..., (void)args);

            mock().actualCall(me.m_name);
            if (me.m_to)
            {
                me.m_to();
            }
        }

        handler_t get()
        {
            return m_run.target<Ret(Args...)>();
        }

        const SimpleString& operator()() const
        {
            return m_name;
        }

        void to(test_state& new_state)
        {
            m_to = std::bind(&ecu_fsm_change_state, new_state.m_fsm, &new_state);
        }

        void to()
        {
            m_to = nullptr;
        }
        
        SimpleString m_name;
        std::function<void(void)> m_to;
        std::function<Ret(Args...)> m_run;
    };

    test_fsm *m_fsm;
    state_function<void(ecu_fsm *)> m_entry;
    state_function<void(ecu_fsm *)> m_exit;
    state_function<void(ecu_fsm *, const void *)> m_handler;
};

struct test_fsm : public ecu_fsm
{
    test_fsm(test_state& init_state)
    {
        ecu_fsm_ctor(this, &init_state);
    }

    void dispatch()
    {
        static constexpr int DUMMY_EVENT = 0;
        ecu_fsm_dispatch(this, &DUMMY_EVENT);
    }
};

TEST_GROUP(Fsm)
{
    TEST_GROUP_CppUTestGroupFsm() : 
        s1{&me, "S1"}, s2{&me, "S2"}, s3{&me, "S3"}, 
        s4{&me, "S4"}, s5{&me, "s5"}, me{s1}
    {

    }

    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
    
    template<class... Args>
    requires (... && std::is_same_v<const SimpleString, std::remove_reference_t<Args>>)
    static void EXPECT_STATE_PATH(const SimpleString& s1, Args&&... args)
    {
        mock().strictOrder();
        mock().expectOneCall(s1);
        (..., mock().expectOneCall(args));
    }

    test_state s1;
    test_state s2;
    test_state s3;
    test_state s4;
    test_state s5;
    test_fsm me;
};

#warning "TODO: Stopped here. Assert firing because state_function.get() returning nullptr...probably because \
    we cant bind a class function to a C function pointer..idk..."
TEST(Fsm, CompileTest)
{
    try
    {
        /* code */
        EXPECT_STATE_PATH(s1.m_entry(), s1.m_exit(), s2.m_entry());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}























// class fsm : fsm_t 
// {
// public:
//     fsm()
//     {
//         init<(state_number)(NUMBER_OF_STATES - 1)>();
//         fsmCtor(this, &states.at(0));
//     }

//     void ctor(state_number s)
//     {
//         fsmCtor(this, &states.at(s));
//     }

//     void start()
//     {
//         fsmStart(this);
//     }

//     void dispatch()
//     {
//         fsmDispatch(this, &DUMMY_EVENT);
//     }

//     /**
//      * @brief Generates definition for the state's onEntry handler 
//      * based on input parameters.
//      * 
//      * @param s Which state's onEntry handler to modify.
//      * @param name Name given to mock expectation when handler runs. Only
//      * applicable if @p type is MOCK.
//      * @param type Specify if onEntry handler should be a STUB or MOCK.
//      * @param to_state Optional. Specify if FSM should transition to another
//      * state in the onEntry handler.
//      */
//     void set_entry(state_number s, const char *name, state_type type, to_state to_state = NO_TRANSITION)
//     {
//         states.at(s).set_entry(name, type, to_state);
//     }

//     /**
//      * @brief Generates definition for the state's onExit handler 
//      * based on input parameters.
//      * 
//      * @param s Which state's onExit handler to modify.
//      * @param name Name given to mock expectation when handler runs. Only
//      * applicable if @p type is MOCK.
//      * @param type Specify if onExit handler should be a STUB or MOCK.
//      * @param to_state Optional. Specify if FSM should transition to another
//      * state in the onExit handler.
//      */
//     void set_exit(state_number s, const char *name, state_type type, to_state to_state = NO_TRANSITION)
//     {
//         states.at(s).set_exit(name, type, to_state);
//     }

//     /**
//      * @brief Generates definition for the state's handler 
//      * based on input parameters.
//      * 
//      * @param s Which state's handler to modify.
//      * @param name Name given to mock expectation when handler runs. Only
//      * applicable if @p type is MOCK.
//      * @param type Specify if handler should be a STUB or MOCK.
//      * @param to_state Optional. Specify if FSM should transition to another
//      * state in the handler.
//      */
//     void set_handler(state_number s, const char *name, state_type type, to_state to_state = NO_TRANSITION)
//     {
//         states.at(s).set_handler(name, type, to_state);
//     }

//     state_number get_current_state() const
//     {
//         state_number s = NUMBER_OF_STATES;

//         for (std::size_t i = 0; i < (std::size_t)NUMBER_OF_STATES; i++)
//         {
//             if (fsm_t::state == &states.at(i))
//             {
//                 s = (state_number)i;
//                 break;
//             }
//         }
        
//         return s;
//     }

//     const SimpleString& get_entry_name(state_number s) const 
//     {
//         return states.at(s).m_entry_name;
//     }

//     const SimpleString& get_exit_name(state_number s) const 
//     {
//         return states.at(s).m_exit_name;
//     }

//     const SimpleString& get_handler_name(state_number s) const 
//     {
//         return states.at(s).m_handler_name;
//     }

// private:
//     /**
//      * @brief Sets handlers in fsm_state_t. Recursive template has to be
//      * used instead of a for-loop since the handler functions are C callbacks.
//      * They must be resolved at compile-time.
//      * 
//      * @tparam N Number of states to initialize. Max = NUMBER_OF_STATES.
//      */
//     template<state_number N>
//     void init()
//     {
//         static_assert( (N < NUMBER_OF_STATES) );
//         init<(state_number)(N-1)>(); /* Recursive. init<0> explicitly specialized. */
//         states.at(N).onEntry = FSM_STATE_ON_ENTRY_CAST(&entry<N>);
//         states.at(N).onExit = FSM_STATE_ON_EXIT_CAST(&exit<N>);
//         states.at(N).handler = FSM_STATE_HANDLER_CAST(&run<N>);

//         /* Default to mocks so test fails if unexpected state function ran. */
//         states.at(N).set_entry("unused_state_entered", MOCK);
//         states.at(N).set_exit("unused_state_exited", MOCK);
//         states.at(N).set_handler("unused_state_ran", MOCK);
//     }

//     /**
//      * @brief Each State's unique onEntry function. This approach allows
//      * each State to have a unique function definition and allows the
//      * function to be passed to fsm_state_t, which takes in C callbacks.
//      */
//     template<state_number S>
//     static void entry(fsm *me)
//     {
//         state_type type = me->states.at(S).m_entry_type;
//         to_state to_state = me->states.at(S).m_entry_to_state;

//         switch (type)
//         {
//             case STUB:
//             {
//                 if (to_state != NO_TRANSITION)
//                 {
//                     fsmChangeState(me, &me->states.at(to_state));
//                 }
//                 break;
//             }

//             case MOCK:
//             {
//                 const SimpleString& name = me->states.at(S).m_entry_name;
//                 mock().actualCall(name);

//                 if (to_state != NO_TRANSITION)
//                 {
//                     fsmChangeState(me, &me->states.at(to_state));
//                 }
//                 break;
//             }

//             default:
//             {
//                 assert( (false) );
//                 break;
//             }
//         }
//     }

//     /**
//      * @brief Each State's unique onExit function. This approach allows
//      * each State to have a unique function definition and allows the
//      * function to be passed to fsm_state_t, which takes in C callbacks.
//      */
//     template<state_number S>
//     static void exit(fsm *me)
//     {
//         state_type type = me->states.at(S).m_exit_type;
//         to_state to_state = me->states.at(S).m_exit_to_state;

//         switch (type)
//         {
//             case STUB:
//             {
//                 if (to_state != NO_TRANSITION)
//                 {
//                     fsmChangeState(me, &me->states.at(to_state));
//                 }
//                 break;
//             }

//             case MOCK:
//             {
//                 const SimpleString& name = me->states.at(S).m_exit_name;
//                 mock().actualCall(name);

//                 if (to_state != NO_TRANSITION)
//                 {
//                     fsmChangeState(me, &me->states.at(to_state));
//                 }
//                 break;
//             }

//             default:
//             {
//                 assert( (false) );
//                 break;
//             }
//         }
//     }

//     /**
//      * @brief Each State's unique handler function. This approach allows
//      * each State to have a unique function definition and allows the
//      * function to be passed to fsm_state_t, which takes in C callbacks.
//      */
//     template<state_number S>
//     static void run(fsm *me, const void *event)
//     {
//         (void)event;
//         state_type type = me->states.at(S).m_handler_type;
//         to_state to_state = me->states.at(S).m_handler_to_state;

//         switch (type)
//         {
//             case STUB:
//             {
//                 if (to_state != NO_TRANSITION)
//                 {
//                     fsmChangeState(me, &me->states.at(to_state));
//                 }
//                 break;
//             }

//             case MOCK:
//             {
//                 const SimpleString& name = me->states.at(S).m_handler_name;
//                 mock().actualCall(name);

//                 if (to_state != NO_TRANSITION)
//                 {
//                     fsmChangeState(me, &me->states.at(to_state));
//                 }
//                 break;
//             }

//             default:
//             {
//                 assert( (false) );
//                 break;
//             }
//         }
//     }

// private:
//     class state : public fsm_state_t
//     {
//     public:
//         void set_entry(const char* name, state_type type, to_state to_state = NO_TRANSITION)
//         {
//             m_entry_name = SimpleString(name);
//             m_entry_type = type;
//             m_entry_to_state = to_state;
//         }

//         void set_exit(const char *name, state_type type, to_state to_state = NO_TRANSITION)
//         {
//             m_exit_name = SimpleString(name);
//             m_exit_type = type;
//             m_exit_to_state = to_state;
//         }

//         void set_handler(const char *name, state_type type, to_state to_state = NO_TRANSITION)
//         {
//             m_handler_name = SimpleString(name);
//             m_handler_type = type;
//             m_handler_to_state = to_state;
//         }

//         to_state m_entry_to_state;
//         to_state m_exit_to_state;
//         to_state m_handler_to_state;
//         state_type m_entry_type;
//         state_type m_exit_type;
//         state_type m_handler_type;
//         SimpleString m_entry_name;
//         SimpleString m_exit_name;
//         SimpleString m_handler_name;
//     };

// private:
//     std::array<state, NUMBER_OF_STATES> states;
//     static constexpr to_state NO_TRANSITION = TO_STATE_NO_TRANSITION;
//     static constexpr std::uint8_t DUMMY_EVENT{0};
// };

// /* C++ standard requires full specialization of member templates to be within 
// the namespace the fsm class is in. So it has to be outside the fsm class. */
// template<>
// void fsm::init<(state_number)0>()
// {
//     states.at(0).onEntry = FSM_STATE_ON_ENTRY_CAST(&entry<(state_number)0>);
//     states.at(0).onExit = FSM_STATE_ON_EXIT_CAST(&exit<(state_number)0>);
//     states.at(0).handler = FSM_STATE_HANDLER_CAST(&run<(state_number)0>);

//     /* Default to mocks so test fails if unexpected state function ran. */
//     states.at(0).set_entry("unused_state_entered", MOCK);
//     states.at(0).set_exit("unused_state_exited", MOCK);
//     states.at(0).set_handler("unused_state_ran", MOCK);
// }



// /**************************************************************************************************
//                                             Static Asserts
// **************************************************************************************************/

// static_assert( ((int)NUMBER_OF_STATES == (int)TO_STATE_NO_TRANSITION), "Values must be equal" );



// /**************************************************************************************************
//                                             Test Groups
// **************************************************************************************************/

// TEST_GROUP(FSM)
// {
// protected:
//     void setup() override
//     {
//         set_assert_handler(AssertResponse::FAIL);
//     }

//     void teardown() override
//     {
//         mock().checkExpectations();
//         mock().removeAllComparatorsAndCopiers();
//         mock().clear();
//     }

//     void EXPECT_STATE_PATH(const std::initializer_list<SimpleString>& states)
//     {
//         /* Add strings to vector so they remain in memory for mock expectations. */
//         expected_states.insert(expected_states.cend(), states);

//         mock().strictOrder();
//         for (const auto& s : states)
//         {
//             mock().expectOneCall(s);
//         }
//     }

// protected:
//     fsm me;
//     std::vector<SimpleString> expected_states;
// };



// /**************************************************************************************************
//                                         Tests - fsmDispatch()
// **************************************************************************************************/

// /**
//  * @brief Expect s1->s1_exit->s1_entry
//  */
// TEST(FSM, SelfTransition)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         me.set_handler(S1, "s1", MOCK, TO_S1); /* self transition. */
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S1, "s1_entry", MOCK);
//         EXPECT_STATE_PATH({"s1", "s1_exit", "s1_entry"});

//         /* Step 2: Action. */
//         me.dispatch();

//         /* Step 3: Assert. Fails if State Path was incorrect. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Transition to a new state in S1 entry handler. Perform
//  * a self-transition in S1 handler. Can happen when user first 
//  * constructs FSM and this is the first state they assign to it. 
//  * This does not require entry handler to run. First dispatched 
//  * event can cause self transition which then runs entry handler.
//  * 
//  * Expect s1->s1_exit->s1_entry->s1_exit->s2_entry.
//  */
// TEST(FSM, SelfTransitionAndTransitionInEntry)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         me.set_handler(S1, "s1", MOCK, TO_S1); /* self transition. */
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S1, "s1_entry", MOCK, TO_S2);
//         me.set_entry(S2, "s2_entry", MOCK);
//         EXPECT_STATE_PATH({"s1", "s1_exit", "s1_entry", "s1_exit", "s2_entry"});

//         /* Step 2: Action. */
//         me.dispatch();

//         /* Step 3: Assert. Fails if State Path was incorrect. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Expect s1->s1_exit->s2_entry
//  */
// TEST(FSM, SingleStateTransition)
// {
//     try
//     {
//         me.set_handler(S1, "s1", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S2, "s2_entry", MOCK);
//         EXPECT_STATE_PATH({"s1", "s1_exit", "s2_entry"});

//         /* Step 2: Action. */
//         me.dispatch();

//         /* Step 3: Assert. Fails if State Path was incorrect. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Expect s1->s1_exit->s2_entry->s2_exit->s3_entry
//  */
// TEST(FSM, MultipleStateTransitionsInARow)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         me.set_handler(S1, "s1", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S2, "s2_entry", MOCK, TO_S3);
//         me.set_exit(S2, "s2_exit", MOCK);
//         me.set_entry(S3, "s3_entry", MOCK);
//         EXPECT_STATE_PATH({"s1", "s1_exit", "s2_entry", "s2_exit", "s3_entry"});

//         /* Step 2: Action. */
//         me.dispatch();

//         /* Step 3: Assert. Fails if State Path was incorrect. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Transition is not allowed in exit handler. Makes no
//  * sense since we are already exiting the State.
//  */
// TEST(FSM, StateTransitionOnExit)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         EXPECT_ASSERTION();
//         me.set_handler(S1, "s1", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK, TO_S2);
//         EXPECT_STATE_PATH({"s1", "s1_exit"}); /* Setup mocks to verify assertion fired in proper location. */

//         /* Step 2: Action. */
//         me.dispatch();

//         /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Do s1->s2 transition. Do self-transition to s2
//  * in s2_entry. This is not allowed since we are already in
//  * s2 and would cause an infinite loop. Should cause
//  * an assertion.
//  */
// TEST(FSM, SelfTransitionOnFirstEntry)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         EXPECT_ASSERTION();
//         me.set_handler(S1, "s1", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S2, "s2_entry", MOCK, TO_S2); /* self transition in entry not allowed. */
//         EXPECT_STATE_PATH({"s1", "s1_exit", "s2_entry"}); /* Setup mocks to verify assertion fired in proper location. */

//         /* Step 2: Action. */
//         me.dispatch();

//         /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Do s1->s2->s3 transition. Do self-transition to s3
//  * in s3_entry. This is not allowed since we are already in
//  * s2 and would cause an infinite loop. Should cause
//  * an assertion.
//  */
// TEST(FSM, SelfTransitionOnSecondEntry)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         EXPECT_ASSERTION();
//         me.set_handler(S1, "s1", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S2, "s2_entry", MOCK, TO_S3);
//         me.set_exit(S2, "s2_exit", MOCK);
//         me.set_entry(S3, "s3_entry", MOCK, TO_S3); /* self transition in entry not allowed. */
//         EXPECT_STATE_PATH({"s1", "s1_exit", "s2_entry", "s2_exit", "s3_entry"}); /* Setup mocks to verify assertion fired in proper location. */

//         /* Step 2: Action. */
//         me.dispatch();

//         /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }



// /**************************************************************************************************
//                                         Tests - fsmStart()
// **************************************************************************************************/

// /**
//  * @brief Start the FSM. Expect only s1_entry.
//  */
// TEST(FSM, StartNoTransition)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         me.set_entry(S1, "s1_entry", MOCK);
//         EXPECT_STATE_PATH({"s1_entry"});

//         /* Step 2: Action. */
//         me.start();

//         /* Step 3: Assert. Fails if State Path was incorrect. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Transition to s2 in s1_entry.
//  * Expect s1_entry->s1_exit->s2_entry.
//  */
// TEST(FSM, StartSingleStateTransitionOnEntry)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         me.set_entry(S1, "s1_entry", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S2, "s2_entry", MOCK);
//         EXPECT_STATE_PATH({"s1_entry", "s1_exit", "s2_entry"});

//         /* Step 2: Action. */
//         me.start();

//         /* Step 3: Assert. Fails if State Path was incorrect. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Transition to s2 in s1_entry. Transition to s3 in s2_entry. 
//  * Expect s1_entry->s1_exit->s2_entry->s2_exit->s3_entry.
//  */
// TEST(FSM, StartMultipleStateTransitionsInARow)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         me.set_entry(S1, "s1_entry", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S2, "s2_entry", MOCK, TO_S3);
//         me.set_exit(S2, "s2_exit", MOCK);
//         me.set_entry(S3, "s3_entry", MOCK);
//         EXPECT_STATE_PATH({"s1_entry", "s1_exit", "s2_entry", "s2_exit", "s3_entry"});

//         /* Step 2: Action. */
//         me.start();

//         /* Step 3: Assert. Fails if State Path was incorrect. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Transition to s2 in s1_entry. Transition to s1 in s1_exit. 
//  * Transition in exit handler should cause assertion. Not allowed
//  * since we are already exiting the State.
//  */
// TEST(FSM, StartStateTransitionOnExit)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         EXPECT_ASSERTION();
//         me.set_entry(S1, "s1_entry", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK, TO_S1); /* State transition in exit handler not allowed. */
//         EXPECT_STATE_PATH({"s1_entry", "s1_exit"}); /* Setup mocks to verify assertion fired in proper location. */

//         /* Step 2: Action. */
//         me.start();

//         /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Transition to s2 in s1_entry. Should cause assertion
//  * since self-transition in entry handler causes infinite loop.
//  */
// TEST(FSM, StartSelfTransitionOnFirstEntry)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         EXPECT_ASSERTION();
//         me.set_entry(S1, "s1_entry", MOCK, TO_S1); /* Self-transition in entry handler not allowed. */
//         EXPECT_STATE_PATH({"s1_entry"}); /* Setup mocks to verify assertion fired in proper location. */

//         /* Step 2: Action. */
//         me.start();

//         /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Transition to s2 in s1_entry. Transition to s2
//  * in s2_entry. Should cause assertion in s2_entry since 
//  * self-transition in entry handler causes infinite loop.
//  */
// TEST(FSM, StartSelfTransitionOnSecondEntry)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         EXPECT_ASSERTION();
//         me.set_entry(S1, "s1_entry", MOCK, TO_S2);
//         me.set_exit(S1, "s1_exit", MOCK);
//         me.set_entry(S2, "s2_entry", MOCK, TO_S2); /* Self-transition in entry handler not allowed. */
//         EXPECT_STATE_PATH({"s1_entry", "s1_exit", "s2_entry"}); /* Setup mocks to verify assertion fired in proper location. */

//         /* Step 2: Action. */
//         me.start();

//         /* Step 3: Assert. Fails if assertion does not fire or fired in wrong location. */
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }
