#ifndef AVD_MESSAGE_DISPLAY_PROTO_HH
#define AVD_MESSAGE_DISPLAY_PROTO_HH


class avdMessageDisplay{
public:
  virtual void out(const cString &){}
  virtual void abort(){ abort(); }
};

class avdDefaultMessageDisplay : public avdMessageDisplay {
protected:
  std::ostream *m_out;
public:
  avdDefaultMessageDisplay(std::ostream *stream):m_out(stream){}
  void out(const cString &final_msg);
};
extern avdDefaultMessageDisplay s_avd_info_msg_out;
extern avdDefaultMessageDisplay s_avd_debug_msg_out;
extern avdDefaultMessageDisplay s_avd_error_msg_out;
extern avdDefaultMessageDisplay s_avd_fatal_msg_out;
extern avdDefaultMessageDisplay s_avd_plain_msg_out;
void setAvdInfoDisplay(avdMessageDisplay &info);
void setAvdDebugDisplay(avdMessageDisplay &md);
void setAvdErrorDisplay(avdMessageDisplay &md);
void setAvdFatalDisplay(avdMessageDisplay &md);
void setAvdPlainDisplay(avdMessageDisplay &md);

class avdMessageType;
class avdMessageClass{
public:
  avdMessageClass(
    const char *class_name,
    avdMessageDisplay **msg_display,
    bool is_fatal,
    bool is_prefix,
    bool no_prefix
  ):m_class_name(class_name), m_msg_display(msg_display),
  m_is_fatal(is_fatal), m_is_prefix(is_prefix), m_no_prefix(no_prefix){}
public:
  void configure(avdMessageType *message_type);
public:
  const char *const m_class_name;
  avdMessageDisplay **m_msg_display;
  bool const m_is_fatal;
  bool const m_is_prefix;
  bool const m_no_prefix;
};

class avdMessageType{
public:
  avdMessageType(const char *type_name, avdMessageClass &message_class);
public:
  const char *m_type_name;
  const avdMessageClass &m_message_class;
  bool m_is_active;
  bool m_show_where;
  bool m_show_function;
};

class QString;
class avdMessageClosure{
protected:
  avdMessageType &m_type;
  const char *m_function;
  const char *m_file;
  int m_line;
  int m_error;
  int m_op_count;
  cString m_msg;
public:
  avdMessageClosure(
    avdMessageType &type,
    const char *function_name,
    const char *file_name,
    int line_number
  );
  ~avdMessageClosure(void);
public:
  avdMessageClosure &operator<<(char c);
  avdMessageClosure &operator<<(unsigned char c);
  avdMessageClosure &operator<<(const char *s);
  avdMessageClosure &operator<<(int i);
  avdMessageClosure &operator<<(unsigned int i);
  avdMessageClosure &operator<<(long i);
  avdMessageClosure &operator<<(unsigned long i);
  avdMessageClosure &operator<<(float f);
  avdMessageClosure &operator<<(double f);
  avdMessageClosure &operator<<(const void *p);
  avdMessageClosure &operator<<(const QString &s);

  avdMessageClosure &operator()(char c){ return operator<<(c); }
  avdMessageClosure &operator()(unsigned char c){ return operator<<(c); }
  avdMessageClosure &operator()(const char *s){ return operator<<(s); }
  avdMessageClosure &operator()(int i){ return operator<<(i); }
  avdMessageClosure &operator()(unsigned int i){ return operator<<(i); }
  avdMessageClosure &operator()(long i){ return operator<<(i); }
  avdMessageClosure &operator()(unsigned long i){ return operator<<(i); }
  avdMessageClosure &operator()(float f){ return operator<<(f); }
  avdMessageClosure &operator()(double f){ return operator<<(f); }
  avdMessageClosure &operator()(const void *p){ return operator<<(p); }
  avdMessageClosure &operator()(const QString &s){ return operator<<(s); }

  avdMessageClosure &va(const char *fmt, ...);
  void prefix(void);
};

#define AVD_MSG_CLOSURE_FL(x)\
  avdMessageClosure(\
    x,\
    __PRETTY_FUNCTION__,\
    __FILE__,\
    __LINE__\
  )

#define AVD_INFO_MSG(type)\
  if (Info_ ## type ## _avdMsg.m_is_active)\
    AVD_MSG_CLOSURE_FL(Info_ ## type ## _avdMsg)

#define AVD_DEBUG_MSG(type)\
  if (Debug_ ## type ## _avdMsg.m_is_active)\
    AVD_MSG_CLOSURE_FL(Debug_ ## type ## _avdMsg)

#define AVD_ERROR_MSG(type)\
  if (Error_ ## type ## _avdMsg.m_is_active)\
    AVD_MSG_CLOSURE_FL(Error_ ## type ## _avdMsg)

#define AVD_FATAL_MSG(type)\
  if (Fatal_ ## type ## _avdMsg.m_is_active)\
    AVD_MSG_CLOSURE_FL(Fatal_ ## type ## _avdMsg)

#define AVD_PLAIN_MSG(type)\
  if (type ## _avdMsg.m_is_active)\
    AVD_MSG_CLOSURE_FL(type ## _avdMsg)

extern avdMessageClass avdMCInfo;
extern avdMessageClass avdMCDebug;
extern avdMessageClass avdMCError;
extern avdMessageClass avdMCFatal;
extern avdMessageClass avdMCPlain;
extern avdMessageType     Info_GEN_avdMsg;
extern avdMessageType    Debug_GEN_avdMsg;
extern avdMessageType    Error_GEN_avdMsg;
extern avdMessageType    Fatal_GEN_avdMsg;
#define avdInfo   AVD_INFO_MSG(GEN)
#define avdDebug AVD_DEBUG_MSG(GEN)
#define avdError AVD_ERROR_MSG(GEN)
#define avdFatal AVD_FATAL_MSG(GEN)
extern avdMessageType Plain_avdMsg;
#define Message AVD_PLAIN_MSG(Plain)

#endif

// arch-tag: proto file for debug-message display handling
