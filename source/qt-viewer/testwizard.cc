
#ifndef TESTWIZARD_HH
#include "testwizard.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#ifndef SETUP_GENESIS_HH
#include "setup_genesis.hh"
#endif

#include "file.hh"

#include <qapplication.h>
#include <qradiobutton.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qmessagebox.h>


using namespace std;


QString
TestWizard::invalidPageValidator(QWidget *page){
  return QString("error --- unknown setup page");
}

TestWizard::PageFunction
TestWizard::PageFunctWrap::s_default_validator_function
= &TestWizard::invalidPageValidator;

TestWizard::TestWizard(
  int argc,
  char *argv[],
  QWidget *parent,
  const char *name,
  bool modal,
  WFlags f
)
: m_argc(argc),
  m_argv(argv),
  SetupAssistant(parent, name, modal, f)
{
  m_end_condition_map.insert(
    GenesisWrapper::MaxUpdatesORMaxGenerations, 0
  );
  m_end_condition_map.insert(
    GenesisWrapper::MaxUpdatesANDMaxGenerations, 1
  );


  m_birth_method_map.insert(
    GenesisWrapper::ReplaceRandomInNeighborhood, 0
  );
  m_birth_method_map.insert(
    GenesisWrapper::ReplaceOldestInNeighborhood, 1
  );
  m_birth_method_map.insert(
    GenesisWrapper::ReplaceLargestAgeOverMeritInNeighborhood, 2
  );
  m_birth_method_map.insert(
    GenesisWrapper::PlaceInEmptyCellsInNeighborhood, 3
  );
  m_birth_method_map.insert(
    GenesisWrapper::ReplaceRandomInPopulation, 4
  );
  m_birth_method_map.insert(
    GenesisWrapper::ReplaceOldestInPopulation, 5
  );


  m_death_method_map.insert(
    GenesisWrapper::NeverDie, 0
  );
  m_death_method_map.insert(
    GenesisWrapper::InstrExecReachesAgeLimit, 1
  );
  m_death_method_map.insert(
    GenesisWrapper::InstrExecReachesAgeLimitTimesLength, 2
  );


  m_alloc_method_map.insert(
    GenesisWrapper::DefaultInst, 0
  );
  m_alloc_method_map.insert(
    GenesisWrapper::Necrophilia, 1
  );
  m_alloc_method_map.insert(
    GenesisWrapper::Random, 2
  );


  m_divide_method_map.insert(
    GenesisWrapper::MotherAndChild, 0
  );
  m_divide_method_map.insert(
    GenesisWrapper::BothChildren, 1
  );


  m_generation_inc_method_map.insert(
    GenesisWrapper::IncrChildOnly, 0
  );
  m_generation_inc_method_map.insert(
    GenesisWrapper::IncrMotherAndChild, 1
  );


  m_slicing_method_map.insert(
    GenesisWrapper::Constant, 0
  );
  m_slicing_method_map.insert(
    GenesisWrapper::Probabilistic, 1
  );
  m_slicing_method_map.insert(
    GenesisWrapper::Integrated, 2
  );


  m_size_merit_method_map.insert(
    GenesisWrapper::MeritOff, 0
  );
  m_size_merit_method_map.insert(
    GenesisWrapper::CopiedSize, 1
  );
  m_size_merit_method_map.insert(
    GenesisWrapper::ExecutedSize, 2
  );
  m_size_merit_method_map.insert(
    GenesisWrapper::FullSize, 3
  );
  m_size_merit_method_map.insert(
    GenesisWrapper::MinExecAndCopied, 4
  );
  m_size_merit_method_map.insert(
    GenesisWrapper::SqrtMinSize, 5
  );


  m_merit_time_map.insert(
    GenesisWrapper::TaskCompletion, 0
  );
  m_merit_time_map.insert(
    GenesisWrapper::PostDivision, 1
  );


  m_species_recording_map.insert(
    GenesisWrapper::SpeciesRecordingOff, 0
  );
  m_species_recording_map.insert(
    GenesisWrapper::FullSpeciesRecording, 1
  );
  m_species_recording_map.insert(
    GenesisWrapper::LimitSpeciesSearch, 2
  );


  m_log_genotypes_map.insert(
    GenesisWrapper::GenotypeLoggingOff, 0
  );
  m_log_genotypes_map.insert(
    GenesisWrapper::LogAllGenotypes, 1
  );
  m_log_genotypes_map.insert(
    GenesisWrapper::LogThresholdGenotypes, 2
  );


  m_log_genealogy_map.insert(
    GenesisWrapper::GenealogyLoggingOff, 0
  );
  m_log_genealogy_map.insert(
    GenesisWrapper::LogAllGenealogy, 1
  );
  m_log_genealogy_map.insert(
    GenesisWrapper::LogParentGenealogy, 2
  );


  m_lineage_creation_method_map.insert(
    GenesisWrapper::ManualLineageCreation, 0
  );
  m_lineage_creation_method_map.insert(
    GenesisWrapper::ChildFitnessVsParent, 1
  );
  m_lineage_creation_method_map.insert(
    GenesisWrapper::ChildFitnessVsPopulation, 2
  );
  m_lineage_creation_method_map.insert(
    GenesisWrapper::ChildFitnessVsDomLinChildInDom, 3
  );
  m_lineage_creation_method_map.insert(
    GenesisWrapper::ChildFitnessVsDomLinAndOwn, 4
  );
  m_lineage_creation_method_map.insert(
    GenesisWrapper::ChildFitnessVsDomLinAndOwn2, 5
  );
  m_lineage_creation_method_map.insert(
    GenesisWrapper::ChildFitnessVsOwnLin, 6
  );


  m_debug_level_map.insert(
    GenesisWrapper::DebugErrorsOnly, 0
  );
  m_debug_level_map.insert(
    GenesisWrapper::DebugWarnings, 1
  );
  m_debug_level_map.insert(
    GenesisWrapper::DebugComments, 2
  );


  m_on_off_map.insert(
    GenesisWrapper::Off, 0
  );
  m_on_off_map.insert(
    GenesisWrapper::On, 1
  );


  // create validator mappings
  //m_vmap[m_genesis_type_page] = PageFunctWrap(&TestWizard::
  //  genesisTypePageValidator
  //);
  insertSetupAndValidatorMappings(
    m_genesis_type_page,
    &TestWizard::genesisTypePageSetup,
    &TestWizard::genesisTypePageValidator
  );
  insertSetupAndValidatorMappings(
    m_existing_genesis_select_page,
    &TestWizard::existingGenesisSelectPageSetup,
    &TestWizard::existingGenesisSelectPageValidator
  );
  insertSetupAndValidatorMappings(
    m_modify_genesis_select_page,
    &TestWizard::modifyGenesisSelectPageSetup,
    &TestWizard::modifyGenesisSelectPageValidator
  );
  insertSetupAndValidatorMappings(
    m_setup_style_page,
    &TestWizard::setupStylePageSetup,
    &TestWizard::setupStylePageValidator
  );
  insertSetupAndValidatorMappings(
    m_simple_topology_page,
    &TestWizard::simpleTopologyPageSetup,
    &TestWizard::simpleTopologyPageValidator
  );
  insertSetupAndValidatorMappings(
    m_simple_2d_size_page,
    &TestWizard::simple2dSizePageSetup,
    &TestWizard::simple2dSizePageValidator
  );
  insertSetupAndValidatorMappings(
    m_simple_wellmixed_size_page,
    &TestWizard::simpleWellmixedSizePageSetup,
    &TestWizard::simpleWellmixedSizePageValidator
  );
  insertSetupAndValidatorMappings(
    m_simple_populate_page,
    &TestWizard::simplePopulatePageSetup,
    &TestWizard::simplePopulatePageValidator
  );
  insertSetupAndValidatorMappings(
    m_simple_endcondition_page,
    &TestWizard::simpleEndconditionPageSetup,
    &TestWizard::simpleEndconditionPageValidator
  );
  insertSetupAndValidatorMappings(
    m_simple_environment_page,
    &TestWizard::simpleEnvironmentPageSetup,
    &TestWizard::simpleEnvironmentPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_endcondition_page,
    &TestWizard::detailedEndconditionPageSetup,
    &TestWizard::detailedEndconditionPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_worldsize_page,
    &TestWizard::detailedWorldsizePageSetup,
    &TestWizard::detailedWorldsizePageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_cputhreads_page,
    &TestWizard::detailedCPUThreadsPageSetup,
    &TestWizard::detailedCPUThreadsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_randomseed_page,
    &TestWizard::detailedRandomseedPageSetup,
    &TestWizard::detailedRandomseedPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_filepaths_page,
    &TestWizard::detailedFilepathsPageSetup,
    &TestWizard::detailedFilepathsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_births_page,
    &TestWizard::detailedBirthsPageSetup,
    &TestWizard::detailedBirthsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_deaths_page,
    &TestWizard::detailedDeathsPageSetup,
    &TestWizard::detailedDeathsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_divrestrictions_page,
    &TestWizard::detailedDivrestrictionsPageSetup,
    &TestWizard::detailedDivrestrictionsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_exemutations_page,
    &TestWizard::detailedExemutationsPageSetup,
    &TestWizard::detailedExemutationsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_divmutations_page,
    &TestWizard::detailedDivmutationsPageSetup,
    &TestWizard::detailedDivmutationsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_reversions_page,
    &TestWizard::detailedReversionsPageSetup,
    &TestWizard::detailedReversionsPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_timeslicing_page,
    &TestWizard::detailedTimeslicingPageSetup,
    &TestWizard::detailedTimeslicingPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_meritsize_page,
    &TestWizard::detailedMeritsizePageSetup,
    &TestWizard::detailedMeritsizePageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_merittasks_page,
    &TestWizard::detailedMerittasksPageSetup,
    &TestWizard::detailedMerittasksPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_genotypeinfo_page,
    &TestWizard::detailedGenotypeinfoPageSetup,
    &TestWizard::detailedGenotypeinfoPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_speciesinfo_page,
    &TestWizard::detailedSpeciesinfoPageSetup,
    &TestWizard::detailedSpeciesinfoPageValidator
  );
  insertSetupAndValidatorMappings(
    m_detailed_logfiles_page,
    &TestWizard::detailedLogfilesPageSetup,
    &TestWizard::detailedLogfilesPageValidator
  );
  insertSetupAndValidatorMappings(
    m_new_savegenesis_page,
    &TestWizard::newSavegenesisPageSetup,
    &TestWizard::newSavegenesisPageValidator
  );
  insertSetupAndValidatorMappings(
    m_modified_savegenesis_page,
    &TestWizard::modifiedSavegenesisPageSetup,
    &TestWizard::modifiedSavegenesisPageValidator
  );

  // create genesis wrappers
  m_existing_genwrap = new GenesisWrapper(
    this,
    "m_existing_genwrap"
  );
  m_new_simple_genwrap = new GenesisWrapper(
    this,
    "m_new_simple_genwrap"
  );
  m_modified_simple_genwrap = new GenesisWrapper(
    this,
    "m_modified_simple_genwrap"
  );
  m_new_detailed_genwrap = new GenesisWrapper(
    this,
    "m_new_detailed_genwrap"
  );
  m_modified_detailed_genwrap = new GenesisWrapper(
    this,
    "m_modified_detailed_genwrap"
  );
  m_current_genwrap = m_existing_genwrap;

  setNextEnabled(m_existing_genesis_select_page, false);
  setNextEnabled(m_new_savegenesis_page, false);
  setNextEnabled(m_modified_savegenesis_page, false);

  setFinishEnabled(m_existing_genesis_select_page, true);
  setFinishEnabled(m_new_savegenesis_page, true);
  setFinishEnabled(m_modified_savegenesis_page, true);

  e_genesis_type = ExistingGenesis;
  e_setup_style = SimpleStyle;
  e_simple_topology = TwoDimensional;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateGenwrap(
    e_genesis_type,
    e_setup_style
  );
  loadGenwrapData(m_current_genwrap);

  connect(
    this, SIGNAL(selected (const QString &)),
    this, SLOT(load(void))
  );
  connect(
    this, SIGNAL(helpClicked(void)),
    this, SLOT(helpButtonClicked(void))
  );
}


