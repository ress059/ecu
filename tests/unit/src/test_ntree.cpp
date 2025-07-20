/**
 * @file
 * @brief Unit tests for public API functions in @ref ntree.h. 
 * Test Summary:
 * 
 * Api:
 * 
 * 
 * 
 * 
 * Iterators:
 * 
 * @ref ECU_NTNODE_CHILD_FOR_EACH(), @ref ECU_NTNODE_CONST_CHILD_FOR_EACH(),
 * @ref ecu_ntnode_child_iterator_begin(), @ref ecu_ntnode_child_iterator_end(),
 * @ref ecu_ntnode_child_iterator_next(), @ref ecu_ntnode_child_iterator_cbegin(),
 * @ref ecu_ntnode_child_iterator_cend(), @ref ecu_ntnode_child_iterator_cnext()
 *      - TEST(NtNode, ChildIterator)
 *      - TEST(NtNode, ConstChildIterator)
 *      - TEST(NtNode, ChildIteratorOneChild)
 *      - TEST(NtNode, ConstChildIteratorOneChild)
 *      - TEST(NtNode, ChildIteratorNoChildren)
 *      - TEST(NtNode, ConstChildIteratorNoChildren)
 *      - TEST(NtNode, ChildIteratorRemoveSome)
 *      - TEST(NtNode, ChildIteratorRemoveAll)
 * 
 * @ref ECU_NTNODE_PARENT_FOR_EACH(), @ref ECU_NTNODE_CONST_PARENT_FOR_EACH(),
 * @ref ecu_ntnode_parent_iterator_begin(), @ref ecu_ntnode_parent_iterator_end(),
 * @ref ecu_ntnode_parent_iterator_next(), @ref ecu_ntnode_parent_iterator_cbegin(),
 * @ref ecu_ntnode_parent_iterator_cend(), @ref ecu_ntnode_parent_iterator_cnext()
 *      - TEST(NtNode, ParentIteratorLeaf)
 *      - TEST(NtNode, ConstParentIteratorLeaf)
 *      - TEST(NtNode, ParentIteratorMiddleChild)
 *      - TEST(NtNode, ConstParentIteratorMiddleChild)
 *      - TEST(NtNode, ParentIteratorOneParent)
 *      - TEST(NtNode, ConstParentIteratorOneParent)
 *      - TEST(NtNode, ParentIteratorRoot)
 *      - TEST(NtNode, ConstParentIteratorRoot)
 *      - TEST(NtNode, ParentIteratorRemoveSome)
 *      - TEST(NtNode, ParentIteratorRemoveAll)
 * 
 * @ref ECU_NTNODE_POSTORDER_FOR_EACH(), @ref ECU_NTNODE_CONST_POSTORDER_FOR_EACH(),
 * @ref ecu_ntnode_postorder_iterator_begin(), @ref ecu_ntnode_postorder_iterator_end(),
 * @ref ecu_ntnode_postorder_iterator_next(), @ref ecu_ntnode_postorder_iterator_cbegin(),
 * @ref ecu_ntnode_postorder_iterator_cend(), @ref ecu_ntnode_postorder_iterator_cnext()
 *      - TEST(NtNode, PostorderIterator)
 *      - TEST(NtNode, ConstPostorderIterator)
 *      - TEST(NtNode, PostorderIteratorLeftMostSubtree)
 *      - TEST(NtNode, ConstPostorderIteratorLeftMostSubtree)
 *      - TEST(NtNode, PostorderIteratorMiddleSubtree)
 *      - TEST(NtNode, ConstPostorderIteratorMiddleSubtree)
 *      - TEST(NtNode, PostorderIteratorRightMostSubtree)
 *      - TEST(NtNode, ConstPostorderIteratorRightMostSubtree)
 *      - TEST(NtNode, PostorderIteratorLeaf)
 *      - TEST(NtNode, ConstPostorderIteratorLeaf)
 *      - TEST(NtNode, PostorderIteratorEmptyRoot)
 *      - TEST(NtNode, ConstPostorderIteratorEmptyRoot)
 *      - TEST(NtNode, PostorderIteratorDegenerateTreeMiddle)
 *      - TEST(NtNode, ConstPostorderIteratorDegenerateTreeMiddle)
 *      - TEST(NtNode, PostorderIteratorDegenerateTreeRoot)
 *      - TEST(NtNode, ConstPostorderIteratorDegenerateTreeRoot)
 *      - TEST(NtNode, PostorderIteratorRemoveSome)
 *      - TEST(NtNode, PostorderIteratorRemoveAll)
 * 
 * @ref ECU_NTNODE_PREORDER_FOR_EACH(), @ref ECU_NTNODE_CONST_PREORDER_FOR_EACH(),
 * @ref ecu_ntnode_preorder_iterator_begin(), @ref ecu_ntnode_preorder_iterator_end(),
 * @ref ecu_ntnode_preorder_iterator_next(), @ref ecu_ntnode_preorder_iterator_cbegin(),
 * @ref ecu_ntnode_preorder_iterator_cend(), @ref ecu_ntnode_preorder_iterator_cnext()
 *      - TEST(NtNode, PreorderIterator)
 *      - TEST(NtNode, ConstPreorderIterator)
 *      - TEST(NtNode, PreorderIteratorLeftMostSubtree)
 *      - TEST(NtNode, ConstPreorderIteratorLeftMostSubtree)
 *      - TEST(NtNode, PreorderIteratorMiddleSubtree)
 *      - TEST(NtNode, ConstPreorderIteratorMiddleSubtree)
 *      - TEST(NtNode, PreorderIteratorRightMostSubtree)
 *      - TEST(NtNode, ConstPreorderIteratorRightMostSubtree)
 *      - TEST(NtNode, PreorderIteratorLeaf)
 *      - TEST(NtNode, ConstPreorderIteratorLeaf)
 *      - TEST(NtNode, PreorderIteratorEmptyRoot)
 *      - TEST(NtNode, ConstPreorderIteratorEmptyRoot)
 *      - TEST(NtNode, PreorderIteratorDegenerateTreeMiddle)
 *      - TEST(NtNode, ConstPreorderIteratorDegenerateTreeMiddle)
 *      - TEST(NtNode, PreorderIteratorDegenerateTreeRoot)
 *      - TEST(NtNode, ConstPreorderIteratorDegenerateTreeRoot)
 *      - TEST(NtNode, PreorderIteratorRemove)
 * 
 * @ref ECU_NTNODE_SIBLING_FOR_EACH(), @ref ECU_NTNODE_CONST_SIBLING_FOR_EACH(),
 * @ref ecu_ntnode_sibling_iterator_begin(), @ref ecu_ntnode_sibling_iterator_end(),
 * @ref ecu_ntnode_sibling_iterator_next(), @ref ecu_ntnode_sibling_iterator_cbegin(),
 * @ref ecu_ntnode_sibling_iterator_cend(), @ref ecu_ntnode_sibling_iterator_cnext()
 *      - TEST(NtNode, SiblingIteratorFirst)
 *      - TEST(NtNode, ConstSiblingIteratorFirst)
 *      - TEST(NtNode, SiblingIteratorMiddle)
 *      - TEST(NtNode, ConstSiblingIteratorMiddle)
 *      - TEST(NtNode, SiblingIteratorLast)
 *      - TEST(NtNode, ConstSiblingIteratorLast)
 *      - TEST(NtNode, SiblingIteratorOneSibling)
 *      - TEST(NtNode, ConstSiblingIteratorOneSibling)
 *      - TEST(NtNode, SiblingIteratorNoSiblings)
 *      - TEST(NtNode, ConstSiblingIteratorNoSiblings)
 *      - TEST(NtNode, SiblingIteratorRemoveSome)
 *      - TEST(NtNode, SiblingIteratorRemoveAll)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-09
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Files under test. */
#include "ecu/ntree.h"

