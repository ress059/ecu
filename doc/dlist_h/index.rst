.. _dlist_h:

dlist.h
###############################################
.. raw:: html

   <hr>

Overview
=================================================
.. note:: 

    The term ``ECU`` in this document refers to Embedded C Utilities, the shorthand name for this project.

Doubly-linked list. Nodes can store any user-defined type (by value) by being intrusive.

Theory 
=================================================

Node Representation
-------------------------------------------------
.. _dlist_node_representation:

List nodes are represented by the :ecudoxygen:`ecu_dnode` structure. They can store any user-defined type by being intrusive. Therefore a user's node consists of data and an :ecudoxygen:`ecu_dnode` member:

    .. code-block:: c

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

Linked list operations are performed on a user's node by passing in the :ecudoxygen:`ecu_dnode` member into this module:

    .. code-block:: c

        struct user_node me;
        ecu_dlist_push_back(&list, &me.node);

When applicable, the :ecudoxygen:`ecu_dnode` type is also returned by this API:

    .. code-block:: c

        struct ecu_dnode *node = ecu_dlist_front(&list);

This module has no knowledge of user-defined types so it must interface through the :ecudoxygen:`ecu_dnode` type. :ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and :ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY <ECU_DNODE_GET_CONST_ENTRY>` should be used to convert the returned :ecudoxygen:`ecu_dnode` back into the user-defined type. For example:

    .. code-block:: c

        struct ecu_dnode *n = ecu_dlist_front(&list);

        if (n)
        {
            struct user_node *me = ECU_DNODE_GET_ENTRY(n, struct user_node, node);
            me->a = 5;
            me->b = 5;
            ....
        }

:ecudoxygen:`ECU_DNODE_GET_ENTRY() <ECU_DNODE_GET_ENTRY>` and :ecudoxygen:`ECU_DNODE_GET_CONST_ENTRY() <ECU_DNODE_GET_CONST_ENTRY>` take in three parameters:

    #. ``ptr_`` = Pointer to intrusive ecu_dnode. In this case, ``n``.
    #. ``type_`` = User's node type. In this case, ``struct user_node``.
    #. ``member_`` = Name of ecu_dnode member within the user's node type. In this case, ``node``.

    .. figure:: /images/dlist/node_representation_get_entry.svg
        :width: 500
        :align: center
    
        Get Entry Macros

Under the hood, these macros perform arithmetic on the supplied :ecudoxygen:`ecu_dnode` pointer to convert it back into the user's node type. The details of this operation are fully explained in :ref:`ECU_CONTAINER_OF() <utils_container_of>` and :ref:`ECU_CONST_CONTAINER_OF() <utils_const_container_of>`.

Node Destroy Callback
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_node_destroy_callback:

A custom destroy callback can optionally be assigned to an :ecudoxygen:`ecu_dnode` when it is constructed. This callback executes when the node is destroyed or when a list containing that node is destroyed. It performs **additional** cleanup specific to the the user's data. In the example below, nodes are LEDs that turn off when destroyed:

    .. code-block:: c

        struct led 
        {
            struct ecu_dnode node;
            uint32_t port;
            uint32_t pin;
        };

        static void turn_led_off(struct ecu_dnode *n, ecu_object_id_t id)
        {
            /* Turn off LED when node is destroyed. */
            struct led *me = ECU_DNODE_GET_ENTRY(n, struct led, node);
            GPIOPinWriteLow(me->port, me->pin);
        }

        struct led led1;
        ecu_dnode_ctor(&led1.node, &turn_led_off, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_destroy(&led1.node); /* turn_led_off(&led1.node) runs. */

    .. warning:: 

        The only API calls allowed in the destroy callback are :ref:`ECU_DNODE_GET_ENTRY() <dlist_ecu_dnode_get_entry>` and :ref:`ECU_DNODE_GET_CONST_ENTRY() <dlist_ecu_dnode_get_const_entry>`. Violating this rule is undefined behavior.

Node ID 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_node_id:

An optional ID can be assigned on construction to help identify different node types stored in the same list. For example:

.. code-block:: c

    /* User-defined IDs. */
    enum user_node_ids
    {
        TYPE1 = ECU_USER_OBJECT_ID_BEGIN,
        TYPE2,
        TYPE3
    };

    /* Different node types stored in same list. */
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

    struct type1 node1;
    struct type2 node2;
    struct type3 node3;
    ecu_dnode_ctor(&node1.node, ECU_DNODE_DESTROY_UNUSED, TYPE1);
    ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, TYPE2);
    ecu_dnode_ctor(&node3.node, ECU_DNODE_DESTROY_UNUSED, TYPE3);

    /* Iterate over list containing these nodes. Use IDs to identify the node type.
    Assume list previously constructed and nodes added to list beforehand for conciseness. */
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
                break;
            }
        }
    }

List Representation
-------------------------------------------------
Lists are represented by the :ecudoxygen:`ecu_dlist` struct. They are HEAD nodes used purely as delimiters. 

    .. figure:: /images/dlist/list_representation.svg
        :width: 400
        :align: center
        
        List Representation

    .. warning::

        HEAD nodes are not apart of a user's list and can never be passed into the API.

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

Retrieves user data stored in an :ecudoxygen:`ecu_dnode` by converting it back into the user's node type. See :ref:`Node Representation Section <dlist_node_representation>` for more details.

ECU_DNODE_GET_CONST_ENTRY()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _dlist_ecu_dnode_get_const_entry:

Const-qualified version of :ref:`ECU_DNODE_GET_ENTRY() <dlist_ecu_dnode_get_entry>`. Returned node is read-only.

ecu_dnode
-------------------------------------------------

Constructors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_dnode_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dnode_ctor:

Constructor. Initializes the :ecudoxygen:`ecu_dnode` data structure for use. An optional destroy callback and node ID can also be assigned in the constructor. See :ref:`Node Destroy Callback Section <dlist_node_destroy_callback>` and :ref:`Node ID Section <dlist_node_id>` for more details. 

    .. warning:: 

        Must be called once on startup before the node is used. User is also responsible for allocating memory since ECU does not use dynamic memory allocation.

    .. code-block:: c 

        struct ecu_dnode node1;   /* User must allocate memory before constructor. */
        ecu_dnode_remove(&node1); /* ILLEGAL. Must construct before using. */

        ecu_dnode_ctor(&node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_remove(&node1); /* Ok. */

ecu_dnode_destroy()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dnode_destroy:

Destructor. Removes the node if it is in a list. Uninitializes the :ecudoxygen:`ecu_dnode` structure then executes the user-defined destroy callback if one was supplied. See :ref:`Node Destroy Callback Section <dlist_node_destroy_callback>` for more details.

    .. warning:: 

        Memory is **not** freed when a node is destroyed since ECU library does not use dynamic memory allocation. Node can be freed within its destroy callback if it was allocated on the heap.

    .. code-block:: c 

        struct ecu_dnode node;     /* User must allocate memory before constructor. */
        ecu_dnode_ctor(&node, &user_destroy, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_destroy(&node);  /* ecu_dnode uninitialized and user_destroy(&node) ran. */

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_dnode_id()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the node's ID assigned when it was constructed. See :ref:`Node ID Section <dlist_node_id>` for more details.

    .. code-block:: c
        
        struct ecu_dnode node;
        ecu_dnode_ctor(&node, ECU_DNODE_DESTROY_UNUSED, 3);
        ecu_dnode_id(&node); /* Returns 3. */

ecu_dnode_in_list()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the node is in a list. False otherwise. Consider the following list:

    .. figure:: /images/dlist/ecu_dnode_in_list.svg
        :width: 600
        :align: center
    
        ecu_dnode_in_list()

    .. code-block:: c 

        ecu_dnode_in_list(&node1); /* True. */
        ecu_dnode_in_list(&node2); /* True. */
        ecu_dnode_in_list(&node3); /* False. */

ecu_dnode_insert_after()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dnode_insert_after:

Inserts a node after the specified position. 

    .. figure:: /images/dlist/ecu_dnode_insert_after.svg
        :width: 600
        :align: center

        ecu_dnode_insert_after()

The node being inserted cannot already be in a list. Thus the following operation is illegal:

    .. figure:: /images/dlist/ecu_dnode_insert_after_illegal_node_in_list.svg
        :width: 600
        :align: center

        Illegal ecu_dnode_insert_after()

The position node must be within a list. Thus the following operation is illegal:

    .. figure:: /images/dlist/ecu_dnode_insert_after_illegal_pos_not_in_list.svg
        :width: 600
        :align: center

        Illegal ecu_dnode_insert_after()

ecu_dnode_insert_before()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts a node before the specified position.

    .. figure:: /images/dlist/ecu_dnode_insert_before.svg
        :width: 600
        :align: center

        ecu_dnode_insert_before()

The node being inserted cannot already be in a list. Thus the following operation is illegal:

    .. figure:: /images/dlist/ecu_dnode_insert_before_illegal_node_in_list.svg
        :width: 600
        :align: center

        Illegal ecu_dnode_insert_before()

The position node must be within a list. Thus the following operation is illegal:

    .. figure:: /images/dlist/ecu_dnode_insert_before_illegal_pos_not_in_list.svg
        :width: 600
        :align: center

        Illegal ecu_dnode_insert_before()

ecu_dnode_next()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dnode_next:

Returns the node next to (right of) the queried node. NULL is returned if the queried node is TAIL or not in a list. Consider the following list:

    .. figure:: /images/dlist/ecu_dnode_next.svg
        :width: 550
        :align: center

        ecu_dnode_next()

    .. code-block:: c

        ecu_dnode_next(&node1); /* Returns &node2 */
        ecu_dnode_next(&node2); /* Returns &node3 */
        ecu_dnode_next(&node3); /* Returns NULL */

ecu_dnode_cnext()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dnode_next() <dlist_ecu_dnode_next>`. Returned node is read-only.

