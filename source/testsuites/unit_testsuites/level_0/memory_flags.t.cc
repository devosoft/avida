#ifndef MEMORY_FLAGS_HH
#include "memory_flags.hh"
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

class cMemoryFlagsOverride : public cMemoryFlags {
public:
  bool getCopied(){ return copied; }
  bool getMutated(){ return mutated; }
  bool getExecuted(){ return executed; }
  bool getBreakpoint(){ return breakpoint; }
  bool getPoint_mut(){ return point_mut; }
  bool getCopy_mut(){ return copy_mut; }
  bool getInjected(){ return injected; }

  void setCopied(bool _copied){ copied = _copied; }
  void setMutated(bool _mutated){ mutated = _mutated; }
  void setExecuted(bool _executed){ executed = _executed; }
  void setBreakpoint(bool _breakpoint){ breakpoint = _breakpoint; }
  void setPoint_mut(bool _point_mut){ point_mut = _point_mut; }
  void setCopy_mut(bool _copy_mut){ copy_mut = _copy_mut; }
  void setInjected(bool _injected){ injected = _injected; }
};

class cMemoryFlagsTest : public cTestCase {
public: virtual void test() {
  // all values should be false after construction.
  cMemoryFlagsOverride mfo;
  test_is_true(!mfo.getCopied());
  test_is_true(!mfo.getMutated());
  test_is_true(!mfo.getExecuted());
  test_is_true(!mfo.getBreakpoint());
  test_is_true(!mfo.getPoint_mut());
  test_is_true(!mfo.getCopy_mut());
  test_is_true(!mfo.getInjected());

  // set all values to true, and double check result.
  mfo.setCopied(true);
  mfo.setMutated(true);
  mfo.setExecuted(true);
  mfo.setBreakpoint(true);
  mfo.setPoint_mut(true);
  mfo.setCopy_mut(true);
  mfo.setInjected(true);
  test_is_true(mfo.getCopied());
  test_is_true(mfo.getMutated());
  test_is_true(mfo.getExecuted());
  test_is_true(mfo.getBreakpoint());
  test_is_true(mfo.getPoint_mut());
  test_is_true(mfo.getCopy_mut());
  test_is_true(mfo.getInjected());

  // all values should be false after Clear().
  mfo.Clear();
  test_is_true(!mfo.getCopied());
  test_is_true(!mfo.getMutated());
  test_is_true(!mfo.getExecuted());
  test_is_true(!mfo.getBreakpoint());
  test_is_true(!mfo.getPoint_mut());
  test_is_true(!mfo.getCopy_mut());
  test_is_true(!mfo.getInjected());
} };

class cTestSuite : public cTestCase {
public: cTestSuite() : cTestCase() {
  adoptTestCase(new cMemoryFlagsTest); 
} };

int main(int argc, char *argv[]){
  cout << endl 
  << "----------------------------------------" << endl
  << "Testing cMemoryFlags." << endl;
  cTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return(t.lastResult().successCount() != t.lastResult().totalTestCount());
}
