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

Event-driven **finite state machine (FSM)** framework that applications inherit from to 
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

**Upcasting in this manner is always legal and safe as long as** :ecudoxygen:`ecu_fsm` **is the first member**.
The C standard mandates there is no struct padding before the first member so a derived pointer can
always be represented as a base pointer. Inheritance is only unsafe if :ecudoxygen:`ecu_fsm` is not 
the first member due to misaligned access:

.. figure:: /images/fsm/inheritance.svg
  :width: 800
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
.. _fsm_state_representation:

States are represented as an :ecudoxygen:`ecu_fsm_state` object that contains 
a set of handler functions. **The contents of the struct are const-qualified,
meaning that states must be initialized at compile-time.**

.. code-block:: c 

    struct ecu_fsm_state
    {
        void (*const entry)(struct ecu_fsm *fsm);
        void (*const exit)(struct ecu_fsm *fsm);
        void (*const handler)(struct ecu_fsm *fsm, const void *event);
    };

- **entry()** is an optional function that executes when the state is first entered.
- **exit()** is an optional function that executes when the state is exited.
- **handler()** is a mandatory function that executes when the FSM is running in this state.

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

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        &state1_on_entry, &state1_on_exit, &state1_handler
    );

**Notice how state functions take in the user's FSM type (struct app_fsm *) but** 
:ecudoxygen:`ecu_fsm_state` **stores functions that take in (struct ecu_fsm \*)**.

States must be able to work with the user's FSM in order to be useful, however this 
framework has no knowledge of the application and its types. As explained in the
:ref:`State Machine Representation Section <fsm_state_machine_representation>`,
this dilemna is solved by inheritance. User FSMs must inherit :ecudoxygen:`ecu_fsm`.
Therefore :ecudoxygen:`ecu_fsm` is a base class that acts as a common interface 
between the application and this framework, allowing these functions to be assigned 
to a state by upcasting.

:ecudoxygen:`ECU_FSM_STATE_CTOR() <ECU_FSM_STATE_CTOR>` performs these upcasts
and expands to the full struct initialization of :ecudoxygen:`ecu_fsm_state`.
The following is equivalent to the code snippet above:

.. code-block:: c 

    static const struct ecu_fsm_state STATE1 = 
    {
        .entry = (void (*)(struct ecu_fsm *))(&state1_on_entry),
        .exit = (void (*)(struct ecu_fsm *))(&state1_on_exit),
        .handler = (void (*)(struct ecu_fsm *, const void *))(&state1_handler)
    };

The :ref:`State Machine Representation Section <fsm_state_machine_representation>`
explains why these casts are always safe.
It is recommended to use this macro for better encapsulation as it contains 
all upcasts and protects the application from any changes to the 
:ecudoxygen:`ecu_fsm_state` struct.

:ecudoxygen:`ECU_FSM_STATE_ENTRY_UNUSED` and :ecudoxygen:`ECU_FSM_STATE_EXIT_UNUSED` can be supplied 
if entry() and exit() functions are unused. In this case, only the mandatory handler() function 
will run for this state:

.. code-block:: c 

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        ECU_FSM_STATE_ENTRY_UNUSED, ECU_FSM_STATE_EXIT_UNUSED, &state1_handler
    );


State Transitions
-------------------------------------------------
.. _fsm_state_transitions:

State machine behavior is abstracted away through the use of this API.
Internally, this framework will execute the proper entry(), exit(), and handler() 
functions for each state defined in the user's state machine. 

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
        printf("state1 handler!");
        if (event causes transition to STATE2)
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

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        &state1_on_entry, &state1_on_exit, &state1_handler
    );

    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(
        &state2_on_entry, &state2_on_exit, &state2_handler
    );

    struct app_fsm APP_FSM; /* Must inherit ecu_fsm. */
    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&APP_FSM), &STATE1);
    ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&APP_FSM), &event_that_causes_transition_to_STATE2);

