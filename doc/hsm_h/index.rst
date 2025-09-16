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

Event-driven **hierarchical state machine (HSM)** framework that applications inherit from to 
create their own state machines. The complexity of running the HSM and handling state
transitions are fully encapsulated within this API.


Theory
=================================================
This section showcases proper use of the framework and explains its design rationale.
It is recommended to read this section before using the framework.


Overview
-------------------------------------------------
.. _hsm_overview:

A hierarchical state machine (HSM) can have nested states to avoid code repetition.

.. figure:: /images/hsm/hsm_vs_fsm.svg
  :width: 600
  :align: center

  HSM vs FSM

In the example above ON_STATE is the **parent** of IDLE_STATE and RUNNING_STATE. 
A specific event causes a transition from the ON_STATE to the OFF_STATE. 

In a finite state machine (FSM), this event must be processed by each state
resulting in code repeition. An HSM eliminates this repetition by allowing child
states (IDLE_STATE and RUNNING_STATE) to offload processing of this event to 
its parent (the ON_STATE).

The flat vs hierarchical model encompasses the differences between this HSM module and 
the :ref:`Finite State Machine Framework <fsm_h>`. State representations and 
transitions must be handled differently to facilitate the hierarchical model, which is
explained in later sections. Apart from these differences, both the HSM and FSM 
frameworks are architected in a similar fashion.


State Machine Representation
-------------------------------------------------
.. _hsm_state_machine_representation:

An HSM is represented by an :ecudoxygen:`ecu_hsm` base class. Application-specific
state machines are defined by **inheriting** this base class via C-style inheritance.
This is accomplished by declaring an :ecudoxygen:`ecu_hsm` object as the **first struct member**.

.. code-block:: c 

    struct app_hsm
    {
        /* Inherit by declaring ecu_hsm object as first member. */
        struct ecu_hsm hsm;

        /* Additional members unique to application-specific hsm. */
        uint8_t counter1;
        uint8_t counter2;
    };

Inheritance provides a common interface between this module and the application's type.
This API has no knowledge of the application, so it is a base class that takes in
a base class type (struct ecu_hsm \*). Derived types are passed into the API by upcasting:

.. code-block:: c 

    struct app_hsm APP_HSM;
    ecu_hsm_ctor((struct ecu_hsm *)&APP_HSM, &INIT_STATE, &TOP_STATE, 3);
    ecu_hsm_start((struct ecu_hsm *)&APP_HSM);
    // ....

For better encapsulation, this cast should be wrapped within :ecudoxygen:`!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!() <!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!>`:

.. code-block:: c 

    struct app_hsm APP_HSM;
    ecu_hsm_ctor(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&APP_HSM), &INIT_STATE, &TOP_STATE, 3);
    ecu_hsm_start(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&APP_HSM));
    // ....

Inheritance in C is accomplished this way. The example above is **functionally** equivalent
to the following C++ code:

.. code-block:: cpp

    class ecu_hsm
    {
        void ecu_hsm_start()
        {
            // ...
        }

        // ...rest of API functions.
    };

    class app_hsm : public ecu_hsm
    {
    public:
        uint8_t counter1;
        uint8_t counter2;
    };

    app_hsm APP_HSM;
    APP_HSM.ecu_hsm_start();

**Upcasting in this manner is always legal and safe as long as** :ecudoxygen:`ecu_hsm` **is the first member**.
The C standard mandates there is no struct padding before the first member so a derived pointer can
always be represented as a base pointer. Inheritance is only unsafe if :ecudoxygen:`ecu_hsm` is not 
the first member due to misaligned access:

.. figure:: /images/hsm/inheritance.svg
  :width: 800
  :align: center

  Inheritance

:ecudoxygen:`TODO!!WAS_ECU_HSM_IS_BASE_OF_BEFORE!!() <TODO!!WAS_ECU_HSM_IS_BASE_OF_BEFORE!!>` macro returns true if :ecudoxygen:`ecu_hsm` is 
correctly inherited. Otherwise it returns false. These values are evaluated at compile-time so the 
condition can be statically asserted:

.. code-block:: c 

    struct correct_hsm
    {
        struct ecu_hsm hsm;
        uint8_t counter1;
        uint8_t counter2;
    };

    struct incorrect_hsm
    {
        uint8_t counter1;
        struct ecu_hsm hsm;
        uint8_t counter2;
    };

    /* Passes. */
    ECU_STATIC_ASSERT( (TODO!!WAS_ECU_HSM_IS_BASE_OF_BEFORE!!(hsm, struct correct_hsm)), "ecu_hsm must be first member.");

    /* Compilation error. */
    ECU_STATIC_ASSERT( (TODO!!WAS_ECU_HSM_IS_BASE_OF_BEFORE!!(hsm, struct incorrect_hsm)), "ecu_hsm must be first member.");


