#ifndef AVD_MESSAGE_CLASS_HH
#include "avd_message_class.hh"
#endif
#ifndef AVD_MESSAGE_CLASS_M_HH
#include "avd_message_class_m.hh"
#endif

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

class cTemplateTest : public test_case {
public: virtual void test() {
  test_is_true(true);
} };

class cTestSuite : public test_case {
public: cTestSuite() : test_case() {
  adopt_test_case(new cTemplateTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing Template." << endl;
  cTestSuite t;
  t.run_test();
  cout << text_test_interpreter().interpretation(t.last_result());
  return(t.last_result().success_count() != t.last_result().total_test_count());
}
