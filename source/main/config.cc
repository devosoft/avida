//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "config.hh"

#include "defs.hh"
#include "genesis.hh"
#include "tools.hh"

using namespace std;

tList<cConfig::cConfigGroup> cConfig::group_list;
bool cConfig::analyze_mode;
bool cConfig::primitive_mode;
bool cConfig::interactive_analyze;
cString cConfig::default_dir;
cString cConfig::genesis_filename;
cString cConfig::inst_filename;
cString cConfig::event_filename;
cString cConfig::analyze_filename;
cString cConfig::env_filename;
cString cConfig::start_creature;
cString cConfig::clone_filename;
cString cConfig::load_pop_filename;
int cConfig::max_updates;
int cConfig::max_generations;
int cConfig::end_condition_mode;
int cConfig::world_x;
int cConfig::world_y;
int cConfig::world_geometry;
int cConfig::num_demes;
int cConfig::rand_seed;
double cConfig::point_mut_prob;
double cConfig::copy_mut_prob;
double cConfig::ins_mut_prob;
double cConfig::del_mut_prob;
double cConfig::div_mut_prob;
double cConfig::divide_mut_prob;
double cConfig::divide_ins_prob;
double cConfig::divide_del_prob;
double cConfig::parent_mut_prob;
int cConfig::special_mut_line;
double cConfig::recomb_prob;
int cConfig::num_modules;
int cConfig::cont_rec_regs;
int cConfig::corespond_rec_regs;
int cConfig::two_fold_cost_sex;
int cConfig::same_length_sex;
int cConfig::num_instructions;
int cConfig::hardware_type;
int cConfig::max_cpu_threads;
double cConfig::thread_slicing_method;
int cConfig::size_merit_method;
int cConfig::base_size_merit;
double cConfig::default_bonus;
int cConfig::task_merit_method;
int cConfig::max_label_exe_size;
int cConfig::merit_time;
double cConfig::merit_given;
double cConfig::merit_received;
int cConfig::max_donate_kin_distance;
int cConfig::max_donate_edit_distance;
int cConfig::max_donates;
int cConfig::num_tasks;
int cConfig::num_reactions;
int cConfig::num_resources;
int cConfig::slicing_method;
int cConfig::birth_method;
int cConfig::prefer_empty;
int cConfig::death_method;
int cConfig::alloc_method;
int cConfig::divide_method;
int cConfig::required_task;
int cConfig::immunity_task;
int cConfig::required_reaction;
int cConfig::lineage_creation_method;
int cConfig::generation_inc_method;
int cConfig::age_limit;
double cConfig::age_deviation;
double cConfig::child_size_range;
double cConfig::min_copied_lines;
double cConfig::min_exe_lines;
int cConfig::require_allocate;
bool cConfig::test_on_divide;
bool cConfig::test_sterilize;
double cConfig::revert_fatal;
double cConfig::revert_neg;
double cConfig::revert_neut;
double cConfig::revert_pos;
double cConfig::sterilize_fatal;
double cConfig::sterilize_neg;
double cConfig::sterilize_neut;
double cConfig::sterilize_pos;
int cConfig::fail_implicit;
int cConfig::ave_time_slice;
int cConfig::species_threshold;
int cConfig::threshold;
int cConfig::genotype_print;
int cConfig::species_print;
int cConfig::species_recording;
int cConfig::genotype_print_dom;
int cConfig::test_cpu_time_mod;
int cConfig::track_main_lineage;
bool cConfig::log_threshold_only;
bool cConfig::log_creatures;
bool cConfig::log_genotypes;
bool cConfig::log_threshold;
bool cConfig::log_species;
bool cConfig::log_landscape;
bool cConfig::log_lineages;
int cConfig::debug_level;
int cConfig::view_mode;
double cConfig::die_prob;

