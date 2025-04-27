.. _fsm_h:

fsm.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

Event-driven finite state machine (FSM) framework that applications inherit from to 
create their own state machines. The complexity of running the FSM and handling state
transitions are fully encapsulated within this API.


Theory
=================================================
This section showcases proper use of the framework and explains its design rationale.
It is recommended to read this section before using the framework.


State Machine Representation
-------------------------------------------------
.. _fsm_state_machine_representation:

A state machine is represented by an :ecudoxygen:`ecu_fsm` base class. Application-specific
state machines are defined by **inheriting** this base class via C-style inheritance.
This is accomplished by declaring an :ecudoxygen:`ecu_fsm` object as the **first struct member**.

.. code-block:: c 

    struct app_fsm
    {
        /* Inherit by declaring ecu_fsm object as first member. */
        struct ecu_fsm fsm;

        /* Additional members unique to application-specific fsm. */
        uint8_t counter1;
        uint8_t counter2;
    };

Inheritance provides a common interface between this module and the application's type.
This API has no knowledge of the application, so it is a base class that takes in
a base class type (struct ecu_fsm \*). Derived types are passed into the API by upcasting:

.. code-block:: c 

    struct app_fsm APP_FSM;
    ecu_fsm_ctor((struct ecu_fsm *)&APP_FSM, &INIT_STATE);
    ecu_fsm_start((struct ecu_fsm *)&APP_FSM);
    // ....

For better encapsulation, this cast should be wrapped within :ecudoxygen:`ECU_FSM_BASE_CAST() <ECU_FSM_BASE_CAST>`:

.. code-block:: c 

    struct app_fsm APP_FSM;
    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&APP_FSM), &INIT_STATE);
    ecu_fsm_start(ECU_FSM_BASE_CAST(&APP_FSM));
    // ....

Inheritance in C is accomplished this way. The example above is **functionally** equivalent
to the following C++ code:

.. code-block:: cpp

    class ecu_fsm
    {
        void ecu_fsm_start()
        {
            // ...
        }

        // ...rest of API functions.
    };

    class app_fsm : public ecu_fsm
    {
    public:
        uint8_t counter1;
        uint8_t counter2;
    };

    app_fsm APP_FSM;
    APP_FSM.ecu_fsm_start();

Upcasting in this manner is always legal and safe **as long as :ecudoxygen:`ecu_fsm` is the first member**.
The C standard mandates there is no struct padding before the first member so a derived pointer can
always be represented as a base pointer. Inheritance is only unsafe if :ecudoxygen:`ecu_fsm` is not 
the first member due to misaligned access:

.. figure:: /images/fsm/inheritance.svg
  :width: 600
  :align: center

  Inheritance

:ecudoxygen:`ECU_FSM_IS_BASEOF() <ECU_FSM_IS_BASEOF>` macro returns true if :ecudoxygen:`ecu_fsm` is 
correctly inherited. Otherwise it returns false. These values are evaluated at compile-time so the 
condition can be statically asserted:

.. code-block:: c 

    struct correct_fsm
    {
        struct ecu_fsm fsm;
        uint8_t counter1;
        uint8_t counter2;
    };

    struct incorrect_fsm
    {
        uint8_t counter1;
        struct ecu_fsm fsm;
        uint8_t counter2;
    };

    /* Passes. */
    ECU_STATIC_ASSERT( (ECU_FSM_IS_BASEOF(fsm, struct correct_fsm)), "ecu_fsm must be first member.");

    /* Compilation error. */
    ECU_STATIC_ASSERT( (ECU_FSM_IS_BASEOF(fsm, struct incorrect_fsm)), "ecu_fsm must be first member.");


State Representation
-------------------------------------------------
States are represented as an :ecudoxygen:`ecu_fsm_state` object that contains 
a set of handler functions:

.. code-block:: c 

    struct ecu_fsm_state
    {
        void (*entry)(struct ecu_fsm *fsm);
        void (*exit)(struct ecu_fsm *fsm);
        void (*handler)(struct ecu_fsm *fsm, const void *event);
    };

- **entry()** is an optional function that executes when the state is first entered.
- **exit()** is an optional function that executes when the state is exited.
- **handler()** is a mandatory function that executes when the FSM is running in this state.

