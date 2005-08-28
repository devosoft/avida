//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_CONFIG_HH
#include "setup_wizard_config.hh"
#endif

#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef GENESIS_HH
#include "genesis.hh"
#endif
#ifndef TOOLS_HH
#include "tools.hh"
#endif

#include "cPopulation_descr.h" // declarations and definitions


using namespace std;


void cSetupAssistantConfig::Setup(
  cGenesis &genesis
){
  cout << "<cPopulation>" << endl;
  
  // Load Archetecture...
  max_updates     = genesis.ReadInt("MAX_UPDATES", -1);
  max_generations = genesis.ReadInt("MAX_GENERATIONS", -1);
  end_condition_mode = genesis.ReadInt("END_CONDITION_MODE", 0);
  world_x         = genesis.ReadInt("WORLD-X");
  world_y         = genesis.ReadInt("WORLD-Y");

  birth_method   = genesis.ReadInt("BIRTH_METHOD", POSITION_CHILD_AGE);
  death_method   = genesis.ReadInt("DEATH_METHOD", DEATH_METHOD_OFF);
  alloc_method   = genesis.ReadInt("ALLOC_METHOD", ALLOC_METHOD_DEFAULT);
  divide_method  = genesis.ReadInt("DIVIDE_METHOD", DIVIDE_METHOD_SPLIT);
  required_task   = genesis.ReadInt("REQUIRED_TASK", -1);
  lineage_creation_method =
    genesis.ReadInt("LINEAGE_CREATION_METHOD", 0);
  generation_inc_method =
    genesis.ReadInt("GENERATION_INC_METHOD", GENERATION_INC_BOTH);
  age_limit      = genesis.ReadInt("AGE_LIMIT", -1);
  child_size_range = genesis.ReadFloat("CHILD_SIZE_RANGE", 2.0);
  min_copied_lines = genesis.ReadFloat("MIN_COPIED_LINES", 0.5);
  min_exe_lines    = genesis.ReadFloat("MIN_EXE_LINES", 0.5);
  require_allocate = genesis.ReadInt("REQUIRE_ALLOCATE", 1);

  revert_fatal = genesis.ReadFloat("REVERT_FATAL", 0.0);
  revert_neg   = genesis.ReadFloat("REVERT_DETRIMENTAL", 0.0);
  revert_neut  = genesis.ReadFloat("REVERT_NEUTRAL", 0.0);
  revert_pos   = genesis.ReadFloat("REVERT_BENEFICIAL", 0.0);
  sterilize_fatal = genesis.ReadFloat("STERILIZE_FATAL", 0.0);
  sterilize_neg   = genesis.ReadFloat("STERILIZE_DETRIMENTAL", 0.0);
  sterilize_neut  = genesis.ReadFloat("STERILIZE_NEUTRAL", 0.0);
  sterilize_pos   = genesis.ReadFloat("STERILIZE_BENEFICIAL", 0.0);
  test_on_divide = (revert_fatal > 0.0) || (revert_neg > 0.0) ||
    (revert_neut > 0.0) || (revert_pos > 0.0) || (sterilize_fatal > 0.0) ||
    (sterilize_neg > 0.0) || (sterilize_neut > 0.0) || (sterilize_pos > 0.0);
  fail_implicit = genesis.ReadInt("FAIL_IMPLICIT", 0);

  // Genealogy
  species_threshold  = genesis.ReadInt("SPECIES_THRESHOLD");
  threshold          = genesis.ReadInt("THRESHOLD");
  genotype_print     = genesis.ReadInt("GENOTYPE_PRINT");
  species_print      = genesis.ReadInt("SPECIES_PRINT");
  species_recording  = genesis.ReadInt("SPECIES_RECORDING");
  genotype_print_dom = genesis.ReadInt("GENOTYPE_PRINT_DOM");
  test_cpu_time_mod  = genesis.ReadInt("TEST_CPU_TIME_MOD", 20);
  track_main_lineage = genesis.ReadInt("TRACK_MAIN_LINEAGE", 0);

  // CPU Info
  max_cpu_threads = genesis.ReadInt("MAX_CPU_THREADS", 1);

  // Time Slicing Info
  slicing_method = genesis.ReadInt("SLICING_METHOD", SLICE_CONSTANT);
  size_merit_method = genesis.ReadInt("SIZE_MERIT_METHOD", 0);
  base_size_merit   = genesis.ReadInt("BASE_SIZE_MERIT", 0);
  ave_time_slice = genesis.ReadInt("AVE_TIME_SLICE", 30);
  merit_time = genesis.ReadInt("MERIT_TIME", 0);

  // Task Merit Method
  task_merit_method = genesis.ReadInt("TASK_MERIT_METHOD", TASK_MERIT_NORMAL);
  //max_num_tasks_rewarded = genesis.ReadInt("MAX_NUM_TASKS_REWARDED", -1);
  max_label_exe_size = genesis.ReadInt("MAX_LABEL_EXE_SIZE", 1);

  // Load Mutation Info
  point_mut_prob  = genesis.ReadFloat("POINT_MUT_PROB");
  copy_mut_prob   = genesis.ReadFloat("COPY_MUT_PROB");
  ins_mut_prob    = genesis.ReadFloat("INS_MUT_PROB");
  del_mut_prob    = genesis.ReadFloat("DEL_MUT_PROB");
  divide_mut_prob = genesis.ReadFloat("DIVIDE_MUT_PROB");
  divide_ins_prob = genesis.ReadFloat("DIVIDE_INS_PROB");
  divide_del_prob = genesis.ReadFloat("DIVIDE_DEL_PROB");
  parent_mut_prob = genesis.ReadFloat("PARENT_MUT_PROB");
  //crossover_prob  = genesis.ReadFloat("CROSSOVER_PROB");
  //aligned_cross_prob = genesis.ReadFloat("ALIGNED_CROSS_PROB");
  //exe_err_prob    = genesis.ReadFloat("EXE_ERROR_PROB");


  // Load Viewer Info...
  view_mode = genesis.ReadInt("VIEW_MODE");
  //viewer_type = genesis.ReadInt("VIEWER_TYPE",0);

  //log_breed_count = genesis.ReadInt("LOG_BREED_COUNT", 0);
  log_creatures   = genesis.ReadInt("LOG_CREATURES", 0);
  //log_phylogeny   = genesis.ReadInt("LOG_PHYLOGENY", 0);

  log_genotypes = genesis.ReadInt("LOG_GENOTYPES", 0);
  log_threshold_only = false;
  if (log_genotypes > 1) log_threshold_only = true;

  log_threshold = genesis.ReadInt("LOG_THRESHOLD", 0);
  log_species   = genesis.ReadInt("LOG_SPECIES", 0);
  log_landscape = genesis.ReadInt("LOG_LANDSCAPE", 0);
/* FIXME -- removed from base class. -- k */
  //log_mutations = genesis.ReadInt("LOG_MUTATIONS", 0);
  log_lineages = genesis.ReadInt("LOG_LINEAGES", 0);
}

