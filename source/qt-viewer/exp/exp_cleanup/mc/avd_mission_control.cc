#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QMUTEX_H
#include <qmutex.h>
#endif

#ifndef STRING_HH
#include "tools/string.hh"
#endif

#ifndef AVD_MC_DBG_HH
#include "mc/avd_mc_dbg.hh"
#endif
#ifndef AVD_AVIDA_STATE_MEDIATOR_HH
#include "mc/avd_avida_state_mediator.hh"
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
#ifndef AVD_SETTINGS_MEDIATOR_HH
#include "mc/avd_settings_mediator.hh"
#endif
#ifndef REQUESTS_ARGV_HH
#include "mc/attributes/requests_argv.hh"
#endif
#ifndef SCOPEGUARD_H_
#include "memory_mgt/ScopeGuard.h"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "mc/avd_mission_control.hh"
#endif


using namespace std;


void avdMissionControl::setMediator(avdMCMediator *mdtr)
{ SETretainable(m_mdtr, mdtr); }
void avdMissionControl::setMenuBarHdlr(avdAbstractCtrl *menubar_hdlr)
{ SETretainable(m_menubar_hdlr, menubar_hdlr); }
void avdMissionControl::setPrefsCtrl(avdAbstractCtrl *prefs_ctrl)
{ SETretainable(m_prefs_ctrl, prefs_ctrl); }
void avdMissionControl::setConsoleCtrl(avdAbstractCtrl *console_ctrl)
{ SETretainable(m_console_ctrl, console_ctrl); }
void avdMissionControl::setAvidaThreadHdlr(avdAbstractCtrl *avida_thread_hdlr)
{ SETretainable(m_avida_thread_hdlr, avida_thread_hdlr); }

void avdMissionControl::provideArgvSlot(aReqArgcArgv &requestor)
{ requestor.setArgcArgv(argc(), argv()); };
int avdMissionControl::argc(void){ return m_argc; }
char **avdMissionControl::argv(void){ return m_argv; }

void avdMissionControl::quitSlot(void){ QApplication::exit(0); }   

avdAbstractCtrl *avdMissionControl::newGUIController(const QString &ctrl_type){
  avdAbstractCtrl *c = avdControllerFactory::Instance().CreateObject(ctrl_type);
  //if(c){ c->setup(getMediator()); }
  if(!c) {
    Error
    << "Couldn't allocate new user interface of type \"" << ctrl_type << "\".\n"
    << "Normally the user interface factory \"avdControllerFactory\"\n"
    << "creates a user interface of a type specified by name.\n"
    << "In this case, the factory couldn't produce the requested object,\n"
    << "but I'm going to try to continue running anyway.\n";
    return 0;
  }
  ScopeGuard c_guard = MakeObjGuard(*c, &cRetainable::release);
  if(!c->setup(getMediator())){
    Error
    << "Couldn't setup new user interface of type \"" << ctrl_type << "\".\n"
    << "Normally, after the user interface is created, it's told to setup\n"
    << "its visible elements.  In this case, the user interface was\n"
    << "unable to complete its setup, so i'm going to delete the user\n"
    << "interface; then I'm going to try to continue running.\n";
    return 0;
  }
  c_guard.Dismiss();
  return c;
}   

avdMissionControl::avdMissionControl(
  int argc,
  char **argv,
  QObject *parent,
  const char *name
):QObject(parent, name)
{ 
  m_argc = argc;
  m_argv = argv;

  setMediator(new avdMCMediator(0, "<avdMissionControl::m_mdtr>"));
  if(!getMediator())
  { Fatal << "Couldn't create main message mediator (out of memory?)"; }
  getMediator()->release();
  getMediator()->nameWatched("<avdMissionControl::m_mdtr>");

  connect(this, SIGNAL(openAvidaSig()),
    getMediator()->getAvidaStateMediator(),
    SIGNAL(doOpenAvidaSig()));
  connect(getMediator()->getGUIFactoryMediator(),
    SIGNAL(newGUIControllerSig(const QString &)),
    this, SLOT(newGUIControllerSlot(const QString &)));
  connect(getMediator()->getSettingsMediator(),
    SIGNAL(requestArgvSig(aReqArgcArgv &)),
    this, SLOT(provideArgvSlot(aReqArgcArgv &)));

  setMenuBarHdlr(newGUIController("avdMenuBarHdlr"));
  if(!getMenuBarHdlr())
  { Fatal << "Couldn't create menubar handler."; }
  getMenuBarHdlr()->release();
  getMenuBarHdlr()->nameWatched("<avdMissionControl::m_menubar_hdlr>");

  setPrefsCtrl(newGUIController("avdPrefsGUI"));
  if(!getPrefsCtrl())
  { Fatal << "Couldn't create preferences controller."; }
  getPrefsCtrl()->release();
  getPrefsCtrl()->nameWatched("<avdMissionControl::m_prefs_ctrl>");

  setConsoleCtrl(newGUIController("avdUserMsgGUI"));
  if(!getConsoleCtrl())
  { Fatal << "Couldn't create info console controller."; }
  getConsoleCtrl()->release();
  getConsoleCtrl()->nameWatched("<avdMissionControl::m_console_ctrl>");

  setAvidaThreadHdlr(newGUIController("avdAvidaThreadHdlr"));
  if(!getAvidaThreadHdlr())
  { Fatal << "Couldn't create avida thread handler."; }
  getAvidaThreadHdlr()->release();
  getAvidaThreadHdlr()->nameWatched("<avdMissionControl::m_avida_thread_hdlr>");

  emit(openAvidaSig());

  newGUIController("avdDumbGUI");
  //newGUIController("avdUserMsgGUI");
  newGUIController("avdAnalyzerGUI");
}                   

avdMissionControl::~avdMissionControl(){
  setAvidaThreadHdlr(0);
  setConsoleCtrl(0);
  setPrefsCtrl(0);
  setMenuBarHdlr(0);
  setMediator(0);
}   

// arch-tag: implementation file for mission-control
/* vim: set ts=2 ai et: */
