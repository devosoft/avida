//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_HH
#define CONFIG_HH

#ifndef GENESIS_HH
#include "genesis.hh"
#endif
#ifndef STRING_LIST_HH
#include "string_list.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

#define SLICE_CONSTANT         0
#define SLICE_PROB_MERIT       1
#define SLICE_INTEGRATED_MERIT 2

#define POSITION_CHILD_RANDOM           0
#define POSITION_CHILD_AGE              1
#define POSITION_CHILD_MERIT            2
#define POSITION_CHILD_EMPTY            3
#define NUM_LOCAL_POSITION_CHILD        4

#define POSITION_CHILD_FULL_SOUP_RANDOM 4
#define POSITION_CHILD_FULL_SOUP_ELDEST 5
#define POSITION_CHILD_DEME_RANDOM      6

#define DEATH_METHOD_OFF       0
#define DEATH_METHOD_CONST     1
#define DEATH_METHOD_MULTIPLE  2

#define ALLOC_METHOD_DEFAULT   0
#define ALLOC_METHOD_NECRO     1
#define ALLOC_METHOD_RANDOM    2

#define DIVIDE_METHOD_OFFSPRING 0
#define DIVIDE_METHOD_SPLIT     1
#define DIVIDE_METHOD_BIRTH     2

#define GENERATION_INC_OFFSPRING 0
#define GENERATION_INC_BOTH      1

#define TASK_MERIT_OFF         0
#define TASK_MERIT_EXPONENTIAL 1
#define TASK_MERIT_LINEAR      2

#define SIZE_MERIT_OFF         0
#define SIZE_MERIT_COPIED      1
#define SIZE_MERIT_EXECUTED    2
#define SIZE_MERIT_FULL        3
#define SIZE_MERIT_LEAST       4
#define SIZE_MERIT_SQRT_LEAST  5

#define GEOMETRY_GLOBAL 0
#define GEOMETRY_GRID   1
#define GEOMETRY_TORUS  2

class cString; // aggregate
class cStringList; // aggregate
class cGenesis;
template <class T> class tList; // aggregate
template <class T> class tListIterator; // aggregate

class cConfig {
protected:
  class cConfigEntryBase {
  protected:
    const cString genesis_tag;
    const cString default_value;
    const cStringList description;
  public:
    cConfigEntryBase(const cString & _tag, const cString & _def,
		     const cString & _desc)
      : genesis_tag(_tag), default_value(_def), description(_desc, '\n') { ; }
    virtual ~cConfigEntryBase(){}

    //    virtual ~cConfigEntryBase();
    const cString & GetTag() { return genesis_tag; }
    const cString & GetDefault() { return default_value; }
    const cStringList & GetDesc() { return description; }

    virtual bool LoadValue(cGenesis & genesis) = 0;
  };

  template <class T> class tConfigEntry : public cConfigEntryBase {
  private:
    T & variable;
  public:
    tConfigEntry(T & _var, const cString & _def, const cString & _tag,
		 const cString _desc)
      : cConfigEntryBase(_tag, _def, _desc), variable(_var)
    { ; }

    bool LoadValue(cGenesis & genesis) {
      genesis.Read(variable, genesis_tag, default_value);
      return true;
    }
  };

  class cConfigGroup {
  private:
    const cString group_name;
    cStringList comment;
    tList<cConfigEntryBase> group_entries;
  public:
    cConfigGroup(const cString & _name) : group_name(_name) { ; }
    ~cConfigGroup() {
      while (group_entries.GetSize() > 0) delete group_entries.Pop();
    }

    void SetComment(const cString & _comment) { comment.Load(_comment, '\n'); }

    const cString & GetName() { return group_name; }
    const cStringList & GetComment() { return comment; }
    tList<cConfigEntryBase> & GetEntries() { return group_entries; }

    void LoadValues(cGenesis & genesis) {
      tListIterator<cConfigEntryBase> group_it(group_entries);
      cConfigEntryBase * cur_entry;
      while ((cur_entry = group_it.Next()) != NULL) {
	cur_entry->LoadValue(genesis);
      }
    };

    void Add(int & _var, const cString &  _def, const cString & _tag,
	     const cString & _desc)
    {
      group_entries.
	PushRear(new cConfig::tConfigEntry<int>(_var,_def,_tag,_desc));
    }

    void Add(double & _var, const cString & _def, const cString & _tag,
	     const cString & _desc)
    {
      group_entries.
	PushRear(new cConfig::tConfigEntry<double>(_var,_def,_tag,_desc));
    }

