/*
 *  PrintActions.cc
 *  Avida
 *
 *  Created by David on 5/11/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "PrintActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyze.h"
#include "cAnalyzeGenotype.h"
#include "cBioGroup.h"
#include "cBioGroupManager.h"
#include "tArrayUtils.h"
#include "cClassificationManager.h"
#include "cCPUTestInfo.h"
#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHistogram.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenPlastGenotype.h"
#include "cPhenPlastUtil.h"
#include "cPlasticPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cSequence.h"
#include "cStats.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "tAutoRelease.h"
#include "tIterator.h"
#include "tVector.h"
#include <cmath>
#include <cerrno>
#include <map>
#include <algorithm>

class cBioGroup;


#define STATS_OUT_FILE(METHOD, DEFAULT)                                                   /*  1 */ \
class cAction ## METHOD : public cAction {                                                /*  2 */ \
private:                                                                                  /*  3 */ \
cString m_filename;                                                                     /*  4 */ \
public:                                                                                   /*  5 */ \
cAction ## METHOD(cWorld* world, const cString& args) : cAction(world, args)            /*  6 */ \
{                                                                                       /*  7 */ \
cString largs(args);                                                                  /*  8 */ \
if (largs == "") m_filename = #DEFAULT; else m_filename = largs.PopWord();            /*  9 */ \
}                                                                                       /* 10 */ \
static const cString GetDescription() { return "Arguments: [string fname=\"" #DEFAULT "\"]"; }  /* 11 */ \
void Process(cAvidaContext& ctx) { m_world->GetStats().METHOD(m_filename); }            /* 12 */ \
}                                                                                         /* 13 */ \

STATS_OUT_FILE(PrintAverageData,            average.dat         );
STATS_OUT_FILE(PrintDemeAverageData,        deme_average.dat    );
STATS_OUT_FILE(PrintErrorData,              error.dat           );
STATS_OUT_FILE(PrintVarianceData,           variance.dat        );
STATS_OUT_FILE(PrintDominantData,           dominant.dat        );
STATS_OUT_FILE(PrintStatsData,              stats.dat           );
STATS_OUT_FILE(PrintCountData,              count.dat           );
STATS_OUT_FILE(PrintMessageData,            message.dat         );
STATS_OUT_FILE(PrintMessageLog,             message_log.dat     );
STATS_OUT_FILE(PrintInterruptData,          interrupt.dat       );
STATS_OUT_FILE(PrintTotalsData,             totals.dat          );
STATS_OUT_FILE(PrintTasksData,              tasks.dat           );
STATS_OUT_FILE(PrintHostTasksData,          host_tasks.dat      );
STATS_OUT_FILE(PrintParasiteTasksData,      parasite_tasks.dat  );
STATS_OUT_FILE(PrintTasksExeData,           tasks_exe.dat       );
STATS_OUT_FILE(PrintNewTasksData,			newtasks.dat		);
STATS_OUT_FILE(PrintNewReactionData,		newreactions.dat	);
STATS_OUT_FILE(PrintNewTasksDataPlus,		newtasksplus.dat	);
STATS_OUT_FILE(PrintTasksQualData,          tasks_quality.dat   );
STATS_OUT_FILE(PrintResourceData,           resource.dat        );
STATS_OUT_FILE(PrintReactionData,           reactions.dat       );
STATS_OUT_FILE(PrintReactionExeData,        reactions_exe.dat   );
STATS_OUT_FILE(PrintCurrentReactionData,    cur_reactions.dat   );
STATS_OUT_FILE(PrintReactionRewardData,     reaction_reward.dat );
STATS_OUT_FILE(PrintCurrentReactionRewardData,     cur_reaction_reward.dat );
STATS_OUT_FILE(PrintTimeData,               time.dat            );
STATS_OUT_FILE(PrintExtendedTimeData,       xtime.dat           );
STATS_OUT_FILE(PrintMutationRateData,       mutation_rates.dat  );
STATS_OUT_FILE(PrintDivideMutData,          divide_mut.dat      );
STATS_OUT_FILE(PrintParasiteData,           parasite.dat        );
STATS_OUT_FILE(PrintMarketData,             market.dat          );
STATS_OUT_FILE(PrintSenseData,              sense.dat           );
STATS_OUT_FILE(PrintSenseExeData,           sense_exe.dat       );
STATS_OUT_FILE(PrintInternalTasksData,      in_tasks.dat        );
STATS_OUT_FILE(PrintInternalTasksQualData,  in_tasks_quality.dat);
STATS_OUT_FILE(PrintSleepData,              sleep.dat           );
STATS_OUT_FILE(PrintCompetitionData,        competition.dat     );
STATS_OUT_FILE(PrintDemeReplicationData,    deme_repl.dat       );
STATS_OUT_FILE(PrintDemeReactionDiversityReplicationData, deme_rx_repl.dat );
STATS_OUT_FILE(PrintDemeTreatableReplicationData,    deme_repl_treatable.dat       );
STATS_OUT_FILE(PrintDemeUntreatableReplicationData,  deme_repl_untreatable.dat       );
STATS_OUT_FILE(PrintDemeTreatableCount,    deme_treatable.dat       );

STATS_OUT_FILE(PrintDemeCompetitionData,    deme_compete.dat);
STATS_OUT_FILE(PrintDemeNetworkData,        deme_network.dat);
STATS_OUT_FILE(PrintDemeNetworkTopology,    deme_network_topology.dat);
STATS_OUT_FILE(PrintDemeFoundersData,       deme_founders.dat   );
STATS_OUT_FILE(PrintPerDemeTasksData,       per_deme_tasks.dat      );
STATS_OUT_FILE(PrintPerDemeTasksExeData,    per_deme_tasks_exe.dat  );
STATS_OUT_FILE(PrintAvgDemeTasksExeData,    avg_deme_tasks_exe.dat  );
STATS_OUT_FILE(PrintAvgTreatableDemeTasksExeData, avg_treatable_deme_tasks_exe.dat  );
STATS_OUT_FILE(PrintAvgUntreatableDemeTasksExeData, avg_untreatable_deme_tasks_exe.dat  );
STATS_OUT_FILE(PrintPerDemeReactionData,    per_deme_reactions.dat  );
STATS_OUT_FILE(PrintDemeTasksData,          deme_tasks.dat      );
STATS_OUT_FILE(PrintDemeTasksExeData,       deme_tasks_exe.dat  );
STATS_OUT_FILE(PrintDemeReactionData,       deme_reactions.dat  );
STATS_OUT_FILE(PrintDemeOrgTasksData,       deme_org_tasks.dat      );
STATS_OUT_FILE(PrintDemeOrgTasksExeData,    deme_org_tasks_exe.dat  );
STATS_OUT_FILE(PrintDemeOrgReactionData,    deme_org_reactions.dat  );
STATS_OUT_FILE(PrintDemeCurrentTaskExeData,	deme_cur_task_exe.dat	);
STATS_OUT_FILE(PrintDemeMigrationSuicidePoints,	deme_mig_suicide_points.dat	);
STATS_OUT_FILE(PrintMultiProcessData,       multiprocess.dat);
STATS_OUT_FILE(PrintProfilingData,          profiling.dat);

STATS_OUT_FILE(PrintCurrentTaskCounts,      curr_task_counts.dat);
STATS_OUT_FILE(PrintGermlineData,           germline.dat        );
STATS_OUT_FILE(PrintDemeResourceThresholdPredicate,     deme_resourceThresholdPredicate.dat );
STATS_OUT_FILE(PrintPredicatedMessages,     messages.dat        );
STATS_OUT_FILE(PrintCellData,               cell_data.dat       );
STATS_OUT_FILE(PrintConsensusData,          consensus.dat       );
STATS_OUT_FILE(PrintSimpleConsensusData,    simple_consensus.dat);
STATS_OUT_FILE(PrintCurrentOpinions,        opinions.dat        );
STATS_OUT_FILE(PrintOpinionsSetPerDeme,     opinions_set.dat    );
STATS_OUT_FILE(PrintPerDemeGenPerFounderData,   deme_gen_between_founders.dat );
STATS_OUT_FILE(PrintSynchronizationData,    sync.dat            );
STATS_OUT_FILE(PrintDetailedSynchronizationData, sync-detail.dat);
// @WRE: Added output event for collected visit counts
STATS_OUT_FILE(PrintCellVisitsData,         visits.dat			);
STATS_OUT_FILE(PrintFlowRateTuples,         flow_rate_tuples.dat);
STATS_OUT_FILE(PrintDynamicMaxMinData,		maxmin.dat			);
STATS_OUT_FILE(PrintNumOrgsKilledData,      orgs_killed.dat);
STATS_OUT_FILE(PrintMigrationData,      migration.dat);
STATS_OUT_FILE(PrintAgePolyethismData, age_polyethism.dat);


// reputation
STATS_OUT_FILE(PrintReputationData,         reputation.dat);
STATS_OUT_FILE(PrintShadedAltruists,         shadedaltruists.dat);
STATS_OUT_FILE(PrintDirectReciprocityData,         reciprocity.dat);
STATS_OUT_FILE(PrintStringMatchData,         stringmatch.dat);

// group formation 
STATS_OUT_FILE(PrintGroupsFormedData,         groupformation.dat);
STATS_OUT_FILE(PrintGroupIds,         groupids.dat);

// hgt information
STATS_OUT_FILE(PrintHGTData, hgt.dat);


#define POP_OUT_FILE(METHOD, DEFAULT)                                                     /*  1 */ \
class cAction ## METHOD : public cAction {                                                /*  2 */ \
private:                                                                                  /*  3 */ \
cString m_filename;                                                                     /*  4 */ \
public:                                                                                   /*  5 */ \
cAction ## METHOD(cWorld* world, const cString& args) : cAction(world, args)            /*  6 */ \
{                                                                                       /*  7 */ \
cString largs(args);                                                                  /*  8 */ \
if (largs == "") m_filename = #DEFAULT; else m_filename = largs.PopWord();            /*  9 */ \
}                                                                                       /* 10 */ \
static const cString GetDescription() { return "Arguments: [string fname=\"" #DEFAULT "\"]"; }  /* 11 */ \
void Process(cAvidaContext& ctx) { m_world->GetPopulation().METHOD(m_filename); }       /* 12 */ \
}                                                                                         /* 13 */ \

POP_OUT_FILE(PrintPhenotypeData,       phenotype_count.dat );
POP_OUT_FILE(PrintHostPhenotypeData,      host_phenotype_count.dat );
POP_OUT_FILE(PrintParasitePhenotypeData,  parasite_phenotype_count.dat );
POP_OUT_FILE(PrintPhenotypeStatus,     phenotype_status.dat);
POP_OUT_FILE(PrintDemeTestamentStats,  deme_testament.dat  );
POP_OUT_FILE(PrintCurrentMeanDemeDensity,  deme_currentMeanDensity.dat  );


class cActionPrintData : public cAction
{
private:
  cString m_filename;
  cString m_format;
public:
  cActionPrintData(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    m_filename = largs.PopWord();
    m_format = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <cString fname> <cString format>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintDataFile(m_filename, m_format, ',');
  }
};


class cActionPrintInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintInstructionData(cWorld* world, const cString& args)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "instruction.dat";
    }
    
    if (m_filename == "") m_filename.Set("instruction-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintInstructionData(m_filename, m_inst_set);
  }
};


class cActionPrintInstructionAbundanceHistogram : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintInstructionAbundanceHistogram(cWorld* world, const cString& args)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "instruction_histogram.dat";
    }
    
    if (m_filename == "") m_filename.Set("instruction_histogram-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"instruction_histogram-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& population = m_world->GetPopulation();
    
    // ----- number of instructions available?
    const cInstSet& is = m_world->GetHardwareManager().GetInstSet(m_inst_set);
    
    tArray<int> inst_counts(is.GetSize());
    inst_counts.SetAll(0);
    
    //looping through all CPUs counting up instructions
    const int num_cells = population.GetSize();
    for (int x = 0; x < num_cells; x++) {
      cPopulationCell& cell = population.GetCell(x);
      if (cell.IsOccupied() && cell.GetOrganism()->GetGenome().GetInstSet() == is.GetInstSetName()) {
        // access this CPU's code block
        cCPUMemory& cpu_mem = cell.GetOrganism()->GetHardware().GetMemory();
        const int mem_size = cpu_mem.GetSize();
        for (int y = 0; y < mem_size; y++) inst_counts[cpu_mem[y].GetOp()]++;     
      }
    }
    
    // ----- output instruction counts
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    for (int i = 0; i < is.GetSize(); i++) df.Write(inst_counts[i], is.GetName(i));
    df.Endl();
  }
};


class cActionPrintDepthHistogram : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintDepthHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "depth_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"depth_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    // Output format:    update  min  max  histogram_values...
    int min = INT_MAX;
    int max = 0;
    
    // Two pass method
    
    // Loop through all genotypes getting min and max values
    cClassificationManager& classmgr = m_world->GetClassificationManager();
    tAutoRelease<tIterator<cBioGroup> > it;
    
    it.Set(classmgr.GetBioGroupManager("genotype")->Iterator());
    while (it->Next()) {
      cBioGroup* bg = it->Get();
      if (bg->GetDepth() < min) min = bg->GetDepth();
      if (bg->GetDepth() > max) max = bg->GetDepth();
    }
    assert(max >= min);
    
    // Allocate the array for the bins (& zero)
    tArray<int> n(max - min + 1);
    n.SetAll(0);
    
    // Loop through all genotypes binning the values
    it.Set(classmgr.GetBioGroupManager("genotype")->Iterator());
    while (it->Next()) {
      n[it->Get()->GetDepth() - min] += it->Get()->GetNumUnits();
    }
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    df.Write(min, "Minimum");
    df.Write(max, "Maximum");
    for (int i = 0; i < n.GetSize(); i++)  df.WriteAnonymous(n[i]);
    df.Endl();
  }
};


class cActionEcho : public cAction
{
private:
  cString m_filename;
public:
  cActionEcho(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "Arguments: <cString message>"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_args == "") {
      m_args.Set("Echo : Update = %f\t AveGeneration = %f", m_world->GetStats().GetUpdate(),
                 m_world->GetStats().SumGeneration().Average());
    }
    m_world->GetDriver().NotifyComment(m_args);
  }
};


class cActionPrintGenotypeAbundanceHistogram : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintGenotypeAbundanceHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "genotype_abundance_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"genotype_abundance_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    // Allocate array for the histogram & zero it
    tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    tArray<int> hist(it->Next()->GetNumUnits());
    hist.SetAll(0);
    
    // Loop through all genotypes binning the values
    do {
      assert(it->Get()->GetNumUnits() - 1 >= 0);
      assert(it->Get()->GetNumUnits() - 1 < hist.GetSize());
      hist[it->Get()->GetNumUnits() - 1]++;
    } while (it->Next());
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    for (int i = 0; i < hist.GetSize(); i++) df.Write(hist[i],"");
    df.Endl();
  }
};

