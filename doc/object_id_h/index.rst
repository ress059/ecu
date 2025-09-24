.. _object_id_h:

object_id.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
.. note:: 

    The term :term:`ECU` in this document refers to Embedded C Utilities, 
    the shorthand name for this project.

Allows users to identify different types stored in the same data structure
without conflicting ID values.


Theory
=================================================

Object ID Overview
-------------------------------------------------
An object ID is a unique integer value that can be assigned to different
data types. The scheme presented in this module prevents an object ID used 
internally by ECU and a user's object ID from sharing the same value.
This is accomplished by starting user-defined object ID's at :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN`:

.. code-block:: c

    enum user_object_ids
    {
        USER_OBJECT_ID_1 = ECU_USER_OBJECT_ID_BEGIN,
        USER_OBJECT_ID_2,
        USER_OBJECT_ID_3
    };

:ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN` is guaranteed to always be 0. IDs less 
than :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN` are reserved for internal use 
by ECU library. Therefore reserved IDs are always negative. User-defined IDs
are always >= 0.

Example
-------------------------------------------------
The following example stores different node types in the same linked list
(:ref:`dlist.h <dlist_h>`) and uses the object ID scheme to identify them:

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

    /* Construct list and nodes. Assign object IDs to each node to identify their data types. */
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
                break;
            }
        }
    }


API 
=================================================
.. toctree::
    :maxdepth: 1

    object_id.h </doxygen/html/object__id_8h>
