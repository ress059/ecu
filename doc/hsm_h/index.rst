.. _hsm_h:

hsm.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

Framework that creates and runs hierarchical state machines (HSMs). Applications use
this framework by containing an intrusive :ecudoxygen:`ecu_hsm` member. 

This framework models HSMs as closely as possible to UML state machines 
with the following known deviations:

#. Actions associated with state transitions are performed before 
   states are exited. UML mandates: exit->action->entry. This
   framework instead executes: action->exit->entry

#. `External state transitions <https://en.wikipedia.org/wiki/UML_state_machine#Local_versus_external_transitions>`_ are not supported.

Theory
=================================================
.. _hsm_theory:

Terminology
-------------------------------------------------
The following terminology is used to describe the hierarchical state machine (HSM):

.. figure:: /images/hsm/terminology.svg
    :width: 600
    :align: center

    Example

#. **Composite state**: state that contains other states. In the figure above, **ON_STATE** is
   a composite state.

#. **Leaf state**: state that does not contain any other states. In the figure above, **IDLE_STATE**,
   **RUNNING_STATE**, and **OFF_STATE** are leaf states.

#. **Least common ancestor (LCA)**: The deepest state in the hierarchy that has both A and B as
   descendants. For example:

    - LCA(IDLE_STATE, RUNNING_STATE) == ON_STATE
    - LCA(IDLE_STATE, ON_STATE) == ON_STATE
    - LCA(RUNNING_STATE, OFF_STATE) == TOP_STATE

#. **Initial transition**: mandatory state transition taken by a composite state(s) to 
   eventually get into a leaf state. The red arrows in the figure above are
   initial transitions.

#. **Source state**: original state the HSM is in before an event is dispatched to it.

#. **Target state**: new state the HSM should be in after a transition. For example if the
   HSM in the figure above is in the RUNNING_STATE and an OFF_EVENT is dispatched:

    - Source state == RUNNING_STATE
    - Target state == OFF_STATE

FSM vs HSM
-------------------------------------------------
.. _hsm_fsm_vs_hsm:

A hierarchical state machine (HSM) can have composite states to avoid code repetition.

.. figure:: /images/hsm/fsm_vs_hsm.svg
    :width: 600
    :align: center

    FSM vs HSM

The finite state machine (FSM) in the example above repeats code by handling
the OFF_EVENT in both the IDLE_STATE and RUNNING_STATE. The HSM avoids this
repetition by only handling the OFF_EVENT in the ON_STATE.

When an event is dispatched to an HSM, it is propagated up the state hierarchy
until it is handled. For example the execution order for an OFF_EVENT while in 
the IDLE_STATE is:

#. IDLE_STATE::handler(OFF_EVENT)
#. ON_STATE::handler(OFF_EVENT)

And similarly for an OFF_EVENT while in the RUNNING_STATE:

#. RUNNING_STATE::handler(OFF_EVENT)
#. ON_STATE::handler(OFF_EVENT)

Obviously no such propagaton exists for the FSM.

The nested model of the HSM also requires state transitions to be handled differently
from FSMs. Transitions in an FSM simply involves exiting the source state and entering
the target state. For example, the OFF_EVENT in the RUNNING_STATE causes the following
execution order in the FSM:

#. RUNNING_STATE::handler(OFF_EVENT)
#. RUNNING_STATE::exit()
#. OFF_STATE::entry()

However an HSM must exit and enter all necessary states in the hierarchy. The same
OFF_EVENT in the HSM's RUNNING_STATE causes:

#. RUNNING_STATE::handler(OFF_EVENT)
#. ON_STATE::handler(OFF_EVENT)
#. RUNNING_STATE::exit()
#. ON_STATE::exit()
#. OFF_STATE::enter()

HSM state transitions are explained in detail in the :ref:`State Transitions Section <hsm_state_transitions>`.
The basic algorithm involves:

#. Find LCA(Source state, Target State). I.e. LCA(RUNNING_STATE, OFF_STATE) == TOP_STATE.
#. Exit up from the source state until the LCA. I.e. exit up from RUNNING_STATE until TOP_STATE.
#. Enter from the LCA to the target state. I.e. enter from TOP_STATE to OFF_STATE.

State Representation
-------------------------------------------------
.. _hsm_state_representation:

States are represented by the :ecudoxygen:`ecu_hsm_state` struct. It contains
a set of handler functions that the user defines to describe the state's behavior.
This framework automatically executes the correct sequence of handler functions while the
state machine is running:

.. code-block:: c 

    struct ecu_hsm_state
    {
        void (*const entry)(struct ecu_hsm *me);
        void (*const exit)(struct ecu_hsm *me);
        void (*const initial)(struct ecu_hsm *me);
        bool (*const handler)(struct ecu_hsm *me, const void *event);
        const struct ecu_hsm_state *const parent;
    };

- **entry()** is an optional function that executes when the state is first entered.
  Set to :ecudoxygen:`ECU_HSM_STATE_ENTRY_UNUSED` if unused.
- **exit()** is an optional function that executes when the state is exited.
  Set to :ecudoxygen:`ECU_HSM_STATE_EXIT_UNUSED` if unused.
- **initial()** applies only for composite states. This is called when the HSM transitions
  into this composite state. The user must transition to a state lower in the hierarchy.
  This should only be set to :ecudoxygen:`ECU_HSM_STATE_INITIAL_UNUSED` for leaf states.
- **handler()** is a mandatory function that executes when the HSM is running in this state.
  Returns true if the dispatched event was handled. Returns false if the event should be 
  propagated up the state hierarchy.
- **parent** is this state's parent. All states must have a parent be another user-defined 
  state or :ecudoxygen:`ECU_HSM_TOP_STATE`.

The contents are const-qualified, forcing every state to be created at compile-time
via the :ecudoxygen:`ECU_HSM_STATE_CTOR() <ECU_HSM_STATE_CTOR>` macro:

.. code-block:: c

    /* Defined by user. */
    static void on_state_on_entry(struct ecu_hsm *hsm);
    static void on_state_on_exit(struct ecu_hsm *hsm);
    static void on_state_initial(struct ecu_hsm *hsm);
    static bool on_state_handler(struct ecu_hsm *hsm, const void *event);

    static const struct ecu_hsm_state ON_STATE = ECU_HSM_STATE_CTOR(
        &on_state_on_entry, &on_state_on_exit, &on_state_initial, &on_state_handler, &ECU_HSM_TOP_STATE
    );

Representing states as objects allows them to be shared between 
multiple instances of the same HSM. No additional memory or overhead is required:

.. figure:: /images/hsm/state_representation_state_reuse.svg
    :width: 400
    :align: center

    State Reuse

State Machine Representation
-------------------------------------------------
.. _hsm_state_machine_representation:

HSMs are represented by the :ecudoxygen:`ecu_hsm` struct. 
Applications use this framework by containing :ecudoxygen:`ecu_hsm` as
an intrusive member:

.. code-block:: c

    struct app_hsm
    {
        int app_data1;
        struct ecu_hsm hsm_member;
        int app_data2;
    };

This framework has no knowledge of the application's state machine type so it 
must only use :ecudoxygen:`ecu_hsm` to remain portable. The :ecudoxygen:`ecu_hsm` 
member acts as a common interface between the two mediums. Thus each state
handler must take in an :ecudoxygen:`ecu_hsm` pointer:

.. code-block:: c 

    /* Defined by user. */
    static void on_state_on_entry(struct ecu_hsm *hsm);
    static void on_state_on_exit(struct ecu_hsm *hsm);
    static void on_state_initial(struct ecu_hsm *hsm);
    static bool on_state_handler(struct ecu_hsm *hsm, const void *event);

The application's state machine type can be retrieved within each handler's definition
via the :ecudoxygen:`ECU_HSM_GET_CONTEXT() <ECU_HSM_GET_CONTEXT>` macro:

.. code-block:: c

    static void on_state_on_entry(struct ecu_hsm *hsm)
    {
        struct app_hsm *me = ECU_HSM_GET_CONTEXT(hsm, struct app_hsm, hsm_member);
        me->app_data1 = 10;
        me->app_data2 = 10;
    }

    static void on_state_on_exit(struct ecu_hsm *hsm)
    {
        struct app_hsm *me = ECU_HSM_GET_CONTEXT(hsm, struct app_hsm, hsm_member);
        me->app_data1 = 0;
        me->app_data2 = 0;
    }

    // etc...

This allows the framework to interact with the application through a common interface
(the :ecudoxygen:`ecu_hsm` struct), without inheritance. The macro takes in
three parameters:

    #. ``ecu_hsm_ptr_`` = Pointer to intrusive ecu_hsm member. In this case, ``hsm``.
    #. ``type_`` = User's HSM type. In this case, ``struct app_hsm``.
    #. ``member_`` = Name of ecu_hsm member within the user's type. In this case, ``hsm_member``.

