/**
 * @file
 * @brief Unit tests for public API functions in @ref ecu_dlist.h
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */


/*
TESTS ADDED AFTER REFACTORING
1. ECU_DNODE_GET_ENTRY(), ECU_DNODE_GET_CONST_ENTRY()
- DONE 

2. ecu_dnode_ctor(), ecu_dnode_destroy(), ecu_dlist_ctor(), ecu_dlist_destroy() 
DONE
- NodeDestroy: node's destroy callback called.
- ReconstructDestroyedNode: reconstructing destroyed node allows it to be readded to a list (used again). 
- ReconstructDestroyedList: reconstructing destroyed list makes it useable again. 

3. ecu_dnode_insert_before()
DONE
- InsertBeforeHead: can insert node before head. added node should be TAIL. 
- InsertBeforeTail: can insert node before tail. added node should be one before TAIL. 
- InsertBeforePositionNodeNotInList: can't insert if position node not in list. 
- InsertBeforeAddNodeInList: can't insert if node already in list. 
- InsertBeforeAddDestroyedNode: Can't insert destroyed node. 
- InsertBeforeAddHeadNode: Node can't be HEAD.

4. ecu_dnode_insert_after()
DONE
- InsertAfterHead: can insert node after head. added node should be one after HEAD. 
- InsertAfterTail: can insert node after TAIL. added node should be the new TAIL.
- InsertAfterPositionNodeNotInList: can't insert if position node not in list. 
- InsertAfterAddNodeInList: can't insert if node is already in list. 
- InsertAfterAddDestroyedNode: Can't insert destroyed node. 
- InsertAfterAddHeadNode: Node can't be HEAD.

5. ecu_dnode_remove()
DONE
- Remove node in middle of list. Verify list still intact.
- Can re-add a removed node. Remove node from listA and add node to listB. Verify with iterators.
- Can't remove node not in list. 
- Can't remove HEAD node. 

6. ecu_dnode_in_list()
DONE
- Just add and remove node from list and verify function returns true/false. 
- Returns true if HEAD is supplied.

7. ecu_dnode_get_id() 
DONE. Did trivial test mainly for code coverage.
- Dont think I need to test this..trivial.. 

12. ecu_dlist_clear()
DONE
- All nodes removed from list but nodes can be re-used. 
- Node destroy callbacks are NOT called.

8. ecu_dlist_push_front() 
DONE
- Adds node one after HEAD. 
- Can't add destroyed node. 
- Node can't be HEAD. 
- Node can't be in a list.

9. ecu_dlist_push_back() 
DONE
- Makes node the new TAIL. 
- Can't add destroyed node. 
- Node can't be HEAD. 
- Node can't be in a list. 

10. ecu_dlist_insert_before() 
DONE
- Verify node placed right before condition passes. Only make condition for one node pass. 
- Verify node is new TAIL if condition fails for all nodes. 
- Verify node is new TAIL if list is empty.
- Verify all nodes in linked list are passed to condition function (besides HEAD). Use mock to verify (struct ecu_dnode *current) 
  for each node iteration. 
- Have multiple condition functions pass. Verify node is added before the first condition pass ( 
  all other passed conditions do not matter).
- Node can't be HEAD. 
- Node can't be in a list. 

11. ecu_dlist_sort()
TODO
- List is sorted correctly. 

13. ecu_dlist_get_size()
- Just add/remove nodes and verify size is correct. 
TODO

14. ecu_dlist_is_empty()
TODO 
- Just add/remove nodes and verify function returns correct status.

15. non-const iterator 
TODO
- General iterator test. Verify iterator goes over all nodes in correct order. 
- Verify it is safe to remove nodes in middle of iteration. 
- Iterating over empty list is OK and immediately returns (doesn't go over any nodes).
- It is OK to use ecu_dnode_insert_after() and ecu_dnode_insert_before() in the middle of an iteration. 
    After adding, iterate over the list again and just verify nodes are in proper place and list still intact.

16. const iterator 
TODO
- General iterator test. Verify iterator goes over all nodes in correct order. 
*/

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Files under test. */
#include "ecu/dlist.h"

/* STDLib. */
#include <cassert>

/* Stubs. */
#include "stubs/stub_asserter.hpp"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*------------------------- NAMESPACES -----------------------*/
/*------------------------------------------------------------*/

using namespace stubs;

/*------------------------------------------------------------*/
/*----------------------- FILE-SCOPE TYPES -------------------*/
/*------------------------------------------------------------*/

