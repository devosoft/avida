/*
add CDATA designation so this shows up correctly in SGML documentation
<![CDATA[
*/

#include "test_case_point.h"
#include "cTextTestInterpreter.hh"

#include <iostream>

int
main( int ac, char**av )
{
  point_test_suite pnt_test;
  pnt_test.runTest();
  cTextTestInterpreter interpreter;
  
  std::cout << interpreter.interpretation( pnt_test.lastResult() );
}
/*
end CDATA designation
]]>
*/

