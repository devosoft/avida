//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_GENESIS_HH
#define SETUP_GENESIS_HH

#include <qobject.h>
#include <qptrlist.h>
#include <qmap.h>


class cGenesis;

// functional setup classes
class cSetupIntRange;
class cSetupIntChoice;
class cSetupDoubleRange;
class cSetupQString;
class cSetupDescription;

class GenesisWrapper
: public QObject
{
protected:
  cGenesis *m_genesis;
public:
  enum EndConditionMode {
    MaxUpdatesORMaxGenerations = 0,
    MaxUpdatesANDMaxGenerations = 1
  };
  enum BirthMethodMode {
    ReplaceRandomInNeighborhood = 0,
    ReplaceOldestInNeighborhood = 1,
    ReplaceLargestAgeOverMeritInNeighborhood = 2,
    PlaceInEmptyCellsInNeighborhood = 3,
    ReplaceRandomInPopulation = 4,
    ReplaceOldestInPopulation = 5
  };
  enum DeathMethodMode {
    NeverDie = 0,
    InstrExecReachesAgeLimit = 1,
    InstrExecReachesAgeLimitTimesLength = 2
  };
  enum AllocMethodMode {
    DefaultInst = 0,
    Necrophilia = 1,
    Random = 2
  };
  enum DivideMethodMode {
    MotherAndChild = 0,
    BothChildren = 1
  };
  enum GenerationIncMethodMode {
    IncrChildOnly = 0,
    IncrMotherAndChild = 1
  };
  enum SlicingMethodMode {
    Constant = 0,
    Probabilistic = 1,
    Integrated = 2
  };
  enum SizeMeritMethod {
    MeritOff = 0,
    CopiedSize = 1,
    ExecutedSize = 2,
    FullSize = 3,
    MinExecAndCopied = 4,
    SqrtMinSize = 5
  };
  enum MeritTimeMode {
    TaskCompletion = 0,
    PostDivision = 1
  };
  enum SpeciesRecordingMode {
    SpeciesRecordingOff = 0,
    FullSpeciesRecording = 1,
    LimitSpeciesSearch = 2
  };
  enum GenotypeLoggingMode {
    GenotypeLoggingOff = 0,
    LogAllGenotypes = 1,
    LogThresholdGenotypes = 2
  };
  enum GenealogyLoggingMode {
    GenealogyLoggingOff = 0,
    LogAllGenealogy = 1,
    LogParentGenealogy = 2
  };
  enum LineageCreationMethodMode {
    ManualLineageCreation = 0,
    ChildFitnessVsParent = 1,
    ChildFitnessVsPopulation = 2,
    ChildFitnessVsDomLinChildInDom = 3,
    ChildFitnessVsDomLinAndOwn = 4,
    ChildFitnessVsDomLinAndOwn2 = 5,
    ChildFitnessVsOwnLin = 6
  };
  enum DebugLevelMode {
    DebugErrorsOnly = 0,
    DebugWarnings = 1,
    DebugComments = 2
  };
  enum OnOffMode {
    Off = 0,
    On = 1
  };

protected:
  class SetupDescriptionWrapper {
  protected:
    static cSetupDescription *s_bogus_setup_description;
    cSetupDescription *m_setup_description;
  public:
    SetupDescriptionWrapper()
    : m_setup_description(s_bogus_setup_description)
    {}
    SetupDescriptionWrapper(cSetupDescription *setup_description)
    : m_setup_description(setup_description)
    {}
    cSetupDescription *operator()(){ return m_setup_description; }
  };
  typedef QMap<QString, SetupDescriptionWrapper> cSetupDescriptionMap;
  cSetupDescriptionMap m_sdmap;
  QPtrList<cSetupDescription> m_setup_descriptions;

  // Path to genesis file
  cSetupQString *m_genesis_path;

  // Avida file version ID
  cSetupQString *m_version_id;

  // Architecture variables
  cSetupIntRange *m_max_updates;
  cSetupIntRange *m_max_generations;
  cSetupIntChoice *m_end_condition;
  cSetupIntRange *m_world_x;
  cSetupIntRange *m_world_y;
  cSetupIntRange *m_max_cpu_threads;
  cSetupIntRange *m_random_seed;

  // Configuration files
  cSetupQString *m_work_directory_path;
  cSetupQString *m_inst_set_path;
  cSetupQString *m_event_file_path;
  cSetupQString *m_analyze_file_path;
  cSetupQString *m_environment_file_path;
  cSetupQString *m_start_creature_path;

  // Reproduction
  cSetupIntChoice *m_birth_method;
  cSetupIntChoice *m_death_method;
  cSetupIntRange *m_age_limit;
  cSetupDoubleRange *m_age_deviation;
  cSetupIntChoice *m_alloc_method;
  cSetupIntChoice *m_divide_method;
  cSetupIntChoice *m_generation_inc_method;

  // Divide restrictions
  cSetupDoubleRange *m_child_size_range;
  cSetupDoubleRange *m_min_copied_lines;
  cSetupDoubleRange *m_min_exe_lines;
  cSetupIntChoice *m_require_allocate;
  cSetupIntRange *m_required_task;

  // Mutations
    // mutations that occur during execution
  cSetupDoubleRange *m_point_mut_prob;
  cSetupDoubleRange *m_copy_mut_prob;
    // mutations that occur on divide
  cSetupDoubleRange *m_ins_mut_prob;
  cSetupDoubleRange *m_del_mut_prob;
  cSetupDoubleRange *m_divide_mut_prob;
  cSetupDoubleRange *m_divide_ins_prob;
  cSetupDoubleRange *m_divide_del_prob;
  cSetupDoubleRange *m_parent_mut_prob;
  cSetupDoubleRange *m_crossover_prob;
  cSetupDoubleRange *m_aligned_cross_prob;
  cSetupDoubleRange *m_exe_err_prob;

  // Mutation reversions
  cSetupDoubleRange *m_revert_fatal;
  cSetupDoubleRange *m_revert_detrimental;
  cSetupDoubleRange *m_revert_neutral;
  cSetupDoubleRange *m_revert_beneficial;
  cSetupDoubleRange *m_sterilize_fatal;
  cSetupDoubleRange *m_sterilize_detrimental;
  cSetupDoubleRange *m_sterilize_neutral;
  cSetupDoubleRange *m_sterilize_beneficial;
  cSetupIntChoice *m_fail_implicit;

  // Time slicing
  cSetupIntRange *m_ave_time_slice;
  cSetupIntChoice *m_slicing_method;
  cSetupIntChoice *m_size_merit_method;
  cSetupIntRange *m_base_size_merit;
  cSetupIntChoice *m_task_merit_method;
  cSetupIntRange *m_max_label_exe_size;
  cSetupIntChoice *m_merit_time;
  cSetupIntRange *m_max_num_tasks_rewarded;

  // Genotype info
  cSetupIntRange *m_threshold;
  cSetupIntChoice *m_genotype_print;
  cSetupIntRange *m_genotype_print_dom;
  cSetupIntRange *m_species_threshold;
  cSetupIntChoice *m_species_recording;
  cSetupIntChoice *m_species_print;
  cSetupIntRange *m_test_cpu_time_mod;
  cSetupIntChoice *m_track_main_lineage;

  // Log files
  cSetupIntChoice *m_log_breed_count;
  cSetupIntChoice *m_log_creatures;
  cSetupIntChoice *m_log_phylogeny;
  cSetupIntChoice *m_log_genotypes;
  cSetupIntChoice *m_log_threshold;
  cSetupIntChoice *m_log_species;
  cSetupIntChoice *m_log_genealogy;
  cSetupIntChoice *m_log_landscape;
  cSetupIntChoice *m_log_mutations;
  cSetupIntChoice *m_log_lineages;
  cSetupIntChoice *m_lineage_creation_method;

  // Debug
  cSetupIntChoice *m_debug_level;
public:
  GenesisWrapper(
    QObject *parent = 0,
    const char *name = 0
  );
  ~GenesisWrapper();
  void setGenesis(cGenesis *genesis);
  cGenesis *getGenesis(void);
  int writeToGenesis(void);
  int loadGenesis(const QString &genesis_filename);
  void saveGenesis(const QString &genesis_filename);

  bool isFileKeyValid(
    QString key
  );
  QString defaultQStringValue(
    QString key,
    bool &ok
  );
  int defaultIntValue(
    QString key,
    bool &ok
  );
  double defaultDblValue(
    QString key,
    bool &ok
  );
  QString qStringValue(
    QString key,
    bool &ok
  );
  int intValue(
    QString key,
    bool &ok
  );
  double dblValue(
    QString key,
    bool &ok
  );
  int set(
    QString key,
    QString value
  );
  int set(
    QString key,
    int value
  );
  int set(
    QString key,
    double value
  );
  int check(
    QString key,
    QString value
  );
  int check(
    QString key,
    int value
  );
  int check(
    QString key,
    double value
  );
protected:
  void insertSetupDescriptionMapping(
    cSetupDescription *setup_description
  );
  void insertSetupDescription(
    cSetupDescription *setup_description
  );
};

#endif /* !SETUP_GENESIS_HH */
