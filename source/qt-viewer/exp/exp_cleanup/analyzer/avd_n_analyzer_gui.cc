#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif
#ifndef AVD_N_ANALYZER_CTRL_HH
#include "avd_n_analyzer_ctrl.hh"
#endif
#ifndef AVD_N_ANALYZER_DBG_MSGS_HH
#include "avd_n_analyzer_dbg_msgs.hh"
#endif
#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif
#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif
#ifndef AVD_MC_MEDIATOR_HH
#include "mc/avd_mc_mediator.hh"
#endif
#ifndef AVD_MENUBAR_MEDIATOR_HH
#include "mc/avd_menubar_mediator.hh"
#endif



avdMessageType  Info_ANALYZER_avdMsg("Analyzer", avdMCInfo);
avdMessageType Debug_ANALYZER_avdMsg("Analyzer", avdMCDebug);
avdMessageType Error_ANALYZER_avdMsg("Analyzer", avdMCError);
avdMessageType Fatal_ANALYZER_avdMsg("Analyzer", avdMCFatal);

template <class avdAnalyzerCtrl> class pAnalyzerConnections {
protected:
  void setupConnections(avdMCMediator *med, avdAnalyzerCtrl *ctrl){
    QObject::connect(
      ctrl, SIGNAL(doResetMenuBar(QMenuBar *)),
      med->getMenuBarMediator(), SIGNAL(setupMenuBarSig(QMenuBar *)));
} };
typedef tGUI<avdAnalyzerCtrl, pAnalyzerConnections> avdAnalyzerGUI;

namespace {
  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Info",   &Info_ANALYZER_avdMsg);
        m_msg_types->Register("Debug", &Debug_ANALYZER_avdMsg);
        m_msg_types->Register("Error", &Error_ANALYZER_avdMsg);
        m_msg_types->Register("Fatal", &Fatal_ANALYZER_avdMsg);
        avdMessageSetTracker::Instance().Register("Analyzer", m_msg_types);
    } }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("Analyzer");
      if(m_msg_types) delete m_msg_types; 
  } };
  const DebugMsgTypeSetRegistration s_registration;
  /* Registers avdAnalyzerGUI creator with GUIFactory. */
  avdAbstractCtrl *createAnalyzerGUI(void){
    avdAnalyzerGUI *g = new avdAnalyzerGUI(0, "<(avdAnalyzerGUI)>");
    g->getCtrl()->nameWatched("<createAnalyzerGUI(avdAnalyzerCtrl)>");
    return g;
  }
  QString s_key("avdAnalyzerGUI");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createAnalyzerGUI);
}       

// arch-tag: implementation file for analyzer gui
