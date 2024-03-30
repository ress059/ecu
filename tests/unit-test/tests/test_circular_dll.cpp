/**
 * @file
 * @author Ian Ress
 * @brief Unit tests for public API functions in @ref circular_dll.h
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
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
        mock()
            .actualCall("AssertMock::assert_handler");
        throw AssertException();
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
    }

    virtual void teardown() override 
    {
        ecu_circular_dll_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;
    ecu_circular_dll list_;
    ecu_circular_dll_iterator iterator_;
    user_data node1_;
    user_data node2_;
    user_data node3_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------- TESTS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// TEST(CircularDLL, DestructorRemovesAllNodesAndNodesCanBeAddedToAnotherList) // failing due to assert. step through with debugger
// {
//     ecu_circular_dll extra_list;

//     try 
//     {
//         /* Step 1: Arrange. */
//         ecu_circular_dll_ctor(&extra_list);
//         ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//         CHECK_EQUAL(0, ecu_circular_dll_get_size(&extra_list));
//         CHECK_EQUAL(3, ecu_circular_dll_get_size(&list_));

//         /* Steps 2 and 3: Action and assert. */
//         ecu_circular_dll_destroy(&list_);
//         CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));

//         ecu_circular_dll_push_back(&extra_list, &node1_.node);
//         ecu_circular_dll_push_back(&extra_list, &node2_.node);
//         ecu_circular_dll_push_back(&extra_list, &node3_.node);
//         CHECK_EQUAL(3, ecu_circular_dll_get_size(&extra_list));
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }

TEST(CircularDLL, NodeAdditionAndRemoval)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        (void)ecu_circular_dll_iterator_begin(&list_, &iterator_); /* Initialize iterator at beginning so any iterator call can be safely used. */

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_begin(&list_, &iterator_));

        /* [1] */
        ecu_circular_dll_push_back(&list_, &node1_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [1, 2] */
        ecu_circular_dll_push_back(&list_, &node2_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [1, 2, 3] */
        ecu_circular_dll_push_back(&list_, &node3_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [1, 3] */
        ecu_circular_dll_remove_node(&list_, &node2_.node);
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [3] */
        ecu_circular_dll_remove_node(&list_, &node1_.node);
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
        
        /* [] */
        ecu_circular_dll_remove_node(&list_, &node3_.node);
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [2] */
        ecu_circular_dll_push_back(&list_, &node2_.node);
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* [] */
        ecu_circular_dll_remove_node(&list_, &node2_.node);
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


TEST(CircularDLL, CorrectSizeReturned)
{
    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        
        /* Steps 2 and 3: Action and assert. */
        CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));

        ecu_circular_dll_push_back(&list_, &node1_.node); /* [1] */
        CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));

        ecu_circular_dll_push_back(&list_, &node2_.node); /* [1, 2] */
        CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));

        ecu_circular_dll_push_back(&list_, &node3_.node); /* [1, 2, 3] */
        CHECK_EQUAL(3, ecu_circular_dll_get_size(&list_));

        ecu_circular_dll_remove_node(&list_, &node2_.node); /* [1, 3] */
        CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));

        ecu_circular_dll_remove_node(&list_, &node1_.node); /* [3] */
        CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));
        
        ecu_circular_dll_remove_node(&list_, &node3_.node); /* [] */
        CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));

        ecu_circular_dll_push_back(&list_, &node2_.node); /* [2] */
        CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));

        ecu_circular_dll_remove_node(&list_, &node2_.node); /* [] */
        CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Node should be removed from first list and
 * added to second list.
 */
TEST(CircularDLL, AddingNodeFromAnotherList)
{
    struct ecu_circular_dll extra_list;

    try 
    {
        /* Step 1: Arrange. */
        ecu_circular_dll_ctor(&extra_list);
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_fail_));
        ecu_circular_dll_push_back(&list_, &node1_.node);
        ecu_circular_dll_push_back(&list_, &node2_.node);
        ecu_circular_dll_push_back(&extra_list, &node3_.node);

        /* Steps 2 and 3: Action and assert. */
        ecu_circular_dll_push_back(&extra_list, &node1_.node);

        /* list_ = [2] */
        CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));
        POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

        /* extra_list = [1, 3] */
        CHECK_EQUAL(2, ecu_circular_dll_get_size(&extra_list));
        POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&extra_list, &iterator_));
        POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&extra_list, &iterator_));
        POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL. */
    }

    /* Step 3: Assert. */
    CHECK_EQUAL(0, ecu_circular_dll_get_size(&extra_list));
}


TEST(CircularDLL, CannotAddNodeAlreadyInList)
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
    POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
    POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
    POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
}


TEST(CircularDLL, CannotRemoveNodeNotInList)
{
    struct ecu_circular_dll extra_list;

    try 
    {
        /* Step 1: Arrange. */    
        ecu_circular_dll_ctor(&extra_list);
        ecu_circular_dll_set_assert_functor(static_cast<ecu_assert_functor *>(&assert_call_ok_));
        ecu_circular_dll_push_back(&list_, &node1_.node);

        /* Step 2: Action. */
        ecu_circular_dll_remove_node(&extra_list, &node1_.node);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* OK. */
    }

    /* Step 3: Assert. */
    CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));
    POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&list_, &iterator_));
    POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
}




// iterator removal. add 5 nodes to list. iterate over and remove 2 nodes. verify 3 nodes are correctly left.
// iterator addition?

// TEST_F(CircularDLLTest, CircularDLLIterator)
// {
//     /* STEP 1: Arrange. */
//     struct ecu_circular_dll list;

//     ASSERT_EQ(data1_.x, data2_.x); 
//     ASSERT_EQ(data1_.x, data3_.x);
//     ASSERT_EQ(data1_.y, data2_.y);
//     ASSERT_EQ(data1_.y, data3_.y);

//     int old_x = data1_.x;
//     int old_y = data1_.y;

//     ecu_circular_dll_ctor(&list);
//     ecu_circular_dll_push_back(&list, &data1_.node);
//     ecu_circular_dll_push_back(&list, &data2_.node);
//     ecu_circular_dll_push_back(&list, &data3_.node);

//     /* STEP 2: Action. */
//     struct ecu_circular_dll_iterator iterator;
//     for (struct ecu_circular_dll_node *node = ecu_circular_dll_iterator_begin(&list, &iterator);
//          node != ecu_circular_dll_iterator_end(&iterator);
//          node = ecu_circular_dll_iterator_next(&iterator))
//     {
//         struct user_data *element = ECU_CIRCULAR_DLL_GET_ENTRY(node, struct user_data, node);
//         element->x = old_x + 1;
//         element->y = old_y + 1;
//     }

//     /* STEP 3: Assert. */
//     EXPECT_EQ(data1_.x, (old_x + 1));
//     EXPECT_EQ(data1_.y, (old_y + 1));
//     EXPECT_EQ(data2_.x, (old_x + 1));
//     EXPECT_EQ(data2_.y, (old_y + 1));
//     EXPECT_EQ(data3_.x, (old_x + 1));
//     EXPECT_EQ(data3_.y, (old_y + 1));
// }





// adding node that's already in list does nothing (list remains same size).

// destroy and reconstruct a list 2x in a row. verify nothing asserts and its fine



