/**
 * @file
 * @brief Unit tests for public API functions in @ref ntree.h. 
 * Test Summary:
 * 
 * @ref ECU_NTNODE_CHILD_FOR_EACH(), @ref ECU_NTNODE_CONST_CHILD_FOR_EACH(),
 * @ref ecu_ntnode_child_iterator_begin(), @ref ecu_ntnode_child_iterator_end(),
 * @ref ecu_ntnode_child_iterator_next(), @ref ecu_ntnode_child_iterator_cbegin(),
 * @ref ecu_ntnode_child_iterator_cend(), @ref ecu_ntnode_child_iterator_cnext(),
 * @ref ecu_ntnode_push_back(), @ref ecu_ntnode_remove(), @ref ecu_ntnode_is_root(),
 * @ref ecu_ntnode_in_subtree()
 *      - TEST(NTree, ChildIterator)
 *      - TEST(NTree, ConstChildIterator)
 *      - TEST(NTree, ChildIteratorOneChild)
 *      - TEST(NTree, ConstChildIteratorOneChild)
 *      - TEST(NTree, ChildIteratorNoChildren)
 *      - TEST(NTree, ConstChildIteratorNoChildren)
 *      - TEST(NTree, ChildIteratorRemoveSome)
 *      - TEST(NTree, ChildIteratorRemoveAll)
 * 
 * @ref ECU_NTNODE_PARENT_FOR_EACH(), @ref ECU_NTNODE_CONST_PARENT_FOR_EACH(),
 * @ref ecu_ntnode_parent_iterator_begin(), @ref ecu_ntnode_parent_iterator_end(),
 * @ref ecu_ntnode_parent_iterator_next(), @ref ecu_ntnode_parent_iterator_cbegin(),
 * @ref ecu_ntnode_parent_iterator_cend(), @ref ecu_ntnode_parent_iterator_cnext(),
 * @ref ecu_ntnode_push_back(), @ref ecu_ntnode_push_front(), @ref ecu_ntnode_push_front(), 
 * @ref ecu_ntnode_remove(), @ref ecu_ntnode_in_subtree(), @ref ecu_ntnode_is_root()
 *      - TEST(NTree, ParentIteratorLeaf)
 *      - TEST(NTree, ConstParentIteratorLeaf)
 *      - TEST(NTree, ParentIteratorMiddleChild)
 *      - TEST(NTree, ConstParentIteratorMiddleChild)
 *      - TEST(NTree, ParentIteratorOneParent)
 *      - TEST(NTree, ConstParentIteratorOneParent)
 *      - TEST(NTree, ParentIteratorRoot)
 *      - TEST(NTree, ConstParentIteratorRoot)
 *      - TEST(NTree, ParentIteratorRemoveSome)
 *      - TEST(NTree, ParentIteratorRemoveAll)
 * 
 * @ref ECU_NTNODE_SIBLING_FOR_EACH(), @ref ECU_NTNODE_CONST_SIBLING_FOR_EACH(),
 * @ref ecu_ntnode_sibling_iterator_begin(), @ref ecu_ntnode_sibling_iterator_end(),
 * @ref ecu_ntnode_sibling_iterator_next(), @ref ecu_ntnode_sibling_iterator_cbegin(),
 * @ref ecu_ntnode_sibling_iterator_cend(), @ref ecu_ntnode_sibling_iterator_cnext(),
 * @ref ecu_ntnode_push_back(), @ref ecu_ntnode_remove(), @ref ecu_ntnode_is_root(),
 * @ref ecu_ntnode_in_subtree()
 *      - TEST(NTree, SiblingIteratorFirst)
 *      - TEST(NTree, ConstSiblingIteratorFirst)
 *      - TEST(NTree, SiblingIteratorMiddle)
 *      - TEST(NTree, ConstSiblingIteratorMiddle)
 *      - TEST(NTree, SiblingIteratorLast)
 *      - TEST(NTree, ConstSiblingIteratorLast)
 *      - TEST(NTree, SiblingIteratorOneSibling)
 *      - TEST(NTree, ConstSiblingIteratorOneSibling)
 *      - TEST(NTree, SiblingIteratorNoSiblings)
 *      - TEST(NTree, ConstSiblingIteratorNoSiblings)
 *      - TEST(NTree, SiblingIteratorRemoveSome)
 *      - TEST(NTree, SiblingIteratorRemoveAll)
 * 
 * @ref ECU_NTNODE_PREORDER_FOR_EACH(), @ref ECU_NTNODE_CONST_PREORDER_FOR_EACH(),
 * @ref ecu_ntnode_preorder_iterator_begin(), @ref ecu_ntnode_preorder_iterator_end(),
 * @ref ecu_ntnode_preorder_iterator_next(), @ref ecu_ntnode_preorder_iterator_cbegin(),
 * @ref ecu_ntnode_preorder_iterator_cend(), @ref ecu_ntnode_preorder_iterator_cnext(),
 * @ref ecu_ntnode_push_back(), @ref ecu_ntnode_push_front()
 *      - TEST(NTree, PreorderIterator)
 *      - TEST(NTree, ConstPreorderIterator)
 *      - TEST(NTree, PreorderIteratorSubtree)
 *      - TEST(NTree, ConstPreorderIteratorSubtree)
 *      - TEST(NTree, PreorderIteratorSingleBranch)
 *      - TEST(NTree, ConstPreorderIteratorSingleBranch)
 *      - TEST(NTree, PreorderIteratorLeaf)
 *      - TEST(NTree, ConstPreorderIteratorLeaf)
 *      - TEST(NTree, PreorderIteratorRemove)
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

/*------------------------------------------------------------*/
/*------------------------- TEST GROUPS ----------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(NTree)
{
    struct visitor;

    struct ntnode : public ecu_ntnode
    {
    protected:
        ntnode()
        {
            ecu_ntnode_ctor(this, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        }

    public:
        virtual void accept(visitor& v) = 0;
        virtual void accept(visitor& v) const = 0;

        const ntnode *value() const
        {
            return this;
        }
    };

    /// @brief Read-write node. Write operations can be applied to it.
    struct rw_ntnode : public ntnode
    {
        void accept(visitor& v) override
        {
            v.visit(*this);
        }

        void accept(visitor& v) const override
        {
            v.visit(*this);
        }
    };

    /// @brief Read-only node. Write operations cannot be applied to it.
    struct ro_ntnode : public ntnode
    {
        void accept(visitor& v) override
        {
            v.visit(*this);
        }

        void accept(visitor& v) const override
        {
            v.visit(*this);
        }
    };

    /// @brief Visitor interface.
    struct visitor
    {
        /// @brief Any operation applied to read-write node.
        virtual void visit(rw_ntnode& n) = 0;

        /// @brief Only read-only operations can be applied since
        /// read-write node is const.
        virtual void visit(const rw_ntnode& n) = 0;
        
        /// @brief Only read-only operations can be applied since
        /// node is read-only type (and const).
        virtual void visit(const ro_ntnode& n) = 0;

    protected:
        visitor() = default;
    };

    struct apply_expectations : public visitor
    {
        void visit(rw_ntnode& n) override
        {
            mock().actualCall("node_in_tree")
                  .withParameter("value", n.value());
        }

        void visit(const rw_ntnode& n) override
        {
            mock().actualCall("node_in_tree")
                  .withParameter("value", n.value());
        }

        void visit(const ro_ntnode& n) override
        {
            mock().actualCall("node_in_tree")
                  .withParameter("value", n.value());
        }
    };

    struct remove : public visitor
    {
        void visit(rw_ntnode& n) override
        {
            ecu_ntnode_remove(&n);
        }

        void visit(const rw_ntnode& n) override
        {
            /* Do nothing since const. */
            (void)n;
        }

        void visit(const ro_ntnode& n) override
        {
            /* Do nothing since read-only node. */
            (void)n;
        }
    };

    /// @brief Strategy.
    /// Interface to test ECU tree iterators which are in C.
    /// begin(), end(), and next() functions return ecu_ntnode since
    /// some iterators return dummy delimiters not apart of this C++
    /// interface. The convert function acts a safety mechanism to
    /// convert the C ecu_ntnode into our C++ interface ONLY if the 
    /// returned node is valid (not a C delimiter).
    struct iterator
    {
        virtual ecu_ntnode *begin(ntnode& start) = 0;
        virtual ecu_ntnode *end() = 0;
        virtual ecu_ntnode *next() = 0;
        virtual ntnode& convert(ecu_ntnode *node) = 0;

        /// @brief Applies visitor to all nodes. Also tests FOR_EACH() macros.
        virtual void traverse(ntnode& start, visitor& v) = 0;

    protected:
        iterator() = default;
    };

    struct const_iterator
    {
        virtual const ecu_ntnode *cbegin(const ntnode& start) = 0;
        virtual const ecu_ntnode *cend() = 0;
        virtual const ecu_ntnode *cnext() = 0;
        virtual const ntnode& convert(const ecu_ntnode *node) = 0;

        /// @brief Applies visitor to all nodes. Also tests CONST_FOR_EACH() macros.
        virtual void traverse(const ntnode& start, visitor& v) = 0;

    protected:
        const_iterator() = default;
    };

    struct child_iterator : public iterator
    {
        ecu_ntnode *begin(ntnode& start) override
        {
            return ecu_ntnode_child_iterator_begin(&m_me, &start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_child_iterator_end(&m_me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_child_iterator_next(&m_me);
        }

        ntnode& convert(ecu_ntnode *node)
        {
            /* All children returned can be safely converted. */
            assert( (node) );
            return *static_cast<ntnode *>(node);
        }

        void traverse(ntnode& start, visitor& v) override
        {
            ECU_NTNODE_CHILD_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_child_iterator m_me;
    };

    struct const_child_iterator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ntnode& start) override
        {
            return ecu_ntnode_child_iterator_cbegin(&m_me, &start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_child_iterator_cend(&m_me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_child_iterator_cnext(&m_me);
        }

        const ntnode& convert(const ecu_ntnode *node) override
        {
            /* All children returned can be safely converted. */
            assert( (node) );
            return *static_cast<const ntnode *>(node);
        }

        void traverse(const ntnode& start, visitor& v) override
        {
            ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_child_citerator m_me;
    };

    struct parent_iterator : public iterator
    {
        ecu_ntnode *begin(ntnode& start) override
        {
            return ecu_ntnode_parent_iterator_begin(&m_me, &start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_parent_iterator_end(&m_me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_parent_iterator_next(&m_me);
        }

        ntnode& convert(ecu_ntnode *node) override
        {
            /* All parents returned can be safely converted. */
            assert( (node) );
            return *static_cast<ntnode *>(node);
        }

        void traverse(ntnode& start, visitor& v) override
        {
            ECU_NTNODE_PARENT_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_parent_iterator m_me;
    };

    struct const_parent_iterator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ntnode& start) override
        {
            return ecu_ntnode_parent_iterator_cbegin(&m_me, &start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_parent_iterator_cend(&m_me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_parent_iterator_cnext(&m_me);
        }

        const ntnode& convert(const ecu_ntnode *node) override
        {
            /* All parents returned can be safely converted. */
            assert( (node) );
            return *static_cast<const ntnode *>(node);
        }

        void traverse(const ntnode& start, visitor& v) override
        {
            ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_parent_citerator m_me;
    };

    struct sibling_iterator : public iterator
    {
        ecu_ntnode *begin(ntnode& start) override
        {
            return ecu_ntnode_sibling_iterator_begin(&m_me, &start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_sibling_iterator_end(&m_me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_sibling_iterator_next(&m_me);
        }

        ntnode& convert(ecu_ntnode *node) override
        {
            /* All siblings returned can be safely converted. */
            assert( (node) );
            return *static_cast<ntnode *>(node);
        }

        void traverse(ntnode& start, visitor& v) override
        {
            ECU_NTNODE_SIBLING_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_sibling_iterator m_me;
    };

    struct const_sibling_iterator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ntnode& start) override
        {
            return ecu_ntnode_sibling_iterator_cbegin(&m_me, &start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_sibling_iterator_cend(&m_me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_sibling_iterator_cnext(&m_me);
        }

        const ntnode& convert(const ecu_ntnode *node) override
        {
            /* All siblings returned can be safely converted. */
            assert( (node) );
            return *static_cast<const ntnode *>(node);
        }

        void traverse(const ntnode& start, visitor& v) override
        {
            ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_sibling_citerator m_me;
    };

    struct preorder_iterator : public iterator
    {
        ecu_ntnode *begin(ntnode& start) override
        {
            return ecu_ntnode_preorder_iterator_begin(&m_me, &start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_preorder_iterator_end(&m_me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_preorder_iterator_next(&m_me);
        }

        ntnode& convert(ecu_ntnode *node) override
        {
            /* Delimiter node is in C-struct so cannot be converted. */
            assert( (node) );
            assert( (node != &m_me.delimiter) );
            return *static_cast<ntnode *>(node);
        }

        void traverse(ntnode& start, visitor& v) override
        {
            ECU_NTNODE_PREORDER_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_preorder_iterator m_me;
    };

    struct const_preorder_iterator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ntnode& start) override
        {
            return ecu_ntnode_preorder_iterator_cbegin(&m_me, &start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_preorder_iterator_cend(&m_me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_preorder_iterator_cnext(&m_me);
        }

        const ntnode& convert(const ecu_ntnode *node) override
        {
            /* Delimiter node is in C-struct so cannot be converted. */
            assert( (node) );
            assert( (node != &m_me.delimiter) );
            return *static_cast<const ntnode *>(node);
        }

        void traverse(const ntnode& start, visitor& v) override
        {
            ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_preorder_citerator m_me;
    };

    struct postorder_iterator : public iterator
    {
        ecu_ntnode *begin(ntnode& start) override
        {
            return ecu_ntnode_postorder_iterator_begin(&m_me, &start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_postorder_iterator_end(&m_me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_postorder_iterator_next(&m_me);
        }

        ntnode& convert(ecu_ntnode *node) override
        {
            /* Delimiter node is in C-struct so cannot be converted. */
            assert( (node) );
            assert( (node != &m_me.delimiter) );
            return *static_cast<ntnode *>(node);
        }

        void traverse(ntnode& start, visitor& v) override
        {
            ECU_NTNODE_POSTORDER_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_postorder_iterator m_me;
    };

    struct const_postorder_iterator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ntnode& start) override
        {
            return ecu_ntnode_postorder_iterator_cbegin(&m_me, &start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_postorder_iterator_cend(&m_me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_postorder_iterator_cnext(&m_me);
        }

        const ntnode& convert(const ecu_ntnode *node) override
        {
            /* Delimiter node is in C-struct so cannot be converted. */
            assert( (node) );
            assert( (node != &m_me.delimiter) );
            return *static_cast<const ntnode *>(node);
        }

        void traverse(const ntnode& start, visitor& v) override
        {
            ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &m_me, &start)
            {
                convert(n).accept(v);
            }
        }

        ecu_ntnode_postorder_citerator m_me;
    };

    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    /// @brief Expect nodes to be in tree in specific order.
    template<typename... Nodes>
    requires ((std::is_base_of_v<ntnode, Nodes> && !std::is_same_v<ntnode, Nodes>) && ...)
    static void EXPECT_NODES_IN_ORDER(const ntnode &n1, const Nodes&... n)
    {
        mock().strictOrder();
        mock().expectOneCall("node_in_tree")
              .withParameter("value", n1.value());

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock().expectOneCall("node_in_tree")
                   .withParameter("value", n.value()), ...);
        }
    }

    /// @brief Calls the node_in_tree mock on an entire tree/subtree.
    /// Order of applied expectations depends on concrete iterator passed
    /// in.
    template<typename T>
    requires ((std::is_base_of_v<iterator, T> && !std::is_same_v<iterator, T>) || 
              (std::is_base_of_v<const_iterator, T> && !std::is_same_v<const_iterator, T>))
    static void TREE_APPLY_EXPECTATIONS(ntnode& start, T&& iter)
    {
        apply_expectations expectations;
        iter.traverse(start, expectations);
    }

    /// @brief Helper to create test tree. Adds any number of children to parent.
    template<typename... Nodes>
    requires ((std::is_base_of_v<ntnode, Nodes> && !std::is_same_v<ntnode, Nodes>) && ...)
    static void add_children(ntnode& parent, ntnode& c0, Nodes&... cn)
    {
        ecu_ntnode_push_back(&parent, &c0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            (ecu_ntnode_push_back(&parent, &cn), ...);
        }
    }

    /// @brief Helper to create a test tree. Creates a branch by adding children,
    /// grandchildren, etc. I.e. add_branch(0, 1, 2) means 1 is child of 0 and 
    /// 2 is grandchild of 0.
    template<typename... Nodes>
    requires ((std::is_base_of_v<ntnode, Nodes> && !std::is_same_v<ntnode, Nodes>) && ...)
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
        ecu_ntnode_push_front(&parent, &child);
    }

    /// @brief Helper to evaluate pre and post conditions for unit tests.
    /// Applies supplied condition function to all nodes. Terminates
    /// and returns false as soon as a false condition occurs. Otherwise
    /// all nodes pass and this function returns true. If additional 
    /// arguments must be passed to function use std::bind. Function takes
    /// in pointer instead of reference so ECU tree functions can be supplied.
    template<typename... Nodes>
    requires ((std::is_base_of_v<ntnode, Nodes> && !std::is_same_v<ntnode, Nodes>) && ...)
    static bool condition(const std::function<bool(const ntnode *)>& c, const ntnode& n1, const Nodes&... n)
    {
        bool status = c(&n1);

        if constexpr(sizeof...(Nodes) > 0)
        {
            status = status && ((c(&n)) && ...); /* Only keep evaluating if true return. */
        }

        return status;
    }

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
TEST(NTree, ChildIterator)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_ORDER(RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(0), child_iterator());

        /* Step 3: Assert. Test fails if expected children not iterated over. */
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
TEST(NTree, ConstChildIterator)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_ORDER(RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(0), const_child_iterator());

        /* Step 3: Assert. Test fails if expected children not iterated over. */
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
TEST(NTree, ChildIteratorOneChild)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_ORDER(RW.at(5));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(2), child_iterator());

        /* Step 3: Assert. Test fails if expected children not iterated over. */
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
TEST(NTree, ConstChildIteratorOneChild)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        EXPECT_NODES_IN_ORDER(RW.at(5));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(2), const_child_iterator());

        /* Step 3: Assert. Test fails if expected children not iterated over. */
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
TEST(NTree, ChildIteratorNoChildren)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        add_children(RW.at(0), RW.at(1));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(1), child_iterator());

        /* Step 3: Assert. Test fails if iteration does not terminate immediately. */
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
TEST(NTree, ConstChildIteratorNoChildren)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        add_children(RW.at(0), RW.at(1));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(1), const_child_iterator());

        /* Step 3: Assert. Test fails if iteration does not terminate immediately. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove some nodes in the middle of a child iteration.
 * 
 * @note Nodes are only checked if they are in a tree or not
 * for test verification to limit dependencies.
 */
TEST(NTree, ChildIteratorRemoveSome)
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
        add_children(RW.at(0), RW.at(1), RO.at(0), RW.at(2), RO.at(1));
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(1), RO.at(0), RW.at(2), RO.at(1))) ); /* Precondition. */

        /* Step 2: Action. */
        remove r;
        child_iterator iter;
        iter.traverse(RW.at(0), r);

        /* Step 3: Assert. Test fails if proper nodes not removed. */
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RO.at(0), RO.at(1))) );
        CHECK_TRUE( (condition(&ecu_ntnode_is_root, RW.at(1), RW.at(2))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes returned by child iteration.
 * 
 * @note Nodes are only checked if they are in a tree or not
 * for test verification to limit dependencies.
 */
TEST(NTree, ChildIteratorRemoveAll)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(1), RW.at(2), RW.at(3), RW.at(4))) ); /* Precondition. */

        /* Step 2: Action. */
        remove r;
        child_iterator iter;
        iter.traverse(RW.at(0), r);

        /* Step 3: Assert. Test fails if proper nodes not removed. */
        CHECK_TRUE( (condition(&ecu_ntnode_is_root, RW.at(1), RW.at(2), RW.at(3), RW.at(4))) );
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
TEST(NTree, ParentIteratorLeaf)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_ORDER(RW.at(4), RW.at(3), RW.at(2), RW.at(0));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(5), parent_iterator());

        /* Step 3: Assert. Test fails if expected parents not iterated over. */
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
TEST(NTree, ConstParentIteratorLeaf)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_ORDER(RW.at(4), RW.at(3), RW.at(2), RW.at(0));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(5), const_parent_iterator());

        /* Step 3: Assert. Test fails if expected parents not iterated over. */
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
TEST(NTree, ParentIteratorMiddleChild)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_ORDER(RW.at(2), RW.at(0));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(3), parent_iterator());

        /* Step 3: Assert. Test fails if expected parents not iterated over. */
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
TEST(NTree, ConstParentIteratorMiddleChild)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_ORDER(RW.at(2), RW.at(0));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(3), const_parent_iterator());

        /* Step 3: Assert. Test fails if expected parents not iterated over. */
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
TEST(NTree, ParentIteratorOneParent)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_ORDER(RW.at(0));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(1), parent_iterator());

        /* Step 3: Assert. Test fails if expected parents not iterated over. */
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
TEST(NTree, ConstParentIteratorOneParent)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_IN_ORDER(RW.at(0));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(1), const_parent_iterator());

        /* Step 3: Assert. Test fails if expected parents not iterated over. */
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
TEST(NTree, ParentIteratorRoot)
{
    try
    {
        /* Step 1: Arrrange. Start iteration at RW0.
        RW0
        |
        RW1 
        */
        add_children(RW.at(0), RW.at(1));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(0), parent_iterator());

        /* Step 3: Assert. Test fails if iteration does not terminate immediately. */
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
TEST(NTree, ConstParentIteratorRoot)
{
    try
    {
        /* Step 1: Arrrange. Start iteration at RW0.
        RW0
        |
        RW1 
        */
        add_children(RW.at(0), RW.at(1));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(0), const_parent_iterator());

        /* Step 3: Assert. Test fails if iteration does not terminate immediately. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove some nodes in the middle of a parent iteration.
 * 
 * @note Nodes are only checked if they are in a tree or not
 * for test verification to limit dependencies.
 */
TEST(NTree, ParentIteratorRemoveSome)
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
        add_children(RO.at(0), RW.at(0), RW.at(1), RW.at(4));
        add_branch(RW.at(1), RO.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(4), RW.at(5));
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(3), RW.at(2), RO.at(1), RW.at(1))) ); /* Precondition. */
        
        /* Step 2: Action. */
        remove r;
        parent_iterator iter;
        iter.traverse(RW.at(3), r);

        /* Step 3: Assert. Test fails if proper nodes not removed. */
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(3), RO.at(1))) );
        CHECK_TRUE( (condition(&ecu_ntnode_is_root, RW.at(2), RW.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes returned by parent iteration.
 * 
 * @note Nodes are only checked if they are in a tree or not
 * for test verification to limit dependencies.
 */
TEST(NTree, ParentIteratorRemoveAll)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(5), RW.at(4), RW.at(3), RW.at(2))) ); /* Precondition. */
        
        /* Step 2: Action. */
        remove r;
        parent_iterator iter;
        iter.traverse(RW.at(5), r);

        /* Step 3: Assert. Test fails if proper nodes not removed. */
        CHECK_TRUE( (ecu_ntnode_in_subtree(&RW.at(5))) );
        CHECK_TRUE( (condition(&ecu_ntnode_is_root, RW.at(4), RW.at(3), RW.at(2))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - SIBLING ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Start iteration at first sibling.
 */
TEST(NTree, SiblingIteratorFirst)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(4), sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, ConstSiblingIteratorFirst)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(4), const_sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, SiblingIteratorMiddle)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(7), RW.at(8), RW.at(4), RW.at(5));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(6), sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, ConstSiblingIteratorMiddle)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(7), RW.at(8), RW.at(4), RW.at(5));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(6), const_sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, SiblingIteratorLast)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(4), RW.at(5), RW.at(6), RW.at(7));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(8), sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, ConstSiblingIteratorLast)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(4), RW.at(5), RW.at(6), RW.at(7));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(8), const_sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, SiblingIteratorOneSibling)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(10));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(9), sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, ConstSiblingIteratorOneSibling)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        EXPECT_NODES_IN_ORDER(RW.at(10));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(9), const_sibling_iterator());

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
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
TEST(NTree, SiblingIteratorNoSiblings)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        add_children(RW.at(0), RW.at(1));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(1), sibling_iterator());

        /* Step 3: Assert. Test fails if iteration does not terminate immediately. */
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
TEST(NTree, ConstSiblingIteratorNoSiblings)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1 
        */
        add_children(RW.at(0), RW.at(1));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(1), const_sibling_iterator());

        /* Step 3: Assert. Test fails if iteration does not terminate immediately. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove some nodes in the middle of a sibling iteration.
 * 
 * @note Nodes are only checked if they are in a tree or not
 * for test verification to limit dependencies.
 */
