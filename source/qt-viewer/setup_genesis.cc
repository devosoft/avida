//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#include <qdir.h>
#include <qfile.h>

#ifndef GENESIS_HH
#include "genesis.hh"
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

#ifndef SETUP_WIZARD_CONSTANTS_HH
#include "setup_wizard_constants.hh"
#endif

#include "file.hh"


using namespace std;


using namespace std;


cSetupDescription *
GenesisWrapper::SetupDescriptionWrapper::s_bogus_setup_description = 0;

GenesisWrapper::GenesisWrapper(
  QObject *parent,
  const char *name
)
: QObject(parent, name),
  m_genesis(0)
{
  GenDebug("entered.");

  m_genesis_path = new cSetupQString(
    "GENESIS_PATH",
    "# path to genesis file\n",
    "genesis",
    this,
    "m_genesis_path"
  );

  m_version_id = new cSetupQString(
    "VERSION_ID",
    "# Do not change version id!\n",
    "1.6",
    this,
    "genesis VERSION_ID"
  );

  // Architecture variables.
  m_max_updates = new cSetupIntRange(
    "MAX_UPDATES",
    "# Maximum updates to run simulation (-1 = no limit)\n",
    -1,
    INT_MAX,
    -1,
    false,
    this,
    "m_max_updates"
  );

  m_max_generations = new cSetupIntRange(
    "MAX_GENERATIONS",
    "# Maximum generations to run simulation (-1 = no limit)\n",
    -1,
    INT_MAX,
    -1,
    false,
    this,
    "m_max_generations"
  );

  m_end_condition = new cSetupIntChoice(
    "END_CONDITION_MODE",
    "# End run when ...\n",
    MaxUpdatesORMaxGenerations,
    true,
    this,
    "m_end_condition"
  );
  m_end_condition->insertMapping(
    MaxUpdatesORMaxGenerations,
    "MAX_UPDATES _OR_ MAX_GENERATIONS is reached"
  );
  m_end_condition->insertMapping(
    MaxUpdatesANDMaxGenerations,
    "MAX_UPDATES _AND_ MAX_GENERATIONS is reached"
  );

  m_world_x = new cSetupIntRange(
    "WORLD-X",
    "# width of the world in Avida mode\n",
    1,
    INT_MAX,
    60,
    false,
    this,
    "m_world_x"
  );

  m_world_y = new cSetupIntRange(
    "WORLD-Y",
    "# height of the world in Avida mode\n",
    1,
    INT_MAX,
    60,
    false,
    this,
    "m_world_y"
  );

  m_max_cpu_threads = new cSetupIntRange(
    "MAX_CPU_THREADS",
    "# Number of Threads CPUs can spawn\n",
    1,
    INT_MAX,
    1,
    false,
    this,
    "m_max_cpu_threads"
  );

  m_random_seed = new cSetupIntRange(
    "RANDOM_SEED",
    "# Random number seed. (0 for based on time)\n",
    0,
    INT_MAX,
    0,
    false,
    this,
    "m_random_seed"
  );


  // Configuration files.
  m_work_directory_path = new cSetupQString(
    "DEFAULT_DIR",
    "# Directory in which config files are found\n",
#ifdef PLATFORM_IS_MAC_OS_X
    QDir::currentDirPath().latin1(),
#else
    "../work/",
#endif
    this,
    "m_work_directory_path"
  );

  m_inst_set_path = new cSetupQString(
    "INST_SET",
    "# File containing instruction set\n",
    "inst_set.default",
    this,
    "m_inst_set_path"
  );

  m_event_file_path = new cSetupQString(
    "EVENT_FILE",
    "# File containing list of events during run\n",
    "events.cfg",
    this,
    "m_event_file_path"
  );

  m_analyze_file_path = new cSetupQString(
    "ANALYZE_FILE",
    " File used for analysis mode\n",
    "analyze.cfg",
    this,
    "m_analyze_file_path"
  );

  m_environment_file_path = new cSetupQString(
    "ENVIRONMENT_FILE",
    "# File that describes the environment\n",
    "environment.cfg",
    this,
    "m_environment_file_path"
  );

  m_start_creature_path = new cSetupQString(
    "START_CREATURE",
    "# Organism to seed the soup\n",
    "organism.default",
    this,
    "m_start_creature_path"
  );


  // Reproduction.
  m_birth_method = new cSetupIntChoice(
    "BIRTH_METHOD",
    "# Placement of offspring (determines topology)\n",
    ReplaceLargestAgeOverMeritInNeighborhood,
    true,
    this,
    "m_birth_method"
  );
  m_birth_method->insertMapping(
    ReplaceRandomInNeighborhood,
    "Replace random organism in neighborhood"
  );
  m_birth_method->insertMapping(
    ReplaceOldestInNeighborhood,
    "Replace oldest organism in neighborhood"
  );
  m_birth_method->insertMapping(
    ReplaceLargestAgeOverMeritInNeighborhood,
    "Replace larges Age/Merit in neighborhood"
  );
  m_birth_method->insertMapping(
    PlaceInEmptyCellsInNeighborhood,
    "Place only in empty cells in neighborhood"
  );
  m_birth_method->insertMapping(
    ReplaceRandomInPopulation,
    "Replace random from entire population (Mass Action)"
  );
  m_birth_method->insertMapping(
    ReplaceOldestInPopulation,
    "Replace oldest in entire population (like Tierra)"
  );

  m_death_method = new cSetupIntChoice(
    "DEATH_METHOD",
    "# Death method\n",
    NeverDie,
    true,
    this,
    "m_death_method"
  );
  m_death_method->insertMapping(
    NeverDie,
    "Never die of old age"
  );
  m_death_method->insertMapping(
    InstrExecReachesAgeLimit,
    "Die when inst executed = AGE_LIMIT (with deviation)"
  );
  m_death_method->insertMapping(
    InstrExecReachesAgeLimitTimesLength,
    "Die when inst executed = length * AGE_LIMIT (+ dev.)"
  );

  m_age_limit = new cSetupIntRange(
    "AGE_LIMIT",
    "# Modifies DEATH_METHOD\n",
    1,
    INT_MAX,
    5000,
    false,
    this,
    "m_age_limit"
  );

  m_age_deviation = new cSetupDoubleRange(
    "AGE_DEVIATION",
    "# Modifies DEATH_METHOD\n",
    0.0,
    DBL_MAX,
    0.0,
    false,
    this,
    "m_age_deviation"
  );

  m_alloc_method = new cSetupIntChoice(
    "ALLOC_METHOD",
    "# Instructions to place in newly allocated memory\n",
    DefaultInst,
    true,
    this,
    "m_alloc_method"
  );
  m_alloc_method->insertMapping(
    DefaultInst,
    "Allocated space is set to default instruction"
  );
  m_alloc_method->insertMapping(
    Necrophilia,
    "Set to section of dead genome (Necrophilia)"
  );
  m_alloc_method->insertMapping(
    Random,
    "Allocated space is set to random instruction"
  );

  m_divide_method = new cSetupIntChoice(
    "DIVIDE_METHOD",
    "# States of child and mother after divide\n",
    BothChildren,
    true,
    this,
    "m_divide_method"
  );
  m_divide_method->insertMapping(
    MotherAndChild,
    "Divide leaves state of mother untouched"
  );
  m_divide_method->insertMapping(
    BothChildren,
    "Divide resets state of mother (gives two children)"
  );

  m_generation_inc_method = new cSetupIntChoice(
    "GENERATION_INC_METHOD",
    "# Method to increment generation after divide\n",
    IncrMotherAndChild,
    true,
    this,
    "m_generation_inc_method"
  );
  m_generation_inc_method->insertMapping(
    IncrChildOnly,
    "Only increase child generation"
  );
  m_generation_inc_method->insertMapping(
    IncrMotherAndChild,
    "Increase mother and child generations (good with DIVIDE_METHOD 1)"
  );


  // Divide restrictions
  m_child_size_range = new cSetupDoubleRange(
    "CHILD_SIZE_RANGE",
    "# Maximal differential between child and parent sizes\n",
    0.0,
    DBL_MAX,
    2.0,
    false,
    this,
    "m_child_size_range"
  );

  m_min_copied_lines = new cSetupDoubleRange(
    "MIN_COPIED_LINES",
    "# Code fraction which must be copied before divide\n",
    0.0,
    DBL_MAX,
    0.5,
    false,
    this,
    "m_min_copied_lines"
  );

  m_min_exe_lines = new cSetupDoubleRange(
    "MIN_EXE_LINES",
    "# Code fraction which must be executed before divide\n",
    0.0,
    DBL_MAX,
    0.5,
    false,
    this,
    "m_min_exe_lines"
  );

  m_require_allocate = new cSetupIntChoice(
    "REQUIRE_ALLOCATE",
    "# Is a an allocate required before a divide?\n",
    On,
    true,
    this,
    "m_require_allocate"
  );
  m_require_allocate->insertMapping(
    Off,
    "No"
  );
  m_require_allocate->insertMapping(
    On,
    "Yes"
  );

  m_required_task = new cSetupIntRange(
    "REQUIRED_TASK",
    "# Number (id?) of a task required for successful divide\n"
    "# FIXME:\n"
    "#  Get clarification, or search source,\n"
    "#  to figure out correct description.\n"
    "# -- kaben.\n",
    -1,
    INT_MAX,
    -1,
    false,
    this,
    "m_required_task"
  );


  // Mutations
    // mutations that occur during execution
  m_point_mut_prob = new cSetupDoubleRange(
    "POINT_MUT_PROB",
    "# Mutation rate (per-location per update)\n",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_point_mut_prob"
  );

  m_copy_mut_prob = new cSetupDoubleRange(
    "COPY_MUT_PROB",
    "# Mutation rate (per copy)\n",
    0.0,
    1.0,
    0.0075,
    false,
    this,
    "m_copy_mut_prob"
  );

    // mutations that occur on divide
  m_ins_mut_prob = new cSetupDoubleRange(
    "INS_MUT_PROB",
    "# Insertion rate (per site, applied on divide)\n",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_ins_mut_prob"
  );

  m_del_mut_prob = new cSetupDoubleRange(
    "DEL_MUT_PROB",
    "# Deletion rate (per site, applied on divide)\n",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_del_mut_prob"
  );

  m_divide_mut_prob = new cSetupDoubleRange(
    "DIVIDE_MUT_PROB",
    "# Mutation rate (per divide)\n",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_divide_mut_prob"
  );

  m_divide_ins_prob = new cSetupDoubleRange(
    "DIVIDE_INS_PROB",
    "# Insertion rate (per divide)\n",
    0.0,
    1.0,
    0.05,
    false,
    this,
    "m_divide_ins_prob"
  );

  m_divide_del_prob = new cSetupDoubleRange(
    "DIVIDE_DEL_PROB",
    "# Deletion rate (per divide)\n",
    0.0,
    1.0,
    0.05,
    false,
    this,
    "m_divide_del_prob"
  );

  m_parent_mut_prob = new cSetupDoubleRange(
    "PARENT_MUT_PROB",
    "# Per-site, in parent, on divide\n",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_parent_mut_prob"
  );

  m_crossover_prob = new cSetupDoubleRange(
    "CROSSOVER_PROB",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_crossover_prob"
  );

  m_aligned_cross_prob = new cSetupDoubleRange(
    "ALIGNED_CROSS_PROB",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_aligned_cross_prob"
  );

  m_exe_err_prob = new cSetupDoubleRange(
    "EXE_ERROR_PROB",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_exe_err_prob"
  );


  // Mutation reversions
  m_revert_fatal = new cSetupDoubleRange(
    "REVERT_FATAL",
    "# Should any mutations be reverted on birth?\n",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_revert_fatal"
  );

  m_revert_detrimental = new cSetupDoubleRange(
    "REVERT_DETRIMENTAL",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_revert_detrimental"
  );

  m_revert_neutral = new cSetupDoubleRange(
    "REVERT_NEUTRAL",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_revert_neutral"
  );

  m_revert_beneficial = new cSetupDoubleRange(
    "REVERT_BENEFICIAL",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_revert_beneficial"
  );

  m_sterilize_fatal = new cSetupDoubleRange(
    "STERILIZE_FATAL",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_sterilize_fatal"
  );
  
  m_sterilize_detrimental = new cSetupDoubleRange(
    "STERILIZE_DETRIMENTAL",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_sterilize_detrimental"
  );

  m_sterilize_neutral = new cSetupDoubleRange(
    "STERILIZE_NEUTRAL",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_sterilize_neutral"
  );

  m_sterilize_beneficial = new cSetupDoubleRange(
    "STERILIZE_BENEFICIAL",
    "",
    0.0,
    1.0,
    0.0,
    false,
    this,
    "m_sterilize_beneficial"
  );

  m_fail_implicit = new cSetupIntChoice(
    "FAIL_IMPLICIT",
    "# Should copies that failed *not* due to mutations be eliminated?\n",
    Off,
    true,
    this,
    "m_fail_implicit"
  );
  m_fail_implicit->insertMapping(
    Off,
    "No"
  );
  m_fail_implicit->insertMapping(
    On,
    "Yes"
  );


  // Time slicing
  m_ave_time_slice = new cSetupIntRange(
    "AVE_TIME_SLICE",
    "",
    1,
    INT_MAX,
    30,
    false,
    this,
    "m_ave_time_slice"
  );

  m_slicing_method = new cSetupIntChoice(
    "SLICING_METHOD",
    "",
    Integrated,
    true,
    this,
    "m_slicing_method"
  );
  m_slicing_method->insertMapping(
    Constant,
    "CONSTANT: all organisms get default..."
  );
  m_slicing_method->insertMapping(
    Probabilistic,
    "PROBABILISTIC: Run _prob_ proportional to merit"
  );
  m_slicing_method->insertMapping(
    Integrated,
    "INTEGRATED: Perfectly integrated deterministic"
  );

  m_size_merit_method = new cSetupIntChoice(
    "SIZE_MERIT_METHOD",
    "",
    MinExecAndCopied,
    true,
    this,
    "m_size_merit_method"
  );
  m_size_merit_method->insertMapping(
    MeritOff,
    "off (merit is independent of size)"
  );
  m_size_merit_method->insertMapping(
    CopiedSize,
    "Merit proportional to copied size"
  );
  m_size_merit_method->insertMapping(
    ExecutedSize,
    "Merit prop. to executed size"
  );
  m_size_merit_method->insertMapping(
    FullSize,
    "Merit prop. to full size"
  );
  m_size_merit_method->insertMapping(
    MinExecAndCopied,
    "Merit prop. to min of executed or copied size"
  );
  m_size_merit_method->insertMapping(
    SqrtMinSize,
    "Merit prop. to sqrt of the minimum size"
  );

  m_base_size_merit = new cSetupIntRange(
    "BASE_SIZE_MERIT",
    "# Base merit\n",
    0,
    INT_MAX,
    0,
    false,
    this,
    "m_base_size_merit"
  );

  m_task_merit_method = new cSetupIntChoice(
    "TASK_MERIT_METHOD",
    "",
    On,
    true,
    this,
    "m_task_merit_method"
  );
  m_task_merit_method->insertMapping(
    Off,
    "No task bonuses"
  );
  m_task_merit_method->insertMapping(
    On,
    "Bonus just equals the task bonus"
  );

  m_max_label_exe_size = new cSetupIntRange(
    "MAX_LABEL_EXE_SIZE",
    "# Max nops marked as executed when labels are used\n",
    0,
    INT_MAX,
    1,
    false,
    this,
    "m_max_label_exe_size"
  );

  m_merit_time = new cSetupIntChoice(
    "MERIT_TIME",
    "",
    PostDivision,
    true,
    this,
    "m_merit_time"
  );
  m_merit_time->insertMapping(
    TaskCompletion,
    "Merit calculated when task completed"
  );
  m_merit_time->insertMapping(
    PostDivision,
    "Merit calculated on divide"
  );

  m_max_num_tasks_rewarded = new cSetupIntRange(
    "MAX_NUM_TASKS_REWARDED",
    "# -1 = Unlimited\n",
    -1,
    INT_MAX,
    -1,
    false,
    this,
    "m_max_num_tasks_rewarded"
  );

  // Genotype info
  m_threshold = new cSetupIntRange(
    "THRESHOLD",
    "# Number of organisms in a genotype needed for it\n"
    "#   to be considered viable\n",
    1,
    INT_MAX,
    3,
    false,
    this,
    "m_threshold"
  );

  m_genotype_print = new cSetupIntChoice(
    "GENOTYPE_PRINT",
    "# Print out all threshold genotypes?\n",
    Off,
    true,
    this,
    "m_genotype_print"
  );
  m_genotype_print->insertMapping(
    Off,
    "Off"
  );
  m_genotype_print->insertMapping(
    On,
    "One"
  );

  m_genotype_print_dom = new cSetupIntRange(
    "GENOTYPE_PRINT_DOM",
    "# Print out a genotype if it stays dominant\n"
    "# for this many updates (0 = off)\n",
    0,
    INT_MAX,
    0,
    false,
    this,
    "m_genotype_print_dom"
  );

  m_species_threshold = new cSetupIntRange(
    "SPECIES_THRESHOLD",
    "# max failure count for organisms to be same species\n",
    0,
    INT_MAX,
    2,
    false,
    this,
    "m_species_threshold"
  );

  m_species_recording = new cSetupIntChoice(
    "SPECIES_RECORDING",
    "",
    SpeciesRecordingOff,
    true,
    this,
    "m_species_recording"
  );
  m_species_recording->insertMapping(
    SpeciesRecordingOff,
    "Off"
  );
  m_species_recording->insertMapping(
    FullSpeciesRecording,
    "Full"
  );
  m_species_recording->insertMapping(
    LimitSpeciesSearch,
    "Limited search (parent only)"
  );

  m_species_print = new cSetupIntChoice(
    "SPECIES_PRINT",
    "# Print out all species?\n",
    Off,
    true,
    this,
    "m_species_print"
  );
  m_species_print->insertMapping(
    Off,
    "No"
  );
  m_species_print->insertMapping(
    On,
    "Yes"
  );

  m_test_cpu_time_mod = new cSetupIntRange(
    "TEST_CPU_TIME_MOD",
    "# Time allocated in test CPUs (multiple of length)\n",
    1,
    INT_MAX,
    20,
    false,
    this,
    "m_test_cpu_time_mod"
  );

  m_track_main_lineage = new cSetupIntChoice(
    "TRACK_MAIN_LINEAGE",
    "# Track primary lineage leading to final population?\n",
    Off,
    true,
    this,
    "m_track_main_lineage"
  );
  m_track_main_lineage->insertMapping(
    Off,
    "No"
  );
  m_track_main_lineage->insertMapping(
    On,
    "Yes"
  );


  // Log files
  m_log_breed_count = new cSetupIntChoice(
    "LOG_BREED_COUNT",
    "# toggle to print file\n",
    Off,
    true,
    this,
    "m_log_breed_count"
  );
  m_log_breed_count->insertMapping(
    Off,
    "Off"
  );
  m_log_breed_count->insertMapping(
    On,
    "On"
  );

  m_log_creatures = new cSetupIntChoice(
    "LOG_CREATURES",
    "# toggle to print file\n",
    Off,
    true,
    this,
    "m_log_creatures"
  );
  m_log_creatures->insertMapping(
    Off,
    "Off"
  );
  m_log_creatures->insertMapping(
    On,
    "On"
  );

  m_log_phylogeny = new cSetupIntChoice(
    "LOG_PHYLOGENY",
    "# toggle to print file\n",
    Off,
    true,
    this,
    "m_log_phylogeny"
  );
  m_log_phylogeny->insertMapping(
    Off,
    "Off"
  );
  m_log_phylogeny->insertMapping(
    On,
    "On"
  );

  m_log_genotypes = new cSetupIntChoice(
    "LOG_GENOTYPES",
    "",
    GenotypeLoggingOff,
    true,
    this,
    "m_log_genotypes"
  );
  m_log_genotypes->insertMapping(
    GenotypeLoggingOff,
    "Off"
  );
  m_log_genotypes->insertMapping(
    LogAllGenotypes,
    "Print ALL"
  );
  m_log_genotypes->insertMapping(
    LogThresholdGenotypes,
    "Print threshold ONLY"
  );

  m_log_threshold = new cSetupIntChoice(
    "LOG_THRESHOLD",
    "# toggle to print file\n",
    Off,
    true,
    this,
    "m_log_threshold"
  );
  m_log_threshold->insertMapping(
    Off,
    "Off"
  );
  m_log_threshold->insertMapping(
    On,
    "On"
  );

  m_log_species = new cSetupIntChoice(
    "LOG_SPECIES",
    "# toggle to print file\n",
    Off,
    true,
    this,
    "m_log_species"
  );
  m_log_species->insertMapping(
    Off,
    "Off"
  );
  m_log_species->insertMapping(
    On,
    "On"
  );

  m_log_genealogy = new cSetupIntChoice(
    "LOG_GENEOLOGY",
    "",
    GenealogyLoggingOff,
    true,
    this,
    "m_log_genealogy"
  );
  m_log_genealogy->insertMapping(
    GenealogyLoggingOff,
    "Off"
  );
  m_log_genealogy->insertMapping(
    LogAllGenealogy,
    "All"
  );
  m_log_genealogy->insertMapping(
    LogParentGenealogy,
    "Parents only"
  );

  m_log_landscape = new cSetupIntChoice(
    "LOG_LANDSCAPE",
    "# toggle to print file\n",
    Off,
    true,
    this,
    "m_log_landscape"
  );
  m_log_landscape->insertMapping(
    Off,
    "Off"
  );
  m_log_landscape->insertMapping(
    On,
    "On"
  );

  m_log_mutations = new cSetupIntChoice(
    "LOG_MUTATIONS",
    "# toggle to print file\n",
    Off,
    true,
    this,
    "m_log_mutations"
  );
  m_log_mutations->insertMapping(
    Off,
    "Off"
  );
  m_log_mutations->insertMapping(
    On,
    "On"
  );

  m_log_lineages = new cSetupIntChoice(
    "LOG_LINEAGES",
    "# toggle to log advantageous mutations.\n"
    "# This one can slow down avida a lot. It is used to get an\n"
    "# idea of how often an advantageous mutation arises, and where\n"
    "# it goes afterwards.\n"
    "# See also LINEAGE_CREATION_METHOD.\n",
    Off,
    true,
    this,
    "m_log_lineages"
  );
  m_log_lineages->insertMapping(
    Off,
    "Off"
  );
  m_log_lineages->insertMapping(
    On,
    "On"
  );

  m_lineage_creation_method = new cSetupIntChoice(
    "LINEAGE_CREATION_METHOD",
    "# Works only when LOG_LINEAGES is on.\n",
    ManualLineageCreation,
    true,
    this,
    "m_lineage_creation_method"
  );
  m_lineage_creation_method->insertMapping(
    ManualLineageCreation,
    "manual creation\n"
    "  #   (on inject, use successive integers as lineage labels)"
  );
  m_lineage_creation_method->insertMapping(
    ChildFitnessVsParent,
    "when a child's (potential) fitness is higher\n"
    "  #   than its parent"
  );
  m_lineage_creation_method->insertMapping(
    ChildFitnessVsPopulation,
    "when a child's (potential) fitness is higher\n"
    "  #   than max in population"
  );
  m_lineage_creation_method->insertMapping(
    ChildFitnessVsDomLinChildInDom,
    "when a child's (potential) fitness is higher\n"
    "  #   than max in dom.  lineage\n"
    "  #   *and* the child is in the dominant lineage, or (2)"
  );
  m_lineage_creation_method->insertMapping(
    ChildFitnessVsDomLinAndOwn,
    "when a child's (potential) fitness is higher\n"
    "  #   than max in dom.  lineage\n"
    "  #   (and that of its own lineage)"
  );
  m_lineage_creation_method->insertMapping(
    ChildFitnessVsDomLinAndOwn2,
    "same as (4)???"
  );
  m_lineage_creation_method->insertMapping(
    ChildFitnessVsOwnLin,
    "when a child's (potential) fitness is higher\n"
    "  #   than own lineage"
  );


  // Debug
  m_debug_level = new cSetupIntChoice(
    "DEBUG_LEVEL",
    "",
    DebugComments,
    true,
    this,
    "m_debug_level"
  );
  m_debug_level->insertMapping(
    DebugErrorsOnly,
    "ERRORS ONLY"
  );
  m_debug_level->insertMapping(
    DebugWarnings,
    "WARNINGS"
  );
  m_debug_level->insertMapping(
    DebugComments,
    "COMMENTS"
  );


  /*
  order of appends below determines order of entries in genesis file.
  */
  insertSetupDescription(
    new cSetupHeader(
      "### This file includes all the run-time defines...\n",
      this,
      "genesis header description"
    )
  );
  insertSetupDescription(m_version_id);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Architecture Variables ###\n",
      this,
      "genesis ### Architecture Variables ###"
    )
  );
  insertSetupDescription(m_max_updates);
  insertSetupDescription(m_max_generations);
  insertSetupDescription(m_end_condition);
  insertSetupDescription(m_world_x);
  insertSetupDescription(m_world_y);
  insertSetupDescription(m_max_cpu_threads);
  insertSetupDescription(m_random_seed);

  m_setup_descriptions.append(
    new cSetupHeader(
      "\n### Configuration Files ###\n",
      this,
      "genesis ### Configuration Files ###"
    )
  );
  insertSetupDescription(m_work_directory_path);
  insertSetupDescription(m_inst_set_path);
  insertSetupDescription(m_event_file_path);
  insertSetupDescription(m_analyze_file_path);
  insertSetupDescription(m_environment_file_path);
  insertSetupDescription(m_start_creature_path);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Reproduction ###\n",
      this,
      "genesis ### Reproduction ###"
    )
  );
  insertSetupDescription(m_birth_method);
  insertSetupDescription(m_death_method);
  insertSetupDescription(m_age_limit);
  insertSetupDescription(m_age_deviation);
  insertSetupDescription(m_alloc_method);
  insertSetupDescription(m_divide_method);
  insertSetupDescription(m_generation_inc_method);
  insertSetupDescription(m_child_size_range);
  insertSetupDescription(m_min_copied_lines);
  insertSetupDescription(m_min_exe_lines);
  insertSetupDescription(m_require_allocate);
  insertSetupDescription(m_required_task);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Mutations ###\n",
      this,
      "genesis ### Mutations ###"
    )
  );
  insertSetupDescription(
    new cSetupHeader(
      "\n## mutations that occur during execution\n",
      this,
      "genesis ## mutations that occur during execution"
    )
  );
  insertSetupDescription(m_point_mut_prob);
  insertSetupDescription(m_copy_mut_prob);
  insertSetupDescription(
    new cSetupHeader(
      "\n## mutations that occur on divide\n",
      this,
      "genesis ## mutations that occur on divide"
    )
  );
  insertSetupDescription(m_ins_mut_prob);
  insertSetupDescription(m_del_mut_prob);
  insertSetupDescription(m_divide_mut_prob);
  insertSetupDescription(m_divide_ins_prob);
  insertSetupDescription(m_divide_del_prob);
  insertSetupDescription(m_parent_mut_prob);
  insertSetupDescription(m_crossover_prob);
  insertSetupDescription(m_aligned_cross_prob);
  insertSetupDescription(m_exe_err_prob);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Mutation reversions ###\n"
      "# these slow down avida a lot, and should be set to 0 normally.\n",
      this,
      "genesis ### Mutation reversions ###"
    )
  );
  insertSetupDescription(m_revert_fatal);
  insertSetupDescription(m_revert_detrimental);
  insertSetupDescription(m_revert_neutral);
  insertSetupDescription(m_revert_beneficial);
  insertSetupDescription(m_sterilize_fatal);
  insertSetupDescription(m_sterilize_detrimental);
  insertSetupDescription(m_sterilize_neutral);
  insertSetupDescription(m_sterilize_beneficial);
  insertSetupDescription(m_fail_implicit);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Time slicing ###\n",
      this,
      "genesis ### Time slicing ###"
    )
  );
  insertSetupDescription(m_ave_time_slice);
  insertSetupDescription(m_slicing_method);
  insertSetupDescription(m_size_merit_method);
  insertSetupDescription(m_base_size_merit);
  insertSetupDescription(m_task_merit_method);
  insertSetupDescription(m_max_label_exe_size);
  insertSetupDescription(m_merit_time);
  insertSetupDescription(m_max_num_tasks_rewarded);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Genotype info ###\n",
      this,
      "genesis ### Genotype info ###"
    )
  );
  insertSetupDescription(m_threshold);
  insertSetupDescription(m_genotype_print);
  insertSetupDescription(m_genotype_print_dom);
  insertSetupDescription(m_species_threshold);
  insertSetupDescription(m_species_recording);
  insertSetupDescription(m_species_print);
  insertSetupDescription(m_test_cpu_time_mod);
  insertSetupDescription(m_track_main_lineage);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Log files ###\n",
      this,
      "genesis ### Log files ###"
    )
  );
  insertSetupDescription(m_log_breed_count);
  insertSetupDescription(m_log_creatures);
  insertSetupDescription(m_log_phylogeny);
  insertSetupDescription(m_log_genotypes);
  insertSetupDescription(m_log_threshold);
  insertSetupDescription(m_log_species);
  insertSetupDescription(m_log_genealogy);
  insertSetupDescription(m_log_landscape);
  insertSetupDescription(m_log_mutations);
  insertSetupDescription(m_log_lineages);
  insertSetupDescription(m_lineage_creation_method);

  insertSetupDescription(
    new cSetupHeader(
      "\n### Debug ###\n",
      this,
      "genesis ### Debug ###"
    )
  );
  insertSetupDescription(m_debug_level);



  // Path to genesis file
  insertSetupDescriptionMapping(m_genesis_path);

  // Avida file version ID
  insertSetupDescriptionMapping(m_version_id);

  // Architecture variables
  insertSetupDescriptionMapping(m_max_updates);
  insertSetupDescriptionMapping(m_max_generations);
  insertSetupDescriptionMapping(m_end_condition);
  insertSetupDescriptionMapping(m_world_x);
  insertSetupDescriptionMapping(m_world_y);
  insertSetupDescriptionMapping(m_max_cpu_threads);
  insertSetupDescriptionMapping(m_random_seed);

  // Configuration files
  insertSetupDescriptionMapping(m_work_directory_path);
  insertSetupDescriptionMapping(m_inst_set_path);
  insertSetupDescriptionMapping(m_event_file_path);
  insertSetupDescriptionMapping(m_analyze_file_path);
  insertSetupDescriptionMapping(m_environment_file_path);
  insertSetupDescriptionMapping(m_start_creature_path);

  // Reproduction
  insertSetupDescriptionMapping(m_birth_method);
  insertSetupDescriptionMapping(m_death_method);
  insertSetupDescriptionMapping(m_age_limit);
  insertSetupDescriptionMapping(m_age_deviation);
  insertSetupDescriptionMapping(m_alloc_method);
  insertSetupDescriptionMapping(m_divide_method);
  insertSetupDescriptionMapping(m_generation_inc_method);

  // Divide restrictions
  insertSetupDescriptionMapping(m_child_size_range);
  insertSetupDescriptionMapping(m_min_copied_lines);
  insertSetupDescriptionMapping(m_min_exe_lines);
  insertSetupDescriptionMapping(m_require_allocate);
  insertSetupDescriptionMapping(m_required_task);

  // Mutations
    // mutations that occur during execution
  insertSetupDescriptionMapping(m_point_mut_prob);
  insertSetupDescriptionMapping(m_copy_mut_prob);
    // mutations that occur on divide
  insertSetupDescriptionMapping(m_ins_mut_prob);
  insertSetupDescriptionMapping(m_del_mut_prob);
  insertSetupDescriptionMapping(m_divide_mut_prob);
  insertSetupDescriptionMapping(m_divide_ins_prob);
  insertSetupDescriptionMapping(m_divide_del_prob);
  insertSetupDescriptionMapping(m_parent_mut_prob);
  insertSetupDescriptionMapping(m_crossover_prob);
  insertSetupDescriptionMapping(m_aligned_cross_prob);
  insertSetupDescriptionMapping(m_exe_err_prob);

  // Mutation reversions
  insertSetupDescriptionMapping(m_revert_fatal);
  insertSetupDescriptionMapping(m_revert_detrimental);
  insertSetupDescriptionMapping(m_revert_neutral);
  insertSetupDescriptionMapping(m_revert_beneficial);
  insertSetupDescriptionMapping(m_sterilize_fatal);
  insertSetupDescriptionMapping(m_sterilize_detrimental);
  insertSetupDescriptionMapping(m_sterilize_neutral);
  insertSetupDescriptionMapping(m_sterilize_beneficial);
  insertSetupDescriptionMapping(m_fail_implicit);

  // Time slicing
  insertSetupDescriptionMapping(m_ave_time_slice);
  insertSetupDescriptionMapping(m_slicing_method);
  insertSetupDescriptionMapping(m_size_merit_method);
  insertSetupDescriptionMapping(m_base_size_merit);
  insertSetupDescriptionMapping(m_task_merit_method);
  insertSetupDescriptionMapping(m_max_label_exe_size);
  insertSetupDescriptionMapping(m_merit_time);
  insertSetupDescriptionMapping(m_max_num_tasks_rewarded);

  // Genotype info
  insertSetupDescriptionMapping(m_threshold);
  insertSetupDescriptionMapping(m_genotype_print);
  insertSetupDescriptionMapping(m_genotype_print_dom);
  insertSetupDescriptionMapping(m_species_threshold);
  insertSetupDescriptionMapping(m_species_recording);
  insertSetupDescriptionMapping(m_species_print);
  insertSetupDescriptionMapping(m_test_cpu_time_mod);
  insertSetupDescriptionMapping(m_track_main_lineage);

  // Log files
  insertSetupDescriptionMapping(m_log_breed_count);
  insertSetupDescriptionMapping(m_log_creatures);
  insertSetupDescriptionMapping(m_log_phylogeny);
  insertSetupDescriptionMapping(m_log_genotypes);
  insertSetupDescriptionMapping(m_log_threshold);
  insertSetupDescriptionMapping(m_log_species);
  insertSetupDescriptionMapping(m_log_genealogy);
  insertSetupDescriptionMapping(m_log_landscape);
  insertSetupDescriptionMapping(m_log_mutations);
  insertSetupDescriptionMapping(m_log_lineages);
  insertSetupDescriptionMapping(m_lineage_creation_method);

  // Debug
  insertSetupDescriptionMapping(m_debug_level);
}

