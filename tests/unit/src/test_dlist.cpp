/**
 * @file
 * @brief Unit tests for public API functions in @ref dlist.h.
 * Test Summary:
 * 
 * @ref ECU_DNODE_GET_ENTRY() and @ref ECU_DNODE_GET_CONST_ENTRY()
 *      - TEST(DList, DNodeGetEntry)
 * 
 * @ref ecu_dnode_ctor(), @ref ecu_dnode_destroy()
 *      - TEST(DList, DNodeDestroy)
 * 
 * @ref ecu_dnode_id()
 *      - TEST(DList, DNodeID)
 * 
 * @ref ecu_dnode_in_list()
 *      - TEST(DList, DNodeInList)
 *      - TEST(DList, DNodeInListNodeIsHead)
 * 
 * @ref ecu_dnode_insert_after()
 *      - TEST(DList, DNodeInsertAfterPosIsFront)
 *      - TEST(DList, DNodeInsertAfterPosIsMiddle)
 *      - TEST(DList, DNodeInsertAfterPosIsTail)
 *      - TEST(DList, DNodeInsertAfterPosNotInList)
 *      - TEST(DList, DNodeInsertAfterNodeInList)
 *      - TEST(DList, DNodeInsertAfterPosIsHead)
 *      - TEST(DList, DNodeInsertAfterNodeIsHead)
 *      - TEST(DList, DNodeInsertAfterPosEqualsNode)
 * 
 * @ref ecu_dnode_insert_before()
 *      - TEST(DList, DNodeInsertBeforePosIsFront)
 *      - TEST(DList, DNodeInsertBeforePosIsMiddle)
 *      - TEST(DList, DNodeInsertBeforePosIsTail)
 *      - TEST(DList, DNodeInsertBeforePosNotInList)
 *      - TEST(DList, DNodeInsertBeforeNodeInList)
 *      - TEST(DList, DNodeInsertBeforePosIsHead)
 *      - TEST(DList, DNodeInsertBeforeNodeIsHead)
 *      - TEST(DList, DNodeInsertBeforePosEqualsNode)
 * 
 * @ref ecu_dnode_next(), @ref ecu_dnode_cnext()
 *      - TEST(DList, DNodeNext)
 *      - TEST(DList, DNodeNextNodeIsHead)
 *      - TEST(DList, DNodeCNextNodeIsHead)
 *      - TEST(DList, DNodeNextNodeIsTail)
 *      - TEST(DList, DNodeNextNodeNotInList)
 * 
 * @ref ecu_dnode_prev(), @ref ecu_dnode_cprev()
 *      - TEST(DList, DNodePrev)
 *      - TEST(DList, DNodePrevNodeIsHead)
 *      - TEST(DList, DNodeCPrevNodeIsHead)
 *      - TEST(DList, DNodePrevNodeIsFirst)
 *      - TEST(DList, DNodePrevNodeNotInList)
 * 
 * @ref ecu_dnode_remove()
 *      - TEST(DList, DNodeRemove)
 *      - TEST(DList, DNodeRemoveAndReAddNodes)
 *      - TEST(DList, DNodeRemoveNodeNotInList)
 *      - TEST(DList, DNodeRemoveNodeIsHead)
 * 
 * @ref ecu_dnode_valid()
 *      - TEST(DList, DNodeValid)
 * 
 * @ref ecu_dlist_ctor(), @ref ecu_dlist_destroy()
 *      - TEST(DList, DListDestroy)
 * 
 * @ref ecu_dlist_back(), ecu_dlist_cback()
 *      - TEST(DList, DListBack)
 *      - TEST(DList, DListBackListIsEmpty)
 * 
 * @ref ecu_dlist_clear()
 *      - TEST(DList, DListClear)
 *      - TEST(DList, DListClearAndReAdd)
 *      - TEST(DList, DListClearListIsEmpty)
 * 
 * @ref ecu_dlist_empty()
 *      - TEST(DList, DListEmpty)
 * 
 * @ref ecu_dlist_front(), @ref ecu_dlist_cfront()
 *      - TEST(DList, DListFront)
 *      - TEST(DList, DListFrontListIsEmpty)
 * 
 * @ref ecu_dlist_insert_before()
 *      - TEST(DList, DListInsertBeforeMiddleConditionPasses)
 *      - TEST(DList, DListInsertBeforeFirstConditionPasses)
 *      - TEST(DList, DListInsertBeforeAllConditionsFalse)
 *      - TEST(DList, DListInsertBeforeListIsEmpty)
 *      - TEST(DList, DListInsertBeforeNodeInList)
 *      - TEST(DList, DListInsertBeforeNodeIsHead)
 * 
 * @ref ecu_dlist_push_back()
 *      - TEST(DList, DListPushBack)
 *      - TEST(DList, DListPushBackListIsEmpty)
 *      - TEST(DList, DListPushBackNodeInList)
 *      - TEST(DList, DListPushBackNodeIsHead)
 * 
 * @ref ecu_dlist_push_front()
 *      - TEST(DList, DListPushFront)
 *      - TEST(DList, DListPushFrontListIsEmpty)
 *      - TEST(DList, DListPushFrontNodeInList)
 *      - TEST(DList, DListPushFrontNodeIsHead)
 * 
 * @ref ecu_dlist_pop_back()
 *      - TEST(DList, DListPopBack)
 *      - TEST(DList, DListPopBackListIsEmpty)
 *      - TEST(DList, DListPopBackListWithOneNode)
 * 
 * @ref ecu_dlist_pop_front()
 *      - TEST(DList, DListPopFront)
 *      - TEST(DList, DListPopFrontListIsEmpty)
 *      - TEST(DList, DListPopFrontListWithOneNode)
 * 
 * @ref ecu_dlist_size()
 *      - TEST(DList, DListSize)
 *      - TEST(DList, DListSizeListIsEmpty)
 * 
 * @ref ecu_dlist_sort()
 *      - TEST(DList, DListSortUniqueSortEven)
 *      - TEST(DList, DListSortUniqueSortOdd)
 *      - TEST(DList, DListSortNonUniqueSortEven)
 *      - TEST(DList, DListSortNonUniqueSortOdd)
 * 
 * @ref ecu_dlist_swap()
 *      - TEST(DList, DListSwapBothListsNotEmpty)
 *      - TEST(DList, DListSwapMeListEmptyOtherListNotEmpty)
 *      - TEST(DList, DListSwapMeListNotEmptyOtherListEmpty)
 *      - TEST(DList, DListSwapBothListsEmpty)
 *      - TEST(DList, DListSwapSameListsSupplied)
 * 
 * @ref ecu_dlist_valid()
 *      - TEST(DList, DListValid)
 * 
 * @ref ECU_DLIST_FOR_EACH(), @ref ECU_DLIST_CONST_FOR_EACH(), @ref ecu_dlist_iterator_begin(),
 * @ref ecu_dlist_iterator_end(), @ref ecu_dlist_iterator_next(), @ref ecu_dlist_iterator_cbegin(),
 * @ref ecu_dlist_iterator_cend(), @ref ecu_dlist_iterator_cnext()
 *      - TEST(DList, Iterator)
 *      - TEST(DList, ConstIterator)
 *      - TEST(DList, IteratorListIsEmpty)
 *      - TEST(DList, ConstIteratorListIsEmpty)
 *      - TEST(DList, IteratorRemoveSome)
 *      - TEST(DList, IteratorRemoveAll)
 * 
 * @ref ECU_DLIST_AT_FOR_EACH(), @ref ECU_DLIST_CONST_AT_FOR_EACH(), @ref ecu_dlist_iterator_at(),
 * @ref ecu_dlist_iterator_end(), @ref ecu_dlist_iterator_next(), @ref ecu_dlist_iterator_cat(),
 * @ref ecu_dlist_iterator_cend(), @ref ecu_dlist_iterator_cnext()
 *      - TEST(DList, AtIterator)
 *      - TEST(DList, ConstAtIterator)
 *      - TEST(DList, AtIteartorStartNodeNotInList)
 *      - TEST(DList, ConstAtIteratorStartNodeNotInList)
 *      - TEST(DList, AtIteratorStartNodeIsHead)
 *      - TEST(DList, ConstAtIteratorStartNodeIsHead)
 *      - TEST(DList, AtIteratorListWithOneNode)
 *      - TEST(DList, ConstAtIteratorListWithOneNode)
 *      - TEST(DList, AtIteratorStartNodeIsTail)
 *      - TEST(DList, ConstAtIteratorStartNodeIsTail)
 *      - TEST(DList, AtIteratorRemoveSome)
 *      - TEST(DList, AtIteratorRemoveAll)
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
#include <type_traits>
#include <vector>

/* Stubs and helpers. */
#include "stubs/stub_asserter.hpp"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*------------------------- NAMESPACES -----------------------*/
/*------------------------------------------------------------*/

using namespace stubs;

namespace
{
/*------------------------------------------------------------*/
/*--------------- FILE-SCOPE FUNCTION DECLARATIONS -----------*/
/*------------------------------------------------------------*/

/* Forward declarations. */
struct dnode;
struct rw_dnode;
struct ro_dnode;

/**
 * @brief Safely converts (ecu_dnode *) which is a C structure
 * into the dnode C++ wrapper class. Common use case would be
 * converting (ecu_dnode *) returned by C-based iterators under test.
 * Asserts if ecu_dnode_valid(n) fails.
 */
dnode& convert(ecu_dnode *n);

/**
 * @brief Safely converts (const ecu_dnode *) which is a C structure
 * into the dnode C++ wrapper class. Common use case would be
 * converting (ecu_dnode *) returned by C-based iterators under test.
 * Asserts if ecu_dnode_valid(n) fails.
 */
const dnode& convert(const ecu_dnode *n);

/*------------------------------------------------------------*/
/*----------- FILE-SCOPE TYPES - VISITOR INTERFACE -----------*/
/*------------------------------------------------------------*/

/**
 * @brief Visitor interface. Used to give tests more
 * flexility in case test code is edited in the future.
 */
struct visitor
{
    /// @brief Virtual base class destructor.
    virtual ~visitor() = default;

