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
  
}