/* STDLib. */
#include <cassert>
#include <memory>
#include <type_traits>

/* Stubs. */
#include "stubs/stub_asserter.hpp"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*------------------------- NAMESPACES -----------------------*/
/*------------------------------------------------------------*/

using namespace stubs;

namespace {
/*------------------------------------------------------------*/
/*--------------- FILE-SCOPE FUNCTION DECLARATIONS -----------*/
/*------------------------------------------------------------*/

/* Forward declarations. */
struct ntnode;
struct rw_ntnode;
struct ro_ntnode;

/**
 * @brief Safely converts (ecu_ntnode *) which is a C structure
 * into the ntnode C++ wrapper class. Common use case would be
 * converting (ecu_ntnode *) returned by C-based iterators under test.
 * Asserts if ecu_ntnode_valid(n) fails.
 */
ntnode& convert(ecu_ntnode *n);

/**
 * @brief Safely converts (const ecu_ntnode *) which is a C structure
 * into the ntnode C++ wrapper class. Common use case would be
 * converting (ecu_ntnode *) returned by C-based iterators under test.
 * Asserts if ecu_ntnode_valid(n) fails.
 */
const ntnode& convert(const ecu_ntnode *n);

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
    virtual void visit(rw_ntnode& n) = 0;

    /// @brief Only read-only operations can be applied since
    /// read-write node is const.
    virtual void visit(const rw_ntnode& n) = 0;
    
    /// @brief Only read-only operations can be applied since
    /// node is read-only type (and const).
    virtual void visit(const ro_ntnode& n) = 0;

protected:
    /// @brief Default constructor.
    visitor() = default;
};

/*------------------------------------------------------------*/
/*-------------------- FILE-SCOPE TYPES - NODES --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Ntnode interface. This is a C++ wrapper class around C 
 * structure under test, @ref ecu_ntnode. Used to create a C++ 
 * interface for an @ref ecu_ntnode to facilitate easier test 
 * development.
 */