void
TestWizard::appropriatePages(
  eGenesisType genesis_type,
  eSetupStyle setup_style,
  eSimpleTopology simple_topology
)
{
  GenDebug("entered.");

  setAppropriate(
    m_existing_genesis_select_page,
    (genesis_type == ExistingGenesis)
  );
  setAppropriate(
    m_modify_genesis_select_page,
    (genesis_type == ModifiedGenesis)
  );
  setAppropriate(
    m_setup_style_page,
    (genesis_type == NewGenesis)
    ||
    (genesis_type == ModifiedGenesis)
  );
  setAppropriate(
    m_simple_topology_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == SimpleStyle)
  );
  setAppropriate(
    m_simple_2d_size_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == SimpleStyle)
    &&
    (simple_topology == TwoDimensional)
  );
  setAppropriate(
    m_simple_wellmixed_size_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == SimpleStyle)
    &&
    (simple_topology == WellStirredFluid)
  );
  setAppropriate(
    m_simple_populate_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == SimpleStyle)
  );
  setAppropriate(
    m_simple_endcondition_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == SimpleStyle)
  );
  setAppropriate(
    m_simple_environment_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == SimpleStyle)
  );
  setAppropriate(
    m_detailed_endcondition_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_worldsize_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_cputhreads_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_randomseed_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_filepaths_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_births_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_deaths_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_divrestrictions_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_exemutations_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_divmutations_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_reversions_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_timeslicing_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_meritsize_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_merittasks_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_genotypeinfo_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_speciesinfo_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_detailed_logfiles_page,
    ( (genesis_type == NewGenesis)
      ||
      (genesis_type == ModifiedGenesis)
    )
    &&
    (setup_style == AdvancedStyle)
  );
  setAppropriate(
    m_new_savegenesis_page,
    (genesis_type == NewGenesis)
  );
  setAppropriate(
    m_modified_savegenesis_page,
    (genesis_type == ModifiedGenesis)
  );
}


