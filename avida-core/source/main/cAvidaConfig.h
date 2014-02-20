/*
 *  cAvidaConfig.h
 *  Avida
 *
 *  Created by David on 10/16/05.
 *  Designed by Charles.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cAvidaConfig_h
#define cAvidaConfig_h

// This class is designed to easily allow the construction of a dynamic 
// configuration object that will handle the loading and management of all
// variables declared at runtime.  It will also allow for some variables to
// be transparently set at compile time and allow the programmer to easily
// add in new settings with a single line of code.

#include "apto/core/Mutex.h"

#include <iostream>


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
// 4 - Create a LoadStr() method to load the settings in from a string.
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
  void LoadStr(const cString& str_value) {                         /* 4 */ \
    value = cStringUtil::Convert(str_value, value);                           \
  }                                                                           \
  bool EqualsString(const cString& str_value) const {                 /* 5 */ \
    return (value == cStringUtil::Convert(str_value, value));                 \
  }                                                                           \
  cEntry_ ## NAME() : cBaseConfigEntry(#NAME,#TYPE,#DEFAULT,DESC) {   /* 6 */ \
    LoadStr(GetDefault());                                         /* 7 */ \
    global_group_list.GetLast()->AddEntry(this);                      /* 8 */ \
  }                                                                           \
  TYPE Get() const { return value; }                                  /* 9 */ \
  void Set(TYPE in_value) { value = in_value; }                               \
  cString AsString() const { return cStringUtil::Convert(value); }    /* 10 */\
} NAME                                                                /* 11 */\


// Sometimes we will want to have multiple names associated with a setting.
// This macro allows the programmer to add such an alias to the most recent
// setting created.

#define CONFIG_ADD_ALIAS(ALIAS)                                      \
class cAlias_ ## ALIAS {                                             \
private:                                                             \
  cBaseConfigEntry * primary_entry;                                  \
public:                                                              \
  cAlias_ ## ALIAS() {                                               \
    primary_entry = global_group_list.GetLast()->GetLastEntry();     \
    primary_entry->AddAlias(#ALIAS);				     \
  }                                                                  \
} ALIAS

// Now we're going to make another macro to deal with groups.  This time its
// a bit simpler since there is only one type of group.  The reason that we
// want to make a new class for each new group is so that we can set default
// values for the class with its name and description.  Otherwise we would
// need to send those parameters in elsewhere.

