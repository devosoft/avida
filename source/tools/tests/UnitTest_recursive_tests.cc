#ifdef NDEBUG
#undef NDEBUG
#endif

#include "recursive_tests_defn_hdrs.hh"
#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace std;


/*
test objects.
*/
class cRed {
protected:
  int m_i;
  unsigned int m_verify_ct;;
  static unsigned int s_unit_test_ct;
public:
  cRed():m_i(23){}
  int &i(){ return m_i; }
  unsigned int verifyCt(){ return m_verify_ct; }
  static unsigned int unitTestCt(){ return s_unit_test_ct; }
  void incVerifyCt(){ m_verify_ct++; }
  static void incUnitTestCt(){ s_unit_test_ct++; }
  void reset(){ s_unit_test_ct = 0; m_verify_ct = 0; }
public:
  avdRecursiveTestsDeclMacro(cRed,);
};

/* Test suites for class cRed. */
avdRecursiveTestsDefnMacro(cRed,);
unsigned int cRed::s_unit_test_ct(0);
class cRedTest : public cTestCase { public: virtual void test() {
  cRed::incUnitTestCt(); // this is for unit-testing tUnitTestSuite.
} };
template <> void tUnitTestSuite<cRed>::adoptUnitTests(){
  adoptUnitTestCase(new cRedTest);
}
template <> void tVerifierSuite<cRed>::verify(cRed& red){
  test_is_true(23 == red.i());

  red.incVerifyCt(); // this is for unit-testing tVerifierSuite.
}

class cGreen {
protected:
  cRed m_red;
  cRed *m_red_prime;
  int m_i;
  unsigned int m_verify_ct;;
  static unsigned int s_unit_test_ct;
public:
  cGreen():m_red_prime(0), m_i(42){}
public:
  cRed &r(){ return m_red; }
  cRed *rprime(){ return m_red_prime; }
  int &i(){ return m_i; }
  unsigned int verifyCt(){ return m_verify_ct; }
  static unsigned int unitTestCt(){ return s_unit_test_ct; }
  void incVerifyCt(){ m_verify_ct++; }
  static void incUnitTestCt(){ s_unit_test_ct++; }
  void reset(){ s_unit_test_ct = 0; m_verify_ct = 0; }
  void setRPrime(cRed *red){ m_red_prime = red; }
public:
  avdRecursiveTestsDeclMacro(cGreen,);
};

/* Test suites for class cGreen. */
avdRecursiveTestsDefnMacro(cGreen,);
unsigned int cGreen::s_unit_test_ct(0);
class cGreenTest : public cTestCase { public: virtual void test() {
  cGreen::incUnitTestCt(); // this is for unit-testing tUnitTestSuite.
} };
template <> void tUnitTestSuite<cGreen>::adoptUnitTests(){
  adoptUnitTestSuite<cRed>();
  adoptUnitTestCase(new cGreenTest);
}
template <> void tVerifierSuite<cGreen>::verify(cGreen& green){
  adoptVerifierSuite<>(green.r());
  if(green.rprime()) adoptVerifierSuite<>(*green.rprime());
  test_is_true(42 == green.i());

  green.incVerifyCt(); // this is for unit-testing tVerifierSuite.
}

class cBlue {
protected:
  cRed m_red;
  cGreen m_green;
  unsigned int m_verify_ct;
  static unsigned int s_unit_test_ct;
public:
  cRed &r(){ return m_red; }
  cGreen &g(){ return m_green; }
  unsigned int verifyCt(){ return m_verify_ct; }
  static unsigned int unitTestCt(){ return s_unit_test_ct; }
  void incVerifyCt(){ m_verify_ct++; }
  static void incUnitTestCt(){ s_unit_test_ct++; }
  void reset(){ s_unit_test_ct = 0; m_verify_ct = 0; }
public:
  avdRecursiveTestsDeclMacro(cBlue,);
};

/* Test suites for class cGreen. */
avdRecursiveTestsDefnMacro(cBlue,);
unsigned int cBlue::s_unit_test_ct(0);
class cBlueTest : public cTestCase { public: virtual void test() {
  cBlue::incUnitTestCt(); // this is for unit-testing tUnitTestSuite.
} };
template <> void tUnitTestSuite<cBlue>::adoptUnitTests(){
  adoptUnitTestSuite<cRed>();
  adoptUnitTestSuite<cGreen>();
  adoptUnitTestCase(new cBlueTest);
}
template <> void tVerifierSuite<cBlue>::verify(cBlue& blue){
  adoptVerifierSuite<>(blue.r());
  adoptVerifierSuite<>(blue.g());
  test_is_true(true);

  blue.incVerifyCt(); // this is for unit-testing tVerifierSuite.
}


