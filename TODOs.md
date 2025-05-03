Completed.
1. asserter.h/.c DONE.
2. attributes.h. DONE.
3. dlist.h/.c. DONE.
4. endian.h. DONE.
5. event.h/.c DONE. TODO May delete this.
6. fsm.h/.c. TODO. Refactor, new tests, clang-format, documentation.
7. hsm.h/.c. TODO. Implmentation, tests, clang-format, documentation.
8. object_id.h/.c. DONE.
9. timer.h/.c. DONE.
10. tree.h/.c. Tests done. TODO Code cleanup and Documentation. May refactor.

## Ring buffer
1. Add ring buffer module (currently stashed). Add tests and documentation.


## FSM
1. Make helper macro that allows users to static assert that ecu_fsm is
the first member of their fsm.
2. **Test calling ecu_fsm_dispatch() within an fsm state handler**.

1. Should entry and exit handlers both take in an event? Reasoning is in
case fsm should be updated with any event data on entry/exit.
I.e. 
```C
// Should I do this instead?
entry_handler(struct ecu_fsm* const struct ecu_event*);
exit_handler(struct ecu_fsm* const struct ecu_event*);

// current.
exit_handler(struct ecu_fsm*);
exit_handler(struct ecu_fsm*);
```

2. Should ecu_event passed into fsm be non-const? Think const is fine for now?
I.e.
```C
// Should I do this instead?
ecu_fsm_dispatch(struct ecu_fsm *, struct ecu_event *);

// current.
ecu_fsm_dispatch(struct ecu_fsm *, const struct ecu_event *);
```

3. Create compile-time state constructors for fsm. Obviously won't be backwards compatible
if any parameters have to change but that is the same case for run-time functions. I.e.
```C
#define ECU_FSM_STATE_CTOR_COMPILETIME(state, entry_handler, exit_handler, state_handler) ....
```


## DList
1. In ECU_DNODE_GET_ENTRY() macro call, getting warning about how cast from char*
to struct ecu_timer* increases alignment requirements (when cross-compiling for 32-bit ARM). 
THIS IS OK. Maybe add GCC pragmas to get rid of warning? Wrap this in an #ifdef GCC macro
so this only applies to GCC. I.e.
```C
#ifdef GCC
#pragma GCC -wnocast-align //whatever syntax to ignore wcast-align warnings
#define ECU_DNODE_GET_ENTRY() ....
#end pragma
#endif
```

2. Refactor EXPECT_NODE_IN_LIST() to be a varidic template if you have time.

# Timer
1. When timer and FSM done, use it in main.c build test to verify linkage.


## Tree
1. In ECU_TREE_NODE_GET_ENTRY() macro call, getting warning about how cast from char*
to struct ecu_timer* increases alignment requirements (when cross-compiling for 32-bit ARM). 
THIS IS OK. Maybe add GCC pragmas to get rid of warning? Wrap this in an #ifdef GCC macro
so this only applies to GCC. I.e.
```C
#ifdef GCC
#pragma GCC -wnocast-align //whatever syntax to ignore wcast-align warnings
#define ECU_TREE_NODE_GET_ENTRY() ....
#end pragma
#endif
```
2. Add add_sibling_left() and add_sibling_right() function. 
3. NULL check all parameters!!!!!!! You cant assume the node_valid() function will NULL check. It can change!!
4. Write tests for ecu_tree_node_check_object_id() and ecu_tree_node_get_object_id().
5. Finished (done!) tree.c cleanup and code verification (asserts, style, etc).
6. Finished (done!) destructor tests and tests for adding nodes in middle of iteration. 
7. Just need to do cleanup and documentation. Stopped at ecu_tree_remove_node() function.


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
