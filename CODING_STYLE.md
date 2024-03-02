# Naming Conventions

1. Macros, defines, and enum members in all uppercase with underscores. Everything else lowercase with underscores (snake case). Example:
```C
#define MY_MACRO_IS_ALL_UPPERCASE

void my_function_is_all_lowercase(void);

int my_variable_is_all_lowercase;
```

2. Anything publicly exposed with its own scope must be prefixed with "ecu_" to make it obvious in larger apps that the type/function/whatever comes from this library. Example:
```C
/***************** public_header.h *******************/
#define ECU_PUBLIC_MACRO

struct ecu_struct
{
    /* These should not be prefixed with "ecu_" because it is within
    ecu_struct scope. */
    int x;
    int y;
}

extern void ecu_public_function(void);

/**************** private_file.c *********************/
static void private_function(void);
```

3. Source code file names in all lowercase with underscores (snake case). Do not prefix file names with "ecu_" as public headers are required to prefix ecu into their include paths. See Include Paths section.


# Include Paths
1. Include paths for publicly-exposed headers will always begin with ecu in order for larger apps to have scope for include files. For example <ecu/asserter.h>, <ecu/fsm.h>, etc.


# Other
1. Same typedef rules as linux kernel. Essentially no typedefs unless absolutely necessary.