In this case the code execution order is:

.. code-block:: text 

    "state1 handler!" -> "state1 exited!" -> "state2 entered!"

Once complete, the FSM is now in STATE2. If an event is dispatched 
via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`, it will be processed 
in state2_handler().

Consecutive state transitions are also allowed by calling :ecudoxygen:`ecu_fsm_change_state() <ecu_fsm_change_state>`
within a state's entry() function. For example:

.. figure:: /images/fsm/consecutive_state_transition.svg
  :width: 800
  :align: center

  Consecutive State Transition

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
        printf("state1 handler!");
        if (event causes transition to STATE2)
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE2);
        }
    }

    /*--------------------- STATE2 definition ---------------------*/
    static void state2_on_entry(struct app_fsm *me)
    {
        printf("state2 entered!");
        if (fsm needs to transition to STATE3)
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

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        &state1_on_entry, &state1_on_exit, &state1_handler
    );

    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(
        &state2_on_entry, &state2_on_exit, &state2_handler
    );

    static const struct ecu_fsm_state STATE3 = ECU_FSM_STATE_CTOR(
        &state3_on_entry, &state3_on_exit, &state3_handler
    );

    struct app_fsm APP_FSM; /* Must inherit ecu_fsm. */
    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&APP_FSM), &STATE1);
    ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&APP_FSM), &event_that_causes_transition_to_STATE2);

Assuming the FSM needs to transition to STATE3 in state2_entry(), the code 
execution order is:

.. code-block:: text 

    "state1 handler!" -> "state1 exited!" -> "state2 entered!" -> "state2 exited!" -> "state3 entered!"

Once complete, the FSM is now in STATE3. If an event is dispatched 
via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`, it will be processed 
in state3_handler().

Finally, self-state transitions are also allowed. This is often done to reset a state:

.. figure:: /images/fsm/self_state_transition.svg
  :width: 600
  :align: center

  Self State Transition

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
        printf("state1 handler!");
        if (event causes state reset)
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE1);
        }
    }

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        &state1_on_entry, &state1_on_exit, &state1_handler
    );

    struct app_fsm APP_FSM; /* Must inherit ecu_fsm. */
    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&APP_FSM), &STATE1);
    ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&APP_FSM), &event_that_causes_state_reset);

In this case the code execution order is:

.. code-block:: text 

    "state1 handler!" -> "state1 exited!" -> "state1 entered!"

Once complete, the FSM is still in STATE1. If an event is dispatched 
via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`, it will still be 
processed in state1_handler().


Event-Driven Paradigm
-------------------------------------------------
.. _fsm_event_driven_paradigm:

State machines built with this framework only perform work when events (button press, value change, etc)
are dispatched via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`. All processing is 
encapsulated within the FSM's implementation. **The application never cares about
the state machine's internal details. It treats it as a black-box and simply dispatches events 
to it.**

.. figure:: /images/fsm/event_driven_state_machine.svg
  :width: 600
  :align: center

  Event-Driven State Machine

This pattern naturally decouples the FSM from the application, as all 
forms of communication between the two are limited to event dispatching.
This is fully showcased in the :ref:`Example Section <fsm_example>`.

Notable advantages an event-driven approach provides are:

#. The FSM implementation is fully reusable across multiple applications running
   on different targets. 

#. The decoupling makes code far easier to test. Test code simply creates an event, dispatches 
   it to the state machine, and verifies its output.

#. The decoupling makes thread-safety trivial. Events are **not** shared resources since they 
   are represented as an **object**. Each thread can create their own events. The only shared 
   resource is the FSM object instance which can be delegated to a separate task with an 
   event queue. This is discussed in the :ref:`Run to Completion Semantics Section <fsm_run_to_completion_semantics>`.

#. Less CPU overhead. The state machine only performs work when it needs to.

