#ifndef CPU_STATS_HH
#include "cpu_stats.hh"
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

class cTestSetup : public cTestCase {
public:
  virtual void test() {
    sCPUStats s;
    s.Setup(0);
    test_is_true(s.mut_stats.point_mut_count == 0);
    test_is_true(s.mut_stats.copy_mut_count == 0);
    test_is_true(s.mut_stats.insert_mut_count == 0);
    test_is_true(s.mut_stats.delete_mut_count == 0);
    test_is_true(s.mut_stats.div_mut_count == 0);
    test_is_true(s.mut_stats.point_mut_line_count == 0);
    test_is_true(s.mut_stats.parent_mut_line_count == 0);
    test_is_true(s.mut_stats.copy_mut_line_count == 0);
    test_is_true(s.mut_stats.divide_mut_count == 0);
    test_is_true(s.mut_stats.divide_insert_mut_count == 0);
    test_is_true(s.mut_stats.divide_delete_mut_count == 0);
    test_is_true(s.mut_stats.copies_exec == 0);
  }
};

class cTestClear : public cTestCase {
public:
  virtual void test() {
    sCPUStats s;
    s.mut_stats.point_mut_count = 1;
    s.mut_stats.copy_mut_count = 2;
    s.mut_stats.insert_mut_count = 3;
    s.mut_stats.delete_mut_count = 4;
    s.mut_stats.div_mut_count = 5;
    s.mut_stats.point_mut_line_count = 6;
    s.mut_stats.parent_mut_line_count = 7;
    s.mut_stats.copy_mut_line_count = 8;
    s.mut_stats.divide_mut_count = 9;
    s.mut_stats.divide_insert_mut_count = 10;
    s.mut_stats.divide_delete_mut_count = 11;
    s.mut_stats.copies_exec = 12;

    s.Clear();
    test_is_true(s.mut_stats.point_mut_count == 0);
    test_is_true(s.mut_stats.copy_mut_count == 0);
    test_is_true(s.mut_stats.insert_mut_count == 0);
    test_is_true(s.mut_stats.delete_mut_count == 0);
    test_is_true(s.mut_stats.div_mut_count == 0);
    test_is_true(s.mut_stats.point_mut_line_count == 0);
    test_is_true(s.mut_stats.parent_mut_line_count == 0);
    test_is_true(s.mut_stats.copy_mut_line_count == 0);
    test_is_true(s.mut_stats.divide_mut_count == 0);
    test_is_true(s.mut_stats.divide_insert_mut_count == 0);
    test_is_true(s.mut_stats.divide_delete_mut_count == 0);
    test_is_true(s.mut_stats.copies_exec == 0);
  }
};

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cTestSetup); 
  adoptTestCase(new cTestClear); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing sCPUStats." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
