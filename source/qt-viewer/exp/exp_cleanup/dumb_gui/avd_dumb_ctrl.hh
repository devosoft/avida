#ifndef AVD_DUMB_CTRL_HH
#define AVD_DUMB_CTRL_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QMenuBar;
class avdDumbView;
class cAvidaDriver_Population;
class avdDumbCtrl : public QObject, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdDumbView> m_view;
protected:
  void setView(avdDumbView *view);
  avdDumbView *getView(){ return m_view; }
public:
  avdDumbCtrl(QObject *parent = 0, const char *name = 0);
  ~avdDumbCtrl();
  bool setup();
public slots:
  void avidaStateChangedSlot(cAvidaDriver_Population *pop);
signals:
  void doStartAvidaSig();
  void doPauseAvidaSig();
  void doUpdateAvidaSig();
  void doStepAvidaSig(int cell_id);

  void doResetAvidaSig();
  void doOpenAvidaSig();
  void doCloseAvidaSig();

  void doResetMenuBar(QMenuBar *);
};

#endif

// arch-tag: header file for dumb gui controller
