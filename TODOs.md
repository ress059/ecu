Completed.
1. asserter.h/.c DONE.
2. attributes.h. DONE.
3. circular_dll.h/.c. Tests done. TODO new features and documentation.
4. endian.h. DONE.
5. event.h/.c TODO Documentation.
6. fsm.h/.c. Tests done. TODO - Possibly new features. TODO Documentation.
7. hsm.h/.c. TODO.
8. object_id.h/.c. TODO documentation.
9. timer.h/.c done. Tests done. TODO Documentation.
10. tree.h/.c. Tests done. TODO Code cleanup and Documentation.


## Ring buffer
1. Add ring buffer module (currently stashed). Add tests and documentation.


## FSM
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


## Circular DLL
1. Allow node to be added/removed anywhere in the list...that's the point of a linked list...
Currently only support push_back(). Add tests for this.

2. Remove separate structure for list and node (similar to linux where node is a list). I.e.
```C
/* Get rid of this */
struct ecu_circular_dll
{
};

/* Only have this. */
struct ecu_circular_dll_node
{
}
```

3. Change names from ecu_circular_dll to just ecu_dll. Applies to everything (file name,
struct names, function names, etc).

4. In ECU_CIRCULAR_DLL_GET_ENTRY() macro call, getting warning about how cast from char*
to struct ecu_timer* increases alignment requirements (when cross-compiling for 32-bit ARM). 
THIS IS OK. Maybe add GCC pragmas to get rid of warning? Wrap this in an #ifdef GCC macro
so this only applies to GCC. I.e.
```C
#ifdef GCC
#pragma GCC -wnocast-align //whatever syntax to ignore wcast-align warnings
#define ECU_CIRCULAR_DLL_GET_ENTRY() ....
#end pragma
#endif
```


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

2. NULL check all parameters!!!!!!! You cant assume the node_valid() function will NULL check. It can change!!
3. Write tests for ecu_tree_node_check_object_id() and ecu_tree_node_get_object_id().
4. Finished (done!) tree.c cleanup and code verification (asserts, style, etc).
5. Finished (done!) destructor tests and tests for adding nodes in middle of iteration. 
6. Just need to do cleanup and documentation. Stopped at ecu_tree_remove_node() function.


## Build system and syntax
0. Add build tests (executables) for different hardware targets. stm32l0, stm32l3, etc.
Need linker scripts for each target. Toolchain files will be for cm0, cm4, etc.

1. Print "cannot use endian.h" warning only at build-time NOT configuration time.
Otherwise message is always printed
- Think I may need to dereference ${CMAKE_C_BYTE_ORDER} in if-else statement?
- Note the if() elseif() stuff is evaluated at configuration time.

2. When using ecu in external project, setting ecu to c_std_23 does not use static_assert()??? 
It uses the extern char array[]???? Maybe cause it's passing -std=gnu2x? Look into this...


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
1. Possible include file comments about PRIVATE members for restof source files (like the description in circular_dll.h).
