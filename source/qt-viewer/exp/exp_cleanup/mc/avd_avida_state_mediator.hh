#ifndef AVD_AVIDA_STATE_MEDIATOR_HH
#define AVD_AVIDA_STATE_MEDIATOR_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif

class cAvidaDriver_Population;
class avdAvidaStateMediator : public QObject {
  Q_OBJECT
public:
  avdAvidaStateMediator(
    QObject *parent = 0, const char *name = "(avdAvidaStateMediator)")
  :QObject(parent, name){}
signals:
  void doStartAvidaSig();
  void doPauseAvidaSig();
  void doUpdateAvidaSig();
  void doStepAvidaSig(int cell_id);
  void doStepAvidaSig();

  void doResetAvidaSig();
  void doOpenAvidaSig();
  void doCloseAvidaSig();
signals:
  void avidaStateChangedSig(cAvidaDriver_Population *pop);
  void avidaUpdatedSig(cAvidaDriver_Population *pop);
  void avidaSteppedSig(cAvidaDriver_Population *pop, int cell_id);
  void avidaBreakSig(cAvidaDriver_Population *pop, int cell_id);
  void avidaOpenedSig(cAvidaDriver_Population *pop);
  void avidaClosedSig(cAvidaDriver_Population *pop);
};

#endif

// arch-tag: header file for avida state mediator