void cConfig::InitGroupList(){
  // Start with the Architecture variables...
  cConfigGroup * arch_group = new cConfigGroup("Architecture Variables");
  group_list.PushRear(arch_group);
  
  arch_group->Add(max_updates, "-1", "MAX_UPDATES",
                  "Maximum updates to run experiment (-1 = no limit)");
  arch_group->Add(max_generations, "-1", "MAX_GENERATIONS",
                  "Maximum generations to run experiment (-1 = no limit)");
  arch_group->Add(end_condition_mode, "0", "END_CONDITION_MODE",
                  "End run when ...\n0 = MAX_UPDATES _OR_ MAX_GENERATIONS is reached\n1 = MAX_UPDATES _AND_ MAX_GENERATIONS is reached");
  arch_group->Add(world_x, "100", "WORLD-X",
                  "Width of the Avida world");
  arch_group->Add(world_y, "100", "WORLD-Y",
                  "Height of the Avida world");
  arch_group->Add(world_geometry, "2", "WORLD_GEOMETRY",
                  "1 = Bounded Grid\n2 = Torus");
  arch_group->Add(num_demes, "0", "NUM_DEMES",
                  "Number of independed groups in the population; 0=off");
  arch_group->Add(rand_seed, "0", "RANDOM_SEED",
                  "Random number seed (0 for based on time)");
  arch_group->Add(hardware_type, "0", "HARDWARE_TYPE",
                  "0 = Original CPUs\n1 = New, Stack-based CPUs");
  
  // Configuration file group.
  cConfigGroup * file_group = new cConfigGroup("Configuration Files");
  group_list.PushRear(file_group);
  
  file_group->Add(default_dir, DEFAULT_DIR, "DEFAULT_DIR",
                  "Directory in which config files are found");
  file_group->Add(inst_filename, "inst_set.default", "INST_SET",
                  "File containing instruction set");
  file_group->Add(event_filename, "events.cfg", "EVENT_FILE",
                  "File containing list of events during run");
  file_group->Add(analyze_filename, "analyze.cfg", "ANALYZE_FILE",
                  "File used for analysis mode");
  file_group->Add(env_filename, "environment.cfg", "ENVIRONMENT_FILE",
                  "File that describes the environment");
  file_group->Add(start_creature, "organism.default", "START_CREATURE",
                  "Organism to seed the soup");
  
  // Reproduction group.
  cConfigGroup * repro_group = new cConfigGroup("Birth and Death");
  group_list.PushRear(repro_group);
  
  repro_group->Add(birth_method, "4", "BIRTH_METHOD",
                   "0 = Replace random organism in neighborhood\n1 = Replace oldest organism in neighborhood\n2 = Replace largest Age/Merit in neighborhood\n3 = Place only in empty cells in neighborhood\n4 = Replace random from population (Mass Action)\n5 = Replace oldest in entire population (like Tierra)\n6 = Replace random within deme");
  repro_group->Add(prefer_empty, "1", "PREFER_EMPTY",
                   "Are empty cells given preference in offsping placement?");
  repro_group->Add(death_method, "0", "DEATH_METHOD",
                   "0 = Never die of old age.\n1 = Die when inst executed = AGE_LIMIT (+deviation)\n2 = Die when inst executed = length*AGE_LIMIT (+dev)");
  repro_group->Add(age_limit, "5000", "AGE_LIMIT",
                   "Modifies DEATH_METHOD");
  repro_group->Add(age_deviation, "0", "AGE_DEVIATION",
                   "Creates a distribution around AGE_LIMIT");
  repro_group->Add(alloc_method, "0", "ALLOC_METHOD",
                   "0 = Allocated space is set to default instruction.\n1 = Set to section of dead genome (Necrophilia)\n2 = Allocated space is set to random instruction.");
  repro_group->Add(divide_method, "1", "DIVIDE_METHOD",
                   "0 = Divide leaves state of mother untouched.\n1 = Divide resets state of mother (after the divide, we have 2 children)\n2 = Divide resets state of current thread only(does not touch possible parasite threads)");
  repro_group->Add(generation_inc_method, "1", "GENERATION_INC_METHOD",
                   "0 = Only the generation of the child is\n    increased on divide.\n1 = Both the generation of the mother and child are\n    increased on divide (good with DIVIDE_METHOD 1).");
  repro_group->Add(recomb_prob, "1", "RECOMBINATION_PROB",
                   "probability that recombination will happen when div-sex is used");
  repro_group->Add(num_modules, "0", "MODULE_NUM",
                   "number of modules in the genome");
  repro_group->Add(cont_rec_regs, "1", "CONT_REC_REGS",
                   "are (modular) recombination regions continuous");
  repro_group->Add(corespond_rec_regs, "1", "CORESPOND_REC_REGS",
                   "are (modular) recombination regions swapped at random or with corresponding ones, by location");
  repro_group->Add(two_fold_cost_sex, "0", "TWO_FOLD_COST_SEX",
                   "1 = only one recombined offspring is born.\n2 = both offspring are born");
  repro_group->Add(same_length_sex, "0", "SAME_LENGTH_SEX",
                   "0 = recombine with any genome\n1 = only recombine w/ same length");
  
  
  // Divide Restrictions Group.
  cConfigGroup * div_group = new cConfigGroup("Divide Restrictions");
  group_list.PushRear(div_group);
  
  div_group->Add(child_size_range, "2.0", "CHILD_SIZE_RANGE",
                 "Maximal differential between child and parent sizes.");
  div_group->Add(min_copied_lines, "0.5", "MIN_COPIED_LINES",
                 "Code fraction which must be copied before divide.");
  div_group->Add(min_exe_lines, "0.5", "MIN_EXE_LINES",
                 "Code fraction which must be executed before divide.");
  div_group->Add(require_allocate, "1", "REQUIRE_ALLOCATE",
                 "Is a an allocate required before a divide? (0/1)");
  div_group->Add(required_task, "-1", "REQUIRED_TASK",
                 "Task ID required for successful divide.");
  div_group->Add(immunity_task, "-1", "IMMUNITY_TASK",
                 "Task providing immunity from the required task.");
  div_group->Add(required_reaction, "-1", "REQUIRED_REACTION",
                 "Reaction ID required for successful divide.");
  div_group->Add(die_prob, "0", "DIE_PROB",
                 "probability of death when 'die' instruction is executed"); 
  
  
  
  // Mutations Group
  cConfigGroup * muts_group = new cConfigGroup("Mutations");
  group_list.PushRear(muts_group);
  
  muts_group->Add(point_mut_prob, "0.0", "POINT_MUT_PROB",
                  "Mutation rate (per-location per update)");
  muts_group->Add(copy_mut_prob, "0.0075", "COPY_MUT_PROB",
                  "Mutation rate (per copy)");
  muts_group->Add(ins_mut_prob, "0.0", "INS_MUT_PROB",
                  "Insertion rate (per site, applied on divide)");
  muts_group->Add(del_mut_prob, "0.0", "DEL_MUT_PROB",
                  "Deletion rate (per site, applied on divide)");
  muts_group->Add(div_mut_prob, "0.0", "DIV_MUT_PROB",
                  "Mutation rate (per site, applied on divide)");
  muts_group->Add(divide_mut_prob, "0.0", "DIVIDE_MUT_PROB",
                  "Mutation rate (per divide)");
  muts_group->Add(divide_ins_prob, "0.05", "DIVIDE_INS_PROB",
                  "Insertion rate (per divide)");
  muts_group->Add(divide_del_prob, "0.05", "DIVIDE_DEL_PROB",
                  "Deletion rate (per divide)");
  muts_group->Add(parent_mut_prob, "0.0", "PARENT_MUT_PROB",
                  "Per-site, in parent, on divide");
  muts_group->Add(special_mut_line, "-1", "SPECIAL_MUT_LINE",
                  "If this is >= 0, ONLY this line is mutated");
  
  
  // Mutation reversions group
  cConfigGroup * rev_group = new cConfigGroup("Mutation Reversion");
  rev_group->SetComment("These slow down avida a lot, and should be set to 0.0 normally.");
  group_list.PushRear(rev_group);
  
  rev_group->Add(revert_fatal, "0.0", "REVERT_FATAL",
                 "Should any mutations be reverted on birth?");
  rev_group->Add(revert_neg, "0.0", "REVERT_DETRIMENTAL",
                 "  0.0 to 1.0; Probability of reversion.");
  rev_group->Add(revert_neut, "0.0", "REVERT_NEUTRAL",
                 "");
  rev_group->Add(revert_pos, "0.0", "REVERT_BENEFICIAL",
                 "");
  rev_group->Add(sterilize_fatal, "0.0", "STERILIZE_FATAL",
                 "Should any mutations clear (kill) the organism?");
  rev_group->Add(sterilize_neg, "0.0", "STERILIZE_DETRIMENTAL",
                 "  0.0 to 1.0; Probability of reset.");
  rev_group->Add(sterilize_neut, "0.0", "STERILIZE_NEUTRAL",
                 "");
  rev_group->Add(sterilize_pos, "0.0", "STERILIZE_BENEFICIAL",
                 "");
  rev_group->Add(fail_implicit, "0", "FAIL_IMPLICIT",
                 "Should copies that failed *not* due to mutations\nbe eliminated?");
  
  // Time slicing group
  cConfigGroup * time_group = new cConfigGroup("Time Slicing");
  group_list.PushRear(time_group);
  
  time_group->Add(ave_time_slice, "30", "AVE_TIME_SLICE",
                  "Ave number of insts per org per update");
  time_group->Add(slicing_method, "2", "SLICING_METHOD",
                  "0 = CONSTANT: all organisms get default...\n1 = PROBABILISTIC: Run _prob_ proportional to merit.\n2 = INTEGRATED: Perfectly integrated deterministic.");
  time_group->Add(size_merit_method, "0", "SIZE_MERIT_METHOD",
                  "0 = off (merit is independent of size)\n1 = Merit proportional to copied size\n2 = Merit prop. to executed size\n3 = Merit prop. to full size\n4 = Merit prop. to min of executed or copied size\n5 = Merit prop. to sqrt of the minimum size");
  time_group->Add(task_merit_method, "1", "TASK_MERIT_METHOD",
                  "0 = No task bonuses\n1 = Bonus just equals the task bonus");
  time_group->Add(max_cpu_threads, "1", "MAX_CPU_THREADS",
                  "Number of Threads a CPU can spawn");
  time_group->Add(thread_slicing_method, "0", "THREAD_SLICING_METHOD",
                  "Formula for and organism's thread slicing -> 1 + (num_organism_threads-1) * THREAD_SLICING_METHOD.\n0 = One thread executed per time slice.\n1 = All threads executed each time slice.\n");
  time_group->Add(max_label_exe_size, "1", "MAX_LABEL_EXE_SIZE",
                  "Max nops marked as executed when labels are used");
  time_group->Add(base_size_merit, "100", "BASE_SIZE_MERIT",
                  "Base merit when size is *not* used");
  time_group->Add(default_bonus, "1", "DEFAULT_BONUS",
                  "The bonus an organism gets before it has completed any tasks");
  time_group->Add(merit_time, "0", "MERIT_TIME",
                  "0 = Merit Calculated when task completed\n1 = Merit Calculated on Divide");
  time_group->Add(merit_given, "0.0", "MERIT_GIVEN",
                  "Fraction of merit donated with 'donate' command");
  time_group->Add(merit_received, "0.0", "MERIT_RECEIVED",
                  "Multiplier of merit given with 'donate' command");
  time_group->Add(max_donate_kin_distance, "-1", "MAX_DONATE_KIN_DIST",
                  "Limit on distance of relation for donate; -1=no max");
  time_group->Add(max_donate_edit_distance, "-1", "MAX_DONATE_EDIT_DIST",
                  "Limit on genetic (edit) distance for donate; -1=no max");
  time_group->Add(max_donates, "1000000", "MAX_DONATES",
                  "Limit on number of donates organisms are allowed.");
  
  
  // Geneology group
  cConfigGroup * gen_group = new cConfigGroup("Geneology");
  group_list.PushRear(gen_group);
  
  gen_group->Add(track_main_lineage, "0", "TRACK_MAIN_LINEAGE",
                 "Keep all ancestors of the active population?\n0=no, 1=yes, 2=yes,w/sexual population");
  gen_group->Add(threshold, "3", "THRESHOLD",
                 "Number of organisms in a genotype needed for it\n  to be considered viable.");
  gen_group->Add(genotype_print, "0", "GENOTYPE_PRINT",
                 "0/1 (off/on) Print out all threshold genotypes?");
  gen_group->Add(genotype_print_dom, "0", "GENOTYPE_PRINT_DOM",
                 "Print out a genotype if it stays dominant for\n  this many updates. (0 = off)");
  gen_group->Add(species_threshold, "2", "SPECIES_THRESHOLD",
                 "max failure count for organisms to be same species");
  gen_group->Add(species_recording, "0", "SPECIES_RECORDING",
                 "1 = full, 2 = limited search (parent only)");
  gen_group->Add(species_print, "0", "SPECIES_PRINT",
                 "0/1 (off/on) Print out all species?");
  gen_group->Add(test_cpu_time_mod, "20", "TEST_CPU_TIME_MOD",
                 "Time allocated in test CPUs (multiple of length)");
  
  
  // Log Files group
  cConfigGroup * log_group = new cConfigGroup("Log Files");
  group_list.PushRear(log_group);
  
  log_group->Add(log_creatures, "0", "LOG_CREATURES",
                 "0/1 (off/on) toggle to print file.");
  log_group->Add(log_genotypes, "0", "LOG_GENOTYPES",
                 "0 = off, 1 = print ALL, 2 = print threshold ONLY.");
  log_group->Add(log_threshold, "0", "LOG_THRESHOLD",
                 "0/1 (off/on) toggle to print file.");
  log_group->Add(log_species, "0", "LOG_SPECIES",
                 "0/1 (off/on) toggle to print file.");
  log_group->Add(log_landscape, "0", "LOG_LANDSCAPE",
                 "0/1 (off/on) toggle to print file.");
  
  
  // Viewer group
  cConfigGroup * view_group = new cConfigGroup("Viewer Settings");
  group_list.PushRear(view_group);
  
  view_group->Add(view_mode, "0", "VIEW_MODE",
                  "Initial viewer screen");
  
  // Lineages group
  cConfigGroup * lin_group = new cConfigGroup("Lineage");
  lin_group->SetComment("NOTE: This should probably be called \"Clade\"\nThis one can slow down avida a lot. It is used to get an idea of how\noften an advantageous mutation arises, and where it goes afterwards.\nLineage creation options are.  Works only when LOG_LINEAGES is set to 1.\n  0 = manual creation (on inject, use successive integers as lineage labels).\n  1 = when a child's (potential) fitness is higher than that of its parent.\n  2 = when a child's (potential) fitness is higher than max in population.\n  3 = when a child's (potential) fitness is higher than max in dom. lineage\n*and* the child is in the dominant lineage, or (2)\n  4 = when a child's (potential) fitness is higher than max in dom. lineage\n(and that of its own lineage)\n  5 = same as child's (potential) fitness is higher than that of the\n      currently dominant organism, and also than that of any organism\n      currently in the same lineage.\n  6 = when a child's (potential) fitness is higher than any organism\n      currently in the same lineage.\n  7 = when a child's (potential) fitness is higher than that of any\n      organism in its line of descent");
  
  group_list.PushRear(lin_group);
  
  lin_group->Add(log_lineages, "0", "LOG_LINEAGES",
                 "");
  lin_group->Add(lineage_creation_method, "0", "LINEAGE_CREATION_METHOD",
                 "");
  
}

