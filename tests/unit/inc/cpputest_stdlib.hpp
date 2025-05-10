/**
 * @file
 * @brief CppUTest overloads operator new so there may be conflicts when
 * using new. This header allows STDLib to be used without any conflicts 
 * with CppUTest. It is the same solution proposed in CppUTest's manual: 
 * https://cpputest.github.io/manual.html. See "Resolving conflicts with STL"
 * section. Note that only STDLib headers that use new need to be included
 * here. 
 * 
 * This file should not be included in code directly. The build system
 * automatically includes this file using the -include GCC flag.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-04-08
 * @copyright Copyright (c) 2025
 */

#ifndef CPPUTEST_STDLIB_HPP_
#define CPPUTEST_STDLIB_HPP_
 
/* STDLib headers that may use new. */
#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>
 
/* CppUTest. */
#include "CppUTest/MemoryLeakDetectorNewMacros.h"
 
#endif /* CPPUTEST_STDLIB_HPP_ */
