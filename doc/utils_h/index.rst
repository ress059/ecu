.. _utils_h:

utils.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

Miscellaneous helper macros.


Macros
=================================================


ECU_FIELD_SIZEOF()
-------------------------------------------------
Returns the size of a member within a struct or union at compile-time.

.. code-block:: c 

    struct my_struct
    {
        uint16_t a;
        uint32_t b;
    };

    ECU_FIELD_SIZEOF(struct my_struct, a); /* Returns 2 at compile-time. */
    ECU_FIELD_SIZEOF(struct my_struct, b); /* Returns 4 at compile-time. */


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/utils_8h>
