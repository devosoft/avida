#ifndef AVD_VERIFIABLE_HH
#define AVD_VERIFIABLE_HH

#ifndef TLIST_HH
#include "tList.hh"
#endif

class avdVerifiable {
protected:
  const char *m_name;
  avdVerifiable &m_parent;
  tList<avdVerifiable> m_child_list;
protected:
  avdVerifiable(const char *name, avdVerifiable &parent = avdVerifiable::s_null);
  virtual ~avdVerifiable(){}
  void addVerifiable(avdVerifiable &child);
  avdVerifiable &getParent(){ return m_parent; }
  tList<avdVerifiable> &getChildList(){ return m_child_list; }
  bool nullParent();
public:
  static avdVerifiable s_null;
public:
  bool operator==(const avdVerifiable &in);
  const char *getName() const { return m_name; }
  virtual bool verify();
};

#endif