void cConfig::Setup(cGenesis & genesis)
{
  rand_seed = genesis.ReadInt("RANDOM_SEED");
  cout << "Random Seed: " << rand_seed;
  g_random.ResetSeed(rand_seed);
  if( rand_seed != g_random.GetSeed() ) cout << " -> " << g_random.GetSeed();
  cout << endl;
  
  tListIterator<cConfigGroup> group_it(group_list);
  cConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    cur_group->LoadValues(genesis);
  }
  
  /***
    * Handle any special modifications to any of the variables now that
    * they've been loaded.
    ***/
  
  // The default directory should end in a '/'.
  char dir_tail = default_dir[default_dir.GetSize() - 1];
  if (dir_tail != '\\' && dir_tail != '/') default_dir += "/";
  
  // Determine if any variables were set that require test CPUs to be run
  // at every divide.
  test_on_divide = false;
  if ((revert_fatal > 0.0) || (revert_neg > 0.0) || (revert_neut > 0.0) ||
      (revert_pos > 0.0) || (fail_implicit > 0)) {
    test_on_divide = true;
  }
  
  test_sterilize = false;
  if ((sterilize_fatal > 0.0) || (sterilize_neg > 0.0) ||
      (sterilize_neut > 0.0) || (sterilize_pos > 0.0)) {
    test_sterilize = true;
  }
  
  // Determine if we are only logging threshold genotypes...
  log_threshold_only = false;
  if (log_genotypes > 1) log_threshold_only = true;
  
  // Warn if there are settings in the genesis file that have not been read.
  genesis.WarnUnused();
  
  // Test printing... @CAO
  //  PrintGenesis("genesis.test");
}