void cSetupAssistantConfig::SetupFiles(
  cGenesis &genesis
){
  // Load the default directory...
  default_dir = genesis.ReadString("DEFAULT_DIR", DEFAULT_DIR);
  char dir_tail = default_dir[default_dir.GetSize() - 1];
  if (dir_tail != '\\' && dir_tail != '/') default_dir += "/";

  // Input files...
  inst_filename  = genesis.ReadString("INST_SET", "inst_set");
  event_filename = genesis.ReadString("EVENT_FILE", "events.cfg");
  analyze_filename = genesis.ReadString("ANALYZE_FILE", "analyze.cfg");
  env_filename = genesis.ReadString("ENVIRONMENT_FILE", "environment.cfg");
  start_creature = genesis.ReadString("START_CREATURE");

}

void cSetupAssistantConfig::SetupFiles(
  cString _default_dir,
  cString _inst_filename,
  cString _event_filename,
  cString _analyze_filename,
  cString _env_filename,
  cString _start_creature
){
  // Load the default directory...
  default_dir = _default_dir;
  char dir_tail = default_dir[default_dir.GetSize() - 1];
  if (dir_tail != '\\' && dir_tail != '/') default_dir += "/";

  // Input files...
  inst_filename  = _inst_filename;
  event_filename = _event_filename;
  analyze_filename = _analyze_filename;
  env_filename = _env_filename;
  start_creature = _start_creature;

}

