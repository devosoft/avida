
//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "avd_mission_control.hh"
#include "n_orig_instruction_cpu_widget.hh"
#include "population_cell_wrapped_accessors.hh"
#include "config.hh"
#include "environment.hh"
#include "genotype.hh"
#include "hardware_cpu.hh"
#include "message_display_hdrs.hh"
#include "organism.hh"
#include "phenotype.hh"
#include "population.hh"
#include "population_cell.hh"
#include "task_entry.hh"
#include "species.hh"
#include "string.hh"

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qhbox.h>
#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qvgroupbox.h>
#include <qwidgetstack.h>

#include <fstream>
#include <iostream>


using namespace std;

cLabeledField::cLabeledField(QWidget *parent, const char *name, WFlags f)
: QWidget(parent, name, f)
{
  GenDebug("entered.");

  m_hboxlayout = new QHBoxLayout(this);
  m_title_label = new QLabel(this);
  //QFont new_font(m_title_label->font());
  //new_font.setPointSize(new_font.pointSize() - 4);
  //setFont(new_font);
  //m_title_label->setFont(new_font);
  m_hboxlayout->addWidget(m_title_label);

  m_data1_label = new QLabel(this);
  //m_data1_label->setFont(new_font);
  //m_data1_label->setAlignment((m_data1_label->alignment() & !(Qt::AlignRight)) | Qt::AlignRight);
  //m_data1_label->setSizePolicy(QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
  m_hboxlayout->addWidget(m_data1_label);

  m_data2_label = new QLabel(this);
  //m_data2_label->setAlignment((m_data2_label->alignment() & !(Qt::AlignRight)) | Qt::AlignRight);
  //m_data2_label->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
  m_hboxlayout->addWidget(m_data2_label);
}
void cLabeledField::setTitle(const QString &title)
{ m_title_label->setText(title); }
void cLabeledField::setData1Text(const QString &text)
{
  m_data1_label->setText(text);
}
void cLabeledField::setData2Text(const QString &text)
{ m_data2_label->setText(text); }
void cLabeledField::setData2Binary(unsigned long value)
{ m_data2_label->setText(QString("[%1]").arg(value, 0, 2)); }
void cLabeledField::setData2BinaryWidth(void){
  int zwidth = QApplication::fontMetrics().width("0");
  m_data2_label->setMinimumWidth(30 * zwidth);
}
void cLabeledField::setData1DecimalWidth(void){
  int zwidth = QApplication::fontMetrics().width("0");
  m_data1_label->setMinimumWidth(11 * zwidth);
}
void cLabeledField::showData2Text(bool yes){
  if(yes) m_data2_label->show();
  else m_data2_label->hide();
}
void cLabeledField::setBase(int base){
  m_base = base;
  int zwidth = QApplication::fontMetrics().width("0");
  if(base<10){
    m_data1_label->setMinimumWidth(25 * zwidth);
  } else if (10<=base && base<16){
    m_data1_label->setMinimumWidth(11 * zwidth);
  } else if (16<=base){
    m_data1_label->setMinimumWidth(9 * zwidth);
  }
}


