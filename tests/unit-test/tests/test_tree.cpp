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
    struct ecu_tree tree_;
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
    struct ecu_tree tree1_;
    struct ecu_tree tree2_;
    struct ecu_tree_node node1_;
    struct ecu_tree_node node2_;
    struct ecu_tree_node node3_;
    struct ecu_tree_node node4_;
    struct ecu_tree_node node5_;
    struct ecu_tree_node node6_;
    struct ecu_tree_node node7_;
};


TEST_GROUP(GetLCA)
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

    struct ecu_tree tree1_;
    struct ecu_tree tree2_;
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
                    child1------child2-----child3
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

        ecu_tree_node_ctor(&parent_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child2_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child3_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        ecu_tree_node_add_child_push_back(&parent_, &child1_);
        ecu_tree_node_add_child_push_back(&parent_, &child2_);
        ecu_tree_node_add_child_push_back(&parent_, &child3_);

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
        ecu_tree_node_ctor(&parent_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        /* Steps 2 and 3: Action and assert. */
        ecu_tree_node_add_child_push_back(&parent_, &child1_);
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
        
        ecu_tree_node_ctor(&parent_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        ecu_tree_node_add_child_push_back(&parent_, &child1_);

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
                    child1------child2-----child3
                                |
                                child4-----child5-----child6
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
        
        ecu_tree_node_ctor(&parent_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child2_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child3_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child4_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child5_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child6_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        /* Add children to root (parent). */
        ecu_tree_node_add_child_push_back(&parent_, &child1_);
        ecu_tree_node_add_child_push_back(&parent_, &child2_);
        ecu_tree_node_add_child_push_back(&parent_, &child3_);

        /* Add grandchildren (child2 has children now). */
        ecu_tree_node_add_child_push_back(&child2_, &child4_);
        ecu_tree_node_add_child_push_back(&child2_, &child5_);
        ecu_tree_node_add_child_push_back(&child2_, &child6_);

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

        /* Add children to root (parent). These children will be the target nodes
        we test the iterator on. */
        ecu_tree_node_add_child_push_back(&parent_, &child1_);
        ecu_tree_node_add_child_push_back(&parent_, &child2_);
        ecu_tree_node_add_child_push_back(&parent_, &child3_);

        /* Add grandchildren (child1, child2, and child3 now all have 2 children). */
        ecu_tree_node_add_child_push_back(&child1_, &child4_);
        ecu_tree_node_add_child_push_back(&child1_, &child5_);    

        ecu_tree_node_add_child_push_back(&child2_, &child6_);
        ecu_tree_node_add_child_push_back(&child2_, &child7_);

        ecu_tree_node_add_child_push_back(&child3_, &child8_);
        ecu_tree_node_add_child_push_back(&child3_, &child9_);

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


TEST(ChildIterator, RemoveChildrenInMiddleOfIteration)
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
        
        ecu_tree_node_ctor(&parent_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child2_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child3_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child4_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        ecu_tree_node_add_child_push_back(&parent_, &child1_);
        ecu_tree_node_add_child_push_back(&parent_, &child2_);
        ecu_tree_node_add_child_push_back(&parent_, &child3_);
        ecu_tree_node_add_child_push_back(&parent_, &child4_);

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


/**
 * @brief Add children to each node we iterate through.
 * Verify it does not effect the iteration.
 */
TEST(ChildIterator, AddGrandchildrenInMiddleOfIteration)
{
    /* Test tree: note test will add one child to child1, child2, and child3. 

                    parent
                    |
                    child1------child2-----child3
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
        
        ecu_tree_node_ctor(&parent_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child1_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child2_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child3_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child4_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child5_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&child6_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        ecu_tree_node_add_child_push_back(&parent_, &child1_);
        ecu_tree_node_add_child_push_back(&parent_, &child2_);
        ecu_tree_node_add_child_push_back(&parent_, &child3_);

        /* Steps 2 and 3: Action and assert. Add one grandchild to each node
        we iterate through. */
        for (struct ecu_tree_node *i = ecu_tree_child_iterator_begin(&iterator_, &parent_);
             i != ecu_tree_child_iterator_end(&iterator_);
             i = ecu_tree_child_iterator_next(&iterator_))
        {
            if (i == &child1_)
            {
                ecu_tree_node_add_child_push_back(i, &child4_);
            }
            else if (i == &child2_)
            {
                ecu_tree_node_add_child_push_back(i, &child5_);
            }
            else if (i == &child3_)
            {
                ecu_tree_node_add_child_push_back(i, &child6_);
            }
            else
            {
                FAIL("Invalid path");
            }

            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Also verify grandchildren were added 
        successfully within the iteration. */
        POINTERS_EQUAL(&child4_, ecu_tree_child_iterator_begin(&iterator_, &child1_));
        POINTERS_EQUAL(&child5_, ecu_tree_child_iterator_begin(&iterator_, &child2_));
        POINTERS_EQUAL(&child6_, ecu_tree_child_iterator_begin(&iterator_, &child3_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}



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

                    root
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
              .withParameter("node", &tree_.root);

        ecu_tree_ctor(&tree_);
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

        /* Level 1 */
        ecu_tree_add_child_push_back(&tree_, &node2_);
        ecu_tree_add_child_push_back(&tree_, &node10_);
        ecu_tree_add_child_push_back(&tree_, &node12_);

        /* Level 2 */
        ecu_tree_node_add_child_push_back(&node2_, &node1_);

        ecu_tree_node_add_child_push_back(&node10_, &node3_);
        ecu_tree_node_add_child_push_back(&node10_, &node7_);
        ecu_tree_node_add_child_push_back(&node10_, &node9_);

        ecu_tree_node_add_child_push_back(&node12_, &node11_);

        /* Level 3 */
        ecu_tree_node_add_child_push_back(&node7_, &node5_);
        ecu_tree_node_add_child_push_back(&node7_, &node6_);

        ecu_tree_node_add_child_push_back(&node9_, &node8_);

        /* Level 4 */
        ecu_tree_node_add_child_push_back(&node5_, &node4_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &tree_.root);
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
TEST(PostOrderIterator, MultiLevelSubTreeWithSiblings)
{
    /* Test tree: note that node number is the order in which it should be iterated.
    but we will only be doing node9's subtree for this test.

                    root
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

        ecu_tree_ctor(&tree_);
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

        /* Level 1 */
        ecu_tree_add_child_push_back(&tree_, &node9_);
        ecu_tree_add_child_push_back(&tree_, &node11_);
        ecu_tree_add_child_push_back(&tree_, &node12_);

        /* Level 2 */
        ecu_tree_node_add_child_push_back(&node9_, &node5_);
        ecu_tree_node_add_child_push_back(&node9_, &node6_);
        ecu_tree_node_add_child_push_back(&node9_, &node8_);

        ecu_tree_node_add_child_push_back(&node11_, &node10_);

        /* Level 3 */
        ecu_tree_node_add_child_push_back(&node5_, &node1_);
        ecu_tree_node_add_child_push_back(&node5_, &node4_);

        ecu_tree_node_add_child_push_back(&node8_, &node7_);

        /* Level 4 */
        ecu_tree_node_add_child_push_back(&node4_, &node2_);
        ecu_tree_node_add_child_push_back(&node4_, &node3_);

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

                    root
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
              .withParameter("node", &tree_.root);

        ecu_tree_ctor(&tree_);
        ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        ecu_tree_add_child_push_back(&tree_, &node3_);
        ecu_tree_node_add_child_push_back(&node3_, &node2_);
        ecu_tree_node_add_child_push_back(&node2_, &node1_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &tree_.root);
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
 * should only return this root.
 */
TEST(PostOrderIterator, SingleNode)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        mock().strictOrder();
        mock().expectOneCall("IteratorTreeNodeMock::verify_node")
              .withParameter("node", &tree_.root);

        ecu_tree_ctor(&tree_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &tree_.root);
             i != ecu_tree_postorder_iterator_end(&iterator_);
             i = ecu_tree_postorder_iterator_next(&iterator_))
        {
            IteratorTreeNodeMock::verify_node(i);
        }

        /* Steps 2 and 3: Action and assert. Verify wraparound. */
        POINTERS_EQUAL(&tree_.root, ecu_tree_postorder_iterator_next(&iterator_));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}


TEST(PostOrderIterator, RemoveNodesInMiddleOfIteration)
{
    /* Test tree: note that node number is the order in which it should be iterated

                    root
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
              .withParameter("node", &tree_.root);

        ecu_tree_ctor(&tree_);
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

        /* Level 1 */
        ecu_tree_add_child_push_back(&tree_, &node2_);
        ecu_tree_add_child_push_back(&tree_, &node3_);
        ecu_tree_add_child_push_back(&tree_, &node5_);
        ecu_tree_add_child_push_back(&tree_, &node6_);
        ecu_tree_add_child_push_back(&tree_, &node12_);

        /* Level 2 */
        ecu_tree_node_add_child_push_back(&node2_, &node1_);

        ecu_tree_node_add_child_push_back(&node5_, &node4_);

        ecu_tree_node_add_child_push_back(&node12_, &node9_);
        ecu_tree_node_add_child_push_back(&node12_, &node10_);
        ecu_tree_node_add_child_push_back(&node12_, &node11_);

        /* Level 3 */
        ecu_tree_node_add_child_push_back(&node9_, &node7_);
        ecu_tree_node_add_child_push_back(&node9_, &node8_);

        /* Steps 2 and 3: Action and assert. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &tree_.root);
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

#warning "TODO: Adding nodes in middle of postorder iteration?"



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------- TESTS - ADDING AND REMOVING NODES -------------------------------------------*/
/*------------------------------- THESE TESTS FOCUS MORE ON IF THE SUBTREE IS REMOVED CORRECTLY -----------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// 1. Add subtree that has no siblings to another tree node.
// 2. Add subtree that has siblings but is the first child to another tree node.
// 3. Add subtree that is the middle sibling to another tree node.
// 4. Add subtree that is the last sibling to another tree node.
// 5. Add subtree leaf node to another tree.

TEST(AddRemoveNode, AddSubTreeWithNoSibilngsToAnotherTree)
{

}


/**
 * @brief Subtree has multiple siblings.
 */
TEST(AddRemoveNode, AddSubTreeThatIsFirstChildToAnotherTree)
{

}


TEST(AddRemoveNode, AddSubTreeThatIsMiddleChildToAnotherTree)
{

}


TEST(AddRemoveNode, AddSubTreeThatIsLastChildToAnotherTree)
{

}


TEST(AddRemoveNode, AddLeafNodeToAnotherTree)
{

}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------- TESTS - ADDING AND REMOVING NODES -------------------------------------------*/
/*-------------------------------- THESE TESTS FOCUS MORE ON IF THE SUBTREE IS ADDED CORRECTLY ------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

// 1. Add subtree not in tree to another tree.
// 2. Add subtree to tree node that has no children.
// 3. Add subtree to tree node that has children.
// 4. Move subtree within same tree (e.g. file system moving folder two directories up).
// 5. Cannot add tree root to another tree.

TEST(AddRemoveNode, AddSubTreeNotInTreeToAnotherTree)
{

}


TEST(AddRemoveNode, AddSubTreeToTreeNodeThatHasNoChildren)
{

}


TEST(AddRemoveNode, AddSubTreeToTreeNodeThatHasChildren)
{

}


/**
 * @brief Use case could be a file system where you move a folder to
 * a different directory.
 */
TEST(AddRemoveNode, MoveSubTreeWithinSameTree)
{

}


TEST(AddRemoveNode, CannotAddTreeRootToAnotherTree)
{
    
}



// /**
//  * @pre Postorder iterator is working properly since it is used
//  * for test validation.
//  */
// TEST(AddRemoveNode, AddSubTreeNotInTreeToAnotherTree)
// {
//     /* Trees we are adding together. Node4 and its subtree is being added
//     to Node5:
//                     root1                               node4
//                     |                                   |
//                     node5-----------node7               node1-----node2-----node3
//                                     |
//                                     node6

//     Post-addition the tree should be:

//                     root
//                     |
//                     node5---------------------------node7
//                     |                               |
//                     node4                           node6
//                     |
//                     node1-----node2-----node3
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node2_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node3_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node4_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node5_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node6_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node7_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree1_.root);

//         ecu_tree_ctor(&tree1_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

//         /* Create main tree. */
//         ecu_tree_add_child_push_back(&tree1_, &node5_);
//         ecu_tree_add_child_push_back(&tree1_, &node7_);
//         ecu_tree_node_add_child_push_back(&node7_, &node6_);

//         /* Create node4's subtree */
//         ecu_tree_node_add_child_push_back(&node4_, &node1_);
//         ecu_tree_node_add_child_push_back(&node4_, &node2_);
//         ecu_tree_node_add_child_push_back(&node4_, &node3_);

//         /* Steps 2 and 3: Action and assert. Add node4 and its subtree to node5. */
//         ecu_tree_node_add_child_push_back(&node5_, &node4_);

//         /* Verify tree. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
//              i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//              i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


// TEST(AddRemoveNode, AddSubTreeWithNoSiblingsToAnotherTreeNodeThatHasNoChildren)
// {
//     /* Trees we are adding together. Node4 and its subtree is being added
//     to Node5:
//                     root1                               root2
//                     |                                   |
//                     node5-----------node7               node4
//                                     |                   |
//                                     node6               node1-----node2-----node3

//     Post-addition the tree should be:

//                     root
//                     |
//                     node5---------------------------node7
//                     |                               |
//                     node4                           node6
//                     |
//                     node1-----node2-----node3
//     */
// }


// /**
//  * @pre Postorder iterator is working properly since it is used
//  * for test validation.
//  * 
//  * @brief Add child to a parent that already has children.
//  */
// TEST(AddRemoveNode, AddSubTreeToTreeNodeWithChildren)
// {
//     /* Trees we are adding together. Node6 and its subtree is being added
//     to root:

//                     root                            node6
//                     |                               |
//                     node1-------node3               node4-----node5
//                                 |
//                                 node2



//     Post-addition the tree should be:

//                     root
//                     |
//                     node1-------node3-------node6
//                                 |           |
//                                 node2       node4-----node5
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node2_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node3_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node4_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node5_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node6_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree1_.root);

//         ecu_tree_ctor(&tree1_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

//         /* Create main tree. */
//         ecu_tree_add_child_push_back(&tree1_, &node1_);
//         ecu_tree_add_child_push_back(&tree1_, &node3_);
//         ecu_tree_node_add_child_push_back(&node3_, &node2_);

//         /* Create node6's subtree */
//         ecu_tree_node_add_child_push_back(&node6_, &node4_);
//         ecu_tree_node_add_child_push_back(&node6_, &node5_);

//         /* Steps 2 and 3: Action and assert. Add node6 and its subtree to root. */
//         ecu_tree_add_child_push_back(&tree1_, &node6_);

//         /* Verify tree. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
//              i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//              i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


// /**
//  * @pre Postorder iterator is working properly since it is used
//  * for test validation.
//  */
// TEST(AddRemoveNode, AddSingleNodeInSubTreeToTree)
// {
//     /* Trees we are adding together. Node4 is being added to root:

//                     root                            node6
//                     |                               |
//                     node1-------node3               node4-----node5
//                                 |
//                                 node2



//     Post-addition the trees should be:

//                     root                                node6
//                     |                                   |
//                     node1-------node3-------node4       node5
//                                 |
//                                 node2
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node2_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node3_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node4_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree1_.root);

//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node5_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node6_);

//         ecu_tree_ctor(&tree1_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

//         /* Create main tree. */
//         ecu_tree_add_child_push_back(&tree1_, &node1_);
//         ecu_tree_add_child_push_back(&tree1_, &node3_);
//         ecu_tree_node_add_child_push_back(&node3_, &node2_);

//         /* Create node6's subtree */
//         ecu_tree_node_add_child_push_back(&node6_, &node4_);
//         ecu_tree_node_add_child_push_back(&node6_, &node5_);

//         /* Steps 2 and 3: Action and assert. Add node4 to root. */
//         ecu_tree_add_child_push_back(&tree1_, &node4_);

//         /* Verify both tree and subtree. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
//              i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//              i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


// TEST(AddRemoveNode, CannotAddTreeRootToAnotherTree)
// {

// }


// /**
//  * @pre Postorder iterator is working properly since it is used
//  * for test validation.
//  */
// TEST(AddRemoveNode, CannotAddNodeAlreadyInTreeToAnotherTree)
// {
//     /* Test trees. We are attempting to add node1 to root2. Trees should
//     remain the same since this operation is illegal.

//                     root1                   root2
//                     |
//                     node1
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_ok_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree1_.root);

//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree2_.root);

//         ecu_tree_ctor(&tree1_);
//         ecu_tree_ctor(&tree2_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_add_child_push_back(&tree1_, &node1_);

//         /* Step 2: Action. */
//         ecu_tree_add_child_push_back(&tree2_, &node1_);
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* OK */
//     }

//     /* Step 3: Assert. Both trees should remain the same since this operation is illegal.
//     Verify with mocks. Do this outside of try-catch in case assert in API fires (expected behavior). */
//     for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
//          i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//          i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//     {
//         IteratorTreeNodeMock::verify_node(i);
//     }

//     for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree2_.root);
//          i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//          i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//     {
//         IteratorTreeNodeMock::verify_node(i);
//     }
// }


// /**
//  * @pre Postorder iterator is working properly since it is used
//  * for test validation.
//  */
// TEST(AddRemoveNode, CannotAddNodeAlreadyInTreeToAnotherSubTree)
// {
//     /* Test trees. We are attempting to add node1 to node2. Trees should
//     remain the same since this operation is illegal.

//                     root1                   node2
//                     |
//                     node1
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_ok_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree1_.root);

//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node2_);

//         ecu_tree_ctor(&tree1_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_add_child_push_back(&tree1_, &node1_);

//         /* Step 2: Action. */
//         ecu_tree_node_add_child_push_back(&node2_, &node1_);
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* OK */
//     }

//     /* Step 3: Assert. Both trees should remain the same since this operation is illegal.
//     Verify with mocks. Do this outside of try-catch in case assert in API fires (expected behavior). */
//     for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
//          i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//          i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//     {
//         IteratorTreeNodeMock::verify_node(i);
//     }

//     for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree2_.root);
//          i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//          i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//     {
//         IteratorTreeNodeMock::verify_node(i);
//     }
// }


// /**
//  * @pre Postorder iterator is working properly since it is used
//  * for test validation.
//  * 
//  * @brief Edge case. Verify main tree is correct after removal. 
//  * Verify removed node's subtree is unharmed.
//  */
// TEST(AddRemoveNode, RemoveFirstChildThatHasNoSiblings)
// {
//     /* Test tree before removal. We are removing Node4. Its subtree
//     should be unharmed.

//                     root
//                     |
//                     node5---------------------------node7
//                     |                               |
//                     node4                           node6
//                     |
//                     node1-----node2-----node3

//     Trees after removal should be:

//                     root                                node4
//                     |                                   |
//                     node5-----------node7               node1-----node2-----node3
//                                     |
//                                     node6
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node2_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node3_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node4_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node5_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node6_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node7_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree1_.root);

//         ecu_tree_ctor(&tree1_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

//         /* Level 1 */
//         ecu_tree_add_child_push_back(&tree1_, &node5_);
//         ecu_tree_add_child_push_back(&tree1_, &node7_);

//         /* Level 2 */
//         ecu_tree_node_add_child_push_back(&node5_, &node4_);

//         ecu_tree_node_add_child_push_back(&node7_, &node6_);

//         /* Level 3 */
//         ecu_tree_node_add_child_push_back(&node4_, &node1_);
//         ecu_tree_node_add_child_push_back(&node4_, &node2_);
//         ecu_tree_node_add_child_push_back(&node4_, &node3_);

//         /* Steps 2 and 3: Action and assert. */
//         ecu_tree_remove_node(&node4_);

//         /* Verify node4's subtree is unharmed. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &node4_);
//              i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//              i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }

//         /* Verify main tree is as expected. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
//              i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//              i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


// /**
//  * @pre Postorder iterator is working properly since it is used
//  * for test validation.
//  * 
//  * @brief Edge case. Verify main tree is correct after removal. 
//  * Verify removed node's subtree is unharmed.
//  */
// TEST(AddRemoveNode, RemoveFirstChildThatHasSiblings)
// {
//     /* Test tree before removal. We are removing Node3. Its subtree
//     should be unharmed.

//                     root
//                     |
//                     node6
//                     |
//                     node3-----node4-----node5
//                     |
//                     node1-----node2

//     Trees after removal should be:

//                     root                    node3
//                     |                       |
//                     node6                   node1-----node2
//                     |
//                     node4-----node5
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node2_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node3_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node4_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node5_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node6_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree1_.root);

//         ecu_tree_ctor(&tree1_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

//         /* Level 1 */
//         ecu_tree_add_child_push_back(&tree1_, &node6_);

//         /* Level 2 */
//         ecu_tree_node_add_child_push_back(&node6_, &node3_);
//         ecu_tree_node_add_child_push_back(&node6_, &node4_);
//         ecu_tree_node_add_child_push_back(&node6_, &node5_);

//         /* Level 3 */
//         ecu_tree_node_add_child_push_back(&node3_, &node1_);
//         ecu_tree_node_add_child_push_back(&node3_, &node2_);

//         /* Steps 2 and 3: Action and assert. */
//         ecu_tree_remove_node(&node3_);

//         /* Verify node3's subtree is unharmed. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &node3_);
//              i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//              i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }

//         /* Verify main tree is as expected. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
//              i != ecu_tree_postorder_iterator_end(&postorder_iterator_);
//              i = ecu_tree_postorder_iterator_next(&postorder_iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


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

                    root
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
              .withParameter("node", &tree1_.root);

        ecu_tree_ctor(&tree1_);
        ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        /* Level 1 */
        ecu_tree_add_child_push_back(&tree1_, &node6_);

        /* Level 2 */
        ecu_tree_node_add_child_push_back(&node6_, &node3_);
        ecu_tree_node_add_child_push_back(&node6_, &node4_);
        ecu_tree_node_add_child_push_back(&node6_, &node5_);

        /* Level 3 */
        ecu_tree_node_add_child_push_back(&node3_, &node1_);
        ecu_tree_node_add_child_push_back(&node3_, &node2_);

        /* Steps 2 and 3: Action and assert. */
        ecu_tree_remove_node(&tree1_.root);

        /* Verify tree is the same post-removal of root. */
        for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&postorder_iterator_, &tree1_.root);
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