void
TestWizard::appropriateButtonStates(
  eGenesisType genesis_type,
  eSetupStyle setup_style,
  eSimpleTopology simple_topology
){
  switch(genesis_type){
  case ExistingGenesis: m_existing_world_rb->setChecked(true); break;
  case NewGenesis: m_new_world_rb->setChecked(true); break;
  case ModifiedGenesis: m_modify_world_rb->setChecked(true); break;
  default:
    qFatal("<TestWizard::appropriateButtonStates> bad genesis_type");
  }
  switch(setup_style){
  case SimpleStyle: m_simple_setup_rb->setChecked(true); break;
  case AdvancedStyle: m_advanced_setup_rb->setChecked(true); break;
  default:
    qFatal("<TestWizard::appropriateButtonStates> bad setup_style");
  }
  switch(simple_topology){
  case TwoDimensional: m_simple_twodimensional_rb->setChecked(true); break;
  case WellStirredFluid: m_simple_wellmixed_rb->setChecked(true); break;
  default:
    qFatal("<TestWizard::appropriateButtonStates> bad simple_topology");
  }
}

void
TestWizard::appropriateGenwrap(
  eGenesisType genesis_type,
  eSetupStyle setup_style
){
  switch(genesis_type){
  case ExistingGenesis:
    m_current_genwrap = m_existing_genwrap;
    break;
  case NewGenesis:
    switch(setup_style){
    case SimpleStyle:
      m_current_genwrap = m_new_simple_genwrap;
      break;
    case AdvancedStyle:
      m_current_genwrap = m_new_detailed_genwrap;
      break;
    default:
      qFatal("<TestWizard::appropriateGenwrap> bad setup_style");
      break; // redundant
    }
    break; // redundant
  case ModifiedGenesis:
    switch(setup_style){
    case SimpleStyle:
      m_current_genwrap = m_modified_simple_genwrap;
      break;
    case AdvancedStyle:
      m_current_genwrap = m_modified_detailed_genwrap;
      break;
    default:
      qFatal("<TestWizard::appropriateGenwrap> bad setup_style");
      break; // redundant
    }
    break; // redundant
  default:
    qFatal("<TestWizard::appropriateGenwrap> bad genesis_type");
  }
}


