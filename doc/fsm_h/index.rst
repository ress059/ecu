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

Framework that creates and runs finite state machines (FSMs). Any user-defined 
FSM can use this framework by containing an intrusive :ecudoxygen:`ecu_fsm` member.


Theory
=================================================

.. note:: 

    It is recommended to read all sections as they build off of one another.

State Representation
-------------------------------------------------
.. _fsm_state_representation:

States are represented by the :ecudoxygen:`ecu_fsm_state` struct. It contains
a set of handler functions that the user defines to describe the state's behavior:

.. code-block:: c 

    struct ecu_fsm_state
    {
        void (*const entry)(struct ecu_fsm *fsm);
        void (*const exit)(struct ecu_fsm *fsm);
        void (*const handler)(struct ecu_fsm *fsm, const void *event);
    };

- **entry()** is an optional function that executes when the state is first entered.
  Set to :ecudoxygen:`ECU_FSM_STATE_ENTRY_UNUSED` if unused.
- **exit()** is an optional function that executes when the state is exited.
  Set to :ecudoxygen:`ECU_FSM_STATE_EXIT_UNUSED` if unused.
- **handler()** is a mandatory function that executes when the FSM is running in this state.

**The contents are const-qualified, forcing every state to be created at compile-time.**
This can be done through the :ecudoxygen:`ECU_FSM_STATE_CTOR() <ECU_FSM_STATE_CTOR>` macro:

.. code-block:: c

    /* Defined by user. */
    static void running_state_on_entry(struct ecu_fsm *fsm);
    static void running_state_on_exit(struct ecu_fsm *fsm);
    static void running_state_handler(struct ecu_fsm *fsm, const void *event);

    static const struct ecu_fsm_state RUNNING_STATE = ECU_FSM_STATE_CTOR(
        &running_state_on_entry, &running_state_on_exit, &running_state_handler
    );

Or if the option entry and exit handlers are unused:

.. code-block:: c

    /* Defined by user. */
    static void running_state_handler(struct ecu_fsm *fsm, const void *event);

    static const struct ecu_fsm_state RUNNING_STATE = ECU_FSM_STATE_CTOR(
        ECU_FSM_STATE_ENTRY_UNUSED, ECU_FSM_STATE_EXIT_UNUSED, &running_state_handler
    );

Representing states as objects allows multiple instances of the same FSM to use
the same states. No additional memory or overhead is required:

.. figure:: /images/fsm/state_representation_state_reuse.svg
    :width: 500
    :align: center

    State Reuse

State Machine Representation
-------------------------------------------------
.. _fsm_state_machine_representation:

FSMs are represented by the :ecudoxygen:`ecu_fsm` struct. Application-specific FSMs
use this framework by containing :ecudoxygen:`ecu_fsm` as an intrusive member:

.. code-block:: c

    struct app_fsm
    {
        int app_data1;
        struct ecu_fsm fsm_member;
        int app_data2;
    };

This type is retrieved within the FSM's definition of each state via the
:ecudoxygen:`ECU_FSM_GET_CONTEXT() <ECU_FSM_GET_CONTEXT>` macro:

.. code-block:: c

    static void running_state_on_entry(struct ecu_fsm *fsm)
    {
        struct app_fsm *me = ECU_FSM_GET_CONTEXT(fsm, struct app_fsm, fsm_member);
        me->app_data1 = 0;
        me->app_data2 = 0;
    }

    static void running_state_handler(struct ecu_fsm *fsm, const void *event)
    {
        struct app_fsm *me = ECU_FSM_GET_CONTEXT(fsm, struct app_fsm, fsm_member);

        if (event == relevant)
        {
            do_work(me);
        }
    }

This allows the framework to interact with the application through a common interface
(the :ecudoxygen:`ecu_fsm` struct), without inheritance. The macro takes in
three parameters:

    #. ``ecu_fsm_ptr_`` = Pointer to intrusive ecu_fsm member. In this case, ``fsm``.
    #. ``type_`` = User's FSM type. In this case, ``struct app_fsm``.
    #. ``member_`` = Name of ecu_fsm member within the user's type. In this case, ``fsm_member``.

