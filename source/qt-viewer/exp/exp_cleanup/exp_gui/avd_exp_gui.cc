#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif
#ifndef AVD_EXP_CTRL_HH
#include "avd_exp_ctrl.hh"
#endif
#ifndef AVD_EXP_GUI_DBG_HH
#include "avd_exp_gui_dbg.hh"
#endif
#ifndef AVD_EXP_VIEW_HH
#include "avd_exp_view.hh"
#endif
#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif
#ifndef AVD_MC_MEDIATOR_HH
#include "mc/avd_mc_mediator.hh"
#endif
#ifndef AVD_MENUBAR_MEDIATOR_HH
#include "mc/avd_menubar_mediator.hh"
#endif
#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif


/* Definitions of debugging messages. */
avdMessageType  Info_EXP_GUI_avdMsg("ExpGUI", avdMCInfo);
avdMessageType Debug_EXP_GUI_avdMsg("ExpGUI", avdMCDebug);
avdMessageType Error_EXP_GUI_avdMsg("ExpGUI", avdMCError);
avdMessageType Fatal_EXP_GUI_avdMsg("ExpGUI", avdMCFatal);

template <class avdExpCtrl> class pExpConnections {
protected:
  void setupConnections(avdMCMediator *med, avdExpCtrl *ctrl){
    QObject::connect(
      ctrl, SIGNAL(doResetMenuBar(QMenuBar *)),
      med->getMenuBarMediator(), SIGNAL(setupMenuBarSig(QMenuBar *)));
} };
typedef tGUI<avdExpCtrl, pExpConnections> avdExpGUI;

namespace {
  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Info",   &Info_EXP_GUI_avdMsg);
        m_msg_types->Register("Debug", &Debug_EXP_GUI_avdMsg);
        m_msg_types->Register("Error", &Error_EXP_GUI_avdMsg);
        m_msg_types->Register("Fatal", &Fatal_EXP_GUI_avdMsg);
        avdMessageSetTracker::Instance().Register("ExpGUI", m_msg_types);
    } }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("ExpGUI");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;

  /* Registers avdExpGUI creator with GUIFactory. */
  avdAbstractCtrl *createExpGUI(void){
    avdExpGUI *g = new avdExpGUI(0, "<(avdExpGUI)>");
    g->getCtrl()->nameWatched("<createExpGUI(avdExpCtrl)>");
    return g;
  }
  QString s_key("avdExpGUI");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createExpGUI);
}

// arch-tag: implementation file for devel-experimentation gui factory and debug messages