cSummaryWidget::cSummaryWidget(QWidget *parent, const char *name, WFlags f)
: QVBox(parent, name, f)
{
  GenDebug("entered.");
  //setInsideSpacing(0);

  
  setMargin(2);
  m_genotype_id = new cLabeledField(this);
  m_genotype_name = new cLabeledField(this);
  m_species_id = new cLabeledField(this);

  m_fitness = new cLabeledField(this);
  m_gestation_time = new cLabeledField(this);
  m_cpu_speed = new cLabeledField(this);
  m_current_merit = new cLabeledField(this);
  m_genome_size = new cLabeledField(this);
  m_memory_size = new cLabeledField(this);
  m_faults = new cLabeledField(this);

  m_location = new cLabeledField(this);
  m_facing = new cLabeledField(this);

  m_generation = new cLabeledField(this);
  m_age = new cLabeledField(this);
  m_executed = new cLabeledField(this);
  m_last_divide = new cLabeledField(this);
  m_offspring = new cLabeledField(this);

  m_mem_allocated = new cLabeledField(this);
  m_parent_true = new cLabeledField(this);
  m_injected = new cLabeledField(this);
  m_parasite = new cLabeledField(this);
  m_mutated = new cLabeledField(this);
  m_modified = new cLabeledField(this);


  m_genotype_id->setTitle("Genotype ID:");
  m_genotype_name->setTitle("Genotype Name:");
  m_species_id->setTitle("Species ID:");

  m_fitness->setTitle("Fitness:");
  m_gestation_time->setTitle("Gestation Time:");
  m_cpu_speed->setTitle("CPU Speed:");
  m_current_merit->setTitle("Current Merit:");
  m_genome_size->setTitle("Genome Size:");
  m_memory_size->setTitle("Memory Size:");
  m_faults->setTitle("Faults:");

  m_location->setTitle("Location:");
  m_facing->setTitle("Facing:");

  m_generation->setTitle("Generation:");
  m_age->setTitle("Age:");
  m_executed->setTitle("Executed:");
  m_last_divide->setTitle("Last Divide:");
  m_offspring->setTitle("Offspring:");

  m_mem_allocated->setTitle("Mem Allocated:");
  m_parent_true->setTitle("Parent True:");
  m_injected->setTitle("Injected:");
  m_parasite->setTitle("Parasite:");
  m_mutated->setTitle("Mutated:");
  m_modified->setTitle("Modified:");


  m_genotype_id->showData2Text(false);
  m_genotype_name->showData2Text(false);
  m_species_id->showData2Text(false);

  m_fitness->showData2Text(false);
  m_gestation_time->showData2Text(false);
  m_cpu_speed->showData2Text(false);
  m_current_merit->showData2Text(false);
  m_genome_size->showData2Text(false);
  m_memory_size->showData2Text(false);
  m_faults->showData2Text(false);

  m_location->showData2Text(false);
  m_facing->showData2Text(false);

  m_generation->showData2Text(false);
  m_age->showData2Text(false);
  m_executed->showData2Text(false);
  m_last_divide->showData2Text(false);
  m_offspring->showData2Text(false);

  m_mem_allocated->showData2Text(false);
  m_parent_true->showData2Text(false);
  m_injected->showData2Text(false);
  m_parasite->showData2Text(false);
  m_mutated->showData2Text(false);
  m_modified->showData2Text(false);
}
void cSummaryWidget::setMissionControl(avd_MissionControl *mission_control){
  GenDebug("entered.");
  m_mission_control = mission_control;
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
}
void cSummaryWidget::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  updateState();
}
void cSummaryWidget::updateState(void){
  GenDebug("entered.");
  if(!isVisible() || !m_mission_control) return;
  m_mission_control->lock();
  cPopulation *population = m_mission_control->getPopulation();
  cPopulationCell *population_cell =
     GetPopulationCell(m_mission_control, m_cell_id);
  cGenotype *genotype =
    GetGenotype(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)));
  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)));
  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)));
  if(!population || !population_cell){
    m_location->setData1Text("");
    m_facing->setData1Text("");
  } else {
    m_location->setData1Text(QString("[%1, %2] (%3)")
      .arg(population_cell->GetID() % population->GetWorldX())
      .arg(population_cell->GetID() / population->GetWorldY())
      .arg(population_cell->GetID()));
    m_facing->setData1Text(QString("[%1, %2] (%3)")
      .arg(population_cell->ConnectionList().GetFirst()->GetID() % population->GetWorldX())
      .arg(population_cell->ConnectionList().GetFirst()->GetID() / population->GetWorldY())
      .arg(population_cell->ConnectionList().GetFirst()->GetID()));
  }
  if(!genotype){
    m_genotype_id->setData1Text("");
    m_genotype_name->setData1Text("");
    m_species_id->setData1Text("");
    m_genome_size->setData1Text("");
  } else {
    m_genotype_id->setData1Text(QString("%1").arg(genotype->GetID()));
    m_genotype_name->setData1Text(QString(genotype->GetName()));
    if(!genotype->GetSpecies()) m_species_id->setData1Text("(none)");
    else m_species_id->setData1Text(QString("%1").arg(genotype->GetSpecies()->GetID()));
    m_genome_size->setData1Text(QString("%1").arg(genotype->GetLength()));
  }
  if(!phenotype){
    m_fitness->setData1Text("");
    m_gestation_time->setData1Text("");
    m_cpu_speed->setData1Text("");
    m_current_merit->setData1Text("");
    m_faults->setData1Text("");
    m_generation->setData1Text("");
    m_age->setData1Text("");
    m_executed->setData1Text("");
    m_last_divide->setData1Text("");
    m_offspring->setData1Text("");
    m_parent_true->setData1Text("");
    m_injected->setData1Text("");
    m_parasite->setData1Text("");
    m_mutated->setData1Text("");
    m_modified->setData1Text("");
  } else {
    m_fitness->setData1Text(QString("%1").arg(phenotype->GetFitness()));
    m_gestation_time->setData1Text(QString("%1").arg(phenotype->GetGestationTime()));
    m_cpu_speed->setData1Text(QString("%1").arg(phenotype->GetMerit().GetDouble()));
    m_current_merit->setData1Text(QString("%1").arg(cMerit(phenotype->GetCurBonus()).GetDouble()));
    m_faults->setData1Text(QString("%1").arg(phenotype->GetCurNumErrors()));
    m_generation->setData1Text(QString("%1").arg(phenotype->GetGeneration()));
    m_generation->setData1Text(QString("%1").arg(phenotype->GetGeneration()));
    m_age->setData1Text(QString("%1").arg(phenotype->GetAge()));
    m_executed->setData1Text(QString("%1").arg(phenotype->GetTimeUsed()));
    m_last_divide->setData1Text(QString("%1").arg(phenotype->GetGestationStart()));
    m_offspring->setData1Text(QString("%1").arg(phenotype->GetNumDivides()));
    m_parent_true->setData1Text((phenotype->ParentTrue())?("yes"):("no"));
    m_injected->setData1Text((phenotype->IsInjected())?("yes"):("no"));
    m_parasite->setData1Text((phenotype->IsParasite())?("yes"):("no"));
    m_mutated->setData1Text((phenotype->IsMutated())?("yes"):("no"));
    m_modified->setData1Text((phenotype->IsModified())?("yes"):("no"));
  }
  if(!hardware){
    m_memory_size->setData1Text("");
    m_mem_allocated->setData1Text("");
  }
  else {
    m_memory_size->setData1Text(QString("%1").arg(hardware->GetMemory().GetSize()));
    m_mem_allocated->setData1Text((hardware->GetMalActive())?("yes"):("no"));
  }
  m_mission_control->unlock();
}

