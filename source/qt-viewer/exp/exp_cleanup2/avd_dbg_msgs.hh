#ifndef AVD_DBG_MSGS_HH
#define AVD_DBG_MSGS_HH

#ifndef AVD_MESSAGE_CLOSURE_HH
#include "avd_message_closure.hh"
#endif
#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

extern const avdMessageTypeBase             &Info_GEN_avdMsg;
extern const avdMessageTypeBase            &Debug_GEN_avdMsg;
extern const avdMessageTypeBase            &Error_GEN_avdMsg;
extern const avdMessageTypeBase            &Fatal_GEN_avdMsg;
#define avdInfo   AVD_INFO_MSG(avdMessageClosure, GEN)
#define avdDebug AVD_DEBUG_MSG(avdMessageClosure, GEN)
#define avdError AVD_ERROR_MSG(avdMessageClosure, GEN)
#define avdFatal AVD_FATAL_MSG(avdMessageClosure, GEN)

extern const avdMessageTypeBase &Plain_avdMsg;
#define Message AVD_PLAIN_MSG(avdMessageClosure, Plain)

#endif
