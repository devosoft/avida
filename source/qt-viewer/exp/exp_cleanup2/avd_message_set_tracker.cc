#ifndef AVD_MESSAGE_SET_TRACKER_HH
#include "avd_message_set_tracker.hh"
#endif

#ifndef AVD_DBG_MSGS_HH
#include "avd_dbg_msgs.hh"
#endif
#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

#include <iostream>

using namespace std;

/*
A private namespace containing load-time initializations; these register
the default error types with the avdMessageSetTracker singleton.  (The
avdMessageSetTracker singleton is used in the GUI by the log window to
keep track of how messages of a given type should be displayed).
*/
namespace {
  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Plain",   &Plain_avdMsg);
        m_msg_types->Register("Info",   &Info_GEN_avdMsg);
        m_msg_types->Register("Debug", &Debug_GEN_avdMsg);
        m_msg_types->Register("Error", &Error_GEN_avdMsg);
        m_msg_types->Register("Fatal", &Fatal_GEN_avdMsg);
        avdMessageSetTracker::Instance().Register("General", m_msg_types);
      } else {
        cerr << "* Error in message display system *" << endl;
        cerr << "* in class DebugMsgTypeSetRegistration constructor *" << endl;
        cerr << endl;
        cerr << "Couldn't allocate \"avdMessageTypeSet\" object for use in" << endl;
        cerr << "registering general message types \"Plain\", \"Info\"," << endl;
        cerr << "\"Debug\", \"Error\", and \"Fatal\" with the message-type" << endl;
        cerr << "tracking system." << endl;
        cerr << "Normally the message-type tracking system is used by a log" << endl;
        cerr << "window, available in the graphic user interface, to" << endl;
        cerr << "determine how messages of a given type should be" << endl;
        cerr << "displayed to the user." << endl;
        cerr << endl;
      }
    }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("General");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;
}
