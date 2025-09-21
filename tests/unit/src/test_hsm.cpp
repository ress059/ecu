/**
 * @file
 * @brief Unit tests for public API functions in @ref hsm.h.
 * Majority of tests model a specific state machine and verify
 * behavior. Functions tested:
 * @ref ecu_hsm_ctor(), @ref ecu_hsm_change_state(), 
 * @ref ecu_hsm_dispatch(), @ref ecu_hsm_start() tested.
 * 
 * Test Summary:
 * 
 * @ref ECU_HSM_GET_CONTEXT()
 *      - TEST(Hsm, GetContext)
 * 
 * @ref ecu_hsm_ctor()
 *      - TEST(Hsm, CtorInitialStateIsTop)
 *      - TEST(Hsm, CtorInitialStateHasNoParent)
 * 
 * @ref ecu_hsm_change_state()
 *      - TEST(Hsm, ChangeStateNewStateIsTop)
 *      - TEST(Hsm, ChangeStateNewStateHasNoParent)
 * 
 * @ref ecu_hsm_dispatch()
 *      - TEST(Hsm, DispatchEventUnhandledInEntireStateHierarchy)
 *      - TEST(Hsm, DispatchSelfStateTransitionEntryAndExitUnused)
 *      - TEST(Hsm, DispatchSelfStateTransitionEntrySignalsTransition)
 *      - TEST(Hsm, DispatchSelfStateTransitionExitSignalsTransition)
 *      - TEST(Hsm, DispatchStateTransitionDoesNotHandleEvent)
 *      - TEST(Hsm, DispatchStateTransitionOutOfStateBranchWithHeightGreaterThanMax)
 *      - TEST(Hsm, DispatchStateTransitionToStateBranchWithHeightGreaterThanMax)
 *      - TEST(Hsm, DispatchStateTransitionOutOfStateBranchWithNoTop)
 *      - TEST(Hsm, DispatchStateTransitionToStateBranchWithNoTop)
 *      - TEST(Hsm, DispatchStateTransitionToCompositeStateWithNoInitialTransition)
 *      - TEST(Hsm, DispatchStateTransitionToCompositeStateWithNoInitialTransition)
 *      - TEST(Hsm, DispatchStateTransitionToCompositeStateWithInitialTransitionToParent)
 *      - TEST(Hsm, DispatchStateTransitionToCompositeStateWithInitialTransitionToSelf)
 * 
 * HsmVariant1 - Start:
 *      - TEST(Hsm, HsmVariant1Start)
 * 
 * HsmVariant1 - S211
 *      - TEST(Hsm, HsmVariantS211EventA)
 *      - TEST(Hsm, HsmVariant1S211EventB)
 *      - TEST(Hsm, HsmVariant1S211EventC)
 *      - TEST(Hsm, HsmVariant1S211EventD)
 *      - TEST(Hsm, HsmVariant1S211EventE)
 *      - TEST(Hsm, HsmVariant1S211EventF)
 *      - TEST(Hsm, HsmVariant1S211EventG)
 *      - TEST(Hsm, HsmVariant1S211EventH)
 *      - TEST(Hsm, HsmVariant1S211EventIFooFalse)
 *      - TEST(Hsm, HsmVariant1S211EventIFooTrue)
 *      - TEST(Hsm, HsmVariant1S211DummyEvent)
 * 
 * HsmVariant1 - S11
 *      - TEST(Hsm, HsmVariantS11EventA)
 *      - TEST(Hsm, HsmVariantS11EventB)
 *      - TEST(Hsm, HsmVariantS11EventC)
 *      - TEST(Hsm, HsmVariantS11EventDFooFalse)
 *      - TEST(Hsm, HsmVariantS11EventDFooTrue)
 *      - TEST(Hsm, HsmVariantS11EventE)
 *      - TEST(Hsm, HsmVariantS11EventF)
 *      - TEST(Hsm, HsmVariantS11EventG)
 *      - TEST(Hsm, HsmVariantS11EventH)
 *      - TEST(Hsm, HsmVariantS11DummyEvent)
 * 
 * HsmVariant1 - Misc
 *      - TEST(Hsm, HsmVariant1DispatchMultipleEvents)
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

namespace
{
/**
 * @brief Test events to dispatch to state machines
 * under test.
 */
enum class event : std::uint8_t
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    /**************/
    DUMMY_EVENT
};

/**
 * @brief Each ID represents a unique state.
 * @warning If a new state is added, add the ID here and also
 * call its reset routine TEST_GROUP(Hsm)::setup()!
 */
enum state_id
{
    S,
    S1,
    S11,
    S2,
    S21,
    S211,
    /********************/
    NUMBER_OF_STATE_IDS
};

/**
 * @brief C++ wrapper around @ref ecu_hsm_state.
 * The @ref ecu_hsm_state under test is a C structure
 * that takes in function pointers for its state handlers.
 * Therefore handlers in this class must be static.
 * This class is then parametrized by a unique state ID
 * so the compiler creates unique instances of each static
 * handler for every state. Because of this, each state
 * is a singleton.
 */
template<state_id CURRENT_STATE>
struct hsm_state : public ecu_hsm_state
{
    /// @brief Delete copy semantics since singleton.
    hsm_state(const hsm_state& other) = delete;

    /// @brief Delete copy semantics since singleton.
    hsm_state& operator=(const hsm_state& other) = delete;

    /// @brief Delete move semantics since singleton.
    hsm_state(hsm_state&& other) = delete;

