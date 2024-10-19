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

- C11 to C23: static assert is natively supported by the standard via _Static_assert(). assert.h header 
  must be included to use static_assert(), which is a convenience macro defined by the standard that 
  expands to _Static_assert().

+ C23 and greater: static assert is natively supported by the standard via static_assert(). assert.h not needed.

This module detects the C or C++ standard being used and adjusts to the different implementations accordingly.
This allows the application to freely use :ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>` across any 
C and C++ standard without worrying about implementation details.


ECU Runtime Asserts
=================================================
Stdlib asserts are never appropriate for embedded targets since it crashes the program.
This framework provides an alternative by executing a user-defined handler when a run-time 
assert fires. The file name and line number where the assert fired are also passed into this 
handler in a more memory-efficient manner. Example use:

.. code-block:: c

    /*----------------------- file.c -----------------------*/
    #include "ecu/asserter.h"

    ECU_ASSERT_DEFINE_NAME("file.c") /* File name passed into handler if any runtime assert fires in this file. */

    void foo(int *ptr)
    {
        /* Assert ptr is not NULL. */
        ECU_RUNTIME_ASSERT( (ptr) );
    }

.. note:: 

    Conditions evaluated by this macro should always be true unless there is a software bug 
    or processor error. I.e. NULL pointer dereference, out of bounds array access, invalid 
    function parameters.

    Undesirable system behavior that is possible should never be an assert condition. I.e. 
    do not assert out of range temperature readings, bus errors, etc.

:ecudoxygen:`ecu_assert_handler() <ecu_assert_handler>` function executes if a run-time assert 
fires. The user **MUST** provide a definition for this function and explain how their system behaves
under an assert condition. The file name in :ecudoxygen:`ECU_ASSERT_DEFINE_NAME() <ECU_ASSERT_DEFINE_NAME>`
and the line number are passed into this handler.

.. code-block:: c

    /*----------------------- app_assert_handler.c -----------------------*/
    #include "ecu/asserter.h"

    void ecu_assert_handler(const char *file, int line)
    {
        /* Application MUST provide a definition for this handler. Define how
        your system behaves if an assert fires. Pseudocode example. In this case, 
        we record the error then reset the CPU. */
        record(file, line);
        reset_cpu();
    }

:ecudoxygen:`ECU_ASSERT_DEFINE_NAME() <ECU_ASSERT_DEFINE_NAME>` must be used at the beginning of
any source file that uses ECU run-time asserts. This macro defines the file name passed into your 
handler. For example:

.. code-block:: c

    /*----------------------- file1.c -----------------------*/
    #include "ecu/asserter.h"

    ECU_ASSERT_DEFINE_NAME("file1.c")

    static void file1(int *ptr)
    {
        /* "file1.c" will be passed into your assert handler if any assert fires within this file. */
        ECU_RUNTIME_ASSERT( (ptr) );
    }


    /*----------------------- file2.c -----------------------*/
    #include "ecu/asserter.h"

    ECU_ASSERT_DEFINE_NAME("file2.c")

    static void file2(int *ptr)
    {
        /* "file2.c" will be passed into your assert handler if any assert fires within this file. */
        ECU_RUNTIME_ASSERT( (ptr) );
    }

This macro is a more memory-efficient alternative to the __FILE__ macro. Memory for only ONE 
string will be allocated whereas memory is allocated for every __FILE__ declaration. 


Asserts in ECU Source 
""""""""""""""""""""""""""""""""""""""""""""""""
Instead of propagating error codes up the call stack, :ecudoxygen:`ECU_RUNTIME_ASSERT() <ECU_RUNTIME_ASSERT>`
is also used within ECU source code in case the API is misused. It makes bugs far 
easier to find and respond to. This is enabled by default. 


Enabling and Disabling Runtime Asserts 
""""""""""""""""""""""""""""""""""""""""""""""""
To disable runtime asserts, the "ECU_DISABLE_RUNTIME_ASSERTS" preprocessor directive can be globally 
defined. This reduces overhead at the cost of reliability. This is backwards compatible and any code 
using ECU's runtime assert framework can remain unchanged. 

.. code-block:: c

    /*---------------- ECU_DISABLE_RUNTIME_ASSERTS NOT defined so asserts are active. ----------*/
    #include "ecu/asserter.h"

    ECU_ASSERT_DEFINE_NAME("foo.c") /* Macro is active. */

    static void foo(int *ptr)
    {
        ECU_RUNTIME_ASSERT( (ptr) ); /* Asserts are active in this case. */
    }


    /*---------------- ECU_DISABLE_RUNTIME_ASSERTS defined so asserts are NOT active. ----------*/
    #include "ecu/asserter.h"

    ECU_ASSERT_DEFINE_NAME("foo.c") /* Macro expands to nothing. */

    static void foo(int *ptr)
    {
        ECU_RUNTIME_ASSERT( (ptr) ); /* Macro expands to ((void)0) so it does nothing. NULL pointer is possible. */
    }

.. warning:: 

    All checks done within ECU source code will not occur if runtime asserts are disabled.
    Also note that the runtime assert framework provided by ECU will no longer be active
    if used in your application.

.. note:: 

    This does not effect ECU static asserts. These are always enabled since they are 
    evaluated at compile-time.


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/asserter_8h>