State Representation
-------------------------------------------------
.. _hsm_state_representation:

States are represented as an :ecudoxygen:`ecu_hsm_state` object. **The 
contents of the struct are const-qualified, meaning every state must be 
initialized at compile-time.**

.. code-block:: c 

    struct ecu_hsm_state
    {
        void (*const entry)(struct ecu_hsm *me);
        void (*const exit)(struct ecu_hsm *me);
        bool (*const handler)(struct ecu_hsm *me, const void *event);
        const struct ecu_hsm_state *const parent;
    };

- **entry()** is an optional function that executes when the state is first entered.
- **exit()** is an optional function that executes when the state is exited.
- **handler()** is a mandatory function that executes when the HSM is running in this state.
- **parent** is this state's parent. This can only be :ecudoxygen:`ECU_HSM_STATE_NO_PARENT`
  if this is the top state. Otherwise this must always be another state.

To create a state, the handler functions are defined and assigned to an :ecudoxygen:`ecu_hsm_state`
object through use of the :ecudoxygen:`ECU_HSM_STATE_CTOR() <ECU_HSM_STATE_CTOR>` macro.
A parent also needs to be assigned. Therefore states must be defined in top-down order so they 
are visible to the compiler when assigning parents:

.. code-block:: c 

    struct app_hsm
    {
        /* Inherit by declaring ecu_hsm object as first member. */
        struct ecu_hsm hsm;

        /* Additional members unique to application-specific hsm. */
        uint8_t counter1;
        uint8_t counter2;
    };

    static void top_state_on_entry(struct app_hsm *me)
    {
        printf("top state entered!\n");
    }

    static void top_state_on_exit(struct app_hsm *me)
    {
        printf("top state exited!\n");
    }

    static bool top_state_handler(struct app_hsm *me, const void *event)
    {
        printf("top state handled!\n");
        return true;
    }

    static void state1_on_entry(struct app_hsm *me)
    {
        me->counter1++;
        me->counter2++;
        printf("state1 entered!\n");
    }

    static void state1_on_exit(struct app_hsm *me)
    {
        me->counter1--;
        me->counter2--;
        printf("state1 exited!\n");
    }

    static bool state1_handler(struct app_hsm *me, const void *event)
    {
        printf("state1 handled!\n");
        return true;
    }

    /* Notice how states are created in top-down order. */
    static const struct ecu_hsm_state TOP_STATE = ECU_HSM_STATE_CTOR(
        &top_state_on_entry, &top_state_on_exit, &top_state_handler, ECU_HSM_STATE_NO_PARENT
    );

    static const struct ecu_hsm_state STATE1 = ECU_HSM_STATE_CTOR(
        &state1_on_entry, &state1_on_exit, &state1_handler, &TOP_STATE
    );

**Notice how state functions take in the user's HSM type (struct app hsm *) but** 
:ecudoxygen:`ecu_hsm_state` **stores functions that take in (struct ecu_hsm \*)**.

States must be able to work with the user's HSM in order to be useful, however this 
framework has no knowledge of the application and its types. As explained in the
:ref:`State Machine Representation Section <hsm_state_machine_representation>`,
this dilemna is solved by inheritance. User HSMs must inherit :ecudoxygen:`ecu_hsm`.
Therefore :ecudoxygen:`ecu_hsm` is a base class that acts as a common interface 
between the application and this framework, allowing these functions to be assigned 
to a state by upcasting.

:ecudoxygen:`ECU_HSM_STATE_CTOR() <ECU_HSM_STATE_CTOR>` performs these upcasts
and expands to the full struct initialization of :ecudoxygen:`ecu_hsm_state`.
The following is equivalent to the code snippet above:

.. code-block:: c 

    static const struct ecu_hsm_state TOP_STATE = 
    {
        .entry = (void (*)(struct ecu_hsm *))(&top_state_on_entry),
        .exit = (void (*)(struct ecu_hsm *))(&top_state_on_exit),
        .handler = (bool (*)(struct ecu_hsm *, const void *))(&top_state_handler),
        .parent = ECU_HSM_STATE_NO_PARENT
    };

    static const struct ecu_hsm_state STATE1 = 
    {
        .entry = (void (*)(struct ecu_hsm *))(&state1_on_entry),
        .exit = (void (*)(struct ecu_hsm *))(&state1_on_exit),
        .handler = (bool (*)(struct ecu_hsm *, const void *))(&state1_handler),
        .parent = &TOP_STATE
    };

