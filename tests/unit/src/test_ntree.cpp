/**
 * @file
 * @brief Unit tests for public API functions in @ref ntree.h. 
 * Test Summary:
 * 
 * @ref ECU_NTNODE_CHILD_FOR_EACH(), @ref ECU_NTNODE_CONST_CHILD_FOR_EACH(), 
 * @ref ecu_ntnode_child_iterator_begin(), @ref ecu_ntnode_child_iterator_end(),
 * @ref ecu_ntnode_child_iterator_next(), @ref ecu_ntnode_child_iterator_cbegin(),
 * @ref ecu_ntnode_child_iterator_cend(), @ref ecu_ntnode_child_iterator_cnext()
 *      - TEST(Tree, SiblingIteratorGeneralTest)
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
/*----------------------- FILE-SCOPE TYPES -------------------*/
/*------------------------------------------------------------*/

namespace 
{
struct test_node : public ecu_ntnode
{
    test_node()
    {
        ecu_ntnode_ctor(this, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    }
};
}

/*------------------------------------------------------------*/
/*------------------------- TEST GROUPS ----------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Tree)
{
    enum class iterator_type : std::uint8_t
    {
        CHILD,
        PARENT,
        SIBLING,
        PREORDER,
        POSTORDER
    };

    struct iterator
    {
        virtual ecu_ntnode *begin(ecu_ntnode *start) = 0;
        virtual ecu_ntnode *end() = 0;
        virtual ecu_ntnode *next() = 0;
        virtual test_node& convert(ecu_ntnode *node) = 0;

    protected:
        iterator() = default;
    };

    struct const_iterator
    {
        virtual const ecu_ntnode *cbegin(const ecu_ntnode *start) = 0;
        virtual const ecu_ntnode *cend() = 0;
        virtual const ecu_ntnode *cnext() = 0;
        virtual const test_node& convert(const ecu_ntnode *node) = 0;

    protected:
        const_iterator() = default;
    };

    struct child_iterator : public iterator
    {
        ecu_ntnode *begin(ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_child_iterator_begin(&me, start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_child_iterator_end(&me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_child_iterator_next(&me);
        }

        test_node& convert(ecu_ntnode *node)
        {
            assert( (node) );
            return *static_cast<test_node *>(node);
        }

        ecu_ntnode_child_iterator me;
    };

    struct child_citerator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_child_iterator_cbegin(&me, start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_child_iterator_cend(&me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_child_iterator_cnext(&me);
        }

        const test_node& convert(const ecu_ntnode *node) override
        {
            assert( (node) );
            return *static_cast<const test_node *>(node);
        }

        ecu_ntnode_child_citerator me;
    };

    struct parent_iterator : public iterator
    {
        ecu_ntnode *begin(ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_parent_iterator_begin(&me, start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_parent_iterator_end(&me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_parent_iterator_next(&me);
        }

        test_node& convert(ecu_ntnode *node) override
        {
            assert( (node) );
            return *static_cast<test_node *>(node);
        }

        ecu_ntnode_parent_iterator me;
    };

    struct parent_citerator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_parent_iterator_cbegin(&me, start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_parent_iterator_cend(&me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_parent_iterator_cnext(&me);
        }

        const test_node& convert(const ecu_ntnode *node) override
        {
            assert( (node) );
            return *static_cast<const test_node *>(node);
        }

        ecu_ntnode_parent_citerator me;
    };

    struct sibling_iterator : public iterator
    {
        ecu_ntnode *begin(ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_sibling_iterator_begin(&me, start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_sibling_iterator_end(&me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_sibling_iterator_next(&me);
        }

        test_node& convert(ecu_ntnode *node) override
        {
            assert( (node) );
            return *static_cast<test_node *>(node);
        }

        ecu_ntnode_sibling_iterator me;
    };

    struct sibling_citerator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_sibling_iterator_cbegin(&me, start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_sibling_iterator_cend(&me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_sibling_iterator_cnext(&me);
        }

        const test_node& convert(const ecu_ntnode *node) override
        {
            assert( (node) );
            return *static_cast<const test_node *>(node);
        }

        ecu_ntnode_sibling_citerator me;
    };

    struct preorder_iterator : public iterator
    {
        ecu_ntnode *begin(ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_preorder_iterator_begin(&me, start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_preorder_iterator_end(&me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_preorder_iterator_next(&me);
        }

        test_node& convert(ecu_ntnode *node) override
        {
            assert( (node) );
            assert( (node != &me.delimiter) );
            return *static_cast<test_node *>(node);
        }

        ecu_ntnode_preorder_iterator me;
    };

    struct preorder_citerator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_preorder_iterator_cbegin(&me, start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_preorder_iterator_cend(&me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_preorder_iterator_cnext(&me);
        }

        const test_node& convert(const ecu_ntnode *node) override
        {
            assert( (node) );
            assert( (node != &me.delimiter) );
            return *static_cast<const test_node *>(node);
        }

        ecu_ntnode_preorder_citerator me;
    };

    struct postorder_iterator : public iterator
    {
        ecu_ntnode *begin(ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_postorder_iterator_begin(&me, start);
        }

        ecu_ntnode *end() override
        {
            return ecu_ntnode_postorder_iterator_end(&me);
        }

        ecu_ntnode *next() override
        {
            return ecu_ntnode_postorder_iterator_next(&me);
        }

        test_node& convert(ecu_ntnode *node) override
        {
            assert( (node) );
            assert( (node != &me.delimiter) );
            return *static_cast<test_node *>(node);
        }

        ecu_ntnode_postorder_iterator me;
    };

    struct postorder_citerator : public const_iterator
    {
        const ecu_ntnode *cbegin(const ecu_ntnode *start) override
        {
            assert( (start) );
            return ecu_ntnode_postorder_iterator_cbegin(&me, start);
        }

        const ecu_ntnode *cend() override
        {
            return ecu_ntnode_postorder_iterator_cend(&me);
        }

        const ecu_ntnode *cnext() override
        {
            return ecu_ntnode_postorder_iterator_cnext(&me);
        }

        const test_node& convert(const ecu_ntnode *node) override
        {
            assert( (node) );
            assert( (node != &me.delimiter) );
            return *static_cast<const test_node *>(node);
        }

        ecu_ntnode_postorder_citerator me;
    };

    std::shared_ptr<iterator> create_iterator(iterator_type type)
    {
        std::shared_ptr<iterator> instance;

        switch (type)
        {
            case iterator_type::CHILD:
            {
                instance = std::shared_ptr<iterator>(new child_iterator);
                break;
            }

            case iterator_type::PARENT:
            {
                instance = std::shared_ptr<iterator>(new parent_iterator);
                break;
            }

            case iterator_type::SIBLING:
            {
                instance = std::shared_ptr<iterator>(new sibling_iterator);
                break;
            }

            case iterator_type::PREORDER:
            {
                instance = std::shared_ptr<iterator>(new preorder_iterator);
                break;
            }

            case iterator_type::POSTORDER:
            {
                instance = std::shared_ptr<iterator>(new postorder_iterator);
                break;
            }

            default:
            {
                assert( (false) );
                break;
            }
        }

        return instance;
    }

    std::shared_ptr<const_iterator> create_const_iterator(iterator_type type)
    {
        std::shared_ptr<const_iterator> instance;

        switch (type)
        {
            case iterator_type::CHILD:
            {
                instance = std::shared_ptr<const_iterator>(new child_citerator);
                break;
            }

            case iterator_type::PARENT:
            {
                instance = std::shared_ptr<const_iterator>(new parent_citerator);
                break;
            }

            case iterator_type::SIBLING:
            {
                instance = std::shared_ptr<const_iterator>(new sibling_citerator);
                break;
            }

            case iterator_type::PREORDER:
            {
                instance = std::shared_ptr<const_iterator>(new preorder_citerator);
                break;
            }

            case iterator_type::POSTORDER:
            {
                instance = std::shared_ptr<const_iterator>(new postorder_citerator);
                break;
            }

            default:
            {
                assert( (false) );
                break;
            }
        }

        return instance;
    }

    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void create_default_tree()
    {
        #warning "TODO:"
    }

    /**
     * @brief Expect nodes to be in tree. Order does not matter.
     */
    template<typename... Nodes>
    requires (std::is_same_v<test_node, Nodes> && ...)
    static void EXPECT_NODES(test_node &n1, Nodes&... tn)
    {
        mock().expectOneCall("node")
              .withParameter("value", static_cast<const void *>(&n1));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock().expectOneCall("node")
                   .withParameter("value", static_cast<const void *>(&tn)), ...);
        }
    }

    /**
     * @brief Expect nodes to be in tree in specific order.
     */
    template<typename... Nodes>
    requires (std::is_same_v<test_node, Nodes> && ...)
    static void EXPECT_NODES_IN_ORDER(test_node &n1, Nodes&... tn)
    {
        mock().strictOrder();
        mock().expectOneCall("node_expectation")
              .withParameter("value", static_cast<const void *>(&n1));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock().expectOneCall("node_expectation")
                   .withParameter("value", static_cast<const void *>(&tn)), ...);
        }
    }

    /**
     * @brief Applies user-defined action to all nodes returned by iteration.
     */
    template<typename... Args>
    static void visit(test_node& start, 
                      iterator *iter, 
                      const std::function<void(test_node&, Args...)>& func,
                      Args&&... args)
    {
        assert( (iter && func) );

        for (auto i = iter->begin();
             i != iter->end();
             i = iter->next())
        {
            func(iter->convert(i), std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    static void visit(const test_node& start, 
                      const_iterator *iter, 
                      const std::function<void(const test_node&, Args...)>& func,
                      Args&&... args)
    {
        assert( (iter && func) );

        for (auto i = iter->cbegin();
             i != iter->cend();
             i = iter->cnext())
        {
            func(iter->convert(i), std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    static void visit(test_node& start, 
                      const std::shared_ptr<iterator>& iter, 
                      const std::function<void(test_node&, Args...)>& func,
                      Args&&... args)
    {
        assert( (iter && func) );
        visit(start, iter.get(), func, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void visit(const test_node& start, 
                      const std::shared_ptr<const_iterator>& iter, 
                      const std::function<void(const test_node&, Args...)>& func,
                      Args&&... args)
    {
        assert( (iter && func) );
        visit(start, iter.get(), func, std::forward<Args>(args)...);
    }

    /**
     * @brief Calls the node_expectation mock on an entire tree/subtree.
     * Order of applied expectations depends on concrete iterator passed
     * in.
     */
    static void TREE_APPLY_EXPECTATIONS(test_node& start, const std::shared_ptr<iterator>& iter)
    {
        static const std::function<void(test_node&)> func = [](test_node& n)
        {
            mock().actualCall("node_expectation")
                  .withParameter("value", static_cast<const void *>(&n));
        };

        visit(start, iter, func);
    }

    static void TREE_APPLY_EXPECTATIONS(const test_node& start, const std::shared_ptr<const_iterator>& iter)
    {
        static const std::function<void(const test_node&)> func = [](const test_node& n)
        {
            mock().actualCall("node_expectation")
                  .withParameter("value", static_cast<const void *>(&n));
        };

        visit(start, iter, func);
    }

    template<typename... Nodes>
    requires (std::is_same_v<test_node, Nodes> && ...)
    static void add_children(test_node& parent, test_node& c1, Nodes&... cn)
    {
        ecu_ntnode_push_back(static_cast<ecu_ntnode *>(&parent), static_cast<ecu_ntnode *>(&c1));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (ecu_ntnode_push_back(static_cast<ecu_ntnode *>(&parent), static_cast<ecu_ntnode *>(&cn)), ...);
        }
    }

    std::array<test_node, 50> N;
};


/*------------------------------------------------------------*/
/*------------------ TESTS - SIBLING ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Start iteration at first sibling.
 */
TEST(Tree, SiblingIteratorFirst)
{
    try
    {
        /* Step 1: Arrange. Start iteration at N4.
        N0
        |
        N1--N2----------------------N3
            |                       |
            N4--N5--N6--N7--N8      N9---N10 */
        add_children(N.at(0), N.at(1), N.at(2), N.at(3));
        add_children(N.at(2), N.at(4), N.at(5), N.at(6), N.at(7), N.at(8));
        add_children(N.at(3), N.at(9), N.at(10));
        EXPECT_NODES_IN_ORDER(N.at(5), N.at(6), N.at(7), N.at(8));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(N.at(4), create_iterator(iterator_type::SIBLING));

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
TEST(Tree, SiblingIteratorMiddle)
{
    try
    {
        /* Step 1: Arrange. Start iteration at N6.
        N0
        |
        N1--N2----------------------N3
            |                       |
            N4--N5--N6--N7--N8      N9---N10 */
        add_children(N.at(0), N.at(1), N.at(2), N.at(3));
        add_children(N.at(2), N.at(4), N.at(5), N.at(6), N.at(7), N.at(8));
        add_children(N.at(3), N.at(9), N.at(10));
        EXPECT_NODES_IN_ORDER(N.at(7), N.at(8), N.at(4), N.at(5));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(N.at(6), create_iterator(iterator_type::SIBLING));

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
TEST(Tree, SiblingIteratorLast)
{
    try
    {
        /* Step 1: Arrange. Start iteration at N8.
        N0
        |
        N1--N2----------------------N3
            |                       |
            N4--N5--N6--N7--N8      N9---N10 */
        add_children(N.at(0), N.at(1), N.at(2), N.at(3));
        add_children(N.at(2), N.at(4), N.at(5), N.at(6), N.at(7), N.at(8));
        add_children(N.at(3), N.at(9), N.at(10));
        EXPECT_NODES_IN_ORDER(N.at(4), N.at(5), N.at(6), N.at(7));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(N.at(8), create_iterator(iterator_type::SIBLING));

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
TEST(Tree, SiblingIteratorNoSiblings)
{
    try
    {
        /* Step 1: Arrange. Start iteration at N1.
        N0
        |
        N1 */
        add_children(N.at(0), N.at(1));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(N.at(1), create_iterator(iterator_type::SIBLING));

        /* Step 3: Assert. Test fails if iterator does not terminate immediately. */
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
TEST(Tree, SiblingIteratorOneSibling)
{
    try
    {
        /* Step 1: Arrange. Start iteration at N9.
        N0
        |
        N1--N2----------------------N3
            |                       |
            N4--N5--N6--N7--N8      N9---N10 */
        add_children(N.at(0), N.at(1), N.at(2), N.at(3));
        add_children(N.at(2), N.at(4), N.at(5), N.at(6), N.at(7), N.at(8));
        add_children(N.at(3), N.at(9), N.at(10));
        EXPECT_NODES_IN_ORDER(N.at(10));

        /* Step 2: Action. */
        TREE_APPLY_EXPECTATIONS(N.at(9), create_iterator(iterator_type::SIBLING));

        /* Step 3: Assert. Test fails if expected siblings not iterated over. */
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove nodes in the middle of a sibling iteration.
 */
TEST(Tree, SiblingIteratorRemove)
{
    try
    {
        /* Step 1: Arrange. Start iteration at N1 and remove all siblings.
        N0
        |
        N1--N2----------------------N3
            |                       |
            N4--N5--N6--N7--N8      N9---N10 */
        add_children(N.at(0), N.at(1), N.at(2), N.at(3));
        add_children(N.at(2), N.at(4), N.at(5), N.at(6), N.at(7), N.at(8));
        add_children(N.at(3), N.at(9), N.at(10));

    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}
