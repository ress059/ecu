/**
 * @file
 * @brief Unit tests for public API functions in @ref circular_dll.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */


/* Files under test. */
#include <ecu/asserter.h>
#include <ecu/circular_dll.h>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- FILE-SCOPE TYPES -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

struct user_data
{
    int x;
    struct ecu_circular_dll_node node;
    int y;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------- STUBS AND MOCKS -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

class AssertException
{
};


/**
 * @brief Use when ECU library function calls within the unit test could 
 * cause assert to fire but you wish to ignore it. This throws an exception
 * so control immediately returns to the caller (the unit test). This 
 * prevents library code from running under an assert condition, leading to
 * undefined behavior.
 */
struct AssertCallOk : public ecu_assert_functor 
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)me;
        (void)file;
        (void)line;
        throw AssertException();
    }
};


/**
 * @brief Use when ECU library function calls within the unit test should
 * never fire an assert. Calls a mock which will cause the test to fail
 * due to an unexpected call. Also throws an exception so control immediately
 * returns to the caller (the unit test). This prevents library code from 
 * running under an assert condition, leading to undefined behavior.
 */
struct AssertCallFail : public ecu_assert_functor 
{
    static void assert_handler(struct ecu_assert_functor *me, const char *file, int line)
    {
        (void)me;
        (void)file;
        (void)line;
        mock().actualCall("AssertMock::assert_handler");
        throw AssertException();
    }
};


struct NodeDestroyMock : public ecu_circular_dll_node
{
    static void destroy(struct ecu_circular_dll_node *me)
    {
        mock().actualCall("NodeDestroyMock::destroy")
              .onObject(me);
    }
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(CircularDLL)
{
    virtual void setup() override 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;
        ecu_circular_dll_ctor(&list_);
        ecu_circular_dll_node_ctor(&node1_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node2_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node3_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
    }

    virtual void teardown() override 
    {
        ecu_circular_dll_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;
    struct ecu_circular_dll list_;
    struct ecu_circular_dll_iterator iterator_;
    struct user_data node1_;
    struct user_data node2_;
    struct user_data node3_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------- TESTS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Construct list and nodes, add nodes to list, and call list destructor.
 * Repeat process multiple times in a row and verify no assert fires since this
 * should be defined behavior.
 */
TEST(CircularDLL, ConstructorDestructorTest)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));

        /* Steps 2 and 3: Action and assert. */
        /* First sequence. */
        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&list_, &node3_.node);
        ecu_circular_dll_destroy(&list_);

        /* Second sequence. */
        ecu_circular_dll_ctor(&list_);
        ecu_circular_dll_node_ctor(&node1_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node2_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node3_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&list_, &node3_.node);
        ecu_circular_dll_destroy(&list_);

        /* Third sequence. */
        ecu_circular_dll_ctor(&list_);
        ecu_circular_dll_node_ctor(&node1_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node2_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node3_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&list_, &node3_.node);
        ecu_circular_dll_destroy(&list_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


TEST(CircularDLL, AllNodeDestructorCallbacksCalled)
{
    struct NodeDestroyMock dnode1_;
    struct NodeDestroyMock dnode2_;
    struct NodeDestroyMock dnode3_;

    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("NodeDestroyMock::destroy")
              .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode1_));

        mock().expectOneCall("NodeDestroyMock::destroy")
              .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode2_));

        mock().expectOneCall("NodeDestroyMock::destroy")
              .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode3_));   

        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));

        ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode1_), 
                                   &NodeDestroyMock::destroy,
                                   ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode2_), 
                                   &NodeDestroyMock::destroy,
                                   ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode3_), 
                                   &NodeDestroyMock::destroy,
                                   ECU_OBJECT_ID_UNUSED);

        ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode1_));
        ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode2_));
        ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode3_));

        /* Steps 2 and 3: Action and assert. */
        ecu_circular_dll_destroy(&list_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Some nodes have destroy callbacks and other nodes have NULL node 
 * destroy callbacks. Verify this is OK.
 */
TEST(CircularDLL, NodeDestroyNullCallbacks)
{
    struct NodeDestroyMock dnode2_;

    try 
    {
        /* Step 1: Arrange. */
        mock().expectOneCall("NodeDestroyMock::destroy")
              .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode2_));

        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));

        ecu_circular_dll_node_ctor(&node1_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode2_), 
                                   &NodeDestroyMock::destroy,
                                   ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node3_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);

        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode2_));
        ecu_circular_dll_push_back(&list_, &node3_.node);

        /* Steps 2 and 3: Action and assert. */
        ecu_circular_dll_destroy(&list_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify nodes are in correct order by directly comparing 
 * node pointer to elements returned by iterator.
 */
TEST(CircularDLL, NodeAdditionAndRemoval)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        (void)ecu_circular_dll_iterator_begin(&iterator_, &list_); /* Initialize iterator at beginning so any iterator call can be safely used. */

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_begin(&iterator_, &list_));

        /* [1] */
        ecu_circular_dll_push_back(&list_, &node1_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [1, 2] */
        ecu_circular_dll_push_back(&list_, &node2_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [1, 2, 3] */
        ecu_circular_dll_push_back(&list_, &node3_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [1, 3] */
        ecu_circular_dll_remove_node(&node2_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [3] */
        ecu_circular_dll_remove_node(&node1_.node);
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
        
        /* [] */
        ecu_circular_dll_remove_node(&node3_.node);
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_begin(&iterator_, &list_));

        /* [2] */
        ecu_circular_dll_push_back(&list_, &node2_.node);
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [] */
        ecu_circular_dll_remove_node(&node2_.node);
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_begin(&iterator_, &list_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL. */
    }
}


TEST(CircularDLL, CannotAddNodeAlreadyInSameList)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_ok_));
        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);

        /* Step 2: Action. */
        ecu_circular_dll_push_back(&list_, &node2_.node);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* OK. */
    }

    /* Step 3: Assert. */
    CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));
    POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
    POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
    POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
}


