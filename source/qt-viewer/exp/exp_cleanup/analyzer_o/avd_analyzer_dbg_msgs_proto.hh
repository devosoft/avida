#ifndef AVD_ANALYZER_DBG_MSGS_PROTO_HH
#define AVD_ANALYZER_DBG_MSGS_PROTO_HH

extern cMessageType Info_GUI_ANALYZE_Msg;
extern cMessageType Debug_GUI_ANALYZE_Msg;
extern cMessageType Error_GUI_ANALYZE_Msg;
extern cMessageType Fatal_GUI_ANALYZE_Msg;
#define AnGuiInfo _INFO_MSG(GUI_ANALYZE)
#define AnGuiDebug _DEBUG_MSG(GUI_ANALYZE)
#define AnGuiError _ERROR_MSG(GUI_ANALYZE)
#define AnGuiFatal _FATAL_MSG(GUI_ANALYZE)


#endif

// arch-tag: proto file for old analyzer debug message objects
