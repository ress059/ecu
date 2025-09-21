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

Framework that creates and runs finite state machines (FSMs). Applications use
this framework by containing an intrusive :ecudoxygen:`ecu_fsm` member.


Theory
=================================================

State Representation
-------------------------------------------------
.. _fsm_state_representation:

States are represented by the :ecudoxygen:`ecu_fsm_state` struct. It contains
a set of handler functions that the user defines to describe the state's behavior.
This framework automatically executes the correct sequence of handler functions while the
state machine is running:

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

The contents are const-qualified, forcing every state to be created at compile-time
via the :ecudoxygen:`ECU_FSM_STATE_CTOR() <ECU_FSM_STATE_CTOR>` macro:

.. code-block:: c

    /* Defined by user. */
    static void running_state_on_entry(struct ecu_fsm *fsm);
    static void running_state_on_exit(struct ecu_fsm *fsm);
    static void running_state_handler(struct ecu_fsm *fsm, const void *event);

    static const struct ecu_fsm_state RUNNING_STATE = ECU_FSM_STATE_CTOR(
        &running_state_on_entry, &running_state_on_exit, &running_state_handler
    );

Or if the optional entry and exit handlers are unused:

.. code-block:: c

    /* Defined by user. */
    static void running_state_handler(struct ecu_fsm *fsm, const void *event);

    static const struct ecu_fsm_state RUNNING_STATE = ECU_FSM_STATE_CTOR(
        ECU_FSM_STATE_ENTRY_UNUSED, ECU_FSM_STATE_EXIT_UNUSED, &running_state_handler
    );

Representing states as objects allows them to be shared between 
multiple instances of the same FSM. No additional memory or overhead is required:

.. figure:: /images/fsm/state_representation_state_reuse.svg
    :width: 400
    :align: center

    State Reuse

State Machine Representation
-------------------------------------------------
.. _fsm_state_machine_representation:

Finite state machines are represented by the :ecudoxygen:`ecu_fsm` struct. 
Applications use this framework by containing :ecudoxygen:`ecu_fsm` as
an intrusive member:

.. code-block:: c

    struct app_fsm
    {
        int app_data1;
        struct ecu_fsm fsm_member;
        int app_data2;
    };

This framework has no knowledge of the application's state machine type so it 
must only use :ecudoxygen:`ecu_fsm` to remain portable. The :ecudoxygen:`ecu_fsm` 
member acts as a common interface between the two mediums. Thus each state
handler must take in an :ecudoxygen:`ecu_fsm` pointer:

.. code-block:: c 

    /* Defined by user. */
    static void running_state_on_entry(struct ecu_fsm *fsm);
    static void running_state_on_exit(struct ecu_fsm *fsm);
    static void running_state_handler(struct ecu_fsm *fsm, const void *event);

The application's state machine type can be retrieved within each handler's definition
via the :ecudoxygen:`ECU_FSM_GET_CONTEXT() <ECU_FSM_GET_CONTEXT>` macro:

.. code-block:: c

    static void running_state_on_entry(struct ecu_fsm *fsm)
    {
        struct app_fsm *me = ECU_FSM_GET_CONTEXT(fsm, struct app_fsm, fsm_member);
        me->app_data1 = 10;
        me->app_data2 = 10;
    }

    static void running_state_on_exit(struct ecu_fsm *fsm)
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
    :width: 450
    :align: center
  
    ECU_FSM_GET_CONTEXT()

Under the hood, this macro does pointer arithmetic to perform the conversion.
The details of this operation are fully explained in 
:ref:`ECU_CONTAINER_OF() <utils_container_of>`.

State Transitions
-------------------------------------------------
.. _fsm_state_transitions:

Within each state's definition, the application can perform a state transition 
by calling :ecudoxygen:`ecu_fsm_change_state() <ecu_fsm_change_state>`:

.. figure:: /images/fsm/state_transitions_single_state_transition.svg
    :width: 450
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

