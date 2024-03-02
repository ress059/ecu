#include <ecu/circular_dll.h>
#include <gtest/gtest.h>


struct user_data
{
    int x;
    struct circular_dll_node node;
    int y;
};


class CircularDLLTest : public testing::Test 
{
protected:
    virtual void SetUp() override 
    {
        data1_.x = 5;
        data1_.y = 5;
        data1_.node = {0};

        data2_.x = 5;
        data2_.y = 5;
        data2_.node = {0};

        data3_.x = 5;
        data3_.y = 5;
    }

    struct user_data data1_;
    struct user_data data2_;
    struct user_data data3_;
};


TEST_F(CircularDLLTest, CircularDLLSize)
{
    /* STEP 1: Arrange. */
    struct circular_dll list;
    circular_dll_ctor(&list);
    
    /* STEPS 2 and 3: Action and assert. */
    EXPECT_EQ(circular_dll_get_size(&list), 0);

    circular_dll_push_back(&list, &data1_.node); /* [1] */
    EXPECT_EQ(circular_dll_get_size(&list), 1);

    circular_dll_push_back(&list, &data2_.node); /* [1, 2] */
    EXPECT_EQ(circular_dll_get_size(&list), 2);

    circular_dll_push_back(&list, &data3_.node); /* [1, 2, 3] */
    EXPECT_EQ(circular_dll_get_size(&list), 3);

    circular_dll_remove_node(&list, &data2_.node); /* [1, 3] */
    EXPECT_EQ(circular_dll_get_size(&list), 2);

    circular_dll_remove_node(&list, &data1_.node); /* [3] */
    EXPECT_EQ(circular_dll_get_size(&list), 1);
    
    circular_dll_remove_node(&list, &data3_.node); /* [] */
    EXPECT_EQ(circular_dll_get_size(&list), 0);

    circular_dll_push_back(&list, &data2_.node); /* [2] */
    EXPECT_EQ(circular_dll_get_size(&list), 1);

    circular_dll_remove_node(&list, &data2_.node); /* [] */
    EXPECT_EQ(circular_dll_get_size(&list), 0);
}

// removing node from empty list asserts

// adding node that's already in list asserts.

// destroy and reconstruct a list 2x in a row. verify nothing asserts and its fine



TEST_F(CircularDLLTest, CircularDLLIterator)
{
    /* STEP 1: Arrange. */
    struct circular_dll list;

    ASSERT_EQ(data1_.x, data2_.x); 
    ASSERT_EQ(data1_.x, data3_.x);
    ASSERT_EQ(data1_.y, data2_.y);
    ASSERT_EQ(data1_.y, data3_.y);

    int old_x = data1_.x;
    int old_y = data1_.y;

    circular_dll_ctor(&list);
    circular_dll_push_back(&list, &data1_.node);
    circular_dll_push_back(&list, &data2_.node);
    circular_dll_push_back(&list, &data3_.node);

    /* STEP 2: Action. */
    struct circular_dll_iterator iterator;
    for (struct circular_dll_node *node = circular_dll_iterator_begin(&list, &iterator);
         node != circular_dll_iterator_end(&iterator);
         node = circular_dll_iterator_next(&iterator))
    {
        struct user_data *element = CIRCULAR_DLL_GET_ENTRY(node, struct user_data, node);
        element->x = old_x + 1;
        element->y = old_y + 1;
    }

    /* STEP 3: Assert. */
    EXPECT_EQ(data1_.x, (old_x + 1));
    EXPECT_EQ(data1_.y, (old_y + 1));
    EXPECT_EQ(data2_.x, (old_x + 1));
    EXPECT_EQ(data2_.y, (old_y + 1));
    EXPECT_EQ(data3_.x, (old_x + 1));
    EXPECT_EQ(data3_.y, (old_y + 1));
}