#ifndef AVD_N_ANALYZER_DBG_MSGS_PROTO_HH
#define AVD_N_ANALYZER_DBG_MSGS_PROTO_HH

extern avdMessageType  Info_ANALYZER_avdMsg;
extern avdMessageType Debug_ANALYZER_avdMsg;
extern avdMessageType Error_ANALYZER_avdMsg;
extern avdMessageType Fatal_ANALYZER_avdMsg;
#define  Info  AVD_INFO_MSG(ANALYZER)
#define Debug AVD_DEBUG_MSG(ANALYZER)
#define Error AVD_ERROR_MSG(ANALYZER)
#define Fatal AVD_FATAL_MSG(ANALYZER)

#endif

// arch-tag: proto file for analyzer debug message objects
