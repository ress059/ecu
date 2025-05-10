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

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(&state1_on_entry, 
                                                                  &state1_on_exit, 
                                                                  &state1_handler);

**Notice how state functions take in a pointer to the user's FSM type but** 
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

It is recommended to use this macro for better encapsulation as it contains 
all upcasts and protects the application from any changes to the 
:ecudoxygen:`ecu_fsm_state` struct.

:ecudoxygen:`ECU_FSM_STATE_ENTRY_UNUSED` and :ecudoxygen:`ECU_FSM_STATE_EXIT_UNUSED` can be supplied 
if entry() and exit() functions are unused. In this case, only the mandatory handler() function 
will run for this state:

.. code-block:: c 

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(ECU_FSM_STATE_ENTRY_UNUSED, 
                                                                  ECU_FSM_STATE_EXIT_UNUSED, 
                                                                  &state1_handler);


State Transitions
-------------------------------------------------
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

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(&state1_on_entry,
                                                                  &state1_on_exit,
                                                                  &state1_handler);
    static const struct ecu_fsm_state STATE2 = ECU_FSM_STATE_CTOR(&state2_on_entry,
                                                                  &state2_on_exit,
                                                                  &state2_handler);
    static const struct ecu_fsm_state STATE3 = ECU_FSM_STATE_CTOR(&state3_on_entry,
                                                                  &state3_on_exit,
                                                                  &state3_handler);

In this case, "state1 handler!", "state1 exited!", "state2 entered!", "state2 exited!",
"state3 entered!" is printed to the console in order. The FSM is now in STATE3 so if an 
event is dispatched via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`, it will be 
processed in state3_handler().

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

    static const struct ecu_fsm_state STATE1 = ECU_FSM_STATE_CTOR(&state1_on_entry,
                                                                  &state1_on_exit,
                                                                  &state1_handler);

In this case, "state1 handler!", "state1 exited!", "state1 entered!" is printed 
to the console in order. The FSM is still in STATE1 so if an event is dispatched 
via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`, it will be processed in state1_handler().


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
forms of communication between the two are limited to event dispatching. In other words,
the only common interface between the two are events:

.. code-block:: c

    /*--------------------- app_fsm.h ------------------*/
    struct app_fsm
    {
        struct ecu_fsm fsm;
        uint8_t max_time;
        uint8_t elapsed_time;
    };

    enum app_fsm_event_id
    {
        BUTTON_PRESS_EVENT,
        TIMER_TICK_EVENT
    };

    struct app_fsm_event
    {
        enum app_fsm_event_id id;
        uint8_t elapsed_time;
    };

    /*--------------------- app_fsm.c ------------------*/
    static void on_state_entry(struct app_fsm *me)
    {
        me->elapsed_time = 0;
    }

    static void on_state_handler(struct app_fsm *me, const void *event)
    {
        const struct app_fsm_event *e = (const struct app_fsm_event *)event;
        switch (e->id)
        {
            case BUTTON_PRESS_EVENT:
            {
                if (me->elapsed_time >= me->max_time)
                {
                    ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &OFF_STATE);
                }
                break;
            }

            case TIMER_TICK_EVENT:
            {
                me->elapsed_time += e->elapsed_time;
                if (me->elapsed_time >= me->max_time)
                {
                    ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &OFF_STATE);
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }

    static void off_state_entry(struct app_fsm *me)
    {
        me->elapsed_time = 0;
    }

    static void off_state_handler(struct app_fsm *me, const void *event)
    {
        const struct app_fsm_event *e = (const struct app_fsm_event *)event;
        switch (e->id)
        {
            case BUTTON_PRESS_EVENT:
            {
                if (me->elapsed_time >= me->max_time)
                {
                    ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &ON_STATE);
                }
                break;
            }

            case TIMER_TICK_EVENT:
            {
                me->elapsed_time += e->elapsed_time;
                if (me->elapsed_time >= me->max_time)
                {
                    ecu_fsm_change_state(ECU_FSM_BASE_CAST(me), &ON_STATE);
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }

    static const struct ecu_fsm_state ON_STATE = ECU_FSM_STATE_CTOR(&on_state_entry,
                                                                    ECU_FSM_STATE_EXIT_UNUSED,
                                                                    &on_state_handler);
    static const struct ecu_fsm_state OFF_STATE = ECU_FSM_STATE_CTOR(&off_state_entry,
                                                                     ECU_FSM_STATE_EXIT_UNUSED,
                                                                     &off_state_handler);

    /*------------- Application 1. main.c --------------*/
    /* Reusable state machine implementation. */
    #include "app_fsm.h"
    struct app_fsm APP_FSM;

    /* Event dispatching is the common interface between the 
    FSM implementation (app_fsm.h/.c) and the application. */
    struct app_fsm_event event = {...};
    ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&APP_FSM), &event);


    /*------------- Application 2. main.c --------------*/
    /* Reusable state machine implementation. */
    #include "app_fsm.h"
    struct app_fsm APP_FSM;

    /* Event dispatching is the common interface between the 
    FSM implementation (app_fsm.h/.c) and the application. */
    struct app_fsm_event event = {...};
    ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&APP_FSM), &event);

