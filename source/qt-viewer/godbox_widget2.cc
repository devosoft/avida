//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

// including <float.h> to get DBL_DIG
// -- Number of decimal digits of precision in a double
#include <float.h>

#include <qapplication.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qvalidator.h>
#include <qwidget.h>

#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>


#include "config.hh"
#include "environment.hh"
#include "event_list.hh"
#include "genome.hh"
#include "inst_util.hh"
#include "population.hh"
#include "population_cell.hh"
#include "string.hh"
#ifndef TOOLS_HH
#include "tools.hh"
#endif

#include "apocalypse_widget.hh"
#include "avd_rate_slider2.hh"
#include "avd_rate_validator2.hh"
#include "avd_lograte_widget.hh"

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif

//#include "messaging_population_wrapper.hh"
#include "repopulate_widget.hh"

#include "godbox_widget2.hh"

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


/*
static class variables
*/
QString GodBoxWidget2::s_preset_organisms_dir;


/*
public member functions
*/

GodBoxWidget2::GodBoxWidget2(
  QWidget *parent,
  const char *name,
  WFlags f
)
//: GodBox2(parent, name, f)
//: QWidget(parent, name, f)
: QMainWindow(parent, name, f)
{
  GenDebug << "constructor.";

  m_mission_control = 0;

  setCaption(trUtf8("Tweak Basic World Parameters"));
  setupLayout();
  minimizeLayout();
  setupRates();

  GenDebug("done.");
}

GodBoxWidget2::~GodBoxWidget2(void)
{
  GenDebug << "destructor.";

  GenDebug("done.");
}

void
GodBoxWidget2::GodBoxWidget2_dynamicConfig(void)
{
  GenDebug << "entered.";

  /*
  FIXME:  preset_organisms shouldn't be hard-wired!  -- kaben.
  */
#ifdef PLATFORM_IS_MAC_OS_X
  QFileInfo app_fi(qApp->argv()[0]);
  QDir app_dir(app_fi.dir(false));
  app_dir.cd("../Resources/preset_organisms");
  s_preset_organisms_dir = app_dir.path();
#else
  s_preset_organisms_dir =
    QString(cConfig::GetDefaultDir()) + "preset_organisms/";
#endif
  GenDebug << " --- s_preset_organisms_dir " << s_preset_organisms_dir;

  GenDebug("done.");
}

