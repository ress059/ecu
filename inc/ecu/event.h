/**
 * @file
 * @brief
 * @rst
 * See :ref:`event.h section <event_h>` in Sphinx documentation.
 * @endrst
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-04-07
 * @copyright Copyright (c) 2024
 */


#ifndef ECU_EVENT_H_
#define ECU_EVENT_H_



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ INCLUDES ----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- MACROS ------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Verify user-defined event correctly inherits @ref ecu_event.
 * @details Returns true if @ref ecu_event is a base class of @p derived.
 * False otherwise. This macro is meant to be used at compile-time.
 * 
 * @param base Name of @ref ecu_event member declared in user's @p derived type.
 * @param derived User-defined event type that inherits @ref ecu_event base class.
 * I.e. struct derived_event.
 */
#define ECU_EVENT_IS_BASE_OF(base, derived)     ((bool)(offsetof(derived, base) == (size_t)(0)))

/**
 * @brief Upcasts derived event class pointer @p e to an @ref ecu_event 
 * base class pointer.
 * @details This macro should be used when dispatching derived events to 
 * ECU library functions.
 * 
 * @param e Pointer to user-defined event that inherits @ref ecu_event. 
 * This can be a pointer to const or non-const.
 */
#define ECU_EVENT_BASE_CAST(e)                  ((const struct ecu_event *)(e))



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------ EVENT IDS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Event IDs reserved for ECU library.
 * @details Scheme that allows the library and users to uniquely define
 * their own event IDs without conflicts. Event IDs reserved for ECU
 * library will always be negative. Event IDs the user defines will 
 * start at 0 which is always @ref ECU_USER_EVENT_ID_BEGIN.
 */
enum ecu_reserved_event_ids
{
    /*------------------------------------------------------------------*/
    /*--------------------- RESERVED EVENT IDS SECTION -----------------*/
    /*--------- LAST MEMBER MUST EQUAL ECU_VALID_EVENT_ID_BEGIN --------*/
    /*------------------------------------------------------------------*/
    /* ADD FUTURE PRIVATE AND RESERVED EVENTS HERE. */
    ECU_RESERVED_EVENT_ID       = -1,   /**< PRIVATE. FOR LIBRARY USE ONLY. */
    /*******************************/
    ECU_VALID_EVENT_ID_BEGIN    = 0,    /**< PRIVATE. FOR LIBRARY USE ONLY. Start of reserved IDs that the user can assign to events. */

    /*------------------------------------------------------------------*/
    /*--------------------- AVAILABLE EVENT IDS SECTION ----------------*/
    /*--------- FIRST MEMBER MUST EQUAL ECU_VALID_EVENT_ID_BEGIN -------*/
    /*------------------------------------------------------------------*/
    ECU_USER_EVENT_ID_BEGIN     = ECU_VALID_EVENT_ID_BEGIN  /**< 0. Start of user-defined event IDs. Will always be 0 for future compatibility. */
};

/**
 * @private 
 * @brief PRIVATE. Generic type that represents an event ID. 
 * @details Used so the library can implicitly typecast between this 
 * value, @ref ecu_reserved_event_ids enumerations, and user-defined 
 * event ID enumerations.
 * 
 * @warning This must be a signed integer type in order to 
 * handle @ref ecu_reserved_event_ids enumerations less than 0. A 
 * compilation error will occur if this is declared as an unsigned 
 * type.
 */
typedef int32_t ecu_event_id;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- BASE EVENT CLASS ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Base event class. 
 * @details Only contains the minimum event information required which 
 * is the event ID. Users inherit this class to add custom data to their
 * events.
 */
struct ecu_event
{
    /**
     * @brief Identifies the type of event that was dispatched.
     */
    ecu_event_id id;
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- PUBLIC FUNCTIONS -----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @pre Memory already allocated for @p me
 * @brief Assign ID to an event.
 * 
 * @param me Event to initialize. This cannot be NULL. To pass in a 
 * derived event, user must explicitly upcast to an @ref ecu_event 
 * base class. Do not use @ref ECU_EVENT_BASE_CAST() since the macro
 * const casts. 
 * @param id_0 User-defined event ID. This must be greater than or 
 * equal to @ref ECU_VALID_EVENT_ID_BEGIN.
 */
extern void ecu_event_ctor(struct ecu_event *me, 
                           ecu_event_id id_0);

#ifdef __cplusplus
}
#endif

#endif /* ECU_EVENT_H_ */
