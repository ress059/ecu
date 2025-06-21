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
same list using :ref:`a node ID <dlist_ecu_dnode_id>`.

Theory 
=================================================
This section explains how lists and nodes are represented. It is recommended 
to read this before using the API.

List And Node Representation
-------------------------------------------------
.. _dlist_list_and_node_representation:

Lists are represented by the :ecudoxygen:`ecu_dlist` struct. Nodes are represented by
the :ecudoxygen:`ecu_dnode` struct.

Lists are dummy delimeters used as anchors. Therefore :ecudoxygen:`ecu_dlist::head` is 
not apart of the user's list. Nodes are user-defined and inserted into lists. 

.. figure:: /images/dlist/list_and_node_representation.svg
  :width: 600
  :align: center
  
  List and Node Representation


Storing Node Data 
-------------------------------------------------
.. _dlist_storing_node_data:

Lists are intrusive. Custom data is stored within the node by containing 
an :ecudoxygen:`ecu_dnode` member in a user-defined type. This :ecudoxygen:`ecu_dnode` 
member is passed to the API. For example:

.. code-block:: c 

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
    struct user_node node1, node2;

    /* Pass ecu_dnode member to API to add/remove user_node from a list. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node2.node);


Getting Node Data
-------------------------------------------------
.. _dlist_getting_node_data:

As explained in the :ref:`Storing Node Data Section <dlist_storing_node_data>`, custom data is 
stored within a node by containing an :ecudoxygen:`ecu_dnode` member in a user-defined 
type. However this module can never directly reference the user's type, as it is specific
to the application. This presents an obvious problem: how does the API use only an :ecudoxygen:`ecu_dnode`
structure while supporting any user-defined nodes?

The :ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` macros make this possible.
They convert an :ecudoxygen:`ecu_dnode` struct back into the original user-defined type, 
allowing custom data to be retrieved:

.. code-block:: c 

    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Dnode. */
        struct ecu_dnode node;

        /* More user data. */
        int c;
    };

    struct ecu_dlist list;
    struct user_node node1;
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node); /* Nodes are intrusive. */

    /* ..... */
    /* ..... */

    /* At a later time retrieve user data stored in front node. API returns (struct ecu_dnode *).
    This can be converted back into (struct user_node *) to retrieve custom data. */
    struct ecu_dnode *n = ecu_dlist_front(&list);
    if (n)
    {
        struct user_node *me = ECU_DNODE_GET_ENTRY(n, struct user_node, node);
        me->a = 5;
        me->b = 5;
        me->c = 5;
    }

:ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` take in three parameters:

    1. ``ptr_`` = Pointer to ecu_dnode returned by API. In this case, ``n``.
    2. ``type_`` = User-defined node type. In this case, ``struct user_node``.
    3. ``member_`` = Name of ecu_dnode member within the user-defined type. In this case, ``node``.

.. figure:: /images/dlist/getting_node_data_ecu_dnode_get_entry.svg
  :width: 500
  :align: center
  
  Get Entry Macro Parameters

Under the hood, these macros perform arithmetic on the supplied :ecudoxygen:`ecu_dnode`
pointer to convert it back into the original user-defined type. Consider the following:

.. code-block:: c 

    struct user_node 
    {
        int a;
        int b;
        struct ecu_dnode node;
        int c;
    };

    /* (struct ecu_dnode *)&me.node supplied and returned by API functions. 
    GET_ENTRY() macros convert this back into (struct user_node *)&me. */
    struct user_node me;

.. figure:: /images/dlist/getting_node_data_get_entry_offset.svg
  :width: 600
  :align: center

  Address Offsets In user_node

(struct ecu_dnode \*)&me.node would be supplied and returned by API functions, since the 
:ecudoxygen:`ecu_dnode` structure is common between the API and any user-defined node. 
Supplying this pointer into the GET_ENTRY() macros converts it back into the user-defined type. 
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
the compiler allocates memory **starting at an address that satisfies the alignment requirements 
of type (struct user_node)**. The GET_ENTRY() macros return this raw starting address, which is 
guaranteed to be aligned according the requirements of (struct user_node), since this was originally 
done by the compiler.

As a final note, :ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` performs 
the same operations explained above, but all casts are const qualified. A const-qualified type 
is also returned.


ecu_dnode
=================================================


Constructors
-------------------------------------------------


ecu_dnode_ctor()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_ctor:

Constructor. Initializes the :ecudoxygen:`ecu_dnode` data structure for use.

.. warning:: 

    Must be called once on startup before the node is used. User is also responsible 
    for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct ecu_dnode node1;   /* User must allocate memory before constructor. */
    ecu_dnode_remove(&node1); /* ILLEGAL. Must construct before using. */

    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_remove(&node1); /* Ok. */

