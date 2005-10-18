#ifndef DOUBLE_SUM_HH
#include "cDoubleSum.h"
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

class cDoubleSumTest : public cTestCase {
public: virtual void test() {
  cDoubleSum ds;
  // test of default constructor, which should Clear()
  test_double_is_equal(ds.Count(), 0.);
  test_double_is_equal(ds.N(), 0.);
  test_double_is_equal(ds.Sum(), 0.);
  test_double_is_equal(ds.S1(), 0.);
  test_double_is_equal(ds.SumOfSquares(), 0.);
  test_double_is_equal(ds.S2(), 0.);
  test_double_is_equal(ds.SumOfCubes(), 0.);
  test_double_is_equal(ds.S3(), 0.);
  test_double_is_equal(ds.S4(), 0.);
  test_double_is_equal(ds.Average(), 0.);
  test_double_is_equal(ds.Variance(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.StdDeviation(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.StdError(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Skewness(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Kurtosis(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Ave(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Var(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Kur(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Skw(), cDoubleSum::INF_ERR);

  ds.Add(1.);
  ds.Add(2., 2.);
  ds.Add(3., 3.);
  ds.Add(4., 4.);
  double ave(ds.Average());
  double var(ds.Variance());
  double sdv(ds.StdDeviation());
  double sde(ds.StdError());
  double skw(ds.Skewness());
  double krt(ds.Kurtosis());
  ds.Add(5., 5.);
  double_is_within_tolerance(ds.Average(), 3.6666, 0.0001);
  double_is_within_tolerance(ds.Variance(), 55.524, 0.001);
  double_is_within_tolerance(ds.StdDeviation(), 7.4514, 0.0001);
  double_is_within_tolerance(ds.StdError(), 1.9239, 0.0001);
  double_is_within_tolerance(ds.Skewness(), 9.2512e+02, 1.e-02);
  double_is_within_tolerance(ds.Kurtosis(), 2.5596e+04, 1.);

  //cout
  //<< " ave: " << ave
  //<< " var: " << var
  //<< " sdv: " << sdv
  //<< " sde: " << sde
  //<< " skw: " << skw
  //<< " krt: " << krt
  //<< endl;
  ds.Subtract(5., 5.);
  double_is_within_tolerance(ds.Average(), ave, 0.0001);
  double_is_within_tolerance(ds.Variance(), var, 1.e-03);
  double_is_within_tolerance(ds.StdDeviation(), sdv, 0.0001);
  double_is_within_tolerance(ds.StdError(), sde, 0.0001);
  double_is_within_tolerance(ds.Skewness(), skw, 1.e-02);
  double_is_within_tolerance(ds.Kurtosis(), krt, 1.e-01);

  ds.Clear();
  test_double_is_equal(ds.Count(), 0.);
  test_double_is_equal(ds.N(), 0.);
  test_double_is_equal(ds.Sum(), 0.);
  test_double_is_equal(ds.S1(), 0.);
  test_double_is_equal(ds.SumOfSquares(), 0.);
  test_double_is_equal(ds.S2(), 0.);
  test_double_is_equal(ds.SumOfCubes(), 0.);
  test_double_is_equal(ds.S3(), 0.);
  test_double_is_equal(ds.S4(), 0.);
  test_double_is_equal(ds.Average(), 0.);
  test_double_is_equal(ds.Variance(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.StdDeviation(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.StdError(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Skewness(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Kurtosis(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Ave(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Var(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Kur(), cDoubleSum::INF_ERR);
  test_double_is_equal(ds.Skw(), cDoubleSum::INF_ERR);

  // FIXME
  cout
  << "***" << endl
  << "FIXME:" << endl
  << "We should test for precision and range." << endl
  << "Discuss with Charles.  -- kgn" << endl
  << "***" << endl;
} };

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cDoubleSumTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cDoubleSum." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