// @TODO - needs species support
//class cActionPrintSpeciesAbundanceHistogram : public cAction
//{
//private:
//  cString m_filename;
//public:
//  cActionPrintSpeciesAbundanceHistogram(cWorld* world, const cString& args) : cAction(world, args)
//  {
//    cString largs(args);
//    if (largs == "") m_filename = "species_abundance_histogram.dat"; else m_filename = largs.PopWord();
//  }
//  
//  static const cString GetDescription() { return "Arguments: [string fname=\"species_abundance_histogram.dat\"]"; }
//  
//  void Process(cAvidaContext& ctx)
//  {
//    int max = 0;
//    
//    // Find max species abundance...
//    cClassificationManager& classmgr = m_world->GetClassificationManager();
//    cSpecies* cur_species = classmgr.GetFirstSpecies();
//    for (int i = 0; i < classmgr.GetNumSpecies(); i++) {
//      if (max < cur_species->GetNumOrganisms()) {
//        max = cur_species->GetNumOrganisms();
//      }
//      cur_species = cur_species->GetNext();
//    }
//    
//    // Allocate array for the histogram & zero it
//    tArray<int> hist(max);
//    hist.SetAll(0);
//    
//    // Loop through all species binning the values
//    cur_species = classmgr.GetFirstSpecies();
//    for (int i = 0; i < classmgr.GetNumSpecies(); i++) {
//      assert( cur_species->GetNumOrganisms() - 1 >= 0 );
//      assert( cur_species->GetNumOrganisms() - 1 < hist.GetSize() );
//      hist[cur_species->GetNumOrganisms() - 1]++;
//      cur_species = cur_species->GetNext();
//    }
//    
//    // Actual output
//    cDataFile& df = m_world->GetDataFile(m_filename);
//    df.Write(m_world->GetStats().GetUpdate(), "Update");
//    for (int i = 0; i < hist.GetSize(); i++) df.WriteAnonymous(hist[i]);
//    df.Endl();
//  }
//};

// @TODO - needs lineage support
//class cActionPrintLineageTotals : public cAction
//{
//private:
//  cString m_filename;
//  int m_verbose;
//public:
//  cActionPrintLineageTotals(cWorld* world, const cString& args) : cAction(world, args), m_verbose(1)
//  {
//    cString largs(args);
//    if (largs.GetSize()) m_filename = largs.PopWord(); else m_filename = "lineage_totals.dat";
//    if (largs.GetSize()) m_verbose = largs.PopWord().AsInt();
//  }
//  
//  static const cString GetDescription() { return "Arguments: [string fname='lineage_totals.dat'] [int verbose=1]"; }
//  
//  void Process(cAvidaContext& ctx)
//  {
//    if (!m_world->GetConfig().LOG_LINEAGES.Get()) {
//      m_world->GetDataFileOFStream(m_filename) << "No lineage data available!" << endl;
//      return;
//    }
//    m_world->GetClassificationManager().PrintLineageTotals(m_filename, m_verbose);
//  }
//};


// @TODO - needs lineage support
//class cActionPrintLineageCounts : public cAction
//{
//private:
//  cString m_filename;
//  int m_verbose;
//public:
//  cActionPrintLineageCounts(cWorld* world, const cString& args) : cAction(world, args), m_verbose(1)
//  {
//    cString largs(args);
//    if (largs.GetSize()) m_filename = largs.PopWord(); else m_filename = "lineage_counts.dat";
//    if (largs.GetSize()) m_verbose = largs.PopWord().AsInt();
//  }
//  
//  static const cString GetDescription() { return "Arguments: [string fname='lineage_counts.dat'] [int verbose=1]"; }
//  
//  void Process(cAvidaContext& ctx)
//  {
//    if (!m_world->GetConfig().LOG_LINEAGES.Get()) {
//      m_world->GetDataFileOFStream(m_filename) << "No lineage data available!" << endl;
//      return;
//    }
//    if (m_verbose) {    // verbose mode is the same in both methods
//      m_world->GetClassificationManager().PrintLineageTotals(m_filename, m_verbose);
//      return;
//    }
//    m_world->GetClassificationManager().PrintLineageCurCounts(m_filename);
//  }
//};


/*
 Write the currently dominant genotype to disk.
 
 Parameters:
 filename (string)
 The name under which the genotype should be saved. If no
 filename is given, the genotype is saved into the directory
 archive, under the name that the archive has associated with
 this genotype.
 */
class cActionPrintDominantGenotype : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintDominantGenotype(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    cBioGroup* bg = it->Next();
    if (bg) {
      cString filename(m_filename);
      if (filename == "") filename.Set("archive/%s.org", (const char*)bg->GetProperty("name").AsString());
      cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
      testcpu->PrintGenome(ctx, cGenome(bg->GetProperty("genome").AsString()), filename, m_world->GetStats().GetUpdate());
      delete testcpu;
    }
  }
};



/*
 This function prints out fitness data. The main point is that it
 calculates the average fitness from info from the testCPU + the actual
 merit of the organisms, and assigns zero fitness to those organisms
 that will never reproduce.
 
 The function also determines the maximum fitness genotype, and can
 produce fitness histograms.
 
 Parameters
 datafn (cString)
 Where the fitness data should be written.
 histofn (cString)
 Where the fitness histogram should be written.
 histotestfn (cString)
 Where the fitness histogram as determined exclusively from the test-CPU should be written.
 save_max_f_genotype (bool)
 Whether the genotype with the maximum fitness should be saved into the classmgr.
 print_fitness_histo (bool)
 Determines whether fitness histograms should be written.
 hist_fmax (double)
 The maximum fitness value to be taken into account for the fitness histograms.
 hist_fstep (double)
 The width of the individual bins in the fitness histograms.
 */
class cActionPrintDetailedFitnessData : public cAction
{
private:
  int m_save_max;
  int m_print_fitness_histo;
  double m_hist_fmax;
  double m_hist_fstep;
  cString m_filenames[3];
  
public:
  cActionPrintDetailedFitnessData(cWorld* world, const cString& args)
  : cAction(world, args), m_save_max(0), m_print_fitness_histo(0), m_hist_fmax(1.0), m_hist_fstep(0.1)
  {
    cString largs(args);
    if (largs.GetSize()) m_save_max = largs.PopWord().AsInt();
    if (largs.GetSize()) m_print_fitness_histo = largs.PopWord().AsInt();
    if (largs.GetSize()) m_hist_fmax = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_hist_fstep = largs.PopWord().AsDouble();
    if (!largs.GetSize()) m_filenames[0] = "fitness.dat"; else m_filenames[0] = largs.PopWord();
    if (!largs.GetSize()) m_filenames[1] = "fitness_histos.dat"; else m_filenames[1] = largs.PopWord();
    if (!largs.GetSize()) m_filenames[2] = "fitness_histos_testCPU.dat"; else m_filenames[2] = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [int save_max_f_genotype=0] [int print_fitness_histo=0] [double hist_fmax=1] [double hist_fstep=0.1] [string datafn=\"fitness.dat\"] [string histofn=\"fitness_histos.dat\"] [string histotestfn=\"fitness_histos_testCPU.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& pop = m_world->GetPopulation();
    const int update = m_world->GetStats().GetUpdate();
    const double generation = m_world->GetStats().SumGeneration().Average();
    
    // the histogram variables
    tArray<int> histo;
    tArray<int> histo_testCPU;
    int bins = 0;
    
    if (m_print_fitness_histo) {
      bins = static_cast<int>(m_hist_fmax / m_hist_fstep) + 1;
      histo.Resize(bins, 0);
      histo_testCPU.Resize(bins, 0 );
    }
    
    int n = 0;
    int nhist_tot = 0;
    int nhist_tot_testCPU = 0;
    double fave = 0;
    double fave_testCPU = 0;
    double max_fitness = -1; // we set this to -1, so that even 0 is larger...
    cBioGroup* max_f_genotype = NULL;
    
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    
    for (int i = 0; i < pop.GetSize(); i++) {
      if (pop.GetCell(i).IsOccupied() == false) continue;  // One use organisms.
      
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      cBioGroup* genotype = organism->GetBioGroup("genotype");
      
      cCPUTestInfo test_info;
      testcpu->TestGenome(ctx, test_info, cGenome(genotype->GetProperty("genome").AsString()));
      // We calculate the fitness based on the current merit,
      // but with the true gestation time. Also, we set the fitness
      // to zero if the creature is not viable.
      const double f = (test_info.IsViable()) ? organism->GetPhenotype().GetMerit().CalcFitness(test_info.GetTestPhenotype().GetGestationTime()) : 0;
      const double f_testCPU = test_info.GetColonyFitness();
      
      // Get the maximum fitness in the population
      // Here, we want to count only organisms that can truly replicate,
      // to avoid complications
      if (f_testCPU > max_fitness && test_info.GetTestPhenotype().CopyTrue()) {
        max_fitness = f_testCPU;
        max_f_genotype = genotype;
      }
      
      fave += f;
      fave_testCPU += f_testCPU;
      n += 1;
      
      
      // histogram
      if (m_print_fitness_histo && f < m_hist_fmax) {
        histo[static_cast<int>(f / m_hist_fstep)] += 1;
        nhist_tot += 1;
      }
      
      if (m_print_fitness_histo && f_testCPU < m_hist_fmax) {
        histo_testCPU[static_cast<int>(f_testCPU / m_hist_fstep)] += 1;
        nhist_tot_testCPU += 1;
      }
    }
    
    
    // determine the name of the maximum fitness genotype
    cString max_f_name;
    if (max_f_genotype->GetProperty("threshold").AsBool())
      max_f_name = max_f_genotype->GetProperty("name").AsString();
    else // we put the current update into the name, so that it becomes unique.
      max_f_name.Set("%03d-no_name-u%i", cGenome(max_f_genotype->GetProperty("genome").AsString()).GetSequence().GetSize(), update);
    
    cDataFile& df = m_world->GetDataFile(m_filenames[0]);
    df.Write(update, "Update");
    df.Write(generation, "Generation");
    df.Write(fave / static_cast<double>(n), "Average Fitness");
    df.Write(fave_testCPU / static_cast<double>(n), "Average Test Fitness");
    df.Write(n, "Organism Total");
    df.Write(max_fitness, "Maximum Fitness");
    df.Write(max_f_name, "Maxfit genotype name");
    df.Endl();
    
    if (m_save_max) {
      cString filename;
      filename.Set("archive/%s", static_cast<const char*>(max_f_name));
      testcpu->PrintGenome(ctx, cGenome(max_f_genotype->GetProperty("genome").AsString()), filename);
    }
    
    delete testcpu;
    
    if (m_print_fitness_histo) {
      cDataFile& hdf = m_world->GetDataFile(m_filenames[1]);
      hdf.Write(update, "Update");
      hdf.Write(generation, "Generation");
      hdf.Write(fave / static_cast<double>(n), "Average Fitness");
      
      // now output the fitness histo
      for (int i = 0; i < histo.GetSize(); i++)
        hdf.WriteAnonymous(static_cast<double>(histo[i]) / static_cast<double>(nhist_tot));
      hdf.Endl();
      
      
      cDataFile& tdf = m_world->GetDataFile(m_filenames[2]);
      tdf.Write(update, "Update");
      tdf.Write(generation, "Generation");
      tdf.Write(fave / static_cast<double>(n), "Average Fitness");
      
      // now output the fitness histo
      for (int i = 0; i < histo_testCPU.GetSize(); i++)
        tdf.WriteAnonymous(static_cast<double>(histo_testCPU[i]) / static_cast<double>(nhist_tot_testCPU));
      tdf.Endl();
    }
  }
};


/*
 This function requires that TRACK_CCLADES be enabled and avida is
 not in analyze mode.
 
 Parameters
 filename (cString)
 Where the clade information should be stored.
 
 Please note the structure to this file is not a matrix.
 Each line is formatted as follows: 
 update number_cclades ccladeID0 ccladeID0_count ccladeID1 
 
 @MRR May 2007
 */
class cActionPrintCCladeCounts : public cAction
{
private:
  cString filename;
  bool first_time;
  
public:
  cActionPrintCCladeCounts(cWorld* world, const cString& args)
  : cAction(world, args)
  {
    cString largs(args);
    filename = (!largs.GetSize()) ? "cclade_count.dat" : largs.PopWord();
    first_time = true;
  }
  
  static const cString GetDescription() { return "Arguments: [filename = \"cclade_count.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    //Handle possible errors
    if (ctx.GetAnalyzeMode())
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeCount requires avida to be in run mode.");
    
    if (m_world->GetConfig().TRACK_CCLADES.Get() == 0)
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeCount requires coalescence clade tracking to be enabled.");
    
    
    tHashMap<int, int> cclade_count;  //A count for each clade in the population
    set<int>             clade_ids;
    
    cPopulation& pop = m_world->GetPopulation();
    const int update = m_world->GetStats().GetUpdate();
    
    //For each organism in the population, find what coalescence clade it belongs to and count
    for (int k = 0; k < pop.GetSize(); k++)
    {
      if (!pop.GetCell(k).IsOccupied())
        continue;
      int cclade_id = pop.GetCell(k).GetOrganism()->GetCCladeLabel();
      int count = 0;
      if (!cclade_count.Find(cclade_id,count))
        clade_ids.insert(cclade_id);
      cclade_count.Set(cclade_id, ++count);
    }
    
    ofstream& fp = m_world->GetDataFileManager().GetOFStream(filename);
    if (!fp.is_open())
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeCount: Unable to open output file.");
    if (first_time)
    {
      fp << "# Each line is formatted as follows:" << endl;
      fp << "#   update number_cclades ccladeID0 ccladeID0_count ccladeID1" << endl;
      fp << endl;
      first_time = false;
    }
    fp << update <<  " "
    << clade_ids.size() << " ";
    
    set<int>::iterator sit = clade_ids.begin();
    while(sit != clade_ids.end())
    {
      int count = 0;
      cclade_count.Find(*sit, count);
      fp << *sit << " " << count << " ";
      sit++;
    }
    fp << endl;
    
  }
};


/*
 @MRR May 2007 [BETA]
 This function prints out fitness data. The main point is that it
 calculates the average fitness from info from the testCPU + the actual
 merit of the organisms, and assigns zero fitness to those organisms
 that will never reproduce.
 
 The function also determines the maximum fitness genotype, and can
 produce fitness histograms.
 
 This version of the DetailedFitnessData prints the information as a log histogram.
 
 THIS FUNCTION CONTAINS STATIC METHODS USED IN OTHER PRINT ACTION CLASSES.
 MOVEMENT OF THIS FUNCTION TO A LOWER POINT IN THE FILE MAY CAUSE CONFLICTS.
 
 Parameters:
 filename   (cString)     Where the fitness histogram should be written.
 fit_mode   (cString)     Either {Current, Actual, TestCPU}, where
 Current is the current value in the grid.  [Default]
 Actual uses the current merit, but the true gestation time.
 TestCPU determined.
 hist_fmin  (double)      The minimum fitness value for the fitness histogram.  [Default: -3]
 hist_fmax  (double)      The maximum fitness value for the fitness histogram.  [Default: 12]
 hist_fstep (double)      The width of the individual bins in the histogram.    [Default: 0.5]
 */
class cActionPrintLogFitnessHistogram : public cAction
{
private:
  