GenesisWrapper::~GenesisWrapper(){ if(m_genesis) delete m_genesis; }

void
GenesisWrapper::setGenesis(cGenesis *genesis){
  GenDebug("entered.");
  cGenesis *old_genesis = m_genesis;
  m_genesis = genesis;
  if(old_genesis) {
    GenDebug(" --- delete old_genesis");
    delete old_genesis;
  }
}

cGenesis *
GenesisWrapper::getGenesis(void){
  GenDebug("entered.");
  return m_genesis;
}

int
GenesisWrapper::writeToGenesis(){
  GenDebug("entered.");

  cGenesis *genesis = new cGenesis();
  genesis->SetVerbose(true);

  GenDebug(" --- 1:  genesis ")(genesis);

  cSetupDescriptionMap::Iterator it;
  for ( it = m_sdmap.begin(); it != m_sdmap.end(); ++it ) {
    it.data()()->save(m_genesis);
    GenDebug(" --- saved key:  ")(it.key());
  }

  GenDebug(" --- 2:  genesis ")(genesis);

  setGenesis(genesis);

  GenDebug(" --- 3:  genesis ")(genesis);

  return 0;
}

int
GenesisWrapper::loadGenesis(const QString &genesis_filename){
  GenDebug("entered.");

  cGenesis *genesis = new cGenesis(genesis_filename.latin1());
  genesis->SetVerbose(true);

  cSetupDescriptionMap::Iterator it;
  for ( it = m_sdmap.begin(); it != m_sdmap.end(); ++it ) {
    it.data()()->load(genesis);
    GenDebug(" --- got key:  ")(it.key());
  }

  setGenesis(genesis);

  return 0;
}

