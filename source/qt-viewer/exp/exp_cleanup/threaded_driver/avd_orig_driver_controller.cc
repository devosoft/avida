#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif  
  
#ifndef AVIDA_HH
#include "main/avida.hh"
#endif
#ifndef CONFIG_HH
#include "main/config.hh"
#endif
#ifndef ENVIRONMENT_HH
#include "main/environment.hh"
#endif
#ifndef POPULATION_HH
#include "main/population.hh"
#endif

#ifndef AVD_AVIDA_DRIVER_HH
#include "avd_avida_driver.hh"
#endif
#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif
#ifndef AVD_AVIDA_THREAD_DRVR_DBG_HH
#include "avd_avida_thread_drvr_dbg.hh"
#endif
#ifndef SETUP_AVIDA_HH
#include "mc/setup_avida.hh"
#endif
 
#ifndef AVD_ORIG_DRIVER_CONTROLLER_HH
#include "avd_orig_driver_controller.hh"
#endif

using namespace std;


void avdAvidaThreadCtrl::openAvidaSlot(){
  closeAvidaSlot();
  m_environment = new cEnvironment();
  m_test_interface = new cPopulationInterface();
  emit(requestArgvSig(*this));
  SetupAvidaQtMultithreading(m_argc, m_argv, *m_environment, *m_test_interface);
  setThreadDriver(new avdAvidaThreadDrvr(*m_environment));
  if(!getThreadDriver()){
    Fatal << "couldn't create new avida thread driver.";
  }
  getThreadDriver()->release();
  
  cAvidaDriver_Base::main_driver = getThreadDriver();

  avdConnections cnct(this, getThreadDriver());
  cnct.add(SIGNAL(doPauseAvidaSig()), SLOT(doPauseAvidaSlot()));
  cnct.add(SIGNAL(doStartAvidaSig()), SLOT(doStartAvidaSlot()));
  cnct.add(SIGNAL(doUpdateAvidaSig()), SLOT(doUpdateAvidaSlot()));
  cnct.add(SIGNAL(doStepAvidaSig(int)), SLOT(doStepAvidaSlot(int)));
  cnct.add(SIGNAL(doCloseAvidaSig()), SLOT(doCloseAvidaSlot()));

  avdConnections cnct2(getThreadDriver(), this);
  cnct2.add(SIGNAL(avidaUpdatedSig(cAvidaDriver_Population *)),
    SIGNAL(avidaUpdatedSig(cAvidaDriver_Population*)));
  cnct2.add(SIGNAL(avidaSteppedSig(cAvidaDriver_Population *, int)),
    SIGNAL(avidaSteppedSig(cAvidaDriver_Population *, int)));
  cnct2.add(SIGNAL(avidaBreakSig(cAvidaDriver_Population *, int)),
    SIGNAL(avidaBreakSig(cAvidaDriver_Population *, int)));
  cnct2.add(SIGNAL(avidaClosedSig(cAvidaDriver_Population *)),
    SIGNAL(avidaClosedSig(cAvidaDriver_Population *)));
  cnct2.add(SIGNAL(doStartAvidaSig()), SIGNAL(doStartAvidaSig()));

  getThreadDriver()->Run();
  emit(avidaOpenedSig(getThreadDriver()));
}

void avdAvidaThreadCtrl::closeAvidaSlot(){
  emit(doCloseAvidaSig());
  setThreadDriver(0);
  if(m_environment){ delete m_environment; m_environment = 0; }
  if(m_test_interface){ delete m_test_interface; m_test_interface = 0; }
}

void avdAvidaThreadCtrl::setThreadDriver(avdAvidaThreadDrvr *driver)
{ SETretainable(m_driver, driver); }
void avdAvidaThreadCtrl::setArgcArgv(int argc, char **argv)
{ m_argc = argc; m_argv = argv; }

cEnvironment *avdAvidaThreadCtrl::getEnvironment()
{ return m_environment; }
cPopulationInterface * avdAvidaThreadCtrl::getPopulationInterface()
{ return m_test_interface; }

avdAvidaThreadCtrl::avdAvidaThreadCtrl(QObject *parent, const char *name)
: QObject(parent, name),
  m_environment(0),
  m_test_interface(0),
  m_driver(0)
{} 
avdAvidaThreadCtrl::~avdAvidaThreadCtrl(){ closeAvidaSlot(); }

// arch-tag: implementation file for avida thread controller
/* vim: set ts=2 ai et: */