ecu_dnode_prev()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dnode_prev:

Returns the node previous to (left of) the queried node. NULL is returned if the queried node is one after HEAD or not in a list. Consider the following list:

    .. figure:: /images/dlist/ecu_dnode_prev.svg
        :width: 550
        :align: center

        ecu_dnode_prev()

    .. code-block:: c

        ecu_dnode_prev(&node1); /* Returns NULL */
        ecu_dnode_prev(&node2); /* Returns &node1 */
        ecu_dnode_prev(&node3); /* Returns &node2 */

ecu_dnode_cprev()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dnode_prev() <dlist_ecu_dnode_prev>`. Returned node is read-only.

ecu_dnode_remove()
"""""""""""""""""""""""""""""""""""""""""""""""""
Removes the node from a list. It can be reused and added to another list without reconstruction. If the supplied node is not in a list, this function does nothing.

    .. figure:: /images/dlist/ecu_dnode_remove.svg
        :width: 500
        :align: center

        ecu_dnode_remove()

ecu_dnode_valid()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the supplied node has been constructed. False otherwise.

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

        Must be called once on startup before the list is used. User is also responsible for allocating memory since ECU does not use dynamic memory allocation.

    .. code-block:: c 

        struct ecu_dlist list;  /* User must allocate memory before constructor. */
        ecu_dlist_empty(&list); /* ILLEGAL. Must construct before using. */

        ecu_dlist_ctor(&list);
        ecu_dlist_empty(&list); /* Ok. */

ecu_dlist_destroy()
"""""""""""""""""""""""""""""""""""""""""""""""""
Destructor. Destroys the list and all nodes within the list. All destroyed objects must be reconstructed in order to be used again. The user-supplied destroy callback for each node executes as they are destroyed. See :ref:`Node Destroy Callback Section <dlist_node_destroy_callback>` for more details.

    .. warning:: 

        Memory is **not** freed when a list is destroyed since ECU library does not use dynamic memory allocation. Destroyed nodes in the list can be freed within their destroy callbacks if they were allocated on the heap. The list must be freed after this destructor.

    .. figure:: /images/dlist/ecu_dlist_destroy.svg
        :width: 550
        :align: center

        ecu_dlist_destroy()

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_dlist_back()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dlist_back:

Returns the tail node but does not remove it. If the list is empty, NULL is returned. Consider the following lists:

    .. figure:: /images/dlist/ecu_dlist_back.svg
        :width: 450
        :align: center

        ecu_dlist_back()

    .. code-block:: c
        
        ecu_dlist_back(&list1); /* Returns &node1 */
        ecu_dlist_back(&list2); /* Returns &node3 */
        ecu_dlist_back(&list3); /* Returns NULL */ 

ecu_dlist_cback()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dlist_back() <dlist_ecu_dlist_back>`. Returned node is read-only.