void cConfig::Setup(int argc, char * argv[])
{
  /***
  * Load all of the variables from genesis.
  ***/
  
  default_dir = DEFAULT_DIR;
  
  cGenesis genesis;
  genesis.SetVerbose();
  ProcessConfiguration(argc, argv, genesis);
  
  Setup(genesis);
}

void cConfig::SetupMS()
{
#ifdef PLATFORM_WINDOWS
  OSVERSIONINFO	osVersion;
  
  SetFileApisToOEM();             // use console character set for files
  memset (&osVersion, 0, sizeof (osVersion));
  osVersion.dwOSVersionInfoSize = sizeof (osVersion);
  GetVersionEx (&osVersion);
  if (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    // Only need to set Code Pages for NT. Default console on Win95 is OEM.
    // In fact, these APIs fail on Win95 w/ rc = 120 (API only works in NT
    //  mode).
    if (!SetConsoleCP (GetOEMCP()))        // and for Console input
      cerr << "Warning: Unable to set input character set, rc="
        << GetLastError() << endl;
    if (!SetConsoleOutputCP (GetOEMCP()))  // and for Console output
      cerr << "Warning: Unable to set output character set, rc="
        << GetLastError() << endl;
  }
#endif
}

void cConfig::PrintGenesis(const cString & filename)
{
  ofstream fp(filename);
  
  // Print out the generic header, including the version ID.
  fp << "#############################################################################" << endl
    << "# This file includes all the basic run-time defines for avida." << endl
    << "# For more information, see doc/genesis.html" << endl
    << "#############################################################################" << endl
    << endl
    << "VERSION_ID " << VERSION << "   # Do not change this value."
    << endl;
  
  // Loop through the groups, and print out all of the variables.
  
  tListIterator<cConfigGroup> group_it(group_list);
  cConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    // Print out the group name...
    fp << endl;
    fp << "### " << cur_group->GetName() << " ###" << endl;
    
    // If we have a comment about the current group, include it.
    for (int i = 0; i < cur_group->GetComment().GetSize(); i++) {
      fp << "# " << cur_group->GetComment().GetLine(i) << endl;
    }
    
    // Print out everything for this group...
    tListIterator<cConfigEntryBase> entry_it(cur_group->GetEntries());
    cConfigEntryBase * cur_entry;
    
    // First, figure out the widest entry so we know where to put comments.
    int max_width = 0;
    while ((cur_entry = entry_it.Next()) != NULL) {
      int cur_width = cur_entry->GetTag().GetSize() +
      cur_entry->GetDefault().GetSize() + 1;
      if (cur_width > max_width) max_width = cur_width;
    }
    
    // Now, make a second pass printing everything.
    entry_it.Reset();
    while ((cur_entry = entry_it.Next()) != NULL) {
      int cur_width = cur_entry->GetTag().GetSize() +
      cur_entry->GetDefault().GetSize() + 1;
      // Print the variable and its setting...
      fp << cur_entry->GetTag() << " " << cur_entry->GetDefault();
      
      // Print some spaces before the description.
      for (int i = cur_width; i < max_width; i++) fp << " ";
      
      // Print the first line of the description.
      if (cur_entry->GetDesc().GetSize() == 0) {
        fp << "  # " << endl;
      } else {
        fp << "  # " << cur_entry->GetDesc().GetLine(0) << endl;
      }
      
      // Print the remaining lines of a description.
      for (int i = 1; i < cur_entry->GetDesc().GetSize(); i++) {
        for (int j = 0; j < max_width; j++) fp << " ";
        fp << "  # " << cur_entry->GetDesc().GetLine(i) << endl;
      }
    }
  }
}