/* NOTE: Adding node in a SUBTREE to another tree is perfectly valid and what you want
in a tree API. */


// TEST(??, CannotRemoveNodeNotInTree)
// {

// }


/* Purposefully not done since no way of directly verifying by inspecting the tree
or tree node. Can only verify by checking if an assert fired but do not want to do that since that
is more implementation-defined. */
// TEST(CircularDLL, CannotRemoveNodeNotInList)
// {
// }











// // verify ECU_TREE_NODE_GET_ENTRY macro.
// TEST(??, TreeNodeGetUserData)
// {

// }


// TEST(GetLevel, NodesInDegenerateTree)
// {
//     // make degenerate tree 
//     // verify level of all nodes in tree is correct
// }


// TEST(GetLevel, NodesInMultiLevelTree)
// {
//     // make degenerate tree 
//     // verify level of all nodes in tree is correct
// }


// TEST(GetLevel, AddAndRemoveNodesInDegenerateTree)
// {
//     // make degenerate tree 
//     // add some nodes and test the level is correct.
//     // remove some nodes and verify level of removed nodes is 0.
//     // verify level of nodes that are still in tree are adjusted properly.
// }


// TEST(GetLevel, NodeNotInTree)
// {
//     // make degenerate tree
//     // Add node to tree. verify level is correct (whatever non-zero value you choose depending on tree structure)
//     // remove node from tree.
//     // verify level is now 0.
// }

