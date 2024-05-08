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
/*--------------------------------------------------- STUB AND MOCK DECLARATIONS --------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/



// add. edge cases: 
//  1. parent node not in a tree.
//  2. node already in tree.
//  3. adding to parent that does not have any children.

// remove. edge cases:
//  1. node not in tree.
//  2. node is the only child of parent.
//  3. Parent has multiple children and node is the last child in list.

// test level. add/remove some nodes and keep testing level. Similar to dll test size.

// test lca. edge cases: two nodes are the same. two nodes are in different trees. one node is the root and one is another.

// children and postorder iterator.
//  1. Basic iteration (static tree)
//  2. Remove nodes in the middle of the iteration.
