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
with user-defined IDs. Some additional notes:

    - Object IDs reserved by ECU will always be negative.

    + Reserved object IDs starting from the :ecudoxygen:`ECU_VALID_OBJECT_ID_BEGIN` 
      enumeration can be assigned to an object. However, :ecudoxygen:`ECU_VALID_OBJECT_ID_BEGIN`
      is for internal use only and should never be used directly. **Currently**  
      :ecudoxygen:`ECU_OBJECT_ID_RESERVED` **is the only reserved ID available for use.**

    - :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN` enumeration will always be 0, and marks the start 
      of user-defined object IDs. Therefore user-defined object IDs will always be 
      greater than or equal to :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN`.

Object ID Example 
=================================================
The following pseudocode example shows a use case for the object ID module. 
Nodes of different data types are added to a linked list. The object ID is 
used to identify the type of each node.

.. code-block:: c

    #include "ecu/dlist.h"
    #include "ecu/object_id.h"

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
        struct ecu_dlist_node node;
    };

    struct type2 
    {
        struct ecu_dlist_node node;
        int b;
    };

    struct type3 
    {
        int c;
        struct ecu_dlist_node node;
    };

    struct ecu_dlist_iterator iterator;
    struct ecu_dlist list;
    struct type1 type1_node;
    struct type2 type2_node;
    struct type3 type3_node;

    /* Construct list and nodes. Assign object IDs to each node to identify
    their data types. */
    ecu_dlist_ctor(&list);
    ecu_dlist_node_ctor(&type1_node.node, ECU_DLIST_NODE_DESTROY_UNUSED, TYPE1);
    ecu_dlist_node_ctor(&type2_node.node, ECU_DLIST_NODE_DESTROY_UNUSED, TYPE2);
    ecu_dlist_node_ctor(&type3_node.node, ECU_DLIST_NODE_DESTROY_UNUSED, TYPE3);

    /* Add nodes to list. */
    ecu_dlist_push_back(&list, &type2_node.node);
    ecu_dlist_push_back(&list, &type3_node.node);
    ecu_dlist_push_back(&list, &type1_node.node);

    /* Iterate over list. Use object ID to identify the data type stored in each node. */
    for (struct ecu_dlist_node *i = ecu_dlist_iterator_begin(&iterator, &list);
         i != ecu_dlist_iterator_end(&list);
         i = ecu_dlist_iterator_next(&list))
    {
        switch (ecu_dlist_node_get_id(i))
        {
            case TYPE1:
            {
                struct type1 *me = ECU_DLIST_GET_ENTRY(i, struct type1, node);
                me->a = 5;
                break;
            }

            case TYPE2:
            {
                struct type2 *me = ECU_DLIST_GET_ENTRY(i, struct type2, node);
                me->b = 5;
                break;
            }

            case TYPE3:
            {
                struct type1 *me = ECU_DLIST_GET_ENTRY(i, struct type3, node);
                me->c = 5;
                break;
            }
        }
    }
    

API
=================================================
.. toctree:: 
    :maxdepth: 1

    API </doxygen/html/object__id_8h>