TEST(CircularDLL, CannotAddNodeFromAnotherList)
{
    struct ecu_circular_dll extra_list;

    try
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_ctor(&extra_list);
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_ok_));
        
        /* list_ = [1, 2]. extra_list = [3] */
        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&extra_list, &node3_.node);

        /* Step 2: Action. */
        ecu_circular_dll_push_back(&list_, &node3_.node);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* OK. */
    }

    /* Step 3: Assert. */
    /* list_ = [1, 2] */
    CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));
    POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
    POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
    POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

    /* extra_list = [3] */
    CHECK_EQUAL(1, ecu_circular_dll_get_size(&extra_list));
    POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&iterator_, &extra_list));
    POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
}


/* Purposefully not done since no way of directly verifying by inspecting the list.
Can only verify by checking if an assert fired but do not want to do that since that
is more implementation-defined. */
// TEST(CircularDLL, CannotRemoveNodeNotInList)
// {
// }


TEST(CircularDLL, CorrectSizeReturned)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        
        /* Steps 2 and 3: Action and assert. */
        CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));

        /* [1] */
        ecu_circular_dll_push_back(&list_, &node1_.node);
        CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));

        /* [1, 2] */
        ecu_circular_dll_push_back(&list_, &node2_.node);
        CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));

        /* [1, 2, 3] */
        ecu_circular_dll_push_back(&list_, &node3_.node);
        CHECK_EQUAL(3, ecu_circular_dll_get_size(&list_));

        /* [1, 3] */
        ecu_circular_dll_remove_node(&node2_.node);
        CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));

        /* [3] */
        ecu_circular_dll_remove_node(&node1_.node);
        CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));
        
        /* [] */
        ecu_circular_dll_remove_node(&node3_.node);
        CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));

        /* [2] */
        ecu_circular_dll_push_back(&list_, &node2_.node);
        CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));

        /* [] */
        ecu_circular_dll_remove_node(&node2_.node);
        CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL. */
    }
}


/**
 * @brief Test function returning true means list is empty.
 */
TEST(CircularDLL, IsEmpty)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE(ecu_circular_dll_is_empty(&list_));

        /* [1] */
        ecu_circular_dll_push_back(&list_, &node1_.node);
        CHECK_FALSE(ecu_circular_dll_is_empty(&list_));

        /* [1, 2] */
        ecu_circular_dll_push_back(&list_, &node2_.node);
        CHECK_FALSE(ecu_circular_dll_is_empty(&list_));

        /* [2] */
        ecu_circular_dll_remove_node(&node1_.node);
        CHECK_FALSE(ecu_circular_dll_is_empty(&list_));

        /* [] */
        ecu_circular_dll_remove_node(&node2_.node);
        CHECK_TRUE(ecu_circular_dll_is_empty(&list_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL. */
    }
}


/**
 * @brief Edit data of nodes through iterator. Verify all data was
 * changed, showing we can iterate over the entire list.
 */
TEST(CircularDLL, IterateOverListAndEditAllNodes)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        node1_.x = 5;
        node1_.y = 5;
        node2_.x = 5;
        node2_.y = 5;
        node3_.x = 5;
        node3_.y = 5;
        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&list_, &node3_.node);

        /* Step 2: Action. */
        for (struct ecu_circular_dll_node *i = ecu_circular_dll_iterator_begin(&iterator_, &list_);
             i != ecu_circular_dll_iterator_end(&iterator_);
             i = ecu_circular_dll_iterator_next(&iterator_))
        {
            struct user_data *data = ECU_CIRCULAR_DLL_GET_ENTRY(i, struct user_data, node);
            data->x = 10;
            data->y = 10;
        }

        /* Step 3: Assert. */
        CHECK_EQUAL(10, node1_.x);
        CHECK_EQUAL(10, node1_.y);
        CHECK_EQUAL(10, node2_.x);
        CHECK_EQUAL(10, node2_.y);
        CHECK_EQUAL(10, node3_.x);
        CHECK_EQUAL(10, node3_.y);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL. */
    }
}