.. figure:: /images/hsm/ecu_hsm_get_context.svg
    :width: 450
    :align: center
  
    ECU_HSM_GET_CONTEXT()

Under the hood, this macro does pointer arithmetic to perform the conversion.
The details of this operation are fully explained in 
:ref:`ECU_CONTAINER_OF() <utils_container_of>`.

Event-Driven Paradigm
-------------------------------------------------
The application can only interact with HSMs created with this framework
by dispatching **events** via :ecudoxygen:`ecu_hsm_dispatch() <ecu_hsm_dispatch>`.

Events are objects that describe what happened and contain any relevant data. This 
pattern naturally decouples the state machine from the application and is fully 
explained in the :ref:`Event-Driven Paradigm Section of the FSM Framework <fsm_event_driven_paradigm>`.
The exact same principles apply to HSMs.

Event Propagation
-------------------------------------------------
When an event is dispatched to an HSM, it is processed in the current state.
If the event is unhanded, it is propagated up the state hierarchy. Take this
example HSM (initial transitions, etc not shown for conciseness):

.. figure:: /images/hsm/event_propagation.svg
    :width: 600
    :align: center
  
    Event Propagation

If the HSM is in S11 and EVENT_A is dispatched, it is propgated up the state hierarchy
and eventually handled in state S. The full handling order is:

#. S11:handler(EVENT_A)
#. S1::handler(EVENT_A)
#. S::handler(EVENT_A)

**A state handles the event (stops propagating it) by returning true
in its handler definition. A state propgates the event up the hierarchy by
returning false in its handler definition.** In the example HSM shown above
S11, S1, and S handler definitions would look like:

.. code-block:: c

    static bool S11_handler(struct ecu_hsm *hsm, const void *event)
    {
        return false; /* Propagate all events up state hierarchy. */
    }

    static bool S1_handler(struct ecu_hsm *hsm, const void *event)
    {
        return false; /* Propagate all events up state hierarchy. */
    }

    static bool S_handler(struct ecu_hsm *hsm, const void *event)
    {
        /* Handle EVENT_A. All other events propagated up the state hierarchy. */
        if (event == EVENT_A)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

**Event propagation completes when the top state (root) is reached. This framework
requires the top state to be :ecudoxygen:`ECU_HSM_TOP_STATE`. It is a dummy default state
that handles all events by always returning true in its handler definition.**

.. warning::

    All HSMs are **required** to have their top states be :ecudoxygen:`ECU_HSM_TOP_STATE`.

Note how this top state is also required to guarantee a cohesive state hierarchy.
I.e. A tree must always have a root.

State Transitions
-------------------------------------------------
.. _hsm_state_transitions:

Within each state's initial or handler function, the application can perform a 
state transition by calling :ecudoxygen:`ecu_hsm_change_state() <ecu_hsm_change_state>`.
For example:

.. code-block:: c

    static bool state_handler(struct ecu_hsm *hsm, const void *event)
    {
        if (event == causes transition)
        {
            ecu_hsm_change_state(hsm, &NEW_STATE);
        }

        return true;
    }

This framework automatically executes the proper exit and entry paths. The complexity
of this operation is fully encapsulated in :ecudoxygen:`ecu_hsm_change_state() <ecu_hsm_change_state>`.

.. warning::

    The following rules **MUST** be satisfied when performing a state transition:

    #. A state transition can **never** occur in a state's entry and exit handlers.
    #. All composite states **must** define an initial transition down the state hierarchy.
       This is because the HSM must be in a leaf state once all transitions are complete.
    #. A transition **cannot** be taken to :ecudoxygen:`ECU_HSM_TOP_STATE`.

The following example HSM shows the state transitions executed by this framework when
different events are dispatched:

Source State == S1
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. figure:: /images/hsm/state_transitions.svg
    :width: 800
    :align: center
  
    State Transitions

#. EVENT_A:
    - ecu_hsm_dispatch(hsm, EVENT_A)
    - S1::handler(EVENT_A)
    - S1::exit()
    - S12::entry()

#. EVENT_B:
    - ecu_hsm_dispatch(hsm, EVENT_B)
    - S1::handler(EVENT_B)
    - S::handler(EVENT_B)
    - ECU_HSM_TOP_STATE::handler(EVENT_B)

#. EVENT_C:
    - ecu_hsm_dispatch(hsm, EVENT_C)
    - S1::handler(EVENT_C)
    - S::handler(EVENT_C)
    - S1::exit()
    - S1::entry()

#. EVENT_D:
    - ecu_hsm_dispatch(hsm, EVENT_D)
    - S1::handler(EVENT_D)
    - S1::exit()
    - S::initial()
    - S1::entry()

#. EVENT_E:
    - ecu_hsm_dispatch(hsm, EVENT_E)
    - S1::handler(EVENT_E)
    - S::handler(EVENT_E)
    - S1::exit()
    - S::exit()
    - S::entry()
    - S::initial()
    - S1::entry()

#. EVENT_F:
    - ecu_hsm_dispatch(hsm, EVENT_F)
    - S1::handler(EVENT_F)
    - S::handler(EVENT_F)
    - ECU_HSM_TOP_STATE::handler(EVENT_F)

#. EVENT_G:
    - ecu_hsm_dispatch(hsm, EVENT_G)
    - S1::handler(EVENT_G)
    - S::handler(EVENT_G)
    - S1::exit()
    - S::exit()
    - S2::entry()

Source State == S12
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. figure:: /images/hsm/state_transitions.svg
    :width: 800
    :align: center
  
    State Transitions

#. EVENT_A:
    - ecu_hsm_dispatch(hsm, EVENT_A)
    - S12::handler(EVENT_A)
    - S::handler(EVENT_A)
    - ECU_HSM_TOP_STATE::handler(EVENT_A)

#. EVENT_B:
    - ecu_hsm_dispatch(hsm, EVENT_B)
    - S12::handler(EVENT_B)
    - S12::exit()
    - S1::entry()

#. EVENT_C:
    - ecu_hsm_dispatch(hsm, EVENT_C)
    - S12::handler(EVENT_C)
    - S::handler(EVENT_C)
    - S12::exit()
    - S1::entry()

#. EVENT_D:
    - ecu_hsm_dispatch(hsm, EVENT_D)
    - S12::handler(EVENT_D)
    - S::handler(EVENT_D)
    - ECU_HSM_TOP_STATE::handler(EVENT_D)

#. EVENT_E:
    - ecu_hsm_dispatch(hsm, EVENT_E)
    - S12::handler(EVENT_E)
    - S::handler(EVENT_E)
    - S12::exit()
    - S::exit()
    - S::entry()
    - S::initial()
    - S1::entry()

#. EVENT_F:
    - ecu_hsm_dispatch(hsm, EVENT_F)
    - S12::handler(EVENT_F)
    - S::handler(EVENT_F)
    - ECU_HSM_TOP_STATE::handler(EVENT_F)

#. EVENT_G:
    - ecu_hsm_dispatch(hsm, EVENT_G)
    - S12::handler(EVENT_G)
    - S::handler(EVENT_G)
    - S12::exit()
    - S::exit()
    - S2::entry()


Source State == S2
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. figure:: /images/hsm/state_transitions.svg
    :width: 800
    :align: center
  
    State Transitions

#. EVENT_F:
    - ecu_hsm_dispatch(hsm, EVENT_F)
    - S2::handler(EVENT_F)
    - S2::exit()
    - S::entry()
    - S::initial()
    - S1::entry()

#. All other events (EVENT_A, EVENT_B, EVENT_C, EVENT_D, EVENT_E, EVENT_G):
    - ecu_hsm_dispatch(hsm, EVENT_x)
    - S2::handler(EVENT_x)
    - ECU_HSM_TOP_STATE::handler(EVENT_x)

Example
-------------------------------------------------
This example aims to demonstrate the points outlined in the :ref:`Event-Driven Paradigm Section 
of the FSM Framework that also applies to HSMs <fsm_event_driven_paradigm>` by creating 
the following keyboard state machine:

.. figure:: /images/hsm/example.svg
    :width: 800
    :align: center
  
    Example

The keyboard stores which keys have been pressed in a standard USB HID report.
It is sent to the computer when the application requests to send it, or when the 
report becomes full. Caps lock logic is also handled.

The complexity of these operations are simplified by modeling the keyboard's
behavior as an HSM, whose implementation is fully encapsulated within a reusable
keyboard object defined in ``keyboard.h`` and ``keyboard.c``. Therefore the application 
does not have to worry about any of the complexities explained above. It simply
interacts with the keyboard by blindly dispatching events to it.

This module (keyboard object) is then ported to two separate applications. The first on a microcontroller
in ``main.c``, where a keyboard object is created and used. The second on a computer
in ``tests.c``, where the keyboard module is tested.

keyboard.h
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    /*-------------------------- keyboard.h --------------------------*/
    struct keyboard
    {
        struct ecu_hsm hsm;
        uint8_t modifier; /* HID report byte 0. Bitmask of modifier (shift, etc) keys pressed. Unused for this example. */
        uint8_t data[6]; /* HID report bytes 2 to 7. Stores keys that have been pressed. */
        size_t index; /* Current index in data[]. */
        void (*send)(const uint8_t *report, size_t count, void *obj); /* Dependency injection. Send HID report to computer. */
        void *obj; /* Optional object passed to user-defined callbacks. */
    };

    enum keyboard_event_id
    {
        KEYBOARD_CONNECTED_EVENT, /* Keyboard disconnected from computer. */
        KEYBOARD_DISCONNECTED_EVENT, /* Keyboard reconnected to computer. */
        KEYBOARD_KEYPRESS_EVENT, /* Key pressed. */
        KEYBOARD_SEND_EVENT /* Application requests to send HID report. */
    };

    struct keyboard_event
    {
        enum keyboard_event_id id;
        uint8_t keycode;
    };

    extern void keyboard_ctor(struct keyboard *me,
                              void (*send)(const uint8_t *, size_t, void *),
                              void *obj);

    extern void keyboard_start(struct keyboard *me);

    extern void keyboard_dispatch(struct keyboard *me, const void *event);

keyboard.c
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    /*-------------------------- keyboard.c --------------------------*/
    #include "keyboard.h"

    #define KEYCODE_CAPS_LOCK 0x39U

    static void keycode_report_clear(struct keyboard *me); /* Resets HID report buffer but does not send it. */
    static void keycode_report_send(struct keyboard *me); /* Sends current HID report to computer then resets it. */

    static void connected_state_on_entry(struct ecu_hsm *hsm);
    static void connected_state_initial(struct ecu_hsm *hsm);
    static bool connected_state_handler(struct ecu_hsm *hsm, const void *event);
    static bool default_state_handler(struct ecu_hsm *hsm, const void *event);
    static bool caps_lock_state_handler(struct ecu_hsm *hsm, const void *event);
    static bool disconnected_state_handler(struct ecu_hsm *hsm, const void *event);

    static const struct ecu_hsm_state CONNECTED_STATE = ECU_HSM_STATE_CTOR(
        &connected_state_on_entry, ECU_HSM_STATE_EXIT_UNUSED, &connected_state_initial, &connected_state_handler, &ECU_HSM_TOP_STATE
    );

    static const struct ecu_hsm_state DEFAULT_STATE = ECU_HSM_STATE_CTOR(
        ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &default_state_handler, &CONNECTED_STATE
    );

    static const struct ecu_hsm_state CAPS_LOCK_STATE = ECU_HSM_STATE_CTOR(
        ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &caps_lock_state_handler, &CONNECTED_STATE
    );

    static const struct ecu_hsm_state DISCONNECTED_STATE = ECU_HSM_STATE_CTOR(
        ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &disconnected_state_handler, &ECU_HSM_TOP_STATE
    );

    static void keycode_report_clear(struct keyboard *me)
    {
        me->modifier = 0;
        memset(&me->data[0], 0, sizeof(me->data));
        me->index = 0;
    }

    static void keycode_report_send(struct keyboard *me)
    {
        uint8_t report[8];

        report[0] = me->modifier;
        report[1] = 0; /* Reserved. */
        memcpy(&report[2], &me->data[0], sizeof(me->data));
        (*me->send)(&report[0], sizeof(report), me->obj);

        keycode_report_clear(me);
    }

    static void connected_state_on_entry(struct ecu_hsm *hsm)
    {
        struct keyboard *me = ECU_HSM_GET_CONTEXT(hsm, struct keyboard, hsm);
        keycode_report_clear(me);
    }

    static void connected_state_initial(struct ecu_hsm *hsm)
    {
        ecu_hsm_change_state(hsm, &DEFAULT_STATE);
    }

    static bool connected_state_handler(struct ecu_hsm *hsm, const void *event)
    {
        bool status = true;
        struct keyboard *me = ECU_HSM_GET_CONTEXT(hsm, struct keyboard, hsm);
        const struct keyboard_event *e = (const struct keyboard_event *)event;

        switch (e->id)
        {
            case KEYBOARD_DISCONNECTED_EVENT:
            {
                ecu_hsm_change_state(hsm, &DISCONNECTED_STATE);
                break;
            }

            default:
            {
                /* Propagate event up hierarchy. */
                status = false;
                break;
            }
        }

        return status;
    }

    static bool default_state_handler(struct ecu_hsm *hsm, const void *event)
    {
        bool status = true;
        struct keyboard *me = ECU_HSM_GET_CONTEXT(hsm, struct keyboard, hsm);
        const struct keyboard_event *e = (const struct keyboard_event *)event;

        switch (e->id)
        {
            case KEYBOARD_KEYPRESS_EVENT:
            {
                ECU_ASSERT( (me->index < sizeof(me->data)) );
                uint8_t keycode = e->keycode;

                if (keycode == KEYCODE_CAPS_LOCK)
                {
                    keycode_report_send(me); /* Send report that had keys without caps lock modifier. */
                    me->data[0] = KEYCODE_CAPS_LOCK; /* Set caps lock modifier. */
                    me->index = 1;
                    keycode_report_send(me); /* Send report that says caps lock now pressed. */
                    ecu_hsm_change_state(hsm, &CAPS_LOCK_STATE);
                }
                else if (me->index == sizeof(me->data) - 1) 
                {
                    /* If added keycode causes report to be full, send report. */
                    me->data[me->index] = keycode;
                    keycode_report_send(me);
                }
                else
                {
                    /* Otherwise just store the keycode. */
                    me->data[me->index] = keycode;
                    me->index++;
                }
                break;
            }

            case KEYBOARD_SEND_EVENT:
            {
                keycode_report_send(me);
                break;
            }

            default:
            {
                /* Propagate event up hierarchy. */
                status = false;
                break;
            }
        }

        return status;
    }

    static bool caps_lock_state_handler(struct ecu_hsm *hsm, const void *event)
    {
        bool status = true;
        struct keyboard *me = ECU_HSM_GET_CONTEXT(hsm, struct keyboard, hsm);
        const struct keyboard_event *e = (const struct keyboard_event *)event;

        switch (e->id)
        {
            case KEYBOARD_KEYPRESS_EVENT:
            {
                ECU_ASSERT( (me->index < sizeof(me->data)) );
                uint8_t keycode = e->keycode;

                if (keycode == KEYCODE_CAPS_LOCK)
                {
                    keycode_report_send(me); /* Send report that had keys with caps lock modifier. */
                    keycode_report_send(me); /* Send report that says caps lock no longer pressed. */
                    ecu_hsm_change_state(hsm, &DEFAULT_STATE);
                }
                else if (me->index == sizeof(me->data) - 1) 
                {
                    /* If added keycode causes report to be full, send report. Afterwards reinitialize the caps lock modifier. */
                    me->data[me->index] = keycode;
                    keycode_report_send(me);
                    me->data[0] = KEYCODE_CAPS_LOCK;
                    me->index = 1;
                }
                else
                {
                    /* Otherwise just store the keycode. */
                    me->data[me->index] = keycode;
                    me->index++;
                }
                break;
            }

            case KEYBOARD_SEND_EVENT:
            {
                /* Send report then reinitialize caps lock modifier. */
                keycode_report_send(me);
                me->data[0] = KEYCODE_CAPS_LOCK;
                me->index = 1;
                break;
            }

            default:
            {
                /* Propagate event up hierarchy. */
                status = false;
                break;
            }
        }

        return status;
    }

    static bool disconnected_state_handler(struct ecu_hsm *hsm, const void *event)
    {
        bool status = true;
        const struct keyboard_event *e = (const struct keyboard_event *)event;

        switch (e->id)
        {
            case KEYBOARD_CONNECTED_EVENT:
            {
                ecu_hsm_change_state(hsm, &CONNECTED_STATE);
                break;
            }

            default:
            {
                /* Propagate event up hierarchy. */
                status = false;
                break;
            }
        }

        return status;
    }

    void keyboard_ctor(struct keyboard *me,
                       void (*send)(const uint8_t *, size_t, void *),
                       void *obj)
    {
        ecu_hsm_ctor(&me->hsm, &CONNECTED_STATE, 2);
        
        me->send = send;
        me->obj = obj;
        keycode_report_clear(me);
    }

    void keyboard_start(struct keyboard *me)
    {
        ecu_hsm_start(&me->hsm);
    }

    void keyboard_dispatch(struct keyboard *me, const void *event)
    {
        ecu_hsm_dispatch(&me->hsm, event);
    }

main.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    /*-------------------------- main.c --------------------------*/
    #include "keyboard.h" /* Reusable keyboard object we created. */

    static void usb_send(const uint8_t *report, size_t count, void *obj)
    {
        (void)obj;
        USBHardwareSend(report, count);
    }

    static struct keyboard kb;
    static struct keyboard_event event;

    int main(void)
    {
        keyboard_ctor(&kb, &usb_send, NULL);

        while (1)
        {
            /* Blindly dispatch events. */
            uint8_t keycode = get_key_pressed();
            event.id = KEYBOARD_KEYPRESS_EVENT;
            event.keycode = keycode;
            keyboard_dispatch(&kb, &event);

            if (connected)
            {
                event.id = KEYBOARD_CONNECTED_EVENT;
                keyboard_dispatch(&kb, &event);
            }
            else if (disconnected)
            {
                event.id = KEYBOARD_DISCONNECTED_EVENT;
                keyboard_dispatch(&kb, &event);
            }

            if (greater than 10ms elapsed)
            {
                event.id = KEYBOARD_SEND_EVENT;
                keyboard_dispatch(&kb, &event);
            }
        }

        return 0;
    }

tests.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    /*-------------------------- tests.c -------------------------*/
    #include "keyboard.h" /* Reusable keyboard module under test. */

    static void send(const uint8_t *report, size_t count, void *obj)
    {
        (void)obj;

        for (size_t i = 0; i < count; i++)
        {
            printf("%02x ", report[i]);
        }
        printf("\n");
    }

    TEST()
    {
        struct keyboard kb;
        struct keyboard_event event;
        keyboard_ctor(&kb, &send, NULL);

        event.id = KEYBOARD_KEYPRESS_EVENT;
        event.keycode = A;
        keyboard_dispatch(&kb, &event);
        keyboard_dispatch(&kb, &event);
        event.keybode = B;
        keyboard_dispatch(&kb, &event);

        event.id = KEYBOARD_SEND_EVENT;
        keyboard_dispatch(&kb, &event); /* "aab" */
    }

    TEST()
    {
        struct keyboard kb;
        struct keyboard_event event;
        keyboard_ctor(&kb, &send, NULL);

        event.id = KEYBOARD_KEYPRESS_EVENT;
        event.keycode = CAPS_LOCK;
        keyboard_dispatch(&kb, &event);

        event.keycode = A;
        keyboard_dispatch(&kb, &event);
        keyboard_dispatch(&kb, &event);
        event.keybode = B;
        keyboard_dispatch(&kb, &event);

        event.id = KEYBOARD_SEND_EVENT;
        keyboard_dispatch(&kb, &event); /* "AAB" */
    }

    TEST()
    {
        struct keyboard kb;
        struct keyboard_event event;
        keyboard_ctor(&kb, &send, NULL);

        event.id = KEYBOARD_DISCONNECTED_EVENT;
        keyboard_dispatch(&kb, &event);

        /* Nothing printed. */
        event.id = KEYBOARD_KEYPRESS_EVENT;
        event.keycode = A;
        keyboard_dispatch(&kb, &event);
        keyboard_dispatch(&kb, &event);
        keyboard_dispatch(&kb, &event);
        event.id = KEYBOARD_SEND_EVENT;
        keyboard_dispatch(&kb, &event);
        keyboard_dispatch(&kb, &event);
    }

API 
=================================================
.. toctree::
    :maxdepth: 1

    hsm.h </doxygen/html/hsm_8h>

Macros
-------------------------------------------------

ECU_HSM_GET_CONTEXT()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Converts intrusive :ecudoxygen:`ecu_hsm` member back into the user's state machine type.
This should be used inside a state handler's definition. See
:ref:`State Machine Representation Section <hsm_state_machine_representation>`
for more details:

.. code-block:: c

    struct app_hsm
    {
        int app_data1;
        struct ecu_hsm hsm_member;
        int app_data2;
    };

    static void running_state_on_entry(struct ecu_hsm *hsm)
    {
        struct app_hsm *me = ECU_HSM_GET_CONTEXT(hsm, struct app_hsm, hsm_member);
        me->app_data1 = 0;
        me->app_data2 = 0;
    }

ECU_HSM_STATE_CTOR()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Creates an :ecudoxygen:`ecu_hsm_state` at compile-time.
See :ref:`State Representation Section <hsm_state_representation>`
for more details:

.. code-block:: c

    /* Defined by user. */
    static void running_state_on_entry(struct ecu_hsm *hsm);
    static void running_state_on_exit(struct ecu_hsm *hsm);
    static void running_state_initial(struct ecu_hsm *hsm);
    static bool running_state_handler(struct ecu_hsm *hsm, const void *event);

    static const struct ecu_hsm_state RUNNING_STATE = ECU_HSM_STATE_CTOR(
        &running_state_on_entry, &running_state_on_exit, &running_state_initial, &running_state_handler, &ECU_HSM_TOP_STATE
    );

ecu_hsm
-------------------------------------------------

Constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_hsm_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _hsm_ecu_hsm_ctor:

Constructor. Initializes the :ecudoxygen:`ecu_hsm` data structure for use.
The state machine's level starts at 1 since all HSMs are **required** to use
:ecudoxygen:`ECU_HSM_TOP_STATE` as the default top state. For example:

.. figure:: /images/hsm/ecu_hsm_ctor.svg
    :width: 350
    :align: center

    ecu_hsm_ctor() Height

.. warning:: 

    Must be called once on startup before the state machine is used. User is 
    also responsible for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct ecu_hsm hsm;  /* User must allocate memory before constructor. */
    ecu_hsm_start(&hsm); /* ILLEGAL. Must construct before using. */
    ecu_hsm_ctor(&hsm, &INIT_STATE, 2);
    ecu_hsm_start(&hsm); /* Ok. */

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_hsm_change_state()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _hsm_ecu_hsm_change_state:

Transitions the HSM into a new state. See
:ref:`State Transitions Section <hsm_state_transitions>` for
more details.

.. warning:: 
    
    This function can only be called within a state's main handler 
    or initial handler. If a transition is signalled in a state's main 
    handler, it must handle the event by returning true. The HSM must 
    be in a leaf state after all transitions are completed.

.. code-block:: c

    static bool running_state_handler(struct ecu_hsm *hsm, const void *event)
    {
        if (event == STOP_EVENT)
        {
            ecu_hsm_change_state(hsm, &STOPPED_STATE);
            return true; /* Handle event! */
        }
    }

ecu_hsm_dispatch()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Relays event to the HSM where it is processed by
the current state's handler function. The event is propagated 
up the state hierarchy until it is handled. All state transitions
signalled via :ref:`ecu_hsm_change_state() <hsm_ecu_hsm_change_state>` 
are also managed in this function. The :ref:`Theory Section <hsm_theory>`
fully explains how the HSM runs.

.. warning:: 

    This function must run to completion. The HSM 
    must be in a leaf state after this function completes.

.. code-block:: c 

    enum event_id
    {
        STOP_EVENT_ID,
        START_EVENT_ID,
        RESET_EVENT_ID
    };

    struct event
    {
        enum event_id id;
        int data;
        int more_data;
    };

    struct event stop_event = {STOP_EVENT_ID, 0, 0};
    ecu_hsm_dispatch(&hsm, &stop_event);

ecu_hsm_start()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Starts the HSM by entering from :ecudoxygen:`ECU_HSM_TOP_STATE`
to the target state supplied to :ref:`ecu_hsm_ctor() <hsm_ecu_hsm_ctor>`. 
If the target is a composite state, initial handlers are ran to
fully transition down the state hierarchy. The resulting execution 
order for the following example is:

- ecu_hsm_start(&hsm)
- S::entry()
- S::initial()
- S1::entry()
- S1::initial()
- S2::entry()
     
.. figure:: /images/hsm/ecu_hsm_start.svg
    :width: 350
    :align: center

    ecu_hsm_start()

.. code-block:: c

    ecu_hsm_ctor(&hsm, &S);
    ecu_hsm_start(&hsm);

The resulting execution order for this example is:

- ecu_hsm_start(&hsm)
- S::entry()
- S1::entry()
- S1::initial()
- S2::entry()
     
.. figure:: /images/hsm/ecu_hsm_start2.svg
    :width: 350
    :align: center

    ecu_hsm_start()

.. code-block:: c

    ecu_hsm_ctor(&hsm, &S1);
    ecu_hsm_start(&hsm);

.. warning:: 

    This function should only be called once on startup and 
    must run to completion. The HSM must be in a leaf state after 
    this function completes.