  double m_hist_fmin;
  double m_hist_fstep;
  double m_hist_fmax;
  cString m_mode;
  cString m_filename;
  
public:
  cActionPrintLogFitnessHistogram(cWorld* world, const cString& args)
  : cAction(world, args)
  {
    cString largs(args);
    m_filename   = (largs.GetSize()) ? largs.PopWord()           : "fitness_log_hist.dat";
    m_mode       = (largs.GetSize()) ? largs.PopWord().ToUpper() : "CURRENT";
    m_hist_fmin  = (largs.GetSize()) ? largs.PopWord().AsDouble(): -3.0;
    m_hist_fstep = (largs.GetSize()) ? largs.PopWord().AsDouble(): 0.5;
    m_hist_fmax  = (largs.GetSize()) ? largs.PopWord().AsDouble(): 12;
  }
  
  static const cString GetDescription() { return  "Parameters: <filename> <mode> <min> <step> <max>";}
  
  //Given a min:step:max and bin number, return a string reprsenting the range of fitness values.
  //This function may be called from other classes.
  static cString GetHistogramBinLabel(int k, double min, double step, double max)
  {
    int num_bins = static_cast<int>(ceil( (max - min) / step)) + 3;
    cString retval; 
    
    if (k == 0)
      retval = "Inviable";
    else if (k == 1)
      retval = cString("[<") + cStringUtil::Convert(min) + ", " + cStringUtil::Convert(min) + cString(")");
    else if (k < num_bins - 1)
      retval = cString("(") + cStringUtil::Convert(min+step*(k-2)) 
      + cString(", ") + cStringUtil::Convert(min+step*(k-1)) +
      + cString("]");
    else
      retval = cString("[>") + cStringUtil::Convert(max) + cString("]");
    return retval;
  }
  
  
  //This function may get called by outside classes to generate a histogram of log10 fitnesses;
  //max may be updated by this function if the range is not evenly divisible by the step
  static tArray<int> MakeHistogram(const tArray<cOrganism*>& orgs, const tArray<cBioGroup*>& gens, 
                                   double min, double step, double& max, const cString& mode, cWorld* world,
                                   cAvidaContext& ctx)
  {
    //Set up histogram; extra columns prepended (non-viable, < m_hist_fmin) and appended ( > f_hist_fmax)
    //If the bin size is not a multiple of the step size, the last bin is expanded to make it a multiple.
    //All bins are [min, max)
    tArray<int> histogram;
    int num_bins = static_cast<int>(ceil( (max - min) / step)) + 3;
    max  = min + (num_bins - 3) * step;
    histogram.Resize(num_bins, 0);
    cTestCPU* testcpu = world->GetHardwareManager().CreateTestCPU();
    
    
    // We calculate the fitness based on the current merit,
    // but with the true gestation time. Also, we set the fitness
    // to zero if the creature is not viable.
    tArray<cBioGroup*>::const_iterator git;
    tArray<cOrganism*>::const_iterator oit;
    for (git = gens.begin(), oit = orgs.begin(); git != gens.end(); git++, oit++){
      cCPUTestInfo test_info;
      double fitness = 0.0;
      if (mode == "TEST_CPU" || mode == "ACTUAL"){
        test_info.UseManualInputs( (*oit)->GetOrgInterface().GetInputs() );
        testcpu->TestGenome(ctx, test_info, cGenome((*git)->GetProperty("genome").AsString()));
      }
      
      if (mode == "TEST_CPU"){
        fitness = test_info.GetColonyFitness();
      }
      else if (mode == "CURRENT"){
        fitness = (*oit)->GetPhenotype().GetFitness();
      }
      else if (mode == "ACTUAL"){
        fitness = (test_info.IsViable()) ? 
        (*oit)->GetPhenotype().GetMerit().CalcFitness(test_info.GetTestPhenotype().GetGestationTime()) : 0.0;
      }
      else
        world->GetDriver().RaiseFatalException(1, "PrintLogFitnessHistogram::MakeHistogram: Invalid fitness mode requested.");
      
      //Update the histogram
      int update_bin = (fitness == 0) ? 0 :    
      static_cast<int>((log10(fitness) - min) / step);
      
      // Bin 0   Inviable
      //     1   Below Range
      //     2   [min, min+step)
      // #bin-1  [max-step, max)
      // num_bin Above Range
      
      if (fitness == 0)
        update_bin = 0;
      else if (log10(fitness) < min)
        update_bin = 1;
      else if (log10(fitness) > max)
        update_bin = num_bins - 1;
      else
        update_bin = static_cast<int>(log10(fitness) - min / step) + 2;
      
      histogram[update_bin]++;
    }
    delete testcpu;
    return histogram;
  }
  
  void Process(cAvidaContext& ctx)
  {
    //Verify input parameters
    if ( (m_mode != "ACTUAL" && m_mode != "CURRENT" && m_mode != "TESTCPU") ||
        m_hist_fmin > m_hist_fmax)
    {
      cerr << "cActionPrintFitnessHistogram: Please check arguments.  Abort.\n";
      cerr << "Parameters: " << m_filename << ", " << m_mode << ", " << m_hist_fmin << ":" << m_hist_fstep << ":" << m_hist_fmax << endl;
      return;
    }
    cerr << "Parameters: " << m_filename << ", " << m_mode << ", " << m_hist_fmin << ":" << m_hist_fstep << ":" << m_hist_fmax << endl;
    
    
    //Gather data objects
    cPopulation& pop        = m_world->GetPopulation();
    const int    update     = m_world->GetStats().GetUpdate();
    const double generation = m_world->GetStats().SumGeneration().Average();
    tArray<cOrganism*> orgs;
    tArray<cBioGroup*> gens;
    
    for (int i = 0; i < pop.GetSize(); i++)
    {
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      cBioGroup* genotype = organism->GetBioGroup("genotype");
      orgs.Push(organism);
      gens.Push(genotype);
    }
    
    tArray<int> histogram = MakeHistogram(orgs, gens, m_hist_fmin, m_hist_fstep, m_hist_fmax, m_mode, m_world, ctx);
    
    
    //Output histogram
    cDataFile& hdf = m_world->GetDataFile(m_filename);
    hdf.Write(update, "Update");
    hdf.Write(generation, "Generation");
    
    for (int k = 0; k < histogram.GetSize(); k++)
      hdf.Write(histogram[k], GetHistogramBinLabel(k, m_hist_fmin, m_hist_fstep, m_hist_fmax));
    hdf.Endl();
  }
};



/*
 @MRR May 2007  [BETA]
 
 This function requires Avida be in run mode.
 
 This function will print histograms of the relative fitness of
 organisms as compared to the parent.
 
 STATIC METHODS IN THIS CLASS ARE CALLED BY OTHER ACTIONS.
 MOVING THIS CLASS MAY BREAK DEPENDENCIES.
 
 Parameters:
 filename  (cString)        Name of the output file
 fit_mode (cString)        Either {Current, Actual, TestCPU}, where
 Current is the current value in the grid. [Default]
 Actual uses the current merit, but the true gestation time.
 that have reproduced.
 TestCPU determined.
 hist_fmin  (double)      The minimum fitness value for the fitness histogram.  [Default: 0.50]
 hist_fmax  (double)      The maximum fitness value for the fitness histogram.  [Default: 0.02]
 hist_fstep (double)      The width of the individual bins in the histogram.    [Default: 1.50]
 
 The file will be formatted:
 <update>  [ <min, min, min+step, ..., max-step, max, >max], each bin [min,max)
 */
class cActionPrintRelativeFitnessHistogram : public cAction
{
private:
  double m_hist_fmin;
  double m_hist_fstep;
  double m_hist_fmax;
  cString m_mode;
  cString m_filename;
  bool    first_run;
  
public:
  cActionPrintRelativeFitnessHistogram(cWorld* world, const cString& args) : cAction(world, args) 
  { 
    cString largs(args);
    m_filename   = (largs.GetSize()) ? largs.PopWord()           : "rel_fitness_hist.dat";
    m_mode       = (largs.GetSize()) ? largs.PopWord().ToUpper() : "CURRENT";
    m_hist_fmin  = (largs.GetSize()) ? largs.PopWord().AsDouble(): 0;
    m_hist_fstep = (largs.GetSize()) ? largs.PopWord().AsDouble(): 0.1;
    m_hist_fmax  = (largs.GetSize()) ? largs.PopWord().AsDouble(): 2;
  }
  
  static const cString GetDescription() { return "Arguments: [filename] [fit_mode] [hist_min] [hist_step] [hist_max]"; }
  
  
  static cString GetHistogramBinLabel(int k, double min, double step, double max)
  {
    int num_bins = static_cast<int>(ceil( (max - min) / step)) + 2;
    cString retval; 
    
    if (k == 0)
      retval = "Inviable";
    else if (k == 1)
      retval = cString("[<") + cStringUtil::Convert(min) + ", " + cStringUtil::Convert(min) + cString(")");
    else if (k < num_bins - 1)
      retval = cString("(") + cStringUtil::Convert(min+step*(k-2)) 
      + cString(", ") + cStringUtil::Convert(min+step*(k-1)) +
      + cString("]");
    else
      retval = cString("[>") + cStringUtil::Convert(max) + cString("]");
    
    return retval;
  }
  
  static tArray<int> MakeHistogram(const tArray<cOrganism*>& orgs, const tArray<cBioGroup*>& gens, 
                                   double min, double step, double& max, const cString& mode, cWorld* world,
                                   cAvidaContext& ctx)
  {
    //Set up histogram; extra columns prepended (non-viable, < m_hist_fmin) and appended ( > f_hist_fmax)
    //If the bin size is not a multiple of the step size, the last bin is expanded to make it a multiple.
    //All bins are [min, max)
    tArray<int> histogram;
    int num_bins = static_cast<int>(ceil( (max - min) / step)) + 3;
    max  = min + (num_bins - 3) * step;
    histogram.Resize(num_bins, 0);
    cTestCPU* testcpu = world->GetHardwareManager().CreateTestCPU();
    
    
    // We calculate the fitness based on the current merit,
    // but with the true gestation time. Also, we set the fitness
    // to zero if the creature is not viable.
    tArray<cBioGroup*>::const_iterator git;
    tArray<cOrganism*>::const_iterator oit;
    for (git = gens.begin(), oit = orgs.begin(); git != gens.end(); git++, oit++){
      cCPUTestInfo test_info;
      double fitness = 0.0;
      double parent_fitness = 1.0;
      if ((*git)->GetProperty("parents") != "") {
        cStringList parents((*git)->GetProperty("parents").AsString(), ',');
        
        cBioGroup* pbg = world->GetClassificationManager().GetBioGroupManager("genotype")->GetBioGroup(parents.Pop().AsInt());
        parent_fitness = pbg->GetProperty("fitness").AsDouble();
      }
      
      if (mode == "TEST_CPU" || mode == "ACTUAL"){
        test_info.UseManualInputs( (*oit)->GetOrgInterface().GetInputs() );  
        testcpu->TestGenome(ctx, test_info, cGenome((*git)->GetProperty("genome").AsString()));
      }
      
      if (mode == "TEST_CPU"){
        fitness = test_info.GetColonyFitness();
      }
      else if (mode == "CURRENT"){
        fitness = (*oit)->GetPhenotype().GetFitness();
      }
      else if (mode == "ACTUAL"){
        fitness = (test_info.IsViable()) ? 
        (*oit)->GetPhenotype().GetMerit().CalcFitness(test_info.GetTestPhenotype().GetGestationTime()) : 0.0;
      }
      else
        world->GetDriver().RaiseFatalException(1, "MakeHistogram: Invalid fitness mode requested.");
      
      //Update the histogram
      if (parent_fitness <= 0.0)
        world->GetDriver().RaiseFatalException(1, cString("PrintRelativeFitness::MakeHistogram reports a parent fitness is zero.") + (*git)->GetProperty("parents").AsString());
      
      int update_bin = 0;
      double rfitness = fitness/parent_fitness;        
      
      if (fitness == 0.0)
        update_bin = 0;
      else if (rfitness < min)
        update_bin = 1;
      else if (rfitness > max)
        update_bin = num_bins - 1;
      else
        update_bin = static_cast<int>( ((fitness/parent_fitness) - min) / step) + 2;
      
      histogram[update_bin]++;
    }
    delete testcpu;
    return histogram;
  }
  
  
  
  void Process(cAvidaContext& ctx)
  {
    //Handle possible errors
    if (ctx.GetAnalyzeMode())
      m_world->GetDriver().RaiseFatalException(1, "PrintRelativeFitnessHistogram requires avida to be in run mode.");
    
    //Gather data objects
    cPopulation& pop        = m_world->GetPopulation();
    const int    update     = m_world->GetStats().GetUpdate();
    const double generation = m_world->GetStats().SumGeneration().Average();
    tArray<cOrganism*> orgs;
    tArray<cBioGroup*> gens;
    
    for (int i = 0; i < pop.GetSize(); i++)
    {
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      cBioGroup* genotype = organism->GetBioGroup("genotype");
      orgs.Push(organism);
      gens.Push(genotype);
    }
    
    tArray<int> histogram = MakeHistogram(orgs, gens, m_hist_fmin, m_hist_fstep, m_hist_fmax, m_mode, m_world, ctx);
    
    
    //Output histogram
    cDataFile& hdf = m_world->GetDataFile(m_filename);
    hdf.Write(update, "Update");
    hdf.Write(generation, "Generation");
    
    for (int k = 0; k < histogram.GetSize(); k++)
      hdf.Write(histogram[k], GetHistogramBinLabel(k, m_hist_fmin, m_hist_fstep, m_hist_fmax));
    hdf.Endl();
  }
};



/*
 @MRR May 2007 [BETA]
 This function requires CCLADE_TRACKING to be enabled and avida
 operating non-analyze mode.
 
 This function will print histograms of log10 fitness of each of the
 tagged clades.
 
 Parameters:
 filename  (cString)        Name of the output file
 fit_mode (cString)        Either {Current, Actual, TestCPU}, where
 Current is the current value in the grid. [Default]
 Actual uses the current merit, but the true gestation time.
 TestCPU determined.
 hist_fmin  (double)      The minimum fitness value for the fitness histogram.  [Default: -3]
 hist_fmax  (double)      The maximum fitness value for the fitness histogram.  [Default: 12]
 hist_fstep (double)      The width of the individual bins in the histogram.    [Default: 0.5]
 
 The file will be formatted:
 <update> <cclade_count> <cclade_id> [...] <cclade_id> [...] ...
 where [...] will be [ <min, min, min+step, ..., max-step, max, > max], each bin (min,max]
 */
class cActionPrintCCladeFitnessHistogram : public cAction
{
private:
  double m_hist_fmin;
  double m_hist_fstep;
  double m_hist_fmax;
  cString m_mode;
  cString m_filename;
  bool    first_run;
  
public:
  cActionPrintCCladeFitnessHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    m_filename   = (largs.GetSize()) ? largs.PopWord()           : "cclade_fitness_hist.dat";
    m_mode       = (largs.GetSize()) ? largs.PopWord().ToUpper() : "CURRENT";
    m_hist_fmin  = (largs.GetSize()) ? largs.PopWord().AsDouble(): -3.0;
    m_hist_fstep = (largs.GetSize()) ? largs.PopWord().AsDouble(): 0.5;
    m_hist_fmax  = (largs.GetSize()) ? largs.PopWord().AsDouble(): 12;
    first_run = true; 
  }
  
