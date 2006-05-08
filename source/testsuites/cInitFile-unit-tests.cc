#include "cDataEntry.h"
#include "cFile.h"
#include "cInitFile.h"
#include "cRandom.h"
#include "cString.h"
#include "cTemplateTests.h"

#include <boost/detail/lightweight_test.hpp>

int main() {

  nInitFile::UnitTests(true);

  return boost::report_errors();
}

