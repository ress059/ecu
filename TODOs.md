Completed.
1. asserter.h/.c DONE. TODO clang-format
2. attributes.h. DONE. TODO clang-format
3. dlist.h/.c. DONE. TODO proofread docs. TODO clang-format
4. endian.h. DONE. TODO clang-format
5. event.h/.c TODO Documentation.
6. fsm.h/.c. Tests done. TODO - Possibly new features. TODO Documentation.
7. hsm.h/.c. TODO.
8. object_id.h/.c. TODO documentation.
9. timer.h/.c done. Tests done. TODO Documentation.
10. tree.h/.c. Tests done. TODO Code cleanup and Documentation.


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

# Timer
0. Update to use new dlist.h API.
1. When adding a new timer, order list nodes by timeout ticks (timer closest to timing
out is at HEAD). This way ecu_timer_collection_tick() only has to check HEAD instead of
iterating through entire list.
2. When applicable use the ECU_DLIST_FOR_EACH() macros intead of manually iterating
through list.


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
1. Change all class members from _ to m_. Example change event_; to m_event;
2. In all unit tests do catch a const exception instead of nonconst exception.
I.e. catch (const AssertException&) instead of catch (AssertException&)


## Build system and syntax
0. Add build tests (executables) for different hardware targets. stm32l0, stm32l3, etc.
Need linker scripts for each target. Toolchain files will be for cm0, cm4, etc.

1. Print "cannot use endian.h" warning only at build-time NOT configuration time.
Otherwise message is always printed
- Think I may need to dereference ${CMAKE_C_BYTE_ORDER} in if-else statement?
- Note the if() elseif() stuff is evaluated at configuration time.

2. When using ecu in external project, setting ecu to c_std_23 does not use static_assert()??? 
It uses the extern char array[]???? Maybe cause it's passing -std=gnu2x? Look into this...

# Clang-format
1. One line max now.
2. Braces (if, while, for, etc).
3. Pointer location. I.e. void* vs void *


## CI Pipeline Steps:
If any step fails do not move on.
1. format check. clang-format.
2. Compile. cmake and cmake --build
- cmake -DECU_DISABLE_RUNTIME_ASSERTS=OFF --preset linux-gnu-build // for runtime asserts enabled
- cmake --build --preset linux-gnu-build

- cmake -DECU_DISABLE_RUNTIME_ASSERTS=ON --preset linux-gnu-build // for runtime asserts disabled
- cmake --build --preset linux-gnu-build

3. static analysis. clang-tidy
4. run unit tests.
5. generate documentation. doxygen


## All:
0. All declarations are max 1 line now. I.e.
extern void foo(void);
// only one line in between.
extern void bar(void);

1. Possible include file comments about PRIVATE members for restof source files (like the description in circular_dll.h).
2. Remove all @ref file in doxygen comments. I.e. "See @ref endian.h for more details", "see @ref circular_dll.h for more details", etc.