.. figure:: /images/fsm/ecu_fsm_get_context.svg
    :width: 500
    :align: center
  
    ECU_FSM_GET_CONTEXT()

Under the hood, this macro performs pointer arithmetic to do the conversion.
The details of this operation are fully explained in 
:ref:`ECU_CONTAINER_OF() <utils_container_of>`.

State Transitions
-------------------------------------------------

The FSM's definition calls :ecudoxygen:`ecu_fsm_change_state() <ecu_fsm_change_state>` to signal
a state transition:

.. code-block:: c

    static void running_state_handler(struct ecu_fsm *fsm, const void *event)
    {
        if (event == causes state transition)
        {
            ecu_fsm_change_state(fsm, &new_state_to_transition_into);
        }
    }

If used, the current state's exit handler is ran then the new state's entry handler is ran.
The following example transitions the FSM from the RUNNING_STATE to the STOPPED_STATE when
a STOP_EVENT is received. This is a **single state transition**:

.. figure:: /images/fsm/state_transitions_single_state_transition.svg
    :width: 500
    :align: center
  
    Single State Transition

.. code-block:: c

    static void running_state_handler(struct ecu_fsm *fsm, const void *event)
    {
        if (event == STOP_EVENT)
        {
            ecu_fsm_change_state(fsm, &STOPPED_STATE);
        }
    }

The full execution order is:

#. ecu_fsm_dispatch(fsm, STOP_EVENT)
#. RUNNING_STATE::handler()
#. RUNNING_STATE::exit()
#. STOPPED_STATE::entry()

.. warning:: 

    No state transitions are allowed in the exit handler. This is pointless since
    when the exit handler runs, that state is already being exited:

    .. figure:: /images/fsm/state_transitions_transition_on_exit.svg
        :width: 500
        :align: center
    
        Transition on Exit Not Allowed

    .. code-block:: c

        static void state1_on_exit(struct ecu_fsm *fsm)
        {
            ecu_fsm_change_state(fsm, &STATE2); /* NOT ALLOWED! */
        }

A **Consecutive state transition** can occur by calling :ecudoxygen:`ecu_fsm_change_state() <ecu_fsm_change_state>`
in a state's entry handler. The following example transitions the FSM from the
STOPPED_STATE to the PREOPERATIONAL_STATE then to the OPERATIONAL_STATE when a
START_EVENT is received:

.. figure:: /images/fsm/state_transitions_consecutive_state_transition.svg
    :width: 500
    :align: center
  
    Consecutive State Transition

.. code-block:: c

    static void stopped_state_handler(struct ecu_fsm *fsm, const void *event)
    {
        if (event == START_EVENT)
        {
            ecu_fsm_change_state(fsm, &PREOPERATIONAL_STATE);
        }
    }

    static void preoperational_state_on_entry(struct ecu_fsm *fsm)
    {
        do_preoperational_work(fsm);
        ecu_fsm_change_state(fsm, &OPERATIONAL_STATE);
    }

The full execution order is:

#. ecu_fsm_dispatch(fsm, START_EVENT)
#. STOPPED_STATE::handler()
#. STOPPED_STATE::exit()
#. PREOPERATIONAL_STATE::entry()
#. PREOPERATIONAL_STATE::exit()
#. OPERATIONAL_STATE::entry()

.. warning:: 

    A self-state transition is not allowed in the entry handler as this
    causes an infinite loop:

    .. figure:: /images/fsm/state_transitions_self_transition_on_entry.svg
        :width: 500
        :align: center
    
        Self Transition on Entry Not Allowed

    .. code-block:: c

        static void state1_on_entry(struct ecu_fsm *fsm)
        {
            ecu_fsm_change_state(fsm, &STATE1); /* NOT ALLOWED! */
        }

A **self-state transition** can occur by supplying the current state to
`ecu_fsm_change_state() <ecu_fsm_change_state>` within the state's handler:

.. figure:: /images/fsm/state_transitions_self_transition.svg
    :width: 500
    :align: center

    Self-State Transition