TEST(NTree, SiblingIteratorRemoveSome)
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
        add_children(RW.at(0), RW.at(1), RO.at(0), RW.at(2), RW.at(3), RO.at(1));
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(1), RO.at(0), RW.at(2), RW.at(3), RO.at(1))) ); /* Precondition. */

        /* Step 2: Action. */
        remove r;
        sibling_iterator iter;
        iter.traverse(RW.at(1), r);
        iter.traverse(RW.at(1), r); /* Run iteration a few more times to verify no more nodes removed. */
        iter.traverse(RW.at(1), r); /* Run iteration a few more times to verify no more nodes removed. */

        /* Step 3: Assert. Test fails if proper nodes not removed. */
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(1), RO.at(0), RO.at(1))) );
        CHECK_TRUE( (condition(&ecu_ntnode_is_root, RW.at(2), RW.at(3))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes returned by sibling iteration.
 * 
 * @note Nodes are only checked if they are in a tree or not
 * for test verification to limit dependencies.
 */
TEST(NTree, SiblingIteratorRemoveAll)
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
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        CHECK_TRUE( (condition(&ecu_ntnode_in_subtree, RW.at(1), RW.at(2), RW.at(3), RW.at(4), RW.at(5))) ); /* Precondition. */

        /* Step 2: Action. */
        remove r;
        sibling_iterator iter;
        iter.traverse(RW.at(1), r);
        iter.traverse(RW.at(1), r); /* Run iteration a few more times to verify no more nodes removed. */
        iter.traverse(RW.at(1), r); /* Run iteration a few more times to verify no more nodes removed. */

        /* Step 3: Assert. Test fails if proper nodes not removed. */
        CHECK_TRUE( (ecu_ntnode_in_subtree(&RW.at(1))) );
        CHECK_TRUE( (condition(&ecu_ntnode_is_root, RW.at(2), RW.at(3), RW.at(4), RW.at(5))) );
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
 * @brief General iteration test. Verify all nodes
 * iterated over.
 */
TEST(NTree, PreorderIterator)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        RW0
        |
        RW1---------------RW2---RW3-----RW4
        |                       |       |
        RW5---RW6---RW7         RW8     RW9
                                        |
                                        RW10----RW11
                                        |
                                        RW12
        */
       add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
       add_children(RW.at(1), RW.at(5), RW.at(6), RW.at(7));
       add_children(RW.at(3), RW.at(8));
       add_branch(RW.at(4), RW.at(9), RW.at(10), RW.at(12));
       add_children(RW.at(9), RW.at(11));

       EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(5), RW.at(6), RW.at(7),
                             RW.at(2), RW.at(3), RW.at(8), RW.at(4), RW.at(9),
                             RW.at(10), RW.at(12), RW.at(11));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(0), preorder_iterator());

        /* Step 3: Assert. Test fails if expected nodes not iterated over. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief General iteration test. Verify all nodes
 * iterated over.
 */
