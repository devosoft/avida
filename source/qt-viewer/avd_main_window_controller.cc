

#ifndef AVD_MAIN_WINDOW_CONTROLLER_HH
#include "avd_main_window_controller.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif

#ifndef AVD_MAIN_WINDOW_HH
#include "avd_main_window.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QMESSAGEBOX_H
#include <qmessagebox.h>
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


avd_MainWindowController::avd_MainWindowController(
  avd_MissionControl &mission_control,
  QObject *parent,
  const char *name
):QObject(parent, name),
  m_mission_control(mission_control)
{
  GenDebug("entered.");

  /*
  FIXME: until avida-reset works, this hack helps to keep main window
  from being raised above setup wizard.

  -- kgn
  */
  connect(
    &m_mission_control, SIGNAL(newMainWindowSig(void)),
    this, SLOT(newMainWindowSlot(void))
  );
  /*
  FIXME: end disabling hack.  -- kgn.
  */

  connect(
    &m_mission_control, SIGNAL(avidaOpenedSig(void)),
    this, SLOT(newMainWindowSlot(void))
  );

  connect(
    &m_mission_control, SIGNAL(quitSig(void)),
    this, SLOT(confirmQuit(void))
  );
  connect(
    this, SIGNAL(quitSig(void)),
    &m_mission_control, SLOT(quitSlot(void))
  );

  GenDebug("done.");
}

avd_MainWindowController::~avd_MainWindowController(){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_MainWindowController::wrapupInit(void){
  GenDebug("entered.");

  /*
  FIXME: until avida-reset works, this hack helps to keep main window
  from being raised above setup wizard.

  -- kgn
  */
  //newMainWindowSlot();
  /*
  FIXME: end disabling hack.  -- kgn.
  */
  GenDebug("done.");
}


void
avd_MainWindowController::closeEventSlot(avd_MainWindow *main_window){
  GenDebug("entered.").va(" --- m_main_window_list.count():  %d", m_main_window_list.count());
  if(m_main_window_list.count() <= 1){
    confirmQuit();
  } else {
    avd_MainWindowList::iterator it; 
    it = m_main_window_list.find(main_window);
    if(it == m_main_window_list.end()) exit(1);
    delete main_window;
    m_main_window_list.remove(it);
  }
  
  GenDebug("done.");
}   

void
avd_MainWindowController::confirmQuit(void){
  GenDebug("entered.");
  
  if(
    QMessageBox::information(
      0,
      "Test Application",
      "Are you sure you want to quit?",
      "&Quit",
      "&Cancel",
      QString::null,
      0,
      1
    ) 
    == 0
  ) emit quitSig();
    
  GenDebug("done.");
}
  
void
avd_MainWindowController::newMainWindowSlot(void){
  GenDebug("entered.");

  avd_MainWindow *main_window = new avd_MainWindow(
    m_mission_control,
    0,
    "<avd_MainWindowController::newMainWindowSlot(main_window)>"
  );
  connect(
    main_window, SIGNAL(closeEventSig(avd_MainWindow *)),
    this, SLOT(closeEventSlot(avd_MainWindow *))
  );
  m_main_window_list.append(avd_MainWindowGuardedPtr(main_window));
  main_window->show();

  GenDebug("done.");
}


/* vim: set ts=2 ai et: */

