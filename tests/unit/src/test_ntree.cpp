/**
 * @file
 * @brief Unit tests for public API functions in @ref ntree.h. 
 * Test Summary:
 * 
 * Api:
 * 
 * @ref ecu_ntnode_destroy()
 *      - TEST(NtNode, DestroyNodeIsNonEmptyRoot)
 *      - TEST(NtNode, DestroyNodeIsNonEmptySubroot)
 *      - TEST(NtNode, DestroyNodeIsLeaf)
 *      - TEST(NtNode, DestroyNodeIsEmptyRoot)
 *      - TEST(NtNode, DestroyUseAPIInCallback)
 * 
 * @ref ecu_ntnode_clear()
 *      - TEST(NtNode, ClearNodeIsNonEmptyRoot)
 *      - TEST(NtNode, ClearNodeIsNonEmptySubroot)
 *      - TEST(NtNode, ClearNodeIsLeaf)
 *      - TEST(NtNode, ClearNodeIsEmptyRoot)
 * 
 * @ref ecu_ntnode_count()
 *      - TEST(NtNode, CountNodeWithMultipleChildren)
 *      - TEST(NtNode, CountNodeWithOneChild)
 *      - TEST(NtNode, CountNodeWithNoChildren)
 *      - TEST(NtNode, CountAddAndRemoveChildren)
 * 
 * @ref ecu_ntnode_empty()
 *      - TEST(NtNode, EmptyNodeIsRootWithOneChild)
 *      - TEST(NtNode, EmptyNodeIsEmptyRoot)
 *      - TEST(NtNode, EmptyNodeIsSubrootWithOneChild)
 *      - TEST(NtNode, EmptyNodeIsLeaf)
 *      - TEST(NtNode, EmptyAddAndRemoveNodes)
 * 
 * @ref ecu_ntnode_first_child(), @ref ecu_ntnode_first_cchild()
 *      - TEST(NtNode, FirstChildNodeWithMultipleChildren)
 *      - TEST(NtNode, FirstChildNodeWithOneChild)
 *      - TEST(NtNode, FirstChildNodeWithNoChildren)
 * 
 * @ref ecu_ntnode_id()
 *      - TEST(NtNode, Id)
 * 
 * @ref ecu_ntnode_in_subtree()
 *      - TEST(NtNode, InSubtreeNodeIsNonEmptyRoot)
 *      - TEST(NtNode, InSubtreeNodeIsEmptyRoot)
 *      - TEST(NtNode, InSubtreeNodeIsEmptySubroot)
 *      - TEST(NtNode, InSubtreeNodeIsLeaf)
 *      - TEST(NtNode, InSubtreeAddAndRemoveNode)
 * 
 * @ref ecu_ntnode_insert_sibling_after()
 *      - TEST(NtNode, InsertSiblingAfterPosIsLeftMostSubroot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsMiddleSubroot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsRightMostSubroot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsLeaf)
 *      - TEST(NtNode, InsertSiblingAfterPosIsRoot)
 *      - TEST(NtNode, InsertSiblingAfterPosEqualsSibling)
 *      - TEST(NtNode, InsertSiblingAfterSiblingInTree)
 * 
 * @ref ecu_ntnode_insert_sibling_before()
 *      - TEST(NtNode, InsertSiblingBeforePosIsLeftMostSubroot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsMiddleSubroot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsRightMostSubroot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsLeaf)
 *      - TEST(NtNode, InsertSiblingBeforePosIsRoot)
 *      - TEST(NtNode, InsertSiblingBeforePosEqualsSibling)
 *      - TEST(NtNode, InsertSiblingBeforeSiblingInTree)
 * 
 * @ref ecu_ntnode_is_leaf()
 *      - TEST(NtNode, IsLeafNodeIsNonEmptyRoot)
 *      - TEST(NtNode, IsLeafNodeIsEmptyRoot)
 *      - TEST(NtNode, IsLeafNodeIsNonemptySubroot)
 *      - TEST(NtNode, IsLeafNodeIsLeaf)
 *      - TEST(NtNode, IsLeafAddAndRemoveNodes)
 * 
 * @ref ecu_ntnode_is_root()
 *      - TEST(NtNode, IsRootNodeIsNonEmptyRoot)
 *      - TEST(NtNode, IsRootNodeIsEmptyRoot)
 *      - TEST(NtNode, IsRootNodeIsSubroot)
 *      - TEST(NtNode, IsRootNodeIsLeaf)
 *      - TEST(NtNode, IsRootAddAndRemoveNode)
 * 
 * @ref ecu_ntnode_last_child(), @ref ecu_ntnode_last_cchild()
 *      - TEST(NtNode, LastChildNodeWithMultipleChildren)
 *      - TEST(NtNode, LastChildNodeWithOneChild)
 *      - TEST(NtNode, LastChildNodeWithNoChildren)
 *      
 * @ref ecu_ntnode_lca(), @ref ecu_ntnode_clca()
 *      - TEST(NtNode, LCANode1LowerLevelThanNode2)
 *      - TEST(NtNode, LCANode2LowerLevelThanNode1)
 *      - TEST(NtNode, LCANode1AndNode2SameLevel)
 *      - TEST(NtNode, LCANode1EqualsNode2AndNotRoot)
 *      - TEST(NtNode, LCANode1AndNode2EqualRoot)
 *      - TEST(NtNode, LCANodesInDifferentTrees)
 *      - TEST(NtNode, LCANode1DirectParentOfNode2)
 *      - TEST(NtNode, LCANode2DirectParentOfNode1)
 * 
 * @ref ecu_ntnode_level()
 *      - TEST(NtNode, LevelNodeInTree)
 *      - TEST(NtNode, LevelNodeIsNonEmptyRoot)
 *      - TEST(NtNode, LevelNodeNodeIsEmptyRoot)
 *      - TEST(NtNode, LevelAddAndRemoveNodeFromTree)
 *      - TEST(NtNode, LevelAddAndRemoveLevels)
 * 
 * @ref ecu_ntnode_next(), @ref ecu_ntnode_cnext()
 *      - TEST(NtNode, NextNodeIsFirstSibling)
 *      - TEST(NtNode, NextNodeIsMiddleSibling)
 *      - TEST(NtNode, NextNodeIsLastSibling)
 *      - TEST(NtNode, NextNodeIsRoot)
 *      - TEST(NtNode, NextNodeWithNoSiblings)
 * 
 * @ref ecu_ntnode_parent(), @ref ecu_ntnode_cparent()
 *      - TEST(NtNode, ParentNodeIsSubroot)
 *      - TEST(NtNode, ParentNodeIsNonEmptyRoot)
 *      - TEST(NtNode, ParentNodeIsEmptyRoot)
 * 
 * @ref ecu_ntnode_prev(), @ref ecu_ntnode_cprev()
 *      - TEST(NtNode, PrevNodeIsFirstSibling)
 *      - TEST(NtNode, PrevNodeIsMiddleSibling)
 *      - TEST(NtNode, PrevNodeIsLastSibling)
 *      - TEST(NtNode, PrevNodeIsRoot)
 *      - TEST(NtNode, PrevNodeWithNoSiblings)
 * 
 * @ref ecu_ntnode_push_child_back()
 *      - TEST(NtNode, PushChildBackParentWithMultipleChildren)
 *      - TEST(NtNode, PushChildBackParentWithOneChild)
 *      - TEST(NtNode, PushChildBackParentWithNoChildren)
 *      - TEST(NtNode, PushChildBackParentEqualsChild)
 *      - TEST(NtNode, PushChildBackChildInTree)
 * 
 * @ref ecu_ntnode_push_child_front()
 *      - TEST(NtNode, PushChildFrontParentWithMultipleChildren)
 *      - TEST(NtNode, PushChildFrontParentWithOneChild)
 *      - TEST(NtNode, PushChildFrontParentWithNoChildren)
 *      - TEST(NtNode, PushChildFrontParentEqualsChild)
 *      - TEST(NtNode, PushChildFrontChildInTree)
 * 
 * @ref ecu_ntnode_remove()
 *      - TEST(NtNode, RemoveNodeIsLeftMostSubroot)
 *      - TEST(NtNode, RemoveNodeIsMiddleSubroot)
 *      - TEST(NtNode, RemoveNodeIsRightMostSubroot)
 *      - TEST(NtNode, RemoveNodeIsLeaf)
 *      - TEST(NtNode, RemoveNodeIsNonEmptyRoot)
 *      - TEST(NtNode, RemoveNodeIsEmptyRoot)
 * 
 * @ref ecu_ntnode_size()
 *      - TEST(NtNode, SizeNodeIsNonEmptyRoot)
 *      - TEST(NtNode, SizeNodeIsNonEmptySubroot)
 *      - TEST(NtNode, SizeNodeIsLeaf)
 *      - TEST(NtNode, SizeNodeIsEmptyRoot)
 *      - TEST(NtNode, SizeAddAndRemoveNodes)
 * 
 * @ref ecu_ntnode_valid()
 *      - TEST(NtNode, Valid)
 * 
 * Iterators:
 * 
 * @ref ECU_NTNODE_CHILD_FOR_EACH(), @ref ECU_NTNODE_CONST_CHILD_FOR_EACH(),
 * @ref ecu_ntnode_child_iterator_begin(), @ref ecu_ntnode_child_iterator_end(),
 * @ref ecu_ntnode_child_iterator_next(), @ref ecu_ntnode_child_iterator_cbegin(),
 * @ref ecu_ntnode_child_iterator_cend(), @ref ecu_ntnode_child_iterator_cnext()
 *      - TEST(NtNode, ChildIterator)
 *      - TEST(NtNode, ConstChildIterator)
 *      - TEST(NtNode, ChildIteratorParentHasOneChild)
 *      - TEST(NtNode, ConstChildIteratorParentHasOneChild)
 *      - TEST(NtNode, ChildIteratorParentHasNoChildren)
 *      - TEST(NtNode, ConstChildIteratorParentHasNoChildren)
 *      - TEST(NtNode, ChildIteratorRemoveSome)
 *      - TEST(NtNode, ChildIteratorRemoveAll)
 *      - TEST(NtNode, ChildIteratorDestroySome)
 *      - TEST(NtNode, ChildIteratorDestroyAll)
 *      - TEST(NtNode, ChildIteratorNextAfterDone)
 *      - TEST(NtNode, ConstChildIteratorNextAfterDone)
 *      - TEST(NtNode, ChildIteratorMultipleTimes)
 *      - TEST(NtNode, ConstChildIteratorMultipleTimes)
 * 
 * @ref ECU_NTNODE_PARENT_AT_FOR_EACH(), @ref ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(),
 * @ref ecu_ntnode_parent_iterator_at(), @ref ecu_ntnode_parent_iterator_end(),
 * @ref ecu_ntnode_parent_iterator_next(), @ref ecu_ntnode_parent_iterator_cat(),
 * @ref ecu_ntnode_parent_iterator_cend(), @ref ecu_ntnode_parent_iterator_cnext()
 *      - TEST(NtNode, ParentAtIteratorStartIsLeaf)
 *      - TEST(NtNode, ConstParentAtIteratorStartIsLeaf)
 *      - TEST(NtNode, ParentAtIteratorStartIsMiddleChild)
 *      - TEST(NtNode, ConstParentAtIteratorStartIsMiddleChild)
 *      - TEST(NtNode, ParentAtIteratorStartsParentIsRoot)
 *      - TEST(NtNode, ConstParentAtIteratorStartsParentIsRoot)
 *      - TEST(NtNode, ParentAtIteratorStartIsRoot)
 *      - TEST(NtNode, ConstParentAtIteratorStartIsRoot)
 *      - TEST(NtNode, ParentAtIteratorRemoveSome)
 *      - TEST(NtNode, ParentAtIteratorRemoveAll)
 *      - TEST(NtNode, ParentAtIteratorDestroySome)
 *      - TEST(NtNode, ParentAtIteratorDestroyAll)
 *      - TEST(NtNode, ParentAtIteratorNextAfterDone)
 *      - TEST(NtNode, ConstParentAtIteratorNextAfterDone)
 *      - TEST(NtNode, ParentAtIteratorMultipleTimes)
 *      - TEST(NtNode, ConstParentAtIteratorMultipleTimes)
 * 
 * @ref ECU_NTNODE_PARENT_FOR_EACH(), @ref ECU_NTNODE_CONST_PARENT_FOR_EACH(),
 * @ref ecu_ntnode_parent_iterator_begin(), @ref ecu_ntnode_parent_iterator_end(),
 * @ref ecu_ntnode_parent_iterator_next(), @ref ecu_ntnode_parent_iterator_cbegin(),
 * @ref ecu_ntnode_parent_iterator_cend(), @ref ecu_ntnode_parent_iterator_cnext()
 *      - TEST(NtNode, ParentIteratorStartIsLeaf)
 *      - TEST(NtNode, ConstParentIteratorStartIsLeaf)
 *      - TEST(NtNode, ParentIteratorStartIsMiddleChild)
 *      - TEST(NtNode, ConstParentIteratorStartIsMiddleChild)
 *      - TEST(NtNode, ParentIteratorStartsParentIsRoot)
 *      - TEST(NtNode, ConstParentIteratorStartsParentIsRoot)
 *      - TEST(NtNode, ParentIteratorStartIsRoot)
 *      - TEST(NtNode, ConstParentIteratorStartIsRoot)
 *      - TEST(NtNode, ParentIteratorRemoveAll)
 *      - TEST(NtNode, ParentIteratorDestroySome)
 *      - TEST(NtNode, ParentIteratorDestroyAll)
 *      - TEST(NtNode, ParentIteratorNextAfterDone)
 *      - TEST(NtNode, ConstParentIteratorNextAfterDone)
 *      - TEST(NtNode, ParentIteratorMultipleTimes)
 *      - TEST(NtNode, ConstParentIteratorMultipleTimes)
 * 
 * @ref ECU_NTNODE_POSTORDER_FOR_EACH(), @ref ECU_NTNODE_CONST_POSTORDER_FOR_EACH(),
 * @ref ecu_ntnode_postorder_iterator_begin(), @ref ecu_ntnode_postorder_iterator_end(),
 * @ref ecu_ntnode_postorder_iterator_next(), @ref ecu_ntnode_postorder_iterator_cbegin(),
 * @ref ecu_ntnode_postorder_iterator_cend(), @ref ecu_ntnode_postorder_iterator_cnext()
 *      - TEST(NtNode, PostorderIterator)
 *      - TEST(NtNode, ConstPostorderIterator)
 *      - TEST(NtNode, PostorderIteratorStartIsLeftMostSubtree)
 *      - TEST(NtNode, ConstPostorderIteratorStartIsLeftMostSubtree)
 *      - TEST(NtNode, PostorderIteratorStartIsMiddleSubtree)
 *      - TEST(NtNode, ConstPostorderIteratorStartIsMiddleSubtree)
 *      - TEST(NtNode, PostorderIteratorStartIsRightMostSubtree)
 *      - TEST(NtNode, ConstPostorderIteratorStartIsRightMostSubtree)
 *      - TEST(NtNode, PostorderIteratorStartIsLeaf)
 *      - TEST(NtNode, ConstPostorderIteratorStartIsLeaf)
 *      - TEST(NtNode, PostorderIteratorStartIsEmptyRoot)
 *      - TEST(NtNode, ConstPostorderIteratorStartIsEmptyRoot)
 *      - TEST(NtNode, PostorderIteratorStartIsMiddleChildInDegenerateTree)
 *      - TEST(NtNode, ConstPostorderIteratorStartIsMiddleChildInDegenerateTree)
 *      - TEST(NtNode, PostorderIteratorStartIsRootOfDegenerateTree)
 *      - TEST(NtNode, ConstPostorderIteratorStartIsRootOfDegenerateTree)
 *      - TEST(NtNode, PostorderIteratorRemoveSomeStartIsRoot)
 *      - TEST(NtNode, PostorderIteratorRemoveSomeStartIsSubroot)
 *      - TEST(NtNode, PostorderIteratorRemoveAllStartIsRoot)
 *      - TEST(NtNode, PostorderIteratorRemoveAllStartIsSubroot)
 *      - TEST(NtNode, PostorderIteratorDestroySomeStartIsRoot)
 *      - TEST(NtNode, PostorderIteratorDestroySomeStartIsSubroot)
 *      - TEST(NtNode, PostorderIteratorDestroyAllStartIsRoot)
 *      - TEST(NtNode, PostorderIteratorDestroyAllStartIsSubroot)
 *      - TEST(NtNode, PostorderIteratorNextAfterDone)
 *      - TEST(NtNode, ConstPostorderIteratorNextAfterDone)
 *      - TEST(NtNode, PostorderIteratorMultipleTimes)
 *      - TEST(NtNode, ConstPostorderIteratorMultipleTimes)
 * 
 * !!! TODO Preorder iterator !!!!
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
 * !!!! TODO Sibling AT FOR EACH!!!
 * !!!! TODO Add destroy tests to sibling iterator !!!!!
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

#warning "TODO: Prevent child, parent, sibling, and preorder iterator next() from being called after the iteration is done" 

using namespace stubs;

namespace {
/*------------------------------------------------------------*/
/*--------------- FILE-SCOPE FUNCTION DECLARATIONS -----------*/
/*------------------------------------------------------------*/

