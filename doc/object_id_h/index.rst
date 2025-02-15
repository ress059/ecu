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

Allows users to identify different types within the same data structure. 
For example, linked list nodes can be assigned unique object IDs that 
indicate the type of data they store.


Defining Object IDs
=================================================
IDs greater than or equal to :ecudoxygen:`ECU_VALID_OBJECT_ID_BEGIN` enumeration
can be assigned to an object.

Users can define their own IDs starting at :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN` 
enumeration. The recommended implementation is as follows:

.. code-block:: c

    #include "ecu/object_id.h"

    /* User-defined object IDs. */
    enum user_object_ids
    {
        TYPE1 = ECU_USER_OBJECT_ID_BEGIN,
        TYPE2,
        TYPE3
    };

This scheme ensures object ID values reserved by ECU never overlap
with user-defined ID values. Some additional notes:

    - Object IDs reserved by ECU will always be negative.

    + Reserved object IDs greater than or equal to :ecudoxygen:`ECU_VALID_OBJECT_ID_BEGIN` 
      enumeration can be assigned to an object. **Currently** :ecudoxygen:`ECU_OBJECT_ID_UNUSED` 
      **is the only reserved ID available for use.**
      
      .. note:: 

        :ecudoxygen:`ECU_VALID_OBJECT_ID_BEGIN` enumeration should not be used 
        in the application. ECU uses this value internally to know when a supplied 
        object has a valid ID value.

    - :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN` enumeration will **always be 0**, and marks the start 
      of user-defined object IDs. Therefore user-defined object IDs will always be 
      greater than or equal to :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN`.

Object ID Example 
=================================================
The following pseudocode example shows a use case for the object ID module. 
Nodes of different data types are added to a linked list. The object ID is 
used to identify the type of each node.

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
    

API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/object__id_8h>
