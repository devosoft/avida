#ifndef AVD_BOOL_HH
#define AVD_BOOL_HH

class avdBool {
protected:
  bool m_is_true;
public:
  bool isTrue(){ return m_is_true; }
  void setTrue(bool is_true){ m_is_true = is_true; }
};

#endif