#. Representing the state machine as an **object** allows multiple instances of the same 
   FSM to be created that operate **independently from one another**.

Compare this to the traditional polling state machine that is unfortunately most commonly used.
The FSM's implementation and communication with the application is usually facilitated 
through extensive use of flags:

.. code-block:: c 

    /*-------------------------- led.h --------------------------*/
    enum state_t{LED_ON_STATE, LED_OFF_STATE};
    extern bool button_pressed;
    extern state_t requested_state;
    extern void polled_led_fsm_run(void);

    /*-------------------------- led.c --------------------------*/
    bool button_pressed = false;
    state_t requested_state = LED_OFF_STATE;

    void polled_led_fsm_run(void)
    {
        /* The FSM is implemented through extensive use of internal flags. 
        Even worse, the FSM and application communicate through global flags. */
        static enum state_t state = LED_OFF_STATE;
        static bool do_work = false;

        switch (state)
        {
            case LED_ON_STATE:
            {
                if (!do_work)
                {
                    turn_led_on();
                    do_work = true;
                }

                if (button_pressed || requested_state == LED_OFF_STATE)
                {
                    state = LED_OFF_STATE;
                    button_pressed = false;
                    do_work = false;
                }
                break;
            }

            case LED_OFF_STATE:
            {
                if (!do_work)
                {
                    turn_led_off();
                    do_work = true;
                }

                if (button_pressed || requested_state == LED_ON_STATE)
                {
                    state = LED_ON_STATE;
                    button_pressed = false;
                    do_work = false;
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }

    /*------------------------- main.c --------------------------*/
    #include "led.h" /* Polled FSM that is tightly coupled and not reusable. */

    while (1)
    {
        /* The polled FSM and application communicate through global flags. */
        button_pressed = button_read();
        if (need to go to OFF STATE)
        {
            requested_state = LED_OFF_STATE;
        }
        else if (need to go to ON STATE)
        {
            requested_state = LED_ON_STATE;
        }

        polled_led_fsm_run();
    }

Compare this to the :ref:`event-driven FSM example <fsm_example>`, which is the 
far superior approach. Polling state machines like the one above are anti-patterns because:

#. They are tightly coupled to the application, making them not reusable.
   Porting a polled FSM to different applications is not trivial since each 
   global flag must be properly initialized and handled by every application that uses it.
   
#. If the FSM's implementation ever changes, then every application that uses it must 
   also change how it edits these global flags.

#. The state machine is **not** an object so only one instance can be created.
   Even worse, its implementation is controlled by static flags that test code 
   cannot change.

#. The state machine is very difficult to test due to the reasonings in the previous points.

#. Thread-safety is **not** trivial since every single global flag and the FSM are shared
   resources.

#. The state machine has to repeatedly run, even when no processing is required most of the time.

#. The state machine is essentially a shared and editable singleton, which is an anti-pattern
   in most cases.


Run to Completion Semantics
-------------------------------------------------
.. _fsm_run_to_completion_semantics:

As discussed in the :ref:`previous section <fsm_event_driven_paradigm>`, event-driven state 
machines make thread-safety trivial as they naturally decouple themselves from the application.
The only shared resource is the FSM object instance, which **must run to completion** to ensure
thread safety.

This means that when an event is dispatched, the FSM must finish processing it before the next one
is processed. Violations of this rule include:

#. Pre-empting calls to this API. Primarily :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`.

#. Misuse of this framework by calling the API (besides :ecudoxygen:`ecu_fsm_change_state() <ecu_fsm_change_state>`)
   within a state's function.

The following assumes the framework is used correctly, so only the first point is a concern.
Thread safety is trivial as the FSM object instance can be offloaded to an RTOS task with 
its own event queue.

.. figure:: /images/fsm/queued_fsm.svg
  :width: 600
  :align: center

  Queued State Machine

.. code-block:: c

    /*--------------------- app_fsm_task.c ----------------------*/
    #include "app_fsm.h" /* Reusable state machine implementation. */
    struct rtos_queue event_queue; /* The event queue is the only shared resource. */

    /* RTOS task. */
    static void app_fsm_task(..)
    {
        /* Create state machine object instance that will run in this task. */
        struct app_fsm APP_FSM;
        
        while (1)
        {
            if (!queue_empty(&event_queue))
            {
                event = queue_read(&event_queue);
                ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&APP_FSM), &event);
            }
        }
    }

    /*------------------------- main.c --------------------------*/
    queue_write(&event_queue, &event1);
    queue_write(&event_queue, &event2);
    queue_write(&event_queue, &event3);

The application posts events to the task's queue instead of directly dispatching them.
The only shared resource is the event queue, which is provided by the RTOS. Therefore 
all reads and writes to it are guaranteed to be thread safe.

Run to completion is always satisfied because the event processing is conducted
within app_fsm_task(), which is an RTOS task. A task cannot pre-empt itself, therefore
all calls to :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>` are guaranteed to finish.


Example
-------------------------------------------------
.. _fsm_example:

This in-depth example uses this state machine framework to create 
the following LED FSM:

.. figure:: /images/fsm/led_fsm.svg
  :width: 600
  :align: center

  LED FSM Example

When a button is pressed the LED's state changes from OFF to ON or from ON to OFF.
The application can also request the LED to turn on or off by dispatching the 
LED_ON_REQUEST_EVENT and LED_OFF_REQUEST_EVENT respectively.

This LED FSM is then ported to two separate applications running on different hardware 
targets. The first on a microcontroller in ``main.c`` and the second on a computer in ``tests.c``. 
For conciseness, run-time checks (NULL assertions, etc) are not done.

This example also shows how an event-driven state machine is far superior 
to the polled state machine shown in the :ref:`Event-Driven Paradigm Section <fsm_event_driven_paradigm>`.
It aims to demonstate:

#. **How an object-oriented and event-driven state machine combined with**
   `dependency injection <https://en.wikipedia.org/wiki/Dependency_injection>`_ **fully 
   decouples it from the application.**

#. How the FSM is fully reusable. Porting it across multiple applications 
   running on different targets is trivial.

#. How representing the FSM as an **object** allows multiple instances of the same 
   FSM to be created that operate **independently from one another**.

#. How a decoupled FSM is easy to test. Test code simply creates an event, dispatches 
   it to the state machine, and verifies its output.

#. How representing states as functions eliminates the use of flags and eliminates spaghetti code.

#. How each state **only requires one instance**, which can be shared across
   any number of FSM instances.

#. Proper use of this framework and how it abstracts away state transition details.


led.h/.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
These files create the LED FSM described above by using this framework.
The main takeaways are:

+ This module is fully reusable across any application running on any target.

- Representing states as functions eliminates the need for flags.

+ Any number of LED FSM instances can be created that operate independently from one another.

- Each state requires only one instance, which can be shared across any number 
  of LED FSM instances.

.. code-block:: c

    /*-------------------------- led.h --------------------------*/
    #include "ecu/fsm.h"

    /*---------------------- LED FSM Object ---------------------*/
    struct led
    {
        /* Inherit base fsm class. */
        struct ecu_fsm fsm;

        /* Members specific to led fsm. */
        struct
        {
            /* Use dependency injection to handle hardware-specific code */
            void (*turn_on)(void *obj);     /* User-defined function that turns LED off. */
            void (*turn_off)(void *obj);    /* User-defined function that turns LED on. */
            void *obj;                      /* Optional object to pass to turn_led_off and turn_led_on. */
        } api;
    };

    enum led_event_id
    {
        LED_BUTTON_PRESS_EVENT,
        LED_ON_REQUEST_EVENT,
        LED_OFF_REQUEST_EVENT
    };
    
    struct led_event
    {
        /* Wrap event ID in a struct in case more members have to be added in the future. */
        enum led_event_id id;
    };

    /*------------------ LED Member Functions -------------------*/
    extern void led_ctor(struct led *me,
                         void (*turn_on)(void *obj),
                         void (*turn_off)(void *obj),
                         void *obj);

    /* Wrappers can be created to enforce a specific event type. */
    extern void led_dispatch(struct led *me, const struct led_event *event);

.. code-block:: c

    /*-------------------------- led.c --------------------------*/
    #include "led.h"
    #include "ecu/asserter.h"

    /* Assert at compile-time ecu_fsm was properly inherited. */
    ECU_STATIC_ASSERT( (ECU_FSM_IS_BASEOF(fsm, struct led)), "led must inherit ecu_fsm." ); 

    /*--------------------- Create LED states -------------------*/
    static void led_on_entry(struct led *me)
    {
        (*me->api.turn_on)(me->api.obj);
    }

    static void led_on_handler(struct led *me, const void *event)
    {
        const struct led_event *e = (const struct led_event *)event;

        switch (e->id)
        {
            case LED_OFF_REQUEST_EVENT:
            {
                ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_OFF_STATE);
                break;
            }

            case LED_BUTTON_PRESS_EVENT:
            {
                ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_OFF_STATE);
                break;
            }

            default:
            {
                /* Notice how LED_ON_REQUEST_EVENT is ignored since we are 
                already in the ON state. Ignore this and any other events. */
                break;
            }
        }
    }

    static void led_off_entry(struct led *me)
    {
        (*me->api.turn_off)(me->api.obj);
    }

    static void led_off_handler(struct led *me, const void *event)
    {
        const struct led_event *e = (const struct led_event *)event;

        switch (e->id)
        {
            case LED_ON_REQUEST_EVENT:
            {
                ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_ON_STATE);
                break;
            }

            case LED_BUTTON_PRESS_EVENT:
            {
                ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &LED_ON_STATE);
                break;
            }
            
            default:
            {
                /* Notice how LED_OFF_REQUEST_EVENT is ignored since we are 
                already in the OFF state. Ignore this and any other events. */
                break;
            }
        }
    }

    /* Notice how only one instance has to be created per state. Notice how
    a single state instance can be shared across any number of LED fsms. */
    static const struct ecu_fsm_state LED_ON_STATE = ECU_FSM_STATE_CTOR(
        &led_on_entry, ECU_FSM_STATE_EXIT_UNUSED, &led_on_handler
    );

    static const struct ecu_fsm_state LED_OFF_STATE = ECU_FSM_STATE_CTOR(
        &led_off_entry, ECU_FSM_STATE_EXIT_UNUSED, &led_off_handler
    );

    /*------------------ LED Member Functions -------------------*/
    void led_ctor(struct led *me,
                  void (*turn_on)(void *obj),
                  void (*turn_off)(void *obj),
                  void *obj)
    {
        ecu_fsm_ctor(ECU_FSM_BASE_CAST(me), &LED_OFF_STATE);
        me->api.turn_on = turn_on;
        me->api.turn_off = turn_off;
        me->api.obj = obj;
    }

    void led_dispatch(struct led *me, const struct led_event *event)
    {
        /* Notice how this wrapper can be used to enforce a specific event type. */
        ecu_fsm_dispatch(ECU_FSM_BASE_CAST(me), event);
    }


