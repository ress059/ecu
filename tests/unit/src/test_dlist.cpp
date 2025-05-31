/**
 * @file
 * @brief Unit tests for public API functions in @ref dlist.h.
 * Test Summary:
 * 
 * @ref ECU_DNODE_GET_ENTRY() and @ref ECU_DNODE_GET_CONST_ENTRY()
 *      - TEST(DListMacros, GetEntryRead)
 *      - TEST(DListMacros, GetEntryWrite)
 *      - TEST(DListMacros, GetConstEntry)
 * 
 * @ref ecu_dnode_ctor(), @ref ecu_dnode_destroy(), @ref ecu_dlist_ctor(), @ref ecu_dlist_destroy()
 *      - TEST(DListDNodeCtors, NodeDestroy)
 *      - TEST(DListDNodeCtors, ReconstructDestroyedNode)
 *      - TEST(DListDNodeCtors, ListDestroy)
 *      - TEST(DListDNodeCtors, ReconstructDestroyedList)
 * 
 * @ref ecu_dnode_insert_before()
 *      - TEST(DNode, InsertBeforeHead)
 *      - TEST(DNode, InsertBeforeTail)
 *      - TEST(DNode, InsertBeforePositionNodeNotInList)
 *      - TEST(DNode, InsertBeforeAddNodeInList)
 *      - TEST(DNode, InsertBeforeAddDestroyedNode)
 *      - TEST(DNode, InsertBeforeAddHeadNode)
 * 
 * @ref ecu_dnode_insert_after()
 *      - TEST(DNode, InsertAfterHead)
 *      - TEST(DNode, InsertAfterTail)
 *      - TEST(DNode, InsertAfterPositionNodeNotInList)
 *      - TEST(DNode, InsertAfterAddNodeInList)
 *      - TEST(DNode, InsertAfterAddDestroyedNode)
 *      - TEST(DNode, InsertAfterAddHeadNode)
 * 
 * @ref ecu_dnode_remove()
 *      - TEST(DNode, Remove)
 *      - TEST(DNode, RemoveAndReAddNode)
 *      - TEST(DNode, RemoveHeadNode)
 * 
 * @ref ecu_dnode_in_list()
 *      - TEST(DNode, InList)
 *      - TEST(DNode, InListHead)
 * 
 * @ref ecu_dnode_get_id()
 *      - TEST(DNode, GetID)
 * 
 * @ref ecu_dlist_clear()
 *      - TEST(DListBase, ClearEmptyList)
 *      - TEST(DList, Clear)
 * 
 * @ref ecu_dlist_front(), @ref ecu_dlist_cfront()
 *      - TEST(DList, FrontEmptyList)
 *      - TEST(DList, FrontNonEmptyList)
 * 
 * @ref ecu_dlist_push_front()
 *      - TEST(DList, PushFront)
 *      - TEST(DList, PushFrontAddNodeInList)
 *      - TEST(DList, PushFrontAddDestroyedNode)
 *      - TEST(DList, PushFrontAddHeadNode)
 * 
 * @ref ecu_dlist_pop_front()
 *      - TEST(DList, PopFrontEmptyList)
 *      - TEST(DList, PopFrontListWithOneNode)
 *      - TEST(DList, PopFrontListWithMultipleNodes)
 * 
 * @ref ecu_dlist_back(), ecu_dlist_cback()
 *      - TEST(DList, BackEmptyList)
 *      - TEST(DList, BackNonEmptyList)
 * 
 * @ref ecu_dlist_push_back()
 *      - TEST(DList, PushBack)
 *      - TEST(DList, PushBackAddNodeInList)
 *      - TEST(DList, PushBackAddDestroyedNode)
 *      - TEST(DList, PushBackAddHeadNode)
 * 
 * @ref ecu_dlist_pop_back()
 *      - TEST(DList, PopBackEmptyList)
 *      - TEST(DList, PopBackListWithOneNode)
 *      - TEST(DList, PopBackListWithMultipleNodes)
 * 
 * @ref ecu_dlist_insert_before()
 *      - TEST(DListInsertBefore, MiddleConditionPasses)
 *      - TEST(DListInsertBefore, FirstConditionPasses)
 *      - TEST(DListInsertBefore, AllConditionsFalse)
 *      - TEST(DListInsertBefore, ConditionParameters)
 *      - TEST(DListInsertBefore, EmptyList)
 *      - TEST(DListInsertBefore, AddNodeInList)
 *      - TEST(DListInsertBefore, AddDestroyedNode)
 *      - TEST(DListInsertBefore, AddHeadNode)
 * 
 * @ref ecu_dlist_sort()
 *      - TEST(DListSort, UniqueSortEven)
 *      - TEST(DListSort, UniqueSortOdd)
 *      - TEST(DListSort, NonUniqueSortEven)
 *      - TEST(DListSort, NonUniqueSortOdd)
 * 
 * @ref ecu_dlist_swap()
 *      - TEST(DListBase, SwapBothListsNonEmpty)
 *      - TEST(DListBase, SwapMeListEmptyOtherListNonEmpty)
 *      - TEST(DListBase, SwapMeListNonEmptyOtherListEmpty)
 *      - TEST(DListBase, SwapBothListsEmpty)
 *      - TEST(DListBase, SwapSameListsSupplied)
 * 
 * @ref ecu_dlist_size()
 *      - TEST(DListBase, SizeGeneralTest)
 * 
 * @ref ecu_dlist_empty()
 *      - TEST(DListBase, EmptyGeneralTest)
 * 
 * @ref ECU_DLIST_FOR_EACH(), @ref ECU_DLIST_CONST_FOR_EACH(), @ref ecu_dlist_iterator_begin(),
 * @ref ecu_dlist_iterator_end(), @ref ecu_dlist_iterator_next(), @ref ecu_dlist_iterator_cbegin(),
 * @ref ecu_dlist_iterator_cend(), @ref ecu_dlist_iterator_cnext()
 *      - TEST(DList, IteratorGeneralTest)
 *      - TEST(DList, ConstIteratorGeneralTest)
 *      - TEST(DList, IteratorOverEmptyList)
 *      - TEST(DList, ConstIteratorOverEmptyList)
 *      - TEST(DList, IteratorRemoveNodesInMiddleOfIteration)
 * 
 * @ref ECU_DLIST_AT_FOR_EACH(), @ref ECU_DLIST_CONST_AT_FOR_EACH(), @ref ecu_dlist_iterator_at(),
 * @ref ecu_dlist_iterator_end(), @ref ecu_dlist_iterator_next(), @ref ecu_dlist_iterator_cat(),
 * @ref ecu_dlist_iterator_cend(), @ref ecu_dlist_iterator_cnext()
 *      - TEST(DList, AtIteratorGeneralTest)
 *      - TEST(DList, ConstAtIteratorGeneralTest)
 *      - TEST(DList, AtIteartorOverNodeNotInList)
 *      - TEST(DList, AtIteratorOverListWithOneNode)
 *      - TEST(DList, AtIteratorStartAtTailNode)
 *      - TEST(DList, AtIteratorRemoveNodesInMiddleOfIteration)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
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
 * @brief Actual mock call to node's destroy callback.
 */