.. code-block:: c 

    static void running_state_handler(struct ecu_fsm *fsm, const void *event)
    {
        if (event == RESET_EVENT)
        {
            /* Current state is RUNNING_STATE. Self-transition. */
            ecu_fsm_change_state(fsm, &RUNNING_STATE); 
        }
    }

The full execution order is:

#. ecu_fsm_dispatch(fsm, RESET_EVENT)
#. RUNNING_STATE::handler()
#. RUNNING_STATE::exit()
#. RUNNING_STATE::entry()

Event-Driven Paradigm
-------------------------------------------------
The application interacts with state machines generated by this framework through **events.**
Events are objects that describe the type of event and contain any relevant data:

.. code-block:: c

    enum event_id
    {
        STOP_EVENT,
        START_EVENT,
        RESET_EVENT
    };

    struct event
    {
        enum event_id id;
        int data;
        int more_data;
    };

When an event occurs, it is sent to the state machine via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`:
The application treats it as a black-box and blindly dispatches events to it: 

.. code-block:: c

    int main()
    {
        if (requested to stop)
        {
            ecu_fsm_dispatch(fsm, &stop_event);
        }
    } 

.. figure:: /images/fsm/event_driven_paradigm_event_driven_state_machine.svg
    :width: 500
    :align: center

    Event-Driven State Machine

This pattern naturally decouples the FSM from the application as all forms of communication 
between the two are limited to event dispatching. Therefore the state machine's implementation 
details are fully encapsulated. 

Compare this to the traditional polling state machine that is unfortunately most
commonly used:

.. figure:: /images/fsm/event_driven_paradigm_polling_state_machine.svg
    :width: 500
    :align: center

    Polling State Machine

Polling state machines tightly couple themselves to the application as the two must
communicate with each other through extensive use of global flags. An event-driven
approach is far superior because the decoupling offers these advantages compared to 
a polling state machine:

#. An event-driven state machine's implementation is fully reusable and encapsulated. 
   Porting the state machine to a new application simply involves dispatching events 
   under different conditions. Applications also remain uneffected if the state machine's 
   internal details were to ever change:

    .. code-block:: c

        /* Application #1. */
        int main()
        {
            if (button pressed 5 times)
            {
                ecu_fsm_dispatch(&fsm, &STOP_EVENT);
            }
        }

    .. code-block:: c

        /* Application #2. */
        int main()
        {
            if (button pressed once)
            {
                ecu_fsm_dispatch(&fsm, &STOP_EVENT);
            }
        }

    A polling state machine's implementation is neither reusable nor encapsulated:

    .. figure:: /images/fsm/event_driven_paradigm_polling_state_machine.svg
        :width: 500
        :align: center

    Polling State Machine

    Offloading the button press logic to the application does not fix the coupling
    since a different global flag must be used instead:

    .. figure:: /images/fsm/event_driven_paradigm_polling_state_machine2.svg
        :width: 500
        :align: center

    Polling State Machine

    Porting a polled state machine to a new application is not trivial because it must
    carefully edit these global flags in a predefined fashion. If the state machine's 
    details change, every application that uses it would have to be refactored.

#. Multiple instances of the same event-driven state machine can be created, with each
   instance operating **independently** from one another:

    .. code-block:: c

        struct led_fsm led1;
        struct led_fsm led2;

        int main()
        {
            ecu_fsm_dispatch(&led1, &ON_EVENT);
            ecu_fsm_dispatch(&led2, &OFF_EVENT);
        }

    .. figure:: /images/fsm/event_driven_paradigm_event_driven_state_machine_multiple_instances.svg
        :width: 500
        :align: center

    Event-Driven State Machine Multiple Instances

    A polling state machine can only have one instance since global communication flags 
    are used:

    .. figure:: /images/fsm/event_driven_paradigm_polling_state_machine_multiple_instances.svg
        :width: 500
        :align: center

    Polling State Machine Multiple Instances

#. An event-driven state machine is far easier to test. Test code simply creates an event, dispatches 
   it to the state machine, and verifies its output. The state machine can be easily reset since multiple
   independent instances can be created:

    .. code-block:: c

        TEST()
        {
            struct fsm test_fsm;

            create(&test_fsm);
            ecu_fsm_dispatch(&test_fsm, &EVENT);
            verify_behavior();
        }

    Polling state machines are extremely difficult to test since it is basically a singleton.
    The state machine's state is also often stored as a static variable in a local function,
    making it impossible to reset without preprocessor directives and condition compilation:

    .. code-block:: c

        static void polled_fsm_implementation(void)
        {
            /* Good luck resetting this. */
            static enum {ON_STATE, OFF_STATE} state;

            switch (state)
            {
                case ON_STATE:
                    /* Good luck resetting global flags and maintaining that in test code. */
                    if (off_flag)
                    {
                        off_flag = false;
                        state = OFF_STATE;
                    }
                    break;

                // ...
            }
        } 


!!!!!!!!!! TODO Stopped here !!!

#. Thread-safety is trivial. Events are **not** shared resources since they 
   are represented as an **object**. Each thread can create their own events. The only shared 
   resource is the FSM object instance which can be delegated to a separate task with an 
   event queue. This is discussed in the :ref:`Run to Completion Semantics Section <fsm_run_to_completion_semantics>`.

#. Less CPU overhead. The state machine only performs work when it needs to.

#. Representing the state machine as an **object** allows multiple instances of the same 
   FSM to be created that operate **independently from one another**.




Disadvantages of the polling state machine are:

Polling state machines like the one above are anti-patterns because:

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














State machines built with this framework are **event-driven**. They only perform work
when an event is dispatched to it via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`:



