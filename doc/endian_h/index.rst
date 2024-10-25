.. _endian_h:

endian.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
Provides a portable way to send and receive data in a specific endianness. 
No edits are required if the endianness of your target hardware ever changes.
Also provides general-purpose byte swapping macros.


Receiving Little Endian Data 
=================================================
.. _receiving_le_data:

The following macros allow your application to correctly read little endian 
encoded data, regardless of your CPU's endianness.

    - :ecudoxygen:`ECU_LE16_TO_CPU_COMPILETIME() <ECU_LE16_TO_CPU_COMPILETIME>`
    - :ecudoxygen:`ECU_LE16_TO_CPU_RUNTIME() <ECU_LE16_TO_CPU_RUNTIME>`
    - :ecudoxygen:`ECU_LE32_TO_CPU_COMPILETIME() <ECU_LE32_TO_CPU_COMPILETIME>`
    - :ecudoxygen:`ECU_LE32_TO_CPU_RUNTIME() <ECU_LE32_TO_CPU_RUNTIME>`
    - :ecudoxygen:`ECU_LE64_TO_CPU_COMPILETIME() <ECU_LE64_TO_CPU_COMPILETIME>`
    - :ecudoxygen:`ECU_LE64_TO_CPU_RUNTIME() <ECU_LE64_TO_CPU_RUNTIME>`

The images below depict this:

.. figure:: /images/endian/endian_bus_to_cpu_no_macro.svg
  :width: 400
  :align: center
  
  Without ECU Macro

.. figure:: /images/endian/endian_bus_to_cpu_with_macro.svg
  :width: 500
  :align: center

  With ECU Macro

Raw little endian data will be misinterpreted on big endian machines. A common
solution is to manually swap bytes, however this makes the application unportable.
It will now break when using a little endian CPU. 

Instead, the ECU endian macros should be used. They will only swap bytes when
necessary, otherwise they will do nothing. In this example, the ECU macros will 
swap bytes if compiling on a big endian CPU. They will do nothing if compiling 
on a little endian CPU, thus making your application fully portable. In 
pseudocode this would look something like:

.. code-block:: c

    #include "ecu/attributes.h"
    #include "ecu/endian.h"

    /* CANOpen is little endian protocol. */
    struct canopen_rx_pdo1
    {
        uint16_t switches;
        uint16_t height;
        uint32_t direction;
    } ECU_ATTRIBUTED_PACKED;

    void foo(void)
    {
        struct canopen_tx_pdo1 msg;
        can_bus_receive(&msg); /* Receive little endian encoded data, byte by byte. */

        /* Data will always be correctly interpreted.
        Macros do nothing if you are compiling on little endian machine.
        Macros will swap bytes if you are compiling on a big endian machine. */
        uint16_t switch_data    = ECU_LE16_TO_CPU_RUNTIME(msg.switches);
        uint16_t height_data    = ECU_LE16_TO_CPU_RUNTIME(msg.height);
        uint32_t direction_data = ECU_LE32_TO_CPU_RUNTIME(msg.direction);
    }


Receiving Big Endian Data 
=================================================
.. _receiving_be_data:

Same logic as :ref:`Receiving Little Endian Data Section <receiving_le_data>`. 
However this set of macros is for reading big endian encoded data. Bytes will 
be swapped if compiling on a little endian CPU. Nothing will be done if compiling 
on a big endian CPU.

    - :ecudoxygen:`ECU_BE16_TO_CPU_COMPILETIME() <ECU_BE16_TO_CPU_COMPILETIME>`
    - :ecudoxygen:`ECU_BE16_TO_CPU_RUNTIME() <ECU_BE16_TO_CPU_RUNTIME>`
    - :ecudoxygen:`ECU_BE32_TO_CPU_COMPILETIME() <ECU_BE32_TO_CPU_COMPILETIME>`
    - :ecudoxygen:`ECU_BE32_TO_CPU_RUNTIME() <ECU_BE32_TO_CPU_RUNTIME>`
    - :ecudoxygen:`ECU_BE64_TO_CPU_COMPILETIME() <ECU_BE64_TO_CPU_COMPILETIME>`
    - :ecudoxygen:`ECU_BE64_TO_CPU_RUNTIME() <ECU_BE64_TO_CPU_RUNTIME>`


Sending Little Endian Data 
=================================================
.. _sending_le_data:

The following macros always store data in little endian encoded format, regardless 
of your CPU's endianness.

    - :ecudoxygen:`ECU_CPU_TO_LE16_COMPILETIME() <ECU_CPU_TO_LE16_COMPILETIME>`
    - :ecudoxygen:`ECU_CPU_TO_LE16_RUNTIME() <ECU_CPU_TO_LE16_RUNTIME>`
    - :ecudoxygen:`ECU_CPU_TO_LE32_COMPILETIME() <ECU_CPU_TO_LE32_COMPILETIME>`
    - :ecudoxygen:`ECU_CPU_TO_LE32_RUNTIME() <ECU_CPU_TO_LE32_RUNTIME>`
    - :ecudoxygen:`ECU_CPU_TO_LE64_COMPILETIME() <ECU_CPU_TO_LE64_COMPILETIME>`
    - :ecudoxygen:`ECU_CPU_TO_LE64_RUNTIME() <ECU_CPU_TO_LE64_RUNTIME>`

The images below depict this:

