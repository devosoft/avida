#ifndef AVD_MESSAGE_TYPE_M_HH
#define AVD_MESSAGE_TYPE_M_HH

#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

class avdMessageClassBase;
class mockMessageType : public avdMessageTypeBase{
public:
  mockMessageType()
  {}
  virtual const char *typeName(){ return 0; }
  virtual const avdMessageClassBase &messageClass();
  virtual bool isActive(){ return false; }
  virtual bool showWhere(){ return false; }
  virtual bool showFunction(){ return false; }
  virtual void setIsActive(bool is_active){}
  virtual void setShowWhere(bool show_where){}
  virtual void setShowFunction(bool show_function){}
};

#endif