    /// @brief Any operation applied to read-write node.
    virtual void visit(rw_dnode& n) = 0;

    /// @brief Only read-only operations can be applied since
    /// read-write node is const.
    virtual void visit(const rw_dnode& n) = 0;
    
    /// @brief Only read-only operations can be applied since
    /// node is read-only type (and const).
    virtual void visit(const ro_dnode& n) = 0;

protected:
    /// @brief Default constructor.
    visitor() = default;
};

/*------------------------------------------------------------*/
/*-------------------- FILE-SCOPE TYPES - NODES --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Dnode interface. This is a C++ wrapper class around C 
 * structure under test, ecu_dnode. Used to create a C++ interface 
 * for an ecu_dnode to facilitate easier test development.
 */
struct dnode : public ecu_dnode
{
protected:
    /// @brief Default constructor. Node's destroy callback
    /// and ID are unused.
    dnode()
    {
        ecu_dnode_ctor(this, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    }

    /// @brief Constructor assigns supplied ID to node. Node's 
    /// destroy callback unused.
    dnode(ecu_object_id id_)
    {
        ecu_dnode_ctor(this, ECU_DNODE_DESTROY_UNUSED, id_);
    }

    /// @brief Constructor assigns supplied destroy callback and 
    /// optional ID to ecu_dnode. Function takes in the C-structure
    /// under test (ecu_dnode *) to be compatible with the C API and
    /// to avoid having to explicitly upcast. Do not convert into 
    /// this C++ wrapper class (dnode *), as ecu_dnode_destroy() will
    /// have already been called on the node, thus invalidating it.
    dnode(void (*destroy_)(ecu_dnode *, ecu_object_id), ecu_object_id id_ = ECU_OBJECT_ID_UNUSED)
    {
        ecu_dnode_ctor(this, destroy_, id_);
    }

    /// @brief For now, copy semantics are removed to simplify test class.
    dnode(const dnode& other) = delete;

    /// @brief For now, copy semantics are removed to simplify test class.
    dnode& operator=(const dnode& other) = delete;

    /// @brief Move semantics are required if an rvalue node is passed
    /// into @ref dlist class constructor. The rvalue is moved onto the
    /// heap so it remains in memory and stored in a vector of pointers.
    dnode(dnode&& other) noexcept
    {
        /* It should be impossible for an rvalue node to be in a list. 
        This should only be possible if an lvalue was explicitly casted
        to an rvalue, which is an error. */
        assert( (!ecu_dnode_in_list(&other)) );
        ecu_dnode_ctor(this, other.destroy, other.id);

        /* Reset values to avoid mock going off from destroyed temporary. */
        other.destroy = ECU_DNODE_DESTROY_UNUSED;
        other.id = ECU_OBJECT_ID_UNUSED;
    }

    /// @brief Only move constructor currently used so delete move 
    /// assignment operator.
    dnode& operator=(dnode&& other) = delete;

public:
    /// @brief Virtual base class destructor. C-destructor
    /// ecu_dnode_destroy() not called since tests call
    /// it directly. Do not place in C++ destructor or else
    /// ecu_dnode_destroy() is called twice, first in the test
    /// and then in this destructor. The 2nd call causes
    /// problems because an invalidated node (next, prev, etc
    /// reset to NULL from first ecu_dnode_destroy call) is 
    /// passed, causing ecu_dnode_valid() to fail. This is
    /// meant to happen since C-destructor under test does
    /// not use dynamic memory allocation, so it only invalidates
    /// the object.
    virtual ~dnode() = default;

    /// @brief Runs concrete visitor on this node.
    /// Pure virtual in order to perform double dispatching.
    virtual void accept(visitor& v) = 0;

    /// @brief Runs concrete visitor on this node.
    /// Pure virtual in order to perform double dispatching.
    virtual void accept(visitor& v) const = 0;
};

/**
 * @brief Read-write node.
 */
struct rw_dnode : public dnode
{
    /// @brief Default constructor. Node's destroy callback
    /// and ID are unused.
    rw_dnode() 
        : dnode()
    {
        
    }

    /// @brief Constructor assigns supplied ID to node. Node's 
    /// destroy callback unused.
    rw_dnode(ecu_object_id id_)
        : dnode(std::move(id_))
    {

    }

    /// @brief Constructor assigns supplied destroy callback and 
    /// optional ID to ecu_dnode. Function takes in the C-structure
    /// under test (ecu_dnode *) to be compatible with the C API and
    /// to avoid having to explicitly upcast. Do not convert into 
    /// the C++ base wrapper class (dnode *), as ecu_dnode_destroy() will
    /// have already been called on the node, thus invalidating it.
    rw_dnode(void (*destroy_)(ecu_dnode *, ecu_object_id), ecu_object_id id_ = ECU_OBJECT_ID_UNUSED)
        : dnode(std::move(destroy_), std::move(id_))
    {

    }

    /// @brief For now, copy semantics are removed to simplify test class.
    rw_dnode(const rw_dnode& other) = delete;

    /// @brief For now, copy semantics are removed to simplify test class.
    rw_dnode& operator=(const rw_dnode& other) = delete;

    /// @brief Move semantics are required if an rvalue node is passed
    /// into @ref dlist class constructor. The rvalue is moved onto the
    /// heap so it remains in memory and stored in a vector of pointers.
    rw_dnode(rw_dnode&& other) noexcept
        : dnode(std::move(other))
    {
        /* Currently no additional members to move. */
    }

    /// @brief Only move constructor currently used so delete move 
    /// assignment operator.
    rw_dnode& operator=(rw_dnode&& other) = delete;

    /// @brief Runs concrete visitor on this specific node type.
    void accept(visitor& v) override
    {
        v.visit(*this);
    }

    /// @brief Runs concrete visitor on this specific node type.
    void accept(visitor& v) const override
    {
        v.visit(*this);
    }
};

/**
 * @brief Read-only node.
 */
struct ro_dnode : public dnode
{
    /// @brief Default constructor. Node's destroy callback
    /// and ID are unused.
    ro_dnode() 
        : dnode()
    {

    }

    /// @brief Constructor assigns supplied ID to node. Node's 
    /// destroy callback unused.
    ro_dnode(ecu_object_id id_)
        : dnode(std::move(id_))
    {

    }

    /// @brief Constructor assigns supplied destroy callback and 
    /// optional ID to ecu_dnode. Function takes in the C-structure
    /// under test (ecu_dnode *) to be compatible with the C API and
    /// to avoid having to explicitly upcast. Do not convert into 
    /// the C++ base wrapper class (dnode *), as ecu_dnode_destroy() will
    /// have already been called on the node, thus invalidating it.
    ro_dnode(void (*destroy_)(ecu_dnode *, ecu_object_id), ecu_object_id id_ = ECU_OBJECT_ID_UNUSED)
        : dnode(std::move(destroy_), std::move(id_))
    {
        
    }

    /// @brief For now, copy semantics are removed to simplify test class.
    ro_dnode(const ro_dnode& other) = delete;

    /// @brief For now, copy semantics are removed to simplify test class.
    ro_dnode& operator=(const ro_dnode& other) = delete;

    /// @brief Move semantics are required if an rvalue node is passed
    /// into @ref dlist class constructor. The rvalue is moved onto the
    /// heap so it remains in memory and stored in a vector of pointers.
    ro_dnode(ro_dnode&& other) noexcept
        : dnode(std::move(other))
    {
        /* Currently no additional members to move. */
    }

    /// @brief Only move constructor currently used so delete move 
    /// assignment operator.
    ro_dnode& operator=(ro_dnode&& other) = delete;

    /// @brief Runs concrete visitor on this specific node type.
    void accept(visitor& v) override
    {
        v.visit(*this);
    }

    /// @brief Runs concrete visitor on this specific node type.
    void accept(visitor& v) const override
    {
        v.visit(*this);
    }
};

/*------------------------------------------------------------*/
/*-------------------- FILE-SCOPE TYPES - LIST ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief C++ wrapper class around C structure ecu_dlist 
 * under test. Used to facilitate easier testing.
 */
struct dlist : public ecu_dlist
{
    /// @brief Default constructor. Constructs empty list.
    dlist()
    {
        ecu_dlist_ctor(this);
    }

    /// @brief Construct list with supplied nodes. Nodes are
    /// added to the list in the order they are supplied.
    /// If supplied node is an lvalue it is directly added.
    /// If supplied node is an rvalue memory is allocated
    /// for it on the heap then it is added to the list.
    template<typename ...Nodes>
    requires (std::is_base_of_v<dnode, std::remove_reference_t<Nodes>> && ...)
    dlist(Nodes&&... n)
    {
        ecu_dlist_ctor(this);
        add(std::forward<Nodes>(n)...); /* add(Nodes&&... n). */
    }

    /// @brief Virtual base class destructor. Frees any
    /// nodes allocated on the heap. C-destructor
    /// ecu_dlist_destroy() not called since tests call
    /// it directly. Do not place in C++ destructor or else
    /// ecu_dlist_destroy() is called twice, first in the test
    /// and then in this destructor. The 2nd call causes
    /// problems because an invalidated list (next, prev, etc
    /// reset to NULL from first ecu_dlist_destroy call) is 
    /// passed, causing ecu_dlist_valid() to fail. This is
    /// meant to happen since C-destructor under test does
    /// not use dynamic memory allocation, so it only invalidates
    /// the object.
    virtual ~dlist()
    {
        /* Free memory for any rvalue nodes that were passed to the constructor. */
        for (auto *n : m_nodes)
        {
            delete n;
        }
    }

