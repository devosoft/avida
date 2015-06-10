/*
 *  cAnalyzeGenotype.h
 *  Avida
 *
 *  Called "analyze_genotype.hh" prior to 12/2/05.
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

#ifndef cAnalyzeGenotype_h
#define cAnalyzeGenotype_h

#include "apto/core/RWLock.h"
#include "avida/core/Genome.h"
#include "avida/core/Properties.h"

#include <fstream>

#include "cCPUMemory.h"
#include "cGenotypeData.h"
#include "cInstSet.h"
#include "cLandscape.h"
#include "cPhenPlastGenotype.h"
#include "cString.h"
#include "cStringList.h"
#include "cStringUtil.h"
#include "cPhenPlastSummary.h"

// cAnalyzeGenotype    : Collection of information about loaded genotypes

class cAvidaContext;
class cInstSet;
class cTestCPU;
class cWorld;
template<class T> class tDataCommandManager;

class cAnalyzeGenotype;

using namespace Avida;


class cAnalyzeGenotypeLink
{
private:
  cAnalyzeGenotype *m_parent;
  tList<cAnalyzeGenotype> m_child_list;
  
public:
  cAnalyzeGenotypeLink() : m_parent(NULL) { m_child_list.Clear(); }
  
  void SetParent(cAnalyzeGenotype* parent) { m_parent = parent; }
  cAnalyzeGenotype* GetParent() { return m_parent; }
  tList<cAnalyzeGenotype>& GetChildList() { return m_child_list; }
  cAnalyzeGenotype* FindChild(cAnalyzeGenotype* child) { return GetChildList().FindPtr(child); }
  cAnalyzeGenotype* RemoveChild(cAnalyzeGenotype* child) { return GetChildList().Remove(child); }
  void AddChild(cAnalyzeGenotype* child) { if(!FindChild(child)) GetChildList().PushRear(child); }
};


class cAnalyzeGenotype
{
  friend class ReadToken;
private:
  cWorld* m_world;
  Genome m_genome;        // Full Genome
  cString name;              // Name, if one was provided in loading
  cCPUTestInfo m_cpu_test_info; // Use this test info
  
  struct sGenotypeDatastore : public Apto::RefCountObject<Apto::ThreadSafe>
  {
    mutable Apto::RWLock rwlock;
    mutable Apto::Map<int, cGenotypeData*> dmap;
    
    sGenotypeDatastore() { ; }
    sGenotypeDatastore(const sGenotypeDatastore& ds) : Apto::RefCountObject<Apto::ThreadSafe>(ds) { ; } // Note that data objects are not copied right now
    
    ~sGenotypeDatastore();
  };
  Apto::SmartPtr<sGenotypeDatastore, Apto::InternalRCObject> m_data;
  
  cString aligned_sequence;  // Sequence (in ASCII) after alignment
  cString tag;               // All genotypes in a batch can be tagged

  cAnalyzeGenotypeLink m_link;

  bool viable;

  // Group 1 : Load-in Stats (Only obtained if available for input file)
  int id_num;
  Systematics::Source m_src;
  cString m_src_args;
  cString m_parent_str;
  int parent_id;
  int parent2_id;
  int num_cpus;
  int total_cpus;
  int update_born;
  int update_dead;
  int depth;
  cString m_cells;
  cString m_gest_offsets;

  // Group 2 : Basic Execution Stats (Obtained from test CPUs)
  int length;
  int copy_length;
  int exe_length;
  double merit;
  int gest_time;
  double fitness;
  int errors;
  double div_type;
  int mate_id;
  cString executed_flags; // converted into a string
  Apto::Array<int> inst_executed_counts;
  Apto::Array<int> task_counts;
  Apto::Array<double> task_qualities;
  Apto::Array<int> internal_task_counts;
  Apto::Array<double> internal_task_qualities;
  Apto::Array<double> rbins_total;
  Apto::Array<double> rbins_avail;
  Apto::Array<int> collect_spec_counts;
  Apto::Array<int> m_env_inputs;
  int m_mating_type; //@CHC
  int m_mate_preference; //@CHC
  int m_mating_display_a; //@CHC
  int m_mating_display_b; //@CHC


  // Group 3 : Stats requiring parental genotype (Also from test CPUs)
  double fitness_ratio;
  double efficiency_ratio;
  double comp_merit_ratio;
  int parent_dist;
  int ancestor_dist;
  int lineage_label;
  cString parent_muts;

  // Group 4 : Landscape stats (obtained from testing all possible mutations)
  class cAnalyzeKnockouts {
  public:
    // Calculations based off of all single knockouts
    int dead_count;
    int neg_count;
    int neut_count;
    int pos_count;
    
    // Extra calculations based off of double knockouts...
    bool has_pair_info;  // Have these calculations been made?
    int pair_dead_count;
    int pair_neg_count;
    int pair_neut_count;
    int pair_pos_count;
    
    bool has_chart_info; // Keep a chart of which sites affect which tasks?
    Apto::Array< Apto::Array<int> > task_counts;

    void Reset() {
      dead_count = 0;
      neg_count = 0;
      neut_count = 0;
      pos_count = 0;
      
      has_pair_info = false;
      pair_dead_count = 0;
      pair_neg_count = 0;
      pair_neut_count = 0;
      pair_pos_count = 0;

      has_chart_info = false;
      task_counts.Resize(0);
    }
    
    cAnalyzeKnockouts() { Reset(); }
  };
  mutable cAnalyzeKnockouts* knockout_stats;

  mutable cLandscape* m_land;

  // Group 5 : More complex stats (obtained indvidually, through tests)
  cString task_order;

  
  // Group 6: Phenotypic Plasticity
  mutable cPhenPlastSummary* m_phenplast_stats;
  
  int NumCompare(double new_val, double old_val) const {
    if (new_val == old_val) return  0;
    else if (new_val == 0)       return -2;
    else if (old_val == 0)       return +2;
    else if (new_val < old_val)  return -1;
    // else if (new_val > old_val)
    return +1;
  }

  int CalcMaxGestation() const;
  void CalcKnockouts(bool check_pairs = false, bool check_chart = false) const;
  void CheckLand() const;
  void CheckPhenPlast() const;
  void SummarizePhenotypicPlasticity(const cPhenPlastGenotype& pp) const;
  
  static tDataCommandManager<cAnalyzeGenotype>* buildDataCommandManager();



public:
  cAnalyzeGenotype(cWorld* world, const Genome& genome);
  cAnalyzeGenotype(const cAnalyzeGenotype& _gen);
  ~cAnalyzeGenotype();
  
  static void Initialize();
  static tDataCommandManager<cAnalyzeGenotype>& GetDataCommandManager();
  
  class ReadToken;
  ReadToken* GetReadToken() const { m_data->rwlock.ReadLock(); return new ReadToken(this); }

  void SetGenotypeData(int data_id, cGenotypeData* data);
  cGenotypeData* GetGenotypeData(ReadToken* tk, int data_id) const { tk->Validate(this); return m_data->dmap.GetWithDefault(data_id, NULL); }
  
  void SetCPUTestInfo(cCPUTestInfo& in_cpu_test_info) { m_cpu_test_info = in_cpu_test_info; }
  
  void Recalculate(cAvidaContext& ctx, cCPUTestInfo* test_info = NULL, cAnalyzeGenotype* parent_genotype = NULL, int num_trials = 1);
  void PrintTasks(std::ofstream& fp, int min_task = 0, int max_task = -1);
  void PrintTasksQuality(std::ofstream& fp, int min_task = 0, int max_task = -1);
  void PrintInternalTasks(std::ofstream& fp, int min_task = 0, int max_task = -1);
  void PrintInternalTasksQuality(std::ofstream& fp, int min_task = 0, int max_task = -1);
  void CalcLandscape(cAvidaContext& ctx);

  // Set...
  void SetInstSet(const cString& inst_set);
  void SetName(const cString& _name) { name = _name; }
  void SetAlignedSequence(const cString & _seq) { aligned_sequence = _seq; }
  void SetTag(const cString& _tag) { tag = _tag; }

  void SetViable(bool _viable) { viable = _viable; }

  void SetID(int _id) { id_num = _id; }
  void SetSource(int _src) { m_src.transmission_type = (Systematics::TransmissionType)_src; }
  void SetSourceArgs(const cString& src_args) { m_src_args = src_args; }
  void SetParents(const cString& parent_str);
  void SetParentID(int _parent_id);
  void SetParent2ID(int _parent_id);
  void SetNumCPUs(int _cpus) { num_cpus = _cpus; }
  void SetTotalCPUs(int _cpus) { total_cpus = _cpus; }
  void SetUpdateBorn(int _born) { update_born = _born; }
  void SetUpdateDead(int _dead) { update_dead = _dead; }
  void SetDepth(int _depth) { depth = _depth; }
  void SetCells(const cString& cells) { m_cells = cells; }
  void SetGestOffsets(const cString& gest_offsets) { m_gest_offsets = gest_offsets; }
  void SetGenome(Genome& genome) {m_genome = genome;};

  void SetLength(int _length) { length = _length; }
  void SetCopyLength(int _length) { copy_length = _length; }
  void SetExeLength(int _length) { exe_length = _length; }
  void SetMerit(double _merit) { merit = _merit; }
  void SetGestTime(int _gest) { gest_time = _gest; }
  void SetFitness(double _fitness) { fitness = _fitness; }
  void SetDivType(double _div_type) { div_type = _div_type; }
  void SetMateID(int _mate_id) { mate_id = _mate_id; }
  void SetParentDist(int _dist) { parent_dist = _dist; }
  void SetAncestorDist(int _dist) { ancestor_dist = _dist; }
  void SetLineageLabel(int _label) { lineage_label = _label; }
  void SetMatingType(int _mating_type) { m_mating_type = _mating_type; } //@CHC
  void SetMatePreference(int _mate_preference) { m_mate_preference = _mate_preference; } //@CHC
  void SetMatingDisplayA(int _mating_display_a) { m_mating_display_a = _mating_display_a; } //@CHC
  void SetMatingDisplayB(int _mating_display_b) { m_mating_display_b = _mating_display_b; } //@CHC


  void SetParentMuts(const cString & in_muts) { parent_muts = in_muts; }

  void SetTaskOrder(const cString & in_order) { task_order = in_order; }

  // A set of NULL accessors to simplyfy automated accesses.
  void SetNULL(int, int) { ; }
  void SetNULL(int dummy) { (void) dummy; }
  void SetNULL(char dummy) { (void) dummy; }
  void SetNULL(double dummy) { (void) dummy; }
  void SetNULL(const cString & dummy) { (void) dummy; }
  void SetNULL(cString dummy) { (void) dummy; }

  // Accessors...
  cWorld* GetWorld() { return m_world; }

  Genome& GetGenome() { return m_genome; }

  const Genome& GetGenome() const { return m_genome; }
  const cString& GetName() const { return name; }
  const cString& GetAlignedSequence() const { return aligned_sequence; }
  cString GetExecutedFlags() const { return executed_flags; }
  cString GetAlignmentExecutedFlags() const;
  const Apto::Array<int>& GetInstExecutedCounts() const { return inst_executed_counts; }
  int GetInstExecutedCount(int _inst_num) const;
  cString DescInstExe(int _inst_id) const;
  const cString & GetTag() const { return tag; }

  bool GetViable() const { return viable; }

  int GetID() const { return id_num; }
  int GetHWType() const { return m_genome.HardwareType(); }
  int GetSource() const { return m_src.transmission_type; }
  const cString& GetSourceArgs() const { return m_src_args; }
  const cString& GetParents() const { return m_parent_str; }
  int GetParentID() const { return parent_id; }
  int GetParent2ID() const { return parent2_id; }
  int GetParentDist() const { return parent_dist; }
  int GetAncestorDist() const { return ancestor_dist; }
  int GetLineageLabel() const { return lineage_label; }
  int GetNumCPUs() const { return num_cpus; }
  int GetTotalCPUs() const { return total_cpus; }
  int GetLength() const { return length; }
  int GetCopyLength() const { return copy_length; }
  int GetExeLength() const { return exe_length; }
  int GetMinLength() const { return Apto::Min(exe_length, copy_length); }
  double GetMerit() const { return merit; }
  double GetCompMerit() const { return merit / (double) GetMinLength(); }
  int GetGestTime() const { return gest_time; }
  double GetEfficiency() const { return ((double) GetMinLength()) / (double) gest_time; }
  double GetFitness() const { return fitness; }
  double GetDivType() const { return div_type; }
  int GetMateID() const { return mate_id; }
  int GetUpdateBorn() const { return update_born; }
  int GetUpdateDead() const { return update_dead; }
  int GetDepth() const { return depth; }
  const cString& GetCells() const { return m_cells; }
  const cString& GetGestOffsets() const { return m_gest_offsets; }

  const cString& GetParentMuts() const { return parent_muts; }

  int GetMatingType() const { return m_mating_type; }
  int GetMatePreference() const { return m_mate_preference; }
  int GetMatingDisplayA() const { return m_mating_display_a; }
  int GetMatingDisplayB() const { return m_mating_display_b; }

  // Knockout accessors
  int GetKO_DeadCount() const;
  int GetKO_NegCount() const;
  int GetKO_NeutCount() const;
  int GetKO_PosCount() const;
  int GetKO_Complexity() const;
  int GetKOPair_DeadCount() const;
  int GetKOPair_NegCount() const;
  int GetKOPair_NeutCount() const;
  int GetKOPair_PosCount() const;
  int GetKOPair_Complexity() const;
  const Apto::Array< Apto::Array<int> > & GetKO_TaskCounts() const;
  
  // Landscape accessors
  double GetFracDead() const  { CheckLand(); return m_land->GetProbDead(); }
  double GetFracNeg() const { CheckLand(); return m_land->GetProbNeg(); }
  double GetFracNeut() const { CheckLand(); return m_land->GetProbNeut(); }
  double GetFracPos() const { CheckLand(); return m_land->GetProbPos(); }
  double GetComplexity() const { CheckLand(); return m_land->GetComplexity(); }
  double GetLandscapeFitness() const { CheckLand(); return m_land->GetAveFitness(); }

  
  // Phenotypic Plasticity accessors
  int    GetNumPhenotypes()     const { CheckPhenPlast(); return m_phenplast_stats->m_num_phenotypes; }
  double GetPhenotypicEntropy() const { CheckPhenPlast(); return m_phenplast_stats->m_phenotypic_entropy; }
  double GetMaximumFitness()    const { CheckPhenPlast(); return m_phenplast_stats->m_max_fitness; }
  double GetMaximumFitnessFrequency() const {CheckPhenPlast(); return m_phenplast_stats->m_min_fit_frequency;}
  double GetMinimumFitness()    const { CheckPhenPlast(); return m_phenplast_stats->m_min_fitness; }
  double GetMinimumFitnessFrequency() const {CheckPhenPlast(); return m_phenplast_stats->m_min_fit_frequency;}
  double GetAverageFitness()    const { CheckPhenPlast(); return m_phenplast_stats->m_avg_fitness; }
  double GetLikelyFrequency()  const { CheckPhenPlast(); return m_phenplast_stats->m_likely_frequency; }
  double GetLikelyFitness()     const { CheckPhenPlast(); return m_phenplast_stats->m_likely_fitness; }
  int    GetNumTrials()         const { CheckPhenPlast(); return m_phenplast_stats->m_recalculate_trials; }
  double GetViableProbability()  const { CheckPhenPlast(); return m_phenplast_stats->m_viable_probability; }
  double GetTaskProbability(int task_id) const { 
    if (task_id >= m_world->GetEnvironment().GetNumTasks()) return 0.0;
    CheckPhenPlast();
    return m_phenplast_stats->m_task_probabilities[task_id];
  }
  cString DescTaskProb(int task_id) const;
  Apto::Array<double> GetTaskProbabilities() const { CheckPhenPlast(); return m_phenplast_stats->m_task_probabilities; }
    
  
  double GetFitnessRatio() const { return fitness_ratio; }
  double GetEfficiencyRatio() const { return efficiency_ratio; }
  double GetCompMeritRatio() const { return comp_merit_ratio; }

  const cString & GetTaskOrder() const { return task_order; }
  cString GetTaskList() const;

  cString GetInstSet() const { return cString((const char*)m_genome.Properties().Get("instset").StringValue()); }
  cString GetSequence() const;
  void SetSequence(cString _seq);
  cString GetHTMLSequence() const;

  cString GetMapLink() const {
    return cStringUtil::Stringf("<a href=\"tasksites.%s.html\">Phenotype Map</a>", static_cast<const char*>(GetName()));
  }

  int GetNumTasks() const { return task_counts.GetSize(); }
  int GetTaskCount(int task_id) const {
    if (task_id >= task_counts.GetSize()) return 0;
    return task_counts[task_id];
  }
  int GetTaskCount(int task_id, const cStringList& args) const {
    if (task_id >= task_counts.GetSize()) return 0;
    if (args.HasString("binary")) return (task_counts[task_id] > 0);
    return task_counts[task_id];
  }
  const Apto::Array<int>& GetTaskCounts() const { return task_counts; }
  cString DescTask(int task_id) const;
  
  double GetTaskQuality(int task_id) const {
	  if (task_id >= task_counts.GetSize()) return 0;
	  return task_qualities[task_id];
  }
  const Apto::Array<double>& GetTaskQualities() const { return task_qualities; }
  
  
  // number of different tasks performed
  int GetTotalTaskCount() const {
  	int total_task_count = 0;
  	for(int i = 0; i < task_counts.GetSize(); i++)
  	{ if (task_counts[i] > 0) total_task_count++; }
  	return total_task_count;
  }
  
  // total number of tasks performed, including multiple performances
  int GetTotalTaskPerformanceCount() const {
  	int total_task_performance_count = 0;
  	for(int i = 0; i < task_counts.GetSize(); i++)
  	{ total_task_performance_count += task_counts[i]; }
  	return total_task_performance_count;
  }
  
  int GetEnvInput(int input_id) const{
    if (input_id >= m_env_inputs.GetSize()) return 0;
    return m_env_inputs[input_id];
  }
  const Apto::Array<int>& GetEnvInputs() const{
    return m_env_inputs;
  }
  cString DescEnvInput(int input_id) const { return cStringUtil::Stringf("task.%d", input_id); }
  
  double GetRBinTotal(int resource_id) const {
    if (resource_id >= rbins_total.GetSize()) return -1;
    return rbins_total[resource_id];
  }
  cString DescRTot(int resource_id) const { return cStringUtil::Stringf("Resource %d Total", resource_id);}
  
  double GetRBinAvail(int resource_id) const {
    if (resource_id >= rbins_avail.GetSize()) return -1;
    return rbins_avail[resource_id];
  }
  cString DescRAvail(int resource_id) const { return cStringUtil::Stringf("Resource %d Available", resource_id);}
  
  int GetRSpec(int spec_id) const {
    if (spec_id >= collect_spec_counts.GetSize() || spec_id < 0) return -1;
    return collect_spec_counts[spec_id];
  }
  cString DescRSpec(int spec_id) const { return cStringUtil::Stringf("# times specification %d used", spec_id);}

  // Comparisons...  Compares a genotype to the "previous" one, which is
  // passed in, in one specified phenotype.
  // Return values are:
  //   -2 : Toggle; no longer has phenotype it used to...
  //   -1 : Reduction in strength of phenotype
  //    0 : Identical in phenotype
  //   +1 : Improvement in phenotype
  //   +2 : Toggle; phenotype now present that wasn't.
  int CompareNULL(cAnalyzeGenotype* prev) const { (void) prev; return 0; }
  int CompareArgNULL(cAnalyzeGenotype* prev, int i) const { (void) prev; (void) i; return 0; }
  int CompareLength(cAnalyzeGenotype* prev) const
  {
    if (GetLength() < MIN_GENOME_LENGTH && prev->GetLength() > MIN_GENOME_LENGTH) return -2;
    if (GetLength() > MIN_GENOME_LENGTH && prev->GetLength() < MIN_GENOME_LENGTH) return 2;
    return 0;
  }
  int CompareMerit(cAnalyzeGenotype * prev) const { return NumCompare(GetMerit(), prev->GetMerit()); }
  int CompareCompMerit(cAnalyzeGenotype * prev) const { return NumCompare(GetCompMerit(), prev->GetCompMerit()); }
  int CompareGestTime(cAnalyzeGenotype * prev) const
  {
    const int max_time = CalcMaxGestation();
    const int cur_time = max_time - GetGestTime();
    const int prev_time = max_time - prev->GetGestTime();
    return NumCompare(cur_time, prev_time);
  }
  int CompareEfficiency(cAnalyzeGenotype * prev) const { return NumCompare(GetEfficiency(), prev->GetEfficiency()); }
  int CompareFitness(cAnalyzeGenotype * prev) const { return NumCompare(GetFitness(), prev->GetFitness()); }
  int CompareTaskCount(cAnalyzeGenotype * prev, int task_id) const
  {
    return NumCompare(GetTaskCount(task_id), prev->GetTaskCount(task_id));
  }

  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cAnalyzeGenotype& in) const { return &in == this; }

  
  cAnalyzeGenotypeLink& GetLink() { return m_link; }  
  cAnalyzeGenotype* GetParent() { return GetLink().GetParent(); }
  
  void LinkParent(cAnalyzeGenotype *parent) {
    if (GetParent() && GetParent() != parent) GetParent()->GetLink().RemoveChild(this);
    GetLink().SetParent(parent);
    if (parent) parent->GetLink().AddChild(this);
  }
  
  void LinkChild(cAnalyzeGenotype &child) { child.LinkParent(this); }
  
  void UnlinkParent() { LinkParent(0); }
  
  tList<cAnalyzeGenotype>& GetChildList() { return GetLink().GetChildList(); }
  
  void UnlinkChildren()
  {
    tListIterator<cAnalyzeGenotype> it(GetChildList());
    while (it.Next() != NULL) {
      it.Get()->UnlinkParent();
    }
  }
  
  void Unlink()
  {
    UnlinkParent();
    UnlinkChildren();
  }
  
  bool HasChild(cAnalyzeGenotype &child) { return GetLink().FindChild(&child); }
  
  bool UnlinkChild(cAnalyzeGenotype &child)
  {
    if(HasChild(child)) {
      child.UnlinkParent();
      return true;
    } else {
      return false;
    }
  }
  
  
  class ReadToken
  {
    friend class cAnalyzeGenotype;
  private:
    const cAnalyzeGenotype* m_ptr;

    ReadToken(const ReadToken&); // @not_implemented
    ReadToken& operator=(const ReadToken&); // @not_implemented

    inline ReadToken(const cAnalyzeGenotype* ptr) : m_ptr(ptr) { ; }
    
    inline void Validate(const cAnalyzeGenotype* ptr) { assert(ptr == m_ptr); (void) ptr; }
    
  public:
    ~ReadToken() { m_ptr->m_data->rwlock.ReadUnlock(); }
  };
    
};

#endif