The application treats the state machine as a black-box and blindly dispatches events
to it. Thus, the state machine's implementation details are fully encapsulated.



State machines built with this framework only perform work when events (button press, value change, etc)
are dispatched via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`. All processing is 
encapsulated within the FSM's implementation. **The application never cares about
the state machine's internal details. It treats it as a black-box and simply dispatches events 
to it.**




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
                ecu_fsm_dispatch(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&APP_FSM), &event);
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






!!! TODO STopped here


run the fsm in a task that is synchronized by an event queue.


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
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &LED_OFF_STATE);
                break;
            }

            case LED_BUTTON_PRESS_EVENT:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &LED_OFF_STATE);
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
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &LED_ON_STATE);
                break;
            }

            case LED_BUTTON_PRESS_EVENT:
            {
                ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &LED_ON_STATE);
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
        ecu_fsm_ctor(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &LED_OFF_STATE);
        me->api.turn_on = turn_on;
        me->api.turn_off = turn_off;
        me->api.obj = obj;
    }

    void led_dispatch(struct led *me, const struct led_event *event)
    {
        /* Notice how this wrapper can be used to enforce a specific event type. */
        ecu_fsm_dispatch(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), event);
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

        ecu_fsm_start(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&TEST_LED));    /* "LED turned off!" */
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
    ecu_fsm_start(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm)); /* ILLEGAL. Must construct before using. */

    ecu_fsm_ctor(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm), &INIT_STATE);
    ecu_fsm_start(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm)); /* Ok. */


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
  

TODO_WAS_FSM_BASE_CAST_BEFORE!!()
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
    ecu_fsm_ctor(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm), &INIT_STATE);
    ecu_fsm_start(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm));
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
        ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &STATE2);
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
    ecu_fsm_ctor(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm), &STATE1);
    ecu_fsm_start(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm));

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
            ecu_fsm_change_state(TODO_WAS_FSM_BASE_CAST_BEFORE!!(me), &STATE2);
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
    ecu_fsm_ctor(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm), &STATE1);
    ecu_fsm_dispatch(TODO_WAS_FSM_BASE_CAST_BEFORE!!(&fsm), &event_that_causes_transition_to_STATE2);

ecu_fsm_dispatch() causes the following code execution order:

.. code-block:: text 

    "S1 handled!" -> "S1 exited!" -> "S2 entered!"

Once complete, the FSM will now be in STATE2.


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/fsm_8h>
