#include <iostream>

#ifndef INSTRUCTION_HH
#include "main/cInstruction.h"
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
namespace nInstructionTest {
  class cTestGetSetOp : public cTestCase {
  public: virtual void test() {
      cInstruction inst;
      inst.SetOp(5); test_is_true(inst.GetOp() == 5);
  } };
  class cTestSetSymbol : public cTestCase {
  public: virtual void test() {
      cInstruction inst;
      // Legal symbols are a-z, A-Z, and 0-9,
      // and map to values 0-61.
      // All other symbols should map to value 254 (error).
      inst.SetSymbol('b'); test_is_true(inst.GetOp() == 1);
      inst.SetSymbol('z'); test_is_true(inst.GetOp() == 25);
      inst.SetSymbol('A'); test_is_true(inst.GetOp() == 26);
      inst.SetSymbol('Z'); test_is_true(inst.GetOp() == 51);
      inst.SetSymbol('0'); test_is_true(inst.GetOp() == 52);
      inst.SetSymbol('9'); test_is_true(inst.GetOp() == 61);
      inst.SetSymbol('9' + 1); test_is_true(inst.GetOp() == 254);
      inst.SetSymbol('.'); test_is_true(inst.GetOp() == 254);
  } };
  class cTestGetSymbol : public cTestCase {
  public: virtual void test() {
      cInstruction inst;
      // Values 0-61 map to symbols a-z, A-Z, and 0-9,
      // in that order.
      // All other values should map to symbol '?'.
      // @DMB - why does value 255 map to symbol '_'?
      inst.SetOp(1); test_is_true(inst.GetSymbol() == 'b');
      inst.SetOp(25); test_is_true(inst.GetSymbol() == 'z');
      inst.SetOp(26); test_is_true(inst.GetSymbol() == 'A');
      inst.SetOp(51); test_is_true(inst.GetSymbol() == 'Z');
      inst.SetOp(52); test_is_true(inst.GetSymbol() == '0');
      inst.SetOp(61); test_is_true(inst.GetSymbol() == '9');
      inst.SetOp(62); test_is_true(inst.GetSymbol() == '?');
      inst.SetOp(255); test_is_true(inst.GetSymbol() == '_');
  } };
  class cTestConstructor : public cTestCase {
  public: virtual void test() {
      cInstruction inst;
      test_is_true(inst.GetOp() == 0);
  } };
  class cTestIntConstructor : public cTestCase {
  public: virtual void test() {
      cInstruction inst(5);
      test_is_true(inst.GetOp() == 5);
  } };
  class cTestCopyConstructor : public cTestCase {
  public: virtual void test() {
      cInstruction inst(5);
      cInstruction inst2(inst);
      test_is_true(inst2.GetOp() == 5);
      inst.SetOp(6);
      test_is_true(inst2.GetOp() == 5); // should not now == 6.
      cInstruction inst3(inst);
      test_is_true(inst3.GetOp() == 6);
  } };
  class cTestAssignmentOperator : public cTestCase {
  public: virtual void test() {
      cInstruction inst(5);
      cInstruction inst2 = inst;
      test_is_true(inst2.GetOp() == 5);
      inst.SetOp(6);
      test_is_true(inst2.GetOp() == 5); // should not now == 6.
      cInstruction inst3 = inst;
      test_is_true(inst3.GetOp() == 6);
  } };
  class cTestEqualityOperators : public cTestCase {
  public: virtual void test() {
      cInstruction inst(5);
      cInstruction inst2(6);
      cInstruction inst3(6);
      test_is_true(inst2 == inst3);
      test_is_true(inst != inst2);
      test_is_true(inst2 != inst); // commutes?
  } };
  
  class cTestSuite : public cTestCase {
  public: cTestSuite() : cTestCase() {
    adoptTestCase(new cTestGetSetOp); 
    adoptTestCase(new cTestSetSymbol); 
    adoptTestCase(new cTestGetSymbol); 
    adoptTestCase(new cTestConstructor); 
    adoptTestCase(new cTestIntConstructor); 
    adoptTestCase(new cTestCopyConstructor); 
    adoptTestCase(new cTestAssignmentOperator); 
    adoptTestCase(new cTestEqualityOperators); 
  } };
}

using namespace nInstructionTest;
int main(int argc, char *argv[]){
  cout << endl << "Testing cInstruction." << endl;
  int successes = 0, test_count = 0;
  cTestSuite inst_test;
  inst_test.runTest();
  successes += inst_test.lastResult().successCount();
  test_count += inst_test.lastResult().totalTestCount();
  cTextTestInterpreter interpreter;
  cout << interpreter.interpretation(inst_test.lastResult());
  return(successes != test_count);
}
