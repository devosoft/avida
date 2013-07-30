/*
 *  cPhenotype.h
 *  Avida
 *
 *  Called "phenotype.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cPhenotype_h
#define cPhenotype_h

#include "avida/core/InstructionSequence.h"

#include <fstream>

#include "cMerit.h"
#include "cString.h"
#include "cWorld.h"


/*************************************************************************
 *
 * The cPhenotype object contains a general description of all the
 * phenotypic characteristics an organism has displayed.  That is, it
 * monitors all of the organisms behaviors.
 *
 * After a phenotype is created in organism and organism within a population,
 * it must have either SetupOffspring() or SetupInject() run on it to prime
 * it for that population.  After that, it must have DivideReset() run on it
 * whenever it produces an offspring.
 *
 * If the phenotype is part of an organism in a test cpu, no initial priming
 * is required, and SetupTestDivide() needs to be run once it finally has
 * produced an offspring in order to properly lock in any final values.
 *
 * In addition to a reference to the relevent environment, the data
 * contained within this class comes in six flavors:
 *   1. Calculations made at the previous divide.
 *   2. Stats which are accumulated over each gestation cycle.
 *   3. The final result of accumulations over the previous gestation cycle.
 *   4. Accumulations over the entire life of the genome.
 *   5. A collection of flags to indicate the presence of characteristics.
 *   6. Information about the child being constructed.
 *
 *************************************************************************/

class cAvidaContext;
class cContextPhenotype;
class cEnvironment;
template <class T> class tBuffer;
template <class T> class tList;
class cTaskContext;
class cTaskState;
class cReactionResult;

using namespace Avida;


class cPhenotype
{
  friend class cOrganism;
private:
  cWorld* m_world;
  bool initialized;

  // 1. These are values calculated at the last divide (of self or offspring)
  cMerit merit;             // Relative speed of CPU
  double executionRatio;    //  ratio of current execution merit over base execution merit
  int genome_length;        // Number of instructions in genome.
  int bonus_instruction_count; // Number of times MERIT_BONUS_INT is in genome.
  int copied_size;          // Instructions copied into genome.
  int executed_size;        // Instructions executed from genome.
  int gestation_time;       // CPU cycles to produce offspring (or be produced),
                            // including additional time costs of some instructions.
  int gestation_start;      // Total instructions executed at last divide.
  double fitness;           // Relative effective replication rate...
  double div_type;          // Type of the divide command used

  // 2. These are "in progress" variables, updated as the organism operates
  double cur_bonus;                           // Current Bonus
  int cur_num_errors;                         // Total instructions executed illeagally.
  int cur_num_donates;                        // Number of donations so far

  Apto::Array<int> cur_task_count;                 // Total times each task was performed
  Apto::Array<int> cur_para_tasks;                 // Total times each task was performed by the parasite @LZ
  Apto::Array<int> cur_host_tasks;                 // Total times each task was done by JUST the host @LZ
  Apto::Array<int> cur_internal_task_count;        // Total times each task was performed using internal resources
  Apto::Array<int> eff_task_count;                 // Total times each task was performed (resetable during the life of the organism)
  Apto::Array<double> cur_task_quality;            // Average (total?) quality with which each task was performed
  Apto::Array<double> cur_task_value;              // Value with which this phenotype performs task
  Apto::Array<double> cur_internal_task_quality;   // Average (total?) quaility with which each task using internal resources was performed
  Apto::Array<double> cur_rbins_total;             // Total amount of resources collected over the organism's life
  Apto::Array<double> cur_rbins_avail;             // Amount of internal resources available
  Apto::Array<int> cur_collect_spec_counts;        // How many times each nop-specification was used in a collect-type instruction
  Apto::Array<int> cur_reaction_count;             // Total times each reaction was triggered.
  Apto::Array<int> first_reaction_cycles;          // CPU cycles of first time reaction was triggered.
  Apto::Array<int> first_reaction_execs;            // Execution count at first time reaction was triggered (will be > cycles in parallel exec multithreaded orgs).
  Apto::Array<int> cur_stolen_reaction_count;      // Total counts of reactions stolen by predators.
  Apto::Array<double> cur_reaction_add_reward;     // Bonus change from triggering each reaction.
  Apto::Array<int> cur_inst_count;                 // Instruction exection counter
  Apto::Array<int> cur_from_sensor_count;           // Use of inputs that originated from sensory data were used in execution of this instruction.
  Apto::Array< Apto::Array<int> > cur_group_attack_count;
  Apto::Array< Apto::Array<int> > cur_top_pred_group_attack_count;
  Apto::Array<int> cur_killed_targets;
  
