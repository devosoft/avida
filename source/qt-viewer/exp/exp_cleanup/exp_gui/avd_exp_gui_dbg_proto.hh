#ifndef AVD_EXP_GUI_DBG_PROTO_HH
#define AVD_EXP_GUI_DBG_PROTO_HH

extern avdMessageType  Info_EXP_GUI_avdMsg;
extern avdMessageType Debug_EXP_GUI_avdMsg;
extern avdMessageType Error_EXP_GUI_avdMsg;
extern avdMessageType Fatal_EXP_GUI_avdMsg;
#define  Info  AVD_INFO_MSG(EXP_GUI)
#define Debug AVD_DEBUG_MSG(EXP_GUI)
#define Error AVD_ERROR_MSG(EXP_GUI)
#define Fatal AVD_FATAL_MSG(EXP_GUI)

#endif

// arch-tag: proto file for devel-experimentation debug messages