TEST(NTree, ConstPreorderIterator)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        RW0
        |
        RW1---------------RW2---RW3-----RW4
        |                       |       |
        RW5---RW6---RW7         RW8     RW9
                                        |
                                        RW10----RW11
                                        |
                                        RW12
        */
       add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
       add_children(RW.at(1), RW.at(5), RW.at(6), RW.at(7));
       add_children(RW.at(3), RW.at(8));
       add_branch(RW.at(4), RW.at(9), RW.at(10), RW.at(12));
       add_children(RW.at(9), RW.at(11));

       EXPECT_NODES_IN_ORDER(RW.at(0), RW.at(1), RW.at(5), RW.at(6), RW.at(7),
                             RW.at(2), RW.at(3), RW.at(8), RW.at(4), RW.at(9),
                             RW.at(10), RW.at(12), RW.at(11));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(RW.at(0), const_preorder_iterator());

        /* Step 3: Assert. Test fails if expected nodes not iterated over. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a node that is in a tree.
 */
TEST(NTree, PreorderIteratorSubtree)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a node that is in a tree.
 */
TEST(NTree, ConstPreorderIteratorSubtree)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Iterate over tree that has a single branch.
 */
TEST(NTree, PreorderIteratorSingleBranch)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Iterate over a tree that has a single branch.
 */
TEST(NTree, ConstPreorderIteratorSingleBranch)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a leaf node. Iteration should
 * return the leaf then exit. This is the same behavior as
 * starting at an empty root.
 */
TEST(NTree, PreorderIteratorLeaf)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at a leaf node. Iteration should
 * return the leaf then exit. This is the same behavior as
 * starting at an empty root.
 */
TEST(NTree, ConstPreorderIteratorLeaf)
{
    try
    {
        /* code */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Removing nodes in the middle of a preorder
 * iteration is prohibited. Assert should fire.
 */
TEST(NTree, PreorderIteratorRemove)
{
    try
    {
        /* code */
        // EXPECT_ASSERTION();
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}