void cSetupAssistantConfig::ProcessConfiguration(
  int argc,
  char * argv[],
  cGenesis & genesis
){
  genesis_filename = "genesis"; // Name of genesis file.
  int arg_num = 1;              // Argument number being looked at.
  analyze_mode = false;         // Initialize analyze_mode tp be off.
  primitive_mode = false;       // Initialize primitive_mode tp be off.

  // Load all of the args into string objects for ease of access.
  cString * args = new cString[argc];
  for (int i = 0; i < argc; i++) args[i] = argv[i];


  // -genesis option

  if (argc > 1 && (args[1] == "-g" || args[1] == "-genesis")) {
    if (argc < 3) {
      cerr << "Need filename for genesis file used." << endl;
      exit(0);
    }
    genesis_filename = args[2];
    arg_num += 2;
  }


  // Open and verify the genesis file.

  genesis.Open(genesis_filename);
  cString version_id = genesis.ReadString("VERSION_ID", "Unknown");
  if (genesis.IsOpen() == true && version_id != AVIDA_VERSION) {
    cerr << "/  WARNING   WARNING   WARNING   WARNING   WARNING  \\" << endl
	 << "|   Using incorrect genesis file.                   |" << endl
	 << "|   Version needed = \"" << AVIDA_VERSION
	 << "\".  Version used = \"" << version_id() << "\"   |" << endl
	 << "\\  WARNING   WARNING   WARNING   WARNING   WARNING  /" << endl
	 << endl;
  }

  // Then scan through and process the rest of the args.

  while (arg_num < argc || genesis.IsOpen() == false) {
    cString cur_arg = genesis.IsOpen() ? static_cast<cString>( args[arg_num] )
		      : static_cast<cString>( "--help" );

    // Test against the possible inputs.
    if (cur_arg == "-events" || cur_arg == "-e") {
      cout << "Known events:" << endl;
      for ( int i=0; i<cEventDescrs::num_of_events; i++ ){
	cout << "-----  "
	     << cEventDescrs::entries[i].GetName()
	     << "  -----" << endl;
	cout << cEventDescrs::entries[i].GetDescription() << endl;
      }
      exit(0);
    }
    else if (cur_arg == "--help" || cur_arg == "-help" ||
	cur_arg == "-h"     || genesis.IsOpen() == false) {
      cout << "Options:"<<endl
	   << "  -g[enesis] <filename> Set genesis file to be <filename>"<<endl
	   << "  -h[elp]               Help on options (this listing)"<<endl
	   << "  -e[vents]             Print a list of all known events"<< endl
	   << "  -s[eed] <value>       Set random seed to <value>"<<endl
	   << "  -viewer <value>       Sets Viewer to <value>"<<endl
	   << "  -v[ersion]            Prints the version number"<<endl
	   << "  -set <name> <value>   Overide the genesis file"<<endl
	   << "  -l[oad] <filename>    Load a clone file"<<endl
	   << "  -loadpop <filename>   Load a saved population file (precedence over load)"<<endl
	   << "  -a[nalyze]            Process analyze.cfg instead of normal run."<<endl
	// <<"  -p[rimitive]          Overide viewer to be primitive."<<endl
	   << endl;
	
      exit(0);
    }
    else if (cur_arg == "-seed" || cur_arg == "-s") {
      int in_seed = 0;
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
	cerr<<"Must include a number as the random seed!"<<endl;
	exit(0);
      } else {
	arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
	in_seed = cur_arg.AsInt();
      }
      genesis.AddInput("RANDOM_SEED", in_seed);
    } else if (cur_arg == "-analyze" || cur_arg == "-a") {
      analyze_mode = true;
    } else if (cur_arg == "-primitive" || cur_arg == "-p") {
      primitive_mode = true;
    } else if (cur_arg == "-load" || cur_arg == "-l") {
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
	cerr<<"Must include a filename to load from"<<endl;
	exit(0);
      } else {
	arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
	clone_filename = cur_arg;
      }
    } else if (cur_arg == "-loadpop" || cur_arg == "-lp") {
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
	cerr<<"Must include a filename to load from"<<endl;
	exit(0);
      } else {
	arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
	load_pop_filename = cur_arg;
      }
    } else if (cur_arg == "-version" || cur_arg == "-v") {
      cout << " by Charles Ofria" << endl;
      cout << " designed by Charles Ofria, Chris Adami, Travis Collier, C. Titus Brown, and Claus Wilke" << endl;
      cout << " For more information, see: http://dllab.caltech.edu/avida/" << endl;
      exit(0);
    } else if (cur_arg == "-set") {
      if (arg_num + 1 == argc || arg_num + 2 == argc) {
	cerr << "'-set' option must be followed by name and value" << endl;
	exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString name(cur_arg);
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString value(cur_arg);
      cout << "SET " << name() << " = " << value() << endl;
      genesis.AddInput(name(), value());
    } else if (cur_arg == "-viewer") {
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
	cerr<<"Must include viewer type"<<endl;
	exit(0);
      } else {
	arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
	//viewer_type = cur_arg.AsInt();
      }
      //genesis.AddInput("VIEWER_TYPE", viewer_type);


    } else if (cur_arg == "-g" || cur_arg == "-genesis") {
      cerr << "Error: -g[enesis] option must be listed first." << endl;
      exit(0);
    } else {
      cerr << "Unknown Option: " << argv[arg_num] << endl
	   << "Type: \"" << argv[0] << " -h\" for a full option list." << endl;
      exit(0);
    }

    arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
  }

  rand_seed = genesis.ReadInt("RANDOM_SEED");
  cout << "Random Seed: " << rand_seed;
  g_random.ResetSeed(rand_seed);
  if( rand_seed != g_random.GetSeed() ) cout << " -> " << g_random.GetSeed();
  cout << endl;

  delete [] args;
}

