.. _event_h:

event.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

Framework that can optionally be used to create and dispatch user-defined events to
ECU library functions (i.e. state machines in :ref:`fsm.h <fsm_h>` and :ref:`hsm.h <hsm_h>`).


Theory
=================================================

Event Representation
-------------------------------------------------
.. _event_event_representation:

Events created with this framework are represented by a common :ecudoxygen:`ecu_event`
base class:

.. figure:: /images/event/event_representation_class_diagram.svg
    :width: 300
    :align: center

    Event Class Diagram

Users create their own events by inheriting :ecudoxygen:`ecu_event` via C-style inheritance.
This is accomplished by declaring :ecudoxygen:`ecu_event` as the first member:

.. code-block:: c

    enum user_event_ids
    {
        USER_EVENT_ID_0 = ECU_USER_EVENT_ID_BEGIN,
        USER_EVENT_ID_1,
    }

    struct user_event0
    {
        struct ecu_event base; /* Inherit by declaring ecu_event as first member. */
        int data;
    };

    struct user_event1
    {
        struct ecu_event base; /* Inherit by declaring ecu_event as first member. */
        char data;
        int data2;
    };

**To create an instance of a derived event, the ecu_event base class constructor must also be called.**
This can be done at compile-time via :ecudoxygen:`ECU_EVENT_CTOR() <ECU_EVENT_CTOR>`:

.. code-block:: c

    static const struct user_event0 event = 
    {
        .base = ECU_EVENT_CTOR(USER_EVENT_ID_0, ECU_EVENT_SIZE_UNUSED),
        .data = 10
    };

Or at run-time via :ecudoxygen:`ecu_event_ctor() <ecu_event_ctor>`:

.. code-block:: c

    void user_event0_ctor(struct user_event0 *event)
    {
        ecu_event_ctor(ECU_EVENT_BASE_CAST(event), USER_EVENT_ID_0, ECU_EVENT_SIZE_UNUSED);
        me->data = 10;
    }

Notice in the snippet above that :ecudoxygen:`ECU_EVENT_BASE_CAST() <ECU_EVENT_BASE_CAST>`
is used to pass the derived event into the :ecudoxygen:`ecu_event` base class constructor.
:ecudoxygen:`ECU_EVENT_BASE_CAST() <ECU_EVENT_BASE_CAST>` and :ecudoxygen:`ECU_EVENT_CONST_BASE_CAST() <ECU_EVENT_CONST_BASE_CAST>`
should be used to supply derived events into the :ecudoxygen:`ecu_event` base class API.
These macros simply upcast back into the :ecudoxygen:`ecu_event` base class to create
a scheme that is **functionally** equivalent to C++ inheritance:

.. code-block:: c

    ECU_EVENT_BASE_CAST(event) /* ((struct ecu_event *)(event)) */
    ECU_EVENT_CONST_BASE_CAST(event) /* ((const struct ecu_event *)(event)) */

These casts are always safe **in C** as long as :ecudoxygen:`ecu_event` is the first member 
because the C standard mandates there is no padding before the first struct member:

.. figure:: /images/event/event_representation_valid_upcast.svg
    :width: 700
    :align: center

    Valid Upcast

.. warning::

    This behavior is **not** mandated in C++. Class memory layout is implementation-defined
    and there is no guarantee (struct ecu_event \*)&event == &event.base like in the example 
    above. Compilers often put virtual tables as the first class member, making these expressions
    **not** equal.

The figure above shows why :ecudoxygen:`ecu_event` must be the first member. Incorrect
inheritance causes misinterpretation of the event type, making the above casts unsafe. 
In this example, int data would be interpreted as a struct ecu_event type:

.. code-block:: c

    struct invalid_event
    {
        int data;
        struct ecu_event base;
    };

.. figure:: /images/event/event_representation_invalid_upcast.svg
    :width: 700
    :align: center

    Invalid Upcast

Valid inheritance can be asserted at compile-time via the 
:ecudoxygen:`ECU_EVENT_IS_BASE_OF() <ECU_EVENT_IS_BASE_OF>` macro:

.. code-block:: c

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

    /* Passes. */
    ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct valid_event)), "Event must inherit ecu_event." );

    /* Fails. Compilation error. */
    ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct invalid_event)), "Event must inherit ecu_event." );

Event ID
-------------------------------------------------
.. _event_event_id:

The derived event type can be determined from the :ecudoxygen:`ecu_event` base class
using an event ID. It is simply a unique integer value assigned to each event type.

The scheme presented in this module prevents an event ID used 
internally by ECU and a user's event ID from sharing the same value.
This is accomplished by starting user-defined event ID's at :ecudoxygen:`ECU_USER_EVENT_ID_BEGIN`:

.. code-block:: c

    enum user_event_ids
    {
        USER_EVENT_ID_0 = ECU_USER_EVENT_ID_BEGIN,
        USER_EVENT_ID_1,
    }

:ecudoxygen:`ECU_USER_EVENT_ID_BEGIN` is guaranteed to always be 0. IDs less 
than :ecudoxygen:`ECU_USER_EVENT_ID_BEGIN` are reserved for internal use 
by ECU library. Therefore reserved IDs are always negative. User-defined IDs
are always >= 0.

Event Size
-------------------------------------------------
.. _event_event_size:

The event's size can optionally be specified in the constructor and returned
via :ecudoxygen:`ecu_event_size() <ecu_event_size>`:

.. code-block:: c

    struct user_event0
    {
        struct ecu_event base;
        int data;
    };

    struct user_event1
    {
        struct ecu_event base;
        char data;
        int data2;
    };

    struct user_event0 event0;
    ecu_event_ctor(&event0, ID, sizeof(event0));
    ecu_event_size(&event); /* Returns sizeof(event0). */

    static const struct user_event1 event1 = 
    {
        .base = ECU_EVENT_CTOR(ID, sizeof(struct user_event1)),
        // ... other data
    };
    ecu_event_size(&event1); /* Returns sizeof(struct user_event1). */

This value should be the number of bytes of the **derived event**
and is meant to facilitate easier event handling (i.e. reading and 
writing different event types to the same queue). :ecudoxygen:`ECU_EVENT_SIZE_UNUSED`
can be supplied to the constructor if this parameter is unused.

Example
-------------------------------------------------
.. _event_example:

The following example uses this framework to process multiple event types
dispatched to a pseudocode state machine created using :ref:`ECU's finite state machine framework <fsm_h>`:

.. code-block:: c

    enum user_event_ids
    {
        USER_EVENT_ID_0 = ECU_USER_EVENT_ID_BEGIN,
        USER_EVENT_ID_1,
    }

    struct user_event0
    {
        struct ecu_event base;
        int data;
    };

    struct user_event1
    {
        struct ecu_event base;
        char data;
        int data2;
    };

    void state_handler(struct ecu_fsm *fsm, const void *event)
    {
        const struct ecu_event *base_event = event;

        switch (ecu_event_id(base_event))
        {
            case USER_EVENT_ID_0:
            {
                const struct user_event0 *derived_event = (const struct user_event0 *)base_event;
                process_integer_data(derived_event->data); 
                break;
            }

            case USER_EVENT_ID_1:
            {
                const struct user_event1 *derived_event = (const struct user_event1 *)base_event;
                process_char_data(derived_event->data);
                process_integer_data(derived_event->data2); 
                break;
            }

            default:
            {
                break;
            }
        }
    }

    int main()
    {
        struct user_event0 event0 = 
        {
            .base = ECU_EVENT_CTOR(USER_EVENT_ID_0, ECU_EVENT_SIZE_UNUSED),
            .data = 0
        };

        struct user_event1 event1 =
        {
            .base = ECU_EVENT_CTOR(USER_EVENT_ID_1, ECU_EVENT_SIZE_UNUSED),
            .data = 0,
            .data2 = 0
        };

        ecu_fsm_dispatch(fsm, &event0); /* state_handler(fsm, &event0); */
        ecu_fsm_dispatch(fsm, &event1); /* state_handler(fsm, &event1); */

        return 0;
    }


API 
=================================================
.. toctree::
    :maxdepth: 1

    event.h </doxygen/html/event_8h>

Macros
-------------------------------------------------

ECU_EVENT_BASE_CAST()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _event_ecu_event_base_cast:

Upcasts derived event back into the :ecudoxygen:`ecu_event` base class. 
Fully explained in :ref:`Event Representation Section <event_event_representation>`.

ECU_EVENT_CONST_BASE_CAST()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Same as :ref:`ECU_EVENT_BASE_CAST() <event_ecu_event_base_cast>` but performs
a const-qualified upcast. Fully explained in :ref:`Event Representation Section <event_event_representation>`.

ECU_EVENT_CTOR()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Constructs an :ecudoxygen:`ecu_event` at compile-time.
Fully explained in :ref:`Event Representation Section <event_event_representation>`.

ecu_event
-------------------------------------------------

Constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_event_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
Constructs an :ecudoxygen:`ecu_event` at run-time.
Fully explained in :ref:`Event Representation Section <event_event_representation>`.

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_event_id()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the event's ID that was assigned in its constructor. See
:ref:`Event ID<event_event_id>` and :ref:`Example <event_example>` sections
for more details.

ecu_event_size()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the size (number of bytes) of the derived event.
Fully explained in :ref:`Event Size Section <event_event_size>`.