static void node_destroy_mock(ecu_dnode *node, ecu_object_id id);

/**
 * @brief Actual mock call that node is in the list.
 */
static void node_in_list_mock(const ecu_dlist *list, const ecu_dnode *node);

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
    mock().actualCall(__func__)
          .withParameter("node", node);
}

static void node_in_list_mock(const ecu_dlist *list, const ecu_dnode *node)
{
    mock().actualCall("node_in_list_mock")
          .withParameter("list", list)
          .withParameter("node", node);
}

static void list_order_check_expectations(const ecu_dlist *list)
{
    assert( (list) );
    ecu_dlist_citerator citerator;

    ECU_DLIST_CONST_FOR_EACH(i, &citerator, list)
    {
        node_in_list_mock(list, i);
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

TEST_GROUP(DListBase)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);

        ecu_dlist_ctor(&m_list);
        ecu_dlist_ctor(&m_other_list);
        ecu_dnode_ctor(&m_node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node4, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node5, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_inserted_node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dnode_ctor(&m_node_not_in_list, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    ecu_dlist_iterator m_iterator;
    ecu_dlist_citerator m_citerator;
    ecu_dlist m_list;
    ecu_dlist m_other_list;
    ecu_dnode m_node1;
    ecu_dnode m_node2;
    ecu_dnode m_node3;
    ecu_dnode m_node4;
    ecu_dnode m_node5;
    ecu_dnode m_inserted_node;
    ecu_dnode m_node_not_in_list;
};

TEST_GROUP_BASE(DNode, TEST_GROUP_CppUTestGroupDListBase)
{
    void setup() override
    {
        TEST_GROUP_CppUTestGroupDListBase::setup();

        /* HEAD, 1, 2, 3. Use dnode_insert() functions instead of push_back()
        since this module tests insert() functions. Avoids dependencies. */
        ecu_dnode_insert_after(&m_list.head, &m_node1);
        ecu_dnode_insert_after(&m_node1, &m_node2);
        ecu_dnode_insert_after(&m_node2, &m_node3);
    }
};

TEST_GROUP_BASE(DList, TEST_GROUP_CppUTestGroupDListBase)
{
    void setup() override
    {
        TEST_GROUP_CppUTestGroupDListBase::setup();

        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);
        ecu_dlist_push_back(&m_list, &m_node3);
    }
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
        ecu_dlist_citerator citerator;

        ECU_DLIST_CONST_FOR_EACH(i, &citerator, &m_list)
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
    POINTERS_EQUAL(&m_node, me);
    UNSIGNED_LONGS_EQUAL(m_node.a, me->a);
    UNSIGNED_LONGS_EQUAL(m_node.b, me->b);
    UNSIGNED_LONGS_EQUAL(m_node.c, me->c);

    POINTERS_EQUAL(&m_node, me2);
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
    POINTERS_EQUAL(&m_node, me);
    UNSIGNED_LONGS_EQUAL(m_node.a, me->a);
    UNSIGNED_LONGS_EQUAL(m_node.b, me->b);
    UNSIGNED_LONGS_EQUAL(m_node.c, me->c);

    POINTERS_EQUAL(&m_node, me2);
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
        ecu_dnode_insert_before(&m_list.head, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is at the end of the list. */
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_before(&m_node3, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is before the tail (m_node3). */
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_before(&m_node_not_in_list, &m_inserted_node);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Add m_inserted_node to list to verify m_node_not_in_list
    wasn't attached to m_inserted_node. Do this outside first try-catch. */
    try 
    {
        set_assert_handler(AssertResponse::FAIL);
        ecu_dnode_insert_before(&m_list.head, &m_inserted_node);
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_before(&m_other_list.head, &m_inserted_node);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert node from listA into listB. */
        ecu_dnode_insert_before(&m_inserted_node, &m_node1);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_before(&m_node3, &m_inserted_node);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_before(&m_node3, &m_other_list.head);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_after(&m_list.head, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is one after HEAD. */
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_after(&m_node3, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is now the tail. */
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_after(&m_node_not_in_list, &m_inserted_node);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Add m_inserted_node to list to verify m_node_not_in_list
    wasn't attached to m_inserted_node. Do this outside first try-catch. */
    try 
    {
        set_assert_handler(AssertResponse::FAIL);
        ecu_dnode_insert_after(&m_node3, &m_inserted_node);
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_after(&m_other_list.head, &m_inserted_node);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Attempt to insert node from listA into listB. */
        ecu_dnode_insert_after(&m_inserted_node, &m_node1);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_after(&m_node3, &m_inserted_node);
    }
    catch (const AssertException& e)
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
        ecu_dnode_insert_after(&m_node3, &m_other_list.head);
    }
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
        ecu_dnode_insert_after(&m_other_list.head, &m_node2);

        /* Step 3: Assert. Verify list intact. */
        list_order_check_expectations(&m_list);
        list_order_check_expectations(&m_other_list);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

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
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify list intact. */
    list_order_check_expectations(&m_list);
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
        ecu_dnode_insert_after(&m_node3, &m_inserted_node);
        CHECK_TRUE( (ecu_dnode_in_list(&m_inserted_node)) );
        ecu_dnode_remove(&m_inserted_node);
        CHECK_FALSE( (ecu_dnode_in_list(&m_inserted_node)) );
    }

    catch (const AssertException& e)
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
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
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
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - DLIST CLEAR ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Assertion should not fire. Operation is valid to 
 * do on an empty list.
 */
TEST(DListBase, ClearEmptyList)
{
    try 
    {
        /* Step 1: Arrange. Precondition must be true to produce useful results. */
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
        
        /* Step 2: Action. */
        ecu_dlist_clear(&m_list);

        /* Step 3: Assert. */
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

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
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
};

/*------------------------------------------------------------*/
/*------------------ TESTS - DLIST FRONT CFRONT --------------*/
/*------------------------------------------------------------*/

/**
 * @brief NULL returned on empty list.
 */
TEST(DList, FrontEmptyList)
{
    try
    {
        /* Step 1: Arrange. Precondition must be true to produce useful results. */
        ecu_dlist_clear(&m_list);
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_front(&m_list) == nullptr) );
        CHECK_TRUE( (ecu_dlist_cfront(&m_list) == nullptr) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Front node returned. Front node is not popped.
 */
TEST(DList, FrontNonEmptyList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_front(&m_list) == &m_node1) );
        CHECK_TRUE( (ecu_dlist_cfront(&m_list) == &m_node1) );
        list_order_check_expectations(&m_list); /* Front node not popped. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*-------------------- TESTS - DLIST POP FRONT ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief NULL returned.
 */
TEST(DList, PopFrontEmptyList)
{
    try
    {
        /* Step 1: Arrange. Precondition must be true to produce useful results. */
        ecu_dlist_clear(&m_list);
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_pop_front(&m_list) == nullptr) );
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Popped node returned. Resulting list is now empty.
 */
TEST(DList, PopFrontListWithOneNode)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_dlist_clear(&m_list);
        ecu_dlist_push_back(&m_list, &m_node1);
        CHECK_TRUE( (ecu_dlist_size(&m_list) == 1) );

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_pop_front(&m_list) == &m_node1) );
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Popped node returned. Resulting list remains intact.
 */
TEST(DList, PopFrontListWithMultipleNodes)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_pop_front(&m_list) == &m_node1) );
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DLIST BACK CBACK ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief NULL returned on empty list.
 */
TEST(DList, BackEmptyList)
{
    try
    {
        /* Step 1: Arrange. Precondition must be true to produce useful results. */
        ecu_dlist_clear(&m_list);
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_back(&m_list) == nullptr) );
        CHECK_TRUE( (ecu_dlist_cback(&m_list) == nullptr) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Tail node returned. Tail node is not popped.
 */
TEST(DList, BackNonEmptyList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_back(&m_list) == &m_node3) );
        CHECK_TRUE( (ecu_dlist_cback(&m_list) == &m_node3) );
        list_order_check_expectations(&m_list); /* Tail node not popped. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
    Do this outside try-catch. */
    list_order_check_expectations(&m_list);
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - DLIST POP BACK ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief NULL returned.
 */
TEST(DList, PopBackEmptyList)
{
    try
    {
        /* Step 1: Arrange. Precondition must be true to produce useful results. */
        ecu_dlist_clear(&m_list);
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_pop_back(&m_list) == nullptr) );
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Popped node returned. Resulting list is now empty.
 */
TEST(DList, PopBackListWithOneNode)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_dlist_clear(&m_list);
        ecu_dlist_push_back(&m_list, &m_node1);
        CHECK_TRUE( (ecu_dlist_size(&m_list) == 1) );

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_pop_back(&m_list) == &m_node1) );
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Popped node returned. Resulting list remains intact.
 */
