//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////


// porting to gcc 3.1 -- k

#include <qfiledialog.h>
#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qwidgetstack.h>

#include <fstream>

#include "genotype.hh"
#include "avd_mission_control.hh"
#include "population_cell_wrapped_accessors.hh"
#include "test_util.hh"

#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#ifndef N_ORIG_INSTRUCTION_BUTTONS_WIDGET_HH
#include "n_orig_instruction_buttons_widget.hh"
#endif


using namespace std;


N_Instruction_ButtonsWidget::N_Instruction_ButtonsWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  GenDebug("constructor.");

  m_vboxlayout = new QVBoxLayout(this, 0, -1, "<N_Instruction_ButtonsWidget::m_vboxlayout1>");
  m_hboxlayout1 = new QHBoxLayout(m_vboxlayout, -1, "<N_Instruction_ButtonsWidget::m_hboxlayout1>");
  //m_vboxlayout->addLayout(m_hboxlayout1);
  m_widgetstack = new QWidgetStack(this, "<N_Instruction_ButtonsWidget::m_widgetstack>");
  m_hboxlayout1->addWidget(m_widgetstack);

  m_stop_pbutton = new QPushButton("Stop", this);
  m_widgetstack->addWidget(m_stop_pbutton);

  m_start_pbutton = new QPushButton("Start", this);
  m_widgetstack->addWidget(m_start_pbutton);

  // move this line elsewhere.
  m_widgetstack->raiseWidget(m_start_pbutton);

  m_step_pbutton = new QPushButton("Step", this);
  m_hboxlayout1->addWidget(m_step_pbutton);

  m_update_pbutton = new QPushButton("Update", this);
  m_hboxlayout1->addWidget(m_update_pbutton);

  m_hboxlayout2 = new QHBoxLayout(m_vboxlayout, -1, "<N_Instruction_ButtonsWidget::m_hboxlayout2>");

  m_hboxlayout2->addItem(new QSpacerItem(20, 20));

  m_extract_pbutton = new QPushButton("Save Organism", this);
  m_hboxlayout2->addWidget(m_extract_pbutton);

  m_hboxlayout2->addItem(new QSpacerItem(20, 20));

  //// when avida is started
  //connect(
  //  m_start_pbutton, SIGNAL(clicked()),
  //  this, SLOT(raiseStopButtonSlot())
  //);
  //// redundant, but what the heck, complete.
  //connect(
  //  m_start_pbutton, SIGNAL(clicked()),
  //  this, SLOT(disableStartButtonSlot())
  //);
  //connect(
  //  m_start_pbutton, SIGNAL(clicked()),
  //  this, SLOT(disableStepUpdateExtractButtonsSlot())
  //);

  //// when avida is stepped
  //connect(
  //  m_step_pbutton, SIGNAL(clicked()),
  //  this, SLOT(disableStartButtonSlot())
  //);
  //connect(
  //  m_step_pbutton, SIGNAL(clicked()),
  //  this, SLOT(disableStepUpdateExtractButtonsSlot())
  //);

  //// when avida is updated
  //connect(
  //  m_update_pbutton, SIGNAL(clicked()),
  //  this, SLOT(disableStartButtonSlot())
  //);
  //connect(
  //  m_update_pbutton, SIGNAL(clicked()),
  //  this, SLOT(disableStepUpdateExtractButtonsSlot())
  //);

  // creature extraction
  connect(
    m_extract_pbutton, SIGNAL(clicked()),
    this, SLOT(extractCreatureSlot())
  );
}

void
N_Instruction_ButtonsWidget::setMissionControl(
  avd_MissionControl *mission_control
){
  GenDebug("entered.");
  m_mission_control = mission_control;

  // connections to avida
  connect(
    m_start_pbutton, SIGNAL(clicked()),
    //mission_control, SLOT(startAvidaSlot())
    mission_control, SIGNAL(startAvidaSig())
  );
  connect(
    m_stop_pbutton, SIGNAL(clicked()),
    //mission_control, SLOT(stopAvidaSlot())
    mission_control, SIGNAL(stopAvidaSig())
  );
  connect(
    m_step_pbutton, SIGNAL(clicked()),
    this, SLOT(localStepSlot())
  );
  connect(
    this, SIGNAL(localStepSig(int)),
    //mission_control, SLOT(stepAvidaSlot(int))
    mission_control, SIGNAL(stepAvidaSig(int))
  );
  connect(
    m_update_pbutton, SIGNAL(clicked()),
    //mission_control, SLOT(updateAvidaSlot())
    mission_control, SIGNAL(updateAvidaSig())
  );

  // connections from avida

  // when avida is starting in some way...
  connect(
    mission_control, SIGNAL(startAvidaSig()),
    this, SLOT(avidaStartingSlot())
  );
  connect(
    mission_control, SIGNAL(stepAvidaSig(int)),
    this, SLOT(avidaSteppingSlot())
  );
  connect(
    mission_control, SIGNAL(updateAvidaSig()),
    this, SLOT(avidaUpdatingSlot())
  );

  // when avida has stopped in some way...
  connect(
    mission_control, SIGNAL(avidaStoppedSig()),
    this, SLOT(avidaStoppedSlot())
  );
  connect(
    mission_control, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(avidaStoppedSlot())
  );
  connect(
    mission_control, SIGNAL(avidaBreakpointSig(int)),
    this, SLOT(avidaStoppedSlot())
  );
}