The example above showcases a **single state transition**. When this occurs,
the framework executes the current state's exit handler then the new state's entry handler.
Thus the full execution order is:

#. ecu_fsm_dispatch(fsm, STOP_EVENT)
#. RUNNING_STATE::handler()
#. RUNNING_STATE::exit()
#. STOPPED_STATE::entry()

.. warning:: 

    No state transitions are allowed in the exit handler. This is pointless since
    when the exit handler runs, that state is already being exited:

    .. code-block:: c

        static void state1_on_exit(struct ecu_fsm *fsm)
        {
            ecu_fsm_change_state(fsm, &STATE2); /* NOT ALLOWED! */
        }

A **consecutive state transition** can occur by calling :ecudoxygen:`ecu_fsm_change_state() <ecu_fsm_change_state>`
within a state's entry handler. The following example transitions the state machine from the
STOPPED_STATE to the PREOPERATIONAL_STATE then to the OPERATIONAL_STATE when a
START_EVENT is received:

.. figure:: /images/fsm/state_transitions_consecutive_state_transition.svg
    :width: 700
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

This framework automatically executes the proper state handlers,
making the full execution order:

#. ecu_fsm_dispatch(fsm, START_EVENT)
#. STOPPED_STATE::handler()
#. STOPPED_STATE::exit()
#. PREOPERATIONAL_STATE::entry()
#. PREOPERATIONAL_STATE::exit()
#. OPERATIONAL_STATE::entry()

.. warning:: 

    A self-state transition is not allowed in the entry handler as this
    would cause an infinite loop:

    .. code-block:: c

        static void state1_on_entry(struct ecu_fsm *fsm)
        {
            ecu_fsm_change_state(fsm, &STATE1); /* NOT ALLOWED! */
        }

The final type of transition that can occur is the **self-state transition**.
This is accomplished by supplying the current state to
:ref:`ecu_fsm_change_state() <fsm_ecu_fsm_change_state>` within the state's main
handler function:

.. figure:: /images/fsm/state_transitions_self_transition.svg
    :width: 250
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

This framework automatically executes the proper state handlers,
making the full execution order:

#. ecu_fsm_dispatch(fsm, RESET_EVENT)
#. RUNNING_STATE::handler()
#. RUNNING_STATE::exit()
#. RUNNING_STATE::entry()

Event-Driven Paradigm
-------------------------------------------------
.. _fsm_event_driven_paradigm:

The application interacts with state machines generated by this framework through **events.**
Events are objects that describe what happened and contain any relevant data:

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