void
TestWizard::insertSetupAndValidatorMappings(
  QWidget *page,
  PageFunction setup_function,
  PageFunction validator_function
)
{
  m_smap[page] = PageFunctWrap(setup_function);
  m_vmap[page] = PageFunctWrap(validator_function);
}


void
TestWizard::load()
{
  GenDebug("entered.");

  QString error_check = (this->*m_smap[currentPage()]())(currentPage());
  GenDebug(error_check);
  //if(error_check == QString::null) QWizard::next();
}


void
TestWizard::next()
{
  GenDebug("entered.");

  QString error_check = (this->*m_vmap[currentPage()]())(currentPage());
  GenDebug(error_check);
  if(error_check == QString::null){
    QWizard::next();
  } else {
    QMessageBox::information(
      0,
      "Problems occurred:",
      error_check
    );
  }
}


void
TestWizard::accept()
{
  GenDebug("entered.");

  QString error_check = (this->*m_vmap[currentPage()]())(currentPage());
  GenDebug(error_check);
  if(error_check == QString::null){
    QWizard::accept();
  } else {
    QMessageBox::information(
      0,
      "Problems occurred:",
      error_check
    );
  }
}

void
TestWizard::reject()
{
  GenDebug("entered.");

  exit(0);
}


// selection of genesis type...
void
TestWizard::useExistingWorldSlot()
{
  GenDebug("entered.");
  e_genesis_type = ExistingGenesis;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateGenwrap(
    e_genesis_type,
    e_setup_style
  );
  loadGenwrapData(m_current_genwrap);
}

