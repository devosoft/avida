#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
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
#ifndef AVD_PREFS_DBG_HH
#include "avd_prefs_dbg.hh"
#endif
#ifndef AVD_PREFS_CTRL_HH
#include "avd_prefs_ctrl.hh"
#endif
#ifndef AVD_SETTINGS_MEDIATOR_HH
#include "mc/avd_settings_mediator.hh"
#endif
#ifndef AVD_WINDOWS_MEDIATOR_HH
#include "mc/avd_windows_mediator.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif


template <class avdPrefsCtrl> class pPrefsConnections {
protected:
  void setupConnections(avdMCMediator *med, avdPrefsCtrl *ctrl){
    Debug << "<pPrefsConnections> entered.";
    ctrl->setMdtr(med);
    QObject::connect(
      ctrl, SIGNAL(doResetMenuBar(QMenuBar *)),
      med->getMenuBarMediator(), SIGNAL(setupMenuBarSig(QMenuBar *)));
    QObject::connect(
      ctrl, SIGNAL(settingsChangedSig()),
      med->getSettingsMediator(), SIGNAL(settingsChangedSig()));
    QObject::connect(
      med->getWindowsMediator(), SIGNAL(showPrefsGUISig()),
      ctrl, SLOT(showPrefsGUISlot()));
    Debug << "<pPrefsConnections> done.";
} };

typedef tGUI<avdPrefsCtrl, pPrefsConnections> avdPrefsGUI;

namespace {
  avdAbstractCtrl *createPrefsGUI(){
    avdPrefsGUI *g = new avdPrefsGUI(0, "<avdPrefsGUI>");
    if(!g)
    { Error << "can't create avdPrefsGUI.";
      return 0; }
    g->nameWatched("<avdPrefsGUI>");
    if(!g->getCtrl())
    { Error << "can't create avdPrefsCtrl.";
      g->release();
      return 0; }
    g->getCtrl()->nameWatched("<avdPrefsCtrl>");
    return g;
  }
  QString s_key("avdPrefsGUI");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createPrefsGUI);
}

// arch-tag: implementation file for preferences gui factory
