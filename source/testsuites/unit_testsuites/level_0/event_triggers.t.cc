#ifndef EVENT_TRIGGERS_HH
#include "cEventTriggers.h"
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

class cTemplateTest : public cTestCase {
public: virtual void test() {
  // FIXME
  cout
  << "***" << endl
  << "FIXME:" << endl
  << "cEventTriggers can't be instantiated" << endl
  << "because it is a pure virtual base class;" << endl
  << "but it can be Mock-subclassed..." << endl
  << "try this out.  -- kgn" << endl
  << "***" << endl
  << "PS: cAvidaTriggers is an instantiable subclass." << endl
  << "***" << endl;
} };

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cTemplateTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cEventTriggers." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
