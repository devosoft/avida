#ifndef AVD_MESSAGE_CLASS_HH
#include "avd_message_class.hh"
#endif

#ifndef AVD_MESSAGE_CLASS_BASE_HH
#include "avd_message_class_base.hh"
#endif 
#ifndef AVD_MESSAGE_DISPLAY_BASE_HH
#include "avd_message_display_base.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif

#ifndef _CPP_IOSTREAM
#include <iostream>
#endif

using namespace std;

avdMessageClass::avdMessageClass(
  const char *class_name,
  avdMessageDisplayBase **msg_display,
  bool is_fatal,
  bool is_prefix,
  bool no_prefix
):m_class_name(class_name)
, m_msg_display(msg_display)
, m_is_fatal(is_fatal)
, m_is_prefix(is_prefix)
, m_no_prefix(no_prefix)
{}
static avdMessageClass  s_info_message_class("Info:",  &s_avd_info_display, false, false, false);
static avdMessageClass s_debug_message_class("Debug:", &s_avd_debug_display, false, false, false);
static avdMessageClass s_error_message_class("Error:", &s_avd_error_display, false, true, false);
static avdMessageClass s_fatal_message_class("Fatal:", &s_avd_fatal_display, true, true, false);
static avdMessageClass s_plain_message_class("Plain:", &s_avd_plain_display, false, true, true);
avdMessageClassBase   &avdMCInfo(s_info_message_class);
avdMessageClassBase &avdMCDebug(s_debug_message_class);
avdMessageClassBase &avdMCError(s_error_message_class);
avdMessageClassBase &avdMCFatal(s_fatal_message_class);
avdMessageClassBase &avdMCPlain(s_plain_message_class);
