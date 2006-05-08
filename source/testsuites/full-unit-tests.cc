#include "cDataEntry.h"
#include "cFile.h"
#include "cInitFile.h"
#include "cRandom.h"
#include "cString.h"
#include "nTemplateTests.h"

#include <boost/detail/lightweight_test.hpp>

int main() {

  nDataEntry::UnitTests(true);
  nFile::UnitTests(true);
  nInitFile::UnitTests(true);
  nRandom::UnitTests(true);
  nString::UnitTests(true);
  nStringList::UnitTests(true);
  nTemplateTests::UnitTests(true);

  return boost::report_errors();
}

