/**
 * @file
 * @brief Unit tests for public API functions in @ref ntnode.h. 
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
 * @ref ecu_ntnode_first_child(), @ref ecu_ntnode_first_cchild()
 *      - TEST(NtNode, FirstChildNodeWithMultipleChildren)
 *      - TEST(NtNode, FirstChildNodeWithOneChild)
 *      - TEST(NtNode, FirstChildNodeWithNoChildren)
 * 
 * @ref ecu_ntnode_id()
 *      - TEST(NtNode, Id)
 * 
 * @ref ecu_ntnode_in_tree()
 *      - TEST(NtNode, InTreeNodeIsEmptyRoot)
 *      - TEST(NtNode, InTreeNodeIsNonEmptyRoot)
 *      - TEST(NtNode, InTreeNodeIsNonEmptySubroot)
 *      - TEST(NtNode, InTreeNodeIsLeaf)
 * 
 * @ref ecu_ntnode_insert_sibling_after()
 *      - TEST(NtNode, InsertSiblingAfterPosIsLeftMostSubroot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsMiddleSubroot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsRightMostSubroot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsLeaf)
 *      - TEST(NtNode, InsertSiblingAfterSiblingIsEmptyRoot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsEmptyRoot)
 *      - TEST(NtNode, InsertSiblingAfterPosIsNonEmptyRoot)
 *      - TEST(NtNode, InsertSiblingAfterPosEqualsSibling)
 *      - TEST(NtNode, InsertSiblingAfterSiblingIsDescendant)
 * 
 * @ref ecu_ntnode_insert_sibling_before()
 *      - TEST(NtNode, InsertSiblingBeforePosIsLeftMostSubroot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsMiddleSubroot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsRightMostSubroot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsLeaf)
 *      - TEST(NtNode, InsertSiblingBeforeSiblingIsEmptyRoot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsEmptyRoot)
 *      - TEST(NtNode, InsertSiblingBeforePosIsNonEmptyRoot)
 *      - TEST(NtNode, InsertSiblingBeforePosEqualsSibling)
 *      - TEST(NtNode, InsertSiblingBeforeSiblingIsDescendant)
 * 
 * @ref ecu_ntnode_is_descendant()
 *      - TEST(NtNode, IsDescendantNodeIsEmptyRoot)
 *      - TEST(NtNode, IsDescendantNodeIsNonEmptyRoot)
 *      - TEST(NtNode, IsDescendantNodeIsNonEmptySubroot)
 *      - TEST(NtNode, IsDescendantNodeIsLeaf)
 * 
 * @ref ecu_ntnode_is_leaf()
 *      - TEST(NtNode, IsLeafNodeIsEmptyRoot)
 *      - TEST(NtNode, IsLeafNodeIsNonEmptyRoot)
 *      - TEST(NtNode, IsLeafNodeIsNonemptySubroot)
 *      - TEST(NtNode, IsLeafNodeIsLeaf)
 * 
 * @ref ecu_ntnode_is_root()
 *      - TEST(NtNode, IsRootNodeIsEmptyRoot)
 *      - TEST(NtNode, IsRootNodeIsNonEmptyRoot)
 *      - TEST(NtNode, IsRootNodeIsNonEmptySubroot)
 *      - TEST(NtNode, IsRootNodeIsLeaf)
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
 *      - TEST(NtNode, LevelNodeIsEmptyRoot)
 *      - TEST(NtNode, LevelNodeIsNonEmptyRoot)
 *      - TEST(NtNode, LevelNodeIsNonEmptySubroot)
 *      - TEST(NtNode, LevelNodeIsLeaf)
 *      - TEST(NtNode, LevelAddAndRemoveNodeFromTree)
 * 
 * @ref ecu_ntnode_next(), @ref ecu_ntnode_cnext()
 *      - TEST(NtNode, NextNodeIsEmptyRoot)
 *      - TEST(NtNode, NextNodeIsNonEmptyRoot)
 *      - TEST(NtNode, NextNodeIsFirstSibling)
 *      - TEST(NtNode, NextNodeIsMiddleSibling)
 *      - TEST(NtNode, NextNodeIsLastSibling)
 *      - TEST(NtNode, NextNodeWithNoSiblings)
 * 
 * @ref ecu_ntnode_parent(), @ref ecu_ntnode_cparent()
 *      - TEST(NtNode, ParentNodeIsEmptyRoot)
 *      - TEST(NtNode, ParentNodeIsNonEmptyRoot)
 *      - TEST(NtNode, ParentNodeIsNonEmptySubroot)
 *      - TEST(NtNode, ParentNodeIsLeaf)
 * 
 * @ref ecu_ntnode_prev(), @ref ecu_ntnode_cprev()
 *      - TEST(NtNode, PrevNodeIsEmptyRoot)
 *      - TEST(NtNode, PrevNodeIsNonEmptyRoot)
 *      - TEST(NtNode, PrevNodeIsFirstSibling)
 *      - TEST(NtNode, PrevNodeIsMiddleSibling)
 *      - TEST(NtNode, PrevNodeIsLastSibling)
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
 *      - TEST(NtNode, SizeNodeIsEmptyRoot)
 *      - TEST(NtNode, SizeNodeIsNonEmptyRoot)
 *      - TEST(NtNode, SizeNodeIsNonEmptySubroot)
 *      - TEST(NtNode, SizeNodeIsLeaf)
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
 * @ref ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(), @ref ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(),
 * @ref ecu_ntnode_next_sibling_iterator_at(), @ref ecu_ntnode_next_sibling_iterator_end(),
 * @ref ecu_ntnode_next_sibling_iterator_next(), @ref ecu_ntnode_next_sibling_iterator_cat(),
 * @ref ecu_ntnode_next_sibling_iterator_cend(), @ref ecu_ntnode_next_sibling_iterator_cnext()
 *      - TEST(NtNode, NextSiblingAtIteratorStartIsFirstSibling)
 *      - TEST(NtNode, ConstNextSiblingAtIteratorStartIsFirstSibling)
 *      - TEST(NtNode, NextSiblingAtIteratorStartIsMiddleSibling)
 *      - TEST(NtNode, ConstNextSiblingAtIteratorStartIsMiddleSibling)
 *      - TEST(NtNode, NextSiblingAtIteratorStartIsLastSibling)
 *      - TEST(NtNode, ConstNextSiblingAtIteratorStartIsLastSibling)
 *      - TEST(NtNode, NextSiblingAtIteratorStartIsRoot)
 *      - TEST(NtNode, ConstNextSiblingAtIteratorStartIsRoot)
 *      - TEST(NtNode, NextSiblingAtIteratorStartHasOneSibling)
 *      - TEST(NtNode, ConstNextSiblingAtIteratorStartHasOneSibling)
 *      - TEST(NtNode, NextSiblingAtIteratorRemoveSome)
 *      - TEST(NtNode, NextSiblingAtIteratorRemoveAll)
 *      - TEST(NtNode, NextSiblingAtIteratorStartIsRootRemoveRoot)
 *      - TEST(NtNode, NextSiblingAtIteratorDestroySome)
 *      - TEST(NtNode, NextSiblingAtIteratorDestroyAll)
 *      - TEST(NtNode, NextSiblingAtIteratorStartIsRootDestroyRoot)
 *      - TEST(NtNode, NextSiblingAtIteratorNextAfterDone)
 *      - TEST(NtNode, ConstNextSiblingAtIteratorNextAfterDone)
 *      - TEST(NtNode, NextSiblingAtIteratorMultipleTimes)
 *      - TEST(NtNode, ConstNextSiblingAtIteratorMultipleTimes)
 * 
 * @ref ECU_NTNODE_NEXT_SIBLING_FOR_EACH(), @ref ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(),
 * @ref ecu_ntnode_next_sibling_iterator_begin(), @ref ecu_ntnode_next_sibling_iterator_end(),
 * @ref ecu_ntnode_next_sibling_iterator_next(), @ref ecu_ntnode_next_sibling_iterator_cbegin(),
 * @ref ecu_ntnode_next_sibling_iterator_cend(), @ref ecu_ntnode_next_sibling_iterator_cnext()
 *      - TEST(NtNode, NextSiblingIteratorStartIsFirstSibling)
 *      - TEST(NtNode, ConstNextSiblingIteratorStartIsFirstSibling)
 *      - TEST(NtNode, NextSiblingIteratorStartIsMiddleSibling)
 *      - TEST(NtNode, ConstNextSiblingIteratorStartIsMiddleSibling)
 *      - TEST(NtNode, NextSiblingIteratorStartIsLastSibling)
 *      - TEST(NtNode, ConstNextSiblingIteratorStartIsLastSibling)
 *      - TEST(NtNode, NextSiblingIteratorStartIsRoot)
 *      - TEST(NtNode, ConstNextSiblingIteratorStartIsRoot)
 *      - TEST(NtNode, NextSiblingIteratorStartHasOneSibling)
 *      - TEST(NtNode, ConstNextSiblingIteratorStartHasOneSibling)
 *      - TEST(NtNode, NextSiblingIteratorRemoveSome)
 *      - TEST(NtNode, NextSiblingIteratorRemoveAll)
 *      - TEST(NtNode, NextSiblingIteratorDestroySome)
 *      - TEST(NtNode, NextSiblingIteratorDestroyAll)
 *      - TEST(NtNode, NextSiblingIteratorNextAfterDone)
 *      - TEST(NtNode, ConstNextSiblingIteratorNextAfterDone)
 *      - TEST(NtNode, NextSiblingIteratorMultipleTimes)
 *      - TEST(NtNode, ConstNextSiblingIteratorMultipleTimes)
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
 *      - TEST(NtNode, ParentAtIteratorStartIsRootRemoveRoot)
 *      - TEST(NtNode, ParentAtIteratorDestroySome)
 *      - TEST(NtNode, ParentAtIteratorDestroyAll)
 *      - TEST(NtNode, ParentAtIteratorStartIsRootDestroyRoot)
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
 * @ref ECU_NTNODE_PREORDER_FOR_EACH(), @ref ECU_NTNODE_CONST_PREORDER_FOR_EACH(),
 * @ref ecu_ntnode_preorder_iterator_begin(), @ref ecu_ntnode_preorder_iterator_end(),
 * @ref ecu_ntnode_preorder_iterator_next(), @ref ecu_ntnode_preorder_iterator_cbegin(),
 * @ref ecu_ntnode_preorder_iterator_cend(), @ref ecu_ntnode_preorder_iterator_cnext()
 *      - TEST(NtNode, PreorderIterator)
 *      - TEST(NtNode, ConstPreorderIterator)
 *      - TEST(NtNode, PreorderIterator2)
 *      - TEST(NtNode, ConstPreorderIterator2)
 *      - TEST(NtNode, PreorderIteratorStartIsLeftMostSubtree)
 *      - TEST(NtNode, ConstPreorderIteratorStartIsLeftMostSubtree)
 *      - TEST(NtNode, PreorderIteratorStartIsMiddleSubtree)
 *      - TEST(NtNode, ConstPreorderIteratorStartIsMiddleSubtree)
 *      - TEST(NtNode, PreorderIteratorStartIsRightMostSubtree)
 *      - TEST(NtNode, ConstPreorderIteratorStartIsRightMostSubtree)
 *      - TEST(NtNode, PreorderIteratorStartIsLeaf)
 *      - TEST(NtNode, ConstPreorderIteratorStartIsLeaf)
 *      - TEST(NtNode, PreorderIteratorStartIsEmptyRoot)
 *      - TEST(NtNode, ConstPreorderIteratorStartIsEmptyRoot)
 *      - TEST(NtNode, PreorderIteratorStartIsMiddleChildInDegenerateTree)
 *      - TEST(NtNode, ConstPreorderIteratorStartIsMiddleChildInDegenerateTree)
 *      - TEST(NtNode, PreorderIteratorStartIsRootOfDegenerateTree)
 *      - TEST(NtNode, ConstPreorderIteratorStartIsRootOfDegenerateTree)
 *      - TEST(NtNode, PreorderIteratorRemoveNodeInNonEmptyTree)
 *      - TEST(NtNode, PreorderIteratorDestroyNodeInNonEmptyTree)
 *      - TEST(NtNode, PreorderIteratorNextAfterDone)
 *      - TEST(NtNode, ConstPreorderIteratorNextAfterDone)
 *      - TEST(NtNode, PreorderIteratorMultipleTimes)
 *      - TEST(NtNode, ConstPreorderIteratorMultipleTimes)
 * 
 * @ref ECU_NTNODE_SIBLING_FOR_EACH(), @ref ECU_NTNODE_CONST_SIBLING_FOR_EACH(),
 * @ref ecu_ntnode_sibling_iterator_begin(), @ref ecu_ntnode_sibling_iterator_end(),
 * @ref ecu_ntnode_sibling_iterator_next(), @ref ecu_ntnode_sibling_iterator_cbegin(),
 * @ref ecu_ntnode_sibling_iterator_cend(), @ref ecu_ntnode_sibling_iterator_cnext()
 *      - TEST(NtNode, SiblingIteratorStartIsFirstSibling)
 *      - TEST(NtNode, ConstSiblingIteratorStartIsFirstSibling)
 *      - TEST(NtNode, SiblingIteratorStartIsMiddleSibling)
 *      - TEST(NtNode, ConstSiblingIteratorStartIsMiddleSibling)
 *      - TEST(NtNode, SiblingIteratorStartIsLastSibling)
 *      - TEST(NtNode, ConstSiblingIteratorStartIsLastSibling)
 *      - TEST(NtNode, SiblingIteratorStartIsRoot)
 *      - TEST(NtNode, ConstSiblingIteratorStartIsRoot)
 *      - TEST(NtNode, SiblingIteratorStartHasOneSibling)
 *      - TEST(NtNode, ConstSiblingIteratorStartHasOneSibling)
 *      - TEST(NtNode, SiblingIteratorRemoveSome)
 *      - TEST(NtNode, SiblingIteratorRemoveAll)
 *      - TEST(NtNode, SiblingIteratorDestroySome)
 *      - TEST(NtNode, SiblingIteratorDestroyAll)
 *      - TEST(NtNode, SiblingIteratorNextAfterDone)
 *      - TEST(NtNode, ConstSiblingIteratorNextAfterDone)
 *      - TEST(NtNode, SiblingIteratorMultipleTimes)
 *      - TEST(NtNode, ConstSiblingIteratorMultipleTimes)
 * 
 * !!! TODO Sibling next, sibling at next, sibling prev, and sibling at prev iterators!!!!
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
#include "ecu/ntnode.h"

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

    /// @brief Returns true if all supplied nodes are in a tree
    /// and are not roots.
    /// False otherwise. Function immediately stops evaluating
    /// when the first node not in a tree is processed.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static bool is_descendant(const ntnode &n0, const Nodes&... n)
    {
        bool status = ecu_ntnode_is_descendant(&n0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            status = status && ((ecu_ntnode_is_descendant(&n)) && ...); /* Only keep evaluating if true return. */
        }

        return status;
    }

    /// @brief Returns true if all supplied nodes are empty roots.
    /// False otherwise. Function immediately stops evaluating
    /// when the first node that is not empty is processed.
    template<typename... Nodes>
    requires (std::is_base_of_v<ntnode, Nodes> && ...)
    static bool not_in_tree(const ntnode &n0, const Nodes&... n)
    {
        bool status = !ecu_ntnode_in_tree(&n0);

        if constexpr(sizeof...(Nodes) > 0)
        {
            status = status && ((!ecu_ntnode_in_tree(&n)) && ...); /* Only keep evaluating if true return. */
        }

        return status;
    }

    /// @brief Returns true if all supplied nodes are roots (empty or non-empty).
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
        CHECK_TRUE( (not_in_tree(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief All nodes in subtree should be removed. Rest of 
 * tree unharmed.
 * @warning Test validation requires working postorder iterator.
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
        CHECK_TRUE( (not_in_tree(RW.at(2), RW.at(3), RW.at(4), RW.at(5))) );
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(0));
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
        CHECK_TRUE( (not_in_tree(RW.at(2))) );
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
        rw_ntnode empty_root;

        /* Step 2: Action. */
        ecu_ntnode_clear(&empty_root);

        /* Step 3: Assert. */
        CHECK_TRUE( (not_in_tree(empty_root)) );
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
/*----------------- TESTS - ECU_NTNODE_IN_TREE ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not in tree returned.
 */
TEST(NtNode, InTreeNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_in_tree(&empty_root)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief In tree returned.
 */
TEST(NtNode, InTreeNodeIsNonEmptyRoot)
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
        CHECK_TRUE( (ecu_ntnode_in_tree(&RW.at(0))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief In tree returned.
 */
TEST(NtNode, InTreeNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_in_tree(&RW.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief In tree returned.
 */
TEST(NtNode, InTreeNodeIsLeaf)
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
        CHECK_TRUE( (ecu_ntnode_in_tree(&RW.at(1))) );
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

/**
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingAfterPosIsLeftMostSubroot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0                             RW8
        |                               |
        RW1---------RW2-----RW3         RW9---RW10
        |           |       |
        RW4---RW5   RW6     RW7

        After:
        RW0
        |
        RW1---------RW8---------RW2-----RW3
        |           |           |       |
        RW4---RW5   RW9---RW10  RW6     RW7
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(3), RW.at(7));
        add_children(RW.at(8), RW.at(9), RW.at(10));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(1), &RW.at(8));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(1), RW.at(9), RW.at(10), RW.at(8),
                             RW.at(6), RW.at(2), RW.at(7), RW.at(3), RW.at(0));
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
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingAfterPosIsMiddleSubroot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0                             RW8
        |                               |
        RW1---------RW2-----RW3         RW9---RW10
        |           |       |
        RW4---RW5   RW6     RW7

        After:
        RW0
        |
        RW1---------RW2-----RW8-------------RW3
        |           |       |               |
        RW4---RW5   RW6     RW9---RW10      RW7
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(3), RW.at(7));
        add_children(RW.at(8), RW.at(9), RW.at(10));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(2), &RW.at(8));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(1), RW.at(6), RW.at(2), RW.at(9),
                             RW.at(10), RW.at(8), RW.at(7), RW.at(3), RW.at(0));
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
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingAfterPosIsRightMostSubroot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0                             RW8
        |                               |
        RW1---------RW2-----RW3         RW9---RW10
        |           |       |
        RW4---RW5   RW6     RW7

        After:
        RW0
        |
        RW1---------RW2-----RW3-----RW8
        |           |       |       |
        RW4---RW5   RW6     RW7     RW9---RW10
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(3), RW.at(7));
        add_children(RW.at(8), RW.at(9), RW.at(10));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(3), &RW.at(8));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(1), RW.at(6), RW.at(2), RW.at(7),
                             RW.at(3), RW.at(9), RW.at(10), RW.at(8), RW.at(0));
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
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingAfterPosIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0                         RW6
        |                           |
        RW1---RW2---RW3             RW7
                    |
                    RW4---RW5

        After:
        RW0
        |
        RW1---RW2---RW3
                    |
                    RW4---RW5---RW6
                                |
                                RW7
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(3), RW.at(4), RW.at(5));
        add_children(RW.at(6), RW.at(7));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(5), &RW.at(6));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(4), RW.at(5), RW.at(7), RW.at(6), RW.at(3), RW.at(0));
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
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingAfterSiblingIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0             RW3
        |
        RW1---RW2
        
        After:
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(2), &RW.at(3));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(3), RW.at(0));
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
 * @brief Not allowed. Position cannot be root.
 */
TEST(NtNode, InsertSiblingAfterPosIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode root;
        rw_ntnode sibling;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&root, &sibling);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position cannot be root.
 */
TEST(NtNode, InsertSiblingAfterPosIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. 
        RW0             RW3
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(0), &RW.at(3));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(1), &RW.at(1));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, InsertSiblingAfterSiblingIsDescendant)
{
    try
    {
        /* Step 1: Arrange. Insert RW3 after RW1.
        RW0     RW2
        |       |
        RW1     RW3 
        */
        add_children(RW.at(0), RW.at(1));
        add_children(RW.at(2), RW.at(3));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_after(&RW.at(1), &RW.at(3));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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

/**
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingBeforePosIsLeftMostSubroot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0         RW4
        |           |
        RW1         RW5---RW6
        |
        RW2---RW3

        After:
        RW0
        |
        RW4---------RW1
        |           |
        RW5---RW6   RW2---RW3
        */
        add_children(RW.at(0), RW.at(1));
        add_children(RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(4), RW.at(5), RW.at(6));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(1), &RW.at(4));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(4), RW.at(2), RW.at(3), RW.at(1), RW.at(0));
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
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingBeforePosIsMiddleSubroot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0                     RW5
        |                       |
        RW1-----RW2---RW3       RW6
                |
                RW4

        After:
        RW0
        |
        RW1-----RW5-----RW2---RW3
                |       |
                RW6     RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4));
        add_children(RW.at(5), RW.at(6));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(2), &RW.at(5));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(6), RW.at(5), RW.at(4), RW.at(2), RW.at(3), RW.at(0));
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
 * @brief Sibling inserted correctly.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, InsertSiblingBeforePosIsRightMostSubroot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0                 RW5
        |                   |
        RW1-----RW2         RW6
        |       |           |
        RW3     RW4         RW7
        
        After:
        RW0
        |
        RW1-----RW5-----RW2
        |       |       |
        RW3     RW6     RW4
                |
                RW7
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        add_children(RW.at(2), RW.at(4));
        add_branch(RW.at(5), RW.at(6), RW.at(7));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(2), &RW.at(5));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(1), RW.at(7), RW.at(6), RW.at(5), RW.at(4), RW.at(2), RW.at(0));
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingBeforePosIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0     RW2
        |       |
        RW1     RW3
        
        After:
        RW0
        |
        RW2---RW1
        |
        RW3
        */
        add_children(RW.at(0), RW.at(1));
        add_children(RW.at(2), RW.at(3));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(1), &RW.at(2));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(1), RW.at(0));
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
 * @brief Sibling inserted correctly.
 */
