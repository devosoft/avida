#ifndef AVD_PREFS_DBG_PROTO_HH
#define AVD_PREFS_DBG_PROTO_HH

extern avdMessageType  Info_PREFS_avdMsg; 
extern avdMessageType Debug_PREFS_avdMsg;
extern avdMessageType Error_PREFS_avdMsg;
extern avdMessageType Fatal_PREFS_avdMsg;
#define  Info  AVD_INFO_MSG(PREFS)
#define Debug AVD_DEBUG_MSG(PREFS)
#define Error AVD_ERROR_MSG(PREFS)
#define Fatal AVD_FATAL_MSG(PREFS)

#endif

// arch-tag: proto file for preferences gui debug messages