An optional destroy function and node ID can be assigned in the constructor. The destroy
function defines any **additional** cleanup required when the node is destroyed, and is 
explained in detail in :ref:`ecu_dnode_destroy() <dlist_ecu_dnode_destroy>`.
The node ID allows different types to be stored and identified in the same list. This 
is explained in detail in :ref:`ecu_dnode_id() <dlist_ecu_dnode_id>`.

:ecudoxygen:`ECU_DNODE_DESTROY_UNUSED` and :ecudoxygen:`ECU_OBJECT_ID_UNUSED` should be
passed if these optional features are unused.

.. code-block:: c 

    struct ecu_dnode node1, node2, node3, node4;

    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED); /* Optional features unused. */
    ecu_dnode_ctor(&node2, &custom_destroy, ECU_OBJECT_ID_UNUSED);          /* Node ID unused. */
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, CUSTOM_ID);            /* Destroy unused. */
    ecu_dnode_ctor(&node4, &custom_destroy, CUSTOM_ID);                     /* Both used. */


ecu_dnode_destroy()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_destroy:

Destructor. Removes the node if it was in a list and executes the custom destroy 
function if one was supplied in :ref:`ecu_dnode_ctor() <dlist_ecu_dnode_ctor>`.
Resets members within the :ecudoxygen:`ecu_dnode` data structure such that 
reconstruction via :ref:`ecu_dnode_ctor() <dlist_ecu_dnode_ctor>` is required 
in order to reuse the node.

.. warning:: 

    Memory is **not** freed memory in the destructor since ECU library does not use 
    dynamic memory allocation. Node can be freed within its custom destroy function 
    (explained later) if it was allocated on the heap.

.. code-block:: c

    struct ecu_dnode node1;

    ecu_dnode_ctor(&node1, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_destroy(&node1); /* Destroys node. Executes custom_destroy(). */

    ecu_dnode_remove(&node1);  /* ILLEGAL. Must reconstruct node in order to use it. */

    ecu_dnode_ctor(&node1, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_remove(&node1);  /* Ok. */

Destroying an individual node within a list removes it while keeping the remaining the list intact:

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1, node2;

    /* Before. list = [node1, node2]. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);

    /* After. list = [node2]. */
    ecu_dnode_destroy(&node1);

.. figure:: /images/dlist/ecu_dnode_destroy.svg
  :width: 600
  :align: center

  ecu_dnode_destroy()

The destructor also executes if the node was within a list that was destroyed 
via :ref:`ecu_dlist_destroy() <dlist_ecu_dlist_destroy>`:

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node3;

    /* list = [node1, node2]. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);

    /* node1 and node2 destroyed since they were in this list.
    custom_destroy(node1) and custom_destroy(node2) run. */
    ecu_dlist_destroy(&list);
    ecu_dnode_remove(&node3); /* node3 was not destroyed so still usable. */

The node's optional destroy callback defines additional cleanup. This API
will pass a pointer to the node being destroyed and its ID into the callback.

.. warning:: 

    The :ecudoxygen:`ecu_dnode` struct is reset before the destroy callback executes.
    Therefore the only API calls allowed in the callback are :ref:`ECU_DNODE_GET_ENTRY() <dlist_ecu_dnode_get_entry>` 
    and :ref:`ECU_DNODE_GET_CONST_ENTRY() <dlist_ecu_dnode_get_const_entry>`.
    Violating this rule is undefined behavior.

In this example the destroy callback is used to free the node's memory:

.. code-block:: c

    struct user_node 
    {
        struct ecu_dnode node;
        void *heap_obj;
    };

    static void destroy(struct ecu_dnode *n, ecu_object_id id)
    {
        /* Define additional cleanup for user_node type. DO NOT USE DNODE
        MEMBER FUNCTIONS IN THE DESTROY CALLBACK. */
        ECU_RUNTIME_ASSERT( (n) );

        if (id == USER_NODE_ID)
        {
            struct user_node *me = ECU_DNODE_GET_ENTRY(n, struct user_node, node);
            free(me->heap_obj);
            free(me); /* Free entire node, including ecu_dnode. */
        }
    }

    /* user_node and heap_obj allocated on heap. */
    struct user_node *node1 = malloc(sizeof(struct user_node));
    node1->heap_obj = malloc(20);
    ecu_dnode_ctor(&node1->node, &destroy, USER_NODE_ID);

    /* destroy() called, automatically freeing memory. */
    ecu_dnode_destroy(&node1->node);

Note that this feature can be applied to many other scenarios besides heap allocation.
For example, a linked list of LEDs. When an LED node is destroyed, it can be turned off:

.. code-block:: c

    struct led 
    {
        struct ecu_dnode node;
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

        if (id == LED_ID)
        {
            struct led *me = ECU_DNODE_GET_ENTRY(n, struct led, node);
            turn_led_off(me->gpio.port, me->gpio.pin);
        }
    }

    /* Construct node. Supply custom destroy callback. Turn LED on. */
    struct led led1;
    ecu_dnode_ctor(&led1.node, &led_destroy, LED_ID);
    led1.gpio.port = PORTA;
    led1.gpio.pin = 5;
    turn_led_on(led1.gpio.port, led1.gpio.pin);

    /* led_destroy() called, automatically turning off the LED. */
    ecu_dnode_destroy(&led1.node);


Macros
-------------------------------------------------


ECU_DNODE_GET_ENTRY()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_get_entry:

Retrieves data stored in the node by converting :ecudoxygen:`ecu_dnode` 
type into user-defined type. Allows nodes to be intrusive without dynamic
memory allocation. See :ref:`Storing Node Data Section <dlist_storing_node_data>`
and :ref:`Getting Node Data Section <dlist_getting_node_data>` for more details.

.. code-block:: c 

    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Dnode. */
        struct ecu_dnode node;

        /* More user data. */
        int c;
    };

    struct ecu_dlist list;
    struct user_node node1;
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1.node); /* Nodes are intrusive. */

    /* ..... */
    /* ..... */

    /* At a later time retrieve user data stored in front node. API returns (struct ecu_dnode *).
    This can be converted back into (struct user_node *) to retrieve custom data. */
    struct ecu_dnode *n = ecu_dlist_front(&list);
    if (n)
    {
        struct user_node *me = ECU_DNODE_GET_ENTRY(n, struct user_node, node);
        me->a = 5;
        me->b = 5;
        me->c = 5;
    }


