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

N-ary tree **without** dynamic memory allocation or recursion. Nodes can 
store any user data by value by being intrusive.


Theory 
=================================================

Tree Representation
-------------------------------------------------
Tree nodes are represented by the :ecudoxygen:`ecu_ntnode` struct. Root
and descendant (children, grandchildren, etc) nodes are both represented by this type.

Internally, :ecudoxygen:`ecu_ntnode` is a linked list node that also contains
a linked list of children:

.. figure:: /images/ntnode/tree_representation_linked_list.svg
    :width: 1000
    :align: center

    Tree Node Internals

Therefore inserting tree nodes simply involves inserting nodes into a linked list. 
In the example below node4 is added as a last child of node0:

.. figure:: /images/ntnode/tree_representation_inserting.svg
    :width: 600
    :align: center

    Inserting

With the accompanying pseudocode being:

.. code-block:: c

    linked_list_push_back(node0_children_linked_list, node4);

Notice how node4's tree is automatically preserved during the operation. Removing tree 
nodes works in a similar fashion. The node is simply removed from the linked list:

.. figure:: /images/ntnode/tree_representation_removing.svg
    :width: 600
    :align: center

    Removing

With the accompanying pseudocode being:

.. code-block:: c

    linked_list_node_remove(node4);

.. note::

    To avoid confusion, the rest of this documentation uses the standard tree depiction.
    **Both representations are functionally the same.**

    .. figure:: /images/ntnode/tree_representation_standard_tree_depiction.svg
        :width: 500
        :align: center

        Standard vs Linked List Tree Depiction

Each tree must always have a root. The bottom tree in the example below is 
not valid since it does not have a root. The API's insertion and removal functions
enforce this requirement. I.e. trying to insert a sibling next to a root triggers
an assertion.

.. figure:: /images/ntnode/tree_representation_root.svg
    :width: 400
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
        struct user_node *n = ECU_NTNODE_GET_ENTRY(first_child, struct user_node, node);
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
**additional** cleanup specific to the the user's data. For example cleaning up
a file system or a GUI represented as a tree. The API handles 
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

    /* Data types of nodes stored in tree. */
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
explained in :ref:`Node Data Section <ntnode_node_data>`.

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

    Must be called once on startup before the node is used. User is responsible 
    for allocating memory since ECU does not use dynamic memory allocation.

