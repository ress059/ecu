/**
 * @file
 * @brief Unit tests for public API functions in @ref tree.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-05-08
 * @copyright Copyright (c) 2024
 */



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- INCLUDES -----------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/* Files under test. */
#include <ecu/tree.h>

/* Mocks */
#include <mocks/mock_asserter.hpp>

/* CppUTest. */
#include <CppUTestExt/MockSupport.h>
#include <CppUTest/TestHarness.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- FILE-SCOPE TYPES -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Used to test @ref ECU_TREE_NODE_GET_ENTRY() macro. Different
 * types are used to try to make the memory layout of the struct
 * non-uniform.
 */
struct user_tree_node_type
{
    user_tree_node_type() : a_(0), b_(0), c_(0), d_(0)
    {
        ecu_tree_node_ctor(&node_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
    }

    uint8_t a_;
    uint64_t b_;
    struct ecu_tree_node node_;
    uint16_t c_;
    uint32_t d_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- STUB AND MOCK DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

class IteratorTreeNodeMock
{
public:
    /**
     * @brief Used to verify tree node returned by iterator is correct.
     * Mock is used instead of direct comparison to make creation of
     * tests easier.
     */
    static void verify_node(struct ecu_tree_node *me)
    {
        mock().actualCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", me);
    }

private:
    IteratorTreeNodeMock() = default;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- TEST GROUPS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(ChildIterator)
{
    virtual void setup() override 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;

        ecu_tree_node_ctor(&parent_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child2_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child3_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child4_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child5_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child6_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child7_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child8_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child9_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
    }

    virtual void teardown() override
    {
        ecu_tree_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;

    struct ecu_tree_child_iterator iterator_;
    struct ecu_tree_node parent_;
    struct ecu_tree_node child1_;
    struct ecu_tree_node child2_;
    struct ecu_tree_node child3_;
    struct ecu_tree_node child4_;
    struct ecu_tree_node child5_;
    struct ecu_tree_node child6_;
    struct ecu_tree_node child7_;
    struct ecu_tree_node child8_;
    struct ecu_tree_node child9_;
};


TEST_GROUP(PostOrderIterator)
{
    virtual void setup() override 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;

        ecu_tree_node_ctor(&root1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node8_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node9_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node10_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node11_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node12_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
    }

    virtual void teardown() override
    {
        ecu_tree_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;

    struct ecu_tree_postorder_iterator iterator_;
    struct ecu_tree_node root1_;
    struct ecu_tree_node node1_;
    struct ecu_tree_node node2_;
    struct ecu_tree_node node3_;
    struct ecu_tree_node node4_;
    struct ecu_tree_node node5_;
    struct ecu_tree_node node6_;
    struct ecu_tree_node node7_;
    struct ecu_tree_node node8_;
    struct ecu_tree_node node9_;
    struct ecu_tree_node node10_;
    struct ecu_tree_node node11_;
    struct ecu_tree_node node12_;
};


TEST_GROUP(AddRemoveNode)
{
    virtual void setup() override 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;

        ecu_tree_node_ctor(&root1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&root2_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node2_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node3_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node4_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node5_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node6_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node7_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
    }

    virtual void teardown() override
    {
        ecu_tree_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;

    struct ecu_tree_postorder_iterator postorder_iterator_;
    struct ecu_tree_child_iterator child_iterator_;
    struct ecu_tree_node root1_;
    struct ecu_tree_node root2_;
    struct ecu_tree_node node1_;
    struct ecu_tree_node node2_;
    struct ecu_tree_node node3_;
    struct ecu_tree_node node4_;
    struct ecu_tree_node node5_;
    struct ecu_tree_node node6_;
    struct ecu_tree_node node7_;
};


#warning "TODO"
// TEST_GROUP(Destructor)
// {
// 	ecu_tree_node_ctor(&node, &destroy_callback_that_calls_mock, ECU_OBJECT_ID_UNUSED);
// };


TEST_GROUP(GetLevelAndLCA)
{
    virtual void setup() override 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;

        ecu_tree_node_ctor(&root1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&root2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node8_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node9_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node10_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node11_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node12_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
    }

    virtual void teardown() override
    {
        ecu_tree_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;

    struct ecu_tree_node root1_;
    struct ecu_tree_node root2_;
    struct ecu_tree_node node1_;
    struct ecu_tree_node node2_;
    struct ecu_tree_node node3_;
    struct ecu_tree_node node4_;
    struct ecu_tree_node node5_;
    struct ecu_tree_node node6_;
    struct ecu_tree_node node7_;
    struct ecu_tree_node node8_;
    struct ecu_tree_node node9_;
    struct ecu_tree_node node10_;
    struct ecu_tree_node node11_;
    struct ecu_tree_node node12_;
};


TEST_GROUP(TreeMisc)
{
    virtual void setup() override 
    {
        assert_call_ok_.handler = &AssertCallOk::assert_handler;
        assert_call_fail_.handler = &AssertCallFail::assert_handler;
    }

    virtual void teardown() override
    {
        ecu_tree_set_assert_functor(ECU_DEFAULT_FUNCTOR);
        mock().checkExpectations();
        mock().clear();
    }

    AssertCallOk assert_call_ok_;
    AssertCallFail assert_call_fail_;
	user_tree_node_type user_tree_node_;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- TESTS - CHILD ITERATOR -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Also verify wraparound. I.e. calling iterator_next() 
 * after iterator reaches end.
 */
TEST(ChildIterator, ParentWithMultipleChildren)
{
    /* Test tree:

            parent
            |
            child1--child2--child3
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child3_);

        ecu_tree_add_child_push_back(&parent_, &child1_);
        ecu_tree_add_child_push_back(&parent_, &child2_);
        ecu_tree_add_child_push_back(&parent_, &child3_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &parent_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
        
        /* Steps 2 and 3: Action and assert. Test wraparound. */
        POINTERS_EQUAL(&child1_, ecu_tree_child_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


TEST(ChildIterator, ParentWithNoChildren)
{
    /* Test tree:
                
                parent
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        /* Steps 2 and 3: Action and assert. */
        ecu_tree_add_child_push_back(&parent_, &child1_);
        ecu_tree_remove_node(&child1_);

        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &parent_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            /* No mocks should be called since no children present. */
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Also verify wraparound. I.e. calling iterator_next() 
 * after iterator reaches end.
 */
TEST(ChildIterator, ParentWithOneChild)
{
    /* Test tree:

                parent
                |
                child1
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child1_);

        ecu_tree_add_child_push_back(&parent_, &child1_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &parent_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Test wraparound. */
        POINTERS_EQUAL(&child1_, ecu_tree_child_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Parent's child now also has children. Verify 
 * only parent's direct children (not grandchildren) are 
 * iterated through. Also verify wraparound.
 */
TEST(ChildIterator, Grandparent)
{
    /* Test tree:

                    parent
                    |
                    child1--child2--child3
                            |
                            child4--child5--child6
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child3_);

        ecu_tree_add_child_push_back(&parent_, &child1_);
        ecu_tree_add_child_push_back(&parent_, &child2_);
        ecu_tree_add_child_push_back(&parent_, &child3_);
        ecu_tree_add_child_push_back(&child2_, &child4_);
        ecu_tree_add_child_push_back(&child2_, &child5_);
        ecu_tree_add_child_push_back(&child2_, &child6_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &parent_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Test wraparound. */
        POINTERS_EQUAL(&child1_, ecu_tree_child_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Parent has siblings and each of them have children. 
 * Use child iterator on each sibling. Verify only the target 
 * node's children are iterated through. Also verify wraparounds.
 */
TEST(ChildIterator, ParentWithSiblingsThatAllHaveChildren)
{
    /* Test tree:

                    parent
                    |
                    child1--------------child2--------------child3
                    |                   |                   |
                    child4---child5     child6---child7     child8---child9
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child8_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child9_);

        ecu_tree_add_child_push_back(&parent_, &child1_);
        ecu_tree_add_child_push_back(&parent_, &child2_);
        ecu_tree_add_child_push_back(&parent_, &child3_);
        ecu_tree_add_child_push_back(&child1_, &child4_);
        ecu_tree_add_child_push_back(&child1_, &child5_);    
        ecu_tree_add_child_push_back(&child2_, &child6_);
        ecu_tree_add_child_push_back(&child2_, &child7_);
        ecu_tree_add_child_push_back(&child3_, &child8_);
        ecu_tree_add_child_push_back(&child3_, &child9_);

        /* Steps 2 and 3: Action and assert. Verify iterator on 
        child1 only returns child 4 and child 5. Also verify wraparound. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &child1_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
        POINTERS_EQUAL(&child4_, ecu_tree_child_iterator_next(&iterator_));

        /* Steps 2 and 3: Action and assert. Verify iterator on 
        child2 only returns child 6 and child 7. Also verify wraparound. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &child2_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
        POINTERS_EQUAL(&child6_, ecu_tree_child_iterator_next(&iterator_));

        /* Steps 2 and 3: Action and assert. Verify iterator on 
        child3 only returns child 8 and child 9. Also verify wraparound. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &child3_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
        POINTERS_EQUAL(&child8_, ecu_tree_child_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify removal of children is a safe operation and
 * you can still successfully iterate over all children.
 */
TEST(ChildIterator, CanRemoveChildrenInMiddleOfIteration)
{
    /* Test tree:

                    parent
                    |
                    child1-----child2-----child3-----child4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &child4_);

        ecu_tree_add_child_push_back(&parent_, &child1_);
        ecu_tree_add_child_push_back(&parent_, &child2_);
        ecu_tree_add_child_push_back(&parent_, &child3_);
        ecu_tree_add_child_push_back(&parent_, &child4_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &parent_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            ecu_tree_remove_node(i);
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


#warning "TODO:"
// TEST(ChildIterator, CanAddChildrenInMiddleOfIteration)
// {
// }



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ TESTS - POSTORDER ITERATOR -----------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Baseline test. Verify all nodes in tree are 
 * iterated through. Also verify wraparound.
 */
TEST(PostOrderIterator, MultiLevelTree)
{
    /* Test tree: note that node number is the order in which it should be iterated.

                    root1
                    |
                    node2-------node10------------------------------------------node11
                    |           |                                               |
                    node1       node3-------node7---------------node9           node12
                                            |                   |
                                            node5-----node6     node8
                                            |
                                            node4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node8_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node9_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node10_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node11_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node12_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        ecu_tree_add_child_push_back(&root1_, &node2_);
        ecu_tree_add_child_push_back(&root1_, &node10_);
        ecu_tree_add_child_push_back(&root1_, &node12_);
        ecu_tree_add_child_push_back(&node2_, &node1_);
        ecu_tree_add_child_push_back(&node10_, &node3_);
        ecu_tree_add_child_push_back(&node10_, &node7_);
        ecu_tree_add_child_push_back(&node10_, &node9_);
        ecu_tree_add_child_push_back(&node12_, &node11_);
        ecu_tree_add_child_push_back(&node7_, &node5_);
        ecu_tree_add_child_push_back(&node7_, &node6_);
        ecu_tree_add_child_push_back(&node9_, &node8_);
        ecu_tree_add_child_push_back(&node5_, &node4_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&iterator_);
             i = ecu_tree_postorder_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Verify wraparound for some nodes after. */
        POINTERS_EQUAL(&node1_, ecu_tree_postorder_iterator_next(&iterator_));
        POINTERS_EQUAL(&node2_, ecu_tree_postorder_iterator_next(&iterator_));
        POINTERS_EQUAL(&node3_, ecu_tree_postorder_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify iterator works on subtree. Also verify wraparound.
 */
TEST(PostOrderIterator, MultiLevelSubTreeWithSiblings)
{
    /* Test tree: note that node number is the order in which it should be iterated.
    but we will only be doing node9's subtree for this test.

                    root1
                    |
                    node9-------------------------------node11-----------node12
                    |                                   |
                    node5-------node6------node8        node10
                    |
                    node1-------node4
                                |
                                node2------node3
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node8_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node9_);

        ecu_tree_add_child_push_back(&root1_, &node9_);
        ecu_tree_add_child_push_back(&root1_, &node11_);
        ecu_tree_add_child_push_back(&root1_, &node12_);
        ecu_tree_add_child_push_back(&node9_, &node5_);
        ecu_tree_add_child_push_back(&node9_, &node6_);
        ecu_tree_add_child_push_back(&node9_, &node8_);
        ecu_tree_add_child_push_back(&node11_, &node10_);
        ecu_tree_add_child_push_back(&node5_, &node1_);
        ecu_tree_add_child_push_back(&node5_, &node4_);
        ecu_tree_add_child_push_back(&node8_, &node7_);
        ecu_tree_add_child_push_back(&node4_, &node2_);
        ecu_tree_add_child_push_back(&node4_, &node3_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &node9_);
             i != ecu_tree_postorder_iterator_end(&iterator_);
             i = ecu_tree_postorder_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Verify wraparound for some nodes after. */
        POINTERS_EQUAL(&node1_, ecu_tree_postorder_iterator_next(&iterator_));
        POINTERS_EQUAL(&node2_, ecu_tree_postorder_iterator_next(&iterator_));
        POINTERS_EQUAL(&node3_, ecu_tree_postorder_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Also verify wraparound.
 */
TEST(PostOrderIterator, MultiLevelDegenerateTree)
{
    /* Test tree: note that node number is the order in which it should be iterated.

                    root1
                    |
                    node3
                    |
                    node2
                    |
                    node1
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&node3_, &node2_);
        ecu_tree_add_child_push_back(&node2_, &node1_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&iterator_);
             i = ecu_tree_postorder_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Verify wraparound for some nodes after. */
        POINTERS_EQUAL(&node1_, ecu_tree_postorder_iterator_next(&iterator_));
        POINTERS_EQUAL(&node2_, ecu_tree_postorder_iterator_next(&iterator_));
        POINTERS_EQUAL(&node3_, ecu_tree_postorder_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Single node for this test is tree root. Iterator
 * should only return this root. Also verify wraparound.
 */
TEST(PostOrderIterator, SingleNode)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&iterator_);
             i = ecu_tree_postorder_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Verify wraparound. */
        POINTERS_EQUAL(&root1_, ecu_tree_postorder_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief Verify removal of nodes is a safe operation and
 * you can still successfully iterate over all children.
 */
TEST(PostOrderIterator, CanRemoveNodesInMiddleOfIteration)
{
    /* Test tree: note that node number is the order in which it should be iterated

                    root1
                    |
                    node2-------node3-------node5----node6------node12
                    |                       |                   |
                    node1                   node4               node9------node10------node11
                                                                |
                                                                node7------node8
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node8_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node9_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node10_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node11_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node12_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        ecu_tree_add_child_push_back(&root1_, &node2_);
        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&root1_, &node5_);
        ecu_tree_add_child_push_back(&root1_, &node6_);
        ecu_tree_add_child_push_back(&root1_, &node12_);
        ecu_tree_add_child_push_back(&node2_, &node1_);
        ecu_tree_add_child_push_back(&node5_, &node4_);
        ecu_tree_add_child_push_back(&node12_, &node9_);
        ecu_tree_add_child_push_back(&node12_, &node10_);
        ecu_tree_add_child_push_back(&node12_, &node11_);
        ecu_tree_add_child_push_back(&node9_, &node7_);
        ecu_tree_add_child_push_back(&node9_, &node8_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&iterator_);
             i = ecu_tree_postorder_iterator_next(&iterator_))
        {
            ecu_tree_remove_node(i);
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


#warning "TODO:"
// TEST(PostOrderIterator, CanAddNodesInMiddleOfIteration)
// {
// }



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------- TESTS - ADDING AND REMOVING NODES -------------------------------------------*/
/*------------------------------- THESE TESTS FOCUS MORE ON IF THE SUBTREE IS REMOVED CORRECTLY -----------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, AddSubTreeWithNoSibilngsToAnotherTree)
{
    /* Trees before operation. We are adding Node5 to Root1.

                    root1                           root2
                    |                               |
                    node1-------node2               node6
                                                    |
                                                    node5
                                                    |
                                                    node3-------node4

    Post-operation the trees should be:

                    root1                                   root2
                    |                                       |
                    node1---node2---node5                   node6
                                    |
                                    node3-------node4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree1 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Tree2 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root2_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);

        /* Create Tree2. */
        ecu_tree_add_child_push_back(&root2_, &node6_);
        ecu_tree_add_child_push_back(&node6_, &node5_);
        ecu_tree_add_child_push_back(&node5_, &node3_);
        ecu_tree_add_child_push_back(&node5_, &node4_);

        /* Steps 2 and 3: Action and assert. Add node5 and its subtree to root1. */
        ecu_tree_add_child_push_back(&root1_, &node5_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Verify Tree2. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root2_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 * 
 * @brief Subtree has multiple siblings.
 */
TEST(AddRemoveNode, AddSubTreeThatIsFirstChildToAnotherTree)
{
    /* Trees before operation. We are adding Node5 to Root1.

                    root1                   root2
                    |                       |
                    node1---node2           node5---node6---node7
                                            |
                                            node3---node4

    Post-operation the trees should be:

                    root1                               root2
                    |                                   |
                    node1---node2---node5               node6---node7
                                    |
                                    node3---node4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree1 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Tree2 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root2_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);

        /* Create Tree2. */
        ecu_tree_add_child_push_back(&root2_, &node5_);
        ecu_tree_add_child_push_back(&root2_, &node6_);
        ecu_tree_add_child_push_back(&root2_, &node7_);
        ecu_tree_add_child_push_back(&node5_, &node3_);
        ecu_tree_add_child_push_back(&node5_, &node4_);

        /* Steps 2 and 3: Action and assert. Add node5 and its subtree to root1. */
        ecu_tree_add_child_push_back(&root1_, &node5_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Verify Tree2. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root2_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, AddSubTreeThatIsMiddleChildToAnotherTree)
{
    /* Trees before operation. We are adding Node5 to Root1.

                    root1                   root2
                    |                       |
                    node1---node2           node6---node5---node7
                                                    |
                                                    node3---node4

    Post-operation the trees should be:

                    root1                               root2
                    |                                   |
                    node1---node2---node5               node6---node7
                                    |
                                    node3---node4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree1 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Tree2 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root2_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);

        /* Create Tree2. */
        ecu_tree_add_child_push_back(&root2_, &node6_);
        ecu_tree_add_child_push_back(&root2_, &node5_);
        ecu_tree_add_child_push_back(&root2_, &node7_);
        ecu_tree_add_child_push_back(&node5_, &node3_);
        ecu_tree_add_child_push_back(&node5_, &node4_);

        /* Steps 2 and 3: Action and assert. Add node5 and its subtree to root1. */
        ecu_tree_add_child_push_back(&root1_, &node5_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Verify Tree2. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root2_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, AddSubTreeThatIsLastChildToAnotherTree)
{
    /* Trees before operation. We are adding Node5 to Root1.

                    root1                   root2
                    |                       |
                    node1---node2           node6---node7---node5
                                                            |
                                                            node3---node4

    Post-operation the trees should be:

                    root1                               root2
                    |                                   |
                    node1---node2---node5               node6---node7
                                    |
                                    node3---node4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree1 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Tree2 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root2_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);

        /* Create Tree2. */
        ecu_tree_add_child_push_back(&root2_, &node6_);
        ecu_tree_add_child_push_back(&root2_, &node7_);
        ecu_tree_add_child_push_back(&root2_, &node5_);
        ecu_tree_add_child_push_back(&node5_, &node3_);
        ecu_tree_add_child_push_back(&node5_, &node4_);

        /* Steps 2 and 3: Action and assert. Add node5 and its subtree to root1. */
        ecu_tree_add_child_push_back(&root1_, &node5_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Verify Tree2. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root2_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, AddLeafNodeToAnotherTree)
{
    /* Trees before operation. We are adding Node3 to Root1.

                    root1                   root2
                    |                       |
                    node1---node2           node5---node6---node7
                                            |
                                            node3---node4

    Post-operation the trees should be:

                    root1                               root2
                    |                                   |
                    node1---node2---node3               node5---node6---node7
                                                        |
                                                        node4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree1 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Tree2 post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node7_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root2_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);

        /* Create Tree2. */
        ecu_tree_add_child_push_back(&root2_, &node5_);
        ecu_tree_add_child_push_back(&root2_, &node6_);
        ecu_tree_add_child_push_back(&root2_, &node7_);
        ecu_tree_add_child_push_back(&node5_, &node3_);
        ecu_tree_add_child_push_back(&node5_, &node4_);

        /* Steps 2 and 3: Action and assert. Add node3 leaf to root1. */
        ecu_tree_add_child_push_back(&root1_, &node3_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Verify Tree2. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root2_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------- TESTS - ADDING AND REMOVING NODES -------------------------------------------*/
/*-------------------------------- THESE TESTS FOCUS MORE ON IF THE SUBTREE IS ADDED CORRECTLY ------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, AddTreeToAnotherTree)
{
    /* Trees before operation. We are adding node4 tree to root1.

                    root1                   node4
                    |                       |
                    node1---node2           node3

    Post-operation the tree should be:

                    root1
                    |
                    node1---node2---node4
                                    |
                                    node3
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);

        /* Create node4 tree. */
        ecu_tree_add_child_push_back(&node4_, &node3_);

        /* Steps 2 and 3: Action and assert. Add node4 tree to root1. */
        ecu_tree_add_child_push_back(&root1_, &node4_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, AddTreeToTreeNodeThatHasNoChildren)
{
    /* Trees before operation. We are adding node2 tree to node3.

                    root1                   node2
                    |                       |
                    node3---node4           node1

    Post-operation the tree should be:

                    root1
                    |
                    node3---node4
                    |
                    node2
                    |
                    node1
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&root1_, &node4_);

        /* Create node2 tree. */
        ecu_tree_add_child_push_back(&node2_, &node1_);

        /* Steps 2 and 3: Action and assert. Add node2 tree to node3. */
        ecu_tree_add_child_push_back(&node3_, &node2_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, AddTreeToTreeNodeThatHasChildren)
{
    /* Trees before operation. We are adding node4 tree to node5.

                    root1                   node4
                    |                       |
                    node5---node6           node3
                    |
                    node1---node2

    Post-operation the tree should be:

                    root1
                    |
                    node5---node6
                    |
                    node1---node2---node4
                                    |
                                    node3
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node5_);
        ecu_tree_add_child_push_back(&root1_, &node6_);
        ecu_tree_add_child_push_back(&node5_, &node1_);
        ecu_tree_add_child_push_back(&node5_, &node2_);

        /* Create node4 tree. */
        ecu_tree_add_child_push_back(&node4_, &node3_);

        /* Steps 2 and 3: Action and assert. Add node4 tree to node5. */
        ecu_tree_add_child_push_back(&node5_, &node4_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 * 
 * @brief Use case could be a file system where you move a folder to
 * a different directory.
 */
TEST(AddRemoveNode, MoveSubTreeUpWithinSameTree)
{
    /* Tree before operation. We are moving Node6 and its subtree to Root1.

                    root1
                    |
                    node2---node3---node4
                    |
                    node6---node1
                    |
                    node5

    Post-operation the tree should be:

                    root1
                    |
                    node2---node3---node4---node6
                    |                       |
                    node1                   node5
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node2_);
        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&root1_, &node4_);
        ecu_tree_add_child_push_back(&node2_, &node6_);
        ecu_tree_add_child_push_back(&node2_, &node1_);
        ecu_tree_add_child_push_back(&node6_, &node5_);

        /* Steps 2 and 3: Action and assert. Moving node6 and its subtree to root1. */
        ecu_tree_add_child_push_back(&root1_, &node6_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 */
TEST(AddRemoveNode, PushBackSubTreeWithinSameTree)
{
    /* Tree before operation. We are trying to push back Node4 to Root1.

                    root1
                    |
                    node4---node1
                    |
                    node2---node3

        Post-operation the tree should be:

                    root1
                    |
                    node1---node4
                            |
                            node2---node3
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node4_);
        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&node4_, &node2_);
        ecu_tree_add_child_push_back(&node4_, &node3_);

        /* Steps 2 and 3: Action and assert. Push back node4 and its subtree to root1. */
        ecu_tree_add_child_push_back(&root1_, &node4_);

        /* Verify Tree1. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 * 
 * @brief Basically calling add function twice. Verify tree
 * remains the same.
 */
TEST(AddRemoveNode, AddSubTreeAlreadyInRequestedPlace)
{
    /* Tree before operation. We are trying to add Node3 to Root1 again even 
    though it is already there. Tree should be exactly the same post-operation.

                    root1
                    |
                    node3
                    |
                    node1---node2
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();

        /* Tree post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&node3_, &node1_);
        ecu_tree_add_child_push_back(&node3_, &node2_);

        /* Steps 2 and 3: Action and assert. Add node3 to root again even though it is already in place. */
        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&root1_, &node3_);

        /* Verify Tree1 is the same. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation. Asserts enabled.
 */
TEST(AddRemoveNode, CannotAddNodeToItself)
{
    /* Tree before operation. We are trying to add Node3 to itself which is illegal. 
    Tree should be exactly the same post-operation.

                    root1
                    |
                    node3
                    |
                    node1---node2
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_ok_));

        mock().strictOrder();

        /* Tree post-operation check. */
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        /* Create Tree1. */
        ecu_tree_add_child_push_back(&root1_, &node3_);
        ecu_tree_add_child_push_back(&node3_, &node1_);
        ecu_tree_add_child_push_back(&node3_, &node2_);

        /* Steps 2 and 3: Action and assert. Add node3 to itself which is illegal. */
        ecu_tree_add_child_push_back(&node3_, &node3_);
        ecu_tree_add_child_push_back(&node3_, &node3_);
        ecu_tree_add_child_push_back(&node3_, &node3_);
    }
    catch (AssertException& e)
    {
        (void)e;
        /* OK */
    }

    /* Verify Tree1 is the same. Do this outside try-catch in case assert fires. */
    for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
         i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
         i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
    {
        IteratorTreeNodeMock::verify_node(i);
    }
}


/**
 * @pre Postorder iterator is working properly since it is used
 * for test validation.
 * 
 * @brief Verify nothing happens to tree when root is removed.
 */
TEST(AddRemoveNode, RemoveRoot)
{
    /* Test tree: note that node number is the order in which it should be iterated.
    This tree should remain the same pre and post-removal of root.

                    root1
                    |
                    node6
                    |
                    node3-----node4-----node5
                    |
                    node1-----node2
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node1_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node2_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node3_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node4_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node5_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &node6_);
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &root1_);

        ecu_tree_add_child_push_back(&root1_, &node6_);
        ecu_tree_add_child_push_back(&node6_, &node3_);
        ecu_tree_add_child_push_back(&node6_, &node4_);
        ecu_tree_add_child_push_back(&node6_, &node5_);
        ecu_tree_add_child_push_back(&node3_, &node1_);
        ecu_tree_add_child_push_back(&node3_, &node2_);

        /* Steps 2 and 3: Action and assert. */
        ecu_tree_remove_node(&root1_);

        /* Verify tree is the same post-removal of root. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &root1_);
             i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
             i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- TESTS - DESTRUCTOR ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/
#warning "TODO:"

// /**
//  * @pre Node, addition, removal, and postorder iterator tests are 
//  * passing since the destructor relies on these operations. This 
//  * test also uses postorder iterator for verification.
//  * 
//  * @brief Verify all node destroy callbacks are called. Verify 
//  * all nodes are not in a tree after destroy operation.
//  */
// TEST(??, DestroyEntireTree)
// {
// 	// verify nodes are not in tree with postorder iterator.
// }


// /**
//  * @pre Node, addition, removal, and postorder iterator tests are 
//  * passing since the destructor relies on these operations. This 
//  * test also uses postorder iterator for verification.
//  * 
//  * @brief Verify only node destroy callbacks in destroyed subtree 
//  * were called. Verify all nodes in destroyed subtree are not apart
//  * of a tree anymore. Verify remaining tree is still intact with
//  * proper structure.
//  */
// TEST(??, DestroySubTreeThatHasNoSiblings)
// {

// }


// /**
//  * @pre Node, addition, removal, and postorder iterator tests are 
//  * passing since the destructor relies on these operations. This 
//  * test also uses postorder iterator for verification.
//  * 
//  * @brief Subtree has siblings. Verify only node destroy callbacks 
//  * in destroyed subtree were called. Verify all nodes in destroyed 
//  * subtree are not apart of a tree anymore. Verify remaining tree 
//  * is still intact with proper structure.
//  */
// TEST(??, DestroySubTreeThatIsFirstChild)
// {
// 	// subtree has siblings
// }


// /**
//  * @pre Node, addition, removal, and postorder iterator tests are 
//  * passing since the destructor relies on these operations. This 
//  * test also uses postorder iterator for verification.
//  * 
//  * @brief Verify only node destroy callbacks in destroyed subtree 
//  * were called. Verify all nodes in destroyed subtree are not 
//  * apart of a tree anymore. Verify remaining tree is still intact 
//  * with proper structure.
//  */
// TEST(??, DestroySubTreeThatIsMiddleChild)
// {

// }


// /**
//  * @pre Node, addition, removal, and postorder iterator tests are 
//  * passing since the destructor relies on these operations. This 
//  * test also uses postorder iterator for verification.
//  * 
//  * @brief Verify only node destroy callbacks in destroyed subtree 
//  * were called. Verify all nodes in destroyed subtree are not 
//  * apart of a tree anymore. Verify remaining tree is still intact 
//  * with proper structure.
//  */
// TEST(??, DestroySubTreeThatIsLastChild)
// {

// }


// /**
//  * @pre Node, addition, removal, and postorder iterator tests are 
//  * passing since the destructor relies on these operations. This 
//  * test also uses postorder iterator for verification.
//  * 
//  * @brief Verify only destroy callback for leaf node was caled. 
//  * Verify leaf is no longer in a tree. Verify remaining tree is still 
//  * intact with proper structure.
//  */
// TEST(???, DestroyLeafNode)
// {

// }



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- TESTS - GET LEVEL -------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Verify level of all nodes in tree is correct.
 */
TEST(GetLevelAndLCA, LevelNodesInDegenerateTree)
{
    /* Test tree:

                    root1
                    |
                    node1
                    |
                    node2
                    |
                    node3
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&node1_, &node2_);
        ecu_tree_add_child_push_back(&node2_, &node3_);

        /* Steps 2 and 3: Action and assert. */
        UNSIGNED_LONGS_EQUAL(0, ecu_tree_get_level(&root1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node1_));
		UNSIGNED_LONGS_EQUAL(2, ecu_tree_get_level(&node2_));
		UNSIGNED_LONGS_EQUAL(3, ecu_tree_get_level(&node3_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


TEST(GetLevelAndLCA, LevelNodesInGenericTree)
{
    /* Test tree:

                    root1
                    |
                    node1---node2---node3
                    		|		|
                    		node4	node5
									|
									node6---node7
									|
									node8
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        ecu_tree_add_child_push_back(&root1_, &node1_);
		ecu_tree_add_child_push_back(&root1_, &node2_);
		ecu_tree_add_child_push_back(&root1_, &node3_);
		ecu_tree_add_child_push_back(&node2_, &node4_);
		ecu_tree_add_child_push_back(&node3_, &node5_);
		ecu_tree_add_child_push_back(&node5_, &node6_);
		ecu_tree_add_child_push_back(&node5_, &node7_);
		ecu_tree_add_child_push_back(&node6_, &node8_);

        /* Steps 2 and 3: Action and assert. */
        UNSIGNED_LONGS_EQUAL(0, ecu_tree_get_level(&root1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node2_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node3_));
		UNSIGNED_LONGS_EQUAL(2, ecu_tree_get_level(&node4_));
		UNSIGNED_LONGS_EQUAL(2, ecu_tree_get_level(&node5_));
		UNSIGNED_LONGS_EQUAL(3, ecu_tree_get_level(&node6_));
		UNSIGNED_LONGS_EQUAL(3, ecu_tree_get_level(&node7_));
		UNSIGNED_LONGS_EQUAL(4, ecu_tree_get_level(&node8_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


TEST(GetLevelAndLCA, LevelAddAndRemoveSubTreesInTree)
{
    /* Test trees before addition. Add root2 to node1.

            root1					root2
            |						|
            node1---node2			node3---node4

		Tree after addition. Remove node1 subtree.

			root1
			|
			node1---node2
			|
			root2
			|
			node3---node4

		Test trees after removal of node1 subtree.

			root1			node1
            |				|
            node2			root2
							|
							node3---node4
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

		/* Create Tree1. */
		ecu_tree_add_child_push_back(&root1_, &node1_);
		ecu_tree_add_child_push_back(&root1_, &node2_);

		/* Create Tree2. */
		ecu_tree_add_child_push_back(&root2_, &node3_);
		ecu_tree_add_child_push_back(&root2_, &node4_);

        /* Steps 2 and 3: Action and assert. Verify levels of nodes in both trees. */
        UNSIGNED_LONGS_EQUAL(0, ecu_tree_get_level(&root1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node2_));

        UNSIGNED_LONGS_EQUAL(0, ecu_tree_get_level(&root2_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node3_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node4_));

        /* Steps 2 and 3: Action and assert. Add trees together. Verify levels of nodes after addition. */
		ecu_tree_add_child_push_back(&node1_, &root2_);
        UNSIGNED_LONGS_EQUAL(0, ecu_tree_get_level(&root1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node2_));
        UNSIGNED_LONGS_EQUAL(2, ecu_tree_get_level(&root2_));
		UNSIGNED_LONGS_EQUAL(3, ecu_tree_get_level(&node3_));
		UNSIGNED_LONGS_EQUAL(3, ecu_tree_get_level(&node4_));

        /* Steps 2 and 3: Action and assert. Remove subtree. Verify levels of nodes in both after removal. */
		ecu_tree_remove_node(&node1_);
        UNSIGNED_LONGS_EQUAL(0, ecu_tree_get_level(&root1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&node2_));

		UNSIGNED_LONGS_EQUAL(0, ecu_tree_get_level(&node1_));
		UNSIGNED_LONGS_EQUAL(1, ecu_tree_get_level(&root2_));
		UNSIGNED_LONGS_EQUAL(2, ecu_tree_get_level(&node3_));
		UNSIGNED_LONGS_EQUAL(2, ecu_tree_get_level(&node4_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- TESTS - GET LCA --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Following tests are done:
 * 1. Random nodes within the tree.
 * 2. One node is the root of the tree. LCA should be the root.
 * 3. One node is the parent of another node. LCA should be the parent.
 * 4. One node is the grandparent of another node. LCA should be the grandparent.
 * 5. Test associativity for all cases. I.e. lca(a, b) == lca(b, a)
 */
TEST(GetLevelAndLCA, LCAGenericTreeMultipleTestCases)
{
    /* Test tree:

                    root1
                    |
                    node1-----------------------------------------------------------node2
                    |                                                               |
                    node3-------node4-------node5                                   node6
                                |           |                                       |
                                node7       node8-------node9------node10           node11
                                                        |
                                                        node12
    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);
        ecu_tree_add_child_push_back(&node1_, &node3_);
        ecu_tree_add_child_push_back(&node1_, &node4_);
        ecu_tree_add_child_push_back(&node1_, &node5_);
        ecu_tree_add_child_push_back(&node2_, &node6_);
        ecu_tree_add_child_push_back(&node4_, &node7_);
        ecu_tree_add_child_push_back(&node5_, &node8_);
        ecu_tree_add_child_push_back(&node5_, &node9_);
        ecu_tree_add_child_push_back(&node5_, &node10_);
        ecu_tree_add_child_push_back(&node6_, &node11_);
        ecu_tree_add_child_push_back(&node9_, &node12_);

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL(&node1_, ecu_tree_get_lca(&node4_, &node12_));
        POINTERS_EQUAL(&node1_, ecu_tree_get_lca(&node12_, &node4_));

        POINTERS_EQUAL(&node5_, ecu_tree_get_lca(&node8_, &node10_));
        POINTERS_EQUAL(&node5_, ecu_tree_get_lca(&node10_, &node8_));

        POINTERS_EQUAL(&root1_, ecu_tree_get_lca(&node3_, &node11_));
        POINTERS_EQUAL(&root1_, ecu_tree_get_lca(&node11_, &node3_));

        POINTERS_EQUAL(&root1_, ecu_tree_get_lca(&root1_, &node12_));   /* One node is root. LCA should be root. */
        POINTERS_EQUAL(&root1_, ecu_tree_get_lca(&node12_, &root1_));   /* One node is root. LCA should be root. */

        POINTERS_EQUAL(&node9_, ecu_tree_get_lca(&node9_, &node12_));   /* node9 is parent of node12. LCA should be node9 (the parent). */
        POINTERS_EQUAL(&node9_, ecu_tree_get_lca(&node12_, &node9_));   /* node9 is parent of node12. LCA should be node9 (the parent). */

        POINTERS_EQUAL(&node5_, ecu_tree_get_lca(&node5_, &node12_));   /* node5 is grandparent of node12. LCA should be node5 (the grandparent). */
        POINTERS_EQUAL(&node5_, ecu_tree_get_lca(&node12_, &node5_));   /* node5 is grandparent of node12. LCA should be node5 (the grandparent). */
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief LCA is the node itself.
 */
TEST(GetLevelAndLCA, LCATwoNodesThatAreTheSame)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL(&node1_, ecu_tree_get_lca(&node1_, &node1_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief LCA function should return null. Also pass
 * in the two different tree roots to verify null is
 * also returned.
 */
TEST(GetLevelAndLCA, LCATwoNodesInDifferentTrees)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root2_, &node2_);

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL((struct ecu_tree_node *)0, ecu_tree_get_lca(&node1_, &node2_));
        POINTERS_EQUAL((struct ecu_tree_node *)0, ecu_tree_get_lca(&root1_, &root2_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


/**
 * @brief This function calls LCA functions so included here.
 */
TEST(GetLevelAndLCA, TestNodesInSameTreeFunction)
{
    /* Test trees:

        root1                   root2
        |                       |
        node1---node2           node3

    */
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        ecu_tree_add_child_push_back(&root1_, &node1_);
        ecu_tree_add_child_push_back(&root1_, &node2_);
        ecu_tree_add_child_push_back(&root2_, &node3_);

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE(ecu_tree_nodes_in_same_tree(&root1_, &root1_)); /* Same parameter edge case. */
        CHECK_TRUE(ecu_tree_nodes_in_same_tree(&root1_, &node1_));
        CHECK_TRUE(ecu_tree_nodes_in_same_tree(&node1_, &node2_));
        CHECK_TRUE(ecu_tree_nodes_in_same_tree(&root2_, &node3_));
        CHECK_TRUE(ecu_tree_nodes_in_same_tree(&node3_, &node3_)); /* Same parameter edge case. */

        CHECK_FALSE(ecu_tree_nodes_in_same_tree(&root1_, &root2_));
        CHECK_FALSE(ecu_tree_nodes_in_same_tree(&node3_, &node1_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- TESTS - MISC ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Verify @ref ECU_TREE_NODE_GET_ENTRY() macro.
 */
TEST(TreeMisc, EcuTreeNodeGetEntryMacro)
{
	/* Step 1: Arrange. Preconditions. */
	CHECK( (user_tree_node_.a_ != 1) );
	CHECK( (user_tree_node_.b_ != 2) );
	CHECK( (user_tree_node_.c_ != 3) );
	CHECK( (user_tree_node_.d_ != 4) );

	/* Step 2: Action. */
	struct user_tree_node_type *me = ECU_TREE_NODE_GET_ENTRY(&user_tree_node_.node_, 
															 struct user_tree_node_type, 
															 node_);

	me->a_ = 1;
	me->b_ = 2;
	me->c_ = 3;
	me->d_ = 4;

	/* Step 3: Assert. */
	UNSIGNED_LONGS_EQUAL(1, user_tree_node_.a_);
	UNSIGNED_LONGLONGS_EQUAL(2, user_tree_node_.b_);
	UNSIGNED_LONGS_EQUAL(3, user_tree_node_.c_);
	UNSIGNED_LONGS_EQUAL(4, user_tree_node_.d_);
}