cRegistersWidget::cRegistersWidget(QWidget *parent, const char *name)
: QGroupBox(parent, name)
{
  GenDebug("entered.");
  setTitle("Registers");
  setColumns(1);
  //setInsideSpacing(0);
  m_registers.setAutoDelete(TRUE);
  for(int i = 0; i < NUM_REGISTERS; i++){
    cLabeledField *labeled_field = new cLabeledField(this);
    char title = 'A' + i;
    labeled_field->setTitle(QString("%1:").arg(title));
    labeled_field->setData1DecimalWidth();
    //labeled_field->setData2BinaryWidth();
    m_registers.append(labeled_field);
  }
}
void cRegistersWidget::setMissionControl(avd_MissionControl *mission_control){
  GenDebug("entered.");
  m_mission_control = mission_control;
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
}
void cRegistersWidget::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  updateState();
}
void cRegistersWidget::updateState(void){
  GenDebug("entered.");
  if(!isVisible() || !m_mission_control) return;
  m_mission_control->lock();
  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)));
  if(!hardware){
    int i = 0;
    cLabeledField *labeled_field = m_registers.first();
    for(; i < NUM_REGISTERS && 0 != labeled_field; i++, labeled_field = m_registers.next()){
      labeled_field->setData1Text("");
      //labeled_field->setData2Text("");
    }
  } else {
    int i = 0;
    cLabeledField *labeled_field = m_registers.first();
    for(; i < NUM_REGISTERS && 0 != labeled_field; i++, labeled_field = m_registers.next()){
      labeled_field->setData1Text(QString("%1").arg(
        (unsigned long)hardware->Register(i), 0, labeled_field->getBase()));
      //labeled_field->setData2Binary(hardware->Register(i));
    }
  }
  m_mission_control->unlock();
}
void cRegistersWidget::showData2Text(bool should_show){
  int i = 0;
  cLabeledField *labeled_field = m_registers.first();
  for(; i < NUM_REGISTERS && 0 != labeled_field; i++, labeled_field = m_registers.next())
  { labeled_field->showData2Text(should_show); }
}
void cRegistersWidget::chooseBaseSlot(int base){
  GenDebug("entered.  base")(base)(".");
  int i = 0;
  cLabeledField *labeled_field = m_registers.first();
  for(; i < NUM_REGISTERS && 0 != labeled_field; i++, labeled_field = m_registers.next())
  { labeled_field->setBase(base); }
  updateState();
  GenDebug("done.");
}