  Apto::Array<int> cur_sense_count;                // Total times resource combinations have been sensed; @JEB
  Apto::Array<double> sensed_resources;            // Resources which the organism has sensed; @JEB
  Apto::Array<double> cur_task_time;               // Time at which each task was last performed; WRE 03-18-07
  Apto::Map<void*, cTaskState*> m_task_states;
  Apto::Array<double> cur_trial_fitnesses;         // Fitnesses of various trials.; @JEB
  Apto::Array<double> cur_trial_bonuses;           // Bonuses of various trials.; @JEB
  Apto::Array<int> cur_trial_times_used;           // Time used in of various trials.; @JEB

  int trial_time_used;                        // like time_used, but reset every trial; @JEB
  int trial_cpu_cycles_used;                  // like cpu_cycles_used, but reset every trial; @JEB

  int mating_type;                            // Organism's phenotypic sex @CHC
  int mate_preference;                        // Organism's mating preference @CHC
  
  int cur_mating_display_a;                   // value of organism's current mating display A trait
  int cur_mating_display_b;                   // value of organism's current mating display B trait

  cReactionResult* m_reaction_result;
  


  // 3. These mark the status of "in progress" variables at the last divide.
  double last_merit_base;         // Either constant or based on genome length.
  double last_bonus;
  int last_num_errors;
  int last_num_donates;

  Apto::Array<int> last_task_count;
  Apto::Array<int> last_para_tasks;
  Apto::Array<int> last_host_tasks;                // Last task counts from hosts only, before last divide @LZ
  Apto::Array<int> last_internal_task_count;
  Apto::Array<double> last_task_quality;
  Apto::Array<double> last_task_value;
  Apto::Array<double> last_internal_task_quality;
  Apto::Array<double> last_rbins_total;
  Apto::Array<double> last_rbins_avail;
  Apto::Array<int> last_collect_spec_counts;
  Apto::Array<int> last_reaction_count;
  Apto::Array<double> last_reaction_add_reward;
  Apto::Array<int> last_inst_count;	  // Instruction exection counter
  Apto::Array<int> last_from_sensor_count;
  Apto::Array<int> last_sense_count;   // Total times resource combinations have been sensed; @JEB
  Apto::Array< Apto::Array<int> > last_group_attack_count;
  Apto::Array< Apto::Array<int> > last_top_pred_group_attack_count;
  Apto::Array<int> last_killed_targets;

  double last_fitness;            // Used to determine sterilization.
  int last_cpu_cycles_used;
  
  int last_mating_display_a;                   // value of organism's last mating display A trait
  int last_mating_display_b;                   // value of organism's last mating display B trait
  