  static const cString GetDescription() { return "Arguments: [filename] [fit_mode] [hist_min] [hist_step] [hist_max]"; }
  
  void Process(cAvidaContext& ctx)
  {
    //Handle possible errors
    if (ctx.GetAnalyzeMode())
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeFitnessHistogram requires avida to be in run mode.");
    
    if (m_world->GetConfig().TRACK_CCLADES.Get() == 0)
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeFitnessHistogram requires coalescence clade tracking to be enabled.");        
    
    //Verify input parameters
    if ( (m_mode != "ACTUAL" && m_mode != "CURRENT" && m_mode != "TESTCPU") ||
        m_hist_fmin > m_hist_fmax)
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeFitnessHistogram: Check parameters.");
    
    //Gather data objects
    cPopulation& pop        = m_world->GetPopulation();
    const int    update     = m_world->GetStats().GetUpdate();
    map< int, tArray<cOrganism*> > org_map;  //Map of ccladeID to array of organism IDs
    map< int, tArray<cBioGroup*> > gen_map;  //Map of ccladeID to array of genotype IDs 
    
    //Collect clade information
    for (int i = 0; i < pop.GetSize(); i++){
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      cBioGroup* genotype = organism->GetBioGroup("genotype");
      int cladeID = organism->GetCCladeLabel();
      
      map< int, tArray<cOrganism*> >::iterator oit = org_map.find(cladeID);
      map< int, tArray<cBioGroup*> >::iterator git = gen_map.find(cladeID);
      if (oit == org_map.end()) {
        //The clade is new
        org_map[cladeID] = tArray<cOrganism*>(1, organism);
        gen_map[cladeID] = tArray<cBioGroup*>(1, genotype);
      } else { 
        //The clade is known
        oit->second.Push(organism);
        git->second.Push(genotype);
      }
    }
    
    //Create and print the histograms; this calls a static method in another action
    ofstream& fp = m_world->GetDataFileManager().GetOFStream(m_filename);
    if (!fp.is_open())
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeFitnessHistogram: Unable to open output file.");
    map< int, tArray<cOrganism*> >::iterator oit = org_map.begin();
    map< int, tArray<cBioGroup*> >::iterator git = gen_map.begin();
    for (; oit != org_map.end(); oit++, git++) {
      tArray<int> hist = 
      cActionPrintLogFitnessHistogram::MakeHistogram((oit->second), (git->second), m_hist_fmin, m_hist_fstep, m_hist_fmax, m_mode, m_world, ctx );
      if (first_run) {
        // Print header information if first time through
        first_run = false;
        fp << "# PrintCCladeFitnessHistogram" << endl << "# Bins: ";
        for (int k = 0; k < hist.GetSize(); k++)
          fp << " " <<  cActionPrintLogFitnessHistogram::GetHistogramBinLabel(k, m_hist_fmin, m_hist_fstep, m_hist_fmax);
        fp << endl << endl;
      }
      
      if (oit == org_map.begin()) {
        // Print update and clade count if first clade
        fp << update << " " << org_map.size() << " ";
      }
      
      fp << oit->first << " [";
      for (int k = 0; k < hist.GetSize(); k++) fp << " " << hist[k];
      fp << " ] ";
    }
    fp << endl;
  }
};



/*
 @MRR May 2007  [BETA]
 This function requires CCLADE_TRACKING to be enabled and Avida
 operating non-analyze mode.
 
 This function will print histograms of the relative fitness of
 clade members as compared to the parent.
 
 Parameters:
 filename  (cString)        Name of the output file
 fit_mode (cString)        Either {Current, Actual, ActualRepro, TestCPU}, where
 Current is the current value in the grid. [Default]
 Actual uses the current merit, but the true gestation time.
 CurrentRepro is the same as current, but counts only those orgs
 that have reproduced.
 TestCPU determined.
 hist_fmin  (double)      The minimum fitness value for the fitness histogram.  [Default: 0.50]
 hist_fmax  (double)      The maximum fitness value for the fitness histogram.  [Default: 0.02]
 hist_fstep (double)      The width of the individual bins in the histogram.    [Default: 1.50]
 
 The file will be formatted:
 <update> <cclade_count> <cclade_id> [...] <cclade_id> [...] ...
 where [...] will be [ <min, min, min+step, ..., max-step, max, >max], each bin [min,max}
 */
class cActionPrintCCladeRelativeFitnessHistogram : public cAction
{
private:
  double m_hist_fmin;
  double m_hist_fstep;
  double m_hist_fmax;
  cString m_mode;
  cString m_filename;
  bool first_run;
  
public:
  cActionPrintCCladeRelativeFitnessHistogram(cWorld* world, const cString& args) : cAction(world, args) 
  {
    cString largs(args);
    m_filename   = (largs.GetSize()) ? largs.PopWord()           : "cclade_rel_fitness_hist.dat";
    m_mode       = (largs.GetSize()) ? largs.PopWord().ToUpper() : "CURRENT";
    m_hist_fmin  = (largs.GetSize()) ? largs.PopWord().AsDouble(): 0;
    m_hist_fstep = (largs.GetSize()) ? largs.PopWord().AsDouble(): 0.2;
    m_hist_fmax  = (largs.GetSize()) ? largs.PopWord().AsDouble(): 2.0;
    first_run = true;
  }
  
  static const cString GetDescription() { return "Arguments: [filename] [fit_mode] [hist_min] [hist_step] [hist_max]"; }
  
  void Process(cAvidaContext& ctx)
  {
    //Handle possible errors
    if (ctx.GetAnalyzeMode())
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeRelativeFitnessHistogram requires avida to be in run mode.");
    
    if (m_world->GetConfig().TRACK_CCLADES.Get() == 0)
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeRelativeFitnessHistogram requires coalescence clade tracking to be enabled.");
    
    //Verify input parameters
    if ( (m_mode != "ACTUAL" && m_mode != "CURRENT" && m_mode != "TESTCPU") ||
        m_hist_fmin > m_hist_fmax)
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeRelativeFitness: check parameters");
    
    ///Gather data objects
    cPopulation& pop        = m_world->GetPopulation();
    const int    update     = m_world->GetStats().GetUpdate();
    map< int, tArray<cOrganism*> > org_map;  //Map of ccladeID to array of organism IDs
    map< int, tArray<cBioGroup*> > gen_map;  //Map of ccladeID to array of genotype IDs 
    
    //Collect clade information
    for (int i = 0; i < pop.GetSize(); i++) {
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      cBioGroup* genotype = organism->GetBioGroup("genotype");
      int cladeID = organism->GetCCladeLabel();
      
      map< int, tArray<cOrganism*> >::iterator oit = org_map.find(cladeID);
      map< int, tArray<cBioGroup*> >::iterator git = gen_map.find(cladeID);
      if (oit == org_map.end()) {
        // The clade is new
        org_map[cladeID] = tArray<cOrganism*>(1, organism);
        gen_map[cladeID] = tArray<cBioGroup*>(1, genotype);
      } else {
        // The clade is known
        oit->second.Push(organism);
        git->second.Push(genotype);
      }
    }
    
    //Create and print the histograms; this calls a static method in another action
    ofstream& fp = m_world->GetDataFileManager().GetOFStream(m_filename);
    if (!fp.is_open())
      m_world->GetDriver().RaiseFatalException(1, "PrintCCladeRelativeFitnessHistogram: Unable to open output file.");
    map< int, tArray<cOrganism*> >::iterator oit = org_map.begin();
    map< int, tArray<cBioGroup*> >::iterator git = gen_map.begin();
    for (; oit != org_map.end(); oit++, git++) {
      tArray<int> hist = cActionPrintRelativeFitnessHistogram::MakeHistogram( (oit->second), (git->second),
                                                                             m_hist_fmin, m_hist_fstep, m_hist_fmax,
                                                                             m_mode, m_world, ctx );
      if (first_run){  //Print header information if first time through
        first_run = false;
        fp << "# PrintCCladeFitnessHistogram" << endl << "# Bins: ";
        for (int k = 0; k < hist.GetSize(); k++)
          fp << " " <<  cActionPrintRelativeFitnessHistogram::GetHistogramBinLabel(k, m_hist_fmin, m_hist_fstep, m_hist_fmax);
        fp << endl << endl;
      }
      if (oit == org_map.begin()) //Print update and clade count if first clade
        fp << update << " " << org_map.size() << " ";
      fp << oit->first << " [";
      for (int k = 0; k < hist.GetSize(); k++)
        fp << " " << hist[k];
      fp << " ] ";
    }
    fp << endl;
    
  }
};






/*
 @MRR March 2007 [UNTESTED]
 This function will take the initial genotype for each organism in the
 population/batch, align them, and calculate the per-site entropy of the
 aligned sequences.  Please note that there may be a variable number
 of columns in each line if the runs are not fixed length.  The site
 entropy will be measured in mers, normalized by the instruction set size.
 This is a population/batch measure of entropy, not a mutation-selection balance
 measure.  
 */
class cActionPrintGenomicSiteEntropy : public cAction
{
private:
  cString m_filename;
  bool    m_use_gap;
  
public:
  cActionPrintGenomicSiteEntropy(cWorld* world, const cString& args) : cAction(world, args){
    cString largs = args;
    m_filename = (largs.GetSize()) ? largs.PopWord() : "GenomicSiteEntropy.dat";
  }
  
  static const cString GetDescription() { return "Arguments: [filename = \"GenomicSiteEntropyData.dat\"]";}
  
  void Process(cAvidaContext& ctx) 
  {
    const int        num_insts  = m_world->GetHardwareManager().GetDefaultInstSet().GetSize();
    tArray<cString> aligned;  //This will hold all of our aligned sequences
    
    if (ctx.GetAnalyzeMode()) //We're in analyze mode, so process the current batch
    {
      cAnalyze& analyze = m_world->GetAnalyze();  
      if (!analyze.GetCurrentBatch().IsAligned()) analyze.AlignCurrentBatch(); //Let analyze take charge of aligning this batch
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while((genotype = batch_it.Next()))
      {
        aligned.Push(genotype->GetAlignedSequence());
      }
    }
    else //We're not in analyze mode, process the population
    {
      cPopulation& pop = m_world->GetPopulation();
      for (int i = 0; i < pop.GetSize(); i++)
      {
        if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
        aligned.Push(pop.GetCell(i).GetOrganism()->GetGenome().GetSequence().AsString());
      }
      AlignStringArray(aligned);  //Align our population genomes
    }
    
    //With all sequences aligned and stored, we can proceed to calculate per-site entropies
    if (!aligned.GetSize())
    {
      m_world->GetDriver().NotifyComment("cActionPrintGenomicSiteEntropy: No sequences available.  Abort.");
      return;
    }
    
    const int gen_size = aligned[0].GetSize();
    tArray<double> site_entropy(gen_size);
    site_entropy.SetAll(0.0);
    
    tArray<int> inst_count( (m_use_gap) ? num_insts + 1 : num_insts);  //Add an extra place if we're using gaps
    inst_count.SetAll(0);
    for (int pos = 0; pos < gen_size; pos++)
    {
      inst_count.SetAll(0);  //Reset the counter for each aligned position
      int total_count = 0;
      for (int seq = 0; seq < aligned.GetSize(); seq++)
      {
        char ch = aligned[seq][pos];
        if (ch == '_' && !m_use_gap) continue;                  //Skip gaps when applicable
        else if (ch == '_') site_entropy[num_insts]++;          //Update gap count at end
        else inst_count[ cInstruction::ConvertSymbol(ch) ]++;   //Update true instruction count
        total_count++;
      }
      for (int c = 0; c < inst_count.GetSize(); c++)
      {
        double p = (inst_count[c] > 0) ? inst_count[c] / static_cast<double>(total_count) : 0.0;
        site_entropy[pos] += (p > 0.0) ? - p * log(p) / log(static_cast<double>(inst_count.GetSize())) : 0.0;
      }
    }
  }
  
  
private:
  void AlignStringArray(tArray<cString>& unaligned)  //Taken from cAnalyze::CommandAnalyze
  {
    // Create an array of all the sequences we need to align.
    const int num_sequences = unaligned.GetSize();
    
    // Move through each sequence an update it.
    cString diff_info;
    for (int i = 1; i < num_sequences; i++) {
      // Track of the number of insertions and deletions to shift properly.
      int num_ins = 0;
      int num_del = 0;
      
      // Compare each string to the previous.
      cStringUtil::EditDistance(unaligned[i], unaligned[i-1], diff_info, '_');
      while (diff_info.GetSize() != 0) {
        cString cur_mut = diff_info.Pop(',');
        const char mut_type = cur_mut[0];
        cur_mut.ClipFront(1); cur_mut.ClipEnd(1);
        int position = cur_mut.AsInt();
        if (mut_type == 'M') continue;   // Nothing to do with Mutations
        
        if (mut_type == 'I') {           // Handle insertions
          for (int j = 0; j < i; j++)    // Loop back and insert an '_' into all previous sequences
            unaligned[j].Insert('_', position + num_del);
          num_ins++;
        }
        
        else if (mut_type == 'D'){      // Handle Deletions
          // Insert '_' into the current sequence at the point of deletions.
          unaligned[i].Insert("_", position + num_ins);
          num_del++;
        }
      }
    }
  }
};



/*
 This function will go through all genotypes in the population/batch and
 allow you to retrieve information about the different plastic phenotypes.
 Arguments:
 filename    name of output file in analyze mode; root of filename in
 run mode (-update.dat appeneded in run mode).
 [default: phenpalst-update.dat in run-mode, phenplast.dat in analyze]
 trials      number of test_cpu recalculations for each genotype [default: 1000]
 */
class cActionPrintPhenotypicPlasticity : public cAction
{
private:
  cString m_filename;
  int     m_num_trials;
  
private:
  void PrintHeader(ofstream& fot)
  {
    fot << "# Phenotypic Plasticity" << endl
    << "# Format: " << endl
    << "# genotype id" << endl
    << "# parent genotype id" << endl
    << "# phenotypic varient number" << endl
    << "# varient frequency" << endl
    << "# fitness" << endl
    << "# merit" << endl
    << "# gestation time" << endl;
    for (int k = 0; k < m_world->GetEnvironment().GetNumTasks(); k++)
      fot << "# task." << k << endl;
    for (int k = 0; k < m_world->GetEnvironment().GetInputSize(); k++)
      fot << "# env_input." << k << endl;
    fot << endl;
  }
  
