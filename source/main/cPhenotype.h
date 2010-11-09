/*
 *  cPhenotype.h
 *  Avida
 *
 *  Called "phenotype.hh" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cPhenotype_h
#define cPhenotype_h

#include <fstream>

#ifndef cSequence_h
#include "cSequence.h"
#endif
#ifndef cMerit_h
#include "cMerit.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef cCodeLabel_h
#include "cCodeLabel.h"
#endif
#ifndef tHashMap_h
#include "tHashMap.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif


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
class cEnvironment;
template <class T> class tBuffer;
template <class T> class tList;
class cTaskContext;
class cTaskState;
class cPhenPlastSummary;
class cReactionResult;

class cPhenotype
{
  friend class cOrganism;
private:
  cWorld* m_world;
  bool initialized;

  // 1. These are values calculated at the last divide (of self or offspring)
  cMerit merit;             // Relative speed of CPU
  double executionRatio;    //  ratio of current execution merit over base execution merit
  double energy_store;      // Amount of energy.  Determines relative speed of CPU when turned on.
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
  double cur_energy_bonus;                    // Current energy bonus
  double energy_tobe_applied;                 // Energy that has not yet been added to energy store.
  double energy_testament;
  double energy_received_buffer;              // Energy received through donation, but not yet applied to energy store
  double total_energy_donated;                // Tota amount of energy that has been donated
  double total_energy_received;               // Total amount of energy received through donations
  double total_energy_applied;                // Total amount of received energy applied to energy store
  int num_energy_requests;                    // Number of times organism has requested energy
  int num_energy_donations;                   // Number of times energy has been donated
  int num_energy_receptions;                    // Number of times organism has received energy donations
  int num_energy_applications;                // Number of times organism has applied donated energy to its energy store
  int cur_num_errors;                         // Total instructions executed illeagally.
  int cur_num_donates;                        // Number of donations so far
  tArray<int> cur_task_count;                 // Total times each task was performed
  tArray<int> cur_para_tasks;                 // Total times each task was performed by the parasite @LZ
  tArray<int> cur_host_tasks;                 // Total times each task was done by JUST the host @LZ
  tArray<int> cur_internal_task_count;        // Total times each task was performed using internal resources
  tArray<int> eff_task_count;                 // Total times each task was performed (resetable during the life of the organism)
  tArray<double> cur_task_quality;            // Average (total?) quality with which each task was performed
  tArray<double> cur_task_value;              // Value with which this phenotype performs task
  tArray<double> cur_internal_task_quality;   // Average (total?) quaility with which each task using internal resources was performed
  tArray<double> cur_rbins_total;             // Total amount of resources collected over the organism's life
  tArray<double> cur_rbins_avail;             // Amount of internal resources available
  tArray<int> cur_collect_spec_counts; // How many times each nop-specification was used in a collect-type instruction
  tArray<int> cur_reaction_count;             // Total times each reaction was triggered.  
  tArray<double> cur_reaction_add_reward;     // Bonus change from triggering each reaction.
  tArray<int> cur_inst_count;                 // Instruction exection counter
  tArray<int> cur_sense_count;                // Total times resource combinations have been sensed; @JEB 
  tArray<double> sensed_resources;            // Resources which the organism has sensed; @JEB 
  tArray<double> cur_task_time;    // Time at which each task was last performed; WRE 03-18-07
  tHashMap<void*, cTaskState*> m_task_states;
  tArray<double> cur_trial_fitnesses;         // Fitnesses of various trials.; @JEB
  tArray<double> cur_trial_bonuses;           // Bonuses of various trials.; @JEB
  tArray<int> cur_trial_times_used;           // Time used in of various trials.; @JEB
  int trial_time_used;                        // like time_used, but reset every trial; @JEB
  int trial_cpu_cycles_used;                  // like cpu_cycles_used, but reset every trial; @JEB
  double last_child_germline_propensity;   // chance of child being a germline cell; @JEB

  cReactionResult* m_reaction_result;
  


  // 3. These mark the status of "in progess" variables at the last divide.
  double last_merit_base;         // Either constant or based on genome length.
  double last_bonus;
  double last_energy_bonus;
  int last_num_errors;
  int last_num_donates;
  tArray<int> last_task_count;
  tArray<int> last_para_tasks;
  tArray<int> last_host_tasks;                // Last task counts from hosts only, before last divide @LZ
  tArray<int> last_internal_task_count;
  tArray<double> last_task_quality;
  tArray<double> last_task_value;
  tArray<double> last_internal_task_quality;
  tArray<double> last_rbins_total;
  tArray<double> last_rbins_avail;
  tArray<int> last_collect_spec_counts;
  tArray<int> last_reaction_count;
  tArray<double> last_reaction_add_reward; 
  tArray<int> last_inst_count;	  // Instruction exection counter
  tArray<int> last_sense_count;   // Total times resource combinations have been sensed; @JEB 
  double last_fitness;            // Used to determine sterilization.
  int last_cpu_cycles_used;
  double cur_child_germline_propensity;   // chance of child being a germline cell; @JEB
  
  

  // 4. Records from this organism's life...
  int num_divides_failed; //Number of failed divide events @LZ
  int num_divides;       // Total successful divides organism has produced.
  int generation;        // Number of birth events to original ancestor.
  int cpu_cycles_used;   // Total CPU cycles consumed. @JEB
  int time_used;         // Total CPU cycles consumed, including additional time costs of some instructions.
  int age;               // Number of updates organism has survived for.
  cString fault_desc;    // A description of the most recent error.
  double neutral_metric; // Undergoes drift (gausian 0,1) per generation
  double life_fitness; 	 // Organism fitness during its lifetime, 
		         // calculated based on merit just before the divide
  int exec_time_born;    // @MRR number of instructions since seed ancestor start
  double gmu_exec_time_born; //@MRR mutation-rate and gestation time scaled time of birth
  int birth_update;      // @MRR update *organism* born
	tArray<int> testCPU_inst_count;	  // Instruction exection counter as calculated by Test CPU
	int last_task_id; // id of the previous task
	int num_new_unique_reactions; // count the number of new unique reactions this organism has performed.
	double res_consumed; // amount of resources consumed since the organism last turned them over to the deme.

  
  // 5. Status Flags...  (updated at each divide)
  bool to_die;		 // Has organism has triggered something fatal?
  bool to_delete;        // Should this organism be deleted when finished?
  bool is_injected;      // Was this organism injected into the population?
  bool is_donor_cur;     // Has this organism attempted to donate merit?  
  bool is_donor_last;    // Did this organism's parent attempt to donate merit? 
  bool is_donor_rand;    // Has this organism attempted a random donation?
  bool is_donor_rand_last; // Did this org's parent attempt to donate randomly
  bool is_donor_null;    // Has this organism attempted a null donation?
  bool is_donor_null_last;// Did this org's parent attempt a null donation?
  bool is_donor_kin;     // Has this organism kin_donated?
  bool is_donor_kin_last;// Did this org's parent kin_donate?
  bool is_donor_edit;    // Has this organism edit_donated?
  bool is_donor_edit_last; // Did this org's parent edit_donate?
  bool is_donor_gbg;     //  Has this organism gbg_donated (green beard gene)?
  bool is_donor_gbg_last;// Did this org's parent gbg_donate?
  bool is_donor_truegb;  // Has this organism truegb_donated (true green beard)? 
  bool is_donor_truegb_last;// Did this org's parent truegb_donate? 
  bool is_donor_threshgb;  // Has this organism threshgb_donated (true green beard)? 
  bool is_donor_threshgb_last;// Did this org's parent threshgbg_donate? 
  bool is_donor_quanta_threshgb;  // Has this organism quanta_threshgb_donated (true green beard)? 
  bool is_donor_quanta_threshgb_last;// Did this org's parent quanta_threshgbg_donate?
  bool is_donor_shadedgb; // Has this organism shaded_gb_donated (true shaded green beard)? 
  bool is_donor_shadedgb_last; // Did this org's parent shaded_gb_donate? 
  tArray<bool> is_donor_locus; // Did this org target a donation at a specific locus.
  tArray<bool> is_donor_locus_last; // Did this org's parent target a donation at a specific locus.
  bool is_energy_requestor; // Has this organism requested energy?
  bool is_energy_donor; // Has this organism donated energy?
  bool is_energy_receiver;  // Has this organism received an energy donation?
  bool has_used_donated_energy; // Has the organism actively used an energy donation?
  bool has_open_energy_request; // Does the organism have an energy request that hasn't been answered?
  int num_thresh_gb_donations;  // Num times this organism threshgb_donated (thresh green beard)? 
  int num_thresh_gb_donations_last; // Num times this org's parent thresh_donated? 
  int num_quanta_thresh_gb_donations;  // Num times this organism threshgb_donated (thresh green beard)? 
  int num_quanta_thresh_gb_donations_last; // Num times this org's parent thresh_donated? 
  int num_shaded_gb_donations; // Num times this org shaded_gb_donated? 
  int num_shaded_gb_donations_last; // Num times this org's parent shaded_gb_donated?
  int num_donations_locus; // Num times this org targeted a donation to a position.
  int num_donations_locus_last; // Num times this org's parent targeted a donation to a position.
  bool is_receiver;      // Has this organism ever received merit donation?
  bool is_receiver_last;      // Did this organism's parent receive a merit donation?
  bool is_receiver_rand; // Has this organism ever received random merit donation?
  bool is_receiver_kin;  // Has this organism ever received kin merit donation?
  bool is_receiver_kin_last;  // Did this organism's parent receive a kin merit donation?
  bool is_receiver_edit; // Has this organism ever received edit donation?
  bool is_receiver_edit_last; // Did this organism's parent receive an edit donation?
  bool is_receiver_gbg;  // Has this organism ever received gbg donation?
  bool is_receiver_truegb;// Has this organism ever received truegb donation?
  bool is_receiver_truegb_last;// Did this organism's parent receive a truegb donation?
  bool is_receiver_threshgb;// Has this organism ever received a threshgb donation?
  bool is_receiver_threshgb_last;// Did this organism's parent receive a threshgb donation?
  bool is_receiver_quanta_threshgb;// Has this organism ever received a quanta_threshgb donation?
  bool is_receiver_quanta_threshgb_last;// Did this organism's parent receive a quanta_threshgb donation?
  bool is_receiver_shadedgb; // Has this organism ever received a shaded_gb donation? 
  bool is_receiver_shadedgb_last; // Did this organism's parent receive a shaded gb donation?
  bool is_receiver_gb_same_locus; // Has this org ever received a donation for a specific locus.
  bool is_receiver_gb_same_locus_last; // Did this org's parent ever received a donation for a specific locus.
  bool is_modifier;      // Has this organism modified another?
  bool is_modified;      // Has this organism been modified by another?
  bool is_fertile;       // Do we allow this organisms to produce offspring?
  bool is_mutated;       // Has this organism been subject to any mutations?
  bool is_multi_thread;  // Does this organism have 2 or more threads?
  bool parent_true;      // Is this genome an exact copy of its parent's?
  bool parent_sex;       // Did the parent divide with sex?
  int  parent_cross_num; // How many corssovers did the parent do? 

  // 6. Child information...
  bool copy_true;        // Can this genome produce an exact copy of itself?
  bool divide_sex;       // Was this child created with a sexual divide?
  int mate_select_id;    // If divide sex, who to mate with?
  int  cross_num  ;      // ...how many crossovers should this child do?
  bool child_fertile;    // Will this organism's next child be fertile?
  bool last_child_fertile;  // Was the child being born to be fertile?
  int child_copied_size; // Instruction copied into child.

  // 7. Information that is set once (when organism was born)
  double permanent_germline_propensity;
  

  inline void SetInstSetSize(int inst_set_size);

  
public:
  cPhenotype() : m_world(NULL), m_reaction_result(NULL) { ; } // Will not construct a valid cPhenotype! Only exists to support incorrect cDeme tArray usage.
  cPhenotype(cWorld* world, int parent_generation, int num_nops);


  cPhenotype(const cPhenotype&); 
  cPhenotype& operator=(const cPhenotype&); 
  ~cPhenotype();
  
  enum energy_levels {ENERGY_LEVEL_LOW = 0, ENERGY_LEVEL_MEDIUM, ENERGY_LEVEL_HIGH};
	
  bool OK();

  void ResetMerit(const cSequence & _cgenome);
  void Sterilize();
  // Run when being setup *as* and offspring.
  void SetupOffspring(const cPhenotype & parent_phenotype, const cSequence & _genome);

  // Run when being setup as an injected organism.
  void SetupInject(const cSequence & _genome);

  // Run when this organism successfully executes a divide.
  void DivideReset(const cSequence & _genome);
  
  // Same as DivideReset(), but only run in test CPUs.
  void TestDivideReset(const cSequence & _genome);

  // Run when an organism is being forced to replicate, but not at the end
  // of its replication cycle.  Assume exact clone with no mutations.
  void SetupClone(const cPhenotype & clone_phenotype);

  // Input and Output Reaction Tests
  bool TestInput(tBuffer<int>& inputs, tBuffer<int>& outputs);
  bool TestOutput(cAvidaContext& ctx, cTaskContext& taskctx,
                  const tArray<double>& res_in, const tArray<double>& rbins_in, tArray<double>& res_change,
                  tArray<cString>& insts_triggered, bool is_parasite=false);

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
  double GetEnergyUsageRatio() const { assert(initialized == true); return executionRatio; }
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
  double GetStoredEnergy() const { return energy_store; }
  double GetEnergyBonus() const { assert(initialized == true); return cur_energy_bonus; }
  int GetDiscreteEnergyLevel() const;
  double GetEnergyInBufferAmount() const { return energy_received_buffer; }
  
  double ConvertEnergyToMerit(double energy) const;
  
  //@MRR Organism-specific birth tracking
  double GetGMuExecTimeBorn() const {return gmu_exec_time_born;}
  int GetExecTimeBorn() const {return exec_time_born;}
  int GetUpdateBorn() const {return birth_update;}
  
  
  
  bool GetToDie() const { assert(initialized == true); return to_die; }
  bool GetToDelete() const { assert(initialized == true); return to_delete; }
  int GetCurNumErrors() const { assert(initialized == true); return cur_num_errors; }
  int GetCurNumDonates() const { assert(initialized == true); return cur_num_donates; }
  const tArray<int>& GetCurTaskCount() const { assert(initialized == true); return cur_task_count; }
  const tArray<int>& GetCurHostTaskCount() const { assert(initialized == true); return cur_host_tasks; }
  const tArray<int>& GetCurParasiteTaskCount() const { assert(initialized == true); return cur_para_tasks; }
  const tArray<int>& GetCurInternalTaskCount() const { assert(initialized == true); return cur_internal_task_count; }
  void ClearEffTaskCount() { assert(initialized == true); eff_task_count.SetAll(0); }
  const tArray<double> & GetCurTaskQuality() const { assert(initialized == true); return cur_task_quality; }
  const tArray<double> & GetCurTaskValue() const { assert(initialized == true); return cur_task_value; }
  const tArray<double> & GetCurInternalTaskQuality() const { assert(initialized == true); return cur_internal_task_quality; }
  const tArray<double>& GetCurRBinsTotal() const { assert(initialized == true); return cur_rbins_total; }
  double GetCurRBinTotal(int index) const { assert(initialized == true); return cur_rbins_total[index]; }
  const tArray<double>& GetCurRBinsAvail() const { assert(initialized == true); return cur_rbins_avail; }
  double GetCurRBinAvail(int index) const { assert(initialized == true); return cur_rbins_avail[index]; }
  const tArray<int>& GetCurReactionCount() const { assert(initialized == true); return cur_reaction_count;}
  const tArray<double>& GetCurReactionAddReward() const { assert(initialized == true); return cur_reaction_add_reward;}
  const tArray<int>& GetCurInstCount() const { assert(initialized == true); return cur_inst_count; }
  const tArray<int>& GetCurSenseCount() const { assert(initialized == true); return cur_sense_count; }
  double GetSensedResource(int _in) { assert(initialized == true); return sensed_resources[_in]; }
  const tArray<int>& GetCurCollectSpecCounts() const { assert(initialized == true); return cur_collect_spec_counts; }
  int GetCurCollectSpecCount(int spec_id) const { assert(initialized == true); return cur_collect_spec_counts[spec_id]; }
	const tArray<int>& GetTestCPUInstCount() const { assert(initialized == true); return testCPU_inst_count; }


  void  NewTrial(); //Save the current fitness, and reset the bonus. @JEB
  void  TrialDivideReset(const cSequence & _genome); //Subset of resets specific to division not done by NewTrial. @JEB
  const tArray<double>& GetTrialFitnesses() { return cur_trial_fitnesses; }; //Return list of trial fitnesses. @JEB
  const tArray<double>& GetTrialBonuses() { return cur_trial_bonuses; }; //Return list of trial bonuses. @JEB
  const tArray<int>& GetTrialTimesUsed() { return cur_trial_times_used; }; //Return list of trial times used. @JEB

  double GetLastMeritBase() const { assert(initialized == true); return last_merit_base; }
  double GetLastBonus() const { assert(initialized == true); return last_bonus; }

  double GetLastMerit() const { assert(initialized == true); return last_merit_base*last_bonus; }
  int GetLastNumErrors() const { assert(initialized == true); return last_num_errors; }
  int GetLastNumDonates() const { assert(initialized == true); return last_num_donates; }
  const tArray<int>& GetLastTaskCount() const { assert(initialized == true); return last_task_count; }
  const tArray<int>& GetLastHostTaskCount() const { assert(initialized == true); return last_host_tasks; }
  const tArray<int>& GetLastParasiteTaskCount() const { assert(initialized == true); return last_para_tasks; }
  void  SetLastParasiteTaskCount(tArray<int>  oldParaPhenotype);
  const tArray<int>& GetLastInternalTaskCount() const { assert(initialized == true); return last_internal_task_count; }
  const tArray<double>& GetLastTaskQuality() const { assert(initialized == true); return last_task_quality; }
  const tArray<double>& GetLastTaskValue() const { assert(initialized == true); return last_task_value; }
  const tArray<double>& GetLastInternalTaskQuality() const { assert(initialized == true); return last_internal_task_quality; }
  const tArray<double>& GetLastRBinsTotal() const { assert(initialized == true); return last_rbins_total; }
  const tArray<double>& GetLastRBinsAvail() const { assert(initialized == true); return last_rbins_avail; }
  const tArray<int>& GetLastReactionCount() const { assert(initialized == true); return last_reaction_count; }
  const tArray<double>& GetLastReactionAddReward() const { assert(initialized == true); return last_reaction_add_reward; }
  const tArray<int>& GetLastInstCount() const { assert(initialized == true); return last_inst_count; }
  const tArray<int>& GetLastSenseCount() const { assert(initialized == true); return last_sense_count; }
  double GetLastFitness() const { assert(initialized == true); return last_fitness; }
  double GetPermanentGermlinePropensity() const { assert(initialized == true); return permanent_germline_propensity; }
  const tArray<int>& GetLastCollectSpecCounts() const { assert(initialized == true); return last_collect_spec_counts; }
  int GetLastCollectSpecCount(int spec_id) const { assert(initialized == true); return last_collect_spec_counts[spec_id]; }

  int GetNumDivides() const { assert(initialized == true); return num_divides;}
  int GetNumDivideFailed() const { assert(initialized == true); return num_divides_failed;}

  int GetGeneration() const { return generation; }
  int GetCPUCyclesUsed() const { assert(initialized == true); return cpu_cycles_used; }
  int GetTimeUsed()   const { assert(initialized == true); return time_used; }
  int GetTrialTimeUsed()   const { assert(initialized == true); return trial_time_used; }
  int GetAge()        const { assert(initialized == true); return age; }
  const cString& GetFault() const { assert(initialized == true); return fault_desc; }
  double GetNeutralMetric() const { assert(initialized == true); return neutral_metric; }
  double GetLifeFitness() const { assert(initialized == true); return life_fitness; }
  int  GetNumThreshGbDonations() const { assert(initialized == true); return num_thresh_gb_donations; }
  int  GetNumThreshGbDonationsLast() const { assert(initialized == true); return num_thresh_gb_donations_last; }
  int  GetNumQuantaThreshGbDonations() const { assert(initialized == true); return num_quanta_thresh_gb_donations; }
  int  GetNumQuantaThreshGbDonationsLast() const { assert(initialized == true); return num_quanta_thresh_gb_donations_last; }
  int  GetNumShadedGbDonations() const { assert(initialized == true); return num_shaded_gb_donations; }
  int  GetNumShadedGbDonationsLast() const { assert(initialized == true); return num_shaded_gb_donations_last; }
  int GetNumDonationsLocus() const { assert(initialized == true); return num_donations_locus; }
  int GetNumDonationsLocusLast() const { assert(initialized == true); return num_donations_locus_last; }

  bool IsInjected() const { assert(initialized == true); return is_injected; }
  bool IsDonorCur() const { assert(initialized == true); return is_donor_cur; }
  bool IsDonorLast() const { assert(initialized == true); return is_donor_last; }
  bool IsDonorRand() const { assert(initialized == true); return is_donor_rand; }
  bool IsDonorRandLast() const { assert(initialized == true); return is_donor_rand_last; }
  bool IsDonorKin() const { assert(initialized == true); return is_donor_kin; }
  bool IsDonorKinLast() const { assert(initialized == true); return is_donor_kin_last; }
  bool IsDonorEdit() const { assert(initialized == true); return is_donor_edit; }
  bool IsDonorEditLast() const { assert(initialized == true); return is_donor_edit_last; }
  bool IsDonorGbg() const { assert(initialized == true); return is_donor_gbg; }
  bool IsDonorGbgLast() const { assert(initialized == true); return is_donor_gbg_last; }
  bool IsDonorTrueGb() const { assert(initialized == true); return is_donor_truegb; }
  bool IsDonorTrueGbLast() const { assert(initialized == true); return is_donor_truegb_last; }
  bool IsDonorThreshGb() const { assert(initialized == true); return is_donor_threshgb; }
  bool IsDonorThreshGbLast() const { assert(initialized == true); return is_donor_threshgb_last; }
  bool IsDonorQuantaThreshGb() const { assert(initialized == true); return is_donor_quanta_threshgb; }
  bool IsDonorQuantaThreshGbLast() const { assert(initialized == true); return is_donor_quanta_threshgb_last; }
  bool IsDonorShadedGb() const { assert(initialized == true); return is_donor_shadedgb; }
  bool IsDonorShadedGbLast() const { assert(initialized == true); return is_donor_shadedgb_last; }	
  bool IsDonorPosition(int pos) const {assert(initialized == true); return is_donor_locus.GetSize() > pos ? is_donor_locus[pos] : 0; }
  bool IsDonorPositionLast(int pos) const {assert(initialized == true); return is_donor_locus_last.GetSize() > pos ? is_donor_locus_last[pos] : 0; }
  
  bool IsEnergyRequestor() const { assert(initialized == true); return is_energy_requestor; }
  bool IsEnergyDonor() const { assert(initialized == true); return is_energy_donor; }
  bool IsEnergyReceiver() const { assert(initialized == true); return is_energy_receiver; }
  bool HasUsedEnergyDonation() const { assert(initialized == true); return has_used_donated_energy; }
  bool HasOpenEnergyRequest() const { assert(initialized == true); return has_open_energy_request; }
  bool IsReceiver() const { assert(initialized == true); return is_receiver; }
  bool IsReceiverLast() const { assert(initialized == true); return is_receiver_last; }
  bool IsReceiverRand() const { assert(initialized == true); return is_receiver_rand; }
  bool IsReceiverKin() const { assert(initialized == true); return is_receiver_kin; }
  bool IsReceiverKinLast() const { assert(initialized == true); return is_receiver_kin_last; }
  bool IsReceiverEdit() const { assert(initialized == true); return is_receiver_edit; }
  bool IsReceiverEditLast() const { assert(initialized == true); return is_receiver_edit_last; }
  bool IsReceiverGbg() const { assert(initialized == true); return is_receiver_gbg; }
  bool IsReceiverTrueGb() const { assert(initialized == true); return is_receiver_truegb; }
  bool IsReceiverTrueGbLast() const { assert(initialized == true); return is_receiver_truegb_last; }
  bool IsReceiverThreshGb() const { assert(initialized == true); return is_receiver_threshgb; }
  bool IsReceiverThreshGbLast() const { assert(initialized == true); return is_receiver_threshgb_last; }
  bool IsReceiverQuantaThreshGb() const { assert(initialized == true); return is_receiver_quanta_threshgb; }
  bool IsReceiverQuantaThreshGbLast() const { assert(initialized == true); return is_receiver_quanta_threshgb_last; }
  bool IsReceiverShadedGb() const { assert(initialized == true); return is_receiver_shadedgb; }
  bool IsReceiverShadedGbLast() const { assert(initialized == true); return is_receiver_shadedgb_last; }
  bool IsReceiverGBSameLocus() const { assert(initialized == true); return is_receiver_gb_same_locus; }
  bool IsReceiverGBSameLocusLast() const { assert(initialized == true); return is_receiver_gb_same_locus_last; }
  bool IsModifier() const { assert(initialized == true); return is_modifier; }
  bool IsModified() const { assert(initialized == true); return is_modified; }
  bool IsFertile() const  { assert(initialized == true); return is_fertile; }
  bool IsMutated() const  { assert(initialized == true); return is_mutated; }
  bool IsMultiThread() const { assert(initialized == true); return is_multi_thread; }
  bool ParentTrue() const { assert(initialized == true); return parent_true; }
  bool ParentSex() const  { assert(initialized == true); return parent_sex; }
  int  ParentCrossNum() const  { assert(initialized == true); return parent_cross_num; }

  bool CopyTrue() const   { assert(initialized == true); return copy_true; }
  bool DivideSex() const  { assert(initialized == true); return divide_sex; }
  int MateSelectID() const { assert(initialized == true); return mate_select_id; }
  int CrossNum() const  { assert(initialized == true); return cross_num; }
  bool ChildFertile() const { assert(initialized == true); return child_fertile;}
  int GetChildCopiedSize() const { assert(initialized == true); return child_copied_size; }


  ////////////////////  Accessors -- Modifying  ///////////////////
  void SetMerit(const cMerit& in_merit) { merit = in_merit; }
  void SetFitness(const double in_fit) { fitness = in_fit; }
  void ReduceEnergy(const double cost);
  void SetEnergy(const double value);
  void SetGestationTime(int in_time) { gestation_time = in_time; }
  void SetTimeUsed(int in_time) { time_used = in_time; }
  void SetTrialTimeUsed(int in_time) { trial_time_used = in_time; }
  void SetGeneration(int in_generation) { generation = in_generation; }
  void SetPermanentGermlinePropensity(double _in) { permanent_germline_propensity = _in; }
  void SetFault(const cString& in_fault) { fault_desc = in_fault; }
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
	void SetTestCPUInstCount(const tArray<int>& in_counts) { testCPU_inst_count = in_counts; }
  void IncreaseEnergyDonated(double amount) { assert(amount >=0); total_energy_donated += amount; }
  void IncreaseEnergyReceived(double amount) { assert(amount >=0); total_energy_received += amount; }
  void IncreaseEnergyApplied(double amount) { assert(amount >=0); total_energy_applied += amount; }
  void IncreaseNumEnergyRequests() { num_energy_requests++; }
  void IncreaseNumEnergyDonations() { num_energy_donations++; }
  void IncreaseNumEnergyApplications() { num_energy_applications++; }
  void IncreaseNumEnergyReceptions() { num_energy_receptions++; }
  double GetAmountEnergyDonated() { return total_energy_donated; }
  double GetAmountEnergyReceived() { return total_energy_received; }
  double GetAmountEnergyApplied() { return total_energy_applied; }
  int GetNumEnergyDonations() { return num_energy_donations; }
  int GetNumEnergyReceptions() { return num_energy_receptions; }
  int GetNumEnergyApplications() { return num_energy_applications; }
  
  void SetCurRBinsAvail(const tArray<double>& in_avail) { cur_rbins_avail = in_avail; }
  void SetCurRbinsTotal(const tArray<double>& in_total) { cur_rbins_total = in_total; }
  void SetCurRBinAvail(int index, double val) { cur_rbins_avail[index] = val; }
  void SetCurRBinTotal(int index, double val) { cur_rbins_total[index] = val; }
  void AddToCurRBinAvail(int index, double val) { cur_rbins_avail[index] += val; }
  void AddToCurRBinTotal(int index, double val) { cur_rbins_total[index] += val; }
  void SetCurCollectSpecCount(int spec_id, int val) { cur_collect_spec_counts[spec_id] = val; }

  void SetIsMultiThread() { is_multi_thread = true; }
  void SetIsDonorCur() { is_donor_cur = true; } 
  void SetIsDonorRand() { SetIsDonorCur(); is_donor_rand = true; }
  void SetIsDonorKin() { SetIsDonorCur(); is_donor_kin = true; }
  void SetIsDonorNull() { SetIsDonorCur(); is_donor_null = true; }
  void SetIsDonorEdit() { SetIsDonorCur(); is_donor_edit = true; }
  void SetIsDonorGbg() { SetIsDonorCur(); is_donor_gbg = true; }
  void SetIsDonorTrueGb() { SetIsDonorCur(); is_donor_truegb = true; }
  void SetIsDonorThreshGb() { SetIsDonorCur(); is_donor_threshgb = true; }
  void SetIsDonorQuantaThreshGb() { SetIsDonorCur(); is_donor_quanta_threshgb = true; }
  void SetIsDonorShadedGb() { SetIsDonorCur(); is_donor_shadedgb = true; }
  void SetIsDonorPosition(int pos) { SetIsDonorCur(); if (is_donor_locus.GetSize() <= pos) is_donor_locus.Resize(pos+1, false); is_donor_locus[pos] = true; }
  void SetIsReceiver() { is_receiver = true; } 
  void SetIsReceiverRand() { SetIsReceiver(); is_receiver_rand = true; } 
  void SetIsReceiverKin() { SetIsReceiver(); is_receiver_kin = true; } 
  void SetIsReceiverEdit() { SetIsReceiver(); is_receiver_edit = true; } 
  void SetIsReceiverGbg() { SetIsReceiver(); is_receiver_gbg = true; } 
  void SetIsReceiverTrueGb() { SetIsReceiver(); is_receiver_truegb = true; } 
  void SetIsReceiverThreshGb() { SetIsReceiver(); is_receiver_threshgb = true; } 
  void SetIsReceiverQuantaThreshGb() { SetIsReceiver(); is_receiver_quanta_threshgb = true; } 
  void SetIsReceiverShadedGb() { SetIsReceiver(); is_receiver_shadedgb = true; }
  void SetIsReceiverGBSameLocus() { SetIsReceiver(); is_receiver_gb_same_locus = true; }
  void SetIsEnergyRequestor() { is_energy_requestor = true; }
  void SetIsEnergyDonor() { is_energy_donor = true; }
  void SetIsEnergyReceiver() { is_energy_receiver = true; }
  void SetHasUsedDonatedEnergy() {has_used_donated_energy = true; }
  void SetHasOpenEnergyRequest() { has_open_energy_request = true; }
  void ClearHasOpenEnergyRequest() { has_open_energy_request = false; }
  void ClearIsMultiThread() { is_multi_thread = false; }
  
  void SetCurBonus(double _bonus) { cur_bonus = _bonus; }
  void SetCurBonusInstCount(int _num_bonus_inst) {bonus_instruction_count = _num_bonus_inst;}

  void IncCurInstCount(int _inst_num)  { assert(initialized == true); cur_inst_count[_inst_num]++; } 
  void DecCurInstCount(int _inst_num)  { assert(initialized == true); cur_inst_count[_inst_num]--; } 
  
  void IncNumThreshGbDonations() { assert(initialized == true); num_thresh_gb_donations++; }
  void IncNumQuantaThreshGbDonations() { assert(initialized == true); num_quanta_thresh_gb_donations++; }
  void IncNumShadedGbDonations() { assert(initialized == true); num_shaded_gb_donations++; }	
  void IncNumGreenBeardSameLocus() { assert(initialized == true); num_donations_locus++; }	
  void IncAge()      { assert(initialized == true); age++; }
  void IncCPUCyclesUsed() { assert(initialized == true); cpu_cycles_used++; trial_cpu_cycles_used++; }
  void DecCPUCyclesUsed() { assert(initialized == true); cpu_cycles_used--; trial_cpu_cycles_used--; }
  void IncTimeUsed(int i=1) { assert(initialized == true); time_used+=i; trial_time_used+=i; }
  void IncErrors()   { assert(initialized == true); cur_num_errors++; }
  void IncDonates()   { assert(initialized == true); cur_num_donates++; }
  void IncSenseCount(const int i) { /*assert(initialized == true); cur_sense_count[i]++;*/ }  
  
  bool& IsInjected() { assert(initialized == true); return is_injected; }
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
  
  void DoubleEnergyUsage();
  void HalveEnergyUsage();
  void DefaultEnergyUsage();
	
	// --- Support for Division of Labor --- //
	int GetLastTaskID() const { return last_task_id; }
	int  GetNumNewUniqueReactions() const {assert(initialized == true);  return num_new_unique_reactions; }
	void  ResetNumNewUniqueReactions()  {num_new_unique_reactions =0; }
	double GetResourcesConsumed(); 

  //LZ - Parasite Etc. Helpers
  void DivideFailed();
  void UpdateParasiteTasks() { last_para_tasks = cur_para_tasks; cur_para_tasks.SetAll(0); return; }


  
  void RefreshEnergy();
  void ApplyToEnergyStore();
  void EnergyTestament(const double value); //! external energy given to organism
  void ApplyDonatedEnergy();
  void ReceiveDonatedEnergy(const double value);
  double ExtractParentEnergy();
  
  bool operator<(const cPhenotype& rhs) const;
  bool operator>(const cPhenotype& rhs) const;
  bool operator==(const cPhenotype& rhs) const;
  bool operator!=(const cPhenotype& rhs) const; 
  
  struct lt_phenotype{
    bool operator()(const cPhenotype* lhs, const cPhenotype* rhs) const
      {return *lhs < *rhs;}   // operator< in cPhenotype
  };
};


inline void cPhenotype::SetInstSetSize(int inst_set_size)
{
  cur_inst_count.Resize(inst_set_size, 0);
  last_inst_count.Resize(inst_set_size, 0);
}

#endif