void
GodBoxWidget2::setMissionControl(avd_MissionControl *mission_control)
{
  GenDebug << "entered.";

  m_mission_control = mission_control;
  /*
  add menu items
  */
  QPopupMenu *file_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(file_menu)>");
    //file_menu->insertItem( "Open Avida Session", this, SLOT(openAvidaSlot()), CTRL+Key_O);
    //file_menu->insertItem( "Close Avida Session", this, SLOT(closeAvidaSlot()));
    //file_menu->insertItem( "Reset Avida Session", this, SLOT(resetAvidaSlot()));
    //file_menu->insertItem( "Settings", m_mission_control, SIGNAL(userSettingsSig()));
    //file_menu->insertItem( "Quit", this, SIGNAL(quitSig()), CTRL+Key_Q);

    file_menu->insertItem( "Start Avida", m_mission_control, SIGNAL(startAvidaSig()));
    file_menu->insertItem( "Pause Avida", m_mission_control, SIGNAL(stopAvidaSig()));
    file_menu->insertItem( "Step Avida", m_mission_control, SIGNAL(stepAvidaSig()));
    file_menu->insertItem( "Update Avida", m_mission_control, SIGNAL(updateAvidaSig()));

    file_menu->insertSeparator();

    file_menu->insertItem( "Close Control Box", this, SLOT(close()), CTRL+Key_W);
    file_menu->insertItem( "Quit", m_mission_control, SIGNAL(quitSig()), CTRL+Key_Q);

  menuBar()->insertItem( "File", file_menu);

  //QPopupMenu *control_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(control_menu)>");
  //  control_menu->insertItem( "Start Avida", this, SLOT(startAvidaSlot()));
  //  control_menu->insertItem( "Pause Avida", this, SLOT(stopAvidaSlot()));
  //  control_menu->insertItem( "Step Avida", this, SLOT(stepAvidaSlot()));
  //  control_menu->insertItem( "Update Avida", this, SLOT(updateAvidaSlot()));
  //menuBar()->insertItem( "Control", control_menu);

  QPopupMenu *view_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(view_menu)>");
    view_menu->insertItem( "New Map Viewer", m_mission_control, SIGNAL(newMainWindowSig()), CTRL+Key_N);
    view_menu->insertItem( "Instruction Viewer", m_mission_control, SIGNAL(newInstructionViewerSig()));
    view_menu->insertItem( "Event Editor", m_mission_control, SIGNAL(newEventViewerSig()));
    view_menu->insertItem( "Control Box", m_mission_control, SIGNAL(newGodBoxSig()));
    view_menu->insertItem( "New Plot Viewer...", m_mission_control, SIGNAL(newPlotViewerSig()));
  menuBar()->insertItem( "Viewers", view_menu);

  QPopupMenu *help_menu = new QPopupMenu( this, "<N_Instruction_Viewer::N_Instruction_Viewer(help_menu)>");
    help_menu->insertItem("Documentation Contents", this, SLOT(helpContents())); help_menu->insertSeparator();
    help_menu->insertItem("Setting up the Genesis File", this, SLOT(helpGenesis()));
    help_menu->insertItem("Setting up the Instruction Set", this, SLOT(helpInstructionSet()));
    help_menu->insertItem("Guidelines to setting Events", this, SLOT(helpEvents()));
    help_menu->insertItem("Guidelines to setting up Environments", this, SLOT(helpEnvironments()));
    help_menu->insertItem("Guidelines to Analyze Mode", this, SLOT(helpAnalyzeMode()));
  menuBar()->insertItem( "Help", help_menu);

  GenDebug("done.");
}

void
GodBoxWidget2::setup(void){
  GenDebug << "entered.";

  if(m_mission_control){
    setupConnects();
    loadMutRates();
    loadCreatureList();
  }
  GenDebug("done.");
}

void
GodBoxWidget2::setupLayout(void)
{
  GenDebug << "entered.";

  QWidget *central_widget = new QWidget(
    this,
    "<GodBoxWidget2::setupLayout(central_widget)>"
  );
  setCentralWidget(central_widget);

  m_vbox_layout = new QVBoxLayout(
    central_widget,
    11,
    6,
    "<GodBoxWidget2::m_vbox_layout>"
  );

  m_hbox_controls_layout = new QHBoxLayout(0);

  m_mutation_rates_groupbox = new QGroupBox(
    1,
    Qt::Vertical, 
    central_widget
  );
  m_mutation_rates_groupbox->setTitle(trUtf8("Mutation Rates"));
  m_hbox_controls_layout->addWidget(m_mutation_rates_groupbox);

  m_pt_mutation_widget = new avd_LogRateWidget(m_mutation_rates_groupbox);
  m_pt_mutation_widget->setLabel(trUtf8("Cosmic Rays:"));

  m_ins_del_mutation_widget = new avd_LogRateWidget(m_mutation_rates_groupbox);
  m_ins_del_mutation_widget->setLabel(trUtf8("Insert/Delete:"));

  m_cp_mutation_widget = new avd_LogRateWidget(m_mutation_rates_groupbox);
  m_cp_mutation_widget->setLabel(trUtf8("Copy:"));

  m_apocalypse_groupbox = new QGroupBox(
    1,
    Qt::Vertical, 
    this
  );
  m_apocalypse_groupbox->setTitle(trUtf8("Apocalypse"));
  m_hbox_controls_layout->addWidget(m_apocalypse_groupbox);

  m_apocalypse_widget = new ApocalypseWidget(m_apocalypse_groupbox);
  m_apocalypse_widget->setLabel(trUtf8("Death Rate:"));
  m_apocalypse_widget->setButtonText(trUtf8("Kill Creatures"));

  m_repopulate_groupbox = new QGroupBox(
    1,
    Qt::Vertical, 
    this
  );
  m_repopulate_groupbox->setTitle(trUtf8("Repopulate"));
  m_hbox_controls_layout->addWidget(m_repopulate_groupbox);

  m_repopulate_widget = new RepopulateWidget(m_repopulate_groupbox);
  m_repopulate_widget->setLabel(trUtf8("New Creature Type:"));
  m_repopulate_widget->setInjectButtonText(trUtf8("Inject One Creature"));
  m_repopulate_widget->setInjectAllButtonText(trUtf8("Replace Population"));


  m_hbox_buttons_layout = new QHBoxLayout(0);

  /*
  FIXME:  reconnect help button soon.  -- kgn.
  */
  //m_help_button = new QPushButton(central_widget);
  //m_help_button->setText(trUtf8("Help"));
  //m_hbox_buttons_layout->addWidget(m_help_button);

  QSpacerItem *m_button_spacer = new QSpacerItem(
    20,
    20,
    QSizePolicy::Expanding,
    QSizePolicy::Minimum
  );
  m_hbox_buttons_layout->addItem(m_button_spacer);

  m_close_button = new QPushButton(central_widget);
  m_close_button->setText(trUtf8("Close"));
  m_hbox_buttons_layout->addWidget(m_close_button);

  m_vbox_layout->addLayout(m_hbox_controls_layout);
  m_vbox_layout->addLayout(m_hbox_buttons_layout);

  GenDebug("done.");
}

void
GodBoxWidget2::minimizeLayout(void)
{
  GenDebug << "entered.";
  
  int minimum_widget_width = 0;

  /*
  set widths of mutation subwidgets to max. of their min. widths.
  */
  m_pt_mutation_widget->adjustSize();
  m_ins_del_mutation_widget->adjustSize();
  m_cp_mutation_widget->adjustSize();

  if (minimum_widget_width < m_pt_mutation_widget->width())
    minimum_widget_width = m_pt_mutation_widget->width();

  if (minimum_widget_width < m_ins_del_mutation_widget->width())
    minimum_widget_width = m_ins_del_mutation_widget->width();

  if (minimum_widget_width < m_cp_mutation_widget->width())
    minimum_widget_width = m_cp_mutation_widget->width();

  m_pt_mutation_widget->setFixedWidth(minimum_widget_width);
  m_ins_del_mutation_widget->setFixedWidth(minimum_widget_width);
  m_cp_mutation_widget->setFixedWidth(minimum_widget_width);

  /*
  minimize the rest of the subwidgets.
  */
  m_apocalypse_widget->adjustSize();
  m_repopulate_widget->adjustSize();

  /*
  this sets a minimum size for whole widget.
  */
  m_repopulate_widget->setFixedHeight(200);

  /*
  minimize the rest of the container subwidgets.
  */
  m_mutation_rates_groupbox->adjustSize();
  m_apocalypse_groupbox->adjustSize();
  m_repopulate_groupbox->adjustSize();
  m_hbox_controls_layout->activate();

  //adjustSize();
  GenDebug << " --- main width " << width() << '.';

  //setFixedWidth(width());
  //setMinimumHeight(300);

  GenDebug("done.");
}

/*
private member functions
*/

