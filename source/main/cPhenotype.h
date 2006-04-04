/*
 *  cPhenotype.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cPhenotype_h
#define cPhenotype_h

#include <fstream>

#ifndef cMerit_h
#include "cMerit.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
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
class cWorld;

class cPhenotype
{
private:
  cWorld* m_world;
  bool initialized;

  // 1. These are values calculated at the last divide (of self or offspring)
  cMerit merit;          // Relative speed of CPU
  int genome_length;     // Number of instructions in genome.
  int copied_size;       // Instructions copied into genome.
  int executed_size;     // Instructions executed from genome.
  int gestation_time;    // CPU cycles to produce offspring (or be produced)
  int gestation_start;   // Total instructions executed at last divide.
  double fitness;        // Relative efective replication rate...
  double div_type;	     // Type of the divide command used

  // 2. These are "in progress" variables, updated as the organism operates
  double cur_bonus;               // Current Bonus
  int cur_num_errors;             // Total instructions executed illeagally.
  int cur_num_donates;            // Number of donations so far
  tArray<int> cur_task_count;     // Total times each task was performed
  tArray<double> cur_task_quality;	  // Average (total?) quality with which each task was performed
  tArray<int> cur_reaction_count; // Total times each reaction was triggered.
  tArray<int> cur_inst_count;	  // Intruction exection counter
  tArray<double> sensed_resources; // Resources of which the organism is explictly aware
  
  // 3. These mark the status of "in progess" variables at the last divide.
  double last_merit_base;         // Either constant or based on genome length.
  double last_bonus;
  int last_num_errors;
  int last_num_donates;
  tArray<int> last_task_count;
  tArray<double> last_task_quality;
  tArray<int> last_reaction_count;
  tArray<int> last_inst_count;	  // Intruction exection counter
  double last_fitness;            // Used to determine sterilization.

  // 4. Records from this organisms life...
  int num_divides;       // Total successful divides organism has produced.
  int generation;        // Number of birth events to original ancestor.
  int time_used;         // Total CPU cycles consumed.
  int age;               // Number of updates organism has survived for.
  cString fault_desc;    // A description of the most recent error.
  double neutral_metric; // Undergoes drift (gausian 0,1) per generation
  double life_fitness; 	 // Organism fitness during it's lifetime, 
		         // calculated based on merit just before the divide

  // 5. Status Flags...  (updated at each divide)
  bool to_die;		 // Has organism has triggered something fatal?
  bool to_delete;        // Should this organism be deleted when finished?
  bool is_injected;      // Was this organism injected into the population?
  bool is_parasite;      // Has this organism ever executed outside code?
  bool is_donor_cur;     // Has this organism ever donated merit?  
  bool is_donor_last;    // Did this organism's parent ever donate merit? 
  bool is_receiver;      // Has this organism ever received merit donation?
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


  cPhenotype(); // @not_implemented
  cPhenotype(const cPhenotype&); // @not_implemented
  cPhenotype& operator=(const cPhenotype&); // @not_implemented
  
public:
  cPhenotype(cWorld* world);
  ~cPhenotype();

  bool OK();

  // Run when being setup *as* and offspring.
  void SetupOffspring(const cPhenotype & parent_phenotype, int _length);

  // Run when being setup as an injected organism.
  void SetupInject(int _length);

  // Run when this organism successfully executes a divide.
  void DivideReset(int _length);
  
  // Same as DivideReset(), but only run in test CPUs.
  void TestDivideReset(int _length);

  // Run when an organism is being forced to replicate, but not at the end
  // of its replication cycle.  Assume exact clone with no mutations.
  void SetupClone(const cPhenotype & clone_phenotype);

  // Input and Output Reaction Tests
  bool TestInput(tBuffer<int>& inputs, tBuffer<int>& outputs);
  bool TestOutput(cAvidaContext& ctx, cTaskContext& taskctx, tBuffer<int>& send_buf, tBuffer<int>& receive_buf,
                  const tArray<double>& res_in, tArray<double>& res_change, tArray<int>& insts_triggered);

  // State saving and loading, and printing...
  bool SaveState(std::ofstream& fp);
  bool LoadState(std::ifstream & fp);
  void PrintStatus(std::ostream& fp);

  // Some useful methods...
  int CalcSizeMerit() const;
  double CalcFitnessRatio() {
    const int merit_base = CalcSizeMerit();
    const double cur_fitness = merit_base * cur_bonus / time_used;
    return cur_fitness / last_fitness;
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
  double GetCurMeritBase() const { assert(initialized == true); return CalcSizeMerit(); }
  bool GetToDie() const { assert(initialized == true); return to_die; }
  bool GetToDelete() const { assert(initialized == true); return to_delete; }
  int GetCurNumErrors() const { assert(initialized == true); return cur_num_errors; }
  int GetCurNumDonates() const { assert(initialized == true); return cur_num_donates; }
  const tArray<int>& GetCurTaskCount() const { assert(initialized == true); return cur_task_count; }
  const tArray<double> & GetCurTaskQuality() const { assert(initialized == true); return cur_task_quality; }
  const tArray<int>& GetCurReactionCount() const { assert(initialized == true); return cur_reaction_count;}
  const tArray<int>& GetCurInstCount() const { assert(initialized == true); return cur_inst_count; }
  
  double GetSensedResource(int _in) { assert(initialized == true); return sensed_resources[_in]; }

  double GetLastMeritBase() const { assert(initialized == true); return last_merit_base; }
  double GetLastBonus() const { assert(initialized == true); return last_bonus; }
  const double GetLastMerit() const { assert(initialized == true); return last_merit_base*last_bonus; }
  int GetLastNumErrors() const { assert(initialized == true); return last_num_errors; }
  int GetLastNumDonates() const { assert(initialized == true); return last_num_donates; }
  const tArray<int>& GetLastTaskCount() const { assert(initialized == true); return last_task_count; }
    const tArray<double> & GetLastTaskQuality() const { assert(initialized == true); return last_task_quality; }
  const tArray<int>& GetLastReactionCount() const { assert(initialized == true); return last_reaction_count; }
  const tArray<int>& GetLastInstCount() const { assert(initialized == true); return last_inst_count; }
  double GetLastFitness() const { assert(initialized == true); return last_fitness; }

  int GetNumDivides() const { assert(initialized == true); return num_divides;}
  int GetGeneration() const { assert(initialized == true); return generation; }
  int GetTimeUsed()   const { assert(initialized == true); return time_used; }
  int GetAge()        const { assert(initialized == true); return age; }
  const cString& GetFault() const { assert(initialized == true); return fault_desc; }
  double GetNeutralMetric() const { assert(initialized == true); return neutral_metric; }
  double GetLifeFitness() const { assert(initialized == true); return life_fitness; }

  bool IsInjected() const { assert(initialized == true); return is_injected; }
  bool IsParasite() const { assert(initialized == true); return is_parasite; }
  bool IsDonorCur() const { assert(initialized == true); return is_donor_cur; }
  bool IsDonorLast() const { assert(initialized == true); return is_donor_last; }
  bool IsReceiver() const { assert(initialized == true); return is_receiver; }
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
  int  CrossNum() const  { assert(initialized == true); return cross_num; }
  bool  ChildFertile() const { assert(initialized == true); return child_fertile;}
  int GetChildCopiedSize() const { assert(initialized == true); return child_copied_size; }


  ////////////////////  Accessors -- Modifying  ///////////////////
  void SetMerit(const cMerit& in_merit) { merit = in_merit; }
  void SetGestationTime(int in_time) { gestation_time = in_time; }
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

  void SetIsDonorCur() { is_donor_cur = true; } 
  void SetIsReceiver() { is_receiver = true; } 
  

  void IncCurInstCount(int _inst_num)  { assert(initialized == true); cur_inst_count[_inst_num]++; } 
  void DecCurInstCount(int _inst_num)  { assert(initialized == true); cur_inst_count[_inst_num]--; } 

  void IncAge()      { assert(initialized == true); age++; }
  void IncTimeUsed() { assert(initialized == true); time_used++; }
  void IncErrors()   { assert(initialized == true); cur_num_errors++; }
  void IncDonates()   { assert(initialized == true); cur_num_donates++; }
  bool& IsInjected() { assert(initialized == true); return is_injected; }
  bool& IsParasite() { assert(initialized == true); return is_parasite; }
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
};

#endif
