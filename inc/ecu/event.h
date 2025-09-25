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

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stddef.h>
#include <stdint.h>

/* ECU. */
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*-------------------------- MACROS --------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Upcasts derived event back into the @ref ecu_event 
 * base class.
 * 
 * @param event_ptr_ Pointer to derived event. This type
 * must inherit @ref ecu_event base class and must be
 * pointer to non-const.
 */
#define ECU_EVENT_BASE_CAST(event_ptr_) \
    ((struct ecu_event *)(event_ptr_))

/**
 * @brief Same as @ref ECU_EVENT_BASE_CAST() but performs
 * a const-qualified upcast.
 * 
 * @param event_ptr_ Pointer to derived event. This type
 * must inherit @ref ecu_event base class. Can be either
 * pointer to const or non-const.
 */
#define ECU_EVENT_CONST_BASE_CAST(event_ptr_) \
    ((const struct ecu_event *)(event_ptr_))

/**
 * @brief Constructs an @ref ecu_event at compile-time.
 * 
 * @warning Validity of ID is not checked.
 * 
 * @param id_ ID to assign event.
 * @param size Number of bytes of derived event. Allows for easier 
 * handling. I.e. reading and writing events to queues. Supply
 * @ref ECU_EVENT_SIZE_UNUSED if unused.
 */
#define ECU_EVENT_CTOR(id_, size_)  \
    {                               \
        .id = id_,                  \
        .size = size_               \
    }

/**
 * @brief Verifies, at compile-time, that derived event 
 * correctly inherits base event class via C-style inheritance. 
 * Returns true if this condition is satisfied. False otherwise.
 * 
 * @param base_ Name of base class member within 
 * user's @p derived_ event type.
 * @param derived_ Derived event type to check.
 */
#define ECU_EVENT_IS_BASE_OF(base_, derived_) \
    ECU_IS_BASE_OF(base_, derived_)

/**
 * @brief Helper macro supplied to event constructors
 * if event's size field is unused.
 */
#define ECU_EVENT_SIZE_UNUSED \
    ((size_t)0)

/*------------------------------------------------------------*/
/*------------------------ EVENT IDS -------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Event IDs reserved for ECU library.
 * This scheme allows the library and users to uniquely define
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
    ECU_RESERVED_EVENT_ID = -1, /**< PRIVATE. FOR LIBRARY USE ONLY. */
    /*******************************/
    ECU_VALID_EVENT_ID_BEGIN = 0, /**< PRIVATE. FOR LIBRARY USE ONLY. Start of reserved IDs that the user can assign to events. */

    /*------------------------------------------------------------------*/
    /*--------------------- AVAILABLE EVENT IDS SECTION ----------------*/
    /*--------- FIRST MEMBER MUST EQUAL ECU_VALID_EVENT_ID_BEGIN -------*/
    /*------------------------------------------------------------------*/
    ECU_USER_EVENT_ID_BEGIN = ECU_VALID_EVENT_ID_BEGIN /**< 0. Start of user-defined event IDs. Will always be 0 for future compatibility. */
};

/**
 * @brief Event ID type. Used so the library can implicitly typecast 
 * between this value, @ref ecu_reserved_event_ids, and user-defined 
 * event ID enumerations.
 *
 * @warning This must be a signed integer type in order to
 * handle @ref ecu_reserved_event_ids enumerations less than 0. A
 * compilation error will occur if this is declared as an unsigned
 * type.
 */
typedef int32_t ecu_event_id_t;

/*------------------------------------------------------------*/
/*--------------------------- EVENT --------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Base event class. Custom event data is stored
 * by inheriting this class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct ecu_event
{
    /// @brief Identifies the event type.
    ecu_event_id_t id;

    /// @brief Number of bytes of derived event. Allows for easier 
    /// handling. I.e. reading and writing events to queues.
    size_t size;
};

/*------------------------------------------------------------*/
/*------------------ EVENT MEMBER FUNCTIONS ------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me
 * @brief Constructs an @ref ecu_event at run-time.
 *
 * @param me Event to construct. This should be a derived
 * event passed into this function via @ref ECU_EVENT_BASE_CAST().
 * @param id User-defined event ID. This must be greater than or
 * equal to @ref ECU_VALID_EVENT_ID_BEGIN.
 * @param size Number of bytes of derived event. Allows for easier 
 * handling. I.e. reading and writing events to queues. Supply
 * @ref ECU_EVENT_SIZE_UNUSED if unused.
 */
extern void ecu_event_ctor(struct ecu_event *me, ecu_event_id_t id, size_t size);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me constructed via @ref ECU_EVENT_CTOR() or @ref ecu_event_ctor().
 * @brief Returns the event's ID.
 * 
 * @param me Event to check. This should be a derived
 * event passed into this function via @ref ECU_EVENT_CONST_BASE_CAST().
 */
extern ecu_event_id_t ecu_event_id(const struct ecu_event *me);

/**
 * @pre @p me constructed via @ref ECU_EVENT_CTOR() or @ref ecu_event_ctor().
 * @brief Returns the size (number of bytes) of the derived event.
 * 
 * @param me Event to check. This should be a derived
 * event passed into this function via @ref ECU_EVENT_CONST_BASE_CAST().
 */
extern size_t ecu_event_size(const struct ecu_event *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* ECU_EVENT_H_ */