TEST(NtNode, InsertSiblingBeforeSiblingIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0         RW3
        |
        RW1
        |
        RW2

        After:
        RW0
        |
        RW3-----RW1
                |
                RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(1), &RW.at(3));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(2), RW.at(1), RW.at(0));
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
 * @brief Not allowed. Position cannot be root.
 */
TEST(NtNode, InsertSiblingBeforePosIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        rw_ntnode sibling;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&empty_root, &sibling);

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Not allowed. Position cannot be root.
 */
TEST(NtNode, InsertSiblingBeforePosIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. 
        RW0             RW3
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(0), &RW.at(3));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(1), &RW.at(1));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, InsertSiblingBeforeSiblingIsDescendant)
{
    try
    {
        /* Step 1: Arrange.
        RW0     RW2
        |       |
        RW1     RW3
        */
        add_children(RW.at(0), RW.at(1));
        add_children(RW.at(2), RW.at(3));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_insert_sibling_before(&RW.at(1), &RW.at(3));

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------- TESTS - ECU_NTNODE_IS_DESCENDANT -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Not descendant returned.
 */
TEST(NtNode, IsDescendantNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_is_descendant(&empty_root)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Not descendant returned.
 */
TEST(NtNode, IsDescendantNodeIsNonEmptyRoot)
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
        CHECK_FALSE( (ecu_ntnode_is_descendant(&RW.at(0))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Is descendant returned.
 */
TEST(NtNode, IsDescendantNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_is_descendant(&RW.at(1))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Is descendant returned.
 */
TEST(NtNode, IsDescendantNodeIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_is_descendant(&RW.at(2))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*----------------- TESTS - ECU_NTNODE_IS_LEAF ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Is leaf returned.
 */
TEST(NtNode, IsLeafNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_is_leaf(&empty_root)) );
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
TEST(NtNode, IsLeafNodeIsNonEmptyRoot)
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
        CHECK_FALSE( (ecu_ntnode_is_leaf(&RW.at(0))) );
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
TEST(NtNode, IsLeafNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_is_leaf(&RW.at(1))) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_is_leaf(&RW.at(2))) );
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
TEST(NtNode, IsRootNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_is_root(&empty_root)) );
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
TEST(NtNode, IsRootNodeIsNonEmptyRoot)
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
        CHECK_TRUE( (ecu_ntnode_is_root(&RW.at(0))) );
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
TEST(NtNode, IsRootNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1
        |
        RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_is_root(&RW.at(1))) );
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
        /* Step 1: Arrange. 
        RW0
        |
        RW1
        |
        RW2
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_is_root(&RW.at(2))) );
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

/**
 * @brief Last child returned.
 */
TEST(NtNode, LastChildNodeWithMultipleChildren)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
        |       |
        RW3     RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        add_children(RW.at(2), RW.at(4));
        ntnode *last;
        const ntnode *clast;

        /* Step 2: Action. */
        last = &convert(ecu_ntnode_last_child(&RW.at(0)));
        clast = &convert(ecu_ntnode_last_cchild(&RW.at(0)));

        /* Step 3: Assert. */
        CHECK_TRUE( (last == &RW.at(2)) );
        CHECK_TRUE( (clast == &RW.at(2)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        ntnode *last;
        const ntnode *clast;

        /* Step 2: Action. */
        last = &convert(ecu_ntnode_last_child(&RW.at(1)));
        clast = &convert(ecu_ntnode_last_cchild(&RW.at(1)));

        /* Step 3: Assert. */
        CHECK_TRUE( (last == &RW.at(3)) );
        CHECK_TRUE( (clast == &RW.at(3)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_last_child(&RW.at(1)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_last_cchild(&RW.at(1)) == nullptr) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3-----RW4
                        |
                        RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(4), RW.at(5), RW.at(6));
        ntnode *lca;
        const ntnode *clca;

        /* Step 2: Action. */
        lca = &convert(ecu_ntnode_lca(&RW.at(6), &RW.at(3)));
        clca = &convert(ecu_ntnode_clca(&RW.at(6), &RW.at(3)));

        /* Step 3: Assert. */
        CHECK_TRUE( (lca == &RW.at(2)) );
        CHECK_TRUE( (clca == &RW.at(2)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3-----RW4
                        |
                        RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(4), RW.at(5), RW.at(6));
        ntnode *lca;
        const ntnode *clca;

        /* Step 2: Action. */
        lca = &convert(ecu_ntnode_lca(&RW.at(3), &RW.at(6)));
        clca = &convert(ecu_ntnode_clca(&RW.at(3), &RW.at(6)));

        /* Step 3: Assert. */
        CHECK_TRUE( (lca == &RW.at(2)) );
        CHECK_TRUE( (clca == &RW.at(2)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2-----RW3
                |       |
                RW4     RW5---RW6---RW7
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4));
        add_children(RW.at(3), RW.at(5), RW.at(6), RW.at(7));
        ntnode *lca;
        const ntnode *clca;

        /* Step 2: Action. */
        lca = &convert(ecu_ntnode_lca(&RW.at(4), &RW.at(6)));
        clca = &convert(ecu_ntnode_clca(&RW.at(6), &RW.at(4))); /* Reverse parameters on purpose. */

        /* Step 3: Assert. */
        CHECK_TRUE( (lca == &RW.at(0)) );
        CHECK_TRUE( (clca == &RW.at(0)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        ntnode *lca;
        const ntnode *clca;

        /* Step 2: Action. */
        lca = &convert(ecu_ntnode_lca(&RW.at(1), &RW.at(1)));
        clca = &convert(ecu_ntnode_clca(&RW.at(1), &RW.at(1)));

        /* Step 3: Assert. */
        CHECK_TRUE( (lca == &RW.at(1)) );
        CHECK_TRUE( (clca == &RW.at(1)) );
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
        /* Step 1: Arrange. 
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        ntnode *lca;
        const ntnode *clca;

        /* Step 2: Action. */
        lca = &convert(ecu_ntnode_lca(&RW.at(0), &RW.at(0)));
        clca = &convert(ecu_ntnode_clca(&RW.at(0), &RW.at(0)));

        /* Step 3: Assert. */
        CHECK_TRUE( (lca == &RW.at(0)) );
        CHECK_TRUE( (clca == &RW.at(0)) );
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
        /* Step 1: Arrange.
        RW0             RW5
        |               |
        RW1-----RW2     RW6
                |
                RW3
                |
                RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_branch(RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_lca(&RW.at(6), &RW.at(4)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_clca(&RW.at(4), &RW.at(6)) == nullptr) ); /* Reverse parameters on purpose. */
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        ntnode *lca;
        const ntnode *clca;

        /* Step 2: Action. */
        lca = &convert(ecu_ntnode_lca(&RW.at(0), &RW.at(1)));
        clca = &convert(ecu_ntnode_clca(&RW.at(0), &RW.at(1)));

        /* Step 3: Assert. */
        CHECK_TRUE( (lca == &RW.at(0)) );
        CHECK_TRUE( (clca == &RW.at(0)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        ntnode *lca;
        const ntnode *clca;

        /* Step 2: Action. */
        lca = &convert(ecu_ntnode_lca(&RW.at(1), &RW.at(0)));
        clca = &convert(ecu_ntnode_clca(&RW.at(1), &RW.at(0)));

        /* Step 3: Assert. */
        CHECK_TRUE( (lca == &RW.at(0)) );
        CHECK_TRUE( (clca == &RW.at(0)) );
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
 * @brief 0 returned.
 */
TEST(NtNode, LevelNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_level(&empty_root) == 0) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_level(&RW.at(0)) == 0) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Correct level returned.
 */
TEST(NtNode, LevelNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        |
        RW4---RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        add_children(RW.at(3), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_level(&RW.at(3)) == 2) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Correct level returned.
 */
TEST(NtNode, LevelNodeIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        |
        RW4---RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        add_children(RW.at(3), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_level(&RW.at(5)) == 3) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Add node to different levels in tree.
 * Correct level values returned.
 */
TEST(NtNode, LevelAddAndRemoveNodeFromTree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        |
        RW4---RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        add_children(RW.at(3), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_level(&RW.at(3)) == 2) );
        ecu_ntnode_remove(&RW.at(3));
        CHECK_TRUE( (ecu_ntnode_level(&RW.at(3)) == 0) );
        ecu_ntnode_push_child_back(&RW.at(0), &RW.at(3));
        CHECK_TRUE( (ecu_ntnode_level(&RW.at(3)) == 1) );
        ecu_ntnode_remove(&RW.at(3));
        CHECK_TRUE( (ecu_ntnode_level(&RW.at(3)) == 0) );
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
 * @brief NULL returned.
 */
TEST(NtNode, NextNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_next(&empty_root) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cnext(&empty_root) == nullptr) );
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
TEST(NtNode, NextNodeIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_next(&RW.at(0)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cnext(&RW.at(0)) == nullptr) );
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
TEST(NtNode, NextNodeIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2-----RW3
        |       |       |
        RW4     RW5     RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        add_children(RW.at(3), RW.at(6));
        ntnode *next;
        const ntnode *cnext;

        /* Step 2: Action. */
        next = &convert(ecu_ntnode_next(&RW.at(1)));
        cnext = &convert(ecu_ntnode_cnext(&RW.at(1)));

        /* Step 3: Assert. */
        CHECK_TRUE( (next == &RW.at(2)) );
        CHECK_TRUE( (cnext == &RW.at(2)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2-----RW3
        |       |       |
        RW4     RW5     RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        add_children(RW.at(3), RW.at(6));
        ntnode *next;
        const ntnode *cnext;

        /* Step 2: Action. */
        next = &convert(ecu_ntnode_next(&RW.at(2)));
        cnext = &convert(ecu_ntnode_cnext(&RW.at(2)));

        /* Step 3: Assert. */
        CHECK_TRUE( (next == &RW.at(3)) );
        CHECK_TRUE( (cnext == &RW.at(3)) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2-----RW3
        |       |       |
        RW4     RW5     RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        add_children(RW.at(3), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_next(&RW.at(3)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cnext(&RW.at(3)) == nullptr) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2---RW3
        */
        add_children(RW.at(0), RW.at(1));
        add_children(RW.at(1), RW.at(2), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_next(&RW.at(1)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cnext(&RW.at(1)) == nullptr) );
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
 * @brief NULL returned.
 */
TEST(NtNode, ParentNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_parent(&empty_root) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cparent(&empty_root) == nullptr) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_parent(&RW.at(0)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cparent(&RW.at(0)) == nullptr) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Parent returned.
 */
TEST(NtNode, ParentNodeIsNonEmptySubroot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---RW3
                |
                RW3---RW4
                |
                RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(3), RW.at(5));
        ntnode *parent;
        const ntnode *cparent;

        /* Step 2: Action. */
        parent = &convert(ecu_ntnode_parent(&RW.at(3)));
        cparent = &convert(ecu_ntnode_cparent(&RW.at(3)));

        /* Step 3: Assert. */
        CHECK_TRUE( (parent == &RW.at(2)) );
        CHECK_TRUE( (cparent == &RW.at(2)) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Parent returned.
 */
TEST(NtNode, ParentNodeIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---RW3
                |
                RW3---RW4
                |
                RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(3), RW.at(5));
        ntnode *parent;
        const ntnode *cparent;

        /* Step 2: Action. */
        parent = &convert(ecu_ntnode_parent(&RW.at(5)));
        cparent = &convert(ecu_ntnode_cparent(&RW.at(5)));

        /* Step 3: Assert. */
        CHECK_TRUE( (parent == &RW.at(3)) );
        CHECK_TRUE( (cparent == &RW.at(3)) );
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
TEST(NtNode, PrevNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_prev(&empty_root) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cprev(&empty_root) == nullptr) );
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
TEST(NtNode, PrevNodeIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_prev(&RW.at(0)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cprev(&RW.at(0)) == nullptr) );
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
TEST(NtNode, PrevNodeIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1-----RW2-----RW3
        |               |
        RW4             RW6
                        |
                        RW7---RW8
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_prev(&RW.at(1)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cprev(&RW.at(1)) == nullptr) );
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
        /* Step 1: Arrange. 
        RW0
        |
        RW1-----RW2-----RW3
        |               |
        RW4             RW6
                        |
                        RW7---RW8
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(6), RW.at(7), RW.at(8));
        ntnode *prev;
        const ntnode *cprev;

        /* Step 2: Action. */
        prev = &convert(ecu_ntnode_prev(&RW.at(2)));
        cprev = &convert(ecu_ntnode_cprev(&RW.at(2)));

        /* Step 3: Assert. */
        CHECK_TRUE( (prev == &RW.at(1)) );
        CHECK_TRUE( (cprev == &RW.at(1)) );
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
        /* Step 1: Arrange. 
        RW0
        |
        RW1-----RW2-----RW3
        |               |
        RW4             RW6
                        |
                        RW7---RW8
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(6), RW.at(7), RW.at(8));
        ntnode *prev;
        const ntnode *cprev;

        /* Step 2: Action. */
        prev = &convert(ecu_ntnode_prev(&RW.at(3)));
        cprev = &convert(ecu_ntnode_cprev(&RW.at(3)));

        /* Step 3: Assert. */
        CHECK_TRUE( (prev == &RW.at(2)) );
        CHECK_TRUE( (cprev == &RW.at(2)) );
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
        CHECK_TRUE( (ecu_ntnode_prev(&RW.at(4)) == nullptr) );
        CHECK_TRUE( (ecu_ntnode_cprev(&RW.at(4)) == nullptr) );
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

/**
 * @brief Child added to back.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, PushChildBackParentWithMultipleChildren)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0                                 RW11
        |                                   |
        RW1-----RW2---RW3                   RW12---RW13
                |                           |
                RW4-----RW5---------RW6     RW14
                |       |           |
                RW7     RW8---RW9   RW10

        After:
        RW0
        |
        RW1-----RW2---RW3
                |
                RW4-----RW5---------RW6-----RW11
                |       |           |       |
                RW7     RW8---RW9   RW10    RW12---RW13
                                            |
                                            RW14
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6));
        add_children(RW.at(4), RW.at(7));
        add_children(RW.at(5), RW.at(8), RW.at(9));
        add_children(RW.at(6), RW.at(10));
        add_children(RW.at(11), RW.at(12), RW.at(13));
        add_children(RW.at(12), RW.at(14));

        /* Step 2: Action. */
        ecu_ntnode_push_child_back(&RW.at(2), &RW.at(11));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(7), RW.at(4), RW.at(8), RW.at(9), RW.at(5),
                             RW.at(10), RW.at(6), RW.at(14), RW.at(12), RW.at(13), RW.at(11),
                             RW.at(2), RW.at(3), RW.at(0));
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
 * @brief Child added to back.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, PushChildBackParentWithOneChild)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0                     RW9
        |                       |
        RW1-----RW2-----RW3     RW10----RW11
        |       |       |               |
        RW4     RW5     RW6             RW12
                |
                RW7---RW8

        After:
        RW0
        |
        RW1-----RW2-----------------RW3
        |       |                   |
        RW4     RW5---------RW9     RW6
                |           |
                RW7---RW8   RW10----RW11
                                    |
                                    RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(5), RW.at(7), RW.at(8));
        add_children(RW.at(9), RW.at(10), RW.at(11));
        add_children(RW.at(11), RW.at(12));

        /* Step 2: Action. */
        ecu_ntnode_push_child_back(&RW.at(2), &RW.at(9));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(1), RW.at(7), RW.at(8), RW.at(5), RW.at(10), RW.at(12),
                             RW.at(11), RW.at(9), RW.at(2), RW.at(6), RW.at(3), RW.at(0));
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
 * @brief Child added.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, PushChildBackParentWithNoChildren)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0                         RW6
        |
        RW1-----RW2-----RW3
                |       |
                RW4     RW5

        After:
        RW0
        |
        RW1-----RW2-----RW3
        |       |       |
        RW6     RW4     RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4));
        add_children(RW.at(3), RW.at(5));

        /* Step 2: Action. */
        ecu_ntnode_push_child_back(&RW.at(1), &RW.at(6));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(1), RW.at(4), RW.at(2), RW.at(5),
                             RW.at(3), RW.at(0));
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
 * @brief Not allowed.
 */
TEST(NtNode, PushChildBackParentEqualsChild)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_push_child_back(&RW.at(0), &RW.at(0));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
        /* Step 1: Arrange.
        RW0         RW3
        |           |
        RW1---RW2   RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(3), RW.at(4));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_push_child_back(&RW.at(1), &RW.at(4));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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

/**
 * @brief Child added to front.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, PushChildFrontParentWithMultipleChildren)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0                                 RW11
        |                                   |
        RW1-----RW2---RW3                   RW12---RW13
                |                           |
                RW4-----RW5---------RW6     RW14
                |       |           |
                RW7     RW8---RW9   RW10

        After:
        RW0
        |
        RW1-----RW2---RW3
                |
                RW11------------RW4-----RW5---------RW6
                |               |       |           |
                RW12---RW13     RW7     RW8---RW9   RW10
                |
                RW14
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5), RW.at(6));
        add_children(RW.at(4), RW.at(7));
        add_children(RW.at(5), RW.at(8), RW.at(9));
        add_children(RW.at(6), RW.at(10));
        add_children(RW.at(11), RW.at(12), RW.at(13));
        add_children(RW.at(12), RW.at(14));

        /* Step 2: Action. */
        ecu_ntnode_push_child_front(&RW.at(2), &RW.at(11));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(14), RW.at(12), RW.at(13), RW.at(11), RW.at(7),
                             RW.at(4), RW.at(8), RW.at(9), RW.at(5), RW.at(10), RW.at(6),
                             RW.at(2), RW.at(3), RW.at(0));
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
 * @brief Child added to front.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, PushChildFrontParentWithOneChild)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0                     RW9
        |                       |
        RW1-----RW2-----RW3     RW10----RW11
        |       |       |               |
        RW4     RW5     RW6             RW12
                |
                RW7---RW8

        After:
        RW0
        |
        RW1-----RW2-------------------------RW3
        |       |                           |
        RW4     RW9-------------RW5         RW6
                |               |
                RW10----RW11    RW7---RW8
                        |
                        RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5));
        add_children(RW.at(3), RW.at(6));
        add_children(RW.at(5), RW.at(7), RW.at(8));
        add_children(RW.at(9), RW.at(10), RW.at(11));
        add_children(RW.at(11), RW.at(12));

        /* Step 2: Action. */
        ecu_ntnode_push_child_front(&RW.at(2), &RW.at(9));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(1), RW.at(10), RW.at(12), RW.at(11), RW.at(9),
                             RW.at(7), RW.at(8), RW.at(5), RW.at(2), RW.at(6), RW.at(3), RW.at(0));
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
 * @brief Child added.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, PushChildFrontParentWithNoChildren)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0                         RW6
        |
        RW1-----RW2-----RW3
                |       |
                RW4     RW5

        After:
        RW0
        |
        RW1-----RW2-----RW3
        |       |       |
        RW6     RW4     RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4));
        add_children(RW.at(3), RW.at(5));

        /* Step 2: Action. */
        ecu_ntnode_push_child_front(&RW.at(1), &RW.at(6));

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(1), RW.at(4), RW.at(2), RW.at(5),
                             RW.at(3), RW.at(0));
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
 * @brief Not allowed.
 */
