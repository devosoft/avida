#ifndef AVD_MAP_GUI_DBG_PROTO_HH
#define AVD_MAP_GUI_DBG_PROTO_HH

extern avdMessageType  Info_MAP_GUI_avdMsg;
extern avdMessageType Debug_MAP_GUI_avdMsg;
extern avdMessageType Error_MAP_GUI_avdMsg;
extern avdMessageType Fatal_MAP_GUI_avdMsg;
#define  Info  AVD_INFO_MSG(MAP_GUI)
#define Debug AVD_DEBUG_MSG(MAP_GUI)
#define Error AVD_ERROR_MSG(MAP_GUI)
#define Fatal AVD_FATAL_MSG(MAP_GUI)

#endif

// arch-tag: proto file for map gui debug messages