/**
 * @brief Following tests are done:
 * 1. Random nodes within the tree.
 * 2. One node is the root of the tree. LCA should be the root.
 * 3. One node is the parent of another node. LCA should be the parent.
 * 4. One node is the grandparent of another node. LCA should be the grandparent.
 */
TEST(GetLCA, GenericTreeMultipleTestCases)
{
    /* Test tree:

                    root
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

        ecu_tree_ctor(&tree1_);
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

        /* Level 1 */
        ecu_tree_add_child_push_back(&tree1_, &node1_);
        ecu_tree_add_child_push_back(&tree1_, &node2_);

        /* Level 2 */
        ecu_tree_node_add_child_push_back(&node1_, &node3_);
        ecu_tree_node_add_child_push_back(&node1_, &node4_);
        ecu_tree_node_add_child_push_back(&node1_, &node5_);

        ecu_tree_node_add_child_push_back(&node2_, &node6_);

        /* Level 3 */
        ecu_tree_node_add_child_push_back(&node4_, &node7_);
        ecu_tree_node_add_child_push_back(&node5_, &node8_);
        ecu_tree_node_add_child_push_back(&node5_, &node9_);
        ecu_tree_node_add_child_push_back(&node5_, &node10_);

        ecu_tree_node_add_child_push_back(&node6_, &node11_);

        /* Level 4 */
        ecu_tree_node_add_child_push_back(&node9_, &node12_);

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL(&node1_, ecu_tree_get_lca(&node4_, &node12_));
        POINTERS_EQUAL(&node5_, ecu_tree_get_lca(&node8_, &node10_));
        POINTERS_EQUAL(&tree1_.root, ecu_tree_get_lca(&node3_, &node11_));
        POINTERS_EQUAL(&tree1_.root, ecu_tree_get_lca(&tree1_.root, &node12_));   /* One node is root. LCA should be root. */
        POINTERS_EQUAL(&node9_, ecu_tree_get_lca(&node9_, &node12_));           /* node9 is parent of node12. LCA should be node9 (the parent). */
        POINTERS_EQUAL(&node5_, ecu_tree_get_lca(&node5_, &node12_));           /* node5 is grandparent of node12. LCA should be node5 (the grandparent). */
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
TEST(GetLCA, TwoNodesThatAreTheSame)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));
        ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

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
TEST(GetLCA, TwoNodesInDifferentTrees)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

        ecu_tree_ctor(&tree1_);
        ecu_tree_ctor(&tree2_);
        ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
        ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

        ecu_tree_add_child_push_back(&tree1_, &node1_);
        ecu_tree_add_child_push_back(&tree2_, &node2_);

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL((struct ecu_tree_node *)0, ecu_tree_get_lca(&node1_, &node2_));
        POINTERS_EQUAL((struct ecu_tree_node *)0, ecu_tree_get_lca(&tree1_.root, &tree2_.root));
    }
    catch (AssertException& e)
    {
        (void)e;
        /* FAIL */
    }
}












