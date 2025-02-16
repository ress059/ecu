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
same list using :ref:`a node ID <dlist_node_id>`.


List And Node Structure
=================================================
Lists are represented by the :ecudoxygen:`ecu_dlist` struct. Nodes are represented by
the :ecudoxygen:`ecu_dnode` struct.

Lists are dummy delimeters used as anchors. Therefore :ecudoxygen:`ecu_dlist::head` is 
not apart of the user's list. Nodes are user-defined and inserted into lists. 

.. figure:: /images/dlist/dlist_dnode_structure.svg
  :width: 600
  :align: center
  
  List and Node Structure


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

    /* Pass ecu_dnode member to API to add/remove user_node from a list. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dnode_insert_before(&node2.node, &node1.node);


Getting Node Data
=================================================
As explained in the :ref:`Storing Node Data Section <dlist_storing_node_data>`, custom data is 
stored within a node by containing an :ecudoxygen:`ecu_dnode` member in a user-defined 
type. However this module can never directly reference the user's type, as it is specific,
to the application. This presents an obvious problem: how does the API use only an :ecudoxygen:`ecu_dnode`
structure while supporting any user-defined nodes?

The :ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` macros make this possible.
They convert an :ecudoxygen:`ecu_dnode` struct back into the original user-defined type, 
allowing custom data to be retrieved:

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

    /* At a later time iterate over list. API returns (struct ecu_dnode *).
    This can be converted back into (struct user_node *) to retrieve custom data. */
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

    1. ``ptr_`` = Pointer to ecu_dnode returned by API. In this case, ``i``.
    2. ``type_`` = User-defined node type. In this case, ``struct user_node``.
    3. ``member_`` = Name of ecu_dnode member within the user-defined type. In this case, ``node``.

.. figure:: /images/dlist/dnode_get_entry.svg
  :width: 500
  :align: center
  
  Get Entry Macro Parameters

Under the hood, these macros perform arithmetic on the supplied ecu_dnode pointer to convert 
it back into the original user-defined type. Consider the following:

.. code-block:: c 

    struct user_node 
    {
        int a;
        int b;
        struct ecu_dnode node;
        int c;
    };

    /* &me.node supplied and returned by API functions. GET_ENTRY() macros convert
    this back into (struct user_node *)&me. */
    struct user_node me;

.. figure:: /images/dlist/dnode_get_entry_offset.svg
  :width: 600
  :align: center

  Address Offsets In user_node

(struct ecu_dnode \*)&me.node would be supplied and returned by API functions, since the 
:ecudoxygen:`ecu_dnode` structure is common between the API and any user-defined node. 
Supplying this pointer into the GET_ENTRY() macros, converts it back into the user-defined type. 
In this case, (struct user_node \*)&me. 

The figure above shows that this can be accomplished by subtracting (struct ecu_node \*)&me.node 
by the offset 'X'. This offset is determined at compile-time using `offsetof() <https://en.cppreference.com/w/c/types/offsetof>`_.
offset() is mandated by the C standard since C99, so this is guaranteed to be 
a portable solution. Therefore the initial expression is:

.. code-block:: text

    (struct user_node *)&me == (struct ecu_dnode *)&me.node - X
    (struct user_node *)&me == (struct ecu_dnode *)&me.node - offsetof(struct user_node, node)

The supplied pointer is casted to (uint8_t \*) so it is properly decremented. The
expression becomes:

.. code-block:: text

    (struct user_node *)&me == (uint8_t *)&me.node - offsetof(struct user_node, node)

The decremented (uint8_t \*) pointer is then casted back into the user-defined type to 
complete the conversion, resulting in the final expression:

.. code-block:: text

    (struct user_node *)&me == (struct user_node *)((uint8_t *)&me.node - offsetof(struct user_node, node))

Doing (uint8_t \*) pointer arithmetic and casting it back into a (struct user_node \*) 
is illegal in most circumstances as it would increase the alignment requirements of the pointer.
However for this use case, it is **always** a safe operation. When object 'me' is defined,
the compiler allocates memory starting at an address that satisfies the alignment requirements 
of type (struct user_node). The GET_ENTRY() macros return this raw starting address, which is 
guaranteed to be aligned according the requirements of (struct user_node), since this was originally 
done by the compiler.

