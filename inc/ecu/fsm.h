/**
 * @file
 * @brief 
 * @rst
 * See :ref:`fsm.h section <fsm_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-14
 * @copyright Copyright (c) 2025
 */

#ifndef ECU_FSM_H_
#define ECU_FSM_H_

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
 * @brief Verifies, at compile-time, that derived fsm
 * correctly inherits @ref ecu_fsm base class. Returns
 * true if correctly inherited. False otherwise.
 * 
 * @param base_ Name of @ref ecu_fsm <b>member</b> within 
 * user's @p derived_ fsm type.
 * @param derived_ Derived fsm type to check.
 */
#define ECU_FSM_IS_BASEOF(base_, derived_) \
    ((bool)(offsetof(derived_, base_) == (size_t)0))

/**
 * @brief Upcasts derived fsm pointer into @ref ecu_fsm
 * base class pointer. This macro encapsulates the cast
 * and allows derived fsms to be passed into base class 
 * functions defined in this module.
 * 
 * @param me_ Pointer to derived fsm. This must inherit
 * @ref ecu_fsm base class and must be pointer to non-const.
 */
#define ECU_FSM_BASE_CAST(me_) \
    ((struct ecu_fsm *)(me_))

/**
 * @brief Helper macro supplied to @ref ECU_FSM_STATE_CTOR()
 * if state's entry handler is unused.
 */
#define ECU_FSM_STATE_ENTRY_UNUSED \
    ((void (*)(struct ecu_fsm *))0)

/**
 * @brief Helper macro supplied to @ref ECU_FSM_STATE_CTOR()
 * if state's exit handler is unused.
 */
#define ECU_FSM_STATE_EXIT_UNUSED \
    ((void (*)(struct ecu_fsm *))0)

/**
 * @brief Creates a state at compile-time. Supplied state
 * functions taking in a derived fsm pointer are upcasted
 * to state functions taking in an @ref ecu_fsm base class
 * pointer. The results of these casts are assigned to
 * members of @ref ecu_fsm_state. This macro encapsulates
 * all casts and member initializations of @ref ecu_fsm_state.
 * Example usage:
 * @code{.c}
 * static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
 *      &entry_handler, ECU_FSM_STATE_EXIT_UNUSED, &state_handler
 * );
 * @endcode
 * 
 * @param entry_ Optional function that executes when state is 
 * first entered. Supply @ref ECU_FSM_STATE_ENTRY_UNUSED if unused.
 * @param exit_ Optional function that executes when state exits.
 * Supply @ref ECU_FSM_STATE_EXIT_UNUSED if unused.
 * @param handler_ Mandatory function that processes events
 * dispatched to this state.
 */
#define ECU_FSM_STATE_CTOR(entry_, exit_, handler_)                     \
    {                                                                   \
        .entry = (void (*)(struct ecu_fsm *))(entry_),                  \
        .exit = (void (*)(struct ecu_fsm *))(exit_),                    \
        .handler = (void (*)(struct ecu_fsm *, const void *))(handler_) \
    }

/*------------------------------------------------------------*/
/*---------------------------- FSM ---------------------------*/
/*------------------------------------------------------------*/

/* Forward declaration for ecu_fsm_state. */
struct ecu_fsm;

/**
 * @brief Single state in fsm, initialized via @ref ECU_FSM_STATE_CTOR().
 * Pointers are const-qualified to only allow states to be
 * created at compile-time.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_fsm_state
{
    /* Normally, pointers are const-qualified to only allow states to
    be created at compile-time. Const is stripped only for unit tests 
    since test code framework requires state creation at run-time. */
#ifdef ECU_UNIT_TEST
    /// @brief Executes when state first entered. Optional.
    void (*entry)(struct ecu_fsm *fsm);
    
    /// @brief Executes when state exits. Optional.
    void (*exit)(struct ecu_fsm *fsm);

    /// @brief Processes events dispatched to this state. Mandatory.
    void (*handler)(struct ecu_fsm *fsm, const void *event);
#else
    /// @brief Executes when state first entered. Optional.
    void (*const entry)(struct ecu_fsm *fsm);
    
    /// @brief Executes when state exits. Optional.
    void (*const exit)(struct ecu_fsm *fsm);

    /// @brief Processes events dispatched to this state. Mandatory.
    void (*const handler)(struct ecu_fsm *fsm, const void *event);
#endif
};

/**
 * @brief Base fsm class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_fsm
{
    /// @brief Current state the fsm is in.
    const struct ecu_fsm_state *state;

    /// @brief Bitmap representing fsm's state transition context.
    uint32_t transition;
};

/*------------------------------------------------------------*/
/*-------------------- FSM MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Fsm Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p state constructed via @ref ECU_FSM_STATE_CTOR().
 * @brief Fsm constructor.
 * 
 * @param me Fsm to construct.
 * @param state Fsm's initial state. 
 */
extern void ecu_fsm_ctor(struct ecu_fsm *me, const struct ecu_fsm_state *state);
/**@}*/

/**
 * @name Fsm Member Functions
 */
/**@{*/
/**
 * @pre @p me constructed via @ref ecu_fsm_ctor().
 * @brief Runs the initial state's entry handler and manages 
 * all state transition logic if any state changes were siganlled 
 * via @ref ecu_fsm_change_state(). This function does nothing
 * if the initial state's entry handler is unused.
 * 
 * @warning This function should only be called once on
 * startup and must run to completion.
 * 
 * @param me Fsm to start. This should not be an already 
 * running fsm.
 */
extern void ecu_fsm_start(struct ecu_fsm *me);

/**
 * @pre @p me constructed via @ref ecu_fsm_ctor().
 * @pre @p state constructed via @ref ECU_FSM_STATE_CTOR().
 * @brief Transitions fsm into a new state.
 * 
 * @warning This must only be called within @ref ecu_fsm_state.handler
 * and @ref ecu_fsm_state.entry.
 * 
 * @param me Fsm to transition.
 * @param state State to transition into. If fsm's current state == @p state
 * then a self-transition will occur. I.e. @p state::exit then @p state::entry.
 */
extern void ecu_fsm_change_state(struct ecu_fsm *me, const struct ecu_fsm_state *state);

/**
 * @pre @p me constructed via @ref ecu_fsm_ctor().
 * @brief Relays event to fsm where it is processed by
 * the current state's handler function. Manages all
 * state transition logic if any state changes were
 * signalled via @ref ecu_fsm_change_state().
 * 
 * @warning This function must run to completion.
 * 
 * @param me Fsm to run.
 * @param event Event to dispatch. This cannot be NULL.
 */
extern void ecu_fsm_dispatch(struct ecu_fsm *me, const void *event);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_FSM_H_ */