  // 4. Records from this organism's life...
  int num_divides_failed; //Number of failed divide events @LZ
  int num_divides;       // Total successful divides organism has produced.
  int generation;        // Number of birth events to original ancestor.
  int cpu_cycles_used;   // Total CPU cycles consumed. @JEB
  int time_used;         // Total CPU cycles consumed, including additional time costs of some instructions.
  int num_execs;         // Total number of instructions executions attempted...accounts for parallel executions in multi-threaded orgs & corrects for cpu-cost 'pauses'
  int age;               // Number of updates organism has survived for.
  double neutral_metric; // Undergoes drift (gausian 0,1) per generation
  double life_fitness; 	 // Organism fitness during its lifetime, 
		         // calculated based on merit just before the divide
  int exec_time_born;    // @MRR number of instructions since seed ancestor start
  double gmu_exec_time_born; //@MRR mutation-rate and gestation time scaled time of birth
  int birth_update;      // @MRR update *organism* born
  int birth_cell_id;
  int av_birth_cell_id;
  int birth_group_id;
  int birth_forager_type;
  Apto::Array<int> testCPU_inst_count;	  // Instruction exection counter as calculated by Test CPU
  int last_task_id; // id of the previous task
  int num_new_unique_reactions; // count the number of new unique reactions this organism has performed.
  double res_consumed; // amount of resources consumed since the organism last turned them over to the ...
  int last_task_time; // time at which the previous task was performed
  
  

  
  // 5. Status Flags...  (updated at each divide)
  bool to_die;		 // Has organism has triggered something fatal?
  bool to_delete;        // Should this organism be deleted when finished?
  bool is_injected;      // Was this organism injected into the population?
  bool is_clone;      // Was this organism created as a clone in the population?
  
  
  bool is_modifier;      // Has this organism modified another?
  bool is_modified;      // Has this organism been modified by another?
  bool is_fertile;       // Do we allow this organisms to produce offspring?
  bool is_mutated;       // Has this organism been subject to any mutations?
  bool is_multi_thread;  // Does this organism have 2 or more threads?
  bool parent_true;      // Is this genome an exact copy of its parent's?
  bool parent_sex;       // Did the parent divide with sex?
  int  parent_cross_num; // How many corssovers did the parent do?
  bool born_parent_group;// Was offspring born into the parent's group?

  // 6. Child information...
  bool copy_true;        // Can this genome produce an exact copy of itself?
  bool divide_sex;       // Was this child created with a sexual divide?
  int mate_select_id;    // If divide sex, who to mate with?
  int  cross_num  ;      // ...how many crossovers should this child do?
  bool child_fertile;    // Will this organism's next child be fertile?
  bool last_child_fertile;  // Was the child being born to be fertile?
  int child_copied_size; // Instruction copied into child.

  // 7. Information that is set once (when organism was born)
  

  inline void SetInstSetSize(int inst_set_size);
  
public:
  cPhenotype(cWorld* world, int parent_generation, int num_nops);


  cPhenotype(const cPhenotype&); 
  cPhenotype& operator=(const cPhenotype&); 
  ~cPhenotype();
  
  void ResetMerit();
  void Sterilize();
  // Run when being setup *as* and offspring.
  void SetupOffspring(const cPhenotype & parent_phenotype, const InstructionSequence & _genome);

  // Run when being setup as an injected organism.
  void SetupInject(const InstructionSequence & _genome);

  // Run when this organism successfully executes a divide.
  void DivideReset(const InstructionSequence & _genome);
  
  // Same as DivideReset(), but only run in test CPUs.
  void TestDivideReset(const InstructionSequence & _genome);

  // Run when an organism is being forced to replicate, but not at the end
  // of its replication cycle.  Assume exact clone with no mutations.
  void SetupClone(const cPhenotype & clone_phenotype);

  // Input and Output Reaction Tests
  bool TestInput(tBuffer<int>& inputs, tBuffer<int>& outputs);
  bool TestOutput(cAvidaContext& ctx, cTaskContext& taskctx,
                  const Apto::Array<double>& res_in, const Apto::Array<double>& rbins_in, Apto::Array<double>& res_change,
                  Apto::Array<cString>& insts_triggered, bool is_parasite=false, cContextPhenotype* context_phenotype = 0);

  // State saving and loading, and printing...
  void PrintStatus(std::ostream& fp) const;

  // Some useful methods...
  int CalcSizeMerit() const;
  double CalcCurrentMerit() const;
  double CalcFitness(double _merit_base, double _bonus, int _gestation_time, int _cpu_cycles) const;