    /// @brief Delete move semantics since singleton.
    hsm_state& operator=(hsm_state&& other) = delete;

    /// @brief Returns the singleton instance.
    static hsm_state<CURRENT_STATE>& get_instance()
    {
        static hsm_state<CURRENT_STATE> me;
        return me;
    }

    /// @brief Resets this object to a known state. This must be
    /// called on startup by the test group since each state is
    /// a singleton.
    void reset()
    {
        entry = &this->with_entry_cb;
        exit = &this->with_exit_cb;
        initial = &this->with_initial_cb;
        handler = &this->with_handler_cb;
        parent = &ECU_HSM_TOP_STATE;
    }

    /// @brief Builder that sets this state's entry handler.
    /// The handler calls the mock expectation set by @ref entered().
    hsm_state<CURRENT_STATE>& with_entry()
    {
        entry = &with_entry_cb;
        return *this;
    }

    /// @brief Builder that sets this state's entry handler.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_entry_no_mock_to()
    {
        entry = &with_entry_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's entry handler.
    /// The handler calls the mock expectation set by @ref entered()
    /// then transitions to the new state specified by the template
    /// parameter.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_entry_to()
    {
        entry = &with_entry_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that makes this state not have an entry handler.
    hsm_state<CURRENT_STATE>& with_entry_unused()
    {
        entry = ECU_HSM_STATE_ENTRY_UNUSED;
        return *this;
    }

    /// @brief Builder that sets this state's exit handler.
    /// The handler calls the mock expectation set by @ref exited().
    hsm_state<CURRENT_STATE>& with_exit()
    {
        exit = &with_exit_cb;
        return *this;
    }

    /// @brief Builder that sets this state's exit handler.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_exit_no_mock_to()
    {
        exit = &with_exit_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's exit handler.
    /// The handler calls the mock expectation set by @ref exited()
    /// then transitions to the new state specified by the template
    /// parameter.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_exit_to()
    {
        exit = &with_exit_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that makes this state not have an exit handler.
    hsm_state<CURRENT_STATE>& with_exit_unused()
    {
        exit = ECU_HSM_STATE_EXIT_UNUSED;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler calls the mock expectation set by @ref handled()
    /// and handles the event.
    hsm_state<CURRENT_STATE>& with_handler()
    {
        handler = &with_handler_cb;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function
    /// to the supplied function.
    hsm_state<CURRENT_STATE>& with_handler(bool (*handle)(ecu_hsm *, const void *))
    {
        assert( (handle) );
        handler = handle;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler is a dummy (non-null) function that just handles
    /// the event.
    hsm_state<CURRENT_STATE>& with_handler_no_mock()
    {
        handler = &with_handler_no_mock_cb;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    /// The event is handled.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_handler_no_mock_to()
    {
        handler = &with_handler_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler calls the mock expectation set by @ref handled()
    /// then transitions to the new state specified by the template
    /// parameter. The event is handled.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_handler_to()
    {
        handler = &with_handler_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler calls the mock expectation set by @ref handled()
    /// but does not handle the event.
    hsm_state<CURRENT_STATE>& with_handler_unhandled()
    {
        handler = &with_handler_unhandled_cb;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler is a dummy (non-null) function that does nothing
    /// and does not handle the event.
    hsm_state<CURRENT_STATE>& with_handler_unhandled_no_mock()
    {
        handler = &with_handler_unhandled_no_mock_cb;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    /// The event is not handled.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_handler_unhandled_no_mock_to()
    {
        handler = &with_handler_unhandled_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's main handler function.
    /// The handler calls the mock expectation set by @ref handled()
    /// then transitions to the new state specified by the template
    /// parameter. The event is not handled.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_handler_unhandled_to()
    {
        handler = &with_handler_unhandled_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that makes this state not have a main handler function.
    hsm_state<CURRENT_STATE>& with_handler_unused()
    {
        handler = nullptr;
        return *this;
    }

    /// @brief Builder that sets this state's initial handler function.
    /// The handler calls the mock expectation set by @ref init() but
    /// does not transition to a leaf state.
    hsm_state<CURRENT_STATE>& with_initial()
    {
        initial = &with_initial_cb;
        return *this;
    }

    /// @brief Builder that sets this state's initial handler function.
    /// The handler is a dummy (non-null) function that does nothing.
    hsm_state<CURRENT_STATE>& with_initial_no_mock()
    {
        initial = &with_initial_no_mock_cb;
        return *this;
    }

    /// @brief Builder that sets this state's initial handler function.
    /// The handler transitions to the new state specified by the
    /// template parameter but does not call a mock expectation.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_initial_no_mock_to()
    {
        initial = &with_initial_no_mock_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that sets this state's initial handler function.
    /// The handler calls the mock expectation set by @ref init()
    /// then transitions to the new state specified by the template
    /// parameter.
    template<state_id NEW_STATE>
    hsm_state<CURRENT_STATE>& with_initial_to()
    {
        initial = &with_initial_to_cb<NEW_STATE>;
        return *this;
    }

    /// @brief Builder that makes this state not have an initial handler function.
    hsm_state<CURRENT_STATE>& with_initial_unused()
    {
        initial = ECU_HSM_STATE_INITIAL_UNUSED;
        return *this;
    }

    /// @brief Builder that sets this state's parent.
    template<state_id PARENT_STATE>
    hsm_state<CURRENT_STATE>& with_parent()
    {
        parent = &hsm_state<PARENT_STATE>::get_instance();
        return *this;
    }

    /// @brief Builder that sets this state's parent to @ref ECU_HSM_TOP_STATE.
    hsm_state<CURRENT_STATE>& with_parent_top()
    {
        parent = &ECU_HSM_TOP_STATE;
        return *this;
    }

    /// @brief Builder that makes this state not have any parent.
    hsm_state<CURRENT_STATE>& with_parent_unused()
    {
        parent = nullptr;
        return *this;
    }

    /// @brief Entry handler that runs if state was built with @ref with_entry().
    static void with_entry_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        mock().actualCall("entered")
              .withParameter("state", CURRENT_STATE);
    }

    /// @brief Entry handler that runs if state was built with @ref with_entry_no_mock_to().
    template<state_id NEW_STATE>
    static void with_entry_no_mock_to_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
    }

    /// @brief Entry handler that runs if state was built with @ref with_entry_to().
    template<state_id NEW_STATE>
    static void with_entry_to_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        mock().actualCall("entered")
              .withParameter("state", CURRENT_STATE);

        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
    }

    /// @brief Exit handler that runs if state was built with @ref with_exit().
    static void with_exit_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        mock().actualCall("exited")
              .withParameter("state", CURRENT_STATE);
    }

    /// @brief Exit handler that runs if state was built with @ref with_exit_no_mock_to().
    template<state_id NEW_STATE>
    static void with_exit_no_mock_to_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
    }

    /// @brief Exit handler that runs if state was built with @ref with_exit_to().
    template<state_id NEW_STATE>
    static void with_exit_to_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        mock().actualCall("exited")
              .withParameter("state", CURRENT_STATE);

        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
    }

    /// @brief Main handler that runs if state was built with @ref with_handler().
    static bool with_handler_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        mock().actualCall("handled")
              .withParameter("state", CURRENT_STATE);

        return true;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_no_mock().
    static bool with_handler_no_mock_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        return true;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_no_mock_to().
    template<state_id NEW_STATE>
    static bool with_handler_no_mock_to_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
        return true;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_to().
    template<state_id NEW_STATE>
    static bool with_handler_to_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        mock().actualCall("handled")
              .withParameter("state", CURRENT_STATE);

        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
        return true;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_unhandled().
    static bool with_handler_unhandled_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        mock().actualCall("handled")
              .withParameter("state", CURRENT_STATE);

        return false;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_unhandled_no_mock().
    static bool with_handler_unhandled_no_mock_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        return false;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_unhandled_no_mock_to().
    template<state_id NEW_STATE>
    static bool with_handler_unhandled_no_mock_to_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
        return false;
    }

    /// @brief Main handler that runs if state was built with @ref with_handler_unhandled_to().
    template<state_id NEW_STATE>
    static bool with_handler_unhandled_to_cb(ecu_hsm *hsm, const void *event)
    {
        assert( (hsm) );
        (void)event;
        mock().actualCall("handled")
              .withParameter("state", CURRENT_STATE);

        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
        return false;
    }

    /// @brief Initial handler that runs if state was built with @ref with_initial().
    static void with_initial_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        mock().actualCall("init")
              .withParameter("state", CURRENT_STATE);
    }

    /// @brief Initial handler that runs if state was built with @ref with_initial_no_mock().
    static void with_initial_no_mock_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
    }

    /// @brief Initial handler that runs if state was built with @ref with_initial_no_mock_to().
    template<state_id NEW_STATE>
    static void with_initial_no_mock_to_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
    }

    /// @brief Initial handler that runs if state was built with @ref with_initial_to().
    template<state_id NEW_STATE>
    static void with_initial_to_cb(ecu_hsm *hsm)
    {
        assert( (hsm) );
        mock().actualCall("init")
              .withParameter("state", CURRENT_STATE);

        auto& new_state = hsm_state<NEW_STATE>::get_instance();
        ecu_hsm_change_state(hsm, &new_state);
    }

private:
    /// @brief Default constructor. Private since singleton.
    hsm_state()
    {

    }
};

/**
 * @brief Test HSM modeled in test_hsm_variant1.png.
 * This image is by Miro Samek, Quantum Leaps LLC: https://github.com/QuantumLeaps/modern-embedded-programming-course/blob/main/lesson-42/qhsmtst-console/SM_of_QHsmTst.png
 */
struct hsm_variant1 : public ecu_hsm
{
    /// @brief Default constructor. Creates HSM defined by test_hsm_variant1.png.
    hsm_variant1()
    {
        /* Define HSM states. */
        (void)hsm_state<S>::get_instance()
                            .with_entry()
                            .with_exit()
                            .with_initial_to<S11>()
                            .with_handler(&s_handler)
                            .with_parent_top();

        (void)hsm_state<S1>::get_instance()
                            .with_entry()
                            .with_exit()
                            .with_initial_to<S11>()
                            .with_handler(&s1_handler)
                            .with_parent<S>();

        (void)hsm_state<S11>::get_instance()
                            .with_entry()
                            .with_exit()
                            .with_initial_unused()
                            .with_handler(&s11_handler)
                            .with_parent<S1>();

        (void)hsm_state<S2>::get_instance()
                            .with_entry()
                            .with_exit()
                            .with_initial_to<S211>()
                            .with_handler(&s2_handler)
                            .with_parent<S>();

        (void)hsm_state<S21>::get_instance()
                            .with_entry()
                            .with_exit()
                            .with_initial_to<S211>()
                            .with_handler(&s21_handler)
                            .with_parent<S2>();

        (void)hsm_state<S211>::get_instance()
                            .with_entry()
                            .with_exit()
                            .with_initial_unused()
                            .with_handler(&s211_handler)
                            .with_parent<S21>();

        /* Create HSM. */
        ecu_hsm_ctor(this, &hsm_state<S2>::get_instance(), 4);
    }

    /// @brief Starts state machine in S11 with all mocks disabled. 
    /// Re-enables mocks once done.
    void start_s11_no_mock()
    {
        static const event EVENT_G = event::G;

        mock().disable();
        ecu_hsm_start(this);
        ecu_hsm_dispatch(this, static_cast<const void *>(&EVENT_G));
        mock().enable();
    }

    /// @brief Starts state machine in S211 with all mocks disabled. 
    /// Re-enables mocks once done.
    void start_s211_no_mock()
    {
        mock().disable();
        ecu_hsm_start(this);
        mock().enable();
    }

    /// @brief Main handler for state S.
    static bool s_handler(ecu_hsm *me, const void *e)
    {
        assert( (me && e) );
        hsm_variant1 *hsm = static_cast<hsm_variant1 *>(me);
        const event signal = *(static_cast<const event *>(e));
        bool status = true;
        mock().actualCall("handled").withParameter("state", S);

        switch (signal)
        {
            case event::E:
            {
                ecu_hsm_change_state(me, &hsm_state<S11>::get_instance());
                break;
            }

            case event::I:
            {
                if (hsm->foo)
                {
                    hsm->foo = false;
                }
                else
                {
                    status = false;
                }
                break;
            }

            default:
            {
                status = false;
                break;
            }
        }

        return status;
    }

    /// @brief Main handler for state S1.
    static bool s1_handler(ecu_hsm *me, const void *e)
    {
        assert( (me && e) );
        hsm_variant1 *hsm = static_cast<hsm_variant1 *>(me);
        const event signal = *(static_cast<const event *>(e));
        bool status = true;
        mock().actualCall("handled").withParameter("state", S1);

        switch (signal)
        {
            case event::A:
            {
                ecu_hsm_change_state(me, &hsm_state<S1>::get_instance());
                break;
            }

            case event::B:
            {
                ecu_hsm_change_state(me, &hsm_state<S11>::get_instance());
                break;
            }

            case event::C:
            {
                ecu_hsm_change_state(me, &hsm_state<S2>::get_instance());
                break;
            }

            case event::D:
            {
                if (!hsm->foo)
                {
                    hsm->foo = true;
                    ecu_hsm_change_state(me, &hsm_state<S>::get_instance());
                }
                else
                {
                    status = false;
                }
                break;
            }

            case event::F:
            {
                ecu_hsm_change_state(me, &hsm_state<S211>::get_instance());
                break;
            }

            case event::I:
            {
                /* Does nothing but handle event. */
                break;
            }

            default:
            {
                status = false;
                break;
            }
        }

        return status;
    }

    /// @brief Main handler for state S11.
    static bool s11_handler(ecu_hsm *me, const void *e)
    {
        assert( (me && e) );
        hsm_variant1 *hsm = static_cast<hsm_variant1 *>(me);
        const event signal = *(static_cast<const event *>(e));
        bool status = true;
        mock().actualCall("handled").withParameter("state", S11);

        switch (signal)
        {
            case event::D:
            {
                if (hsm->foo)
                {
                    hsm->foo = false;
                    ecu_hsm_change_state(me, &hsm_state<S1>::get_instance());
                }
                else
                {
                    status = false;
                }
                break;
            }

            case event::G:
            {
                ecu_hsm_change_state(me, &hsm_state<S211>::get_instance());
                break;
            }

            case event::H:
            {
                ecu_hsm_change_state(me, &hsm_state<S>::get_instance());
                break;
            }

            default:
            {
                status = false;
                break;
            }
        }

        return status;
    }

    /// @brief Main handler for state S2.
    static bool s2_handler(ecu_hsm *me, const void *e)
    {
        assert( (me && e) );
        hsm_variant1 *hsm = static_cast<hsm_variant1 *>(me);
        const event signal = *(static_cast<const event *>(e));
        bool status = true;
        mock().actualCall("handled").withParameter("state", S2);

        switch (signal)
        {
            case event::C:
            {
                ecu_hsm_change_state(me, &hsm_state<S1>::get_instance());
                break;
            }

            case event::I:
            {
                if (!hsm->foo)
                {
                    hsm->foo = true;
                }
                else
                {
                    status = false;
                }
                break;
            }

            case event::F:
            {
                ecu_hsm_change_state(me, &hsm_state<S11>::get_instance());
                break;
            }

            default:
            {
                status = false;
                break;
            }
        }

        return status;
    }

    /// @brief Main handler for state S21.
    static bool s21_handler(ecu_hsm *me, const void *e)
    {
        assert( (me && e) );
        const event signal = *(static_cast<const event *>(e));
        bool status = true;
        mock().actualCall("handled").withParameter("state", S21);

        switch (signal)
        {
            case event::A:
            {
                ecu_hsm_change_state(me, &hsm_state<S21>::get_instance());
                break;
            }

            case event::B:
            {
                ecu_hsm_change_state(me, &hsm_state<S211>::get_instance());
                break;
            }

            case event::G:
            {
                ecu_hsm_change_state(me, &hsm_state<S1>::get_instance());
                break;
            }

            default:
            {
                status = false;
                break;
            }
        }

        return status;
    }

    /// @brief Main handler for state S211.
    static bool s211_handler(ecu_hsm *me, const void *e)
    {
        assert( (me && e) );
        const event signal = *(static_cast<const event *>(e));
        bool status = true;
        mock().actualCall("handled").withParameter("state", S211);

        switch (signal)
        {
            case event::D:
            {
                ecu_hsm_change_state(me, &hsm_state<S21>::get_instance());
                break;
            }

            case event::H:
            {
                ecu_hsm_change_state(me, &hsm_state<S>::get_instance());
                break;
            }

            default:
            {
                status = false;
                break;
            }
        }

        return status;
    }

    bool foo{false};
};
} /* namespace. */

/*------------------------------------------------------------*/
/*------------------------- TEST GROUPS ----------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Hsm)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
        state_S = &hsm_state<S>::get_instance();
        state_S1 = &hsm_state<S1>::get_instance();
        state_S11 = &hsm_state<S11>::get_instance();
        state_S2 = &hsm_state<S2>::get_instance();
        state_S21 = &hsm_state<S21>::get_instance();
        state_S211 = &hsm_state<S211>::get_instance();

        state_S->reset();
        state_S1->reset();
        state_S11->reset();
        state_S2->reset();
        state_S21->reset();
        state_S211->reset();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();

        hsm_state<S>::get_instance().reset();
        hsm_state<S1>::get_instance().reset();
        hsm_state<S11>::get_instance().reset();
        hsm_state<S2>::get_instance().reset();
        hsm_state<S21>::get_instance().reset();
        hsm_state<S211>::get_instance().reset();
    }

    /// @brief Helper that dispatches events to state machine.
    /// Allows test code to dispatch any number of events by
    /// r-value reference. Events are dispatched in the order
    /// they are supplied.
    template<typename... Events>
    requires (std::is_same_v<event, Events> && ...)
    static void dispatch(ecu_hsm *hsm, const event& e0, const Events&... en)
    {
        assert( (hsm) );
        ecu_hsm_dispatch(hsm, static_cast<const void *>(&e0));

        if constexpr(sizeof...(en) > 0)
        {
            (ecu_hsm_dispatch(hsm, static_cast<const void *>(&en)), ...);
        }
    }

    /// @brief Returns callable that sets mock expectation when 
    /// it executes. The expectation is that this state's entry
    /// handler must be called. Meant to be supplied as a parameter
    /// to EXPECT_STATE_PATH() within the test group.
    template<state_id CURRENT_STATE>
    static std::function<void()> entered()
    {
        static auto expectation = [](){
            mock().expectOneCall("entered").withParameter("state", CURRENT_STATE);
        };

        return std::function<void(void)>(expectation);
    }

    /// @brief Returns callable that sets mock expectation when 
    /// it executes. The expectation is that this state's exit
    /// handler must be called. Meant to be supplied as a parameter
    /// to EXPECT_STATE_PATH() within the test group.
    template<state_id CURRENT_STATE>
    static std::function<void()> exited()
    {
        static auto expectation = [](){
            mock().expectOneCall("exited").withParameter("state", CURRENT_STATE);
        };

        return std::function<void(void)>(expectation);
    }

    /// @brief Returns callable that sets mock expectation when 
    /// it executes. The expectation is that this state's initial
    /// handler must be called. Meant to be supplied as a parameter
    /// to EXPECT_STATE_PATH() within the test group.
    template<state_id CURRENT_STATE>
    static std::function<void()> init()
    {
        static auto expectation = [](){
            mock().expectOneCall("init").withParameter("state", CURRENT_STATE);
        };

        return std::function<void(void)>(expectation);
    }

    /// @brief Returns callable that sets mock expectation when 
    /// it executes. The expectation is that this state's main handler
    /// function must be called. Meant to be supplied as a parameter
    /// to EXPECT_STATE_PATH() within the test group.
    template<state_id CURRENT_STATE>
    static std::function<void()> handled()
    {
        static auto expectation = [](){
            mock().expectOneCall("handled").withParameter("state", CURRENT_STATE);
        };

        return std::function<void(void)>(expectation);
    }

    /// @brief Sets expectation that state machine under test
    /// must have the specified transition path.
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

    /// @brief Dummy event that can be dispatched to state machines under test.
    static constexpr event DUMMY_EVENT{event::DUMMY_EVENT};

    /**@{*/
    /// @brief Events that can be dispatched to state machines under test.
    static constexpr event EVENT_A{event::A};
    static constexpr event EVENT_B{event::B};
    static constexpr event EVENT_C{event::C};
    static constexpr event EVENT_D{event::D};
    static constexpr event EVENT_E{event::E};
    static constexpr event EVENT_F{event::F};
    static constexpr event EVENT_G{event::G};
    static constexpr event EVENT_H{event::H};
    static constexpr event EVENT_I{event::I};
    /**@}*/

    /**@{*/
    /// @brief To assist in debugging.
    hsm_state<S> *state_S;
    hsm_state<S1> *state_S1;
    hsm_state<S11> *state_S11;
    hsm_state<S2> *state_S2;
    hsm_state<S21> *state_S21;
    hsm_state<S211> *state_S211;
    /**@}*/
};

/*------------------------------------------------------------*/
/*----------------- TESTS - ECU_HSM_GET_CONTEXT --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Convert intrusive hsm into application hsm type.
 * Verifies returned pointer points to start of user's type.
 */
TEST(Hsm, GetContext)
{
    try
    {
        /* Step 1: Arrange. */
        struct app_hsm_t
        {
            std::uint8_t a;
            ecu_hsm hsm;
            int b;
            std::uint8_t c;
        } app_hsm;

        /* Step 2: Action. */
        app_hsm_t *app_hsm_ptr = ECU_HSM_GET_CONTEXT(&app_hsm.hsm, app_hsm_t, hsm);

        /* Step 3: Assert. */
        POINTERS_EQUAL(&app_hsm, app_hsm_ptr);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - ecu_hsm_ctor() -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not allowed. Starting state must be user-defined
 * and cannot be default top state.
 */
TEST(Hsm, CtorInitialStateIsTop)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_hsm me;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_ctor(&me, &ECU_HSM_TOP_STATE, 1);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. All states must have parents since
 * default top state is supplied.
 */
TEST(Hsm, CtorInitialStateHasNoParent)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_no_mock()
                                .with_initial_unused()
                                .with_parent_unused();

        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_ctor(&me, state_S, 1);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------- TESTS - ecu_hsm_change_state() --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not allowed. New state must be user-defined
 * and cannot be default top state.
 */
TEST(Hsm, ChangeStateNewStateIsTop)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_no_mock()
                                .with_initial_unused()
                                .with_parent_top();

        ecu_hsm_ctor(&me, state_S, 1);
        ecu_hsm_start(&me);
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_change_state(&me, &ECU_HSM_TOP_STATE);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. All states must have parents since
 * default top state is supplied.
 */
TEST(Hsm, ChangeStateNewStateHasNoParent)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_no_mock()
                                .with_initial_unused()
                                .with_parent_top();

        state_S1 = &hsm_state<S1>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_no_mock()
                                .with_initial_unused()
                                .with_parent_unused();

        ecu_hsm_ctor(&me, state_S, 1);
        ecu_hsm_start(&me);
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_change_state(&me, state_S1);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------------- TESTS - ecu_hsm_dispatch() ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not allowed since default top state always handles 
 * the event. Suggests user is not using default top state.
 */
TEST(Hsm, DispatchEventUnhandledInEntireStateHierarchy)
{
    try
    {
        /* Step 1: Arrange. 
        S
        |
        S1
        |
        S11 
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_unhandled_no_mock()
                                .with_initial_no_mock_to<S11>()
                                .with_parent_unused();

        state_S1 = &hsm_state<S1>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_unhandled_no_mock()
                                .with_initial_no_mock_to<S11>()
                                .with_parent<S>();

        state_S11 = &hsm_state<S11>::get_instance()
                                    .with_entry_unused()
                                    .with_exit_unused()
                                    .with_handler_unhandled_no_mock()
                                    .with_initial_unused()
                                    .with_parent<S1>();

        ecu_hsm_ctor(&me, state_S11, 10); /* Set level very high to verify error caught. */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Self-state transition is signalled and state has no
 * entry and exit handler. OK. Primarily for code coverage
 * that HSMVariant1 has not satisfied.
 */
TEST(Hsm, DispatchSelfStateTransitionEntryAndExitUnused)
{
    try
    {
        /* Step 1: Arrange. Transition from S to S.
        TOP
        |
        S 
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_to<S>()
                                .with_parent_top();

        EXPECT_STATE_PATH(handled<S>());
        ecu_hsm_ctor(&me, state_S, 1);

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Test fails if expected state paths not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. No transitions allowed in entry handler.
 */
TEST(Hsm, DispatchSelfStateTransitionEntrySignalsTransition)
{
    try
    {
        /* Step 1: Arrange. Transition from S to S.
        TOP
        |
        S 
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_no_mock_to<S>()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S>()
                                .with_parent_top();

        EXPECT_ASSERTION();
        ecu_hsm_ctor(&me, state_S, 1);

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. No transitions allowed in exit handler.
 */
TEST(Hsm, DispatchSelfStateTransitionExitSignalsTransition)
{
    try
    {
        /* Step 1: Arrange. Transition from S to S.
        TOP
        |
        S 
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_no_mock_to<S>()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S>()
                                .with_parent_top();

        EXPECT_ASSERTION();
        ecu_hsm_ctor(&me, state_S, 1);

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. User must handle event when doing a state
 * transition.
 */
TEST(Hsm, DispatchStateTransitionDoesNotHandleEvent)
{
    try
    {
        /* Step 1: Arrange. Transition from S to S1.
        TOP
        |
        S---S1
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_unhandled_no_mock_to<S1>()
                                .with_initial_unused()
                                .with_parent_top();

        state_S1 = &hsm_state<S1>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_handler_no_mock()
                                .with_initial_unused()
                                .with_parent_top();

        ecu_hsm_ctor(&me, state_S, 1);
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Can happen if HSM not set up correctly.
 */
TEST(Hsm, DispatchStateTransitionOutOfStateBranchWithHeightGreaterThanMax)
{
    try
    {
        /* Step 1: Arrange. Transition from S1 to S2.
        TOP
        |
        S---S2
        |
        S1
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_no_mock_to<S1>()
                                .with_handler_no_mock()
                                .with_parent_top();

        state_S1 = &hsm_state<S1>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S2>()
                                .with_parent<S>();

        state_S2 = &hsm_state<S2>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock()
                                .with_parent_top();

        ecu_hsm_ctor(&me, state_S1, 1); /* Incorrectly set height to 1 instead of 2. */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Can happen if HSM not set up correctly.
 */
TEST(Hsm, DispatchStateTransitionToStateBranchWithHeightGreaterThanMax)
{
    try
    {
        /* Step 1: Arrange. Transition from S to S21.
        TOP
        |
        S---S2
            |
            S21
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S21>()
                                .with_parent_top();

        state_S2 = &hsm_state<S2>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_no_mock_to<S21>()
                                .with_handler_no_mock()
                                .with_parent_top();

        state_S21 = &hsm_state<S21>::get_instance()
                                    .with_entry_unused()
                                    .with_exit_unused()
                                    .with_initial_unused()
                                    .with_handler_no_mock()
                                    .with_parent<S2>();

        ecu_hsm_ctor(&me, state_S, 1); /* Incorrectly set height to 1 instead of 2. */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Starting state branch being transitioned 
 * out of does not use the default top state, which is not allowed.
 */
TEST(Hsm, DispatchStateTransitionOutOfStateBranchWithNoTop)
{
    try
    {
        /* Step 1: Arrange. Transition from S1 to S2.
        TOP     S
        |       |
        S2      S1
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_no_mock_to<S1>()
                                .with_handler_no_mock()
                                .with_parent_unused();

        state_S1 = &hsm_state<S1>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S2>()
                                .with_parent<S>();

        state_S2 = &hsm_state<S2>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock()
                                .with_parent_top();

        ecu_hsm_ctor(&me, state_S1, 10); /* Set level very high to verify error is caught. */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. State branch being transition into 
 * does not use default top state, which is not allowed.
 */
TEST(Hsm, DispatchStateTransitionToStateBranchWithNoTop)
{
    try
    {
        /* Step 1: Arrange. Transition from S2 to S1. 
        TOP     S
        |       |
        S2      S1
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_no_mock_to<S1>()
                                .with_handler_no_mock()
                                .with_parent_unused();

        state_S1 = &hsm_state<S1>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock()
                                .with_parent<S>();

        state_S2 = &hsm_state<S2>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S1>()
                                .with_parent_top();

        ecu_hsm_ctor(&me, state_S2, 10); /* Set level very high to verify error is caught. */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. All initial handlers defined must transition
 * to a state.
 */
TEST(Hsm, DispatchStateTransitionToCompositeStateWithNoInitialTransition)
{
    try
    {
        /* Step 1: Arrange. Transition from S to S2.
        TOP
        |
        S---S2
            |
            S21
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S2>()
                                .with_parent_top();

        state_S2 = &hsm_state<S2>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_no_mock() /* Not allowed. */
                                .with_handler_no_mock()
                                .with_parent_top();

        state_S21 = &hsm_state<S21>::get_instance()
                                    .with_entry_unused()
                                    .with_exit_unused()
                                    .with_initial_unused()
                                    .with_handler_no_mock()
                                    .with_parent<S2>();

        ecu_hsm_ctor(&me, state_S, 2);
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. All initial transitions must be to children
 * to drill into state hierarchy.
 */
TEST(Hsm, DispatchStateTransitionToCompositeStateWithInitialTransitionToParent)
{
    try
    {
        /* Step 1: Arrange. Transition from S to S21.
        TOP
        |
        S---S2
            |
            S21
            |
            S211
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S21>()
                                .with_parent_top();

        state_S2 = &hsm_state<S2>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_no_mock_to<S211>()
                                .with_handler_no_mock()
                                .with_parent_top();

        state_S21 = &hsm_state<S21>::get_instance()
                                    .with_entry_unused()
                                    .with_exit_unused()
                                    .with_initial_no_mock_to<S2>() /* Not allowed. */
                                    .with_handler_no_mock()
                                    .with_parent<S2>();

        state_S211 = &hsm_state<S211>::get_instance()
                                    .with_entry_unused()
                                    .with_exit_unused()
                                    .with_initial_unused()
                                    .with_handler_no_mock()
                                    .with_parent<S21>();

        ecu_hsm_ctor(&me, state_S, 3);
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. All initial transitions must be to children
 * to drill into state hierarchy.
 */
TEST(Hsm, DispatchStateTransitionToCompositeStateWithInitialTransitionToSelf)
{
    try
    {
        /* Step 1: Arrange. Transition from S1 to S. 
        TOP
        |
        S
        |
        S1
        */
        ecu_hsm me;

        state_S = &hsm_state<S>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_no_mock_to<S>() /* Not allowed. */
                                .with_handler_no_mock()
                                .with_parent_top();

        state_S1 = &hsm_state<S1>::get_instance()
                                .with_entry_unused()
                                .with_exit_unused()
                                .with_initial_unused()
                                .with_handler_no_mock_to<S>()
                                .with_parent<S>();

        ecu_hsm_ctor(&me, state_S1, 2);
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*---------------- TESTS - HSM_VARIANT1 START ----------------*/
/*------------------------------------------------------------*/

/// @brief Verify proper state path.
TEST(Hsm, HsmVariant1Start)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        EXPECT_STATE_PATH(entered<S>(), entered<S2>(), init<S2>(), entered<S21>(), entered<S211>());

        /* Step 2: Action. */
        ecu_hsm_start(&me);

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*---------------- TESTS - HSM_VARIANT1 S211 -----------------*/
/*------------------------------------------------------------*/

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS211EventA)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), exited<S211>(), exited<S21>(), entered<S21>(), init<S21>(), entered<S211>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_A));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventB)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), exited<S211>(), entered<S211>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_B));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventC)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), handled<S2>(), exited<S211>(), exited<S21>(), exited<S2>(),
                          entered<S1>(), init<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_C));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventD)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), exited<S211>(), init<S21>(), entered<S211>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_D));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventE)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), handled<S2>(), handled<S>(), exited<S211>(),
                          exited<S21>(), exited<S2>(), entered<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_E));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventF)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), handled<S2>(), exited<S211>(),
                          exited<S21>(), exited<S2>(), entered<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_F));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventG)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), exited<S211>(), exited<S21>(),
                          exited<S2>(), entered<S1>(), init<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_G));