#define CONFIG_ADD_GROUP(NAME, DESC)                                       \
class cGroup_ ## NAME : public cBaseConfigGroup {                          \
public:                                                                    \
  cGroup_ ## NAME() : cBaseConfigGroup(#NAME, DESC) { ; }                  \
} NAME



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
private:
  // The cBaseConfigEntry class is a bass class for all configuration entries.
  // It is used to manage the various types of entries in a dynamic fashion.
  class cBaseConfigEntry {
  private:
    Apto::Array<cString> config_name;  // Names for this setting (first is main name; remainder are aliases)
    const cString type;           // What type does this entry return?
    cString default_value;        // Value to use if not found in config file.
    const cString description;    // Explaination of the use of this setting
    
    // If we automatically regenerate this file to optimize its performace,
    // we can explicitly build some of these classes to return constant 
    // values (not changeable at run time).  Should this instance be one of
    // those classes?
    bool use_overide;
    
  public:
    cBaseConfigEntry(const cString& _name, const cString& _type, const cString& _def, const cString& _desc);
    virtual ~cBaseConfigEntry() { ; }
    
    virtual void LoadStr(const cString& str_value) = 0;
    virtual bool EqualsString(const cString& str_value) const = 0;
    
    const cString& GetName(int id=0) const { return config_name[id]; }
    const Apto::Array<cString>& GetNames() const { return config_name; }
    const cString& GetType() const { return type; }
    const cString& GetDefault() const { return default_value; }
    const cString& GetDesc() const { return description; }
    bool GetUseOveride() const { return use_overide; }
    int GetNumNames() const { return config_name.GetSize(); }

    void AddAlias(const cString & alias) { config_name.Push(alias); }

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
    cBaseConfigEntry * GetLastEntry() { return entry_list.GetLast(); }
    
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
    
    cStringList& Get() { return m_value; }
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
    
    void LoadStr(const cString& str_value) { cString lname(m_name); m_format->Add(lname + " " + str_value); }
    
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
  static Apto::Mutex global_list_mutex;
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
  
  // -------- Configuration File config options --------
  CONFIG_ADD_GROUP(CONFIG_FILE_GROUP, "Other configuration Files");
  CONFIG_ADD_VAR(DATA_DIR, cString, "data", "Directory in which config files are found");
  
  
  // -------- Mutation config options --------
  CONFIG_ADD_GROUP(MUTATION_GROUP, "Mutation rates");  
  CONFIG_ADD_VAR(COPY_MUT_PROB, double, 0.0075, "Substitution rate (per copy)");
  CONFIG_ADD_VAR(COPY_INS_PROB, double, 0.0, "Insertion rate (per copy)");
  CONFIG_ADD_VAR(COPY_DEL_PROB, double, 0.0, "Deletion rate (per copy)");
  CONFIG_ADD_VAR(COPY_UNIFORM_PROB, double, 0.0, "Uniform mutation probability (per copy)\n- Randomly apply insertion, deletion or substition mutation");
  CONFIG_ADD_VAR(COPY_SLIP_PROB, double, 0.0, "Slip rate (per copy)");
  
  CONFIG_ADD_VAR(POINT_MUT_PROB, double, 0.0, "Point (Cosmic-Ray) substitution rate (per-location per update)");
  CONFIG_ADD_VAR(POINT_INS_PROB, double, 0.0, "Point (Cosmic-Ray) insertion rate (per-location per update)");
  CONFIG_ADD_VAR(POINT_DEL_PROB, double, 0.0, "Point (Cosmic-Ray) deletion rate (per-location per update)");
  CONFIG_ADD_VAR(INST_POINT_MUT_PROB, double, 0.0, "Mutation rate (per-location per time instruction (point-mut) is executed)");
  CONFIG_ADD_VAR(INST_POINT_MUT_SLOPE, double, 0.0, "Slope for point mutation rate");
  CONFIG_ADD_VAR(INST_POINT_REPAIR_COST, int, 0, "The cost, in cycles, of avoiding mutations when the point-mut instruction is executed");
  CONFIG_ADD_VAR(POINT_MUT_REPAIR_START, int, 0, "The starting condition for repairs (on=1; off=0)");

  
  CONFIG_ADD_VAR(DIV_MUT_PROB, double, 0.0, "Substitution rate (per site, applied on divide)");
  CONFIG_ADD_VAR(DIV_INS_PROB, double, 0.0, "Insertion rate (per site, applied on divide)");
  CONFIG_ADD_VAR(DIV_DEL_PROB, double, 0.0, "Deletion rate (per site, applied on divide)");
  CONFIG_ADD_VAR(DIV_UNIFORM_PROB, double, 0.0, "Uniform mutation probability (per site, applied on divide)\n- Randomly apply insertion, deletion or point mutation");
  CONFIG_ADD_VAR(DIV_SLIP_PROB, double, 0.0, "Slip rate (per site, applied on divide)");
  CONFIG_ADD_VAR(DIV_TRANS_PROB, double, 0.0, "Translocation rate (per site, applied on divide)");
  
  CONFIG_ADD_VAR(DIVIDE_MUT_PROB, double, 0.0, "Substitution rate (max one, per divide)");
  CONFIG_ADD_VAR(DIVIDE_INS_PROB, double, 0.05, "Insertion rate (max one, per divide)");
  CONFIG_ADD_VAR(DIVIDE_DEL_PROB, double, 0.05, "Deletion rate (max one, per divide)");
  CONFIG_ADD_VAR(DIVIDE_UNIFORM_PROB, double, 0.0, "Uniform mutation probability (per divide)\n- Randomly apply insertion, deletion or point mutation");
  CONFIG_ADD_VAR(DIVIDE_SLIP_PROB, double, 0.0, "Slip rate (per divide) - creates large deletions/duplications");
  CONFIG_ADD_VAR(DIVIDE_TRANS_PROB, double, 0.0, "Translocation rate (per divide) - creates large deletions/duplications");
  
  CONFIG_ADD_VAR(DIVIDE_POISSON_MUT_MEAN, double, 0.0, "Substitution rate (Poisson distributed, per divide)");
  CONFIG_ADD_VAR(DIVIDE_POISSON_INS_MEAN, double, 0.0, "Insertion rate (Poisson distributed, per divide)");
  CONFIG_ADD_VAR(DIVIDE_POISSON_DEL_MEAN, double, 0.0, "Deletion rate (Poisson distributed, per divide)");
  CONFIG_ADD_VAR(DIVIDE_POISSON_SLIP_MEAN, double, 0.0, "Slip rate (Poisson distributed, per divide)");
  CONFIG_ADD_VAR(DIVIDE_POISSON_TRANS_MEAN, double, 0.0, "Translocation rate (Poisson distributed, per divide)");
  
  CONFIG_ADD_VAR(INJECT_MUT_PROB, double, 0.0, "Substitution rate (per site, applied on inject)");
  CONFIG_ADD_VAR(INJECT_INS_PROB, double, 0.0, "Insertion rate (per site, applied on inject)");
  CONFIG_ADD_VAR(INJECT_DEL_PROB, double, 0.0, "Deletion rate (per site, applied on inject)");
  
  CONFIG_ADD_VAR(SLIP_FILL_MODE, int, 0, "Fill insertions from slip mutations with:\n0 = Duplication\n1 = nop-X\n2 = Random\n3 = Scrambled\n4 = nop-C");
  CONFIG_ADD_VAR(SLIP_COPY_MODE, int, 0, "How to handle 'on-copy' slip mutations:\n0 = actual read head slip\n1 = instant large mutation (obeys slip mode)");
  CONFIG_ADD_VAR(TRANS_FILL_MODE, int, 0, "Fill insertions from translocation mutations with:\n0 = Duplication\n1 = Scrambled");
  CONFIG_ADD_VAR(PARENT_MUT_PROB, double, 0.0, "Parent substitution rate (per-site, applied on divide)");
  CONFIG_ADD_VAR(PARENT_INS_PROB, double, 0.0, "Parent insertion rate (per-site, applied on divide)");
  CONFIG_ADD_VAR(PARENT_DEL_PROB, double, 0.0, "Parent deletion rate (per-site, applied on divide)");
  CONFIG_ADD_VAR(SPECIAL_MUT_LINE, int, -1, "If this is >= 0, ONLY this line is mutated");
  CONFIG_ADD_VAR(META_COPY_MUT, double, 0.0, "Prob. of copy mutation rate changing (per gen)");
  CONFIG_ADD_VAR(META_STD_DEV, double, 0.0, "Standard deviation of meta mutation size.");
  CONFIG_ADD_VAR(MUT_RATE_SOURCE, int, 1, "1 = Mutation rates determined by environment.\n2 = Mutation rates inherited from parent.");
  
  
  // -------- Birth and Death config options --------
  CONFIG_ADD_GROUP(REPRODUCTION_GROUP, "Birth and Death config options");
  CONFIG_ADD_VAR(DIVIDE_FAILURE_RESETS, int, 0, "When Divide fails, organisms are interally reset");
  CONFIG_ADD_VAR(BIRTH_METHOD, int, 0, "Which organism should be replaced when a birth occurs?\n0 = Random organism in neighborhood\n1 = Oldest in neighborhood\n2 = Largest Age/Merit in neighborhood\n3 = None (use only empty cells in neighborhood)\n4 = Random from population (Mass Action)\n5 = Oldest in entire population\n7 = Organism faced by parent\n8 = Next grid cell (id+1)\n11 = Local neighborhood dispersal\n12 = Kill offpsring after recording birth stats (for behavioral trials)\n13 = Kill parent and offpsring (for behavioral trials)");
  CONFIG_ADD_VAR(PREFER_EMPTY, int, 1, "Overide BIRTH_METHOD to preferentially choose empty cells for offsping?");
  CONFIG_ADD_VAR(ALLOW_PARENT, int, 1, "Should parents be considered when deciding where to place offspring?");
  CONFIG_ADD_VAR(DISPERSAL_RATE, double, 0.0, "Rate of dispersal under birth method 11\n(poisson distributed random connection list hops)");
  CONFIG_ADD_VAR(DEATH_PROB, double, 0.0, "Probability of death when dividing.");
  CONFIG_ADD_VAR(DEATH_METHOD, int, 2, "When should death by old age occur?\n0 = Never\n1 = When executed AGE_LIMIT (+deviation) total instructions\n2 = When executed genome_length * AGE_LIMIT (+dev) instructions");
  CONFIG_ADD_VAR(AGE_LIMIT, int, 20, "See DEATH_METHOD");
  CONFIG_ADD_VAR(AGE_DEVIATION, int, 0, "Creates a normal distribution around AGE_LIMIT for time of death");
  CONFIG_ADD_VAR(JUV_PERIOD, int, 0, "Number of CPU cycles before newborn orgs can execute various instructions / behaviors");
  CONFIG_ADD_VAR(ALLOC_METHOD, int, 0, "When allocating blank tape, how should it be initialized?\n0 = Allocated space is set to default instruction.\n1 = Set to section of dead genome (creates potential for recombination)\n2 = Allocated space is set to random instruction.");
  CONFIG_ADD_VAR(DIVIDE_METHOD, int, 1, "0 = Divide leaves state of mother untouched.\n1 = Divide resets state of mother(effectively creating 2 offspring)\n2 = Divide resets state of current thread only");
  CONFIG_ADD_VAR(GENERATION_INC_METHOD, int, 1, "0 = Only increase generation of offspring on divide.\n1 = Increase generation of both parent and offspring\n   (suggested with DIVIDE_METHOD 1).");
  CONFIG_ADD_VAR(RESET_INPUTS_ON_DIVIDE, int, 0, "Reset environment inputs of parent upon successful divide.");
  CONFIG_ADD_VAR(INHERIT_MERIT, int, 1, "Should merit be inhereted from mother parent? (in asexual)");
  CONFIG_ADD_VAR(INHERIT_MULTITHREAD, int, 0, "Should offspring of parents with multiple threads be marked multithreaded?");
  CONFIG_ADD_ALIAS(INHERIT_MULTI_THREAD_CLASSIFICATION);
  
	

  // -------- Divide Restrictions config options --------
  CONFIG_ADD_GROUP(DIVIDE_GROUP, "Divide restrictions and triggers - settings describe conditions for a successful divide");
  CONFIG_ADD_VAR(OFFSPRING_SIZE_RANGE, double, 2.0, "Maximal differential between offspring and parent length.\n(Checked BEFORE mutations applied on divide.)");
  CONFIG_ADD_ALIAS(CHILD_SIZE_RANGE);
  CONFIG_ADD_VAR(MIN_COPIED_LINES, double, 0.5, "Code fraction that must be copied before divide");
  CONFIG_ADD_VAR(MIN_EXE_LINES, double, 0.5, "Code fraction that must be executed before divide");
  CONFIG_ADD_VAR(MIN_GENOME_SIZE, int, 0, "Minimum number of instructions allowed in a genome. 0 = OFF");
  CONFIG_ADD_VAR(MAX_GENOME_SIZE, int, 0, "Maximum number of instructions allowed in a genome. 0 = OFF");
  CONFIG_ADD_VAR(MIN_CYCLES, int, 0, "Min number of CPU cycles (age) required before reproduction.");
  CONFIG_ADD_VAR(REQUIRE_ALLOCATE, int, 1, "(Original CPU Only) Require allocate before divide?");
  CONFIG_ADD_VAR(REQUIRED_TASK, int, -1, "Task ID required for successful divide");
  CONFIG_ADD_VAR(IMMUNITY_TASK, int, -1, "Task providing immunity from the required task");
  CONFIG_ADD_VAR(REQUIRED_REACTION, int, -1, "Reaction ID required for successful divide");
  CONFIG_ADD_VAR(IMMUNITY_REACTION, int, -1, "Reaction ID that provides immunity for successful divide");
  CONFIG_ADD_VAR(REQUIRE_SINGLE_REACTION, int, 0, "If set to 1, at least one reaction is required for a successful divide");
  CONFIG_ADD_VAR(REQUIRED_BONUS, double, 0.0, "Required bonus to divide");
  CONFIG_ADD_VAR(REQUIRE_EXACT_COPY, int, 0, "Require offspring to be an exact copy (checked before divide mutations)");
  CONFIG_ADD_VAR(REQUIRED_RESOURCE, int, -1, "ID of resource required in organism's internal bins for successful\n  divide (resource not consumed)");
  CONFIG_ADD_VAR(REQUIRED_RESOURCE_LEVEL, double, 0.0, "Level of resource needed for REQUIRED_RESOURCE");  
  CONFIG_ADD_VAR(IMPLICIT_REPRO_BONUS, int, 0, "Call Inst_Repro to divide upon achieving this bonus. 0 = OFF");
  CONFIG_ADD_VAR(IMPLICIT_REPRO_CPU_CYCLES, int, 0, "Call Inst_Repro after this many cpu cycles. 0 = OFF");  
  CONFIG_ADD_VAR(IMPLICIT_REPRO_TIME, int, 0, "Call Inst_Repro after this time used. 0 = OFF");  
  CONFIG_ADD_VAR(IMPLICIT_REPRO_END, int, 0, "Call Inst_Repro after executing the last instruction in the genome.");  

  // -------- Recombination config options --------
  CONFIG_ADD_GROUP(RECOMBINATION_GROUP, "Sexual Recombination and Modularity");
  CONFIG_ADD_VAR(RECOMBINATION_PROB, double, 1.0, "Probability of recombination in div-sex");
  CONFIG_ADD_VAR(MAX_BIRTH_WAIT_TIME, int, -1, "Updates incipiant orgs can wait for crossover (-1 = unlimited)");
  CONFIG_ADD_VAR(MODULE_NUM, int, 0, "Number of modules in the genome");
  CONFIG_ADD_VAR(CONT_REC_REGS, int, 1, "Are (modular) recombination regions continuous?");
  CONFIG_ADD_VAR(CORESPOND_REC_REGS, int, 1, "Are (modular) recombination regions swapped randomly\n or with corresponding positions?");
  CONFIG_ADD_VAR(TWO_FOLD_COST_SEX, int, 0, "0 = Both offspring are born (no two-fold cost)\n1 = only one recombined offspring is born.");
  CONFIG_ADD_VAR(SAME_LENGTH_SEX, int, 0, "0 = Recombine with any genome\n1 = Recombine only w/ same length");
  CONFIG_ADD_VAR(ALLOW_MATE_SELECTION, bool, 0, "Allow organisms to select mates (requires instruction set support)");

	
  
  // -------- Reversion config options --------
  CONFIG_ADD_GROUP(REVERSION_GROUP, "Mutation Reversion\nMost of these slow down avida a lot, and should be set to 0.0 normally.");
  CONFIG_ADD_VAR(REVERT_FATAL, double, 0.0, "Prob of lethal mutations being reverted on birth");
  CONFIG_ADD_VAR(REVERT_DETRIMENTAL, double, 0.0, "Prob of harmful (but non-lethal) mutations reverting on birth");
  CONFIG_ADD_VAR(REVERT_NEUTRAL, double, 0.0, "Prob of neutral mutations being reverted on birth");
  CONFIG_ADD_VAR(REVERT_BENEFICIAL, double, 0.0, "Prob of beneficial mutations being reverted on birth");
  CONFIG_ADD_VAR(REVERT_TASKLOSS, double, 0.0, "Prob of mutations that cause task loss without any tasks gained being reverted on birth");
  CONFIG_ADD_VAR(REVERT_EQUALS, double, 0.0, "Prob of mutations which grant EQU being reverted on birth");
  CONFIG_ADD_VAR(STERILIZE_FATAL, double, 0.0, "Prob of lethal mutations steralizing an offspring (typically no effect!)");
  CONFIG_ADD_VAR(STERILIZE_DETRIMENTAL, double, 0.0, "Prob of harmful (but non-lethal) mutations sterilizing an offspring");
  CONFIG_ADD_VAR(STERILIZE_NEUTRAL, double, 0.0, "Prob of neutral mutations sterilizing an offspring");
  CONFIG_ADD_VAR(STERILIZE_BENEFICIAL, double, 0.0, "Prob of beneficial mutations sterilizing an offspring");
  CONFIG_ADD_VAR(STERILIZE_TASKLOSS, double, 0.0, "Prob of mutations causing task loss without task gain sterilizing an offspring");
  CONFIG_ADD_VAR(STERILIZE_UNSTABLE, int, 0, "Should genotypes that cannot replicate perfectly not be allowed to replicate?");
  CONFIG_ADD_ALIAS(FAIL_IMPLICIT);
  CONFIG_ADD_VAR(NEUTRAL_MAX,double, 0.0, "Percent benifical change from parent fitness to be considered neutral.");
  CONFIG_ADD_VAR(NEUTRAL_MIN,double, 0.0, "Percent deleterious change from parent fitness to be considered neutral.");

  
  // -------- Time Slicing config options --------
  CONFIG_ADD_GROUP(TIME_GROUP, "Time Slicing");
  CONFIG_ADD_VAR(AVE_TIME_SLICE, int, 30, "Average number of CPU-cycles per org per update");
  CONFIG_ADD_VAR(SLICING_METHOD, int, 1, "0 = CONSTANT: all organisms receive equal number of CPU cycles\n1 = PROBABILISTIC: CPU cycles distributed randomly, proportional to merit.\n2 = INTEGRATED: CPU cycles given out deterministicly, proportional to merit");
  CONFIG_ADD_VAR(BASE_MERIT_METHOD, int, 4, "How should merit be initialized?\n0 = Constant (merit independent of size)\n1 = Merit proportional to copied size\n2 = Merit prop. to executed size\n3 = Merit prop. to full size\n4 = Merit prop. to min of executed or copied size\n5 = Merit prop. to sqrt of the minimum size\n6 = Merit prop. to num times MERIT_BONUS_INST is in genome.");
  CONFIG_ADD_VAR(BASE_CONST_MERIT, int, 100, "Base merit valse for BASE_MERIT_METHOD 0");
  CONFIG_ADD_VAR(MERIT_BONUS_INST, int, 0, "Instruction ID to count for BASE_MERIT_METHOD 6"); 
  CONFIG_ADD_VAR(MERIT_BONUS_EFFECT, int, 0, "Amount of merit earn per instruction for BASE_MERIT_METHOD 6 (-1 = penalty, 0 = no effect)"); 
  CONFIG_ADD_VAR(DEFAULT_BONUS, double, 1.0, "Initial bonus before any tasks");
  CONFIG_ADD_VAR(MERIT_DEFAULT_BONUS, int, 0, "Instead of inheriting bonus from parent, use this value instead (0 = off)"); 
  CONFIG_ADD_VAR(MERIT_INC_APPLY_IMMEDIATE, bool, 0, "Should merit increases (above current) be applied immediately, or delayed until divide?");
  CONFIG_ADD_VAR(TASK_REFRACTORY_PERIOD, double, 0.0, "Number of updates after taske until regain full value");
  CONFIG_ADD_VAR(FITNESS_METHOD, int, 0, "0 = default, 1 = sigmoidal, ");
  CONFIG_ADD_VAR(FITNESS_COEFF_1, double, 1.0, "1st FITNESS_METHOD parameter");
  CONFIG_ADD_VAR(FITNESS_COEFF_2, double, 1.0, "2nd FITNESS_METHOD parameter");
  CONFIG_ADD_VAR(MAX_CPU_THREADS, int, 1, "Maximum number of Threads a CPU can spawn");
  CONFIG_ADD_VAR(THREAD_SLICING_METHOD, int, 0, "Formula for allocating CPU cycles across threads in an organism\n  (num_threads-1) * THREAD_SLICING_METHOD + 1\n0 = One thread executed per time slice.\n1 = All threads executed each time slice.\n");
  CONFIG_ADD_VAR(NO_CPU_CYCLE_TIME, int, 0, "Don't count each CPU cycle as part of gestation time\n");
  CONFIG_ADD_VAR(MAX_LABEL_EXE_SIZE, int, 1, "Max nops marked as executed when labels are used");
  

  // -------- Resource Hoarding (Collect) config options --------
  CONFIG_ADD_GROUP(HOARD_RESOURCE_GROUP, "Resource Hoarding Parameters");
  CONFIG_ADD_VAR(USE_RESOURCE_BINS, bool, 0, "Enable resource bin use.  This serves as a guard on most resource hoarding code.");
  CONFIG_ADD_VAR(ABSORB_RESOURCE_FRACTION, double, .0025, "Fraction of available environmental resource an organism absorbs.");
  CONFIG_ADD_VAR(MULTI_ABSORB_TYPE, int, 0, "What to do if a collect instruction is called on a range of resources.\n 0 = absorb a random resource in the range\n 1 = absorb the first resource in the range\n 2 = absorb the last resource in the range\n 3 = absorb ABSORB_RESOURCE_FRACTION / (# of resources in range) of each resource in the range");
  CONFIG_ADD_VAR(MAX_TOTAL_STORED, double, -1, "Maximum total amount of all resources an organism can store.\n <0 = no maximum");
  CONFIG_ADD_VAR(USE_STORED_FRACTION, double, 1.0, "The fraction of stored resource to use.");
  CONFIG_ADD_VAR(ENV_FRACTION_THRESHOLD, double, 1.0, "The fraction of available environmental resource to compare available stored resource to when deciding whether to use stored resource.");
  CONFIG_ADD_VAR(RETURN_STORED_ON_DEATH, bool, 1, "Return an organism's stored resources to the world when it dies?");
  CONFIG_ADD_VAR(SPLIT_ON_DIVIDE, bool, 1, "Split mother cell's resources between two daughter cells on division?");
  CONFIG_ADD_VAR(COLLECT_SPECIFIC_RESOURCE, int, 0, "Resource to be collected by the \"collect-specific\" instruction.");
  CONFIG_ADD_VAR(RESOURCE_GIVEN_ON_INJECT, double, 0.0, "Units of collect-specific resources given on inject.");  
  CONFIG_ADD_VAR(RESOURCE_GIVEN_AT_BIRTH, double, 0.0, "Units of collect-specific resources given to offspring upon birth (will be added to SPLIT_ON_DIVIDE amount for collect-specific resource if both enabled.");  
  CONFIG_ADD_VAR(COLLECT_PROB_DIVISOR, int, 1, "Divisor for probabilistic collect instructions.");
  
#endif
  
  bool Load(const cString& filename, const cString& working_dir, Feedback* feedback = NULL,
            const Apto::Map<Apto::String, Apto::String>* mappings = NULL, bool warn_default = true);
  void Print(const cString& filename);
  void Status();
  void PrintReview();
  
  
  bool Get(const cString& entry, cString& ret) const;
  bool HasEntry(const cString& entry) const { cString rtn; return Get(entry, rtn); }
  
  bool Set(const cString& entry, const cString& val);
  void Set(Apto::Map<Apto::String, Apto::String>& sets);
  
  void GenerateOverides();
};



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
//  config ID
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
