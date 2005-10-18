#include <iostream>

#ifndef CODE_LABEL_HH
#include "cpu/cCodeLabel.h"
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

using namespace std;
namespace nCodeLabelTest {
  class cTestConstructor : public cTestCase {
  public: virtual void test() {
    cCodeLabel label;
    test_is_true(label.GetSize() == 0);
  } };
  class cTestCopyConstructor : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  
  class cTestOK : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestEqualityOperator : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestInequalityOperator : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestIndexOperator : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestFindSublabel : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  
  class cTestClear : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestAddNop : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestRotate : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  
  class cTestGetSize : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestAsString : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  class cTestAsInt : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented." << endl; 
  } };
  
  class cTestSaveState : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented," << endl <<
    "because void cCodeLabel::SaveState(ostream &) is not implemented."
    << endl;
  } };
  class cTestLoadState : public cTestCase {
  public: virtual void test() {
    cout << __PRETTY_FUNCTION__ << " is not implemented," << endl <<
    "because void cCodeLabel::LoadState(istream &) is not implemented."
    << endl;
  } };
  
  class cTestSuite : public cTestCase {
  public: cTestSuite() : cTestCase() {
    adoptTestCase(new cTestConstructor); 
    adoptTestCase(new cTestCopyConstructor); 

    adoptTestCase(new cTestOK); 
    adoptTestCase(new cTestEqualityOperator); 
    adoptTestCase(new cTestInequalityOperator); 
    adoptTestCase(new cTestIndexOperator); 
    adoptTestCase(new cTestFindSublabel); 

    adoptTestCase(new cTestClear); 
    adoptTestCase(new cTestAddNop); 
    adoptTestCase(new cTestRotate); 

    adoptTestCase(new cTestGetSize); 
    adoptTestCase(new cTestAsString); 
    adoptTestCase(new cTestAsInt); 

    adoptTestCase(new cTestSaveState); 
    adoptTestCase(new cTestLoadState); 
  } };
}

using namespace nCodeLabelTest;
int main(int argc, char *argv[]){
  cout << endl << "Testing cCodeLabel." << endl;
  int successes = 0, test_count = 0;
  cTestSuite inst_test;
  inst_test.runTest();
  successes += inst_test.lastResult().successCount();
  test_count += inst_test.lastResult().totalTestCount();
  cTextTestInterpreter interpreter;
  cout << interpreter.interpretation(inst_test.lastResult());
  return(successes != test_count);
}
