.. _ntnode_h:

ntnode.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

N-ary tree. Nodes can store any user data by value without dynamic 
memory allocation by being intrusive.


Theory 
=================================================

Tree Representation
-------------------------------------------------
Tree nodes are represented by the :ecudoxygen:`ecu_ntnode` struct. Root nodes
and descendant (in tree and not root) nodes are both represented by this type.

Internally, the :ecudoxygen:`ecu_ntnode` is a linked list node that also contains
a linked list of children:

    .. figure:: /images/ntnode/tree_representation_linked_list.svg
        :width: 500
        :align: center
    
    Tree Node Internals

Therefore inserting tree nodes simply involves inserting nodes into a linked list. 
In the example below node4 is added as a child of node0:

    .. figure:: /images/ntnode/tree_representation_inserting.svg
        :width: 500
        :align: center
    
    Inserting

    .. code-block:: c

        linked_list_push_back(node0_children_linked_list, node4_linked_list_node);

Notice how node4's tree is automatically preserved during the operation. 

Removing tree nodes works in a similar fashion. The node is simply removed from
the linked list:

    .. figure:: /images/ntnode/tree_representation_removing.svg
        :width: 500
        :align: center
    
    Removing

    .. code-block:: c

        linked_list_node_remove(node4_linked_list_node);

.. note::

    To avoid confusion, the rest of this documentation uses the standard tree depiction.
    **Both representations are the functionally the same.**

    .. figure:: /images/ntnode/tree_representation_standard_tree_depiction.svg
        :width: 500
        :align: center

    Standard vs Linked List Tree Depiction

Each tree must always have a root. The bottom tree in the example below is 
not valid since it does not have a root. The API's insertion and removal functions
enforce this requirement. I.e. trying to insert a sibling to a root is asserted.

    .. figure:: /images/ntnode/tree_representation_root.svg
        :width: 500
        :align: center

    Tree Root

Node Data
-------------------------------------------------
.. _ntnode_node_data:

Tree nodes store data without dynamic memory allocation by being intrusive.
The user's node consists of data and an :ecudoxygen:`ecu_ntnode` member.

.. code-block:: c

    struct user_node
    {
        /* User data. */
        int a;
        int b;

        /* Store ecu_ntnode in user node. */
        struct ecu_ntnode node;

        /* More user data. */
        int c;
    };

The :ecudoxygen:`ecu_ntnode` member is passed and returned by the API:

.. code-block:: c

    struct user_node n1, n2;
    ecu_ntnode_ctor(&n1.node, ...);
    ecu_ntnode_ctor(&n2.node, ...);
    ecu_ntnode_push_child_back(&n1.node, &n2.node);
    struct ecu_ntnode *first_child = ecu_ntnode_first_child(&n1.node);

The API has no knowledge of the user's node type so it must only use :ecudoxygen:`ecu_ntnode`
to remain portable. :ecudoxygen:`ECU_NTNODE_GET_ENTRY() <ECU_NTNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_NTNODE_GET_CONST_ENTRY() <ECU_NTNODE_GET_CONST_ENTRY>` convert an :ecudoxygen:`ecu_ntnode`
into the user's node type, allowing data to be retrieved. Following up on the 
previous code example above: 

.. code-block:: c

    struct ecu_ntnode *first_child = ecu_ntnode_first_child(&n1.node);

    if (first_child)
    {
        struct user_node *n = ECU_NTNODE_GET_ENTRY(first_child, struct user_node node);
        n->a = 5;
        n->b = 5;
        n->c = 5;
    }

:ecudoxygen:`ECU_NTNODE_GET_ENTRY() <ECU_NTNODE_GET_ENTRY>` and 
:ecudoxygen:`ECU_NTNODE_GET_CONST_ENTRY() <ECU_NTNODE_GET_CONST_ENTRY>` take in three parameters:

    #. ``ptr_`` = Pointer to intrusive ecu_ntnode. In this case, ``first_child``.
    #. ``type_`` = User's node type. In this case, ``struct user_node``.
    #. ``member_`` = Name of ecu_ntnode member within the user's node type. In this case, ``node``.

.. figure:: /images/ntnode/node_data_retrieval.svg
  :width: 500
  :align: center
  
  Get Entry Macro Parameters

Under the hood, these macros perform arithmetic on the supplied :ecudoxygen:`ecu_ntnode`
pointer to convert it back into the user's node type. The details of this operation
are fully explained in :ref:`ECU_CONTAINER_OF() <utils_container_of>` and 
:ref:`ECU_CONST_CONTAINER_OF() <utils_const_container_of>`.

Node Destroy Callback
-------------------------------------------------
.. _ntnode_node_destroy_callback:

A custom destroy callback can be assigned to an :ecudoxygen:`ecu_ntnode` when it
is constructed. This callback executes when the node is destroyed. It performs 
**additional** cleanup specific to the the user's data. The API handles 
uninitializing the :ecudoxygen:`ecu_ntnode` member. For example:

.. code-block:: c

    struct user_node
    {
        int a;
        struct ecu_ntnode node;
    };

    static void custom_destroy(struct ecu_ntnode *me, ecu_object_id id)
    {
        printf("Additional cleanup");
        struct user_node *data = ECU_NTNODE_GET_ENTRY(me, struct user_node, node);
        data->a = 0;
    }

    struct user_node n1;
    ecu_ntnode_ctor(&n1.node, &custom_destroy, ECU_OBJECT_ID_UNUSED);
    ecu_ntnode_destroy(&n1.node); /* custom_destroy(&n1.node) runs. */

.. warning:: 

    The only API calls allowed in the destroy callback are 
    :ecudoxygen:`ECU_NTNODE_GET_ENTRY() <ECU_NTNODE_GET_ENTRY>` and 
    :ecudoxygen:`ECU_NTNODE_GET_CONST_ENTRY() <ECU_NTNODE_GET_CONST_ENTRY>`.
    Violating this rule is undefined behavior.

Node ID 
-------------------------------------------------
.. _ntnode_node_id:

Nodes can be parametrized by unique IDs to identify different
types stored in the same tree. IDs are assigned when each node is 
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
        struct ecu_ntnode node;
    };

    struct type2 
    {
        struct ecu_ntnode node;
        int b;
    };

    struct type3 
    {
        int c;
        struct ecu_ntnode node;
        int d;
    };

    struct type1 n1;
    struct type2 n2;
    struct type3 n3;
    ecu_ntnode_ctor(&n1.node, ECU_NTNODE_DESTROY_UNUSED, TYPE1);
    ecu_ntnode_ctor(&n2.node, ECU_NTNODE_DESTROY_UNUSED, TYPE2);
    ecu_ntnode_ctor(&n3.node, ECU_NTNODE_DESTROY_UNUSED, TYPE3);

    /* Assume tree already constructed. Iterate over tree. Use IDs to 
    identify different data types in the same tree. */
    ECU_NTNODE_POSTORDER_FOR_EACH(i, ....)
    {
        switch (ecu_ntnode_id(i))
        {
            case TYPE1:
            {
                struct type1 *me = ECU_NTNODE_GET_ENTRY(i, struct type1, node);
                me->a = 5;
                break;
            }

            case TYPE2:
            {
                struct type2 *me = ECU_NTNODE_GET_ENTRY(i, struct type2, node);
                me->b = 10;
                break;
            }

            case TYPE3:
            {
                struct type3 *me = ECU_NTNODE_GET_ENTRY(i, struct type3, node);
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

API 
=================================================
.. toctree::
    :maxdepth: 1

    ntnode.h </doxygen/html/ntnode_8h>

Macros
-------------------------------------------------

ECU_NTNODE_GET_ENTRY()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. _ntnode_ecu_ntnode_get_entry:

Retrieves user data stored in an :ecudoxygen:`ecu_ntnode` by converting 
the supplied :ecudoxygen:`ecu_ntnode` back into the user's type. Fully 
explained in :ref:`Node Data Section <ntnode_node_data>`

ECU_NTNODE_GET_CONST_ENTRY()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Const-qualified version of :ref:`ECU_NTNODE_GET_ENTRY() <ntnode_ecu_ntnode_get_entry>`.

ecu_ntode
-------------------------------------------------

Constructors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_ntnode_ctor()
"""""""""""""""""""""""""""""""""""""""""""""""""
Constructor. Initializes the :ecudoxygen:`ecu_ntnode` data structure for use.
An optional destroy callback and object ID can be assigned to the node. See
:ref:`Node Destroy Callback <ntnode_node_destroy_callback>` and
:ref:`Node ID <ntnode_node_id>` sections for more details.

.. warning:: 

    Must be called once on startup before the node is used. User is also responsible 
    for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct ecu_ntnode node;     /* User must allocate memory before constructor. */
    ecu_ntnode_remove(&node);   /* ILLEGAL. Must construct before using. */

    ecu_ntnode_ctor(&node, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_ntnode_remove(&node);   /* Ok. */

ecu_ntnode_destroy()
"""""""""""""""""""""""""""""""""""""""""""""""""
Destructor. If supplied node is in a tree it is removed. The :ecudoxygen:`ecu_ntnode` 
data structure is then unitialized such that is has to be reconstructed in order to be 
used again. If the node has a destroy callback it is ran. See :ref:`Node Destroy 
Callback Section <ntnode_node_destroy_callback>` for more details.

.. code-block:: c 

    struct ecu_ntnode node;     /* User must allocate memory before constructor. */
    ecu_ntnode_ctor(&node, &destroy, ECU_OBJECT_ID_UNUSED);
    ecu_ntnode_destroy(&node);  /* ecu_ntnode uninitialized and destroy(&node) ran. */

If the supplied node has children, all of those children are also destroyed.
For example:

.. figure:: /images/ntnode/ecu_ntnode_destroy.svg
  :width: 500
  :align: center
  
  ecu_ntnode_destroy()

Node2 is destroyed. Nodes 4, 5, 6, 8, and 9 are also destroyed since they are node 2's
children. If node7 was destroyed, no other nodes would be destroyed since it is has
no children.

.. warning:: 

    Memory is **not** freed when a node is destroyed since ECU library does not use 
    dynamic memory allocation. Node can be freed within its destroy callback 
    if it was allocated on the heap.

.. warning:: 

    Currently nodes are destroyed in postorder order. However this may be subject to change.
    The API only guarantees the proper nodes are destroyed. Application should not rely 
    on destruction order.

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_ntnode_clear()
"""""""""""""""""""""""""""""""""""""""""""""""""
Removes node if it is in a tree. All of the node's children are also
removed. For example:

.. figure:: /images/ntnode/ecu_ntnode_clear.svg
  :width: 500
  :align: center
  
  ecu_ntnode_clear()

Node2 is cleared. Nodes 4, 5, 6, 8, and 9 are also cleared since they are node 2's
children. If node7 was cleared, no other nodes would be effected since it is has
no children. Cleared nodes can be reused and readded to trees without reconstruction.

ecu_ntnode_count()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns number of direct children the supplied node has. Grandchildren, 
great-granchildren, etc are not counted. Returns 0 if the node is a leaf 
and has no children. Take the example tree below:

.. figure:: /images/ntnode/ecu_ntnode_count.svg
  :width: 500
  :align: center
  
  ecu_ntnode_count()

.. code-block:: c

    ecu_ntnode_count(&node0); /* Returns 3. */
    ecu_ntnode_count(&node4); /* Returns 2. */
    ecu_ntnode_count(&node3); /* Returns 0. */
    ecu_ntnode_count(&node1); /* Returns 0. */

ecu_ntnode_first_child()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_first_child:

Returns the node's first (leftmost) child. NULL is returned if the node is 
a leaf and has no children. Take the example tree below:

.. figure:: /images/ntnode/ecu_ntnode_first_child.svg
  :width: 500
  :align: center
  
  ecu_ntnode_first_child()

.. code-block:: c

    ecu_ntnode_first_child(&node0); /* Returns &node1. */
    ecu_ntnode_first_child(&node1); /* Returns NULL. */
    ecu_ntnode_first_child(&node4); /* Returns &node5. */
    ecu_ntnode_first_child(&node6); /* Returns NULL. */

ecu_ntnode_first_cchild()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_ntnode_first_cchild() <ntnode_ecu_ntnode_first_child>`.

ecu_ntnode_id()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns node's ID set in constructor. See :ref:`Node Id Section <ntnode_node_id>`.

.. code-block:: c
    
    struct ecu_ntnode node;
    ecu_ntnode_ctor(&node, ECU_NTNODE_DESTROY_UNUSED, 3);
    ecu_ntnode_id(&node); /* Returns 3. */

ecu_ntnode_in_tree()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the node is in a tree. False otherwise.
Note that a root with children is considered to be in a tree.
Consider the following:

.. figure:: /images/ntnode/ecu_ntnode_in_tree.svg
  :width: 500
  :align: center
  
  ecu_ntnode_in_tree()

.. code-block:: c 

    ecu_ntnode_in_tree(&node0); /* Returns true. */
    ecu_ntnode_in_tree(&node1); /* Returns true. */
    ecu_ntnode_in_tree(&node2); /* Returns true. */
    ecu_ntnode_in_tree(&node3); /* Returns true. */
    ecu_ntnode_in_tree(&node4); /* Returns false. */

ecu_ntnode_insert_sibling_after()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts a sibling node into the tree after the specified position.
For example:

 !!figure

The node being inserted cannot be in a tree unless it is a root.
If it is a root, it is still inserted but its tree remains intact.
For example:

!! figure

ecu_ntnode_insert_sibling_before()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_is_descendant()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_is_leaf()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_is_root()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_last_child()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_last_cchild()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_lca()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_clca()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_level()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_next()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_cnext()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_parent()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_cparent()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_prev()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_cprev()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_push_child_back()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_push_child_front()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_remove()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_size()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ecu_ntnode_valid()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

Iterators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ECU_NTNODE_CHILD_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_CONST_CHILD_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_PARENT_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_CONST_PARENT_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_PARENT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_CONST_PARENT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_POSTORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!! mention iterator does not use recursion.

ECU_NTNODE_CONST_POSTORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_PREORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!! mention iterator does not use recursion.

ECU_NTNODE_CONST_PREORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_SIBLING_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_CONST_SIBLING_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!

ECU_NTNODE_CONST_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
TODO!!!