cInputsWidget::cInputsWidget(QWidget *parent, const char *name)
: QGroupBox(parent, name)
{
  GenDebug("entered.");
  setTitle("Inputs");
  setColumns(1);
  //setInsideSpacing(0);
  m_inputs.setAutoDelete(TRUE);
  for(int i = 0; i < NUM_REGISTERS; i++){
    cLabeledField *labeled_field = new cLabeledField(this);
    labeled_field->setTitle(QString("%1:").arg(i + 1));
    labeled_field->setData1DecimalWidth();
    //labeled_field->setData2BinaryWidth();
    m_inputs.append(labeled_field);
  }
}
void cInputsWidget::setMissionControl(avd_MissionControl *mission_control){
  GenDebug("entered.");
  m_mission_control = mission_control;
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
}
void cInputsWidget::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  updateState();
}
void cInputsWidget::updateState(void){
  GenDebug("entered.");
  if(!isVisible() || !m_mission_control) return;
  m_mission_control->lock();
  cPopulationCell *pop_cell =
    GetPopulationCell(m_mission_control, m_cell_id);
  if(!pop_cell || !GetOrganism(pop_cell)){
    int i = 0;
    cLabeledField *labeled_field = m_inputs.first();
    for(; i < IO_SIZE && 0 != labeled_field; i++, labeled_field = m_inputs.next()){
      labeled_field->setData1Text("");
      labeled_field->setData2Text("");
    }
  } else {
    int i = 0;
    cLabeledField *labeled_field = m_inputs.first();
    for(; i < IO_SIZE && 0 != labeled_field; i++, labeled_field = m_inputs.next()){
      labeled_field->setData1Text(QString("%1").arg(
        (unsigned long)pop_cell->GetInput(i), 0, labeled_field->getBase()));
      //labeled_field->setData2Binary(pop_cell->GetInput(i));
    }
  }
  m_mission_control->unlock();
}
void cInputsWidget::showData2Text(bool should_show){
  int i = 0;
  cLabeledField *labeled_field = m_inputs.first();
  for(; i < IO_SIZE && 0 != labeled_field; i++, labeled_field = m_inputs.next())
  { labeled_field->showData2Text(should_show); }
}
void cInputsWidget::chooseBaseSlot(int base){
  GenDebug("entered.  base")(base)(".");
  int i = 0;
  cLabeledField *labeled_field = m_inputs.first();
  for(; i < IO_SIZE && 0 != labeled_field; i++, labeled_field = m_inputs.next())
  { labeled_field->setBase(base); }
  updateState();
  GenDebug("done.");
}

cStackField::cStackField(QWidget *parent, const char *name, WFlags f)
: cLabeledField(parent, name, f)
{
  GenDebug("entered.");
  m_popup_pbutton = new QPushButton("...", this);
  connect(m_popup_pbutton, SIGNAL(pressed()), this, SLOT(displayPopupSlot()));
  m_hboxlayout->addWidget(m_popup_pbutton);
  m_stack_popup = new cStackPopup(this);
}
void cStackField::useFrame(bool use_frame){
  GenDebug("entered.");
  if(use_frame){
    if (m_data1_label->frameStyle() != (QFrame::Box | QFrame::Plain))
      m_data1_label->setFrameStyle(QFrame::Box | QFrame::Plain);
  } else if (m_data1_label->frameStyle() != QFrame::NoFrame)
    m_data1_label->setFrameStyle(QFrame::NoFrame);
}
void cStackField::displayPopupSlot(void){
  GenDebug("entered.");
  if(m_stack_popup->isVisible()){ m_stack_popup->hide(); }
  else {
    m_stack_popup->resize(width(), m_stack_popup->height());
    QDesktopWidget *d = QApplication::desktop();
    int w=d->width();
    int h=d->height();
    QPoint popup_pt(mapToGlobal(QPoint(m_data1_label->x(), m_data1_label->y())));
    if(popup_pt.x() + m_stack_popup->width() >= w){
      popup_pt.setX(w - (m_stack_popup->width() + 5));
    }
    if(popup_pt.y() + m_stack_popup->height() >= h){
      popup_pt.setY(h - (m_stack_popup->height() + 5));
    }
    m_stack_popup->move(popup_pt);
    m_stack_popup->show();
    m_stack_popup->updateState();
  }
  m_popup_pbutton->setDown(false);
}
void cStackField::setMissionControl(avd_MissionControl *mission_control){
  GenDebug("entered.");
  m_mission_control = mission_control;
  m_stack_popup->setMissionControl(mission_control);
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
}
void cStackField::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  m_stack_popup->setPopulationCell(cell_id);
  updateState();
}
void cStackField::setStackNumber(int stack_number){
  GenDebug("entered.");
  m_stack_no = stack_number;
  m_stack_popup->setStackNumber(stack_number);
  updateState();
}
void cStackField::updateState(void){
  GenDebug("entered.");
  if(!isVisible() || !m_mission_control) return;
  m_mission_control->lock();
  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)));
  if(!hardware){
    GenDebug(" --- 0 == hardware");
    setData1Text("");
    setData2Text("");
    useFrame(false);
  } else {
    setData1Text(QString("%1").arg( (unsigned long)hardware->GetStack(0, m_stack_no), 0, getBase()));
    //setData2Binary(hardware->GetStack(0, m_stack_no));
    GenDebug(" --- m_stack_no ")(m_stack_no);
    GenDebug(" --- m_cell_id ")(m_cell_id);
    GenDebug(" --- stack top ")(hardware->GetStack(0, m_stack_no));
    if (hardware->GetActiveStackID() == m_stack_no){
      GenDebug(" --- use frame");
      useFrame(true);
    } else {
      GenDebug(" --- don't use frame");
      useFrame(false);
    }
  }
  m_mission_control->unlock();
}

