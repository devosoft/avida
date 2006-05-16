/*
 *  PrintActions.cc
 *  Avida
 *
 *  Created by David on 5/11/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "PrintActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyzeUtil.h"
#include "cStats.h"
#include "cWorld.h"


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
  const cString GetDescription() { return #METHOD " [cString fname=\"" #DEFAULT "\"]"; }  /* 11 */ \
  void Process(cAvidaContext& ctx) { m_world->GetStats().METHOD(m_filename); }            /* 12 */ \
}                                                                                         /* 13 */ \

STATS_OUT_FILE(PrintAverageData,       average.dat         );
STATS_OUT_FILE(PrintErrorData,         error.dat           );
STATS_OUT_FILE(PrintVarianceData,      variance.dat        );
STATS_OUT_FILE(PrintDominantData,      dominant.dat        );
STATS_OUT_FILE(PrintStatsData,         stats.dat           );
STATS_OUT_FILE(PrintCountData,         count.dat           );
STATS_OUT_FILE(PrintTotalsData,        totals.dat          );
STATS_OUT_FILE(PrintTasksData,         tasks.dat           );
STATS_OUT_FILE(PrintTasksExeData,      tasks_exe.dat       );
STATS_OUT_FILE(PrintTasksQualData,     tasks_quality.dat   );
STATS_OUT_FILE(PrintResourceData,      resource.dat        );
STATS_OUT_FILE(PrintTimeData,          time.dat            );
STATS_OUT_FILE(PrintMutationData,      mutation.dat        );
STATS_OUT_FILE(PrintMutationRateData,  mutation_rates.dat  );
STATS_OUT_FILE(PrintDivideMutData,     divide_mut.dat      );
STATS_OUT_FILE(PrintDominantParaData,  parasite.dat        );
STATS_OUT_FILE(PrintInstructionData,   instruction.dat     );
STATS_OUT_FILE(PrintGenotypeMap,       genotype_map.m      );


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
  
  const cString GetDescription() { return "PringData <cString fname> <cString format>"; }

  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintDataFile(m_filename, m_format, ',');
  }
};


class cActionPrintInstructionAbundanceHistogram : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintInstructionAbundanceHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "instruction_histogram.dat"; else m_filename = largs.PopWord();
  }

  const cString GetDescription() { return "PrintInstructionAbundanceHistogram [cString fname=\"instruction_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cAnalyzeUtil::PrintInstructionAbundanceHistogram(m_world, m_world->GetDataFileOFStream(m_filename));
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
  
  const cString GetDescription() { return "PrintDepthHistogram [cString fname=\"depth_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cAnalyzeUtil::PrintDepthHistogram(m_world, m_world->GetDataFileOFStream(m_filename));
  }
};


void RegisterPrintActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionPrintAverageData>("PrintAverageData");
  action_lib->Register<cActionPrintErrorData>("PrintErrorData");
  action_lib->Register<cActionPrintVarianceData>("PrintVarianceData");
  action_lib->Register<cActionPrintDominantData>("PrintDominantData");
  action_lib->Register<cActionPrintStatsData>("PrintStatsData");
  action_lib->Register<cActionPrintCountData>("PrintCountData");
  action_lib->Register<cActionPrintTotalsData>("PrintTotalsData");
  action_lib->Register<cActionPrintTasksData>("PrintTasksData");
  action_lib->Register<cActionPrintTasksExeData>("PrintTasksExeData");
  action_lib->Register<cActionPrintTasksQualData>("PrintTasksQualData");
  action_lib->Register<cActionPrintResourceData>("PrintResourceData");
  action_lib->Register<cActionPrintTimeData>("PrintTimeData");
  action_lib->Register<cActionPrintMutationData>("PrintMutationData");
  action_lib->Register<cActionPrintMutationRateData>("PrintMutationRateData");
  action_lib->Register<cActionPrintDivideMutData>("PrintDivideMutData");
  action_lib->Register<cActionPrintDominantParaData>("PrintDominantParaData");
  action_lib->Register<cActionPrintInstructionData>("PrintInstructionData");
  action_lib->Register<cActionPrintGenotypeMap>("PrintGenotypeMap");
  
  action_lib->Register<cActionPrintData>("PrintData");
  action_lib->Register<cActionPrintInstructionAbundanceHistogram>("PrintInstructionAbundanceHistogram");
  action_lib->Register<cActionPrintDepthHistogram>("PrintDepthHistogram");
  // @DMB - TODO
