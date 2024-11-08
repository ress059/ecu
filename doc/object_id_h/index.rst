.. _object_id_h:

object_id.h
###############################################
.. raw:: html

   <hr>


Overview
=================================================
Lets users identify different types stored in the same data structure.
For example, unique IDs can be assigned to nodes in a tree. The ID values
specify the data type stored in each tree node.


Defining Object IDs
=================================================
Users can define their own object IDs, starting at value :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN`. 
This allows ECU library and the end user to uniquely define their own object 
IDs without conflicts (library and user IDs won't be the same values). 
The recommended implementation is as follows:

.. code-block:: c

    #include "ecu/object_id.h"

    /* User-defined object IDs. */
    enum user_object_ids
    {
        TYPE1 = ECU_USER_OBJECT_ID_BEGIN,
        TYPE2,
        TYPE3
    };

Some other notes:

    - Object IDs reserved for ECU library will always be negative.

    - Reserved object IDs the end user can use will start at value :ecudoxygen:`ECU_VALID_OBJECT_ID_BEGIN`.
      Note that this should never be used by the application. It is used internally by ECU to know when an object 
      ID is valid.
      
    - :ecudoxygen:`ECU_USER_OBJECT_ID_BEGIN` will always be 0 and marks the start 
      of user-defined object IDs. User-defined object IDs will always be greater than
      or equal to this value.


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

    /* Construct list and nodes. Assign object IDs to each node to identify its data type. */
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
        switch (ecu_dlist_node_get_id())
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
