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
Lists are represented by the :ecudoxygen:`ecu_dlist` struct. Nodes are represented by
the :ecudoxygen:`ecu_dnode` struct.

Lists are dummy delimeters used as anchors. Therefore :ecudoxygen:`ecu_dlist::head` is 
not apart of the user's list. Nodes are user-defined and inserted into lists. 

.. figure:: /images/dlist/dlist_empty_list.svg
  :width: 300
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
an :ecudoxygen:`ecu_dnode` member in a user-defined type. This :ecudoxygen:`ecu_dnode` 
member is passed to the API. For example:

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

    /* Pass ecu_dnode member to API to add/remove a node from a list. */
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

    /* Pseudocode. For this example, define a user_node called 'me'. 
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

The supplied pointer is casted to (uint8_t \*) so it is properly decremented. The
expression becomes:

.. math:: 

    addr = (uint8_t \*)&me.node - offsetof(struct user_node node);

The decremented (uint8_t \*) pointer is then casted back to the user-defined type to 
complete the conversion. 

.. math:: 

    addr = (struct user_node \*)((uint8_t \*)&me.node - offsetof(struct user_node node));

Casting a memory address represented as a (uint8_t \*) back to a (struct user_node \*) will 
always be a safe operation in this use case. The value of 'addr' will always be properly aligned
according to the alignment requirements of (struct user_node) because the compiler will properly 
align the (struct user_node) object when it is defined.

:ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` performs the same operations
but all casts are const qualified. A const-qualified type is also returned.


Node Insert and Removal
=================================================
:ecudoxygen:`ecu_dnode_insert_before() <ecu_dnode_insert_before>` and 
:ecudoxygen:`ecu_dnode_insert_after() <ecu_dnode_insert_after>` allow nodes to be 
inserted into a list, without needed the list object.

The code example below initializes a list of 2 nodes. 2 more nodes are added using the
node insert functions.

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
    struct user_node node3;
    struct user_node node4;

    /* Iniitalize list of 2 nodes. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node4.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node3.node);

    /* ..... */
    /* ..... */

    /* Insert more nodes to list. */
    ecu_dnode_insert_before(&node2.node, &node3.node);
    ecu_dnode_insert_after(&node4.node, &node3.node);

.. figure:: /images/dlist/dnode_insert.svg
  :width: 600
  :align: center

Similarly, nodes can be removed using :ecudoxygen:`ecu_dnode_remove() <ecu_dnode_remove>`.
Building off the previous code example, node1 is removed:

.. code-block:: c 

    ecu_dnode_remove(&node1.node);

.. figure:: /images/dlist/dnode_remove.svg
  :width: 600
  :align: center

Inserting a node requires the following:

- Node cannot be within a list.
- Specified position node must be within a list.

Thus the following are illegal operations:

.. figure:: /images/dlist/dnode_illegal_insert_remove.svg
  :width: 600
  :align: center

Node1 is already in a list and Node2 is not within a list, making the attempted
insert illegal. The attempted removal is illegal since Node2 is not within a list.


Node ID
=================================================
An ID can be assigned to a node when it is constructed. It allows different 
user-defined types to be stored and identified in the same list. For example:

.. code-block:: c

    #include "ecu/dlist.h"

    /* User-defined object IDs. */
    enum user_object_ids
    {
        TYPE1 = ECU_USER_OBJECT_ID_BEGIN,
        TYPE2,
        TYPE3
    };

    /* Data types of nodes stored in linked list. */
    struct type1 
    {
        int a;
        struct ecu_dnode node;
    };

    struct type2 
    {
        struct ecu_dnode node;
        int b;
    };

    struct type3 
    {
        int c;
        struct ecu_dnode node;
        int d;
    };

    struct ecu_dlist_iterator iterator;
    struct ecu_dlist list;
    struct type1 node1;
    struct type2 node2;
    struct type3 node3;

    /* Construct list and nodes. Assign object IDs to each node to identify
    their data types. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, TYPE1);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, TYPE2);
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, TYPE3);

    /* Add nodes to list. */
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node2.node);
    ecu_dlist_push_back(&list, &node3.node);

    /* Iterate over list. Use object ID to identify the data type stored in each node. */
    ECU_DLIST_FOR_EACH(i, &iterator, &list)
    {
        switch (ecu_dnode_get_id(i))
        {
            case TYPE1:
            {
                struct type1 *me = ECU_DNODE_GET_ENTRY(i, struct type1, node);
                me->a = 5;
                break;
            }

            case TYPE2:
            {
                struct type2 *me = ECU_DNODE_GET_ENTRY(i, struct type2, node);
                me->b = 10;
                break;
            }

            case TYPE3:
            {
                struct type3 *me = ECU_DNODE_GET_ENTRY(i, struct type3, node);
                me->c = 15;
                me->d = 20;
                break;
            }
        }
    }