main.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This file runs the LED FSM defined in ``led.h`` and ``led.c`` on a microcontroller.
The main takeaways are:

+ Any number of LED FSM instances can be created that operate independently from one another.

- An object-oriented and event-driven approach fully decouples every LED FSM from the application. 
  Events can be blindly dispatched without any concern for each FSM's internal details. No 
  communication flags are used and the LED state does not have to be tracked.

.. code-block:: c

    /*-------------------------- main.c --------------------------*/
    #include "led.h" /* Reusable FSM we created. */

    /* Notice how hardware-specific code is fully decoupled through dependency injection. */
    struct led_gpio
    {
        uint32_t port;
        uint32_t pin;
    };

    static void turn_led_on(void *obj)
    {
        struct led_gpio *gpio = (struct led_gpio *)obj;
        HardwareGpioPinWriteHigh(gpio->port, gpio->pin);
    }

    static void turn_led_off(void *obj)
    {
        struct led_gpio *gpio = (struct led_gpio *)obj;
        HardwareGpioPinWriteLow(gpio->port, gpio->pin);
    }

    static struct led led1;
    static struct led led2;
    static struct led led3;
    static struct led_gpio led1_gpio = {GPIO_PORTA, 15};
    static struct led_gpio led2_gpio = {GPIO_PORTD, 3};
    static struct led_gpio led3_gpio = {GPIO_PORTC, 8};
    static const struct led_event BUTTON_PRESS_EVENT = {LED_BUTTON_PRESS_EVENT};
    static const struct led_event ON_REQUEST_EVENT = {LED_ON_REQUEST_EVENT};
    static const struct led_event OFF_REQUEST_EVENT = {LED_OFF_REQUEST_EVENT};

    int main()
    {
        /* Notice how hardware-specific code is fully decoupled through dependency injection. */
        led_ctor(&led1, &turn_led_on, &turn_led_off, &led1_gpio);
        led_ctor(&led2, &turn_led_on, &turn_led_off, &led2_gpio);
        led_ctor(&led3, &turn_led_on, &turn_led_off, &led3_gpio);

        /* Pseudocode. Notice how the application blindly dispatches events to the
        state machine. It does not care about its internal details. Also notice how
        multiple instances of the same FSM can be created and they operate independently
        from one another. */
        while (1)
        {
            if (button1 pressed)
            {
                led_dispatch(&led1, &BUTTON_PRESS_EVENT);
            }
            else if (led1 on requested)
            {
                led_dispatch(&led1, &ON_REQUEST_EVENT);
            }
            else if (led1 off requested)
            {
                led_dispatch(&led1, &OFF_REQUEST_EVENT);
            }

            if (button2 pressed)
            {
                led_dispatch(&led2, &BUTTON_PRESS_EVENT);
            }
            else if (led2 on requested)
            {
                led_dispatch(&led2, &ON_REQUEST_EVENT);
            }

            //.......
        }
    }


