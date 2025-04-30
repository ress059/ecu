.. _timer_h:

timer.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

Runs an arbitrary number of configurable software timers through a single call 
to :ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>`. When a timer expires,
its callback function executes.


Theory
=================================================
This section explains this module's design and how to use it. It is recommended
to read this before using the API.


Example
-------------------------------------------------
The example below runs a one-shot timer with a period of 50 ticks and a 
periodic timer with a period of 100 ticks. It provides a brief introduction
to this module. Implementation details are further explained in the following 
subsections:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    static bool callback(struct ecu_timer *me, void *obj)
    {
        return true;
    }

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &callback, ECU_TIMER_OBJ_UNUSED);

    ecu_tlist_timer_arm(&tlist, &t1, 50, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_tlist_timer_arm(&tlist, &t2, 100, ECU_TIMER_TYPE_PERIODIC);

    ecu_tlist_service(&tlist, 50);  /* t1 expires. */
    ecu_tlist_service(&tlist, 50);  /* t2 expires. */
    ecu_tlist_service(&tlist, 50);
    ecu_tlist_service(&tlist, 50);  /* t2 expires. */
    ecu_tlist_service(&tlist, 100); /* t2 expires. */
    ecu_tlist_service(&tlist, 100); /* t2 expires. */


Timer Units Representation
-------------------------------------------------
All units are in **timer ticks** to remain fully hardware-agnostic.
The translation between ticks and units of time is a property of the application,
and depends on how a **hardware** timer is configured.

Therefore the application is responsible for this conversion if units of time
is desired. It is recommended to contain this conversion within a macro and feed 
its output into the API:

.. code-block:: c

    /* In this case a timer tick increments 10 times every 1ms. */
    #define MS_TO_TICKS(x_)     ((ecu_tick_t)(10U * (x_)))

    /* Start a 10ms timer. Convert milliseconds to timer ticks to feed into this module. */
    ecu_tlist_timer_arm(&tlist, &timer, MS_TO_TICKS(10), ECU_TIMER_TYPE_ONE_SHOT);

    /* Pass elapsed hardware timer ticks to service function. */
    ecu_tlist_service(&tlist, get_elapsed_ticks());

    /* If it is easier for the application to use units of time, convert elapsed ms to elapsed ticks. */
    ecu_tlist_service(&tlist, MS_TO_TICKS(5));

In the example above, the conversion macro returns an :ecudoxygen:`ecu_tick_t` value.
The module defines this type to store timer ticks in case it has to change in the future.

Currently :ecudoxygen:`ecu_tick_t` is an **unsigned int**.
The C standard dictates that int is the natural size suggested by the architecture
of the execution environment. In other words, it suggests that int is the word-size
of the target. 

This module is driven by a user's hardware timer. Its elapsed ticks is fed into
:ecudoxygen:`ecu_tlist_service() ecu_tlist_service`.
On almost all microcontrollers, a timer's tick width is less than or equal to 
its word-size. I.e. a 32-bit microcontroller may have 
8-bit timers, 16-bit timers, and 32-bit timers, but usually not 64-bit timers.
An unsigned int type best matches this pattern since it is implementation-defined 
and, in almost all cases, will be the word-size of the target platform.


Timer List Representation
-------------------------------------------------
.. _timer_list_representation:

A timer list is the "engine" that runs all of the individual software timers (:ecudoxygen:`ecu_timer`) 
added to it. It is represented by the :ecudoxygen:`ecu_tlist` object.

:ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>` must be periodically called by the 
application to service every timer in the :ecudoxygen:`ecu_tlist`. The application must pass 
the **elapsed ticks since the last service call** into this function. For the best accuracy,
this value must be derived from a hardware timer. Thus each hardware timer in use should map 
to a unique :ecudoxygen:`ecu_tlist` object:

