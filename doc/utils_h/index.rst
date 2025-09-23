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

Miscellaneous helpers.

API
=================================================
.. toctree:: 
    :maxdepth: 1

    utils.h </doxygen/html/utils_8h>

Macros
-------------------------------------------------

ECU_CONTAINER_OF()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _utils_container_of:

Converts an intrusive member back into the data type that stores it.
A common use case for this macro is for intrusive nodes. 

The following example uses a linked list node API that can only work with
linked list node types to remain portable. This macro
allows the linked list node to be converted back into the user's node
type, allowing any data to be stored without dynamic memory allocation:

.. code-block:: c

    /*------------ Linked List API - linked_list.h ------------*/
    struct linked_list_node
    {
        /* Members specific to data structure... */
    };

    /* API functions can only work with linked_list_node type to remain portable. */
    extern struct linked_list_node *linked_list_next(struct linked_list_node *n);

    /*-------------------- User's main.c ----------------------*/
    #include "linked_list.h"

    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Make linked_list_node intrusive so we can store any data. */
        struct linked_list_node node;

        /* More user data. */
        int c;
    };

    /* Make user node. Assume everything is initialized already
    and it's in a list. Use linked list API. Convert returned 
    value back into user's node type. */
    struct user_node n1;
    struct linked_list_node *next = linked_list_next(&n1.node);
    struct user_node *me = ECU_CONTAINER_OF(next, struct user_node, node);
    me->a = 5;
    me->b = 5;
    me->c = 5;

This macro takes in three parameters:

    #. ``ptr_`` = Pointer to intrusive member. In this case, ``next``.
    #. ``type_`` = User's data type containing the intrusive member. In this case, ``struct user_node``.
    #. ``member_`` = Name of intrusive member within the user's type. In this case, ``node``.

.. figure:: /images/utils/ecu_container_of.svg
  :width: 500
  :align: center
  
  Container Of Macro Parameters

The conversion is performed by doing pointer arithmetic. The macro expands to the following,
with each step being fully explained below:

.. figure:: /images/utils/ecu_container_of_steps.svg
  :width: 500
  :align: center
  
  Container Of Macro Steps

#.  The supplied pointer is subtracted by an offset to convert it back into
    the user's type. The value of this offset is 'X':

    .. figure:: /images/utils/ecu_container_of_offset.svg
        :width: 500
        :align: center
        
        Container Of Macro Pointer Arithmetic

    In pseudocode this yields:

    .. code-block:: c

        struct user_node *me = next - X;

    The supplied pointer is casted to (uint8_t \*) so it is properly decremented, updating the 
    expression to:

    .. code-block:: c

        struct user_node *me = (uint8_t *)next - X;

    The offset 'X' can be determined at compile-time using `offsetof() <https://en.cppreference.com/w/c/types/offsetof>`_.
    offsetof() is mandated by the C standard since C89, so this is guaranteed to be 
    a portable solution. Substituting this value in for 'X' completes the first portion
    of the expression:

    .. code-block:: c

        struct user_node *me = (uint8_t *)next - offsetof(struct user_node, node);

#.  The result of the previous step is casted to void to get rid of the following compiler warning: 

    .. figure:: /images/utils/cast_align_warning.png
        :width: 500
        :align: center
        
        Cast Align Warning

    This warning is triggered whenever a pointer cast results in increased alignment requirements.
    In this case, casting (uint8_t \*) to (struct user_node \*) triggers the warning because 
    (struct user_node \*) must be aligned on a greater byte boundary than (uint8_t \*). 
   
    Under normal circumstances, this cast is illegal and this warning must be addressed. 
    **However for this use case, the cast is always a safe operation.** When a (struct user_node) 
    object is defined, the compiler allocates memory **starting 
    at an address that satisfies the alignment requirements of type (struct user_node)**. 
    The CONTAINER_OF() macros return this raw starting address, which is guaranteed to be aligned 
    according the requirements of (struct user_node) since this was originally done by the compiler.
    Therefore the updated expression becomes:

    .. code-block:: c

        struct user_node *me = ((void *)((uint8_t *)next - offsetof(struct user_node, node)));

#.  The final step casts from void back to the user's node type. In this case, (void \*)
    to (struct user_node \*), thus completing the expression:

    .. code-block:: c

        /* struct user_node *me = ECU_CONTAINER_OF(next, struct user_node, node); */
        struct user_node *me = ((struct user_node *)(void *)((uint8_t *)next - offsetof(struct user_node, node)));

ECU_CONST_CONTAINER_OF()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _utils_const_container_of:

Const-qualified version of :ref:`ECU_CONTAINER_OF() <utils_container_of>`.

ECU_FIELD_SIZEOF()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns the size of a member within a struct or union **declaration** at compile-time.

.. code-block:: c 

    struct my_struct
    {
        uint16_t a;
        uint32_t b;
    };

    ECU_FIELD_SIZEOF(struct my_struct, a); /* Returns 2 at compile-time. */
    ECU_FIELD_SIZEOF(struct my_struct, b); /* Returns 4 at compile-time. */

ECU_IS_BASE_OF()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Verifies, at compile-time, that derived class correctly
inherits base class via C-style inheritance. Returns true
if this condition is satisfied. False otherwise:

.. code-block:: c 

    struct my_base
    {
        int a;
    };

    struct my_derived_correct
    {
        struct my_base base; /* Use C-style inheritance by declaring my_base as FIRST member. */
        int b;
        int c;
    };

    struct my_derived_incorrect
    {
        int b;
        struct my_base base;
        int c;
    };

    /* Passes. */
    ECU_STATIC_ASSERT( (ECU_IS_BASEOF(base, struct my_derived_correct)), "struct my_base must be first member.");

    /* Compilation error. */
    ECU_STATIC_ASSERT( (ECU_IS_BASEOF(base, struct my_derived_incorrect)), "struct my_base must be first member.");

ECU_IS_SIGNED()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Verifies, at compile-time, that supplied type is signed. Returns true
if this condition is satisfied. False otherwise:

.. code-block:: c 

    /* Passes. */
    ECU_STATIC_ASSERT( (ECU_IS_SIGNED(int8_t)), "type must be signed." );
    ECU_STATIC_ASSERT( (ECU_IS_SIGNED(float)), "type must be signed." );

    /* Fails. */
    ECU_STATIC_ASSERT( (ECU_IS_SIGNED(uint8_t)), "type must be signed." );

ECU_IS_UNSIGNED()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Verifies, at compile-time, that supplied type is unsigned. Returns true
if this condition is satisfied. False otherwise:

.. code-block:: c 

    /* Passes. */
    ECU_STATIC_ASSERT( (ECU_IS_UNSIGNED(uint8_t)), "type must be unsigned." );

    /* Fails. */
    ECU_STATIC_ASSERT( (ECU_IS_UNSIGNED(int8_t)), "type must be unsigned." );
    ECU_STATIC_ASSERT( (ECU_IS_UNSIGNED(float)), "type must be unsigned." );