This API has no knowledge of the application, so :ecudoxygen:`ecu_fsm_state` stores
functions to the base class type (struct ecu_fsm \*). The inheritance scheme described in the 
:ref:`State Machine Representation Section <fsm_state_machine_representation>` 
allows the application to define state functions that take in a derived fsm type. These
are stored in the :ecudoxygen:`ecu_fsm_state` by upcasting them back into functions that
take in the base class type (struct ecu_fsm \*).

To create a state, these functions are defined and assigned to an :ecudoxygen:`ecu_fsm_state`
object through use of the :ecudoxygen:`ECU_FSM_STATE_CTOR() <ECU_FSM_STATE_CTOR>` macro:

.. code-block:: c 

    struct app_fsm
    {
        /* Inherit by declaring ecu_fsm object as first member. */
        struct ecu_fsm fsm;

        /* Additional members unique to application-specific fsm. */
        uint8_t counter1;
        uint8_t counter2;
    };

    static void state1_on_entry(struct app_fsm *me)
    {
        me->counter1++;
        me->counter2++;
        printf("state1 entered!\n");
    }

    static void state1_on_exit(struct app_fsm *me)
    {
        me->counter1--;
        me->counter2--;
        printf("state1 exited!\n");
    }

    static void state1_handler(struct app_fsm *me, const void *event)
    {
        printf("state1 handled!\n");
    }

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(&state1_on_entry, 
                                                                  &state1_on_exit, 
                                                                  &state1_handler);

:ecudoxygen:`ECU_FSM_STATE_CTOR() <ECU_FSM_STATE_CTOR>` is a helper macro that expands to
the full struct initialization of :ecudoxygen:`ecu_fsm_state`. The above code is equivalent to:

.. code-block:: c 

    static const struct ecu_fsm_state STATE1 = 
    {
        .entry = (void (*)(struct ecu_fsm *))(&state1_on_entry),
        .exit = (void (*)(struct ecu_fsm *))(&state1_on_exit),
        .handler = (void (*)(struct ecu_fsm *, const void *))(&state1_handler)
    };

It is recommended to use the macro for better encapsulation. It contains all casts and protects the 
application from any changes to the :ecudoxygen:`ecu_fsm_state` struct. The changepoint would be 
limited to the definition of the :ecudoxygen:`ECU_FSM_STATE_CTOR() <ECU_FSM_STATE_CTOR>` macro as 
oppposed to the entire application.

:ecudoxygen:`ECU_FSM_STATE_ENTRY_UNUSED` and :ecudoxygen:`ECU_FSM_STATE_EXIT_UNUSED` can be supplied 
if entry() and exit() functions are unused.

.. code-block:: c 

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(ECU_FSM_STATE_ENTRY_UNUSED, 
                                                                  ECU_FSM_STATE_EXIT_UNUSED, 
                                                                  &state1_handler);

In this case, only the mandatory handler() function will run for this state.


State Transitions
-------------------------------------------------
State machine behavior is solely defined using this API without having to worry about
the complexities of running the FSM and handling state transitions. Internally, this framework
will execute the proper entry(), exit(), and handler() functions for each state defined in 
the user's state machine. 

For example if the user's FSM transitions from STATE1 to STATE2, the framework will execute the 
following in order:

.. figure:: /images/fsm/single_state_transition.svg
  :width: 600
  :align: center

  Single State Transition

Accompanying pseudocode defining this behavior is shown below:

.. code-block:: c 

    /*--------------------- STATE1 definition ---------------------*/
    static void state1_on_entry(struct app_fsm *me)
    {
        printf("state1 entered!");
    }

    static void state1_on_exit(struct app_fsm *me)
    {
        printf("state1 exited!");
    }

    static void state1_handler(struct app_fsm *me, const void *event)
    {
        const struct app_event *e = (const struct app_event *)event;
        printf("state1 handler!");

        if (e->id == go to STATE2)
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE2);
        }
    }

    /*--------------------- STATE2 definition ---------------------*/
    static void state2_on_entry(struct app_fsm *me)
    {
        printf("state2 entered!");
    }

    static void state2_on_exit(struct app_fsm *me)
    {
        printf("state2 exited!");
    }

    static void state2_handler(struct app_fsm *me, const void *event)
    {
        printf("state2 handler!");
    }

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(&state1_on_entry,
                                                                  &state1_on_exit,
                                                                  &state1_handler);
    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(&state2_on_entry,
                                                                  &state2_on_exit,
                                                                  &state2_handler);

