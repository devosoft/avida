#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif

#ifndef AVD_MC_DBG_HH
#include "avd_mc_dbg.hh"
#endif

avdMessageType  Info_MC_avdMsg("MainController", avdMCInfo);
avdMessageType Debug_MC_avdMsg("MainController", avdMCDebug);
avdMessageType Error_MC_avdMsg("MainController", avdMCError);
avdMessageType Fatal_MC_avdMsg("MainController", avdMCFatal);
namespace {
  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Info",   &Info_MC_avdMsg);  
        m_msg_types->Register("Debug", &Debug_MC_avdMsg);  
        m_msg_types->Register("Error", &Error_MC_avdMsg);  
        m_msg_types->Register("Fatal", &Fatal_MC_avdMsg);  
        avdMessageSetTracker::Instance().Register("MainController", m_msg_types);
    } }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("MainController");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;
}

// arch-tag: implementation file for mission-control factory and debug messages
