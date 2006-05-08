#include "cDataEntry.h"
#include "cFile.h"
#include "cInitFile.h"
#include "cRandom.h"
#include "cString.h"
#include "cTemplateTests.h"

#include <boost/detail/lightweight_test.hpp>

int main() {

  nDataEntry::UnitTests(true);
  nFile::UnitTests(true);
  nInitFile::UnitTests(true);
  nRandom::UnitTests(true);
  cString::UnitTests(true);
  cStringList::UnitTests(true);
  cTemplateTests::UnitTests(true);

  return boost::report_errors();
}