void
GenesisWrapper::saveGenesis(const QString &genesis_filename){
  GenDebug("entered.");

  QFile genesis_file(genesis_filename); 
  if (genesis_file.open(IO_WriteOnly)){
    QTextStream t(&genesis_file);
    t.setEncoding(QTextStream::Latin1);
    cSetupDescription *setup_description;
    for(
      setup_description = m_setup_descriptions.first();
      setup_description;
      setup_description = m_setup_descriptions.next()
    ){
      setup_description->writeFileEntry(t);
    }
    genesis_file.close();
  } else {
    qFatal("<GenesisWrapper::saveGenesis> couldn't open file for write");
  }
}

void
GenesisWrapper::insertSetupDescriptionMapping(
  cSetupDescription *setup_description
)
{
  m_sdmap[setup_description->key()]
    = SetupDescriptionWrapper(setup_description);
}

void
GenesisWrapper::insertSetupDescription(
  cSetupDescription *setup_description
)
{
  m_setup_descriptions.append(setup_description);
}




bool
GenesisWrapper::isFileKeyValid(QString key){
  return (0 != m_sdmap[key]());
}

QString
GenesisWrapper::defaultQStringValue(
  QString key,
  bool &ok
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->defaultQStringValue(ok);
  else {
    ok = false;
    return "error:  invalid key";
  }
}