TEST(NtNode, PushChildFrontParentEqualsChild)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        */
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_push_child_front(&RW.at(0), &RW.at(0));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
        /* Step 1: Arrange.
        RW0         RW3
        |           |
        RW1---RW2   RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(3), RW.at(4));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_push_child_front(&RW.at(1), &RW.at(4));

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, RemoveNodeIsLeftMostSubroot)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---------RW2-----RW3
        |           |       |
        RW4---RW5   RW6     RW7

        After:
        RW0             RW1
        |               |
        RW2-----RW3     RW4---RW5
        |       |
        RW6     RW7
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(3), RW.at(7));

        /* Step 2: Action. */
        ecu_ntnode_remove(&RW.at(1));

        /* Step 3: Assert. Verify tree still intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(2), RW.at(7), RW.at(3), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert: Verify removed tree still intact. */
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(1));
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
 * @brief Node removed correctly. Node's tree and
 * tree it was removed from unharmed.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, RemoveNodeIsMiddleSubroot)
{
    try
    {
        /* Step 1: Arrange.
        
        Before:
        RW0
        |
        RW1---------RW2-----RW3
        |           |       |
        RW4---RW5   RW6     RW7
                    |
                    RW8
                    |
                    RW9

        After:
        RW0                 RW2
        |                   |
        RW1---------RW3     RW6
        |           |       |
        RW4---RW5   RW7     RW8
                            |
                            RW9
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_branch(RW.at(2), RW.at(6), RW.at(8), RW.at(9));
        add_children(RW.at(3), RW.at(7));

        /* Step 2: Action. */
        ecu_ntnode_remove(&RW.at(2));

        /* Step 3: Assert. Verify tree still intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(1), RW.at(7), RW.at(3), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert: Verify removed tree still intact. */
        EXPECT_NODES_IN_TREE(RW.at(9), RW.at(8), RW.at(6), RW.at(2));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(2))
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
 * @brief Node removed correctly. Node's tree and
 * tree it was removed from unharmed.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, RemoveNodeIsRightMostSubroot)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---------RW2-----RW3
        |           |       |
        RW4---RW5   RW6     RW7

        After:
        RW0                 RW3
        |                   |
        RW1---------RW2     RW7
        |           |
        RW4---RW5   RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(3), RW.at(7));

        /* Step 2: Action. */
        ecu_ntnode_remove(&RW.at(3));

        /* Step 3: Assert. Verify tree still intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(1), RW.at(6), RW.at(2), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert: Verify removed tree still intact. */
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(3));
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
 * @brief Node removed correctly. Tree node was
 * removed from unharmed.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, RemoveNodeIsLeaf)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1
        |
        RW2

        After:
        RW0     RW2
        |
        RW1
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2));

        /* Step 2: Action. */
        ecu_ntnode_remove(&RW.at(2));

        /* Step 3: Assert. Verify tree still intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert: Verify removed tree still intact. */
        EXPECT_NODES_IN_TREE(RW.at(2));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(2))
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
 * @brief OK. Does nothing.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, RemoveNodeIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));

        /* Step 2: Action. */
        ecu_ntnode_remove(&RW.at(0));

        /* Step 3: Assert. Verify tree unchanged. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(0));
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
 * @brief OK. Does nothing.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, RemoveNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        
        /* Step 2: Action. */
        ecu_ntnode_remove(&empty_root);

        /* Step 3: Assert. Verify tree unchanged. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(empty_root);
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &empty_root)
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
/*------------------- TESTS - ECU_NTNODE_SIZE ----------------*/
/*------------------------------------------------------------*/