tests.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This file is a completely separate application running on a computer that also uses 
the same LED FSM defined in ``led.h`` and ``led.c``. The main takeaways are:

+ Any number of LED FSM instances can be created that operate independently from one another.

- An object-oriented and event-driven approach fully decouples every LED FSM from the application. 
  It makes ``led.h`` and ``led.c`` a fully reusable module. Porting it between ``main.c`` and ``tests.c``
  is trivial.

+ Since the FSM is fully decoupled, it can easily be tested. Test code 
  just dispatches events to the FSM and verifies its output.

- Since the FSM is represented as an independent object, testing one 
  object instance verifies all object instances will also work.

.. code-block:: c

    /*-------------------------- tests.c -------------------------*/
    #include "led.h" /* Reusable FSM module under test. */

    static void turn_led_on(void *obj)
    {
        printf("LED turned on!");
    }

    static void turn_led_off(void *obj)
    {
        printf("LED turned off!");
    }

    /*------------------ Create led events for testing -----------*/
    static const struct led_event BUTTON_PRESS_EVENT = {LED_BUTTON_PRESS_EVENT};
    static const struct led_event ON_REQUEST_EVENT = {LED_ON_REQUEST_EVENT};
    static const struct led_event OFF_REQUEST_EVENT = {LED_OFF_REQUEST_EVENT};

    /*---------------------------- Tests -------------------------*/
    TEST()
    {
        struct led TEST_LED;
        led_ctor(&TEST_LED, &turn_led_on, &turn_led_off, NULL);

        ecu_fsm_start(ECU_FSM_BASE_CAST(&TEST_LED));    /* "LED turned off!" */
        led_dispatch(&TEST_LED, &ON_REQUEST_EVENT);     /* "LED turned on!" */
        led_dispatch(&TEST_LED, &ON_REQUEST_EVENT);     /* Nothing. */
        led_dispatch(&TEST_LED, &BUTTON_PRESS_EVENT);   /* "LED turned off! */
        led_dispatch(&TEST_LED, &BUTTON_PRESS_EVENT);   /* "LED turned on! */
        led_dispatch(&TEST_LED, &OFF_REQUEST_EVENT);    /* "LED turned off! */
        led_dispatch(&TEST_LED, &OFF_REQUEST_EVENT);    /* Nothing. */
        led_dispatch(&TEST_LED, &BUTTON_PRESS_EVENT);   /* "LED turned on! */
    }