  double CalcFitnessRatio() {
    const int merit_base = CalcSizeMerit();
    const double cur_fitness = merit_base * cur_bonus / time_used;
    return cur_fitness / last_fitness;
  }
  int CalcID() const {
    int phen_id = 0;
    for (int i = 0; i < last_task_count.GetSize(); i++) {
      if (last_task_count[i] > 0) phen_id += (1 << i);
    }
    return phen_id;
  }

  /////////////////////  Accessors -- Retrieving  ////////////////////
  const cMerit & GetMerit() const { assert(initialized == true); return merit; }
  int GetGenomeLength() const { assert(initialized == true); return genome_length; }
  int GetCopiedSize() const { assert(initialized == true); return copied_size; }
  int GetExecutedSize() const { assert(initialized == true); return executed_size; }
  int GetGestationTime() const { assert(initialized == true); return gestation_time; }
  int GetGestationStart() const { assert(initialized == true); return gestation_start; }
  double GetFitness() const { assert(initialized == true); return fitness; }
  double GetDivType() const { assert(initialized == true); return div_type; }

  double GetCurBonus() const { assert(initialized == true); return cur_bonus; }
  int    GetCurBonusInstCount() const { assert(bonus_instruction_count >= 0); return bonus_instruction_count; }

  double GetCurMeritBase() const { assert(initialized == true); return CalcSizeMerit(); }
  
  //@MRR Organism-specific birth tracking
  double GetGMuExecTimeBorn() const {return gmu_exec_time_born;}
  int GetExecTimeBorn() const {return exec_time_born;}
  int GetUpdateBorn() const {return birth_update;}
  
  int GetBirthCell() const { return birth_cell_id; }
  int GetAVBirthCell() const { return av_birth_cell_id; }
  int GetBirthGroupID() const { return birth_group_id; }
  int GetBirthForagerType() const { return birth_forager_type; }
  inline void SetBirthCellID(int birth_cell);
  inline void SetAVBirthCellID(int av_birth_cell);
  inline void SetBirthGroupID(int group_id);
  inline void SetBirthForagerType(int forager_type);

  int GetMatingType() const { return mating_type; } //@CHC
  int GetMatePreference() const { return mate_preference; } //@CHC

  int GetCurMatingDisplayA() const { return cur_mating_display_a; } //@CHC
  int GetCurMatingDisplayB() const { return cur_mating_display_b; } //@CHC
  int GetLastMatingDisplayA() const { return last_mating_display_a; } //@CHC
  int GetLastMatingDisplayB() const { return last_mating_display_b; } //@CHC

  bool GetToDie() const { assert(initialized == true); return to_die; }
  bool GetToDelete() const { assert(initialized == true); return to_delete; }
  int GetCurNumErrors() const { assert(initialized == true); return cur_num_errors; }
  int GetCurNumDonates() const { assert(initialized == true); return cur_num_donates; }
  int GetCurCountForTask(int idx) const { assert(initialized == true); return cur_task_count[idx]; }
  const Apto::Array<int>& GetCurTaskCount() const { assert(initialized == true); return cur_task_count; }
  const Apto::Array<int>& GetCurHostTaskCount() const { assert(initialized == true); return cur_host_tasks; }
  const Apto::Array<int>& GetCurParasiteTaskCount() const { assert(initialized == true); return cur_para_tasks; }
  const Apto::Array<int>& GetCurInternalTaskCount() const { assert(initialized == true); return cur_internal_task_count; }
  void ClearEffTaskCount() { assert(initialized == true); eff_task_count.SetAll(0); }
  const Apto::Array<double> & GetCurTaskQuality() const { assert(initialized == true); return cur_task_quality; }
  const Apto::Array<double> & GetCurTaskValue() const { assert(initialized == true); return cur_task_value; }
  const Apto::Array<double> & GetCurInternalTaskQuality() const { assert(initialized == true); return cur_internal_task_quality; }
  const Apto::Array<double>& GetCurRBinsTotal() const { assert(initialized == true); return cur_rbins_total; }
  double GetCurRBinTotal(int index) const { assert(initialized == true); return cur_rbins_total[index]; }
  const Apto::Array<double>& GetCurRBinsAvail() const { assert(initialized == true); return cur_rbins_avail; }
  double GetCurRBinAvail(int index) const { assert(initialized == true); return cur_rbins_avail[index]; }

