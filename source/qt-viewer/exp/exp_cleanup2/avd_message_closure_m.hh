#ifndef AVD_MESSAGE_CLOSURE_M_HH
#define AVD_MESSAGE_CLOSURE_M_HH

#ifndef AVD_MESSAGE_CLOSURE_BASE_HH
#include "avd_message_closure_base.hh"
#endif
#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

class mockMessageClosure : public avdMessageClosureBase{
public:
  mockMessageClosure(
    avdMessageTypeBase &type,
    const char *function_name,
    const char *file_name,
    int line_number
  )
  {}
  virtual avdMessageClosureBase &operator<<(char c){ return *this; }
  virtual avdMessageClosureBase &operator<<(unsigned char c){ return *this; }
  virtual avdMessageClosureBase &operator<<(const char *s){ return *this; }
  virtual avdMessageClosureBase &operator<<(int i){ return *this; }
  virtual avdMessageClosureBase &operator<<(unsigned int i){ return *this; }
  virtual avdMessageClosureBase &operator<<(long i){ return *this; }
  virtual avdMessageClosureBase &operator<<(unsigned long i){ return *this; }
  virtual avdMessageClosureBase &operator<<(float f){ return *this; }
  virtual avdMessageClosureBase &operator<<(double f){ return *this; }
  virtual avdMessageClosureBase &operator<<(const void *p){ return *this; }
  virtual avdMessageClosureBase &operator<<(const QString &s){ return *this; }
  virtual avdMessageClosureBase &va(const char *fmt, ...){ return *this; }
  virtual void prefix(void){}
};

#endif