ecu_fsm_state
=================================================
This is an object that represents a single state within the user's state machine.
It is recommended to read the :ref:`State Representation Section <fsm_state_representation>` 
before using this API, as it explains the :ecudoxygen:`ecu_fsm_state` object in detail.


Constructors
-------------------------------------------------


ECU_FSM_STATE_CTOR()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Creates an :ecudoxygen:`ecu_fsm_state` at compile-time. If a state's entry 
or exit handler are unused, :ecudoxygen:`ECU_FSM_STATE_ENTRY_UNUSED` and 
:ecudoxygen:`ECU_FSM_STATE_EXIT_UNUSED` can be supplied. The details of this 
macro are fully explained in the :ref:`State Representation Section <fsm_state_representation>`:

.. code-block:: c 

    static void state1_entry(struct app_fsm *me);
    static void state1_exit(struct app_fsm *me);
    static void state1_handler(struct app_fsm *me, const void *event);
    static void state2_handler(struct app_fsm *me, const void *event);

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        &state1_entry, &state1_exit, &state1_handler
    );

    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(
        ECU_FSM_STATE_ENTRY_UNUSED, ECU_FSM_STATE_EXIT_UNUSED, &state2_handler
    );


ecu_fsm
=================================================
This is an object that represents the base finite state machine (FSM) class. 
Users create their own FSMs by inheriting this base class and passing it to 
this API by upcasting. It is recommended to read the
:ref:`State Machine Representation Section <fsm_state_machine_representation>` 
before using this API, as it explains this inheritance scheme in detail.