.. code-block:: c

    /*-------------- hardware_timer.h/.c -------------*/
    static volatile ecu_tick_t t1_current = 0;
    static volatile ecu_tick_t t2_current = 0;

    static void timer1_isr(void)
    {
        t1_current++;
    }

    static void timer2_isr(void)
    {
        t2_current++;
    }

    ecu_tick_t get_timer1_current(void)
    {
        ecu_tick_t current;
        disable_interrupts(); /* Critical section enter. */
        current = t1_current; /* t1_current updated in ISR context! */
        enable_interrupts();  /* Critical section exit. */

        return current;
    }

    ecu_tick_t get_timer2_current(void)
    {
        ecu_tick_t current;
        disable_interrupts(); /* Critical section enter. */
        current = t2_current; /* t2_current updated in ISR context! */
        enable_interrupts();  /* Critical section exit. */

        return current;
    }

    /*---------------------- main.c ------------------*/
    static ecu_tick_t t1_prev, t1_current = 0;
    static ecu_tick_t t2_prev, t2_current = 0;
    static struct ecu_tlist tlist1, tlist2;

    int main()
    {
        ecu_tlist_ctor(&tlist1);
        ecu_tlist_ctor(&tlist2);

        while (1)
        {
            t1_current = get_timer1_current();
            t2_current = get_timer2_current();

            /* Pass in elapsed ticks. Notice how timer1 is used to 
            service tlist1 and timer2 is used to service tlist2. */
            if ((t1_current - t1_prev) > 0)
            {
                ecu_tlist_service(&tlist1, (t1_current - t1_prev));
                t1_prev = t1_current;
            }
            if ((t2_current - t2_prev) > 0)
            {
                ecu_tlist_service(&tlist2, (t2_current - t2_prev));
                t2_prev = t2_current;
            }
        }
    }

.. warning::

    The application is responsible for ensuring exclusive access to :ecudoxygen:`ecu_tlist` 
    and every :ecudoxygen:`ecu_timer` if this API is called within an ISR, or used in 
    a multi-threaded environment.

Efficiency is maximized for :ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>` since it must 
be called periodically. This is accomplished by storing all timers in an **ordered** linked list.
When a timer is added by calling :ecudoxygen:`ecu_tlist_timer_arm() <ecu_tlist_timer_arm>`
or :ecudoxygen:`ecu_tlist_timer_rearm() <ecu_tlist_timer_rearm>`, it is inserted into the proper 
position such that all timers are sorted by **absolute expiration date**:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2, t3;
    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t3, &callback, ECU_TIMER_OBJ_UNUSED);

    ecu_tlist_timer_arm(&tlist, &t1, 30, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_tlist_timer_arm(&tlist, &t2, 10, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_tlist_timer_arm(&tlist, &t3, 20, ECU_TIMER_TYPE_ONE_SHOT);

.. figure:: /images/timer/tlist_timer_order.svg
  :width: 600
  :align: center

  Timer List Order

This makes :ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>` an O(N) function where N = the number 
of **expired** timers, **not** the total number of timers. Following off of the above code snippet:

.. code-block:: c

    ecu_tlist_service(10); /* Function exits as soon as non-expired timer (t3) is reached. */

.. figure:: /images/timer/tlist_service_order_n.svg
  :width: 600
  :align: center

  Timer Service O(N)

Starting at HEAD, the service function iterates over timers in the linked list. 
The current timer's value is checked. If it has expired, the iteration continues
and the next timer is checked. Otherwise the function can immediately exit, as 
an ordered list guarantees the remaining timers have also not expired.

To maintain this order, every :ecudoxygen:`ecu_tlist` stores an **absolute** timestamp 
(:ecudoxygen:`ecu_tlist.current`) to keep track of time. This stored value is incremented by the 
number of elapsed ticks passed to :ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>`. Therefore 
it is updated in every call to :ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>`:

.. code-block:: c

    struct ecu_tlist tlist;

    ecu_tlist_ctor(&tlist);         /* ecu_tlist::current = 0 at start.*/
    ecu_tlist_service(&tlist, 5);   /* ecu_tlist::current = 5. */
    ecu_tlist_service(&tlist, 5);   /* ecu_tlist::current = 10. */
    ecu_tlist_service(&tlist, 40);  /* ecu_tlist::current = 50. */
    ecu_tlist_service(&tlist, 10);  /* ecu_tlist::current = 60. */
    // ....

A timer's **absolute expiration date** is calculated using this timestamp:

    Absolute expiration date = ecu_tlist.current + ecu_timer.period

