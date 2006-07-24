#ifndef B_HH
#define B_HH

#ifndef A_HH
#include "A.hh"
#endif

class B : public A {
public:
  B(){}
  virtual void B_does_nothing(){};
  virtual int getBlah(A &a){ return a.m_blah; }
  //virtual void adoptTestCase(A &a){ }
  virtual void whatDoesAdo(A &a){ a.A_does_nothing(); }
};

#endif