TEST(DList, PopBackListWithMultipleNodes)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dlist_pop_back(&m_list) == &m_node3) );
        list_order_check_expectations(&m_list);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
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
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - DLIST SWAP -------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Normal swap.
 */
TEST(DListBase, SwapBothListsNonEmpty)
{
    try
    {
        /* Step 1: Arrange. me = [1, 2, 3]. other = [4, 5]. */
        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);
        ecu_dlist_push_back(&m_list, &m_node3);
        ecu_dlist_push_back(&m_other_list, &m_node4);
        ecu_dlist_push_back(&m_other_list, &m_node5);

        /* Step 1: Arrange. Expected lists after swap. me = [4, 5]. other = [1, 2, 3]. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node4);
        EXPECT_NODE_IN_LIST(&m_list, &m_node5);
        EXPECT_NODE_IN_LIST(&m_other_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_other_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_other_list, &m_node3);

        /* Step 2: Action. */
        ecu_dlist_swap(&m_list, &m_other_list);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_list);
        list_order_check_expectations(&m_other_list);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Me list gets other's contents. Other list 
 * becomes empty.
 */
TEST(DListBase, SwapMeListEmptyOtherListNonEmpty)
{
    try
    {
        /* Step 1: Arrange. me = []. other = [1, 2]. */
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) ); /* Precondition. */
        ecu_dlist_push_back(&m_other_list, &m_node1);
        ecu_dlist_push_back(&m_other_list, &m_node2);

        /* Step 1: Arrange. Expected lists after swap. me = [1, 2]. other = []. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);

        /* Step 2: Action. */
        ecu_dlist_swap(&m_list, &m_other_list);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_list);
        CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Me list becomes empty. Other list gets
 * me's contents.
 */
