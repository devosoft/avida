
  
#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
  
//#ifndef MAIN_WIDGET_HH
//#include "main_widget_controller.hh"
//#endif
//#ifndef HELP_CONTROLLER_HH
//#include "help_controller.hh"
//#endif
//#ifndef SETTINGS_CONTROLLER_HH
//#include "settings_controller.hh"
//#endif
#ifndef AVD_DRIVER_CONTROLLER_HH
#include "avd_driver_controller.hh"
#endif
#ifndef AVD_MAIN_WINDOW_CONTROLLER_HH
#include "avd_main_window_controller.hh"
#endif
#ifndef AVD_INSTRUCTIONS_VIEW_CONTROLLER_HH
#include "avd_instruction_viewer_controller.hh"
#endif
#ifndef AVD_EVENT_VIEWER_CONTROLLER_HH
#include "avd_event_viewer_controller.hh"
#endif
#ifndef AVD_GODBOX_CONTROLLER_HH
#include "avd_godbox_controller.hh"
#endif
#ifndef AVD_PLOT_VIEWER_CONTROLLER_HH
#include "avd_plot_viewer_controller.hh"
#endif
#ifndef AVD_HELP_CONTROLLER_HH
#include "avd_help_controller.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QMUTEX_H
#include <qmutex.h>
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


struct avd_MissionControlData {
  //QGuardedPtr<avd_HelpController> m_help_controller;
  //QGuardedPtr<avd_MainWidgetController> m_main_widget_controller;
  //QGuardedPtr<avd_SettingsController> m_settings_controller;

  QGuardedPtr<avd_DriverController> m_driver_controller;
  QGuardedPtr<avd_MainWindowController> m_main_window_controller;
  QGuardedPtr<avd_InstructionViewerController> m_instruction_viewer_controller;
  QGuardedPtr<avd_EventViewerController> m_event_viewer_controller;
  QGuardedPtr<avd_GodBoxController> m_godbox_controller;
  QGuardedPtr<avd_PlotViewerController> m_plot_viewer_controller;
  QGuardedPtr<avd_HelpController> m_help_controller;

  int m_argc;
  char **m_argv;

  QMutex *m_mutex;

  cPopulation *m_population;
  cEventList *m_event_list;

  int m_selected_population_cell_id;
};


avd_MissionControl::avd_MissionControl(
  int argc,
  char **argv,
  QObject *parent,
  const char *name
):QObject(parent, name)
{ 
  GenDebug("entered.");
  
  d = new avd_MissionControlData;

  d->m_argc = argc;
  d->m_argv = argv;

  d->m_mutex = new QMutex(false); // parameter false means create nonrecursive mutex.
  d->m_population = 0;
  d->m_event_list = 0;
  d->m_selected_population_cell_id = 0;

  //d->m_help_controller = new avd_HelpController(
  //  this,
  //  0, 
  //  "<avd_MissionControl::m_help_controller>"
  //);
  //d->m_settings_controller = new avd_SettingsController(
  //  this,
  //  0,
  //  "<avd_MissionControl::m_settings_controller>"
  //);
  //d->m_main_widget_controller = new avd_MainWidgetController(
  //  this,
  //  0, 
  //  "<avd_MissionControl::m_main_widget_controller>"
  //);

  d->m_driver_controller = new avd_DriverController(
    this,
    0, 
    "<avd_MissionControl::m_driver_controller>"
  );
  d->m_main_window_controller = new avd_MainWindowController(
    *this,
    0, 
    "<avd_MissionControl::m_main_window_controller>"
  );
  d->m_instruction_viewer_controller = new avd_InstructionViewerController(
    *this,
    0, 
    "<avd_MissionControl::m_instruction_viewer_controller>"
  );
  d->m_event_viewer_controller = new avd_EventViewerController(
    *this,
    0, 
    "<avd_MissionControl::m_event_viewer_controller>"
  );
  //d->m_godbox_controller = new avd_GodBoxController(
  //  *this,
  //  0, 
  //  "<avd_MissionControl::m_godbox_controller>"
  //);
  d->m_plot_viewer_controller = new avd_PlotViewerController(
    *this,
    0, 
    "<avd_MissionControl::m_plot_viewer_controller>"
  );
  d->m_help_controller = new avd_HelpController(
    *this,
    0, 
    "<avd_MissionControl::m_help_controller>"
  );


  //d->m_help_controller->setMissionControl(this);
  //d->m_settings_controller->setMissionControl(this);
  //d->m_main_widget_controller->setMissionControl(this);

  //d->m_driver_controller->setMissionControl(this);
  //d->m_main_window_controller->setMissionControl(this);


  //d->m_help_controller->wrapupInit();
  //d->m_settings_controller->wrapupInit();
  //d->m_main_widget_controller->wrapupInit();

  d->m_driver_controller->wrapupInit();
  d->m_main_window_controller->wrapupInit();
  d->m_instruction_viewer_controller->wrapupInit();
  //d->m_event_viewer_controller->wrapupInit();
  d->m_godbox_controller->wrapupInit();
  d->m_plot_viewer_controller->wrapupInit();
  d->m_help_controller->wrapupInit();

  emitOpenAvidaSig();

  GenDebug("done.");
}                   
                    