  const Apto::Array<int>& GetCurReactionCount() const { assert(initialized == true); return cur_reaction_count;}
  const Apto::Array<int>& GetFirstReactionCycles() const { assert(initialized == true); return first_reaction_cycles;}
  void SetFirstReactionCycle(int idx) { if (first_reaction_cycles[idx] < 0) first_reaction_cycles[idx] = time_used; }
  const Apto::Array<int>& GetFirstReactionExecs() const { assert(initialized == true); return first_reaction_execs;}
  void SetFirstReactionExec(int idx) { if (first_reaction_execs[idx] < 0) first_reaction_execs[idx] = num_execs; }

  const Apto::Array<int>& GetStolenReactionCount() const { assert(initialized == true); return cur_stolen_reaction_count;}
  const Apto::Array<double>& GetCurReactionAddReward() const { assert(initialized == true); return cur_reaction_add_reward;}
  const Apto::Array<int>& GetCurInstCount() const { assert(initialized == true); return cur_inst_count; }
  const Apto::Array<int>& GetCurSenseCount() const { assert(initialized == true); return cur_sense_count; }

  double GetSensedResource(int _in) { assert(initialized == true); return sensed_resources[_in]; }
  const Apto::Array<int>& GetCurCollectSpecCounts() const { assert(initialized == true); return cur_collect_spec_counts; }
  int GetCurCollectSpecCount(int spec_id) const { assert(initialized == true); return cur_collect_spec_counts[spec_id]; }
  const Apto::Array<int>& GetTestCPUInstCount() const { assert(initialized == true); return testCPU_inst_count; }

  void  NewTrial(); //Save the current fitness, and reset the bonus. @JEB
  void  TrialDivideReset(const InstructionSequence & _genome); //Subset of resets specific to division not done by NewTrial. @JEB
  const Apto::Array<double>& GetTrialFitnesses() { return cur_trial_fitnesses; }; //Return list of trial fitnesses. @JEB
  const Apto::Array<double>& GetTrialBonuses() { return cur_trial_bonuses; }; //Return list of trial bonuses. @JEB
  const Apto::Array<int>& GetTrialTimesUsed() { return cur_trial_times_used; }; //Return list of trial times used. @JEB

  tList<int>& GetToleranceImmigrants() { assert(initialized == true); return m_tolerance_immigrants; }
  tList<int>& GetToleranceOffspringOwn() { assert(initialized == true); return m_tolerance_offspring_own; }
  tList<int>& GetToleranceOffspringOthers() { assert(initialized == true); return m_tolerance_offspring_others; }
  Apto::Array<pair<int,int> >& GetIntolerances() { assert(initialized == true); return m_intolerances; }
  int CalcToleranceImmigrants();
  int CalcToleranceOffspringOwn();
  int CalcToleranceOffspringOthers();

  double GetLastMeritBase() const { assert(initialized == true); return last_merit_base; }
  double GetLastBonus() const { assert(initialized == true); return last_bonus; }

  double GetLastMerit() const { assert(initialized == true); return last_merit_base*last_bonus; }
  int GetLastNumErrors() const { assert(initialized == true); return last_num_errors; }
  int GetLastNumDonates() const { assert(initialized == true); return last_num_donates; }

