#ifndef AVD_MESSAGE_DISPLAY_HH
#include "avd_message_display.hh"
#endif
#ifndef AVD_MESSAGE_DISPLAY_M_HH
#include "avd_message_display_m.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
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

#include <sys/types.h>
#include <iostream>
#include <regex.h>
#include <string>
#include <fstream>
#include <set>

using namespace std;

char outbuf[5120];
char errbuf[5120];

class cMockCoutTest : public test_case {
public: virtual void test() {
  mockMessageDisplay md;
  md.out("test\n");
  md.out("fubar\n");
  md.abort();
} };

class cCoutTest : public test_case {
public: virtual void test() {
  avdMessageDisplay md(&cout);
  cout.flush();
  md.out("test\n");
  test_is_true(strncmp(outbuf, "test\n", 5) == 0);
  cout.flush();
  md.out("fubar\n");
  test_is_true(strncmp(outbuf, "fubar\n", 6) == 0);
} };

class cCerrTest : public test_case {
public: virtual void test() {
  avdMessageDisplay md(&cerr);
  cerr.flush();
  md.out("test\n");
  test_is_true(strncmp(errbuf, "test\n", 5) == 0);
  md.out("fubar\n");
  cerr.flush();
  test_is_true(strncmp(errbuf, "fubar\n", 6) == 0);
} };

class cTestSuite : public test_case {
public: cTestSuite() : test_case() {
  adopt_test_case(new cMockCoutTest); 
  adopt_test_case(new cCoutTest); 
  adopt_test_case(new cCerrTest); 
} };

int
main (int argc, char *argv[]) {
// snippet adapted from dejagnu sources.
#ifdef __STDC_HOSTED__
  cout.rdbuf()->pubsetbuf(outbuf, 5120);
  cerr.rdbuf()->pubsetbuf(errbuf, 5120);
#else
  cout.rdbuf()->setbuf(outbuf, 5120);
  cerr.rdbuf()->setbuf(errbuf, 5120);
#endif
  
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing avdMessageDisplay and avdMessageDisplay." << endl;
  cTestSuite t;
  t.run_test();
  cout << text_test_interpreter().interpretation(t.last_result());
  return(t.last_result().success_count() != t.last_result().total_test_count());
}