//  action_lib->Register<cActionEcho>("Echo");
//  action_lib->Register<cActionPrintGenotypeAbundanceHistogram>("PrintGenotypeAbundanceHistogram");
//  action_lib->Register<cActionPrintSpeciesAbundanceHistogram>("PrintSpeciesAbundanceHistogram");
//  action_lib->Register<cActionPrintLineageTotals>("PrintLineageTotals");
//  action_lib->Register<cActionPrintLineageCounts>("PrintLineageCounts");
//  action_lib->Register<cActionPrintDominantGenotype>("PrintDominantGenotype");
//  action_lib->Register<cActionPrintParasiteDebug>("PrintParasiteDebug");
//  action_lib->Register<cActionPrintDominantParasiteGenotype>("PrintDominantParasiteGenotype");
//  action_lib->Register<cActionPrintNumberPhenotypes>("PrintNumberPhenotypes");
//  action_lib->Register<cActionPrintPhenotypeStatus>("PrintPhenotypeStatus");
  


  // @DMB - The following actions are DEPRECATED aliases - These will be removed in 2.7.
  action_lib->Register<cActionPrintAverageData>("print_average_data");
  action_lib->Register<cActionPrintErrorData>("print_error_data");
  action_lib->Register<cActionPrintVarianceData>("print_variance_data");
  action_lib->Register<cActionPrintDominantData>("print_dominant_data");
  action_lib->Register<cActionPrintStatsData>("print_stats_data");
  action_lib->Register<cActionPrintCountData>("print_count_data");
  action_lib->Register<cActionPrintTotalsData>("print_totals_data");
  action_lib->Register<cActionPrintTasksData>("print_tasks_data");
  action_lib->Register<cActionPrintTasksExeData>("print_tasks_exe_data");
  action_lib->Register<cActionPrintTasksQualData>("print_tasks_qual_data");
  action_lib->Register<cActionPrintResourceData>("print_resource_data");
  action_lib->Register<cActionPrintTimeData>("print_time_data");
  action_lib->Register<cActionPrintMutationData>("print_mutation_dat");
  action_lib->Register<cActionPrintMutationRateData>("print_mutation_rate_data");
  action_lib->Register<cActionPrintDivideMutData>("print_divide_mut_data");
  action_lib->Register<cActionPrintDominantParaData>("print_dom_parasite_data");
  action_lib->Register<cActionPrintInstructionData>("print_instruction_data");
  action_lib->Register<cActionPrintGenotypeMap>("print_genotype_map");
  
  action_lib->Register<cActionPrintData>("print_data");
  action_lib->Register<cActionPrintInstructionAbundanceHistogram>("print_instruction_abundance_histogram");
  action_lib->Register<cActionPrintDepthHistogram>("print_depth_histogram");
  // @DMB - TODO
//  action_lib->Register<cActionEcho>("echo");
//  action_lib->Register<cActionPrintGenotypeAbundanceHistogram>("print_genotype_abundance_histogram");
//  action_lib->Register<cActionPrintSpeciesAbundanceHistogram>("print_species_abundance_histogram");
//  action_lib->Register<cActionPrintLineageTotals>("print_lineage_totals");
//  action_lib->Register<cActionPrintLineageCounts>("print_lineage_counts");
//  action_lib->Register<cActionPrintDominantGenotype>("print_dom");
//  action_lib->Register<cActionPrintParasiteDebug>("parasite_debug");
//  action_lib->Register<cActionPrintDominantParasiteGenotype>("print_dom_parasite");
//  action_lib->Register<cActionPrintNumberPhenotypes>("print_number_phenotypes");
//  action_lib->Register<cActionPrintPhenotypeStatus>("print_phenotype_status");
}
