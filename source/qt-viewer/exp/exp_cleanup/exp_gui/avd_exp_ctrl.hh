#ifndef AVD_EXP_CTRL_HH
#define AVD_EXP_CTRL_HH

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
class avdExpView;
class avdExpCtrl : public QObject, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdExpView> m_view;
protected:
  void setView(avdExpView *view);
  avdExpView *getView(){ return m_view; }
public:
  avdExpCtrl(QObject *parent = 0, const char *name = 0);
  ~avdExpCtrl();
  bool setup();
public slots:
  void scopeguardTestBtnClickedSlot();
signals:
  void doResetMenuBar(QMenuBar *);
};

#endif

// arch-tag: header file for devel-experimentation gui controller