ECU_DNODE_GET_CONST_ENTRY()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_get_const_entry:

Const-qualified version of :ref:`ECU_DNODE_GET_ENTRY() <dlist_ecu_dnode_get_entry>`. 
The returned node is read-only.


Member Functions
-------------------------------------------------


ecu_dnode_id()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_id:

Returns node ID supplied in :ref:`ecu_dnode_ctor() <dlist_ecu_dnode_ctor>`.
IDs allow different user-defined types to be stored and identified in the 
same list:

.. code-block:: c

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
        switch (ecu_dnode_id(i))
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

            default:
            {
                ECU_RUNTIME_ASSERT( (false) );
                break;
            }
        }
    }


ecu_dnode_in_list()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns true if the supplied node is in a list. Otherwise returns false.

.. code-block:: c 

    bool status;
    struct ecu_dlist list;
    struct ecu_dnode node1, node2;
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    ecu_dlist_push_back(&list, &node1);
    status = ecu_dnode_in_list(&node1); /* True. */
    status = ecu_dnode_in_list(&node2); /* False. */

    ecu_dnode_remove(&node1);
    status = ecu_dnode_in_list(&node1); /* False. */


ecu_dnode_insert_after()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Inserts node after specified position. The node being inserted cannot already
be in a list.

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node3;
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list = [node1, node3]. */ 
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node3);

    /* After. Insert node2 after node1. list = [node1, node2, node3]. */
    ecu_dnode_insert_after(&node1, &node2);

.. figure:: /images/dlist/ecu_dnode_insert_after.svg
  :width: 600
  :align: center

  ecu_dnode_insert_after()

The position node (first argument) **must** be within a list. Otherwise 
this operation is illegal:

.. figure:: /images/dlist/ecu_dnode_insert_after_illegal.svg
  :width: 600
  :align: center

  Illegal ecu_dnode_insert_after()


ecu_dnode_insert_before()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Inserts node before specified position. The node being inserted cannot already
be in a list.

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node3;
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list = [node1, node3]. */ 
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node3);

    /* After. Insert node2 before node3. list = [node1, node2, node3]. */
    ecu_dnode_insert_before(&node3, &node2);

.. figure:: /images/dlist/ecu_dnode_insert_before.svg
  :width: 600
  :align: center

  ecu_dnode_insert_before()

The position node (first argument) **must** be within a list. Otherwise 
this operation is illegal:

.. code-block:: c 

    struct ecu_dnode node2, node3;
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* ILLEGAL. Position node (node3) not in a list. */
    ecu_dnode_insert_before(&node3, &node2);

.. figure:: /images/dlist/ecu_dnode_insert_before_illegal.svg
  :width: 600
  :align: center

  Illegal ecu_dnode_insert_before()


ecu_dnode_next()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_next:

Returns the next node. NULL is returned if the supplied node is the last 
node in the list or if it is not in a list.

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node_not_in_list;

    /* For conciceness assume all lists and nodes have been constructed. */

    /* list = [node1, node2]. */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);

    ecu_dnode_next(&list.head); /* Returns &node1. */
    ecu_dnode_next(&node1); /* Returns &node2. */
    ecu_dnode_next(&node2); /* Returns NULL since node2 is the last node in the list. */
    ecu_dnode_next(&node_not_in_list); /* Returns NULL since supplied node is not in a list. */


