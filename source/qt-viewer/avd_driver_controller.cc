
  
#ifndef AVD_DRIVER_CONTROLLER_HH
#include "avd_driver_controller.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
#ifndef AVD_MESSAGE_QUEUE_HDRS_HH
#include "avd_message_queue_hdrs.hh"
#endif
 
#ifndef AVIDA_HH
#include "avida.hh"
#endif
#ifndef CONFIG_HH
#include "config.hh"
#endif
#ifndef ENVIRONMENT_HH
#include "environment.hh"
#endif
#ifndef POPULATION_HH
#include "population.hh"
#endif
#ifndef AVD_AVIDA_DRIVER_HH
#include "avd_avida_driver.hh"
#endif
#ifndef SETUP_AVIDA_HH
#include "setup_avida.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QOBJECTLIST_H
#include <qobjectlist.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif  
#ifndef QSTRING_H
#include <qstring.h>
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


class avd_DriverControllerData {
private:
  // m_mission_control owns avd_DriverController, not the other way
  // around, so don't delete it.
  avd_MissionControl *m_mission_control;

  // everything else is owned by avd_DriverController, so they should be
  // deleted.
  QGuardedPtr<avd_AvidaDriver> m_avida_driver;
  cEnvironment *m_environment;
  cPopulationInterface *m_test_interface;

public:
  avd_DriverControllerData(avd_MissionControl *mission_control);
  ~avd_DriverControllerData();

  void closeAvida();
  void openAvida(int argc, char **argv);

  avd_MissionControl *getMissionControl();
  QGuardedPtr<avd_AvidaDriver>      getAvidaDriver();
  cEnvironment                     *getEnvironment();
  cPopulationInterface             *getPopulationInterface();

  bool avidaIsRunning();
};

avd_DriverControllerData::avd_DriverControllerData(avd_MissionControl *mission_control)
:m_mission_control(mission_control),
 m_environment(0),
 m_test_interface(0)
{
  GenDebug("entered.");
  GenDebug("done.");
}

avd_DriverControllerData::~avd_DriverControllerData(){
  GenDebug("entered.");
  closeAvida();
  GenDebug("done.");
}

void
avd_DriverControllerData::closeAvida(){
  GenDebug("entered.");
  if(m_avida_driver){
    m_mission_control->emitExitAvidaSig();
    delete m_avida_driver;
    m_avida_driver = 0;
  }
  if(m_environment){
    delete m_environment;
    m_environment = 0;
  }
  if(m_test_interface){
    delete m_test_interface;
    m_test_interface = 0;
  }
  GenDebug("done.");
}

void
avd_DriverControllerData::openAvida(int argc, char **argv){
  GenDebug("entered.");

  closeAvida();

  m_environment = new cEnvironment();
  m_test_interface = new cPopulationInterface();

  //SetupAvida(
  //  argc,
  //  argv,
  //  *m_environment,
  //  *m_test_interface
  //);
  SetupAvidaQtMultithreading(
    argc,
    argv,
    *m_environment,
    *m_test_interface
  );
  m_avida_driver = new avd_AvidaDriver(
    m_mission_control,
    *m_environment 
  );
  cAvidaDriver_Base::main_driver = m_avida_driver;
  m_avida_driver->Run();
  m_mission_control->emitAvidaOpenedSig();

  GenDebug("done.");
}

avd_MissionControl *
avd_DriverControllerData::getMissionControl(){
  GenDebug("entered.");
  GenDebug("done.");
  return m_mission_control;
}

QGuardedPtr<avd_AvidaDriver>
avd_DriverControllerData::getAvidaDriver(){
  GenDebug("entered.");
  GenDebug("done.");
  return m_avida_driver;
}

cEnvironment *
avd_DriverControllerData::getEnvironment(){
  GenDebug("entered.");
  GenDebug("done.");
  return m_environment;
}

cPopulationInterface *
avd_DriverControllerData::getPopulationInterface(){
  GenDebug("entered.");
  GenDebug("done.");
  return m_test_interface;
}

bool
avd_DriverControllerData::avidaIsRunning(){
  GenDebug("entered.");

  bool yesno;
  
  if(m_avida_driver.isNull()){
    GenDebug("no driver; returning false.");
    yesno = false;
  } else {
    yesno = m_avida_driver->avidaIsRunning();
    if(yesno){
      GenDebug("returning true.");
    } else {
      GenDebug("returning false.");
    }
  }
  GenDebug("done.");
  return yesno;
}


    
avd_DriverController::avd_DriverController(
  avd_MissionControl *mission_control,
  QObject *parent,
  const char *name
):QObject(parent, name)
{ 
  GenDebug("entered.");
  d = new avd_DriverControllerData(mission_control);
  connect(
    mission_control, SIGNAL(openAvidaSig(void)),
    this, SLOT(openAvidaSlot(void))
  );
  connect(
    mission_control, SIGNAL(closeAvidaSig(void)),
    this, SLOT(closeAvidaSlot(void))
  );
  GenDebug("done.");
} 
  
avd_DriverController::~avd_DriverController(){
  GenDebug("entered.");
  delete d;
  GenDebug("done.");
}

void
avd_DriverController::wrapupInit(void){
  GenDebug("entered.");
  GenDebug("done.");
}


void
avd_DriverController::openAvidaSlot(void){
  GenDebug("entered.");
  d->openAvida(d->getMissionControl()->argc(), d->getMissionControl()->argv());
  GenDebug("done.");
} 

void
avd_DriverController::closeAvidaSlot(void){
  GenDebug("entered.");
  d->closeAvida();
  GenDebug("done.");
} 

bool
avd_DriverController::avidaIsRunning(){
  GenDebug("entered.");
  bool yesno = d->avidaIsRunning();
  if(yesno){
    GenDebug("returning true.");
  } else {
    GenDebug("returning false.");
  }
  GenDebug("done.");
  return yesno;
}

/* vim: set ts=2 ai et: */
