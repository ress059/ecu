.. _event_h:

event.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
Lets users define their own events with custom data. These events 
can then be dispatched to ECU library functions.


Creating An Event 
=================================================
Follow these general steps to create an event:

    #. Define your event IDs. See `Event ID Section <event_id>`. 

    #. Create your custom event struct by inheriting :ecudoxygen:`ecu_event` base class
       and declaring additional data members. See `Custom Event Data <custom_event_data>`
       and `Event Inheritance Explained <event_inheritance_explained>` sections.

    #. Use :ecudoxygen:`ECU_EVENT_BASE_CAST() <ECU_EVENT_BASE_CAST>` macro to pass your 
       custom event to ECU library functions. See `Custom Event Data <custom_event_data>`
       and `Event Inheritance Explained <event_inheritance_explained>` sections.


.. _event_id:
Event ID 
"""""""""""""""""""""""""""""""""""""""""""""""""
Every event must be assigned a unique integer ID that is greater than 
or equal to :ecudoxygen:`ECU_VALID_EVENT_ID_BEGIN`. 

Users can define their own IDs, starting at value :ecudoxygen:`ECU_USER_EVENT_ID_BEGIN`. 
This allows ECU library and the end user to uniquely define their own event 
IDs without conflicts (library and user IDs won't be the same values). 
The recommended implementation is as follows:

.. code-block:: c

    #include "ecu/event.h"

    /* User-defined event IDs. */
    enum user_event_ids
    {
        BUTTON_PRESS_EVENT = ECU_USER_EVENT_ID_BEGIN,
        TIMEOUT_EVENT,
        ERROR_EVENT
    };

Some other notes:

    - Event IDs reserved for ECU library will always be negative.
    - Reserved event IDs the end user can use will start at :ecudoxygen:`ECU_VALID_EVENT_ID_BEGIN`.
      This is a placeholder for now.
    - :ecudoxygen:`ECU_USER_EVENT_ID_BEGIN` will always be 0 and marks the start 
      of user-defined event IDs. User-defined event IDs will always be greater than
      or equal to this value.


.. _custom_event_data:
Custom Event Data 
"""""""""""""""""""""""""""""""""""""""""""""""""
All ECU functions take in a pointer to :ecudoxygen:`ecu_event` base class, 
allowing you to dispatch any event that inherits :ecudoxygen:`ecu_event`.
For example, this is a function prototype in the ECU finite state machine 
module:

.. todo::

    Add ECU FSM link when it's created here.


.. code-block:: c 

    /* Takes in a base event class pointer. */
    extern void ecu_fsm_dispatch(struct ecu_fsm *me, const struct ecu_event *event);

Custom data is added to an event by using C-style inheritance. **The user's 
event must inherit :ecudoxygen:`ecu_event` base class by declaring it as 
its first member.** :ecudoxygen:`ECU_EVENT_BASE_CAST() <ECU_EVENT_BASE_CAST>`
macro must then be used to pass the custom event to ECU library functions:

.. code-block:: c 

    #include "ecu/event.h"
    #include "ecu/fsm.h"

    /* User-defined event IDs. */
    enum user_event_ids
    {
        BUTTON_PRESS_EVENT = ECU_USER_EVENT_ID_BEGIN,
        TIMEOUT_EVENT,
        ERROR_EVENT
    };

    /* User-defined event with custom data. */
    struct button_press_event
    {
        /* Inherit ecu event base class by declaring it as
        first member. MANDATORY. */
        struct ecu_event base;

        /* Custom data in your event. Derived class members. */
        uint8_t gpio_port;
        uint8_t gpio_pin;
    };

    /* User-defined event with custom data. */
    struct timeout_event 
    {
        /* Inherit ecu event base class by declaring it as
        first member. MANDATORY. */
        struct ecu_event base;

        /* Custom data in your event. Derived class members. */
        uint16_t timeout_ms;
    };

    /* Create a const button press event. */
    const struct button_press_event be = 
    {
        .base.id = BUTTON_PRESS_EVENT,
        .gpio_port = 1,
        .gpio_pin = 24
    };

    /* Create a non-const timeout event. */
    struct timeout_event te =
    {
        .base.id = TIMEOUT_EVENT,
        .timeout_ms = 100
    };

    /* Dispatch custom events to ECU library functions. */
    ecu_fsm_dispatch(&fsm, ECU_EVENT_BASE_CAST(&be));
    ecu_fsm_dispatch(&fsm, ECU_EVENT_BASE_CAST(&te));

If your event has no additional data, the base :ecudoxygen:`ecu_event` class 
can just be used directly:

.. code-block:: c

    #include "ecu/event.h"

    const struct ecu_event error_event =
    {
        .id = ERROR_EVENT
    };

    /* ECU_EVENT_BASE_CAST() macro optional in this case since we 
    are using ecu_event base class directly. */
    ecu_fsm_dispatch(&fsm, ECU_EVENT_BASE_CAST(&my_event));


.. _event_inheritance_explained:
Event Inheritance Explained
"""""""""""""""""""""""""""""""""""""""""""""""""
:ecudoxygen:`ECU_EVENT_BASE_CAST() <ECU_EVENT_BASE_CAST>` upcasts your derived
event class pointer back to an :ecu_doxygen:`ecu_event` base class pointer. 
**This operation is always safe as long as :ecudoxygen:`ecu_event` base class 
is the first member, which is how C-style inheritance works.**

.. figure:: /images/event/event_safe_cast.svg
  :width: 500
  :align: center
  
  Safe Cast

.. figure:: /images/event/event_safe_cast.svg
  :width: 500
  :align: center
  
  Unsafe Cast

Event inheritance can be verified at compile-time by using the 
:ecudoxygen:`ECU_EVENT_IS_BASE_OF(), ECU_EVENT_IS_BASE_OF` macro.
This returns true if a custom event properly inherits :ecudoxygen:`ecu_event`
base class. Otherwise it returns false:

.. code-block:: c

    #include "ecu/event.h"

    struct valid_event 
    {
        struct ecu_event base;
        int data;
    };

    struct invalid_event
    {
        int data;
        struct ecu_event base;
    };

    /* Passes since (struct valid_event) properly inherits (struct ecu_event) 
    by declaring it as its first member. */
    ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct valid_event) ), 
                        "Event must inherit ecu_event base class.");

    /* Compilation error since (struct invalid_event) does not properly 
    inherit (struct ecu_event). ecu_event is not declared as the first member. */
    ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct valid_event) ), 
                        "Event must inherit ecu_event base class.");

Verification would look like this if you have a multi-level inheritance tree:

.. code-block:: c

    #include "ecu/event.h"

    struct derived 
    {
        struct ecu_event base1;
        int data1;
    };

    struct derived_derived
    {
        struct derived base2;
        int data2;
    };

    /* You CANNOT do ECU_EVENT_IS_BASE_OF(base2, struct derived_derived). If base1
    is not the first member of (struct derived) this will not be caught. */
    ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base2.base1, struct derived_derived) ), 
                        "Event must inherit ecu_event base class.");


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/event_8h>