This ensures proper order when a new timer is added to an :ecudoxygen:`ecu_tlist`
that already contains active timers:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &callback, ECU_TIMER_OBJ_UNUSED);

    ecu_tlist_timer_arm(&tlist, &t1, 100, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_tlist_service(&tlist, 95); /* ecu_tlist::current = 95. t1 expires @ 100. */
    ecu_tlist_timer_arm(&t2, 20, ECU_TIMER_TYPE_ONE_SHOT); /* ecu_tlist::current = 95. t1 expires @ 100. t2 expires @ 115. */

.. figure:: /images/timer/tlist_absolute_expiration_date_oneshot.svg
  :width: 600
  :align: center

  Absolute Expiration Date Example

Readability is straightforward as all timers are simply sorted by increasing 
expiration dates (least to greatest). Therefore t2 is inserted **after** t1, even 
though its period is less. At the time t2 is added, t1 has 5 ticks remaining 
until expiration.

For completeness, the following example shows this scheme in action for a periodic timer:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_tlist_timer_arm(&tlist, &t1, 100, ECU_TIMER_TYPE_PERIODIC);

    ecu_tlist_service(&tlist, 100); /* ecu_tlist::current = 100. t1 expires. */
    ecu_tlist_service(&tlist, 100); /* ecu_tlist::current = 200. t1 expires. */
    ecu_tlist_service(&tlist, 100); /* ecu_tlist::current = 300. t1 expires. */

.. figure:: /images/timer/tlist_absolute_expiration_date_periodic.svg
  :width: 600
  :align: center

  Absolute Expiration Date Example

This scheme allows all timers in :ecudoxygen:`ecu_tlist` to be driven by the elapsed ticks
parameter passed into :ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>`. **No user-defined 
callbacks are required**.
Since the current timestamp is stored in the :ecudoxygen:`ecu_tlist` object, timers being 
used across different :ecudoxygen:`ecu_tlist` objects are also automatically handled. Recall

    Absolute expiration date = ecu_tlist.current + ecu_timer.period

where ecu_tlist.current is **unique** to each :ecudoxygen:`ecu_tlist` instance. For example:

.. code-block:: c

    struct ecu_tlist tlist1, tlist2;
    struct ecu_timer t1;

    ecu_tlist_ctor(&tlist1);
    ecu_tlist_ctor(&tlist2);
    ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);

    /* tlist1.current = 0.
    t1.expire = tlist1.current + t1.period = 100. 
    t1 expires after 100 ticks. */
    ecu_tlist_timer_arm(&tlist1, &t1, 100, ECU_TIMER_TYPE_ONE_SHOT);

    /* tlist2.current = 2000.
    t1.expire = tlist2.current + t1.period = 2100. 
    t1 added to new list and still expires after 100 ticks. */
    ecu_tlist_service(&tlist2, 2000);
    ecu_tlist_timer_arm(&tlist2, &t1, 100, ECU_TIMER_TYPE_ONE_SHOT);

After an :ecudoxygen:`ecu_tlist` has been running for a while, its absolute timestamp will
eventually overflow:

.. code-block:: c

    ecu_tlist_service(&tlist, ECU_TICK_MAX); /* ecu_tlist::current = ECU_TICK_MAX. About to wraparound. */
    ecu_tlist_service(&tlist, 1); /* ecu_tlist::current = 0. Wraparound! */

This edge case is handled by a :ecudoxygen:`wraparounds linked list (ecu_tlist.wraparounds) <ecu_tlist.wraparounds>`.
Timers that are running but expire after an :ecudoxygen:`ecu_tlist.current` overflow are stored
in this list. Otherwise :ecudoxygen:`ecu_tlist.wraparounds` acts the exact same as :ecudoxygen:`ecu_tlist.timers`.
The following example shows how overflow is handled. For simplicity, the counter overflows once it 
exceeds 255. I.e. 255 + 1 = 0.

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2, t3, t4;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t3, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t4, &callback, ECU_TIMER_OBJ_UNUSED);

    /* Max of ecu_tlist::current is 255! ecu_tlist::current = 245. 
    t1 = expires @ 250. t2 = expires @ 5 (wraparound!). 
    t3 = expires @ 10 (wraparound!). t4 = expires @ 20 (wraparound!). */
    ecu_tlist_service(&tlist, 245); 
    ecu_tlist_timer_arm(&tlist, &t1, 5, ECU_TIMER_TYPE_ONE_SHOT);   /* t1 added to timers list. */
    ecu_tlist_timer_arm(&tlist, &t2, 16, ECU_TIMER_TYPE_ONE_SHOT);  /* t2 added to wraparounds list. */
    ecu_tlist_timer_arm(&tlist, &t3, 21, ECU_TIMER_TYPE_ONE_SHOT);  /* t3 added to wraparounds list. */
    ecu_tlist_timer_arm(&tlist, &t4, 31, ECU_TIMER_TYPE_ONE_SHOT);  /* t4 added to wraparounds list. */

    /* Overflow! ecu_tlist::current = 255 + 16 = 5. This is automatically handled! */
    ecu_tlist_service(&tlist, 16);

.. figure:: /images/timer/tlist_counter_overflow.svg
  :width: 800
  :align: center

  Timer List Counter Overflow

The steps in the figure above are taken to handle overflow in the service call:

#. :ecudoxygen:`ecu_tlist.current` is updated. Since it's maximum value is 255 
   it overflows back to 5. 
        
        current = 245 + 16 = (uint8_t)261 = 5

#. Since the counter overflowed, it is guaranteed that all timers in the :ecudoxygen:`ecu_tlist.timers`
   list have expired. This condition does not have to be checked - all timers in this list 
   are just expired.

#. The contents of :ecudoxygen:`ecu_tlist.wraparounds` are swapped into :ecudoxygen:`ecu_tlist.timers`
   so normal behavior can continue until the next overflow. I.e. :ecudoxygen:`ecu_tlist.timers` list
   acts the same. If timers with periods between 251-255 are armed at this point (current = 5), they 
   would be inserted into :ecudoxygen:`ecu_tlist.wraparounds`.

#. The remaining timers in the updated :ecudoxygen:`ecu_tlist.timers` list are serviced like normal.
   :ecudoxygen:`ecu_tlist.current` is 5, so only t2 expires.

#. List iteration has reached a non-expired timer (t2) so function immediately exits.

The examples above show why the :ecudoxygen:`ecu_tlist` object is required to arm 
an :ecudoxygen:`ecu_timer`. The application can create wrappers that allow the list 
to not have to be passed:

.. code-block:: c

    /*------------------- app_timers.c ---------------*/
    static struct ecu_tlist tlist;

    /* Public wrapper functions. */
    void tlist_init(void)
    {
        ecu_tlist_ctor(&tlist);
    }

    void timer_arm(struct ecu_timer *me, ecu_tick_t period, enum ecu_timer_type type)
    {
        ecu_tlist_timer_arm(&tlist, me, period, type);
    }

    void tlist_service(void)
    {
        ecu_tlist_service(&tlist, get_elapsed_ticks());
    }

    /*---------------------- main.c ------------------*/
    struct ecu_timer t1, t2;

    int main(void)
    {
        tlist_init();
        ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);
        ecu_timer_ctor(&t2, &callback, ECU_TIMER_OBJ_UNUSED);

        /* Public wrappers allow list to not have to be passed in. */
        timer_arm(&t1, 10, ECU_TIMER_TYPE_PERIODIC);
        timer_arm(&t2, 50, ECU_TIMER_TYPE_ONE_SHOT);
        tlist_service();
    }


Timer Representation
-------------------------------------------------
.. _timer_representation:

A single software timer is represented by the :ecudoxygen:`ecu_timer` object. 
In order to run, it must be added to an :ecudoxygen:`ecu_tlist` by calling 
:ecudoxygen:`ecu_tlist_timer_arm() <ecu_tlist_timer_arm>` or 
:ecudoxygen:`ecu_tlist_timer_rearm() <ecu_tlist_timer_rearm>`. The timer's 
period and type are specified in these calls:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &callback, ECU_TIMER_OBJ_UNUSED);

    /* Set timer's period and type without starting it. */
    ecu_timer_set(&t2, 50, ECU_TIMER_TYPE_ONE_SHOT);

    /* Starts timer. Sets its period and type. */
    ecu_tlist_timer_arm(&tlist, &t1, 100, ECU_TIMER_TYPE_ONE_SHOT);

    /* Already set timers can be rearmed. Its previously set type
    and period are used. */
    ecu_tlist_timer_rearm(&tlist, &t1);
    ecu_tlist_timer_rearm(&tlist, &t2);

As explained in :ref:`Timer List Representation Section <timer_list_representation>`, all timers 
added to an :ecudoxygen:`ecu_tlist` are serviced in the call to :ecudoxygen:`ecu_tlist_service() <ecu_tlist_service>`.
When a timer expires, its callback executes. It is safe to use this API within the callback
to edit the timer. In this example a one-shot timer is rearmed in its callback and a periodic
timer is disarmed in its callback.

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2, t3;

    static bool t1_callback(struct ecu_timer *me, void *obj)
    {
        ecu_tlist_timer_rearm(&tlist, me);
        return true;
    }

    static bool t2_callback(struct ecu_timer *me, void *obj)
    {
        ecu_timer_disarm(me);
        return true;
    }

    static bool t3_callback(struct ecu_timer *me, void *obj)
    {
        return true;
    }

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &t2_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t3, &t3_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_tlist_timer_arm(&tlist, &t1, 50, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_tlist_timer_arm(&tlist, &t2, 50, ECU_TIMER_TYPE_PERIODIC);
    ecu_tlist_timer_arm(&tlist, &t3, 50, ECU_TIMER_TYPE_PERIODIC);

    ecu_tlist_service(&tlist, 50); /* t1, t2, and t3 expire. One-shot t1 rearmed. Periodic t2 disarmed. Periodic t3 rearmed. */
    ecu_tlist_service(&tlist, 50); /* t1 and t3 expire. One-shot t1 rearmed. Periodic t3 rearmed. */
    ecu_tlist_service(&tlist, 50); /* t1 and t3 expire. One-shot t1 rearmed. Periodic t3 rearmed. */

The callback can return false if it needs to be reattempted in the next service call.
For example:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1;

    static bool t1_callback(struct ecu_timer *me, void *obj)
    {
        return false;
    }

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_tlist_timer_arm(&tlist, &t1, 100, ECU_TIMER_TYPE_ONE_SHOT);

    ecu_tlist_service(&tlist, 100); /* t1 expires. Callback returns false. */
    ecu_tlist_service(&tlist, 1);   /* t1 expires. Callback reattempted. */
    ecu_tlist_service(&tlist, 1);   /* t1 expires. Callback reattempted. */
    ecu_tlist_service(&tlist, 1);   /* t1 expires. Callback reattempted. */

A more useful showcase of this feature could be writing to a queue. The write 
will always be reattempted on the next service call until successful:

.. code-block:: c

    static bool t1_callback(struct ecu_timer *me, void *obj)
    {
        /* Returns false if queue write failed due to it being full. */
        return queue_post(timer_expired_event);
    }

As explained in :ref:`Timer List Representation Section <timer_list_representation>`, timers in
an :ecudoxygen:`ecu_tlist` are stored in an **ordered** linked list. All timers are sorted by 
**absolute expiration date**. A false callback is handled by setting its absolute expiration
date to 0 and pushing it to the front of the list:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    static bool t1_callback(struct ecu_timer *me, void *obj)
    {
        return false;
    }

    static bool t2_callback(struct ecu_timer *me, void *obj)
    {
        return true;
    }

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &t2_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_tlist_timer_arm(&tlist, &t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_tlist_timer_arm(&tlist, &t2, 50, ECU_TIMER_TYPE_ONE_SHOT);

    ecu_tlist_service(&tlist, 10); /* t1 expires. rearmed since callback returned false. */
    ecu_tlist_service(&tlist, 1);  /* t1 expires. rearmed since callback returned false. */

.. figure:: /images/timer/timer_false_callback.svg
  :width: 600
  :align: center

  Timer False Callback


ecu_timer 
=================================================


Constructors
-------------------------------------------------


ecu_timer_ctor()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
See the :ecudoxygen:`API <ecu_timer_ctor>`. Callback implementation details explained 
in :ref:`Timer Representation Section <timer_representation>`.


Member Functions
-------------------------------------------------


ecu_timer_set()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sets the timer's period and type. If the timer was currently running, it is stopped.
For example:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &t2_callback, ECU_TIMER_OBJ_UNUSED);

    /* Before. */
    ecu_tlist_timer_arm(&tlist, &t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_tlist_timer_arm(&tlist, &t2, 50, ECU_TIMER_TYPE_ONE_SHOT);

    /* After. */
    ecu_timer_set(&t1, 20, ECU_TIMER_TYPE_PERIODIC);

.. figure:: /images/timer/ecu_timer_set.svg
  :width: 600
  :align: center

  ecu_timer_set()


ecu_timer_disarm()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Stops the timer. This can be called on a timer that has already been stopped:

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &t2_callback, ECU_TIMER_OBJ_UNUSED);

    /* Before. */
    ecu_tlist_timer_arm(&tlist, &t1, 10, ECU_TIMER_TYPE_ONE_SHOT);

    /* After. */
    ecu_timer_disarm(&t1);
    ecu_timer_disarm(&t2); /* Can be called on t2 which was already stopped. */

.. figure:: /images/timer/ecu_timer_disarm.svg
  :width: 600
  :align: center

  ecu_timer_disarm()


ecu_timer_is_active()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns true if the timer is running. False otherwise.

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &t2_callback, ECU_TIMER_OBJ_UNUSED);

    ecu_tlist_timer_arm(&tlist, &t1, 10, ECU_TIMER_TYPE_ONE_SHOT);
    ecu_timer_is_active(&t1); /* Returns true. */
    ecu_timer_is_active(&t2); /* Returns false. */


ecu_tlist 
=================================================


Constructors
-------------------------------------------------


ecu_tlist_ctor()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
See the :ecudoxygen:`API <ecu_tlist_ctor>`.


Member Functions
-------------------------------------------------


ecu_tlist_timer_arm()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Starts the timer. If the timer is already running it is restarted
with the specified values. 

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_timer_ctor(&t2, &t2_callback, ECU_TIMER_OBJ_UNUSED);

    /* Before. */
    ecu_tlist_timer_arm(&tlist, &t2, 50, ECU_TIMER_TYPE_ONE_SHOT);

    /* After. */
    ecu_tlist_timer_arm(&tlist, &t1, 30, ECU_TIMER_TYPE_ONE_SHOT);

.. figure:: /images/timer/ecu_tlist_timer_arm.svg
  :width: 600
  :align: center

  ecu_tlist_timer_arm()

Since the timer list is ordered, this operation is O(N), where N = the total 
number of timers currently in the supplied :ecudoxygen:`ecu_tlist`. 
:ref:`Timer List Representation Section <timer_list_representation>` explains 
the arming process in detail.


ecu_tlist_timer_rearm()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Restarts a previously set timer. Since the timer list is ordered, this operation is O(N), 
where N = the total number of timers currently in the supplied :ecudoxygen:`ecu_tlist`. 
:ref:`Timer List Representation Section <timer_list_representation>` explains 
the arming process in detail.

.. code-block:: c

    struct ecu_tlist tlist;
    struct ecu_timer t1, t2;

    ecu_tlist_ctor(&tlist);
    ecu_timer_ctor(&t1, &t1_callback, ECU_TIMER_OBJ_UNUSED);
    ecu_tlist_timer_arm(&tlist, &t1, 50, ECU_TIMER_TYPE_ONE_SHOT);

    ecu_tlist_service(&tlist, 45);      /* t1 expires in 5 ticks. */
    ecu_tlist_timer_rearm(&tlist, &t1); /* t1 reset. Expires in 50 ticks. */
    ecu_tlist_service(&tlist, 45);      /* t1 expires in 5 ticks. */
    ecu_tlist_timer_rearm(&tlist, &t1); /* t1 reset. Expires in 50 ticks. */
    ecu_tlist_service(&tlist, 45);      /* t1 expires in 5 ticks. */
    ecu_tlist_service(&tlist, 5);       /* t1 expires. */

.. warning::

    The timer's period and type must be previously set by calling 
    :ecudoxygen:`ecu_timer_set() <ecu_timer_set>` or :ecudoxygen:`ecu_tlist_timer_arm() <ecu_tlist_timer_arm>`
    before using this function.


ecu_tlist_service()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Services all timers in the passed :ecudoxygen:`ecu_tlist`. Must be called periodically by 
the application. This function is explained in detail in the 
:ref:`Timer List Representation Section <timer_list_representation>`. Also see the 
:ecudoxygen:`API <ecu_tlist_service>.`

.. warning::

    The application is responsible for ensuring exclusive access to :ecudoxygen:`ecu_tlist` 
    and every :ecudoxygen:`ecu_timer` if this function or the rest of the API is called 
    within an ISR, or used in a multi-threaded environment.

API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/timer_8h>