Constructors
-------------------------------------------------


ecu_fsm_ctor()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _fsm_ecu_fsm_ctor:

Sets the starting state the FSM should be in.

.. warning:: 

    Must be called once on startup before the FSM is used. User is also responsible 
    for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct app_fsm fsm;  /* Must inherit ecu_fsm. User must allocate memory before constructor. */
    ecu_fsm_start(ECU_FSM_BASE_CAST(&fsm)); /* ILLEGAL. Must construct before using. */

    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&fsm), &INIT_STATE);
    ecu_fsm_start(ECU_FSM_BASE_CAST(&fsm)); /* Ok. */


Member Functions
-------------------------------------------------


ECU_FSM_IS_BASEOF()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Verifies, at compile-time, that a user's FSM correctly inherits 
the :ecudoxygen:`ecu_fsm` base class:

.. code-block:: c 

    struct correct_fsm
    {
        struct ecu_fsm fsm; /* Inherit by declaring ecu_fsm object as first member. */
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
  

ECU_FSM_BASE_CAST()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Upcasts a user's derived FSM type back into the :ecudoxygen:`ecu_fsm` 
base class. This macro encapsulates the cast and allows derived FSMs 
to be passed into base class functions defined in this framework.

.. code-block:: c 

    struct app_fsm
    {
        /* Inherit by declaring ecu_fsm object as first member. */
        struct ecu_fsm fsm;

        /* Additional members unique to application-specific fsm. */
        uint8_t counter1;
        uint8_t counter2;
    };

    struct app_fsm fsm;
    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&fsm), &INIT_STATE);
    ecu_fsm_start(ECU_FSM_BASE_CAST(&fsm));
    // ...