  void PrintPPG(ofstream& fot, tAutoRelease<cPhenPlastGenotype>& ppgen, int id, const cString& pid)
  {
    
    for (int k = 0; k < ppgen->GetNumPhenotypes(); k++){
      const cPlasticPhenotype* pp = ppgen->GetPlasticPhenotype(k);
      fot << id << " "
      << pid << " "
      << k << " "
      << pp->GetFrequency() << " "
      << pp->GetFitness() << " "
      << pp->GetMerit() << " "
      << pp->GetGestationTime() << " ";
      tArray<int> tasks = pp->GetLastTaskCount();
      for (int t = 0; t < tasks.GetSize(); t++)
        fot << tasks[t] << " ";
      tArray<int> env_inputs = pp->GetEnvInputs();
      for (int e = 0; e < env_inputs.GetSize(); e++)
        fot << env_inputs[e] << " ";
      fot << endl;
      
    }
  }
  
public:
  cActionPrintPhenotypicPlasticity(cWorld* world, const cString& args)
  : cAction(world,  args)
  {
    cString largs(args);
    m_filename = (largs.GetSize()) ? largs.PopWord() : "phenplast";
    m_num_trials = (largs.GetSize()) ? largs.PopWord().AsInt() : 1000;
  }
  
  static const cString GetDescription() { return "Arguments: [string filename='phenplast'] [int num_trials=1000]"; };
  
  void Process(cAvidaContext& ctx)
  {
    cCPUTestInfo test_info;
    
    if (ctx.GetAnalyzeMode()){ // Analyze mode
      cString this_path = m_filename;
      ofstream& fot = m_world->GetDataFileOFStream(this_path);
      PrintHeader(fot);
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while((genotype = batch_it.Next())){
        tAutoRelease<cPhenPlastGenotype> ppgen(new cPhenPlastGenotype(genotype->GetGenome(), m_num_trials, test_info, m_world, ctx));
        PrintPPG(fot, ppgen, genotype->GetID(), genotype->GetParents());
      }
      m_world->GetDataFileManager().Remove(this_path);
    } else{  // Run mode
      cString this_path = m_filename + "-" + cStringUtil::Convert(m_world->GetStats().GetUpdate()) + ".dat";
      ofstream& fot = m_world->GetDataFileOFStream(this_path);
      PrintHeader(fot);
      
      tAutoRelease<tIterator<cBioGroup> > it;
      it.Set(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
      while (it->Next()) {
        cBioGroup* bg = it->Get();
        tAutoRelease<cPhenPlastGenotype> ppgen(new cPhenPlastGenotype(cGenome(bg->GetProperty("genome").AsString()), m_num_trials, test_info, m_world, ctx));
        PrintPPG(fot, ppgen, bg->GetID(), bg->GetProperty("parents").AsString());
      }
      m_world->GetDataFileManager().Remove(this_path);
    }
  }
};


/*
 @MRR May 2009
 This function will go through all genotypes in the current batch or run-time population
 and print a task probability histogram for each task in the environment.
 
 Paramters:
 m_fillename (cString)
 The output file
 m_weighted  (int)
 Should abundances be weighted by num_cpus
 */


class cActionPrintTaskProbHistogram : public cAction
{
private:
  cString m_filename;  //Name of the output file
  bool    m_first_run; //Is this the first time the process is run?
  bool    m_weighted;  //Weight by num_cpu?
  
  void PrintHeader(ofstream& fot){
    fot << "# Task Probability Histogram" << endl
    << "#format update task_id [0] (0,0.5] (0.5,0.10] ... (0.90,0.95], (0.95, 1.0], [1.0]" << endl << endl;
    return;
  }
  
public:
  cActionPrintTaskProbHistogram(cWorld* world, const cString& args)
  : cAction(world, args), m_filename("task_prob_hist.dat")
  {
    m_first_run = true;
    cString largs(args);
    m_filename = (largs.GetSize()) ? largs.PopWord() : "task_prob_hist.dat";
    m_weighted = (largs.GetSize()) ? (largs.PopWord().AsInt() != 0) : false;
  }
  
  static const cString GetDescription() { return "Arguments: [filename=pp_histogram.dat] [weightbycpus=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    
    // Setup
    tMatrix<int> m_bins; // Task, Plasticity bins
    int num_tasks = m_world->GetEnvironment().GetNumTasks();
    m_bins.ResizeClear(num_tasks, 22);  // Bins 0  (0,0.05]  (0.05,0.10] (0.10,0.15] ... (0.90, 0.95] (0.95, 1.0)  1.0
    m_bins.SetAll(0);
    ofstream& fot = m_world->GetDataFileOFStream(m_filename);  //Setup output file
    if (m_first_run == true){
      PrintHeader(fot);
      m_first_run = false;
    }
    
    //Select runtime mode
    if (ctx.GetAnalyzeMode()){  // A N A L Y Z E    M O D E
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while((genotype = batch_it.Next())){                               //For each genotype
        tArray<double> task_prob = genotype->GetTaskProbabilities();     //    get the taks probabilities
        int weight = (m_weighted) ? genotype->GetNumCPUs() : 1;          //    get the proper tally weighting
        for (int k = 0; k < task_prob.GetSize(); k++){                   //    For each task
          int bin_id = (task_prob[k] < 1.0) ? (int) ceil( ( task_prob[k] * 100 ) / 5 ) : 21;  // find the bin to put it into
          m_bins(k,bin_id) += weight;                                                   //   ... and tally it
        }
      }
    } 
    else {  // E X P E R I M E N T    M O D E  (See above for explination)
      tAutoRelease<tIterator<cBioGroup> > it;
      
      it.Set(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
      while (it->Next()) {
        cBioGroup* bg = it->Get();
        
        int weight = (m_weighted) ? bg->GetNumUnits() : 1;
        tArray<double> task_prob = cPhenPlastUtil::GetTaskProbabilities(ctx, m_world, bg);
        for (int k = 0; k < task_prob.GetSize(); k++){
          int bin_id = (task_prob[k] < 1.0) ? (int) ceil( ( task_prob[k] * 100 ) / 5 ) : 21;
          m_bins(k,bin_id) += weight; 
        }
      }
    }// End selection of runtime context
    
    
    int update = (ctx.GetAnalyzeMode()) ? -1 : m_world->GetStats().GetUpdate();
    
    //Print out our bins
    for (int t = 0; t < num_tasks; t++){
      fot << update << " " << t << " ";
      for (int b = 0; b < 22; b++)
        fot << m_bins(t,b) << (  (b != 21) ? " " : "" );
      fot << endl;
    }
    
    //Cleanup
    if (ctx.GetAnalyzeMode())
      m_world->GetDataFileManager().Remove(m_filename);     
  } //End Process
};


/* @MRR May 2009
 This function will print some plasticity information about the genotypes
 in the population or batch.
 
 Parameters:
 m_filename  (cString)
 The output file name
 */
class cActionPrintPlasticGenotypeSummary : public cAction
{
private:
  cString m_filename;
  bool    m_first_run;
  
  void PrintHeader(ofstream& fot)
  {
    fot << "# Plastic Genotype Sumary" << endl
    <<  "#format  update num_genotypes num_plastic_genotypes num_gen_taskplast num_orgs num_plastic_orgs num_org_taskplast median_phenplast_entropy median_taskplast_entropy" << endl
    <<  "# update" << endl
    <<  "# num_genotypes              Number of genotypes in the population." << endl
    <<  "# num_plastic_genotypes      Number of genotypes that show any plasticity." << endl
    <<  "# num_gen_taskplast          Number of genotypes that show task plasticity." << endl
    <<  "# num_orgs                   Number of organisms in the population." << endl
    <<  "# num_plastic_orgs           Number of organisms that show any plasticity." << endl
    <<  "# num_org_taskplast          Number of organisms that show task plasticity." << endl
    <<  "# median_phenplast_entropy   Median entropy of plastic genotypes." << endl
    <<  "# median_taskplast_entropy   Median entropy of task-plastic genotypes." << endl << endl;
  }
  
  inline bool HasPlasticTasks(tArray<double> task_probs){
    for (int k = 0; k < task_probs.GetSize(); k++)
      if (task_probs[k] != 0 && task_probs[k] != 1) return true;
    return false;
  }
  
public:
  cActionPrintPlasticGenotypeSummary(cWorld* world, const cString& args)
  : cAction(world, args)
  {
    cString largs(args);
    m_filename = (largs.GetSize()) ? largs.PopWord() : "genotype_plasticity.dat";
    m_first_run = true;
  }
  
  static const cString GetDescription() { return "Arguments: [string filename='genotype_plsticity.dat']"; }
  
  void Process(cAvidaContext& ctx)
  {
    
    //Setup
    ofstream& fot = m_world->GetDataFileOFStream(m_filename);
    if (m_first_run == true){
      PrintHeader(fot);
      m_first_run = false;
    }
    double median = -1.0;           // Will hold the median phenotypic value (excluding 0.0)
    double task_median = -1.0;      // Will hold the median phenotypic entropy value of only those genotypes showing task plasticity
    tArray<double> pp_entropy;      // Will hold the phenotypic entropy values greater than 0.0
    tArray<double> pp_taskentropy;  // Will hold phenotypic entropy values for only those organisms with task plasticity
    int num_plast_genotypes = 0;    // Number of plastic genotypes
    int num_genotypes = 0;          // Number of genotypes in the population
    int num_orgs = 0;               // Number of organisms in the population
    int num_plast_orgs = 0;         // Number of plastic organisms in the population
    int gen_task_plast = 0;         // Number of genotypes with task plasticity
    int org_task_plast = 0;         // Number of organisms with task plasticity
    
    //Collect data using methods from the correct mode
    if (ctx.GetAnalyzeMode()){  // A N A L Y Z E    M O D E
      num_genotypes = m_world->GetAnalyze().GetCurrentBatch().GetSize();
      pp_entropy.ResizeClear(num_genotypes);
      pp_taskentropy.ResizeClear(num_genotypes);
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while((genotype = batch_it.Next())){  //For each genotype
        int num = genotype->GetNumCPUs();   //   find the number of organisms
        num_orgs += num;                    //   add it to the total number of organisms
        if (genotype->GetNumPhenotypes() > 1){                     //If the genotype is plastic
          double entropy = genotype->GetPhenotypicEntropy();       //   get the entropy
          pp_entropy[num_plast_genotypes++] = entropy;             //   append the entropy to our array
          num_plast_orgs += num;                                   //   count the organisms as plastic
          if (HasPlasticTasks(genotype->GetTaskProbabilities())){        // If the genotype has tasks plasticity
            org_task_plast += num;                                       //    count the organisms belonging to the genotype as plastic
            pp_taskentropy[gen_task_plast++] = entropy;                  //    append the plastic genotype to the taskentropy array
          } // End if probabilistic tasks
        } // End if plastic phenotype
      } // End looping through genotypes
    }
    else {  // E X P E R I M E N T    M O D E    (See above for explination)
      tAutoRelease<tIterator<cBioGroup> > it;
      it.Set(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
      pp_entropy.ResizeClear(num_genotypes);
      pp_taskentropy.ResizeClear(num_genotypes);
      while (it->Next()) {
        cBioGroup* bg = it->Get();
        int num = bg->GetNumUnits();
        num_orgs += num;
        if (cPhenPlastUtil::GetNumPhenotypes(ctx, m_world, bg) > 1) {
          double entropy = cPhenPlastUtil::GetPhenotypicEntropy(ctx, m_world, bg);
          pp_entropy[num_plast_genotypes++] = entropy;
          num_plast_orgs += num;
          if (HasPlasticTasks(cPhenPlastUtil::GetTaskProbabilities(ctx, m_world, bg))) {
            org_task_plast += num;
            pp_taskentropy[gen_task_plast++] = entropy;
          }
        }
      }
    }// End selection of runtime context
    
    // Finish gathering data
    // The median will be calculated as either -1 (set above) if there is no data
    //    or as the median if there is an odd number of elements or an average
    //    of the middle two elements if there is an even number of elements.
    int update = (ctx.GetAnalyzeMode()) ? -1 : m_world->GetStats().GetUpdate();
    if (num_plast_genotypes > 0){   //Handle our array of entropies if we need to
      tArrayUtils::QSort(pp_entropy, 0, num_plast_genotypes-1);
      int ndx    = num_plast_genotypes / 2;
      median     = (num_plast_genotypes % 2 == 1) ? pp_entropy[ndx] : (pp_entropy[ndx-1] + pp_entropy[ndx]) / 2.0;
      if (gen_task_plast > 0){      //Handle our second array of entropies if we need to
        tArrayUtils::QSort(pp_taskentropy, 0, gen_task_plast-1);
        ndx    = gen_task_plast / 2;
        task_median  = (gen_task_plast % 2 == 1) ? pp_taskentropy[ndx] : (pp_taskentropy[ndx-1] + pp_taskentropy[ndx]) / 2.0;
      }
    } 
    
    //Printing
    fot << update << " " 
    << num_genotypes << " "
    << num_plast_genotypes << " "
    << gen_task_plast << " "
    << num_orgs << " "
    << num_plast_orgs << " "
    << org_task_plast << " "
    << median << " "
    << task_median << endl;
    
    //Cleanup
    if (ctx.GetAnalyzeMode())
      m_world->GetDataFileManager().Remove(m_filename);  
  }
  
};


/*
 This function goes through all genotypes currently present in the soup,
 and writes into an output file the average Hamming distance between the
 creatures in the population and a given reference genome.
 
 Parameters
 ref_creature_file (cString)
 Filename for the reference genome, defaults to START_ORGANISM
 fname (cString)
 Name of file to create, defaults to 'genetic_distance.dat'
 */
class cActionPrintGeneticDistanceData : public cAction
{
private:
  cGenome m_reference;
  cString m_filename;
  
public:
  cActionPrintGeneticDistanceData(cWorld* world, const cString& args)
  : cAction(world, args), m_filename("genetic_distance.dat")
  {
    cString creature_file;
    cString largs(args);
    
    // Load the genome of the reference creature
    creature_file.PopWord();
    if (creature_file == "" || creature_file == "START_ORGANISM") creature_file = m_world->GetConfig().START_ORGANISM.Get();
    m_reference.LoadFromDetailFile(creature_file, m_world->GetWorkingDir(), world->GetHardwareManager());
    
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string ref_creature_file='START_ORGANISM'] [string fname='genetic_distance.dat']"; }
  
