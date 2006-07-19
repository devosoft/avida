
using namespace std;

/*
cUnitTestDemo Declarations
*/
class cUnitTestDemo {
public:
  bool blah();
};

/*
Declare a function "UnitTests" that runs the testsuite for
cUnitTestDemo. It lives in a special namespace "nUnitTestDemo" named
after cUnitTestDemo.
*/
namespace nUnitTestDemo {
  void UnitTests(bool full = false);
}

/*
cUnitTestDemo Definitions
*/
bool cUnitTestDemo::blah(){
  return false;
}

/* Include the unit-testing header file. */
#include "lightweight_test.h"

#include <iostream>
using namespace std;

/*
Define a testsuite for cUnitTestDemo. It consists of a bunch of tests,
each in its own namespace, and a function "UnitTests" to run all of the
tests.
*/
namespace nUnitTestDemo {
  /* A unit test. */
  namespace ut_hello_world {
    void test(){
      /*
      CURRENT_FUNCTION is part of lightweight_test.h. It expands to a
      string containing the name of this function.
      */
      cout << CURRENT_FUNCTION << endl;
      /*
      'TEST()' is used in the same way as 'assert()', except that on a
      failure, it doesn't abort. Instead it tells you about the failure,
      including the line number and file name.
      */
      /* This test should pass... */
      TEST(true);
      /* This test should fail... */
      TEST(false);
      cout << "finished " << CURRENT_FUNCTION << endl;
    }
  }
  /* Another unit test. */
  namespace ut_blah {
    void test(){
      cout << CURRENT_FUNCTION << endl;
      cUnitTestDemo d;
      TEST(!d.blah());
      cout << "finished " << CURRENT_FUNCTION << endl;
    }
  }
  /* A function to run all of the above tests. */
  void UnitTests(bool full)
  {
    if(full) ut_hello_world::test();
    if(full) ut_blah::test();
  }
}

/*
Calls "UnitTests" for "cUnitTestDemo". If you had more classes to tests,
each would have its own "UnitTests", and they would be called here.
*/
int main(){
  /* Run all of the unit tests for cUnitTestDemo. */
  nUnitTestDemo::UnitTests(true);
  return report_errors();
}
