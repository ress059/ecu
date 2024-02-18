#include <ecu/circular_sll.h>
#include <gtest/gtest.h>


TEST(CircularSLLTest, CircularSLLTestSize)
{
    /* STEP 1: Arrange. */
    struct circular_sll list;
    struct circular_sll_node node1;
    struct circular_sll_node node2;
    struct circular_sll_node node3;
    
    /* STEPS 2 and 3: Action and assert. */
    // #error "TODO: Stopped here. Think circular_sll_get_size() never returns causing test to permanetly hang."
    circular_sll_ctor(&list);
    EXPECT_EQ(circular_sll_get_size(&list), 0);

    circular_sll_push_back(&list, &node1); /* [1] */
    EXPECT_EQ(circular_sll_get_size(&list), 1);

    circular_sll_push_back(&list, &node2); /* [1, 2] */
    EXPECT_EQ(circular_sll_get_size(&list), 2);

    circular_sll_add_node(&node1, &node3); /* [1, 3, 2] */
    EXPECT_EQ(circular_sll_get_size(&list), 3);

    circular_sll_remove_node(&node2); /* [1, 3] */
    EXPECT_EQ(circular_sll_get_size(&list), 2);

    circular_sll_remove_node(&node1); /* [3] */
    EXPECT_EQ(circular_sll_get_size(&list), 1);

    circular_sll_push_back(&list, &node3); /* [1, 3] */
    EXPECT_EQ(circular_sll_get_size(&list), 2);

    circular_sll_remove_node(&node3); /* [1] */
    EXPECT_EQ(circular_sll_get_size(&list), 1);

    circular_sll_remove_node(&node1);
    EXPECT_EQ(circular_sll_get_size(&list), 0);
}