The :ref:`State Machine Representation Section <hsm_state_machine_representation>`
explains why these casts are always safe.
It is recommended to use this macro for better encapsulation as it contains 
all upcasts and protects the application from any changes to the 
:ecudoxygen:`ecu_hsm_state` struct.

:ecudoxygen:`ECU_HSM_STATE_ENTRY_UNUSED` and :ecudoxygen:`ECU_HSM_STATE_EXIT_UNUSED` can be supplied 
if entry() and exit() functions are unused. In this case, only the mandatory handler() function 
will run for this state:

.. code-block:: c 

    static const struct ecu_hsm_state STATE1 = ECU_HSM_STATE_CTOR(
        ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &state1_handler, &TOP_STATE
    );


State Hierarchy
-------------------------------------------------
.. _hsm_state_hierarchy:

An HSM's structure is solely defined by the parent members in :ecudoxygen:`ecu_hsm_state`.
Take this example HSM of a simple wireless keyboard:

.. figure:: /images/hsm/keyboard_hsm.svg
  :width: 600
  :align: center

  Keyboard HSM

It would be defined through the following parent member assignments:

.. code-block:: c 

    /* Notice how states are defined in top-down order to allow child
    states to assign their parents. The compiler must see the object's
    full definition before it can be assinged to another struct. I.e. 
    TOP_STATE must be defined before ON_STATE can assign it as its parent. */
    static const struct ecu_hsm_state TOP_STATE = ECU_HSM_STATE_CTOR(
        ..., ECU_HSM_STATE_NO_PARENT
    );

    static const struct ecu_hsm_state ON_STATE = ECU_HSM_STATE_CTOR(
        ..., &TOP_STATE
    );

    static const struct ecu_hsm_state OFF_STATE = ECU_HSM_STATE_CTOR(
        ..., &TOP_STATE
    );

    static const struct ecu_hsm_state DEFAULT_STATE = ECU_HSM_STATE_CTOR(
        ..., &ON_STATE
    );

    static const struct ecu_hsm_state CAPS_LOCK_STATE = ECU_HSM_STATE_CTOR(
        ..., &ON_STATE
    );

An HSM has **only one top state**, which encompasses the entire state hierarchy for 
better organization. It has no parent, so it is assigned :ecudoxygen:`ECU_HSM_STATE_NO_PARENT`.
**This is only allowed for the top state. Every other state must have a non-NULL parent.**

As stated in the :ref:`Overview Section <hsm_overview>`, code repetition is avoided
by allowing events to be propagated up to parent states. **Each state's handler function 
can return false to propagate the event up to its parent. Otherwise a handler should return 
true when it processes an event, which stops the event propagation.**

The example HSM shows both the DEFAULT_STATE and CAPS_LOCK_STATE transition to the OFF_STATE on the 
DISCONNECTED event. An FSM would require this event to be processed in both states, resulting in code
repetition. Notice the repetition in the processing of the DISCONNECTED event:

.. figure:: /images/hsm/keyboard_fsm.svg
  :width: 600
  :align: center

  Keyboard FSM Code Repetition

.. code-block:: c 

    struct app_fsm
    {
        /* Inherit base ecu_fsm class. */
        struct ecu_fsm fsm;
    };

    static void default_state_handler(struct app_fsm *me, const void *event)
    {
        switch (event id)
        {
            case DISCONNECTED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &OFF_STATE);
                break;
            }

            case CAPS_LOCK_PRESSED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &CAPS_LOCK_STATE);
                break;
            }

            default:
            {
                /* Ignore all other events. */
                break;
            }
        }
    }

    static void caps_lock_state_handler(struct app_fsm *me, const void *event)
    {
        switch (event id)
        {
            case DISCONNECTED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &OFF_STATE);
                break;
            }

            case CAPS_LOCK_PRESSED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &DEFAULT_STATE);
                break;
            }

            default:
            {
                /* Ignore all other events. */
                break;
            }
        }
    }

An HSM allows these two states to be children of the ON_STATE. Instead of 
processing the DISCONNECTED event directly, DEFAULT_STATE and CAPS_LOCK_STATE
can propagate it up to its parent (the ON_STATE) by returning false in their 
respective handler functions:

.. code-block:: c 

    struct app_hsm
    {
        /* Inherit base ecu_hsm class. */
        struct ecu_hsm hsm;
    };

    static bool on_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case DISCONNECTED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &OFF_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (TOP_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

    static bool default_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &CAPS_LOCK_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

    static bool caps_lock_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &DEFAULT_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

Notice only the ON_STATE processes the DISCONNECTED event, eliminating any code repetition.
Also notice how if a new state that is a child of ON_STATE is added, it does not need to 
process the DISCONNECTED event. Like the DEFAULT_STATE and CAPS_LOCK_STATE, it can simply
propagate it up to its parent.

The advantages an HSM provides becomes more apparent as state machines increase in complexity.
For example, shift key support can be added to the keyboard that was previously modeled.
The status of the shift key is dispatched to the state machine with the SHIFT_KEY_PRESSED 
and SHIFT_KEY_RELEASED events.

Modeling this behavior with an FSM makes code repetition even more pronounced, as processing 
of these two events is identical in the DEFAULT_STATE and CAPS_LOCK_STATE:

.. code-block:: c 

    struct app_fsm
    {
        /* Inherit base ecu_fsm class. */
        struct ecu_fsm fsm;

        /* Members specific to application fsm. */
        bool shifted;
    };

    static void default_state_handler(struct app_fsm *me, const void *event)
    {
        switch (event id)
        {
            case DISCONNECTED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &OFF_STATE);
                break;
            }

            case SHIFT_PRESSED:
            {
                me->shifted = true;
                break;
            }

            case SHIFT_RELEASED:
            {
                me->shifted = false;
                break;
            }

            case CAPS_LOCK_PRESSED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &CAPS_LOCK_STATE);
                break;
            }

            default:
            {
                /* Ignore all other events. */
                break;
            }
        }
    }

    static void caps_lock_state_handler(struct app_fsm *me, const void *event)
    {
        switch (event id)
        {
            case DISCONNECTED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &OFF_STATE);
                break;
            }

            case SHIFT_PRESSED:
            {
                me->shifted = true;
                break;
            }

            case SHIFT_RELEASED:
            {
                me->shifted = false;
                break;
            }

            case CAPS_LOCK_PRESSED:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &DEFAULT_STATE);
                break;
            }

            default:
            {
                /* Ignore all other events. */
                break;
            }
        }
    }

Notice the repetition in the processing of the DISCONNECTED, SHIFT_PRESSED, and 
SHIFT_RELEASED events. Like before, an HSM eliminates this repetition by allowing
the SHIFT_PRESSED and SHIFT_RELEASED events to be processed in the parent state (ON_STATE).

.. code-block:: c 

    struct app_hsm
    {
        /* Inherit base ecu_hsm class. */
        struct ecu_hsm hsm;

        /* Members specific to application hsm. */
        bool shifted;
    };

    static bool on_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case DISCONNECTED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &OFF_STATE);
                break;
            }

            case SHIFT_PRESSED:
            {
                me->shifted = true;
                break;
            }

            case SHIFT_RELEASED:
            {
                me->shifted = false;
                break;
            }

            default:
            {
                /* Propagate all other events to parent (TOP_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

    static bool default_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &CAPS_LOCK_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

    static bool caps_lock_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &DEFAULT_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

As a final note, **it is mandatory for the state machine's handler function to eventually
return true.** Building off of the same keyboard HSM example, the following implementation 
of the top state would be illegal:

.. code-block:: c 

    static bool top_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            /*... all event IDs that should be processed in the TOP_STATE. */

            default:
            {
                /* Ignore all other events. ILLEGAL IMPLEMENTATION. */
                handled = false;
                break;
            }
        }
    }

If a garbage event is dispatched, it would eventually reach the TOP_STATE where
the handler would still return false. **If the event should be ignored the TOP_STATE
should simply not process the event and return true. Otherwise the framework 
assumes the event was never processed due to the HSM being implemented incorrectly.**
Therefore the correct TOP_STATE implementation would be:

.. code-block:: c 

    static bool top_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            /*... all event IDs that should be processed in the TOP_STATE. */

            default:
            {
                /* Ignore all other events but return true. Tells the 
                framework the event has been processed. */
                break;
            }
        }
    }


State Transitions
-------------------------------------------------
.. _hsm_state_transitions:

A transition is signalled by the user by calling :ecudoxygen:`ecu_hsm_change_state() <ecu_hsm_change_state>`.