  int GetLastCountForTask(int idx) const { assert(initialized == true); return last_task_count[idx]; }
  const Apto::Array<int>& GetLastTaskCount() const { assert(initialized == true); return last_task_count; }
  void SetLastTaskCount(Apto::Array<int> tasks) { assert(initialized == true); last_task_count = tasks; }
  const Apto::Array<int>& GetLastHostTaskCount() const { assert(initialized == true); return last_host_tasks; }
  const Apto::Array<int>& GetLastParasiteTaskCount() const { assert(initialized == true); return last_para_tasks; }
  void  SetLastParasiteTaskCount(Apto::Array<int>  oldParaPhenotype);
  const Apto::Array<int>& GetLastInternalTaskCount() const { assert(initialized == true); return last_internal_task_count; }
  const Apto::Array<double>& GetLastTaskQuality() const { assert(initialized == true); return last_task_quality; }
  const Apto::Array<double>& GetLastTaskValue() const { assert(initialized == true); return last_task_value; }
  const Apto::Array<double>& GetLastInternalTaskQuality() const { assert(initialized == true); return last_internal_task_quality; }
  const Apto::Array<double>& GetLastRBinsTotal() const { assert(initialized == true); return last_rbins_total; }
  const Apto::Array<double>& GetLastRBinsAvail() const { assert(initialized == true); return last_rbins_avail; }
  const Apto::Array<int>& GetLastReactionCount() const { assert(initialized == true); return last_reaction_count; }
  const Apto::Array<double>& GetLastReactionAddReward() const { assert(initialized == true); return last_reaction_add_reward; }
  const Apto::Array<int>& GetLastInstCount() const { assert(initialized == true); return last_inst_count; }
  const Apto::Array<int>& GetLastFromSensorInstCount() const { assert(initialized == true); return last_from_sensor_count; }
  const Apto::Array<int>& GetLastSenseCount() const { assert(initialized == true); return last_sense_count; }

  double GetLastFitness() const { assert(initialized == true); return last_fitness; }
  const Apto::Array<int>& GetLastCollectSpecCounts() const { assert(initialized == true); return last_collect_spec_counts; }
  int GetLastCollectSpecCount(int spec_id) const { assert(initialized == true); return last_collect_spec_counts[spec_id]; }

  int GetNumDivides() const { assert(initialized == true); return num_divides;}
  int GetNumDivideFailed() const { assert(initialized == true); return num_divides_failed;}

  int GetGeneration() const { return generation; }
  int GetCPUCyclesUsed() const { assert(initialized == true); return cpu_cycles_used; }
  int GetTimeUsed()   const { assert(initialized == true); return time_used; }
  int GetNumExecs() const { assert(initialized == true); return num_execs; }
  int GetTrialTimeUsed()   const { assert(initialized == true); return trial_time_used; }
  int GetAge()        const { assert(initialized == true); return age; }
  double GetNeutralMetric() const { assert(initialized == true); return neutral_metric; }
  double GetLifeFitness() const { assert(initialized == true); return life_fitness; }

  bool IsInjected() const { assert(initialized == true); return is_injected; }
  bool IsClone() const { assert(initialized == true); return is_clone; }
  
  bool IsModifier() const { assert(initialized == true); return is_modifier; }
  bool IsModified() const { assert(initialized == true); return is_modified; }
  bool IsFertile() const  { assert(initialized == true); return is_fertile; }
  bool IsMutated() const  { assert(initialized == true); return is_mutated; }
  bool IsMultiThread() const { assert(initialized == true); return is_multi_thread; }
  bool ParentTrue() const { assert(initialized == true); return parent_true; }
  bool ParentSex() const  { assert(initialized == true); return parent_sex; }
  int  ParentCrossNum() const  { assert(initialized == true); return parent_cross_num; }
  bool BornParentGroup() const { assert(initialized == true); return born_parent_group; } 

