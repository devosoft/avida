#ifndef AVD_VERIFIABLE_M_HH
#define AVD_VERIFIABLE_M_HH

#ifndef AVD_VERIFIABLE_HH
#include "avd_verifiable.hh"
#endif

class stubVerifiable : public avdVerifiable {
  bool m_should_fail;
public:
  stubVerifiable(const char *name, avdVerifiable &parent = avdVerifiable::s_null)
  : avdVerifiable(name, parent)
  {}
  virtual bool verify(){ return ((!m_should_fail) && avdVerifiable::verify()); }
public:
  void access_addVerifiable(avdVerifiable &child){ addVerifiable(child); }
  avdVerifiable &access_getParent() { return getParent(); }
  tList<avdVerifiable> &access_getChildList(){ return getChildList(); }
  bool access_nullParent(){ return nullParent(); }
  void shouldFail(bool should_fail){ m_should_fail = should_fail; }
};

#endif
