#ifndef AVD_MESSAGE_DISPLAY_M_HH
#define AVD_MESSAGE_DISPLAY_M_HH

#ifndef AVD_MESSAGE_DISPLAY_BASE_HH
#include "avd_message_display_base.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif

class cString;
class mockMessageDisplay : public avdMessageDisplayBase{
public:
  mockMessageDisplay()
  {}
  virtual void out(const cString &) const {}
  virtual void abort() const {}
};

#endif