/* Forward declarations. */
struct ntnode;
struct rw_ntnode;
struct ro_ntnode;
struct destroy_ntnode;

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

    /// @brief Any operation applied to this node type.
    virtual void visit(rw_ntnode& n) = 0;

    /// @brief Any operation applied to this node type.
    virtual void visit(const rw_ntnode& n) = 0;
    
    /// @brief Any operation applied to this node type.
    virtual void visit(ro_ntnode& n) = 0;

    /// @brief Any operation applied to this node type.
    virtual void visit(const ro_ntnode& n) = 0;

    /// @brief Any operation applied to this node type.
    virtual void visit(destroy_ntnode& n) = 0;

    /// @brief Any operation applied to this node type.
    virtual void visit(const destroy_ntnode& n) = 0;

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

    /// @brief Default destroy callback used. Calls mock when
    /// node is destroyed.
    static void destroy(struct ecu_ntnode *me, ecu_object_id id)
    {
        assert( (me) );
        (void)id;
        mock("node_destroy").actualCall("callback")
                            .withParameter("value", static_cast<const ecu_ntnode *>(me));
    }
};

/**
 * @brief Read-write node.
 */
struct rw_ntnode : public ntnode
{
    /// @brief Default constructor. Default destroy callback that
    /// calls mock is used. ID unused.
    rw_ntnode() 
        : ntnode(&destroy, ECU_OBJECT_ID_UNUSED)
    {
        
    }