Node Destruction
=================================================
A node is destroyed when it is passed to :ecudoxygen:`ecu_dnode_destroy() <ecu_dnode_destroy>` or 
when the node is within a list that is destroyed by :ecudoxygen:`ecu_dlist_destroy() <ecu_dlist_destroy>`.

Additional cleanup that is application-specific may be necessary for a user-defined node.
For example a node may contain data that is on the heap, requiring it to be explicitly
freed when the node is destroyed.

An optional destroy callback can be suplied to the node's constructor in order to define
this behavior. This callback will execute when the node is destroyed. For example:

.. code-block:: c

    #include <stdlib.h>
    #include "ecu/asserter.h"
    #include "ecu/dlist.h"

    struct user_node 
    {
        ecu_dnode node;
        void *heap_obj;
    };

    static void destroy(struct ecu_dnode *n, ecu_object_id id)
    {
        /* Define additional cleanup for user_node type. DO NOT USE API CALLS
        THAT EDIT THE ECU_DNODE (node insert, remove, etc). */
        ECU_RUNTIME_ASSERT( (n) );
        (void)id;

        struct user_node *me = ECU_DNODE_GET_ENTRY(n, struct user_node, node);
        free(me->heap_obj);
    }

    /* Construct node. Supply custom destroy callback and allocate heap. */
    struct user_node node1;
    ecu_dnode_ctor(&node1.node, &destroy, ECU_OBJECT_ID_UNUSED);
    node1.heap_obj = malloc(20);

    /* ..... */
    /* ..... */

    /* At a later time node is destroyed. Node destructor calls destroy() 
    function, freeing heap_obj. No additional intervention needed. */
    ecu_dnode_destroy(&node1);

.. warning:: 

    This module cleans up the :ecudoxygen:`ecu_dnode` and removes it from the list.
    Do not use API calls that edit the ecu_dnode (node insert, remove, etc) within the 
    destroy callback. Doing so is undefined. 

This mechanism applies to many other scenarios besides heap allocation. For example,
a linked list of LEDs. When the node is destroyed, the LED can be turned off:

.. code-block:: c

    #include <stdlib.h>
    #include "ecu/asserter.h"
    #include "ecu/dlist.h"

    struct led 
    {
        ecu_dnode node;
        struct 
        {
            uint32_t port;
            uint32_t pin;
        } gpio;
    };

    static void led_destroy(struct ecu_dnode *n, ecu_object_id id)
    {
        /* Turn off LED when node is destroyed. */
        ECU_RUNTIME_ASSERT( (n) );
        (void)id;

        struct led *me = ECU_DNODE_GET_ENTRY(n, struct led, node);
        gpio_set_low(me->gpio.port, me->gpio.pin);
    }

    /* Construct node. Supply custom destroy callback. Turn LED on. */
    struct led led1;
    ecu_dnode_ctor(&led1.node, &led_destroy, ECU_OBJECT_ID_UNUSED);
    led1.gpio.port = PORTA;
    led1.gpio.pin = 5;
    gpio_set_high(led1.gpio.port, led1.gpio.pin);

    /* ..... */
    /* ..... */

    /* At a later time node is destroyed. Node destructor calls led_destroy() 
    function, turning LED off. No additional intervention needed. */
    ecu_dnode_destroy(&node1);


List Insert and Removal
=================================================
!!!!!!!!!!!!!!!!!! TODO Stopped here !!!!!
list_insert_before(), push_front(), push_back(), list_clear().


List Sort
=================================================

List Destruction
=================================================




Iterators
=================================================
Talk about DLIST_FOR_EACH() and DLIST_CONST_FOR_EACH().


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/dlist_8h>
