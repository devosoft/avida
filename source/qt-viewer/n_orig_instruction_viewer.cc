//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////



#include <qvbox.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qpushbutton.h>
#include <qmenubar.h> 
#include <qpopupmenu.h> 
#include <qstatusbar.h>

#include "avd_mission_control.hh"
#include "n_orig_instruction_view_widget.hh"

#ifndef N_ORIG_INSTRUCTION_BUTTONS_WIDGET_HH
#include "n_orig_instruction_buttons_widget.hh"
#endif

#ifndef N_ORIG_INSTRUCTION_CPU_WIDGET_HH
#include "n_orig_instruction_cpu_widget.hh"
#endif

#include "n_orig_instruction_viewer.hh"


#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


N_Instruction_Viewer::N_Instruction_Viewer(
  QWidget *parent,
  const char *name,
  WFlags f
)
//: QWidget(parent, name, f)
: QMainWindow(parent, name, f)
{
  QWidget *central_widget = new QWidget(
    this, "<N_Instruction_Viewer::N_Instruction_Viewer(central_widget)>");
  setCentralWidget(central_widget);
  m_hboxlayout1 = new QHBoxLayout(
    central_widget, 0, -1, "<N_Instruction_Viewer::N_Instruction_Viewer(m_hboxlayout1)>");
  m_vboxlayout1 = new QVBoxLayout(
    m_hboxlayout1, -1, "<N_Instruction_Viewer::N_Instruction_Viewer(m_vboxlayout1)>");
  m_instruction_view_widget = new N_Instruction_ViewWidget(central_widget);
  m_vboxlayout1->addWidget(m_instruction_view_widget);
  m_instruction_buttons_widget = new N_Instruction_ButtonsWidget(central_widget);
  m_vboxlayout1->addWidget(m_instruction_buttons_widget);
  m_vboxlayout2 = new QVBoxLayout(m_hboxlayout1);
  m_instruction_cpu_widget = new N_Instruction_CPUWidget(central_widget);
  m_vboxlayout2->addWidget(m_instruction_cpu_widget);
  m_vboxlayout2->addItem(new QSpacerItem(20, 20));
  setCaption("Instruction Viewer");
}

N_Instruction_Viewer::~N_Instruction_Viewer(void){
  GenDebug << "destructor called.";
}

void
N_Instruction_Viewer::setMissionControl(
  avd_MissionControl *mission_control
){
  m_mission_control = mission_control;
  m_instruction_view_widget->setMissionControl(mission_control);
  m_instruction_buttons_widget->setMissionControl(mission_control);
  m_instruction_cpu_widget->setMissionControl(mission_control);

  // prepare connections from avida.
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaBreakpointSig(int)), this, SLOT(setPopulationCell(int)));

  // disable unsafe buttons while avida is working.
  connect(mission_control, SIGNAL(startAvidaSig()), this, SLOT(disableUnsafeButtonsSlot()));
  connect(mission_control, SIGNAL(updateAvidaSig()), this, SLOT(disableUnsafeButtonsSlot()));
  connect(mission_control, SIGNAL(stepAvidaSig(int)), this, SLOT(disableUnsafeButtonsSlot()));

  // enable unsafe buttons when avida is done.
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(enableUnsafeButtonsSlot()));
  connect(mission_control, SIGNAL(avidaBreakpointSig(int)), this, SLOT(enableUnsafeButtonsSlot()));
  connect(mission_control, SIGNAL(avidaStoppedSig()), this, SLOT(enableUnsafeButtonsSlot()));

  connect(mission_control, SIGNAL(avidaStatusSig(bool)), this, SLOT(avidaStatusSlot(bool)));
  connect(this, SIGNAL(isAvidaRunningSig()), mission_control, SIGNAL(isAvidaRunningSig()));
  mission_control->emitIsAvidaRunningSig();
  
  /*
  add menu items
  */
  QPopupMenu *file_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(file_menu)>");
    //file_menu->insertItem( "Open Avida Session", this, SLOT(openAvidaSlot()), CTRL+Key_O);
    //file_menu->insertItem( "Close Avida Session", this, SLOT(closeAvidaSlot()));
    //file_menu->insertItem( "Reset Avida Session", this, SLOT(resetAvidaSlot()));
    //file_menu->insertItem( "Settings", m_mission_control, SIGNAL(userSettingsSig()));
    //file_menu->insertItem( "Quit", this, SIGNAL(quitSig()), CTRL+Key_Q);

    file_menu->insertItem( "Start Avida", this, SLOT(startAvidaSlot()));
    file_menu->insertItem( "Pause Avida", this, SLOT(stopAvidaSlot()));
    file_menu->insertItem( "Step Avida", this, SLOT(stepAvidaSlot()));
    file_menu->insertItem( "Update Avida", this, SLOT(updateAvidaSlot()));
    file_menu->insertSeparator();
    file_menu->insertItem( "Close Instruction Viewer", this, SLOT(close()), CTRL+Key_W);
    file_menu->insertItem( "Quit", m_mission_control, SIGNAL(quitSig()), CTRL+Key_Q);

  menuBar()->insertItem( "File", file_menu);
  QPopupMenu *view_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(view_menu)>");
    view_menu->insertItem( "New Map Viewer", m_mission_control, SIGNAL(newMainWindowSig()), CTRL+Key_N);
    view_menu->insertItem( "Instruction Viewer", m_mission_control, SIGNAL(newInstructionViewerSig()));
    view_menu->insertItem( "&Event Editor", m_mission_control, SIGNAL(newEventViewerSig()));
    view_menu->insertItem( "Environment Control Box", m_mission_control, SIGNAL(newGodBoxSig()));
    view_menu->insertItem( "New Plot Viewer...", m_mission_control, SIGNAL(newPlotViewerSig()));
  menuBar()->insertItem( "Viewers", view_menu);

  QPopupMenu *help_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(help_menu)>");

    help_menu->insertItem("Documentation Contents", this, SLOT(helpContents()));
    help_menu->insertSeparator();
    help_menu->insertItem("Setting up the Genesis File", this, SLOT(helpGenesis()));
    help_menu->insertItem("Setting up the Instruction Set", this, SLOT(helpInstructionSet()));
    help_menu->insertItem("Guidelines to setting Events", this, SLOT(helpEvents()));
    help_menu->insertItem("Guidelines to setting up Environments", this, SLOT(helpEnvironments()));
    help_menu->insertItem("Guidelines to Analyze Mode", this, SLOT(helpAnalyzeMode()));

  menuBar()->insertItem( "Help", help_menu);

  // FIXME
  // if(mission_control->avidaIsRunning()){
  //   GenDebug("calling disableUnsafeButtonsSlot().");
  //   //disableUnsafeButtonsSlot();
  //   m_instruction_buttons_widget->avidaStartingSlot();
  // } else {
  //   //GenDebug("calling enableUnsafeButtonsSlot().");
  //   //enableUnsafeButtonsSlot();
  //   //enableUnsafeButtonsSlot();
  // }
  GenDebug("done.");

}