ecu_dnode_cnext()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Const-qualified version of :ref:`ecu_dnode_next() <dlist_ecu_dnode_next>`.
Returned node is read-only.


ecu_dnode_prev()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_prev:

Returns the previous node. NULL is returned if the supplied node is the first 
node in the list or if it is not in a list.

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node_not_in_list;

    /* For conciceness assume all lists and nodes have been constructed. */

    /* list = [node1, node2]. */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);

    ecu_dnode_prev(&list.head); /* Returns &node2. */
    ecu_dnode_prev(&node1); /* Returns NULL since node1 is the first node in the list. */
    ecu_dnode_prev(&node2); /* Returns &node1. */
    ecu_dnode_prev(&node_not_in_list); /* Returns NULL since supplied node is not in a list. */


ecu_dnode_cprev()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Const-qualified version of :ref:`ecu_dnode_prev() <dlist_ecu_dnode_prev>`.
Returned node is read-only.


ecu_dnode_remove()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Removes a node from a list.

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dnode node1, node2;
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list = [node1, node2]. */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);

    /* After. list = [node2]. */
    ecu_dnode_remove(&node1);

.. figure:: /images/dlist/ecu_dnode_remove.svg
  :width: 600
  :align: center

  ecu_dnode_remove()

If the supplied node is not in a list, this function does nothing:

.. code-block:: c 

    struct ecu_dnode node1;
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Doing this is OK. */
    ecu_dnode_remove(&node1);
    ecu_dnode_remove(&node1);
    ecu_dnode_remove(&node1);


ecu_dnode_valid()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
TODO!!!!


ecu_dlist
=================================================


Constructors
-------------------------------------------------


ecu_dlist_ctor()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Constructor. Initializes the :ecudoxygen:`ecu_dlist` data structure for use.

.. warning:: 

    Must be called once on startup before the list is used. User is also responsible 
    for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct ecu_dlist list;  /* User must allocate memory before constructor. */
    ecu_dlist_empty(&list); /* ILLEGAL. Must construct before using. */

    ecu_dlist_ctor(&list);
    ecu_dlist_empty(&list); /* Ok. */


ecu_dlist_destroy()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dlist_destroy:

List destructor. All nodes within the list are also destroyed by calling
:ref:`ecu_dnode_destroy() <dlist_ecu_dnode_destroy>` on them. 
Destroyed list and nodes must be reconstructed in order to be used again.

.. warning:: 

    Memory is **not** freed memory in this destructor since ECU library is meant to be 
    used without dynamic memory allocation. If :ecudoxygen:`ecu_dnodes <ecu_dnode>` are 
    allocated on the heap, they can be freed within their destroy callbacks (see 
    :ref:`ecu_dnode_destroy() <dlist_ecu_dnode_destroy>`). If the :ecudoxygen:`ecu_dlist` 
    is allocated on the heap, it must be freed elsewhere, **after** calling this function.

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node3;

    /* Before. list = [node1, node2, node3]. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);
    ecu_dlist_push_back(&list, &node3);

    /* After. list = []. */
    ecu_dlist_destroy(&list);

    /* ILLEGAL. List and nodes must be reconstructed in order to be used again. */
    ecu_dlist_push_back(&list, &node1); 

.. figure:: /images/dlist/ecu_dlist_destroy.svg
  :width: 600
  :align: center

  ecu_dlist_destroy()

An optional destroy callback can be supplied to each node's constructor that allows
users to define additional, application-specific cleanup. When each node in the list 
is destroyed, its destroy callback executes if one was supplied. This is explained 
in detail in :ref:`ecu_dnode_destroy() <dlist_ecu_dnode_destroy>`.

In the following example the :ecudoxygen:`ecu_dlist` and each :ecudoxygen:`ecu_dnode` 
is allocated on the heap. Each node is freed within its destroy callback. The list 
is freed **after** the destructor is called.