TEST(DListBase, SwapMeListNonEmptyOtherListEmpty)
{
    try
    {
        /* Step 1: Arrange. me = [1, 2]. other = []. */
        CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) ); /* Precondition. */
        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);

        /* Step 1: Arrange. Expected lists after swap. me = []. other = [1, 2]. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_other_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_other_list, &m_node2);

        /* Step 2: Action. */
        ecu_dlist_swap(&m_list, &m_other_list);

        /* Step 3: Assert. */
        list_order_check_expectations(&m_other_list);
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Allowed. Nothing should happen.
 */
TEST(DListBase, SwapBothListsEmpty)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
        CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );

        /* Step 2: Action. */
        ecu_dlist_swap(&m_list, &m_other_list);

        /* Step 3: Assert. */
        CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
        CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Assertion should fire.
 */
TEST(DListBase, SwapSameListsSupplied)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ecu_dlist_swap(&m_list, &m_list);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DLIST GET SIZE -----------------*/
/*------------------------------------------------------------*/

TEST(DListBase, SizeGeneralTest)
{
    try
    {
        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)0);
        ecu_dlist_push_back(&m_list, &m_node1);
        CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)1);
        ecu_dlist_push_back(&m_list, &m_node2);
        CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)2);
        ecu_dlist_push_front(&m_list, &m_node3);
        CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)3);
        ecu_dnode_remove(&m_node2);
        CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)2);
        ecu_dlist_clear(&m_list);
        CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)0);
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DLIST IS EMPTY -----------------*/
/*------------------------------------------------------------*/