void
GodBoxWidget2::setupRates(void)
{
  GenDebug << "entered.";

  m_rate_validator = new avd_RateValidator2(this);

  m_pt_mutation_widget->setRateValidator(m_rate_validator);
  m_ins_del_mutation_widget->setRateValidator(m_rate_validator);
  m_cp_mutation_widget->setRateValidator(m_rate_validator);
  m_apocalypse_widget->setRateValidator(m_rate_validator);

  m_pt_mutation_widget->setMinRate(0.00001);
  m_ins_del_mutation_widget->setMinRate(0.00001);
  m_cp_mutation_widget->setMinRate(0.00001);
  m_apocalypse_widget->setMaxRate(0.99);

  m_apocalypse_widget->valueChangedSlot(0.99);

  m_pt_mutation_widget->setLineStep(0.005);
  m_ins_del_mutation_widget->setLineStep(0.005);
  m_cp_mutation_widget->setLineStep(0.005);
  //m_apocalypse_widget

  m_pt_mutation_widget->setPageStep(0.05);
  m_ins_del_mutation_widget->setPageStep(0.05);
  m_cp_mutation_widget->setPageStep(0.05);
  //m_apocalypse_widget

  GenDebug("done.");
}

void
GodBoxWidget2::setupConnects(void)
{
  GenDebug << "entered.";

  connect(
    m_pt_mutation_widget, SIGNAL(rateChanged(double)),
    this, SLOT(ptMutRateWidgetChanged(double))
  );

  connect(
    m_ins_del_mutation_widget, SIGNAL(rateChanged(double)),
    this, SLOT(insDelMutRateWidgetChanged(double))
  );

  connect(
    m_cp_mutation_widget, SIGNAL(rateChanged(double)),
    this, SLOT(cpMutRateWidgetChanged(double))
  );

  connect(
    m_apocalypse_widget, SIGNAL(clicked(void)),
    this, SLOT(apocalypsePushButtonClicked(void))
  );

  connect(
    m_repopulate_widget, SIGNAL(injectOneButtonClicked(void)),
    this, SLOT(injectOneButtonClicked(void))
  );
  connect(
    m_repopulate_widget, SIGNAL(injectAllButtonClicked(void)),
    this, SLOT(injectAllButtonClicked(void))
  );

  /*
  FIXME:  reconnect help button soon.  -- kgn.
  */
  //connect(
  //  m_help_button, SIGNAL(clicked(void)),
  //  this, SLOT(helpButtonClicked(void))
  //);
  connect(
    m_close_button, SIGNAL(clicked(void)),
    this, SLOT(closeButtonClicked(void))
  );

  connect(
    m_mission_control, SIGNAL(avidaUpdatedSig()),
    this, SLOT(stateChanged())
  );
  connect(
    m_mission_control, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(stateChanged())
  );

  GenDebug("done.");
}

void
GodBoxWidget2::loadMutRates(void)
{
  GenDebug("entered.");

  if (!m_pt_mutation_widget->hasFocus()){
    m_pt_mutation_widget->setRate(cConfig::GetPointMutProb());
  }

  if (!m_ins_del_mutation_widget->hasFocus()){
    m_ins_del_mutation_widget->setRate(cConfig::GetInsMutProb());
  }

  if (!m_cp_mutation_widget->hasFocus()){
    m_cp_mutation_widget->setRate(cConfig::GetCopyMutProb());
  }

  GenDebug("done.");
}

void
GodBoxWidget2::loadCreatureList(void)
{
  GenDebug << "entered.";

  m_repopulate_widget->clear();
  m_repopulate_widget->insertItem("-default-");

#ifdef PLATFORM_IS_MAC_OS_X
  QFileInfo app_fi(qApp->argv()[0]);
  QDir preset_organisms_dir_access(app_fi.dir(false));
  preset_organisms_dir_access.cd("../Resources/preset_organisms");
#else
  QDir preset_organisms_dir_access(s_preset_organisms_dir); 
#endif
  preset_organisms_dir_access.setFilter(
      QDir::Files
    | QDir::Readable
  );
  preset_organisms_dir_access.setSorting(
      QDir::Name
  );

  GenDebug << " --- path " << preset_organisms_dir_access.path();
  GenDebug << " --- absPath " << preset_organisms_dir_access.absPath();
  GenDebug
  << " --- canonicalPath "
  << preset_organisms_dir_access.canonicalPath();
  GenDebug << " --- dirName " << preset_organisms_dir_access.dirName();

  GenDebug << " --- available creatures: ";

  QStringList preset_organism(
    preset_organisms_dir_access.entryList()
  );

  for(
    QStringList::Iterator it = preset_organism.begin();
    it != preset_organism.end();
    ++it
  ){
    GenDebug << " --- " << *it;
    m_repopulate_widget->insertItem(*it);
  }

  GenDebug("done.");
}