struct ntnode : public ecu_ntnode
{
protected:
    /// @brief Default constructor. Node's destroy callback
    /// and ID are unused.
    ntnode()
    {
        ecu_ntnode_ctor(this, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    }

    /// @brief Constructor assigns supplied ID to node. Node's 
    /// destroy callback unused.
    ntnode(ecu_object_id id_)
    {
        ecu_ntnode_ctor(this, ECU_NTNODE_DESTROY_UNUSED, id_);
    }

    /// @brief Constructor assigns supplied destroy callback and 
    /// optional ID to ecu_ntnode. Function takes in the C-structure
    /// under test (ecu_ntnode *) to be compatible with the C API and
    /// to avoid having to explicitly upcast. Do not convert into 
    /// this C++ wrapper class (ntnode *), as ecu_ntnode_destroy() will
    /// have already been called on the node, thus invalidating it.
    ntnode(void (*destroy_)(ecu_ntnode *, ecu_object_id), ecu_object_id id_ = ECU_OBJECT_ID_UNUSED)
    {
        ecu_ntnode_ctor(this, destroy_, id_);
    }

public:
    /// @brief Virtual base class destructor. C-destructor
    /// ecu_ntnode_destroy() not called since tests call
    /// it directly. Do not place in C++ destructor or else
    /// ecu_ntnode_destroy() is called twice, first in the test
    /// and then in this destructor. The 2nd call causes
    /// problems because an invalidated node (next, prev, etc
    /// reset to NULL from first ecu_ntnode_destroy call) is 
    /// passed, causing ecu_ntnode_valid() to fail. This is
    /// meant to happen since C-destructor under test does
    /// not use dynamic memory allocation, so it only invalidates
    /// the object.
    virtual ~ntnode() = default;

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
struct rw_ntnode : public ntnode
{
    /// @brief Default constructor. Node's destroy callback
    /// and ID are unused.
    rw_ntnode() 
        : ntnode()
    {
        
    }

    /// @brief Constructor assigns supplied ID to node. Node's 
    /// destroy callback unused.
    rw_ntnode(ecu_object_id id_)
        : ntnode(std::move(id_))
    {

    }

    /// @brief Constructor assigns supplied destroy callback and 
    /// optional ID to ecu_ntnode. Function takes in the C-structure
    /// under test (ecu_ntnode *) to be compatible with the C API and
    /// to avoid having to explicitly upcast. Do not convert into 
    /// the C++ base wrapper class (ntnode *), as ecu_ntnode_destroy() will
    /// have already been called on the node, thus invalidating it.
    rw_ntnode(void (*destroy_)(ecu_ntnode *, ecu_object_id), ecu_object_id id_ = ECU_OBJECT_ID_UNUSED)
        : ntnode(std::move(destroy_), std::move(id_))
    {

    }

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
struct ro_ntnode : public ntnode
{
    /// @brief Default constructor. Node's destroy callback
    /// and ID are unused.
    ro_ntnode() 
        : ntnode()
    {

    }

    /// @brief Constructor assigns supplied ID to node. Node's 
    /// destroy callback unused.
    ro_ntnode(ecu_object_id id_)
        : ntnode(std::move(id_))
    {

    }

    /// @brief Constructor assigns supplied destroy callback and 
    /// optional ID to ecu_ntnode. Function takes in the C-structure
    /// under test (ecu_ntnode *) to be compatible with the C API and
    /// to avoid having to explicitly upcast. Do not convert into 
    /// the C++ base wrapper class (ntnode *), as ecu_ntnode_destroy() will
    /// have already been called on the node, thus invalidating it.
    ro_ntnode(void (*destroy_)(ecu_ntnode *, ecu_object_id), ecu_object_id id_ = ECU_OBJECT_ID_UNUSED)
        : ntnode(std::move(destroy_), std::move(id_))
    {
        
    }

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
/*------------- FILE-SCOPE TYPES - CONCRETE VISITORS ---------*/
/*------------------------------------------------------------*/

/**
 * @brief Concrete visitor that calls node in tree mock
 * expectation, where value is the node object's address. 
 * Applies to all node types.
 */
struct node_obj_in_tree_actual_call : public visitor
{
    /// @brief Calls node in tree mock expectation. 
    /// Takes in base C++ wrapper to make this a common function.
    static void call(const ntnode &n)
    {
        /* Note how n is explicitly upcasted to (const ecu_ntnode *) to standardize
        pointers passed in. It is NOT guaranteed (const ecu_ntnode *) == (const ntnode *). 
        Do const-qualified cast to allow const and non-const parameters. */
        mock().actualCall("node_in_tree")
              .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n));
    }

    /// @brief Calls node in tree mock expectation on non-const read-write node.
    void visit(rw_ntnode& n) override
    {
        call(n);
    }

    /// @brief Calls node in tree mock expectation on const read-write node.
    void visit(const rw_ntnode& n) override
    {
        call(n);
    }

    /// @brief Calls node in tree mock expectation on read-only node.
    void visit(const ro_ntnode& n) override
    {
        call(n);
    }
};

/**
 * @brief Concrete visitor that removes node. Only applies
 * to editable nodes. I.e. non-const read-write nodes are
 * removed but read-only nodes are not.
 */
struct node_remove : public visitor
{
    /// @brief Removes non-const read-write node from the tree.
    void visit(rw_ntnode& n) override
    {
        ecu_ntnode_remove(&n);
    }

    /// @brief Does nothing since const read-write node supplied.
    void visit(const rw_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Does nothing since read-only node supplied.
    void visit(const ro_ntnode& n) override
    {
        (void)n;
    }
};

/*------------------------------------------------------------*/
/*---------------- FILE-SCOPE FUNCTION DEFINITIONS -----------*/
/*------------------------------------------------------------*/

ntnode& convert(ecu_ntnode *n)
{
    assert( (n) );
    assert( (ecu_ntnode_valid(n)) );
    return (*static_cast<ntnode *>(n));
}

const ntnode& convert(const ecu_ntnode *n)
{
    assert( (n) );
    assert( (ecu_ntnode_valid(n)) );
    return (*static_cast<const ntnode *>(n));
}
} /* namespace. */

// /// @brief Strategy.
// /// Interface to test ECU tree iterators which are in C.
// /// begin(), end(), and next() functions return ecu_ntnode since
// /// some iterators return dummy delimiters not apart of this C++
// /// interface. The convert function acts a safety mechanism to
// /// convert the C ecu_ntnode into our C++ interface ONLY if the 
// /// returned node is valid (not a C delimiter).
// struct iterator
// {
//     virtual ecu_ntnode *begin(ntnode& start) = 0;
//     virtual ecu_ntnode *end() = 0;
//     virtual ecu_ntnode *next() = 0;
//     virtual ntnode& convert(ecu_ntnode *node) = 0;

//     /// @brief Applies visitor to all nodes. Also tests FOR_EACH() macros.
//     virtual void traverse(ntnode& start, visitor& v) = 0;

// protected:
//     iterator() = default;
// };

// struct const_iterator
// {
//     virtual const ecu_ntnode *cbegin(const ntnode& start) = 0;
//     virtual const ecu_ntnode *cend() = 0;
//     virtual const ecu_ntnode *cnext() = 0;
//     virtual const ntnode& convert(const ecu_ntnode *node) = 0;

//     /// @brief Applies visitor to all nodes. Also tests CONST_FOR_EACH() macros.
//     virtual void traverse(const ntnode& start, visitor& v) = 0;

// protected:
//     const_iterator() = default;
// };

// struct child_iterator : public iterator
// {
//     ecu_ntnode *begin(ntnode& start) override
//     {
//         return ecu_ntnode_child_iterator_begin(&m_me, &start);
//     }

//     ecu_ntnode *end() override
//     {
//         return ecu_ntnode_child_iterator_end(&m_me);
//     }

//     ecu_ntnode *next() override
//     {
//         return ecu_ntnode_child_iterator_next(&m_me);
//     }

//     ntnode& convert(ecu_ntnode *node)
//     {
//         /* All children returned can be safely converted. */
//         assert( (node) );
//         return *static_cast<ntnode *>(node);
//     }

//     void traverse(ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_CHILD_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_child_iterator m_me;
// };

// struct const_child_iterator : public const_iterator
// {
//     const ecu_ntnode *cbegin(const ntnode& start) override
//     {
//         return ecu_ntnode_child_iterator_cbegin(&m_me, &start);
//     }

//     const ecu_ntnode *cend() override
//     {
//         return ecu_ntnode_child_iterator_cend(&m_me);
//     }

//     const ecu_ntnode *cnext() override
//     {
//         return ecu_ntnode_child_iterator_cnext(&m_me);
//     }

//     const ntnode& convert(const ecu_ntnode *node) override
//     {
//         /* All children returned can be safely converted. */
//         assert( (node) );
//         return *static_cast<const ntnode *>(node);
//     }

//     void traverse(const ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_child_citerator m_me;
// };

// struct parent_iterator : public iterator
// {
//     ecu_ntnode *begin(ntnode& start) override
//     {
//         return ecu_ntnode_parent_iterator_begin(&m_me, &start);
//     }

//     ecu_ntnode *end() override
//     {
//         return ecu_ntnode_parent_iterator_end(&m_me);
//     }

//     ecu_ntnode *next() override
//     {
//         return ecu_ntnode_parent_iterator_next(&m_me);
//     }

//     ntnode& convert(ecu_ntnode *node) override
//     {
//         /* All parents returned can be safely converted. */
//         assert( (node) );
//         return *static_cast<ntnode *>(node);
//     }

//     void traverse(ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_PARENT_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_parent_iterator m_me;
// };

// struct const_parent_iterator : public const_iterator
// {
//     const ecu_ntnode *cbegin(const ntnode& start) override
//     {
//         return ecu_ntnode_parent_iterator_cbegin(&m_me, &start);
//     }

//     const ecu_ntnode *cend() override
//     {
//         return ecu_ntnode_parent_iterator_cend(&m_me);
//     }

//     const ecu_ntnode *cnext() override
//     {
//         return ecu_ntnode_parent_iterator_cnext(&m_me);
//     }

//     const ntnode& convert(const ecu_ntnode *node) override
//     {
//         /* All parents returned can be safely converted. */
//         assert( (node) );
//         return *static_cast<const ntnode *>(node);
//     }

//     void traverse(const ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_parent_citerator m_me;
// };

// struct sibling_iterator : public iterator
// {
//     ecu_ntnode *begin(ntnode& start) override
//     {
//         return ecu_ntnode_sibling_iterator_begin(&m_me, &start);
//     }

//     ecu_ntnode *end() override
//     {
//         return ecu_ntnode_sibling_iterator_end(&m_me);
//     }

//     ecu_ntnode *next() override
//     {
//         return ecu_ntnode_sibling_iterator_next(&m_me);
//     }

//     ntnode& convert(ecu_ntnode *node) override
//     {
//         /* All siblings returned can be safely converted. */
//         assert( (node) );
//         return *static_cast<ntnode *>(node);
//     }

//     void traverse(ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_SIBLING_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_sibling_iterator m_me;
// };

// struct const_sibling_iterator : public const_iterator
// {
//     const ecu_ntnode *cbegin(const ntnode& start) override
//     {
//         return ecu_ntnode_sibling_iterator_cbegin(&m_me, &start);
//     }

//     const ecu_ntnode *cend() override
//     {
//         return ecu_ntnode_sibling_iterator_cend(&m_me);
//     }

//     const ecu_ntnode *cnext() override
//     {
//         return ecu_ntnode_sibling_iterator_cnext(&m_me);
//     }

//     const ntnode& convert(const ecu_ntnode *node) override
//     {
//         /* All siblings returned can be safely converted. */
//         assert( (node) );
//         return *static_cast<const ntnode *>(node);
//     }

//     void traverse(const ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_sibling_citerator m_me;
// };

// struct preorder_iterator : public iterator
// {
//     ecu_ntnode *begin(ntnode& start) override
//     {
//         return ecu_ntnode_preorder_iterator_begin(&m_me, &start);
//     }

//     ecu_ntnode *end() override
//     {
//         return ecu_ntnode_preorder_iterator_end(&m_me);
//     }

//     ecu_ntnode *next() override
//     {
//         return ecu_ntnode_preorder_iterator_next(&m_me);
//     }

//     ntnode& convert(ecu_ntnode *node) override
//     {
//         /* Delimiter node is in C-struct so cannot be converted. */
//         assert( (node) );
//         assert( (node != &m_me.delimiter) );
//         return *static_cast<ntnode *>(node);
//     }

//     void traverse(ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_PREORDER_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_preorder_iterator m_me;
// };

// struct const_preorder_iterator : public const_iterator
// {
//     const ecu_ntnode *cbegin(const ntnode& start) override
//     {
//         return ecu_ntnode_preorder_iterator_cbegin(&m_me, &start);
//     }

//     const ecu_ntnode *cend() override
//     {
//         return ecu_ntnode_preorder_iterator_cend(&m_me);
//     }

//     const ecu_ntnode *cnext() override
//     {
//         return ecu_ntnode_preorder_iterator_cnext(&m_me);
//     }

//     const ntnode& convert(const ecu_ntnode *node) override
//     {
//         /* Delimiter node is in C-struct so cannot be converted. */
//         assert( (node) );
//         assert( (node != &m_me.delimiter) );
//         return *static_cast<const ntnode *>(node);
//     }

//     void traverse(const ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_preorder_citerator m_me;
// };

// struct postorder_iterator : public iterator
// {
//     ecu_ntnode *begin(ntnode& start) override
//     {
//         return ecu_ntnode_postorder_iterator_begin(&m_me, &start);
//     }

//     ecu_ntnode *end() override
//     {
//         return ecu_ntnode_postorder_iterator_end(&m_me);
//     }

//     ecu_ntnode *next() override
//     {
//         return ecu_ntnode_postorder_iterator_next(&m_me);
//     }

//     ntnode& convert(ecu_ntnode *node) override
//     {
//         /* Delimiter node is in C-struct so cannot be converted. */
//         assert( (node) );
//         assert( (node != &m_me.delimiter) );
//         return *static_cast<ntnode *>(node);
//     }

//     void traverse(ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_POSTORDER_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_postorder_iterator m_me;
// };

// struct const_postorder_iterator : public const_iterator
// {
//     const ecu_ntnode *cbegin(const ntnode& start) override
//     {
//         return ecu_ntnode_postorder_iterator_cbegin(&m_me, &start);
//     }

//     const ecu_ntnode *cend() override
//     {
//         return ecu_ntnode_postorder_iterator_cend(&m_me);
//     }

//     const ecu_ntnode *cnext() override
//     {
//         return ecu_ntnode_postorder_iterator_cnext(&m_me);
//     }

//     const ntnode& convert(const ecu_ntnode *node) override
//     {
//         /* Delimiter node is in C-struct so cannot be converted. */
//         assert( (node) );
//         assert( (node != &m_me.delimiter) );
//         return *static_cast<const ntnode *>(node);
//     }

//     void traverse(const ntnode& start, visitor& v) override
//     {
//         ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &m_me, &start)
//         {
//             convert(n).accept(v);
//         }
//     }

//     ecu_ntnode_postorder_citerator m_me;
// };

/*------------------------------------------------------------*/
/*------------------------- TEST GROUPS ----------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(NtNode)
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

    /// @brief Creates node_in_tree mock expectations on the nodes
    /// in the order they are supplied. Value is the node object's address.
    /// Note how only lvalue references can be supplied. Do not allow 
    /// temporaries since object lifetime must outlive the mock.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...) /* Nodes type will never be reference since parameter passed by reference. */
    static void EXPECT_NODES_IN_TREE(ntnode& n0, Nodes&... n)
    {
        /* Note how parameters are explicitly upcasted to (const ecu_ntnode *) to standardize
        pointers passed in. It is NOT guaranteed (const ecu_ntnode *) == (const ntnode *). 
        Do const-qualified cast to allow const and non-const parameters. */
        mock().strictOrder();
        mock().expectOneCall("node_in_tree")
              .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n0));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock().expectOneCall("node_in_tree")
                   .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n)), ...);
        }
    }

    /// @brief Helper to create a test tree. Creates a branch by adding children,
    /// grandchildren, etc to a parent. I.e. add_branch(0, 1, 2) means 1 is child 
    /// of 0 and 2 is grandchild of 0. Has to be a recursive template since
    /// parent node changes with each operation.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static void add_branch(ntnode& root, ntnode& c0, Nodes&... cn)
    {
        add_branch(root, c0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            add_branch(c0, cn...);
        }
    }

    /// @brief Helper method for template parameter pack overload.
    static void add_branch(ntnode& parent, ntnode& child)
    {
        ecu_ntnode_push_child_front(&parent, &child);
    }

    /// @brief Helper to create test tree. Adds any number of children to a parent.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static void add_children(ntnode& parent, ntnode& c0, Nodes&... cn)
    {
        ecu_ntnode_push_child_back(&parent, &c0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            (ecu_ntnode_push_child_back(&parent, &cn), ...);
        }
    }

    /// @brief Returns true if all supplied nodes are in a tree.
    /// False otherwise. Function immediately stops evaluating
    /// when the first node not in a tree is processed.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static bool in_subtree(const ntnode &n0, const Nodes&... n)
    {
        bool status = ecu_ntnode_in_subtree(&n0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            status = status && ((ecu_ntnode_in_subtree(&n)) && ...); /* Only keep evaluating if true return. */
        }

        return status;
    }

    /// @brief Returns true if all supplied nodes are roots.
    /// False otherwise. Function immediately stops evaluating
    /// when the first node that is not a root is processed.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static bool is_root(const ntnode &n0, const Nodes&... n)
    {
        bool status = ecu_ntnode_is_root(&n0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            status = status && ((ecu_ntnode_is_root(&n)) && ...); /* Only keep evaluating if true return. */
        }

        return status;
    }

    node_obj_in_tree_actual_call node_obj_in_tree_visitor;
    node_remove node_remove_visitor;
    std::array<ro_ntnode, 50> RO; /**< Read-only nodes. */
    std::array<rw_ntnode, 50> RW; /**< Read-write nodes. */
};