TEST(DListBase, EmptyGeneralTest)
{
    try
    {
        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE(ecu_dlist_empty(&m_list));
        ecu_dlist_push_back(&m_list, &m_node1);
        CHECK_FALSE(ecu_dlist_empty(&m_list));
        ecu_dlist_push_back(&m_list, &m_node2);
        CHECK_FALSE(ecu_dlist_empty(&m_list));
        ecu_dnode_remove(&m_node1);
        CHECK_FALSE(ecu_dlist_empty(&m_list));
        ecu_dnode_remove(&m_node2);
        CHECK_TRUE(ecu_dlist_empty(&m_list));
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DLIST ITERATORS ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify nonconst iterator iterates over all nodes in list.
 * Also use FOR_EACH() macro.
 */
TEST(DList, IteratorGeneralTest)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Steps 2 Action. */
        ECU_DLIST_FOR_EACH(i, &m_iterator, &m_list)
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Verify const iterator iterates over all nodes in list.
 * Also use FOR_EACH() macro.
 */
TEST(DList, ConstIteratorGeneralTest)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Steps 2 Action. */
        ECU_DLIST_CONST_FOR_EACH(i, &m_citerator, &m_list)
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Iterating over an empty list immediately returns.
 * Also use FOR_EACH() macro.
 */
TEST(DList, IteratorOverEmptyList)
{
    try
    {
        /* Step 1: Arrange. Preconditions must be true for test to produce useful results. */
        CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );

        /* Step 2: Action. */
        ECU_DLIST_FOR_EACH(i, &m_iterator, &m_other_list)
        {
            /* Step 3: Assert. This should never be called since empty list. */
            node_in_list_mock(&m_other_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Const iterating over an empty list immediately returns.
 * Also use FOR_EACH() macro.
 */
TEST(DList, ConstIteratorOverEmptyList)
{
    try
    {
        /* Step 1: Arrange. Preconditions must be true for test to produce useful results. */
        CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );

        /* Step 2: Action. */
        ECU_DLIST_CONST_FOR_EACH(i, &m_citerator, &m_other_list)
        {
            /* Step 3: Assert. This should never be called since empty list. */
            node_in_list_mock(&m_other_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Verify it is OK to remove nodes in the middle of an iteration.
 * Also use FOR_EACH() macro.
 */
TEST(DList, IteratorRemoveNodesInMiddleOfIteration)
{
    try
    {
        /* Step 1: Arrange. First list iteration. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 1: Arrange. Second list iteration. */
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 2: Action. */
        ECU_DLIST_FOR_EACH(i, &m_iterator, &m_list)
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);

            if (i == &m_node2)
            {
                ecu_dnode_remove(i);
            }
        }

        /* Step 3: Assert. Iterate over list again. */
        ECU_DLIST_FOR_EACH(i, &m_iterator, &m_list)
        {
            node_in_list_mock(&m_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - DLIST AT ITERATORS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Iteration begins at start position and terminates
 * at list end.
 */
TEST(DList, AtIteratorGeneralTest)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Steps 2 Action. */
        ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, &m_node2)
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Iteration begins at start position and terminates
 * at list end.
 */
TEST(DList, ConstAtIteratorGeneralTest)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Steps 2 Action. */
        ECU_DLIST_CONST_AT_FOR_EACH(i, &m_citerator, &m_list, &m_node2)
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Assertion should fire.
 */
TEST(DList, AtIteartorOverNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_dnode_remove(&m_node1);
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, &m_node1)
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);
        }

    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Iteration should only be over the starting node.
 */
TEST(DList, AtIteratorOverListWithOneNode)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_dlist_clear(&m_list);
        ecu_dlist_push_back(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);

        /* Step 2: Action. */
        ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, &m_node1)
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Iteration should only be over the tail node.
 */
TEST(DList, AtIteratorStartAtTailNode)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_TRUE( (ecu_dlist_size(&m_list) > 1) ); /* Precondition. */
        EXPECT_NODE_IN_LIST(&m_list, ecu_dlist_cback(&m_list));

        /* Step 2: Action. */
        ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, ecu_dlist_back(&m_list))
        {
            /* Step 3: Assert. */
            node_in_list_mock(&m_list, i);
        }

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief It is OK to remove nodes in the middle of an iteration.
 */
TEST(DList, AtIteratorRemoveNodesInMiddleOfIteration)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node2);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);
        EXPECT_NODE_IN_LIST(&m_list, &m_node1);
        EXPECT_NODE_IN_LIST(&m_list, &m_node3);

        /* Step 2: Action. */
        ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, ecu_dlist_front(&m_list))
        {
            /* Step 3: Assert. */
            if (i == &m_node2)
            {
                ecu_dnode_remove(i);
            }
            node_in_list_mock(&m_list, i);
        }

        /* Step 3: Assert. Iterate over list again. */
        ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, ecu_dlist_front(&m_list))
        {
            node_in_list_mock(&m_list, i);
        }

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}
