#ifndef AVD_DBG_MSGS_HH
#include "avd_dbg_msgs.hh"
#endif

#ifndef AVD_MESSAGE_CLASS_BASE_HH
#include "avd_message_class_base.hh"
#endif 
#ifndef AVD_MESSAGE_TYPE_HH
#include "avd_message_type.hh"
#endif
#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

static avdMessageType s_plain_message_type("General", avdMCPlain);
const avdMessageTypeBase &Plain_avdMsg(s_plain_message_type);

static avdMessageType  s_info_message_type("General", avdMCInfo);
static avdMessageType s_debug_message_type("General", avdMCDebug);
static avdMessageType s_error_message_type("General", avdMCError);
static avdMessageType s_fatal_message_type("General", avdMCFatal);
const avdMessageTypeBase  &Info_GEN_avdMsg(s_info_message_type);
const avdMessageTypeBase &Debug_GEN_avdMsg(s_debug_message_type);
const avdMessageTypeBase &Error_GEN_avdMsg(s_error_message_type);
const avdMessageTypeBase &Fatal_GEN_avdMsg(s_fatal_message_type);
