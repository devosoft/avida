#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_DUMB_CTRL_HH
#include "dumb_gui/avd_dumb_ctrl.hh"
#endif
#ifndef AVD_DUMB_GUI_DBG_HH
#include "dumb_gui/avd_dumb_gui_dbg.hh"
#endif
#ifndef AVD_DUMB_VIEW_HH
#include "dumb_gui/avd_dumb_view.hh"
#endif
#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif
#ifndef TGUI_HH
#include "gui_factory/tGUI.hh"
#endif
#ifndef AVD_AVIDA_STATE_MEDIATOR_HH
#include "mc/avd_avida_state_mediator.hh"
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


/* Definitions of debugging messages. */
cMessageType Info_DUMB_GUI_Msg("avdDumbGUI", MCInfo);
cMessageType Debug_DUMB_GUI_Msg("avdDumbGUI", MCDebug);
cMessageType Error_DUMB_GUI_Msg("avdDumbGUI", MCError);
cMessageType Fatal_DUMB_GUI_Msg("avdDumbGUI", MCFatal);

template <class avdDumbCtrl> class pDumbConnections {
protected:
  void setupConnections(avdMCMediator *med, avdDumbCtrl *ctrl){
    avdConnections cnct(ctrl, med->getAvidaStateMediator());
    cnct.add(SIGNAL(doStartAvidaSig()), SIGNAL(doStartAvidaSig()));
    cnct.add(SIGNAL(doPauseAvidaSig()), SIGNAL(doPauseAvidaSig()));
    cnct.add(SIGNAL(doUpdateAvidaSig()), SIGNAL(doUpdateAvidaSig()));
    cnct.add(SIGNAL(doStepAvidaSig(int)), SIGNAL(doStepAvidaSig(int)));
    cnct.add(SIGNAL(doResetAvidaSig()), SIGNAL(doResetAvidaSig()));
    cnct.add(SIGNAL(doOpenAvidaSig()), SIGNAL(doOpenAvidaSig()));
    cnct.add(SIGNAL(doCloseAvidaSig()), SIGNAL(doCloseAvidaSig()));
    QObject::connect(
      ctrl, SIGNAL(doResetMenuBar(QMenuBar *)),
      med->getMenuBarMediator(), SIGNAL(setupMenuBarSig(QMenuBar *)));
    QObject::connect(
      med->getAvidaStateMediator(), SIGNAL(avidaStateChangedSig(cAvidaDriver_Population *)),
      ctrl, SLOT(avidaStateChangedSlot(cAvidaDriver_Population *)));
} };
typedef tGUI<avdDumbCtrl, pDumbConnections> avdDumbGUI;

/* Registers avdDumbGUI creator with GUIFactory. */
namespace {
  avdAbstractCtrl *createDumbGUI(void){
    avdDumbGUI *g = new avdDumbGUI(0, "<(avdDumbGUI)>");
    g->getCtrl()->nameWatched("<createDumbGUI(avdDumbCtrl)>");
    //if(!g) DumbGuiError("couldn't create new avdDumbGUI.");
    return g; }
  QString s_key("avdDumbGUI");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createDumbGUI);
}

// arch-tag: implementation file for dumb gui factory and message display objects
