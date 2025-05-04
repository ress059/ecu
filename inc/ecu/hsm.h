/**
 * @file
 * @brief todo
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
 
 #define ECU_HSM_IS_BASEOF(base_, derived_) \
    ((bool)(offsetof(derived_, base_) == (size_t)0))
 
 #define ECU_HSM_BASE_CAST(me_) \
    ((struct ecu_hsm *)(me_))
 
 #define ECU_HSM_STATE_ENTRY_UNUSED \
    ((void (*)(struct ecu_hsm *))0)
 
 #define ECU_HSM_STATE_EXIT_UNUSED \
    ((void (*)(struct ecu_hsm *))0)
 
 #define ECU_HSM_STATE_NO_PARENT \
    ((const struct ecu_hsm_state *)0)
 
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
 
 struct ecu_hsm_state
 {
    void (*entry)(struct ecu_hsm *me);
    void (*exit)(struct ecu_hsm *me);
    /* return true if event handled. return false to propogate event to parents. */
    bool (*handler)(struct ecu_hsm *me, const void *event);
    const struct ecu_hsm_state *const parent;
 };
 
 struct ecu_hsm
 {
    const struct ecu_hsm_state *state;
 
    /*number of levels in hsm. fail safe. assertion fires if number 
    of parent traversals exceeds this amount. */
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
 #warning "TODO: Still thinking about supplied top state to ctor"
 extern void ecu_hsm_ctor(struct ecu_hsm *me, 
                          const struct ecu_hsm_state *state,
                          const struct ecu_hsm_state *top,
                          uint8_t height);
 /**@}*/
 
 /**
  * @name Hsm Member Functions
  */
 /**@{*/
 extern void ecu_hsm_start(struct ecu_hsm *me);
 
 extern void ecu_hsm_change_state(struct ecu_hsm *me, const struct ecu_hsm_state *state);
 
 extern void ecu_hsm_dispatch(struct ecu_hsm *me, const void *event);
 /**@}*/
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* ECU_HSM_H_ */