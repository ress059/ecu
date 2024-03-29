```C
/* ------------------------------------- TEST_GROUP_BASE() macro ---------------------------- */
// Macro definition
#define TEST_GROUP_BASE(testGroup, baseclass) \
  extern int externTestGroup##testGroup; \
  int externTestGroup##testGroup = 0; \
  struct TEST_GROUP_##CppUTestGroup##testGroup : public baseclass

// Example use:
TEST_GROUP_BASE(MyGroup1, default_mock_asserter)
{
    void setup()
    {
        // setup stuff
    }

    void teardown()
    {
        // teardown stuff
    }
};

// Expands to:
extern int externTestGroupMyGroup1;
int externTestGroupMyGroup1 = 0;
struct TEST_GROUP_CppUTestGroupMyGroup1 : public default_mock_asserter
{
    void setup() // will override whatever setup you put in default_mock_asserter
    {
        // setup stuff
    }

    void teardown() // will override whatever teardown you put in default_mock_asserter
    {
        // teardown stuff
    }
};



/* -------------------------------------------------- TEST_GROUP() macro ---------------------------- */
// Macro definition
#define TEST_GROUP(testGroup) \
  TEST_GROUP_BASE(testGroup, Utest)

//Example use:
TEST_GROUP(MyGroup2)
{
    void setup()
    {
        // setup stuff
    }

    void teardown()
    {
        // teardown stuff
    }
}


// Expands to:
extern int externTestGroupMyGroup2;
int externTestGroupMyGroup2 = 0;
struct TEST_GROUP_CppUTestGroupMyGroup2 : public Utest
{
    void setup() // will override setup in Utest
    {
        // setup stuff
    }

    void teardown() // will override teardown in Utest
    {
        // teardown stuff
    }
};


/* -------------------------------------------------- TEST() macro ---------------------------- */
// Macro definition
#define TEST(testGroup, testName) \
  /* External declarations for strict compilers */ \
  class TEST_##testGroup##_##testName##_TestShell; \
  extern TEST_##testGroup##_##testName##_TestShell TEST_##testGroup##_##testName##_TestShell_instance; \
  \
  class TEST_##testGroup##_##testName##_Test : public TEST_GROUP_##CppUTestGroup##testGroup \
{ public: TEST_##testGroup##_##testName##_Test () : TEST_GROUP_##CppUTestGroup##testGroup () {} \
       void testBody() _override; }; \
  class TEST_##testGroup##_##testName##_TestShell : public UtestShell { \
      virtual Utest* createTest() _override { return new TEST_##testGroup##_##testName##_Test; } \
  } TEST_##testGroup##_##testName##_TestShell_instance; \
  static TestInstaller TEST_##testGroup##_##testName##_Installer(TEST_##testGroup##_##testName##_TestShell_instance, #testGroup, #testName, __FILE__,__LINE__); \
    void TEST_##testGroup##_##testName##_Test::testBody()


// Example:
TEST(MyGroup1, MyTest1)
{
    //test stuff
}

// Expands to:
class TEST_MyGroup1_MyTest1_TestShell;
extern TEST_MyGroup1_MyTest1_TestShell TEST_MyGroup1_MyTest1_TestShell_instance;

class TEST_MyGroup1_MyTest1_Test : public TEST_GROUP_CppUTestGroupMyGroup1 
{
public:
    TEST_MyGroup1_MyTest1_Test () : TEST_GROUP_CppUTestGroup_MyGroup1 ()
    {

    }

    void testBody() override;
};

class TEST_MyGroup1_MyTest1_TestShell : public UTestShell 
{
    virtual Utest *createTest() override 
    {
        return new TEST_MyGroup1_MyTest1_Test;
    }
} TEST_MyGroup1_MyTest1_TestShell_instance;

static TestInstaller TEST_MyGroup1_MyTest1_Installer(TEST_MyGroup1_MyTest1_TestShell_instance, MyGroup1, MyTest1, __FILE__, __LINE__);
void TEST_MyGroup1_MyTest1_Test::testBody()
{
    //test stuff
}
```