    /// @brief Adds nodes to back of list in order they are supplied
    /// I.e. add(1, 2, 3) produces list [1, 2, 3]. If a supplied node 
    /// is an lvalue it is directly added. If a supplied node is an 
    /// rvalue memory is allocated for it on the heap then it is
    /// added to the list.
    template<typename... Nodes>
    void add(Nodes&&... n)
    {
        static_assert( (sizeof...(Nodes) > 0), "At least one node must be supplied." );
        (add(std::forward<Nodes>(n)), ...); /* add(T& node) or add(T&& node) depending on overload resolution. */
    }

    /// @brief Add node to the back of the list.
    template<typename T>
    requires std::is_base_of_v<dnode, T> /* T should never be reference type since parameter passed by reference. */
    void add(T& node)
    {
        ecu_dlist_push_back(this, &node);
    }

    /// @brief Allocate memory for node and add it to the back
    /// of the list.
    template<typename T>
    requires std::is_base_of_v<dnode, T> /* T should never be reference type due to add(T&) overload. */
    void add(T&& node)
    {
        m_nodes.push_back(new T(std::move(node))); /* Allocate memory for new node. */
        ecu_dlist_push_back(this, m_nodes.back()); /* Add node to back of list. */
    }

    /// @brief Return node at specified position. Index of 0
    /// returns first node after HEAD. Supplied index must be
    /// less than or equal to the number of nodes in the list.
    dnode& at(std::size_t i)
    {
        assert( (i < ecu_dlist_size(this)) );
        struct ecu_dlist_iterator iter;
        ecu_dnode *pos = nullptr;
        std::size_t n = 0;

        ECU_DLIST_FOR_EACH(node, &iter, this)
        {
            if (n == i)
            {
                pos = node;
                break;
            }
            else
            {
                n++;
            }
        }

        return convert(pos);
    }

    /// @brief Return node at specified position. Index of 0
    /// returns first node after HEAD. Supplied index must be
    /// less than or equal to the number of nodes in the list.
    const dnode& at(std::size_t i) const
    {
        assert( (i < ecu_dlist_size(this)) );
        struct ecu_dlist_citerator citer;
        const ecu_dnode *pos = nullptr;
        std::size_t n = 0;

        ECU_DLIST_CONST_FOR_EACH(node, &citer, this)
        {
            if (n == i)
            {
                pos = node;
                break;
            }
            else
            {
                n++;
            }
        }

        return convert(pos);
    }

    /// @brief Applies concrete visitor over all nodes currently 
    /// in the list.
    template<typename T>
    requires std::is_base_of_v<visitor, std::remove_reference_t<T>>
    void accept(T&& v)
    {
        struct ecu_dlist_iterator iter;

        ECU_DLIST_FOR_EACH(n, &iter, this)
        {
            convert(n).accept(v);
        }
    }

    /// @brief Pointer to dnode so vector can store multiple types of nodes.
    /// Objects pointed to are allocated on the heap.
    std::vector<dnode *> m_nodes;
};

/*------------------------------------------------------------*/
/*------------- FILE-SCOPE TYPES - CONCRETE VISITORS ---------*/
/*------------------------------------------------------------*/

/**
 * @brief Concrete visitor that calls node in list mock
 * expectation. Applies to all node types.
 */
struct node_in_list_call : public visitor
{
    /// @brief Calls node in list mock expectation. 
    /// Takes in base C++ wrapper to make this a common function.
    static void node_in_list_actual_call(const dnode &n)
    {
        /* Note how n is explicitly upcasted to (const ecu_dnode *) to standardize
        pointers passed in. It is NOT guaranteed (const ecu_dnode *) == (const dnode *). 
        Do const-qualified cast to allow const and non-const parameters. */
        mock().actualCall("node_in_list")
              .withParameter("value", static_cast<const void *>((const ecu_dnode *)&n));
    }

    /// @brief Calls node in list mock expectation on non-const read-write node.
    void visit(rw_dnode& n) override
    {
        node_in_list_actual_call(n);
    }

    /// @brief Calls node in list mock expectation on const read-write node.
    void visit(const rw_dnode& n) override
    {
        node_in_list_actual_call(n);
    }

    /// @brief Calls node in list mock expectation on read-only node.
    void visit(const ro_dnode& n) override
    {
        node_in_list_actual_call(n);
    }
};

/**
 * @brief Concrete visitor that calls node sorted mock
 * expectation. Applies to all node types.
 */
struct node_sorted_call : public visitor
{
    /// @brief Calls node sorted mock expectation. 
    /// Takes in base node class to make this a common function.
    static void node_sorted_actual_call(const dnode& n)
    {
        mock().actualCall("node_sorted")
              .withParameter("value", ecu_dnode_id(&n));
    }

    /// @brief Calls node sorted mock expectation on non-const read-write node.
    void visit(rw_dnode& n) override
    {
        node_sorted_actual_call(n);
    }

    /// @brief Calls node sorted mock expectation on const read-write node.
    void visit(const rw_dnode& n) override
    {
        node_sorted_actual_call(n);
    }

    /// @brief Calls node sorted mock expectation on read-only node.
    void visit(const ro_dnode& n) override
    {
        node_sorted_actual_call(n);
    }
};

/**
 * @brief Concrete visitor that removes node. Only applies
 * to editable nodes. I.e. non-const read-write nodes are
 * removed but read-only nodes are not.
 */
struct node_remove : public visitor
{
    /// @brief Removes non-const read-write node from the list.
    void visit(rw_dnode& n) override
    {
        ecu_dnode_remove(&n);
    }

    /// @brief Does nothing since const read-write node supplied.
    void visit(const rw_dnode& n) override
    {
        (void)n;
    }

    /// @brief Does nothing since read-only node supplied.
    void visit(const ro_dnode& n) override
    {
        (void)n;
    }
};

/*------------------------------------------------------------*/
/*---------------- FILE-SCOPE FUNCTION DEFINITIONS -----------*/
/*------------------------------------------------------------*/

dnode& convert(ecu_dnode *n)
{
    assert( (n) );
    assert( (ecu_dnode_valid(n)) );
    return (*static_cast<dnode *>(n));
}


const dnode& convert(const ecu_dnode *n)
{
    assert( (n) );
    assert( (ecu_dnode_valid(n)) );
    return (*static_cast<const dnode *>(n));
}
} /* namespace. */

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(DList)
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