void
TestWizard::createNewWorldSlot()
{
  GenDebug("entered.");
  e_genesis_type = NewGenesis;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateGenwrap(
    e_genesis_type,
    e_setup_style
  );
  loadGenwrapData(m_current_genwrap);
}

void
TestWizard::modifyExistingWorldSlot()
{
  GenDebug("entered.");
  e_genesis_type = ModifiedGenesis;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateGenwrap(
    e_genesis_type,
    e_setup_style
  );
  loadGenwrapData(m_current_genwrap);
}


// selection of setup style...
void
TestWizard::useSimpleSetupStyleSlot()
{
  GenDebug("entered.");
  e_setup_style = SimpleStyle;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateGenwrap(
    e_genesis_type,
    e_setup_style
  );
  loadGenwrapData(m_current_genwrap);
}

void
TestWizard::useAdvancedSetupStyleSlot()
{
  GenDebug("entered.");
  e_setup_style = AdvancedStyle;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateGenwrap(
    e_genesis_type,
    e_setup_style
  );
  loadGenwrapData(m_current_genwrap);
}


// simple style topology selection...
void
TestWizard::simpleTwoDimensionalSlot()
{
  GenDebug("entered.");
  e_simple_topology = TwoDimensional;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  m_current_genwrap->set(
    "BIRTH_METHOD",
    GenesisWrapper::ReplaceLargestAgeOverMeritInNeighborhood
  );
}