  bool CopyTrue() const   { assert(initialized == true); return copy_true; }
  bool DivideSex() const  { assert(initialized == true); return divide_sex; }
  int MateSelectID() const { assert(initialized == true); return mate_select_id; }
  int CrossNum() const  { assert(initialized == true); return cross_num; }
  bool ChildFertile() const { assert(initialized == true); return child_fertile;}
  int GetChildCopiedSize() const { assert(initialized == true); return child_copied_size; }
  


  ////////////////////  Accessors -- Modifying  ///////////////////
  void SetMerit(const cMerit& in_merit) { merit = in_merit; }
  void SetFitness(const double in_fit) { fitness = in_fit; }
  void SetGestationTime(int in_time) { gestation_time = in_time; }
  void SetTimeUsed(int in_time) { time_used = in_time; }
  void SetTrialTimeUsed(int in_time) { trial_time_used = in_time; }
  void SetGeneration(int in_generation) { generation = in_generation; }
  void SetNeutralMetric(double _in){ neutral_metric = _in; }
  void SetLifeFitness(double _in){ life_fitness = _in; }
  void SetLinesExecuted(int _exe_size) { executed_size = _exe_size; }
  void SetLinesCopied(int _copied_size) { child_copied_size = _copied_size; }
  void SetDivType(double _div_type) { div_type = _div_type; }  
  void SetDivideSex(bool _divide_sex) { divide_sex = _divide_sex; }  
  void SetMateSelectID(int _select_id) { mate_select_id = _select_id; }
  void SetCrossNum(int _cross_num) { cross_num = _cross_num; }
  void SetToDie() { to_die = true; }
  void SetToDelete() { to_delete = true; }
  void SetTestCPUInstCount(const Apto::Array<int>& in_counts) { testCPU_inst_count = in_counts; }
  
  void SetReactionCount(int index, int val) { cur_reaction_count[index] = val; }
  void SetStolenReactionCount(int index, int val) { cur_stolen_reaction_count[index] = val; }
  
  void SetCurRBinsAvail(const Apto::Array<double>& in_avail) { cur_rbins_avail = in_avail; }
  void SetCurRbinsTotal(const Apto::Array<double>& in_total) { cur_rbins_total = in_total; }
  void SetCurRBinAvail(int index, double val) { cur_rbins_avail[index] = val; }
  void SetCurRBinTotal(int index, double val) { cur_rbins_total[index] = val; }
  void AddToCurRBinAvail(int index, double val) { cur_rbins_avail[index] += val; }
  void AddToCurRBinTotal(int index, double val) { cur_rbins_total[index] += val; }
  void SetCurCollectSpecCount(int spec_id, int val) { cur_collect_spec_counts[spec_id] = val; }

  void SetMatingType(int _mating_type) { mating_type = _mating_type; } //@CHC
  void SetMatePreference(int _mate_preference) { mate_preference = _mate_preference; } //@CHC

  void SetIsMultiThread() { is_multi_thread = true; }
  bool& SetBornParentGroup() { return born_parent_group; }
  void ClearIsMultiThread() { is_multi_thread = false; }
  
  void SetCurBonus(double _bonus) { cur_bonus = _bonus; }
  void SetCurBonusInstCount(int _num_bonus_inst) {bonus_instruction_count = _num_bonus_inst;}

  void IncCurInstCount(int _inst_num)  { assert(initialized == true); cur_inst_count[_inst_num]++; } 
  void DecCurInstCount(int _inst_num)  { assert(initialized == true); cur_inst_count[_inst_num]--; }
  void IncCurFromSensorInstCount(int _inst_num)  { assert(initialized == true); cur_from_sensor_count[_inst_num]++; }
  void IncAttackedPreyFTData(int target_ft);
  Apto::Array<int> GetKilledPreyFTData() { return cur_killed_targets; }
  