.. figure:: /images/endian/endian_cpu_to_bus_no_macro.svg
  :width: 500
  :align: center
  
  Without ECU Macro

.. figure:: /images/endian/endian_cpu_to_bus_with_macro.svg
  :width: 500
  :align: center

  With ECU Macro

The stored value's bytes must be swapped on a big endian machine before 
sending data byte-by-byte over a little endian bus. A common solution is 
to do this manually, however this makes the application unportable. It will 
now break when using a little endian CPU. 

Instead, the ECU endian macros should be used. They will only swap bytes when
necessary, otherwise they will do nothing. In this example the ECU macros will 
swap bytes if compiling on a big endian CPU. They will do nothing if compiling 
on a little endian CPU, thus making your application fully portable. In 
pseudocode this would look something like:

.. code-block:: c

    #include "ecu/attributes.h"
    #include "ecu/endian.h"

    /* CANOpen is little endian protocol. */
    struct canopen_tx_pdo1
    {
        uint16_t temperature;
        uint16_t speed;
        uint32_t acceleration;
    } ECU_ATTRIBUTED_PACKED;

    static void setup_tx_pdo1(struct canopen_tx_pdo1 *pdo, uint16_t temperature, uint16_t speed, uint32_t acceleration)
    {
        pdo->temperature    = ECU_CPU_TO_LE16_RUNTIME(temperature);
        pdo->speed          = ECU_CPU_TO_LE16_RUNTIME(speed);
        pdo->acceleration   = ECU_CPU_TO_LE32_RUNTIME(acceleration);
    }

    void foo()
    {
        uint8_t msg[8];

        struct canopen_tx_pdo1 pdo;
        setup_tx_pdo1(&pdo, 0x1234, 0x5678, 0x12345678);
        memcpy((void *)&msg, (const void *)&pdo, 8);

        /* Send data over the little endian CAN bus, byte by byte. 
        temperature ------- 0x34 0x12 
        speed ------- 0x78 0x56 
        acceleration ------ 0x78 0x56 0x34 0x12 */
        can_bus_send(&msg); 
    }


Sending Big Endian Data 
=================================================
.. _sending_be_data:

Same logic as :ref:`Sending Little Endian Data Section <sending_le_data>`. 
However this set of macros always stores data in big endian format, and 
is meant for sending data across a big endian bus. Bytes will be swapped 
if compiling on a little endian CPU. Nothing will be done if compiling 
on a big endian CPU.

    - :ecudoxygen:`ECU_CPU_TO_BE16_COMPILETIME() <ECU_CPU_TO_BE16_COMPILETIME>`
    - :ecudoxygen:`ECU_CPU_TO_BE16_RUNTIME() <ECU_CPU_TO_BE16_RUNTIME>`
    - :ecudoxygen:`ECU_CPU_TO_BE32_COMPILETIME() <ECU_CPU_TO_BE32_COMPILETIME>`
    - :ecudoxygen:`ECU_CPU_TO_BE32_RUNTIME() <ECU_CPU_TO_BE32_RUNTIME>`
    - :ecudoxygen:`ECU_CPU_TO_BE64_COMPILETIME() <ECU_CPU_TO_BE64_COMPILETIME>`
    - :ecudoxygen:`ECU_CPU_TO_BE64_RUNTIME() <ECU_CPU_TO_BE64_RUNTIME>`


Detecting Endianness
=================================================
ECU attempts to automatically detect the endianness of your target by using 
the CMake variable `CMAKE_C_BYTE_ORDER <https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_BYTE_ORDER.html>`_.
If this is unsuccessful, a warning message will be printed in the console at 
configuration time. A compilation error will also occur if this module is 
used in this case:

.. figure:: /images/endian/endian_warning_message.PNG
  :width: 400
  :align: center

  Warning Message

If auto-detection fails or you are not using a CMake build system you can 
manually specify the endianness of your target by passing ``-D ECU_LITTLE_ENDIAN`` 
or ``-D ECU_BIG_ENDIAN`` to the preprocessor.


Byte Swapping Macros
=================================================
This module also provides general-purpose byte swapping macros. The macros for 
compile-time byte swapping are:

    - :ecudoxygen:`ECU_SWAP16_COMPILETIME() <ECU_SWAP16_COMPILETIME>`
    - :ecudoxygen:`ECU_SWAP32_COMPILETIME() <ECU_SWAP32_COMPILETIME>`
    - :ecudoxygen:`ECU_SWAP64_COMPILETIME() <ECU_SWAP64_COMPILETIME>`

The functions for run-time byte swapping are:

    - :ecudoxygen:`ecu_swap16_runtime() <ecu_swap16_runtime>`
    - :ecudoxygen:`ecu_swap32_runtime() <ecu_swap32_runtime>`
    - :ecudoxygen:`ecu_swap64_runtime() <ecu_swap64_runtime>` 

For example:

.. code-block:: c

    #include "ecu/endian.h"

    static const uint16_t val = ECU_SWAP16_COMPILETIME(0x1234);

    int main()
    {
        uint32_t val2 = ecu_swap32_runtime(0x12345678);
        return 0;
    }

.. note:: 

    These macros will **always** swap bytes, reglardless of the endianness 
    of your CPU.

    Use the macros explained in the previous sections if you wish to selectively
    swap bytes based on your CPU's endianness.


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/endian_8h>