  void Process(cAvidaContext& ctx)
  {
    double hamming_m1 = 0;
    double hamming_m2 = 0;
    int count = 0;
    int dom_dist = 0;
    
    // get the info for the dominant genotype
    
    tAutoRelease<tIterator<cBioGroup> > it;
    it.Set(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    it->Next();
    cSequence best_genome = cGenome(it->Get()->GetProperty("genome").AsString()).GetSequence();
    dom_dist = cSequence::FindHammingDistance(m_reference.GetSequence(), best_genome);
    hamming_m1 += dom_dist;
    hamming_m2 += dom_dist*dom_dist;
    count += it->Get()->GetNumUnits();
    // now cycle over the remaining genotypes
    while ((it->Next())) {
      int dist = cSequence::FindHammingDistance(m_reference.GetSequence(), cGenome(it->Get()->GetProperty("genome").AsString()).GetSequence());
      hamming_m1 += dist;
      hamming_m2 += dist*dist;
      count += it->Get()->GetNumUnits();
    }
    
    hamming_m1 /= static_cast<double>(count);
    hamming_m2 /= static_cast<double>(count);
    
    double hamming_best = cSequence::FindHammingDistance(m_reference.GetSequence(), best_genome);
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    df.Write(hamming_m1, "Average Hamming Distance");
    df.Write(sqrt((hamming_m2 - hamming_m1*hamming_m1) / static_cast<double>(count)), "Standard Error");
    df.Write(hamming_best, "Best Genotype Hamming Distance");
    df.Endl();
  }
};


class cActionDumpMemory : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpMemory(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("memory_dump-%d.dat", m_world->GetStats().GetUpdate());
    m_world->GetPopulation().DumpMemorySummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};


/*
 This action goes through all genotypes currently present in the population,
 and writes into an output file the names of the genotypes, the fitness as
 determined in the test cpu, and the genetic distance to a reference genome.
 */
class cActionPrintPopulationDistanceData : public cAction
{
private:
  cString m_creature;
  cString m_filename;
  int m_save_genotypes;
  
public:
  cActionPrintPopulationDistanceData(cWorld* world, const cString& args)
  : cAction(world, args), m_creature("START_ORGANISM"), m_filename(""), m_save_genotypes(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_creature = largs.PopWord();
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_save_genotypes = largs.PopWord().AsInt();
    
    if (m_creature == "" || m_creature == "START_ORGANISM") m_creature = m_world->GetConfig().START_ORGANISM.Get();
  }
  
  static const cString GetDescription() { return "Arguments: [string creature=\"START_ORGANISM\"] [string fname=\"\"] [int save_genotypes=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("pop_distance-%d.dat", m_world->GetStats().GetUpdate());
    cDataFile& df = m_world->GetDataFile(filename);
    
    double sum_fitness = 0;
    int sum_num_organisms = 0;
    
    // load the reference genome
    cGenome reference_genome;
    reference_genome.LoadFromDetailFile(m_creature, m_world->GetWorkingDir(), m_world->GetHardwareManager());    
    
    // cycle over all genotypes
    tAutoRelease<tIterator<cBioGroup> > it;
    it.Set(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    while ((it->Next())) {
      cBioGroup* bg = it->Get();
      const cGenome& genome = cGenome(bg->GetProperty("genome").AsString());
      const int num_orgs = bg->GetNumUnits();
      
      // now output
      
      sum_fitness += bg->GetProperty("fitness").AsDouble() * num_orgs;
      sum_num_organisms += num_orgs;
      
      df.Write(bg->GetProperty("name").AsString(), "Genotype Name");
      df.Write(bg->GetProperty("fitness").AsDouble(), "Fitness");
      df.Write(num_orgs, "Abundance");
      df.Write(cSequence::FindHammingDistance(reference_genome.GetSequence(), genome.GetSequence()), "Hamming distance to reference");
      df.Write(cSequence::FindEditDistance(reference_genome.GetSequence(), genome.GetSequence()), "Levenstein distance to reference");
      df.Write(genome.AsString(), "Genome");
      
      // save into archive
      if (m_save_genotypes) {
        cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
        testcpu->PrintGenome(ctx, genome, cStringUtil::Stringf("archive/%s.org", (const char*)(bg->GetProperty("name").AsString())));
        delete testcpu;
      }
      
      df.Endl();
    }
    df.WriteRaw(cStringUtil::Stringf("# ave fitness from Test CPU's: %d\n", sum_fitness / sum_num_organisms));
    
    m_world->GetDataFileManager().Remove(filename);
  }
};


class cActionTestDominant : public cAction
{
private:
  cString m_filename;
  
public:
  cActionTestDominant(cWorld* world, const cString& args) : cAction(world, args), m_filename("dom-test.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='dom-test.dat']"; }
  void Process(cAvidaContext& ctx)
  {
    tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    cBioGroup* bg = it->Next();
    cGenome genome(bg->GetProperty("genome").AsString());
    
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    cCPUTestInfo test_info;
    testcpu->TestGenome(ctx, test_info, genome);
    delete testcpu;
    
    cPhenotype& colony_phenotype = test_info.GetColonyOrganism()->GetPhenotype();
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    df.Write(colony_phenotype.GetMerit().GetDouble(), "Merit");
    df.Write(colony_phenotype.GetGestationTime(), "Gestation Time");
    df.Write(colony_phenotype.GetFitness(), "Fitness");
    df.Write(1.0 / (0.1 + colony_phenotype.GetGestationTime()), "Reproduction Rate");
    df.Write(genome.GetSize(), "Genome Length");
    df.Write(colony_phenotype.GetCopiedSize(), "Copied Size");
    df.Write(colony_phenotype.GetExecutedSize(), "Executed Size");
    df.Endl();
  }
};


class cActionPrintTaskSnapshot : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintTaskSnapshot(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("tasks_%d.dat", m_world->GetStats().GetUpdate());
    cDataFile& df = m_world->GetDataFile(filename);
    
    cPopulation& pop = m_world->GetPopulation();
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    
    for (int i = 0; i < pop.GetSize(); i++) {
      if (pop.GetCell(i).IsOccupied() == false) continue;
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      
      // create a test-cpu for the current creature
      cCPUTestInfo test_info;
      testcpu->TestGenome(ctx, test_info, organism->GetGenome());
      cPhenotype& test_phenotype = test_info.GetTestPhenotype();
      cPhenotype& phenotype = organism->GetPhenotype();
      
      int num_tasks = m_world->GetEnvironment().GetNumTasks();
      int sum_tasks_all = 0;
      int sum_tasks_rewarded = 0;
      int divide_sum_tasks_all = 0;
      int divide_sum_tasks_rewarded = 0;
      int parent_sum_tasks_all = 0;
      int parent_sum_tasks_rewarded = 0;
      
      for (int j = 0; j < num_tasks; j++) {
        // get the number of bonuses for this task
        int bonuses = 1; //phenotype.GetTaskLib().GetTaskNumBonus(j);
        int task_count = ( phenotype.GetCurTaskCount()[j] == 0 ) ? 0 : 1;
        int divide_tasks_count = (test_phenotype.GetLastTaskCount()[j] == 0)?0:1;
        int parent_task_count = (phenotype.GetLastTaskCount()[j] == 0) ? 0 : 1;
        
        // If only one bonus, this task is not rewarded, as last bonus is + 0.
        if (bonuses > 1) {
          sum_tasks_rewarded += task_count;
          divide_sum_tasks_rewarded += divide_tasks_count;
          parent_sum_tasks_rewarded += parent_task_count;
        }
        sum_tasks_all += task_count;
        divide_sum_tasks_all += divide_tasks_count;
        parent_sum_tasks_all += parent_task_count;
      }
      
      df.Write(i, "Cell Number");
      df.Write(sum_tasks_rewarded, "Number of Tasks Rewarded");
      df.Write(sum_tasks_all, "Total Number of Tasks Done");
      df.Write(divide_sum_tasks_rewarded, "Number of Rewarded Tasks on Divide");
      df.Write(divide_sum_tasks_all, "Number of Total Tasks on Divide");
      df.Write(parent_sum_tasks_rewarded, "Parent Number of Tasks Rewared");
      df.Write(parent_sum_tasks_all, "Parent Total Number of Tasks Done");
      df.Write(test_info.GetColonyFitness(), "Genotype Fitness");
      df.Write(organism->GetBioGroup("genotype")->GetID(), "Genotype ID");
      df.Endl();
    }
    
    m_world->GetDataFileManager().Remove(filename);
    delete testcpu;
  }
};

class cActionPrintAveNumTasks : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintAveNumTasks(cWorld* world, const cString& args) : cAction(world, args), m_filename("ave_num_tasks.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cDataFile& df = m_world->GetDataFile(m_filename);  
    cPopulation& pop = m_world->GetPopulation();
    
    int ave_tot_tasks = 0;
    int num_task_orgs = 0;
    for (int i = 0; i < pop.GetSize(); i++) {
      if (pop.GetCell(i).IsOccupied() == false) continue;
      
      cPhenotype& phenotype = pop.GetCell(i).GetOrganism()->GetPhenotype();
      int num_tasks = m_world->GetEnvironment().GetNumTasks();
      
      int sum_tasks = 0;
      for (int j = 0; j < num_tasks; j++) 
        sum_tasks += ( phenotype.GetLastTaskCount()[j] == 0 ) ? 0 : 1;
      if (sum_tasks>0) {
        ave_tot_tasks += sum_tasks;
        num_task_orgs++;
      }
    }
    double pop_ave = -1;
    if (num_task_orgs>0)
      pop_ave = ave_tot_tasks/double(num_task_orgs);
    
    df.WriteComment("Avida num tasks data");
    df.WriteTimeStamp();
    df.WriteComment("First column gives the current update, 2nd column gives the average number of tasks performed");
    df.WriteComment("by each organism in the current population that performs at least one task ");
    
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    df.Write(pop_ave, "Ave num tasks done by single org that is doing at least one task");
    df.Endl();
  }
};


class cActionPrintViableTasksData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintViableTasksData(cWorld* world, const cString& args) : cAction(world, args), m_filename("viable_tasks.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='viable_tasks.dat']"; }
  void Process(cAvidaContext& ctx)
  {
    cDataFile& df = m_world->GetDataFile(m_filename);
    cPopulation& pop = m_world->GetPopulation();
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    tArray<int> tasks(num_tasks);
    tasks.SetAll(0);
    
    for (int i = 0; i < pop.GetSize(); i++) {
      if (!pop.GetCell(i).IsOccupied()) continue;
      if (pop.GetCell(i).GetOrganism()->GetTestFitness(ctx) > 0.0) {
        cPhenotype& phenotype = pop.GetCell(i).GetOrganism()->GetPhenotype();
        for (int j = 0; j < num_tasks; j++) if (phenotype.GetCurTaskCount()[j] > 0) tasks[j]++;
      }
    }
    
    df.WriteComment("Avida viable tasks data");
    df.WriteTimeStamp();
    df.WriteComment("First column gives the current update, next columns give the number");
    df.WriteComment("of organisms that have the particular task as a component of their merit");
    
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    for(int i = 0; i < tasks.GetSize(); i++) {
      df.WriteAnonymous(tasks[i]);
    }
    df.Endl();
  }
};



class cActionCalcConsensus : public cAction
{
private:
  int m_lines_saved;
  cString m_inst_set;
  
public:
  cActionCalcConsensus(cWorld* world, const cString& args)
  : cAction(world, args), m_lines_saved(0)
  , m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_lines_saved = largs.PopWord().AsInt();  
    if (largs.GetSize()) m_inst_set = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [int lines_saved=0]"; }
  void Process(cAvidaContext& ctx)
  {
    cGenome mg;
    mg.SetInstSet(m_inst_set);
    mg.SetHardwareType(m_world->GetHardwareManager().GetInstSet(m_inst_set).GetHardwareType());
    const int num_inst = m_world->GetHardwareManager().GetInstSet(m_inst_set).GetSize();
    const int update = m_world->GetStats().GetUpdate();
    cClassificationManager& classmgr = m_world->GetClassificationManager();
    
    // Setup the histogtams...
    tArray<cHistogram> inst_hist(MAX_GENOME_LENGTH);
    for (int i = 0; i < MAX_GENOME_LENGTH; i++) inst_hist[i].Resize(num_inst,-1);
    
    // Loop through all of the genotypes adding them to the histograms.
    tAutoRelease<tIterator<cBioGroup> > it;
    it.Set(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    while ((it->Next())) {
      cBioGroup* bg = it->Get();
      const int num_organisms = bg->GetNumUnits();
      const cGenome& genome = cGenome(bg->GetProperty("genome").AsString());
      const int length = genome.GetSize();
      if (genome.GetInstSet() != m_inst_set) continue;
      
      // Place this genotype into the histograms.
      for (int j = 0; j < length; j++) {
        assert(genome.GetSequence()[j].GetOp() < num_inst);
        inst_hist[j].Insert(genome.GetSequence()[j].GetOp(), num_organisms);
      }
      
      // Mark all instructions beyond the length as -1 in histogram...
      for (int j = length; j < MAX_GENOME_LENGTH; j++) {
        inst_hist[j].Insert(-1, num_organisms);
      }
    }
    
    // Now, lets print something!
    cDataFile& df = m_world->GetDataFile("consensus.dat");
    cDataFile& df_abundance = m_world->GetDataFile("consensus-abundance.dat");
    cDataFile& df_var = m_world->GetDataFile("consensus-var.dat");
    cDataFile& df_entropy = m_world->GetDataFile("consensus-entropy.dat");
    
    // Determine the length of the concensus genome
    int con_length;
    for (con_length = 0; con_length < MAX_GENOME_LENGTH; con_length++) {
      if (inst_hist[con_length].GetMode() == -1) break;
    }
    
    // Build the concensus genotype...
    cSequence& con_genome = mg.GetSequence();
    con_genome = cSequence(con_length);
    double total_entropy = 0.0;
    for (int i = 0; i < MAX_GENOME_LENGTH; i++) {
      const int mode = inst_hist[i].GetMode();
      const int count = inst_hist[i].GetCount(mode);
      const int total = inst_hist[i].GetCount();
      const double entropy = inst_hist[i].GetNormEntropy();
      if (i < con_length) total_entropy += entropy;
      
      // Break out if ALL creatures have a -1 in this area, and we've
      // finished printing all of the files.
      if (mode == -1 && count == total) break;
      
      if ( i < con_length )
        con_genome[i].SetOp(mode);
      
      // Print all needed files.
      if (i < m_lines_saved) {
        df_abundance.WriteAnonymous(count);
        df_var.WriteAnonymous(inst_hist[i].GetCountVariance());
        df_entropy.WriteAnonymous(entropy);
      }
    }
    
    // Put end-of-lines on the files.
    if (m_lines_saved > 0) {
      df_abundance.Endl();
      df_var.Endl();
      df_entropy.Endl();
    }
    
    // --- Study the consensus genome ---
    
    // Loop through genotypes again, and determine the average genetic distance.
    it.Set(classmgr.GetBioGroupManager("genotype")->Iterator());
    cDoubleSum distance_sum;
    while ((it->Next())) {
      const int num_organisms = it->Get()->GetNumUnits();
      const int cur_dist = cSequence::FindEditDistance(con_genome, cGenome(it->Get()->GetProperty("genome").AsString()).GetSequence());
      distance_sum.Add(cur_dist, num_organisms);
    }
    
    // Finally, gather last bits of data and print the results.
    // @TODO - find consensus bio group
    //    cGenotype* con_genotype = classmgr.FindGenotype(con_genome, -1);
    
    it.Set(classmgr.GetBioGroupManager("genotype")->Iterator());
    const int best_dist = cSequence::FindEditDistance(con_genome, cGenome(it->Next()->GetProperty("genome").AsString()).GetSequence());
    
    const double ave_dist = distance_sum.Average();
    const double var_dist = distance_sum.Variance();
    const double complexity_base = static_cast<double>(con_genome.GetSize()) - total_entropy;
    
    cString con_name;
    con_name.Set("archive/%03d-consensus-u%i.gen", con_genome.GetSize(),update);
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    testcpu->PrintGenome(ctx, mg, con_name);
    
    
    //    if (con_genotype) {
    //      df.Write(update, "Update");
    //      df.Write(con_genotype->GetMerit(), "Merit");
    //      df.Write(con_genotype->GetGestationTime(), "Gestation Time");
    //      df.Write(con_genotype->GetFitness(), "Fitness");
    //      df.Write(con_genotype->GetReproRate(), "Reproduction Rate");
    //      df.Write(con_genotype->GetLength(), "Length");
    //      df.Write(con_genotype->GetCopiedSize(), "Copied Size");
    //      df.Write(con_genotype->GetExecutedSize(), "Executed Size");
    //      df.Write(con_genotype->GetBirths(), "Get Births");
    //      df.Write(con_genotype->GetBreedTrue(), "Breed True");
    //      df.Write(con_genotype->GetBreedIn(), "Breed In");
    //      df.Write(con_genotype->GetNumOrganisms(), "Abundance");
    //      df.Write(con_genotype->GetDepth(), "Tree Depth");
    //      df.Write(con_genotype->GetID(), "Genotype ID");
    //      df.Write(update - con_genotype->GetUpdateBorn(), "Age (in updates)");
    //      df.Write(best_dist, "Best Distance");
    //      df.Write(ave_dist, "Average Distance");
    //      df.Write(var_dist, "Var Distance");
    //      df.Write(total_entropy, "Total Entropy");
    //      df.Write(complexity_base, "Complexity");
    //      df.Endl();
    //    } else {
    
    cCPUTestInfo test_info;
    testcpu->TestGenome(ctx, test_info, mg);
    delete testcpu;
    
    cPhenotype& colony_phenotype = test_info.GetColonyOrganism()->GetPhenotype();
    
    df.Write(update, "Update");
    df.Write(colony_phenotype.GetMerit().GetDouble(), "Merit");
    df.Write(colony_phenotype.GetGestationTime(), "Gestation Time");
    df.Write(colony_phenotype.GetFitness(), "Fitness");
    df.Write(1.0 / (0.1  + colony_phenotype.GetGestationTime()), "Reproduction Rate");
    df.Write(con_genome.GetSize(), "Length");
    df.Write(colony_phenotype.GetCopiedSize(), "Copied Size");
    df.Write(colony_phenotype.GetExecutedSize(), "Executed Size");
    df.Write(0, "Get Births");
    df.Write(0, "Breed True");
    df.Write(0, "Breed In");
    df.Write(0, "Abundance");
    df.Write(-1, "Tree Depth");
    df.Write(-1, "Genotype ID");
    df.Write(0, "Age (in updates)");
    df.Write(best_dist, "Best Distance");
    df.Write(ave_dist, "Average Distance");
    df.Write(var_dist, "Var Distance");
    df.Write(total_entropy, "Total Entropy");
    df.Write(complexity_base, "Complexity");
    df.Endl();
    //    }
    
    delete testcpu;
  }
};

/*! Calculate and print average edit distance between organisms in the current population.
 
 Here we calculate the average edit distance between sample_size pairs of organisms
 from the current population, selected at random with replacement.
 */
class cActionPrintEditDistance : public cAction {
public:
  cActionPrintEditDistance(cWorld* world, const cString& args) 
	: cAction(world, args)
	, m_sample_size(100)
	, m_filename("edit_distance.dat") {
    cString largs(args);
		if(largs.GetSize()) { m_sample_size = static_cast<unsigned int>(largs.PopWord().AsInt()); }
    if(largs.GetSize()) {	m_filename = largs.PopWord();	}
  }
  
