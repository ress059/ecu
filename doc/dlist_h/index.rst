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

Doubly-linked list. Nodes can store any user data by value without dynamic 
memory allocation by being intrusive.


Theory 
=================================================

List Representation
-------------------------------------------------
Lists are represented by the :ecudoxygen:`ecu_dlist` struct. They are HEAD nodes
used purely as delimiters. 

.. warning::

    HEAD nodes are not apart of a user's list and can never be passed into the API.

.. figure:: /images/dlist/list_representation.svg
  :width: 600
  :align: center
  
  List Representation

Node Representation
-------------------------------------------------
.. _dlist_node_representation:

Nodes are user-defined and represented by the :ecudoxygen:`ecu_dnode` struct. 
They store user data by value without dynamic memory allocation by being
intrusive. The user's node stores an :ecudoxygen:`ecu_dnode` member:

.. code-block:: c 

    /* Custom user node. */
    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Store ecu_dnode in user node. */
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

The above code showed how user data can be stored in a node. However how can
the data be retrieved? This module has no knowledge of the user's type so it can only use 
the :ecudoxygen:`ecu_dnode` type:

.. code-block:: c 

    /* Returns (ecu_dnode *). Module has no knowledge of user's data. */
    struct ecu_dnode *n = ecu_dlist_pop_back(&list);