When an event occurs, it is sent to the state machine via :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`.
The application treats the state machine as a black-box and blindly dispatches events to it: 

.. code-block:: c

    int main()
    {
        if (requested to stop)
        {
            struct event stop_event = {STOP_EVENT, 0, 0};
            ecu_fsm_dispatch(fsm, &stop_event); /* Don't care what state machine is currently doing. */
        }
    } 

.. figure:: /images/fsm/event_driven_paradigm_event_driven_state_machine.svg
    :width: 350
    :align: center

    Event-Driven State Machine

This pattern naturally decouples the FSM from the application as all forms of communication 
between the two are limited to event dispatching. Therefore the state machine's implementation 
details (the state machine box) are fully encapsulated. 

Compare this to the traditional polling state machine that is unfortunately most
commonly used:

.. figure:: /images/fsm/event_driven_paradigm_polling_state_machine.svg
    :width: 400
    :align: center

    Polling State Machine

Polling state machines tightly couple themselves to the application as the two often
communicate with each other through extensive use of global flags. **The points below
expand upon this and further explain why the event-driven approach is superior**:

#. An event-driven state machine's implementation is fully reusable and encapsulated. 
   Porting the state machine to a new application simply involves dispatching events 
   under different conditions. Applications also remain uneffected if the state machine's 
   internal details were to ever change. At a maximum, the changepoint is limited to 
   creating new event structs:

    .. code-block:: c

        /* Application #1. */
        int main()
        {
            if (button pressed 5 times)
            {
                struct event STOP_EVENT;
                ecu_fsm_dispatch(&fsm, &STOP_EVENT);
            }
        }

    .. code-block:: c

        /* Application #2. */
        int main()
        {
            if (button pressed once)
            {
                struct event STOP_EVENT;
                ecu_fsm_dispatch(&fsm, &STOP_EVENT);
            }
        }

   A polling state machine's implementation is neither reusable nor encapsulated:

    .. figure:: /images/fsm/event_driven_paradigm_polling_state_machine.svg
        :width: 400
        :align: center

        Polling State Machine

   Offloading the button press logic to the application does not fix the coupling.
   It just changes which global flags are used to facilitate communication between the two:

    .. figure:: /images/fsm/event_driven_paradigm_polling_state_machine_different_global_flags.svg
        :width: 400
        :align: center

        Polling State Machine Different Global Flags

   Porting this state machine to a new application is not trivial because it must
   carefully edit these global flags in a predefined fashion. Also if the state machine's 
   details change, every application that uses it would have to be refactored.

#. Multiple instances of the same event-driven state machine can be created since events are
   **not** shared objects. Each state machine instance operates **independently** from one another:

    .. code-block:: c

        struct led_fsm led1;
        struct led_fsm led2;

        int main()
        {
            struct event ON_EVENT1;
            ecu_fsm_dispatch(&led1, &ON_EVENT1);

            struct event ON_EVENT2;
            ecu_fsm_dispatch(&led2, &ON_EVENT2);
        }

    .. figure:: /images/fsm/event_driven_paradigm_event_driven_state_machine_multiple_instances.svg
        :width: 550
        :align: center

        Event-Driven State Machine Multiple Instances

   A polling state machine can only have one instance since global communication flags 
   are used:

    .. figure:: /images/fsm/event_driven_paradigm_polling_state_machine_multiple_instances.svg
        :width: 500
        :align: center

        Polling State Machine Multiple Instances

#. An event-driven state machine is far easier to test. The natural decoupling 
   allows test code to simply create an event, dispatch it to the state machine,
   and verify its output. This encapsulates all implementation details from the test
   code, making it fully portable. The test state machine can also easily be reset by 
   simply creating a new instance:

    .. code-block:: c

        TEST()
        {
            struct fsm test_fsm;
            struct event EVENT;

            create(&test_fsm);
            ecu_fsm_dispatch(&test_fsm, &EVENT);
            verify_output();
        }

   Polling state machines are extremely difficult to test since they are often
   implemented as free functions that are tightly coupled to the application.
   Test code must manually edit flags to reset and fully test the state machine. 
   This is unportable since test code directly interacts with the state machine's implementation
   details. All tests would have to be refactored whenever the state machine's implementation
   changes:

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

        TEST()
        {
            set state == ON_STATE; /* Somehow have to set local flag. */
            off_flag = true;
            polled_fsm_implementation();
            verify_output();
        }

#. Thread-safety is trivial for event-driven state machines. Events are **not** shared resources
   since they are represented as **objects**. Therefore each thread can create their own events, giving them
   exclusive access:
   
    .. figure:: /images/fsm/event_driven_paradigm_exclusive_access_to_events.svg
        :width: 500
        :align: center

        Exclusive Access to Events
   
   This makes the only shared resource the state machine. :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`
   must run to completion and cannot be pre-empted. This requirement is easily satisfied by delegating the
   state machine to its own RTOS thread that blocks on an **event queue**:

    .. figure:: /images/fsm/event_driven_paradigm_event_queue.svg
        :width: 300
        :align: center

        State Machine's Event Queue

   Other threads can only iteract with the state machine by posting events to its queue.
   Queues are a thread-synchronization primitive provided by the RTOS vendor so reads and writes are
   guaranteed to be thread-safe. The state machine performs work (processes events) in its
   own thread. A thread cannot pre-empt itself, so :ecudoxygen:`ecu_fsm_dispatch() <ecu_fsm_dispatch>`
   is guaranteed to run to completion.
   
    .. figure:: /images/fsm/event_driven_paradigm_posting_to_event_queue.svg
        :width: 500
        :align: center

        Posted to State Machine's Event Queue

   Also notice how the state machine is naturally decoupled from the RTOS.
   The implementation (state machine box) is simply wrapped in an RTOS thread. Porting to a different
   RTOS (or bare-metal) simply involves using the same state machine implementation code and wrapping 
   it in RTOS-specific primitives.

   Thread safety is **not** trivial for polling state machines. Every global flag is a shared
   resource that must be carefully guarded:

    .. figure:: /images/fsm/event_driven_paradigm_polling_state_machine_thread_safety.svg
        :width: 450
        :align: center

        Polling State Machine Thread Safety

   It is extremely difficult to analyze how different threads will effect the behavior of each global
   flag, and thus the behavior of the state machine. The state machine and application are also now 
   both tightly coupled to the RTOS. Porting to a different RTOS requires changes everywhere 
   (in this case using a new mutex API). 
   
   The above figures also showcase how much less efficient a polling state machine is. Blocking and CPU processing
   is required every time the state machine is polled (which is repeatedly). Blocking and CPU processing only occurs 
   for event-driven state machines when they need to actually perform work (post an event or process an event).

