#ifndef AVD_MISSION_CONTROL_HH
#define AVD_MISSION_CONTROL_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif

#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class avdAbstractCtrl;
class avdMCMediator;
class aReqArgcArgv;

class avdMissionControl : public QObject, public cRetainable { 
  Q_OBJECT
protected:
  QGuardedPtr<avdMCMediator> m_mdtr;
  avdAbstractCtrl *m_menubar_hdlr;
  avdAbstractCtrl *m_prefs_ctrl;
  avdAbstractCtrl *m_console_ctrl;
  avdAbstractCtrl *m_avida_thread_hdlr;
  int m_argc;
  char **m_argv;
protected:
  void setMediator(avdMCMediator *mdtr);
  void setMenuBarHdlr(avdAbstractCtrl *menubar_hdlr);
  void setPrefsCtrl(avdAbstractCtrl *prefs_ctrl);
  void setConsoleCtrl(avdAbstractCtrl *console_ctrl);
  void setAvidaThreadHdlr(avdAbstractCtrl *avida_thread_hdlr);
  avdMCMediator *getMediator(){ return m_mdtr; }
  avdAbstractCtrl *getMenuBarHdlr(){ return m_menubar_hdlr; }
  avdAbstractCtrl *getPrefsCtrl(){ return m_prefs_ctrl; }
  avdAbstractCtrl *getConsoleCtrl(){ return m_console_ctrl; }
  avdAbstractCtrl *getAvidaThreadHdlr(){ return m_avida_thread_hdlr; }
  int argc(void);
  char **argv(void);
  avdAbstractCtrl *newGUIController(const QString &controller_type);
public:
  avdMissionControl(
    int argc, char **argv, QObject *parent = 0, const char *name = 0);
  ~avdMissionControl();
protected slots:
  void quitSlot(void);
  void newGUIControllerSlot(const QString &controller_type)
  { newGUIController(controller_type); }
  void provideArgvSlot(aReqArgcArgv &requestor);
signals:
  void openAvidaSig(void);
};


#endif

// arch-tag: header file for mission-control
/* vim: set ts=2 ai et: */