The :ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` macros allow user
data to be retrieved by converting an :ecudoxygen:`ecu_dnode` back into the user's node type:

.. code-block:: c 

    /* Custom user node. */
    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Store ecu_dnode in user node. */
        struct ecu_dnode node;

        /* More user data. */
        int c;
    };

    /* Assume list has been constructed and has nodes of user_node type. 
    Retrieve front node and edit it. */
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

    #. ``ptr_`` = Pointer to intrusive ecu_dnode. In this case, ``n``.
    #. ``type_`` = User's node type. In this case, ``struct user_node``.
    #. ``member_`` = Name of ecu_dnode member within the user's node type. In this case, ``node``.

.. figure:: /images/dlist/getting_node_data_ecu_dnode_get_entry.svg
  :width: 500
  :align: center
  
  Get Entry Macro Parameters

Under the hood, these macros perform arithmetic on the supplied :ecudoxygen:`ecu_dnode`
pointer to convert it back into the user's node type. The details of this operation
are fully explained in :ref:`ECU_CONTAINER_OF() <utils_container_of>` and 
:ref:`ECU_CONST_CONTAINER_OF() <utils_const_container_of>`.

Node ID 
-------------------------------------------------
.. _dlist_node_id:

Nodes can be parametrized by unique IDs to identify different
types stored in the same list. IDs are assigned when each node is 
constructed. Each type must have a unique ID. Example usage:

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

    /* Construct list and nodes. Assign IDs to each node to identify their data types. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, TYPE1);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, TYPE2);
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, TYPE3);

    /* Add nodes to list. */
    ecu_dlist_push_back(&list, &node1.node);
    ecu_dlist_push_back(&list, &node2.node);
    ecu_dlist_push_back(&list, &node3.node);

    /* Iterate over list. Use IDs to identify the data type stored in each node. */
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
                ECU_ASSERT( (false) );
                break;
            }
        }
    }

Node Destroy Callback
-------------------------------------------------
.. _dlist_node_destroy_callback:

When a node is destroyed, an optional callback can execute that performs
**additional** cleanup of the user's node type. Like the ID, this callback
is also assigned in the node's constructor.

The following example represents a node as an LED. Its destroy callback turns
the LED off so when the node is destroyed, the LED automatically turns off:

.. code-block:: c

    struct led 
    {
        struct ecu_dnode node;
        uint32_t port;
        uint32_t pin;
    };

    static void turn_led_off(struct ecu_dnode *n, ecu_object_id id)
    {
        /* Turn off LED when node is destroyed. */
        struct led *me = ECU_DNODE_GET_ENTRY(n, struct led, node);
        GPIOPinWriteLow(me->port, me->pin);
    }

    /* Create LED node. */
    struct led led1;
    ecu_dnode_ctor(&led1.node, &turn_led_off, ECU_OBJECT_ID_UNUSED);
    led1.port = GPIO_PORTA;
    led1.pin = GPIO_PIN5;

    /* Destroy node. turn_led_off() called, automatically turning off the LED. */
    ecu_dnode_destroy(&led1.node);

When a list is destroyed, all nodes in that list are destroyed. Building
off of the previous example, a list of LEDs is created. The list is then
destroyed, turning all LEDs off:

.. code-block:: c

    struct ecu_dlist list;
    struct led led1, led2, led3;

    /* Assume user data also initialized. */
    ecu_dlist_ctor(&list);
    ecu_dnode_ctor(&led1.node, &turn_led_off, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&led2.node, &turn_led_off, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_ctor(&led3.node, &turn_led_off, ECU_OBJECT_ID_UNUSED);

    /* List = [led1, led2, led3]. */
    ecu_dlist_push_back(&list, &led1.node);
    ecu_dlist_push_back(&list, &led2.node);
    ecu_dlist_push_back(&list, &led3.node);

    /* Destroy list. All nodes in list destroyed. turn_led_off(led1), 
    turn_led_off(led2), and turn_led_off(led3) called. */
    ecu_dlist_destroy(&list);

.. warning:: 

    Memory is **not** freed when a node is destroyed since ECU library does not use 
    dynamic memory allocation. Node can be freed within its destroy callback 
    if it was allocated on the heap.

.. warning:: 

    The only API calls allowed in the destroy callback are 
    :ref:`ECU_DNODE_GET_ENTRY() <dlist_ecu_dnode_get_entry>` and 
    :ref:`ECU_DNODE_GET_CONST_ENTRY() <dlist_ecu_dnode_get_const_entry>`.
    Violating this rule is undefined behavior.

API 
=================================================
.. toctree::
    :maxdepth: 1

    dlist.h </doxygen/html/dlist_8h>

Macros
-------------------------------------------------

ECU_DNODE_GET_ENTRY()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_get_entry:

Retrieves user data stored in an :ecudoxygen:`ecu_dnode` by converting
an :ecudoxygen:`ecu_dnode` back into the user's node type. See 
:ref:`Node Representation Section <dlist_node_representation>`.

ECU_DNODE_GET_CONST_ENTRY()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_get_const_entry:

Const-qualified version of :ref:`ECU_DNODE_GET_ENTRY() <dlist_ecu_dnode_get_entry>`. 
The returned node is read-only. See :ref:`Node Representation Section <dlist_node_representation>`.

ecu_dnode
-------------------------------------------------

Constructors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_dnode_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
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

An optional destroy callback and node ID can be assigned in the constructor.
This is explained in :ref:`Node ID Section <dlist_node_id>` and 
:ref:`Node Destroy Callback Section <dlist_node_destroy_callback>`.

ecu_dnode_destroy()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dnode_destroy:

Destructor. Removes the node if it was in a list and executes its destroy 
callback (see :ref:`Node Destroy Callback Section <dlist_node_destroy_callback>`) if one 
was supplied in :ref:`ecu_dnode_ctor() <dlist_ecu_dnode_ctor>`.
Resets members within the :ecudoxygen:`ecu_dnode` data structure such that 
reconstruction via :ref:`ecu_dnode_ctor() <dlist_ecu_dnode_ctor>` is required 
in order to reuse the node.

.. warning:: 

    Memory is **not** freed when a node is destroyed since ECU library does not use 
    dynamic memory allocation. Node can be freed within its destroy callback 
    if it was allocated on the heap.

.. warning:: 

    The only API calls allowed in the destroy callback are 
    :ref:`ECU_DNODE_GET_ENTRY() <dlist_ecu_dnode_get_entry>` and 
    :ref:`ECU_DNODE_GET_CONST_ENTRY() <dlist_ecu_dnode_get_const_entry>`.
    Violating this rule is undefined behavior.

.. code-block:: c

    struct ecu_dnode node1;

    ecu_dnode_ctor(&node1, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_destroy(&node1); /* Destroys node. Executes custom_destroy(). */

    ecu_dnode_remove(&node1);  /* ILLEGAL. Must reconstruct node in order to use it. */

    ecu_dnode_ctor(&node1, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_remove(&node1);  /* Ok. */

Destroying an individual node within a list removes it while keeping the remaining list intact:

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

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_dnode_id()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dnode_id:

Returns node ID assigned in :ref:`ecu_dnode_ctor() <dlist_ecu_dnode_ctor>`.
IDs allow different user-defined types to be stored and identified in the 
same list. This is fully explained in the :ref:`Node Id Section <dlist_node_id>`.

.. code-block:: c

    enum user_ids
    {
        ID1 = ECU_USER_OBJECT_ID_BEGIN,
        ID2
    };

    struct ecu_dnode node1, node2;
    ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ID1);
    ecu_dnode_ctor(&node2, ECU_DNODE_DESTROY_UNUSED, ID2);

    ecu_dnode_id(&node1); /* Returns ID1. */
    ecu_dnode_id(&node2); /* Returns ID2. */

ecu_dnode_in_list()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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

    ecu_dnode_next(&node1); /* Returns &node2. */
    ecu_dnode_next(&node2); /* Returns NULL since node2 is the last node in the list. */
    ecu_dnode_next(&node_not_in_list); /* Returns NULL since supplied node is not in a list. */

ecu_dnode_cnext()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dnode_next() <dlist_ecu_dnode_next>`.
Returned node is read-only.