void N_Instruction_Viewer::localStepSlot(void)
{ emit localStepSig(m_cell_id); }

void N_Instruction_Viewer::setPopulationCell(int cell_id){
  GenDebug << "cell_id " << cell_id << '.';
  m_cell_id = cell_id;
  m_instruction_view_widget->setPopulationCell(cell_id);
  m_instruction_buttons_widget->setPopulationCell(cell_id);
  m_instruction_cpu_widget->setPopulationCell(cell_id);
}

void N_Instruction_Viewer::updateState(void)
{ m_instruction_view_widget->updateState(); }
void N_Instruction_Viewer::disableUnsafeButtonsSlot(void)
{ m_instruction_view_widget->buttonsSafe(false); }
void N_Instruction_Viewer::enableUnsafeButtonsSlot(void)
{ m_instruction_view_widget->buttonsSafe(true); }

void N_Instruction_Viewer::avidaStatusSlot(bool running){
  GenDebug("entered.");
  if(running){
    GenDebug("running == true.");
    m_instruction_buttons_widget->avidaStartingSlot();
    m_instruction_buttons_widget->avidaUpdatingSlot();
    disableUnsafeButtonsSlot();
  } else{
    GenDebug("running == false.");
    m_instruction_buttons_widget->avidaStoppedSlot();
    enableUnsafeButtonsSlot();
  }
  GenDebug("done.");
}

void N_Instruction_Viewer::aboutAvidaHelpSlot()
{ m_mission_control->emitHelpURLSig("avida_home.html"); }
void N_Instruction_Viewer::help1Slot()
{ m_mission_control->emitHelpURLSig("help1.html"); }
void N_Instruction_Viewer::help2Slot()
{ m_mission_control->emitHelpURLSig("help2.html"); }
void N_Instruction_Viewer::helpContents()
{ m_mission_control->emitHelpURLSig("index.html"); }
void N_Instruction_Viewer::helpGenesis()
{ m_mission_control->emitHelpURLSig("genesis.html"); }
void N_Instruction_Viewer::helpInstructionSet()
{ m_mission_control->emitHelpURLSig("inst_set.html"); }
void N_Instruction_Viewer::helpEvents()
{ m_mission_control->emitHelpURLSig("events.html"); }
void N_Instruction_Viewer::helpEnvironments()
{ m_mission_control->emitHelpURLSig("environment.html"); }
void N_Instruction_Viewer::helpAnalyzeMode()
{ m_mission_control->emitHelpURLSig("analyze_mode.html"); }

void N_Instruction_Viewer::openAvidaSlot()
{ m_mission_control->emitOpenAvidaSig(); }
void N_Instruction_Viewer::closeAvidaSlot()
{ m_mission_control->emitCloseAvidaSig(); }
void N_Instruction_Viewer::resetAvidaSlot()
{ m_mission_control->emitResetAvidaSig(); }
void N_Instruction_Viewer::startAvidaSlot()
{ m_mission_control->emitStartAvidaSig(); }
void N_Instruction_Viewer::updateAvidaSlot()
{ m_mission_control->emitUpdateAvidaSig(); }
void N_Instruction_Viewer::stepAvidaSlot()
{ m_mission_control->emitStepAvidaSig(m_mission_control->getSelectedPopulationCellID()); }
void N_Instruction_Viewer::stopAvidaSlot()
{ m_mission_control->emitStopAvidaSig(); }
void N_Instruction_Viewer::exitAvidaSlot()
{ m_mission_control->emitExitAvidaSig(); }

