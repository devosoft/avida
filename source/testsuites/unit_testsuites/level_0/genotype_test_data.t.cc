#ifndef GENOTYPE_TEST_DATA_HH
#include "genotype_test_data.hh"
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

class cGenotype_TestDataTest : public cTestCase {
public: virtual void test() {
  cGenotype_TestData gtd;
  //test_is_true(!gtd.is_viable);
  test_double_is_equal(gtd.fitness, -1.);
  //test_double_is_equal(gtd.merit, 0.);
  //test_int_is_equal(gtd.gestation_time, 0);
  //test_int_is_equal(gtd.executed_size, 0);
  //test_int_is_equal(gtd.copied_size, 0);
  //test_double_is_equal(gtd.colony_fitness, 0.);
  //test_int_is_equal(gtd.generations, 0);

  // FIXME
  cout
  << "***" << endl
  << "FIXME:" << endl
  << "why is cGenotype_TestData::fitness" << endl
  << "initially -1.0?" << endl
  << "verify that other members" << endl
  << "need not be initialized. -- kgn" << endl
  << "***" << endl;
} };

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cGenotype_TestDataTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cGenotype_TestData." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
