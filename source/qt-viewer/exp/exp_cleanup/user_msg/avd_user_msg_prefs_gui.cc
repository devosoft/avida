#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_ABSTRACT_PREFS_GUI_HH
#include "prefs_gui_factory/avd_abstract_prefs_gui.hh"
#endif
#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif
#ifndef AVD_MC_MEDIATOR_HH
#include "mc/avd_mc_mediator.hh"
#endif
#ifndef AVD_PREFS_GUI_FACTORY_HH
#include "prefs_gui_factory/avd_prefs_gui_factory.hh"
#endif
#ifndef AVD_SETTINGS_MEDIATOR_HH
#include "mc/avd_settings_mediator.hh"
#endif
#ifndef AVD_USER_MSG_GUI_DBG_HH
#include "avd_user_msg_gui_dbg.hh"
#endif
#ifndef AVD_USER_MSG_PREFS_CTRL_HH
#include "avd_user_msg_prefs_ctrl.hh"
#endif
#ifndef TPREFSGUI_HH
#include "prefs_gui_factory/tPrefsGUI.hh"
#endif


template <class avdUserMsgPrefsCtrl> class pUserMsgPrefsConnections {
protected:
  void setupConnections(avdMCMediator *med, avdUserMsgPrefsCtrl *ctrl){
    Debug << "<pUserMsgPrefsConnections> entered.";
    QObject::connect(
      ctrl, SIGNAL(userMsgPrefsChangedSig(avdUserMsgCtrlFunct &)),
      med->getSettingsMediator(),
      SIGNAL(userMsgPrefsChangedSig(avdUserMsgCtrlFunct &)));
    QObject::connect(
      med->getSettingsMediator(),
      SIGNAL(requestUserMsgPrefsSig(const avdUserMsgCtrl &)),
      ctrl, SLOT(requestUserMsgPrefsSlot(const avdUserMsgCtrl &)));
} };

typedef tPrefsGUI<avdUserMsgPrefsCtrl, pUserMsgPrefsConnections> avdUserMsgPrefsGUI;

namespace {
  avdAbstractPrefsCtrl *createUserMsgPrefsGUI(){
    Debug << "in createUserMsgPrefsGUI()...";
    avdUserMsgPrefsGUI *g = new avdUserMsgPrefsGUI(0, "<avdUserMsgPrefsGUI>");
    if(!g)
    { Error << "can't create avdUserMsgPrefsGUI.";
      return 0; }
    Debug << "avdUserMsgPrefsGUI *g created ok.";
    g->nameWatched("<avdUserMsgPrefsGUI>");
    Debug << "named *g.";
    if(!g->getCtrl())
    { Error << "can't create avdUserMsgPrefsCtrl.";
      g->release();
      return 0; }
    Debug << "avdUserMsgPrefsCtrl g->getCtrl() is ok.";
    g->getCtrl()->nameWatched("<avdUserMsgPrefsCtrl>");
    Debug << "named g->getCtrl().  finishing happy.";
    return g;
  }
  QString s_key("avdUserMsgPrefsGUI");
  const bool s_registered =
    avdPrefsControllerFactory::Instance().Register(s_key, createUserMsgPrefsGUI);
}

// arch-tag: implementation file for user debug-message display preferences gui factory
