/**
 * @file
 * @brief
 * @rst
 * See :ref:`hsm.h section <hsm_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-01
 * @copyright Copyright (c) 2025
 */

#ifndef ECU_HSM_H_
#define ECU_HSM_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verifies, at compile-time, that derived hsm
 * correctly inherits @ref ecu_hsm base class. Returns
 * true if correctly inherited. False otherwise.
 * 
 * @param base_ Name of @ref ecu_hsm <b>member</b> within 
 * user's @p derived_ hsm type.
 * @param derived_ Derived hsm type to check.
 */
#define ECU_HSM_IS_BASEOF(base_, derived_) \
    ((bool)(offsetof(derived_, base_) == (size_t)0))

/**
 * @brief Upcasts derived hsm pointer into @ref ecu_hsm
 * base class pointer. This macro encapsulates the cast
 * and allows derived hsms to be passed into base class 
 * functions defined in this module.
 * 
 * @param me_ Pointer to derived hsm. This must inherit
 * @ref ecu_hsm base class and must be pointer to non-const.
 */
#define ECU_HSM_BASE_CAST(me_) \
    ((struct ecu_hsm *)(me_))

/**
 * @brief Helper macro supplied to @ref ECU_HSM_STATE_CTOR()
 * if state's entry handler is unused.
 */ 
#define ECU_HSM_STATE_ENTRY_UNUSED \
    ((void (*)(struct ecu_hsm *))0)

/**
 * @brief Helper macro supplied to @ref ECU_HSM_STATE_CTOR()
 * if state's exit handler is unused.
 */
#define ECU_HSM_STATE_EXIT_UNUSED \
    ((void (*)(struct ecu_hsm *))0)

/**
 * @brief Helper macro supplied to @ref ECU_HSM_STATE_CTOR()
 * if state has no parent.
 * 
 * @warning This should only be used to initialize the top state.
 */
#define ECU_HSM_STATE_NO_PARENT \
    ((const struct ecu_hsm_state *)0)

/**
 * @brief Creates a state at compile-time. Supplied state
 * functions taking in a derived hsm pointer are upcasted
 * to state functions taking in an @ref ecu_hsm base class
 * pointer. The results of these casts are assigned to
 * members of @ref ecu_hsm_state. This macro encapsulates
 * all casts and member initializations of @ref ecu_hsm_state.
 * Example usage:
 * @code{.c}
 * static const struct ecu_hsm_state STATE1 = ECU_HSM_STATE_CTOR(
 *      &entry_handler, ECU_HSM_STATE_EXIT_UNUSED, &state_handler, &PARENT_STATE
 * );
 * @endcode
 * 
 * @param entry_ Optional function that executes when state is 
 * first entered. A state transition CANNOT be done in the entry 
 * handler. Supply @ref ECU_HSM_STATE_ENTRY_UNUSED if unused.
 * @param exit_ Optional function that executes when state exits.
 * A state transition CANNOT be done in the exit handler.
 * Supply @ref ECU_HSM_STATE_EXIT_UNUSED if unused.
 * @param handler_ Mandatory function that processes events
 * dispatched to this state. Return true if the dispatched event
 * is processed within this state. Return false if the event should
 * be propogated up the state hierarchy.
 * @param parent_ This state's parent. @ref ECU_HSM_STATE_NO_PARENT
 * should only be supplied if this is the top state. Otherwise this
 * must always be another state.
 */
#define ECU_HSM_STATE_CTOR(entry_, exit_, handler_, parent_)                \
    {                                                                       \
        .entry = (void (*)(struct ecu_hsm *))(entry_),                      \
        .exit = (void (*)(struct ecu_hsm *))(exit_),                        \
        .handler = (bool (*)(struct ecu_hsm *, const void *))(handler_),    \
        .parent = (parent_)                                                 \
    }

/*------------------------------------------------------------*/
/*---------------------------- HSM ---------------------------*/
/*------------------------------------------------------------*/

/* Forward declaration for ecu_hsm_state. */
struct ecu_hsm;

