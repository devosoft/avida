#ifndef AVD_MESSAGE_DISPLAY_HH
#define AVD_MESSAGE_DISPLAY_HH

#ifndef _CPP_IOSTREAM
#include <iostream>
#endif

#ifndef AVD_MESSAGE_DISPLAY_BASE_HH
#include "avd_message_display_base.hh"
#endif

class cString;
class avdMessageDisplay : public avdMessageDisplayBase {
protected:
  std::ostream *m_out;
public:
  avdMessageDisplay(std::ostream *stream):m_out(stream){}
  void out(const cString &final_msg) const;
};
extern avdMessageDisplay s_avd_info_msg_out;
extern avdMessageDisplay s_avd_debug_msg_out;
extern avdMessageDisplay s_avd_error_msg_out;
extern avdMessageDisplay s_avd_fatal_msg_out;
extern avdMessageDisplay s_avd_plain_msg_out;

#endif
