#warning "TODO: Update file description when run-time asserter is made"
/**
 * @file
 * @brief Compile-time and run-time assert macros.
 * @details Compile-time assert macros uses static_assert variants or symbol redeclaration to produce
 * compilation errors depending on which C/C++ standard is used when compiling.
 * @author Ian Ress
 */


/* Translation unit. */
#include <ecu/asserter.h>


/**
 * @brief Default assert handler that is assigned if user does not specify one.
 * Executes when a runtime assert fires.
 */
static void default_assert_handler(void)
{
    while(1)
    {
        /* Permanently hang to inspect call stack. */
    }
}

void (*My_Assert_Handler)(void) = &default_assert_handler;









void assert_handler(struct assert_functor *object, const char *file_name, uint32_t line)
{
    if (object)
    {
        if (object->handler)
        {
            (*object->handler)(object, file_name, line);
        }
    }
}