/*
private slots
*/

void
GodBoxWidget2::ptMutRateWidgetChanged(double rate)
{
  GenDebug << "rate " << rate << '.';

  m_mission_control->lock();
  if(m_mission_control->getPopulation()){
    for (int i = 0; i < m_mission_control->getPopulation()->GetSize(); i++) {
      m_mission_control->
        getPopulation()->
          GetCell(i).MutationRates().SetPointMutProb(rate);
    }
    cConfig::SetPointMutProb(rate);
  }
  m_mission_control->unlock();

  GenDebug("done.");
}

void
GodBoxWidget2::insDelMutRateWidgetChanged(double rate)
{
  GenDebug << "rate " << rate << '.';

  m_mission_control->lock();
  if(m_mission_control->getPopulation()){
    for (int i = 0; i < m_mission_control->getPopulation()->GetSize(); i++) {
      m_mission_control->
        getPopulation()->
          GetCell(i).MutationRates().SetInsMutProb(rate);
      m_mission_control->
        getPopulation()->
          GetCell(i).MutationRates().SetDelMutProb(rate);
    }
    cConfig::SetInsMutProb(rate);
    cConfig::SetDelMutProb(rate);
  }
  m_mission_control->unlock();

  GenDebug("done.");
}

void
GodBoxWidget2::cpMutRateWidgetChanged(double rate)
{
  GenDebug << "rate " << rate << '.';

  m_mission_control->lock();
  if(m_mission_control->getPopulation()){
    for (int i = 0; i < m_mission_control->getPopulation()->GetSize(); i++) {
      m_mission_control->
        getPopulation()->
          GetCell(i).MutationRates().SetCopyMutProb(rate);
    }
    cConfig::SetCopyMutProb(rate);
  }
  m_mission_control->unlock();

  GenDebug("done.");
}

void
GodBoxWidget2::apocalypsePushButtonClicked(void)
{
  GenDebug << "entered.";

  double kill_prob = m_apocalypse_widget->rate();
  GenDebug << " --- kill_prob " << kill_prob << '.';

  m_mission_control->lock();
  if(m_mission_control->getPopulation()){
    for (int i = 0; i < m_mission_control->getPopulation()->GetSize(); i++) {
      cPopulationCell & cell = m_mission_control->getPopulation()->GetCell(i);
      if (cell.IsOccupied() == false)
        continue;
      if (g_random.P(kill_prob))
        m_mission_control->getPopulation()->KillOrganism(cell);
    }
  }
  m_mission_control->unlock();

  /* cheat -- force all widgets to update. */
  //m_mission_control->avidaUpdatedSlot();
  m_mission_control->emitUpdateAvidaSig();

  GenDebug("done.");
}

