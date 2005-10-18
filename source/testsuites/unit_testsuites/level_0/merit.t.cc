#ifndef MERIT_HH
#include "cMerit.h"
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

class cMeritOverride : public cMerit {
public:
  void operator=(const cMerit &merit){*static_cast<cMerit *>(this)=merit;}
  int getBits(){ return bits; }
  UINT getBase(){ return base; }
  UINT getOffset(){ return offset; }
  double getValue(){ return value; }
};

class cGetBitTest : public cTestCase {
public: virtual void test() {
  cout << "\tTesting GetDouble() UNIMPLEMENTED." << endl;

  //double mult[];
  //cMerit m;
  //cMeritOverride mo;
  //mo.Clear();
  //for(double d = 0.; d < 64.; d+=0.3){
  //  double value = pow(2.,d);
  //  m = value; mo = m;

  //}
} };

class cGetDoubleTest : public cTestCase {
public: virtual void test() {
  cout << "\tTesting GetDouble()." << endl;
  cMerit m;
  m.Clear();
  for(double d = 0.; d < 64.; d+=0.3){
    double value = pow(2.,d);
    m = value;
    test_double_is_equal(value, m.GetDouble());
  }
} };

class cGetNumBitsTest : public cTestCase {
public: virtual void test() {
  cout << "\tTesting GetNumBits()." << endl;
  cMerit m;
  m.Clear();
  for(double d = 0.; d < 64.; d+=0.3){
    double value = pow(2.,d);
    m = value;
    test_int_is_equal((int)d+1, m.GetNumBits());
  }
} };

class cInternalStateTest : public cTestCase {
public: virtual void test() {
  cout << "\tTesting internal state." << endl;
  cMerit m;
  cMeritOverride mo;
  mo.Clear();
  for(double d = 0.; d < 64.; d += 0.3){
    double value = pow(2., d);
    m = value; mo = m;
    // test bits
    test_int_is_equal((int)d+1, mo.getBits());
    // test value
    test_is_true(
      mo.getValue() <= value * (1 + 1/UINT_MAX) ||
      mo.getValue() >= value / (1 + 1/UINT_MAX));
    // test base and offset
    double reconstructed_value = ((double)(mo.getBase()) * pow(2., (int)mo.getOffset()));
    test_is_true(
      reconstructed_value <= value * (1 + 1/UINT_MAX) ||
      reconstructed_value >= value / (1 + 1/UINT_MAX));
  }
} };

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cInternalStateTest); 
  adoptTestCase(new cGetDoubleTest); 
  adoptTestCase(new cGetNumBitsTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cMerit." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