void
TestWizard::simpleWellMixedFluidSlot()
{
  GenDebug("entered.");
  e_simple_topology = WellStirredFluid;
  appropriatePages(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  appropriateButtonStates(
    e_genesis_type,
    e_setup_style,
    e_simple_topology
  );
  m_current_genwrap->set(
    "BIRTH_METHOD",
    GenesisWrapper::ReplaceRandomInPopulation
  );
}

void
TestWizard::simpleStartcreatureSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_simple_startcreature_lb->text();
  if(filename.isEmpty())
  {
    //filename = QDir::currentDirPath() + "/genesis";
    filename = m_current_genwrap->qStringValue("START_CREATURE", ok);
  }
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Locate organism file:"
  );
  QFileInfo fileinfo(filename);
  if(!filename.isEmpty()){
    if(fileinfo.exists()){
      if(fileinfo.isReadable()){
        m_simple_startcreature_lb->setText(fileinfo.absFilePath());
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Organism file",
        "Can't open organism file:\n"
        +
        error
      );
  }
}

void
TestWizard::simpleEnvfileSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_simple_envfile_lb->text();
  if(filename.isEmpty())
  {
    //filename = QDir::currentDirPath() + "/genesis";
    filename = m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok);
  }
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Locate organism file:"
  );
  QFileInfo fileinfo(filename);
  if(!filename.isEmpty()){
    if(fileinfo.exists()){
      if(fileinfo.isReadable()){
        m_simple_envfile_lb->setText(fileinfo.absFilePath());
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Environment file",
        "Can't open environment file:\n"
        +
        error
      );
  }
}

void
TestWizard::existingWorldSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_existing_world_lb->text();
  if(filename.isEmpty())
  {
    //filename = QDir::currentDirPath() + "/genesis";
    filename = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
  }
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Locate existing world file:"
  );
  QFileInfo genesis_fileinfo(filename);
  if(!filename.isEmpty()){
    if(genesis_fileinfo.exists()){
      if(genesis_fileinfo.isReadable()){
        m_existing_world_lb->setText(genesis_fileinfo.absFilePath());
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Existing world file",
        "Can't open existing world file:\n"
        +
        error
      );
  }
}

void
TestWizard::existingWorkdirSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;

  QString dirname = m_existing_workdir_lb->text();
  if(dirname.isEmpty()) dirname = QDir::currentDirPath();
  GenDebug(" --- dirname:  ")(dirname);
  dirname = QFileDialog::getExistingDirectory(
    dirname,
    this,
    0,
    "Choose work directory:",
    false,
    false
  );
  QFileInfo working_dirinfo(dirname);
  if(!dirname.isEmpty()){
    if(working_dirinfo.exists()){
      if(working_dirinfo.isDir()){
        if(working_dirinfo.isReadable()){
          if(working_dirinfo.isWritable()){
            m_existing_workdir_lb->setText(working_dirinfo.absFilePath());
          } else error = "directory not writable";
        } else error = "directory not readable";
      } else error = "not a directory";
    } else error = "directory does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Chosen work directory",
        "Can't switch to directory:\n"
        +
        error
      );
  }
}

void
TestWizard::modifyWorldSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;

  QString filename = m_world_to_modify_lb->text();
  if(filename.isEmpty()) filename = QDir::currentDirPath() + "/genesis";
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Choose world file to modify:"
  );
  QFileInfo genesis_fileinfo(filename);
  if(!filename.isEmpty()){
    if(genesis_fileinfo.exists()){
      if(genesis_fileinfo.isReadable()){
        m_world_to_modify_lb->setText(genesis_fileinfo.absFilePath());
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Existing world file",
        "Can't open existing world file file:\n"
        +
        error
      );
  }
}