void
GodBoxWidget2::injectOneButtonClicked(void)
{
  GenDebug << "entered.";

  if(!m_mission_control->getPopulation()){

    GenDebug("no population; returning early.");
    return;
  }

  cString fname;
  int cell_id = 0;
  double merit = -1;
  int lineage_label = 0;
  double neutral_metric = 0;


  /*
  FIXME:
  code duplication between
  injectOneButtonClicked() and injectAllButtonClicked().

  -- kaben.
  */
  if(m_repopulate_widget->currentText() == "-default-"){
    fname = cConfig::GetStartCreature();
  }else{
    fname = (
      s_preset_organisms_dir + m_repopulate_widget->currentText()
    ).latin1();
  }

  m_mission_control->lock();
  cGenome genome = cInstUtil::LoadGenome(
    fname,
    m_mission_control->getPopulation()->GetEnvironment().GetInstSet()
  );
  m_mission_control->unlock();

  if(genome.GetSize() == 1){
    /*
    FIXME:
    need error handling here.

    -- kaben.
    */
    GenDebug << " --- bogus genome.";
  } else {
    GenDebug << " --- fname: " << fname;
    
    m_mission_control->lock();
    m_mission_control->getPopulation()->Inject(
      genome,
      cell_id,
      merit,
      lineage_label,
      neutral_metric
    );
    m_mission_control->unlock();

    /* cheat -- force all widgets to update. */
    //m_mission_control->avidaUpdatedSlot();
    m_mission_control->emitUpdateAvidaSig();
  }

  GenDebug("done.");
}

void
GodBoxWidget2::injectAllButtonClicked(void)
{
  GenDebug << "entered.";

  if(!m_mission_control->getPopulation()){

    GenDebug("no population; returning early.");
    return;
  }


  cString fname;
  int cell_id = 0;
  double merit = -1;
  int lineage_label = 0;
  double neutral_metric = 0;

  /*
  FIXME:
  code duplication between
  injectOneButtonClicked() and injectAllButtonClicked().

  -- kaben.
  */
  if(m_repopulate_widget->currentText() == "-default-"){
    fname = cConfig::GetStartCreature();
  }else{
    fname = (
      s_preset_organisms_dir + m_repopulate_widget->currentText()
    ).latin1();
  }

  cGenome genome = cInstUtil::LoadGenome(
    fname,
    m_mission_control->getPopulation()->GetEnvironment().GetInstSet()
  );

  if(genome.GetSize() == 1){
    /*
    FIXME:
    need error handling here.

    -- kaben.
    */
    GenDebug << " --- bogus genome.";
  } else {
    GenDebug << " --- fname: " << fname;
    
    m_mission_control->lock();
    for (int i = 0; i < m_mission_control->getPopulation()->GetSize(); i++) {
      m_mission_control->getPopulation()->Inject(
        genome,
        i,
        merit,
        lineage_label,
        neutral_metric
      );
    }
    m_mission_control->getPopulation()->SetSyncEvents(true);
    m_mission_control->unlock();

    /* cheat -- force all widgets to update. */
    //m_mission_control->avidaUpdatedSlot();
    m_mission_control->emitUpdateAvidaSig();
  }

  GenDebug("done.");
}

void
GodBoxWidget2::helpContents(){
  m_mission_control->emitHelpURLSig("index.html");
}
void
GodBoxWidget2::helpGenesis(){
  m_mission_control->emitHelpURLSig("genesis.html");
}
void
GodBoxWidget2::helpInstructionSet(){
  m_mission_control->emitHelpURLSig("inst_set.html");
}
void
GodBoxWidget2::helpEvents(){
  m_mission_control->emitHelpURLSig("events.html");
}
void
GodBoxWidget2::helpEnvironments(){
  m_mission_control->emitHelpURLSig("environment.html");
}
void
GodBoxWidget2::helpAnalyzeMode(){
  m_mission_control->emitHelpURLSig("analyze_mode.html");
}

void
GodBoxWidget2::helpButtonClicked(void)
{
  GenDebug << "entered.";

  /*
  FIXME:  replace with real help documentation.  -- kgn
  */
  QMessageBox::information(
    this,
    "Environment Controller Help",
    "The beta version of Avida "
    "lacks documentation for this "
    "part of the user interface. "
    "We're working on it."
  );

  GenDebug("done.");
}


void
GodBoxWidget2::closeButtonClicked(void)
{
  GenDebug << "entered.";

  close();

  GenDebug("done.");
}

void
GodBoxWidget2::stateChanged(void)
{
  GenDebug << "entered.";

  loadMutRates();

  GenDebug("done.");
}