void
cSetupAssistantConfig::setDefaultDir(cString _default_dir){
  default_dir = _default_dir;
}

void
cSetupAssistantConfig::setInstFilename(cString _inst_filename){
  inst_filename = _inst_filename;
}
void
cSetupAssistantConfig::setEventFilename(cString _event_filename){
  event_filename = _event_filename;
}
void
cSetupAssistantConfig::setAnalyzeFilename(cString _analyze_filename){
  analyze_filename = _analyze_filename;
}
void
cSetupAssistantConfig::setEnvFilename(cString _env_filename){
  env_filename = _env_filename;
}
void
cSetupAssistantConfig::setStartCreatureFilename(cString _start_creature){
  start_creature = _start_creature;
}

void
cSetupAssistantConfig::setMaxUpdates(int _max_updates){
  max_updates = _max_updates;
}
void
cSetupAssistantConfig::setMaxGenerations(int _max_generations){
  max_generations = _max_generations;
}
void
cSetupAssistantConfig::setEndConditionMode(int _end_condition_mode){
  end_condition_mode = _end_condition_mode;
}
void
cSetupAssistantConfig::setWorldX(int _world_x){
  world_x = _world_x;
}
void
cSetupAssistantConfig::setWorldY(int _world_y){
  world_y = _world_y;
}

