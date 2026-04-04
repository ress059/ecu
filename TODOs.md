Completed.
1. asserter.h/.c DONE.
2. attributes.h. DONE.
3. dlist.h/.c. DONE.
4. endian.h. DONE.
5. event.h/.c DONE.
6. fsm.h/.c. DONE.
7. hsm.h/.c. DONE.
8. object_id.h/.c. DONE.
9. timer.h/.c. DONE.
10. ntnode.h/.c. DONE.
11. utils.h. DONE.

## Ring buffer
1. Add ring buffer module (currently stashed). Add tests and documentation.


# ALL
2. Expose valid() functions publically so they can be used by other modules.
4. Removed hoverxref so must enable link previews in readthedocs.
Settings->Addons->Link previews in admin dashboard.

# FSM and HSM
1. Thinking about stripping away const qualifiers in ecu_fsm_state and ecu_hsm_state. The user can
decide if the states are const or not.
2. Add support for state machine inheritance in fsm and hsm modules.
3. Do simple examples in documentation. Make them short.


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
    
6. Maybe make a ECU_NTNODE_SIBLING_AT_FOR_EACH() iterator. Iterates over siblings until end (rightmost) reached.


## Build system and syntax, CI, etc
0. Still need to clang-format fsm, hsm, timer, and tree.

2. Add clang-format to CMake build tests. Do not auto-format the code. Instead return
error if diff is different before and after formatting.

3. Add build tests (executables) for different hardware targets. stm32l0, stm32l3, etc.
Need linker scripts for each target. Toolchain files will be for cm0, cm4, etc.


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