/**
 * @brief 0 returned.
 */
TEST(NtNode, SizeNodeIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_size(&empty_root) == 0) );
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
TEST(NtNode, SizeNodeIsNonEmptyRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2 
        |
        RW3
        |
        RW4
        */
        add_branch(RW.at(0), RW.at(1), RW.at(3), RW.at(4));
        add_siblings_after(RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(0)) == 4) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---RW3
                |
                RW4---RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(2), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(2)) == 2) );
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
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(1)) == 0) );
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(4)) == 0) );
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(3)) == 0) );
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
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2-----RW3
        |       |       |
        RW4     RW7     RW8
        |
        RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(7));
        add_children(RW.at(3), RW.at(8));
        add_children(RW.at(4), RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(0)) == 8) );
        ecu_ntnode_remove(&RW.at(3));
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(0)) == 6) );
        ecu_ntnode_remove(&RW.at(1));
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(0)) == 2) );
        ecu_ntnode_push_child_back(&RW.at(7), &RW.at(3));
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(0)) == 4) );
        ecu_ntnode_insert_sibling_after(&RW.at(2), &RW.at(10));
        CHECK_TRUE( (ecu_ntnode_size(&RW.at(0)) == 5) );
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
        /* Step 1: Arrange. */
        ecu_ntnode node;

        /* Steps 2 and 3: Action and assert. */
        CHECK_FALSE( (ecu_ntnode_valid(&node)) );
        ecu_ntnode_ctor(&node, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        CHECK_TRUE( (ecu_ntnode_valid(&node)) );
        ecu_ntnode_destroy(&node);
        CHECK_FALSE( (ecu_ntnode_valid(&node)) );
        ecu_ntnode_ctor(&node, ECU_NTNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
        CHECK_TRUE( (ecu_ntnode_valid(&node)) );
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
/*-------------- TESTS - NEXT SIBLING AT ITERATOR ------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, NextSiblingAtIteratorStartIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------RW3
        |       |                       |
        RW4     RW5---RW6---RW7---RW8   RW9---RW10
                |           |
                RW11        RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        add_children(RW.at(5), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(5))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstNextSiblingAtIteratorStartIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------RW3
        |       |                       |
        RW4     RW5---RW6---RW7---RW8   RW9---RW10
                |           |
                RW11        RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        add_children(RW.at(5), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &RW.at(5))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, NextSiblingAtIteratorStartIsMiddleSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3---RW4---RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5), RW.at(6));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(4))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstNextSiblingAtIteratorStartIsMiddleSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3---RW4---RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5), RW.at(6));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &RW.at(4))
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
 * @brief Last sibling returned then iteration ends.
 */
TEST(NtNode, NextSiblingAtIteratorStartIsLastSibling)
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
        EXPECT_NODES_IN_TREE(RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(2))
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
 * @brief Last sibling returned then iteration ends.
 */
TEST(NtNode, ConstNextSiblingAtIteratorStartIsLastSibling)
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
        EXPECT_NODES_IN_TREE(RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &RW.at(2))
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
 * @brief Root returned then iteration ends.
 */
TEST(NtNode, NextSiblingAtIteratorStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_NODES_IN_TREE(empty_root);

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &empty_root)
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
 * @brief Root returned then iteration ends.
 */
TEST(NtNode, ConstNextSiblingAtIteratorStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_NODES_IN_TREE(empty_root);

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &empty_root)
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, NextSiblingAtIteratorStartHasOneSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(1))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstNextSiblingAtIteratorStartHasOneSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &RW.at(1))
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
 * @brief Remove some nodes in the middle of an iteration. 
 * Verify trees intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingAtIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---RW2---RO0-----RO1
        |           |       |
        RW3---RW4   RW5     RW6---RW7
        
        After:
        RW0                 RW1         RW2
        |                   |
        RO0-----RO1         RW3---RW4
        |       |
        RW5     RW6---RW7
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RO.at(0), RO.at(1));
        add_children(RW.at(1), RW.at(3), RW.at(4));
        add_children(RO.at(0), RW.at(5));
        add_children(RO.at(1), RW.at(6), RW.at(7));

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(5), RO.at(0), RW.at(6), RW.at(7), RO.at(1), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert. Verify removed trees intact. */
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(4), RW.at(1));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        CHECK_TRUE( (not_in_tree(RW.at(2))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Remove all nodes in the iteration. Verify
 * trees intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingAtIteratorRemoveAll)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---RW2---RW3-----RW4
        |                   |
        RW5---RW6           RW8---RW9
        
        After:
        RW0     RW1         RW2     RW3     RW4
                |                           |
                RW5---RW6                   RW8---RW9
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(1), RW.at(5), RW.at(6));
        add_children(RW.at(4), RW.at(8), RW.at(9));

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        CHECK_TRUE( (not_in_tree(RW.at(0))) );

        /* Step 3: Assert. Verify removed trees intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(1));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        EXPECT_NODES_IN_TREE(RW.at(8), RW.at(9), RW.at(4));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(4))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        CHECK_TRUE( (not_in_tree(RW.at(2), RW.at(3))) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief Nothing happens to tree.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingAtIteratorStartIsRootRemoveRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(0));
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
 * @brief Destroy some nodes in the middle of an iteration. 
 * Verify remaining tree intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingAtIteratorDestroySome)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        DN0-----DN1---------DN2-----DN3
        |       |           |       |
        RW1     RW2---RW3   RW4     RW5
        |
        RW6 
        
        After:
        RW0
        |
        DN0-----DN1
        |       |
        RW1     RW2---RW3
        |
        RW6
        */
        add_children(RW.at(0), DN.at(0), DN.at(1), DN.at(2), DN.at(3));
        add_children(DN.at(0), RW.at(1));
        add_children(DN.at(1), RW.at(2), RW.at(3));
        add_children(DN.at(2), RW.at(4));
        add_children(DN.at(3), RW.at(5));
        add_children(RW.at(1), RW.at(6));
        EXPECT_NODES_DESTROYED(DN.at(2), RW.at(4), DN.at(3), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &DN.at(2))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(1), DN.at(0), RW.at(2), RW.at(3), DN.at(1), RW.at(0));
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
 * @brief Destroy all nodes in the iteration. Verify
 * remaining tree intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingAtIteratorDestroyAll)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1
        |
        DN0-----DN1---------DN2-----DN3
        |       |           |       |
        RW2     RW3---RW4   RW5     RW6
        |
        RW7 
        
        After:
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        add_children(RW.at(1), DN.at(0), DN.at(1), DN.at(2), DN.at(3));
        add_children(DN.at(0), RW.at(2));
        add_children(DN.at(1), RW.at(3), RW.at(4));
        add_children(DN.at(2), RW.at(5));
        add_children(DN.at(3), RW.at(6));
        add_children(RW.at(2), RW.at(7));
        EXPECT_NODES_DESTROYED(DN.at(0), RW.at(2), RW.at(7), DN.at(1), RW.at(3), RW.at(4), DN.at(2),
                               RW.at(5), DN.at(3), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &DN.at(0))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(0));
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
 * @brief Iteration should be able to exit after root destroyed.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingAtIteratorStartIsRootDestroyRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_NODES_DESTROYED(RW.at(0), RW.at(1), RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(0))
        {
            ecu_ntnode_destroy(n);
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
TEST(NtNode, NextSiblingAtIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &empty_root)
        {
            (void)n;
        }
        (void)ecu_ntnode_next_sibling_iterator_next(&iter);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, ConstNextSiblingAtIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &empty_root)
        {
            (void)n;
        }
        (void)ecu_ntnode_next_sibling_iterator_cnext(&citer);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, NextSiblingAtIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(3), RW.at(2), RW.at(3), RW.at(3));
        ecu_ntnode_next_sibling_iterator iter;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_NEXT_SIBLING_AT_FOR_EACH(n, &iter, &RW.at(3))
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
TEST(NtNode, ConstNextSiblingAtIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(1), RW.at(2), RW.at(3), RW.at(2), RW.at(3), RW.at(3));
        ecu_ntnode_next_sibling_citerator citer;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_NEXT_SIBLING_AT_FOR_EACH(n, &citer, &RW.at(3))
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
/*--------------- TESTS - NEXT SIBLING ITERATOR --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, NextSiblingIteratorStartIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------RW3
        |       |                       |
        RW4     RW5---RW6---RW7---RW8   RW9---RW10
                |           |
                RW11        RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        add_children(RW.at(5), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(5))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstNextSiblingIteratorStartIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------RW3
        |       |                       |
        RW4     RW5---RW6---RW7---RW8   RW9---RW10
                |           |
                RW11        RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8));
        add_children(RW.at(3), RW.at(9), RW.at(10));
        add_children(RW.at(5), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &RW.at(5))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, NextSiblingIteratorStartIsMiddleSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3---RW4---RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5), RW.at(6));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(4))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstNextSiblingIteratorStartIsMiddleSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2
                |
                RW3---RW4---RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(2), RW.at(3), RW.at(4), RW.at(5), RW.at(6));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &RW.at(4))
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
 * @brief Iteration immediately exits.
 */