.. code-block:: c 

    struct ecu_ntnode node;     /* User must allocate memory before constructor. */
    ecu_ntnode_remove(&node);   /* ILLEGAL. Must construct before using. */

    ecu_ntnode_ctor(&node, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    ecu_ntnode_remove(&node);   /* Ok. */

ecu_ntnode_destroy()
"""""""""""""""""""""""""""""""""""""""""""""""""
Destructor. If supplied node is in a tree then it is removed. The :ecudoxygen:`ecu_ntnode` 
data structure is then unitialized such that is has to be reconstructed in order to be 
used again. If the node has a destroy callback it is ran. See :ref:`Node Destroy 
Callback Section <ntnode_node_destroy_callback>` for more details.

.. warning:: 

    Memory is **not** freed when a node is destroyed since ECU library does not use 
    dynamic memory allocation. Node can be freed within its destroy callback 
    if it was allocated on the heap.

.. code-block:: c 

    struct ecu_ntnode node;     /* User must allocate memory before constructor. */
    ecu_ntnode_ctor(&node, &destroy, ECU_OBJECT_ID_UNUSED);
    ecu_ntnode_destroy(&node);  /* ecu_ntnode uninitialized and destroy(&node) ran. */

If the supplied node has descendants (children, grandchildren, etc), they are all
destroyed. For example:

.. figure:: /images/ntnode/ecu_ntnode_destroy.svg
    :width: 500
    :align: center
  
    ecu_ntnode_destroy()

Node2 is destroyed. Nodes 4, 5, 6, 8, and 9 are also destroyed since they are node 2's
descendants. If node7 was destroyed, no other nodes would be destroyed since it is has
no descendants.

.. warning:: 

    Currently nodes are destroyed in postorder order. However this may be subject to change.
    The API only guarantees the proper nodes are destroyed. Application should not rely 
    on destruction order.

Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ecu_ntnode_clear()
"""""""""""""""""""""""""""""""""""""""""""""""""
Removes the supplied node and all of its descendants from a tree. All nodes can be 
reused and added to another tree without reconstruction:

.. figure:: /images/ntnode/ecu_ntnode_clear.svg
    :width: 600
    :align: center
  
    ecu_ntnode_clear()

Node2 is cleared. Nodes 4, 5, 6, 8, and 9 are also cleared since they are node 2's
descendants. If node7 was cleared, no other nodes would be effected since it is has
no descendants.

ecu_ntnode_count()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the number of direct children the supplied node has. Grandchildren, 
great-granchildren, etc are not counted. Returns 0 if the node has no children. 
Consider the following example tree:

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

Returns the node's first (leftmost) child. NULL is returned if the node has no 
children. Consider the following example tree:

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
Returns the node's ID assigned when it was constructed. 
See :ref:`Node Id Section <ntnode_node_id>` for more details.

.. code-block:: c
    
    struct ecu_ntnode node;
    ecu_ntnode_ctor(&node, ECU_NTNODE_DESTROY_UNUSED, 3);
    ecu_ntnode_id(&node); /* Returns 3. */

ecu_ntnode_in_tree()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the node is in a tree. False otherwise.
Note that a root with children is considered to be in a tree.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_in_tree.svg
    :width: 400
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
Inserts a sibling node into the tree after the specified position:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_after_node_is_empty.svg
    :width: 650
    :align: center
  
    ecu_ntnode_insert_sibling_after()

The node being inserted cannot be in a tree unless it is a root. Thus the 
following operation is illegal:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_after_illegal_node_in_tree.svg
    :width: 700
    :align: center
  
    Illegal ecu_ntnode_insert_sibling_after()

The position node cannot be a root since every tree must have one root.
The following operation is illegal since inserting a sibling will cause
the tree to no longer have a root:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_after_illegal_pos_is_root.svg
    :width: 700
    :align: center

    Illegal ecu_ntnode_insert_sibling_after()

If the node being inserted is the root of a tree, its tree remains intact
after insertion. For example:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_after_node_is_root.svg
    :width: 650
    :align: center
  
    ecu_ntnode_insert_sibling_after()

ecu_ntnode_insert_sibling_before()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts a sibling node into the tree before the specified position:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_before_node_is_empty.svg
    :width: 700
    :align: center
  
    ecu_ntnode_insert_sibling_before()

The node being inserted cannot be in a tree unless it is a root. Thus the 
following operation is illegal:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_before_illegal_node_in_tree.svg
    :width: 700
    :align: center
  
    Illegal ecu_ntnode_insert_sibling_before()

The position node cannot be a root since every tree must have one root.
The following operation is illegal since inserting a sibling will cause
the tree to no longer have a root:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_before_illegal_pos_is_root.svg
    :width: 700
    :align: center

    Illegal ecu_ntnode_insert_sibling_before()

If the node being inserted is the root of a tree, its tree remains intact
after insertion. For example:

.. figure:: /images/ntnode/ecu_ntnode_insert_sibling_before_node_is_root.svg
    :width: 700
    :align: center
  
    ecu_ntnode_insert_sibling_before()

ecu_ntnode_is_descendant()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the node is in a tree and is not the root. False otherwise.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_is_descendant.svg
    :width: 400
    :align: center
  
    ecu_ntnode_is_descendant()

.. code-block:: c

    ecu_ntnode_is_descendant(&node0); /* Returns false since node is root. */
    ecu_ntnode_is_descendant(&node1); /* Returns true. */
    ecu_ntnode_is_descendant(&node2); /* Returns true. */
    ecu_ntnode_is_descendant(&node3); /* Returns true. */
    ecu_ntnode_is_descendant(&node4); /* Returns false since node is empty. */

ecu_ntnode_is_leaf()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the node is a leaf, meaning it has no children. False otherwise. 
Note that this returns true for an empty node since that is technically a leaf.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_is_leaf.svg
    :width: 400
    :align: center
  
    ecu_ntnode_is_leaf()

.. code-block:: c

    ecu_ntnode_is_leaf(&node0); /* Returns false. */
    ecu_ntnode_is_leaf(&node1); /* Returns true. */
    ecu_ntnode_is_leaf(&node2); /* Returns false. */
    ecu_ntnode_is_leaf(&node3); /* Returns true. */
    ecu_ntnode_is_leaf(&node4); /* Returns true since an empty node is technically a leaf. */

ecu_ntnode_is_root()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if the node is a root, meaning it has no parent. False otherwise. 
Note that this returns true for an empty node since that is techincally a root.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_is_root.svg
    :width: 400
    :align: center
  
    ecu_ntnode_is_root()

.. code-block:: c

    ecu_ntnode_is_root(&node0); /* Returns true. */
    ecu_ntnode_is_root(&node1); /* Returns false. */
    ecu_ntnode_is_root(&node2); /* Returns false. */
    ecu_ntnode_is_root(&node3); /* Returns false. */
    ecu_ntnode_is_root(&node4); /* Returns true since an empty node is technically a root. */

ecu_ntnode_last_child()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_last_child:

Returns the node's last (rightmost) child. NULL is returned if the node has 
no children. Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_last_child.svg
    :width: 400
    :align: center
  
    ecu_ntnode_last_child()

.. code-block:: c

    ecu_ntnode_last_child(&node0); /* Returns &node2. */
    ecu_ntnode_last_child(&node1); /* Returns NULL. */
    ecu_ntnode_last_child(&node2); /* Returns &node3. */
    ecu_ntnode_last_child(&node3); /* Returns NULL. */
    ecu_ntnode_last_child(&node4); /* Returns NULL. */

ecu_ntnode_last_cchild()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_ntnode_last_child() <ntnode_ecu_ntnode_last_child>`.

ecu_ntnode_lca()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_lca:

Returns the least common ancestor of the two supplied nodes. NULL is returned 
if the nodes are in separate trees and do not have an LCA. 
Consider the following example trees:

.. figure:: /images/ntnode/ecu_ntnode_lca.svg
    :width: 600
    :align: center
  
    ecu_ntnode_lca()

.. code-block:: c

    ecu_ntnode_lca(&node2, &node7); /* Returns &node0. */
    ecu_ntnode_lca(&node7, &node5); /* Returns &node3. */
    ecu_ntnode_lca(&node5, &node7); /* Returns &node3. */
    ecu_ntnode_lca(&node1, &node2); /* Returns &node1. */
    ecu_ntnode_lca(&node2, &node5); /* Returns &node0. */
    ecu_ntnode_lca(&node0, &node8); /* Returns NULL. */
    ecu_ntnode_lca(&node2, &node8); /* Returns NULL. */
    ecu_ntnode_lca(&node5, &node9); /* Returns NULL. */

ecu_ntnode_clca()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_ntnode_lca() <ntnode_ecu_ntnode_lca>`.

ecu_ntnode_level()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns which level of the tree the supplied node is in. Returns 0 if the 
supplied node is a root. Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_level.svg
    :width: 400
    :align: center
  
    ecu_ntnode_level()

.. code-block:: c

    ecu_ntnode_level(&node0); /* Returns 0. */
    ecu_ntnode_level(&node1); /* Returns 1. */
    ecu_ntnode_level(&node2); /* Returns 1. */
    ecu_ntnode_level(&node3); /* Returns 2. */
    ecu_ntnode_level(&node4); /* Returns 0. */

ecu_ntnode_next()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_next:

Returns the node's next (right) sibling. NULL is returned if the node is 
the last (rightmost) sibling or has no siblings. Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_next.svg
    :width: 400
    :align: center
  
    ecu_ntnode_next()

.. code-block:: c

    ecu_ntnode_next(&node0); /* Returns NULL. */
    ecu_ntnode_next(&node1); /* Returns &node2. */
    ecu_ntnode_next(&node2); /* Returns &node3. */
    ecu_ntnode_next(&node3); /* Returns NULL. */
    ecu_ntnode_next(&node4); /* Returns NULL. */
    ecu_ntnode_next(&node5); /* Returns NULL. */

ecu_ntnode_cnext()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_ntnode_next() <ntnode_ecu_ntnode_next>`.

ecu_ntnode_parent()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_parent:

Returns the supplied node's parent. NULL is returned if the node is a root.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_parent.svg
    :width: 400
    :align: center
  
    ecu_ntnode_parent()

.. code-block:: c

    ecu_ntnode_parent(&node0); /* Returns NULL. */
    ecu_ntnode_parent(&node1); /* Returns &node0. */
    ecu_ntnode_parent(&node2); /* Returns &node0. */
    ecu_ntnode_parent(&node3); /* Returns &node2. */
    ecu_ntnode_parent(&node4); /* Returns NULL. */

ecu_ntnode_cparent()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_ntnode_parent() <ntnode_ecu_ntnode_parent>`.

ecu_ntnode_prev()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_prev:

Returns the node's previous (left) sibling. NULL is returned if the node is the 
first (leftmost) sibling or has no siblings. Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_prev.svg
    :width: 400
    :align: center
  
    ecu_ntnode_prev()

.. code-block:: c

    ecu_ntnode_prev(&node0); /* Returns NULL. */
    ecu_ntnode_prev(&node1); /* Returns NULL. */
    ecu_ntnode_prev(&node2); /* Returns &node1. */
    ecu_ntnode_prev(&node3); /* Returns &node2. */
    ecu_ntnode_prev(&node4); /* Returns NULL. */
    ecu_ntnode_prev(&node5); /* Returns NULL. */

ecu_ntnode_cprev()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ecu_ntnode_prev() <ntnode_ecu_ntnode_prev>`.

ecu_ntnode_push_child_back()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts a rightmost child node into the tree. Node being inserted
becomes the supplied parent's last (rightmost) child:

.. figure:: /images/ntnode/ecu_ntnode_push_child_back_node_is_empty.svg
    :width: 700
    :align: center
  
    ecu_ntnode_push_child_back()

The node being inserted cannot be in a tree unless it is a root. Thus the 
following operation is illegal:

.. figure:: /images/ntnode/ecu_ntnode_push_child_back_illegal_node_in_tree.svg
    :width: 700
    :align: center
  
    Illegal ecu_ntnode_push_child_back()

If the node being inserted is the root of a tree, its tree remains intact
after insertion. For example:

.. figure:: /images/ntnode/ecu_ntnode_push_child_back_node_is_root.svg
    :width: 650
    :align: center
  
    ecu_ntnode_push_child_back()

ecu_ntnode_push_child_front()
"""""""""""""""""""""""""""""""""""""""""""""""""
Inserts a leftmost child node into the tree. Node being inserted
becomes the supplied parent's first (leftmost) child:

.. figure:: /images/ntnode/ecu_ntnode_push_child_front_node_is_empty.svg
    :width: 700
    :align: center
  
    ecu_ntnode_push_child_front()

The node being inserted cannot be in a tree unless it is a root. Thus the 
following operation is illegal:

.. figure:: /images/ntnode/ecu_ntnode_push_child_front_illegal_node_in_tree.svg
    :width: 800
    :align: center
  
    Illegal ecu_ntnode_push_child_front()

If the node being inserted is the root of a tree, its tree remains intact
after insertion. For example:

.. figure:: /images/ntnode/ecu_ntnode_push_child_front_node_is_root.svg
    :width: 650
    :align: center
  
    ecu_ntnode_push_child_front()

ecu_ntnode_remove()
"""""""""""""""""""""""""""""""""""""""""""""""""
Removes node from a tree. 

.. figure:: /images/ntnode/ecu_ntnode_remove_node_is_leaf.svg
    :width: 500
    :align: center
  
    ecu_ntnode_remove()

If the removed node has descendants, the node becomes a new root with its tree intact:

.. figure:: /images/ntnode/ecu_ntnode_remove_node_in_tree.svg
    :width: 600
    :align: center
  
    ecu_ntnode_remove()

The tree remains unchanged if an empty or root node is supplied:

.. figure:: /images/ntnode/ecu_ntnode_remove_node_is_root.svg
    :width: 500
    :align: center
  
    ecu_ntnode_remove()

ecu_ntnode_size()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns the total number of descendants (children, grandchildren, etc) the node has. 
Returns 0 if the node has no descendants. Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_size.svg
    :width: 450
    :align: center
  
    ecu_ntnode_size()

.. code-block:: c

    ecu_ntnode_size(&node0); /* Returns 5. */
    ecu_ntnode_size(&node1); /* Returns 0. */
    ecu_ntnode_size(&node2); /* Returns 3. */
    ecu_ntnode_size(&node3); /* Returns 0. */
    ecu_ntnode_size(&node4); /* Returns 1. */
    ecu_ntnode_size(&node5); /* Returns 0. */
    ecu_ntnode_size(&node6); /* Returns 0. */

ecu_ntnode_valid()
"""""""""""""""""""""""""""""""""""""""""""""""""
Returns true if supplied node has been constructed and can be used. 
False otherwise.

.. code-block:: c

    struct ecu_ntnode node;
    ecu_ntnode_valid(&node); /* Returns false. */

    ecu_ntnode_ctor(&node, ...);
    ecu_ntnode_valid(&node); /* Returns true. */

    ecu_ntnode_destroy(&node);
    ecu_ntnode_valid(&node); /* Returns false. */

Iterators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. note::

    All iterators do **not** use recursion.

ECU_NTNODE_CHILD_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_child_for_each:

Iterates (for-loops) over all direct children. Grandchildren, great-grandchildren, 
etc are not included in the iteration. Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_child_for_each.svg
    :width: 400
    :align: center
  
    ECU_NTNODE_CHILD_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node0 returns in this order: node1, node2, node3.
+ Iterating over node2 returns in this order node4, node5.
+ Iterating over node3 immediately terminates since node3 has no children.

.. code-block:: c

    struct ecu_ntnode_child_iterator iter;

    ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 returns in this order: &node1, &node2, &node3. */
    }

    ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node4, &node5. */
    }

    ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &node3)
    {
        /* Iterating over &node3 immediately terminates since node3 has no children. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all of its children:

.. figure:: /images/ntnode/ecu_ntnode_child_for_each_remove.svg
    :width: 650
    :align: center
  
    ECU_NTNODE_CHILD_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_child_iterator iter;

    ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_CHILD_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_CHILD_FOR_EACH() <ntnode_ecu_ntnode_child_for_each>`.
Returned nodes are read-only.

ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_next_sibling_at_for_each:

Iterates (for-loops) over all next (right) siblings, including the supplied 
starting node. Terminates after the last (rightmost) sibling is reached.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_next_sibling_at_for_each.svg
    :width: 500
    :align: center
  
    ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node0 returns node0 then terminates since it has no siblings.
+ Iterating over node1 returns in this order: node1, node2, node3, node4.
+ Iterating over node2 returns in this order: node2, node3, node4.
+ Iterating over node4 returns node4 then terminates since it is the last sibling.

.. code-block:: c

    struct ecu_ntnode_next_sibling_iterator iter;

    ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 returns &node0 then terminates since it has no siblings. */
    }

    ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &node1)
    {
        /* Iterating over &node1 returns in this order: &node1, &node2, &node3, &node4. */
    }

    ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node2, &node3, &node4. */
    }

    ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &node4)
    {
        /* Iterating over &node4 returns &node4 then terminates since it is the last sibling. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all of its 
next siblings:

.. figure:: /images/ntnode/ecu_ntnode_next_sibling_at_for_each_remove.svg
    :width: 700
    :align: center
  
    ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_next_sibling_iterator iter;

    ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH() 
<ntnode_ecu_ntnode_next_sibling_at_for_each>`. Returned nodes are read-only.

ECU_NTNODE_NEXT_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_next_sibling_for_each:

Same as :ref:`ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH() <ntnode_ecu_ntnode_next_sibling_at_for_each>`
but excludes the starting node from the iteration.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_next_sibling_for_each.svg
    :width: 500
    :align: center
  
    ECU_NTNODE_NEXT_SIBLING_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node0 immediately terminates since node0 has no siblings.
+ Iterating over node1 returns in this order: node2, node3, node4.
+ Iterating over node2 returns in this order: node3, node4.
+ Iterating over node4 immediately terminates since node4 is the last sibling.

.. code-block:: c

    struct ecu_ntnode_next_sibling_iterator iter;

    ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 immediately terminates since &node0 has no siblings. */
    }

    ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &node1)
    {
        /* Iterating over &node1 returns in this order: &node2, &node3, &node4. */
    }

    ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node3, &node4. */
    }

    ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &node4)
    {
        /* Iterating over &node4 immediately terminates since &node4 is the last sibling. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all of its
next siblings:

.. figure:: /images/ntnode/ecu_ntnode_next_sibling_for_each_remove.svg
    :width: 600
    :align: center
  
    ECU_NTNODE_NEXT_SIBLING_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_next_sibling_iterator iter;

    ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_NEXT_SIBLING_FOR_EACH() 
<ntnode_ecu_ntnode_next_sibling_for_each>`. Returned nodes are read-only.

ECU_NTNODE_PARENT_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_parent_at_for_each:

Iterates (for-loops) over all parents, including the supplied 
starting node, by traversing up the tree. 
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_parent_at_for_each.svg
    :width: 250
    :align: center
  
    ECU_NTNODE_PARENT_AT_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node2 returns in this order: node2, node1, node0.
+ Iterating over node1 returns in this order: node1, node0.
+ Iterating over node0 returns node0 then terminates since node0 is a root.

.. code-block:: c

    struct ecu_ntnode_parent_iterator iter;

    ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node2, &node1, &node0. */
    }

    ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &node1)
    {
        /* Iterating over &node1 returns in this order: &node1, &node0. */
    }

    ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 returns &node0 then terminates since &node0 is a root. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all parents:

.. figure:: /images/ntnode/ecu_ntnode_parent_at_for_each_remove.svg
    :width: 700
    :align: center
  
    ECU_NTNODE_PARENT_AT_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_parent_iterator iter;

    ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_PARENT_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_PARENT_AT_FOR_EACH() 
<ntnode_ecu_ntnode_parent_at_for_each>`. Returned nodes are read-only.

ECU_NTNODE_PARENT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_parent_for_each:

Same as :ref:`ECU_NTNODE_PARENT_AT_FOR_EACH() <ntnode_ecu_ntnode_parent_at_for_each>`
but excludes the starting node from the iteration.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_parent_for_each.svg
    :width: 250
    :align: center
  
    ECU_NTNODE_PARENT_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node2 returns in this order: node1, node0.
+ Iterating over node1 returns node0.
+ Iterating over node0 immediately terminates since node0 is a root.

.. code-block:: c

    struct ecu_ntnode_parent_iterator iter;

    ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node1, &node0. */
    }

    ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &node1)
    {
        /* Iterating over &node1 returns &node0. */
    }

    ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 immediately terminates since &node0 is a root. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all parents:

.. figure:: /images/ntnode/ecu_ntnode_parent_for_each_remove.svg
    :width: 500
    :align: center
  
    ECU_NTNODE_PARENT_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_parent_iterator iter;

    ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_PARENT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_PARENT_FOR_EACH() 
<ntnode_ecu_ntnode_parent_for_each>`. Returned nodes are read-only.