ecu_fsm_start()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Runs the entry handler of the starting state supplied to :ref:`ecu_fsm_ctor() <fsm_ecu_fsm_ctor>`
and handles all state transitions. The :ref:`State Transitions Section <fsm_state_transitions>`
fully explains how this framework handles transitions.

.. code-block:: c 

    static void state1_entry(struct app_fsm *me)
    {
        printf("S1 entered!");
        ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE2);
    }

    static void state1_exit(struct app_fsm *me)
    {
        printf("S1 exited!");
    }

    static void state1_handler(struct app_fsm *me, const void *event)
    {
        printf("S1 handled!");
    }

    static void state2_entry(struct app_fsm *me)
    {
        printf("S2 entered!");
    }

    static void state2_exit(struct app_fsm *me)
    {
        printf("S2 exited!");
    }

    static void state2_handler(struct app_fsm *me, const void *event)
    {
        printf("S2 handled!");
    }

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        &state1_entry, &state1_exit, &state1_handler
    );

    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(
        &state2_entry, &state2_exit, &state2_handler
    );

    struct app_fsm fsm; /* Must inherit ecu_fsm. */
    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&fsm), &STATE1);
    ecu_fsm_start(ECU_FSM_BASE_CAST(&fsm));

ecu_fsm_start() causes the following code execution order:

.. code-block:: text 

    "S1 entered!" -> "S1 exited!" -> "S2 entered!"

Once complete, the FSM will now be in STATE2.


ecu_fsm_change_state()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _fsm_ecu_fsm_change_state:

Signals that the FSM must transition to a new state or perform a self-transition.
The :ref:`State Transitions Section <fsm_state_transitions>` explains this in detail.

.. warning:: 

    Can only be called within a state's entry and handler functions.
    Using this in a state's exit function results in an error. Using this in
    the external application is undefined behavior.


ecu_fsm_dispatch()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Relays an event to the FSM where it is processed by the current state's 
handler function. All state transitions signaled via :ref:`ecu_fsm_change_state() <fsm_ecu_fsm_change_state>` 
are also handled. The :ref:`State Transitions Section <fsm_state_transitions>`
fully explains how this framework handles transitions. It is also recommended
to read the :ref:`Event Driven Paradigm Section <fsm_event_driven_paradigm>`.

.. code-block:: c 

    static void state1_entry(struct app_fsm *me)
    {
        printf("S1 entered!");
    }

    static void state1_exit(struct app_fsm *me)
    {
        printf("S1 exited!");
    }

    static void state1_handler(struct app_fsm *me, const void *event)
    {
        printf("S1 handled!");
        if (event causes transition to STATE2)
        {
            ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &STATE2);
        }
    }

    static void state2_entry(struct app_fsm *me)
    {
        printf("S2 entered!");
    }

    static void state2_exit(struct app_fsm *me)
    {
        printf("S2 exited!");
    }

    static void state2_handler(struct app_fsm *me, const void *event)
    {
        printf("S2 handled!");
    }

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(
        &state1_entry, &state1_exit, &state1_handler
    );

    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(
        &state2_entry, &state2_exit, &state2_handler
    );

    struct app_fsm fsm; /* Must inherit ecu_fsm. */
    ecu_fsm_ctor(ECU_FSM_BASE_CAST(&fsm), &STATE1);
    ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&fsm), &event_that_causes_transition_to_STATE2);

ecu_fsm_dispatch() causes the following code execution order:

.. code-block:: text 

    "S1 handled!" -> "S1 exited!" -> "S2 entered!"

Once complete, the FSM will now be in STATE2.


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/fsm_8h>