TEST(NtNode, NextSiblingIteratorStartIsLastSibling)
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
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(2))
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
 * @brief Iteration immediately exits.
 */
TEST(NtNode, ConstNextSiblingIteratorStartIsLastSibling)
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
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &RW.at(2))
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
 * @brief Iteration immediately exits.
 */
TEST(NtNode, NextSiblingIteratorStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &empty_root)
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
 * @brief Iteration immediately exits.
 */
TEST(NtNode, ConstNextSiblingIteratorStartIsRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &empty_root)
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
 * @brief Next sibling returned then iteration exits.
 */
TEST(NtNode, NextSiblingIteratorStartHasOneSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
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
 * @brief Next sibling returned then iteration exits.
 */
TEST(NtNode, ConstNextSiblingIteratorStartHasOneSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        |
        RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(2));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &RW.at(1))
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
 * @brief Remove some nodes in the middle of an iteration. 
 * Verify trees intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---RW2---RO0-----RW3
        |           |       |
        RW4---RW5   RW6     RW7---RW8
        
        After:
        RW0                 RW2     RW3
        |                           |
        RW1---------RO0             RW7---RW8
        |           |
        RW4---RW5   RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RO.at(0), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RO.at(0), RW.at(6));
        add_children(RW.at(3), RW.at(7), RW.at(8));

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(5), RW.at(1), RW.at(6), RO.at(0), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert. Verify removed trees intact. */
        CHECK_TRUE( (not_in_tree(RW.at(2))) );
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(8), RW.at(3));
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
 * @brief Remove all nodes in the iteration. Verify
 * trees intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingIteratorRemoveAll)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---RW2---RW3-----RW4
        |           |       |
        RW5---RW6   RW7     RW8---RW9
        
        After:
        RW0         RW2     RW3     RW4
        |                   |       |
        RW1                 RW7     RW8---RW9
        |
        RW5---RW6
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(1), RW.at(5), RW.at(6));
        add_children(RW.at(3), RW.at(7));
        add_children(RW.at(4), RW.at(8), RW.at(9));

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(1), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert. Verify removed trees intact. */
        CHECK_TRUE( (not_in_tree(RW.at(2))) );
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(3));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(3))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        EXPECT_NODES_IN_TREE(RW.at(8), RW.at(9), RW.at(4));
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
 * @brief Destroy some nodes in the middle of an iteration. 
 * Verify remaining tree intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingIteratorDestroySome)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        DN0-----DN1---------DN2-----DN3
        |       |           |       |
        RW1     RW2---RW3   RW4     RW5
        |
        RW6 
        
        After:
        RW0
        |
        DN0-----DN1
        |       |
        RW1     RW2---RW3
        |
        RW6
        */
        add_children(RW.at(0), DN.at(0), DN.at(1), DN.at(2), DN.at(3));
        add_children(DN.at(0), RW.at(1));
        add_children(DN.at(1), RW.at(2), RW.at(3));
        add_children(DN.at(2), RW.at(4));
        add_children(DN.at(3), RW.at(5));
        add_children(RW.at(1), RW.at(6));
        EXPECT_NODES_DESTROYED(DN.at(2), RW.at(4), DN.at(3), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &DN.at(1))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(6), RW.at(1), DN.at(0), RW.at(2), RW.at(3), DN.at(1), RW.at(0));
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
 * @brief Destroy all nodes in the iteration. Verify
 * remaining tree intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, NextSiblingIteratorDestroyAll)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1
        |
        DN0-----DN1---------DN2-----DN3
        |       |           |       |
        RW2     RW3---RW4   RW5     RW6
        |
        RW7 
        
        After:
        RW0
        |
        RW1
        |
        DN0
        |
        RW2
        |
        RW7
        */
        add_children(RW.at(0), RW.at(1));
        add_children(RW.at(1), DN.at(0), DN.at(1), DN.at(2), DN.at(3));
        add_children(DN.at(0), RW.at(2));
        add_children(DN.at(1), RW.at(3), RW.at(4));
        add_children(DN.at(2), RW.at(5));
        add_children(DN.at(3), RW.at(6));
        add_children(RW.at(2), RW.at(7));
        EXPECT_NODES_DESTROYED(DN.at(1), RW.at(3), RW.at(4), DN.at(2), RW.at(5), DN.at(3), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &DN.at(0))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(7), RW.at(2), DN.at(0), RW.at(1), RW.at(0));
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
 * @brief Calling next() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, NextSiblingIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_iterator iter;
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &empty_root)
        {
            (void)n;
        }
        (void)ecu_ntnode_next_sibling_iterator_next(&iter);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, ConstNextSiblingIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_next_sibling_citerator citer;
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &empty_root)
        {
            (void)n;
        }
        (void)ecu_ntnode_next_sibling_iterator_cnext(&citer);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, NextSiblingIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3---RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(3), RW.at(4), RW.at(3), RW.at(4), RW.at(4));
        ecu_ntnode_next_sibling_iterator iter;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_NEXT_SIBLING_FOR_EACH(n, &iter, &RW.at(3))
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
TEST(NtNode, ConstNextSiblingIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3---RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(3), RW.at(4), RW.at(3), RW.at(4), RW.at(4));
        ecu_ntnode_next_sibling_citerator citer;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_NEXT_SIBLING_FOR_EACH(n, &citer, &RW.at(3))
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
        CHECK_TRUE( (is_descendant(RO.at(1))) );
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
 * @brief Nothing should happen to tree.
 * @warning Test verification requires working postorder iterator.
 */