avd_MissionControl::~avd_MissionControl(){
  GenDebug("entered.");
  
  //GenDebug("delete m_main_widget_controller");
  //if(d->m_main_widget_controller) delete d->m_main_widget_controller;
  //GenDebug("delete m_settings_controller");
  //if(d->m_settings_controller) delete d->m_settings_controller;
  //GenDebug("delete m_help_controller");
  //if(d->m_help_controller) delete d->m_help_controller;

  GenDebug("delete m_driver_controller");
  delete d->m_driver_controller;
  GenDebug("delete m_main_window_controller");
  delete d->m_main_window_controller;
  GenDebug("delete m_instruction_viewer_controller");
  delete d->m_instruction_viewer_controller;
  GenDebug("delete m_event_viewer_controller");
  delete d->m_event_viewer_controller;
  GenDebug("delete m_godbox_controller");
  delete d->m_godbox_controller;
  GenDebug("delete m_plot_viewer_controller");
  delete d->m_plot_viewer_controller;
  GenDebug("delete m_help_controller");
  delete d->m_help_controller;

  delete d->m_mutex;

  delete d;
    
  GenDebug("done.");
  QApplication::exit(0);
}   


/*
obtaining original command-line arguments.  these can be used again as
often as needed as long as you don't clobber argv.  i'm hoping to use
them to enable reset of avida core.
*/
int
avd_MissionControl::argc(void){
  return d->m_argc;
}

/*
FIXME:
Maybe this should return a copy of the original argv.
This would require some memory management to prevent memory leak.
*/
char **
avd_MissionControl::argv(void){
  return d->m_argv;
}

/*
mutex management functions.  for protecting avida core data from
reentrance when shared between gui and driver threads.  also used to
protect a message queue for communication between the two threads.
*/
void avd_MissionControl::lock(void){ d->m_mutex->lock(); }
bool avd_MissionControl::locked(void){ return d->m_mutex->locked(); }
bool avd_MissionControl::tryLock(void){ return d->m_mutex->tryLock(); }
void avd_MissionControl::unlock(void){ d->m_mutex->unlock(); }
QMutex * avd_MissionControl::getMutex(void){ return d->m_mutex; }

/*
*/
bool
avd_MissionControl::avidaIsRunning(){
  return d->m_driver_controller->avidaIsRunning();
}

/*
*/
void
avd_MissionControl::setPopulation(cPopulation *population){
  d->m_population = population;
}
void
avd_MissionControl::setEventList(cEventList *event_list){
  d->m_event_list = event_list;
}
void
avd_MissionControl::setSelectedPopulationCellID(int cell_id){
  d->m_selected_population_cell_id = cell_id;
}
cPopulation *
avd_MissionControl::getPopulation(void){
  return d->m_population;
}
cEventList *
avd_MissionControl::getEventList(void){
  return d->m_event_list;
}
int
avd_MissionControl::getSelectedPopulationCellID(void){
  return d->m_selected_population_cell_id;
}