void cStackField::showData2Text(bool should_show){
  //m_stack_popup->showData2Text(should_show);
  cLabeledField::showData2Text(should_show);  
}
void cStackField::chooseBaseSlot(int base){
  GenDebug("entered.  base")(base)(".");
  m_stack_popup->chooseBaseSlot(base);
  cLabeledField::setBase(base);  
  updateState();
  GenDebug("done.");
}

cStackPopup::cStackPopup(QWidget *parent, const char *name)
: QWidget(parent, name, WType_Popup)
{
  GenDebug("entered.");
  m_vboxlayout = new QVBoxLayout(this);
  m_scrollview = new QScrollView(this);
  //  child widget fills scrollview,
  //  but resized scrollview is too narrow
  m_scrollview->setResizePolicy(QScrollView::AutoOneFit);
  //  child widget doesn't fill scrollview,
  //  resized scrollview is too narrow
  //m_scrollview->setResizePolicy(QScrollView::AutoOne);
  m_vboxlayout->addWidget(m_scrollview);
  // initially doesn't fill scrollbiew
  // and resized is too narrow and too short.
  m_layout_widget = new QWidget(m_scrollview->viewport());
  //setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
  //setResizePolicy(QScrollView::AutoOneFit);
  //m_layout_widget = new QWidget(viewport());
  m_layout_widget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
  m_scrollview->addChild(m_layout_widget);
  //addChild(m_layout_widget);
  m_sv_layout = new QVBoxLayout(m_layout_widget);
  //m_sv_layout->addWidget(new QPushButton("Hello, world", m_layout_widget));
  m_stacklines.setAutoDelete(TRUE);
  for(int i = 0; i < STACK_SIZE; i++){
    cLabeledField *labeled_field = new cLabeledField(m_layout_widget);
    m_sv_layout->addWidget(labeled_field);
    labeled_field->setTitle(QString("%1:").arg(i + 1));
    labeled_field->setData1DecimalWidth();
    //labeled_field->setData2BinaryWidth();
    m_stacklines.append(labeled_field);
  }
}
void cStackPopup::setMissionControl(avd_MissionControl *mission_control){
  GenDebug("entered.");
  m_mission_control = mission_control;
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
}
void cStackPopup::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  updateState();
}
void cStackPopup::setStackNumber(int stack_number){
  GenDebug("entered.");
  m_stack_no = stack_number;
  updateState();
}
void cStackPopup::updateState(void){
  GenDebug("entered.");
  if(!m_mission_control) return;
  if(!isVisible() || !isVisible()) return;
  m_mission_control->lock();
  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)));
  if(!hardware){
    int i = 0;
    cLabeledField *labeled_field = m_stacklines.first();
    for(; i < STACK_SIZE && 0 != labeled_field; i++, labeled_field = m_stacklines.next()){
      labeled_field->setData1Text("");
      //labeled_field->setData2Text("");
    }
  } else {
    GenDebug(" --- valid data it seems.");
    int i = 0;
    cLabeledField *labeled_field = m_stacklines.first();
    for(; i < STACK_SIZE && 0 != labeled_field; i++, labeled_field = m_stacklines.next()){
      labeled_field->setData1Text(
        QString("%1").arg((unsigned long)hardware->GetStack(i, m_stack_no), 0, labeled_field->getBase()));
      //labeled_field->setData2Binary(hardware->GetStack(i, m_stack_no));
    }
  }
  m_mission_control->unlock();
}
void cStackPopup::showData2Text(bool should_show){
  int i = 0;
  cLabeledField *labeled_field = m_stacklines.first();
  for(; i < STACK_SIZE && 0 != labeled_field; i++, labeled_field = m_stacklines.next())
  { labeled_field->showData2Text(should_show); }
}
void cStackPopup::chooseBaseSlot(int base){
  GenDebug("entered.  base")(base)(".");
  int i = 0;
  cLabeledField *labeled_field = m_stacklines.first();
  for(; i < STACK_SIZE && 0 != labeled_field; i++, labeled_field = m_stacklines.next())
  { labeled_field->setBase(base); }
  updateState();
  GenDebug("done.");
}