TEST(NtNode, ParentAtIteratorStartIsRootRemoveRoot)
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

        /* Step 2: Action. */
        ecu_ntnode_parent_iterator iter;
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(0))
        {
            ecu_ntnode_remove(n);
        }

        /* Step 3: Assert. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(1), RW.at(5), RW.at(2), RW.at(3), RW.at(0));
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
 * @brief Iteration should be able to successfully terminate
 * if root node destroyed.
 */
TEST(NtNode, ParentAtIteratorStartIsRootDestroyRoot)
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
        EXPECT_NODES_DESTROYED(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_parent_iterator iter;
        ECU_NTNODE_PARENT_AT_FOR_EACH(n, &iter, &RW.at(0))
        {
            ecu_ntnode_destroy(n);
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
 * @brief Calling cnext() after an iteration is finished
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
        CHECK_TRUE( (is_descendant(RW.at(3), RO.at(1))) );
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
        CHECK_TRUE( (is_descendant(RW.at(5))) );
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
        CHECK_TRUE( (is_descendant(RO.at(5), RO.at(6))) );

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
        CHECK_TRUE( (is_descendant(RO.at(5))) );

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
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---RW3-------RW4
        |       |               |
        RW5     RW6-----RW7     RW8
        |               |
        RW9             RW10---RW11
        |               |
        RW12            RW13
        |
        RW14
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_branch(RW.at(1), RW.at(5), RW.at(9), RW.at(12), RW.at(14));
        add_children(RW.at(2), RW.at(6), RW.at(7));
        add_children(RW.at(4), RW.at(8));
        add_children(RW.at(7), RW.at(10), RW.at(11));
        add_children(RW.at(10), RW.at(13));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(5), RW.at(9), RW.at(12), RW.at(14),
                             RW.at(2), RW.at(6), RW.at(7), RW.at(10), RW.at(13),
                             RW.at(11), RW.at(3), RW.at(4), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief General iteration test. Verify all nodes iterated
 * over.
 */
TEST(NtNode, ConstPreorderIterator)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---RW3-------RW4
        |       |               |
        RW5     RW6-----RW7     RW8
        |               |
        RW9             RW10---RW11
        |               |
        RW12            RW13
        |
        RW14
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_branch(RW.at(1), RW.at(5), RW.at(9), RW.at(12), RW.at(14));
        add_children(RW.at(2), RW.at(6), RW.at(7));
        add_children(RW.at(4), RW.at(8));
        add_children(RW.at(7), RW.at(10), RW.at(11));
        add_children(RW.at(10), RW.at(13));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(5), RW.at(9), RW.at(12), RW.at(14),
                             RW.at(2), RW.at(6), RW.at(7), RW.at(10), RW.at(13),
                             RW.at(11), RW.at(3), RW.at(4), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief General iteration test. Verify all nodes iterated
 * over.
 */
TEST(NtNode, PreorderIterator2)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---------------------------------RW2---RW3
        |                                   |
        RW4-------------------------RW5     RW6
        |                           |
        RW7-----RW8---RW9           RW10----RW11
        |       |                           |
        RW12    RW13                        RW14----RW15
                                                    |
                                                    RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(4), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(5), RW.at(10), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        add_children(RW.at(8), RW.at(13));
        add_children(RW.at(11), RW.at(14), RW.at(15));
        add_children(RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(4), RW.at(7), RW.at(12), RW.at(8), RW.at(13),
                             RW.at(9), RW.at(5), RW.at(10), RW.at(11), RW.at(14), RW.at(15), RW.at(16),
                             RW.at(2), RW.at(6), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief General iteration test. Verify all nodes iterated
 * over.
 */
TEST(NtNode, ConstPreorderIterator2)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---------------------------------RW2---RW3
        |                                   |
        RW4-------------------------RW5     RW6
        |                           |
        RW7-----RW8---RW9           RW10----RW11
        |       |                           |
        RW12    RW13                        RW14----RW15
                                                    |
                                                    RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(4), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(5), RW.at(10), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        add_children(RW.at(8), RW.at(13));
        add_children(RW.at(11), RW.at(14), RW.at(15));
        add_children(RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(4), RW.at(7), RW.at(12), RW.at(8), RW.at(13),
                             RW.at(9), RW.at(5), RW.at(10), RW.at(11), RW.at(14), RW.at(15), RW.at(16),
                             RW.at(2), RW.at(6), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Start iteration at node in a left-most subtree.
 */
TEST(NtNode, PreorderIteratorStartIsLeftMostSubtree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---------------------------------RW2---RW3
        |                                   |
        RW4-------------------------RW5     RW6
        |                           |
        RW7-----RW8---RW9           RW10
        |       |
        RW11    RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(4), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(5), RW.at(10));
        add_children(RW.at(7), RW.at(11));
        add_children(RW.at(8), RW.at(12));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(7), RW.at(11), RW.at(8), RW.at(12), RW.at(9));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(4))
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
 * @brief Start iteration at node in a left-most subtree.
 */
TEST(NtNode, ConstPreorderIteratorStartIsLeftMostSubtree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---------------------------------RW2---RW3
        |                                   |
        RW4-------------------------RW5     RW6
        |                           |
        RW7-----RW8---RW9           RW10
        |       |
        RW11    RW12
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(4), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(5), RW.at(10));
        add_children(RW.at(7), RW.at(11));
        add_children(RW.at(8), RW.at(12));
        EXPECT_NODES_IN_TREE(RW.at(4), RW.at(7), RW.at(11), RW.at(8), RW.at(12), RW.at(9));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(4))
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
 * @brief Start iteration at node in a middle subtree.
 */
TEST(NtNode, PreorderIteratorStartIsMiddleSubtree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---------------------------------RW2---RW3
        |                                   |
        RW4-------------------------RW5     RW6
        |                           |
        RW7-----RW8---RW9           RW10----RW11
        |       |                           |
        RW12    RW13                        RW14----RW15
                                                    |
                                                    RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(4), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(5), RW.at(10), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        add_children(RW.at(8), RW.at(13));
        add_children(RW.at(11), RW.at(14), RW.at(15));
        add_children(RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(10), RW.at(11), RW.at(14), RW.at(15), RW.at(16));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(5))
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
 * @brief Start iteration at node in a middle subtree.
 */
TEST(NtNode, ConstPreorderIteratorStartIsMiddleSubtree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---------------------------------RW2---RW3
        |                                   |
        RW4-------------------------RW5     RW6
        |                           |
        RW7-----RW8---RW9           RW10----RW11
        |       |                           |
        RW12    RW13                        RW14----RW15
                                                    |
                                                    RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5));
        add_children(RW.at(2), RW.at(6));
        add_children(RW.at(4), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(5), RW.at(10), RW.at(11));
        add_children(RW.at(7), RW.at(12));
        add_children(RW.at(8), RW.at(13));
        add_children(RW.at(11), RW.at(14), RW.at(15));
        add_children(RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(10), RW.at(11), RW.at(14), RW.at(15), RW.at(16));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(5))
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
TEST(NtNode, PreorderIteratorStartIsRightMostSubtree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-------------RW2
        |               |
        RW3-----RW4     RW5---RW6
                |       |
                RW7     RW8---RW9
                        |
                        RW10----RW11
                        |       |
                        RW12    RW13
                                |
                                RW14
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6));
        add_children(RW.at(4), RW.at(7));
        add_children(RW.at(5), RW.at(8), RW.at(9));
        add_children(RW.at(8), RW.at(10), RW.at(11));
        add_children(RW.at(10), RW.at(12));
        add_branch(RW.at(11), RW.at(13), RW.at(14));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(5), RW.at(8), RW.at(10), RW.at(12), RW.at(11),
                             RW.at(13), RW.at(14), RW.at(9), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(2))
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
TEST(NtNode, ConstPreorderIteratorStartIsRightMostSubtree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-------------RW2
        |               |
        RW3-----RW4     RW5---RW6
                |       |
                RW7     RW8---RW9
                        |
                        RW10----RW11
                        |       |
                        RW12    RW13
                                |
                                RW14
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        add_children(RW.at(1), RW.at(3), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6));
        add_children(RW.at(4), RW.at(7));
        add_children(RW.at(5), RW.at(8), RW.at(9));
        add_children(RW.at(8), RW.at(10), RW.at(11));
        add_children(RW.at(10), RW.at(12));
        add_branch(RW.at(11), RW.at(13), RW.at(14));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(5), RW.at(8), RW.at(10), RW.at(12), RW.at(11),
                             RW.at(13), RW.at(14), RW.at(9), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(2))
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
TEST(NtNode, PreorderIteratorStartIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_NODES_IN_TREE(RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(1))
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
TEST(NtNode, ConstPreorderIteratorStartIsLeaf)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_NODES_IN_TREE(RW.at(1));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(1))
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
TEST(NtNode, PreorderIteratorStartIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_NODES_IN_TREE(empty_root);

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &empty_root)
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
TEST(NtNode, ConstPreorderIteratorStartIsEmptyRoot)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_NODES_IN_TREE(empty_root);

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &empty_root)
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
TEST(NtNode, PreorderIteratorStartIsMiddleChildInDegenerateTree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        |
        RW4
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(3), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(2))
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
TEST(NtNode, ConstPreorderIteratorStartIsMiddleChildInDegenerateTree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        |
        RW4
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(3), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(2))
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
TEST(NtNode, PreorderIteratorStartIsRootOfDegenerateTree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        |
        RW4
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
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
TEST(NtNode, ConstPreorderIteratorStartIsRootOfDegenerateTree)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        |
        RW2
        |
        RW3
        |
        RW4
        */
        add_branch(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Removing nodes is not allowed in a preorder iteration
 * since it will corrupt the current iteration.
 */
TEST(NtNode, PreorderIteratorRemoveNodeInNonEmptyTree)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            ntnode *node = &convert(n);
            if (node == &RW.at(2))
            {
                ecu_ntnode_remove(node);
            }
        }

        /* Step 3: Assert. Test fails if assertion does not fire. */
    }
    catch (const AssertException& e)
    {
        /* OK. */
        (void)e;
    }
}

