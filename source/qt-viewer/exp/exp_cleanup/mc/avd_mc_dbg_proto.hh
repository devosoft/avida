#ifndef AVD_MC_DBG_PROTO_HH
#define AVD_MC_DBG_PROTO_HH

extern avdMessageType  Info_MC_avdMsg;
extern avdMessageType Debug_MC_avdMsg;
extern avdMessageType Error_MC_avdMsg;
extern avdMessageType Fatal_MC_avdMsg;
#define  Info  AVD_INFO_MSG(MC)
#define Debug AVD_DEBUG_MSG(MC)
#define Error AVD_ERROR_MSG(MC)
#define Fatal AVD_FATAL_MSG(MC)

#endif

// arch-tag: proto file for mission-control debug messages