As a final note, :ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` performs 
the same operations explained above, but all casts are const qualified. A const-qualified type 
is also returned.


Node Insert and Removal
=================================================
:ecudoxygen:`ecu_dnode_insert_before() <ecu_dnode_insert_before>` and 
:ecudoxygen:`ecu_dnode_insert_after() <ecu_dnode_insert_after>` allows nodes to be 
inserted into a list, without needing to reference an :ecudoxygen:`ecu_dlist` object:

.. code-block:: c 

    #include "ecu/dlist.h"

    struct ecu_dlist list;
    struct ecu_dnode node1;
    struct ecu_dnode node2;
    struct ecu_dnode node3;
    struct ecu_dnode node4;

    /* Iniitalize list of 2 nodes. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node4, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Starting list = [node1, node3]. */ 
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node3);

    /* ..... */
    /* ..... */

    /* New list = [node1, node2, node3, node4]. */
    ecu_dnode_insert_before(&node2, &node3);
    ecu_dnode_insert_after(&node4, &node3);

.. figure:: /images/dlist/dnode_insert.svg
  :width: 600
  :align: center

  ecu_dnode_insert_before() and ecu_dnode_insert_after()

Similarly, nodes can be removed using :ecudoxygen:`ecu_dnode_remove() <ecu_dnode_remove>`.
Building off the previous code example, node2 is removed:

.. code-block:: c 

    ecu_dnode_remove(&node2);

.. figure:: /images/dlist/dnode_remove.svg
  :width: 600
  :align: center

  ecu_dnode_remove()

Inserting nodes in this manner requires the following:

- Node being inserted cannot be within a list.
- Specified position node must be within a list.

Thus some operations like these are illegal:

.. figure:: /images/dlist/dnode_illegal_insert_remove.svg
  :width: 600
  :align: center

  Some Illegal Dnode Operations

node1 is already in a list and node2 is not within a list, making the attempted
insertions illegal. The attempted removal is illegal since node2 is not within a list.


Node ID
=================================================
.. _dlist_node_id:

!!!!!!!!!!!!!!!!!!!!!!! TODO STOPED PROOFREADING HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!

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
.. _dlist_node_destruction:

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


List Insert
=================================================
:ecudoxygen:`ecu_dlist_push_front() <ecu_dlist_push_front>` adds a node to the front 
of the list, making it the new HEAD. Recall that :ecudoxygen:`ecu_dlist::head` is a dummy 
delimeter not apart of the user's list.

.. code-block:: c

    #include "ecu/dlist.h"

    struct ecu_dlist list;
    struct ecu_dnode node1;
    struct ecu_dnode node2;

    /* Start with list of node1. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Add node2 to front. */
    ecu_dlist_push_front(&list, &node2);

.. figure:: /images/dlist/dlist_push_front.svg
  :width: 600
  :align: center

  ecu_dlist_push_front()

:ecudoxygen:`ecu_dlist_push_back() <ecu_dlist_push_back>` adds a node to the back 
of the list, making it the new TAIL.

.. code-block:: c

    #include "ecu/dlist.h"

    struct ecu_dlist list;
    struct ecu_dnode node1;
    struct ecu_dnode node2;

    /* Start with list of node1. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Add node2 to back. */
    ecu_dlist_push_back(&list, &node2);

.. figure:: /images/dlist/dlist_push_back.svg
  :width: 600
  :align: center

  ecu_dlist_push_back()

:ecudoxygen:`ecu_dlist_insert_before() <ecu_dlist_insert_before>` iterates over the entire 
list, starting from HEAD. Each node in the iteration is passed to a user-defined condition 
function, where the user specifies if the node should be inserted at that location. The function 
exits as soon as the condition passes. If all conditions fail, the node is inserted to the 
back of the list:

.. code-block:: c

    #include "ecu/dlist.h"

    struct user_node 
    {
        struct ecu_dnode node;
        uint32_t val;
    };

    struct ecu_dlist list;
    struct user_node node1;
    struct user_node node2;
    struct user_node node3;

    /* node1 value = 1, node2 value = 2, node3 value = 3. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node1.val = 1;
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node2.val = 2;
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node3.val = 3;

    /* Start with list of node1 and node3. */
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node3.node);

    /* User-defined condition function. */
    static bool condition(const struct ecu_dnode *me, const struct ecu_dnode *position, void *obj)
    {
        bool status = false; /* Return true if condition passes. False otherwise. */
        ECU_RUNTIME_ASSERT( (me && position) );
        (void)obj; /* Optional callback object unused. */

        const struct user_node *me_node = ECU_DNODE_GET_CONST_ENTRY(me, struct user_node, node);
        const struct user_node *pos = ECU_DNODE_GET_CONST_ENTRY(pos, struct user_node, node);

        if (me_node->val <= pos->val)
        {
            /* Node should be inserted before position if its value is less than or equal position's value. */
            status = true;
        }

        return status;
    }
    
    /* New list becomes [node1, node2, node3] */
    ecu_dlist_insert_before(&list, &node2.node, &condition, ECU_DNODE_OBJ_UNUSED);

.. figure:: /images/dlist/dlist_insert_before.svg
  :width: 600
  :align: center

  ecu_dlist_insert_before()

Inserting a node requires that the node is not already within a list. Thus 
some operations like these are illegal:

.. figure:: /images/dlist/dlist_illegal_insert.svg
  :width: 600
  :align: center

  Illegal List Insert

node1 and node 2 cannot be inserted since they are already within a list.


List Sort
=================================================
:ecudoxygen:`ecu_dlist_sort() <ecu_dlist_sort>` does a merge sort over all nodes in the 
list. The sorting condition is specified by a user-defined callback:

.. code-block:: c

    #include "ecu/dlist.h"

    struct user_node 
    {
        struct ecu_dnode node;
        uint32_t val;
    };

    struct ecu_dlist list;
    struct user_node node1;
    struct user_node node2;
    struct user_node node3;
    struct user_node node4;

    /* node1 value = 1, node2 value = 2, node3 value = 3, node4 value = 4. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node1.val = 1;
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node2.val = 2;
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node3.val = 3;
    ecu_dnode_ctor(&node4.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node4.val = 4;

    /* Starting list = [node4, node1, node3, node2] */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node3);

    /* User-defined sort function. */
    static bool (*lhs_less_than_rhs)(const struct ecu_dnode *lhs, const struct ecu_dnode *rhs, void *data)
    {
        bool status = false; /* Return true if lhs node is less than rhs node. False otherwise. */
        ECU_RUNTIME_ASSERT( (lhs && rhs) );
        (void)obj; /* Optional callback object unused. */

        const struct user_node *lhs_node = ECU_DNODE_GET_CONST_ENTRY(lhs, struct user_node, node);
        const struct user_node *rhs_node = ECU_DNODE_GET_CONST_ENTRY(rhs, struct user_node, node);

        if (lhs_node->val < rhs_node->val)
        {
            status = true;
        }

        return status;
    }
    
    /* New sorted list becomes [node1, node2, node3, node4] */
    ecu_dlist_sort(&list);

.. figure:: /images/dlist/dlist_sort.svg
  :width: 600
  :align: center

  ecu_dlist_sort()


List Clear 
=================================================
:ecudoxygen:`ecu_dlist_clear() <ecu_dlist_clear>` removes all nodes from a list. The 
nodes' destroy callbacks are **not** called. The list and nodes are not destroyed, so 
they can all be re-used and passed to API calls without requiring reconstruction. 

.. code-block:: c

    #include "ecu/dlist.h"

    struct ecu_dlist list;
    struct ecu_dnode node1;
    struct ecu_dnode node2;
    struct ecu_dnode node3;

    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Starting list = [node1, node2, node3] */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);
    ecu_dlist_push_back(&list, &node3);

    /* Clear list. List = []. */
    ecu_dlist_clear(&list);

.. figure:: /images/dlist/dlist_clear.svg
  :width: 600
  :align: center

  ecu_dlist_clear()


List Destruction
=================================================
:ecudoxygen:`ecu_dlist_destroy() <ecu_dlist_destroy>` destroys the list, and all nodes 
within the list. The list and nodes must be reconstructed in order to use them in API 
calls again. When each node is destroyed, its destroy callback executes. This is explained 
in detail in :ref:`Node Destruction Section <dlist_node_destruction>`.


Iterators
=================================================
TODO
Talk about DLIST_FOR_EACH() and DLIST_CONST_FOR_EACH().


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/dlist_8h>
