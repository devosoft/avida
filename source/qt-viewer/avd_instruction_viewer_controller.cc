
#ifndef AVD_INSTRUCTIONS_VIEW_CONTROLLER_HH
#include "avd_instruction_viewer_controller.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
#ifndef N_ORIG_INSTRUCTION_VIEWER_HH
#include "n_orig_instruction_viewer.hh"
#endif
#ifndef N_ORIG_INSTRUCTION_SCROLLVIEW_HH
#include "n_orig_instruction_scrollview.hh"
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


avd_InstructionViewerController::avd_InstructionViewerController(
  avd_MissionControl &mission_control,
  QObject *parent,
  const char *name
):QObject(parent, name),
  m_mission_control(mission_control)
{
  GenDebug("entered.");
  connect(
    &m_mission_control, SIGNAL(newInstructionViewerSig(void)),
    this, SLOT(newInstructionViewerSlot(void))
  );
  GenDebug("done.");
}

avd_InstructionViewerController::~avd_InstructionViewerController(){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_InstructionViewerController::wrapupInit(void){
  GenDebug("entered.");

  N_Instruction_ScrollView::N_Instruction_ScrollView_dynamicConfig();

  GenDebug("done.");
}

void
avd_InstructionViewerController::newInstructionViewerSlot(void){
  GenDebug("entered.");

  if(m_instruction_viewer.isNull()){
    m_instruction_viewer = new N_Instruction_Viewer(
      0,
      "<avd_InstructionViewerController::newInstructionViewerSlot(m_instruction_viewer)>"
    );
    m_instruction_viewer->setMissionControl(&m_mission_control);
    m_instruction_viewer->setPopulationCell(
      m_mission_control.getSelectedPopulationCellID()
    );
    connect(
      &m_mission_control, SIGNAL(populationCellSelectedSig(int)),
      m_instruction_viewer, SLOT(setPopulationCell(int))
    );
  }
  m_instruction_viewer->show();

  GenDebug("done.");
}


/* vim: set ts=2 ai et: */