Example
-------------------------------------------------
This example aims to demonstrate the points outlined in the :ref:`Event-Driven Paradigm Section 
<fsm_event_driven_paradigm>` by creating the following LED state machine:

.. figure:: /images/fsm/example.svg
    :width: 500
    :align: center

    LED FSM Example

The LED starts in the OFF_STATE. LED_ON_EVENTs and LED_OFF_EVENTs can be blindly dispatched 
by the application to turn the LED on and off respectively. The state machine's implementation ensures the 
LED is always in the correct state. Its logic is contained within a reusable LED object, whose 
implementation is in ``led.h`` and ``led.c``.

This LED module is then ported to two separate applications. The first on a microcontroller
in ``main.c``, where multiple LED objects are created and used. The second on a computer
in ``tests.c``, where the LED module is tested.

led.h/.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    /*-------------------------- led.h --------------------------*/
    struct led
    {
        void (*turn_on)(void *obj);  /* Dependency injection. User-defined function that turns LED off. */
        void (*turn_off)(void *obj); /* User-defined function that turns LED on. */
        void *obj; /* Optional object passed to user-defined callbacks. */
        struct ecu_fsm fsm; /* State machine framework. */
    };

    enum led_event_id
    {
        LED_ON_EVENT,
        LED_OFF_EVENT
    };

    struct led_event
    {
        enum led_event_id id; /* Wrap ID in struct in case more items have to be added to event in the future. */
    };

    extern void led_ctor(struct led *me,
                         void (*turn_on)(void *obj),
                         void (*turn_off)(void *obj),
                         void *obj);

    extern void led_start(struct led *me);

    extern void led_dispatch(struct led *me, const struct led_event *event);