    /// @brief Creates node_in_list mock expectations in the order
    /// that was supplied. Note how only lvalue references
    /// can be supplied. Do not allow temporaries since object
    /// lifetime must outlive the mock.
    template<typename... Nodes>
    requires (std::is_base_of_v<dnode, Nodes> && ...) /* Nodes type will never be reference since parameter passed by reference. */
    static void EXPECT_NODES_IN_ORDER(dnode& n1, Nodes&... n)
    {
        /* Note how parameters are explicitly upcasted to (const ecu_dnode *) to standardize
        pointers passed in. It is NOT guaranteed (const ecu_dnode *) == (const dnode *). 
        Do const-qualified cast to allow const and non-const parameters. */
        mock().strictOrder();
        mock().expectOneCall("node_in_list")
              .withParameter("value", static_cast<const void *>((const ecu_dnode *)&n1));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock().expectOneCall("node_in_list")
                   .withParameter("value", static_cast<const void *>((const ecu_dnode *)&n)), ...);
        }
    }

    /// @brief Creates node_sorted mock expectations in the order
    /// that was supplied. Note how only lvalue references
    /// can be supplied. Do not allow temporaries since object
    /// lifetime must outlive the mock.
    template<typename... Nodes>
    requires (std::is_base_of_v<dnode, Nodes> && ...) /* Nodes type will never be reference since parameter passed by reference. */
    static void EXPECT_NODE_SORTING_ORDER(dnode& n1, Nodes&... n)
    {
        mock().strictOrder();
        mock().expectOneCall("node_sorted")
              .withParameter("value", ecu_dnode_id(&n1));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock().expectOneCall("node_sorted")
                   .withParameter("value", ecu_dnode_id(&n)), ...);
        }
    }

    /// @brief Creates node_destroy mock expectations in the order
    /// that was supplied. Note how only lvalue references
    /// can be supplied. Do not allow temporaries since object
    /// lifetime must outlive the mock.
    template<typename... Nodes>
    requires (std::is_base_of_v<dnode, Nodes> && ...) /* Nodes type will never be reference since parameter passed by reference. */
    static void EXPECT_NODES_DESTROYED(dnode& n1, Nodes&... n)
    {
        /* Note how parameters are explicitly upcasted to (const ecu_dnode *) to standardize
        pointers passed in. It is NOT guaranteed (const ecu_dnode *) == (const dnode *). 
        Do const-qualified cast to allow const and non-const parameters. */
        mock().strictOrder();
        mock().expectOneCall("node_destroy")
              .withParameter("value", static_cast<const void *>((const ecu_dnode *)&n1));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock().expectOneCall("node_destroy")
                   .withParameter("value", static_cast<const void *>((const ecu_dnode *)&n)), ...);
        }
    }

    /// @brief Destroy callback assigned to some nodes under test.
    /// Executes when node is destroyed by C destroy functions under test.
    static void node_destroy_actual_call(ecu_dnode *node, ecu_object_id id)
    {
        (void)id;
        mock().actualCall("node_destroy")
              .withParameter("value", static_cast<const void *>(node));
    }

    /**
     * @brief Used to test GET_ENTRY() macros. Types
     * chosen in an attempt to create non-uniform
     * memory layout of the test struct.
     */
    struct intrusive_node
    {
        intrusive_node()
        {
            ecu_dnode_ctor(&dnode, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        }

        std::uint8_t a{1};
        ecu_dnode dnode;
        std::uint16_t b{2};
        std::uint16_t c{3};
    };

    /**
     * @brief Used to test GET_ENTRY() macros. Types
     * chosen in an attempt to create non-uniform
     * memory layout of the test struct.
     */
    struct nested_intrusive_node
    {
        nested_intrusive_node()
        {
            ecu_dnode_ctor(&group.dnode, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        }

        std::uint16_t a{1};
        struct 
        {
            std::uint8_t b{2};
            ecu_dnode dnode;
            std::uint16_t c{3};
        } group;
        std::uint16_t d{4};
    };

    std::array<ro_dnode, 50> RO;    /**< Read-only nodes. */
    std::array<rw_dnode, 50> RW;    /**< Read-write nodes. */
};

// /*------------------------------------------------------------*/
// /*---------------- STATIC FUNCTION DECLARATIONS --------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief Mock expectation to set in beginning of test. Call if 
//  * node's destroy callback, @ref node_destroy_mock, is expected
//  * to be called. Provides a more readable expectation and allows 
//  * the string passed to expectOneCall() to be contained within this 
//  * macro in case it changes.
//  */
// static void EXPECT_NODE_DESTROYED(ecu_dnode *node);

// /**
//  * @brief Mock expectation to set in beginning of test. Call
//  * if node should still be in the specified list after the test 
//  * completes. Verify via call to @ref list_order_check_expectations(). Provides a 
//  * more readable expectation and allows the string passed to 
//  * expectOneCall() to be contained within this macro in case it 
//  * changes.
//  */
// static void EXPECT_NODE_IN_LIST(const ecu_dlist *list, const ecu_dnode *node);

// /**
//  * @brief Actual mock call to node's destroy callback.
//  */
// static void node_destroy_mock(ecu_dnode *node, ecu_object_id id);

// /**
//  * @brief Actual mock call that node is in the list.
//  */
// static void node_in_list_mock(const ecu_dlist *list, const ecu_dnode *node);

// /**
//  * @brief Iterates over all nodes in a list and calls 
//  * @ref node_in_list_mock() on each node. Provides a portable way
//  * to verify nodes are in a list via dlist API.
//  */
// static void list_order_check_expectations(const ecu_dlist *list);

// /*------------------------------------------------------------*/
// /*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
// /*------------------------------------------------------------*/

// static void EXPECT_NODE_DESTROYED(ecu_dnode *node)
// {
//     assert( (node) );
//     mock().expectOneCall("node_destroy_mock")
//           .withParameter("node", node);
// }

// static void EXPECT_NODE_IN_LIST(const ecu_dlist *list, const ecu_dnode *node)
// {
//     assert( (list && node) );
//     mock().expectOneCall("node_in_list_mock")
//           .withParameter("list", list)
//           .withParameter("node", node);
// }

// static void node_destroy_mock(ecu_dnode *node, ecu_object_id id)
// {
//     (void)id;
//     mock().actualCall(__func__)
//           .withParameter("node", node);
// }

// static void node_in_list_mock(const ecu_dlist *list, const ecu_dnode *node)
// {
//     mock().actualCall("node_in_list_mock")
//           .withParameter("list", list)
//           .withParameter("node", node);
// }

// static void list_order_check_expectations(const ecu_dlist *list)
// {
//     assert( (list) );
//     ecu_dlist_citerator citerator;

//     ECU_DLIST_CONST_FOR_EACH(i, &citerator, list)
//     {
//         node_in_list_mock(list, i);
//     }
// }

// /*------------------------------------------------------------*/
// /*----------------------- TEST GROUPS ------------------------*/
// /*------------------------------------------------------------*/

// TEST_GROUP(DListMacros)
// {
//     struct composite_node
//     {
//         std::uint32_t a{5};
//         ecu_dnode node1;
//         std::uint8_t b{10};
//         ecu_dnode node2;
//         std::uint16_t c{15};
//     };

//     composite_node m_node;
// };

// TEST_GROUP(DListDNodeCtors)
// {
//     void setup() override
//     {
//         set_assert_handler(AssertResponse::FAIL);
//     }

//     void teardown() override
//     {
//         mock().checkExpectations();
//         mock().clear();
//     }

//     void setup_list(void (*destroy1)(ecu_dnode *, ecu_object_id) = ECU_DNODE_DESTROY_UNUSED,
//                     void (*destroy2)(ecu_dnode *, ecu_object_id) = ECU_DNODE_DESTROY_UNUSED,
//                     void (*destroy3)(ecu_dnode *, ecu_object_id) = ECU_DNODE_DESTROY_UNUSED)
//     {
//         ecu_dlist_ctor(&m_list);
//         ecu_dnode_ctor(&m_node1, destroy1, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_node2, destroy2, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_node3, destroy3, ECU_OBJECT_ID_UNUSED);
//         ecu_dlist_push_back(&m_list, &m_node1);
//         ecu_dlist_push_back(&m_list, &m_node2);
//         ecu_dlist_push_back(&m_list, &m_node3);
//     }

//     ecu_dlist m_list;
//     ecu_dnode m_node1;
//     ecu_dnode m_node2;
//     ecu_dnode m_node3;
// };

// TEST_GROUP(DListBase)
// {
//     void setup() override
//     {
//         set_assert_handler(AssertResponse::FAIL);

//         ecu_dlist_ctor(&m_list);
//         ecu_dlist_ctor(&m_other_list);
//         ecu_dnode_ctor(&m_node1, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_node2, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_node3, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_node4, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_node5, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_inserted_node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
//         ecu_dnode_ctor(&m_node_not_in_list, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
//     }

//     void teardown() override
//     {
//         mock().checkExpectations();
//         mock().clear();
//     }

//     ecu_dlist_iterator m_iterator;
//     ecu_dlist_citerator m_citerator;
//     ecu_dlist m_list;
//     ecu_dlist m_other_list;
//     ecu_dnode m_node1;
//     ecu_dnode m_node2;
//     ecu_dnode m_node3;
//     ecu_dnode m_node4;
//     ecu_dnode m_node5;
//     ecu_dnode m_inserted_node;
//     ecu_dnode m_node_not_in_list;
// };

// TEST_GROUP_BASE(DNode, TEST_GROUP_CppUTestGroupDListBase)
// {
//     void setup() override
//     {
//         TEST_GROUP_CppUTestGroupDListBase::setup();

//         /* HEAD, 1, 2, 3. Use dnode_insert() functions instead of push_back()
//         since this module tests insert() functions. Avoids dependencies. */
//         ecu_dnode_insert_after(&m_list.head, &m_node1);
//         ecu_dnode_insert_after(&m_node1, &m_node2);
//         ecu_dnode_insert_after(&m_node2, &m_node3);
//     }
// };

// TEST_GROUP_BASE(DList, TEST_GROUP_CppUTestGroupDListBase)
// {
//     void setup() override
//     {
//         TEST_GROUP_CppUTestGroupDListBase::setup();

//         ecu_dlist_push_back(&m_list, &m_node1);
//         ecu_dlist_push_back(&m_list, &m_node2);
//         ecu_dlist_push_back(&m_list, &m_node3);
//     }
// };

// TEST_GROUP(DListInsertBefore)
// {
//     void setup() override
//     {
//         set_assert_handler(AssertResponse::FAIL);

//         ecu_dlist_ctor(&m_list);
//         ecu_dlist_ctor(&m_other_list);
//         ecu_dlist_push_back(&m_list, &m_node1);
//         ecu_dlist_push_back(&m_list, &m_node2);
//         ecu_dlist_push_back(&m_list, &m_node3);
//     }

//     void teardown() override
//     {
//         mock().checkExpectations();
//         mock().clear();
//     }

//     static void EXPECT_NODE_CONDITION(const ecu_dnode *node, const ecu_dnode *position, void *data)
//     {
//         mock().expectOneCall("condition_mock_false")
//               .withParameter("node", node)
//               .withParameter("position", position)
//               .withParameter("data", data);
//     }

//     static bool condition(const ecu_dnode *node, const ecu_dnode *position, void *data)
//     {
//         assert( (node && position) );
//         (void)data;
//         const dnode *n = static_cast<const dnode *>(node);
//         const dnode *p = static_cast<const dnode *>(position);

//         if (n->m_val <= p->m_val)
//         {
//             return true;
//         }
//         else
//         {
//             return false;
//         }
//     }

//     static bool condition_stub_false(const ecu_dnode *node, const ecu_dnode *position, void *data)
//     {
//         (void)node;
//         (void)position;
//         (void)data;
//         return false;
//     }

//     static bool condition_mock_false(const ecu_dnode *node, const ecu_dnode *position, void *data)
//     {
//         assert( (node && position && data) );
//         mock().actualCall(__func__)
//               .withParameter("node", node)
//               .withParameter("position", position)
//               .withParameter("data", data);
//         return false;
//     }

//     ecu_dlist m_list;
//     ecu_dlist m_other_list;
//     dnode m_node1{1};
//     dnode m_node2{2};
//     dnode m_node3{3};
//     dnode m_inserted_node;
//     int m_data;
// };

// TEST_GROUP(DListSort)
// {
//     void setup() override
//     {
//         set_assert_handler(AssertResponse::FAIL);
//         ecu_dlist_ctor(&m_list);

//         for (std::size_t i = 0; i < NUM_NODES; ++i)
//         {
//             m_node1[i].set_value(1);
//             m_node2[i].set_value(2);
//             m_node3[i].set_value(3);
//             m_node4[i].set_value(4);
//             m_node5[i].set_value(5);
//             m_node6[i].set_value(6);
//         }
//     }

//     void teardown() override
//     {
//         mock().checkExpectations();
//         mock().clear();
//     }

//     static bool lhs_less_than_rhs(const ecu_dnode *lhs, const ecu_dnode *rhs, void *data)
//     {
//         assert( (lhs && rhs) );
//         (void)data;
//         const dnode *tlhs = static_cast<const dnode *>(lhs);
//         const dnode *trhs = static_cast<const dnode *>(rhs);

//         if (tlhs->get_value() < trhs->get_value())
//         {
//             return true;
//         }
//         else 
//         {
//             return false;
//         }
//     }

//     static void EXPECT_NODE_VALUE(decltype(dnode::m_val) val)
//     {
//         mock().expectOneCall("node_value_mock")
//               .withParameter("val", val);
//     }

//     void list_sort_check_expectations()
//     {
//         ecu_dlist_citerator citerator;

//         ECU_DLIST_CONST_FOR_EACH(i, &citerator, &m_list)
//         {
//             const dnode *n = static_cast<const dnode *>(i);
//             mock().actualCall("node_value_mock")
//                   .withParameter("val", n->get_value());
//         }
//     } 

//     static constexpr std::size_t NUM_NODES = 3; /* Number of nodes with same value. */
//     ecu_dlist m_list;
//     dnode m_node1[NUM_NODES];
//     dnode m_node2[NUM_NODES];
//     dnode m_node3[NUM_NODES];
//     dnode m_node4[NUM_NODES];
//     dnode m_node5[NUM_NODES];
//     dnode m_node6[NUM_NODES];
// };

/*------------------------------------------------------------*/
/*----------------- TESTS - DLIST MACRO TESTS ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Macro successfully converts dnode into
 * user-defind node.
 */
TEST(DList, DNodeGetEntry)
{
    /* Step 1: Arrange. */
    intrusive_node n1;
    nested_intrusive_node n2;

    /* Step 2: Action. */
    intrusive_node *n1_entry = ECU_DNODE_GET_ENTRY(&n1.dnode, struct intrusive_node, dnode);
    const intrusive_node *n1_const_entry = ECU_DNODE_GET_CONST_ENTRY(&n1.dnode, struct intrusive_node, dnode);
    nested_intrusive_node *n2_entry = ECU_DNODE_GET_ENTRY(&n2.group.dnode, struct nested_intrusive_node, group.dnode);
    const nested_intrusive_node *n2_const_entry = ECU_DNODE_GET_CONST_ENTRY(&n2.group.dnode, struct nested_intrusive_node, group.dnode);

    /* Step 3: Assert. */
    POINTERS_EQUAL(&n1, n1_entry);
    POINTERS_EQUAL(&n1, n1_const_entry);
    POINTERS_EQUAL(&n2, n2_entry);
    POINTERS_EQUAL(&n2, n2_const_entry);
}

/*------------------------------------------------------------*/
/*--------- TESTS - DNODE CONSTRUCTOR AND DESTROY ------------*/
/*------------------------------------------------------------*/

/**
 * @brief Node's destroy callback executes and is
 * removed from the list.
 */
TEST(DList, DNodeDestroy)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), rw_dnode(&node_destroy_actual_call), RW.at(1)};
        EXPECT_NODES_DESTROYED(list.at(1));
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1)); /* Expect these nodes to remain after test node destroyed. */

        /* Step 2: Action. */
        ecu_dnode_destroy(&list.at(1));

        /* Step 3: Assert. Test fails if node destroy callback not called or list not intact. */
        list.accept(node_in_list_call());
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