State machine behavior is abstracted away through the use of this function
and the rest of the API. Internally, this framework will execute the proper 
entry(), exit(), and handler() functions for each state defined in the user's 
state machine. An HSM state transition is performed by following these steps:

#. Record three states: the **starting state**, the **new state**, and the 
   **least common ancestor state (LCA)** of the starting and new state.

#. Traverse from the starting state up to the LCA, running each state's
   exit handler along the way. LCA exit is **not** ran.

#. Traverse from the LCA to the new state, running each state's entry
   handler along the way. LCA entry is **not** ran.

The previous keyboard HSM example can be used to showcase a state transition:

.. figure:: /images/hsm/keyboard_hsm.svg
  :width: 600
  :align: center

  Keyboard HSM

The pseudocode implementation of this state machine is recycled from the previous
section as well. Notice how a state transition is signalled in the ON_STATE when 
a DISCONNECTED event is processed:

.. code-block:: c 

    struct app_hsm
    {
        /* Inherit base ecu_hsm class. */
        struct ecu_hsm hsm;
    };

    static bool on_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case DISCONNECTED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &OFF_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (TOP_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

    static bool default_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &CAPS_LOCK_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

    static bool caps_lock_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &DEFAULT_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

This state hierarchy and any other HSM can be represented as a generic tree, with the top state 
being the root:

.. figure:: /images/hsm/keyboard_hsm_tree.svg
  :width: 800
  :align: center

  Keyboard HSM Tree Representation

For this example, the state machine is currently in the **DEFAULT_STATE**. A DISCONNECTED
event is dispatched, causing a state transition from **DEFAULT_STATE** to **OFF_STATE**.
This is known as an **inner state transition**.

.. figure:: /images/hsm/inner_state_transition_path.svg
  :width: 600
  :align: center

  Inner State Transition Path

Following the state transition algorithm steps outlined at the start of this section:

#. Record three states: the **starting state**, the **new state**, and the 
   **least common ancestor state (LCA)** of the starting and new state.

   - **Starting State** = DEFAULT_STATE
   - **New State** = OFF_STATE
   - **LCA(DEFAULT_STATE, OFF_STATE)** = TOP_STATE

#. Traverse from the starting state up to the LCA, running each state's
   exit handler along the way. LCA exit is **not** ran.

   .. code-block:: text

        DEFAULT_STATE::exit() -> ON_STATE::exit()

#. Traverse from the LCA to the new state, running each state's entry
   handler along the way. LCA entry is **not** ran.

    .. code-block:: text

        OFF_STATE::entry()

To summarize, the full code execution order is:

.. code-block:: text

    ecu_hsm_dispatch(&APP_HSM, &DISCONNECTED_EVENT) -> DEFAULT_STATE::handler() -> 
    DEFAULT_STATE::exit() -> ON_STATE::exit() -> OFF_STATE::entry()

:ecudoxygen:`ecu_hsm_dispatch() <ecu_hsm_dispatch>` would be called by the
application code. The remaining handlers are automatically run by this framework.

A **local state transition** occurs when the new state is a child or parent of the
starting state. The state traversal algorithm remains exactly the same. Expanding 
on the DEFAULT_STATE, we can add an event that causes a transition from DEFAULT_STATE
to TOP_STATE:

.. code-block:: c 

    static bool default_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case TO_TOP:
            {
                /* Local state transition. */
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &TOP_STATE);
                break;
            }

            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &CAPS_LOCK_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

Assuming the HSM is in the DEFAULT_STATE, dispatchin the TO_TOP event causes 
a local state transition because TOP_STATE is a parent of DEFAULT_STATE:

.. figure:: /images/hsm/local_state_transition_path.svg
  :width: 600
  :align: center

  Local State Transition Path

Following the state transition algorithm steps outlined at the start of this section:

#. Record three states: the **starting state**, the **new state**, and the 
   **least common ancestor state (LCA)** of the starting and new state.

   - **Starting State** = DEFAULT_STATE
   - **New State** = TOP_STATE
   - **LCA(DEFAULT_STATE, TOP_STATE)** = TOP_STATE

#. Traverse from the starting state up to the LCA, running each state's
   exit handler along the way. LCA exit is **not** ran.

   .. code-block:: text

        DEFAULT_STATE::exit() -> ON_STATE::exit()

#. Traverse from the LCA to the new state, running each state's entry
   handler along the way. LCA entry is **not** ran.

    .. code-block:: text

        Nothing!

To summarize, the full code execution order is:

.. code-block:: text

    ecu_hsm_dispatch(&APP_HSM, &TO_TOP_EVENT) -> DEFAULT_STATE::handler() -> 
    DEFAULT_STATE::exit() -> ON_STATE::exit()