In this case, "state1 handler!", "state1 exited!", "state2 entered!" is printed to the console 
in order. The FSM is now in STATE2 so if an event is dispatched via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`,
it will be processed in state2_handler().

Consecutive state transitions are also allowed by calling :ecudoxygen:`ecu_fsm_change_state() <ecu_fsm_change_state>`
within a state's entry() function. For example:

.. code-block:: c 

    /*--------------------- STATE1 definition ---------------------*/
    static void state1_on_entry(struct app_fsm *me)
    {
        printf("state1 entered!");
    }

    static void state1_on_exit(struct app_fsm *me)
    {
        printf("state1 exited!");
    }

    static void state1_handler(struct app_fsm *me, const void *event)
    {
        const struct app_event *e = (const struct app_event *)event;
        printf("state1 handler!");

        if (e->id == go to STATE2)
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE2);
        }
    }

    /*--------------------- STATE2 definition ---------------------*/
    static void state2_on_entry(struct app_fsm *me)
    {
        printf("state2 entered!");
        if (go to STATE3)
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE3);
        }
    }

    static void state2_on_exit(struct app_fsm *me)
    {
        printf("state2 exited!");
    }

    static void state2_handler(struct app_fsm *me, const void *event)
    {
        printf("state2 handler!");
    }

    /*--------------------- STATE3 definition ---------------------*/
    static void state3_on_entry(struct app_fsm *me)
    {
        printf("state3 entered!");
    }

    static void state3_on_exit(struct app_fsm *me)
    {
        printf("state3 exited!");
    }

    static void state3_handler(struct app_fsm *me, const void *event)
    {
        printf("state3 handler!");
    }

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(&state1_on_entry,
                                                                  &state1_on_exit,
                                                                  &state1_handler);
    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(&state2_on_entry,
                                                                  &state2_on_exit,
                                                                  &state2_handler);
    static const struct ecu_fsm_state STATE3 = ECU_FSM_STATE_CTOR(&state3_on_entry,
                                                                  &state3_on_exit,
                                                                  &state3_handler);

.. figure:: /images/fsm/consecutive_state_transition.svg
  :width: 600
  :align: center

  Consecutive State Transition

In this case, "state1 handler!", "state1 exited!", "state2 entered!", "state2 exited!",
"state3 entered!" is printed to the console in order. The FSM is now in STATE3 so if an 
event is dispatched via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`, it will be 
processed in state3_handler().

Finally, self-state transitions are also allowed. This is often done to reset the state:

.. code-block:: c 

    /*--------------------- STATE1 definition ---------------------*/
    static void state1_on_entry(struct app_fsm *me)
    {
        printf("state1 entered!");
        reinitialize(me);
    }

    static void state1_on_exit(struct app_fsm *me)
    {
        printf("state1 exited!");
        clear(me);
    }

    static void state1_handler(struct app_fsm *me, const void *event)
    {
        const struct app_event *e = (const struct app_event *)event;
        printf("state1 handler!");

        if (e->id == reset state)
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE1);
        }
    }

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(&state1_on_entry,
                                                                  &state1_on_exit,
                                                                  &state1_handler);

.. figure:: /images/fsm/self_state_transition.svg
  :width: 600
  :align: center

  Self State Transition

In this case, "state1 handler!", "state1 exited!", "state1 entered!" is printed 
to the console in order. The FSM is still in STATE1 so if an event is dispatched 
via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`, it will be processed in state1_handler().


Event-Driven Paradigm
-------------------------------------------------
todo


Run to Completion Semantics
-------------------------------------------------
todo



Example
=================================================
todo show example creating fsm with this framework.


ecu_fsm_state
=================================================


Constructors
-------------------------------------------------


ECU_FSM_STATE_CTOR()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
todo


ecu_fsm
=================================================


Constructors
-------------------------------------------------


ecu_fsm_ctor()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
todo


Member Functions
-------------------------------------------------


ecu_fsm_start()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
todo


ecu_fsm_change_state()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
todo


ecu_fsm_dispatch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
todo


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/fsm_8h>