struct test_node : public ecu_dnode
{
    test_node()
    {
        ecu_dnode_ctor(static_cast<ecu_dnode *>(this), ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    }

    test_node(int val) : m_val{val}
    {
        ecu_dnode_ctor(static_cast<ecu_dnode *>(this), ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    }

    void set_value(int val)
    {
        m_val = val;
    }

    int get_value() const
    {
        return m_val;
    }

    int m_val{0};
};

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Mock expectation to set in beginning of test. Call if 
 * node's destroy callback, @ref node_destroy_mock, is expected
 * to be called. Provides a more readable expectation and allows 
 * the string passed to expectOneCall() to be contained within this 
 * macro in case it changes.
 */
static void EXPECT_NODE_DESTROYED(ecu_dnode *node);

/**
 * @brief Mock expectation to set in beginning of test. Call
 * if node should still be in the specified list after the test 
 * completes. Verify via call to @ref list_order_check_expectations(). Provides a 
 * more readable expectation and allows the string passed to 
 * expectOneCall() to be contained within this macro in case it 
 * changes.
 */
static void EXPECT_NODE_IN_LIST(const ecu_dlist *list, const ecu_dnode *node);

/**
 * @brief Mock to assign to node's destroy callback.
 */
static void node_destroy_mock(ecu_dnode *node, ecu_object_id id);

/**
 * @brief Iterates over all nodes in a list and calls 
 * @ref node_in_list_mock() on each node. Provides a portable way
 * to verify nodes are in a list via dlist API.
 */
static void list_order_check_expectations(const ecu_dlist *list);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void EXPECT_NODE_DESTROYED(ecu_dnode *node)
{
    assert( (node) );
    mock().expectOneCall("node_destroy_mock")
          .withParameter("node", node);
}

static void EXPECT_NODE_IN_LIST(const ecu_dlist *list, const ecu_dnode *node)
{
    assert( (list && node) );
    mock().expectOneCall("node_in_list_mock")
          .withParameter("list", list)
          .withParameter("node", node);
}

static void node_destroy_mock(ecu_dnode *node, ecu_object_id id)
{
    (void)id;
    assert( (node) );
    mock().actualCall(__func__)
          .withParameter("node", node);
}

static void list_order_check_expectations(const ecu_dlist *list)
{
    assert( (list) );
    ecu_dlist_const_iterator citerator;

    for (const ecu_dnode *i = ecu_dlist_const_iterator_begin(&citerator, list);
         i != ecu_dlist_const_iterator_end(&citerator);
         i = ecu_dlist_const_iterator_next(&citerator))
    {
        mock().actualCall("node_in_list_mock")
              .withParameter("list", list)
              .withParameter("node", i);
    }
}

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(DListMacros)
{
    struct composite_node
    {
        std::uint32_t a{5};
        ecu_dnode node1;
        std::uint8_t b{10};
        ecu_dnode node2;
        std::uint16_t c{15};
    };

    composite_node m_node;
};

TEST_GROUP(DListDNodeCtors)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void setup_list(void (*destroy1)(ecu_dnode *, ecu_object_id) = ECU_DNODE_DESTROY_UNUSED,
                    void (*destroy2)(ecu_dnode *, ecu_object_id) = ECU_DNODE_DESTROY_UNUSED,
                    void (*destroy3)(ecu_dnode *, ecu_object_id) = ECU_DNODE_DESTROY_UNUSED)
    {
        ecu_dlist_ctor(&m_list);
        ecu_dnode_ctor(&m_node1, destroy1, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node2, destroy2, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node3, destroy3, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);
        ecu_dlist_push_back(&m_list, &m_node3);
    }

    ecu_dlist m_list;
    ecu_dnode m_node1;
    ecu_dnode m_node2;
    ecu_dnode m_node3;
};

TEST_GROUP(DNode)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);

        ecu_dlist_ctor(&m_list);
        ecu_dlist_ctor(&m_other_list);
        ecu_dnode_ctor(&m_node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_inserted_node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node_not_in_list, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

        /* HEAD, 1, 2, 3. Use dnode_insert() functions instead of push_back()
        since this module tests insert() functions. Avoids dependencies. */
        ecu_dnode_insert_after(&m_node1, &m_list.head);
        ecu_dnode_insert_after(&m_node2, &m_node1);
        ecu_dnode_insert_after(&m_node3, &m_node2);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    ecu_dlist m_list;
    ecu_dlist m_other_list;
    ecu_dnode m_node1;
    ecu_dnode m_node2;
    ecu_dnode m_node3;
    ecu_dnode m_inserted_node;
    ecu_dnode m_node_not_in_list;
};

TEST_GROUP(DList)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);

        ecu_dlist_ctor(&m_list);
        ecu_dlist_ctor(&m_other_list);
        ecu_dnode_ctor(&m_node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_inserted_node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node_not_in_list, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);
        ecu_dlist_push_back(&m_list, &m_node3);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    ecu_dlist m_list;
    ecu_dlist m_other_list;
    ecu_dnode m_node1;
    ecu_dnode m_node2;
    ecu_dnode m_node3;
    ecu_dnode m_inserted_node;
    ecu_dnode m_node_not_in_list;
};

TEST_GROUP(DListInsertBefore)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);

        ecu_dlist_ctor(&m_list);
        ecu_dlist_ctor(&m_other_list);
        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);
        ecu_dlist_push_back(&m_list, &m_node3);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    static void EXPECT_NODE_CONDITION(const ecu_dnode *node, const ecu_dnode *position, void *data)
    {
        mock().expectOneCall("condition_mock_false")
              .withParameter("node", node)
              .withParameter("position", position)
              .withParameter("data", data);
    }

    static bool condition(const ecu_dnode *node, const ecu_dnode *position, void *data)
    {
        assert( (node && position) );
        (void)data;
        const test_node *n = static_cast<const test_node *>(node);
        const test_node *p = static_cast<const test_node *>(position);

        if (n->m_val <= p->m_val)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool condition_stub_false(const ecu_dnode *node, const ecu_dnode *position, void *data)
    {
        (void)node;
        (void)position;
        (void)data;
        return false;
    }

    static bool condition_mock_false(const ecu_dnode *node, const ecu_dnode *position, void *data)
    {
        assert( (node && position && data) );
        mock().actualCall(__func__)
              .withParameter("node", node)
              .withParameter("position", position)
              .withParameter("data", data);
        return false;
    }

    ecu_dlist m_list;
    ecu_dlist m_other_list;
    test_node m_node1{1};
    test_node m_node2{2};
    test_node m_node3{3};
    test_node m_inserted_node;
    int m_data;
};

TEST_GROUP(DListSort)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
        ecu_dlist_ctor(&m_list);

        for (std::size_t i = 0; i < NUM_NODES; ++i)
        {
            m_node1[i].set_value(1);
            m_node2[i].set_value(2);
            m_node3[i].set_value(3);
            m_node4[i].set_value(4);
            m_node5[i].set_value(5);
            m_node6[i].set_value(6);
        }
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    static bool lhs_less_than_rhs(const ecu_dnode *lhs, const ecu_dnode *rhs, void *data)
    {
        assert( (lhs && rhs) );
        (void)data;
        const test_node *tlhs = static_cast<const test_node *>(lhs);
        const test_node *trhs = static_cast<const test_node *>(rhs);

        if (tlhs->get_value() < trhs->get_value())
        {
            return true;
        }
        else 
        {
            return false;
        }
    }

    static void EXPECT_NODE_VALUE(decltype(test_node::m_val) val)
    {
        mock().expectOneCall("node_value_mock")
              .withParameter("val", val);
    }

    void list_sort_check_expectations()
    {
        ecu_dlist_const_iterator citerator;

        for (const ecu_dnode *i = ecu_dlist_const_iterator_begin(&citerator, &m_list);
             i != ecu_dlist_const_iterator_end(&citerator);
             i = ecu_dlist_const_iterator_next(&citerator))
        {
            const test_node *n = static_cast<const test_node *>(i);
            mock().actualCall("node_value_mock")
                  .withParameter("val", n->get_value());
        }
    } 

    static constexpr std::size_t NUM_NODES = 3; /* Number of nodes with same value. */
    ecu_dlist m_list;
    test_node m_node1[NUM_NODES];
    test_node m_node2[NUM_NODES];
    test_node m_node3[NUM_NODES];
    test_node m_node4[NUM_NODES];
    test_node m_node5[NUM_NODES];
    test_node m_node6[NUM_NODES];
};

/*------------------------------------------------------------*/
/*----------------- TESTS - DLIST MACRO TESTS ----------------*/
/*------------------------------------------------------------*/

