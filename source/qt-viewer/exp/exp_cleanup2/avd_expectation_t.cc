#ifndef TEST_CASE_H
#include "third-party/yaktest/test_case.h"
#endif
#ifndef TEXT_TEST_INTERPRETER_H
#include "third-party/yaktest/text_test_interpreter.h"
#endif
#ifndef TEST_RESULT_H
#include "third-party/yaktest/test_result.h"
#endif

#include <iostream>

using namespace std;

namespace avd_expectation_t {
  class test_ConstructorWithoutParent : public test_case {
  public: virtual void test() {
    //// Should record name and reference to stubExpectation::s_null.  also
    //// tests getName() and nullParent().
    //const char name[] = "expectation";
    //stubExpectation expectation(name);
    //test_is_true(0 == strncmp(name, expectation.getName(), sizeof(name)));
    //test_is_true(expectation.access_nullParent());
  } };
  
  class test_Constructor : public test_case {
  public: virtual void test() {
    //const char parent_name[] = "parent";
    //const char name[] = "child";
    //stubExpectation parent(parent_name);
    //test_is_true(0 == parent.access_getChildList().GetSize());

    //// Should record name and reference to parent.  Also tests
    //// getName(), operator==(), and getParent().
    //stubExpectation child(name, parent);
    //test_is_true(0 == strncmp(name, child.getName(), sizeof(name)));
    //test_is_true(&parent == &child.access_getParent());

    //// Should add self to parent list.  Also tests getChildList().
    //test_is_true(parent.access_getChildList().Find(&child));
  } };
  
  class test_clearActual : public test_case {
  public: virtual void test() {
    //// Should reset expectation's actual value(s).
  } };
  
  class test_ignoreActual : public test_case {
  public: virtual void test() {
    //// If setIgnoreActual(true) then ignoreActual() == true, and expectation should always succeed
    //// on verify (even if actual fails).
  } };
  
  class test_setIgnoreActual : public test_case {
  public: virtual void test() {
  } };
  
  class cTestSuite : public test_case {
  public: cTestSuite() : test_case() {
    adopt_test_case(new test_ConstructorWithoutParent); 
    adopt_test_case(new test_Constructor); 
  } };
}

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing avdExpectation via stubExpectation." << endl;
  avd_expectation_t::cTestSuite t; t.run_test();
  cout << text_test_interpreter().interpretation(t.last_result());
  return(t.last_result().success_count() != t.last_result().total_test_count());
}