void
TestWizard::detailedEnvfileSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_detailed_envfile_lb->text();
  if(filename.isEmpty())
  {
    filename = m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok);
  }
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Locate environment file:"
  );
  QFileInfo fileinfo(filename);
  if(!filename.isEmpty()){
    if(fileinfo.exists()){
      if(fileinfo.isReadable()){
        m_detailed_envfile_lb->setText(fileinfo.absFilePath());
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Environment file",
        "Can't open environment file:\n"
        +
        error
      );
  }
}

void
TestWizard::detailedEventsfileSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_detailed_eventfile_lb->text();
  if(filename.isEmpty())
  {
    filename = m_current_genwrap->qStringValue("EVENT_FILE", ok);
  }
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Locate events file:"
  );
  QFileInfo fileinfo(filename);
  if(!filename.isEmpty()){
    if(fileinfo.exists()){
      if(fileinfo.isReadable()){
        m_detailed_eventfile_lb->setText(fileinfo.absFilePath());
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Events file",
        "Can't open events file:\n"
        +
        error
      );
  }
}

void
TestWizard::detailedInstfileSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_detailed_instfile_lb->text();
  if(filename.isEmpty())
  {
    filename = m_current_genwrap->qStringValue("INST_SET", ok);
  }
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Locate instructions file:"
  );
  QFileInfo fileinfo(filename);
  if(!filename.isEmpty()){
    if(fileinfo.exists()){
      if(fileinfo.isReadable()){
        m_detailed_instfile_lb->setText(fileinfo.absFilePath());
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Instructions file",
        "Can't open instructions file:\n"
        +
        error
      );
  }
}

void
TestWizard::detailedStartcreatureSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_detailed_startcreaturefile_lb->text();
  if(filename.isEmpty())
  {
    filename = m_current_genwrap->qStringValue("START_CREATURE", ok);
  }
  filename = QFileDialog::getOpenFileName(
    filename,
    QString::null,
    this,
    0,
    "Locate organism file:"
  );
  QFileInfo fileinfo(filename);
  if(!filename.isEmpty()){
    if(fileinfo.exists()){
      if(fileinfo.isReadable()){
        m_detailed_startcreaturefile_lb->setText(
          fileinfo.absFilePath()
        );
      } else error = "file not readable";
    } else error = "file does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Organism file",
        "Can't open organism file:\n"
        +
        error
      );
  }
}

void
TestWizard::modifiedWorldSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString cur_filename = m_modified_world_lb->text();
  if(cur_filename.isEmpty())
  {
    //cur_filename = QDir::currentDirPath() + "/genesis";
    cur_filename = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
  }
  // cur_filename = QFileDialog::getOpenFileName(
  //   cur_filename,
  //   QString::null,
  //   this,
  //   0,
  //   "Locate existing world file:"
  // );
  QString new_filename = QFileDialog::getSaveFileName(
    cur_filename,
    QString::null,
    this,
    0,
    "Save modified world file as:"
  );
  /* early exit! */
  if(new_filename == cur_filename){
    GenDebug("done (early exit because new filename == cur filename).");
    return;
  }

  GenDebug("using new filename.");

  QFileInfo genesis_fileinfo(new_filename);
  if(!new_filename.isEmpty()){
    GenDebug("!new_filename.isEmpty().");
    if(genesis_fileinfo.exists()){
      GenDebug("genesis_fileinfo.exists().");
      if(genesis_fileinfo.isReadable()){
        GenDebug("genesis_fileinfo.isReadable().");
        if(genesis_fileinfo.isWritable()){
          GenDebug("genesis_fileinfo.isWritable().  setting filename.");
          m_modified_world_lb->setText(genesis_fileinfo.absFilePath());
        } else error = "file not writable";
      } else error = "file not readable";
      if(error != QString::null)
        QMessageBox::information(
          0,
          "Existing world file",
          "Can't open existing world file:\n"
          +
          error
        );
    // } else error = "file does not exists";
    } else {
      /*
      FIXME:  need to verify that file can be written.
      */
      // if(error != QString::null)
      //   QMessageBox::information(
      //     0,
      //     "Existing world file",
      //     "Can't open existing world file:\n"
      //     +
      //     error
      //   );
      GenDebug("apparently we're trying to create a new file.  setting filename.");
      m_modified_world_lb->setText(genesis_fileinfo.absFilePath());
    }
  }
  GenDebug("done.");
}

