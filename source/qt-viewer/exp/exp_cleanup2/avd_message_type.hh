#ifndef AVD_MESSAGE_TYPE_HH
#define AVD_MESSAGE_TYPE_HH

#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

class avdMessageClassBase;
class avdMessageDisplayBase;
class avdMessageType : public avdMessageTypeBase{
public:
  avdMessageType(const char *type_name, avdMessageClassBase &message_class);
  virtual const char *const typeName() const { return m_type_name; }
  virtual const char *const className() const { return m_class_name; }
  virtual bool isFatal() const { return m_is_fatal; }
  virtual bool isPrefix() const { return m_is_prefix; }
  virtual bool noPrefix() const { return m_no_prefix; }
  virtual bool isActive() const { return m_is_active; }
  virtual bool showWhere() const { return m_show_where; }
  virtual bool showFunction() const { return m_show_function; }
  virtual avdMessageDisplayBase **messageDisplay() const { return m_msg_display; }
  virtual void setIsActive(bool is_active){ m_is_active = is_active; }
  virtual void setShowWhere(bool show_where){ m_show_where = show_where; }
  virtual void setShowFunction(bool show_function){ m_show_function = show_function; }
private:
  const char *const m_type_name;
  const char *const m_class_name;
  bool const m_is_fatal;
  bool const m_is_prefix;
  bool const m_no_prefix;
  bool m_is_active;
  bool m_show_where;
  bool m_show_function;
  avdMessageDisplayBase **m_msg_display;
};

#endif
