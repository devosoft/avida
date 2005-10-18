#ifndef FIXED_BLOCK_HH
#include "cFixedBlock.h"
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

class cFixedBlockTest : public cTestCase {
public: virtual void test() {
  cFixedBlock fb;
  test_is_true(fb.GetStart() == 0);
  fb.SetStart(5);
  test_is_true(fb.GetStart() == 5);
} };

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cFixedBlockTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cFixedBlock." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