int
GenesisWrapper::defaultIntValue(
  QString key,
  bool &ok
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->defaultIntValue(ok);
  else {
    ok = false;
    return 0;
  }
}

double
GenesisWrapper::defaultDblValue(
  QString key,
  bool &ok
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->defaultDblValue(ok);
  else {
    ok = false;
    return 0.0;
  }
}

QString
GenesisWrapper::qStringValue(
  QString key,
  bool &ok
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->qStringValue(ok);
  else {
    ok = false;
    return "error:  invalid key";
  }
}

int
GenesisWrapper::intValue(
  QString key,
  bool &ok
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->intValue(ok);
  else {
    ok = false;
    return 0;
  }
}

double
GenesisWrapper::dblValue(
  QString key,
  bool &ok
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->dblValue(ok);
  else {
    ok = false;
    return 0.0;
  }
}

int
GenesisWrapper::set(
  QString key,
  QString value
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->set(value);
  else
    return 1;
}

int
GenesisWrapper::set(
  QString key,
  int value
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->set(value);
  else
    return 1;
}

int
GenesisWrapper::set(
  QString key,
  double value
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->set(value);
  else
    return 1;
}

int
GenesisWrapper::check(
  QString key,
  QString value
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->check(value);
  else
    return 1;
}

int
GenesisWrapper::check(
  QString key,
  int value
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->check(value);
  else
    return 1;
}

int
GenesisWrapper::check(
  QString key,
  double value
){
  if(isFileKeyValid(key))
    return m_sdmap[key]()->check(value);
  else
    return 1;
}

