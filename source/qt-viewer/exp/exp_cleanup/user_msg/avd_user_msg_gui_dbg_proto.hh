#ifndef AVD_USER_MSG_GUI_DBG_PROTO_HH
#define AVD_USER_MSG_GUI_DBG_PROTO_HH

extern avdMessageType  Info_USER_MSG_avdMsg;
extern avdMessageType Debug_USER_MSG_avdMsg;
extern avdMessageType Error_USER_MSG_avdMsg;
extern avdMessageType Fatal_USER_MSG_avdMsg;
#define  Info  AVD_INFO_MSG(USER_MSG)
#define Debug AVD_DEBUG_MSG(USER_MSG)
#define Error AVD_ERROR_MSG(USER_MSG)
#define Fatal AVD_FATAL_MSG(USER_MSG)

#endif

// arch-tag: proto file for user debug-message display gui debug messages