.. code-block:: c

    /*-------------------------- led.c --------------------------*/
    #include "led.h"

    static void on_state_entry(struct ecu_fsm *led);
    static void on_state_handler(struct ecu_fsm *led, const void *event);
    static void off_state_entry(struct ecu_fsm *led);
    static void off_state_handler(struct ecu_fsm *led, const void *event);

    /* States can be shared across any number of LED fsms. */
    static const struct ecu_fsm_state ON_STATE = ECU_FSM_STATE_CTOR(
        &on_state_entry, ECU_FSM_STATE_EXIT_UNUSED, &on_state_handler
    );

    static const struct ecu_fsm_state OFF_STATE = ECU_FSM_STATE_CTOR(
        &off_state_entry, ECU_FSM_STATE_EXIT_UNUSED, &off_state_handler
    );

    static void on_state_entry(struct ecu_fsm *led)
    {
        struct led *me = ECU_FSM_GET_CONTEXT(led, struct led, fsm);
        (*me->turn_on)(me->obj);
    }

    static void on_state_handler(struct ecu_fsm *led, const void *event)
    {
        const struct led_event *e = (const struct led_event *)event;

        switch (e->id)
        {
            case LED_OFF_EVENT:
            {
                ecu_fsm_change_state(led, &OFF_STATE);
                break;
            }

            default:
            {
                /* Ignore all other events, including LED_ON_EVENT. */
                break;
            }
        }
    }

    static void off_state_entry(struct ecu_fsm *led)
    {
        struct led *me = ECU_FSM_GET_CONTEXT(led, struct led, fsm);
        (*me->turn_off)(me->obj);
    }

    static void off_state_handler(struct ecu_fsm *led, const void *event)
    {
        const struct led_event *e = (const struct led_event *)event;

        switch (e->id)
        {
            case LED_ON_EVENT:
            {
                ecu_fsm_change_state(led, &ON_STATE);
                break;
            }
            
            default:
            {
                /* Ignore all other events, including LED_OFF_EVENT. */
                break;
            }
        }
    }

    void led_ctor(struct led *me,
                  void (*turn_on)(void *obj),
                  void (*turn_off)(void *obj),
                  void *obj)
    {
        ecu_fsm_ctor(&me->fsm, &LED_OFF_STATE);
        me->turn_on = turn_on;
        me->turn_off = turn_off;
        me->obj = obj;
    }

    void led_start(struct led *me)
    {
        ecu_fsm_start(&me->fsm);
    }

    void led_dispatch(struct led *me, const struct led_event *event)
    {
        ecu_fsm_dispatch(&me->fsm, event);
    }

main.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    /*-------------------------- main.c --------------------------*/
    #include "led.h" /* Reusable LED we created. */

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
    static struct led_gpio led1_gpio = {GPIO_PORTA, 15};
    static struct led_gpio led2_gpio = {GPIO_PORTD, 3};
    static const struct led_event ON_EVENT = {LED_ON_EVENT};
    static const struct led_event OFF_EVENT = {LED_OFF_EVENT};

    int main(void)
    {
        led_ctor(&led1, &turn_led_on, &turn_led_off, &led1_gpio);
        led_ctor(&led2, &turn_led_on, &turn_led_off, &led2_gpio);
        led_start(&led1);
        led_start(&led2);

        while (1)
        {
            /* Blindly dispatch events. */
            led_dispatch(&led1, &OFF_EVENT);
            led_dispatch(&led1, &OFF_EVENT);
            led_dispatch(&led1, &ON_EVENT);
            led_dispatch(&led1, &OFF_EVENT);
            led_dispatch(&led2, ON_EVENT);
            led_dispatch(&led2, ON_EVENT);
            led_dispatch(&led2, OFF_EVENT);
        }

        return 0;
    }

tests.c 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    /*-------------------------- tests.c -------------------------*/
    #include "led.h" /* Reusable LED module under test. */

    static void turn_led_on(void *obj)
    {
        printf("LED turned on!");
    }

    static void turn_led_off(void *obj)
    {
        printf("LED turned off!");
    }

    static const struct led_event ON_EVENT = {LED_ON_EVENT};
    static const struct led_event OFF_EVENT = {LED_OFF_EVENT};

    TEST()
    {
        struct led TEST_LED;
        led_ctor(&TEST_LED, &turn_led_on, &turn_led_off, NULL);
        led_start(&TEST_LED);                   /* "LED turned off!" */
        led_dispatch(&TEST_LED, &OFF_EVENT);    /* Nothing. */
        led_dispatch(&TEST_LED, &ON_EVENT);     /* "LED turned on!" */
        led_dispatch(&TEST_LED, &ON_EVENT);     /* Nothing. */
        led_dispatch(&TEST_LED, &OFF_EVENT);    /* "LED turned off!" */
        led_dispatch(&TEST_LED, &OFF_EVENT);    /* Nothing. */
        led_dispatch(&TEST_LED, &OFF_EVENT);    /* Nothing. */
        led_dispatch(&TEST_LED, &ON_EVENT);     /* "LED turned on!" */
    }

