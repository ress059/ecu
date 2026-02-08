.. _timer_h:

timer.h
###############################################
.. raw:: html

   <hr>

Overview
=================================================
.. note:: 

    The term ``ECU`` in this document refers to Embedded C Utilities, the shorthand name for this project.

Framework for creating and running an arbitrary number of configurable software timers.

Theory
=================================================

Units Representation
-------------------------------------------------
.. _timer_units_representation:

To remain portable, units of time in this framework are measured in ticks represented by the :ecudoxygen:`ecu_tick_t` type. The application is responsible for defining the conversion between ticks and explicit units of time. 

Usually this is dictated by the resolution of available hardware timers and application requirements. In the example below, a hardware timer that ticks 10 times every 1 millisecond drives the framework:

    .. code-block:: c

        /* Helper macro made by application. 10 ticks occur every 1ms. */
        #define MS_TO_TICKS(x_)     ((ecu_tick_t)(10U * (x_)))

        /* Start a 10ms timer. Convert milliseconds to timer ticks to feed into this module. */
        ecu_tlist_timer_arm(&list, &timer, MS_TO_TICKS(10), ECU_TIMER_TYPE_ONE_SHOT);

        /* Service all software timers in list. Read hardware timer to get elapsed ticks. */
        ecu_tlist_service(&list, (ecu_tick_t)get_elapsed_ticks());

    .. note::

        There are no guarantees on :ecudoxygen:`ecu_tick_t's <ecu_tick_t>` size. The framework only mandates that it is unsigned. Currently it is an **unsigned int** since that best matches the target's word-size but is subject to change. The maximum value the type can hold will always be :ecudoxygen:`ECU_TICK_MAX`.

Timer Representation
-------------------------------------------------
A single **software** timer is represented by the :ecudoxygen:`ecu_timer` structure. Internally, this is a linked list node that contains the timer's settings:

    .. figure:: /images/timer/timer_representation.svg
        :width: 250
        :align: center

        Timer Representation

Note however that a timer cannot be a standalone object. It can only be ran by adding it to a :ref:`timer list <timer_timer_list_representation>`, which is an object that manages all timers attached to it.

    .. code-block:: c

        /* Start timer by adding it to list. It is a periodic timer that expires every 100 ticks. */
        ecu_tlist_timer_arm(&list, &timer, 100, ECU_TIMER_TYPE_PERIODIC);

        /* The list will service and manage all timers attached to it. */
        ecu_tlist_service(&list, 20);