/*
unit tests base class.
*/
class cRecursiveTests_Base : public cTestCase {
protected:
  cRed *m_red;
  cGreen *m_green;
  cBlue *m_blue;
public:
  void setR(cRed *red){ m_red = red; }
  void setG(cGreen *green){ m_green = green; }
  void setB(cBlue *blue){ m_blue = blue; }
  void reset(){ R()->reset(); G()->reset(); B()->reset(); }
  cRed *R(){ return m_red; }
  cGreen *G(){ return m_green; }
  cBlue *B(){ return m_blue; }
public:
  cRecursiveTests_Base():m_red(0),m_green(0),m_blue(0){
    setR(new cRed); setG(new cGreen); setB(new cBlue);
    /* This makes B()->r() and *B()->g().r() refer to the same object so that I can test the cycle-detector. */
    B()->g().setRPrime(&B()->r());
  }
  ~cRecursiveTests_Base(){ if(R()) delete R(); if(G()) delete G(); if(B()) delete B(); }
};


/*
test cases.
*/
class cRecursiveTests_tRecursiveTests_Tests : public cRecursiveTests_Base { public: virtual void test() {
  cTestResult tr;

  /* Check nonrecursive unit tests : cRed and cGreen should not be recursively tested.  */
  reset();
  tr = B()->unitTest(false);
  test_is_true(0 == cRed::unitTestCt());
  test_is_true(0 == cGreen::unitTestCt());
  test_is_true(1 == cBlue::unitTestCt());

  /* Check recursive unit tests : cRed and cGreen should be recursively tested once each.  */
  reset();
  tr = B()->unitTest(true);
  /* note : if (1 < cRed::unitTestCt()) then cycle-detection isn't working. */
  test_is_true(1 == cRed::unitTestCt());
  test_is_true(1 == cGreen::unitTestCt());
  test_is_true(1 == cBlue::unitTestCt());

  /* Check nonrecursive verification : B()->r(), B()->g(), and B()->g().r() should not be recursively verified.  */
  reset();
  tr = B()->verify(false);
  test_is_true(1 == B()->verifyCt());
  test_is_true(0 == B()->r().verifyCt());
  test_is_true(0 == B()->g().verifyCt());
  test_is_true(0 == B()->g().r().verifyCt());
  test_is_true(0 == B()->g().rprime()->verifyCt());

  /*
  Check nrecursive verification : B()->r(), B()->g(), and B()->g().r() should be recursively verified. Note that I've made
  B()->r() and *B()->g().r() refer to the same object, so it will be encountered twice in the recursion, but if the
  cycle-detector is working properly then it will be verified only once.
  */
  reset();
  tr = B()->verify(true);
  test_is_true(1 == B()->verifyCt());
  test_is_true(1 == B()->r().verifyCt());
  test_is_true(1 == B()->g().verifyCt());
  test_is_true(1 == B()->g().r().verifyCt());
  test_is_true(1 == B()->g().rprime()->verifyCt());
} };


class cRecursiveTests_cCycleCheck_Tests : public cRecursiveTests_Base { public: virtual void test() {
  cCycleCheck nonrecursive_cyck(false);
  /* does recurse() return the instantiation argument ? */
  test_is_true(false == nonrecursive_cyck.recurse());

  cCycleCheck cyck(true);
  /* does recurse() return the instantiation argument ? */
  test_is_true(true == cyck.recurse());

  /* check should succeed since cyck hasn't seen "fu" before. */
  test_is_true(true == cyck.ckStr("fu"));
  /* now cyck has seen "fu" before, so check should fail. */
  test_is_true(false == cyck.ckStr("fu"));
  /* check should succeed since cyck hasn't seen "bar" before. */
  test_is_true(true == cyck.ckStr("bar"));

  /* as above, but with (hopefully unique) pointers instead of strings. */
  test_is_true(true == cyck.ckID(R()->id()));
  test_is_true(false == cyck.ckID(R()->id()));
  test_is_true(true == cyck.ckID(G()->id()));
  test_is_true(false == cyck.ckID(G()->id()));
  test_is_true(true == cyck.ckID(B()->id()));
  test_is_true(false == cyck.ckID(B()->id()));
  test_is_true(true == cyck.ckID(B()->r().id()));
  test_is_true(false == cyck.ckID(B()->r().id()));
  test_is_true(true == cyck.ckID(B()->g().id()));
  test_is_true(false == cyck.ckID(B()->g().id()));
  test_is_true(true == cyck.ckID(B()->g().r().id()));
  test_is_true(false == cyck.ckID(B()->g().r().id()));
} };