/**
 * @brief General test. Correct ID returned.
 */
TEST(DList, DNodeID)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{rw_dnode(0), rw_dnode(1), rw_dnode(2)};

        /* Steps 2 and 3: Action and assert. */
        LONGS_EQUAL(0, ecu_dnode_id(&list.at(0)));
        LONGS_EQUAL(1, ecu_dnode_id(&list.at(1)));
        LONGS_EQUAL(2, ecu_dnode_id(&list.at(2)));
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DNODE IN LIST ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief General test. Correctly returns if node is in
 * list or not.
 */
TEST(DList, DNodeInList)
{
    try 
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0)};

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_dnode_in_list(&RW.at(0))) );
        ecu_dnode_remove(&RW.at(0));
        CHECK_FALSE( (ecu_dnode_in_list(&RW.at(0))) );
        ecu_dlist_push_back(&list, &RW.at(0));
        CHECK_TRUE( (ecu_dnode_in_list(&RW.at(0))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Supplied node cannot be HEAD.
 */
TEST(DList, DNodeInListNodeIsHead)
{
    try 
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Steps 2 Action. */
        (void)ecu_dnode_in_list(&list.head);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*---------------- TESTS - DNODE INSERT AFTER ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Position node is front (first node after HEAD).
 * Added node should be placed one after front.
 */
TEST(DList, DNodeInsertAfterPosIsFront)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(2)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(2));

        /* Step 2: Action. */
        ecu_dnode_insert_after(&RW.at(0), &RW.at(1));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Position node is in the middle of a list.
 */
TEST(DList, DNodeInsertAfterPosIsMiddle)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(3), RW.at(4)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Step 2: Action. */
        ecu_dnode_insert_after(&RW.at(1), &RW.at(2));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Position node is tail. Added node should
 * be placed at the end of the list.
 */
TEST(DList, DNodeInsertAfterPosIsTail)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(2), RW.at(3));

        /* Step 2: Action. */
        ecu_dnode_insert_after(&RW.at(2), &RW.at(3));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position node must be within a list.
 */
TEST(DList, DNodeInsertAfterPosNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_after(&RW.at(0), &RW.at(1));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Added node cannot already be within a list.
 */
TEST(DList, DNodeInsertAfterNodeInList)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list1{RW.at(0)};
        dlist list2{RW.at(1)};
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_after(&RW.at(0), &RW.at(1));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position cannot be HEAD.
 */
TEST(DList, DNodeInsertAfterPosIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_after(&list.head, &RW.at(0));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Added node cannot be HEAD.
 */
TEST(DList, DNodeInsertAfterNodeIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0)};
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_after(&RW.at(0), &list.head);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position cannot equal node
 * being added.
 */
TEST(DList, DNodeInsertAfterPosEqualsNode)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0)};
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_after(&RW.at(0), &RW.at(0));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*---------------- TESTS - DNODE INSERT BEFORE ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Position node is front (first node after HEAD).
 * Added node should placed at the front.
 */
TEST(DList, DNodeInsertBeforePosIsFront)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(1), RW.at(2)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(2));

        /* Step 2: Action. */
        ecu_dnode_insert_before(&RW.at(1), &RW.at(0));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Position node is in the middle of a list.
 */
TEST(DList, DNodeInsertBeforePosIsMiddle)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(3), RW.at(4)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Step 2: Action. */
        ecu_dnode_insert_before(&RW.at(3), &RW.at(2));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Position node is tail. Added node should be
 * placed one before tail.
 */
TEST(DList, DNodeInsertBeforePosIsTail)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(3)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(2), RW.at(3));

        /* Step 2: Action. */
        ecu_dnode_insert_before(&RW.at(3), &RW.at(2));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position node must be within a list.
 */
TEST(DList, DNodeInsertBeforePosNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_before(&RW.at(0), &RW.at(1));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Added node cannot already be within a list.
 */
TEST(DList, DNodeInsertBeforeNodeInList)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list1{RW.at(0)};
        dlist list2{RW.at(1)};
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_before(&RW.at(0), &RW.at(1));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position cannot be HEAD.
 */
TEST(DList, DNodeInsertBeforePosIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_before(&list.head, &RW.at(0));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Added node cannot be HEAD.
 */
TEST(DList, DNodeInsertBeforeNodeIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0)};
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_before(&RW.at(0), &list.head);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position node cannot equal node
 * being added.
 */
TEST(DList, DNodeInsertBeforePosEqualsNode)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0)};
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_insert_before(&RW.at(0), &RW.at(0));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DNODE NEXT CNEXT ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief General test. Next node should be returned.
 */
