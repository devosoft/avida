#ifndef AVD_ORIG_DRIVER_CONTROLLER_HH
#define AVD_ORIG_DRIVER_CONTROLLER_HH
  
#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif  

#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif
#ifndef REQUESTS_ARGV_HH
#include "mc/attributes/requests_argv.hh"
#endif


class avdAvidaThreadDrvr;
class avdMCMediator;
class cAvidaDriver_Population;
class cEnvironment;
class cPopulationInterface;
class avdAvidaThreadCtrl
: public QObject
, public cRetainable
, public aReqArgcArgv
{
  Q_OBJECT
protected:
  cEnvironment *m_environment;
  cPopulationInterface *m_test_interface;
  //QGuardedPtr<avdAvidaThreadDrvr> m_driver;
  avdAvidaThreadDrvr *m_driver;
protected:
  void setThreadDriver(avdAvidaThreadDrvr *driver);
  avdAvidaThreadDrvr *getThreadDriver(){ return m_driver; }
public:
  int m_argc; char **m_argv;
  void setArgcArgv(int argc, char **argv);
public:
  avdAvidaThreadCtrl(QObject *parent = 0, const char *name = 0);
  ~avdAvidaThreadCtrl();
  bool setup(){ return true; }
  cEnvironment *getEnvironment();
  cPopulationInterface *getPopulationInterface();
  bool avidaIsRunning();
public slots:
  void openAvidaSlot(void);
  void closeAvidaSlot(void);
signals:
  void doPauseAvidaSig();
  void doStartAvidaSig();
  void doUpdateAvidaSig();
  void doStepAvidaSig(int cell_id);
  void doCloseAvidaSig();

  void avidaUpdatedSig(cAvidaDriver_Population *pop);
  void avidaSteppedSig(cAvidaDriver_Population *pop, int cell_id);
  void avidaBreakSig(cAvidaDriver_Population *pop, int cell_id);
  void avidaOpenedSig(cAvidaDriver_Population *pop);
  void avidaClosedSig(cAvidaDriver_Population *pop);
signals:
  void requestArgvSig(aReqArgcArgv &);
};

#endif
  
// arch-tag: header file for avida thread controller
/* vim: set ts=2 ai et: */
