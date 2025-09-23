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
    
Provides wrappers for compiler-specific attributes, allowing them to be 
modularly used within application code.

.. warning::

    Currently only GCC is supported. This module will treat all attributes as unsupported
    when using any other compiler.


Theory
=================================================

Attribute Overview
-------------------------------------------------
An attribute is a compiler extension (not apart of the C standard) used to convey
specific information. For example, this informs GCC that variable foo is potentially
unused. Therefore unused warnings should not be emitted for it:

.. code-block:: c

    static int foo __attribute__((unused)) = 0;

Attributes are unique to each compiler, making the example above unportable.
Code blocks like the one above would have to be manually changed if 
a different compiler that does not support the unused attribute is ever used.
Unfortunately, a common solution is to riddle the code with #ifdefs in order to
support both configurations:

.. code-block:: c

    #ifdef __GNUC__
    static int foo __attribute__((unused)) = 0;
    #else
    static int foo = 0;
    #endif

This module offers a portable solution by providing wrapper definitions that expand
differently depending on the target compiler being used:

.. code-block:: c

    static int foo ECU_ATTRIBUTE_UNUSED = 0;

Critical vs Non-critical Attributes
-------------------------------------------------
This module groups attributes into two categorites: ``critical`` and ``non-critical``.

- ``critical``: these attributes effect the functionality of the program. Critical 
  attributes not supported by the target compiler are **not** defined in order to produce 
  a compilation error via unresolved symbols:

    .. code-block:: c

        struct ECU_ATTRIBUTE_PACKED foo
        {
            char a;
            int c;
            char b;
        };
  
  The packed attribute in the example above is critical because it changes the memory
  layout of struct foo. The program expects foo to be packed and would be invalidated when 
  using compilers that do not support this attribute. If supported, ECU_ATTRIBUTE_PACKED 
  expands to the compiler-specific packed attribute and the program compiles. Otherwise ECU_ATTRIBUTE_PACKED 
  is not defined and a compilation error arises due ECU_ATTRIBUTE_PACKED being an undefined symbol.

- ``non-critical``: these attributes do not effect the functionality of the program. 
  Empty definitions are created for non-critical attributes that are not supported 
  by the target compiler, which allows the program to still compile:

    .. code-block:: c

        /* Expands to static int foo = 0; if attribute unsupported. */
        static int foo ECU_ATTRIBUTE_UNUSED = 0;

  The unused attribute in the example above does not effect the functionality of the program.
  If supported, ECU_ATTRIBUTE_UNUSED expands to the compiler-specific unused attribute. 
  Otherwise an empty definition is created and ECU_ATTRIBUTE_UNUSED expands to nothing, 
  allowing the program to still compile.

API 
=================================================
.. toctree::
    :maxdepth: 1

    attributes.h </doxygen/html/attributes_8h>

Critical Attributes
-------------------------------------------------

ECU_ATTRIBUTE_PACKED
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
If attached to a variable, informs compiler it should have the smallest possible 
alignment. If attached to a type definition, informs compiler the minimum required 
memory should be used to represent the type:

.. code-block:: c

    struct ECU_ATTRIBUTE_PACKED foo
    {
        char a;
        int c;
        char b;
    };

ECU_ATTRIBUTE_SECTION()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Places symbol in user-specified program section:

.. code-block:: c

    uint32_t vector_table[] ECU_ATTRIBUTE_SECTION(".vectors") = {.....};

Non-critical Attributes
-------------------------------------------------

ECU_ATTRIBUTE_UNUSED
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Informs compiler that type, variable, or function may be unused so it does 
not emit an unused warning:

.. code-block:: c

    static int foo ECU_ATTRIBUTE_UNUSED = 0;
