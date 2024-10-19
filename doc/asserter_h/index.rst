.. _asserter_h:

asserter.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
Framework for using static and runtime asserts on embedded platforms. 


ECU Static Asserts
=================================================
The :ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>` macro provides the 
application a portable way to use static assertions across any C and C++ standard. 
Example usage:

.. code-block:: c

    /*----------------------- file.c -----------------------*/
    #include "ecu/asserter.h"

    ECU_STATIC_ASSERT( (sizeof(int) == (size_t)4), "int must be 4 bytes." ); /* Assert condition at compile-time. */


This is useful since the implementation of static assert across language standards is not consistent:
For C++:
+ Pre-C++11: static assert is not natively supported so a custom mechanism must be created.

- C++11 and greater: static assert is natively supported by the standard via static_assert() call. assert.h not needed.

For C:
+ Pre-C11: static assert is not natively supported so a custom mechanism must be created.

- C11 to C23: static assert is supported by the standard via _Static_assert() but assert.h header must be included.

+ C23 and greater: static assert is natively supported by the standard via static_assert(). assert.h not needed.

This module detects the C or C++ standard being used and adjusts to the different implementations accordingly, as 
explained in :ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>`. This allows the application to 
freely use :ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>` across any C and C++ standard.


ECU Runtime Asserts
=================================================
The :ecudoxygen:`ECU_RUNTIME_ASSERT() <ECU_RUNTIME_ASSERT>` macro allows the application
to assert a condition at runtime and is targeted for embedded platforms. A user-defined 
handler executes if a runtime assert fires. The file name and line number where the assert
fired will be passed to this handler. An optional object can also be passed into the handler.
Example usage:

.. code-block:: c

    /*----------------------- file.c -----------------------*/
    #include "ecu/asserter.h"


    void foo(int *ptr)
    {
        /* Assert ptr is not NULL. */
        ECU_RUNTIME_ASSERT( (ptr), &my_assert_functor );
    }


Using Your Own Assert Handler
--------------------------------------------------


.. code-block:: c

    static void my_assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        /* Optional object containing any data can be passed into your handler. */
        struct my_data *data = (struct my_data *)(&me->obj);

        /* Define how you want your system to respond to an assert.
        In this case, record data and reset the program. */
        disable_interrupts();
        record(file, line); /* ECU_RUNTIME_ASSERT() macro will pass in file and line. */
        record(data);
        reset_cpu();
    }

    /* Set up your functor. Usually this is a global functor that is shared 
    across all assert calls but a unique one can be made for any assert call. */
    static struct ecu_assert_functor my_assert_functor = 
    {
        .handler = &my_assert_handler,
        .obj = &some_data
    };




If a runtime 
assert fires a user-defined handler will be called. The file name and line number will 
be passed to this handler. 


The Default Assert Handler
=================================================





Asserts In ECU Source Code
=================================================


Enabling and Disabling Runtime Asserts
=================================================


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/asserter_8h>