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

/* ECU. */
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Converts intrusive @ref ecu_fsm member into the
 * user's FSM type.
 *
 * @param ecu_fsm_ptr_ Pointer to intrusive @ref ecu_fsm.
 * This must be pointer to non-const. I.e. (struct ecu_fsm *).
 * @param type_ User's FSM type containing the intrusive
 * @ref ecu_fsm member. Do not use const specifier. I.e. 
 * (struct my_type), never (const struct my_type).
 * @param member_ Name of @ref ecu_fsm member within user's
 * type.
 */
#define ECU_FSM_GET_CONTEXT(ecu_fsm_ptr_, type_, member_) \
    ECU_CONTAINER_OF(ecu_fsm_ptr_, type_, member_)

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
 * @brief Creates an @ref ecu_fsm_state at compile-time.
 * Example usage:
 * @code{.c}
 * static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
 *      &entry_handler, ECU_FSM_STATE_EXIT_UNUSED, &state_handler
 * );
 * @endcode
 * 
 * @param entry_ Optional function that executes when state is 
 * first entered. This must be of type (void (*)(struct ecu_fsm *)).
 * Supply @ref ECU_FSM_STATE_ENTRY_UNUSED if unused.
 * @param exit_ Optional function that executes when state exits.
 * This must be of type (void (*)(struct ecu_fsm *)).
 * Supply @ref ECU_FSM_STATE_EXIT_UNUSED if unused.
 * @param handler_ Mandatory function that processes events
 * dispatched to this state. This must be of type
 * (void (*)(struct ecu_fsm *, const void *)).
 */
#define ECU_FSM_STATE_CTOR(entry_, exit_, handler_) \
    {                                               \
        .entry = (entry_),                          \
        .exit = (exit_),                            \
        .handler = (handler_)                       \
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
 * @brief Finite state machine. Users create their own
 * FSMs by containing this as an intrusive member.
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
 * @name Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p state constructed via @ref ECU_FSM_STATE_CTOR().
 * @brief Fsm constructor.
 * 
 * @warning Supplied FSM cannot be active, otherwise behavior
 * is undefined.
 * 
 * @param me Fsm to construct.
 * @param state Fsm's initial state. 
 */
extern void ecu_fsm_ctor(struct ecu_fsm *me, const struct ecu_fsm_state *state);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
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
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_FSM_H_ */