TEST(DList, DNodeNext)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2), RW.at(3)};

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( &RW.at(2), &convert(ecu_dnode_next(&RW.at(1))) );
        POINTERS_EQUAL( &RW.at(2), &convert(ecu_dnode_cnext(&RW.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Supplied node cannot be HEAD.
 */
TEST(DList, DNodeNextNodeIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        (void)ecu_dnode_next(&list.head);

        /* Step 3: Assert. Test fails if assertion does nto fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Supplied node cannot be HEAD.
 */
TEST(DList, DNodeCNextNodeIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        (void)ecu_dnode_cnext(&list.head);

        /* Step 3: Assert. Test fails if assertion does nto fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Supplied node is the tail. NULL should be
 * returned.
 */
TEST(DList, DNodeNextNodeIsTail)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2)};

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( nullptr, ecu_dnode_next(&RW.at(2)) );
        POINTERS_EQUAL( nullptr, ecu_dnode_cnext(&RW.at(2)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Supplied node is not in a list. NULL should
 * be returned.
 */
TEST(DList, DNodeNextNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_FALSE( (ecu_dnode_in_list(&RW.at(0))) ); /* Precondition. */

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( nullptr, ecu_dnode_next(&RW.at(0)) );
        POINTERS_EQUAL( nullptr, ecu_dnode_cnext(&RW.at(0)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DNODE PREV CPREV ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief General test. Prev node should be returned.
 */
TEST(DList, DNodePrev)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2), RW.at(3)};

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( &RW.at(1), &convert(ecu_dnode_prev(&RW.at(2))) );
        POINTERS_EQUAL( &RW.at(1), &convert(ecu_dnode_cprev(&RW.at(2))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Supplied node cannot be HEAD.
 */
TEST(DList, DNodePrevNodeIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        (void)ecu_dnode_prev(&list.head);

        /* Step 3: Assert. Test fails if assertion does nto fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Supplied node cannot be HEAD.
 */
TEST(DList, DNodeCPrevNodeIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        (void)ecu_dnode_cprev(&list.head);

        /* Step 3: Assert. Test fails if assertion does nto fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Supplied node is the first (one after HEAD). 
 * NULL should be returned.
 */
TEST(DList, DNodePrevNodeIsFirst)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2)};

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( nullptr, ecu_dnode_prev(&RW.at(0)) );
        POINTERS_EQUAL( nullptr, ecu_dnode_cprev(&RW.at(0)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Supplied node is not in a list. NULL should
 * be returned.
 */
TEST(DList, DNodePrevNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_FALSE( (ecu_dnode_in_list(&RW.at(0))) ); /* Precondition. */

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( nullptr, ecu_dnode_prev(&RW.at(0)) );
        POINTERS_EQUAL( nullptr, ecu_dnode_cprev(&RW.at(0)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------------- TESTS - DNODE REMOVE ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief General removal test.
 */
TEST(DList, DNodeRemove)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(2));

        /* Step 2: Action. */
        ecu_dnode_remove(&RW.at(1));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove and readd nodes in different locations.
 */
TEST(DList, DNodeRemoveAndReAddNodes)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2)};
        EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(2), RW.at(1));

        /* Step 2: Action. */
        ecu_dnode_remove(&RW.at(1));
        ecu_dnode_insert_after(&RW.at(2), &RW.at(1));

        /* Step 3: Assert. */
        list.accept(node_in_list_call());
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Removing a node not in a list is allowed.
 * Assertion should NOT fire.
 */
TEST(DList, DNodeRemoveNodeNotInList)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_FALSE( (ecu_dnode_in_list(&RW.at(0))) ); /* Precondition. */

        /* Step 2: Action. */
        ecu_dnode_remove(&RW.at(0));

        /* Step 3: Assert. Test fails if assertion fires. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Supplied node cannot be HEAD.
 */
TEST(DList, DNodeRemoveNodeIsHead)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_dnode_remove(&list.head);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------------- TESTS - DNODE VALID ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief General test. Dnode is not valid until it 
 * has been constructed. Dnode invalidated when it is
 * destroyed.
 */
TEST(DList, DNodeValid)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_dnode node;

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_dnode_valid(&node)) );
        ecu_dnode_ctor(&node, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        CHECK_TRUE( (ecu_dnode_valid(&node)) );
        ecu_dnode_destroy(&node);
        CHECK_FALSE( (ecu_dnode_valid(&node)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------- TESTS - DLIST CONSTRUCTOR AND DESTROY ----------*/
/*------------------------------------------------------------*/

/**
 * @brief All node destroy callbacks execute.
 */
TEST(DList, DListDestroy)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{rw_dnode(&node_destroy_actual_call), rw_dnode(&node_destroy_actual_call),
                   rw_dnode(&node_destroy_actual_call), rw_dnode(&node_destroy_actual_call)};
        EXPECT_NODES_DESTROYED(list.at(0), list.at(1), list.at(2), list.at(3));

        /* Step 2: Action. */
        ecu_dlist_destroy(&list);

        /* Step 3: Assert. Test fails if all node destroy callbacks not called. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - DLIST BACK CBACK ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief General test. Tail node returned. Tail node 
 * is not popped.
 */
TEST(DList, DListBack)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list{RW.at(0), RW.at(1), RW.at(2)};

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( &RW.at(2), &convert(ecu_dlist_back(&list)) );
        POINTERS_EQUAL( &RW.at(2), &convert(ecu_dlist_cback(&list)) );
        CHECK_TRUE( (ecu_dnode_in_list(&RW.at(2))) ); /* Tail node not popped. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief NULL returned when empty list supplied.
 */
TEST(DList, DListBackListIsEmpty)
{
    try
    {
        /* Step 1: Arrange. */
        dlist list;

        /* Steps 2 and 3: Action and assert. */
        POINTERS_EQUAL( nullptr, ecu_dlist_back(&list) );
        POINTERS_EQUAL( nullptr, ecu_dlist_cback(&list) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

// /*------------------------------------------------------------*/
// /*--------------------- TESTS - DLIST CLEAR ------------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Verify all nodes removed from 
//  * list but their destructors are not called.
//  */
// TEST(DList, DListClear)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_node1);
        
//         /* Step 2: Action. */
//         ecu_dlist_clear(&m_list);
//         ecu_dlist_push_back(&m_other_list, &m_node1); /* Re-use removed node. */

//         /* Step 3: Assert. Fails if any node's are destroyed or removed
//         nodes can't be re-used. */
//         list_order_check_expectations(&m_list);
//         list_order_check_expectations(&m_other_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Verify nodes can be readded to list after list
//  * is cleared.
//  */
// TEST(DList, DListClearAndReAdd)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Assertion should not fire. Operation is valid to 
//  * do on an empty list.
//  */
// TEST(DList, DListClearListIsEmpty)
// {
//     try 
//     {
//         /* Step 1: Arrange. Precondition must be true to produce useful results. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
        
//         /* Step 2: Action. */
//         ecu_dlist_clear(&m_list);

//         /* Step 3: Assert. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*--------------------- TESTS - DLIST EMPTY ------------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Correctly returns when list
//  * is empty or not.
//  */
// TEST(DList, DListEmpty)
// {
//     try
//     {
//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE(ecu_dlist_empty(&m_list));
//         ecu_dlist_push_back(&m_list, &m_node1);
//         CHECK_FALSE(ecu_dlist_empty(&m_list));
//         ecu_dlist_push_back(&m_list, &m_node2);
//         CHECK_FALSE(ecu_dlist_empty(&m_list));
//         ecu_dnode_remove(&m_node1);
//         CHECK_FALSE(ecu_dlist_empty(&m_list));
//         ecu_dnode_remove(&m_node2);
//         CHECK_TRUE(ecu_dlist_empty(&m_list));
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*------------------ TESTS - DLIST FRONT CFRONT --------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Front node returned. Front 
//  * node is not popped.
//  */
// TEST(DList, DListFront)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_front(&m_list) == &m_node1) );
//         CHECK_TRUE( (ecu_dlist_cfront(&m_list) == &m_node1) );
//         list_order_check_expectations(&m_list); /* Front node not popped. */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief NULL returned when empty list supplied.
//  */
// TEST(DList, DListFrontListIsEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. Precondition must be true to produce useful results. */
//         ecu_dlist_clear(&m_list);
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_front(&m_list) == nullptr) );
//         CHECK_TRUE( (ecu_dlist_cfront(&m_list) == nullptr) );
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*----------------- TESTS - DLIST INSERT BEFORE --------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief Node added to correct position in middle of list 
//  * if a middle condition passes.
//  */
// TEST(DList, DListInsertBeforeMiddleConditionPasses)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);
//         m_inserted_node.set_value(2);

//         /* Step 2: Action. */
//         ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition, nullptr);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Node is added to front of list if first condition passes.
//  */
// TEST(DList, DListInsertBeforeFirstConditionPasses)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);
//         m_inserted_node.set_value(1);

//         /* Step 2: Action. */
//         ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition, nullptr);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Verify all nodes iterated over. Node must
//  * be added to the end of the list when all conditions
//  * are false.
//  */
// TEST(DList, DListInsertBeforeAllConditionsFalse)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);
//         EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);

//         /* Step 2: Action. */
//         ecu_dlist_insert_before(&m_list, &m_inserted_node, &condition_stub_false, nullptr);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Node added to end of list if list is empty. Condition 
//  * function should never be called since list is empty.
//  */
// TEST(DList, DListInsertBeforeListIsEmpty)
// {
//     try 
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

//         /* Step 2: Action. */
//         ecu_dlist_insert_before(&m_other_list, &m_inserted_node, &condition_mock_false, nullptr);

//         /* Step 3: Assert. Node added to end of empty list. Also fails if 
//         condition function ever called. */
//         list_order_check_expectations(&m_other_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. Node being added cannot
//  * already be in a list.
//  */
// TEST(DList, DListInsertBeforeNodeInList)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         ecu_dlist_push_back(&m_other_list, &m_inserted_node);

//         /* m_list nodes. */
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* m_other_list nodes. */
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

//         set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

//         /* Step 2: Action. Attempt to insert node from listA into listB. */
//         ecu_dlist_insert_before(&m_other_list, &m_node1, &condition_stub_false, nullptr);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }

//     /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
//     be in m_list. Do this outside try-catch. */
//     list_order_check_expectations(&m_list);
//     list_order_check_expectations(&m_other_list);
// }

// /**
//  * @brief Not allowed. Supplied node cannot be HEAD.
//  */
// TEST(DList, DListInsertBeforeNodeIsHead)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);
//         set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

//         /* Step 2: Action. Attempt to insert listB HEAD to listA. */
//         ecu_dlist_insert_before(&m_list, &m_other_list.head, &condition_stub_false, nullptr);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }

//     /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
//     Do this outside try-catch. */
//     list_order_check_expectations(&m_list);
// }

// /*------------------------------------------------------------*/
// /*------------------ TESTS - DLIST PUSH BACK -----------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Node should be added to back
//  * of the list.
//  */
// TEST(DList, DListPushBack)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);
//         EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);

//         /* Step 2: Action. */
//         ecu_dlist_push_back(&m_list, &m_inserted_node);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Node should be added to the back of the list.
//  */
// TEST(DList, DListPushBackListIsEmpty)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. Node being added cannot already
//  * be within a list.
//  */
// TEST(DList, DListPushBackNodeInList)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         ecu_dlist_push_back(&m_other_list, &m_inserted_node);

//         /* m_list nodes. */
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* m_other_list nodes. */
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

//         set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

//         /* Step 2: Action. Attempt to insert node from listA into listB. */
//         ecu_dlist_push_back(&m_other_list, &m_node1);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }

//     /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
//     be in m_list. Do this outside try-catch. */
//     list_order_check_expectations(&m_list);
//     list_order_check_expectations(&m_other_list);
// }

// /**
//  * @brief Not allowed. Node being added cannot be HEAD.
//  */
// TEST(DList, DListPushBackNodeIsHead)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

//         /* Step 2: Action. Attempt to insert listB HEAD to listA. */
//         ecu_dlist_push_back(&m_list, &m_other_list.head);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }

//     /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
//     Do this outside try-catch. */
//     list_order_check_expectations(&m_list);
// }

// /*------------------------------------------------------------*/
// /*------------------ TESTS - DLIST PUSH FRONT ----------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Node should be added to front
//  * of the list.
//  */
// TEST(DList, DListPushFront)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_inserted_node);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Step 2: Action. */
//         ecu_dlist_push_front(&m_list, &m_inserted_node);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Node should be added to the front of the list.
//  */
// TEST(DList, DListPushFrontListIsEmpty)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. Node being added cannot already
//  * be within a list.
//  */
// TEST(DList, DListPushFrontNodeInList)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         ecu_dlist_push_front(&m_other_list, &m_inserted_node);

//         /* m_list nodes. */
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* m_other_list nodes. */
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_inserted_node);

//         set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

//         /* Step 2: Action. Attempt to insert node from listA into listB. */
//         ecu_dlist_push_front(&m_other_list, &m_node1);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }

//     /* Step 3: Assert. Verify lists weren't changed. m_node1 should still
//     be in m_list. Do this outside try-catch. */
//     list_order_check_expectations(&m_list);
//     list_order_check_expectations(&m_other_list);
// }

// /**
//  * @brief Not allowed. Node being added cannot be HEAD.
//  */
// TEST(DList, DListPushFrontNodeIsHead)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         set_assert_handler(AssertResponse::OK); /* Must be before step 2. */

//         /* Step 2: Action. Attempt to insert listB HEAD to listA. */
//         ecu_dlist_push_front(&m_list, &m_other_list.head);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }

//     /* Step 3: Assert. Verify HEAD of listB was not added to listA. 
//     Do this outside try-catch. */
//     list_order_check_expectations(&m_list);
// }

// /*------------------------------------------------------------*/
// /*--------------------- TESTS - DLIST POP BACK ---------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Popped node returned. 
//  * Resulting list remains intact.
//  */
// TEST(DList, DListPopBack)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_pop_back(&m_list) == &m_node3) );
//         list_order_check_expectations(&m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief NULL returned.
//  */
// TEST(DList, DListPopBackListIsEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. Precondition must be true to produce useful results. */
//         ecu_dlist_clear(&m_list);
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_pop_back(&m_list) == nullptr) );
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Popped node returned. Resulting list is now empty.
//  */
// TEST(DList, DListPopBackListWithOneNode)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_dlist_clear(&m_list);
//         ecu_dlist_push_back(&m_list, &m_node1);
//         CHECK_TRUE( (ecu_dlist_size(&m_list) == 1) );

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_pop_back(&m_list) == &m_node1) );
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*-------------------- TESTS - DLIST POP FRONT ---------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Popped node returned. Resulting 
//  * list remains intact.
//  */
// TEST(DList, DListPopFront)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_pop_front(&m_list) == &m_node1) );
//         list_order_check_expectations(&m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief NULL returned.
//  */
// TEST(DList, DListPopFrontListIsEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. Precondition must be true to produce useful results. */
//         ecu_dlist_clear(&m_list);
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_pop_front(&m_list) == nullptr) );
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Popped node returned. Resulting list is now empty.
//  */
// TEST(DList, DListPopFrontListWithOneNode)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_dlist_clear(&m_list);
//         ecu_dlist_push_back(&m_list, &m_node1);
//         CHECK_TRUE( (ecu_dlist_size(&m_list) == 1) );

//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE( (ecu_dlist_pop_front(&m_list) == &m_node1) );
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*--------------------- TESTS - DLIST SIZE -------------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Verify number of nodes 
//  * returned is correct.
//  */
// TEST(DList, DListSize)
// {
//     try
//     {
//         /* Steps 2 and 3: Action and assert. */
//         CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)0);
//         ecu_dlist_push_back(&m_list, &m_node1);
//         CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)1);
//         ecu_dlist_push_back(&m_list, &m_node2);
//         CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)2);
//         ecu_dlist_push_front(&m_list, &m_node3);
//         CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)3);
//         ecu_dnode_remove(&m_node2);
//         CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)2);
//         ecu_dlist_clear(&m_list);
//         CHECK_TRUE(ecu_dlist_size(&m_list) == (std::size_t)0);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief 0 must be returned if empty list supplied.
//  */
// TEST(DList, DListSizeListIsEmpty)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*--------------------- TESTS - DLIST SORT -------------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief Each node in the test list has a unique value. 
//  * Number of list elements is an even number. Verify list 
//  * sorted correctly. 
//  */
// TEST(DList, DListSortUniqueSortEven)
// {
//     try 
//     {
//         /* Step 1: Arrange. List = 6, 2, 4, 1, 3, 5 */
//         ecu_dlist_push_back(&m_list, &m_node6[0]);
//         ecu_dlist_push_back(&m_list, &m_node2[0]);
//         ecu_dlist_push_back(&m_list, &m_node4[0]);
//         ecu_dlist_push_back(&m_list, &m_node1[0]);
//         ecu_dlist_push_back(&m_list, &m_node3[0]);
//         ecu_dlist_push_back(&m_list, &m_node5[0]);

//         mock().strictOrder();
//         EXPECT_NODE_VALUE(1);
//         EXPECT_NODE_VALUE(2);
//         EXPECT_NODE_VALUE(3);
//         EXPECT_NODE_VALUE(4);
//         EXPECT_NODE_VALUE(5);
//         EXPECT_NODE_VALUE(6);

//         /* Step 2: Action. */
//         ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

//         /* Step 3: Assert. Verify list correctly sorted. */
//         list_sort_check_expectations();
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Each node in the test list has a unique value. 
//  * Number of list elements is an odd number. Verify list 
//  * sorted correctly. 
//  */
// TEST(DList, DListSortUniqueSortOdd)
// {
//     try 
//     {
//         /* Step 1: Arrange. List = 2, 4, 5, 1, 6 */
//         ecu_dlist_push_back(&m_list, &m_node2[0]);
//         ecu_dlist_push_back(&m_list, &m_node4[0]);
//         ecu_dlist_push_back(&m_list, &m_node5[0]);
//         ecu_dlist_push_back(&m_list, &m_node1[0]);
//         ecu_dlist_push_back(&m_list, &m_node6[0]);

//         mock().strictOrder();
//         EXPECT_NODE_VALUE(1);
//         EXPECT_NODE_VALUE(2);
//         EXPECT_NODE_VALUE(4);
//         EXPECT_NODE_VALUE(5);
//         EXPECT_NODE_VALUE(6);

//         /* Step 2: Action. */
//         ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

//         /* Step 3: Assert. Verify list correctly sorted. */
//         list_sort_check_expectations();
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Nodes in test list can have the same values.
//  * Number of list elements is an even number. Verify list 
//  * sorted correctly. 
//  */
// TEST(DList, DListSortNonUniqueSortEven)
// {
//     try 
//     {
//         /* Step 1: Arrange. List = 5, 1, 2, 3, 6, 5, 1, 4, 2, 6 */
//         ecu_dlist_push_back(&m_list, &m_node5[0]);
//         ecu_dlist_push_back(&m_list, &m_node1[0]);
//         ecu_dlist_push_back(&m_list, &m_node2[0]);
//         ecu_dlist_push_back(&m_list, &m_node3[0]);
//         ecu_dlist_push_back(&m_list, &m_node6[0]);
//         ecu_dlist_push_back(&m_list, &m_node5[1]);
//         ecu_dlist_push_back(&m_list, &m_node1[1]);
//         ecu_dlist_push_back(&m_list, &m_node4[0]);
//         ecu_dlist_push_back(&m_list, &m_node2[1]);
//         ecu_dlist_push_back(&m_list, &m_node6[1]);

//         mock().strictOrder();
//         EXPECT_NODE_VALUE(1);
//         EXPECT_NODE_VALUE(1);
//         EXPECT_NODE_VALUE(2);
//         EXPECT_NODE_VALUE(2);
//         EXPECT_NODE_VALUE(3);
//         EXPECT_NODE_VALUE(4);
//         EXPECT_NODE_VALUE(5);
//         EXPECT_NODE_VALUE(5);
//         EXPECT_NODE_VALUE(6);
//         EXPECT_NODE_VALUE(6);

//         /* Step 2: Action. */
//         ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

//         /* Step 3: Assert. Verify list correctly sorted. */
//         list_sort_check_expectations();
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Nodes in test list can have the same values.
//  * Number of list elements is an odd number. Verify list 
//  * sorted correctly. 
//  */
// TEST(DList, DListSortNonUniqueSortOdd)
// {
//     try 
//     {
//         /* Step 1: Arrange. List = 1, 5, 3, 4, 5, 5, 6, 1, 4, 2, 6 */
//         ecu_dlist_push_back(&m_list, &m_node1[0]);
//         ecu_dlist_push_back(&m_list, &m_node5[0]);
//         ecu_dlist_push_back(&m_list, &m_node3[0]);
//         ecu_dlist_push_back(&m_list, &m_node4[0]);
//         ecu_dlist_push_back(&m_list, &m_node5[1]);
//         ecu_dlist_push_back(&m_list, &m_node5[2]);
//         ecu_dlist_push_back(&m_list, &m_node6[0]);
//         ecu_dlist_push_back(&m_list, &m_node1[1]);
//         ecu_dlist_push_back(&m_list, &m_node4[1]);
//         ecu_dlist_push_back(&m_list, &m_node2[0]);
//         ecu_dlist_push_back(&m_list, &m_node6[1]);

//         mock().strictOrder();
//         EXPECT_NODE_VALUE(1);
//         EXPECT_NODE_VALUE(1);
//         EXPECT_NODE_VALUE(2);
//         EXPECT_NODE_VALUE(3);
//         EXPECT_NODE_VALUE(4);
//         EXPECT_NODE_VALUE(4);
//         EXPECT_NODE_VALUE(5);
//         EXPECT_NODE_VALUE(5);
//         EXPECT_NODE_VALUE(5);
//         EXPECT_NODE_VALUE(6);
//         EXPECT_NODE_VALUE(6);

//         /* Step 2: Action. */
//         ecu_dlist_sort(&m_list, &lhs_less_than_rhs, nullptr);

//         /* Step 3: Assert. Verify list correctly sorted. */
//         list_sort_check_expectations();
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*--------------------- TESTS - DLIST SWAP -------------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief Normal swap.
//  */
// TEST(DList, DListSwapBothListsNotEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. me = [1, 2, 3]. other = [4, 5]. */
//         ecu_dlist_push_back(&m_list, &m_node1);
//         ecu_dlist_push_back(&m_list, &m_node2);
//         ecu_dlist_push_back(&m_list, &m_node3);
//         ecu_dlist_push_back(&m_other_list, &m_node4);
//         ecu_dlist_push_back(&m_other_list, &m_node5);

//         /* Step 1: Arrange. Expected lists after swap. me = [4, 5]. other = [1, 2, 3]. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node4);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node5);
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_node3);

//         /* Step 2: Action. */
//         ecu_dlist_swap(&m_list, &m_other_list);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_list);
//         list_order_check_expectations(&m_other_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Me list gets other's contents. Other list 
//  * becomes empty.
//  */
// TEST(DList, DListSwapMeListEmptyOtherListNotEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. me = []. other = [1, 2]. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) ); /* Precondition. */
//         ecu_dlist_push_back(&m_other_list, &m_node1);
//         ecu_dlist_push_back(&m_other_list, &m_node2);

//         /* Step 1: Arrange. Expected lists after swap. me = [1, 2]. other = []. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);

//         /* Step 2: Action. */
//         ecu_dlist_swap(&m_list, &m_other_list);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_list);
//         CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Me list becomes empty. Other list gets
//  * me's contents.
//  */
// TEST(DList, DListSwapMeListNotEmptyOtherListEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. me = [1, 2]. other = []. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) ); /* Precondition. */
//         ecu_dlist_push_back(&m_list, &m_node1);
//         ecu_dlist_push_back(&m_list, &m_node2);

//         /* Step 1: Arrange. Expected lists after swap. me = []. other = [1, 2]. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_other_list, &m_node2);

//         /* Step 2: Action. */
//         ecu_dlist_swap(&m_list, &m_other_list);

//         /* Step 3: Assert. */
//         list_order_check_expectations(&m_other_list);
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Allowed. Nothing should happen.
//  */
// TEST(DList, DListSwapBothListsEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//         CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );

//         /* Step 2: Action. */
//         ecu_dlist_swap(&m_list, &m_other_list);

//         /* Step 3: Assert. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_list)) );
//         CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. The two lists must be unique.
//  */
// TEST(DList, DListSwapSameListsSupplied)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         EXPECT_ASSERTION();

//         /* Steps 2 and 3: Action and assert. */
//         ecu_dlist_swap(&m_list, &m_list);
//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*--------------------- TESTS - DLIST VALID ------------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Dlist is not valid until it 
//  * has been constructed.
//  */
// TEST(DList, DListValid)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*------------------- TESTS - DLIST ITERATORS ----------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Verify all nodes in list iterated over.
//  */
// TEST(DList, Iterator)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Steps 2 Action. */
//         ECU_DLIST_FOR_EACH(i, &m_iterator, &m_list)
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief General test. Verify all nodes in list iterated over.
//  */
// TEST(DList, ConstIterator)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Steps 2 Action. */
//         ECU_DLIST_CONST_FOR_EACH(i, &m_citerator, &m_list)
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Iterating over an empty list immediately returns.
//  */
// TEST(DList, IteratorListIsEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. Preconditions must be true for test to produce useful results. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );

//         /* Step 2: Action. */
//         ECU_DLIST_FOR_EACH(i, &m_iterator, &m_other_list)
//         {
//             /* Step 3: Assert. This should never be called since empty list. */
//             node_in_list_mock(&m_other_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Iterating over an empty list immediately returns.
//  */
// TEST(DList, ConstIteratorListIsEmpty)
// {
//     try
//     {
//         /* Step 1: Arrange. Preconditions must be true for test to produce useful results. */
//         CHECK_TRUE( (ecu_dlist_empty(&m_other_list)) );

//         /* Step 2: Action. */
//         ECU_DLIST_CONST_FOR_EACH(i, &m_citerator, &m_other_list)
//         {
//             /* Step 3: Assert. This should never be called since empty list. */
//             node_in_list_mock(&m_other_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Nodes can be removed in the middle of an iteration.
//  * List must remain intact.
//  */
// TEST(DList, IteratorRemoveSome)
// {
//     try
//     {
//         /* Step 1: Arrange. First list iteration. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Step 1: Arrange. Second list iteration. */
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Step 2: Action. */
//         ECU_DLIST_FOR_EACH(i, &m_iterator, &m_list)
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);

//             if (i == &m_node2)
//             {
//                 ecu_dnode_remove(i);
//             }
//         }

//         /* Step 3: Assert. Iterate over list again. */
//         ECU_DLIST_FOR_EACH(i, &m_iterator, &m_list)
//         {
//             node_in_list_mock(&m_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Verify all nodes removed and list is empty.
//  */
// TEST(DList, IteratorRemoveAll)
// {
//     try
//     {
//         /* code */
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /*------------------------------------------------------------*/
// /*------------------ TESTS - DLIST AT ITERATORS --------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief General test. Iteration begins at start position 
//  * and terminates at list end.
//  */
// TEST(DList, AtIterator)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Steps 2 Action. */
//         ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, &m_node2)
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief General test. Iteration begins at start position 
//  * and terminates at list end.
//  */
// TEST(DList, ConstAtIterator)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Steps 2 Action. */
//         ECU_DLIST_CONST_AT_FOR_EACH(i, &m_citerator, &m_list, &m_node2)
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. Starting node must be within a list.
//  */
// TEST(DList, AtIteartorStartNodeNotInList)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_dnode_remove(&m_node1);
//         EXPECT_ASSERTION();

//         /* Step 2: Action. */
//         ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, &m_node1)
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);
//         }

//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. Starting node must be within a list.
//  */
// TEST(DList, ConstAtIteratorStartNodeNotInList)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. Starting node cannot be HEAD.
//  */
// TEST(DList, AtIteratorStartNodeIsHead)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Not allowed. Starting node cannot be HEAD.
//  */
// TEST(DList, ConstAtIteratorStartNodeIsHead)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* OK. */
//         (void)e;
//     }
// }

// /**
//  * @brief Iteration should only be over the starting node.
//  */
// TEST(DList, AtIteratorListWithOneNode)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         ecu_dlist_clear(&m_list);
//         ecu_dlist_push_back(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);

//         /* Step 2: Action. */
//         ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, &m_node1)
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);
//         }
//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Iteration should only be over the starting node.
//  */
// TEST(DList, ConstAtIteratorListWithOneNode)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Iteration should only be over the starting (tail) node.
//  */
// TEST(DList, AtIteratorStartNodeIsTail)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         CHECK_TRUE( (ecu_dlist_size(&m_list) > 1) ); /* Precondition. */
//         EXPECT_NODE_IN_LIST(&m_list, ecu_dlist_cback(&m_list));

