#ifndef AVD_MC_MEDIATOR_HH
#define AVD_MC_MEDIATOR_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif


class avdAvidaStateMediator;
class avdGUIFactoryMediator;
class avdMenuBarMediator;
class avdSettingsMediator;
class avdWindowsMediator;

// XXX FIXME this can probably be removed (see below).
class avdAbstractCtrl;

class avdMCMediator_prv;
class avdMCMediator : public QObject, public cRetainable {
  Q_OBJECT
protected:
  avdMCMediator_prv *m_d;
public:
  avdMCMediator(QObject *parent = 0, const char *name = 0);
  ~avdMCMediator();
  avdMCMediator_prv *setD(avdMCMediator_prv *d);
  avdMCMediator_prv *D(){ return m_d; }
public:
  avdGUIFactoryMediator *getGUIFactoryMediator();
  avdAvidaStateMediator *getAvidaStateMediator();
  avdMenuBarMediator *getMenuBarMediator();
  avdSettingsMediator *getSettingsMediator();
  avdWindowsMediator *getWindowsMediator();
signals:
  // XXX FIXME this can probably be removed, or moved to avdMenuBarMediator.
  void doSetupMyMenu(avdAbstractCtrl *);
};

#endif

// arch-tag: header file for mission-control mediator
