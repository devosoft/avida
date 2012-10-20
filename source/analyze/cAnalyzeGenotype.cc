/*
 *  cAnalyzeGenotype.cc
 *  Avida
 *
 *  Called "analyze_genotype.cc" prior to 12/2/05.
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

#include "cAnalyzeGenotype.h"

#include "avida/core/WorldDriver.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPhenPlastGenotype.h"
#include "cPlasticPhenotype.h"
#include "cTestCPU.h"
#include "cEnvironment.h"
#include "cHardwareManager.h"
#include "cWorld.h"

#include "tDataCommandManager.h"
#include "tDMSingleton.h"


#include <cmath>
using namespace std;
using namespace Avida;


cAnalyzeGenotype::cAnalyzeGenotype(cWorld* world, const Genome& genome)
: m_world(world)
, m_genome(genome)
, name("")
, m_cpu_test_info() 
, m_data(new sGenotypeDatastore)
, aligned_sequence("")
, tag("")
, viable(false)
, id_num(-1)
, parent_id(-1)
, parent2_id(-1)
, num_cpus(0)
, total_cpus(0)
, update_born(0)
, update_dead(0)
, depth(0)
, length(0)
, copy_length(0)
, exe_length(0)
, merit(0.0)
, gest_time(INT_MAX)
, fitness(0.0)
, errors(0)
, inst_executed_counts(0)
, task_counts(0)
, task_qualities(0)
, internal_task_counts(0)
, internal_task_qualities(0)
, rbins_total(0)
, rbins_avail(0)
, collect_spec_counts(0)
, m_mating_type(MATING_TYPE_JUVENILE)
, m_mate_preference(MATE_PREFERENCE_RANDOM)
, m_mating_display_a(0)
, m_mating_display_b(0)
, fitness_ratio(0.0)
, efficiency_ratio(0.0)
, comp_merit_ratio(0.0)
, parent_dist(0)
, ancestor_dist(0)
, parent_muts("")
, knockout_stats(NULL)
, m_land(NULL)
, m_phenplast_stats(NULL)
{
  
}

cAnalyzeGenotype::cAnalyzeGenotype(const cAnalyzeGenotype& _gen)
: m_world(_gen.m_world)
, m_genome(_gen.m_genome)
, name(_gen.name)
, m_cpu_test_info(_gen.m_cpu_test_info)  
, m_data(_gen.m_data)
, aligned_sequence(_gen.aligned_sequence)
, tag(_gen.tag)
, viable(_gen.viable)
, id_num(_gen.id_num)
, parent_id(_gen.parent_id)
, parent2_id(_gen.parent2_id)
, num_cpus(_gen.num_cpus)
, total_cpus(_gen.total_cpus)
, update_born(_gen.update_born)
, update_dead(_gen.update_dead)
, depth(_gen.depth)
, length(_gen.length)
, copy_length(_gen.copy_length)
, exe_length(_gen.exe_length)
, merit(_gen.merit)
, gest_time(_gen.gest_time)
, fitness(_gen.fitness)
, errors(_gen.errors)
, inst_executed_counts(_gen.inst_executed_counts)
, task_counts(_gen.task_counts)
, task_qualities(_gen.task_qualities)
, internal_task_counts(_gen.internal_task_counts)
, internal_task_qualities(_gen.internal_task_qualities)
, rbins_total(_gen.rbins_total)
, rbins_avail(_gen.rbins_avail)
, collect_spec_counts(_gen.collect_spec_counts)
, m_mating_type(_gen.m_mating_type)
, m_mate_preference(_gen.m_mate_preference)
, m_mating_display_a(_gen.m_mating_display_a)
, m_mating_display_b(_gen.m_mating_display_b)
, fitness_ratio(_gen.fitness_ratio)
, efficiency_ratio(_gen.efficiency_ratio)
, comp_merit_ratio(_gen.comp_merit_ratio)
, parent_dist(_gen.parent_dist)
, ancestor_dist(_gen.ancestor_dist)
, parent_muts(_gen.parent_muts)
, knockout_stats(NULL)
, m_land(NULL)
, m_phenplast_stats(NULL)
{
  if (_gen.knockout_stats != NULL) {
    knockout_stats = new cAnalyzeKnockouts;
    *knockout_stats = *(_gen.knockout_stats);
  }
  if (_gen.m_phenplast_stats != NULL)
    m_phenplast_stats = new cPhenPlastSummary(*_gen.m_phenplast_stats);
}

cAnalyzeGenotype::~cAnalyzeGenotype()
{
  if (knockout_stats != NULL) delete knockout_stats;
  if (m_phenplast_stats != NULL) delete m_phenplast_stats;
  Unlink();
}


void cAnalyzeGenotype::Initialize()
{
  tDMSingleton<tDataCommandManager<cAnalyzeGenotype> >::Initialize(&cAnalyzeGenotype::buildDataCommandManager);
}


tDataCommandManager<cAnalyzeGenotype>* cAnalyzeGenotype::buildDataCommandManager()
{
  tDataCommandManager<cAnalyzeGenotype>* dcm = new tDataCommandManager<cAnalyzeGenotype>;
  
  // A basic macro to link a keyword to a description and Get and Set methods in cAnalyzeGenotype.
#define ADD_GDATA(TYPE, KEYWORD, DESC, GET, SET, COMP, NSTR, HSTR)                                \
{                                                                                               \
cString nstr_str(#NSTR), hstr_str(#HSTR);                                                     \
cString null_str = "0";                                                                       \
if (nstr_str != "0") null_str = NSTR;                                                         \
cString html_str = "align=center";                                                            \
if (hstr_str != "0") html_str = HSTR;                                                         \
\
dcm->Add(KEYWORD, new tDataEntryOfType<cAnalyzeGenotype, TYPE>                              \
(KEYWORD, DESC, &cAnalyzeGenotype::GET, &cAnalyzeGenotype::SET, COMP, null_str, html_str)); \
}
  
  // To add a new keyword connected to a stat in cAnalyzeGenotype, you need to connect all of the pieces here.
  // The ADD_GDATA macro takes eight arguments:
  //  type              : The type of the variables being linked in.
  //  keyword           : The short word used to reference this variable from analyze mode.
  //  description       : A slightly fuller description of what this variable is; used in data legends.
  //  "get" accessor    : The accessor method to retrieve the value of this variable from cAnalyzeGenotype
  //  "set" accessor    : The method to set this variable in cAnalyzeGenotype (use SetNULL if none exists).
  //  comparison method : A method that will take two genotypes and compare this value bewtween them (or CompareNULL)
  //  null keyword      : A string to represent what should be printed if this stat is zero. (0 for default)
  //  html flags        : A string to be included in the <td> when stat is printed in HTML table (0 for "align=center")
  
  // As a reminder about the compare types:
  //   FLEX_COMPARE_NONE   = 0  -- No comparisons should be done at all.
  //   FLEX_COMPARE_DIFF   = 1  -- Only track if a stat has changed, don't worry about direction.
  //   FLEX_COMPARE_MAX    = 2  -- Color higher values as beneficial, lower as harmful.
  //   FLEX_COMPARE_MIN    = 3  -- Color lower values as beneficial, higher as harmful.
  //   FLEX_COMPARE_DIFF2  = 4  -- Same as FLEX_COMPARE_DIFF, but 0 indicates trait is off.
  //   FLEX_COMPARE_MAX2   = 5  -- Same as FLEX_COMPARE_MAX, and 0 indicates trait is off.
  //   FLEX_COMPARE_MIN2   = 6  -- Same as FLEX_COMPARE_MIN, BUT 0 still indicates off.
  
  ADD_GDATA(const cString& (), "name",         "Genotype Name",                 GetName,           SetName,       0, 0, 0);
  ADD_GDATA(bool (),           "viable",       "Is Viable (0/1)",               GetViable,         SetViable,     5, 0, 0);
  ADD_GDATA(int (),            "id",           "Genotype ID",                   GetID,             SetID,         0, 0, 0);
  ADD_GDATA(int (),            "hw_type",      "HW Type",                       GetHWType,                 SetNULL,         0, 0, 0);
  ADD_GDATA(cString (),        "inst_set",      "Instset",                      GetInstSet,                 SetNULL,         0, 0, 0);  
  ADD_GDATA(int (),            "src",          "Genotype Transmission Type",    GetSource,         SetSource,     0, 0, 0);
  ADD_GDATA(const cString& (), "src_args",     "Genotype Source Arguments",     GetSourceArgs,     SetSourceArgs, 0, "(none)", 0);
  ADD_GDATA(const cString& (), "tag",          "Genotype Tag",                  GetTag,            SetTag,        0, "(none)","");
  ADD_GDATA(const cString& (), "parents",      "Parent String",                 GetParents,        SetParents,    0, "(none)", 0);
  ADD_GDATA(int (),            "parent_id",    "Parent ID",                     GetParentID,       SetParentID,   0, 0, 0);
  ADD_GDATA(int (),            "parent2_id",   "Second Parent ID (sexual orgs)",GetParent2ID,      SetParent2ID,  0, 0, 0);
  ADD_GDATA(int (),            "parent_dist",  "Parent Distance",               GetParentDist,     SetParentDist, 0, 0, 0);
  ADD_GDATA(int (),            "ancestor_dist","Ancestor Distance",             GetAncestorDist,   SetAncestorDist, 0, 0, 0);
  ADD_GDATA(int (),            "lineage",      "Unique Lineage Label",          GetLineageLabel,   SetLineageLabel, 0, 0, 0);
  ADD_GDATA(int (),            "num_cpus",     "Number of CPUs",                GetNumCPUs,        SetNumCPUs,    0, 0, 0);
  ADD_GDATA(int (),            "total_cpus",   "Total CPUs Ever",               GetTotalCPUs,      SetTotalCPUs,  0, 0, 0);
  ADD_GDATA(int (),            "num_units",    "Number of CPUs",                GetNumCPUs,        SetNumCPUs,    0, 0, 0);
  ADD_GDATA(int (),            "total_units",  "Total CPUs Ever",               GetTotalCPUs,      SetTotalCPUs,  0, 0, 0);
  ADD_GDATA(int (),            "length",       "Genome Length",                 GetLength,         SetLength,     4, 0, 0);
  ADD_GDATA(int (),            "copy_length",  "Copied Length",                 GetCopyLength,     SetCopyLength, 0, 0, 0);
  ADD_GDATA(int (),            "exe_length",   "Executed Length",               GetExeLength,      SetExeLength,  0, 0, 0);
  ADD_GDATA(double (),         "merit",        "Merit",                         GetMerit,          SetMerit,      5, 0, 0);
  ADD_GDATA(double (),         "comp_merit",   "Computational Merit",           GetCompMerit,      SetNULL,       5, 0, 0);
  ADD_GDATA(double (),         "comp_merit_ratio", "Computational Merit Ratio", GetCompMeritRatio, SetNULL,       5, 0, 0);
  ADD_GDATA(int (),            "gest_time",    "Gestation Time",                GetGestTime,       SetGestTime,   6, "Inf", 0);
  ADD_GDATA(double (),         "efficiency",   "Rep. Efficiency",               GetEfficiency,     SetNULL,       5, 0, 0);
  ADD_GDATA(double (),         "efficiency_ratio", "Rep. Efficiency Ratio",     GetEfficiencyRatio,SetNULL,       5, 0, 0);
  ADD_GDATA(double (),         "fitness",      "Fitness",                       GetFitness,        SetFitness,    5, 0, 0);
  ADD_GDATA(double (),         "div_type",     "Divide Type",                   GetDivType,        SetDivType,    0, 0, 0);
  ADD_GDATA(int (),            "mate_id",      "Mate Selection ID Number",      GetMateID,         SetMateID,     0, 0, 0);
  ADD_GDATA(double (),         "fitness_ratio","Fitness Ratio",                 GetFitnessRatio,   SetNULL,       5, 0, 0);
  ADD_GDATA(int (),            "update_born",  "Update Born",                   GetUpdateBorn,     SetUpdateBorn, 0, 0, 0);
  ADD_GDATA(int (),            "gen_born",     "Update Born",                   GetUpdateBorn,     SetUpdateBorn, 0, 0, 0);
  ADD_GDATA(int (),            "update_dead",  "Update Dead",                   GetUpdateDead,     SetUpdateDead, 0, 0, 0);
  ADD_GDATA(int (),            "update_deactivated",  "Update Dead",            GetUpdateDead,     SetUpdateDead, 0, 0, 0);
  ADD_GDATA(int (),            "depth",        "Tree Depth",                    GetDepth,          SetDepth,      0, 0, 0);  
  ADD_GDATA(const cString& (), "cells",        "Cells",                         GetCells,          SetCells,      0, 0, 0);
  ADD_GDATA(const cString& (), "gest_offset",  "Gest Offsets",                  GetGestOffsets,    SetGestOffsets, 0, 0, 0);
  ADD_GDATA(double (),         "frac_dead",    "Fraction Mutations Lethal",     GetFracDead,       SetNULL,       0, 0, 0);
  ADD_GDATA(double (),         "frac_neg",     "Fraction Mutations Detrimental",GetFracNeg,        SetNULL,       0, 0, 0);
  ADD_GDATA(double (),         "frac_neut",    "Fraction Mutations Neutral",    GetFracNeut,       SetNULL,       0, 0, 0);
  ADD_GDATA(double (),         "frac_pos",     "Fraction Mutations Beneficial", GetFracPos,        SetNULL,       0, 0, 0);
  ADD_GDATA(double (),         "complexity",   "Basic Complexity (beneficial muts are neutral)", GetComplexity, SetNULL, 0, 0, 0);
  ADD_GDATA(double (),         "land_fitness", "Average Lanscape Fitness",      GetLandscapeFitness, SetNULL,     0, 0, 0);
  
  ADD_GDATA(int(),             "mating_type", "Mating type (-1 = juvenile; 0 = female; 1 = male)", GetMatingType, SetMatingType, 0, 0, 0);
  ADD_GDATA(int(),             "mate_preference", "Mate preference", GetMatePreference, SetMatePreference, 0, 0, 0);
  ADD_GDATA(int(),             "mating_display_a", "Mating display A", GetMatingDisplayA, SetMatingDisplayA, 0, 0, 0);
  ADD_GDATA(int(),             "mating_display_b", "Mating display B", GetMatingDisplayB, SetMatingDisplayB, 0, 0, 0);
  
  ADD_GDATA(int (),    "num_phen",           "Number of Plastic Phenotypes",          GetNumPhenotypes,          SetNULL, 0, 0, 0);
  ADD_GDATA(int (),    "num_trials",         "Number of Recalculation Trials",        GetNumTrials,              SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_entropy",       "Phenotpyic Entropy",                    GetPhenotypicEntropy,      SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_max_fitness",   "Phen Plast Maximum Fitness",            GetMaximumFitness,         SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_max_fit_freq",  "Phen Plast Maximum Fitness Frequency",  GetMaximumFitnessFrequency,SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_min_fitness",   "Phen Plast Minimum Fitness",            GetMinimumFitness,         SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_min_freq",      "Phen Plast Minimum Fitness Frequency",  GetMinimumFitnessFrequency,SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_avg_fitness",   "Phen Plast Wtd Avg Fitness",            GetAverageFitness,         SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_likely_freq",   "Freq of Most Likely Phenotype",         GetLikelyFrequency,        SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "phen_likely_fitness","Fitness of Most Likely Phenotype",      GetLikelyFitness,          SetNULL, 0, 0, 0);
  ADD_GDATA(double (), "prob_viable",        "Probability Viable",                    GetViableProbability,      SetNULL, 0, 0, 0);
  
  
  // @JEB There is a difference between these two. parent_muts is based on an alignment. mut_steps is based on recorded mutations during run.
  ADD_GDATA(const cString& (), "parent_muts", "Mutations from Parent",   GetParentMuts,   SetParentMuts, 0, "(none)", "");
  ADD_GDATA(const cString& (), "task_order",  "Task Performance Order",  GetTaskOrder,    SetTaskOrder,  0, "(none)", "");
  ADD_GDATA(cString (),        "sequence",    "Genome Sequence",         GetSequence,     SetSequence,       0, "(N/A)", "");
  ADD_GDATA(const cString& (), "alignment",   "Aligned Sequence",        GetAlignedSequence, SetAlignedSequence, 0, "(N/A)", "");
  
  ADD_GDATA(cString (), "executed_flags", "Executed Flags",             GetExecutedFlags, SetNULL, 0, "(N/A)", "");
  ADD_GDATA(cString (), "alignment_executed_flags", "Alignment Executed Flags", GetAlignmentExecutedFlags, SetNULL, 0, "(N/A)", "");
  ADD_GDATA(cString (), "task_list", "List of all tasks performed",     GetTaskList,     SetNULL, 0, "(N/A)", "");
  
  // @TODO - the following were link.tasksites and html.sequence, respectively.  The period character is now separated as
  //         an argument passed into the function, thus they are matched as the component before the period.  For now
  //         I have simply removed the argument part, since there are not any existing name clashes.  However,  in future
  //         versions we should rename these.
  ADD_GDATA(cString (), "link", "Phenotype Map",              GetMapLink,      SetNULL, 0, 0,       0);
  ADD_GDATA(cString (), "html",  "Genome Sequence",            GetHTMLSequence, SetNULL, 0, "(N/A)", "");
  
  // coarse-grained task stats
  ADD_GDATA(int (), 		"total_task_count","# Different Tasks", 		GetTotalTaskCount, SetNULL, 1, 0, 0);
  ADD_GDATA(int (), 		"total_task_performance_count", "Total Tasks Performed",	GetTotalTaskPerformanceCount, SetNULL, 1, 0, 0);
  
  
  dcm->Add("task", new tDataEntryOfType<cAnalyzeGenotype, int (int, const cStringList&)>
           ("task", &cAnalyzeGenotype::DescTask, &cAnalyzeGenotype::GetTaskCount, 5));
  dcm->Add("task_quality", new tDataEntryOfType<cAnalyzeGenotype, double (int)>
           ("task_quality", &cAnalyzeGenotype::DescTask, &cAnalyzeGenotype::GetTaskQuality, 5));
  dcm->Add("env_input", new tDataEntryOfType<cAnalyzeGenotype, int (int)>
           ("env_input", &cAnalyzeGenotype::DescEnvInput, &cAnalyzeGenotype::GetEnvInput));
  dcm->Add("inst", new tDataEntryOfType<cAnalyzeGenotype, int (int)>
           ("inst", &cAnalyzeGenotype::DescInstExe, &cAnalyzeGenotype::GetInstExecutedCount));
  dcm->Add("r_tot", new tDataEntryOfType<cAnalyzeGenotype, double (int)>
           ("r_tot", &cAnalyzeGenotype::DescRTot, &cAnalyzeGenotype::GetRBinTotal));
  dcm->Add("r_avail", new tDataEntryOfType<cAnalyzeGenotype, double (int)>
           ("r_avail", &cAnalyzeGenotype::DescRAvail, &cAnalyzeGenotype::GetRBinAvail));
  dcm->Add("prob_task", new tDataEntryOfType<cAnalyzeGenotype, double (int)>
           ("prob_task", &cAnalyzeGenotype::DescTaskProb, &cAnalyzeGenotype::GetTaskProbability, 5));
  dcm->Add("r_spec", new tDataEntryOfType<cAnalyzeGenotype, int (int)>
           ("r_spec", &cAnalyzeGenotype::DescRSpec, &cAnalyzeGenotype::GetRSpec));
  
  
  // The remaining values should actually go in a separate list called
  // "population_data_list", but for the moment we're going to put them
  // here so that we only need to worry about a single system to load and
  // save genotype information.
  ADD_GDATA(int (),     "update",       "Update Output",                   GetUpdateDead, SetUpdateDead, 0, 0, 0);
  ADD_GDATA(int (),     "dom_num_cpus", "Number of Dominant Organisms",    GetNumCPUs,    SetNumCPUs,    0, 0, 0);
  ADD_GDATA(int (),     "dom_depth",    "Tree Depth of Dominant Genotype", GetDepth,      SetDepth,      0, 0, 0);
  ADD_GDATA(int (),     "dom_id",       "Dominant Genotype ID",            GetID,         SetID,         0, 0, 0);
  ADD_GDATA(cString (), "dom_sequence", "Dominant Genotype Sequence",      GetSequence,   SetNULL,       0, "(N/A)", "");
  
  
  return dcm;
#undef ADD_GDATA
}

tDataCommandManager<cAnalyzeGenotype>& cAnalyzeGenotype::GetDataCommandManager()
{
  return tDMSingleton<tDataCommandManager<cAnalyzeGenotype> >::GetInstance();
}


cString cAnalyzeGenotype::DescTask(int task_id) const
{
  if (task_id > m_world->GetEnvironment().GetNumTasks()) return "";
  return m_world->GetEnvironment().GetTask(task_id).GetDesc();
}

cString cAnalyzeGenotype::DescTaskProb(int task_id) const
{
  if (task_id > m_world->GetEnvironment().GetNumTasks()) return "";
  return DescTask(task_id) + " (Probability)";
}


cAnalyzeGenotype::sGenotypeDatastore::~sGenotypeDatastore()
{
  for (Apto::Map<int, cGenotypeData*>::ValueIterator it = dmap.Values(); it.Next();) delete *it.Get();
}

void cAnalyzeGenotype::SetGenotypeData(int data_id, cGenotypeData* data)
{
  m_data->rwlock.WriteLock();
  m_data->dmap.Set(data_id, data);
  m_data->rwlock.WriteUnlock();
}


int cAnalyzeGenotype::CalcMaxGestation() const
{
  ConstInstructionSequencePtr seq_p;
  ConstGeneticRepresentationPtr rep_p = m_genome.Representation();
  seq_p.DynamicCastFrom(rep_p);
  const InstructionSequence& seq = *seq_p;
  return m_world->GetConfig().TEST_CPU_TIME_MOD.Get() * seq.GetSize();
}

void cAnalyzeGenotype::CalcKnockouts(bool check_pairs, bool check_chart) const
{
  if (knockout_stats == NULL) {
    // We've never called this before -- setup the stats.
    knockout_stats = new cAnalyzeKnockouts;
  }
  else if (check_pairs == true && knockout_stats->has_pair_info == false) {
    // We don't have the pair stats we need -- keep going.
    knockout_stats->Reset();
  }
  else if (check_chart == true && knockout_stats->has_chart_info == false) {
    // We don't have the phyenotype chart we need -- keep going.
    knockout_stats->Reset();
  }
  else {
    // We already have all the info we need -- just quit.
    return;
  }
  
  cAvidaContext& ctx = m_world->GetDefaultContext();
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
  
  // Calculate the base fitness for the genotype we're working with...
  // (This may not have been run already, and cost negligiably more time
  // considering the number of knockouts we need to do.
  cAnalyzeGenotype base_genotype(m_world, m_genome);
  base_genotype.Recalculate(ctx);
  double base_fitness = base_genotype.GetFitness();
  const Apto::Array<int> base_task_counts( base_genotype.GetTaskCounts() );
  
  // If the base fitness is 0, the organism is dead and has no complexity.
  if (base_fitness == 0.0) {
    knockout_stats->neut_count = length;
    delete testcpu;
    return;
  }
  
  Genome mod_genome(m_genome);
  
  // Setup a NULL instruction needed for testing
  const Instruction null_inst = m_world->GetHardwareManager().GetInstSet(mod_genome.Properties().Get("instset").StringValue()).ActivateNullInst();
  
  // If we are keeping track of the specific effects on tasks from the
  // knockouts, setup the matrix.
  if (check_chart == true) {
    knockout_stats->task_counts.Resize(length);
    knockout_stats->has_chart_info = true;
  }
  
  // Loop through all the lines of code, testing the removal of each.
  // -2=lethal, -1=detrimental, 0=neutral, 1=beneficial
  Apto::Array<int> ko_effect(length);
  for (int line_num = 0; line_num < length; line_num++) {
    // Save a copy of the current instruction and replace it with "NULL"
    InstructionSequencePtr mod_seq_p;
    GeneticRepresentationPtr mod_rep_p = mod_genome.Representation();
    mod_seq_p.DynamicCastFrom(mod_rep_p);
    InstructionSequence& mod_seq = *mod_seq_p;
    int cur_inst = mod_seq[line_num].GetOp();
    mod_seq[line_num] = null_inst;
    cAnalyzeGenotype ko_genotype(m_world, mod_genome);
    ko_genotype.Recalculate(ctx);
    if (check_chart == true) {
      const Apto::Array<int> ko_task_counts( ko_genotype.GetTaskCounts() );
      knockout_stats->task_counts[line_num] = ko_task_counts;
    }
    
    double ko_fitness = ko_genotype.GetFitness();
    if (ko_fitness == 0.0) {
      knockout_stats->dead_count++;
      ko_effect[line_num] = -2;
    } else if (ko_fitness < base_fitness) {
      knockout_stats->neg_count++;
      ko_effect[line_num] = -1;
    } else if (ko_fitness == base_fitness) {
      knockout_stats->neut_count++;
      ko_effect[line_num] = 0;
    } else if (ko_fitness > base_fitness) {
      knockout_stats->pos_count++;
      ko_effect[line_num] = 1;
    } else {
      cerr << "error: internal: illegal state in CalcKnockouts()" << endl;
    }
    
    // Reset the mod_genome back to the original sequence.
    mod_seq[line_num].SetOp(cur_inst);
  }
  
  // Only continue from here if we are looking at all pairs of knockouts
  // as well.
  if (check_pairs == false) {
    delete testcpu;
    return;
  }
  
  Apto::Array<int> ko_pair_effect(ko_effect);
  for (int line1 = 0; line1 < length; line1++) {
    // If this line has already been changed, keep going...
    if (ko_effect[line1] != ko_pair_effect[line1]) continue;
    
    // Loop through all possibilities for the next line.
    for (int line2 = line1+1; line2 < length; line2++) {
      // If this line has already been changed, keep going...
      if (ko_effect[line2] != ko_pair_effect[line2]) continue;
      
      // If the two lines are of different types (one is information and the
      // other is not) then we're not interested in testing this combination
      // since any possible result is reasonable.
      if ((ko_effect[line1] < 0 && ko_effect[line2] >= 0) ||
          (ko_effect[line1] >= 0 && ko_effect[line2] < 0)) {
        continue;
      }
      
      // Calculate the fitness for this pair of knockouts to determine if its
      // something other than what we expected.
      InstructionSequencePtr mod_seq_p;
      GeneticRepresentationPtr mod_rep_p = mod_genome.Representation();
      mod_seq_p.DynamicCastFrom(mod_rep_p);
      InstructionSequence& mod_genome_seq = *mod_seq_p;
      
      int cur_inst1 = mod_genome_seq[line1].GetOp();
      int cur_inst2 = mod_genome_seq[line2].GetOp();
      mod_genome_seq[line1] = null_inst;
      mod_genome_seq[line2] = null_inst;
      cAnalyzeGenotype ko_genotype(m_world, mod_genome);
      ko_genotype.Recalculate(ctx);
      
      double ko_fitness = ko_genotype.GetFitness();
      
      // If the individual knockouts are both harmful, but in combination
      // they are neutral or even beneficial, they should not count as 
      // information.
      if (ko_fitness >= base_fitness &&
          ko_effect[line1] < 0 && ko_effect[line2] < 0) {
        ko_pair_effect[line1] = 0;
        ko_pair_effect[line2] = 0;
      }
      
      // If the individual knockouts are both neutral (or beneficial?),
      // but in combination they are harmful, they are likely redundant
      // to each other.  For now, count them both as information.
      if (ko_fitness < base_fitness &&
          ko_effect[line1] >= 0 && ko_effect[line2] >= 0) {
        ko_pair_effect[line1] = -1;
        ko_pair_effect[line2] = -1;
      }	
      
      // Reset the mod_genome back to the original sequence.
      mod_genome_seq[line1].SetOp(cur_inst1);
      mod_genome_seq[line2].SetOp(cur_inst2);
    }
  }
  
  for (int i = 0; i < length; i++) {
    if (ko_pair_effect[i] == -2) knockout_stats->pair_dead_count++;
    else if (ko_pair_effect[i] == -1) knockout_stats->pair_neg_count++;
    else if (ko_pair_effect[i] == 0) knockout_stats->pair_neut_count++;
    else if (ko_pair_effect[i] == 1) knockout_stats->pair_pos_count++;
  }
  
  knockout_stats->has_pair_info = true;
  delete testcpu;
}

void cAnalyzeGenotype::CheckLand() const
{
  if (m_land == NULL) {
    m_land = new cLandscape(m_world, m_genome);
    m_land->SetCPUTestInfo(m_cpu_test_info);
    m_land->SetDistance(1);
    m_land->Process(m_world->GetDefaultContext());
  }
}

void cAnalyzeGenotype::CheckPhenPlast() const
{
  // Implicit genotype recalculation if required
  if (m_phenplast_stats == NULL) {
    cCPUTestInfo test_info;
    
    cPhenPlastGenotype pp(m_genome, 1000, test_info, m_world, m_world->GetDefaultContext());
    m_phenplast_stats = new cPhenPlastSummary(pp);
  }
}



void cAnalyzeGenotype::CalcLandscape(cAvidaContext& ctx)
{
  if (m_land == NULL) m_land = new cLandscape(m_world, m_genome);
  m_land->SetCPUTestInfo(m_cpu_test_info);
  m_land->SetDistance(1);
  m_land->Process(ctx);
}


void cAnalyzeGenotype::Recalculate(cAvidaContext& ctx, cCPUTestInfo* test_info, cAnalyzeGenotype* parent_genotype, int num_trials)
{  
  // Allocate our own test info if it wasn't provided
  cCPUTestInfo* local_test_info = NULL;
  if (!test_info) {
    local_test_info = new cCPUTestInfo();
    test_info = local_test_info;
  }
  
  // Handling recalculation here
  cPhenPlastGenotype recalc_data(m_genome, num_trials, *test_info, m_world, ctx);
  
  // The most likely phenotype will be assigned to the phenotype stats
  const cPlasticPhenotype* likely_phenotype = recalc_data.GetMostLikelyPhenotype();
  
  viable                = likely_phenotype->IsViable();
  m_env_inputs          = likely_phenotype->GetEnvInputs();
  executed_flags        = likely_phenotype->GetExecutedFlags();
  inst_executed_counts  = likely_phenotype->GetLastInstCount();
  length                = likely_phenotype->GetGenomeLength();
  copy_length           = likely_phenotype->GetCopiedSize();
  exe_length            = likely_phenotype->GetExecutedSize();
  merit                 = likely_phenotype->GetMerit().GetDouble();
  gest_time             = likely_phenotype->GetGestationTime();
  fitness               = likely_phenotype->GetFitness();
  errors                = likely_phenotype->GetLastNumErrors();
  div_type              = likely_phenotype->GetDivType();
  mate_id               = likely_phenotype->MateSelectID();
  task_counts           = likely_phenotype->GetLastTaskCount();
  task_qualities        = likely_phenotype->GetLastTaskQuality();
  internal_task_counts  = likely_phenotype->GetLastInternalTaskCount();
  internal_task_qualities = likely_phenotype->GetLastInternalTaskQuality();
  rbins_total           = likely_phenotype->GetLastRBinsTotal();
  rbins_avail           = likely_phenotype->GetLastRBinsAvail();
  collect_spec_counts   = likely_phenotype->GetLastCollectSpecCounts();
  m_mating_type 		= likely_phenotype->GetMatingType(); //@CHC
  m_mate_preference     = likely_phenotype->GetMatePreference(); //@CHC
  m_mating_display_a    = likely_phenotype->GetCurMatingDisplayA();
  m_mating_display_b    = likely_phenotype->GetCurMatingDisplayB();

  
  // Setup a new parent stats if we have a parent to work with.
  if (parent_genotype != NULL) {
    fitness_ratio = GetFitness() / parent_genotype->GetFitness();
    efficiency_ratio = GetEfficiency() / parent_genotype->GetEfficiency();
    comp_merit_ratio = GetCompMerit() / parent_genotype->GetCompMerit();
    ConstInstructionSequencePtr seq_p;
    GeneticRepresentationPtr rep_p = m_genome.Representation();
    seq_p.DynamicCastFrom(rep_p);
    const InstructionSequence& seq = *seq_p;
    
    const Genome& parent_genome = parent_genotype->GetGenome();
    ConstInstructionSequencePtr parent_seq_p;
    ConstGeneticRepresentationPtr parent_rep_p = parent_genome.Representation();
    parent_seq_p.DynamicCastFrom(parent_rep_p);
    const InstructionSequence& parent_seq = *parent_seq_p;
    
    parent_dist = cStringUtil::EditDistance((const char *)seq.AsString(), (const char *)parent_seq.AsString(), parent_muts);
    
    ancestor_dist = parent_genotype->GetAncestorDist() + parent_dist;
  }
  
  // Summarize plasticity information if multiple recalculations performed
  if (num_trials > 1){
    if (m_phenplast_stats != NULL)
      delete m_phenplast_stats;
    m_phenplast_stats = new cPhenPlastSummary(recalc_data);
  }
  
  delete local_test_info;
}


void cAnalyzeGenotype::PrintTasks(ofstream& fp, int min_task, int max_task)
{
  if (max_task == -1) max_task = task_counts.GetSize();
  
  for (int i = min_task; i < max_task; i++) {
    fp << task_counts[i] << " ";
  }
}

void cAnalyzeGenotype::PrintTasksQuality(ofstream& fp, int min_task, int max_task)
{
  if (max_task == -1) max_task = task_counts.GetSize();
  
  for (int i = min_task; i < max_task; i++) {
    fp << task_qualities[i] << " ";
  }
}

void cAnalyzeGenotype::PrintInternalTasks(ofstream& fp, int min_task, int max_task)
{
  if (max_task == -1) max_task = internal_task_counts.GetSize();
  
  for (int i = min_task; i < max_task; i++) {
    fp << internal_task_counts[i] << " ";
  }
}

void cAnalyzeGenotype::PrintInternalTasksQuality(ofstream& fp, int min_task, int max_task)
{
  if (max_task == -1) max_task = internal_task_counts.GetSize();
  
  for (int i = min_task; i < max_task; i++) {
    fp << internal_task_qualities[i] << " ";
  }
}

void cAnalyzeGenotype::SetParents(const cString& parent_str)
{
  cString lps(parent_str);
  if (lps.GetSize()) parent_id = lps.Pop(',').AsInt();
  if (lps.GetSize()) parent2_id = lps.Pop(',').AsInt();
}

void cAnalyzeGenotype::SetParentID(int _parent_id)
{
  parent_id = _parent_id;
  if (parent_id >= 0) {
    if (parent2_id >= 0) {
      m_parent_str = cStringUtil::Stringf("%d,%d", parent_id, parent2_id);
    } else {
      m_parent_str = cStringUtil::Stringf("%d", parent_id);
    }
  } else {
    m_parent_str = "";
  }
}

void cAnalyzeGenotype::SetParent2ID(int _parent2_id)
{
  parent2_id = _parent2_id;
  if (parent_id >= 0) {
    if (parent2_id >= 0) {
      m_parent_str = cStringUtil::Stringf("%d,%d", parent_id, parent2_id);
    } else {
      m_parent_str = cStringUtil::Stringf("%d", parent_id);
    }
  } else {
    m_parent_str = "";
  }
}

void cAnalyzeGenotype::SetInstSet(const cString& inst_set)
{
  static const Apto::BasicString<Apto::ThreadSafe> prop_instset("instset");
  m_genome.Properties().SetValue(prop_instset, (const char*)inst_set);
}


cString cAnalyzeGenotype::GetAlignmentExecutedFlags() const
{
  // Make this on the fly from executed flags
  // and the genome sequence, inserting gaps...
  cString aligned_executed_flags = GetExecutedFlags();
  cString aligned_seq = GetAlignedSequence();
  
  for (int i=0; i<aligned_seq.GetSize(); i++)
  {
    if (aligned_seq[i] == '_') aligned_executed_flags.Insert("_", i);
  }
  
  return aligned_executed_flags;
}

int cAnalyzeGenotype::GetInstExecutedCount(int _inst_num) const
{
  if(_inst_num < inst_executed_counts.GetSize() && _inst_num > 0)
  { return inst_executed_counts[_inst_num]; }
  
  // If the instruction is not valid, clearly it has never been executed!
  return 0;
}

cString cAnalyzeGenotype::DescInstExe(int _inst_id) const
{
  if(_inst_id > inst_executed_counts.GetSize() || _inst_id < 0) return "";
  
  cString desc("# Times ");
  desc += m_world->GetHardwareManager().GetInstSet(m_genome.Properties().Get("instset").StringValue()).GetName(_inst_id);
  desc += " Executed";
  return desc;
}

int cAnalyzeGenotype::GetKO_DeadCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->dead_count;
}

int cAnalyzeGenotype::GetKO_NegCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->neg_count;
}

int cAnalyzeGenotype::GetKO_NeutCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->neut_count;
}

int cAnalyzeGenotype::GetKO_PosCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->pos_count;
}

int cAnalyzeGenotype::GetKO_Complexity() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->dead_count + knockout_stats->neg_count;
}

int cAnalyzeGenotype::GetKOPair_DeadCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_dead_count;
}

int cAnalyzeGenotype::GetKOPair_NegCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_neg_count;
}

int cAnalyzeGenotype::GetKOPair_NeutCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_neut_count;
}

int cAnalyzeGenotype::GetKOPair_PosCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_pos_count;
}

int cAnalyzeGenotype::GetKOPair_Complexity() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_dead_count + knockout_stats->pair_neg_count;
}

const Apto::Array< Apto::Array<int> > & cAnalyzeGenotype::GetKO_TaskCounts() const
{
  CalcKnockouts(false, true);  // Make sure knockouts are calculated
  return knockout_stats->task_counts;
}

cString cAnalyzeGenotype::GetTaskList() const
{
  const int num_tasks = task_counts.GetSize();
  cString out_string(num_tasks);
  
  for (int i = 0; i < num_tasks; i++) {
    const int cur_count = task_counts[i];
    if (cur_count < 10) {
      out_string[i] = '0' + cur_count;
    }
    else if (cur_count < 30) {
      out_string[i] = 'X';
    }
    else if (cur_count < 80) {
      out_string[i] = 'L';
    }
    else if (cur_count < 300) {
      out_string[i] = 'C';
    }
    else if (cur_count < 800) {
      out_string[i] = 'D';
    }
    else if (cur_count < 3000) {
      out_string[i] = 'M';
    }
    else {
      out_string[i] = '+';
    }
  }
  
  return out_string;
}


cString cAnalyzeGenotype::GetSequence() const 
{ 
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(m_genome.Representation());
  cString ret((const char*)seq->AsString());
//  printf("analyze_getseq: %s, %s\n", (const char*)seq->AsString(), (const char*)ret);
  return ret;
}

void cAnalyzeGenotype::SetSequence(cString _seq)
{
  InstructionSequencePtr seq;
  seq.DynamicCastFrom(m_genome.Representation());  
  InstructionSequence new_seq((const char*)_seq);
  *seq = new_seq;
}


cString cAnalyzeGenotype::GetHTMLSequence() const
{
  ConstInstructionSequencePtr seq_p;
  ConstGeneticRepresentationPtr rep_p = m_genome.Representation();
  seq_p.DynamicCastFrom(rep_p);
  const InstructionSequence& genome_seq = *seq_p;
  
  cString text_genome = (const char *)genome_seq.AsString();
  cString html_code("<tt>");
  
  cString diff_info = parent_muts;
  char mut_type = 'N';
  int mut_pos = -1;
  
  cString cur_mut = diff_info.Pop(',');
  if (cur_mut != "") {
    mut_type = cur_mut[0];
    cur_mut.ClipFront(1); cur_mut.ClipEnd(1);
    mut_pos = cur_mut.AsInt();
  }
  
  int ins_count = 0;
  for (int i = 0; i < genome_seq.GetSize(); i++) {
    char symbol = text_genome[i];
    if (i != mut_pos) html_code += symbol;
    else {
      // Figure out the information for the type of mutation we had...
      cString color;
      if (mut_type == 'M') {
        color = "#FF0000";	
      } else if (mut_type == 'I') {
        color = "#00FF00";
        ins_count++;
      } else { // if (mut_type == 'D') {
        color = "#0000FF";
        symbol = '*';
        i--;  // Rewind - we didn't read the handle character yet!
      }
      
      // Move on to the next mutation...
      cur_mut = diff_info.Pop(',');
      if (cur_mut != "") {
        mut_type = cur_mut[0];
        cur_mut.ClipFront(1); cur_mut.ClipEnd(1);
        mut_pos = cur_mut.AsInt();
        if (mut_type == 'D') mut_pos += ins_count;
      } else mut_pos = -1;
      
      // Tack on the current symbol...
      cString symbol_string;
      symbol_string.Set("<b><font color=\"%s\">%c</font></b>", static_cast<const char*>(color), symbol);
      html_code += symbol_string;
    }
  }
  
  html_code += "</tt>";
  
  return html_code;
}