.. code-block:: c

    static void destroy(struct ecu_dnode *n, ecu_object_id id)
    {
        /* Define additional cleanup for node. DO NOT USE DNODE
        MEMBER FUNCTIONS IN THE DESTROY CALLBACK. */
        ECU_RUNTIME_ASSERT( (n) );
        (void)id;
        free(me); /* Node was allocated on heap so we must free it here. */
    }

    /* Construct list and nodes. Put them all on heap. Supply custom destroy
    callback to node constructors. */
    struct ecu_dlist *list = malloc(sizeof(struct ecu_dlist));
    struct ecu_dnode *node1 = malloc(sizeof(struct ecu_dnode));
    struct ecu_dnode *node2 = malloc(sizeof(struct ecu_dnode));
    struct ecu_dnode *node3 = malloc(sizeof(struct ecu_dnode));
    ecu_dlist_ctor(list);
    ecu_dnode_ctor(node1, &destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(node2, &destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(node3, &destroy, ECU_OBJECT_ID_UNUSED);

    /* Before. list = [node1, node2, node3] */
    ecu_dlist_push_back(list, node1);
    ecu_dlist_push_back(list, node2);
    ecu_dlist_push_back(list, node3);

    /* After. list = [].
    destroy() is called on node1, node2, and node3, freeing them. 
    No additional intervention required for node cleanup. */
    ecu_dlist_destroy(list);
    free(list); /* NOTE THAT LIST MUST STILL BE EXPLICITLY FREED. */

Note that this feature can be applied to many other scenarios besides heap allocation. 
For example, a linked list of LEDs. When the list is destroyed, all LED nodes can be 
turned off:

.. code-block:: c

    struct led 
    {
        struct ecu_dnode node;
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

    struct ecu_dlist list;
    struct led led1, led2, led3;

    /* Construct nodes. Supply custom destroy callback. */
    ecu_dnode_ctor(&led1.node, &led_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&led2.node, &led_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&led3.node, &led_destroy, ECU_OBJECT_ID_UNUSED);
    led1.gpio.port = PORTA;
    led1.gpio.pin = 5;
    led2.gpio.port = PORTC;
    led2.gpio.pin = 2;
    led3.gpio.port = PORTD;
    led3.gpio.pin = 8;

    /* Before. list = [led1, led2, led3]. Turn all LEDs on. */
    gpio_set_high(led1.gpio.port, led1.gpio.pin);
    gpio_set_high(led2.gpio.port, led2.gpio.pin);
    gpio_set_high(led3.gpio.port, led3.gpio.pin);
    ecu_dlist_push_back(&list, &led1.node);
    ecu_dlist_push_back(&list, &led2.node);
    ecu_dlist_push_back(&list, &led3.node);

    /* After. list = [].
    led_destroy() is called on led1, led2, and led3, turning them off. 
    No additional intervention needed. */
    ecu_dlist_destroy(&list);


Member Functions
-------------------------------------------------


ecu_dlist_back()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dlist_back:

Returns tail node in list but does not remove it. If list is empty, returns NULL.

.. code-block:: c

    struct ecu_dnode *pos;
    struct ecu_dlist list1, list2;
    struct ecu_dnode node1, node2;

    ecu_dlist_ctor(&list1);
    ecu_dlist_ctor(&list2);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list1 = [node1, node2]. list2 = []. */
    ecu_dlist_push_back(&list1, &node1);
    ecu_dlist_push_back(&list1, &node2);

    /* After. Lists unchanged. list1 = [node1, node2]. list2 = []. */
    pos = ecu_dlist_back(&list1); /* &node2 returned. */
    pos = ecu_dlist_back(&list2); /* NULL returned. */


ecu_dlist_cback()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Const-qualified version of :ref:`ecu_dlist_back() <dlist_ecu_dlist_back>`.
Tail node returned is read-only.


ecu_dlist_clear()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Removes all nodes in the list. List and nodes are **not** destroyed so they 
can be reused without reconstruction:

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node3;

    /* Before. list = [node1, node2, node3]. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);
    ecu_dlist_push_back(&list, &node3);

    /* After. list = []. */
    ecu_dlist_clear(&list);

.. figure:: /images/dlist/ecu_dlist_clear.svg
  :width: 600
  :align: center

  ecu_dlist_clear()


ecu_dlist_empty()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns true if list is empty. False otherwise.

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1;

    /* For conciceness assume all lists and nodes have been constructed. */

    ecu_dlist_empty(&list); /* Returns true. */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_empty(&list); /* Returns false. */
    ecu_dnode_remove(&node1);
    ecu_dlist_empty(&list); /* Returns true. */

.. figure:: /images/dlist/ecu_dlist_empty.svg
  :width: 600
  :align: center

  ecu_dlist_empty()


ecu_dlist_front()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dlist_front:

Returns front node in list but does not remove it. If list is empty, returns NULL.

.. code-block:: c

    struct ecu_dnode *pos;
    struct ecu_dlist list1, list2;
    struct ecu_dnode node1, node2;

    ecu_dlist_ctor(&list1);
    ecu_dlist_ctor(&list2);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list1 = [node1, node2]. list2 = []. */
    ecu_dlist_push_back(&list1, &node1);
    ecu_dlist_push_back(&list1, &node2);

    /* After. Lists unchanged. list1 = [node1, node2]. list2 = []. */
    pos = ecu_dlist_front(&list1); /* &node1 returned. */
    pos = ecu_dlist_front(&list2); /* NULL returned. */


ecu_dlist_cfront()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Const-qualified version of :ref:`ecu_dlist_front() <dlist_ecu_dlist_front>`.
Front node returned is read-only.


ecu_dlist_insert_before()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Inserts node before specified position. Insertion criteria is defined by a 
user-supplied condition function. Node cannot be within a list prior to insertion.

Starting at HEAD, the list is iterated over. The current position in the iteration and 
the node being inserted are passed into the user-supplied condition function. If it returns true,
the node is inserted before the current position and ecu_dlist_insert_before() exits. 
Otherwise the iteration continues and the process repeats. If the list is empty, or if 
the iteration completes and all condition function calls returned false, the node is inserted at 
the back of the list.

.. code-block:: c

    struct user_node 
    {
        struct ecu_dnode node;
        uint32_t val;
    };

    /* User-defined condition function. */
    static bool condition(const struct ecu_dnode *n, const struct ecu_dnode *position, void *obj)
    {
        bool status = false; /* Return true if condition passes. False otherwise. */
        ECU_RUNTIME_ASSERT( (n && position) );
        (void)obj; /* Optional callback object unused. */

        const struct user_node *me = ECU_DNODE_GET_CONST_ENTRY(n, struct user_node, node);
        const struct user_node *pos = ECU_DNODE_GET_CONST_ENTRY(position, struct user_node, node);

        if (me->val <= pos->val)
        {
            /* Node should be inserted before position if its value is less than or equal 
            to position's value. */
            status = true;
        }

        return status;
    }

    struct ecu_dlist list;
    struct user_node node1, node2, node3;

    /* node1 value = 1, node2 value = 2, node3 value = 3. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node1.val = 1;
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node2.val = 2;
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node3.val = 3;

    /* Before. list = [node1, node3]. */
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node3.node);
    
    /* After. list = [node1, node2, node3]. */
    ecu_dlist_insert_before(&list, &node2.node, &condition, ECU_DNODE_OBJ_UNUSED);

.. figure:: /images/dlist/ecu_dlist_insert_before.svg
  :width: 600
  :align: center

  ecu_dlist_insert_before()

In this example the iteration completes and all user-supplied function calls 
returned false, so the node is inserted to the back of the list:

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1, node2, node3;

    /* User-defined condition function. */
    static bool all_conditions_fail(const struct ecu_dnode *me, const struct ecu_dnode *position, void *obj)
    {
        return false;
    }

    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list = [node1, node3]. */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node3);

    /* After. list = [node1, node3, node2]. */
    ecu_dlist_insert_before(&list, &node2, &all_conditions_fail, ECU_DNODE_OBJ_UNUSED);

.. figure:: /images/dlist/ecu_dlist_insert_before_iteration_done.svg
  :width: 600
  :align: center

  ecu_dlist_insert_before() Iteration Done


ecu_dlist_push_back()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Inserts node to the back of the list. Node cannot be within a list prior to insertion.

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1, node2;

    /* Before. list = [node1]. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1);

    /* After. list = [node1, node2] */
    ecu_dlist_push_back(&list, &node2);

.. figure:: /images/dlist/ecu_dlist_push_back.svg
  :width: 600
  :align: center

  ecu_dlist_push_back()


ecu_dlist_push_front()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Inserts node to the front of the list. Node cannot be within a list prior to insertion.

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1, node2;

    /* Before. list = [node1]. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dlist_push_back(&list, &node1);

    /* After. list = [node2, node1] */
    ecu_dlist_push_front(&list, &node2);

.. figure:: /images/dlist/ecu_dlist_push_front.svg
  :width: 600
  :align: center

  ecu_dlist_push_front()


ecu_dlist_pop_back()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Removes tail node in the list and returns it. If list is empty, returns NULL.

.. code-block:: c

    struct ecu_dnode *pos;
    struct ecu_dlist list1, list2;
    struct ecu_dnode node1, node2;

    ecu_dlist_ctor(&list1);
    ecu_dlist_ctor(&list2);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list1 = [node1, node2]. list2 = []. */
    ecu_dlist_push_back(&list1, &node1);
    ecu_dlist_push_back(&list1, &node2);

    /* After. list1 = [node1]. list2 = []. */
    pos = ecu_dlist_pop_back(&list1); /* &node2 returned. */
    pos = ecu_dlist_pop_back(&list2); /* NULL returned. */

.. figure:: /images/dlist/ecu_dlist_pop_back.svg
  :width: 600
  :align: center

  ecu_dlist_pop_back()


ecu_dlist_pop_front()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Removes front node in the list and returns it. If list is empty, returns NULL.

.. code-block:: c

    struct ecu_dnode *pos;
    struct ecu_dlist list1, list2;
    struct ecu_dnode node1, node2;

    ecu_dlist_ctor(&list1);
    ecu_dlist_ctor(&list2);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list1 = [node1, node2]. list2 = []. */
    ecu_dlist_push_back(&list1, &node1);
    ecu_dlist_push_back(&list1, &node2);

    /* After. list1 = [node2]. list2 = []. */
    pos = ecu_dlist_pop_front(&list1); /* &node1 returned. */
    pos = ecu_dlist_pop_front(&list2); /* NULL returned. */

.. figure:: /images/dlist/ecu_dlist_pop_front.svg
  :width: 600
  :align: center

  ecu_dlist_pop_front()


ecu_dlist_size()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns number of nodes in a list. Returns 0 if list is empty.

.. code-block:: c

    struct ecu_dlist list;
    struct ecu_dnode node1, node2;

    /* For conciceness assume all lists and nodes have been constructed. */

    ecu_dlist_size(&list); /* Returns 0. */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_size(&list); /* Returns 1. */
    ecu_dlist_push_back(&list, &node2);
    ecu_dlist_size(&list); /* Returns 2. */
    ecu_dnode_remove(&node1);
    ecu_dlist_size(&list); /* Returns 1. */

.. figure:: /images/dlist/ecu_dlist_size.svg
  :width: 600
  :align: center

  ecu_dlist_size()


ecu_dlist_sort()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Merge sorts all nodes in the list. The sorting condition is defined by a 
user-supplied function.

The lefthand-side node (lhs) and the righthand-side node (rhs) are supplied
to the user's function. It must return true if lhs < rhs or false if lhs >= rhs.

.. code-block:: c

    struct user_node 
    {
        struct ecu_dnode node;
        uint32_t val;
    };

    /* User-defined sort function. */
    static bool lhs_less_than_rhs(const struct ecu_dnode *lhs, const struct ecu_dnode *rhs, void *data)
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

    struct ecu_dlist list;
    struct user_node node1, node2, node3, node4;

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

    /* Before. list = [node4, node1, node3, node2]. */
    ecu_dlist_push_back(&list, &node4.node);
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node3.node);
    ecu_dlist_push_back(&list, &node2.node);

    /* After. list = [node1, node2, node3, node4]. */
    ecu_dlist_sort(&list, &lhs_less_than_rhs, ECU_DNODE_OBJ_UNUSED);