TEST(DListMacros, GetEntryRead)
{
    /* Step 2: Action. */
    composite_node *me = ECU_DNODE_GET_ENTRY(&m_node.node1, struct composite_node, node1);
    composite_node *me2 = ECU_DNODE_GET_ENTRY(&m_node.node2, struct composite_node, node2);

    /* Step 3: Assert. */
    UNSIGNED_LONGS_EQUAL(m_node.a, me->a);
    UNSIGNED_LONGS_EQUAL(m_node.b, me->b);
    UNSIGNED_LONGS_EQUAL(m_node.c, me->c);

    UNSIGNED_LONGS_EQUAL(m_node.a, me2->a);
    UNSIGNED_LONGS_EQUAL(m_node.b, me2->b);
    UNSIGNED_LONGS_EQUAL(m_node.c, me2->c);
}

TEST(DListMacros, GetEntryWrite)
{
    /* Step 1: Arrange. */
    static constexpr decltype(composite_node::a) a_val = 1;
    static constexpr decltype(composite_node::b) b_val = 2;
    static constexpr decltype(composite_node::c) c_val = 3;
    CHECK_TRUE( (m_node.a != a_val) );
    CHECK_TRUE( (m_node.b != b_val) );
    CHECK_TRUE( (m_node.c != c_val) );

    /* Step 2: Action. */
    composite_node *me = ECU_DNODE_GET_ENTRY(&m_node.node1, struct composite_node, node1);
    me->a = a_val;
    me->b = b_val;
    me->c = c_val;

    /* Step 3: Assert. */
    UNSIGNED_LONGS_EQUAL(a_val, m_node.a);
    UNSIGNED_LONGS_EQUAL(b_val, m_node.b);
    UNSIGNED_LONGS_EQUAL(c_val, m_node.c);
}

TEST(DListMacros, GetConstEntry)
{
    /* Step 2: Action. */
    const composite_node *me = ECU_DNODE_GET_CONST_ENTRY(&m_node.node1, struct composite_node, node1);
    const composite_node *me2 = ECU_DNODE_GET_CONST_ENTRY(&m_node.node2, struct composite_node, node2);

    /* Step 3: Assert. */
    UNSIGNED_LONGS_EQUAL(m_node.a, me->a);
    UNSIGNED_LONGS_EQUAL(m_node.b, me->b);
    UNSIGNED_LONGS_EQUAL(m_node.c, me->c);

    UNSIGNED_LONGS_EQUAL(m_node.a, me2->a);
    UNSIGNED_LONGS_EQUAL(m_node.b, me2->b);
    UNSIGNED_LONGS_EQUAL(m_node.c, me2->c);
}

/*------------------------------------------------------------*/
/*------------------- TESTS - CONSTRUCTORS -------------------*/
/*------------------------------------------------------------*/
/* NOTE: All tests require iterator to be working. */

/**
 * @brief Node's destroy callback executes and is
 * removed from the list.
 */
