#ifndef AVD_DUMB_GUI_DBG_PROTO_HH
#define AVD_DUMB_GUI_DBG_PROTO_HH

extern cMessageType Info_DUMB_GUI_Msg;
extern cMessageType Debug_DUMB_GUI_Msg;
extern cMessageType Error_DUMB_GUI_Msg;
extern cMessageType Fatal_DUMB_GUI_Msg;
#define DumbGuiInfo _INFO_MSG(DUMB_GUI)
#define DumbGuiDebug _DEBUG_MSG(DUMB_GUI)
#define DumbGuiError _ERROR_MSG(DUMB_GUI)
#define DumbGuiFatal _FATAL_MSG(DUMB_GUI)

#endif

// arch-tag: proto file for dumb gui message display objects