:ecudoxygen:`ecu_hsm_dispatch() <ecu_hsm_dispatch>` would be called by the
application code. The remaining handlers are automatically run by this framework.

Finally, **self-state transitions** are also allowed. This is often done to 
reset a state. Expanding on the DEFAULT_STATE, we can add a RESET event that 
cause a self-state transition:

.. code-block:: c 

    static bool default_state_handler(struct app_hsm *me, const void *event)
    {
        bool handled = true;

        switch (event id)
        {
            case RESET:
            {
                /* Self-state transition. */
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &DEFAULT_STATE);
                break;
            }

            case TO_TOP:
            {
                /* Local state transition. */
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &TOP_STATE);
                break;
            }

            case CAPS_LOCK_PRESSED:
            {
                ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &CAPS_LOCK_STATE);
                break;
            }

            default:
            {
                /* Propagate all other events to parent (ON_STATE). */
                handled = false;
                break;
            }
        }

        return handled;
    }

.. figure:: /images/hsm/self_state_transition_path.svg
  :width: 600
  :align: center

  Self State Transition Path

**Self-state transitions are the only transition types that deviate from the 
3-step traversal algorithm explained above.** The state's exit and entry handlers 
are simply ran instead, making the full code execution order:

.. code-block:: text

    ecu_hsm_dispatch(&APP_HSM, &RESET_EVENT) -> DEFAULT_STATE::handler() -> 
    DEFAULT_STATE::exit() -> DEFAULT_STATE::entry()

:ecudoxygen:`ecu_hsm_dispatch() <ecu_hsm_dispatch>` would be called by the
application code. The remaining handlers are automatically run by this framework.

As a final note, **transitions in entry and exit handlers are currently NOT allowed
in order to simplify the state traversal algorithm**.

.. code-block:: c

    static void state_entry(struct app_hsm *me)
    {
        /* NOT allowed!! */
        ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &NEW_STATE);
    }

    static void state_exit(struct app_hsm *me)
    {
        /* NOT allowed!! */
        ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &NEW_STATE);
    }

It is guaranteed that transitions in exit handlers will **never** be allowed.
Support **may** be added in the future to allow transitions in entry handlers.


Event-Driven Paradigm
-------------------------------------------------
.. _hsm_event_driven_paradigm:

See :ref:`Event-Driven Paradigm Section in the Finite State Machine Framework <fsm_event_driven_paradigm>`.
The same logic carries over to HSMs as well.


Run to Completion Semantics
-------------------------------------------------
.. _hsm_run_to_completion_semantics:

.. See :ref:`Run to Completion Semantics Section in the Finite State Machine Framework <fsm_run_to_completion_semantics>`.
.. The same logic carries over to HSMs as well.


ecu_hsm_state
=================================================
This is an object that represents a single state within the user's state machine.
It is recommended to read the :ref:`State Representation Section <hsm_state_representation>` 
before using this API, as it explains the :ecudoxygen:`ecu_hsm_state` object in detail.


Constructors
-------------------------------------------------


ECU_HSM_STATE_CTOR()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Creates an :ecudoxygen:`ecu_hsm_state` at compile-time. If a state's entry 
or exit handler are unused, :ecudoxygen:`ECU_HSM_STATE_ENTRY_UNUSED` and 
:ecudoxygen:`ECU_HSM_STATE_EXIT_UNUSED` can be supplied. The state's parent
**must** be another state unless this is the top state. :ecudoxygen:`ECU_HSM_STATE_NO_PARENT`
must be supplied as the parent member if this is the top state. These details are
fully explained in the :ref:`State Representation Section <hsm_state_representation>`:

.. code-block:: c 

    static void top_state_entry(struct app_hsm *me);
    static void top_state_exit(struct app_hsm *me);
    static bool top_state_handler(struct app_hsm *me, const void *event);
    static void child_state_entry(struct app_hsm *me);
    static void child_state_exit(struct app_hsm *me);
    static bool child_state_handler(struct app_hsm *me, const void *event);

    /* Notice how states are created in top-down order. */
    static const struct ecu_hsm_state TOP_STATE = ECU_HSM_STATE_CTOR(
        &top_state_entry, &top_state_exit, &top_state_handler, ECU_HSM_STATE_NO_PARENT
    );

    static const struct ecu_hsm_state CHILD_STATE = ECU_HSM_STATE_CTOR(
        &child_state_entry, &child_state_exit, &child_state_handler, &TOP_STATE
    );