    /// @brief Constructor assigns supplied ID to node. Default 
    /// destroy callback that calls mock is used.
    rw_ntnode(ecu_object_id id_)
        : ntnode(&destroy, std::move(id_))
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
    /// @brief Default constructor. Default destroy callback that
    /// calls mock is used. ID unused.
    ro_ntnode() 
        : ntnode(&destroy, ECU_OBJECT_ID_UNUSED)
    {
        
    }

    /// @brief Constructor assigns supplied ID to node. Default 
    /// destroy callback that calls mock is used.
    ro_ntnode(ecu_object_id id_)
        : ntnode(&destroy, std::move(id_))
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

/**
 * @brief Node type that accepts destroy visitor. Note that
 * other nodes in this node's subtree are also destroyed.
 */
struct destroy_ntnode : public ntnode
{
    /// @brief Default constructor. Default destroy callback that
    /// calls mock is used. ID unused.
    destroy_ntnode() 
        : ntnode(&destroy, ECU_OBJECT_ID_UNUSED)
    {
        
    }

    /// @brief Constructor assigns supplied ID to node. Default 
    /// destroy callback that calls mock is used.
    destroy_ntnode(ecu_object_id id_)
        : ntnode(&destroy, std::move(id_))
    {

    }

    /// @brief Constructor assigns supplied destroy callback and 
    /// optional ID to ecu_ntnode. Function takes in the C-structure
    /// under test (ecu_ntnode *) to be compatible with the C API and
    /// to avoid having to explicitly upcast. Do not convert into 
    /// the C++ base wrapper class (ntnode *), as ecu_ntnode_destroy() will
    /// have already been called on the node, thus invalidating it.
    destroy_ntnode(void (*destroy_)(ecu_ntnode *, ecu_object_id), ecu_object_id id_ = ECU_OBJECT_ID_UNUSED)
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
        mock("node_in_tree").actualCall("in_tree")
                            .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n));
    }

    /// @brief Node in tree mock expectation called on any node.
    void visit(rw_ntnode& n) override
    {
        call(n);
    }

    /// @brief Node in tree mock expectation called on any node.
    void visit(const rw_ntnode& n) override
    {
        call(n);
    }

    /// @brief Node in tree mock expectation called on any node.
    void visit(ro_ntnode& n) override
    {
        call(n);
    }

    /// @brief Node in tree mock expectation called on any node.
    void visit(const ro_ntnode& n) override
    {
        call(n);
    }

    /// @brief Node in tree mock expectation called on any node.
    void visit(destroy_ntnode& n) override
    {
        call(n);
    }

    /// @brief Node in tree mock expectation called on any node.
    void visit(const destroy_ntnode& n) override
    {
        call(n);
    }
};

/**
 * @brief Concrete visitor that removes node. Only applies
 * to editable read-write nodes.
 */
struct node_remove : public visitor
{
    /// @brief Remove operation applied to editable read-write nodes.
    void visit(rw_ntnode& n) override
    {
        ecu_ntnode_remove(&n);
    }