cStacksWidget::cStacksWidget(QWidget *parent, const char *name)
: QGroupBox(parent, name)
{
  GenDebug("entered.");
  setTitle("Stacks");
  setColumns(1);
  //setInsideSpacing(0);
  m_stackln1 = new cStackField(this);
  m_stackln2 = new cStackField(this);
  m_stackln1->setTitle("A:");
  m_stackln2->setTitle("B:");
  m_stackln1->setStackNumber(0);
  m_stackln2->setStackNumber(1);
  m_stackln1->setData1DecimalWidth();
  //m_stackln1->setData2BinaryWidth();
  m_stackln2->setData1DecimalWidth();
  //m_stackln2->setData2BinaryWidth();
}
void cStacksWidget::setMissionControl(avd_MissionControl *mission_control){
  GenDebug("entered.");
  m_mission_control = mission_control;
  m_stackln1->setMissionControl(mission_control);
  m_stackln2->setMissionControl(mission_control);
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
}
void cStacksWidget::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  m_stackln1->setPopulationCell(cell_id);
  m_stackln2->setPopulationCell(cell_id);
  updateState();
}
void cStacksWidget::updateState(void){
  GenDebug("entered.");
}
void cStacksWidget::showData2Text(bool should_show){
  //m_stackln1->showData2Text(should_show);
  //m_stackln2->showData2Text(should_show);
}
void cStacksWidget::chooseBaseSlot(int base){
  GenDebug("entered.  base")(base)(".");
  m_stackln1->chooseBaseSlot(base);
  m_stackln2->chooseBaseSlot(base);
  GenDebug("done.");
}


cBaseChoiceBtns::cBaseChoiceBtns(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  GenDebug("entered.");

  QHBoxLayout *layout = new QHBoxLayout(this);
  
  //layout->addItem(new QSpacerItem(20, 20));

  //m_widgetstack = new QWidgetStack(this); 
  //layout->addWidget(m_widgetstack);

  //layout->addItem(new QSpacerItem(20, 20));

  ///*
  //FIXME:
  //restore hide/show binary after fixups.  -- kgn
  //*/
  //m_show_binary_pbutton = new QPushButton("Show Binary", m_widgetstack);
  //m_hide_binary_pbutton = new QPushButton("Hide Binary", m_widgetstack);

  //connect(
  //  m_show_binary_pbutton, SIGNAL(clicked()),
  //  this, SLOT(showBinary())
  //);

  //connect(
  //  m_hide_binary_pbutton, SIGNAL(clicked()),
  //  this, SLOT(hideBinary())
  //);

  //m_widgetstack->raiseWidget(m_show_binary_pbutton);
  QHButtonGroup *hbg = new QHButtonGroup(
    "Number System",
    this,
    "<cBaseChoiceBtns::cBaseChoiceBtns(hbg)>"
  );
  layout->addWidget(hbg);
  hbg->insert(new QRadioButton("bin", hbg, "<cBaseChoiceBtns::cBaseChoiceBtns('bin')>"), 2);
  hbg->insert(new QRadioButton("hex", hbg, "<cBaseChoiceBtns::cBaseChoiceBtns('hex')>"), 16);
  hbg->insert(new QRadioButton("dec", hbg, "<cBaseChoiceBtns::cBaseChoiceBtns('dec')>"), 10);
  hbg->setRadioButtonExclusive(true);
  hbg->setButton(10);
  connect(hbg, SIGNAL(clicked(int)), this, SIGNAL(chooseBaseSig(int)));
}