TEST(DListDNodeCtors, NodeDestroy)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list(&node_destroy_mock, &node_destroy_mock, &node_destroy_mock);
        EXPECT_NODE_DESTROYED(&m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 2: Action. */
        ecu_dnode_destroy(&m_node2);

        /* Step 3: Assert. Verify only node2 was removed from list. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Reconstructing a destroyed node makes it useable again.
 */
TEST(DListDNodeCtors, ReconstructDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        ecu_dnode_destroy(&m_node3);

        /* Step 2: Action. */
        ecu_dnode_ctor(&m_node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_push_back(&m_list, &m_node3);

        /* Step 3: Assert. Verify node3 can be used again and was re-added
        to the list. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief All node destroy callbacks execute. 
 */
TEST(DListDNodeCtors, ListDestroy)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list(&node_destroy_mock, &node_destroy_mock, &node_destroy_mock);
        EXPECT_NODE_DESTROYED(&m_node1);
        EXPECT_NODE_DESTROYED(&m_node2);
        EXPECT_NODE_DESTROYED(&m_node3);

        /* Step 2: Action. */
        ecu_dlist_destroy(&m_list);

        /* Step 3: Assert. Fails if all node destroy callbacks are not called. */
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Reconstructing a destroyed list makes it useable again.
 */
TEST(DListDNodeCtors, ReconstructDestroyedList)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        ecu_dlist_destroy(&m_list);

        /* Step 2: Action. */
        ecu_dlist_ctor(&m_list);
        ecu_dnode_ctor(&m_node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_push_back(&m_list, &m_node1);

        /* Step 3: Assert. Verify list and node1 can be used again. Node1
        should be re-added to the list. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*---------------- TESTS - DNODE INSERT BEFORE ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Can dnode_insert_before() HEAD node. Added node should be 
 * the new TAIL.
 */
TEST(DNode, InsertBeforeHead)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);

        /* Step 2: Action. */
        ecu_dnode_insert_before(&m_inserted_node, &m_list.head);

        /* Step 3: Assert. Verify m_inserted_node is at the end of the list. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief dnode_insert_before() TAIL node adds the node one 
 * position before the TAIL.
 */
TEST(DNode, InsertBeforeTail)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 2: Action. */
        ecu_dnode_insert_before(&m_inserted_node, &m_node3);

        /* Step 3: Assert. Verify m_inserted_node is before the tail (m_node3). */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Cannot dnode_insert_before() if position node is not
 * in a list.
 */
TEST(DNode, InsertBeforePositionNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dnode_insert_before(&m_inserted_node, &m_node_not_in_list);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Add m_inserted_node to list to verify m_node_not_in_list
    wasn't attached to m_inserted_node. Do this outside first try-catch. */
    try 
    {
        set_assert_handler(AssertResponse::FAIL);
        ecu_dnode_insert_before(&m_inserted_node, &m_list.head);
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Cannot dnode_insert_before() if added node is already in
 * another list.
 */
TEST(DNode, InsertBeforeAddNodeInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        ecu_dnode_insert_before(&m_inserted_node, &m_other_list.head);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert node from listA into listB. */
        ecu_dnode_insert_before(&m_node1, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
    be in m_list. Do this outside try-catch. */
    list_order_check_expectations(&m_list);
    list_order_check_expectations(&m_other_list);
}

/**
 * @brief A destroyed node cannot be supplied to dnode_insert_before().
 */
TEST(DNode, InsertBeforeAddDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert destroyed node. */
        ecu_dnode_destroy(&m_inserted_node);
        ecu_dnode_insert_before(&m_inserted_node, &m_node3);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to list. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/**
 * @brief A HEAD node cannot be supplied to dnode_insert_before().
 */
TEST(DNode, InsertBeforeAddHeadNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert listB HEAD to listA. */
        ecu_dnode_insert_before(&m_other_list.head, &m_node3);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*---------------- TESTS - DNODE INSERT AFTER ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Can dnode_insert_after() HEAD node. Added node should be 
 * one after HEAD.
 */
TEST(DNode, InsertAfterHead)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 2: Action. */
        ecu_dnode_insert_after(&m_inserted_node, &m_list.head);

        /* Step 3: Assert. Verify m_inserted_node is one after HEAD. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief dnode_insert_after() TAIL node makes the node the new TAIL.
 */
TEST(DNode, InsertAfterTail)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);

        /* Step 2: Action. */
        ecu_dnode_insert_after(&m_inserted_node, &m_node3);

        /* Step 3: Assert. Verify m_inserted_node is now the tail. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Cannot dnode_insert_after() if position node is not
 * in a list.
 */
TEST(DNode, InsertAfterPositionNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dnode_insert_after(&m_inserted_node, &m_node_not_in_list);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Add m_inserted_node to list to verify m_node_not_in_list
    wasn't attached to m_inserted_node. Do this outside first try-catch. */
    try 
    {
        set_assert_handler(AssertResponse::FAIL);
        ecu_dnode_insert_after(&m_inserted_node, &m_node3);
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Cannot dnode_insert_after() if added node is already in
 * another list.
 */
TEST(DNode, InsertAfterAddNodeInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        ecu_dnode_insert_after(&m_inserted_node, &m_other_list.head);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert node from listA into listB. */
        ecu_dnode_insert_after(&m_node1, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
    be in m_list. Do this outside try-catch. */
    list_order_check_expectations(&m_list);
    list_order_check_expectations(&m_other_list);
}

/**
 * @brief A destroyed node cannot be supplied to dnode_insert_after().
 */
TEST(DNode, InsertAfterAddDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert destroyed node. */
        ecu_dnode_destroy(&m_inserted_node);
        ecu_dnode_insert_after(&m_inserted_node, &m_node3);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to list. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/**
 * @brief A HEAD node cannot be supplied to dnode_insert_after().
 */
TEST(DNode, InsertAfterAddHeadNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert listB HEAD to listA. */
        ecu_dnode_insert_after(&m_other_list.head, &m_node3);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*-------------------- TESTS - DNODE REMOVE ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Remove node in the middle of a list. Verify list
 * still intact.
 */
TEST(DNode, Remove)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 2: Action. */
        ecu_dnode_remove(&m_node2);

        /* Step 3: Assert. Verify list intact. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief A removed node can be re-added to lists without
 * needing to be reconstructed.
 */
TEST(DNode, RemoveAndReAddNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_other_list, &m_node2);

        /* Step 2: Action. */
        ecu_dnode_remove(&m_node2);
        ecu_dnode_insert_after(&m_node2, &m_other_list.head);

        /* Step 3: Assert. Verify list intact. */
        list_order_check_expectations(&m_list);
        list_order_check_expectations(&m_other_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Intentionally commented out. No way to portably
 * test this without direct member access of mocking an
 * assert fired. 
 */
// TEST(DNode, RemoveNodeNotInList)
// {

// }

/**
 * @brief The reserved HEAD node can never be removed from
 * a list.
 */
TEST(DNode, RemoveHeadNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dnode_remove(&m_list.head);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify list intact. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*-------------------- TESTS - DNODE GET ID ------------------*/
/*------------------------------------------------------------*/

TEST(DNode, GetID)
{
    try
    {
        /* Step 1: Arrange. */
        static constexpr int TEST_OBJECT_ID = 2;
        ecu_dlist_destroy(&m_list);
        ecu_dnode_ctor(&m_node1, ECU_DNODE_DESTROY_UNUSED, TEST_OBJECT_ID);

        /* Steps 2 and 3: Action and assert. */
        LONGS_EQUAL(TEST_OBJECT_ID, ecu_dnode_get_id(&m_node1));
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DNODE IN LIST ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief General test verifying dnode_in_list().
 */
TEST(DNode, InList)
{
    try 
    {
        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_dnode_in_list(&m_inserted_node)) );
        ecu_dnode_insert_after(&m_inserted_node, &m_node3);
        CHECK_TRUE( (ecu_dnode_in_list(&m_inserted_node)) );
        ecu_dnode_remove(&m_inserted_node);
        CHECK_FALSE( (ecu_dnode_in_list(&m_inserted_node)) );
    }

    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief dnode_in_list() should return true if HEAD is supplied.
 */
TEST(DNode, InListHead)
{
    try 
    {
        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dnode_in_list(&m_list.head)) );
        CHECK_TRUE( (ecu_dnode_in_list(&m_other_list.head)) );
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - DLIST CLEAR ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief All nodes removed from list but their destructors
 * are not called. Removed nodes can be re-added without
 * having to be reconstructed.
 */
TEST(DList, Clear)
{
    try 
    {
        /* Step 1: Arrange. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_node1);
        
        /* Step 2: Action. */
        ecu_dlist_clear(&m_list);
        ecu_dlist_push_back(&m_other_list, &m_node1); /* Re-use removed node. */

        /* Step 3: Assert. Fails if any node's are destroyed or removed
        nodes can't be re-used. */
        list_order_check_expectations(&m_list);
        list_order_check_expectations(&m_other_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
};

/*------------------------------------------------------------*/
/*------------------ TESTS - DLIST PUSH FRONT ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief dlist_push_front() adds node one after HEAD.
 */
TEST(DList, PushFront)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 2: Action. */
        ecu_dlist_push_front(&m_list, &m_inserted_node);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief A node already in a list cannot be supplied to 
 * dlist_push_front().
 */
TEST(DList, PushFrontAddNodeInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        ecu_dlist_push_front(&m_other_list, &m_inserted_node);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert node from listA into listB. */
        ecu_dlist_push_front(&m_other_list, &m_node1);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
    be in m_list. Do this outside try-catch. */
    list_order_check_expectations(&m_list);
    list_order_check_expectations(&m_other_list);
}

/**
 * @brief A destroyed node cannot be supplied to dlist_push_front().
 */
TEST(DList, PushFrontAddDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert destroyed node. */
        ecu_dnode_destroy(&m_inserted_node);
        ecu_dlist_push_front(&m_list, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to list. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/**
 * @brief A head node cannot be supplied to dlist_push_front().
 */
TEST(DList, PushFrontAddHeadNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert listB HEAD to listA. */
        ecu_dlist_push_front(&m_list, &m_other_list.head);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*------------------ TESTS - DLIST PUSH BACK -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief dlist_push_back() adds node to back of list.
 */
TEST(DList, PushBack)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);

        /* Step 2: Action. */
        ecu_dlist_push_back(&m_list, &m_inserted_node);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief A node already in a list cannot be supplied to 
 * dlist_push_back().
 */
TEST(DList, PushBackAddNodeInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        ecu_dlist_push_back(&m_other_list, &m_inserted_node);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert node from listA into listB. */
        ecu_dlist_push_back(&m_other_list, &m_node1);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
    be in m_list. Do this outside try-catch. */
    list_order_check_expectations(&m_list);
    list_order_check_expectations(&m_other_list);
}

/**
 * @brief A destroyed node cannot be supplied to dlist_push_back().
 */
TEST(DList, PushBackAddDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert destroyed node. */
        ecu_dnode_destroy(&m_inserted_node);
        ecu_dlist_push_back(&m_list, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to list. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/**
 * @brief A head node cannot be supplied to dlist_push_back().
 */
TEST(DList, PushBackAddHeadNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert listB HEAD to listA. */
        ecu_dlist_push_back(&m_list, &m_other_list.head);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*----------------- TESTS - DLIST INSERT BEFORE --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Node added to correct position in middle of list 
 * if a middle condition passes.
 */
TEST(DListInsertBefore, MiddleConditionPasses)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        m_inserted_node.set_value(2);

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition, nullptr);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Node is added to front of list if first condition passes.
 */
TEST(DListInsertBefore, FirstConditionPasses)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        m_inserted_node.set_value(1);

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition, nullptr);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Node added to end of list if all conditions are false.
 */
TEST(DListInsertBefore, AllConditionsFalse)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition_stub_false, nullptr);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Verify all nodes in list are iterated over when 
 * evaluating condition. A condition function that always
 * returns false is used to test this since it is OK for 
 * dlist_insert_before() to exit as soon as condition evaluates 
 * to true.
 */
TEST(DListInsertBefore, ConditionParameters)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_CONDITION(&m_inserted_node, &m_node1, &m_data);
        EXPECT_NODE_CONDITION(&m_inserted_node, &m_node2, &m_data);
        EXPECT_NODE_CONDITION(&m_inserted_node, &m_node3, &m_data);

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition_mock_false, &m_data);

        /* Step 3: Assert. Fails if condition function not called on all nodes. */
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Node added to end of list if list is empty. Condition 
 * function should never be called since list is empty.
 */
TEST(DListInsertBefore, EmptyList)
{
    try 
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_other_list, &m_inserted_node, &condition_mock_false, nullptr);

        /* Step 3: Assert. Node added to end of empty list. Also fails if 
        condition function ever called. */
        list_order_check_expectations(&m_other_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief A node already in a list cannot be supplied to 
 * dlist_insert_before().
 */
TEST(DListInsertBefore, AddNodeInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        ecu_dlist_push_back(&m_other_list, &m_inserted_node);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert node from listA into listB. */
        ecu_dlist_insert_before(&m_other_list, &m_node1, &condition_stub_false, nullptr);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
    be in m_list. Do this outside try-catch. */
    list_order_check_expectations(&m_list);
    list_order_check_expectations(&m_other_list);
}

/**
 * @brief A destroyed node cannot be supplied to dlist_insert_before().
 */
TEST(DListInsertBefore, AddDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert destroyed node. */
        ecu_dnode_destroy(&m_inserted_node);
        ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition_stub_false, nullptr);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to list. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/**
 * @brief A head node cannot be supplied to dlist_insert_before().
 */
TEST(DListInsertBefore, AddHeadNode)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert listB HEAD to listA. */
        ecu_dlist_insert_before(&m_list, &m_other_list.head, &condition_stub_false, nullptr);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - DLIST SORT -------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Each node in the test list has a unique value. 
 * Number of list elements is an even number. Verify list 
 * sorted correctly. 
 */
TEST(DListSort, UniqueSortEven)
{
    try 
    {
        /* Step 1: Arrange. List = 6, 2, 4, 1, 3, 5 */
        ecu_dlist_push_back(&m_list, &m_node6[0]);
        ecu_dlist_push_back(&m_list, &m_node2[0]);
        ecu_dlist_push_back(&m_list, &m_node4[0]);
        ecu_dlist_push_back(&m_list, &m_node1[0]);
        ecu_dlist_push_back(&m_list, &m_node3[0]);
        ecu_dlist_push_back(&m_list, &m_node5[0]);

        mock().strictOrder();
        EXPECT_NODE_VALUE(1);
        EXPECT_NODE_VALUE(2);
        EXPECT_NODE_VALUE(3);
        EXPECT_NODE_VALUE(4);
        EXPECT_NODE_VALUE(5);
        EXPECT_NODE_VALUE(6);

        /* Step 2: Action. */
        ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

        /* Step 3: Assert. Verify list correctly sorted. */
        list_sort_check_expectations();
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Each node in the test list has a unique value. 
 * Number of list elements is an odd number. Verify list 
 * sorted correctly. 
 */
TEST(DListSort, UniqueSortOdd)
{
    try 
    {
        /* Step 1: Arrange. List = 2, 4, 5, 1, 6 */
        ecu_dlist_push_back(&m_list, &m_node2[0]);
        ecu_dlist_push_back(&m_list, &m_node4[0]);
        ecu_dlist_push_back(&m_list, &m_node5[0]);
        ecu_dlist_push_back(&m_list, &m_node1[0]);
        ecu_dlist_push_back(&m_list, &m_node6[0]);

        mock().strictOrder();
        EXPECT_NODE_VALUE(1);
        EXPECT_NODE_VALUE(2);
        EXPECT_NODE_VALUE(4);
        EXPECT_NODE_VALUE(5);
        EXPECT_NODE_VALUE(6);

        /* Step 2: Action. */
        ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

        /* Step 3: Assert. Verify list correctly sorted. */
        list_sort_check_expectations();
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Nodes in test list can have the same values.
 * Number of list elements is an even number. Verify list 
 * sorted correctly. 
 */
TEST(DListSort, NonUniqueSortEven)
{
    try 
    {
        /* Step 1: Arrange. List = 5, 1, 2, 3, 6, 5, 1, 4, 2, 6 */
        ecu_dlist_push_back(&m_list, &m_node5[0]);
        ecu_dlist_push_back(&m_list, &m_node1[0]);
        ecu_dlist_push_back(&m_list, &m_node2[0]);
        ecu_dlist_push_back(&m_list, &m_node3[0]);
        ecu_dlist_push_back(&m_list, &m_node6[0]);
        ecu_dlist_push_back(&m_list, &m_node5[1]);
        ecu_dlist_push_back(&m_list, &m_node1[1]);
        ecu_dlist_push_back(&m_list, &m_node4[0]);
        ecu_dlist_push_back(&m_list, &m_node2[1]);
        ecu_dlist_push_back(&m_list, &m_node6[1]);

        mock().strictOrder();
        EXPECT_NODE_VALUE(1);
        EXPECT_NODE_VALUE(1);
        EXPECT_NODE_VALUE(2);
        EXPECT_NODE_VALUE(2);
        EXPECT_NODE_VALUE(3);
        EXPECT_NODE_VALUE(4);
        EXPECT_NODE_VALUE(5);
        EXPECT_NODE_VALUE(5);
        EXPECT_NODE_VALUE(6);
        EXPECT_NODE_VALUE(6);

        /* Step 2: Action. */
        ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

        /* Step 3: Assert. Verify list correctly sorted. */
        list_sort_check_expectations();
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Nodes in test list can have the same values.
 * Number of list elements is an odd number. Verify list 
 * sorted correctly. 
 */
TEST(DListSort, NonUniqueSortOdd)
{
    try 
    {
        /* Step 1: Arrange. List = 1, 5, 3, 4, 5, 5, 6, 1, 4, 2, 6 */
        ecu_dlist_push_back(&m_list, &m_node1[0]);
        ecu_dlist_push_back(&m_list, &m_node5[0]);
        ecu_dlist_push_back(&m_list, &m_node3[0]);
        ecu_dlist_push_back(&m_list, &m_node4[0]);
        ecu_dlist_push_back(&m_list, &m_node5[1]);
        ecu_dlist_push_back(&m_list, &m_node5[2]);
        ecu_dlist_push_back(&m_list, &m_node6[0]);
        ecu_dlist_push_back(&m_list, &m_node1[1]);
        ecu_dlist_push_back(&m_list, &m_node4[1]);
        ecu_dlist_push_back(&m_list, &m_node2[0]);
        ecu_dlist_push_back(&m_list, &m_node6[1]);

        mock().strictOrder();
        EXPECT_NODE_VALUE(1);
        EXPECT_NODE_VALUE(1);
        EXPECT_NODE_VALUE(2);
        EXPECT_NODE_VALUE(3);
        EXPECT_NODE_VALUE(4);
        EXPECT_NODE_VALUE(4);
        EXPECT_NODE_VALUE(5);
        EXPECT_NODE_VALUE(5);
        EXPECT_NODE_VALUE(5);
        EXPECT_NODE_VALUE(6);
        EXPECT_NODE_VALUE(6);

        /* Step 2: Action. */
        ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

        /* Step 3: Assert. Verify list correctly sorted. */
        list_sort_check_expectations();
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}





// 1. push_front() adds node in front of list.
// 2. Cannot push_front() where node is in another list.
// 3. Cannot push_front() where node is HEAD of this list.
// 4. Cannot push_front() where node is HEAD of another list.
// 5. Cannot push_front() node with invalid ID.
// 6 to 10. Repeat same tests for push_back().


// remove() does not change dnode's destroy function or ID.
// Cannot remove() HEAD.
// Cannot remove() node not in a list.

// 4. Test get_size() by calling push_front(), push_back(), dnode_insert_before(), dnode_insert_after() and remove(). general insert and removal test.
// 15. Cannot remove() node not in list.
// 16. Cannot remove() node in another list.
// 19. General iterator test. Verify iterator goes over all nodes in correct order. (set ids, mock strict order).
// 20. Using push_front() in middle of iteration is OK. These nodes WON'T be iterated over.
// 21. Using push_back() in middle of iteration is OK. These nodes WILL be iterated over.
// 22. Using dnode_insert_before() in middle of iteration is OK. These nodes WON'T be iterated over.
// 23. Using dnode_insert_after() in middle of iteration is OK. These nodes WON'T be itreated over.
// 24. Using remove() in middle of iteration is OK if it is on nodes BEFORE and AT current.

// 25. Using remove() in middle of iteration on nodes AFTER current is NOT ok.
// 26. Iterating over empty list is ok. verify begin() == end() == next()
// 27. General is_empty() test. Can just do one call of push_front() and remove(). Don't overcomplicate it.
// */



// TEST(??, EcuDListGetEntryMacro)
// {

// }


// TEST(??, ConstructorDestructorTest)
// {

// }

// TEST(??, NodeDestructorCallbacks)
// {

// }

// // general test for insert(), remove(), push_back(), push_front(), and get_size().
// TEST(??, GetSize)
// {

// }

// TEST(??, NodeIDsAvailableInDestructor)
// {

// }

// TEST(??, InsertAddsNodeBeforePosition)
// {

// }

// // cant insert node that is in another list.
// TEST(??, CannotInsertNodeInAnotherList)
// {

// }

// TEST(??, CannotInsertNodeAlreadyInThisList)
// {

// }

// // cannnot insert node when position is a node from another list.
// TEST(??, CannotInsertNodeToAnotherList)
// {

// }

// TEST(??, CannotRemoveNodeNotInList)
// {

// }

// TEST(??, CannotRemoveNodeFromAnotherList)
// {

// }

// // push front adds node to front of list.
// TEST(??, PushFront)
// {

// }

// // push back adds node to back of list.
// TEST(??, PushBack)
// {

// }

// // verify iterator goes over all nodes
// TEST(??, GeneralIteratorTest)
// {

// }

// // it is OK to do push front. These nodes WONT be iterated over.
// TEST(??, PushFrontDuringIteration)
// {

// }

// // it is OK to do push back.
// TEST(??, PushBackDuringIteration)
// {

// }


























// /**
//  * @brief Construct list and nodes, add nodes to list, and call list destructor.
//  * Repeat process multiple times in a row and verify no assert fires since this
//  * should be defined behavior.
//  */
// TEST(CircularDLL, ConstructorDestructorTest)
// {
//     try 
//     {
//         /* Step 1: Arrange. Done in setup(). */

//         /* Steps 2 and 3: Action and assert. */
//         /* First sequence. */
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//         ecu_circular_dll_destroy(&list_);

//         /* Second sequence. */
//         ecu_circular_dll_ctor(&list_);
//         ecu_circular_dll_node_ctor(&node1_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(&node2_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(&node3_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//         ecu_circular_dll_destroy(&list_);

//         /* Third sequence. */
//         ecu_circular_dll_ctor(&list_);
//         ecu_circular_dll_node_ctor(&node1_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(&node2_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(&node3_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//         ecu_circular_dll_destroy(&list_);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


// TEST(CircularDLL, AllNodeDestructorCallbacksCalled)
// {
//     struct NodeDestroyMock dnode1_;
//     struct NodeDestroyMock dnode2_;
//     struct NodeDestroyMock dnode3_;

//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().expectOneCall("NodeDestroyMock::destroy")
//               .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode1_));

//         mock().expectOneCall("NodeDestroyMock::destroy")
//               .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode2_));

//         mock().expectOneCall("NodeDestroyMock::destroy")
//               .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode3_));   

//         ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode1_), 
//                                    &NodeDestroyMock::destroy,
//                                    ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode2_), 
//                                    &NodeDestroyMock::destroy,
//                                    ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode3_), 
//                                    &NodeDestroyMock::destroy,
//                                    ECU_OBJECT_ID_UNUSED);

//         ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode1_));
//         ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode2_));
//         ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode3_));

//         /* Steps 2 and 3: Action and assert. */
//         ecu_circular_dll_destroy(&list_);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


// /**
//  * @brief Some nodes have destroy callbacks and other nodes have NULL node 
//  * destroy callbacks. Verify this is OK.
//  */
// TEST(CircularDLL, NodeDestroyNullCallbacks)
// {
//     struct NodeDestroyMock dnode2_;

//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().expectOneCall("NodeDestroyMock::destroy")
//               .onObject(static_cast<struct ecu_circular_dll_node *>(&dnode2_));

//         ecu_circular_dll_node_ctor(&node1_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(static_cast<struct ecu_circular_dll_node *>(&dnode2_), 
//                                    &NodeDestroyMock::destroy,
//                                    ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(&node3_.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);

//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, static_cast<struct ecu_circular_dll_node *>(&dnode2_));
//         ecu_circular_dll_push_back(&list_, &node3_.node);

//         /* Steps 2 and 3: Action and assert. */
//         ecu_circular_dll_destroy(&list_);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL */
//     }
// }


// /**
//  * @brief Verify nodes are in correct order by directly comparing 
//  * node pointer to elements returned by iterator.
//  */
// TEST(CircularDLL, NodeAdditionAndRemoval)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         (void)ecu_circular_dll_iterator_begin(&iterator_, &list_); /* Initialize iterator at beginning so any iterator call can be safely used. */

//         /* Steps 2 and 3: Action and assert. */
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_begin(&iterator_, &list_));

//         /* [1] */
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//         /* [1, 2] */
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//         /* [1, 2, 3] */
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//         POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//         /* [1, 3] */
//         ecu_circular_dll_remove_node(&node2_.node);
//         POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//         /* [3] */
//         ecu_circular_dll_remove_node(&node1_.node);
//         POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
        
//         /* [] */
//         ecu_circular_dll_remove_node(&node3_.node);
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_begin(&iterator_, &list_));

//         /* [2] */
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//         /* [] */
//         ecu_circular_dll_remove_node(&node2_.node);
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_begin(&iterator_, &list_));
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL. */
//     }
// }


// TEST(CircularDLL, CannotAddNodeAlreadyInSameList)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         stubs::set_assert_handler(stubs::AssertResponse::OK);
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);

//         /* Step 2: Action. */
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* OK. */
//     }

//     /* Step 3: Assert. */
//     CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));
//     POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//     POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
//     POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
// }


// TEST(CircularDLL, CannotAddNodeFromAnotherList)
// {
//     struct ecu_circular_dll extra_list;

//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_circular_dll_ctor(&extra_list);
//         stubs::set_assert_handler(stubs::AssertResponse::OK);
        
//         /* list_ = [1, 2]. extra_list = [3] */
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&extra_list, &node3_.node);

//         /* Step 2: Action. */
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* OK. */
//     }

//     /* Step 3: Assert. */
//     /* list_ = [1, 2] */
//     CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));
//     POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//     POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
//     POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//     /* extra_list = [3] */
//     CHECK_EQUAL(1, ecu_circular_dll_get_size(&extra_list));
//     POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&iterator_, &extra_list));
//     POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));
// }


// /* Purposefully not done since no way of directly verifying by inspecting the list.
// Can only verify by checking if an assert fired but do not want to do that since that
// is more implementation-defined. */
// // TEST(CircularDLL, CannotRemoveNodeNotInList)
// // {
// // }


// TEST(CircularDLL, CorrectSizeReturned)
// {
//     try 
//     {
//         /* Step 1: Arrange. Done in setup(). */
        
//         /* Steps 2 and 3: Action and assert. */
//         CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));

//         /* [1] */
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));

//         /* [1, 2] */
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));

//         /* [1, 2, 3] */
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//         CHECK_EQUAL(3, ecu_circular_dll_get_size(&list_));

//         /* [1, 3] */
//         ecu_circular_dll_remove_node(&node2_.node);
//         CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));

//         /* [3] */
//         ecu_circular_dll_remove_node(&node1_.node);
//         CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));
        
//         /* [] */
//         ecu_circular_dll_remove_node(&node3_.node);
//         CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));

//         /* [2] */
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         CHECK_EQUAL(1, ecu_circular_dll_get_size(&list_));

//         /* [] */
//         ecu_circular_dll_remove_node(&node2_.node);
//         CHECK_EQUAL(0, ecu_circular_dll_get_size(&list_));
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL. */
//     }
// }


// /**
//  * @brief Test function returning true means list is empty.
//  */
// TEST(CircularDLL, IsEmpty)
// {
//     try 
//     {
//         /* Step 1: Arrange. Done in setup(). */

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE(ecu_circular_dll_is_empty(&list_));

//         /* [1] */
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         CHECK_FALSE(ecu_circular_dll_is_empty(&list_));

//         /* [1, 2] */
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         CHECK_FALSE(ecu_circular_dll_is_empty(&list_));

//         /* [2] */
//         ecu_circular_dll_remove_node(&node1_.node);
//         CHECK_FALSE(ecu_circular_dll_is_empty(&list_));

//         /* [] */
//         ecu_circular_dll_remove_node(&node2_.node);
//         CHECK_TRUE(ecu_circular_dll_is_empty(&list_));
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL. */
//     }
// }


// /**
//  * @brief Edit data of nodes through iterator. Verify all data was
//  * changed, showing we can iterate over the entire list.
//  */
// TEST(CircularDLL, IterateOverListAndEditAllNodes)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         node1_.x = 5;
//         node1_.y = 5;
//         node2_.x = 5;
//         node2_.y = 5;
//         node3_.x = 5;
//         node3_.y = 5;
//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&list_, &node3_.node);

//         /* Step 2: Action. */
//         for (struct ecu_circular_dll_node *i = ecu_circular_dll_iterator_begin(&iterator_, &list_);
//              i != ecu_circular_dll_iterator_end(&iterator_);
//              i = ecu_circular_dll_iterator_next(&iterator_))
//         {
//             struct user_data *data = ECU_CIRCULAR_DLL_GET_ENTRY(i, struct user_data, node);
//             data->x = 10;
//             data->y = 10;
//         }

//         /* Step 3: Assert. */
//         CHECK_EQUAL(10, node1_.x);
//         CHECK_EQUAL(10, node1_.y);
//         CHECK_EQUAL(10, node2_.x);
//         CHECK_EQUAL(10, node2_.y);
//         CHECK_EQUAL(10, node3_.x);
//         CHECK_EQUAL(10, node3_.y);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL. */
//     }
// }


// /**
//  * @brief Only edit data of nodes we aren't removing. Verify correct
//  * nodes removed and their data was not edited, showing we can safely
//  * iterate over the list while removing nodes.
//  */
// TEST(CircularDLL, IterateOverListAndRemoveSomeNodes)
// {
//     struct user_data node4;
//     struct user_data node5;

//     try
//     {
//         /* Step 1: Arrange. */
//         node1_.x = 5;
//         node1_.y = 5;
//         node2_.x = 5;
//         node2_.y = 5;
//         node3_.x = 5;
//         node3_.y = 5;
//         node4.x = 5;
//         node4.y = 5;
//         node5.x = 5;
//         node5.y = 5;

//         ecu_circular_dll_node_ctor(&node4.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(&node5.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);

//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&list_, &node3_.node);
//         ecu_circular_dll_push_back(&list_, &node4.node);
//         ecu_circular_dll_push_back(&list_, &node5.node);

//         /* Step 2: Action. */
//         for (struct ecu_circular_dll_node *i = ecu_circular_dll_iterator_begin(&iterator_, &list_);
//              i != ecu_circular_dll_iterator_end(&iterator_);
//              i = ecu_circular_dll_iterator_next(&iterator_))
//         {
//             /* Selectively remove some nodes. 1 and 5 chosen since this is the start and end of list. */
//             if (i == &node1_.node || i == &node2_.node || i == &node5.node)
//             {
//                 ecu_circular_dll_remove_node(i);
//             }
//             else
//             {
//                 struct user_data *data = ECU_CIRCULAR_DLL_GET_ENTRY(i, struct user_data, node);
//                 data->x = 10;
//                 data->y = 10;
//             }
//         }

//         /* Step 3: Assert. */
//         /* [3, 4] */
//         CHECK_EQUAL(2, ecu_circular_dll_get_size(&list_));
//         POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(&node4.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//         CHECK_EQUAL(5, node1_.x);
//         CHECK_EQUAL(5, node1_.y);
//         CHECK_EQUAL(5, node2_.x);
//         CHECK_EQUAL(5, node2_.y);
//         CHECK_EQUAL(10, node3_.x);
//         CHECK_EQUAL(10, node3_.y);
//         CHECK_EQUAL(10, node4.x);
//         CHECK_EQUAL(10, node4.y);
//         CHECK_EQUAL(5, node5.x);
//         CHECK_EQUAL(5, node5.y);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL. */
//     }
// }


// /**
//  * @brief Iterator also goes through nodes that were added to
//  * end of list via @ref ecu_circular_dll_push_back().
//  */
// TEST(CircularDLL, AddNodesInIteratorPushBack)
// {
//     struct user_data node4;
//     struct user_data node5;

//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_circular_dll_node_ctor(&node4.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);
//         ecu_circular_dll_node_ctor(&node5.node, (void (*)(struct ecu_circular_dll_node *))0, ECU_OBJECT_ID_UNUSED);

//         node1_.x = 5;
//         node1_.y = 5;
//         node2_.x = 5;
//         node2_.y = 5;
//         node3_.x = 5;
//         node3_.y = 5;
//         node4.x = 5;
//         node4.y = 5;
//         node5.x = 5;
//         node5.y = 5;

//         ecu_circular_dll_push_back(&list_, &node1_.node);
//         ecu_circular_dll_push_back(&list_, &node2_.node);
//         ecu_circular_dll_push_back(&list_, &node3_.node);

//         /* Step 2: Action. */
//         for (struct ecu_circular_dll_node *i = ecu_circular_dll_iterator_begin(&iterator_, &list_);
//              i != ecu_circular_dll_iterator_end(&iterator_);
//              i = ecu_circular_dll_iterator_next(&iterator_))
//         {
//             /* [1, 2, 3, 4, 5] */
//             if (i == &node2_.node)
//             {
//                 ecu_circular_dll_push_back(&list_, &node4.node);
//                 ecu_circular_dll_push_back(&list_, &node5.node);
//             }
//             struct user_data *data = ECU_CIRCULAR_DLL_GET_ENTRY(i, struct user_data, node);
//             data->x = 10;
//             data->y = 10;
//         }

//         /* Step 3: Assert. */
//         CHECK_EQUAL(5, ecu_circular_dll_get_size(&list_));
//         POINTERS_EQUAL(&node1_.node, ecu_circular_dll_iterator_begin(&iterator_, &list_));
//         POINTERS_EQUAL(&node2_.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(&node3_.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(&node4.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(&node5.node, ecu_circular_dll_iterator_next(&iterator_));
//         POINTERS_EQUAL(ecu_circular_dll_iterator_end(&iterator_), ecu_circular_dll_iterator_next(&iterator_));

//         CHECK_EQUAL(10, node1_.x);
//         CHECK_EQUAL(10, node1_.y);
//         CHECK_EQUAL(10, node2_.x);
//         CHECK_EQUAL(10, node2_.y);
//         CHECK_EQUAL(10, node3_.x);
//         CHECK_EQUAL(10, node3_.y);
//         CHECK_EQUAL(10, node4.x);
//         CHECK_EQUAL(10, node4.y);
//         CHECK_EQUAL(10, node5.x);
//         CHECK_EQUAL(10, node5.y);
//     }
//     catch (stubs::AssertException& e)
//     {
//         (void)e;
//         /* FAIL. */
//     }
// }
