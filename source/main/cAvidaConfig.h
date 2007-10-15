/*
 *  cAvidaConfig.h
 *  Avida
 *
 *  Created by David on 10/16/05.
 *  Designed by Charles.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cAvidaConfig_h
#define cAvidaConfig_h

// This class is designed to easily allow the construction of a dynamic 
// configuration object that will handle the loading and management of all
// variables declared at runtime.  It will also allow for some variables to
// be transparently set at compile time and allow the programmer to easily
// add in new settings with a single line of code.

#include <iostream>

#include "cMutex.h"
#include "cString.h"
#include "cStringList.h"
#include "cStringUtil.h"
#include "tList.h"

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


template <class T> class tDictionary;

using namespace std;


// The following macro will create all of the code needed to include an
// entry for a new setting in the configuration.
//
// To step through what we are doing:
// 1 - Load in four variables representing the setting:
//     NAME = The name of the variable in the config file.
//     TYPE = The type of the variable (int, double, or cString)
//     DEFAULT = Value to use if this isn't set by user.
//     DESC = a brief description of the setting (a string, enclosed in quotes)
// 2 - Build a new class using the setting name with the prefix cEntry_
//     This class will contain all of the info about this setting, and will
//     be derived from the cBaseConfigEntry class so that we may easily refer
//     to any of these dynamically created classes.
// 3 - Create a private value for this setting.
// 4 - Create a LoadString() method to load the settings in from a string.
// 5 - Create a EqualsString() method to determine if the values are the same.
// 6 - Create a constructor that passes all of the information to the base
//     class that it can manage to.
// 7 - Initialize the value of this setting to its default.
// 8 - Insert the newly built object into the full list of settings objects.
// 9 - Create Get() and Set() methods to act as accessors.
// 10 - Setup a method to return the value of this setting as a string.
// 11 - Declare a variable of this class's type to use in the future.  Since
//     accessors were declared above, we can refer to this setting by the
//     setting name inside of config.

#define CONFIG_ADD_VAR(NAME, TYPE, DEFAULT, DESC)                     /* 1 */ \
class cEntry_ ## NAME : public cBaseConfigEntry {                     /* 2 */ \
private:                                                                      \
  TYPE value;                                                         /* 3 */ \
public:                                                                       \
  void LoadString(const cString& str_value) {                         /* 4 */ \
    value = cStringUtil::Convert(str_value, value);                           \
  }                                                                           \
  bool EqualsString(const cString& str_value) const {                 /* 5 */ \
    return (value == cStringUtil::Convert(str_value, value));                 \
  }                                                                           \
  cEntry_ ## NAME() : cBaseConfigEntry(#NAME,#TYPE,#DEFAULT,DESC) {   /* 6 */ \
    LoadString(GetDefault());                                         /* 7 */ \
    global_group_list.GetLast()->AddEntry(this);                      /* 8 */ \
  }                                                                           \
  TYPE Get() const { return value; }                                  /* 9 */ \
  void Set(TYPE in_value) { value = in_value; }                               \
  cString AsString() const { return cStringUtil::Convert(value); }    /* 10 */\
} NAME                                                                /* 11 */\


// Now we're going to make another macro to deal with groups.  This time its
// a bit simpler since there is only one type of group.  The reason that we
// want to make a new class for each new group is so that we can set default
// values for the class with its name and description.  Otherwise we would
// need to send those parameters in elsewhere.

#define CONFIG_ADD_GROUP(NAME, DESC)                                       \
class cGroup_ ## NAME : public cBaseConfigGroup {                          \
public:                                                                    \
  cGroup_ ## NAME() : cBaseConfigGroup(#NAME, DESC) { ; }                  \
} NAME                                                                     \



#define CONFIG_ADD_CUSTOM_FORMAT(NAME, DESC)                               \
class cCustomFormat_ ## NAME : public cBaseConfigCustomFormat {            \
public:                                                                    \
  cCustomFormat_ ## NAME() : cBaseConfigCustomFormat(#NAME, DESC) { ; }    \
} NAME


#define CONFIG_ADD_FORMAT_VAR(NAME, DESC)                                  \
private:                                                                   \
  class cFormatEntry_ ## NAME : public cBaseConfigFormatEntry {            \
  public:                                                                  \
    cFormatEntry_ ## NAME() : cBaseConfigFormatEntry(#NAME, DESC) { ; }    \
  } NAME;                                                                  \
public:                                                                    \


class cAvidaConfig {
#if USE_tMemTrack
  tMemTrack<cAvidaConfig> mt;
#endif
private:
  // The cBaseConfigEntry class is a bass class for all configuration entries.
  // It is used to manage the various types of entries in a dynamic fashion.
  class cBaseConfigEntry {
  private:
    const cString config_name;   // Name of this setting
    const cString type;          // What type does this entry return?
    cString default_value;       // Value to use if not found in config file.
    const cString description;   // Explaination of the use of this setting
    
    // If we automatically regenerate this file to optimize its performace,
    // we can explicitly build some of these classes to return constant 
    // values (not changeable at run time).  Should this instance be one of
    // those classes?
    bool use_overide;
    
  public:
    cBaseConfigEntry(const cString& _name, const cString& _type,
		     const cString& _def, const cString& _desc);
    virtual ~cBaseConfigEntry() { ; }
    
    virtual void LoadString(const cString& str_value) = 0;
    virtual bool EqualsString(const cString& str_value) const = 0;
    
    const cString& GetName() const { return config_name; }
    const cString& GetType() const { return type; }
    const cString& GetDefault() const { return default_value; }
    const cString& GetDesc() const { return description; }
    bool GetUseOveride() const { return use_overide; }
    
    virtual cString AsString() const = 0;
  };
  
  // The cBaseConfigGroup class is a base class for objects that collect the
  // configuration entries into logical groups.
  class cBaseConfigGroup {
  private:
    cString group_name;
    cString description;
    tList<cBaseConfigEntry> entry_list;
  public:
    cBaseConfigGroup(const cString& _name, const cString& _desc)
      : group_name(_name), description(_desc) { global_group_list.PushRear(this); }
    ~cBaseConfigGroup() { ; }
    
    const cString& GetName() const { return group_name; }
    const cString& GetDesc() const { return description; }
    tList<cBaseConfigEntry>& GetEntryList() { return entry_list; }
    const tList<cBaseConfigEntry>& GetEntryList() const { return entry_list; }
    
    void AddEntry(cBaseConfigEntry* _entry) { entry_list.PushRear(_entry); }
  };
  
  // The cConfigCustomFormat class is a class for objects that collect the custom format configuration entries into
  // a single named custom format.
  class cBaseConfigFormatEntry;
  class cBaseConfigCustomFormat {
  private:
    cString m_format_name;
    cString m_description;
    tList<cBaseConfigFormatEntry> m_entry_list;
    cStringList m_value;
  
  public:
    cBaseConfigCustomFormat(const cString& _name, const cString& _desc)
      : m_format_name(_name), m_description(_desc) { global_format_list.PushRear(this); }
    ~cBaseConfigCustomFormat() { ; }
    
    const cString& GetName() const { return m_format_name; }
    const cString& GetDesc() const { return m_description; }
    tList<cBaseConfigFormatEntry>& GetEntryList() { return m_entry_list; }
    const tList<cBaseConfigFormatEntry>& GetEntryList() const { return m_entry_list; }
    
    void AddEntry(cBaseConfigFormatEntry* _entry) { m_entry_list.PushRear(_entry); }
  
    const cStringList& Get() const { return m_value; }
    void Add(const cString& value) { m_value.PushRear(value); }
  };

  // The cConfigFormatEntry class is a bass class for all configuration entries.
  // It is used to manage the various types of entries in a dynamic fashion.
  class cBaseConfigFormatEntry {
  private:
    const cString m_name;   // Name of this setting
    const cString m_description;   // Explaination of the use of this setting
    cBaseConfigCustomFormat* m_format;
    
  public:
    cBaseConfigFormatEntry(const cString& _name, const cString& _desc)
      : m_name(_name), m_description(_desc), m_format(global_format_list.GetLast())
    {
      m_format->AddEntry(this);
    }
    ~cBaseConfigFormatEntry() { ; }
    
    void LoadString(const cString& str_value) { cString lname(m_name); m_format->Add(lname + " " + str_value); }
    
    const cString& GetName() const { return m_name; }
    const cString& GetDesc() const { return m_description; }    
  };
  
  
  // We need to keep track of all configuration groups and the entry objects
  // that they contain.  To do this, we create a global list of groups that
  // all groups must register themselves in.  A new entry object will always
  // use the last group in the list, so we merely need to make sure to keep
  // things in order.  When all configuration objects are built, we then
  // transfer the list to the local cAvidaConfig object (which occurs in the
  // constructor.)
  static cMutex global_list_mutex;
  static tList<cBaseConfigGroup> global_group_list;
  static tList<cBaseConfigCustomFormat> global_format_list;
  tList<cBaseConfigGroup> m_group_list;
  tList<cBaseConfigCustomFormat> m_format_list;


public:
  cAvidaConfig()
  {
    m_group_list.Transfer(global_group_list);
    m_format_list.Transfer(global_format_list);
    global_list_mutex.Unlock();
  }
  ~cAvidaConfig() { ; }

#ifdef OVERRIDE_CONFIG
#include "config_overrides.h"
#else
  // Since the global lists are static and shared across all threads, we need to have a lock on a mutex to prevent multiple
  // concurrent instantiations from messing each other up.  This class should be instantiated first, locking the mutex
  // that is then unlocked at the completion of the constructor.
  class cGlobalListLockAcquire
  {
  public:
    cGlobalListLockAcquire() { global_list_mutex.Lock(); }
  } __ListLock;
  
  CONFIG_ADD_GROUP(GENERAL_GROUP, "General Settings");
  CONFIG_ADD_VAR(ANALYZE_MODE, int, 0, "0 = Disabled\n1 = Enabled\n2 = Interactive");
  CONFIG_ADD_VAR(VIEW_MODE, int, 1, "Initial viewer screen");
  CONFIG_ADD_VAR(CLONE_FILE, cString, "-", "Clone file to load");
  CONFIG_ADD_VAR(VERBOSITY, int, 1, "0 = No output at all\n1=Normal output\n2 = Verbose output, detailing progress\n3 = High level of details, as available\n4 = Print Debug Information, as applicable");
  
  CONFIG_ADD_GROUP(ARCH_GROUP, "Architecture Variables");
  CONFIG_ADD_VAR(WORLD_X, int, 60, "Width of the Avida world");
  CONFIG_ADD_VAR(WORLD_Y, int, 60, "Height of the Avida world");
  CONFIG_ADD_VAR(WORLD_GEOMETRY, int, 2, "1 = Bounded Grid\n2 = Torus\n3 = Clique");
  CONFIG_ADD_VAR(RANDOM_SEED, int, 0, "Random number seed (0 for based on time)");
  CONFIG_ADD_VAR(HARDWARE_TYPE, int, 0, "0 = Original CPUs\n1 = New SMT CPUs\n2 = Transitional SMT\n3 = Experimental CPU\n4 = Gene Expression CPU");
  
  CONFIG_ADD_GROUP(CONFIG_FILE_GROUP, "Configuration Files");
  CONFIG_ADD_VAR(DATA_DIR, cString, "data", "Directory in which config files are found");
  CONFIG_ADD_VAR(INST_SET, cString, "-", "File containing instruction set");
  CONFIG_ADD_VAR(INST_SET_FORMAT, int, 0, "Instruction set file format.\n0 = Default\n1 = New Style");
  CONFIG_ADD_VAR(EVENT_FILE, cString, "events.cfg", "File containing list of events during run");
  CONFIG_ADD_VAR(ANALYZE_FILE, cString, "analyze.cfg", "File used for analysis mode");
  CONFIG_ADD_VAR(ENVIRONMENT_FILE, cString, "environment.cfg", "File that describes the environment");
  CONFIG_ADD_VAR(START_CREATURE, cString, "default-classic.org", "Organism to seed the soup");
  
  CONFIG_ADD_GROUP(DEME_GROUP, "Demes and Germlines");
  CONFIG_ADD_VAR(NUM_DEMES, int, 1, "Number of independent groups in the population.");
  CONFIG_ADD_VAR(DEMES_USE_GERMLINE, int, 0, "Whether demes use a distinct germline; 0=off");
  CONFIG_ADD_VAR(DEMES_HAVE_MERIT, int, 0, "Whether demes have merit; 0=no");
  CONFIG_ADD_VAR(GERMLINE_COPY_MUT, double, 0.0075, "Prob. of copy mutations occuring during\ngermline replication.");
  CONFIG_ADD_VAR(GERMLINE_REPLACES_SOURCE, int, 0, "Whether the source germline is updated\non replication; 0=no.");
  CONFIG_ADD_VAR(GERMLINE_RANDOM_PLACEMENT, int, 0, "Defines how the seed for a germline is placed\n within the deme;\n0 = organisms is placed in center of deme, no orientation\n1 = organisms is placed in center of deme and oriented\n2 = organism is randomly placed in deme, no orientation");
  CONFIG_ADD_VAR(MAX_DEME_AGE, int, 500, "The maximum age of a deme (in updates) to be\nused for age-based replication (default=500).");  
  
  CONFIG_ADD_GROUP(REPRODUCTION_GROUP, "Birth and Death");
  CONFIG_ADD_VAR(BIRTH_METHOD, int, 0, "Which organism should be replaced on birth?\n0 = Random organism in neighborhood\n1 = Oldest in neighborhood\n2 = Largest Age/Merit in neighborhood\n3 = None (use only empty cells in neighborhood)\n4 = Random from population (Mass Action)\n5 = Oldest in entire population\n6 = Random within deme\n7 = Organism faced by parent\n8 = Next grid cell (id+1)\n9 = Largest energy used in entire population\n10 = Largest energy used in neighborhood");
  CONFIG_ADD_VAR(PREFER_EMPTY, int, 1, "Give empty cells preference in offsping placement?");
  CONFIG_ADD_VAR(ALLOW_PARENT, int, 1, "Allow births to replace the parent organism?");
  CONFIG_ADD_VAR(DEATH_METHOD, int, 2, "0 = Never die of old age.\n1 = Die when inst executed = AGE_LIMIT (+deviation)\n2 = Die when inst executed = length*AGE_LIMIT (+dev)");
  CONFIG_ADD_VAR(AGE_LIMIT, int, 20, "Modifies DEATH_METHOD");
  CONFIG_ADD_VAR(AGE_DEVIATION, int, 0, "Creates a distribution around AGE_LIMIT");
  CONFIG_ADD_VAR(ALLOC_METHOD, int, 0, "(Orignal CPU Only)\n0 = Allocated space is set to default instruction.\n1 = Set to section of dead genome (Necrophilia)\n2 = Allocated space is set to random instruction.");
  CONFIG_ADD_VAR(DIVIDE_METHOD, int, 1, "0 = Divide leaves state of mother untouched.\n1 = Divide resets state of mother\n    (after the divide, we have 2 children)\n2 = Divide resets state of current thread only\n    (does not touch possible parasite threads)");
  CONFIG_ADD_VAR(INJECT_METHOD, int, 0, "0 = Leaves the parasite thread state untouched.\n1 = Resets the calling thread state on inject");
  CONFIG_ADD_VAR(GENERATION_INC_METHOD, int, 1, "0 = Only the generation of the child is\n    increased on divide.\n1 = Both the generation of the mother and child are\n    increased on divide (good with DIVIDE_METHOD 1).");
	CONFIG_ADD_VAR(RESET_INPUTS_ON_DIVIDE, int, 0, "Reset environment inputs of parent upon successful divide.");
	
  CONFIG_ADD_GROUP(RECOMBINATION_GROUP, "Sexual Recombination and Modularity");
  CONFIG_ADD_VAR(RECOMBINATION_PROB, double, 1.0, "probability of recombination in div-sex");
  CONFIG_ADD_VAR(MAX_BIRTH_WAIT_TIME, int, -1, "Updates incipiant orgs can wait for crossover");
  CONFIG_ADD_VAR(MODULE_NUM, int, 0, "number of modules in the genome");
  CONFIG_ADD_VAR(CONT_REC_REGS, int, 1, "are (modular) recombination regions continuous");
  CONFIG_ADD_VAR(CORESPOND_REC_REGS, int, 1, "are (modular) recombination regions swapped randomly\n or with corresponding positions?");
  CONFIG_ADD_VAR(TWO_FOLD_COST_SEX, int, 0, "1 = only one recombined offspring is born.\n2 = both offspring are born");
  CONFIG_ADD_VAR(SAME_LENGTH_SEX, int, 0, "0 = recombine with any genome\n1 = only recombine w/ same length");

  CONFIG_ADD_GROUP(DIVIDE_GROUP, "Divide Restrictions");
  CONFIG_ADD_VAR(CHILD_SIZE_RANGE, double, 2.0, "Maximal differential between child and parent sizes.");
  CONFIG_ADD_VAR(MIN_COPIED_LINES, double, 0.5, "Code fraction which must be copied before divide.");
  CONFIG_ADD_VAR(MIN_EXE_LINES, double, 0.5, "Code fraction which must be executed before divide.");
  CONFIG_ADD_VAR(REQUIRE_ALLOCATE, int, 1, "(Original CPU Only) Require allocate before divide?");
  CONFIG_ADD_VAR(REQUIRED_TASK, int, -1, "Task ID required for successful divide.");
  CONFIG_ADD_VAR(IMMUNITY_TASK, int, -1, "Task providing immunity from the required task.");
  CONFIG_ADD_VAR(REQUIRED_REACTION, int, -1, "Reaction ID required for successful divide.");
  CONFIG_ADD_VAR(REQUIRED_BONUS, double, 0.0, "Required bonus to divide.");
  CONFIG_ADD_VAR(IMPLICIT_REPRO_BONUS, int, 0, "Call Inst_Repro to divide upon achieving this bonus. 0 = OFF");  
  CONFIG_ADD_VAR(IMPLICIT_REPRO_CPU_CYCLES, int, 0, "Call Inst_Repro after this many cpu cycles. 0 = OFF");  
  CONFIG_ADD_VAR(IMPLICIT_REPRO_TIME, int, 0, "Call Inst_Repro after this time used. 0 = OFF");  
  CONFIG_ADD_VAR(IMPLICIT_REPRO_END, int, 0, "Call Inst_Repro after executing the last instruction in the genome.");  

  CONFIG_ADD_GROUP(MUTATION_GROUP, "Mutations");
  CONFIG_ADD_VAR(POINT_MUT_PROB, double, 0.0, "Mutation rate (per-location per update)");
  CONFIG_ADD_VAR(COPY_MUT_PROB, double, 0.0075, "Mutation rate (per copy)");
  CONFIG_ADD_VAR(COPY_SLIP_PROB, double, 0.0, "Slip rate (per copy)");
  CONFIG_ADD_VAR(INS_MUT_PROB, double, 0.0, "Insertion rate (per site, applied on divide)");
  CONFIG_ADD_VAR(DEL_MUT_PROB, double, 0.0, "Deletion rate (per site, applied on divide)");
  CONFIG_ADD_VAR(DIV_MUT_PROB, double, 0.0, "Mutation rate (per site, applied on divide)");
  CONFIG_ADD_VAR(UNIFORM_MUT_PROB, double, 0.0, "Uniform mutation probability (per site, applied on divide)\n- Randomly applies any of the three classes of mutations (ins, del, point).");
  CONFIG_ADD_VAR(DIVIDE_MUT_PROB, double, 0.0, "Mutation rate (per divide)");
  CONFIG_ADD_VAR(DIVIDE_INS_PROB, double, 0.05, "Insertion rate (per divide)");
  CONFIG_ADD_VAR(DIVIDE_DEL_PROB, double, 0.05, "Deletion rate (per divide)");
  CONFIG_ADD_VAR(DIVIDE_SLIP_PROB, double, 0.0, "Slip rate (per divide) - creates large deletions/duplications");
  CONFIG_ADD_VAR(SLIP_FILL_MODE, int, 0, "Fill insertions from slip mutations with 0=duplication, 1=nop-X, 2=random, 3=scrambled");
  CONFIG_ADD_VAR(PARENT_MUT_PROB, double, 0.0, "Per-site, in parent, on divide");
  CONFIG_ADD_VAR(SPECIAL_MUT_LINE, int, -1, "If this is >= 0, ONLY this line is mutated");
  CONFIG_ADD_VAR(INJECT_INS_PROB, double, 0.0, "Insertion rate (per site, applied on inject)");
  CONFIG_ADD_VAR(INJECT_DEL_PROB, double, 0.0, "Deletion rate (per site, applied on inject)");
  CONFIG_ADD_VAR(INJECT_MUT_PROB, double, 0.0, "Mutation rate (per site, applied on inject)");
  CONFIG_ADD_VAR(META_COPY_MUT, double, 0.0, "Prob. of copy mutation rate changing (per gen)");
  CONFIG_ADD_VAR(META_STD_DEV, double, 0.0, "Standard deviation of meta mutation size.");
  CONFIG_ADD_VAR(MUT_RATE_SOURCE, int, 1, "1 = Mutation rates determined by environment.\n2 = Mutation rates inherited from parent.");
 
  CONFIG_ADD_GROUP(REVERSION_GROUP, "Mutation Reversion\nThese slow down avida a lot, and should be set to 0.0 normally.");
  CONFIG_ADD_VAR(REVERT_FATAL, double, 0.0, "Should any mutations be reverted on birth?");
  CONFIG_ADD_VAR(REVERT_DETRIMENTAL, double, 0.0, "  0.0 to 1.0; Probability of reversion.");
  CONFIG_ADD_VAR(REVERT_NEUTRAL, double, 0.0, "");
  CONFIG_ADD_VAR(REVERT_BENEFICIAL, double, 0.0, "");
  CONFIG_ADD_VAR(STERILIZE_FATAL, double, 0.0, "Should any mutations clear (kill) the organism?");
  CONFIG_ADD_VAR(STERILIZE_DETRIMENTAL, double, 0.0, "");
  CONFIG_ADD_VAR(STERILIZE_NEUTRAL, double, 0.0, "");
  CONFIG_ADD_VAR(STERILIZE_BENEFICIAL, double, 0.0, "");
  CONFIG_ADD_VAR(FAIL_IMPLICIT, int, 0, "Should copies that failed *not* due to mutations\nbe eliminated?");
  CONFIG_ADD_VAR(NEUTRAL_MAX,double, 0.0, "The percent benifical change from parent fitness\nto be considered neutral.");
  CONFIG_ADD_VAR(NEUTRAL_MIN,double, 0.0, "The percent deleterious change from parent fitness\nto be considered neutral.");
  
  CONFIG_ADD_GROUP(TIME_GROUP, "Time Slicing");
  CONFIG_ADD_VAR(AVE_TIME_SLICE, int, 30, "Ave number of insts per org per update");
  CONFIG_ADD_VAR(SLICING_METHOD, int, 1, "0 = CONSTANT: all organisms get default...\n1 = PROBABILISTIC: Run _prob_ proportional to merit.\n2 = INTEGRATED: Perfectly integrated deterministic.");
  CONFIG_ADD_VAR(BASE_MERIT_METHOD, int, 4, "0 = Constant (merit independent of size)\n1 = Merit proportional to copied size\n2 = Merit prop. to executed size\n3 = Merit prop. to full size\n4 = Merit prop. to min of executed or copied size\n5 = Merit prop. to sqrt of the minimum size\n6 = Merit prop. to num times MERIT_BONUS_INST is in genome.");
  CONFIG_ADD_VAR(BASE_CONST_MERIT, int, 100, "Base merit when BASE_MERIT_METHOD set to 0");
  CONFIG_ADD_VAR(DEFAULT_BONUS, double, 1.0, "Initial bonus before any tasks");
  CONFIG_ADD_VAR(MERIT_DEFAULT_BONUS, int, 0, "Scale the merit of an offspring by this default bonus\nrather than the accumulated bonus of the parent? 0 = off"); 
  CONFIG_ADD_VAR(MERIT_BONUS_INST, int, 0, "in BASE_MERIT_METHOD 6, this sets which instruction counts\n(-1 = none, 0 = First in INST_SET.)"); 
  CONFIG_ADD_VAR(MERIT_BONUS_EFFECT, int, 0, "in BASE_MERIT_METHOD 6, this sets how much merit is earned\nper instruction (-1 = penalty, 0 = no effect.)"); 
  CONFIG_ADD_VAR(FITNESS_METHOD, int, 0, "0 = default, >1 = experimental"); 
  CONFIG_ADD_VAR(FITNESS_COEFF, double, 1.0, "A FITNESS_METHOD parameter");  
  CONFIG_ADD_VAR(FITNESS_VALLEY, int, 0, "in BASE_MERIT_METHOD 6, this creates valleys from\nFITNESS_VALLEY_START to FITNESS_VALLEY_STOP\n(0 = off, 1 = on)"); 
  CONFIG_ADD_VAR(FITNESS_VALLEY_START, int, 0, "if FITNESS_VALLEY = 1, orgs with num_key_instructions\nfrom FITNESS_VALLEY_START to FITNESS_VALLEY_STOP\nget fitness 1 (lowest)"); 
  CONFIG_ADD_VAR(FITNESS_VALLEY_STOP, int, 0, "if FITNESS_VALLEY = 1, orgs with num_key_instructions\nfrom FITNESS_VALLEY_START to FITNESS_VALLEY_STOP\nget fitness 1 (lowest)"); 
  CONFIG_ADD_VAR(MAX_CPU_THREADS, int, 1, "Number of Threads a CPU can spawn");
  CONFIG_ADD_VAR(THREAD_SLICING_METHOD, int, 0, "Formula for and organism's thread slicing\n  (num_threads-1) * THREAD_SLICING_METHOD + 1\n0 = One thread executed per time slice.\n1 = All threads executed each time slice.\n");
  CONFIG_ADD_VAR(NO_CPU_CYCLE_TIME, int, 0, "Don't count each CPU cycle as part of gestation time\n");
  CONFIG_ADD_VAR(MAX_LABEL_EXE_SIZE, int, 1, "Max nops marked as executed when labels are used");
  CONFIG_ADD_VAR(MERIT_GIVEN, double, 0.0, "Fraction of merit donated with 'donate' command");
  CONFIG_ADD_VAR(MERIT_RECEIVED, double, 0.0, "Multiplier of merit given with 'donate' command"); 
  CONFIG_ADD_VAR(MAX_DONATE_KIN_DIST, int, -1, "Limit on distance of relation for donate; -1=no max");
  CONFIG_ADD_VAR(MAX_DONATE_EDIT_DIST, int, -1, "Limit on genetic (edit) distance for donate; -1=no max");
  CONFIG_ADD_VAR(MIN_GB_DONATE_THRESHOLD, int, -1, "threshold green beard donates only to orgs above this\ndonation attempt threshold; -1=no thresh");
  CONFIG_ADD_VAR(DONATE_THRESH_QUANTA, int, 10, "The size of steps between quanta donate thresholds");
  CONFIG_ADD_VAR(MAX_DONATES, int, 1000000, "Limit on number of donates organisms are allowed.");
  CONFIG_ADD_VAR(PRECALC_PHENOTYPE, int, 0, "0 = Disabled\n 1 = Assign precalculated merit at birth (unlimited resources only)\n 2 = Assign precalculated gestation time\n 3 = Assign precalculated merit AND gestation time.\nFitness will be evaluated for organism based on these settings.");

  CONFIG_ADD_GROUP(GENEOLOGY_GROUP, "Geneology");
  CONFIG_ADD_VAR(TRACK_MAIN_LINEAGE, int, 1, "Keep all ancestors of the active population?\n0=no, 1=yes, 2=yes,w/sexual population");
  CONFIG_ADD_VAR(THRESHOLD, int, 3, "Number of organisms in a genotype needed for it\n  to be considered viable.");
  CONFIG_ADD_VAR(GENOTYPE_PRINT, int, 0, "0/1 (off/on) Print out all threshold genotypes?");
  CONFIG_ADD_VAR(GENOTYPE_PRINT_DOM, int, 0, "Print out a genotype if it stays dominant for\n  this many updates. (0 = off)");
  CONFIG_ADD_VAR(SPECIES_THRESHOLD, int, 2, "max failure count for organisms to be same species");
  CONFIG_ADD_VAR(SPECIES_RECORDING, int, 0, "1 = full, 2 = limited search (parent only)");
  CONFIG_ADD_VAR(SPECIES_PRINT, int, 0, "0/1 (off/on) Print out all species?");
  CONFIG_ADD_VAR(TEST_CPU_TIME_MOD, int, 20, "Time allocated in test CPUs (multiple of length)");
  
  CONFIG_ADD_GROUP(LOG_GROUP, "Log Files");
  CONFIG_ADD_VAR(LOG_CREATURES, bool, 0, "0/1 (off/on) toggle to print file.");
  CONFIG_ADD_VAR(LOG_GENOTYPES, int, 0, "0 = off, 1 = print ALL, 2 = print threshold ONLY.");
  CONFIG_ADD_VAR(LOG_THRESHOLD, bool, 0, "0/1 (off/on) toggle to print file.");
  CONFIG_ADD_VAR(LOG_SPECIES, bool, 0, "0/1 (off/on) toggle to print file.");
  
  CONFIG_ADD_GROUP(LINEAGE_GROUP, "Lineage\nNOTE: This should probably be called \"Clade\"\nThis one can slow down avida a lot. It is used to get an idea of how\noften an advantageous mutation arises, and where it goes afterwards.\nLineage creation options are.  Works only when LOG_LINEAGES is set to 1.\n  0 = manual creation (on inject, use successive integers as lineage labels).\n  1 = when a child's (potential) fitness is higher than that of its parent.\n  2 = when a child's (potential) fitness is higher than max in population.\n  3 = when a child's (potential) fitness is higher than max in dom. lineage\n*and* the child is in the dominant lineage, or (2)\n  4 = when a child's (potential) fitness is higher than max in dom. lineage\n(and that of its own lineage)\n  5 = same as child's (potential) fitness is higher than that of the\n      currently dominant organism, and also than that of any organism\n      currently in the same lineage.\n  6 = when a child's (potential) fitness is higher than any organism\n      currently in the same lineage.\n  7 = when a child's (potential) fitness is higher than that of any\n      organism in its line of descent");
  CONFIG_ADD_VAR(LOG_LINEAGES, bool, 0, "");
  CONFIG_ADD_VAR(LINEAGE_CREATION_METHOD, int, 0, "");
  
  CONFIG_ADD_GROUP(ORGANISM_NETWORK_GROUP, "Organism Network Communication");
  CONFIG_ADD_VAR(NET_ENABLED, bool, 0, "Enable Network Communication Support");
  CONFIG_ADD_VAR(NET_DROP_PROB, double, 0.0, "Message drop rate");
  CONFIG_ADD_VAR(NET_MUT_PROB, double, 0.0, "Message corruption probability");
  CONFIG_ADD_VAR(NET_MUT_TYPE, int, 0, "Type of message corruption.  0 = Random Single Bit, 1 = Always Flip Last");
  CONFIG_ADD_VAR(NET_STYLE, int, 0, "Communication Style.  0 = Random Next, 1 = Receiver Facing");

  CONFIG_ADD_GROUP(BUY_SELL_GROUP, "Buying and Selling Parameters");
  CONFIG_ADD_VAR(SAVE_RECEIVED, bool, 0, "Enable storage of all inputs bought from other orgs");
  CONFIG_ADD_VAR(BUY_PRICE, int, 0, "price offered by organisms attempting to buy");
  CONFIG_ADD_VAR(SELL_PRICE, int, 0, "price offered by organisms attempting to sell");
  
  CONFIG_ADD_GROUP(ANALYZE_GROUP, "Analysis Settings");
  CONFIG_ADD_VAR(MAX_CONCURRENCY, int, -1, "Maximum number of analyze threads, -1 == use all available.");
  CONFIG_ADD_VAR(ANALYZE_OPTION_1, cString, "", "String variable accessible from analysis scripts");
  CONFIG_ADD_VAR(ANALYZE_OPTION_2, cString, "", "String variable accessible from analysis scripts");
  
  CONFIG_ADD_GROUP(ENERGY_GROUP, "Energy Settings");
  CONFIG_ADD_VAR(ENERGY_ENABLED, bool, 0, "Enable Energy Model. 0/1 (off/on)");
  CONFIG_ADD_VAR(ENERGY_GIVEN_ON_INJECT, int, 0, "Energy given to organism upon injection.");
  CONFIG_ADD_VAR(ENERGY_GIVEN_AT_BIRTH, int, 0, "Energy given to offspring upon birth.");
  CONFIG_ADD_VAR(FRAC_PARENT_ENERGY_GIVEN_AT_BIRTH, double, 0.5, "Fraction of perent's energy given to offspring.");
  CONFIG_ADD_VAR(FRAC_ENERGY_DECAY_AT_BIRTH, double, 0.0, "Fraction of energy lost due to decay during reproduction.");
  CONFIG_ADD_VAR(NUM_INST_EXC_BEFORE_0_ENERGY, int, 0, "Number of instructions executed before energy is exhausted.");
  CONFIG_ADD_VAR(ENERGY_CAP, int, -1, "Maximum amount of energy that can be stored in an organism.  -1 means the cap is set to Max Int");  // TODO - is this done?
  CONFIG_ADD_VAR(APPLY_ENERGY_METHOD, int, 0, "When should rewarded energy be applied to current energy?\n0 = on divide\n1 = on completion of task\n2 = on sleep");  
  CONFIG_ADD_VAR(FRAC_ENERGY_TRANSFER, double, 0.0, "Fraction of replaced organism's energy take by new resident");
//  CONFIG_ADD_VAR(ENERGY_VERBOSE, bool, 0, "Print energy and merit values. 0/1 (off/on)");
  CONFIG_ADD_VAR(LOG_SLEEP_TIMES, bool, 0, "Log sleep start and end times. 0/1 (off/on)\nWARNING: may use lots of memory.");
  //  CONFIG_ADD_VAR(FIX_METABOLIC_RATE, bool, 0, "When activated the metabolic rate of all orgiansims are equal. 0/1 (off/on)"); // TODO - check for correctness

  CONFIG_ADD_GROUP(SECOND_PASS_GROUP, "Tracking metrics known after the running experiment previously");
  CONFIG_ADD_VAR(TRACK_CCLADES, int, 0, "Enable tracking of coalescence clades");
  CONFIG_ADD_VAR(TRACK_CCLADES_IDS, cString, "coalescence.ids", "File storing coalescence IDs");

  CONFIG_ADD_GROUP(GX_GROUP, "Gene Expression CPU Settings");
  CONFIG_ADD_VAR(MAX_PROGRAMIDS, int, 16, "Maximum number of programids an organism can create.");
  CONFIG_ADD_VAR(MAX_PROGRAMID_AGE, int, 2000, "Max number of CPU cycles a programid executes before it is removed.");
  CONFIG_ADD_VAR(IMPLICIT_GENE_EXPRESSION, int, 0, "Create executable programids from the genome without explicit allocation and copying?");
  CONFIG_ADD_VAR(IMPLICIT_BG_PROMOTER_RATE, double, 0.0, "Relative rate of non-promoter sites creating programids.");
  CONFIG_ADD_VAR(IMPLICIT_TURNOVER_RATE, double, 0.0, "Number of programids recycled per CPU cycle. 0 = OFF");
  CONFIG_ADD_VAR(IMPLICIT_MAX_PROGRAMID_LENGTH, int, 0, "Creation of an executable programid terminates after this many instructions. 0 = disabled");
  //  CONFIG_ADD_VAR(CLEAR_ON_OUTPUT, int, 0, "Reset input buffer every time output called?"); @JEB Not fully implemented

  CONFIG_ADD_GROUP(PROMOTER_GROUP, "Promoters");
  CONFIG_ADD_VAR(PROMOTERS_ENABLED, int, 0, "Use the promoter/terminator execution scheme.\nCertain instructions must also be included.");
  CONFIG_ADD_VAR(PROMOTER_INST_MAX, int, 0, "Maximum number of instructions to execute before terminating. 0 = off");
  CONFIG_ADD_VAR(PROMOTER_PROCESSIVITY, double, 1.0, "Chance of not terminating after each cpu cycle.");
  CONFIG_ADD_VAR(PROMOTER_PROCESSIVITY_INST, double, 1.0, "Chance of not terminating after each instruction.");
  CONFIG_ADD_VAR(PROMOTER_TO_REGISTER, int, 0, "Place a promoter's base bit code in register BX when starting execution from it?");
  CONFIG_ADD_VAR(TERMINATION_RESETS, int, 0, "Does termination reset the thread's state?");
  CONFIG_ADD_VAR(NO_ACTIVE_PROMOTER_EFFECT, int, 0, "What happens when there are no active promoters?\n0 = Start execution at the beginning of the genome.\n1 = Kill the organism.\n2 = Stop the organism from executing any further instructions.");
  CONFIG_ADD_VAR(PROMOTER_CODE_SIZE, int, 24, "Size of a promoter code in bits. (Maximum value is 32)");
  CONFIG_ADD_VAR(PROMOTER_EXE_LENGTH, int, 3, "Length of promoter windows used to determine execution.");
  CONFIG_ADD_VAR(PROMOTER_EXE_THRESHOLD, int, 2, "Minimum number of bits that must be set in a promoter window to allow execution.");
  CONFIG_ADD_VAR(INST_CODE_LENGTH, int, 3, "Instruction binary code length (number of bits)");
  CONFIG_ADD_VAR(INST_CODE_DEFAULT_TYPE, int, 0, "Default value of instruction binary code value.\n0 = All zeros\n1 = Based off the instruction number");
  CONFIG_ADD_VAR(CONSTITUTIVE_REGULATION, int, 0, "Sense a new regulation value before each CPU cycle?");

  CONFIG_ADD_GROUP(COLORS_GROUP, "Output colors for when data files are printed in HTML mode.\nThere are two sets of these; the first are for lineages,\nand the second are for mutation tests.");
  CONFIG_ADD_VAR(COLOR_DIFF, cString, "CCCCFF", "Color to flag stat that has changed since parent.");
  CONFIG_ADD_VAR(COLOR_SAME, cString, "FFFFFF", "Color to flag stat that has NOT changed since parent.");
  CONFIG_ADD_VAR(COLOR_NEG2, cString, "FF0000", "Color to flag stat that is significantly worse than parent.");
  CONFIG_ADD_VAR(COLOR_NEG1, cString, "FFCCCC", "Color to flag stat that is minorly worse than parent.");
  CONFIG_ADD_VAR(COLOR_POS1, cString, "CCFFCC", "Color to flag stat that is minorly better than parent.");
  CONFIG_ADD_VAR(COLOR_POS2, cString, "00FF00", "Color to flag stat that is significantly better than parent.");

  CONFIG_ADD_VAR(COLOR_MUT_POS,    cString, "00FF00", "Color to flag stat that has changed since parent.");
  CONFIG_ADD_VAR(COLOR_MUT_NEUT,   cString, "FFFFFF", "Color to flag stat that has changed since parent.");
  CONFIG_ADD_VAR(COLOR_MUT_NEG,    cString, "FFFF00", "Color to flag stat that has changed since parent.");
  CONFIG_ADD_VAR(COLOR_MUT_LETHAL, cString, "FF0000", "Color to flag stat that has changed since parent.");

  // @WRE: Additions for approaching various features of biological organisms
  CONFIG_ADD_GROUP(BIOMIMETIC_GROUP, "Biomimetic Features Settings");
  CONFIG_ADD_VAR(BIOMIMETIC_REFRACTORY_PERIOD, double, 0.0, "Number of updates affected by refractory period");
  CONFIG_ADD_VAR(BIOMIMETIC_MOVEMENT_STEP, int, 0, "Number of cells to move Avidian on move instruction");
  CONFIG_ADD_VAR(BIOMIMETIC_K, int, 0, "Carrying capacity in number of organisms");
  
  
  CONFIG_ADD_CUSTOM_FORMAT(INST_SET_NEW, "Instruction Set Definition");
  CONFIG_ADD_FORMAT_VAR(INST, "Instruction entry in the instruction set");
  
#endif
  
  void Load(const cString& filename, const bool& crash_if_not_found);
  void Load(const cString& filename, const tDictionary<cString>& mappings, const bool& crash_if_not_found);
  void Print(const cString& filename);
  void Status();
  void PrintReview();
  
  
  bool Get(const cString& entry, cString& ret) const;
  bool Set(const cString& entry, const cString& val);
  void Set(tDictionary<cString>& sets);
  
  void GenerateOverides();
};


#ifdef ENABLE_UNIT_TESTS
namespace nAvidaConfig {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

// Concept:
// Setup #define to build class that will manage the specific variable inside
// of config, as well as any other necessary functions.  Each class must have
// * A Get() method
// * A Set(in_value) method
// * A Load(config file) method
// * Get Name
// * Get Desc
//
// User must provide in the definition:
//  object name
//  genesis ID
//  default value
//  description
//  type?
//  
//
// We must also register the class so that the Load function can be called
// automatically when the config class is built. (as well as name and help
// and such can be looked up as needed.)
//
// This will all be easiest if we have an abstract base class.
//
// If we want to specify constant values (as opposed to using the dynamic
// loading from a file), the #define will be rebuilt as nothing and a separate
// header will be included that defines each class manually.  This will only
// happen


#endif
