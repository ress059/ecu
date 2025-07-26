Completed.
1. asserter.h/.c TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc).
2. attributes.h. TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc).
3. dlist.h/.c. DONE.
4. endian.h. TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc).
5. event.h/.c TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc). Keep. In sphinx docs show example of creating event base class for fsm/hsm modules.
6. fsm.h/.c. TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc).
7. hsm.h/.c. TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc).
8. object_id.h/.c. TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc).
9. timer.h/.c. TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc).
10. ntree.h/.c. TODO. Alphabetical order (functions, tests, and sphinx docs), doxygen comments in tests, refactor sphinx (same headers - Theory, Member Functions, etc), finish tests, finish sphinx docs.
11. utils.h. DONE.

## Ring buffer
1. Add ring buffer module (currently stashed). Add tests and documentation.


# ALL
1. Public functions now in alphabetical order. Edit code and documentation.
2. Expose valid() functions publically so they can be used by other modules.
3. Remove @details directive for **everything**.


## Tree
1b. ntree.c pretty much done. All node_valid() asserts and correct paths done.
2. Tests and documentation.

4. Trying to encapsulate concrete iterator declaration inside FOR_EACH() macro declaration. 
But can't find a good way to do it. I.e. 
```C
#define ECU_NTNODE_CHILD_FOR_EACH(var_, iter_, parent_)                             \
    struct ecu_ntnode_child_iterator iter_;                                         \
    for (struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_begin(&iter_, parent_); \
         var_ != ecu_ntnode_child_iterator_end(&iter_);                              \
         var_ = ecu_ntnode_child_iterator_next(&iter_))
```

5. **Probably rename to ntnode.h**

This will break down if user uses multiple for-loops with same iter_ name. I.e.
```C
// RESULTS IN ERROR
ECU_NTNODE_CHILD_FOR_EACH(n, iter_, parent_)
{

}

ECU_NTNODE_CHILD_FOR_EACH(n, iter_, parent_)
{

}
// ...

struct ecu_ntnode_child_iterator iter_;
for (struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_begin(&iter_, parent_);
        var_ != ecu_ntnode_child_iterator_end(&iter_);
        var_ = ecu_ntnode_child_iterator_next(&iter_))
{
    // stuff
}

struct ecu_ntnode_child_iterator iter_;   // ERROR
for (struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_begin(&iter_, parent_);
        var_ != ecu_ntnode_child_iterator_end(&iter_);
        var_ = ecu_ntnode_child_iterator_next(&iter_))
{
    // stuff
}
```

I cannot put in block-scope because {} brackets needed after for-loop. I.e. can't do:
```C
#define ECU_NTNODE_CHILD_FOR_EACH(var_, iter_, parent_)                             \
{                                                                                   \
    struct ecu_ntnode_child_iterator iter_;                                         \
    for (struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_begin(&iter_, parent_); \
         var_ != ecu_ntnode_child_iterator_end(&iter_);                              \
         var_ = ecu_ntnode_child_iterator_next(&iter_))                                 \
}
```

And obviously can't declare variables of different types in same for-loop like so:
```C
#define ECU_NTNODE_CHILD_FOR_EACH(var_, iter_, parent_)                             \
    for (struct ecu_ntnode_child_iterator iter_,                                    \
         struct ecu_ntnode *var_ = ecu_ntnode_child_iterator_begin(&iter_, parent_); \
         var_ != ecu_ntnode_child_iterator_end(&iter_);                              \
         var_ = ecu_ntnode_child_iterator_next(&iter_))
```

5. Stuff decided on:
    a. me->parent == me if no parent.
    b. parent function returns null if no parent.
    c. next functions return null if no siblings or is last sibling.
    d. prev functions return null if no siblings or is first sibling.
    e. Root is iterated over for preorder, postorder iterations.
    f. Start node is not iterated over in parent iterator.
    g. Start node is not iterated over in sibling iterator. Otherwise code can not figure out when to terminate.
    h. No separate structure for tree/root. Just nodes.
    
6. Maybe make a ECU_DLIST_SIBLING_AT_FOR_EACH() iterator. Iterates over siblings until end (rightmost) reached.

## Unit Tests
1. Prefix all **helper** class members with m_. TEST_GROUP classes do not have to follow this. 
Example: 
```C
TEST_GROUP(TestClass)
{
    int a;
};

class helper_class
{
    int m_a;
};
```
2. In all unit tests do catch a const exception instead of nonconst exception.
I.e. catch (const AssertException&) instead of catch (AssertException&)


## Build system and syntax, CI, etc
0. Still need to clang-format fsm, hsm, timer, and tree.

1. Add doxygen, doxysphinx, and sphinx documentation build steps to CMake. Fail build
if any error codes returned. !!!in readthedocs.yaml return error code 183 if any doxygen
or doxysphinx warnings/errors occur!!!.

2. Add clang-format to CMake build tests. Do not auto-format the code. Instead return
error if diff is different before and after formatting.

3. Add build tests (executables) for different hardware targets. stm32l0, stm32l3, etc.
Need linker scripts for each target. Toolchain files will be for cm0, cm4, etc.

4. Print "cannot use endian.h" warning only at build-time NOT configuration time.
Otherwise message is always printed
- Think I may need to dereference ${CMAKE_C_BYTE_ORDER} in if-else statement?
- Note the if() elseif() stuff is evaluated at configuration time.

5. When using ecu in external project, setting ecu to c_std_23 does not use static_assert()??? 
It uses the extern char array[]???? Maybe cause it's passing -std=gnu2x? Look into this...

6. Use #pragma message() instead of #warning for TODO messages. I.e.
```C
#pragma message("TODO: Want a timer reset capability without coupling it to tlist. \
    Reset = stop timer but do not restart its timer. When its readded it counts down \
    from its old saved value.")
```

# Clang-format
1. One line max now.
2. Braces (if, while, for, etc).
3. Pointer location. I.e. void* vs void *
4. Limit PRIVATE documentation to just on struct documentation and put it in warning, 
Don't do on individual members. Use one-line doxygen syntax for struct members. I.e.
```C
/**
 * @brief Single node within list. User-defined 
 * nodes contain this object.
 * 
 * @warning PRIVATE. Unless otherwise specified, all 
 * members can only be edited via the public API.
 */
struct ecu_dnode
{
    /// @brief Next node in list.
    struct ecu_dnode *next;

    /// @brief Previous node in list.
    struct ecu_dnode *prev;

    /// @brief Optional user-defined node destructor. Executes 
    /// when @ref ecu_dlist_destroy() or @ref ecu_dnode_destroy() 
    /// are called.
    void (*destroy)(struct ecu_dnode *me, ecu_object_id id);

    /// @brief Optional node ID. Helps user identify 
    /// different types stored in the same list.
    ecu_object_id id;
};
```


## CI Pipeline Steps:
If any step fails do not move on.
1. format check. clang-format.
2. Compile. cmake and cmake --build
- cmake -DECU_DISABLE_RUNTIME_ASSERTS=OFF --preset linux-gnu-build // for runtime asserts enabled
- cmake --build --preset linux-gnu-build

- cmake -DECU_DISABLE_RUNTIME_ASSERTS=ON --preset linux-gnu-build // for runtime asserts disabled
- cmake --build --preset linux-gnu-build
3. run unit tests.
4. generate documentation. doxygen
