#ifndef AVD_AVIDA_THREAD_DRVR_DBG_PROTO_HH
#define AVD_AVIDA_THREAD_DRVR_DBG_PROTO_HH

extern avdMessageType  Info_THRD_DRVR_avdMsg;
extern avdMessageType Debug_THRD_DRVR_avdMsg;
extern avdMessageType Error_THRD_DRVR_avdMsg;
extern avdMessageType Fatal_THRD_DRVR_avdMsg;
#define  Info  AVD_INFO_MSG(THRD_DRVR)
#define Debug AVD_DEBUG_MSG(THRD_DRVR)
#define Error AVD_ERROR_MSG(THRD_DRVR)
#define Fatal AVD_FATAL_MSG(THRD_DRVR)

#endif

// arch-tag: proto file for avida processing thread debug messages
