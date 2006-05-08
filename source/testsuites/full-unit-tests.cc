#include "cDataEntry.h"
#include "cFile.h"
#include "cInitFile.h"
#include "cRandom.h"
#include "cString.h"
#include "cTemplateTests.h"

#include <boost/detail/lightweight_test.hpp>

int main() {

  cDataEntry::UnitTests(true);
  cFile::UnitTests(true);
  nInitFile::UnitTests(true);
  cRandom::UnitTests(true);
  cString::UnitTests(true);
  cStringList::UnitTests(true);
  cTemplateTests::UnitTests(true);

  return boost::report_errors();
}