void
cSetupAssistantConfig::setBirthMethod(int _birth_method){
  birth_method = _birth_method;
}
void
cSetupAssistantConfig::setDeathMethod(int _death_method){
  death_method = _death_method;
}
void
cSetupAssistantConfig::setAllocMethod(int _alloc_method){
  alloc_method = _alloc_method;
}
void
cSetupAssistantConfig::setDivideMethod(int _divide_method){
  divide_method = _divide_method;
}
void
cSetupAssistantConfig::setRequiredTask(int _required_task){
  required_task = _required_task;
}
void
cSetupAssistantConfig::setLineageCreationMethod(int _lineage_creation_method){
  lineage_creation_method = _lineage_creation_method;
}
void
cSetupAssistantConfig::setGenerationIncMethod(int _generation_inc_method){
  generation_inc_method = _generation_inc_method;
}
void
cSetupAssistantConfig::setAgeLimit(int _age_limit){
  age_limit = _age_limit;
}
void
cSetupAssistantConfig::setChildSizeRange(double _child_size_range){
  child_size_range = _child_size_range;
}
void
cSetupAssistantConfig::setMinCopiedLines(double _min_copied_lines){
  min_copied_lines = _min_copied_lines;
}
void
cSetupAssistantConfig::setMinExeLines(double _min_exe_lines){
  min_exe_lines = _min_exe_lines;
}
void
cSetupAssistantConfig::setRequireAllocate(int _require_allocate){
  require_allocate = _require_allocate;
}

void
cSetupAssistantConfig::setRevertFatal(double _revert_fatal){
  revert_fatal = _revert_fatal;
}
void
cSetupAssistantConfig::setRevertNeg(double _revert_neg){
  revert_neg = _revert_neg;
}
void
cSetupAssistantConfig::setRevertNeut(double _revert_neut){
  revert_neut = _revert_neut;
}
void
cSetupAssistantConfig::setRevertPos(double _revert_pos){
  revert_pos = _revert_pos;
}
void
cSetupAssistantConfig::setSterilizeFatal(double _sterilize_fatal){
  sterilize_fatal = _sterilize_fatal;
}
void
cSetupAssistantConfig::setSterilizeNeg(double _sterilize_neg){
  sterilize_neg = _sterilize_neg;
}
void
cSetupAssistantConfig::setSterilizeNeut(double _sterilize_neut){
  sterilize_neut = _sterilize_neut;
}
void
cSetupAssistantConfig::setSterilizePos(double _sterilize_pos){
  sterilize_pos = _sterilize_pos;
}
// watch-out.  call next function only after calling previous eight.
void
cSetupAssistantConfig::setTestOnDivide(void){
  test_on_divide = (revert_fatal > 0.0) || (revert_neg > 0.0) ||
    (revert_neut > 0.0) || (revert_pos > 0.0) || (sterilize_fatal > 0.0) ||
    (sterilize_neg > 0.0) || (sterilize_neut > 0.0) || (sterilize_pos > 0.0);
}
void
cSetupAssistantConfig::setFailImplicit(int _fail_implicit){
  fail_implicit = _fail_implicit;
}

void
cSetupAssistantConfig::setSpeciesThreshold(int _species_threshold){
  species_threshold = _species_threshold;
}
void
cSetupAssistantConfig::setThreshold(int _threshold){
  _threshold = _threshold;
}
void
cSetupAssistantConfig::setGenotypePrint(int _genotype_print){
  genotype_print = _genotype_print;
}
void
cSetupAssistantConfig::setSpeciesPrint(int _species_print){
  species_print = _species_print;
}
void
cSetupAssistantConfig::setSpeciesRecording(int _species_recording){
  species_recording = _species_recording;
}
void
cSetupAssistantConfig::setGenotypePrintDom(int _genotype_print_dom){
  genotype_print_dom = _genotype_print_dom;
}
void
cSetupAssistantConfig::setTestCPUTimeMod(int _test_cpu_time_mod){
  test_cpu_time_mod = _test_cpu_time_mod;
}
void
cSetupAssistantConfig::setTrackMainLineage(int _track_main_lineage){
  track_main_lineage = _track_main_lineage;
}

void
cSetupAssistantConfig::setMaxCPUThreads(int _max_cpu_threads){
  max_cpu_threads = _max_cpu_threads;
}

void
cSetupAssistantConfig::setSlicingMethod(int _slicing_method){
  slicing_method = _slicing_method;
}
void
cSetupAssistantConfig::setSizeMeritMethod(int _size_merit_method){
  size_merit_method = _size_merit_method;
}
void
cSetupAssistantConfig::setBaseSizeMerit(int _base_size_merit){
  base_size_merit = _base_size_merit;
}
void
cSetupAssistantConfig::setAveTimeSlice(int _ave_time_slice){
  ave_time_slice = _ave_time_slice;
}
void
cSetupAssistantConfig::setMeritTime(int _merit_time){
  merit_time = _merit_time;
}