void
cBaseChoiceBtns::showBinary(void){
  GenDebug("entered.");
  /*
  FIXME:
  restore hide/show binary after fixups.  -- kgn
  */
  m_widgetstack->raiseWidget(m_hide_binary_pbutton);
  emit showBinarySig(true);
}

void
cBaseChoiceBtns::hideBinary(void){
  GenDebug("entered.");
  /*
  FIXME:
  restore hide/show binary after fixups.  -- kgn
  */
  //m_widgetstack->raiseWidget(m_show_binary_pbutton);
  //emit showBinarySig(false);
}

cTasksWidget::cTasksWidget(QWidget *parent, const char *name)
: QGroupBox(parent, name)
{
  GenDebug("entered.");
  setTitle("Tasks");
  setColumns(1);
  m_scrollview = new QScrollView(this, "<cTasksWidget::m_scrollview>");
  m_scrollview->setResizePolicy(QScrollView::AutoOneFit);
  QWidget *layout_widget = new QWidget(
    m_scrollview->viewport(), "<cTasksWidget::(layout_widget)>");
  m_scrollview->addChild(layout_widget);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);
  m_vbox = new QVBox(layout_widget, "<cTasksWidget::m_vbox>");
  layout->addWidget(m_vbox);
  layout->addStretch(10);
  m_tasklines.setAutoDelete(true);
}
void cTasksWidget::setMissionControl(avd_MissionControl *mission_control){
  GenDebug("entered.");
  m_mission_control = mission_control;
  if(!mission_control) return;
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
  while(!m_tasklines.isEmpty()) m_tasklines.removeFirst();
  if(!mission_control->getPopulation()) return;
  const cTaskLib &task_lib = mission_control->getPopulation()->GetEnvironment().GetTaskLib();
  for(int task_num = 0; task_num < cConfig::GetNumTasks(); task_num++){
    cLabeledField *lf = new cLabeledField(m_vbox);
    lf->setTitle(QString(task_lib.GetTask(task_num).GetName()()));
    lf->showData2Text(false);
    m_tasklines.append(lf);
    GenDebug("made a new entry titled \"")(task_lib.GetTask(task_num).GetName())("\"");
  }
  GenDebug("done.");
}
void cTasksWidget::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  updateState();
}
void cTasksWidget::updateState(void){
  GenDebug("entered.");
  if(!isVisible() || !m_mission_control) return;
  m_mission_control->lock();
  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)));
  QPtrListIterator<cLabeledField> it(m_tasklines);
  cLabeledField *taskline;
  int task_num = 0;
  while((taskline = it.current()) != 0 && task_num < cConfig::GetNumTasks()){
    if(!phenotype) taskline->setData1Text("");
    else taskline->setData1Text(QString("%1").arg(phenotype->GetCurTaskCount()[task_num]));
    ++it; ++task_num;
  }
  m_mission_control->unlock();
}