API 
=================================================
.. toctree::
    :maxdepth: 1

    fsm.h </doxygen/html/fsm_8h>

Macros
-------------------------------------------------

ECU_FSM_GET_CONTEXT()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Converts intrusive :ecudoxygen:`ecu_fsm` member back into the user's state machine type.
This should be used inside a state handler's definition. See
:ref:`State Machine Representation Section <fsm_state_machine_representation>`
for more details:

.. code-block:: c

    struct app_fsm
    {
        int app_data1;
        struct ecu_fsm fsm_member;
        int app_data2;
    };

    static void running_state_on_entry(struct ecu_fsm *fsm)
    {
        struct app_fsm *me = ECU_FSM_GET_CONTEXT(fsm, struct app_fsm, fsm_member);
        me->app_data1 = 0;
        me->app_data2 = 0;
    }

ECU_FSM_STATE_CTOR()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Creates an :ecudoxygen:`ecu_fsm_state` at compile-time.
See :ref:`State Representation Section <fsm_state_representation>`
for more details:

.. code-block:: c

    /* Defined by user. */
    static void running_state_on_entry(struct ecu_fsm *fsm);
    static void running_state_on_exit(struct ecu_fsm *fsm);
    static void running_state_handler(struct ecu_fsm *fsm, const void *event);

    static const struct ecu_fsm_state RUNNING_STATE = ECU_FSM_STATE_CTOR(
        &running_state_on_entry, &running_state_on_exit, &running_state_handler
    );

ecu_fsm
-------------------------------------------------

Constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_fsm_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
Constructor. Initializes the :ecudoxygen:`ecu_fsm` data structure for use.

.. warning:: 

    Must be called once on startup before the state machine is used. User is 
    also responsible for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct ecu_fsm fsm;  /* User must allocate memory before constructor. */
    ecu_fsm_start(&fsm); /* ILLEGAL. Must construct before using. */
    ecu_fsm_ctor(&fsm, &INIT_STATE);
    ecu_fsm_start(&fsm); /* Ok. */

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_fsm_change_state()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _fsm_ecu_fsm_change_state:

Transitions the FSM into a new state. See
:ref:`State Transitions Section <fsm_state_transitions>` for
more details.

.. warning:: 
    
    This function can only be called within a state's main handler 
    or entry handler.

.. code-block:: c

    static void running_state_handler(struct ecu_fsm *fsm, const void *event)
    {
        if (event == STOP_EVENT)
        {
            ecu_fsm_change_state(fsm, &STOPPED_STATE);
        }
    }

ecu_fsm_dispatch()
"""""""""""""""""""""""""""""""""""""""""""""""""
Relays event to the FSM where it is processed by the current state's 
handler function. Manages all state transition logic if any state 
changes were signalled via :ref:`ecu_fsm_change_state() <fsm_ecu_fsm_change_state>`.
See :ref:`Event-Driven Paradigm Section <fsm_event_driven_paradigm>` for
more details.

.. warning:: 

    This function must run to completion.

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
    ecu_fsm_dispatch(&fsm, &stop_event);

ecu_fsm_start()
"""""""""""""""""""""""""""""""""""""""""""""""""
Runs the initial state's entry handler and manages all state transition 
logic if any state changes were signalled via 
:ref:`ecu_fsm_change_state() <fsm_ecu_fsm_change_state>`. This function 
does nothing if the initial state's entry handler is unused. See
:ref:`State Transitions Section <fsm_state_transitions>` for
more details:

.. warning:: 

    This function should only be called once on startup and 
    must run to completion.