/**
 * @brief Destroying nodes is not allowed in a preorder iteration
 * since it will corrupt the current iteration.
 */
TEST(NtNode, PreorderIteratorDestroyNodeInNonEmptyTree)
{
    try
    {
        /* Step 1: Arrange. N used to make destroy callback unused.
        RW0
        |
        N----RW1
        */
        rw_ntnode N{ECU_NTNODE_DESTROY_UNUSED};
        add_children(RW.at(0), N, RW.at(2));
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            ntnode *node = &convert(n);
            if (node == &N)
            {
                ecu_ntnode_destroy(&N);
            }
        }

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, PreorderIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        ecu_ntnode_preorder_iterator iter;
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            (void)n;
        }
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_preorder_iterator_next(&iter);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, ConstPreorderIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. 
        RW0
        |
        RW1---RW2
        */
        add_children(RW.at(0), RW.at(1), RW.at(2));
        ecu_ntnode_preorder_citerator citer;
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            (void)n;
        }
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_preorder_iterator_cnext(&citer);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, PreorderIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(0), RW.at(1), RW.at(0), RW.at(1));
        ecu_ntnode_preorder_iterator iter;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_PREORDER_FOR_EACH(n, &iter, &RW.at(0))
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
TEST(NtNode, ConstPreorderIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1
        */
        add_children(RW.at(0), RW.at(1));
        EXPECT_NODES_IN_TREE(RW.at(0), RW.at(1), RW.at(0), RW.at(1), RW.at(0), RW.at(1));
        ecu_ntnode_preorder_citerator citer;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_PREORDER_FOR_EACH(n, &citer, &RW.at(0))
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
/*--------------- TESTS - PREV SIBLING ITERATOR --------------*/
/*------------------------------------------------------------*/
#warning "TODO!!"

/*------------------------------------------------------------*/
/*----------------- TESTS - SIBLING ITERATOR -----------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, SiblingIteratorStartIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----------------------------------------RW2---------RW3
        |                                           |           |
        RW4-----RW5-------------RW6-----RW7         RW8--RW9    RW10
        |       |               |       |
        RW11    RW12---RW13     RW14    RW15
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5), RW.at(6), RW.at(7));
        add_children(RW.at(2), RW.at(8), RW.at(9));
        add_children(RW.at(3), RW.at(10));
        add_children(RW.at(4), RW.at(11));
        add_children(RW.at(5), RW.at(12), RW.at(13));
        add_children(RW.at(6), RW.at(14));
        add_children(RW.at(7), RW.at(15));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_iterator iter;
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstSiblingIteratorStartIsFirstSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----------------------------------------RW2---------RW3
        |                                           |           |
        RW4-----RW5-------------RW6-----RW7         RW8--RW9    RW10
        |       |               |       |
        RW11    RW12---RW13     RW14    RW15
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5), RW.at(6), RW.at(7));
        add_children(RW.at(2), RW.at(8), RW.at(9));
        add_children(RW.at(3), RW.at(10));
        add_children(RW.at(4), RW.at(11));
        add_children(RW.at(5), RW.at(12), RW.at(13));
        add_children(RW.at(6), RW.at(14));
        add_children(RW.at(7), RW.at(15));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_citerator citer;
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, SiblingIteratorStartIsMiddleSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------------------RW3
        |       |                                   |
        RW4     RW5---RW6---RW7-----RW8-----RW9     RW10
                |           |       |       |
                RW11--RW12  RW13    RW14    RW15---RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(3), RW.at(10));
        add_children(RW.at(5), RW.at(11), RW.at(12));
        add_children(RW.at(7), RW.at(13));
        add_children(RW.at(8), RW.at(14));
        add_children(RW.at(9), RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(8), RW.at(9), RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_iterator iter;
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(7))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstSiblingIteratorStartIsMiddleSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------------------RW3
        |       |                                   |
        RW4     RW5---RW6---RW7-----RW8-----RW9     RW10
                |           |       |       |
                RW11--RW12  RW13    RW14    RW15---RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(3), RW.at(10));
        add_children(RW.at(5), RW.at(11), RW.at(12));
        add_children(RW.at(7), RW.at(13));
        add_children(RW.at(8), RW.at(14));
        add_children(RW.at(9), RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(8), RW.at(9), RW.at(5), RW.at(6));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_citerator citer;
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(7))
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, SiblingIteratorStartIsLastSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------------------RW3
        |       |                                   |
        RW4     RW5---RW6---RW7-----RW8-----RW9     RW10
                |           |       |       |
                RW11--RW12  RW13    RW14    RW15---RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(3), RW.at(10));
        add_children(RW.at(5), RW.at(11), RW.at(12));
        add_children(RW.at(7), RW.at(13));
        add_children(RW.at(8), RW.at(14));
        add_children(RW.at(9), RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_iterator iter;
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
 * @brief Verify correct nodes in iteration returned.
 */
TEST(NtNode, ConstSiblingIteratorStartIsLastSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2---------------------------------RW3
        |       |                                   |
        RW4     RW5---RW6---RW7-----RW8-----RW9     RW10
                |           |       |       |
                RW11--RW12  RW13    RW14    RW15---RW16
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6), RW.at(7), RW.at(8), RW.at(9));
        add_children(RW.at(3), RW.at(10));
        add_children(RW.at(5), RW.at(11), RW.at(12));
        add_children(RW.at(7), RW.at(13));
        add_children(RW.at(8), RW.at(14));
        add_children(RW.at(9), RW.at(15), RW.at(16));
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(7), RW.at(8));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_citerator citer;
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
 * @brief Iteration immediately ends.
 */