    void Add(cString & _var, const cString & _def, const cString & _tag,
	     const cString & _desc)
    {
      group_entries.
	PushRear(new cConfig::tConfigEntry<cString>(_var,_def,_tag,_desc));
    }

    void Add(bool & _var, const cString & _def, const cString & _tag,
	     const cString & _desc)
    {
      group_entries.
	PushRear(new cConfig::tConfigEntry<bool>(_var,_def,_tag,_desc));
    }
  };

  // Collection of genesis groups...
  static tList<cConfigGroup> group_list;

  // Major Configurations
  static bool analyze_mode;     // Should avida do only analysis work?
  static bool primitive_mode;   // Should avida run in primitive mode?
  static bool interactive_analyze; // Should analyze mode be interactive?

  // Config Filenames
  static cString default_dir;
  static cString genesis_filename;
  static cString inst_filename;
  static cString event_filename;
  static cString analyze_filename;
  static cString env_filename;

  static cString start_creature;
  static cString clone_filename;
  static cString load_pop_filename;

  // Archetecture
  static int max_updates;
  static int max_generations;
  static int end_condition_mode;
  static int world_x;
  static int world_y;
  static int world_geometry;
  static int num_demes;
  static int rand_seed;

  // Mutations
  static double point_mut_prob;
  static double copy_mut_prob;
  static double ins_mut_prob;
  static double del_mut_prob;
  static double div_mut_prob;
  static double divide_mut_prob;
  static double divide_ins_prob;
  static double divide_del_prob;
  static double parent_mut_prob;
  static int special_mut_line;

  // CPU Configutation
  static int num_instructions;
  static int hardware_type;
  static int max_cpu_threads;
  static double thread_slicing_method;

  // Merit info
  static int size_merit_method;
  static int base_size_merit;
  static double default_bonus;
  static int ave_time_slice;

  // Task info
  static int num_tasks;
  static int num_reactions;
  static int num_resources;

  // Task Merit Method
  static int task_merit_method;
  static int merit_time;

  // Transfering merits...
  static double merit_given;
  static double merit_received;
  static int max_donate_kin_distance;
  static int max_donate_edit_distance;
  static int max_donates;

  // Methodology
  static int slicing_method;
  static int birth_method;
  static int prefer_empty;
  static int death_method;
  static int alloc_method;
  static int divide_method;
  static double recomb_prob;
  static int num_modules;
  static int cont_rec_regs;
  static int corespond_rec_regs;
  static int two_fold_cost_sex;
  static int same_length_sex;
  static int required_task;
  static int immunity_task;
  static int required_reaction;
  static int lineage_creation_method;
  static int generation_inc_method;
  static int age_limit;
  static double age_deviation;
  static double child_size_range;
  static double min_copied_lines;
  static double min_exe_lines;
  static int require_allocate;
  static int max_label_exe_size;

  // For specialized experiments -- mutation reversion.
  static bool test_on_divide;
  static bool test_sterilize;
  static double revert_fatal;
  static double revert_neg;
  static double revert_neut;
  static double revert_pos;
  static double sterilize_fatal;
  static double sterilize_neg;
  static double sterilize_neut;
  static double sterilize_pos;
  static int fail_implicit;

  // Genealogy & Recording
  static int species_threshold;
  static int threshold;
  static int genotype_print;
  static int species_print;
  static int species_recording;
  static int genotype_print_dom;
  static int test_cpu_time_mod;
  static int track_main_lineage;

  // Log files...
  static bool log_threshold_only;

  static bool log_creatures;
  static bool log_genotypes;
  static bool log_threshold;
  static bool log_species;
  static bool log_landscape;
  static bool log_mutations;
  static bool log_lineages;

  // Debugging
  static int debug_level;

  // Viewer
  static int view_mode;

  // Death
  static double die_prob; 

  // Other functions...
  static void ProcessConfiguration(int argc, char* argv[], cGenesis & genesis);
public:
  static void InitGroupList();
  static void Setup(cGenesis & genesis);
  static void Setup(int argc, char * argv[]);
  static void SetupMS();
  static void PrintGenesis(const cString & filename);

  // ``Get''
  static bool GetAnalyzeMode() { return analyze_mode; }
  static bool GetPrimitiveMode() { return primitive_mode; }
  static bool GetInteractiveAnalyze() { return interactive_analyze; }

