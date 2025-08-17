.. _endian_h:

endian.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.
    
Provides a portable way to send and receive data in a specific endianness. 
No code changes are required if the target's endianness changes.


Theory 
=================================================

Endianness Problem
-------------------------------------------------
Data is stored in memory differently depending on the hardware target:

.. figure:: /images/endian/theory_endianness.svg
    :width: 400
    :align: center

    Big Endian vs Little Endian

In most scenarios, endianness is automatically handled by the compiler. 
However some use cases arise where it cannot be ignored.
For example, when sending serialized data over a bus. 

In the example below,
0x1234 must be sent over a little endian bus. Sending data directly like
below only works on a little endian target:

.. code-block:: c

    uint16_t value = 0x1234;

    /* Only works on little endian target. */
    send_data_over_le_bus(&value); /* Byte by byte. */

.. figure:: /images/endian/ecu_cpu_to_le_raw_write.svg
    :width: 550
    :align: center

    Sending Little Endian Data without ECU

Unfortunately, a common solution is to manually swap bytes in order
to port the code over to a big endian target. However this reverses the issue.
Now code only works for a big endian target:

.. code-block:: c

    uint16_t value = 0x1234;

    /* Hard-coded to only work for big endian targets. */
    value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
    send_data_over_le_bus(&value); /* Byte by byte. */

.. figure:: /images/endian/ecu_cpu_to_le_manual_byte_swap.svg
    :width: 600
    :align: center

    Sending Little Endian Data without ECU

This is unportable. If the target ever changes back to a little endian
machine, every byte swapping operation scattered throughout the codebase 
would have to be manually changed. 

The macros in this module provide a portable solution by selectively
swapping bytes based on the target's endianness. In this case, bytes
are only swapped when compiling on a big endian machine. Otherwise
the macro does nothing:

.. code-block:: c

    /* Portable. Works on both little and big endian targets. */
    uint16_t value = ECU_CPU_TO_LE16_RUNTIME(0x1234);
    send_data_over_le_bus(&value); /* Byte by byte. */

.. figure:: /images/endian/ecu_cpu_to_le.svg
    :width: 750
    :align: center

    Sending Little Endian Data with ECU

Detecting Endianness
-------------------------------------------------
The endianness of the target must be known so the macros explained 
in the previous section expand to the proper operations. ``ECU_LITTLE_ENDIAN`` 
must be defined for a little endian target **or** ``ECU_BIG_ENDIAN`` must be defined
for a big endian target.

ECU attempts to detect the endianness of the target and automatically define
either ``ECU_LITTLE_ENDIAN`` or ``ECU_BIG_ENDIAN`` by using 
the CMake variable `CMAKE_C_BYTE_ORDER <https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_BYTE_ORDER.html>`_.
If this is unsuccessful, a warning message will be printed in the console at 
configuration time. A compilation error will also occur if this module is 
used in this case:

.. figure:: /images/endian/endian_warning_message.PNG
  :width: 500
  :align: center

  Warning Message

If auto-detection fails or a CMake build system is not being used,
the endianness of the target can be manually specified by passing
``-DECU_LITTLE_ENDIAN`` **or** ``-DECU_BIG_ENDIAN`` compiler flags.


API 
=================================================
.. toctree::
    :maxdepth: 1

    endian.h </doxygen/html/endian_8h>

Macros
-------------------------------------------------

Reading Big Endian Data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _endian_reading_big_endian_data:

The following macros provide a portable way to read big endian encoded data.
Reading data directly like in the example below only works on a big endian target:

.. code-block:: c

    uint16_t value;
    read_be_data(&value); /* memcpy. */

    /* Only works on big endian target. */
    if (value == 0x1234) 
    {
        do_stuff();
    }

.. figure:: /images/endian/ecu_be_to_cpu_raw_read.svg
    :width: 600
    :align: center

    Reading Big Endian Data without ECU

Manually swapping bytes to make the code work for a little endian target
reverses the issue. The code now only works for a little endian target:

.. code-block:: c

    uint16_t value;
    read_be_data(&value); /* memcpy. */

    /* Hard-coded to only work on little endian target. */
    value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
    if (value == 0x1234) 
    {
        do_stuff();
    }

.. figure:: /images/endian/ecu_be_to_cpu_manual_byte_swap.svg
    :width: 600
    :align: center

    Reading Big Endian Data without ECU

These macros provide a portable solution by selectively swapping bytes
based on the target's endianness. Bytes are only swapped when compiling
on a little endian machine. Otherwise these macros do nothing:

.. code-block:: c

    uint16_t value;
    read_be_data(&value); /* memcpy. */

    /* Portable. Works on both little and big endian targets. */
    if (ECU_BE16_TO_CPU_RUNTIME(value) == 0x1234) 
    {
        do_stuff();
    }

.. figure:: /images/endian/ecu_be_to_cpu.svg
    :width: 750
    :align: center

    Reading Big Endian Data with ECU

ECU_BE16_TO_CPU_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_BE32_TO_CPU_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_BE64_TO_CPU_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_BE32_TO_CPU_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_BE64_TO_CPU_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_BE16_TO_CPU_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

.. raw:: html

   <hr>

Reading Little Endian Data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The following macros provide a portable way to read little endian encoded data.
Exact same logic as :ref:`ECU_BE_TO_CPU() macros <endian_reading_big_endian_data>` 
but for little endian data. Bytes are only swapped when compiling on a big endian 
machine. Otherwise these macros do nothing and return the same input.

ECU_LE16_TO_CPU_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_LE32_TO_CPU_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_LE64_TO_CPU_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_LE16_TO_CPU_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_LE32_TO_CPU_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_LE64_TO_CPU_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

.. raw:: html

   <hr>

Sending Big Endian Data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _endian_sending_big_endian_data:

The following macros provide a portable way to send data in
big endian format. Sending data directly like in the example
below only works on a big endian target:

.. code-block:: c

    uint16_t value = 0x1234;

    /* Only works on big endian target. */
    send_data_over_be_bus(&value); /* Byte by byte. */

.. figure:: /images/endian/ecu_cpu_to_be_raw_write.svg
    :width: 550
    :align: center

    Sending Big Endian Data without ECU

Manually swapping bytes to make the code work for a little endian target
reverses the issue. The code now only works for a little endian target:

.. code-block:: c

    uint16_t value = 0x1234;

    /* Hard-coded to only work for little endian targets. */
    value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
    send_data_over_be_bus(&value); /* Byte by byte. */

.. figure:: /images/endian/ecu_cpu_to_be_manual_byte_swap.svg
    :width: 600
    :align: center

    Sending Big Endian Data without ECU

These macros provide a portable solution by selectively swapping bytes
based on the target's endianness. Bytes are only swapped when compiling
on a little endian machine. Otherwise these macros do nothing:

.. code-block:: c

    /* Portable. Works on both little and big endian targets. */
    uint16_t value = ECU_CPU_TO_BE16_RUNTIME(0x1234);
    send_data_over_be_bus(&value); /* Byte by byte. */

.. figure:: /images/endian/ecu_cpu_to_be.svg
    :width: 750
    :align: center

    Sending Big Endian Data with ECU

ECU_CPU_TO_BE16_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_BE32_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_BE64_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_BE16_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_BE32_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_BE64_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

.. raw:: html

   <hr>

Sending Little Endian Data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The following macros provide a portable way to send data in
little endian format. Exact same logic as :ref:`ECU_CPU_TO_BE() macros <endian_sending_big_endian_data>`
but for sending little endian data. Bytes are only swapped when compiling on a big endian 
machine. Otherwise these macros do nothing and return the same input.

ECU_CPU_TO_LE16_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_LE32_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_LE64_COMPILETIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_LE16_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_LE32_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""

ECU_CPU_TO_LE64_RUNTIME()
"""""""""""""""""""""""""""""""""""""""""""""""""
