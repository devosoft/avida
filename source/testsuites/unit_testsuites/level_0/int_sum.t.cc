#ifndef INT_SUM_HH
#include "int_sum.hh"
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

class cIntSumTest : public cTestCase {
public: virtual void test() {
  cIntSum is;
  // test of default constructor, which should Clear()
  test_int_is_equal(is.Count(), 0);
  test_int_is_equal(is.N(), 0);
  test_int_is_equal(is.Sum(), 0);
  test_int_is_equal(is.S1(), 0);
  test_int_is_equal(is.SumOfSquares(), 0);
  test_int_is_equal(is.S2(), 0);
  test_int_is_equal(is.SumOfCubes(), 0);
  test_int_is_equal(is.S3(), 0);
  test_int_is_equal(is.S4(), 0);
  test_double_is_equal(is.Average(), 0.);
  test_double_is_equal(is.Variance(), cIntSum::INF_ERR);
  test_double_is_equal(is.StdDeviation(), cIntSum::INF_ERR);
  test_double_is_equal(is.StdError(), cIntSum::INF_ERR);
  test_double_is_equal(is.Skewness(), cIntSum::INF_ERR);
  test_double_is_equal(is.Kurtosis(), cIntSum::INF_ERR);
  test_double_is_equal(is.Ave(), cIntSum::INF_ERR);
  test_double_is_equal(is.Var(), cIntSum::INF_ERR);
  test_double_is_equal(is.Kur(), cIntSum::INF_ERR);
  test_double_is_equal(is.Skw(), cIntSum::INF_ERR);

  is.Add(1);
  is.Add(2, 2);
  is.Add(3, 3);
  is.Add(4, 4);
  double ave(is.Average());
  double var(is.Variance());
  double sdv(is.StdDeviation());
  double sde(is.StdError());
  double skw(is.Skewness());
  double krt(is.Kurtosis());
  is.Add(5, 5);
  double_is_within_tolerance(is.Average(), 3.6666, 0.0001);
  double_is_within_tolerance(is.Variance(), 55.524, 0.001);
  double_is_within_tolerance(is.StdDeviation(), 7.4514, 0.0001);
  //double_is_within_tolerance(is.StdError(), 1.9239, 0.0001);
  double_is_within_tolerance(is.Skewness(), 9.2512e+02, 1.e-02);
  double_is_within_tolerance(is.Kurtosis(), 2.5596e+04, 1.);

  //cout
  //<< " ave: " << ave
  //<< " var: " << var
  //<< " sdv: " << sdv
  //<< " sde: " << sde
  //<< " skw: " << skw
  //<< " krt: " << krt
  //<< endl;
  is.Subtract(5, 5);
  double_is_within_tolerance(is.Average(), ave, 0.0001);
  double_is_within_tolerance(is.Variance(), var, 1.e-03);
  double_is_within_tolerance(is.StdDeviation(), sdv, 0.0001);
  double_is_within_tolerance(is.StdError(), sde, 0.0001);
  double_is_within_tolerance(is.Skewness(), skw, 1.e-02);
  double_is_within_tolerance(is.Kurtosis(), krt, 1.e-01);

  is.Clear();
  test_int_is_equal(is.Count(), 0);
  test_int_is_equal(is.N(), 0);
  test_int_is_equal(is.Sum(), 0);
  test_int_is_equal(is.S1(), 0);
  test_int_is_equal(is.SumOfSquares(), 0);
  test_int_is_equal(is.S2(), 0);
  test_int_is_equal(is.SumOfCubes(), 0);
  test_int_is_equal(is.S3(), 0);
  test_int_is_equal(is.S4(), 0);
  test_double_is_equal(is.Average(), 0.);
  test_double_is_equal(is.Variance(), cIntSum::INF_ERR);
  test_double_is_equal(is.StdDeviation(), cIntSum::INF_ERR);
  test_double_is_equal(is.StdError(), cIntSum::INF_ERR);
  test_double_is_equal(is.Skewness(), cIntSum::INF_ERR);
  test_double_is_equal(is.Kurtosis(), cIntSum::INF_ERR);
  test_double_is_equal(is.Ave(), cIntSum::INF_ERR);
  test_double_is_equal(is.Var(), cIntSum::INF_ERR);
  test_double_is_equal(is.Kur(), cIntSum::INF_ERR);
  test_double_is_equal(is.Skw(), cIntSum::INF_ERR);

  // FIXME
  cout
  << "***" << endl
  << "FIXME:" << endl
  << "We should test for precision and range." << endl
  << "Discuss with Charles.  -- kgn" << endl
  << "In particular, is.StdError() overflows" << endl
  << "perhaps too easily." << endl
  << endl
  << "Should find out expected range and number" << endl
  << "of inputs and weights.  -- kgn" << endl
  << "***" << endl;
} };

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cIntSumTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cIntSum." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
