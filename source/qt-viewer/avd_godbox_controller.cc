


#ifndef AVD_GODBOX_CONTROLLER_HH
#include "avd_godbox_controller.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
#ifndef GODBOX_WIDGET2_HH
#include "godbox_widget2.hh"
#endif

#ifndef QOBJECTLIST_H
#include <qobjectlist.h>
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


avd_GodBoxController::avd_GodBoxController(
  avd_MissionControl &mission_control,
  QObject *parent,
  const char *name
):QObject(parent, name),
  m_mission_control(mission_control)
{
  GenDebug("entered.");
  connect(
    &m_mission_control, SIGNAL(newGodBoxSig(void)),
    this, SLOT(newGodBoxWidgetSlot(void))
  );
  GenDebug("done.");
}

avd_GodBoxController::~avd_GodBoxController(){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_GodBoxController::wrapupInit(void){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_GodBoxController::newGodBoxWidgetSlot(void){
  GenDebug("entered.");

  if(m_godbox.isNull()){
    m_godbox = new GodBoxWidget2(
      0,
      "<avd_GodBoxController::newGodBoxWidgetSlot(m_godbox)>",
      WDestructiveClose
    );
    m_godbox->setMissionControl(&m_mission_control);
    m_godbox->setup();
  }
  m_godbox->show();
  m_godbox->raise();

  GenDebug("done.");
}


/* vim: set ts=2 ai et: */