/**
 * @brief Single state in hsm, initialized via @ref ECU_HSM_STATE_CTOR().
 * Pointers are const-qualified to only allow states to be
 * created at compile-time.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_hsm_state
{
/* Normally, pointers are const-qualified to only allow states to
be created at compile-time. Const is stripped only for unit tests 
since test code framework requires state creation at run-time. */
#ifdef ECU_UNIT_TEST
    /// @brief Executes when state first entered. Optional.
    void (*entry)(struct ecu_hsm *me);

    /// @brief Executes when state exits. Optional.
    void (*exit)(struct ecu_hsm *me);

    /// @brief Processes events dispatched to this state. Mandatory.
    /// Return true if the dispatched event is processed within this state. 
    /// Return false if the event should be propogated up the state hierarchy.
    bool (*handler)(struct ecu_hsm *me, const void *event);

    /// @brief This state's parent. Can only be @ref ECU_HSM_STATE_NO_PARENT
    /// if this is the top state. Otherwise this must always be another state.
    const struct ecu_hsm_state *parent;
#else
    /// @brief Executes when state first entered. Optional.
    void (*const entry)(struct ecu_hsm *me);

    /// @brief Executes when state exits. Optional.
    void (*const exit)(struct ecu_hsm *me);

    /// @brief Processes events dispatched to this state. Mandatory.
    /// Return true if the dispatched event is processed within this state. 
    /// Return false if the event should be propogated up the state hierarchy.
    bool (*const handler)(struct ecu_hsm *me, const void *event);

    /// @brief This state's parent. Can only be @ref ECU_HSM_STATE_NO_PARENT
    /// if this is the top state. Otherwise this must always be another state.
    const struct ecu_hsm_state *const parent;
#endif
};

/**
 * @brief Base hsm class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_hsm
{
    /// @brief Current state the hsm is in.
    const struct ecu_hsm_state *state;

    /// @brief The hsm's top state.
    const struct ecu_hsm_state *top;

    /// @brief Number of levels in the hsm, starting at 0. Used
    /// as a fail safe to avoid infinite loops when traversing up 
    /// the state hierarchy.
    uint8_t height;

    /// @brief Bitmap representing hsm's state transition context.
    uint32_t transition;
};

/*------------------------------------------------------------*/
/*-------------------- HSM MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Hsm Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p state and @p top constructed via @ref ECU_HSM_STATE_CTOR().
 * @brief Hsm constructor.
 * 
 * @param me Hsm to construct.
 * @param state Hsm's initial state.
 * @param top Hsm's top state.
 * @param height Number of levels in the hsm, starting at 0. For
 * example if the hsm only consists of a top state, this would be 0.
 */
extern void ecu_hsm_ctor(struct ecu_hsm *me, 
                         const struct ecu_hsm_state *state,
                         const struct ecu_hsm_state *top,
                         uint8_t height);
/**@}*/

/**
 * @name Hsm Member Functions
 */
/**@{*/
/**
 * @pre @p me constructed via @ref ecu_hsm_ctor().
 * @brief Runs each state's entry handler, starting from
 * the top state and ending at the initial state assigned
 * in @ref ecu_hsm_ctor(). Inclusive, meaning top state's
 * entry and the initial state's entry are ran.
 * 
 * @warning This function should only be called once on
 * startup and must run to completion.
 * 
 * @param me Hsm to start. This should not be an already 
 * running hsm.
 */
extern void ecu_hsm_start(struct ecu_hsm *me);

/**
 * @pre @p me constructed via @ref ecu_hsm_ctor().
 * @pre @p state constructed via @ref ECU_HSM_STATE_CTOR().
 * @brief Transitions hsm into a new state.
 * 
 * @warning This can only be called within @ref ecu_hsm_state.handler.
 * 
 * @param me Hsm to transition.
 * @param state State to transition into. States are exited up to the
 * least common ancestor (LCA). States are entered from the LCA to
 * @p state. Entry and exit of LCA is not ran. A local transition occurs
 * if @p state is a child or parent state. A self-transition occurs
 * if hsm's current state == @p state. See 
 * @rst 
 * :ref:`hsm.h section in Sphinx documentation <hsm_state_transitions>` 
 * @endrst
 * for more details.
 */
extern void ecu_hsm_change_state(struct ecu_hsm *me, const struct ecu_hsm_state *state);

/**
 * @pre @p me constructed via @ref ecu_hsm_ctor().
 * @brief Relays event to hsm where it is processed by
 * the current state's handler function. The event is propogated 
 * up the state hierarchy until it is handled. All state transitions
 * signalled via @ref ecu_hsm_change_state() are also processed
 * in this function.
 * 
 * @warning This function must run to completion.
 * 
 * @param me Hsm to run.
 * @param event Event to dispatch. This cannot be NULL.
 */
extern void ecu_hsm_dispatch(struct ecu_hsm *me, const void *event);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_HSM_H_ */
