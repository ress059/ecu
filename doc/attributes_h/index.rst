.. _attributes_h:

attributes.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.
    
Provides wrappers for compiler-specific attributes so they can be modularly used within
application code. The benefits of using these wrappers can be seen below:

.. code-block:: c

    /*----------------------- file .c -----------------------*/
    #include "ecu/attributes.h"

    /* If toolchain ever changes only ECU_ATTRIBUTE_UNUSED define will need 
    to be updated. Rest of application code using this define can remain the 
    same. ECU attempts to automatically update these defines based on your 
    toolchain. */
    static int ECU_ATTRIBUTE_UNUSED var = 5;


.. code-block:: c

    /*----------------------- file .c -----------------------*/

    /* No wrapper is used so not modular. If a toolchain that does not support
    this attribute is used, all directives like this will have to be manually
    changed throughout the application code. If you want to have backwards compatibility
    between toolchains, you would have to use #ifdefs at every attribute call. */
    static int __attribute__((unused)) var = 5;


.. warning:: 

    At this time only GCC is supported for this module.


What If My Toolchain Is Not Supported By This Module?
=======================================================
For all ``non-critical`` attributes, ECU will create an empty #define directive so your code
will still compile. Definitions will NOT be provided for ``critical`` attributes, so you will get a 
compilation error wherever they are used in the application.

``Critical`` attributes are ones that effect code functionality. For example :ecudoxygen:`ECU_ATTRIBUTE_PACKED`, which 
effects the memory layout of structs. 

``Non-critical`` attributes are ones that do not effect code functionality. For example :ecudoxygen:`ECU_ATTRIBUTE_UNUSED`,
which just suppresses unused warnings but does not change the code executable.

.. code-block:: c

    /*----------------------- file .c -----------------------*/
    #include "ecu/attributes.h" /* Assume you are using an unsupported toolchain for this example. */

    /* ECU_ATTRIBUTE_UNUSED is non-critical so it expands to nothing. This code
    will still compile even if using an unsupported toolchain. */
    static int ECU_ATTRIBUTE_UNUSED var = 5;

    /* ECU_ATTRIBUTE_PACKED is critical to code's functionality so no definition 
    is provided if using an unsupported toolchain. Code like this will NOT compile,
    assuming a symbol with this exact name is not defined anywhere else. */
    struct ECU_ATTRIBUTE_PACKED my_data
    {
        int x;
        int y;
        int z;
    };


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/attributes_8h>