  static const cString GetDescription() { return "Arguments: [sample_size [filename]]"; }
  
  void Process(cAvidaContext& ctx) {
		cDataFile& df = m_world->GetDataFile(m_filename);
		std::vector<int> occupied_cells(m_world->GetPopulation().GetNumOrganisms());
		std::vector<int>::iterator oiter=occupied_cells.begin();
		for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
			if(m_world->GetPopulation().GetCell(i).IsOccupied()) {
				*oiter = i;
				++oiter;
			}
		}
    
		std::random_shuffle(occupied_cells.begin(), occupied_cells.end());
		if(occupied_cells.size() % 2) {
			occupied_cells.pop_back();
		}
		
		unsigned int max_pairs = occupied_cells.size()/2;
		unsigned int sample_pairs = std::min(m_sample_size, max_pairs);
		
		cDoubleSum edit_distance;
		for(unsigned int i=0; i<sample_pairs; ++i) {
			cOrganism* a = m_world->GetPopulation().GetCell(occupied_cells.back()).GetOrganism(); 
			occupied_cells.pop_back();
			cOrganism* b = m_world->GetPopulation().GetCell(occupied_cells.back()).GetOrganism(); 
			occupied_cells.pop_back();
			edit_distance.Add(cSequence::FindEditDistance(a->GetGenome().GetSequence(), b->GetGenome().GetSequence()));
		}
		
		df.Write(m_world->GetStats().GetUpdate(), "Update [update]");
		df.Write(edit_distance.N(), "Number of pairs in sample [pairs]");
		df.Write(edit_distance.Average(), "Average edit distance [distance]");
		df.Endl();
	}
	
private:
	unsigned int m_sample_size; //!< Number of pairs of organisms to sample for diversity calculation.
  cString m_filename;
};


class cActionDumpEnergyGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpEnergyGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_energy.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_energy = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().GetStoredEnergy() : 0.0;
        fp << cell_energy << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

class cActionDumpExecutionRatioGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpExecutionRatioGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_exe_ratio.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_executionRatio = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().GetEnergyUsageRatio() : 1.0;
        fp << cell_executionRatio << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

class cActionDumpCellDataGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpCellDataGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_cell_data.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_data = cell.GetCellData();
        fp << cell_data << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

class cActionDumpFitnessGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpFitnessGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_fitness-%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        double fitness = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().GetFitness() : 0.0;
        fp << fitness << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};


class cActionDumpClassificationIDGrid : public cAction
{
private:
  cString m_filename;
  cString m_role;
  
public:
  cActionDumpClassificationIDGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename(""), m_role("genotype")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_role = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname_prefix='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename = "grid_class_id";
    filename.Set("%s-%d.dat", (const char*)filename, m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int id = (cell.IsOccupied() && cell.GetOrganism()->GetBioGroup(m_role)) ? cell.GetOrganism()->GetBioGroup(m_role)->GetID() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

class cActionDumpGenotypeColorGrid : public cAction
{
private:
  int m_num_colors;
  int m_threshold;
  cString m_filename;
  tArray<int> m_genotype_chart;
  
public:
  cActionDumpGenotypeColorGrid(cWorld* world, const cString& args)
  : cAction(world, args), m_num_colors(12), m_threshold(10), m_filename(""), m_genotype_chart(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_num_colors = largs.PopWord().AsInt();
    if (largs.GetSize()) m_threshold = largs.PopWord().AsInt();
    if (largs.GetSize()) m_filename = largs.PopWord();
    
    m_genotype_chart.Resize(m_num_colors, 0);
  }
  
  static const cString GetDescription() { return "Arguments: [int num_colors=12] [string fname='']"; }
  
  
  void Process(cAvidaContext& ctx)
  {
    // Update current entries in the color chart
    for (int i = 0; i < m_num_colors; i++) {
      if (m_genotype_chart[i] && FindPos(m_genotype_chart[i]) < 0) m_genotype_chart[i] = 0;
    }
    
    // Add new entries where possible
    tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    for (int i = 0; (it->Next()) && i < m_threshold; i++) {
      if (!isInChart(it->Get()->GetID())) {
        // Add to the genotype chart
        for (int j = 0; j < m_num_colors; j++) {
          if (m_genotype_chart[j] == 0) {
            m_genotype_chart[j] = it->Get()->GetID();
            break;
          }
        }
      }
    }
    
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_genotype_color-%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        cBioGroup* bg = (cell.IsOccupied()) ? cell.GetOrganism()->GetBioGroup("genotype") : NULL;
        if (bg) {
          int color = 0;
          for (; color < m_num_colors; color++) if (m_genotype_chart[color] == bg->GetID()) break;
          if (color == m_num_colors && bg->GetProperty("threshold").AsBool()) color++;
          fp << color << " ";
        } else {
          fp << "-1 ";
        }
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);   
  }
  
private:
  int FindPos(int gid)
  {
    tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
    int i = 0;
    while ((it->Next()) && i < m_num_colors) {
      if (gid == it->Get()->GetID()) return i;
      i++;
    }
    
    return -1;
  }
  
  inline bool isInChart(int gid)
  {
    for (int i = 0; i < m_num_colors; i++) {
      if (m_genotype_chart[i] == gid) return true;
    }
    return false;    
  }
};


class cActionDumpPhenotypeIDGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpPhenotypeIDGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_phenotype_id.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int id = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().CalcID() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};


class cActionDumpSleepGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpSleepGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_sleep.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_energy = (cell.IsOccupied()) ? cell.GetOrganism()->IsSleeping() : 0.0;
        fp << cell_energy << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

class cActionDumpTaskGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpTaskGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_task.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    cPopulation* pop = &m_world->GetPopulation();
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    for (int i = 0; i < pop->GetWorldX(); i++) {
      for (int j = 0; j < pop->GetWorldY(); j++) {
        int task_sum = 0;
        int cell_num = i * pop->GetWorldX() + j;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          cCPUTestInfo test_info;
          testcpu->TestGenome(ctx, test_info, organism->GetGenome());
          cPhenotype& test_phenotype = test_info.GetTestPhenotype();
          for (int k = 0; k < num_tasks; k++) {
            if (test_phenotype.GetLastTaskCount()[k] > 0) task_sum += static_cast<int>(pow(2.0, k)); 
          }
        }
        fp << task_sum << " ";
      }
      fp << endl;
    }
    
    delete testcpu;    
    m_world->GetDataFileManager().Remove(filename);
  }
};

//Dump the reaction grid from the last gestation cycle, so skip the 
//test cpu, and just use what the phenotype has. 

class cActionDumpReactionGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpReactionGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_reactions.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    cPopulation* pop = &m_world->GetPopulation();
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    for (int i = 0; i < pop->GetWorldX(); i++) {
      for (int j = 0; j < pop->GetWorldY(); j++) {
        int task_sum = 0;
        int cell_num = i * pop->GetWorldX() + j;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          
          cPhenotype& test_phenotype = organism->GetPhenotype();
          for (int k = 0; k < num_tasks; k++) {
            if (test_phenotype.GetLastReactionCount()[k] > 0) task_sum += static_cast<int>(pow(2.0, k)); 
          }
        }
        else {task_sum = -1;}
        fp << task_sum << " ";
      }
      fp << endl;
    }
    
    m_world->GetDataFileManager().Remove(filename);
  }
};

class cActionDumpDonorGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpDonorGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_donor.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int donor = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().IsDonorLast() : -1;
        fp << donor << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};


class cActionDumpReceiverGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpReceiverGrid(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_receiver.%d.dat", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int recv = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().IsReceiver() : -1;
        fp << recv << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};


class cActionPrintDonationStats : public cAction
{
public:
  cActionPrintDonationStats(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDonationStats();
  }
};

class cActionPrintDemeAllStats : public cAction
{
public:
  cActionPrintDemeAllStats(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeAllStats();
  }
};

class cActionPrintDemesTotalAvgEnergy : public cAction {
public:
	cActionPrintDemesTotalAvgEnergy(cWorld* world, const cString& args) : cAction(world, args) { ; }
	
	static const cString GetDescription() { return "No Arguments"; }
	
	void Process(cAvidaContext& ctx) {
		m_world->GetPopulation().PrintDemeTotalAvgEnergy();
	}
};

class cActionPrintDemeEnergySharingStats : public cAction
{
public:
  cActionPrintDemeEnergySharingStats(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeEnergySharingStats();
  }
};

class cActionPrintDemeEnergyDistributionStats : public cAction
{
public:
  cActionPrintDemeEnergyDistributionStats(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeEnergyDistributionStats();
  }
};

class cActionPrintDemeDonorStats : public cAction
{
public:
  cActionPrintDemeDonorStats(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeDonor();
  }
};


class cActionPrintDemeSpacialEnergy : public cAction
{
public:
  cActionPrintDemeSpacialEnergy(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeSpatialEnergyData();
  }
};

class cActionPrintDemeSpacialSleep : public cAction
{
public:
  cActionPrintDemeSpacialSleep(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeSpatialSleepData();
  }
};

class cActionPrintDemeResources : public cAction
{
public:
  cActionPrintDemeResources(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeResource();
  }
};

class cActionPrintDemeGlobalResources : public cAction
{
public:
  cActionPrintDemeGlobalResources(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeGlobalResources();
  }
};