.. figure:: /images/dlist/ecu_dlist_sort.svg
  :width: 600
  :align: center

  ecu_dlist_sort()


ecu_dlist_swap()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Swaps contents between two lists.

.. code-block:: c

    struct ecu_dlist list1, list2;
    struct ecu_dnode node1, node2, node3, node4, node5;

    /* For conciceness assume all lists and nodes have been constructed. */

    /* Before. list1 = [node1, node2]. list2 = [node3, node4, node5]. */
    ecu_dlist_push_back(&list1, &node1);
    ecu_dlist_push_back(&list1, &node2);
    ecu_dlist_push_back(&list2, &node3);
    ecu_dlist_push_back(&list2, &node4);
    ecu_dlist_push_back(&list2, &node5);

    /* After. list1 = [node3, node4, node5]. list2 = [node1, node2]. */
    ecu_dlist_swap(&list1, &list2);

.. figure:: /images/dlist/ecu_dlist_swap.svg
  :width: 1000
  :align: center

  ecu_dlist_swap()

If one list is empty, the swapped list will become empty:

.. code-block:: c

    struct ecu_dlist list1, list2;
    struct ecu_dnode node1, node2;

    /* For conciceness assume all lists and nodes have been constructed. */

    /* Before. list1 = [node1, node2]. list2 = []. */
    ecu_dlist_push_back(&list1, &node1);
    ecu_dlist_push_back(&list1, &node2);

    /* After. list1 = []. list2 = [node1, node2]. */
    ecu_dlist_swap(&list1, &list2);


