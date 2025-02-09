.. _dlist_h:

dlist.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

Intrusive, doubly-linked list. Lists can store any user-defined types without 
dynamic memory allocation. Different types can be stored and identified in the 
same list using :ecudoxygen:`a node ID <ecu_dnode::id>`.


List And Node Structure
=================================================
Lists are delimeter HEAD nodes that do not contain any user data.
They are represented by the :ecudoxygen:`ecu_dlist` structure.

Nodes are user-defined and are attached to lists. They are represented
by the :ecudoxygen:`ecu_dnode` structure. 

Therefore a list consists of only one :ecudoxygen:`ecu_dlist` and any 
number of :ecudoxygen:`ecu_dnodes <ecu_dnode>`.

.. figure:: /images/dlist/dlist_empty_list.svg
  :width: 400
  :align: center
  
  Empty List

.. figure:: /images/dlist/dlist_list_with_one_node.svg
  :width: 400
  :align: center
  
  List With One Node

.. figure:: /images/dlist/dlist_list_with_two_nodes.svg
  :width: 400
  :align: center
  
  List With Two Nodes


Storing Node Data 
=================================================
.. _dlist_storing_node_data:

Lists are intrusive. Custom data is stored within the node by containing 
an :ecudoxygen:`ecu_dnode` member in a user-defined type. This 
:ecudoxygen:`ecu_dnode` member is passed to the API.

.. code-block:: c 

    #include "ecu/dlist.h"

    /* User-defined data is apart of the dlist node. */
    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Dlist node. */
        struct ecu_dnode node;

        /* More user data. */
        int c;
    };

    struct ecu_dlist list;
    struct user_node node1;
    struct user_node node2;

    /* Pass ecu_dnode member to API to add/remove your node from a list. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dnode_insert_before(&node2.node, &node1.node);


Getting Node Data
=================================================
As explained in the :ref:`previous section <dlist_storing_node_data>`, custom data is 
stored within a node by containing an :ecudoxygen:`ecu_dnode` member in a user-defined 
type.

The :ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` macros
convert an :ecudoxygen:`ecu_dnode` struct back into the original user-defined type, 
allowing custom data to be retrieved.

.. code-block:: c 

    #include "ecu/dlist.h"

    /* User-defined data is apart of the dlist node. */
    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Dlist node. */
        struct ecu_dnode node;

        /* More user data. */
        int c;
    };

    struct ecu_dlist list;
    struct ecu_dlist_iterator iterator;
    struct user_node node1;
    struct user_node node2;

    /* Create list of user_node types. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node2.node);

    /* ..... */
    /* ..... */

    /* At a later time iterate over list. Retrieve custom data from ecu_dnode. */
    for (struct ecu_dnode *i = ecu_dlist_iterator_begin(&iterator, &list);
         i != ecu_dlist_iterator_end(&itertor);
         i = ecu_dlist_iterator_next(&iterator))
    {
        struct user_node *me = ECU_DNODE_GET_ENTRY(i, struct user_node, node);
        me->a = 5;
        me->b = 5;
        me->c = 5;
    }

:ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` take in three parameters:

    1. ptr\_ = Pointer to ecu_dnode.
    2. type\_ = User-defined type.
    3. member\_ = Name of ecu_dnode member within the user-defined type.

.. figure:: /images/dlist/dlist_dnode_get_entry.svg
  :width: 500
  :align: center
  
  Get Entry Macro Parameters

Arithmetic is done on the supplied pointer to convert it back to the original 
user-defined type. Consider the following:

.. code-block:: c 

    struct user_node 
    {
        int a;
        int b;
        struct ecu_dnode node;
        int c;
    };

    /* Pseudocode. For this example define a user_node called 'me'. 
    &me.node would be supplied to API functions. &me.node would be
    the pointer value returned by iterators. */
    struct user_node me;
    addr = ECU_DNODE_GET_ENTRY(&me.node, struct user_node, node);

.. figure:: /images/dlist/dlist_dnode_get_entry_offset.svg
  :width: 400
  :align: center

(struct ecu_dnode \*)&me.node would be supplied to API functions, and also 
be returned by iterators. Supplying this pointer into the GET_ENTRY macros,
converts it back into (struct user_node \*)&me. 

Subtracting the supplied pointer by the offset 'X' makes this possible.
This offset is determined at compile-time using `offsetof() <https://en.cppreference.com/w/c/types/offsetof>`_.

.. math:: 

    addr = &me.node - X;
    addr = &me.node - offsetof(struct user_node node);

offset() is mandated by the C standard since C99, so this is guaranteed to be 
a portable solution.

The supplied pointer is casted to (uint8_t \*) so it is propertly decremented. The
expression becomes:

.. math:: 

    addr = (uint8_t \*)&me.node - offsetof(struct user_node node);

The decremented (uint8_t \*) pointer is then casted back to the user-defined type to 
complete the conversion. 

.. math:: 

    addr = (struct user_node \*)((uint8_t \*)&me.node - offsetof(struct user_node node));

Casting a memory address represented as a (uint8_t \*) back to a (struct user_node \*) will 
always be a safe operation in this use case. The value of 'addr' will always be properly aligned
according to the alignment requirements of (struct user_node).


!!!!!!!!!! TODO STOPPED HERE
.. math:: 

    (struct user_node \*)addr == &me;

The raw memory address points to an object of type (struct user_node \*). In this case:

When an object of type (struct user_node) is defined, the compiler will always align its 
memory address according to the alignment requirements of (struct user_node).

.. code-block:: c

    /* &me is always aligned according to the alignment requirements of (struct user_node). */
    struct user_node me;

    &me == ECU_DNODE_GET_ENTRY(&me.node, struct user_node, node)


This same logic applied for any user-defined type, which makes :ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>`
and :ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` always safe, assuming
the supplied parameters are correct.


!!!! TODO
ECU_DNODE_GET_CONST_ENTRY() does the same thing, but all casts are const qualified.





Explain how node ID can be used to identify different type stored in the same list.


Node Insert and Removal
=================================================
nodes cannot be shared across multiple lists.
Nodes must be apart of a list.

Allows nodes to be inserted at specific locations within the list.

TODO show example with picture of insert_before() and insert_after().

The position node must be within a list. TODO show pic of illegal behavior (position node not in list).


List Insert and Remove
=================================================


List Sort
=================================================


Iterators
=================================================
Iterators belong to a list rather than an individual node. TODO
Differs from STL as having an iterator to a node not in a list invalidates
the iterator. Attempting to use it causes an assertion.


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/dlist_8h>
