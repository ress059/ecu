.. _asserter_h:

asserter.h
###############################################
.. raw:: html

   <hr>


Overview
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
Framework for using static and runtime asserts on embedded platforms. Example usage:

.. code-block:: c
    /*----------------------- file.c -----------------------*/
    #include "ecu/asserter.h"

    ECU_STATIC_ASSERT( (sizeof(int) == (size_t)4) ); /* Assert condition at compile-time. */

    static void foo(void *ptr)
    {
        /* Assert condition at run-time using default handler. */
        ECU_RUNTIME_ASSERT( (ptr != (void *)0), ECU_DEFAULT_FUNCTOR );

        /* Assert condition at run-time using custom handler. */
        ECU_RUNTIME_ASSERT( (ptr != (void *)0), &my_functor );
    }


Static Assert Module
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
This module and the :ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>` macro provides the 
application a portable way to use static assertions. Example usage:

.. code-block:: c

    /*----------------------- file.c -----------------------*/
    #include "ecu/asserter.h"

    ECU_STATIC_ASSERT( (sizeof(int) == (size_t)4) ); /* Assert condition at compile-time. */


This is useful since the C-standard implementation of static assert is not consistent:

.. code-block:: c

    /*----------------------- Pre-C11: file.c -----------------------*/
    // Have to come up with custom implementation of static assert.


    /*-------------------------- C11: file.c -----------------------*/
    #include <assert.h>

    _Static_assert( (sizeof(int) == (size_t)4) );


    /*----------------------- Post-C11: file.c -----------------------*/
    static_assert( (sizeof(int) == (size_t)4) );

    
+ Pre-C11: static assert is not natively supported so a custom mechanism must be created.

- C11: static assert is supported by the standard but assert.h header must be included. Macro is _Static_assert().

+ Post-C11: static assert is natively supported by the standard. assert.h not needed. Macro is static_assert().

This module detects the C standard being used and adjusts to the different implementations accordingly, as 
explained in :ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>`. This allows the application to 
freely use :ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>` across any C standard.



The Default Assert Handler
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


Using Your Own Assert Handler
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


Asserts In ECU Source Code
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


Enabling and Disabling Runtime Asserts
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


API
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/asserter_8h>