ecu_hsm
=================================================
This is an object that represents the base hierarchical state machine (HSM) class. 
Users create their own HSMs by inheriting this base class and passing it to 
this API by upcasting. It is recommended to read the
:ref:`State Machine Representation Section <hsm_state_machine_representation>` 
before using this API, as it explains this inheritance scheme in detail.


Constructors
-------------------------------------------------


ecu_hsm_ctor()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _hsm_ecu_hsm_ctor:

Sets the starting state the HSM should be in. The height of the HSM must also be specified.

.. warning:: 

    Must be called once on startup before the HSM is used. User is also responsible 
    for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct app_hsm hsm;  /* Must inherit ecu_hsm. User must allocate memory before constructor. */
    ecu_hsm_start(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm)); /* ILLEGAL. Must construct before using. */

    ecu_hsm_ctor(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm), &INIT_STATE, &TOP_STATE, 3);
    ecu_hsm_start(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm)); /* Ok. */

The HSM's height starts at 0 and increases with each additional level added to the state
hierarchy. For example:

.. figure:: /images/hsm/ecu_hsm_ctor.svg
  :width: 600
  :align: center

  ecu_hsm_ctor() Height

This value is used as a fail-safe to prevent infinite loops when traversing up the 
state hierarchy.


Member Functions
-------------------------------------------------


TODO!!WAS_ECU_HSM_IS_BASE_OF_BEFORE!!()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Verifies, at compile-time, that a user's HSM correctly inherits 
the :ecudoxygen:`ecu_hsm` base class:

.. code-block:: c 

    struct correct_hsm
    {
        struct ecu_hsm hsm; /* Inherit by declaring ecu_hsm object as first member. */
        uint8_t counter1;
        uint8_t counter2;
    };

    struct incorrect_hsm
    {
        uint8_t counter1;
        struct ecu_hsm hsm;
        uint8_t counter2;
    };

    /* Passes. */
    ECU_STATIC_ASSERT( (TODO!!WAS_ECU_HSM_IS_BASE_OF_BEFORE!!(hsm, struct correct_hsm)), "ecu_hsm must be first member.");

    /* Compilation error. */
    ECU_STATIC_ASSERT( (TODO!!WAS_ECU_HSM_IS_BASE_OF_BEFORE!!(hsm, struct incorrect_hsm)), "ecu_hsm must be first member.");


!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Upcasts a user's derived HSM type back into the :ecudoxygen:`ecu_hsm` 
base class. This macro encapsulates the cast and allows derived HSMs 
to be passed into base class functions defined in this framework.

.. code-block:: c 

    struct app_hsm
    {
        /* Inherit by declaring ecu_hsm object as first member. */
        struct ecu_hsm hsm;

        /* Additional members unique to application-specific hsm. */
        uint8_t counter1;
        uint8_t counter2;
    };

    struct app_hsm hsm;
    ecu_hsm_ctor(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm), &INIT_STATE, &TOP_STATE, 3);
    ecu_hsm_start(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm));
    // ...


ecu_hsm_start()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Runs each state's entry handler, starting from the top state and ending at 
the initial state assigned in :ref:`ecu_hsm_ctor() <hsm_ecu_hsm_ctor>`. Inclusive,
meaning top state's and initial state's entry handlers are also ran.

.. code-block:: c 

    static void top_state_entry(struct app_hsm *me)
    {
        printf("top entered!");
    }

    static void top_state_exit(struct app_hsm *me)
    {
        printf("top exited!");
    }

    static bool top_state_handler(struct app_hsm *me, const void *event)
    {
        printf("top handled!");
        return true;
    }

    static void state1_entry(struct app_hsm *me)
    {
        printf("S1 entered!");
    }

    static void state1_exit(struct app_hsm *me)
    {
        printf("S1 exited!");
    }

    static bool state1_handler(struct app_hsm *me, const void *event)
    {
        printf("S1 handled!");
        return true;
    }

    static void state2_entry(struct app_hsm *me)
    {
        printf("S2 entered!");
    }

    static void state2_exit(struct app_hsm *me)
    {
        printf("S2 exited!");
    }

    static bool state2_handler(struct app_hsm *me, const void *event)
    {
        printf("S2 handled!");
        return true;
    }

    static const struct ecu_hsm_state TOP_STATE = ECU_HSM_STATE_CTOR(
        &top_state_entry, &top_state_exit, &top_state_handler, ECU_HSM_STATE_NO_PARENT
    );

    static const struct ecu_hsm_state STATE1 = ECU_HSM_STATE_CTOR(
        &state1_entry, &state1_exit, &state1_handler, &TOP_STATE
    );

    static const struct ecu_hsm_state STATE2 = ECU_HSM_STATE_CTOR(
        &state2_entry, &state2_exit, &state2_handler, &STATE1
    );

    struct app_hsm hsm; /* Must inherit ecu_hsm. */
    ecu_hsm_ctor(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm), &STATE2, &TOP_STATE, 2);
    ecu_hsm_start(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm));

