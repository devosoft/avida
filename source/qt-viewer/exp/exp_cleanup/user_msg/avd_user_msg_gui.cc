#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QMENUBAR_H
#include <qmenubar.h>
#endif

#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif
#ifndef AVD_USER_MSG_GUI_DBG_HH
#include "avd_user_msg_gui_dbg.hh"
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
#ifndef AVD_SETTINGS_MEDIATOR_HH
#include "mc/avd_settings_mediator.hh"
#endif
#ifndef AVD_WINDOWS_MEDIATOR_HH
#include "mc/avd_windows_mediator.hh"
#endif
#ifndef AVD_USER_MSG_CTRL_HH
#include "avd_user_msg_ctrl.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif


avdMessageType  Info_USER_MSG_avdMsg("UsrMsgSys", avdMCInfo);
avdMessageType Debug_USER_MSG_avdMsg("UsrMsgSys", avdMCDebug);
avdMessageType Error_USER_MSG_avdMsg("UsrMsgSys", avdMCError);
avdMessageType Fatal_USER_MSG_avdMsg("UsrMsgSys", avdMCFatal);

template <class avdUserMsgCtrl> class pUserMsgCtrlConnections {
protected:
  void setupConnections(avdMCMediator *med, avdUserMsgCtrl *ctrl){
    Debug << "setupConnections entered.";
    QObject::connect(
      ctrl, SIGNAL(doResetMenuBar(QMenuBar *)),
      med->getMenuBarMediator(), SIGNAL(setupMenuBarSig(QMenuBar *)));
    QObject::connect(
      med->getSettingsMediator(),
      SIGNAL(userMsgPrefsChangedSig(avdUserMsgCtrlFunct &)),
      ctrl, SLOT(userMsgPrefsChangedSlot(avdUserMsgCtrlFunct &)));
    QObject::connect(
      ctrl, SIGNAL(requestUserMsgPrefsSig(const avdUserMsgCtrl &)),
      med->getSettingsMediator(),
      SIGNAL(requestUserMsgPrefsSig(const avdUserMsgCtrl &)));
    QObject::connect(
      med->getWindowsMediator(), SIGNAL(showUserMsgGUISig()),
      ctrl, SLOT(showUserMsgGUISlot()));
    Debug << "setupConnections done.";
} };

typedef tGUI<avdUserMsgCtrl, pUserMsgCtrlConnections> avdUserMsgGUI;

namespace {
  avdAbstractCtrl *createUserMsgGUI(void){
    avdUserMsgGUI *g = new avdUserMsgGUI(0, "<(avdUserMsgGUI)>");
    g->nameWatched("<(avdUserMsgGUI)>");
    g->getCtrl()->nameWatched("<(avdUserMsgCtrl)>");
    return g;
  }
  QString s_key("avdUserMsgGUI");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createUserMsgGUI);

  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Info",   &Info_USER_MSG_avdMsg);  
        m_msg_types->Register("Debug", &Debug_USER_MSG_avdMsg);  
        m_msg_types->Register("Error", &Error_USER_MSG_avdMsg);  
        m_msg_types->Register("Fatal", &Fatal_USER_MSG_avdMsg);  
        avdMessageSetTracker::Instance().Register("UsrMsgSys", m_msg_types);
    } }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("UsrMsgSys");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;
}

// arch-tag: implementation file for user debug-message display gui factory and debug messages