class cRecursiveTests_cID_Tests : public cRecursiveTests_Base {
  struct A { cID m_id; cID &id(){ return m_id;} };
  struct B { A m_a; cID m_id; cID &id(){ return m_id;} };
public: virtual void test() {
  B b;
  /* b.m_a, being aggregate and the first data in b, should have the same address as b. */
  test_is_true((void *)&b == (void *)&b.m_a);
  /* b.m_a.id() and b.id() refer to distinct instances of cID, and so should have different addresses. */
  test_is_true(b.id() != b.m_a.id());
  /* does cID have nonzero size even though it has no data ? if not, how can b.m_a.id() and b.id() have different addresses ? */
  test_is_true(0 < sizeof(cID));
} };


/* dummy class, permits a specialization of tUnitTestSuite<>. */
struct UnitTestRecurserTestSupport {};
template <> struct tUnitTestSuite<UnitTestRecurserTestSupport> : public cTestCase {
  cCycleCheck &m_cyck;
  static unsigned int s_unit_test_count;
  tUnitTestSuite(cCycleCheck &cyck):m_cyck(cyck){};
  virtual void test(){
    /* this class' name should already have been inserted (by tUnitTestSuiteRecurser<UnitTestRecurserTestSupport>). */
    test_is_true(false == m_cyck.ckStr(tUnitTestSuite<UnitTestRecurserTestSupport>::getClassName()));
  }
  static const char *getClassName(){ return "tUnitTestSuite<UnitTestRecurserTestSupport>"; }
};

class cRecursiveTests_tUnitTestSuiteRecurser_Tests : public cRecursiveTests_Base {
public: virtual void test() {
  tUnitTestSuiteRecurser<UnitTestRecurserTestSupport> utr(false);
  utr.runTest();
  /* make sure tUnitTestSuite<UnitTestRecurserTestSupport>::test() ran once and succeeded. */
  test_is_true(1 == utr.lastResult().successCount());
  test_is_true(1 == utr.lastResult().totalTestCount());
} };


/* dummy class, permits a specialization of tVerifierSuite<>. */
struct VerifierRecurserTestSupport { unsigned int m_verify_ct; cID m_id; cID &id(){ return m_id;} };
template <> struct tVerifierSuite<VerifierRecurserTestSupport> : public cTestCase {
  cVerifierSuiteRecurser &m_recurser;
  tVerifierSuite(cVerifierSuiteRecurser &recurser): m_recurser(recurser) {}
  void verify(VerifierRecurserTestSupport &obj){
    obj.m_verify_ct++;
    m_recurser.test_is_true(false == m_recurser.m_cyck.ckID(obj.id()));
  }
  static const char *getClassName(){ return "tVerifierSuite<VerifierRecurserTestSupport>"; }
};

class cRecursiveTests_tVerifierSuiteRecurser_Tests : public cRecursiveTests_Base {
public: virtual void test() {
  VerifierRecurserTestSupport obj;
  obj.m_verify_ct = 0;
  tVerifierSuiteRecurser<VerifierRecurserTestSupport> vr(obj, false);
  vr.runTest();
  /* make sure obj was verified exactly once. */
  test_is_true(1 == obj.m_verify_ct);
  test_is_true(1 == vr.lastResult().successCount());
  test_is_true(1 == vr.lastResult().totalTestCount());
} };

/* null test case to make sure cRecursiveTestsTestSuite is working. */
class cRecursiveTests_Blah : public cRecursiveTests_Base { public: virtual void test(){ test_is_true(true); } };

class cRecursiveTests_example : public cTestCase {
public: virtual void test() {
} };


/* main test suite. */
class cRecursiveTestsTestSuite : public cTestCase { public: cRecursiveTestsTestSuite() : cTestCase() {
  adoptTestCase(new cRecursiveTests_tRecursiveTests_Tests);
  adoptTestCase(new cRecursiveTests_cCycleCheck_Tests);
  adoptTestCase(new cRecursiveTests_cID_Tests);
  adoptTestCase(new cRecursiveTests_tUnitTestSuiteRecurser_Tests);
  adoptTestCase(new cRecursiveTests_tVerifierSuiteRecurser_Tests);
  adoptTestCase(new cRecursiveTests_Blah);

  adoptTestCase(new cRecursiveTests_example);
} };

/* test runner. */
int UnitTest_recursive_tests(int, char *[]){
  cRecursiveTestsTestSuite t;
  t.runTest();
  cout << cTextTestInterpreter().interpretation(t.lastResult());
  return( !(t.lastResult().successCount() == t.lastResult().totalTestCount()));
}
