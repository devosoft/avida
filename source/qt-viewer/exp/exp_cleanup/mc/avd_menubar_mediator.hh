#ifndef AVD_MENUBAR_MEDIATOR_HH
#define AVD_MENUBAR_MEDIATOR_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QMenuBar;
class avdAbstractCtrl;
class avdMenuBarMediator : public QObject {
  Q_OBJECT
public:
  avdMenuBarMediator(
    QObject *parent = 0, const char *name = "(avdMenuBarMediator)")
  :QObject(parent, name){}
signals:
  void setupMenuBarSig(QMenuBar *);
};

#endif

// arch-tag: header file for menu bar mediator