When a timer expires its callback assigned in :ref:`ecu_timer_ctor() <timer_ecu_timer_ctor>` executes. This condition is checked and executed in the list's service call, which must be periodically called by the application:

    .. code-block:: c

        static bool expired(struct ecu_timer *me, void *obj)
        {
            printf("Timer expired!");
            return true;
        }

        struct ecu_tlist list;
        struct ecu_timer timer;

        ecu_tlist_ctor(&list);
        ecu_timer_ctor(&t1, &expired, ECU_TIMER_OBJ_UNUSED);
        ecu_tlist_timer_arm(&list, &t1, 10, ECU_TIMER_TYPE_PERIODIC);

        ecu_tlist_service(&list, 5);
        ecu_tlist_service(&list, 5);  /* Timer expired! */
        ecu_tlist_service(&list, 10); /* Timer expired! */
        ecu_tlist_timer_arm(&list, &t1, 5, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_service(&list, 5);  /* Timer expired! */
        ecu_tlist_service(&list, 5);
        ecu_tlist_service(&list, 5);

Timer List Representation
-------------------------------------------------
.. _timer_timer_list_representation:

As stated in the previous section, all timers are managed through a list which is represented by the :ecudoxygen:`ecu_tlist` object. Internally, it is a linked list of timers which are ordered by their expiration dates. When a timer is started, it is sorted and added to the list. For example:

    .. code-block:: c

        ecu_tlist_timer_arm(&list, &t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&list, &t2, 20, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&list, &t3, 30, ECU_TIMER_TYPE_ONE_SHOT);

    .. figure:: /images/timer/list_representation.svg
        :width: 600
        :align: center

        List Representation

Each :ecudoxygen:`ecu_tlist` usually contains multiple software timers (:ecudoxygen:`ecu_timers <ecu_timer>`) and is meant to be driven by a unique hardware timer. Usually separate lists are created for hardware timers possessing different resolutions. For example one list being driven by a hardware timer with 1 millisecond resolution and another list being driven by a hardware timer with 1 microsecond resolution:

    .. code-block:: c

        struct ecu_tlist timer_list_ms; /* Driven by millisecond hardware timer. 1 hardware tick = 1 millisecond. */
        struct ecu_tlist timer_list_us; /* Driven by microsecond hardware timer. 1 hardware tick = 1 microsecond. */

        /* Assume lists constructed for conciseness. */

        ecu_tlist_service(&timer_list_ms, (ecu_tick_t)get_hardware_timer_elapsed_ticks_ms());
        ecu_tlist_service(&timer_list_us, (ecu_tick_t)get_hardware_timer_elapsed_ticks_us());

Software timers requiring a specific resolution can then be added to the appropriate list. As expained :ref:`earlier <timer_units_representation>`, this framework uses an arbitrary tick type to keep track of time. Full flexibility is given to the application by allowing the conversion between ticks and units of time to be freely defined for each list. For simplicity, a 1:1 conversion is used in the example below:

    .. code-block:: c

        struct ecu_timer five_ms_timer;
        struct ecu_timer ten_us_timer;

        /* Assume timers constructed for conciseness. */

        ecu_tlist_timer_arm(&timer_list_ms, &five_ms_timer, 5, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&timer_list_us, &ten_us_timer, 10, ECU_TIMER_TYPE_ONE_SHOT);

As stated earlier, each list orders its timers by expiration dates, which are **absolute**. In other words, the actual timestamp when each timer expires is stored instead of elapsed time until expiration. The list keeps track of the current time with each service call, so a timer's expiration date can be calculated using only its period:

.. _timer_timer_list_representation_expiration_calculation:

    .. code-block:: text

        ecu_timer::expiration = ecu_tlist::current + ecu_timer::period

For example:

    .. code-block:: c

        ecu_tlist_timer_arm(&list, &t1, 40, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&list, &t2, 60, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_service(&list, 20);
        ecu_tlist_timer_arm(&list, &t3, 30, ECU_TIMER_TYPE_ONE_SHOT); /* Order = t1, t3, t2 */

    .. figure:: /images/timer/list_representation_absolute_expiration_date.svg
        :width: 600
        :align: center

        Absolute Expiration Date

This method offers the following benefits:

- Faster list sorting since elapsed times do not have to be calculated. The expiration dates can just be directly compared.

- No hardware dependencies. A callback that gets the current time does not have to provided by the user. Elapsed ticks passed as a parameter into the list's service call is all that is required.

- Faster service call. On a high level, the algorithm loops over all timers in the list and checks if any are expired. This method allows this iteration to immediately exit as soon as an unexpired timer has been reached, as showcased in the example below:

    .. code-block:: c

        ecu_tlist_timer_arm(&list, &t1, 20, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&list, &t2, 40, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&list, &t3, 60, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&list, &t4, 60, ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_service(&list, 20);

    .. figure:: /images/timer/list_representation_faster_service_call.svg
        :width: 700
        :align: center

        Faster Service Call

  Notice how only the timers highlighted in red have to be checked in the service call. The iteration immediately exits once ``t2`` is reached. ``t2`` has not yet expired and all timers after it will have later expiration dates since the list is ordered.
  
- Clean handling of timer overflow. A timer's expiration date can always be calculated using the same equation :ref:`explained previously <timer_timer_list_representation_expiration_calculation>` even if an overflow condition occurs. The result is stored in an :ecudoxygen:`ecu_tick_t` type which is guaranteed to be unsigned and whose maximum value is :ecudoxygen:`ECU_TICK_MAX`. Therefore the following condition can check if the operation overflows:

    .. code-block:: c

        timer->expiration = list->current + timer->period; /* Unsigned overflow occurs if resulting value exceeds ECU_TICK_MAX. */

        if (timer->expiration < list->current)
        {
            timer will expire after list counter overflows.
        }

  Timers meeting this condition are handled by adding them to a second ``wraparounds`` list which is also contained within the :ecudoxygen:`ecu_tlist` object. The example below showcases this, where :ecudoxygen:`ecu_tick_t` is uint8_t and :ecudoxygen:`ECU_TICK_MAX` is 255 for simplicity:

    .. figure:: /images/timer/list_representation_overflow.svg
        :width: 800
        :align: center

        Overflow

  Notice how ``t2's`` expiration is 4 because of unsigned overflow:
  
    .. code-block:: c

        t2::expiration = list::curent + t2::period
        t2::expiration = (ecu_tick_t)(250 + 10) = 4;

  The framework detects overflow occured since the resulting expiration timestamp is **less** than the list's current timestamp. Therefore ``t2`` is added to the ``wraparounds`` list. The service call then knows an overflow condition occurred since the ``wraparounds`` list is no longer empty, and overflow can be handled accordingly.

Example
-------------------------------------------------
.. _timer_example:

In the following example a hardware timer is used to service two one-shot timers and a periodic timer through the help of this framework. This is a miniimal example to showcase basic functionality and can easily be expanded for more complex and realistic scenarios:

    .. code-block:: c

        /*-------------- hardware_timer.h/.c -------------*/
        static volatile ecu_tick_t tick_count = 0;

        static void timer1_isr(void)
        {
            tick_count++;
        }

        ecu_tick_t timer1_get_ticks(void)
        {
            ecu_tick_t ticks;
            CRITICAL_SECTION_ENTER(); /* I.e disable interrupts. */
            ticks = tick_count; /* tick_count updated in ISR context! */
            CRITICAL_SECTION_EXIT(); /* I.e re-enable interrupts. */

            return ticks;
        }

        ecu_tick_t timer1_ms_to_ticks(uint16_t milliseconds)
        {
            /* Convert milliseconds to timer1 ticks. In this case 1:1 ratio. */
            return (ecu_tick_t)milliseconds;
        }

    .. code-block:: c

        /*---------------------- main.c ------------------*/
        static ecu_tick_t t1_prev, t1_current = 0;

        static bool one_shot_timer_expired(struct ecu_timer *me, void *obj)
        {
            printf("One shot timer expired!");
            return true;
        }

        static bool periodic_timer_expired(struct ecu_timer *me, void *obj)
        {
            printf("Periodic timer expired!");
            return true;
        }

        int main()
        {
            struct ecu_tlist list;
            struct ecu_timer timer1, timer2, timer3;

            ecu_tlist_ctor(&list);
            ecu_timer_ctor(&timer1, &one_shot_timer_expired, ECU_TIMER_OBJ_UNUSED);
            ecu_timer_ctor(&timer2, &one_shot_timer_expired, ECU_TIMER_OBJ_UNUSED);
            ecu_timer_ctor(&timer3, &periodic_timer_expired, ECU_TIMER_OBJ_UNUSED);

            ecu_tlist_timer_arm(&list, &timer1, timer1_ms_to_ticks(10), ECU_TIMER_TYPE_ONE_SHOT); /* Start 10ms one-shot timer. */
            ecu_tlist_timer_arm(&list, &timer2, timer1_ms_to_ticks(15), ECU_TIMER_TYPE_ONE_SHOT); /* Start 15ms one-shot timer. */
            ecu_tlist_timer_arm(&list, &timer3, timer1_ms_to_ticks(5), ECU_TIMER_TYPE_ONE_SHOT);  /* Start 5ms periodic timer. */

            while (1)
            {
                t1_current = timer1_get_ticks();

                if ((t1_current - t1_prev) > 0)
                {
                    ecu_tlist_service(&list, (t1_current - t1_prev));
                    t1_prev = t1_current;
                }
            }
        }

API 
=================================================
.. toctree::
    :maxdepth: 1

    timer.h </doxygen/html/timer_8h>

ecu_timer
-------------------------------------------------

Constructors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_timer_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _timer_ecu_timer_ctor:

Timer constructor. Initializes the :ecudoxygen:`ecu_timer` data structure for use and defines its callback that executes when the timer expires:

    .. warning:: 

        Constructor must be called once on startup before the timer is used. User is responsible for allocating memory since ECU does not use dynamic memory allocation.

    .. code-block:: c

        static bool timer_expired_callback(struct ecu_timer *me, void *obj)
        {
            printf("Expired!");
            return true;
        }

        struct ecu_timer me;
        ecu_timer_disarm(&me); /* ILLEGAL. Must construct before using. */

        ecu_timer_ctor(&me, &timer_expired_callback, ECU_TIMER_OBJ_UNUSED);
        ecu_timer_disarm(&me); /* OK. */

The callback can return false if it must be re-executed when the timer is next serviced in :ref:`ecu_tlist_service() <timer_ecu_tlist_service>`. For example:

    .. code-block:: c

        static bool timer_expired_callback(struct ecu_timer *me, void *obj)
        {
            printf("Expired!");
            return false;
        }

        struct ecu_timer me;
        ecu_timer_ctor(&me, &timer_expired_callback, ECU_TIMER_OBJ_UNUSED);
        ecu_tlist_timer_arm(&list, &me, 100, ECU_TIMER_TYPE_ONE_SHOT);

        ecu_tlist_service(&list, 100); /* Expired! */

        /* Timer keeps expiring since callback returns false. */
        ecu_tlist_service(&list, 1); /* Expired! */
        ecu_tlist_service(&list, 1); /* Expired! */
        ecu_tlist_service(&list, 1); /* Expired! */

An example use case of this feature could be that a queue write operation failed due to it being full, so it must be immediately reattempted on the next service call:

    .. code-block:: c

        static bool timer_expired_callback(struct ecu_timer *me, void *obj)
        {
            /* Returns true if successful. False if queue write failed. */
            return queue_write(&timer_expired_event);
        }

This framework can also handle any edits made to the timer within its expiration callback. These include rearming a one-shot timer, disarming a periodic timer, changing a timer's settings, etc. For example:

    .. code-block:: c

        static bool timer_expired_callback(struct ecu_timer *me, void *obj)
        {
            struct ecu_tlist *list = (struct ecu_tlist *)obj;

            if (ecu_timer_type(me) == ECU_TIMER_TYPE_ONE_SHOT)
            {
                if (timer_needs_to_be_rearmed())
                {
                    ecu_tlist_timer_rearm(&list, me);
                }
                else if (timer_needs_to_change_to_periodic())
                {
                    ecu_tlist_timer_arm(&list, me, ecu_timer_period(me), ECU_TIMER_TYPE_PERIODIC);
                }
            }
            else if (ecu_timer_type(me) == ECU_TIMER_TYPE_PERIODIC && timer_needs_to_be_disarmed())
            {
                ecu_timer_disarm(me);
            }

            return true;
        }

        struct ecu_tlist list;
        struct ecu_timer me;
        ecu_timer_ctor(&me, &timer_expired_callback, &list);

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_timer_active()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the timer is currently running. False otherwise.

    .. code-block:: c

        struct ecu_tlist list;
        struct ecu_timer t1, t2;

        ecu_tlist_timer_arm(&list, &t1, ....);
        ecu_timer_active(&t1); /* True. */
        ecu_timer_active(&t2); /* False. */
        ecu_timer_disarm(&t1);
        ecu_timer_active(&t1); /* False. */

ecu_timer_disarm()
"""""""""""""""""""""""""""""""""""""""""""""""""
Stops the timer. Timer can be rearmed without reconstruction. This function can be used on a timer that is already disarmed.

    .. code-block:: c

        struct ecu_tlist list;
        struct ecu_timer t1, t2;

        /* Before. */
        ecu_tlist_timer_arm(&list, &t1, ....);

        /* After. */
        ecu_timer_disarm(&t1);
        ecu_timer_disarm(&t2);

    .. figure:: /images/timer/ecu_timer_disarm.svg
        :width: 700
        :align: center

        ecu_timer_disarm()

ecu_timer_period()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the timer's period that was set when it was started.

    .. code-block:: c
        
        struct ecu_tlist list;
        struct ecu_timer t1, t2;
        
        ecu_tlist_timer_arm(&list, &t1, 100, ....);
        ecu_tlist_timer_arm(&list, &t2, 30, ....);

        ecu_timer_period(&t1); /* 100 */
        ecu_timer_period(&t2); /* 30 */

ecu_timer_set()
"""""""""""""""""""""""""""""""""""""""""""""""""
Stops the timer if it was running and reconfigures it with the newly supplied settings. Timer is not restarted.

    .. code-block:: c

        struct ecu_tlist list;
        struct ecu_timer t1, t2;

        /* Before. */
        ecu_tlist_timer_arm(&tlist, &t1, 10, ....);

        /* After. */
        ecu_timer_set(&t1, 20, ....);

    .. figure:: /images/timer/ecu_timer_set.svg
        :width: 500
        :align: center

        ecu_timer_set()

ecu_timer_type()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the timer's type that was specified when it was started.

    .. code-block:: c

        struct ecu_tlist list;
        struct ecu_timer t1, t2;

        ecu_tlist_timer_arm(&list, &t1, ...., ECU_TIMER_TYPE_ONE_SHOT);
        ecu_tlist_timer_arm(&list, &t2, ...., ECU_TIMER_TYPE_PERIODIC);

        ecu_timer_type(&t1); /* ECU_TIMER_TYPE_ONE_SHOT */
        ecu_timer_type(&t2); /* ECU_TIMER_TYPE_PERIODIC */

ecu_tlist
-------------------------------------------------

Constructors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_tlist_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
Timer list constructor. Initializes the :ecudoxygen:`ecu_tlist` data structure for use, which is an object that manages all software timers attached to it. See :ref:`Timer List Representation <timer_timer_list_representation>` for more details.

    .. warning:: 

        Constructor must be called once on startup before the list is used. User is responsible for allocating memory since ECU does not use dynamic memory allocation.

    .. code-block:: c 

        struct ecu_tlist tlist;         /* User must allocate memory before constructor. */
        ecu_tlist_service(&tlist, 10);  /* ILLEGAL. Must construct before using. */

        ecu_tlist_ctor(&tlist);
        ecu_tlist_service(&tlist, 10);  /* OK. */

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_tlist_service()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _timer_ecu_tlist_service:

Services all timers currently in the list. Servicing involves expiring appropriate timers, handling timer rearming, etc. This must be periodically called by the application at least once every :ecudoxygen:`ECU_TICK_MAX` ticks. However the accuracy of the timers is proportional to how often this function is called. See :ref:`Timer List Representation <timer_timer_list_representation>` and :ref:`Example Section <timer_example>` for more details.

ecu_tlist_timer_arm()
"""""""""""""""""""""""""""""""""""""""""""""""""
Starts a timer with the specified settings. 

    .. code-block:: c

        struct ecu_tlist list;
        struct ecu_timer t1;

        ecu_tlist_timer_arm(&list, &t1, 10, ....);

    
    .. figure:: /images/timer/ecu_tlist_timer_arm.svg
        :width: 600
        :align: center

        ecu_tlist_timer_arm()

If the timer is already running it is restarted and reconfigured with the newly specified settings.

    .. code-block:: c

        struct ecu_tlist list;
        struct ecu_timer t1;

        /* Before. */
        ecu_tlist_timer_arm(&list, &t1, 10, ....);

        /* After. */
        ecu_tlist_timer_arm(&list, &t1, 55, ....);

    .. figure:: /images/timer/ecu_tlist_timer_arm_running_timer.svg
        :width: 600
        :align: center

        ecu_tlist_timer_arm() on Running Timer

ecu_tlist_timer_rearm()
"""""""""""""""""""""""""""""""""""""""""""""""""
Restarts the timer with its same settings.

    .. warning::

        The timer's period and type must have been previously set by calling :ecudoxygen:`ecu_timer_set() <ecu_timer_set>` or :ecudoxygen:`ecu_tlist_timer_arm() <ecu_tlist_timer_arm>` before using this function.

    .. code-block:: c

        struct ecu_tlist list;
        struct ecu_timer t1;

        ecu_tlist_timer_arm(&list, &t1, 10, ....); /* Timer started that will expire in 10 ticks. */
        ecu_tlist_service(&list, 5); /* 5 ticks has elapsed so t1 will now expire in 5 ticks. */

        ecu_tlist_timer_rearm(&list, &t1); /* t1 restarted so it now expires in 10 ticks. */