  /*
  addition to permit access to cConfig::default_dir for use in locating
  preset_organisms subdirectory (accessed in godbox widget).
  -- kaben.
  */
  static cString & GetDefaultDir() { return default_dir; }
  /* -- kaben. */

  static cString & GetGenesisFilename() { return genesis_filename; }
  static cString & GetInstFilename()  { return inst_filename; }
  static cString & GetEventFilename() { return event_filename; }
  static cString & GetAnalyzeFilename() { return analyze_filename; }
  static cString & GetEnvironmentFilename() { return env_filename; }

  static cString & GetStartCreature() { return start_creature; }
  static cString & GetCloneFilename() { return clone_filename; }
  static cString & GetLoadPopFilename() { return load_pop_filename; }

  static int GetMaxUpdates()     { return max_updates; }
  static int GetMaxGenerations() { return max_generations; }
  static int GetEndConditionMode() { return end_condition_mode; }
  static int GetWorldX()         { return world_x; }
  static int GetWorldY()         { return world_y; }
  static int GetWorldGeometry()  { return world_geometry; }
  static int GetNumDemes()       { return num_demes; }
  static int GetRandSeed()       { return rand_seed; }

  static double GetPointMutProb()  { return point_mut_prob; }
  static double GetCopyMutProb()   { return copy_mut_prob; }
  static double GetInsMutProb()    { return ins_mut_prob; }
  static double GetDelMutProb()    { return del_mut_prob; }
  static double GetDivMutProb()    { return div_mut_prob; }
  static double GetDivideMutProb() { return divide_mut_prob; }
  static double GetDivideInsProb() { return divide_ins_prob; }
  static double GetDivideDelProb() { return divide_del_prob; }
  static double GetParentMutProb() { return parent_mut_prob; }
  static int GetSpecialMutLine() { return special_mut_line; }

  static int GetNumInstructions() { return num_instructions; }
  static int GetHardwareType() { return hardware_type; }
  static int GetMaxCPUThreads() { return max_cpu_threads; }
  static double GetThreadSlicingMethod() { return thread_slicing_method; }

  static int GetSizeMeritMethod() { return size_merit_method; }
  static int GetBaseSizeMerit()  { return base_size_merit; }
  static double GetDefaultBonus() { return default_bonus; }
  static int GetTaskMeritMethod() { return task_merit_method; }
  static int GetMaxLabelExeSize() { return max_label_exe_size; }

  static int GetMeritTime() { return merit_time; }

  static double GetMeritGiven() { return merit_given; }
  static double GetMeritReceived() { return merit_received; }
  static int GetMaxDonateKinDistance() { return max_donate_kin_distance; }
  static int GetMaxDonateEditDistance() { return max_donate_edit_distance; }
  static int GetMaxDonates() { return max_donates; }

  static int GetNumTasks() { return num_tasks; }
  static int GetNumReactions() { return num_reactions; }
  static int GetNumResources() { return num_resources; }

  static int GetSlicingMethod() { return slicing_method; }
  static int GetBirthMethod() { return birth_method; }
  static int GetPreferEmpty() { return prefer_empty; }
  static int GetDeathMethod() { return death_method; }
  static int GetAllocMethod() { return alloc_method; }
  static int GetDivideMethod() { return divide_method; }
  static double GetRecombProb() { return recomb_prob; }
  static int GetNumModules() { return num_modules; }
  static int GetContRecRegs() { return cont_rec_regs; }
  static int GetCorespondRecRegs() { return corespond_rec_regs; }
  static int GetTwoFoldCostSex() { return two_fold_cost_sex; }
  static int GetSameLengthSex() { return same_length_sex; }
  static int GetRequiredTask() { return required_task; }
  static int GetImmunityTask() { return immunity_task; }
  static int GetRequiredReaction() { return required_reaction; }
  static int GetLineageCreationMethod() { return lineage_creation_method; }
  static int GetGenerationIncMethod() { return generation_inc_method; }
  static int GetAgeLimit() { return age_limit; }
  static double GetAgeDeviation() { return age_deviation; }
  static double GetChildSizeRange() { return child_size_range; }
  static double GetMinCopiedLines() { return min_copied_lines; }
  static double GetMinExeLines() { return min_exe_lines; }
  static int GetRequireAllocate() { return require_allocate; }