void
avd_MissionControl::quitSlot(void){
  GenDebug("entered.");
  delete this;
  GenDebug("done.");
}   


// for triggering gui messages.
void
avd_MissionControl::emitHelpURLSig(const QString &rel_path){
  GenDebug("entered.");
  emit helpURLSig(rel_path);
  GenDebug("done.");
}
//void
//avd_MissionControl::emitNewMainWidgetSig(void){
//  GenDebug("entered.");
//  emit newMainWidgetSig();
//  GenDebug("done.");
//}
void
avd_MissionControl::emitUserSettingsSig(void){
  GenDebug("entered.");
  emit userSettingsSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitOpenAvidaSig(void){
  GenDebug("entered.");
  emit openAvidaSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitCloseAvidaSig(void){
  GenDebug("entered.");
  emit closeAvidaSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitResetAvidaSig(void){
  GenDebug("entered.");
  emit resetAvidaSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitNewMainWindowSig(void){
  GenDebug("entered.");
  emit newMainWindowSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitNewInstructionViewerSig(void){
  GenDebug("entered.");
  emit newInstructionViewerSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitNewEventViewerSig(void){
  GenDebug("entered.");
  emit newEventViewerSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitNewGodBoxSig(void){
  GenDebug("entered.");
  emit newGodBoxSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitNewPlotViewerSig(void){
  GenDebug("entered.");
  emit newPlotViewerSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitQuitSig(void){
  GenDebug("entered.");
  emit quitSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitAvidaOpenedSig(void){
  GenDebug("entered.");
  emit avidaOpenedSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitAvidaResetSig(void){
  GenDebug("entered.");
  emit avidaResetSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitPopulationCellSelectedSig(int cell_id){
  GenDebug("entered.");
  emit populationCellSelectedSig(cell_id);
  GenDebug("done.");
}

// for triggering messages from the gui to the driver.
void
avd_MissionControl::emitStartAvidaSig(void){
  GenDebug("entered.");
  emit startAvidaSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitUpdateAvidaSig(void){
  GenDebug("entered.");
  emit updateAvidaSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitStepAvidaSig(int cell_id){
  GenDebug("entered.");
  emit stepAvidaSig(cell_id);
  GenDebug("done.");
}
void
avd_MissionControl::emitStopAvidaSig(void){
  GenDebug("entered.");
  emit stopAvidaSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitExitAvidaSig(void){
  GenDebug("entered.");
  emit exitAvidaSig();
  GenDebug("done.");
}

// for triggering messages from the driver to the gui..
void
avd_MissionControl::emitAvidaUpdatedSig(void){
  GenDebug("entered.");
  emit avidaUpdatedSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitAvidaSteppedSig(int cell_id){
  GenDebug("entered.");
  emit avidaSteppedSig(cell_id);
  GenDebug("done.");
}
void
avd_MissionControl::emitAvidaBreakpointSig(int cell_id){
  GenDebug("entered.");
  emit avidaBreakpointSig(cell_id);
  GenDebug("done.");
}
void
avd_MissionControl::emitAvidaStoppedSig(void){
  GenDebug("entered.");
  emit avidaStoppedSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitAvidaExitedSig(void){
  GenDebug("entered.");
  emit avidaExitedSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitQueryGUISig(void){
  GenDebug("entered.");
  emit queryGUISig();
  GenDebug("done.");
}
void
avd_MissionControl::emitGuiStatusSig(bool running){
  GenDebug("entered.");
  emit guiStatusSig(running);
  GenDebug("done.");
}
void
avd_MissionControl::emitIsAvidaRunningSig(void){
  GenDebug("entered.");
  emit isAvidaRunningSig();
  GenDebug("done.");
}
void
avd_MissionControl::emitAvidaStatusSig(bool running){
  GenDebug("entered.");
  emit avidaStatusSig(running);
  GenDebug("done.");
}


/* vim: set ts=2 ai et: */

