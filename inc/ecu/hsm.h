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

/* ECU. */
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Converts intrusive @ref ecu_hsm member into the
 * user's hsm type. This should be used inside each state's 
 * definition.
 *
 * @param ecu_hsm_ptr_ Pointer to intrusive @ref ecu_hsm.
 * This must be pointer to non-const. I.e. (struct ecu_hsm *).
 * @param type_ User's hsm type containing the intrusive
 * @ref ecu_hsm member. Do not use const specifier. I.e. 
 * (struct my_type), never (const struct my_type).
 * @param member_ Name of @ref ecu_hsm member within user's
 * type.
 */
#define ECU_HSM_GET_CONTEXT(ecu_hsm_ptr_, type_, member_) \
    ECU_CONTAINER_OF(ecu_hsm_ptr_, type_, member_)

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
 * if state's initial handler is unused.
 * 
 * @warning This can only be used for leaf states.
 */
#define ECU_HSM_STATE_INITIAL_UNUSED \
    ((void (*)(struct ecu_hsm *))0)

/**
 * @brief Creates an @ref ecu_hsm_state at compile-time.
 * Example usage:
 * @code{.c}
 * static const struct ecu_hsm_state STATE1 = ECU_HSM_STATE_CTOR(
 *      &entry_handler, ECU_HSM_STATE_EXIT_UNUSED, &state_handler, &initial_handler, &PARENT_STATE
 * );
 * @endcode
 * 
 * @param entry_ Optional function that executes when state is 
 * first entered. This must be of type (void (*)(struct ecu_hsm *)).
 * Supply @ref ECU_FSM_STATE_ENTRY_UNUSED if unused.
 * @param exit_ Optional function that executes when state exits.
 * This must be of type (void (*)(struct ecu_hsm *)).
 * Supply @ref ECU_FSM_STATE_EXIT_UNUSED if unused.
 * @param initial_ Function that transitions to starting (child) state
 * when a transition targets a composite state. Mandatory for
 * composite states. Otherwise supply @ref ECU_HSM_STATE_INITIAL_UNUSED
 * for leaf states. This must be of type (void (*)(struct ecu_hsm *)).
 * @param handler_ Mandatory function that processes events
 * dispatched to this state. This must be of type
 * (void (*)(struct ecu_hsm *, const void *)).
 * @param parent_ This state's parent. Supply pointer to @ref ECU_HSM_TOP_STATE
 * if this state's parent is the default top state.
 */
#define ECU_HSM_STATE_CTOR(entry_, exit_, initial_, handler_, parent_)  \
    {                                                                   \
        .entry = (entry_),                                              \
        .exit = (exit_),                                                \
        .initial = (initial_),                                          \
        .handler = (handler_),                                          \
        .parent = (parent_)                                             \
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

    /// @brief Transitions to starting (child) state when a transition
    /// targets a composite state. Mandatory for composite states.
    /// Set to @ref ECU_HSM_STATE_INITIAL_UNUSED for leaf states.
    void (*initial)(struct ecu_hsm *me);

    /// @brief Processes events dispatched to this state. Mandatory.
    /// Return true if the dispatched event is handled within this state. 
    /// Return false if the event should be propagated up the state hierarchy.
    bool (*handler)(struct ecu_hsm *me, const void *event);

    /// @brief This state's parent.
    const struct ecu_hsm_state *parent;
#else
    /// @brief Executes when state first entered. Optional.
    void (*const entry)(struct ecu_hsm *me);

    /// @brief Executes when state exits. Optional.
    void (*const exit)(struct ecu_hsm *me);

    /// @brief Transitions to starting (child) state when a transition
    /// targets a composite state. Mandatory for composite states.
    /// Set to @ref ECU_HSM_STATE_INITIAL_UNUSED for leaf states.
    void (*const initial)(struct ecu_hsm *me);

    /// @brief Processes events dispatched to this state. Mandatory.
    /// Return true if the dispatched event is processed within this state. 
    /// Return false if the event should be propagated up the state hierarchy.
    bool (*const handler)(struct ecu_hsm *me, const void *event);

    /// @brief This state's parent.
    const struct ecu_hsm_state *const parent;
#endif
};

/**
 * @brief Hierarchical state machine. Users create their own
 * HSMs by containing this as an intrusive member.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_hsm
{
    /// @brief Current state the hsm is in.
    const struct ecu_hsm_state *state;

    /// @brief Number of levels in the hsm, starting at 0. Used
    /// as a fail safe to avoid infinite loops when traversing up 
    /// the state hierarchy.
    uint8_t height;

    /// @brief Bitmap representing hsm's state transition context.
    uint32_t transition;
};

/*------------------------------------------------------------*/
/*---------------------- GLOBAL VARIABLES --------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The default top state. Serves as the root
 * of every hsm.
 */
extern const struct ecu_hsm_state ECU_HSM_TOP_STATE;

/*------------------------------------------------------------*/
/*-------------------- HSM MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

/**
 * @name Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p state constructed via @ref ECU_HSM_STATE_CTOR().
 * @brief Hsm constructor.
 * 
 * @param me Hsm to construct.
 * @param state Hsm's initial state. This cannot be @ref ECU_HSM_TOP_STATE.
 * @param height Number of levels in the hsm, starting at 1. For
 * example if the hsm only consists of a single state, this would 
 * be 1 (user's state and @ref ECU_HSM_TOP_STATE).
 */
extern void ecu_hsm_ctor(struct ecu_hsm *me, 
                         const struct ecu_hsm_state *state,
                         uint8_t height);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p state constructed via @ref ECU_HSM_STATE_CTOR().
 * @brief Transitions hsm into a new state.
 * 
 * @warning This can only be called within @ref ecu_hsm_state.handler
 * and @ref ecu_hsm_state.initial. If a transition is signalled 
 * in @ref ecu_hsm_state.handler, it must handle the event by 
 * returning true. The HSM must be in a leaf state after all 
 * transitions are completed.
 * 
 * @param me Hsm to transition.
 * @param state State to transition into. This cannot
 * be @ref ECU_HSM_TOP_STATE.
 */
extern void ecu_hsm_change_state(struct ecu_hsm *me, const struct ecu_hsm_state *state);

/**
 * @pre @p me constructed via @ref ecu_hsm_ctor() and started
 * via @ref ecu_hsm_start().
 * @brief Relays event to hsm where it is processed by
 * the current state's handler function. The event is propagated 
 * up the state hierarchy until it is handled. All state transitions
 * signalled via @ref ecu_hsm_change_state() are also managed
 * in this function.
 * 
 * @warning This function must run to completion. The HSM 
 * must be in a leaf state after this function completes.
 * 
 * @param me Hsm to run.
 * @param event Event to dispatch. This cannot be NULL.
 */
extern void ecu_hsm_dispatch(struct ecu_hsm *me, const void *event);

/**
 * @pre @p me constructed via @ref ecu_hsm_ctor().
 * @brief Starts the hsm by entering from @ref ECU_HSM_TOP_STATE
 * to the target state supplied in @ref ecu_hsm_ctor(). If the
 * target is a composite state, initial handlers are ran to
 * fully transition down the state hierarchy.
 * 
 * @warning This function should only be called once on
 * startup and must run to completion. The HSM must 
 * be in a leaf state after this function completes.
 * 
 * @param me Hsm to start. This should not be an already 
 * running hsm.
 */
extern void ecu_hsm_start(struct ecu_hsm *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_HSM_H_ */