ecu_hsm_start() causes the following code execution order:

.. code-block:: text 

    "top entered!" -> "S1 entered!" -> "S2 entered!"

Once complete, the HSM will now be in STATE2.


ecu_hsm_change_state()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _hsm_ecu_hsm_change_state:

Signals that the HSM must transition to a new state or perform a self-transition.
The :ref:`State Transitions Section <hsm_state_transitions>` explains this in detail.

.. warning:: 

    Can only be called within a state's handler function. Using this in a state's 
    entry or exit handler results in an error. Using this in the external application 
    is undefined behavior.


ecu_hsm_dispatch()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Relays an event to the HSM where it is processed by the current state's handler function. 
The event is propagated up the state hierarchy until it is handled. All state transitions
signalled via :ref:`ecu_hsm_change_state() <hsm_ecu_hsm_change_state>` are also processed
in this function.

The :ref:`State Hierarchy <hsm_state_hierarchy>` and :ref:`State Transitions <hsm_state_transitions>`
sections fully explain how this framework propagates events up the hierarchy and handles 
state transitions. It is also recommended to read the 
:ref:`Event Driven Paradigm Section in the Finite State Machine Framework<fsm_event_driven_paradigm>`.

.. code-block:: c

    static void top_state_entry(struct app_hsm *me)
    {
        printf("top entered!");
    }

    static void top_state_exit(struct app_hsm *me)
    {
        printf("top exited!");
    }

    static bool top_state_handler(struct app_hsm *me, const void *event)
    {
        printf("top handled!");
        return true;
    }

    static void state1_entry(struct app_hsm *me)
    {
        printf("S1 entered!");
    }

    static void state1_exit(struct app_hsm *me)
    {
        printf("S1 exited!");
    }

    static bool state1_handler(struct app_hsm *me, const void *event)
    {
        printf("S1 handled!");
        if (event == TO_S3_EVENT)
        {
            ecu_hsm_change_state(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(me), &STATE3);
        }
        return true;
    }

    static void state2_entry(struct app_hsm *me)
    {
        printf("S2 entered!");
    }

    static void state2_exit(struct app_hsm *me)
    {
        printf("S2 exited!");
    }

    static bool state2_handler(struct app_hsm *me, const void *event)
    {
        printf("S2 handled!");
        return true;
    }

    static void state3_entry(struct app_hsm *me)
    {
        printf("S3 entered!");
    }

    static void state3_exit(struct app_hsm *me)
    {
        printf("S3 exited!");
    }

    static bool state3_handler(struct app_hsm *me, const void *event)
    {
        printf("S3 handled!");
        return true;
    }

    static const struct ecu_hsm_state TOP_STATE = ECU_HSM_STATE_CTOR(
        &top_state_entry, &top_state_exit, &top_state_handler, ECU_HSM_STATE_NO_PARENT
    );

    static const struct ecu_hsm_state STATE1 = ECU_HSM_STATE_CTOR(
        &state1_entry, &state1_exit, &state1_handler, &TOP_STATE
    );

    static const struct ecu_hsm_state STATE2 = ECU_HSM_STATE_CTOR(
        &state2_entry, &state2_exit, &state2_handler, &TOP_STATE
    );

    static const struct ecu_hsm_state STATE3 = ECU_HSM_STATE_CTOR(
        &state3_entry, &state3_exit, &state3_handler, &STATE2
    );

    struct app_hsm hsm; /* Must inherit ecu_hsm. */
    ecu_hsm_ctor(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm), &STATE1, &TOP_STATE, 2);
    ecu_hsm_dispatch(!!TODO_WAS_ECU_HSM_BASE_CAST_BEFORE!!!(&hsm), &TO_S3_EVENT);

ecu_hsm_dispatch() causes the following code execution order:

.. code-block:: text 

    "S1 handled!" -> "S1 exit!" -> "S2 entered!" -> "S3 entered!"

Once complete, the HSM will now be in STATE3.


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/hsm_8h>
