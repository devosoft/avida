#ifndef AVD_AVIDA_STATE_MEDIATOR_HH
#include "mc/avd_avida_state_mediator.hh"
#endif
#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif
#ifndef AVD_GUI_FACTORY_MEDIATOR_HH
#include "mc/avd_gui_factory_mediator.hh"
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
#ifndef AVD_WINDOWS_MEDIATOR_HH
#include "mc/avd_windows_mediator.hh"
#endif
#ifndef AVD_MENUBAR_HANDLER_HH
#include "avd_menubar_handler.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif

template <class avdMenuBarCtrl> class pMenuBarConnections {
protected:
  void setupConnections(avdMCMediator *med, avdMenuBarCtrl *hdlr){
    avdConnections cnct(hdlr, med->getAvidaStateMediator());
    cnct.add(SIGNAL(doStartAvidaSig()), SIGNAL(doStartAvidaSig()));
    cnct.add(SIGNAL(doPauseAvidaSig()), SIGNAL(doPauseAvidaSig()));
    cnct.add(SIGNAL(doUpdateAvidaSig()), SIGNAL(doUpdateAvidaSig()));
    cnct.add(SIGNAL(doStepAvidaSig()), SIGNAL(doStepAvidaSig()));

    avdConnections cnct2(hdlr, med->getWindowsMediator());
    cnct2.add(SIGNAL(showUserMsgGUISig()), SIGNAL(showUserMsgGUISig()));
    cnct2.add(SIGNAL(showPrefsGUISig()), SIGNAL(showPrefsGUISig()));

    QObject::connect(
      hdlr, SIGNAL(newGUIControllerSig(const QString &)),
      med->getGUIFactoryMediator(),
      SIGNAL(newGUIControllerSig(const QString &)));
    QObject::connect(
      med->getMenuBarMediator(), SIGNAL(setupMenuBarSig(QMenuBar *)),
      hdlr, SLOT(setupMenuBarSlot(QMenuBar *)));
} };
typedef tGUI<avdMenuBarCtrl, pMenuBarConnections> avdMenuBarHdlr;

/* Registers avdDumbGUI creator with GUIFactory. */
namespace {
  avdAbstractCtrl *createMenuBarHdlr(void){
    avdMenuBarHdlr *m = new avdMenuBarHdlr(0, "<avdMenuBarHdlr>");
    m->getCtrl()->nameWatched("<avdMenuBarCtrl>");
    m->nameWatched("<avdMenuBarHdlr>");
    return m; }
  QString s_key("avdMenuBarHdlr");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createMenuBarHdlr);
}

// arch-tag: implementation file for menubar handler factory