void
TestWizard::modifiedWorkdirSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;

  QString dirname = m_modified_workdir_lb->text();
  if(dirname.isEmpty()) dirname = QDir::currentDirPath();
  GenDebug(" --- dirname:  ")(dirname);
  dirname = QFileDialog::getExistingDirectory(
    dirname,
    this,
    0,
    "Choose work directory:",
    false,
    false
  );
  QFileInfo working_dirinfo(dirname);
  if(!dirname.isEmpty()){
    if(working_dirinfo.exists()){
      if(working_dirinfo.isDir()){
        if(working_dirinfo.isReadable()){
          if(working_dirinfo.isWritable()){
            m_modified_workdir_lb->setText(working_dirinfo.absFilePath());
          } else error = "directory not writable";
        } else error = "directory not readable";
      } else error = "not a directory";
    } else error = "directory does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Chosen work directory",
        "Can't switch to directory:\n"
        +
        error
      );
  }
}
void
TestWizard::newWorldSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;
  bool ok;

  QString filename = m_new_world_lb->text();
  if(filename.isEmpty())
  {
    //filename = QDir::currentDirPath() + "/genesis";
    filename = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
  }
  filename = QFileDialog::getSaveFileName(
    filename,
    QString::null,
    this,
    0,
    "Select new world file:"
  );
  QFileInfo genesis_fileinfo(filename);
  if(!filename.isEmpty()){
    /*
    FIXME:  need to verify that file can be written.
    */
    //if(genesis_fileinfo.exists()){
      //if(genesis_fileinfo.isReadable()){
      //} else error = "file not readable";
    //} else error = "file does not exists";
    //if(error != QString::null)
    //  QMessageBox::information(
    //    0,
    //    "Existing world file",
    //    "Can't open existing world file:\n"
    //    +
    //    error
    //  );
    m_new_world_lb->setText(genesis_fileinfo.absFilePath());
  }
}

void
TestWizard::newWorkdirSelectSlot()
{
  GenDebug("entered.");

  QString error = QString::null;

  QString dirname = m_new_workdir_lb->text();
  if(dirname.isEmpty()) dirname = QDir::currentDirPath();
  GenDebug(" --- dirname:  ")(dirname);
  dirname = QFileDialog::getExistingDirectory(
    dirname,
    this,
    0,
    "Choose work directory:",
    false,
    false
  );
  QFileInfo working_dirinfo(dirname);
  if(!dirname.isEmpty()){
    if(working_dirinfo.exists()){
      if(working_dirinfo.isDir()){
        if(working_dirinfo.isReadable()){
          if(working_dirinfo.isWritable()){
            m_new_workdir_lb->setText(working_dirinfo.absFilePath());
          } else error = "directory not writable";
        } else error = "directory not readable";
      } else error = "not a directory";
    } else error = "directory does not exists";
    if(error != QString::null)
      QMessageBox::information(
        0,
        "Chosen work directory",
        "Can't switch to directory:\n"
        +
        error
      );
  }
}


void
TestWizard::helpButtonClicked(void)
{
  GenDebug << "entered.";

  /*
  FIXME:  replace with real help documentation.  -- kgn
  */
  QMessageBox::information(
    this,
    "Setup Assistant Help",
    "The beta version of Avida "
    "lacks documentation for this "
    "part of the user interface. "
    "We're working on it."
  );

  GenDebug("done.");
}