ecu_dlist_valid()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
TODO


Iterators
-------------------------------------------------


ECU_DLIST_FOR_EACH()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dlist_for_each:

Iterates over an :ecudoxygen:`ecu_dlist`, starting at HEAD.

.. code-block:: c 

    struct user_node 
    {
        struct ecu_dnode node;
        uint8_t val;
    };

    struct ecu_dlist list;
    struct ecu_dlist_iterator iterator;
    struct user_node node1, node2, node3;

    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node1.val = 5;
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node2.val = 5;
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node3.val = 5;

    /* list = [node1, node2, node3]. */
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node2.node);
    ecu_dlist_push_back(&list, &node3.node);

    /* Iterate over the list and update each node's val. */
    ECU_DLIST_FOR_EACH(curr, &iterator, &list)
    {
        struct user_node *n = ECU_DNODE_GET_ENTRY(curr, struct user_node, node);
        n->val = 10;
    }

.. figure:: /images/dlist/ecu_dlist_for_each.svg
  :width: 800
  :align: center

  ECU_DLIST_FOR_EACH()

It is safe to remove the **current** node in an iteration. Note however that 
this is only possible with non-const iterators:

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dlist_iterator iterator;
    struct ecu_dnode node1, node2, node3;

    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list = [node1, node2, node3] */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);
    ecu_dlist_push_back(&list, &node3);

    /* After. list = [node1, node3] */
    ECU_DLIST_FOR_EACH(i, &iterator, &list)
    {
        if (i == &node2)
        {
            ecu_dnode_remove(i);
        }
    }

