#ifndef PHENOTYPE_HH
#include "phenotype.hh"
#endif

#ifndef TEST_CASE_H
#include "third-party/yaktest/cTestCase.hh"
#endif
#ifndef TEXT_TEST_INTERPRETER_H
#include "third-party/yaktest/cTextTestInterpreter.hh"
#endif
#ifndef TEST_RESULT_H
#include "third-party/yaktest/cTestResult.hh"
#endif

#include <iostream>

using namespace std;
namespace nTemplateTest {
  class cTemplateTest : public cTestCase {
  public: virtual void test() {
      test_is_true(true);
  } };
  
  class cTestSuite : public cTestCase {
  public: cTestSuite() : cTestCase() {
    adoptTestCase(new cTemplateTest); 
  } };
}

using namespace nTemplateTest;
int main(int argc, char *argv[]){
  cout << endl << "Testing Template." << endl;
  int successes = 0, test_count = 0;
  cTestSuite inst_test;
  inst_test.runTest();
  successes += inst_test.lastResult().successCount();
  test_count += inst_test.lastResult().totalTestCount();
  cTextTestInterpreter interpreter;
  cout << interpreter.interpretation(inst_test.lastResult());
  return(successes != test_count);
}
