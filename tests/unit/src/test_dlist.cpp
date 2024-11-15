/**
 * @file
 * @brief Unit tests for public API functions in @ref ecu_dlist.h
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
/*----------------------- FILE SCOPE DEFINES------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Mock expectation to set in beginning of test. Call if 
 * node's destroy callback, @ref node_destroy_mock, is expected
 * to be called. Provides a more readable expectation and allows 
 * the string passed to expectOneCall() to be contained within this 
 * macro in case it changes.
 */
#define EXPECT_NODE_DESTROYED(n)    (mock().expectOneCall("node_destroy_mock").onObject((n)))

/**
 * @brief Mock expectation to set in beginning of test. Call
 * if node should still be in a list after the test completes.
 * Verify via call to @ref list_iteration_mock(). Provides a 
 * more readable expectation and allows the string passed to 
 * expectOneCall() to be contained within this macro in case it 
 * changes.
 */
#define EXPECT_NODE_IN_LIST(n)  (mock().expectOneCall("node_in_list_mock").onObject((n)))

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Mock to assign to node's destroy callback.
 */
static void node_destroy_mock(struct ecu_dlist_node *me, ecu_object_id id);

/**
 * @brief Verify if node is in a list. Call this on a node 
 * returned from an iteration.
 */
static void node_in_list_mock(const struct ecu_dlist_node *me);

/**
 * @brief Iterates over all nodes in a list and calls 
 * @ref node_in_list_mock() on each node. Provides a portable way
 * to verify nodes are in a list via dlist API.
 */
static void list_iteration_mock(const struct ecu_dlist *me);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void node_destroy_mock(struct ecu_dlist_node *me, ecu_object_id id)
{
    (void)id;
    assert(me);
    mock().actualCall(__func__)
          .onObject(me);
}

static void node_in_list_mock(const struct ecu_dlist_node *me)
{
    assert(me);
    mock().actualCall(__func__)
          .onObject(me);
}

static void list_iteration_mock(const struct ecu_dlist *me)
{
    assert(me);
    struct ecu_dlist_const_iterator iterator;

    for (const struct ecu_dlist_node *i = ecu_dlist_const_iterator_begin(&iterator, me);
         i != ecu_dlist_const_iterator_end(&iterator);
         i = ecu_dlist_const_iterator_next(&iterator))
    {
        node_in_list_mock(i);
    }
}

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(DListMacros)
{
    struct test_node
    {
        std::uint32_t a{0};
        struct ecu_dlist_node node1;
        std::uint8_t b{0};
        struct ecu_dlist_node node2;
        std::uint16_t c{0};
    };

    test_node m_node;
};

