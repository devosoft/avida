#ifndef COUNT_TRACKER_HH
#include "cCountTracker.h"
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

class cTestCounting : public cTestCase {
public: virtual void test() {
  int i;
  cCountTracker ct;

  for(i=0;i<5;i++) ct.Inc();
  test_is_true(ct.GetCur() == 5);
  test_is_true(ct.GetLast() == 0);
  test_is_true(ct.GetTotal() == 5);

  for(i=0;i<4;i++) ct.Dec();
  test_is_true(ct.GetCur() == 1);
  test_is_true(ct.GetLast() == 0);
  test_is_true(ct.GetTotal() == 5);

  ct.Next();
  test_is_true(ct.GetCur() == 0);
  test_is_true(ct.GetLast() == 1);
  test_is_true(ct.GetTotal() == 5);

  for(i=0;i<5;i++) ct.Inc();
  test_is_true(ct.GetCur() == 5);
  test_is_true(ct.GetLast() == 1);
  test_is_true(ct.GetTotal() == 10);

  ct.Clear();
  test_is_true(ct.GetCur() == 0);
  test_is_true(ct.GetLast() == 0);
  test_is_true(ct.GetTotal() == 0);
} };

class cTestDefaultConstructor : public cTestCase {
public: virtual void test() {
  cCountTracker ct;
  test_is_true(ct.GetCur() == 0);
  test_is_true(ct.GetLast() == 0);
  test_is_true(ct.GetTotal() == 0);
} };


class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cTestCounting); 
  adoptTestCase(new cTestDefaultConstructor); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cCountTracker." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