.. figure:: /images/dlist/ecu_dlist_for_each_remove.svg
  :width: 800
  :align: center

  Remove Nodes In Middle Of Iteration

Under the hood, this macro expands to a for-loop that calls 
:ecudoxygen:`ecu_dlist_iterator_begin() <ecu_dlist_iterator_begin>`, 
:ecudoxygen:`ecu_dlist_iterator_end() <ecu_dlist_iterator_end>`, and 
:ecudoxygen:`ecu_dlist_iterator_next() <ecu_dlist_iterator_next>`.
The two iterations produce the same code:

.. code-block:: c 

    /* The two iterations are exactly the same. */
    ECU_DLIST_FOR_EACH(curr, &iterator, &list)
    {
        // ....
    }

    for (struct ecu_dnode *curr = ecu_dlist_iterator_begin(&iterator, &list);
         curr != ecu_dlist_iterator_end(&iterator);
         curr = ecu_dlist_iterator_next(&iterator))
    {
        // ....
    }

It is recommended to use the iteration macros instead of for-loops since they shield
the application from future API changes, and require less typing.


ECU_DLIST_CONST_FOR_EACH()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Const-qualified version of :ref:`ECU_DLIST_FOR_EACH() <dlist_ecu_dlist_for_each>`. Nodes 
cannot be edited or removed in the middle of an iteration since this is a const iterator.


ECU_DLIST_AT_FOR_EACH()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dlist_at_for_each:

Iterates over an :ecudoxygen:`ecu_dlist`, starting at the specified position.

.. warning::

    Behavior is undefined if the position node is not within the list being 
    iterated over.

.. code-block:: c 

    struct user_node
    {
        struct ecu_dnode node;
        uint8_t val;
    };

    struct ecu_dlist list;
    struct ecu_dlist_iterator iterator;
    struct user_node node1, node2, node3;

    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node1.val = 5;
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node2.val = 5;
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    node3.val = 5;

    /* list = [node1, node2, node3] */
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node2.node);
    ecu_dlist_push_back(&list, &node3.node);

    /* Iterate over the list starting at node 2. Update each node's val. */
    ECU_DLIST_AT_FOR_EACH(curr, &iterator, &list, &node2.node)
    {
        struct user_node *n = ECU_DNODE_GET_ENTRY(curr, struct user_node, node);
        n->val = 10;
    }

.. figure:: /images/dlist/ecu_dlist_at_for_each.svg
  :width: 800
  :align: center

  ECU_DLIST_AT_FOR_EACH()

It is safe to remove the **current** node in an iteration. Note however that 
this is only possible with non-const iterators:

.. code-block:: c 

    struct ecu_dlist list;
    struct ecu_dlist_iterator iterator;
    struct ecu_dnode node1, node2, node3;

    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

    /* Before. list = [node1, node2, node3] */
    ecu_dlist_push_back(&list, &node1);
    ecu_dlist_push_back(&list, &node2);
    ecu_dlist_push_back(&list, &node3);

    /* After. list = [node1, node3] */
    ECU_DLIST_AT_FOR_EACH(i, &iterator, &list, &node2)
    {
        if (i == &node2)
        {
            ecu_dnode_remove(i);
        }
    }

.. figure:: /images/dlist/ecu_dlist_at_for_each_remove.svg
  :width: 800
  :align: center

  Remove Nodes In Middle Of Iteration

Under the hood, this macro expands to a for-loop that calls 
:ecudoxygen:`ecu_dlist_iterator_at() <ecu_dlist_iterator_at>`, 
:ecudoxygen:`ecu_dlist_iterator_end() <ecu_dlist_iterator_end>`, and 
:ecudoxygen:`ecu_dlist_iterator_next() <ecu_dlist_iterator_next>`.
The two iterations produce the same code:

.. code-block:: c 

    /* The two iterations are exactly the same. */
    ECU_DLIST_AT_FOR_EACH(curr, &iterator, &list, &node2)
    {
        // ....
    }

    for (struct ecu_dnode *curr = ecu_dlist_iterator_at(&iterator, &list, &node2);
         curr != ecu_dlist_iterator_end(&iterator);
         curr = ecu_dlist_iterator_next(&iterator))
    {
        // ....
    }

It is recommended to use the iteration macros instead of for-loops since they shield
the application from future API changes, and require less typing.


ECU_DLIST_CONST_AT_FOR_EACH()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Const-qualified version of :ref:`ECU_DLIST_AT_FOR_EACH() <dlist_ecu_dlist_at_for_each>`. Nodes 
cannot be edited or removed in the middle of an iteration since this is a const iterator.


API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/dlist_8h>
