


#ifndef AVD_EVENT_VIEWER_CONTROLLER_HH
#include "avd_event_viewer_controller.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
//#ifndef N_ORIG_INSTRUCTION_VIEWER_HH
//#include "n_orig_instruction_viewer.hh"
//#endif
//#ifndef N_ORIG_INSTRUCTION_SCROLLVIEW_HH
//#include "n_orig_instruction_scrollview.hh"
//#endif
#ifndef EVENT_VIEW_WIDGET_HH
#include "event_view_widget.hh"
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


avd_EventViewerController::avd_EventViewerController(
  avd_MissionControl &mission_control,
  QObject *parent,
  const char *name
):QObject(parent, name),
  m_mission_control(mission_control)
{
  GenDebug("entered.");
  connect(
    &m_mission_control, SIGNAL(newEventViewerSig(void)),
    this, SLOT(newEventViewWidgetSlot(void))
  );
  GenDebug("done.");
}

avd_EventViewerController::~avd_EventViewerController(){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_EventViewerController::wrapupInit(void){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_EventViewerController::newEventViewWidgetSlot(void){
  GenDebug("entered.");

  if(m_event_view_widget.isNull()){
    m_event_view_widget = new EventViewWidget(
      0,
      "<avd_EventViewerController::newEventViewWidgetSlot(m_event_view_widget)>",
      WDestructiveClose
    );
    m_event_view_widget->setMissionControl(&m_mission_control);
  }
  m_event_view_widget->show();
  m_event_view_widget->raise();

  GenDebug("done.");
}


/* vim: set ts=2 ai et: */

