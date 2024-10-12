

#include "ecu/asserter.h"


static void (*handler)(const char *file, int line) = (void (*)(const char *, int)) = assert_fail;


void ecu_assert_handler(const char *file, int line)
{
    (*handler)(file, line);
}


// in some header
template <typename F> 
void try_catch(F&& f)
{
    try
    {

        f();
    }
    catch (assert_exception) 
    { 
        /* */
    }
}


try_catch ([&](){ 
   task1();
   task2();
   task3();
});


TEST_FAIL_ON_ASSERT()       try_catch( )
TEST_OK_ON_ASSERT()
CUSTOM_BEHAVIOR_ON_ASSERT()



TEST_FAIL_ON_ASSERT()
{
    // enter code here
}