/**
 * @brief Only edit data of nodes we aren't removing. Verify correct
 * nodes removed and their data was not edited, showing we can safely
 * iterate over the list while removing nodes.
 */
TEST(CircularDLL, IterateOverListAndRemoveSomeNodes)
{
    struct user_data node4;
    struct user_data node5;

    try
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        node1_.x = 5;
        node1_.y = 5;
        node2_.x = 5;
        node2_.y = 5;
        node3_.x = 5;
        node3_.y = 5;
        node4.x = 5;
        node4.y = 5;
        node5.x = 5;
        node5.y = 5;

        ecu_circular_dll_node_ctor(&node4.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node5.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);

        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&list_, &node3_.node);
        ecu_circular_dll_push_back(&list_, &node4.node);
        ecu_circular_dll_push_back(&list_, &node5.node);

        /* Step 2: Action. */
        for (struct ecu_circular_dll_node *i = ecu_circular_dll_iterator_begin(&iterator_, &list_);
             i != ecu_circular_dll_iterator_end(&iterator_);
             i = ecu_circular_dll_iterator_next(&iterator_))
        {
            /* Selectively remove some nodes. 1 and 5 chosen since this is the start and end of list. */
            if (i == &node1_.node || i == &node2_.node || i == &node5.node)
            {
                ecu_circular_dll_remove_node(i);
            }
            else
            {
                struct user_data *data = ECU_CIRCULAR_DLL_GET_ENTRY(i, struct user_data, node);
                data->x = 10;
                data->y = 10;
            }
        }

        /* Step 3: Assert. */
        /* [3, 4] */
        CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(&node4.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        CHECK_EQUAL(5, node1_.x);
        CHECK_EQUAL(5, node1_.y);
        CHECK_EQUAL(5, node2_.x);
        CHECK_EQUAL(5, node2_.y);
        CHECK_EQUAL(10, node3_.x);
        CHECK_EQUAL(10, node3_.y);
        CHECK_EQUAL(10, node4.x);
        CHECK_EQUAL(10, node4.y);
        CHECK_EQUAL(5, node5.x);
        CHECK_EQUAL(5, node5.y);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL. */
    }
}


/**
 * @brief Iterator also goes through nodes that were added to
 * end of list via @ref ecu_circular_dll_push_back().
 */
TEST(CircularDLL, AddNodesInIteratorPushBack)
{
    struct user_data node4;
    struct user_data node5;

    try
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        ecu_circular_dll_node_ctor(&node4.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
        ecu_circular_dll_node_ctor(&node5.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);

        node1_.x = 5;
        node1_.y = 5;
        node2_.x = 5;
        node2_.y = 5;
        node3_.x = 5;
        node3_.y = 5;
        node4.x = 5;
        node4.y = 5;
        node5.x = 5;
        node5.y = 5;

        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&list_, &node3_.node);

        /* Step 2: Action. */
        for (struct ecu_circular_dll_node *i = ecu_circular_dll_iterator_begin(&iterator_, &list_);
             i != ecu_circular_dll_iterator_end(&iterator_);
             i = ecu_circular_dll_iterator_next(&iterator_))
        {
            /* [1, 2, 3, 4, 5] */
            if (i == &node2_.node)
            {
                ecu_circular_dll_push_back(&list_, &node4.node);
                ecu_circular_dll_push_back(&list_, &node5.node);
            }
            struct user_data *data = ECU_CIRCULAR_DLL_GET_ENTRY(i, struct user_data, node);
            data->x = 10;
            data->y = 10;
        }

        /* Step 3: Assert. */
        CHECK_EQUAL(5, ecu_circular_dll_get_size(&list_));
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(&node4.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(&node5.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        CHECK_EQUAL(10, node1_.x);
        CHECK_EQUAL(10, node1_.y);
        CHECK_EQUAL(10, node2_.x);
        CHECK_EQUAL(10, node2_.y);
        CHECK_EQUAL(10, node3_.x);
        CHECK_EQUAL(10, node3_.y);
        CHECK_EQUAL(10, node4.x);
        CHECK_EQUAL(10, node4.y);
        CHECK_EQUAL(10, node5.x);
        CHECK_EQUAL(10, node5.y);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL. */
    }
}