ecu_dnode_prev()
"""""""""""""""""""""""""""""""""""""""""""""""""
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

    ecu_dnode_prev(&node1); /* Returns NULL since node1 is the first node in the list. */
    ecu_dnode_prev(&node2); /* Returns &node1. */
    ecu_dnode_prev(&node_not_in_list); /* Returns NULL since supplied node is not in a list. */

ecu_dnode_cprev()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dnode_prev() <dlist_ecu_dnode_prev>`.
Returned node is read-only.

ecu_dnode_remove()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if supplied node has been constructed and the contents
in :ecudoxygen:`ecu_dnode` are valid. False otherwise.

.. code-block:: c 

    struct ecu_dnode node;
    ecu_dnode_valid(&node); /* Returns false. */
    ecu_dnode_ctor(&node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_dnode_valid(&node); /* Returns true. */

ecu_dlist
-------------------------------------------------

Constructors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_dlist_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dlist_destroy:

List destructor. All nodes within the list are also destroyed by calling
:ref:`ecu_dnode_destroy() <dlist_ecu_dnode_destroy>` on them. 
Destroyed list and nodes must be reconstructed in order to be used again.

.. warning:: 

    Memory is **not** freed memory in this destructor since ECU library is meant to be 
    used without dynamic memory allocation. If :ecudoxygen:`ecu_dnodes <ecu_dnode>` are 
    allocated on the heap, they can be freed within their destroy callbacks (see 
    :ref:`Node Destroy Callback Section <dlist_node_destroy_callback>`). If the :ecudoxygen:`ecu_dlist` 
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

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_dlist_back()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dlist_back() <dlist_ecu_dlist_back>`.
Tail node returned is read-only.

ecu_dlist_clear()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dlist_front() <dlist_ecu_dlist_front>`.
Front node returned is read-only.

ecu_dlist_insert_before()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
        ECU_ASSERT( (n && position) );
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
  :width: 400
  :align: center

  ecu_dlist_size()

ecu_dlist_sort()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
        ECU_ASSERT( (lhs && rhs) );
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
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if supplied list has been constructed and the contents
in :ecudoxygen:`ecu_dlist` are valid. False otherwise.

.. code-block:: c 

    struct ecu_dlist list;
    ecu_dlist_valid(&list); /* Returns false. */
    ecu_dlist_ctor(&list);
    ecu_dlist_valid(&list); /* Returns true. */

Iterators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ECU_DLIST_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
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
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_DLIST_AT_FOR_EACH() <dlist_ecu_dlist_at_for_each>`. Nodes 
cannot be edited or removed in the middle of an iteration since this is a const iterator.

ECU_DLIST_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dlist_for_each:

Iterates over an :ecudoxygen:`ecu_dlist`, starting at HEAD. Immediately exits
if list is empty.

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
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_DLIST_FOR_EACH() <dlist_ecu_dlist_for_each>`. Nodes 
cannot be edited or removed in the middle of an iteration since this is a const iterator.