TEST_GROUP(DListCtors)
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

    void setup_list(void (*destroy1)(struct ecu_dlist_node *, ecu_object_id) = ECU_DLIST_NODE_DESTROY_UNUSED,
                    void (*destroy2)(struct ecu_dlist_node *, ecu_object_id) = ECU_DLIST_NODE_DESTROY_UNUSED,
                    void (*destroy3)(struct ecu_dlist_node *, ecu_object_id) = ECU_DLIST_NODE_DESTROY_UNUSED)
    {
        ecu_dlist_ctor(&m_list);
        ecu_dlist_node_ctor(&m_node1, destroy1, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_node_ctor(&m_node2, destroy2, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_node_ctor(&m_node3, destroy3, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);
        ecu_dlist_push_back(&m_list, &m_node3);
    }

    struct ecu_dlist m_list;
    struct ecu_dlist_node m_node1;
    struct ecu_dlist_node m_node2;
    struct ecu_dlist_node m_node3;
};

TEST_GROUP(DListInsert)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);

        ecu_dlist_ctor(&m_list);
        ecu_dlist_ctor(&m_other_list);
        ecu_dlist_node_ctor(&m_node1, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_node_ctor(&m_node2, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_node_ctor(&m_node3, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_node_ctor(&m_inserted_node, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_node_ctor(&m_node_not_in_list, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);

        ecu_dlist_push_back(&m_list, &m_node1);
        ecu_dlist_push_back(&m_list, &m_node2);
        ecu_dlist_push_back(&m_list, &m_node3);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    struct ecu_dlist m_list;
    struct ecu_dlist m_other_list;
    struct ecu_dlist_node m_node1;
    struct ecu_dlist_node m_node2;
    struct ecu_dlist_node m_node3;
    struct ecu_dlist_node m_inserted_node;
    struct ecu_dlist_node m_node_not_in_list;
};

/*------------------------------------------------------------*/
/*----------------- TESTS - DLIST MACRO TESTS ----------------*/
/*------------------------------------------------------------*/

TEST(DListMacros, GetEntryNode1)
{
    /* Step 1: Arrange. */
    static constexpr std::uint32_t a_val = 5;
    static constexpr std::uint8_t b_val = 5;
    static constexpr std::uint16_t c_val = 5;
    CHECK_TRUE( (m_node.a != a_val) );
    CHECK_TRUE( (m_node.b != b_val) );
    CHECK_TRUE( (m_node.c != c_val) );

    /* Step 2: Action. */
    test_node *me = ECU_DLIST_GET_ENTRY(&m_node.node1, struct test_node, node1);
    me->a = 5;
    me->b = 5;
    me->c = 5;

    /* Step 3: Assert. */
    UNSIGNED_LONGS_EQUAL(a_val, m_node.a);
    UNSIGNED_LONGS_EQUAL(b_val, m_node.b);
    UNSIGNED_LONGS_EQUAL(c_val, m_node.c);
}

TEST(DListMacros, GetEntryNode2)
{
    /* Step 1: Arrange. */
    static constexpr std::uint32_t a_val = 5;
    static constexpr std::uint8_t b_val = 5;
    static constexpr std::uint16_t c_val = 5;
    CHECK_TRUE( (m_node.a != a_val) );
    CHECK_TRUE( (m_node.b != b_val) );
    CHECK_TRUE( (m_node.c != c_val) );

    /* Step 2: Action. */
    test_node *me = ECU_DLIST_GET_ENTRY(&m_node.node2, struct test_node, node2);
    me->a = 5;
    me->b = 5;
    me->c = 5;

    /* Step 3: Assert. */
    UNSIGNED_LONGS_EQUAL(a_val, m_node.a);
    UNSIGNED_LONGS_EQUAL(b_val, m_node.b);
    UNSIGNED_LONGS_EQUAL(c_val, m_node.c);
}

TEST(DListMacros, GetConstEntry)
{
    /* Step 2: Action. */
    const test_node *me = ECU_DLIST_GET_CONST_ENTRY(&m_node.node1, struct test_node, node1);
    const test_node *me2 = ECU_DLIST_GET_CONST_ENTRY(&m_node.node2, struct test_node, node2);

    /* Step 3: Assert. */
    POINTERS_EQUAL(&m_node, me);
    POINTERS_EQUAL(&m_node, me2);
}

/*------------------------------------------------------------*/
/*------------------- TESTS - CONSTRUCTORS -------------------*/
/*------------------------------------------------------------*/
/* NOTE: All tests require iterator to be working. */

/**
 * @brief Node's destroy callback executes and is
 * removed from the list.
 */
TEST(DListCtors, NodeDestroy)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list(&node_destroy_mock, &node_destroy_mock, &node_destroy_mock);
        EXPECT_NODE_DESTROYED(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node3);

        /* Step 2: Action. */
        ecu_dlist_node_destroy(&m_node2);

        /* Step 3: Assert. Verify only node2 was removed from list. */
        list_iteration_mock(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief A destroyed node cannot be used again. 
 */
TEST(DListCtors, CannotUseDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        ecu_dlist_node_destroy(&m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be after node_destroy() and before step 2. */

        /* Step 2: Action. */
        ecu_dlist_push_back(&m_list, &m_node3);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify node3 wasn't re-added to the list. 
    Do this outside try-catch. */
    list_iteration_mock(&m_list);
}

/**
 * @brief Reconstructing a destroyed node makes it useable again.
 */
TEST(DListCtors, ReconstructDestroyedNode)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        ecu_dlist_node_destroy(&m_node3);

        /* Step 2: Action. */
        ecu_dlist_node_ctor(&m_node3, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_push_back(&m_list, &m_node3);

        /* Step 3: Assert. Verify node3 can be used again and was re-added
        to the list. */
        list_iteration_mock(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief All node destroy callbacks execute and all nodes
 * are removed from the list. 
 */
TEST(DListCtors, ListDestroy)
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

        /* Step 3: Assert. Verify no nodes are in the list. We 
        have to verify this manually (unportable way) since destroyed 
        list can't be passed to list_iteration_mock().*/
        CHECK_TRUE( (m_list.head.next != &m_node1) );
        CHECK_TRUE( (m_list.head.next != &m_node2) );
        CHECK_TRUE( (m_list.head.next != &m_node3) );
        CHECK_TRUE( (m_list.head.prev != &m_node1) );
        CHECK_TRUE( (m_list.head.prev != &m_node2) );
        CHECK_TRUE( (m_list.head.prev != &m_node3) );
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief A destroyed list cannot be used again. 
 */
TEST(DListCtors, CannotUseDestroyedList)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list();
        ecu_dlist_destroy(&m_list);
        set_assert_handler(AssertResponse::OK); /* Must be after list_destroy() and before step 2. */

        /* Step 2: Action. */
        /* Must construct node to ensure list can't be used due to destroyed list, NOT a destroyed node. */
        ecu_dlist_node_ctor(&m_node1, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_push_back(&m_list, &m_node1);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify destroyed list isn't usable. Do this 
    outside try-catch. Node1 shouldn't be re-added to the list. We 
    have to verify this manually (unportable way) since destroyed 
    list can't be passed to list_iteration_mock(). */
    CHECK_TRUE( (m_list.head.next != &m_node1) );
    CHECK_TRUE( (m_list.head.prev != &m_node1) );
}

/**
 * @brief A node destroyed from the list destructor cannot
 * be used again.
 */
TEST(DListCtors, CannotUseDestroyedListNode)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list();
        ecu_dlist_destroy(&m_list);
        set_assert_handler(AssertResponse::OK); /* Must be after list_destroy() and before step 2. */

        /* Step 2: Action. */
        /* Must construct list to ensure list can't be used due to destroyed node, NOT a destroyed list. */
        ecu_dlist_ctor(&m_list);
        ecu_dlist_push_back(&m_list, &m_node1);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify destroyed node isn't usable. Node1 
    shouldn't be re-added to the list. Do this outside try-catch. */
    list_iteration_mock(&m_list);
}

/**
 * @brief Reconstructing a destroyed list makes it useable again.
 */
TEST(DListCtors, ReconstructDestroyedList)
{
    try
    {
        /* Step 1: Arrange. */
        setup_list();
        EXPECT_NODE_IN_LIST(&m_node1);
        ecu_dlist_destroy(&m_list);

        /* Step 2: Action. */
        ecu_dlist_ctor(&m_list);
        ecu_dlist_node_ctor(&m_node1, ECU_DLIST_NODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        ecu_dlist_push_back(&m_list, &m_node1);

        /* Step 3: Assert. Verify list and node1 can be used again. Node1
        should be re-added to the list. */
        list_iteration_mock(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - INSERT BEFORE -------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Can insert_before() HEAD node. Added node should be at
 * the end of the list.
 */
TEST(DListInsert, InsertBeforeHead)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        EXPECT_NODE_IN_LIST(&m_inserted_node);

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_list.head, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is at the end of the list. */
        list_iteration_mock(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief insert_before() tail node adds node before tail.
 * It should not be at the end of the list.
 */
TEST(DListInsert, InsertBeforeTail)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_inserted_node);
        EXPECT_NODE_IN_LIST(&m_node3);

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_node3, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is before the tail (m_node3). */
        list_iteration_mock(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Cannot insert_before() if position node is not
 * in a list.
 */
TEST(DListInsert, InsertBeforePositionNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_node_not_in_list, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to list. 
    Do this outside try-catch. */
    list_iteration_mock(&m_list);
}

/**
 * @brief Cannot insert_before() if added node is already in
 * another list.
 */
TEST(DListInsert, InsertBeforeNodeAlreadyInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        ecu_dlist_push_back(&m_other_list, &m_inserted_node);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Do not insert m_inserted_node before HEAD or 
        else test will falsely pass if node is incorrectly added to 
        m_other_list. */
        ecu_dlist_insert_before(&m_list, &m_node1, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify lists weren't changed. m_inserted_node should
    still be in m_other_list. m_list must be iterated over first since
    mocks are in strict order. Do this outside try-catch. */
    list_iteration_mock(&m_list);
    list_iteration_mock(&m_other_list);
}

/**
 * @brief Cannot insert_before() when position node is HEAD
 * of another list.
 */
TEST(DListInsert, InsertBeforePositionNodeHeadOfOtherList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_other_list.head, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_other_list.head was not added to m_list. 
    Do this outside try-catch. */
    list_iteration_mock(&m_list);
}

/**
 * @brief Cannot insert_before() node with an invalid ID.
 */
TEST(DListInsert, InsertBeforeNodeWithInvalidID)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        m_inserted_node.id = ECU_OBJECT_ID_RESERVED;
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dlist_insert_before(&m_list, &m_node3, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to m_list. 
    Do this outside try-catch. */
    list_iteration_mock(&m_list);
}

/*------------------------------------------------------------*/
/*------------------- TESTS - INSERT AFTER -------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Can insert_after() HEAD node. Added node should be 
 * one after HEAD.
 */
TEST(DListInsert, InsertAfterHead)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_inserted_node);
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);

        /* Step 2: Action. */
        ecu_dlist_insert_after(&m_list, &m_list.head, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is one after HEAD. */
        list_iteration_mock(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief insert_after() tail node adds node to back of 
 * the list.
 */
TEST(DListInsert, InsertAfterTail)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        EXPECT_NODE_IN_LIST(&m_inserted_node);

        /* Step 2: Action. */
        ecu_dlist_insert_after(&m_list, &m_node3, &m_inserted_node);

        /* Step 3: Assert. Verify m_inserted_node is now the tail. */
        list_iteration_mock(&m_list);
    }
    catch (AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Cannot insert_after() if position node is not
 * in a list.
 */
TEST(DListInsert, InsertAfterPositionNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dlist_insert_after(&m_list, &m_node_not_in_list, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to list. 
    Do this outside try-catch. */
    list_iteration_mock(&m_list);
}

/**
 * @brief Cannot insert_after() if added node is already in
 * another list.
 */
TEST(DListInsert, InsertAfterNodeAlreadyInList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        ecu_dlist_push_back(&m_other_list, &m_inserted_node);

        /* m_list nodes. */
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);

        /* m_other_list nodes. */
        EXPECT_NODE_IN_LIST(&m_inserted_node);

        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. Do not insert m_inserted_node after m_node3 
        or else test will falsely pass if node is incorrectly added to 
        m_other_list. */
        ecu_dlist_insert_after(&m_list, &m_node1, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify lists weren't changed. m_inserted_node should
    still be in m_other_list. m_list must be iterated over first since
    mocks are in strict order. Do this outside try-catch. */
    list_iteration_mock(&m_list);
    list_iteration_mock(&m_other_list);
}

/**
 * @brief Cannot insert_after() when position node is HEAD
 * of another list.
 */
TEST(DListInsert, InsertAfterPositionNodeHeadOfOtherList)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dlist_insert_after(&m_list, &m_other_list.head, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_other_list.head was not added to m_list. 
    Do this outside try-catch. */
    list_iteration_mock(&m_list);
}

/**
 * @brief Cannot insert_after() node with an invalid ID.
 */
TEST(DListInsert, InsertAfterNodeWithInvalidID)
{
    try
    {
        /* Step 1: Arrange. */
        mock().strictOrder();
        EXPECT_NODE_IN_LIST(&m_node1);
        EXPECT_NODE_IN_LIST(&m_node2);
        EXPECT_NODE_IN_LIST(&m_node3);
        m_inserted_node.id = ECU_OBJECT_ID_RESERVED;
        set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

        /* Step 2: Action. */
        ecu_dlist_insert_after(&m_list, &m_node3, &m_inserted_node);
    }
    catch (AssertException& e)
    {
        /* OK. */
        (void)e;
    }

    /* Step 3: Assert. Verify m_inserted_node was not added to m_list. 
    Do this outside try-catch. */
    list_iteration_mock(&m_list);
}


#warning "TODO: Stopped here"
// 1. push_front() adds node in front of list.
// 2. Cannot push_front() where node is in another list.
// 3. Cannot push_front() where node is HEAD of this list.
// 4. Cannot push_front() where node is HEAD of another list.
// 5. Cannot push_front() node with invalid ID.
// 6 to 10. Repeat same tests for push_back().

// 4. Test get_size() by calling push_front(), push_back(), insert_before(), insert_after() and remove(). general insert and removal test.
// 15. Cannot remove() node not in list.
// 16. Cannot remove() node in another list.
// 19. General iterator test. Verify iterator goes over all nodes in correct order. (set ids, mock strict order).
// 20. Using push_front() in middle of iteration is OK. These nodes WON'T be iterated over.
// 21. Using push_back() in middle of iteration is OK. These nodes WILL be iterated over.
// 22. Using insert_before() in middle of iteration is OK. These nodes WON'T be iterated over.
// 23. Using insert_after() in middle of iteration is OK. These nodes WON'T be itreated over.
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