// /**
//  * @brief Baseline test. Verify all nodes in tree are iterated through.
//  */
// TEST(PreOrderIterator, MultiLevelTree)
// {
//     /* Test tree: note that node number is the order in which it should be iterated.

//                     root
//                     |
//                     node1-------node3-------------------------------------------node11
//                     |           |                                               |
//                     node2       node4-------node5---------------node9           node12
//                                             |                   |
//                                             node6-----node8     node10
//                                             |
//                                             node7
//     */
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_tree_set_assert_functor(static_cast<struct ecu_assert_functor *>(&assert_call_fail_));

//         mock().strictOrder();
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &tree_.root);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node1_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node2_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node3_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node4_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node5_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node6_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node7_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node8_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node9_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node10_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node11_);
//         mock().expectOneCall("IteratorTreeNodeMock::verify_node")
//               .withParameter("node", &node12_);

//         ecu_tree_ctor(&tree_);
//         ecu_tree_node_ctor(&node1_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node2_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node3_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node4_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node5_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node6_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node7_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node8_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node9_,  (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node10_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node11_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);
//         ecu_tree_node_ctor(&node12_, (void (*)(struct ecu_tree_node *me))0, ECU_OBJECT_ID_UNUSED);

//         /* Level 1 */
//         ecu_tree_add_child_push_back(&tree_, &node1_);
//         ecu_tree_add_child_push_back(&tree_, &node3_);
//         ecu_tree_add_child_push_back(&tree_, &node11_);

//         /* Level 2 */
//         ecu_tree_node_add_child_push_back(&node1_, &node2_);

//         ecu_tree_node_add_child_push_back(&node3_, &node4_);
//         ecu_tree_node_add_child_push_back(&node3_, &node5_);
//         ecu_tree_node_add_child_push_back(&node3_, &node9_);

//         ecu_tree_node_add_child_push_back(&node11_, &node12_);

//         /* Level 3 */
//         ecu_tree_node_add_child_push_back(&node5_, &node6_);
//         ecu_tree_node_add_child_push_back(&node5_, &node8_);

//         ecu_tree_node_add_child_push_back(&node9_, &node10_);

//         /* Level 4 */
//         ecu_tree_node_add_child_push_back(&node6_, &node7_);

//         /* Steps 2 and 3: Action and assert. */
//         for (struct ecu_tree_node *i = ecu_tree_postorder_iterator_begin(&iterator_, &tree_.root);
//              i != ecu_tree_postorder_iterator_end(&iterator_);
//              i = ecu_tree_postorder_iterator_next(&iterator_))
//         {
//             IteratorTreeNodeMock::verify_node(i);
//         }
//     }
//     catch (AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }
