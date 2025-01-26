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


List vs Node
=================================================
Lists (:ecudoxygen:`ecu_dlist`) are represented as delimiter HEAD nodes. 
Lists are comprised of user-defined nodes (:ecudoxygen:`ecu_dnode`). The photos
below illustrate this concept:

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

HEAD nodes do not contain user data - they are solely used as delimiters.
Node member functions (ecu_dnode\_....) act on a specific node.
List member functions (ecu_dlist\_....) act on the entire list.

For example :ecudoxygen:`ecu_dnode_insert_before() <ecu_dnode_insert_before>` 
acts on a single node. The function inserts the supplied node at the specified position.
Whereas :ecudoxygen:`ecu_dlist_insert_before() <ecu_dlist_insert_before>` acts on the 
entire list. The entire list is iterated over and the supplied node is inserted at the
location specified by the condition function.


Storing Node Data 
=================================================
Lists are intrusive so user-defined data is stored within the node.

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
    };

    struct ecu_dlist list;
    struct user_node node1;
    struct user_node node2;

    /* Pass dlist node to API. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dnode_insert_before(&node2.node, &node1.node);


Getting Node Data
=================================================
ECU_DNODE_GET_ENTRY().
TODO show picture of how offset math works.

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


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/dlist_8h>
