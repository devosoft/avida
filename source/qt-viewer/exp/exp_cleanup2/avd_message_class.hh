#ifndef AVD_MESSAGE_CLASS_HH
#define AVD_MESSAGE_CLASS_HH

#ifndef AVD_MESSAGE_CLASS_BASE_HH
#include "avd_message_class_base.hh"
#endif

class avdMessageClass : public avdMessageClassBase{
public:
  avdMessageClass(
    const char *class_name,
    avdMessageDisplayBase **msg_display,
    bool is_fatal,
    bool is_prefix,
    bool no_prefix
  );
public:
  virtual const char *const className() const { return m_class_name; }
  virtual avdMessageDisplayBase **messageDisplay() const { return m_msg_display; }
  virtual bool isFatal() const { return m_is_fatal; }
  virtual bool isPrefix() const { return m_is_prefix; }
  virtual bool noPrefix() const { return m_no_prefix; }
private:
  const char *const m_class_name;
  avdMessageDisplayBase **m_msg_display;
  bool const m_is_fatal;
  bool const m_is_prefix;
  bool const m_no_prefix;
};

#endif
