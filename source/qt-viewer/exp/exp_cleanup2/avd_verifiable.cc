#ifndef AVD_VERIFIABLE_HH
#include "avd_verifiable.hh"
#endif

avdVerifiable avdVerifiable::s_null("<avdVerifiable::s_null>");

avdVerifiable::avdVerifiable(const char *name, avdVerifiable &parent)
: m_name(name) , m_parent(parent)
{ if(!nullParent()) getParent().addVerifiable(*this); }

void avdVerifiable::addVerifiable(avdVerifiable &child) { getChildList().PushRear(&child); }
bool avdVerifiable::nullParent() { return avdVerifiable::s_null == getParent(); }
bool avdVerifiable::operator==(const avdVerifiable &in) { return (this == &in); }
bool avdVerifiable::verify()
{
  tListIterator<avdVerifiable> it(getChildList());
  while(avdVerifiable *v = it.Next()) if(!v->verify()) return false;
  return true;
}
