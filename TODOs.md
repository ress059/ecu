Completed.
1. asserter.h/.c TODO.
2. event.h/.c and object_id.hs/.c done. Documentation done.
3. circular_dll.h/.c done. Tests done. Don't need to test 2 nodes in same struct that are apart of separate lists
since we are already doing that with tests that use extra_list; Documentation done.
4. timer.h/.c done. Tests done. Documentation done.
5. endian.h. Tests done. Documentation done.
6. fsm.h/.c. Tests done. Documentation done.
7. tree.h/.c. TODO. Tests done.
8. hsm.h/.c. TODO.
9. attributes.h. done. Documentation done.

Ring Buffer:
1. Add tests and documentation. Stashing away for now.

Tree:
- NULL check all parameters!!!!!!! You cant assume the node_valid() function will NULL check. It can change!!
- Write tests for ecu_tree_node_check_object_id() and ecu_tree_node_get_object_id().
1. Finished tree.c cleanup and code verification (asserts, style, etc).
2. Finished destructor tests and tests for adding nodes in middle of iteration. 
3. Just need to do cleanup and documentation. Stopped at ecu_tree_remove_node() function.

Asserter:
0. Update unit tests with new asserter.
1. Finish doxygen and sphinx documentation.

All:
1. Possible include file comments about PRIVATE members for restof source files (like the description in circular_dll.h).

CI Pipeline Steps:
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







# TODOs from Library Testing/Usage
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

## Circular DLL and tree
1. In ECU_CIRCULAR_DLL_GET_ENTRY() macro call, getting warning about how cast from char*
to struct ecu_timer* increases alignment requirements. THIS IS OK.
Maybe add GCC pragmas to get rid of warning? Wrap this in an #ifdef GCC macro
so this only applies to GCC. I.e.
```C
#ifdef GCC
#pragma GCC -wnocast-align //whatever syntax to ignore wcast-align warnings
#define ECU_CIRCULAR_DLL_GET_ENTRY() ....
#end pragma
#endif
```


## Asserter
2. Make set_assert_functor() function that sets the assert handler of all modules. I.e.
```C
void set_assert_functor(functor)
{
    ecu_timer_set_assert_functor(functor);
    ecu_fsm_set_assert_functor(functor);
    ecu_circular_dll_set_assert_functor(functor);
    ....
}
```

3. Figure out scheme for passing file name into assert handler without using __FILE__ macro for each assert check call.


## Build system and syntax
0. Add build tests (executables) for different hardware targets. stm32l0, stm32l3, etc.
Need linker scripts for each target. Toolchain files will be for cm0, cm4, etc.

1. Print "cannot use endian.h" warning only at build-time NOT configuration time.
Otherwise message is always printed
- Think I may need to dereference ${CMAKE_C_BYTE_ORDER} in if-else statement?
- Note the if() elseif() stuff is evaluated at configuration time.

2. Change all ecu includes from <> to "".
I.e. #include <ecu/asserter.h> to #include "ecu/asserter.h"

3. When using ecu in external project, setting ecu to c_std_23 does not use static_assert()??? 
It uses the extern char array[]???? Maybe cause it's passing -std=gnu2x? Look into this...