N_Instruction_CPUWidget::N_Instruction_CPUWidget(
  QWidget *parent, const char *name, WFlags f
//) : QTabWidget(parent, name, f)
) : QVBox(parent, name, f)
{
  GenDebug("entered.");
  QTabWidget *cpu_tab_widget = new QTabWidget(this, "<N_Instruction_CPUWidget::m_component_vbox>");
    //m_component_vbox = new QVBox(this, "<N_Instruction_CPUWidget::m_component_vbox>");
    m_component_vbox = new QVBox(cpu_tab_widget, "<N_Instruction_CPUWidget::m_component_vbox>");
      QHBox *hbox = new QHBox(m_component_vbox);
        m_registers = new cRegistersWidget(hbox);
        m_inputs = new cInputsWidget(hbox);
      m_stacks = new cStacksWidget(m_component_vbox);
      m_basechoice_btns = new cBaseChoiceBtns(m_component_vbox);
      connect(m_basechoice_btns, SIGNAL(showBinarySig(bool)), this, SLOT(showData2Text(bool)));
      connect(m_basechoice_btns, SIGNAL(chooseBaseSig(int)), this, SLOT(chooseBaseSlot(int)));
      m_basechoice_btns->hideBinary();
      m_basechoice_btns->emitChooseBaseSig(10);
      //XXX
      //m_summary1 = new cSummaryWidget(m_component_vbox);
  //addTab(m_component_vbox, "CPU");
  cpu_tab_widget->addTab(m_component_vbox, "CPU");
  //  m_stats_vbox = new QVBox(this, "<N_Instruction_CPUWidget::m_stats_vbox>");
  //    m_summary2 = new cSummaryWidget(m_stats_vbox);
  //addTab(m_stats_vbox, "Stats");
    //m_tasks_vbox = new QVBox(this, "<N_Instruction_CPUWidget::m_tasks_vbox>");
    m_tasks_vbox = new QVBox(cpu_tab_widget, "<N_Instruction_CPUWidget::m_tasks_vbox>");
     
      m_tasks = new cTasksWidget(m_tasks_vbox, "<N_Instruction_CPUWidget::m_tasks>");
       //XXX
       //m_summary3 = new cSummaryWidget(m_tasks_vbox);
  //addTab(m_tasks_vbox, "Tasks");
  cpu_tab_widget->addTab(m_tasks_vbox, "Tasks");
  //  m_genotype_vbox = new QVBox(this, "<N_Instruction_CPUWidget::m_genotype_vbox>");
  //    m_summary4 = new cSummaryWidget(m_genotype_vbox);
  //addTab(m_genotype_vbox, "Genotype");
  //showPage(m_component_vbox);
       //m_summary3 = new cSummaryWidget(m_tasks_vbox);
  QVGroupBox *vgroupbox = new QVGroupBox("Summary", this, "<N_Instruction_CPUWidget::(vgroupbox)>");
  QScrollView *scrollview = new QScrollView(vgroupbox, "<N_Instruction_CPUWidget::(scrollview)>");
  //m_summary1 = new cSummaryWidget(this);
  m_summary1 = new cSummaryWidget(scrollview->viewport(), "<N_Instruction_CPUWidget::m_summary1>");
  scrollview->addChild(m_summary1);

  //scrollview->viewport()->setResizePolicy(QScrollView::AutoOneFit);
  scrollview->setResizePolicy(QScrollView::AutoOneFit);

  cpu_tab_widget->showPage(m_component_vbox);
}
void N_Instruction_CPUWidget::setMissionControl(avd_MissionControl *mission_control) {
  GenDebug("entered.");
  m_mission_control = mission_control;
  //XXX
  m_summary1->setMissionControl(mission_control);
  m_registers->setMissionControl(mission_control);
  m_stacks->setMissionControl(mission_control);
  m_inputs->setMissionControl(mission_control);
  //m_summary2->setMissionControl(mission_control);
  //XXX
  //m_summary3->setMissionControl(mission_control);
  m_tasks->setMissionControl(mission_control);
  //m_summary4->setMissionControl(mission_control);
  connect(mission_control, SIGNAL(avidaUpdatedSig()), this, SLOT(updateState()));
  connect(mission_control, SIGNAL(avidaSteppedSig(int)), this, SLOT(updateState()));
}
void N_Instruction_CPUWidget::setPopulationCell(int cell_id){
  GenDebug("entered.");
  m_cell_id = cell_id;
  //XXX
  m_summary1->setPopulationCell(cell_id);
  m_registers->setPopulationCell(cell_id);
  m_stacks->setPopulationCell(cell_id);
  m_inputs->setPopulationCell(cell_id);
  //m_summary2->setPopulationCell(cell_id);
  //XXX
  //m_summary3->setPopulationCell(cell_id);
  m_tasks->setPopulationCell(cell_id);
  //m_summary4->setPopulationCell(cell_id);
}
void N_Instruction_CPUWidget::updateState(void)
{ GenDebug("entered."); }
void N_Instruction_CPUWidget::showData2Text(bool should_show){
  m_registers->showData2Text(should_show);
  m_stacks->showData2Text(should_show);
  m_inputs->showData2Text(should_show);
}
void N_Instruction_CPUWidget::chooseBaseSlot(int base){
  GenDebug("entered.  base")(base)(".");
  m_registers->chooseBaseSlot(base);
  m_stacks->chooseBaseSlot(base);
  m_inputs->chooseBaseSlot(base);
  GenDebug("done.");
}