  void IncAge()      { assert(initialized == true); age++; }
  void IncCPUCyclesUsed() { assert(initialized == true); cpu_cycles_used++; trial_cpu_cycles_used++; }
  void DecCPUCyclesUsed() { assert(initialized == true); cpu_cycles_used--; trial_cpu_cycles_used--; }
  void IncTimeUsed(int i=1) { assert(initialized == true); time_used+=i; trial_time_used+=i; }
  void IncNumExecs() { assert(initialized == true); num_execs++; }
  void IncErrors()   { assert(initialized == true); cur_num_errors++; }
  void IncDonates()   { assert(initialized == true); cur_num_donates++; }
  void IncSenseCount(const int) { /*assert(initialized == true); cur_sense_count[i]++;*/ }  
  
  void SetCurMatingDisplayA(int _cur_mating_display_a) { cur_mating_display_a = _cur_mating_display_a; } //@CHC
  void SetCurMatingDisplayB(int _cur_mating_display_b) { cur_mating_display_b = _cur_mating_display_b; } //@CHC
  void SetLastMatingDisplayA(int _last_mating_display_a) { last_mating_display_a = _last_mating_display_a; } //@CHC
  void SetLastMatingDisplayB(int _last_mating_display_b) { last_mating_display_b = _last_mating_display_b; } //@CHC
  
  bool& IsInjected() { assert(initialized == true); return is_injected; }
  bool& IsClone() { assert(initialized == true); return is_clone; }
  bool& IsModifier() { assert(initialized == true); return is_modifier; }
  bool& IsModified() { assert(initialized == true); return is_modified; }
  bool& IsFertile()  { assert(initialized == true); return is_fertile; }
  bool& IsMutated()  { assert(initialized == true); return is_mutated; }
  bool& ParentTrue() { assert(initialized == true); return parent_true; }
  bool& ParentSex()  { assert(initialized == true); return parent_sex; }
  int& ParentCrossNum()  { assert(initialized == true); return parent_cross_num; }
  bool& CopyTrue()   { assert(initialized == true); return copy_true; }
  bool& DivideSex()  { assert(initialized == true); return divide_sex; }
  int& MateSelectID() { assert(initialized == true); return mate_select_id; }
  int& CrossNum()     { assert(initialized == true); return cross_num; }
  bool& ChildFertile() { assert(initialized == true); return child_fertile; }
  bool& IsMultiThread() { assert(initialized == true); return is_multi_thread; }
  
  // --- Support for Division of Labor --- //
  int GetLastTaskID() const { return last_task_id; }
  int  GetNumNewUniqueReactions() const {assert(initialized == true);  return num_new_unique_reactions; }
  void  ResetNumNewUniqueReactions()  {num_new_unique_reactions =0; }
  double GetResourcesConsumed(); 
  Apto::Array<int> GetCumulativeReactionCount();
  

  // @LZ - Parasite Etc. Helpers
  void DivideFailed();
  void UpdateParasiteTasks() { last_para_tasks = cur_para_tasks; cur_para_tasks.SetAll(0); return; }
  

  
  // Compare two phenotypes and determine an ordering (arbitrary, but consistant among phenotypes).
  static int Compare(const cPhenotype* lhs, const cPhenotype* rhs);

  // This pseudo-function is used to help sort phenotypes
  struct PhenotypeCompare {
    bool operator()(const cPhenotype* lhs, const cPhenotype* rhs) const;
  };
};


inline void cPhenotype::SetInstSetSize(int inst_set_size)
{
  cur_inst_count.Resize(inst_set_size, 0);
  cur_from_sensor_count.Resize(inst_set_size, 0);
  last_inst_count.Resize(inst_set_size, 0);
  last_from_sensor_count.Resize(inst_set_size, 0);
}


inline void cPhenotype::SetBirthCellID(int birth_cell) { birth_cell_id = birth_cell; }
inline void cPhenotype::SetAVBirthCellID(int av_birth_cell) { av_birth_cell_id = av_birth_cell; }
inline void cPhenotype::SetBirthGroupID(int group_id) { birth_group_id = group_id; }
inline void cPhenotype::SetBirthForagerType(int forager_type) { birth_forager_type = forager_type; }

#endif