        /* Step 3: Assert. Test fails if expected state path not taken. */     
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventH)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), exited<S211>(), exited<S21>(), exited<S2>(), 
                          init<S>(), entered<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_H));

        /* Step 3: Assert. Test fails if expected state path not taken. */  
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventIFooFalse)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), handled<S2>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_I));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211EventIFooTrue)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        mock().disable();
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_I)); /* Set foo true. */
        mock().enable();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), handled<S2>(), handled<S>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_I));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1S211DummyEvent)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), handled<S2>(), handled<S>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------------- TESTS - HSM_VARIANT1 S11 -----------------*/
/*------------------------------------------------------------*/

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventA)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), handled<S1>(), exited<S11>(), exited<S1>(), 
                          entered<S1>(), init<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_A));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventB)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), handled<S1>(), exited<S11>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_B));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventC)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), handled<S1>(), exited<S11>(), exited<S1>(),
                          entered<S2>(), init<S2>(), entered<S21>(), entered<S211>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_C));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventDFooFalse)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), handled<S1>(), exited<S11>(), exited<S1>(),
                          init<S>(), entered<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_D));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventDFooTrue)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        mock().disable();
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_D)); /* Set foo true. */
        mock().enable();
        EXPECT_STATE_PATH(handled<S11>(), exited<S11>(), init<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_D));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventE)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), handled<S1>(), handled<S>(), exited<S11>(),
                          exited<S1>(), entered<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_E));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventF)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), handled<S1>(), exited<S11>(), exited<S1>(),
                          entered<S2>(), entered<S21>(), entered<S211>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_F));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventG)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), exited<S11>(), exited<S1>(), entered<S2>(),
                          entered<S21>(), entered<S211>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_G));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11EventH)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), exited<S11>(), exited<S1>(), init<S>(),
                          entered<S1>(), entered<S11>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_H));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariantS11DummyEvent)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s11_no_mock();
        EXPECT_STATE_PATH(handled<S11>(), handled<S1>(), handled<S>());

        /* Step 2: Action. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));

        /* Step 3: Assert. Test fails if expected state path not taken. */   
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------------- TESTS - HSM_VARIANT1 MISC ----------------*/
/*------------------------------------------------------------*/

