#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif

#ifndef AVD_PREFS_DBG_HH
#include "avd_prefs_dbg.hh"
#endif

avdMessageType  Info_PREFS_avdMsg("Prefs", avdMCInfo);
avdMessageType Debug_PREFS_avdMsg("Prefs", avdMCDebug);
avdMessageType Error_PREFS_avdMsg("Prefs", avdMCError);
avdMessageType Fatal_PREFS_avdMsg("Prefs", avdMCFatal);
namespace {
  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if(m_msg_types = new avdMessageTypeSet){
        m_msg_types->Register("Info",   &Info_PREFS_avdMsg);  
        m_msg_types->Register("Debug", &Debug_PREFS_avdMsg);  
        m_msg_types->Register("Error", &Error_PREFS_avdMsg);  
        m_msg_types->Register("Fatal", &Fatal_PREFS_avdMsg);  
        avdMessageSetTracker::Instance().Register("Prefs", m_msg_types);
    } }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("Prefs");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;
}

// arch-tag: implementation file for preferences gui debug messages
