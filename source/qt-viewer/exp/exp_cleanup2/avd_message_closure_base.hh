#ifndef AVD_MESSAGE_CLOSURE_BASE_HH
#define AVD_MESSAGE_CLOSURE_BASE_HH

class QString;
class avdMessageClosureBase{
public:
  virtual ~avdMessageClosureBase(void){}
  virtual avdMessageClosureBase &operator<<(char c) = 0;
  virtual avdMessageClosureBase &operator<<(unsigned char c) = 0;
  virtual avdMessageClosureBase &operator<<(const char *s) = 0;
  virtual avdMessageClosureBase &operator<<(int i) = 0;
  virtual avdMessageClosureBase &operator<<(unsigned int i) = 0;
  virtual avdMessageClosureBase &operator<<(long i) = 0;
  virtual avdMessageClosureBase &operator<<(unsigned long i) = 0;
  virtual avdMessageClosureBase &operator<<(float f) = 0;
  virtual avdMessageClosureBase &operator<<(double f) = 0;
  virtual avdMessageClosureBase &operator<<(const void *p) = 0;
  virtual avdMessageClosureBase &operator<<(const QString &s) = 0;

  avdMessageClosureBase &operator()(char c){ return operator<<(c); }
  avdMessageClosureBase &operator()(unsigned char c){ return operator<<(c); }
  avdMessageClosureBase &operator()(const char *s){ return operator<<(s); }
  avdMessageClosureBase &operator()(int i){ return operator<<(i); }
  avdMessageClosureBase &operator()(unsigned int i){ return operator<<(i); }
  avdMessageClosureBase &operator()(long i){ return operator<<(i); }
  avdMessageClosureBase &operator()(unsigned long i){ return operator<<(i); }
  avdMessageClosureBase &operator()(float f){ return operator<<(f); }
  avdMessageClosureBase &operator()(double f){ return operator<<(f); }
  avdMessageClosureBase &operator()(const void *p){ return operator<<(p); }
  avdMessageClosureBase &operator()(const QString &s){ return operator<<(s); }

  virtual avdMessageClosureBase &va(const char *fmt, ...) = 0;
  virtual void prefix(void) = 0;
};

#define AVD_MSG_CLOSURE_FL(closure, x)\
  closure(\
    x,\
    __PRETTY_FUNCTION__,\
    __FILE__,\
    __LINE__\
  )

#define AVD_INFO_MSG(closure, type)\
  if (Info_ ## type ## _avdMsg.isActive())\
    AVD_MSG_CLOSURE_FL(closure, Info_ ## type ## _avdMsg)

#define AVD_DEBUG_MSG(closure, type)\
  if (Debug_ ## type ## _avdMsg.isActive())\
    AVD_MSG_CLOSURE_FL(closure, Debug_ ## type ## _avdMsg)

#define AVD_ERROR_MSG(closure, type)\
  if (Error_ ## type ## _avdMsg.isActive())\
    AVD_MSG_CLOSURE_FL(closure, Error_ ## type ## _avdMsg)

#define AVD_FATAL_MSG(closure, type)\
  if (Fatal_ ## type ## _avdMsg.isActive())\
    AVD_MSG_CLOSURE_FL(closure, Fatal_ ## type ## _avdMsg)

#define AVD_PLAIN_MSG(closure, type)\
  if (type ## _avdMsg.isActive())\
    AVD_MSG_CLOSURE_FL(closure, type ## _avdMsg)

#endif