/// @brief Verify proper event handling and state paths.
TEST(Hsm, HsmVariant1DispatchMultipleEvents)
{
    try
    {
        /* Step 1: Arrange. */
        hsm_variant1 me;
        me.start_s211_no_mock();
        EXPECT_STATE_PATH(handled<S211>(), handled<S21>(), handled<S2>(), /* In S211. I */
                          handled<S211>(), handled<S21>(), handled<S2>(), handled<S>(), /* In S211. I */
                          handled<S211>(), handled<S21>(), exited<S211>(), exited<S21>(), entered<S21>(), init<S21>(), entered<S211>(), /* In S211. A */
                          handled<S211>(), handled<S21>(), exited<S211>(), exited<S21>(), exited<S2>(), entered<S1>(), init<S1>(), entered<S11>(), /* In S211. G */
                          handled<S11>(), exited<S11>(), exited<S1>(), entered<S2>(), entered<S21>(), entered<S211>(), /* In S11. G */
                          handled<S211>(), handled<S21>(), handled<S2>(), handled<S>(), /* In S211. DUMMY_EVENT */
                          handled<S211>(), handled<S21>(), handled<S2>(), handled<S>(), exited<S211>(), exited<S21>(), exited<S2>(), entered<S1>(), entered<S11>(), /* In S211. E */
                          handled<S11>(), handled<S1>(), exited<S11>(), exited<S1>(), init<S>(), entered<S1>(), entered<S11>(), /* In S11. D */
                          handled<S11>(), exited<S11>(), init<S1>(), entered<S11>(), /* In S11. D */
                          handled<S11>(), handled<S1>(), exited<S11>(), exited<S1>(), init<S>(), entered<S1>(), entered<S11>(), /* In S11. D */
                          handled<S11>(), handled<S1>(), handled<S>(), exited<S11>(), exited<S1>(), entered<S1>(), entered<S11>(), /* In S11. E. */
                          handled<S11>(), handled<S1>(), exited<S11>(), exited<S1>(), entered<S2>(), init<S2>(), entered<S21>(), entered<S211>() /* In S11. C */);

        /* Step 2: Action. I, I, A, G, DUMMY_EVENT, E, D, D, D, E, C. */
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_I));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_I));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_A));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_G));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_G));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&DUMMY_EVENT));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_E));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_D));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_D));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_D));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_E));
        ecu_hsm_dispatch(&me, static_cast<const void *>(&EVENT_C));

        /* Step 3: Assert. Test fails if expected state path not taken. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}
