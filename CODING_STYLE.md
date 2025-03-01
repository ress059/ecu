# Naming Conventions

1. Macros, defines, and enum members in all uppercase with underscores. Everything else lowercase with underscores (snake case). Example:
```C
#define MY_MACRO_IS_ALL_UPPERCASE

void my_function_is_all_lowercase(void);

int my_variable_is_all_lowercase;
```

2. Anything in the global namespace must be prefixed with "ecu_" to make it obvious in larger apps that the type/function/whatever comes from this library. Example:
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
1. Include paths for publicly-exposed headers will always begin with ecu in order for larger apps to have scope for include files. For example "ecu/asserter.h", "ecu/fsm.h", etc.

# Doxygen Style comments
1. @brief and @details directives never separated by a line. Example:
```C
/**
 * @brief These should never be separated by a line.
 * @details
 */
```

2. File comments follow this exact syntax. No extra line breaks, doxygen commands, etc. Only other doxygen commands
allowed are @warning and @code
```C
/**
 * @file
 * @brief Notice @details directive is never used.
 * 
 * @author Your Name
 * @version 0.1
 * @date 2024-04-07
 * @copyright Copyright (c) 2024
 */
```

# Other
1. Same typedef rules as linux kernel. Essentially no typedefs unless absolutely necessary.