TEST(NtNode, SiblingIteratorStartIsRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_iterator iter;
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(0))
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
 * @brief Iteration immediately ends.
 */
TEST(NtNode, ConstSiblingIteratorStartIsRoot)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_citerator citer;
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(0))
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
 * @brief Next sibling returned then iteration ends.
 */
TEST(NtNode, SiblingIteratorStartHasOneSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2-------------RW3---RW4
        |       |               |
        RW5     RW6-----RW7     RW8
                        |
                        RW9
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(1), RW.at(5));
        add_children(RW.at(2), RW.at(6), RW.at(7));
        add_children(RW.at(3), RW.at(8));
        add_children(RW.at(7), RW.at(9));
        EXPECT_NODES_IN_TREE(RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_iterator iter;
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
 * @brief Next sibling returned then iteration ends.
 */
TEST(NtNode, ConstSiblingIteratorStartHasOneSibling)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1-----RW2-------------RW3---RW4
        |       |               |
        RW5     RW6-----RW7     RW8
                        |
                        RW9
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(4));
        add_children(RW.at(1), RW.at(5));
        add_children(RW.at(2), RW.at(6), RW.at(7));
        add_children(RW.at(3), RW.at(8));
        add_children(RW.at(7), RW.at(9));
        EXPECT_NODES_IN_TREE(RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_citerator citer;
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
 * @brief Remove some nodes in the middle of an iteration. 
 * Verify trees intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, SiblingIteratorRemoveSome)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1-----RW2---------RO0-----RO1-----RW3
        |       |           |       |
        RW4     RW5---RW6   RW7     RW8
        |
        RW11

        After:
        RW0                         RW1     RW3
        |                           |
        RW2---------RO0-----RO1     RW4
        |           |       |       |
        RW5---RW6   RW7     RW8     RW11
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RO.at(0), RO.at(1), RW.at(3));
        add_children(RW.at(1), RW.at(4));
        add_children(RW.at(2), RW.at(5), RW.at(6));
        add_children(RO.at(0), RW.at(7));
        add_children(RO.at(1), RW.at(8));
        add_children(RW.at(4), RW.at(11));

        /* Step 2: Action. */
        ecu_ntnode_sibling_iterator iter;
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(6), RW.at(2), RW.at(7), RO.at(0), RW.at(8), RO.at(1), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert. Verify removed trees intact. */
        EXPECT_NODES_IN_TREE(RW.at(11), RW.at(4), RW.at(1));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        EXPECT_NODES_IN_TREE(RW.at(3));
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
 * @brief Remove all nodes in the iteration. Verify
 * trees intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, SiblingIteratorRemoveAll)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---RW2---RW3
        |
        RW4---RW5---RW6-----RW7
        |           |       |
        RW8         RW9     RW10---RW11

        After:
        RW0                 RW4     RW6     RW7
        |                   |       |       |
        RW1---RW2---RW3     RW8     RW9     RW10---RW11
        |
        RW5
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), RW.at(4), RW.at(5), RW.at(6), RW.at(7));
        add_children(RW.at(4), RW.at(8));
        add_children(RW.at(6), RW.at(9));
        add_children(RW.at(7), RW.at(10), RW.at(11));

        /* Step 2: Action. */
        ecu_ntnode_sibling_iterator iter;
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(5))
        {
            convert(n).accept(node_remove_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(5), RW.at(1), RW.at(2), RW.at(3), RW.at(0));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(0))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }

        /* Step 3: Assert. Verify removed trees intact. */
        EXPECT_NODES_IN_TREE(RW.at(8), RW.at(4));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(4))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        EXPECT_NODES_IN_TREE(RW.at(5));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(5))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        EXPECT_NODES_IN_TREE(RW.at(9), RW.at(6));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(6))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        EXPECT_NODES_IN_TREE(RW.at(10), RW.at(11), RW.at(7));
        ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &RW.at(7))
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
 * @brief Destroy some nodes in the middle of an iteration. 
 * Verify remaining tree intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, SiblingIteratorDestroySome)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        DN0-----RW1---------DN1-----DN2
        |       |           |       |
        RW2     RW3---RW4   RW5     RW6
        |
        RW7

        After:
        RW0
        |
        RW1---------DN2
        |           |
        RW3---RW4   RW6
        */
        add_children(RW.at(0), DN.at(0), RW.at(1), DN.at(1), DN.at(2));
        add_children(DN.at(0), RW.at(2));
        add_children(RW.at(1), RW.at(3), RW.at(4));
        add_children(DN.at(1), RW.at(5));
        add_children(DN.at(2), RW.at(6));
        add_children(RW.at(2), RW.at(7));
        EXPECT_NODES_DESTROYED(DN.at(0), RW.at(2), RW.at(7), DN.at(1), RW.at(5));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_iterator iter;
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &DN.at(2))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(3), RW.at(4), RW.at(1), RW.at(6), DN.at(2), RW.at(0));
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
 * @brief Destroy all nodes in the iteration. Verify
 * remaining tree intact.
 * @warning Test validation requires working postorder iterator.
 */
TEST(NtNode, SiblingIteratorDestroyAll)
{
    try
    {
        /* Step 1: Arrange.

        Before:
        RW0
        |
        RW1---RW2---RW3
        |
        DN0---DN1---DN2-----DN3
        |           |       |
        RW4         RW5     RW6---RW7

        After:
        RW0
        |
        RW1---RW2---RW3
        |
        DN0
        |
        RW4
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        add_children(RW.at(1), DN.at(0), DN.at(1), DN.at(2), DN.at(3));
        add_children(DN.at(0), RW.at(4));
        add_children(DN.at(2), RW.at(5));
        add_children(DN.at(3), RW.at(6), RW.at(7));
        EXPECT_NODES_DESTROYED(DN.at(1), DN.at(2), RW.at(5), DN.at(3), RW.at(6), RW.at(7));

        /* Steps 2 and 3: Action and assert. */
        ecu_ntnode_sibling_iterator iter;
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &DN.at(0))
        {
            convert(n).accept(node_destroy_visitor);
        }

        /* Step 3: Assert. Verify remaining tree intact. */
        ecu_ntnode_postorder_citerator citer;
        EXPECT_NODES_IN_TREE(RW.at(4), DN.at(0), RW.at(1), RW.at(2), RW.at(3), RW.at(0));
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
 * @brief Calling next() after an iteration is finished
 * is not allowed.
 */
TEST(NtNode, SiblingIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_sibling_iterator iter;
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &empty_root)
        {
            (void)n;
        }
        (void)ecu_ntnode_sibling_iterator_next(&iter);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, ConstSiblingIteratorNextAfterDone)
{
    try
    {
        /* Step 1: Arrange. */
        rw_ntnode empty_root;
        EXPECT_ASSERTION();

        /* Step 2: Action. */
        ecu_ntnode_sibling_citerator citer;
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &empty_root)
        {
            (void)n;
        }
        (void)ecu_ntnode_sibling_iterator_cnext(&citer);

        /* Step 3: Assert. Test fails if assertion does not fire. */
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
TEST(NtNode, SiblingIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(3), RW.at(3), RW.at(1), RW.at(1), RW.at(2));
        ecu_ntnode_sibling_iterator iter;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_SIBLING_FOR_EACH(n, &iter, &RW.at(3))
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
TEST(NtNode, ConstSiblingIteratorMultipleTimes)
{
    try
    {
        /* Step 1: Arrange.
        RW0
        |
        RW1---RW2---RW3
        */
        add_children(RW.at(0), RW.at(1), RW.at(2), RW.at(3));
        EXPECT_NODES_IN_TREE(RW.at(2), RW.at(3), RW.at(3), RW.at(1), RW.at(1), RW.at(2));
        ecu_ntnode_sibling_citerator citer;

        /* Steps 2 and 3: Action and assert. */
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(1))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(2))
        {
            convert(n).accept(node_obj_in_tree_visitor);
        }
        ECU_NTNODE_CONST_SIBLING_FOR_EACH(n, &citer, &RW.at(3))
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