class cActionSaveDemeFounders : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSaveDemeFounders(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("deme_founders-%d.dat", m_world->GetStats().GetUpdate());
    m_world->GetPopulation().DumpDemeFounders(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

class cActionSetVerbose : public cAction
{
private:
  cString m_verbose;
  
public:
  cActionSetVerbose(cWorld* world, const cString& args) : cAction(world, args), m_verbose("")
  {
    cString largs(args);
    if (largs.GetSize()) m_verbose = largs.PopWord();
    m_verbose.ToUpper();
  }
  static const cString GetDescription() { return "Arguments: [string verbosity='']"; }
  void Process(cAvidaContext& ctx)
  {
    // If no arguments are given, assume a basic toggle.
    // Otherwise, read in the argument to decide the new mode.
    if (m_verbose.GetSize() == 0 && m_world->GetVerbosity() <= VERBOSE_NORMAL) {
      m_world->SetVerbosity(VERBOSE_ON);
    } else if (m_verbose.GetSize() == 0 && m_world->GetVerbosity() >= VERBOSE_ON) {
      m_world->SetVerbosity(VERBOSE_NORMAL);
    } else if (m_verbose == "SILENT") m_world->SetVerbosity(VERBOSE_SILENT);
    else if (m_verbose == "NORMAL") m_world->SetVerbosity(VERBOSE_NORMAL);
    else if (m_verbose == "QUIET") m_world->SetVerbosity(VERBOSE_NORMAL);
    else if (m_verbose == "OFF") m_world->SetVerbosity(VERBOSE_NORMAL);
    else if (m_verbose == "ON") m_world->SetVerbosity(VERBOSE_ON);
    else if (m_verbose == "DETAILS") m_world->SetVerbosity(VERBOSE_DETAILS);
    else if (m_verbose == "HIGH") m_world->SetVerbosity(VERBOSE_DETAILS);
    else m_world->SetVerbosity(VERBOSE_NORMAL);
    
    // Print out new verbose level (nothing for silent!)
    if (m_world->GetVerbosity() == VERBOSE_NORMAL) {
      cout << "Verbose NORMAL: Using standard log messages..." << endl;
    } else if (m_world->GetVerbosity() == VERBOSE_ON) {
      cout << "Verbose ON: Using verbose log messages..." << endl;
    } else if (m_world->GetVerbosity() == VERBOSE_DETAILS) {
      cout << "Verbose DETAILS: Using detailed log messages..." << endl;
    }    
    
  }
};

class cActionPrintNumOrgsInDeme : public cAction
{
public:
  cActionPrintNumOrgsInDeme(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    cDataFile & df = m_world->GetDataFile("deme_org_count.dat");
    df.WriteComment("Avida deme resource data");
    df.WriteTimeStamp();
    
    cString UpdateStr = cStringUtil::Stringf( "deme_global_resources_%07i = [ ...", m_world->GetStats().GetUpdate());
    df.WriteRaw(UpdateStr);      
    
    for (int d = 0; d < m_world->GetPopulation().GetNumDemes(); d++) {
      cDeme& deme = m_world->GetPopulation().GetDeme(d);
      df.WriteBlockElement(d, 0, 2);
      df.WriteBlockElement(deme.GetOrgCount(), 1, 2);
    }
    
    df.WriteRaw("];");
    df.Endl();
  }
};

class cActionPrintDebug : public cAction
{
public:
  cActionPrintDebug(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
  }
};




void RegisterPrintActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionPrintDebug>("PrintDebug");
  
  
  // Stats Out Files
  action_lib->Register<cActionPrintAverageData>("PrintAverageData");
  action_lib->Register<cActionPrintDemeAverageData>("PrintDemeAverageData");
  action_lib->Register<cActionPrintFlowRateTuples>("PrintFlowRateTuples");
  action_lib->Register<cActionPrintErrorData>("PrintErrorData");
  action_lib->Register<cActionPrintVarianceData>("PrintVarianceData");
  action_lib->Register<cActionPrintDominantData>("PrintDominantData");
  action_lib->Register<cActionPrintStatsData>("PrintStatsData");
  action_lib->Register<cActionPrintCountData>("PrintCountData");
  action_lib->Register<cActionPrintMessageData>("PrintMessageData");
	action_lib->Register<cActionPrintMessageLog>("PrintMessageLog");
  action_lib->Register<cActionPrintInterruptData>("PrintInterruptData");
  action_lib->Register<cActionPrintTotalsData>("PrintTotalsData");
  action_lib->Register<cActionPrintTasksData>("PrintTasksData");
  action_lib->Register<cActionPrintHostTasksData>("PrintHostTasksData");
  action_lib->Register<cActionPrintParasiteTasksData>("PrintParasiteTasksData");
  action_lib->Register<cActionPrintTasksExeData>("PrintTasksExeData");
  action_lib->Register<cActionPrintNewTasksData>("PrintNewTasksData");
  action_lib->Register<cActionPrintNewReactionData>("PrintNewReactionData");
  action_lib->Register<cActionPrintNewTasksDataPlus>("PrintNewTasksDataPlus");
  action_lib->Register<cActionPrintTasksQualData>("PrintTasksQualData");
  action_lib->Register<cActionPrintResourceData>("PrintResourceData");
  action_lib->Register<cActionPrintReactionData>("PrintReactionData");
  action_lib->Register<cActionPrintReactionExeData>("PrintReactionExeData");
  action_lib->Register<cActionPrintCurrentReactionData>("PrintCurrentReactionData");  
  action_lib->Register<cActionPrintReactionRewardData>("PrintReactionRewardData");
  action_lib->Register<cActionPrintCurrentReactionRewardData>("PrintCurrentReactionRewardData");
  action_lib->Register<cActionPrintTimeData>("PrintTimeData");
	action_lib->Register<cActionPrintExtendedTimeData>("PrintExtendedTimeData");
  action_lib->Register<cActionPrintMutationRateData>("PrintMutationRateData");
  action_lib->Register<cActionPrintDivideMutData>("PrintDivideMutData");
  action_lib->Register<cActionPrintParasiteData>("PrintParasiteData");
  action_lib->Register<cActionPrintMarketData>("PrintMarketData");
  action_lib->Register<cActionPrintSenseData>("PrintSenseData");
  action_lib->Register<cActionPrintSenseExeData>("PrintSenseExeData");
  action_lib->Register<cActionPrintInstructionData>("PrintInstructionData");
  action_lib->Register<cActionPrintInternalTasksData>("PrintInternalTasksData");
  action_lib->Register<cActionPrintInternalTasksQualData>("PrintInternalTasksQualData");
  action_lib->Register<cActionPrintSleepData>("PrintSleepData");
  action_lib->Register<cActionPrintCompetitionData>("PrintCompetitionData");
  action_lib->Register<cActionPrintDynamicMaxMinData>("PrintDynamicMaxMinData");
  
  // @WRE: Added printing of visit data
  action_lib->Register<cActionPrintCellVisitsData>("PrintCellVisitsData");
  
  // Population Out Files
  action_lib->Register<cActionPrintPhenotypeData>("PrintPhenotypeData");
  action_lib->Register<cActionPrintParasitePhenotypeData>("PrintParasitePhenotypeData");
  action_lib->Register<cActionPrintHostPhenotypeData>("PrintHostPhenotypeData");
  action_lib->Register<cActionPrintPhenotypeStatus>("PrintPhenotypeStatus");
  
  action_lib->Register<cActionPrintDemeTestamentStats>("PrintDemeTestamentStats");
	action_lib->Register<cActionPrintCurrentMeanDemeDensity>("PrintCurrentMeanDemeDensity");
  
	action_lib->Register<cActionPrintDemeResourceThresholdPredicate>("PrintDemeResourceThresholdPredicate");
	action_lib->Register<cActionPrintPredicatedMessages>("PrintPredicatedMessages");
	action_lib->Register<cActionPrintCellData>("PrintCellData");
	action_lib->Register<cActionPrintConsensusData>("PrintConsensusData");
	action_lib->Register<cActionPrintSimpleConsensusData>("PrintSimpleConsensusData");
	action_lib->Register<cActionPrintCurrentOpinions>("PrintCurrentOpinions");
	action_lib->Register<cActionPrintOpinionsSetPerDeme>("PrintOpinionsSetPerDeme");
	action_lib->Register<cActionPrintSynchronizationData>("PrintSynchronizationData");
  action_lib->Register<cActionPrintDetailedSynchronizationData>("PrintDetailedSynchronizationData");
	
  action_lib->Register<cActionPrintDonationStats>("PrintDonationStats");
  
  
  // deme output files
  action_lib->Register<cActionPrintDemeAllStats>("PrintDemeAllStats");
  action_lib->Register<cActionPrintDemeAllStats>("PrintDemeStats"); //duplicate of previous
  action_lib->Register<cActionPrintDemesTotalAvgEnergy>("PrintDemesTotalAvgEnergy");
  action_lib->Register<cActionPrintDemeEnergySharingStats>("PrintDemeEnergySharingStats");
  action_lib->Register<cActionPrintDemeEnergyDistributionStats>("PrintDemeEnergyDistributionStats");
  action_lib->Register<cActionPrintDemeDonorStats>("PrintDemeDonorStats");
  action_lib->Register<cActionPrintDemeSpacialEnergy>("PrintDemeSpacialEnergyStats");
  action_lib->Register<cActionPrintDemeSpacialSleep>("PrintDemeSpacialSleepStats");
  action_lib->Register<cActionPrintDemeResources>("PrintDemeResourceStats");
  action_lib->Register<cActionPrintDemeGlobalResources>("PrintDemeGlobalResources");
  action_lib->Register<cActionPrintDemeReplicationData>("PrintDemeReplicationData");
	action_lib->Register<cActionPrintDemeReactionDiversityReplicationData>("PrintDemeReactionDiversityReplicationData");
  action_lib->Register<cActionPrintDemeTreatableReplicationData>("PrintDemeTreatableReplicationData");
  action_lib->Register<cActionPrintDemeUntreatableReplicationData>("PrintDemeUntreatableReplicationData");
  action_lib->Register<cActionPrintDemeTreatableCount>("PrintDemeTreatableCount");
  
	
  action_lib->Register<cActionPrintDemeCompetitionData>("PrintDemeCompetitionData");
	action_lib->Register<cActionPrintDemeNetworkData>("PrintDemeNetworkData");
	action_lib->Register<cActionPrintDemeNetworkTopology>("PrintDemeNetworkTopology");
  action_lib->Register<cActionPrintDemeFoundersData>("PrintDemeFoundersData");
  action_lib->Register<cActionPrintGermlineData>("PrintGermlineData");
  action_lib->Register<cActionSaveDemeFounders>("SaveDemeFounders");
  action_lib->Register<cActionPrintPerDemeTasksData>("PrintPerDemeTasksData");
  action_lib->Register<cActionPrintPerDemeTasksExeData>("PrintPerDemeTasksExeData");
  action_lib->Register<cActionPrintAvgDemeTasksExeData>("PrintAvgDemeTasksExeData");
  action_lib->Register<cActionPrintAvgTreatableDemeTasksExeData>("PrintAvgTreatableDemeTasksExeData");
  action_lib->Register<cActionPrintAvgUntreatableDemeTasksExeData>("PrintAvgUntreatableDemeTasksExeData");
  action_lib->Register<cActionPrintPerDemeReactionData>("PrintPerDemeReactionData");
  action_lib->Register<cActionPrintDemeTasksData>("PrintDemeTasksData");
  action_lib->Register<cActionPrintDemeTasksExeData>("PrintDemeTasksExeData");
  action_lib->Register<cActionPrintDemeReactionData>("PrintDemeReactionData");
  action_lib->Register<cActionPrintDemeOrgTasksData>("PrintDemeOrgTasksData");
  action_lib->Register<cActionPrintDemeOrgTasksExeData>("PrintDemeOrgTasksExeData");
  action_lib->Register<cActionPrintDemeOrgReactionData>("PrintDemeOrgReactionData");
  action_lib->Register<cActionPrintDemeCurrentTaskExeData>("PrintDemeCurrentTaskExeData");
  action_lib->Register<cActionPrintCurrentTaskCounts>("PrintCurrentTaskCounts");
  action_lib->Register<cActionPrintPerDemeGenPerFounderData>("PrintPerDemeGenPerFounderData");
  action_lib->Register<cActionPrintDemeMigrationSuicidePoints>("PrintDemeMigrationSuicidePoints");
	action_lib->Register<cActionPrintMultiProcessData>("PrintMultiProcessData");
	action_lib->Register<cActionPrintProfilingData>("PrintProfilingData");
	action_lib->Register<cActionPrintAgePolyethismData>("PrintAgePolyethismData");
	
  
  //Coalescence Clade Actions
  action_lib->Register<cActionPrintCCladeCounts>("PrintCCladeCounts");
  action_lib->Register<cActionPrintCCladeFitnessHistogram>("PrintCCladeFitnessHistogram");
  action_lib->Register<cActionPrintCCladeRelativeFitnessHistogram>("PrintCCladeRelativeFitnessHistogram");  
  
  // Processed Data
  action_lib->Register<cActionPrintData>("PrintData");
  action_lib->Register<cActionPrintInstructionAbundanceHistogram>("PrintInstructionAbundanceHistogram");
  action_lib->Register<cActionPrintDepthHistogram>("PrintDepthHistogram");
  action_lib->Register<cActionEcho>("Echo");
  action_lib->Register<cActionPrintGenotypeAbundanceHistogram>("PrintGenotypeAbundanceHistogram");
  //  action_lib->Register<cActionPrintSpeciesAbundanceHistogram>("PrintSpeciesAbundanceHistogram");
  //  action_lib->Register<cActionPrintLineageTotals>("PrintLineageTotals");
  //  action_lib->Register<cActionPrintLineageCounts>("PrintLineageCounts");
  action_lib->Register<cActionPrintDominantGenotype>("PrintDominantGenotype");
  action_lib->Register<cActionPrintDetailedFitnessData>("PrintDetailedFitnessData");
  action_lib->Register<cActionPrintLogFitnessHistogram>("PrintLogFitnessHistogram");
  action_lib->Register<cActionPrintRelativeFitnessHistogram>("PrintRelativeFitnessHistogram");
  action_lib->Register<cActionPrintGeneticDistanceData>("PrintGeneticDistanceData");
  action_lib->Register<cActionPrintPopulationDistanceData>("PrintPopulationDistanceData");
  
  action_lib->Register<cActionPrintPhenotypicPlasticity>("PrintPhenotypicPlasticity");
  action_lib->Register<cActionPrintTaskProbHistogram>("PrintTaskProbHistogram");
  action_lib->Register<cActionPrintPlasticGenotypeSummary>("PrintPlasticGenotypeSummary");
  
  action_lib->Register<cActionTestDominant>("TestDominant");
  action_lib->Register<cActionPrintTaskSnapshot>("PrintTaskSnapshot");
  action_lib->Register<cActionPrintViableTasksData>("PrintViableTasksData");
  action_lib->Register<cActionPrintAveNumTasks>("PrintAveNumTasks");
  
  action_lib->Register<cActionPrintGenomicSiteEntropy>("PrintGenomicSiteEntropy");
  
  // Grid Information Dumps
  action_lib->Register<cActionDumpMemory>("DumpMemory");
  action_lib->Register<cActionDumpClassificationIDGrid>("DumpClassificationIDGrid");
  action_lib->Register<cActionDumpFitnessGrid>("DumpFitnessGrid");
  action_lib->Register<cActionDumpGenotypeColorGrid>("DumpGenotypeColorGrid");
  action_lib->Register<cActionDumpPhenotypeIDGrid>("DumpPhenotypeIDGrid");
  action_lib->Register<cActionDumpTaskGrid>("DumpTaskGrid");
  action_lib->Register<cActionDumpReactionGrid>("DumpReactionGrid");
  
  action_lib->Register<cActionDumpDonorGrid>("DumpDonorGrid");
  action_lib->Register<cActionDumpReceiverGrid>("DumpReceiverGrid");
  action_lib->Register<cActionDumpEnergyGrid>("DumpEnergyGrid");
  action_lib->Register<cActionDumpExecutionRatioGrid>("DumpExecutionRatioGrid");
  action_lib->Register<cActionDumpCellDataGrid>("DumpCellDataGrid");
  action_lib->Register<cActionDumpSleepGrid>("DumpSleepGrid");
  
  
  action_lib->Register<cActionPrintNumOrgsKilledData>("PrintNumOrgsKilledData");
  action_lib->Register<cActionPrintMigrationData>("PrintMigrationData");
	
  action_lib->Register<cActionPrintReputationData>("PrintReputationData");
	action_lib->Register<cActionPrintDirectReciprocityData>("PrintDirectReciprocityData");
  action_lib->Register<cActionPrintStringMatchData>("PrintStringMatchData");
	action_lib->Register<cActionPrintShadedAltruists>("PrintShadedAltruists");
	
	action_lib->Register<cActionPrintGroupsFormedData>("PrintGroupsFormedData");
	action_lib->Register<cActionPrintGroupIds>("PrintGroupIds");	
	action_lib->Register<cActionPrintHGTData>("PrintHGTData");
	
  action_lib->Register<cActionSetVerbose>("SetVerbose");
  action_lib->Register<cActionSetVerbose>("VERBOSE");
  
  action_lib->Register<cActionPrintNumOrgsInDeme>("PrintNumOrgsInDeme");
  action_lib->Register<cActionCalcConsensus>("CalcConsensus");
	action_lib->Register<cActionPrintEditDistance>("PrintEditDistance");
}