//  void cConfig::PerturbTaskSet(double max_factor)
//  {
//    default_task_lib->Perturb(max_factor);
//  }

//  void cConfig::ChangeTaskSet(double prob_change, double min_bonus,
//  			    double max_bonus)
//  {
//    default_task_lib->Change(prob_change, min_bonus, max_bonus);
//  }


// This function takes in the genesis file and the input arguments, and puts
// out a pointer to a built-up cGenesis object.

// @COW This function depends on the inclusion of the file
// 'vent/cPopulation_descr.ci' for the automatic event documentation.
// If you move the function away from here, move this include as well.


void cConfig::ProcessConfiguration(int argc, char * argv[], cGenesis & genesis)
{
  const cString default_filename = "genesis";
  genesis_filename = default_filename;
  
  int arg_num = 1;              // Argument number being looked at.
  analyze_mode = false;         // Initialize analyze_mode tp be off.
  primitive_mode = false;       // Initialize primitive_mode tp be off.
  interactive_analyze = false;  // Don't start analyze interactively either.
  
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
  
  // If we failed to open the genesis file, and we are using the default,
  // try creating it.
  if (genesis.IsOpen() == false && genesis_filename == default_filename) {
    cerr << "Warning: Unable to find file '" << genesis_filename
    << "'.  Creating." << endl;
    PrintGenesis(genesis_filename);
    genesis.Open(genesis_filename);
  }
  
  cString version_id = genesis.ReadString("VERSION_ID", "Unknown");
  if (genesis.IsOpen() == true && version_id != VERSION) {
    cerr << "/  WARNING   WARNING   WARNING   WARNING   WARNING  \\" << endl
    << "|   Using incorrect genesis file.                   |" << endl
    << "|   Version needed = \"" << VERSION
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
      // DDD - Should print out events here
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
      << "  -i[nteractive]        Run analyze mode interactively."
      // << "  -p[rimitive]          Overide viewer to be primitive."<<endl
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
    } else if (cur_arg == "-interactive" || cur_arg == "-i") {
      analyze_mode = true;
      interactive_analyze = true;
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
      cout << " For more information, see: http://devolab.cse.msu.edu/software/avida/" << endl;
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
  
  delete [] args;
}
