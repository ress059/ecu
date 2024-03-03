#warning "TODO: Set up linker in Linux-GNU.cmake. Have to comment out for now since link command doesn't work."
/**
 * @file 
 * @author Ian Ress
 * @brief See @ref asserter.h
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */


/* Translation unit. */
#include <ecu/asserter.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- FILE-SCOPE VARIABLES ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void(*user_handler)(const char *, int) = (void(*)(const char *, int))0;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- STATIC FUNCTION DECLARATIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void default_assert_handler(const char *file, int line);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- STATIC FUNCTION DEFINITIONS -------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

static void default_assert_handler(const char *file, int line)
{
    (void)file;
    (void)line;

#if !defined(NDEBUG)
    while (1)
    {
        /* Permanently hang to inspect call stack. */
    }
#endif
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- PUBLIC FUNCTIONS --------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void ecu_assert_do_not_use(const char *file, int line)
{
    if (user_handler) /* user-defined handler. */
    {
        (*user_handler)(file, line);
    }
    else /* default handler. */
    {
        default_assert_handler(file, line);
    }
}


void ecu_asserter_set_handler(void (*handler)(const char *file_name, int line))
{
    /* NULL handler means the default assert handler will now run. */
    user_handler = handler;
}
