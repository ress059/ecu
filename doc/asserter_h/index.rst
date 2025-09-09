.. _asserter_h:

asserter.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.
    
Framework for using assertions on embedded platforms.


Theory 
=================================================

Static Assertions
-------------------------------------------------
.. _ecu_static_assertions:

:ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>` provides a portable way
to use static assertions across any C or C++ standard. It triggers a compilation 
error if the supplied condition evaulates to false:

.. code-block:: c

    /* Compilation error if false. */
    ECU_STATIC_ASSERT( (sizeof(int) == (size_t)4), "int must be 4 bytes." );
    ECU_STATIC_ASSERT( (sizeof(char) == (size_t)1), "char must be 1 byte." );

The implementation of static assertions varies across language standards:

- Any standard before 2011: Does not natively support static assertions. A custom implementation is required.
- C++11 and greater: static_assert() natively supported.
- C11 to C23: _Static_assert() natively supported.
- C23 and greater: static_assert() natively supported.

:ecudoxygen:`ECU_STATIC_ASSERT() <ECU_STATIC_ASSERT>` detects the C or C++ standard being used 
and automatically expands to the appropriate implementation.

Runtime Assertions
-------------------------------------------------
.. _ecu_runtime_assertions:

Standard library assertions (the assert() macro) crash the program when an assertion fires, making
them unusable for embedded targets. :ecudoxygen:`ECU_ASSERT() <ECU_ASSERT>`
provides an alternative. When the supplied condition evaluates to false, the macro fires
an assertion by calling :ecudoxygen:`ecu_assert_handler() <ecu_assert_handler>`:

.. code-block:: c

    ECU_ASSERT_DEFINE_FILE("file.c") 

    void foo(int *ptr)
    {
        /* ecu_assert_handler() executes if ptr is NULL. */
        ECU_ASSERT( (ptr) );
    }

:ecudoxygen:`ecu_assert_handler() <ecu_assert_handler>` is a user-defined function that
defines system response under an assert condition:

.. code-block:: c

    void ecu_assert_handler(const char *file, int line)
    {
        /* Called by ECU_ASSERT() when assert fires. Define how system behaves 
        under these conditions. In this case, record the error then reset the CPU. */
        record(file, line);
        reset_cpu();
    }

.. warning::

    :ecudoxygen:`ecu_assert_handler() <ecu_assert_handler>` must be defined by the user
    since system response is a property of the application. A linker error will occur
    if ECU library is used but this function is not defined.

The file name and line number where the assertion fired is passed 
into :ecudoxygen:`ecu_assert_handler() <ecu_assert_handler>`. The file name must be defined
at the start of the file using the :ecudoxygen:`ECU_ASSERT_DEFINE_FILE() <ECU_ASSERT_DEFINE_FILE>` 
macro:

.. code-block:: c

    ECU_ASSERT_DEFINE_FILE("file.c") 

    void foo(int *ptr)
    {
        /* Assert ptr is not NULL. */
        ECU_ASSERT( (ptr) );
    }

If the above assertion fired, "file.c" would be the file name supplied
to :ecudoxygen:`ecu_assert_handler() <ecu_assert_handler>`. This method is a more 
memory-efficient alternative to the __FILE__ macro. The __FILE__ macro allocates
memory every single time it is used, whereas memory is only allocated for one string
literal with :ecudoxygen:`ECU_ASSERT_DEFINE_FILE() <ECU_ASSERT_DEFINE_FILE>`:

.. code-block:: c

    ECU_ASSERT_DEFINE_FILE("file.c") 

    /* Memory allocated for only one "file.c" string literal. */
    ECU_ASSERT( (true) );
    ECU_ASSERT( (true) );
    ECU_ASSERT( (true) );

    /* Memory allocated for three "file.c" string literals since __FILE__ macro used. */
    assert( (true) );
    assert( (true) );
    assert( (true) );

Runtime Assertions in ECU Source 
-------------------------------------------------
:ecudoxygen:`ECU_ASSERT() <ECU_ASSERT>` is also used within ECU source code 
to protect against API misuse. This approach is superior to propagating error codes up the
call stack. API misusage is far easier to detect via assertions and library-specific error
codes do not have to be used in the application.

Disabling Runtime Assertions
-------------------------------------------------
Runtime assertions are enabled by default. They can be disabled via ``ECU_DISABLE_RUNTIME_ASSERTS``
preprocessor define. If using CMake ``ECU_DISABLE_RUNTIME_ASSERTS`` can be set within
the project's CMakeLists.txt file:

.. code-block:: text

    set(ECU_DISABLE_RUNTIME_ASSERTS ON)

Or if invoking CMake from the command line:

.. code-block:: text

    cmake -DECU_DISABLE_RUNTIME_ASSERTS=ON .....

Otherwise ``ECU_DISABLE_RUNTIME_ASSERTS`` can be defined through preprocessor settings in
an IDE, or defined directly when invoking the compiler from the command line:

.. code-block:: text

    gcc -DECU_DISABLE_RUNTIME_ASSERTS .....

:ecudoxygen:`ECU_ASSERT() <ECU_ASSERT>` is backwards compatible and
should remain in the application even when assertions are disabled. The macro simply expands to 
((void)0) when assertions are disabled.

.. warning:: 

    There will be no protection against ECU API misuse when assertions are disabled
    since ECU source code also uses :ecudoxygen:`ECU_ASSERT() <ECU_ASSERT>`.

.. note::

    ECU_DISABLE_RUNTIME_ASSERTS has no effect on static assertions. 
    Static assertions are always enabled.


API 
=================================================
.. toctree::
    :maxdepth: 1

    asserter.h </doxygen/html/asserter_8h>

Macros
-------------------------------------------------

ECU_ASSERT()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
See :ref:`Runtime Assertions Section <ecu_runtime_assertions>`.

ECU_ASSERT_DEFINE_FILE()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
See :ref:`Runtime Assertions Section <ecu_runtime_assertions>`.

ECU_STATIC_ASSERT()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
See :ref:`Static Assertions Section <ecu_static_assertions>`.