void
cSetupAssistantConfig::setTaskMeritMethod(int _task_merit_method){
  task_merit_method = _task_merit_method;
}
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setMaxNumTasksRewarded(int _max_num_tasks_rewarded){
//  max_num_tasks_rewarded = _max_num_tasks_rewarded;
//}
/**/
void
cSetupAssistantConfig::setMaxLabelExeSize(int _max_label_exe_size){
  max_label_exe_size = _max_label_exe_size;
}

void
cSetupAssistantConfig::setPointMutProb(double _point_mut_prob){
  point_mut_prob = _point_mut_prob;
}
void
cSetupAssistantConfig::setCopyMutProb(double _copy_mut_prob){
  copy_mut_prob = _copy_mut_prob;
}
void
cSetupAssistantConfig::setInsMutProb(double _ins_mut_prob){
  ins_mut_prob = _ins_mut_prob;
}
void
cSetupAssistantConfig::setDelMutProb(double _del_mut_prob){
  del_mut_prob = _del_mut_prob;
}
void
cSetupAssistantConfig::setDivideMutProb(double _divide_mut_prob){
  divide_mut_prob = _divide_mut_prob;
}
void
cSetupAssistantConfig::setDivideInsProb(double _divide_ins_prob){
  divide_ins_prob = _divide_ins_prob;
}
void
cSetupAssistantConfig::setDivideDelProb(double _divide_del_prob){
  divide_del_prob = _divide_del_prob;
}
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setParentMutProb(double _parent_mut_prob){
//  parent_mut_prob = _parent_mut_prob;
//}
/**/
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setCrossoverProb(double _crossover_prob){
//  crossover_prob = _crossover_prob;
//}
/**/
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setAlignedCrossoverProb(double _aligned_cross_prob){
//  aligned_cross_prob = _aligned_cross_prob;
//}
/**/
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setExeErrProb(double _exe_err_prob){
//  exe_err_prob = _exe_err_prob;
//}
/**/

void
cSetupAssistantConfig::setViewMode(int _view_mode){
  view_mode = _view_mode;
}
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setViewerType(int _viewer_type){
//  viewer_type = _viewer_type;
//}

/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setLogBreedCount(int _log_breed_count){
//  log_breed_count = _log_breed_count;
//}
/**/
void
cSetupAssistantConfig::setLogCreatures(int _log_creatures){
  log_creatures = _log_creatures;
}
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setLogPhylogeny(int _log_phylogeny){
//  log_phylogeny = _log_phylogeny;
//}
/**/

void
cSetupAssistantConfig::setLogGenotypes(int _log_genotypes){
  log_genotypes = _log_genotypes;
}
// watch out.  only call after calling setLogGenotypes().
void
cSetupAssistantConfig::setLogThresholdOnly(void){
  log_threshold_only = false;
  if (log_genotypes > 1) log_threshold_only = true;
}
void
cSetupAssistantConfig::setLogThreshold(int _log_threshold){
  log_threshold = _log_threshold;
}
void
cSetupAssistantConfig::setLogSpecies(int _log_species){
  log_species = _log_species;
}
void
cSetupAssistantConfig::setLogLandscape(int _log_landscape){
  log_landscape = _log_landscape;
}
/* FIXME -- removed from base class. -- k */
//void
//cSetupAssistantConfig::setLogMutations(int _log_mutations){
//  log_mutations = _log_mutations;
//}
void
cSetupAssistantConfig::setLogLineages(int _log_lineages){
  log_lineages = _log_lineages;
}

void
cSetupAssistantConfig::setRandomSeed(int _rand_seed){
  rand_seed = _rand_seed;
  cout << "Random Seed: " << rand_seed;
  g_random.ResetSeed(rand_seed);
  if( rand_seed != g_random.GetSeed() ) cout << " -> " << g_random.GetSeed();
  cout << endl;
}