ECU_NTNODE_POSTORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_postorder_for_each:

Performs a postorder iteration (for-loop) over a tree. 
The root node is included in the iteration. 
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_postorder_for_each.svg
    :width: 400
    :align: center
  
    ECU_NTNODE_POSTORDER_FOR_EACH()

Iterating over node0 returns in this order: node1, node6, node4, node5, node2,
node3, node0:

.. code-block:: c

    struct ecu_ntnode_postorder_iterator iter;

    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 returns in this order: &node1, &node6, 
        &node4, &node5, &node2, &node3, &node0. */
    }

Iterating over a child returns only its direct descendants. Using the
same example tree as above, iterating over node2 returns in this order:
node6, node4, node5, node2.

.. code-block:: c

    struct ecu_ntnode_postorder_iterator iter;

    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node6, 
        &node4, &node5, &node2. */
    }

It is safe to remove or destroy the current node in the iteration.

ECU_NTNODE_CONST_POSTORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_POSTORDER_FOR_EACH() 
<ntnode_ecu_ntnode_postorder_for_each>`. Returned nodes are read-only.

ECU_NTNODE_PREORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_preorder_for_each:

Performs a preorder iteration (for-loop) over a tree. 
The root node is included in the iteration.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_preorder_for_each.svg
    :width: 400
    :align: center
  
    ECU_NTNODE_PREORDER_FOR_EACH()

Iterating over node0 returns in this order: node0, node1, node2, node4,
node6, node5, node3:

.. code-block:: c

    struct ecu_ntnode_preorder_iterator iter;

    ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 returns in this order: &node0, &node1,
        &node2, &node4, &node6, &node5, &node3. */
    }

Iterating over a child returns only its direct descendants. Using the
same example tree as above, iterating over node2 returns in this order:
node2, node4, node6, node5:

.. code-block:: c

    struct ecu_ntnode_preorder_iterator iter;

    ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node2, &node4,
        &node6, &node5. */
    }

.. warning::
    
    Removing or destroying the current node in the iteration is 
    NOT allowed since this is an unsafe operation during preorder 
    traversal.

ECU_NTNODE_CONST_PREORDER_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_PREORDER_FOR_EACH() 
<ntnode_ecu_ntnode_preorder_for_each>`. Returned nodes are read-only.

ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_prev_sibling_at_for_each:

Iterates (for-loops) over all previous (left) siblings, 
including the supplied starting node. Terminates after the
first (leftmost) sibling is reached. Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_prev_sibling_at_for_each.svg
    :width: 500
    :align: center
  
    ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node0 returns node0 then terminates since it has no siblings.
+ Iterating over node1 returns node1 then terminates since it is the first sibling.
+ Iterating over node2 returns in this order: node2, node1.
+ Iterating over node4 returns in this order: node4, node3, node2, node1.

.. code-block:: c

    struct ecu_ntnode_prev_sibling_iterator iter;

    ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 returns &node0 then terminates since it has no siblings. */
    }

    ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH(n, &iter, &node1)
    {
        /* Iterating over &node1 returns &node1 then terminates since it is the first sibling. */
    }

    ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node2, &node1. */
    }

    ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH(n, &iter, &node4)
    {
        /* Iterating over &node4 returns in this order: &node4, &node3, &node2, &node1. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all of its
previous siblings:

.. figure:: /images/ntnode/ecu_ntnode_prev_sibling_at_for_each_remove.svg
    :width: 600
    :align: center
  
    ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_prev_sibling_iterator iter;

    ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_PREV_SIBLING_AT_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH() 
<ntnode_ecu_ntnode_prev_sibling_at_for_each>`. Returned nodes are read-only.

ECU_NTNODE_PREV_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_prev_sibling_for_each:

Same as :ref:`ECU_NTNODE_PREV_SIBLING_AT_FOR_EACH() <ntnode_ecu_ntnode_prev_sibling_at_for_each>`
but excludes the starting node from the iteration.
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_prev_sibling_for_each.svg
    :width: 500
    :align: center
  
    ECU_NTNODE_PREV_SIBLING_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node0 immediately terminates since node0 has no siblings.
+ Iterating over node1 immediately terminates since node1 is the first sibling.
+ Iterating over node2 returns node1.
+ Iterating over node4 returns in this order: node3, node2, node1.

.. code-block:: c

    struct ecu_ntnode_prev_sibling_iterator iter;

    ECU_NTNODE_PREV_SIBLING_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 immediately terminates since &node0 has no siblings. */
    }

    ECU_NTNODE_PREV_SIBLING_FOR_EACH(n, &iter, &node1)
    {
        /* Iterating over &node1 immediately terminates since &node1 is the first sibling. */
    }

    ECU_NTNODE_PREV_SIBLING_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns &node1. */
    }

    ECU_NTNODE_PREV_SIBLING_FOR_EACH(n, &iter, &node4)
    {
        /* Iterating over &node4 returns in this order: &node3, &node2, &node1. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all of its
previous siblings:

.. figure:: /images/ntnode/ecu_ntnode_prev_sibling_for_each_remove.svg
    :width: 600
    :align: center
  
    ECU_NTNODE_PREV_SIBLING_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_prev_sibling_iterator iter;

    ECU_NTNODE_PREV_SIBLING_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_PREV_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_PREV_SIBLING_FOR_EACH() 
<ntnode_ecu_ntnode_prev_sibling_for_each>`. Returned nodes are read-only.

ECU_NTNODE_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
.. _ntnode_ecu_ntnode_sibling_for_each:

Iterates (for-loops) over all siblings, excluding the starting 
node. Performs wraparound if starting sibling is not the 
first sibling. 
Consider the following example tree:

.. figure:: /images/ntnode/ecu_ntnode_sibling_for_each.svg
    :width: 500
    :align: center
  
    ECU_NTNODE_SIBLING_FOR_EACH()

Iterating over some nodes returns the following:

+ Iterating over node0 immediately terminates since node0 has no siblings.
+ Iterating over node1 returns in this order: node2, node3, node4.
+ Iterating over node2 returns in this order: node3, node4, node1.
+ Iterating over node4 returns in this order: node1, node2, node3.

.. code-block:: c

    struct ecu_ntnode_sibling_iterator iter;

    ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &node0)
    {
        /* Iterating over &node0 immediately terminates since &node0 has no siblings. */
    }

    ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &node1)
    {
        /* Iterating over &node1 returns in this order: &node2, &node3, &node4. */
    }

    ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node2 returns in this order: &node3, &node4, &node1. */
    }

    ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &node2)
    {
        /* Iterating over &node4 returns in this order: &node1, &node2, &node3. */
    }

It is safe to remove or destroy the current node in the iteration.
The following example iterates over node2 and removes all of its
siblings:

.. figure:: /images/ntnode/ecu_ntnode_sibling_for_each_remove.svg
    :width: 600
    :align: center
  
    ECU_NTNODE_SIBLING_FOR_EACH() Remove

.. code-block:: c

    struct ecu_ntnode_sibling_iterator iter;

    ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &node2)
    {
        ecu_ntnode_remove(n);
    }

ECU_NTNODE_CONST_SIBLING_FOR_EACH()
"""""""""""""""""""""""""""""""""""""""""""""""""
Const-qualified version of :ref:`ECU_NTNODE_SIBLING_FOR_EACH() 
<ntnode_ecu_ntnode_sibling_for_each>`. Returned nodes are read-only.
