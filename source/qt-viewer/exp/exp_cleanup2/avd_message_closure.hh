#ifndef AVD_MESSAGE_CLOSURE_HH
#define AVD_MESSAGE_CLOSURE_HH

#ifndef AVD_MESSAGE_CLOSURE_BASE_HH
#include "avd_message_closure_base.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif

class QString;
class avdMessageTypeBase;
class avdMessageClosure : public avdMessageClosureBase{
protected:
  const avdMessageTypeBase &m_type;
  const char *m_function;
  const char *m_file;
  int m_line;
  int m_error;
  int m_op_count;
  cString m_msg;
public:
  avdMessageClosure(
    const avdMessageTypeBase &type,
    const char *function_name,
    const char *file_name,
    int line_number
  );
  ~avdMessageClosure(void);
public:
  virtual avdMessageClosureBase &operator<<(char c);
  virtual avdMessageClosureBase &operator<<(unsigned char c);
  virtual avdMessageClosureBase &operator<<(const char *s);
  virtual avdMessageClosureBase &operator<<(int i);
  virtual avdMessageClosureBase &operator<<(unsigned int i);
  virtual avdMessageClosureBase &operator<<(long i);
  virtual avdMessageClosureBase &operator<<(unsigned long i);
  virtual avdMessageClosureBase &operator<<(float f);
  virtual avdMessageClosureBase &operator<<(double f);
  virtual avdMessageClosureBase &operator<<(const void *p);
  virtual avdMessageClosureBase &operator<<(const QString &s);

  virtual avdMessageClosureBase &va(const char *fmt, ...);
  virtual void prefix(void);
};

#endif