/*------------------------------------------------------------*/
/*-------------------- TESTS - CHILD ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief General iteration test. Verify all children
 * iterated over.
 */
TEST(NtNode, ChildIterator)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        RW0
        |
        RW1-----RW2------RW3----RW4
                |
                RW5 
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief General iteration test. Verify all children
 * iterated over.
 */
TEST(NtNode, ConstChildIterator)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        RW0
        |
        RW1-----RW2------RW3----RW4
                |
                RW5 
        */
        ecu_ntnode_child_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a child iteration on a node that has
 * one child.
 */
TEST(NtNode, ChildIteratorOneChild)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW2.
        RW0
        |
        RW1-----RW2------RW3----RW4
                |
                RW5 
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_TREE(RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a child iteration on a node that has
 * one child.
 */
TEST(NtNode, ConstChildIteratorOneChild)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW2.
        RW0
        |
        RW1-----RW2------RW3----RW4
                |
                RW5 
        */
        ecu_ntnode_child_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_TREE(RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citer, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a child iteration on a node that has
 * no children. Iteration should end immediately.
 */
TEST(NtNode, ChildIteratorNoChildren)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a child iteration on a node that has
 * no children. Iteration should end immediately.
 */
TEST(NtNode, ConstChildIteratorNoChildren)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        ecu_ntnode_child_citerator citer;
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove some nodes in the middle of a child iteration.
 */
TEST(NtNode, ChildIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        Remove RW1 and RW2.

        Before:
        RW0
        |
        RW1----RO0----RW2---RO1
                                        
        After:
        RW0             RW1         RW2
        |
        RO0-----RO1
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), RO.at(0), RW.at(2), RO.at(1));

        /* Step 2: Action. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Proper nodes must be removed. */
        CHECK_TRUE( (is_root(RW.at(1), RW.at(2))) );
        CHECK_TRUE( (in_subtree(RO.at(0), RO.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes returned by child iteration.
 */
TEST(NtNode, ChildIteratorRemoveAll)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        Remove all children.

        Before:
        RW0
        |
        RW1----RW2----RW3---RW4
                                        
        After:
        RW0     RW1      RW2     RW3     RW4
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Step 2: Action. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. All nodes must be removed. */
        CHECK_TRUE( (is_root(RW.at(1), RW.at(2), RW.at(3), RW.at(4))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - PARENT ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief General iteration test where starting node
 * is a leaf.
 */
TEST(NtNode, ParentIteratorLeaf)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW5.
        RW0
        |
        RW1-----RW2-----RW6
                |       |
                RW3     RW7
                |
                RW4
                |
                RW5 
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(3), RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(5))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief General iteration test where starting node
 * is a leaf.
 */
TEST(NtNode, ConstParentIteratorLeaf)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW5.
        RW0
        |
        RW1-----RW2-----RW6
                |       |
                RW3     RW7
                |
                RW4
                |
                RW5 
        */
        ecu_ntnode_parent_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(3), RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(5))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief General iteration test where starting node
 * is a middle child.
 */
TEST(NtNode, ParentIteratorMiddleChild)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW3.
        RW0
        |
        RW1-----RW2-----RW6
                |       |
                RW3     RW7
                |
                RW4
                |
                RW5 
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(3))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief General iteration test where starting node
 * is a middle child.
 */
TEST(NtNode, ConstParentIteratorMiddleChild)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW3.
        RW0
        |
        RW1-----RW2-----RW6
                |       |
                RW3     RW7
                |
                RW4
                |
                RW5 
        */
        ecu_ntnode_parent_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(3))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a parent iteration on a node that has
 * one parent (direct child of a root).
 */
TEST(NtNode, ParentIteratorOneParent)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1-----RW2-----RW6
                |       |
                RW3     RW7
                |
                RW4
                |
                RW5 
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_TREE(RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(6))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a parent iteration on a node that has
 * one parent (direct child of a root).
 */
TEST(NtNode, ConstParentIteratorOneParent)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1-----RW2-----RW6
                |       |
                RW3     RW7
                |
                RW4
                |
                RW5 
        */
        ecu_ntnode_parent_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_TREE(RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(6))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a parent iteration on a node that has
 * no parent. Iteration should end immediately.
 */
TEST(NtNode, ParentIteratorRoot)
{
    try
    {
        /* Step 1: Arrrange. Start iteration at RW0.
        RW0
        |
        RW1 
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a parent iteration on a node that has
 * no parent. Iteration should end immediately.
 */
TEST(NtNode, ConstParentIteratorRoot)
{
    try
    {
        /* Step 1: Arrrange. Start iteration at RW0.
        RW0
        |
        RW1 
        */
        ecu_ntnode_parent_citerator citer;
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove some nodes in the middle of a parent iteration.
 */
TEST(NtNode, ParentIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW3.
        Remove RW2 and RW1.

        Before:
        RO0
        |
        RW0-----RW1-----RW4
                |       |
                RO1     RW5
                |
                RW2
                |
                RW3 
                
        After:
        RO0                 RW2         RW1
        |                   |           |
        RW0-----RW4         RW3         RO1
                |
                RW5 
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RO.at(0), RW.at(0), RW.at(1), RW.at(4));
        add_branch(RW.at(1), RO.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(4), RW.at(5));

        /* Step 2: Action. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(3))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify correct nodes removed. */
        CHECK_TRUE( (is_root(RW.at(2), RW.at(1))) );
        CHECK_TRUE( (in_subtree(RW.at(3), RO.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes returned by parent iteration.
 */
TEST(NtNode, ParentIteratorRemoveAll)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW5.
        Remove all nodes.

        Before:
        RW0
        |
        RW1-----RW2-----RW6
                |       |
                RW3     RW7
                |
                RW4
                |
                RW5 
                
        After:
        RW0             RW4     RW3     RW2
        |               |
        RW1-----RW6     RW5
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        
        /* Step 2: Action. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(5))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify all nodes removed. */
        CHECK_TRUE( (is_root(RW.at(4), RW.at(3), RW.at(2))) );
        CHECK_TRUE( (in_subtree(RW.at(5))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------------- TESTS - POSTORDER ITERATOR ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief General iteration test. Verify all nodes iterated
 * over.
 */
TEST(NtNode, PostorderIterator)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief General iteration test. Verify all nodes iterated
 * over.
 */
TEST(NtNode, ConstPostorderIterator)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a left-most subtree.
 */
TEST(NtNode, PostorderIteratorLeftMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a left-most subtree.
 */
TEST(NtNode, ConstPostorderIteratorLeftMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a middle subtree.
 */
TEST(NtNode, PostorderIteratorMiddleSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a middle subtree.
 */
TEST(NtNode, ConstPostorderIteratorMiddleSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a right-most subtree.
 */
TEST(NtNode, PostorderIteratorRightMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a right-most subtree.
 */
TEST(NtNode, ConstPostorderIteratorRightMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a leaf node. Only the leaf
 * should be returned in the iteration.
 */
TEST(NtNode, PostorderIteratorLeaf)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a leaf node. Only the leaf
 * should be returned in the iteration.
 */
TEST(NtNode, ConstPostorderIteratorLeaf)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform iteration on empty tree. Only root
 * node should be returned in the iteration.
 */
TEST(NtNode, PostorderIteratorEmptyRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform iteration on empty tree. Only root
 * node should be returned in the iteration.
 */
TEST(NtNode, ConstPostorderIteratorEmptyRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at middle node of degenerate tree.
 */
TEST(NtNode, PostorderIteratorDegenerateTreeMiddle)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at middle node of degenerate tree.
 */
TEST(NtNode, ConstPostorderIteratorDegenerateTreeMiddle)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at root node of degenerate tree.
 */
TEST(NtNode, PostorderIteratorDegenerateTreeRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at root node of degenerate tree.
 */
TEST(NtNode, ConstPostorderIteratorDegenerateTreeRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove some nodes in a postorder iteration.
 */
TEST(NtNode, PostorderIteratorRemoveSome)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes in a postorder iteration.
 */
TEST(NtNode, PostorderIteratorRemoveAll)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - PREORDER ITERATOR ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief General iteration test. Verify all nodes iterated
 * over.
 */
TEST(NtNode, PreorderIterator)
{
    // try
    // {
    //     /* Step 1: Arrange. Start iteration at RW0.
    //     RW0
    //     |
    //     RW1---------------RW2---RW3-----RW4
    //     |                       |       |
    //     RW5---RW6---RW7         RW8     RW9
    //                                     |
    //                                     RW10----RW11
    //                                     |
    //                                     RW12
    //     */
    //    add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
    //    add_children(RW.at(1), RW.at(5), RW.at(6), RW.at(7));
    //    add_children(RW.at(3), RW.at(8));
    //    add_branch(RW.at(4), RW.at(9), RW.at(10), RW.at(12));
    //    add_children(RW.at(9), RW.at(11));

    //    EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(5), RW.at(6), RW.at(7),
    //                          RW.at(2), RW.at(3), RW.at(8), RW.at(4), RW.at(9),
    //                          RW.at(10), RW.at(12), RW.at(11));

    //     /* Step 2: Action. */
    //     TREE_APPLY_EXPECTATIONS(RW.at(0), preorder_iterator());

    //     /* Step 3: Assert. Test fails if expected nodes not iterated over. */
    // }
    // catch (const AssertException& e)
    // {
    //     /* FAIL. */
    //     (void)e;
    // }
}

/**
 * @brief General iteration test. Verify all nodes iterated
 * over.
 */
TEST(NtNode, ConstPreorderIterator)
{
    // try
    // {
    //     /* Step 1: Arrange. Start iteration at RW0.
    //     RW0
    //     |
    //     RW1---------------RW2---RW3-----RW4
    //     |                       |       |
    //     RW5---RW6---RW7         RW8     RW9
    //                                     |
    //                                     RW10----RW11
    //                                     |
    //                                     RW12
    //     */
    //    add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
    //    add_children(RW.at(1), RW.at(5), RW.at(6), RW.at(7));
    //    add_children(RW.at(3), RW.at(8));
    //    add_branch(RW.at(4), RW.at(9), RW.at(10), RW.at(12));
    //    add_children(RW.at(9), RW.at(11));

    //    EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(5), RW.at(6), RW.at(7),
    //                          RW.at(2), RW.at(3), RW.at(8), RW.at(4), RW.at(9),
    //                          RW.at(10), RW.at(12), RW.at(11));

    //     /* Step 2: Action. */
    //     TREE_APPLY_EXPECTATIONS(RW.at(0), const_preorder_iterator());

    //     /* Step 3: Assert. Test fails if expected nodes not iterated over. */
    // }
    // catch (const AssertException& e)
    // {
    //     /* FAIL. */
    //     (void)e;
    // }
}

/**
 * @brief Start iteration at node in a left-most subtree.
 */
TEST(NtNode, PreorderIteratorLeftMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a left-most subtree.
 */
TEST(NtNode, ConstPreorderIteratorLeftMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a middle subtree.
 */
TEST(NtNode, PreorderIteratorMiddleSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a middle subtree.
 */
TEST(NtNode, ConstPreorderIteratorMiddleSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a right-most subtree.
 */
TEST(NtNode, PreorderIteratorRightMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at node in a right-most subtree.
 */
TEST(NtNode, ConstPreorderIteratorRightMostSubtree)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a leaf node. Only the leaf
 * should be returned in the iteration.
 */
TEST(NtNode, PreorderIteratorLeaf)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a leaf node. Only the leaf
 * should be returned in the iteration.
 */
TEST(NtNode, ConstPreorderIteratorLeaf)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform iteration on empty tree. Only root
 * node should be returned in the iteration.
 */
TEST(NtNode, PreorderIteratorEmptyRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform iteration on empty tree. Only root
 * node should be returned in the iteration.
 */
TEST(NtNode, ConstPreorderIteratorEmptyRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at middle node of degenerate tree.
 */
TEST(NtNode, PreorderIteratorDegenerateTreeMiddle)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at middle node of degenerate tree.
 */
TEST(NtNode, ConstPreorderIteratorDegenerateTreeMiddle)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at root node of degenerate tree.
 */
TEST(NtNode, PreorderIteratorDegenerateTreeRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at root node of degenerate tree.
 */
TEST(NtNode, ConstPreorderIteratorDegenerateTreeRoot)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Removing nodes is not allowed in a preorder iteration
 * since it will corrupt the current iteration.
 */
TEST(NtNode, PreorderIteratorRemove)
{
    try
    {

    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - SIBLING ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Start iteration at first sibling.
 */
TEST(NtNode, SiblingIteratorFirst)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW4.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(4))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at first sibling.
 */
TEST(NtNode, ConstSiblingIteratorFirst)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW4.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(4))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at middle sibling.
 */
TEST(NtNode, SiblingIteratorMiddle)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW6.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(8), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(6))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at middle sibling.
 */
TEST(NtNode, ConstSiblingIteratorMiddle)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW6.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(8), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(6))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at last sibling.
 */
TEST(NtNode, SiblingIteratorLast)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW8.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(6), RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(8))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    } 
}

/**
 * @brief Start iteration at last sibling.
 */
TEST(NtNode, ConstSiblingIteratorLast)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW8.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(6), RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(8))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    } 
}

/**
 * @brief Perform a sibling iteration on a node that has
 * one sibling.
 */
TEST(NtNode, SiblingIteratorOneSibling)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW9.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(10));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(9))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a sibling iteration on a node that has
 * one sibling.
 */
TEST(NtNode, ConstSiblingIteratorOneSibling)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW9.
        RW0
        |
        RW1-----RW2-------------------------RW3
                |                           |
                RW4--RW5--RW6--RW7--RW8     RW9---RW10 
        */
        ecu_ntnode_sibling_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_TREE(RW.at(10));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(9))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a sibling iteration on a node that has
 * no siblings. Iteration should end immediately.
 */
TEST(NtNode, SiblingIteratorNoSiblings)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        ecu_ntnode_sibling_iterator iter;
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Perform a sibling iteration on a node that has
 * no siblings. Iteration should end immediately.
 */
TEST(NtNode, ConstSiblingIteratorNoSiblings)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        ecu_ntnode_sibling_citerator citer;
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove some nodes in the middle of a sibling iteration.
 */
TEST(NtNode, SiblingIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        Remove RW2 and RW3.

        Before:
        RW0
        |
        RW1----RO0----RW2----RW3---RO1 
        
        After:
        RW0             RW2         RW3
        |
        RW1---RO0---RO1 
        */
        ecu_ntnode_sibling_iterator iter;
        add_children(RW.at(0), RW.at(1), RO.at(0), RW.at(2), RW.at(3), RO.at(1));

        /* Step 2: Action. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify correct nodes removed. */
        CHECK_TRUE( (is_root(RW.at(2), RW.at(3))) );
        CHECK_TRUE( (in_subtree(RW.at(1), RO.at(0), RO.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes returned by sibling iteration.
 */
TEST(NtNode, SiblingIteratorRemoveAll)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        Remove all siblings.

        Before:
        RW0
        |
        RW1----RW2----RW3----RW4---RW5 
        
        After:
        RW0     RW2     RW3     RW4     RW5
        |
        RW1
        */
        ecu_ntnode_sibling_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4), RW.at(5));

        /* Step 2: Action. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify correct nodes removed. */
        CHECK_TRUE( (is_root(RW.at(2), RW.at(3), RW.at(4), RW.at(5))) );
        CHECK_TRUE( (in_subtree(RW.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}