void
N_Instruction_ButtonsWidget::setPopulationCell(int cell_id)
{
  GenDebug("entered.");
  m_cell_id = cell_id;
}

void
N_Instruction_ButtonsWidget::enableStepUpdateExtractButtonsSlot(void)
{
  GenDebug("entered.");
  m_step_pbutton->setEnabled(true);
  m_update_pbutton->setEnabled(true);
  m_extract_pbutton->setEnabled(true);
}

void
N_Instruction_ButtonsWidget::enableStartButtonSlot(void)
{
  GenDebug("entered.");
  m_start_pbutton->setEnabled(true);
}

void
N_Instruction_ButtonsWidget::raiseStartButtonSlot(void)
{
  GenDebug("entered.");
  m_widgetstack->raiseWidget(m_start_pbutton);
  enableStartButtonSlot();
}

void
N_Instruction_ButtonsWidget::raiseStopButtonSlot(void)
{
  GenDebug("entered.");
  m_widgetstack->raiseWidget(m_stop_pbutton);
}

void
N_Instruction_ButtonsWidget::disableStepUpdateExtractButtonsSlot(void)
{
  GenDebug("entered.");
  m_step_pbutton->setEnabled(false);
  m_update_pbutton->setEnabled(false);
  m_extract_pbutton->setEnabled(false);
}

void
N_Instruction_ButtonsWidget::disableStartButtonSlot(void)
{
  GenDebug("entered.");
  m_start_pbutton->setEnabled(false);
}

void
N_Instruction_ButtonsWidget::avidaStartingSlot(void){
  GenDebug("entered.");
  raiseStopButtonSlot();
  disableStartButtonSlot();
  disableStepUpdateExtractButtonsSlot();
} 

void
N_Instruction_ButtonsWidget::avidaSteppingSlot(void){
  GenDebug("entered.");
  disableStartButtonSlot();
  disableStepUpdateExtractButtonsSlot();
} 

void
N_Instruction_ButtonsWidget::avidaUpdatingSlot(void){
  GenDebug("entered.");
  disableStartButtonSlot();
  disableStepUpdateExtractButtonsSlot();
} 

void
N_Instruction_ButtonsWidget::avidaStoppedSlot(void){
  GenDebug("entered.");
  enableStepUpdateExtractButtonsSlot();
  raiseStartButtonSlot();
} 

void
N_Instruction_ButtonsWidget::localStepSlot(void){
  GenDebug("entered.");
  emit localStepSig(m_cell_id);
} 

void
N_Instruction_ButtonsWidget::extractCreatureSlot(void)
{
  GenDebug("entered.");

  disableStartButtonSlot();
  disableStepUpdateExtractButtonsSlot();

  cGenotype *cur_gen = 
    GetGenotype(
      GetOrganism(
        GetPopulationCell(
          m_mission_control,
          m_cell_id
        )
      )
    );
  cString gen_name = cur_gen->GetName();
  if (gen_name == "(no name)")
    gen_name.Set("%03d-unnamed", cur_gen->GetLength());

  QString verified_name = QFileDialog::getSaveFileName(
    QString(gen_name()),
    QString::null,  // use no filename filter
    this,           // parent widget
    "<N_Instruction_ButtonsWidget::extractCreatureSlot> savedialog",
    "Save Extracted Creature to File"   // caption
  );
  GenDebug("  --- chosen name: ")(verified_name);

  // early exit on cancel.
  if(QString::null == verified_name) {
    enableStartButtonSlot();
    enableStepUpdateExtractButtonsSlot();
    return;
  }
  
  gen_name = verified_name.latin1();

  cTestUtil::PrintGenome(cur_gen->GetGenome(), gen_name);

  enableStartButtonSlot();
  enableStepUpdateExtractButtonsSlot();
  
  GenDebug("done.");
}