Notable advantages an event-driven approach provides are:

#. The FSM implementation is fully reusable. The code snippet above shows no code changes 
   in app_fsm.h/.c are required in order to use it across different applications.

#. Decoupling makes code far easier to test. Test code simply creates an event, dispatches 
   it to the state machine, and verifies how it reacts.

#. Decoupling makes thread-safety trivial. Events are **not** a shared resource since they 
   are represented as an **object**. Each thread can create their own events. The only shared 
   resource is the FSM object instance which can be delegated to a separate task with an 
   event queue. This is discussed in the :ref:`Run to Completion Semantics Section <fsm_run_to_completion_semantics>`

#. Less CPU overhead. The state machine only performs work when it needs to.

#. Representing the state machine as an **object** allows multiple instances of the same 
   FSM to be created that operate **independently from one another**.

Compare this to the traditional polling state machine that is unfortunately most commonly used.
Communication between the application is facilitated through global flags:

.. code-block:: c 

    /*----------------- polled.h ---------------*/
    #define MAX_TIME    (50)
    extern bool button_pressed;
    extern uint8_t elapsed_time;

    /*--------------- polled.h/.c --------------*/
    void polled_fsm_run(void)
    {
        enum state_t{OFF_STATE, ON_STATE};
        static enum state_t state = OFF_STATE;
        static uint8_t time = 0;

        switch (state)
        {
            case ON_STATE:
            {
                time += elapsed_time;

                if (button_pressed && time >= MAX_TIME)
                {
                    time = 0;
                    state = OFF_STATE;
                }
                break;
            }

            case OFF_STATE:
            {
                time += elapsed_time;

                if (button_pressed && time >= MAX_TIME)
                {
                    time = 0;
                    state = ON_STATE;
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }

    /*------------- application main.c -------------*/
    /* The polled FSM and application communicate through global flags. */
    bool button_pressed = false;
    uint8_t elapsed_time = 0;

    while (1)
    {
        button_pressed = button_read();
        elapsed_time = get_elapsed_time();
        polled_fsm_run();
    }

Polling state machines are anti-patterns because:

#. They are tightly coupled to the application. Each global flag must be properly initialized
   and handled by every application that wants to use the polled FSM.

#. The state machine is very difficult to test due to the reasonings in the previous point.

#. Thread-safety is **not** trivial since every single global flag and the FSM are shared
   resources.

#. The FSM has to repeatedly run, even when no processing is required most of the time.

#. The state machine is essentially a shared and editable singleton, which is an anti-pattern.


Run to Completion Semantics
-------------------------------------------------
.. _fsm_run_to_completion_semantics:

As discussed in the :ref:`previous section <fsm_event_driven_paradigm>`, event-driven state 
machines make thread-safety trivial as they naturally decouple themselves from the application.
The only shared resource is the FSM object instance, which **must run to completion** to ensure
thread safety.

When an event is dispatched, the FSM must finish processing it before the next one
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

    /*------------------- app_fsm_task.c -----------------*/
    /* Reusable state machine implementation. */
    #include "app_fsm.h"
    struct rtos_queue event_queue; /* The event queue is the only shared resource. */

    /* RTOS task. */
    static void app_fsm_task(..)
    {
        /* Create state machine object instance that will run in this task. */
        struct app_fsm APP_FSM;
        
        while (1)
        {
            if (!queue_not_empty(&event_queue))
            {
                event = queue_read(&event_queue);
                ecu_fsm_dispatch(ECU_FSM_BASE_CAST(&APP_FSM), &event);
            }
        }
    }

    /*------------------------ main.c --------------------*/
    queue_write(&event_queue, &event1);
    queue_write(&event_queue, &event2);
    queue_write(&event_queue, &event3);

The application posts events to the task's queue instead of directly dispatching them.
The only shared resource is the event queue, which is provided by the RTOS. Therefore 
all reads and writes to it are guaranteed to be thread safe.

Run to completion is always satisfied because the app_fsm_task() cannot pre-empt itself. 
Therefore all calls to :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>` are
guaranteed to finish.


Example
-------------------------------------------------
The following in-depth example uses this state machine framework to create 
the following LED FSM. This LED FSM is then ran on two separate applications 
and hardware targets. The first on a microcontroller in main.c and the second 
on a computer in tests.c. For conciseness, run-time checks (NULL assertions, etc) are not done:

.. figure:: /images/fsm/led.svg
  :width: 600
  :align: center

  LED FSM Example

When a button is pressed the LED's state changes from OFF to ON or from ON to OFF.
The application can also request the LED to turn on or off by dispatching the 
LED_ON_REQUEST_EVENT and LED_OFF_REQUEST_EVENT respectively.

This example aims to demonstrate the following points:
#. Proper use of this framework.

#. How this framework handles all transitions by automatically executing the proper 
   entry(), exit(), and handler() functions for each state.

#. How representing states as functions reduces the use of flags and eliminates spaghetti code.

#. How each state **only requires one instance**. This single instance can be shared across
   any number of FSM instances.

#. How representing the FSM as an **object** allows multiple instances of the same 
   FSM to be created that operate **independently from one another**.

#. How an event-driven approach combined with [dependency injection](https://en.wikipedia.org/wiki/Dependency_injection) 
   fully decouples the FSM from the application. The application can blindly 
   dispatch events to the LED FSM without worrying about its internal details.


led.h/.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
These files create the LED FSM described above by using this framework.
Points 1 to 4 are primarily showcases here.

.. code-block:: c

    /*-------------------------- led.h --------------------------*/
    #include "ecu/fsm.h"

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

    /* Wrap event ID in a struct in case more members have to be added in the future. */
    struct led_event
    {
        enum led_event_id id;
    };

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
    ECU_STATIC_ASSERT( (ECU_FSM_IS_BASEOF(fsm, struct led)), "LED must inherit ecu_fsm." ); 

    /* Create LED states. */
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
                /* Ignore all other events. */
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
                /* Ignore all other events. */
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
        ecu_fsm_dispatch(ECU_FSM_BASE_CAST(me), event);
    }


main.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This file runs the LED FSM defined in defined in led.h/.c on a microcontroller.
It primarily showcases points 5 and 6.

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

!!!!!!!!!!!!!!!!!!!!!!!!!!!! TODO Stopped here !!!!!
tests.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This file runs the state machine defined in led.h/.c on a computer. It demonstrates how an
event-driven state machine combined with dependency injection fully decouples led.h/.c from
the application. Notice how <b>no code has to be changed in led.h/.c in order to use it in
a different application and run it on a different target (in this case a computer).</b>


```C
/*-------------------------- tests.c ------------------------*/
#include "led.h" /* Code under test. */

static void turn_led_on(void *obj)
{
    (void)obj;
    printf("LED turned on!");
}

static void turn_led_off(void *obj)
{
    (void)obj;
    printf("LED turned off!");
}


/*------------------ Create led events for testing ----------*/
static const led_event BUTTON_PRESS_EVENT = {LED_BUTTON_PRESS_EVENT};
static const led_event ON_REQUEST_EVENT = {LED_ON_REQUEST_EVENT};
static const led_event OFF_REQUEST_EVENT = {LED_OFF_REQUEST_EVENT};


/*------------------------------ Tests -------------------------*/
TEST()
{
    led TEST_LED;
    ledFsmCtor(&TEST_LED, 3, &turn_led_on, &turn_led_off, NULL); /* Threshold == 3. */

    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT);
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT);
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT); /* "LED turned on!" */

    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT);
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT);
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT); /* "LED turned off!" */

    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT);
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT);
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &BUTTON_PRESS_EVENT); /* "LED turned on!" */
}

TEST()
{
    led TEST_LED;
    ledFsmCtor(&TEST_LED, 3, &turn_led_on, &turn_led_off, NULL);

    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &LED_ON_REQUEST_EVENT);   /* "LED turned on!" */
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &LED_OFF_REQUEST_EVENT);  /* "LED turned off!" */
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &LED_ON_REQUEST_EVENT);   /* "LED turned on!" */
    fsmDispatch(FSM_BASE_CAST(&TEST_LED), &LED_OFF_REQUEST_EVENT);  /* "LED turned off!" */
}
```


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