  static bool GetTestOnDivide() { return test_on_divide; }
  static bool GetTestSterilize() { return test_sterilize; }
  static double GetRevertFatal() { return revert_fatal; }
  static double GetRevertNeg() { return revert_neg; }
  static double GetRevertNeut() { return revert_neut; }
  static double GetRevertPos() { return revert_pos; }
  static double GetSterilizeFatal() { return sterilize_fatal; }
  static double GetSterilizeNeg() { return sterilize_neg; }
  static double GetSterilizeNeut() { return sterilize_neut; }
  static double GetSterilizePos() { return sterilize_pos; }
  static int GetFailImplicit() { return fail_implicit; }


  static int GetAveTimeslice() { return ave_time_slice; }
  static int GetSpeciesThreshold() { return species_threshold; }
  static int GetThreshold() { return threshold; }
  static int GetGenotypePrint() { return genotype_print; }
  static int GetSpeciesPrint() { return species_print; }
  static int GetSpeciesRecording() { return species_recording; }
  static int GetGenotypePrintDom() { return genotype_print_dom; }
  static int GetTestCPUTimeMod() { return test_cpu_time_mod; }
  static int GetTrackMainLineage() { return track_main_lineage; }


  static bool GetLogThresholdOnly()  { return log_threshold_only; }

  static bool GetLogCreatures() { return log_creatures; }
  static bool GetLogGenotypes() { return log_genotypes; }
  static bool GetLogThreshold() { return log_threshold; }
  static bool GetLogSpecies()   { return log_species; }
  static bool GetLogLandscape() { return log_landscape; }
  static bool GetLogLineages()  { return log_lineages; }

  static int GetDebugLevel() { return debug_level; }

  static int GetViewMode() { return view_mode; }

  static double GetDieProb() { return die_prob; }


  // ``Set''
  static void SetInstFilename(const cString & in_name)
    { inst_filename = in_name; }
  static void SetEventFilename(const cString & in_name)
    { event_filename = in_name; }
  static void SetAnalyzeFilename(const cString & in_name)
    { analyze_filename = in_name; }
  static void SetEnvironmentFilename(const cString & in_name)
    { env_filename = in_name; }

  static void SetMaxUpdates(int in_max_updates)
    { max_updates = in_max_updates; }
  static void SetMaxGenerations(int _in)
    { max_generations = _in; }
  static void SetEndConditionMode(int _in)
    { end_condition_mode = _in; }

  static void SetCopyMutProb(double _in)  { copy_mut_prob = _in; }
  static void SetDivMutProb(double _in)  { div_mut_prob = _in; }
  static void SetPointMutProb(double _in) { point_mut_prob = _in; }

  /*
  addition to permit tweaking of ins_mut_prob, del_mut_prob from godbox
  widget.
  -- kaben.
  */
  static void SetInsMutProb(double _in)  { ins_mut_prob = _in; }
  static void SetDelMutProb(double _in) { del_mut_prob = _in; }
  /* -- kaben. */

  static void SetNumInstructions(int in_num_instructions)
    { num_instructions = in_num_instructions; }
  static void SetHardwareType(int in_htype) { hardware_type = in_htype; }
  static void SetMaxCPUThreads(int in_threads)
    { max_cpu_threads = in_threads; }

  static void SetSizeMeritMethod(int in_merit_method)
    { size_merit_method = in_merit_method; }
  static void SetBaseSizeMerit(int in_base_size_merit)
    { base_size_merit = in_base_size_merit; }
  static void SetDefaultBonus(int in_default_bonus)
    { default_bonus = in_default_bonus; }

  static void SetBirthMethod(int in_birth_method)
    { birth_method = in_birth_method; }
  static void SetPreferEmpty(int in_prefer_empty)
    { prefer_empty = in_prefer_empty; }
  static void SetDeathMethod(int in_death_method)
    { death_method = in_death_method; }
  static void SetAllocMethod(int in_alloc_method)
    { alloc_method = in_alloc_method; }
  static void SetDivideMethod(int in_divide_method)
    { divide_method = in_divide_method; }
  static void SetLineageCreationMethod( int in_lineage_creation_method )
    { lineage_creation_method = in_lineage_creation_method; }
  static void SetGenerationIncMethod(int in_generation_inc_method)
    { generation_inc_method = in_generation_inc_method; }
  static void SetAgeLimit(int in_age_limit) { age_limit = in_age_limit; }
  static void SetAgeDeviation(double in_dev) { age_deviation = in_dev; }

  static void SetAveTimeslice(int in_slice) { ave_time_slice = in_slice; }

  static void SetNumTasks(int in_num) { num_tasks = in_num; }
  static void SetNumReactions(int in_num) { num_reactions = in_num; }
  static void SetNumResources(int in_num) { num_resources = in_num; }
};

#endif
