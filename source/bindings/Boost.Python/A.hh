#ifndef A_HH
#define A_HH

#ifndef TEST_RESULT_H
#include "cTestResult.hh"
#endif
#ifndef STRINGABLE_H
#include "cStringable.hh"
#endif

class A {
public:
  A(){}
  A(const A &a){}
  //virtual ~A(){}
  virtual void adoptTestCase(A &new_case){}
  virtual int fuBar(A &new_case){}

  //virtual void test(){}


  int m_blah;
  virtual void A_does_nothing(){}
};

struct C {
  C(){}
};

#endif
