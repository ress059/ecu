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
#include <ecu/circular_dll.h>

/* Mocks. */
#include <mocks/mock_asserter.hpp>

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
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(CircularDLL)
{
};


#error "TODO: Just hard-code assert within test. Stop trying to make external function to be perfectly modular. just get it done"
/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- TESTS - CircularDLLBaseGroup Group ------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Add and remove nodes from the list. Verify returned
 * size is correct.
 */
TEST(CircularDLL, TestGetSize)
{
    /* STEP 1: Arrange. */
    struct ecu_circular_dll list;
    struct user_data data1;
    struct user_data data2;
    struct user_data data3;
    ecu_circular_dll_ctor(&list);
    
    /* STEPS 2 and 3: Action and assert. */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 0);

    ecu_circular_dll_push_back(&list, &data1.node); /* [1] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 1);

    ecu_circular_dll_push_back(&list, &data2.node); /* [1, 2] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 2);

    ecu_circular_dll_push_back(&list, &data3.node); /* [1, 2, 3] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 3);

    ecu_circular_dll_remove_node(&list, &data2.node); /* [1, 3] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 2);

    ecu_circular_dll_remove_node(&list, &data1.node); /* [3] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 1);
    
    ecu_circular_dll_remove_node(&list, &data3.node); /* [] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 0);

    ecu_circular_dll_push_back(&list, &data2.node); /* [2] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 1);

    ecu_circular_dll_remove_node(&list, &data2.node); /* [] */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list), 0);
}


// Normal node addition and removal.




// removing nodes
// have 2 lists - list A and list B. ecu_circular_dll_remove_node(&listA, nodeinListB); Verify this does nothing.
TEST(CircularDLL, TestInvalidNodeRemoval)
{
    /* STEP 1: Arrange. */
    struct ecu_circular_dll list1;
    struct ecu_circular_dll list2;
    struct ecu_circular_dll_node node_in_list1;
    struct ecu_circular_dll_node node_in_list2;

    ecu_circular_dll_ctor(&list1);
    ecu_circular_dll_ctor(&list2);
    ecu_circular_dll_push_back(&list1, &node_in_list1);
    ecu_circular_dll_push_back(&list2, &node_in_list2);

    CHECK_EQUAL(ecu_circular_dll_get_size(&list1), 1);
    CHECK_EQUAL(ecu_circular_dll_get_size(&list2), 1);

    mock().expectCall()

    /* STEP 2: Action. */
    try 
    {
        ecu_circular_dll_remove_node(&list1, &node_in_list2);
    }
    catch (const ECUMockAsserterException& e)
    {
        (void)e;
    }

    /* STEP 3: Assert. */
    CHECK_EQUAL(ecu_circular_dll_get_size(&list1), 1);
    CHECK_EQUAL(ecu_circular_dll_get_size(&list2), 1);
}





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