ecu_dlist_clear()
"""""""""""""""""""""""""""""""""""""""""""""""""
Removes all nodes from the list. List and nodes can be reused without reconstruction.

    .. figure:: /images/dlist/ecu_dlist_clear.svg
        :width: 500
        :align: center

        ecu_dlist_clear()

ecu_dlist_empty()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the list is empty. False otherwise. Consider the following lists:

    .. figure:: /images/dlist/ecu_dlist_empty.svg
        :width: 450
        :align: center

        ecu_dlist_empty()

    .. code-block:: c

        ecu_dlist_empty(&list1); /* False */
        ecu_dlist_empty(&list2); /* True */

ecu_dlist_front()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dlist_front:

Returns the front node in the list but does not remove it. If the list is empty, returns NULL. Consider the following lists:

    .. figure:: /images/dlist/ecu_dlist_front.svg
        :width: 450
        :align: center

        ecu_dlist_front()

    .. code-block:: c

        ecu_dlist_front(&list1); /* Returns &node1 */
        ecu_dlist_front(&list2); /* Returns &node2 */
        ecu_dlist_front(&list3); /* Returns NULL */ 

ecu_dlist_cfront()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_dlist_front() <dlist_ecu_dlist_front>`. Returned node is read-only.

ecu_dlist_insert_before()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts a node before the position specified by a condition function. Starting from HEAD, all nodes within the list are iterated over. Each node is passed as the position parameter to the condition function. The user returns true if the node should be inserted before this position or false to continue the iteration. This function exits as soon as the node is inserted. If the entire list is iterated over but no condition has returned true, the node is added to the back of the list.

    .. code-block:: c

        struct user_node 
        {
            struct ecu_dnode node;
            uint32_t val;
        };

        static bool condition(const struct ecu_dnode *n, const struct ecu_dnode *position, void *data)
        {
            const struct user_node *me = ECU_DNODE_GET_CONST_ENTRY(n, struct user_node, node);
            const struct user_node *pos = ECU_DNODE_GET_CONST_ENTRY(position, struct user_node, node);

            if (me->val <= pos->val)
            {
                return true;
            }

            return false;
        }

        struct user_node node2;
        ecu_dnode_ctor(&node2.node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        node2.val = 2;
        ecu_dlist_insert_before(&list, &node2.node, &condition, ECU_DNODE_OBJ_UNUSED);

    .. figure:: /images/dlist/ecu_dlist_insert_before.svg
        :width: 600
        :align: center

        ecu_dlist_insert_before()

ecu_dlist_push_back()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts node to the back of the list. 

    .. figure:: /images/dlist/ecu_dlist_push_back.svg
        :width: 550
        :align: center

        ecu_dlist_push_back()

The node being inserted cannot already be within a list. Thus the following operation is illegal:

    .. figure:: /images/dlist/ecu_dlist_push_back_illegal.svg
        :width: 600
        :align: center

        Illegal ecu_dlist_push_back()

ecu_dlist_push_front()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts node to the front of the list.

    .. figure:: /images/dlist/ecu_dlist_push_front.svg
        :width: 550
        :align: center

        ecu_dlist_push_front()

The node being inserted cannot already be within a list. Thus the following operation is illegal:

    .. figure:: /images/dlist/ecu_dlist_push_front_illegal.svg
        :width: 600
        :align: center

        Illegal ecu_dlist_push_front()

ecu_dlist_pop_back()
"""""""""""""""""""""""""""""""""""""""""""""""""
Removes the tail node from the list and returns it.

    .. figure:: /images/dlist/ecu_dlist_pop_back.svg
        :width: 500
        :align: center

        ecu_dlist_pop_back()

If the list is empty, returns NULL. For example:

    .. figure:: /images/dlist/ecu_dlist_pop_back_empty.svg
        :width: 200
        :align: center

        Empty ecu_dlist_pop_back()

    .. code-block:: c

        ecu_dlist_pop_back(&list2); /* Returns NULL. */


ecu_dlist_pop_front()
"""""""""""""""""""""""""""""""""""""""""""""""""
Removes the front node from the list and returns it.

    .. figure:: /images/dlist/ecu_dlist_pop_front.svg
        :width: 500
        :align: center

        ecu_dlist_pop_front()

If the list is empty, returns NULL. For example:

    .. figure:: /images/dlist/ecu_dlist_pop_front_empty.svg
        :width: 200
        :align: center

        Empty ecu_dlist_pop_front()

    .. code-block:: c

        ecu_dlist_pop_front(&list2); /* Returns NULL. */

ecu_dlist_size()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the number of nodes in a list. Returns 0 if the list is empty. Consider the following lists:

    .. figure:: /images/dlist/ecu_dlist_size.svg
        :width: 450
        :align: center

        ecu_dlist_size()

    .. code-block:: c

        ecu_dlist_size(&list1); /* Returns 0. */
        ecu_dlist_size(&list2); /* Returns 1. */
        ecu_dlist_size(&list3); /* Returns 2. */

ecu_dlist_sort()
"""""""""""""""""""""""""""""""""""""""""""""""""
Merge sorts all nodes in the list. The sorting condition is defined by a user-supplied function. This function must return true if the supplied left node (lhs) is less than the supplied right node (rhs). Otherwise false must be returned.

    .. code-block:: c

        struct user_node 
        {
            struct ecu_dnode node;
            uint32_t val;
        };

        static bool condition(const struct ecu_dnode *lhs, const struct ecu_dnode *rhs, void *data)
        {
            const struct user_node *left = ECU_DNODE_GET_CONST_ENTRY(lhs, struct user_node, node);
            const struct user_node *right = ECU_DNODE_GET_CONST_ENTRY(rhs, struct user_node, node);

            if (left->val < right->val)
            {
                return true;
            }

            return false;
        }

        ecu_dlist_sort(&list, &condition, ECU_DNODE_OBJ_UNUSED);

    .. figure:: /images/dlist/ecu_dlist_sort.svg
        :width: 700
        :align: center

        ecu_dlist_sort()

ecu_dlist_swap()
"""""""""""""""""""""""""""""""""""""""""""""""""
Swaps nodes between two lists.

    .. figure:: /images/dlist/ecu_dlist_swap.svg
        :width: 700
        :align: center

        ecu_dlist_swap()

If one list is empty, the swapped list will become empty. For example:

    .. figure:: /images/dlist/ecu_dlist_swap_empty.svg
        :width: 600
        :align: center

        Empty ecu_dlist_swap()

ecu_dlist_valid()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the supplied list has been constructed. False otherwise.

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

Iterates (for-loops) over list nodes, starting at the specified position. The specified starting node is included in the iteration, the iteration terminates after the TAIL is reached, and it is safe to remove the current node in the iteration.

Consider the following list:

    .. figure:: /images/dlist/ecu_dlist_at_for_each.svg
        :width: 600
        :align: center

        ECU_DLIST_AT_FOR_EACH()

    .. code-block:: c

        ECU_DLIST_AT_FOR_EACH(i, &iterator, &list, &node1)
        {
            /* Returns &node1, &node2, &node3 */
        }

        ECU_DLIST_AT_FOR_EACH(i, &iterator, &list, &node2)
        {
            /* Returns &node2, &node3 */
        }

        ECU_DLIST_AT_FOR_EACH(i, &iterator, &list, &node3)
        {
            /* Returns &node3 */
        }

    .. warning::

        Behavior is undefined if the starting node is not within the list being iterated over.

ECU_DLIST_CONST_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_DLIST_AT_FOR_EACH() <dlist_ecu_dlist_at_for_each>`. Returned nodes are read-only.

ECU_DLIST_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _dlist_ecu_dlist_for_each:

Iterates (for-loops) over all list nodes starting at HEAD. HEAD is not included in the iteration, the iteration terminates after the TAIL is reached, and it is safe to remove the current node in the iteration.

Consider the following lists:

    .. figure:: /images/dlist/ecu_dlist_for_each.svg
        :width: 600
        :align: center

        ECU_DLIST_FOR_EACH()

    .. code-block:: c

        ECU_DLIST_FOR_EACH(i, &iterator, &list1)
        {
            /* Returns &node1, &node2, &node3 */
        }

        ECU_DLIST_FOR_EACH(i, &iterator, &list2)
        {
            /* Immediatley exits since list2 is empty. */
        }

ECU_DLIST_CONST_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_DLIST_FOR_EACH() <dlist_ecu_dlist_for_each>`. Returned nodes are read-only.
