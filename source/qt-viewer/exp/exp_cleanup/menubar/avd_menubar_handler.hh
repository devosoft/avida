#ifndef AVD_MENUBAR_HANDLER_HH
#define AVD_MENUBAR_HANDLER_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QMenuBar;
class avdMCMediator;
class avdMenuBarCtrl : public QObject, public cRetainable {
  Q_OBJECT
public:
  avdMenuBarCtrl(
    QObject *parent = 0,
    const char *name = 0)
  : QObject(parent, name){}
  bool setup(){return true;}
/*
I wish that popup menu items could emit strings as SIGNAL parameters;
since they can't, I'm using a parameterless SLOT for each of the gui
types that can be created via a menu item.  The menu item calls this
slot, which emits a SIGNAL taking the appropriate string parameter.
*/
public slots:
  void setupMenuBarSlot(QMenuBar *menubar);
  void newAnalyzerControllerSlot();
  void new_o_AnalyzerControllerSlot();
  void newDumbGUI();
  void newExpGUI();
  void newMapGUI();
signals:
  void newGUIControllerSig(const QString &controller_type);
  void showUserMsgGUISig();
  void showPrefsGUISig();
  void doStartAvidaSig();
  void doPauseAvidaSig();
  void doStepAvidaSig();
  void doUpdateAvidaSig();
};

#endif

// arch-tag: header file for menubar handler