    /// @brief Remove operation does not apply to this node type.
    void visit(const rw_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Remove operation does not apply to this node type.
    void visit(ro_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Remove operation does not apply to this node type.
    void visit(const ro_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Remove operation does not apply to this node type.
    void visit(destroy_ntnode &n) override
    {
        (void)n;
    }

    /// @brief Remove operation does not apply to this node type.
    void visit(const destroy_ntnode &n) override
    {
        (void)n;
    }
};

/**
 * @brief Concrete visitor that destroys node.
 */
struct node_destroy : public visitor
{
    /// @brief Destroy operation does not apply to this node type.
    void visit(rw_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Destroy operation does not apply to this node type.
    void visit(const rw_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Destroy operation does not apply to this node type.
    void visit(ro_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Destroy operation does not apply to this node type.
    void visit(const ro_ntnode& n) override
    {
        (void)n;
    }

    /// @brief Destroy operation applied to this node type.
    void visit(destroy_ntnode &n) override
    {
        ecu_ntnode_destroy(&n);
    }

    /// @brief Destroy operation does not apply to this node type.
    void visit(const destroy_ntnode &n) override
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
        mock("node_destroy").checkExpectations();
        mock("node_in_tree").checkExpectations();
        mock().clear();
    }

    /// @brief Creates node_destroyed mock expectations on supplied
    /// nodes. Order does not matter.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...) /* Nodes type will never be reference since parameter passed by reference. */
    static void EXPECT_NODES_DESTROYED(ntnode& n0, Nodes&... n)
    {
        /* Note how parameters are explicitly upcasted to (const ecu_ntnode *) to standardize
        pointers passed in. It is NOT guaranteed (const ecu_ntnode *) == (const ntnode *). 
        Do const-qualified cast to allow const and non-const parameters. */
        mock("node_destroy").expectOneCall("callback")
                            .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n0));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock("node_destroy").expectOneCall("callback")
                                 .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n)), ...);
        }
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
        mock("node_in_tree").strictOrder();
        mock("node_in_tree").expectOneCall("in_tree")
                            .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n0));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock("node_in_tree").expectOneCall("in_tree")
                                 .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n)), ...);
        }
    }

    /// @brief Same as @ref EXPECT_NODES_IN_TREE() however order does
    /// not matter.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...) /* Nodes type will never be reference since parameter passed by reference. */
    static void EXPECT_NODES_IN_TREE_ANY_ORDER(ntnode& n0, Nodes&... n)
    {
        /* Note how parameters are explicitly upcasted to (const ecu_ntnode *) to standardize
        pointers passed in. It is NOT guaranteed (const ecu_ntnode *) == (const ntnode *). 
        Do const-qualified cast to allow const and non-const parameters. */
        mock("node_in_tree").expectOneCall("in_tree")
                            .withParameter("value", static_cast<const void *>((const ecu_ntnode *)&n0));

        if constexpr(sizeof...(Nodes) > 0)
        {
            (mock("node_in_tree").expectOneCall("in_tree")
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

    /// @brief Overload of add_branch() template. Required in order
    /// to use recursive template.
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

    /// @brief Helper to create test tree. Adds any number of siblings 
    /// after specified position node.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static void add_siblings_after(ntnode& pos, ntnode& s0, Nodes&... s)
    {
        add_siblings_after(pos, s0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            add_siblings_after(s0, s...);
        }
    }

    /// @brief Overload of add_branch() template. Required in order
    /// to use recursive template.
    static void add_siblings_after(ntnode& pos, ntnode& sibling)
    {
        ecu_ntnode_insert_sibling_after(&pos, &sibling);
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

    /// @brief Returns true if all supplied nodes are empty.
    /// False otherwise. Function immediately stops evaluating
    /// when the first node that is not empty is processed.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static bool is_empty(const ntnode &n0, const Nodes&... n)
    {
        bool status = ecu_ntnode_empty(&n0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            status = status && ((ecu_ntnode_empty(&n)) && ...); /* Only keep evaluating if true return. */
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

    /// @brief Used to verify using ntnode API within destroy
    /// callback is prohibited.
    static void use_api_in_destroy_callback(ecu_ntnode *me, ecu_object_id id)
    {
        assert( (me) );
        (void)id;
        ecu_ntnode_remove(me);
    }

    node_obj_in_tree_actual_call node_obj_in_tree_visitor;
    node_remove node_remove_visitor;
    node_destroy node_destroy_visitor;
    std::array<ro_ntnode, 50> RO; /**< Read-only nodes. */
    std::array<rw_ntnode, 50> RW; /**< Read-write nodes. */
    std::array<destroy_ntnode, 50> DN; /**< Destroy nodes. */
};

/*------------------------------------------------------------*/
/*------------------ TESTS - ECU_NTNODE_DESTROY --------------*/
/*------------------------------------------------------------*/

/**
 * @brief All nodes in tree should be destroyed.
 */
TEST(NtNode, DestroyNodeIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1-----RW2---RW3
        |       |   
        RW4     RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6));
        EXPECT_NODES_DESTROYED(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4), RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_destroy(&RW.at(0));
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief All nodes in subtree should be destroyed.
 * Rest of tree unharmed.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, DestroyNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange. Destroy RW2.
        RW0
        |
        RW1-----RW2---RW3
        |       |   
        RW4     RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6));
        EXPECT_NODES_DESTROYED(RW.at(2), RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_destroy(&RW.at(2));

        /* Step 3: Assert. Verify remaining tree still intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(1), RW.at(3), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Only leaf node should be destroyed. Rest
 * of tree unharmed.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, DestroyNodeIsLeaf)
{
    try
    {
        /* Step 1: Arrange. Destroy RW2.
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_DESTROYED(RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_destroy(&RW.at(2));

        /* Step 3: Assert. Verify remaining tree still intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(3), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Single root node should be destroyed.
 */
TEST(NtNode, DestroyNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_TRUE( (is_root(RW.at(0))) ); /* Precondition. */
        EXPECT_NODES_DESTROYED(RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_destroy(&RW.at(0));
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Passing destroyed node to API in callback
 * is not allowed.
 */
TEST(NtNode, DestroyUseAPIInCallback)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode node{&use_api_in_destroy_callback};
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_destroy(&node);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - ECU_NTNODE_CLEAR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief All nodes should be removed from tree.
 */
TEST(NtNode, ClearNodeIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2---RW3
        |
        RW4
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(4));
        add_siblings_after(RW.at(2), RW.at(3));

        /* Step 2: Action. */
        ecu_ntnode_clear(&RW.at(0));

        /* Step 3: Assert. */
        CHECK_TRUE( (is_empty(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief All nodes in subtree should be removed.
 * Rest of tree unharmed.
 */
TEST(NtNode, ClearNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange. Clear RW2.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        |
        RW4---RW5
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_siblings_after(RW.at(4), RW.at(5));

        /* Step 2: Action. */
        ecu_ntnode_clear(&RW.at(2));

        /* Step 3: Assert. */
        CHECK_TRUE( (is_empty(RW.at(2), RW.at(3), RW.at(4), RW.at(5))) );
        CHECK_TRUE( (in_subtree(RW.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Only leaf node should be removed. Rest of
 * tree unharmed.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, ClearNodeIsLeaf)
{
    try
    {
        /* Step 1: Arrange. Clear RW2.
        RW0
        |
        RW1
        |
        RW2---RW3
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));
        add_siblings_after(RW.at(2), RW.at(3));

        /* Step 2: Action. */
        ecu_ntnode_clear(&RW.at(2));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(1), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief OK. Does nothing since tree already empty.
 */
TEST(NtNode, ClearNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_TRUE( (is_empty(RW.at(0))) ); /* Precondition. */

        /* Step 2: Action. */
        ecu_ntnode_clear(&RW.at(0));

        /* Step 3: Assert. */
        CHECK_TRUE( (is_empty(RW.at(0))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - ECU_NTNODE_COUNT ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Correct number of children returned.
 */
TEST(NtNode, CountNodeWithMultipleChildren)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---RW3
        |       |
        RW4     RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_count(&RW.at(0)) == 3) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief One is returned.
 */
TEST(NtNode, CountNodeWithOneChild)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---RW3
                |
                RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_count(&RW.at(2)) == 1) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief 0 is returned.
 */
TEST(NtNode, CountNodeWithNoChildren)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_count(&RW.at(1)) == 0) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Correct number of children returned.
 */
TEST(NtNode, CountAddAndRemoveChildren)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3---RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_count(&RW.at(1)) == 2) );
        ecu_ntnode_push_child_back(&RW.at(1), &RW.at(5));
        ecu_ntnode_push_child_front(&RW.at(1), &RW.at(6));
        CHECK_TRUE( (ecu_ntnode_count(&RW.at(1)) == 4) );
        ecu_ntnode_remove(&RW.at(3));
        CHECK_TRUE( (ecu_ntnode_count(&RW.at(1)) == 3) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------ TESTS - ECU_NTNODE_EMPTY ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not empty returned.
 */
TEST(NtNode, EmptyNodeIsRootWithOneChild)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_empty(&RW.at(0))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Is empty returned.
 */
TEST(NtNode, EmptyNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode node;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_empty(&node)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not empty returned since node in tree.
 */
TEST(NtNode, EmptyNodeIsSubrootWithOneChild)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_empty(&RW.at(2))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not empty returned since node in tree.
 */
TEST(NtNode, EmptyNodeIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_empty(&RW.at(3))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Correct status returned.
 */
TEST(NtNode, EmptyAddAndRemoveNodes)
{
    try
    {
        /* Step 1: Arrange. */
        CHECK_TRUE( (ecu_ntnode_empty(&RW.at(0))) ); /* Precondition. */

        /* Steps 2 and 3: Action and assert. */
        add_children(RW.at(0), RW.at(1));
        CHECK_FALSE( (ecu_ntnode_empty(&RW.at(0))) );
        ecu_ntnode_remove(&RW.at(1));
        CHECK_TRUE( (ecu_ntnode_empty(&RW.at(0))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------- TESTS - ECU_NTNODE_FIRST_CHILD --------------*/
/*------------------------------------------------------------*/

/**
 * @brief First child returned.
 */
TEST(NtNode, FirstChildNodeWithMultipleChildren)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3-----RW4-----RW5
                |       |       |
                RW6     RW7     RW8
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(4), RW.at(7));
        add_children(RW.at(5), RW.at(8));
        ecu_ntnode *first;
        const ecu_ntnode *cfirst;

        /* Steps 2 and 3: Action and assert. */
        first = &convert(ecu_ntnode_first_child(&RW.at(2)));
        cfirst = &convert(ecu_ntnode_first_child(&RW.at(2)));
        CHECK_TRUE( (first == &RW.at(3)) );
        CHECK_TRUE( (cfirst == &RW.at(3)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief First child returned.
 */
TEST(NtNode, FirstChildNodeWithOneChild)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3-----RW4-----RW5
                |       |       |
                RW6     RW7     RW8
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(4), RW.at(7));
        add_children(RW.at(5), RW.at(8));
        ecu_ntnode *first;
        const ecu_ntnode *cfirst;

        /* Steps 2 and 3: Action and assert. */
        first = &convert(ecu_ntnode_first_child(&RW.at(4)));
        cfirst = &convert(ecu_ntnode_first_child(&RW.at(4)));
        CHECK_TRUE( (first == &RW.at(7)) );
        CHECK_TRUE( (cfirst == &RW.at(7)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief NULL returned.
 */
TEST(NtNode, FirstChildNodeWithNoChildren)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3-----RW4-----RW5
                |       |       |
                RW6     RW7     RW8
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(4), RW.at(7));
        add_children(RW.at(5), RW.at(8));
        ecu_ntnode *first;
        const ecu_ntnode *cfirst;

        /* Steps 2 and 3: Action and assert. */
        first = ecu_ntnode_first_child(&RW.at(8));
        cfirst = ecu_ntnode_first_child(&RW.at(8));
        CHECK_TRUE( (first == nullptr) );
        CHECK_TRUE( (cfirst == nullptr) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------------- TESTS - ECU_NTNODE_ID ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Correct ID returned.
 */
TEST(NtNode, Id)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode n2{2};
        rw_ntnode n3{3};

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_id(&n2) == 2) );
        CHECK_TRUE( (ecu_ntnode_id(&n3) == 3) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------- TESTS - ECU_NTNODE_IN_SUBTREE --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not in subtree returned.
 */
TEST(NtNode, InSubtreeNodeIsNonEmptyRoot)
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
 * @brief Not in subtree returned.
 */
TEST(NtNode, InSubtreeNodeIsEmptyRoot)
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
 * @brief In subtree returned.
 */
TEST(NtNode, InSubtreeNodeIsEmptySubroot)
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
 * @brief In subtree returned.
 */
TEST(NtNode, InSubtreeNodeIsLeaf)
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
 * @brief Correct status returned.
 */
TEST(NtNode, InSubtreeAddAndRemoveNode)
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
/*--------- TESTS - ECU_NTNODE_INSERT_SIBLING_AFTER ----------*/
/*------------------------------------------------------------*/

# warning "!!!! Have sibling be root with children"

/**
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingAfterPosIsLeftMostSubroot)
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingAfterPosIsMiddleSubroot)
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingAfterPosIsRightMostSubroot)
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingAfterPosIsLeaf)
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
 * @brief Not allowed. Position cannot be root.
 */
TEST(NtNode, InsertSiblingAfterPosIsRoot)
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

/**
 * @brief Not allowed.
 */
TEST(NtNode, InsertSiblingAfterPosEqualsSibling)
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

/**
 * @brief Not allowed.
 */
TEST(NtNode, InsertSiblingAfterSiblingInTree)
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
/*--------- TESTS - ECU_NTNODE_INSERT_SIBLING_BEFORE ---------*/
/*------------------------------------------------------------*/

# warning "!!!! Have sibling be root with children"

/**
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingBeforePosIsLeftMostSubroot)
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingBeforePosIsMiddleSubroot)
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingBeforePosIsRightMostSubroot)
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingBeforePosIsLeaf)
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
 * @brief Not allowed. Position cannot be root.
 */
TEST(NtNode, InsertSiblingBeforePosIsRoot)
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

/**
 * @brief Not allowed.
 */
TEST(NtNode, InsertSiblingBeforePosEqualsSibling)
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

/**
 * @brief Not allowed.
 */
TEST(NtNode, InsertSiblingBeforeSiblingInTree)
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
/*----------------- TESTS - ECU_NTNODE_IS_LEAF ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not a leaf returned.
 */
TEST(NtNode, IsLeafNodeIsNonEmptyRoot)
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
 * @brief Is leaf returned.
 */
TEST(NtNode, IsLeafNodeIsEmptyRoot)
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
 * @brief Not a leaf returned.
 */
TEST(NtNode, IsLeafNodeIsNonemptySubroot)
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
 * @brief Is leaf returned.
 */
TEST(NtNode, IsLeafNodeIsLeaf)
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
 * @brief Correct statuses returned.
 */
TEST(NtNode, IsLeafAddAndRemoveNodes)
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
/*----------------- TESTS - ECU_NTNODE_IS_ROOT ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Is root returned.
 */
TEST(NtNode, IsRootNodeIsNonEmptyRoot)
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
 * @brief Is root returned.
 */
TEST(NtNode, IsRootNodeIsEmptyRoot)
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
 * @brief Not root returned.
 */
TEST(NtNode, IsRootNodeIsSubroot)
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
 * @brief Not root returned.
 */
TEST(NtNode, IsRootNodeIsLeaf)
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
 * @brief Correct statuses returned.
 */
TEST(NtNode, IsRootAddAndRemoveNode)
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
/*--------------- TESTS - ECU_NTNODE_LAST_CHILD --------------*/
/*------------------------------------------------------------*/

#warning "!!! Make nodes multiple levels (each children has children etc)"

/**
 * @brief Last child returned.
 */
TEST(NtNode, LastChildNodeWithMultipleChildren)
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
 * @brief Last child returned.
 */
TEST(NtNode, LastChildNodeWithOneChild)
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
 * @brief NULL returned.
 */
TEST(NtNode, LastChildNodeWithNoChildren)
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
/*------------------- TESTS - ECU_NTNODE_LCA -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Correct LCA returned.
 */
TEST(NtNode, LCANode1LowerLevelThanNode2)
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
 * @brief Correct LCA returned.
 */
TEST(NtNode, LCANode2LowerLevelThanNode1)
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
 * @brief Correct LCA returned.
 */
TEST(NtNode, LCANode1AndNode2SameLevel)
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
 * @brief Node1/2 returned since they are already LCAs.
 */
TEST(NtNode, LCANode1EqualsNode2AndNotRoot)
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
 * @brief Node1/2 returned since they are already LCAs.
 */
TEST(NtNode, LCANode1AndNode2EqualRoot)
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
 * @brief NULL returned.
 */
TEST(NtNode, LCANodesInDifferentTrees)
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
 * @brief Node1 returned.
 */
TEST(NtNode, LCANode1DirectParentOfNode2)
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
 * @brief Node2 returned.
 */
TEST(NtNode, LCANode2DirectParentOfNode1)
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
/*------------------ TESTS - ECU_NTNODE_LEVEL ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Correct level returned.
 */
TEST(NtNode, LevelNodeInTree)
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
 * @brief 0 returned.
 */
TEST(NtNode, LevelNodeIsNonEmptyRoot)
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
 * @brief 0 returned.
 */
TEST(NtNode, LevelNodeNodeIsEmptyRoot)
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
 * @brief Correct level values returned.
 */
TEST(NtNode, LevelAddAndRemoveNodeFromTree)
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
 * @brief Correct level values returned.
 */
TEST(NtNode, LevelAddAndRemoveLevels)
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
/*------------------- TESTS - ECU_NTNODE_NEXT ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Next sibling returned.
 */
TEST(NtNode, NextNodeIsFirstSibling)
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
 * @brief Next sibling returned.
 */
TEST(NtNode, NextNodeIsMiddleSibling)
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
 * @brief NULL returned.
 */
TEST(NtNode, NextNodeIsLastSibling)
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
 * @brief NULL returned.
 */
TEST(NtNode, NextNodeIsRoot)
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
 * @brief NULL returned.
 */
TEST(NtNode, NextNodeWithNoSiblings)
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
/*------------------ TESTS - ECU_NTNODE_PARENT ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Parent returned.
 */
TEST(NtNode, ParentNodeIsSubroot)
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
 * @brief NULL returned.
 */
TEST(NtNode, ParentNodeIsNonEmptyRoot)
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
 * @brief NULL returned.
 */
TEST(NtNode, ParentNodeIsEmptyRoot)
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
/*------------------- TESTS - ECU_NTNODE_PREV ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief NULL returned.
 */
TEST(NtNode, PrevNodeIsFirstSibling)
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
 * @brief Previous sibling returned.
 */
TEST(NtNode, PrevNodeIsMiddleSibling)
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
 * @brief Previous sibling returned.
 */
TEST(NtNode, PrevNodeIsLastSibling)
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
 * @brief NULL returned.
 */
TEST(NtNode, PrevNodeIsRoot)
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
 * @brief NULL returned.
 */
TEST(NtNode, PrevNodeWithNoSiblings)
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
/*------------ TESTS - ECU_NTNODE_PUSH_CHILD_BACK ------------*/
/*------------------------------------------------------------*/

#warning "!!! Have child be root with children!!"

/**
 * @brief Child added to back.
 */
TEST(NtNode, PushChildBackParentWithMultipleChildren)
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
 * @brief Child added to back.
 */
TEST(NtNode, PushChildBackParentWithOneChild)
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
 * @brief Child added.
 */
TEST(NtNode, PushChildBackParentWithNoChildren)
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
 * @brief Not allowed.
 */
TEST(NtNode, PushChildBackParentEqualsChild)
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

/**
 * @brief Not allowed.
 */
TEST(NtNode, PushChildBackChildInTree)
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
/*------------ TESTS - ECU_NTNODE_PUSH_CHILD_FRONT -----------*/
/*------------------------------------------------------------*/

#warning "!!! Have child be root with children!!"

/**
 * @brief Child added to front.
 */
TEST(NtNode, PushChildFrontParentWithMultipleChildren)
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
 * @brief Child added to front.
 */
TEST(NtNode, PushChildFrontParentWithOneChild)
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
 * @brief Child added.
 */
TEST(NtNode, PushChildFrontParentWithNoChildren)
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
 * @brief Not allowed.
 */
TEST(NtNode, PushChildFrontParentEqualsChild)
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

/**
 * @brief Not allowed.
 */
TEST(NtNode, PushChildFrontChildInTree)
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
/*----------------- TESTS - ECU_NTNODE_REMOVE ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Node removed correctly. Node's tree and
 * tree it was removed from unharmed.
 */
TEST(NtNode, RemoveNodeIsLeftMostSubroot)
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
 * @brief Node removed correctly. Node's tree and
 * tree it was removed from unharmed.
 */
TEST(NtNode, RemoveNodeIsMiddleSubroot)
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
 * @brief Node removed correctly. Node's tree and
 * tree it was removed from unharmed.
 */
TEST(NtNode, RemoveNodeIsRightMostSubroot)
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
 * @brief Node removed correctly. Tree node was
 * removed from unharmed.
 */
TEST(NtNode, RemoveNodeIsLeaf)
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
 * @brief OK. Does nothing.
 */
TEST(NtNode, RemoveNodeIsNonEmptyRoot)
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
 * @brief OK. Does nothing.
 */
TEST(NtNode, RemoveNodeIsEmptyRoot)
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
/*------------------- TESTS - ECU_NTNODE_SIZE ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Correct size returned.
 */
TEST(NtNode, SizeNodeIsNonEmptyRoot)
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
 * @brief Correct size returned.
 */
TEST(NtNode, SizeNodeIsNonEmptySubroot)
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
 * @brief 0 returned.
 */
TEST(NtNode, SizeNodeIsLeaf)
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
 * @brief 0 returned.
 */
TEST(NtNode, SizeNodeIsEmptyRoot)
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
 * @brief Correct size returned.
 */
TEST(NtNode, SizeAddAndRemoveNodes)
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
/*------------------ TESTS - ECU_NTNODE_VALID ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Correct status returned.
 */
TEST(NtNode, Valid)
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
/*------------------- TESTS - CHILD ITERATOR -----------------*/
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
TEST(NtNode, ChildIteratorParentHasOneChild)
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
TEST(NtNode, ConstChildIteratorParentHasOneChild)
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
TEST(NtNode, ChildIteratorParentHasNoChildren)
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
TEST(NtNode, ConstChildIteratorParentHasNoChildren)
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
 * @warning Test verification requires working postorder iterator.
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

        /* Step 3: Assert. */
        CHECK_TRUE( (is_root(RW.at(1), RW.at(2))) );

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RO.at(0), RO.at(1), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
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
 * @brief Remove all nodes returned by child iteration.
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ChildIteratorRemoveAll)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW2.
        Remove all children.

        Before:
        RW0
        |
        RW1-----RW2---RW3---RW4
                |
                RW5---RW6---RW7
                                        
        After:
        RW0                         RW5  RW6  RW7
        |
        RW1-----RW2---RW3---RW4
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7));

        /* Step 2: Action. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. */
        CHECK_TRUE( (is_root(RW.at(5), RW.at(6), RW.at(7))) );

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(3), RW.at(4), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
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
 * @brief Destroy some nodes in the middle of a child iteration.
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ChildIteratorDestroySome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        Destroy DN nodes.

        Before:
        RW0
        |
        RW1---DN0---RW2---DN1
                                        
        After:
        RW0
        |
        RW1---RW2
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), DN.at(0), RW.at(2), DN.at(1));
        EXPECT_NODES_DESTROYED(DN.at(0), DN.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
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
 * @brief Destroy all nodes returned by child iteration.
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ChildIteratorDestroyAll)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW2.
        Destroy DN nodes.

        Before:
        RW0
        |
        RW1-----RW2---RW3
                |
                DN0---DN1---DN2
                                        
        After:
        RW0
        |
        RW1-----RW2----RW3
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), DN.at(0), DN.at(1), DN.at(2));
        EXPECT_NODES_DESTROYED(DN.at(0), DN.at(1), DN.at(2));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(3), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
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
 * @brief Calling next() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, ChildIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_child_iterator iter;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_child_iterator_next(&iter);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Calling cnext() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, ConstChildIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_child_citerator citer;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citer, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_child_iterator_cnext(&citer);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, ChildIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        ecu_ntnode_child_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(1), RW.at(2), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CHILD_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
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
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, ConstChildIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        ecu_ntnode_child_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(1), RW.at(2), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
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

/*------------------------------------------------------------*/
/*------------------ TESTS - PARENT AT ITERATOR --------------*/
/*------------------------------------------------------------*/

/**
 * @brief General iteration test where starting node
 * is a leaf.
 */
TEST(NtNode, ParentAtIteratorStartIsLeaf)
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
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(4), RW.at(3), RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(5))
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
TEST(NtNode, ConstParentAtIteratorStartIsLeaf)
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
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(4), RW.at(3), RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(5))
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
TEST(NtNode, ParentAtIteratorStartIsMiddleChild)
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
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(3))
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
TEST(NtNode, ConstParentAtIteratorStartIsMiddleChild)
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
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(3))
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
TEST(NtNode, ParentAtIteratorStartsParentIsRoot)
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
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(1))
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
TEST(NtNode, ConstParentAtIteratorStartsParentIsRoot)
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
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(1))
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
 * no parent. Iteration should return only root node.
 */
TEST(NtNode, ParentAtIteratorStartIsRoot)
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
        EXPECT_NODES_IN_TREE(RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(0))
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
 * no parent. Iteration should return only root node.
 */
TEST(NtNode, ConstParentAtIteratorStartIsRoot)
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
        EXPECT_NODES_IN_TREE(RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(0))
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
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ParentAtIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW3.
        Remove read-write nodes.

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
        RO0             RW3     RW2     RW1
        |                               |
        RW0-----RW4                     RO1
                |
                RW5 
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RO.at(0), RW.at(0), RW.at(1), RW.at(4));
        add_branch(RW.at(1), RO.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(4), RW.at(5));

        /* Step 2: Action. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(3))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(5), RW.at(4), RO.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RO.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        CHECK_TRUE( (is_root(RW.at(3), RW.at(2), RW.at(1))) );
        CHECK_TRUE( (in_subtree(RO.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes returned by parent iteration.
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ParentAtIteratorRemoveAll)
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
        RW0             RW5    RW4     RW3     RW2
        |               
        RW1-----RW6
                |
                RW7    
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(6));
        add_branch(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));
        
        /* Step 2: Action. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(5))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(7), RW.at(6), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        CHECK_TRUE( (is_root(RW.at(5), RW.at(4), RW.at(3), RW.at(2))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Destroy some nodes in the middle of a parent iteration.
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ParentAtIteratorDestroySome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at DN2.
        Destroy DN nodes.

        Before:
        RW0
        |
        DN0---RW1---RW2
        |
        RW3---RW4
        |
        DN1---RW5
        |
        DN2

        After:
        RW0
        |
        RW1---RW2
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), DN.at(0), RW.at(1), RW.at(2));
        add_children(DN.at(0), RW.at(3), RW.at(4));
        add_children(RW.at(3), DN.at(1), RW.at(5));
        add_children(DN.at(1), DN.at(2));
        EXPECT_NODES_DESTROYED(DN.at(2), DN.at(1), DN.at(0), RW.at(3), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &DN.at(2))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
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
 * @brief Destroy all nodes returned by parent iteration.
 */
TEST(NtNode, ParentAtIteratorDestroyAll)
{
    try
    {
        /* Step 1: Arrange. Start iteration at DN4.
        Destroy DN nodes.

        Before:
        DN0
        |
        DN1---RW0---RW1
        |
        DN2---RW2
        |
        DN3---RW3
        |
        DN4

        After:
        */
        ecu_ntnode_parent_iterator iter;
        add_children(DN.at(0), DN.at(1), RW.at(0), RW.at(1));
        add_children(DN.at(1), DN.at(2), RW.at(2));
        add_children(DN.at(2), DN.at(3), RW.at(3));
        add_children(DN.at(3), DN.at(4));
        EXPECT_NODES_DESTROYED(DN.at(4), DN.at(3), DN.at(2), RW.at(3), DN.at(1), RW.at(2), DN.at(0), RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &DN.at(4))
        {
            convert(n).accept(node_destroy_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Calling next() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, ParentAtIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_parent_iterator iter;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_parent_iterator_next(&iter);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Calling next() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, ConstParentAtIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_parent_citerator citer;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_parent_iterator_cnext(&citer);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, ParentAtIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_parent_iterator iter;
        CHECK_TRUE( (is_root(RW.at(0))) ); /* Precondition. */
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(0), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, ConstParentAtIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_parent_citerator citer;
        CHECK_TRUE( (is_root(RW.at(0))) ); /* Precondition. */
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(0), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_PARENT_AT_FOR_EACH(n, &citer, &RW.at(0))
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

/*------------------------------------------------------------*/
/*------------------- TESTS - PARENT ITERATOR ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief General iteration test where starting node
 * is a leaf.
 */
TEST(NtNode, ParentIteratorStartIsLeaf)
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
TEST(NtNode, ConstParentIteratorStartIsLeaf)
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
TEST(NtNode, ParentIteratorStartIsMiddleChild)
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
TEST(NtNode, ConstParentIteratorStartIsMiddleChild)
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
TEST(NtNode, ParentIteratorStartsParentIsRoot)
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
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(1))
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
TEST(NtNode, ConstParentIteratorStartsParentIsRoot)
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
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(1))
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
TEST(NtNode, ParentIteratorStartIsRoot)
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
TEST(NtNode, ConstParentIteratorStartIsRoot)
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
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ParentIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW3.
        Remove read-write nodes.

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

        /* Step 3: Assert. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(5), RW.at(4), RO.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RO.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

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
 * @warning Test verification requires working postorder iterator.
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
                |
                RW7
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

        /* Step 3: Assert. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(7), RW.at(6), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        CHECK_TRUE( (is_root(RW.at(4), RW.at(3), RW.at(2))) );
        CHECK_TRUE( (in_subtree(RW.at(5))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Destroy some nodes in the middle of a parent iteration.
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ParentIteratorDestroySome)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW5.
        Destroy DN nodes.

        Before:
        RW0
        |
        RW1---RW2---RW3
        |
        DN0---RW4
        |
        RW5

        After:
        RW0
        |
        RW1---RW2---RW3
        |
        RW4
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), DN.at(0), RW.at(4));
        add_children(DN.at(0), RW.at(5));
        EXPECT_NODES_DESTROYED(DN.at(0), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(5))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_iterator postorder_iter;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(1), RW.at(2), RW.at(3), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &postorder_iter, &RW.at(0))
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
 * @brief Destroy all nodes returned by parent iteration.
 */
TEST(NtNode, ParentIteratorDestroyAll)
{
    try
    {
        /* Step 1: Arrange. Start iteration at DN4.
        Destroy DN nodes.

        Before:
        DN0
        |
        DN1---RW0---RW1
        |
        DN2---RW2
        |
        DN3---RW3
        |
        DN4

        After:
        */
        ecu_ntnode_parent_iterator iter;
        add_children(DN.at(0), DN.at(1), RW.at(0), RW.at(1));
        add_children(DN.at(1), DN.at(2), RW.at(2));
        add_children(DN.at(2), DN.at(3), RW.at(3));
        add_children(DN.at(3), DN.at(4));
        EXPECT_NODES_DESTROYED(DN.at(4), DN.at(3), DN.at(2), RW.at(3), DN.at(1), RW.at(2), DN.at(0), RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &DN.at(4))
        {
            convert(n).accept(node_destroy_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Calling next() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, ParentIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_parent_iterator iter;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_parent_iterator_next(&iter);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Calling cnext() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, ConstParentIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_parent_citerator citer;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_parent_iterator_cnext(&citer);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, ParentIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1
        */
        ecu_ntnode_parent_iterator iter;
        add_children(RW.at(0), RW.at(1));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(0), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_PARENT_FOR_EACH(n, &iter, &RW.at(1))
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
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, ConstParentIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1
        */
        ecu_ntnode_parent_citerator citer;
        add_children(RW.at(0), RW.at(1));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(0), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_PARENT_FOR_EACH(n, &citer, &RW.at(1))
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

/*------------------------------------------------------------*/
/*----------------- TESTS - POSTORDER ITERATOR ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief General iteration test. Start at root. 
 * Verify all nodes iterated over.
 */
TEST(NtNode, PostorderIterator)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1-------------------------RW2---------------------RW3
        |                           |                       |
        RW4---RW5---RW6---RW7       RW8---RW9---RW10        RW11
                    |               |                       |
                    RW12            RW13                    RW14
                    |                                       |
                    RW15                                    RW16
                                                            |
                                                            RW17
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5), RW.at(6), RW.at(7));
        add_children(RW.at(2), RW.at(8), RW.at(9), RW.at(10));
        add_branch(RW.at(3), RW.at(11), RW.at(14), RW.at(16), RW.at(17));
        add_branch(RW.at(6), RW.at(12), RW.at(15));
        add_children(RW.at(8), RW.at(13));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(15), RW.at(12), RW.at(6), 
                             RW.at(7), RW.at(1), RW.at(13), RW.at(8), RW.at(9), 
                             RW.at(10), RW.at(2), RW.at(17), RW.at(16), RW.at(14), 
                             RW.at(11), RW.at(3), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief General iteration test. Start at root.
 * Verify all nodes iterated over.
 */
TEST(NtNode, ConstPostorderIterator)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1-------------------------RW2---------------------RW3
        |                           |                       |
        RW4---RW5---RW6---RW7       RW8---RW9---RW10        RW11
                    |               |                       |
                    RW12            RW13                    RW14
                    |                                       |
                    RW15                                    RW16
                                                            |
                                                            RW17
        */
        ecu_ntnode_postorder_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5), RW.at(6), RW.at(7));
        add_children(RW.at(2), RW.at(8), RW.at(9), RW.at(10));
        add_branch(RW.at(3), RW.at(11), RW.at(14), RW.at(16), RW.at(17));
        add_branch(RW.at(6), RW.at(12), RW.at(15));
        add_children(RW.at(8), RW.at(13));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(15), RW.at(12), RW.at(6), 
                             RW.at(7), RW.at(1), RW.at(13), RW.at(8), RW.at(9), 
                             RW.at(10), RW.at(2), RW.at(17), RW.at(16), RW.at(14), 
                             RW.at(11), RW.at(3), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Start iteration at subroot node in a left-most subtree.
 */
TEST(NtNode, PostorderIteratorStartIsLeftMostSubtree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1---RW2---RW3
        |
        RW4
        |
        RW5---RW6
        |
        RW7
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_branch(RW.at(1), RW.at(4), RW.at(5), RW.at(7));
        add_siblings_after(RW.at(5), RW.at(6));
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(5), RW.at(6), RW.at(4), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(1))
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
 * @brief Start iteration at subroot node in a left-most subtree.
 */
TEST(NtNode, ConstPostorderIteratorStartIsLeftMostSubtree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1---RW2---RW3
        |
        RW4
        |
        RW5---RW6
        |
        RW7
        */
        ecu_ntnode_postorder_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_branch(RW.at(1), RW.at(4), RW.at(5), RW.at(7));
        add_siblings_after(RW.at(5), RW.at(6));
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(5), RW.at(6), RW.at(4), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(1))
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
 * @brief Start iteration at subroot node in a middle subtree.
 */
TEST(NtNode, PostorderIteratorStartIsMiddleSubtree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW3.
        RW0
        |
        RW1-----RW2-----RW3-----------------RW4
                |       |                   |
                RW5     RW6---RW7---RW8     RW9
                |                   |       |
                RW10                RW11    RW12
        
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_branch(RW.at(2), RW.at(5), RW.at(10));
        add_children(RW.at(3), RW.at(6), RW.at(7), RW.at(8));
        add_branch(RW.at(4), RW.at(9), RW.at(12));
        add_children(RW.at(8), RW.at(11));
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(7), RW.at(11), RW.at(8), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(3))
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
 * @brief Start iteration at subroot node in a middle subtree.
 */
TEST(NtNode, ConstPostorderIteratorStartIsMiddleSubtree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW3.
        RW0
        |
        RW1-----RW2-----RW3-----------------RW4
                |       |                   |
                RW5     RW6---RW7---RW8     RW9
                |                   |       |
                RW10                RW11    RW12
        
        */
        ecu_ntnode_postorder_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_branch(RW.at(2), RW.at(5), RW.at(10));
        add_children(RW.at(3), RW.at(6), RW.at(7), RW.at(8));
        add_branch(RW.at(4), RW.at(9), RW.at(12));
        add_children(RW.at(8), RW.at(11));
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(7), RW.at(11), RW.at(8), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(3))
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
 * @brief Start iteration at node in a right-most subtree.
 */
TEST(NtNode, PostorderIteratorStartIsRightMostSubtree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW4.
        RW0
        |
        RW1-----RW2
        |       |
        RW3     RW4
        |       |
        RW5     RW6---RW7---RW8
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_branch(RW.at(1), RW.at(3), RW.at(5));
        add_branch(RW.at(2), RW.at(4), RW.at(6));
        add_siblings_after(RW.at(6), RW.at(7), RW.at(8));
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(7), RW.at(8), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(4))
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
 * @brief Start iteration at node in a right-most subtree.
 */
TEST(NtNode, ConstPostorderIteratorStartIsRightMostSubtree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW4.
        RW0
        |
        RW1-----RW2
        |       |
        RW3     RW4
        |       |
        RW5     RW6---RW7---RW8
        */
        ecu_ntnode_postorder_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_branch(RW.at(1), RW.at(3), RW.at(5));
        add_branch(RW.at(2), RW.at(4), RW.at(6));
        add_siblings_after(RW.at(6), RW.at(7), RW.at(8));
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(7), RW.at(8), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(4))
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
 * @brief Start iteration at a leaf node. Only the leaf
 * should be returned in the iteration.
 */
TEST(NtNode, PostorderIteratorStartIsLeaf)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1-----RW2
                |
                RW3---RW4
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(1))
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
 * @brief Start iteration at a leaf node. Only the leaf
 * should be returned in the iteration.
 */
TEST(NtNode, ConstPostorderIteratorStartIsLeaf)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1-----RW2
                |
                RW3---RW4
        */
        ecu_ntnode_postorder_citerator citer;
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(1))
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
 * @brief Perform iteration on empty tree. Only root
 * node should be returned in the iteration.
 */
TEST(NtNode, PostorderIteratorStartIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        */
        ecu_ntnode_postorder_iterator iter;
        EXPECT_NODES_IN_TREE(RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief Perform iteration on empty tree. Only root
 * node should be returned in the iteration.
 */
TEST(NtNode, ConstPostorderIteratorStartIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Start iteration at middle node of degenerate tree.
 */
TEST(NtNode, PostorderIteratorStartIsMiddleChildInDegenerateTree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        */
        ecu_ntnode_postorder_iterator iter;
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(1))
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
 * @brief Start iteration at middle node of degenerate tree.
 */
TEST(NtNode, ConstPostorderIteratorStartIsMiddleChildInDegenerateTree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        */
        ecu_ntnode_postorder_citerator citer;
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(1))
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
 * @brief Start iteration at root node of degenerate tree.
 */
TEST(NtNode, PostorderIteratorStartIsRootOfDegenerateTree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        */
        ecu_ntnode_postorder_iterator iter;
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(1), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief Start iteration at root node of degenerate tree.
 */
TEST(NtNode, ConstPostorderIteratorStartIsRootOfDegenerateTree)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        */
        ecu_ntnode_postorder_citerator citer;
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(1), RW.at(0));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Start iteration at root. Remove some nodes.
 */
TEST(NtNode, PostorderIteratorRemoveSomeStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RO0.

        Before:
        RO0
        |
        RW0-----RO1-----------------RO2
                |                   |
                RW1---RO3           RO4
                |                   |
                RO5---RW2---RW3     RW4
                                    |
                                    RO6
        
        After:
        RO0             RW0     RW1     RW2     RW3     RW4
        |                       |                       |
        RO1-----RO2             RO5                     RO6
        |       |
        RO3     RO4
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RO.at(0), RW.at(0), RO.at(1), RO.at(2));
        add_children(RO.at(1), RW.at(1), RO.at(3));
        add_children(RW.at(1), RO.at(5), RW.at(2), RW.at(3));
        add_branch(RO.at(2), RO.at(4), RW.at(4), RO.at(6));

        /* Step 2: Action. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RO.at(0))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. */
        CHECK_TRUE( (is_root(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4))) );
        CHECK_TRUE( (in_subtree(RO.at(5), RO.at(6))) );

        /* Step 3: Assert. Verify tree intact. */
        ecu_ntnode_postorder_iterator iter2; /* Do not reuse old iterator. That functionality is tested elsewhere. */
        EXPECT_NODES_IN_TREE(RO.at(3), RO.at(1), RO.at(4), RO.at(2), RO.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter2, &RO.at(0))
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
 * @brief Start iteration at subroot. Remove some nodes.
 */
TEST(NtNode, PostorderIteratorRemoveSomeStartIsSubroot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW1.

        Before:
        RO0
        |
        RW0-----RO1-----------------RO2
                |                   |
                RW1---RO3           RO4
                |                   |
                RO5---RW2---RW3     RW4
                                    |
                                    RO6
        
        After:
        RO0                     RW1     RW2     RW3
        |                       |
        RW0-----RO1-----RO2     RO5
                |       |
                RO3     RO4
                        |
                        RW4
                        |
                        RO6
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RO.at(0), RW.at(0), RO.at(1), RO.at(2));
        add_children(RO.at(1), RW.at(1), RO.at(3));
        add_children(RW.at(1), RO.at(5), RW.at(2), RW.at(3));
        add_branch(RO.at(2), RO.at(4), RW.at(4), RO.at(6));

        /* Step 2: Action. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. */
        CHECK_TRUE( (is_root(RW.at(1), RW.at(2), RW.at(3))) );
        CHECK_TRUE( (in_subtree(RO.at(5))) );

        /* Step 3: Assert. Verify tree intact. */
        ecu_ntnode_postorder_iterator iter2; /* Do not reuse old iterator. That functionality is tested elsewhere. */
        EXPECT_NODES_IN_TREE(RW.at(0), RO.at(3), RO.at(1), RO.at(6), RW.at(4), RO.at(4), RO.at(2), RO.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter2, &RO.at(0))
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
 * @brief Start iteration at root. Remove all nodes.
 */
TEST(NtNode, PostorderIteratorRemoveAllStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        
        Before:
        RW0
        |
        RW1---RW2---RW3
        |           |
        RW4         RW5-----RW6---RW7
                            |
                            RW8
                            |
                            RW9-----RW10
                                    |
                                    RW11
        
        After:
        All nodes roots.
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(3), RW.at(5), RW.at(6), RW.at(7));
        add_children(RW.at(6), RW.at(8));
        add_children(RW.at(8), RW.at(9), RW.at(10));
        add_children(RW.at(10), RW.at(11));

        /* Step 2: Action. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. */
        CHECK_TRUE( (is_root(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4), RW.at(5),
                             RW.at(6), RW.at(7), RW.at(8), RW.at(9), RW.at(10), RW.at(11))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at subroot. Remove all nodes.
 */
TEST(NtNode, PostorderIteratorRemoveAllStartIsSubroot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW6.
        
        Before:
        RW0
        |
        RW1---RW2---RW3
        |           |
        RW4         RW5-----RW6---RW7
                            |
                            RW8
                            |
                            RW9-----RW10
                                    |
                                    RW11
        
        After:
        RW0                     RW6  RW8  RW9  RW10  RW11
        |
        RW1---RW2---RW3
        |           |
        RW4         RW5---RW7
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(3), RW.at(5), RW.at(6), RW.at(7));
        add_children(RW.at(6), RW.at(8));
        add_children(RW.at(8), RW.at(9), RW.at(10));
        add_children(RW.at(10), RW.at(11));

        /* Step 2: Action. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(6))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. */
        CHECK_TRUE( (is_root(RW.at(6), RW.at(8), RW.at(9), RW.at(10), RW.at(11))) );

        /* Step 3: Assert. Verify tree intact. */
        ecu_ntnode_postorder_iterator iter2; /* Do not reuse old iterator. That functionality is tested elsewhere. */
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(1), RW.at(2), RW.at(5), RW.at(7), RW.at(3), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter2, &RW.at(0))
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
 * @brief Start iteration at root. Destroy some nodes.
 */
TEST(NtNode, PostorderIteratorDestroySomeStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW0.
        
        Before:
        RW0
        |
        RW1---DN0---RW2
        |           |
        DN1         RW3-----RW4---RW5
                            |
                            DN2
                            |
                            RW6-----RW7
                                    |
                                    RW8
        
        After:
        RW0
        |
        RW1-----RW2
                |
                RW3---RW4---RW5
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), DN.at(0), RW.at(2));
        add_children(RW.at(1), DN.at(1));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(4), DN.at(2));
        add_children(DN.at(2), RW.at(6), RW.at(7));
        add_children(RW.at(7), RW.at(8));
        EXPECT_NODES_DESTROYED(DN.at(1), DN.at(0), DN.at(2), RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree still intact. */
        ecu_ntnode_postorder_iterator iter2; /* Do not reuse old iterator. That functionality is tested elsewhere. */
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(3), RW.at(4), RW.at(5), RW.at(2), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter2, &RW.at(0))
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
 * @brief Start iteration at subroot. Destroy some nodes.
 */
TEST(NtNode, PostorderIteratorDestroySomeStartIsSubroot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at RW2.
        
        Before:
        RW0
        |
        RW1---DN0---RW2
        |           |
        DN1         DN2-----RW3---DN3
                            |
                            DN4-----RW4
                            |
                            RW5-----RW6
                                    |
                                    RW7
        
        After:
        RW0
        |
        RW1---DN0---RW2
        |           |
        DN1         RW3
                    |
                    RW4
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(RW.at(0), RW.at(1), DN.at(0), RW.at(2));
        add_children(RW.at(1), DN.at(1));
        add_children(RW.at(2), DN.at(2), RW.at(3), DN.at(3));
        add_children(RW.at(3), DN.at(4), RW.at(4));
        add_children(DN.at(4), RW.at(5), RW.at(6));
        add_children(RW.at(6), RW.at(7));
        EXPECT_NODES_DESTROYED(DN.at(2), DN.at(3), DN.at(4), RW.at(5), RW.at(6), RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree still intact. */
        ecu_ntnode_postorder_iterator iter2; /* Do not reuse old iterator. That functionality is tested elsewhere. */
        EXPECT_NODES_IN_TREE(DN.at(1), RW.at(1), DN.at(0), RW.at(4), RW.at(3), RW.at(2), RW.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter2, &RW.at(0))
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
 * @brief Start iteration at root. Destroy all nodes.
 */
TEST(NtNode, PostorderIteratorDestroyAllStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at DN0.
        
        Before:
        DN0
        |
        DN1---DN2---DN3
        |           |
        DN4         DN5-----DN6---DN7
                            |
                            DN8
                            |
                            DN9-----DN10
                                    |
                                    DN11
        
        After:
        All nodes destroyed.
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(DN.at(0), DN.at(1), DN.at(2), DN.at(3));
        add_children(DN.at(1), DN.at(4));
        add_children(DN.at(3), DN.at(5), DN.at(6), DN.at(7));
        add_children(DN.at(6), DN.at(8));
        add_children(DN.at(8), DN.at(9), DN.at(10));
        add_children(DN.at(10), DN.at(11));
        EXPECT_NODES_DESTROYED(DN.at(0), DN.at(1), DN.at(2), DN.at(3), DN.at(4), DN.at(5), DN.at(6),
                               DN.at(7), DN.at(8), DN.at(9), DN.at(10), DN.at(11));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &DN.at(0))
        {
            convert(n).accept(node_destroy_visitor);
        }
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Start iteration at subroot. Destroy all nodes.
 */
TEST(NtNode, PostorderIteratorDestroyAllStartIsSubroot)
{
    try
    {
        /* Step 1: Arrange. Start iteration at DN6.
        
        Before:
        DN0
        |
        DN1---DN2---DN3
        |           |
        DN4         DN5-----DN6---DN7
                            |
                            DN8
                            |
                            DN9-----DN10
                                    |
                                    DN11
        
        After:
        DN0
        |
        DN1---DN2---DN3
        |           |
        DN4         DN5---DN7
        */
        ecu_ntnode_postorder_iterator iter;
        add_children(DN.at(0), DN.at(1), DN.at(2), DN.at(3));
        add_children(DN.at(1), DN.at(4));
        add_children(DN.at(3), DN.at(5), DN.at(6), DN.at(7));
        add_children(DN.at(6), DN.at(8));
        add_children(DN.at(8), DN.at(9), DN.at(10));
        add_children(DN.at(10), DN.at(11));
        EXPECT_NODES_DESTROYED(DN.at(6), DN.at(8), DN.at(9), DN.at(10), DN.at(11));

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &DN.at(6))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_iterator iter2; /* Do not reuse old iterator. That functionality is tested elsewhere. */
        EXPECT_NODES_IN_TREE(DN.at(4), DN.at(1), DN.at(2), DN.at(5), DN.at(7), DN.at(3), DN.at(0));
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter2, &DN.at(0))
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
 * @brief Calling next() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, PostorderIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_postorder_iterator iter;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_postorder_iterator_next(&iter);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Calling cnext() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, ConstPostorderIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_ASSERTION();

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            (void)n;
        }
        (void)ecu_ntnode_postorder_iterator_cnext(&citer);
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, PostorderIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_postorder_iterator iter;
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(0), RW.at(0));
        CHECK_TRUE( (is_root(RW.at(0))) ); /* Precondition. */
        
        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief Iteration can be conducted multiple times in a row.
 */
TEST(NtNode, ConstPostorderIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(0), RW.at(0));
        CHECK_TRUE( (is_root(RW.at(0))) ); /* Precondition. */
        
        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
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