//         /* Step 2: Action. */
//         ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, ecu_dlist_back(&m_list))
//         {
//             /* Step 3: Assert. */
//             node_in_list_mock(&m_list, i);
//         }

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Iteration should only be over the starting (tail) node.
//  */
// TEST(DList, ConstAtIteratorStartNodeIsTail)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Nodes can be removed in the middle of an iteration.
//  * List must remain intact.
//  */
// TEST(DList, AtIteratorRemoveSome)
// {
//     try
//     {
//         /* Step 1: Arrange. */
//         mock().strictOrder();
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node2);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node1);
//         EXPECT_NODE_IN_LIST(&m_list, &m_node3);

//         /* Step 2: Action. */
//         ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, ecu_dlist_front(&m_list))
//         {
//             /* Step 3: Assert. */
//             if (i == &m_node2)
//             {
//                 ecu_dnode_remove(i);
//             }
//             node_in_list_mock(&m_list, i);
//         }

//         /* Step 3: Assert. Iterate over list again. */
//         ECU_DLIST_AT_FOR_EACH(i, &m_iterator, &m_list, ecu_dlist_front(&m_list))
//         {
//             node_in_list_mock(&m_list, i);
//         }

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }

// /**
//  * @brief Verify all nodes removed and list before starting
//  * node remains intact.
//  */
// TEST(DList, AtIteratorRemoveAll)
// {
//     try
//     {

//     }
//     catch (const AssertException& e)
//     {
//         /* FAIL. */
//         (void)e;
//     }
// }
