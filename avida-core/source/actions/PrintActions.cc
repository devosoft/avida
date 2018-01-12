/*
 *  PrintActions.cc
 *  Avida
 *
 *  Created by David on 5/11/06.
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

#include "PrintActions.h"

#include "avida/core/Feedback.h"
#include "avida/core/InstructionSequence.h"
#include "avida/core/WorldDriver.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include "avida/data/Recorder.h"
#include "avida/output/File.h"
#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"

#include "avida/private/util/GenomeLoader.h"

#include "apto/rng.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyze.h"
#include "cAnalyzeGenotype.h"
#include "cCPUTestInfo.h"
#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHistogram.h"
#include "cInstSet.h"
#include "cMigrationMatrix.h"
#include "cOrganism.h"
#include "cPhenPlastGenotype.h"
#include "cPhenPlastUtil.h"
#include "cPlasticPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cReaction.h"
#include "cReactionLib.h"
#include "cStats.h"
#include "cWorld.h"
#include "cUserFeedback.h"
#include "cParasite.h"
#include "cBirthEntry.h"
#include "nGeometry.h"

#include <cmath>
#include <cerrno>
#include <map>
#include <algorithm>

class cBioGroup;

using namespace Avida;


#define STATS_OUT_FILE(METHOD, DEFAULT)                                                   /*  1 */ \
class cAction ## METHOD : public cAction {                                                /*  2 */ \
private:                                                                                  /*  3 */ \
cString m_filename;                                                                     /*  4 */ \
public:                                                                                   /*  5 */ \
cAction ## METHOD(cWorld* world, const cString& args, Feedback&) : cAction(world, args)            /*  6 */ \
{                                                                                       /*  7 */ \
cString largs(args);                                                                  /*  8 */ \
if (largs == "") m_filename = #DEFAULT; else m_filename = largs.PopWord();            /*  9 */ \
}                                                                                       /* 10 */ \
static const cString GetDescription() { return "Arguments: [string fname=\"" #DEFAULT "\"]"; }  /* 11 */ \
void Process(cAvidaContext&) { m_world->GetStats().METHOD(m_filename); }            /* 12 */ \
}                                                                                         /* 13 */ \

STATS_OUT_FILE(PrintAverageData,            average.dat         );
STATS_OUT_FILE(PrintDemeAverageData,        deme_average.dat    );
STATS_OUT_FILE(PrintErrorData,              error.dat           );
STATS_OUT_FILE(PrintVarianceData,           variance.dat        );
STATS_OUT_FILE(PrintCountData,              count.dat           );
STATS_OUT_FILE(PrintMessageData,            message.dat         );
STATS_OUT_FILE(PrintMessageLog,             message_log.dat     );
STATS_OUT_FILE(PrintRetMessageLog,          retmessage_log.dat  );
STATS_OUT_FILE(PrintInterruptData,          interrupt.dat       );
STATS_OUT_FILE(PrintTotalsData,             totals.dat          );
STATS_OUT_FILE(PrintTasksData,              tasks.dat           );
STATS_OUT_FILE(PrintThreadsData,            threads.dat         );
STATS_OUT_FILE(PrintHostTasksData,          host_tasks.dat      );
STATS_OUT_FILE(PrintParasiteTasksData,      parasite_tasks.dat  );
STATS_OUT_FILE(PrintTasksExeData,           tasks_exe.dat       );
STATS_OUT_FILE(PrintNewTasksData,           newtasks.dat	);
STATS_OUT_FILE(PrintNewReactionData,	    newreactions.dat	);
STATS_OUT_FILE(PrintNewTasksDataPlus,       newtasksplus.dat	);
STATS_OUT_FILE(PrintTasksQualData,          tasks_quality.dat   );
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
STATS_OUT_FILE(PrintPreyAverageData,        prey_average.dat   );
STATS_OUT_FILE(PrintPredatorAverageData,    predator_average.dat   );
STATS_OUT_FILE(PrintTopPredatorAverageData,    top_pred_average.dat   );
STATS_OUT_FILE(PrintPreyErrorData,          prey_error.dat   );
STATS_OUT_FILE(PrintPredatorErrorData,      predator_error.dat   );
STATS_OUT_FILE(PrintTopPredatorErrorData,      top_pred_error.dat   );
STATS_OUT_FILE(PrintPreyVarianceData,       prey_variance.dat   );
STATS_OUT_FILE(PrintPredatorVarianceData,   predator_variance.dat   );
STATS_OUT_FILE(PrintTopPredatorVarianceData,   top_pred_variance.dat   );
STATS_OUT_FILE(PrintSenseData,              sense.dat           );
STATS_OUT_FILE(PrintSenseExeData,           sense_exe.dat       );
STATS_OUT_FILE(PrintInternalTasksData,      in_tasks.dat        );
STATS_OUT_FILE(PrintInternalTasksQualData,  in_tasks_quality.dat);
STATS_OUT_FILE(PrintSleepData,              sleep.dat           );
STATS_OUT_FILE(PrintCompetitionData,        competition.dat     );
STATS_OUT_FILE(PrintDemeReplicationData,    deme_repl.dat       );
STATS_OUT_FILE(PrintDemeGermlineSequestration, deme_germ.dat);
STATS_OUT_FILE(PrintDemeOrgGermlineSequestration, deme_org_germ.dat);
STATS_OUT_FILE(PrintDemeGLSFounders, deme_gls_founders.dat);
STATS_OUT_FILE(PrintDemeReactionDiversityReplicationData, deme_rx_repl.dat );
STATS_OUT_FILE(PrintDemeGermResourcesData, deme_germ_res.dat );
STATS_OUT_FILE(PrintWinningDeme, deme_winners.dat);
STATS_OUT_FILE(PrintDemeTreatableReplicationData,    deme_repl_treatable.dat       );
STATS_OUT_FILE(PrintDemeUntreatableReplicationData,  deme_repl_untreatable.dat       );
STATS_OUT_FILE(PrintDemeTreatableCount,     deme_treatable.dat       );


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
STATS_OUT_FILE(PrintDemesTasksData,         demes_tasks.dat); //@JJB**
STATS_OUT_FILE(PrintDemesReactionsData,     demes_reactions.dat); //@JJB**
STATS_OUT_FILE(PrintDemesFitnessData,       demes_fitness.dat); //@JJB**
STATS_OUT_FILE(PrintDemeReactionData,       deme_reactions.dat  );
STATS_OUT_FILE(PrintDemeOrgTasksData,       deme_org_tasks.dat      );
STATS_OUT_FILE(PrintDemeOrgTasksExeData,    deme_org_tasks_exe.dat  );
STATS_OUT_FILE(PrintDemeOrgReactionData,    deme_org_reactions.dat  );
STATS_OUT_FILE(PrintDemeCurrentTaskExeData,	deme_cur_task_exe.dat	);
STATS_OUT_FILE(PrintDemeMigrationSuicidePoints,	deme_mig_suicide_points.dat	);
STATS_OUT_FILE(PrintMultiProcessData,       multiprocess.dat);
STATS_OUT_FILE(PrintProfilingData,          profiling.dat);
STATS_OUT_FILE(PrintOrganismLocation,       location.dat);

STATS_OUT_FILE(PrintCurrentTaskCounts,      curr_task_counts.dat);
STATS_OUT_FILE(PrintGermlineData,           germline.dat        );
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
STATS_OUT_FILE(PrintFlowRateTuples,         flow_rate_tuples.dat        );
STATS_OUT_FILE(PrintDynamicMaxMinData,	    maxmin.dat			);
STATS_OUT_FILE(PrintNumOrgsKilledData,      orgs_killed.dat);
STATS_OUT_FILE(PrintMigrationData,          migration.dat);
STATS_OUT_FILE(PrintAgePolyethismData,      age_polyethism.dat);
STATS_OUT_FILE(PrintIntrinsicTaskSwitchingCostData, intrinsic_task_switching_cost.dat);
STATS_OUT_FILE(PrintDenData, den_data.dat);

//mating type/male-female stats data
STATS_OUT_FILE(PrintMaleAverageData,    male_average.dat   );
STATS_OUT_FILE(PrintFemaleAverageData,    female_average.dat   );
STATS_OUT_FILE(PrintMaleErrorData, male_error.dat   );
STATS_OUT_FILE(PrintFemaleErrorData, female_error.dat   );
STATS_OUT_FILE(PrintMaleVarianceData, male_variance.dat   );
STATS_OUT_FILE(PrintFemaleVarianceData, female_variance.dat   );

// reputation
STATS_OUT_FILE(PrintReputationData,         reputation.dat);
STATS_OUT_FILE(PrintShadedAltruists,         shadedaltruists.dat);
STATS_OUT_FILE(PrintDirectReciprocityData,         reciprocity.dat);
STATS_OUT_FILE(PrintStringMatchData,         stringmatch.dat);

// kabooms
STATS_OUT_FILE(PrintKaboom, kabooms.dat);
STATS_OUT_FILE(PrintQuorum, threshold.dat);

// group formation
STATS_OUT_FILE(PrintGroupsFormedData,         groupformation.dat);
STATS_OUT_FILE(PrintGroupIds,                 groupids.dat);
STATS_OUT_FILE(PrintTargets,                  targets.dat);
STATS_OUT_FILE(PrintMimicDisplays,            mimics.dat);
STATS_OUT_FILE(PrintTopPredTargets,           top_pred_targets.dat);
STATS_OUT_FILE(PrintToleranceInstructionData, toleranceinstruction.dat); 
STATS_OUT_FILE(PrintToleranceData,            tolerance.dat);

STATS_OUT_FILE(PrintAttacks,            attacks.dat);

// hgt information
STATS_OUT_FILE(PrintHGTData, hgt.dat);


#define POP_OUT_FILE(METHOD, DEFAULT)                                                     /*  1 */ \
class cAction ## METHOD : public cAction {                                                /*  2 */ \
private:                                                                                  /*  3 */ \
cString m_filename;                                                                     /*  4 */ \
public:                                                                                   /*  5 */ \
cAction ## METHOD(cWorld* world, const cString& args, Feedback&) : cAction(world, args)            /*  6 */ \
{                                                                                       /*  7 */ \
cString largs(args);                                                                  /*  8 */ \
if (largs == "") m_filename = #DEFAULT; else m_filename = largs.PopWord();            /*  9 */ \
}                                                                                       /* 10 */ \
static const cString GetDescription() { return "Arguments: [string fname=\"" #DEFAULT "\"]"; }  /* 11 */ \
void Process(cAvidaContext&) { m_world->GetPopulation().METHOD(m_filename); }       /* 12 */ \
}                                                                                         /* 13 */ \

POP_OUT_FILE(PrintPhenotypeData,       phenotype_count.dat );
POP_OUT_FILE(PrintHostPhenotypeData,      host_phenotype_count.dat );
POP_OUT_FILE(PrintParasitePhenotypeData,  parasite_phenotype_count.dat );
POP_OUT_FILE(PrintPhenotypeStatus,     phenotype_status.dat);
POP_OUT_FILE(PrintDemeTestamentStats,  deme_testament.dat  );
POP_OUT_FILE(PrintCurrentMeanDemeDensity,  deme_currentMeanDensity.dat  );


class cActionPrintResourceData : public cAction
{
private:
  cString m_filename;
  cString m_maps;
public:
  cActionPrintResourceData(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    m_filename = (largs.GetSize()) ? largs.PopWord() :  "resource.dat";
    m_maps = (largs.GetSize()) ? largs.PopWord() : "1"; 
  }
  static const cString GetDescription() { return "Arguments: [string fname=\"resource.dat\"]"; }
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().UpdateResStats(ctx);
    m_world->GetStats().PrintResourceData(m_filename, m_maps);
  }
};


class cActionPrintSpatialResources : public cAction
{
private:
  cString m_filename;
  bool m_first_run;
public:
  
  cActionPrintSpatialResources(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_first_run(true)
  {
    cString largs(args);
    m_filename = (largs.GetSize()) ? largs.PopWord() :  "resources.dat";
  }
  static const cString GetDescription() { return "Arguments: [string fname=\"resource.dat\"]"; }
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().UpdateResStats(ctx);
    const cStats& stats = m_world->GetStats();
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    if (m_first_run){
      df->WriteComment("Avida Spatial Resource Information");
      df->WriteComment("Column 1 is the update");
      df->WriteComment("Column 2 is the name of the resource");
      df->WriteComment("The remaining columns are abundance of the named resource per cell");
      df->FlushComments();
      m_first_run = false;
    }
    
    cPopulation& pop = m_world->GetPopulation();
    const cResourceCount& res_count = pop.GetResourceCount();
    int world_size = pop.GetSize();
    
    for (int res_id=0; res_id < stats.GetResources().GetSize(); res_id++){
      int geometry = stats.GetResourceGeometries()[res_id];
      if (res_count.IsSpatialResource(res_id)){
        df->WriteAnonymous(stats.GetUpdate());
        df->WriteAnonymous(stats.GetResourceNames()[res_id]);
        for (int cell_ndx=0; cell_ndx < world_size; cell_ndx++){
          df->OFStream() << stats.GetSpatialResourceCount()[res_id][cell_ndx];
          if (cell_ndx < world_size-1){
            df->OFStream() << " ";
          } 
        }
        df->Endl();
      }
    }
  }
};

class cActionPrintResourceLocData : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintResourceLocData(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "resourceloc.dat"; else m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname=\"resourceloc.dat\"]"; }
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().TriggerDoUpdates(ctx);
    m_world->GetStats().PrintResourceLocData(m_filename, ctx);
  }
};

class cActionPrintResWallLocData : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintResWallLocData(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "reswallloc.dat"; else m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname=\"reswallloc.dat\"]"; }
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().TriggerDoUpdates(ctx);
    m_world->GetStats().PrintResWallLocData(m_filename, ctx);
  }
};

class cActionPrintGroupTolerance : public cAction 
{
private:
  cString m_filename;
public:
  cActionPrintGroupTolerance(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "grouptolerance.dat"; else m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname=\"grouptolerance.dat\"]"; }
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().UpdateResStats(ctx);
    m_world->GetStats().PrintGroupTolerance(m_filename);
  }
};

class cActionPrintGroupMTTolerance : public cAction 
{
private:
  cString m_filename;
public:
  cActionPrintGroupMTTolerance(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "groupMTtolerance.dat"; else m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname=\"groupMTtolerance.dat\"]"; }
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().UpdateResStats(ctx);
    m_world->GetStats().PrintGroupMTTolerance(m_filename);
  }
};

class cActionPrintData : public cAction
{
private:
  cString m_filename;
  cString m_format;
public:
  cActionPrintData(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    m_filename = largs.PopWord();
    m_format = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <cString fname> <cString format>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetStats().PrintDataFile(m_filename, m_format, ',');
  }
};

class cActionPrintInstructionData : public cAction, public Data::Recorder
{
private:
  cString m_filename;
  Apto::String m_inst_set;
  Data::DataID m_data_id;
  Data::PackagePtr m_data;
  
public:
  cActionPrintInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "instruction.dat";
    }
    if (largs.GetSize()) m_inst_set = (const char*)largs.PopWord();
    
    if (m_filename == "") m_filename.Set("instruction-%s.dat", (const char*)m_inst_set);
    
    m_data_id = Apto::FormatStr("core.population.inst_exec_counts[%s]", (const char*)m_inst_set);
    
    Data::RecorderPtr thisPtr(this);
    this->AddReference();
    m_world->GetDataManager()->AttachRecorder(thisPtr);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  Data::ConstDataSetPtr RequestedData() const
  {
    Data::DataSetPtr ds(new Data::DataSet);
    ds->Insert(m_data_id);
    return ds;
  }
  
  
  void NotifyData(Update, Data::DataRetrievalFunctor retrieve_data)
  {
    m_data = retrieve_data(m_data_id);
  }
  
  void Process(cAvidaContext&)
  {
    const cInstSet& is = m_world->GetHardwareManager().GetInstSet(m_inst_set);
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    df->WriteComment("Avida instruction execution data");
    df->WriteTimeStamp();
    
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    
    if (m_data) {
      for (int i = 0; i < m_data->NumComponents(); i++) {
        df->Write(m_data->GetComponent(i)->IntValue(), is.GetName(i));
      }
    }
    
    df->Endl();
  }
};

class cActionPrintFromMessageInstructionData : public cAction, public Data::Recorder
{
private:
  cString m_filename;
  Apto::String m_inst_set;
  Data::DataID m_data_id;
  Data::PackagePtr m_data;
  
public:
  cActionPrintFromMessageInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "from_msg_instruction.dat";
    }
    if (largs.GetSize()) m_inst_set = (const char*)largs.PopWord();
    
    if (m_filename == "") m_filename.Set("from_msg_instruction-%s.dat", (const char*)m_inst_set);
    
    m_data_id = Apto::FormatStr("core.population.from_message_inst_exec_counts[%s]", (const char*)m_inst_set);
    
    Data::RecorderPtr thisPtr(this);
    this->AddReference();
    m_world->GetDataManager()->AttachRecorder(thisPtr);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"from_msg_instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  Data::ConstDataSetPtr RequestedData() const
  {
    Data::DataSetPtr ds(new Data::DataSet);
    ds->Insert(m_data_id);
    return ds;
  }
  
  
  void NotifyData(Update, Data::DataRetrievalFunctor retrieve_data)
  {
    m_data = retrieve_data(m_data_id);
  }
  
  void Process(cAvidaContext&)
  {
    const cInstSet& is = m_world->GetHardwareManager().GetInstSet(m_inst_set);
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    df->WriteComment("Avida from message instruction execution data");
    df->WriteTimeStamp();
    
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    
    if (m_data) {
      for (int i = 0; i < m_data->NumComponents(); i++) {
        df->Write(m_data->GetComponent(i)->IntValue(), is.GetName(i));
      }
    }
    
    df->Endl();
  }
};


class cActionPrintPreyInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintPreyInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "prey_instruction.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("prey_instruction-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"prey_instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetStats().PrintPreyInstructionData(m_filename, m_inst_set);
  }
};

class cActionPrintPredatorInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintPredatorInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "predator_instruction.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("predator_instruction-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"predator_instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetStats().PrintPredatorInstructionData(m_filename, m_inst_set);
  }
};

class cActionPrintTopPredatorInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintTopPredatorInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "top_pred_instruction.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("top_pred_instruction-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"top_pred_instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintTopPredatorInstructionData(m_filename, m_inst_set);
  }
};

class cActionPrintPreyFromSensorInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintPreyFromSensorInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "prey_from_sensor_exec.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("prey_from_sensor_exec-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"prey_from_sensor_exec-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintPreyFromSensorInstructionData(m_filename, m_inst_set);
  }
};

class cActionPrintPredatorFromSensorInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintPredatorFromSensorInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "predator_from_sensor_exec.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("predator_from_sensor_exec-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"predator_from_sensor_exec-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintPredatorFromSensorInstructionData(m_filename, m_inst_set);
  }
};

class cActionPrintTopPredatorFromSensorInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintTopPredatorFromSensorInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "top_pred_from_sensor_exec.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("top_pred_from_sensor_exec-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"top_pred_from_sensor_exec-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintTopPredatorFromSensorInstructionData(m_filename, m_inst_set);
  }
};

class cActionPrintGroupAttackData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintGroupAttackData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "group_attacks.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("group_attacks-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"group_attacks-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintGroupAttackData(m_filename, m_inst_set);
  }
};

class cActionPrintKilledPreyFTData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintKilledPreyFTData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args)
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "killed_prey.dat";
    }
    if (m_filename == "") m_filename.Set("killed_prey.dat");
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"killed_prey.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintKilledPreyFTData(m_filename);
  }
};

class cActionPrintMaleInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintMaleInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "male_instruction.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("male_instruction-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"male_instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetStats().PrintMaleInstructionData(m_filename, m_inst_set);
  }
};

class cActionPrintFemaleInstructionData : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintFemaleInstructionData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_inst_set(world->GetHardwareManager().GetDefaultInstSet().GetInstSetName())
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else {
      if (m_filename == "") m_filename = "female_instruction.dat";
    }
    if (largs.GetSize()) m_inst_set = largs.PopWord();
    
    if (m_filename == "") m_filename.Set("female_instruction-%s.dat", (const char*)m_inst_set);
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"female_instruction-${inst_set}.dat\"] [string inst_set]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintFemaleInstructionData(m_filename, m_inst_set);
  }
};


class cActionPrintInstructionAbundanceHistogram : public cAction
{
private:
  cString m_filename;
  cString m_inst_set;
  
public:
  cActionPrintInstructionAbundanceHistogram(cWorld* world, const cString& args, Feedback&)
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
  void Process(cAvidaContext&)
  {
    cPopulation& population = m_world->GetPopulation();
    
    // ----- number of instructions available?
    const cInstSet& is = m_world->GetHardwareManager().GetInstSet((const char*)m_inst_set);
    
    Apto::Array<int> inst_counts(is.GetSize());
    inst_counts.SetAll(0);
    
    //looping through all CPUs counting up instructions
    const int num_cells = population.GetSize();
    for (int x = 0; x < num_cells; x++) {
      cPopulationCell& cell = population.GetCell(x);
      if (cell.IsOccupied() && cell.GetOrganism()->GetGenome().Properties().Get("instset").StringValue() == is.GetInstSetName()) {
        // access this CPU's code block
        cCPUMemory& cpu_mem = cell.GetOrganism()->GetHardware().GetMemory();
        const int mem_size = cpu_mem.GetSize();
        for (int y = 0; y < mem_size; y++) inst_counts[cpu_mem[y].GetOp()]++;
      }
    }
    
    // ----- output instruction counts
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    for (int i = 0; i < is.GetSize(); i++) df->Write(inst_counts[i], is.GetName(i));
    df->Endl();
  }
};


class cActionPrintDepthHistogram : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintDepthHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "depth_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"depth_histogram.dat\"]"; }
  void Process(cAvidaContext&)
  {
    // Output format:    update  min  max  histogram_values...
    int min = INT_MAX;
    int max = 0;
    
    // Two pass method
    
    // Loop through all genotypes getting min and max values
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    
    while (it->Next()) {
      Systematics::GroupPtr bg = it->Get();
      if (bg->Depth() < min) min = bg->Depth();
      if (bg->Depth() > max) max = bg->Depth();
    }
    assert(max >= min);
    
    // Allocate the array for the bins (& zero)
    Apto::Array<int> n(max - min + 1);
    n.SetAll(0);
    
    // Loop through all genotypes binning the values
    it = classmgr->ArbiterForRole("genotype")->Begin();
    while (it->Next()) {
      n[it->Get()->Depth() - min] += it->Get()->NumUnits();
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(min, "Minimum");
    df->Write(max, "Maximum");
    for (int i = 0; i < n.GetSize(); i++)  df->WriteAnonymous(n[i]);
    df->Endl();
  }
};

//Depth Histogram for Parasites Only
class cActionPrintParasiteDepthHistogram : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintParasiteDepthHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "depth_parasite_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"depth_parasite_histogram.dat\"]"; }
  
  void Process(cAvidaContext&)
  {
    // Output format:    update  min  max  histogram_values...
    int min = INT_MAX;
    int max = 0;
    
    // Two pass method
    
    // Loop through all genotypes getting min and max values
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    
    while (it->Next()) {
      Systematics::GroupPtr bg = it->Get();
      int transmission_type = Apto::StrAs(bg->Properties().Get("src_transmission_type"));
      if(transmission_type == Systematics::HORIZONTAL || transmission_type == Systematics::VERTICAL)
      {
        if (bg->Depth() < min) min = bg->Depth();
        if (bg->Depth() > max) max = bg->Depth();
      }
    }
    
    //crappy hack, but sometimes we wont have parasite genotypes
    if(min == INT_MAX) min=0;
    
    assert(max >= min);
    
    // Allocate the array for the bins (& zero)
    Apto::Array<int> n(max - min + 1);
    n.SetAll(0);
    
    // Loop through all genotypes binning the values
    it = classmgr->ArbiterForRole("genotype")->Begin();
    while (it->Next()) {
      Systematics::GroupPtr bg = it->Get();
      int transmission_type = Apto::StrAs(bg->Properties().Get("src_transmission_type"));
      if(transmission_type == Systematics::HORIZONTAL || transmission_type == Systematics::VERTICAL)
      {
        n[bg->Depth() - min] += bg->NumUnits();
      }
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(min, "Minimum");
    df->Write(max, "Maximum");
    for (int i = 0; i < n.GetSize(); i++)  df->WriteAnonymous(n[i]);
    df->Endl();
  }
};

//Depth Histogram for Parasites Only
class cActionPrintHostDepthHistogram : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintHostDepthHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "depth_host_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"depth_host_histogram.dat\"]"; }
  
  void Process(cAvidaContext&)
  {
    // Output format:    update  min  max  histogram_values...
    int min = INT_MAX;
    int max = 0;
    
    // Two pass method
    
    // Loop through all genotypes getting min and max values
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    
    while (it->Next()) {
      Systematics::GroupPtr bg = it->Get();
      int transmission_type = Apto::StrAs(bg->Properties().Get("src_transmission_type"));
      if(transmission_type == Systematics::HORIZONTAL || transmission_type == Systematics::VERTICAL)
      {
        if (bg->Depth() < min) min = bg->Depth();
        if (bg->Depth() > max) max = bg->Depth();
      }
    }
    
    assert(max >= min);
    
    // Allocate the array for the bins (& zero)
    Apto::Array<int> n(max - min + 1);
    n.SetAll(0);
    
    // Loop through all genotypes binning the values
    it = classmgr->ArbiterForRole("genotype")->Begin();
    while (it->Next()) {
      Systematics::GroupPtr bg = it->Get();
      int transmission_type = Apto::StrAs(bg->Properties().Get("src_transmission_type"));
      if(transmission_type == Systematics::HORIZONTAL || transmission_type == Systematics::VERTICAL)
      {
        n[bg->Depth() - min] += bg->NumUnits();
      }
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(min, "Minimum");
    df->Write(max, "Maximum");
    for (int i = 0; i < n.GetSize(); i++)  df->WriteAnonymous(n[i]);
    df->Endl();
  }
};


class cActionEcho : public cAction
{
private:
  cString m_filename;
public:
  cActionEcho(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "Arguments: <cString message>"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_args == "") {
      m_args.Set("Echo : Update = %f\t AveGeneration = %f", m_world->GetStats().GetUpdate(),
                 m_world->GetStats().SumGeneration().Average());
    }
    ctx.Driver().Feedback().Notify(m_args);
  }
};


class cActionPrintGenotypeAbundanceHistogram : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintGenotypeAbundanceHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "genotype_abundance_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"genotype_abundance_histogram.dat\"]"; }
  void Process(cAvidaContext&)
  {
    // Allocate array for the histogram & zero it
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    Apto::Array<int> hist(it->Next()->NumUnits());
    hist.SetAll(0);
    
    // Loop through all genotypes binning the values
    do {
      assert(it->Get()->NumUnits() - 1 >= 0);
      assert(it->Get()->NumUnits() - 1 < hist.GetSize());
      hist[it->Get()->NumUnits() - 1]++;
    } while (it->Next());
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    for (int i = 0; i < hist.GetSize(); i++) df->Write(hist[i],"");
    df->Endl();
  }
};


//LHZ - slower version that doesn't need "lineage support"
class cActionPrintLineageCounts : public cAction
{
private:
  cString m_filename;
  bool first_run;
  Apto::Array<int> lineage_labels;
public:
  cActionPrintLineageCounts(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord(); else m_filename = "lineage_counts.dat";
    first_run = false;
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='lineage_counts.dat']\n  WARNING: This will only have the appropriate header if all lineages are present before this action is run for the first time."; }
  void Process(cAvidaContext&)
  {
    const int update = m_world->GetStats().GetUpdate();
    const double generation = m_world->GetStats().SumGeneration().Average();
    
    //only loop through living organisms
    const Apto::Array<cOrganism*, Apto::Smart>& living_orgs = m_world->GetPopulation().GetLiveOrgList();
    
    Apto::Map<int, int> lineage_label_counts;
    
    //build hash of lineage_label -> count
    for(int i = 0; i < living_orgs.GetSize(); i++) {
      const int cur_lineage_label = living_orgs[i]->GetLineageLabel();
      if (lineage_label_counts.Has(cur_lineage_label)) lineage_label_counts[cur_lineage_label]++;
      else lineage_label_counts[cur_lineage_label] = 1;
    }
    
    //setup lineage labels in the first pass
    if (first_run == false) {
      for (Apto::Map<int, int>::KeyIterator it = lineage_label_counts.Keys(); it.Next();) lineage_labels.Push(*it.Get());
      first_run = true;
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(update, "Update");
    df->Write(generation, "Generation");
    
    //for each lineage label, output the counts
    //@LZ - handle dead lineages appropriately
    for (int i = 0; i < lineage_labels.GetSize(); i++) {
      //default to 0 in case this lineage is dead
      int count = 0;
      
      lineage_label_counts.Get(lineage_labels[i], count);
      
      df->Write(count, cStringUtil::Stringf("Lineage Label %d", lineage_labels[i]));
    }
    
    df->Endl();
  }
};


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
  cActionPrintDominantGenotype(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    Systematics::GroupPtr bg = it->Next();
    if (bg) {
      cString filename(m_filename);
      if (filename == "") filename.Set("archive/%s.org", (const char*)bg->Properties().Get("name").StringValue());
      cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
      testcpu->PrintGenome(ctx, Genome(bg->Properties().Get("genome")), filename, m_world->GetStats().GetUpdate());
      delete testcpu;
    }
  }
};

class cActionPrintDominantGroupGenotypes : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintDominantGroupGenotypes(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    int num_groups = 0;
    map<int,int> groups_formed = m_world->GetPopulation().GetFormedGroups();    
    map <int,int>::iterator itr;    
    for(itr = groups_formed.begin();itr!=groups_formed.end();itr++) {
      double cur_size = itr->second;
      if (cur_size > 0) num_groups++; 
    }
    
    Apto::Array<int, Apto::Smart> birth_groups_checked;
    
    for (int i = 0; i < num_groups; i++) {
      Systematics::GroupPtr bg = it->Next();
      bool already_used = false;
      
      if (!bg) break;
      
      if (bg && ((bool)Apto::StrAs(bg->Properties().Get("threshold")) || i == 0)) {
        int last_birth_group_id = Apto::StrAs(bg->Properties().Get("last_group_id")); 
        int last_birth_cell = Apto::StrAs(bg->Properties().Get("last_birth_cell"));
        int last_birth_forager_type = Apto::StrAs(bg->Properties().Get("last_forager_type")); 
        if (i != 0) {
          for (int j = 0; j < birth_groups_checked.GetSize(); j++) {
            if (last_birth_group_id == birth_groups_checked[j]) {
              already_used = true;
              i--;
              break;
            }
          }
        }
        if (!already_used) birth_groups_checked.Push(last_birth_group_id);
        if (already_used) continue;
        
        cString filename(m_filename);
        if (filename == "") filename.Set("archive/grp%d_ft%d_%s.org", last_birth_group_id, last_birth_forager_type, (const char*)bg->Properties().Get("name").StringValue());
        else filename = filename.Set(filename + "grp%d_ft%d", last_birth_group_id, last_birth_forager_type); 
        
        // need a random number generator to pass to testcpu that does not affect any other random number pulls (since this is just for printing the genome)
        Apto::RNG::AvidaRNG rng(0);
        cAvidaContext ctx2(&m_world->GetDriver(), rng);
        cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx2);
        testcpu->PrintGenome(ctx2, Genome(bg->Properties().Get("genome")), filename, m_world->GetStats().GetUpdate(), true, last_birth_cell, last_birth_group_id, last_birth_forager_type);
        delete testcpu;
      }
    }
  }
};

class cActionPrintDominantForagerGenotypes : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintDominantForagerGenotypes(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    int num_fts = 1;
    if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) num_fts = 3;
    else num_fts = 1;  // account for -1's
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;    
    for(itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) num_fts++;
    
    Apto::Array<int, Apto::Smart> birth_forage_types_checked;
    
    for (int i = 0; i < num_fts; i++) {
      bool already_used = false;
      Systematics::GroupPtr bg = it->Next();
      
      if (!bg) break;
      
      if (bg && ((bool)Apto::StrAs(bg->Properties().Get("threshold")) || i == 0)) {
        int last_birth_group_id = Apto::StrAs(bg->Properties().Get("last_group_id")); 
        int last_birth_cell = Apto::StrAs(bg->Properties().Get("last_birth_cell"));
        int last_birth_forager_type = Apto::StrAs(bg->Properties().Get("last_forager_type")); 
        if (i != 0) {
          for (int j = 0; j < birth_forage_types_checked.GetSize(); j++) {
            if (last_birth_forager_type == birth_forage_types_checked[j]) { 
              already_used = true; 
              i--;
              break; 
            }
          }
        }
        if (!already_used) birth_forage_types_checked.Push(last_birth_forager_type);
        if (already_used) continue;
        
        
        cString filename(m_filename);
        if (filename == "") filename.Set("archive/ft%d_grp%d_%s.org", last_birth_forager_type, last_birth_group_id, (const char*)bg->Properties().Get("name").StringValue());
        else filename = filename.Set(filename + ".ft%d_grp%d", last_birth_forager_type, last_birth_group_id); 
        
        // need a random number generator to pass to testcpu that does not affect any other random number pulls (since this is just for printing the genome)
        Apto::RNG::AvidaRNG rng(0);
        cAvidaContext ctx2(&m_world->GetDriver(), rng);
        cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx2);
        testcpu->PrintGenome(ctx2, Genome(bg->Properties().Get("genome")), filename, m_world->GetStats().GetUpdate(), true, last_birth_cell, last_birth_group_id, last_birth_forager_type);
        delete testcpu;
      }
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
  cActionPrintDetailedFitnessData(cWorld* world, const cString& args, Feedback&)
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
    Apto::Array<int> histo;
    Apto::Array<int> histo_testCPU;
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
    Systematics::GroupPtr max_f_genotype;
    
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
    
    for (int i = 0; i < pop.GetSize(); i++) {
      if (pop.GetCell(i).IsOccupied() == false) continue;  // One use organisms.
      
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      Systematics::GroupPtr genotype = organism->SystematicsGroup("genotype");
      
      cCPUTestInfo test_info;
      testcpu->TestGenome(ctx, test_info, Genome(genotype->Properties().Get("genome")));
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
    if ((bool)Apto::StrAs(max_f_genotype->Properties().Get("threshold")))
      max_f_name = max_f_genotype->Properties().Get("name").StringValue();
    else {
      // we put the current update into the name, so that it becomes unique.
      Genome gen(max_f_genotype->Properties().Get("genome"));
      InstructionSequencePtr seq;
      seq.DynamicCastFrom(gen.Representation());
      max_f_name.Set("%03d-no_name-u%i", seq->GetSize(), update);
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filenames[0]);
    df->Write(update, "Update");
    df->Write(generation, "Generation");
    df->Write(fave / static_cast<double>(n), "Average Fitness");
    df->Write(fave_testCPU / static_cast<double>(n), "Average Test Fitness");
    df->Write(n, "Organism Total");
    df->Write(max_fitness, "Maximum Fitness");
    df->Write(max_f_name, "Maxfit genotype name");
    df->Endl();
    
    if (m_save_max) {
      cString filename;
      filename.Set("archive/%s", static_cast<const char*>(max_f_name));
      testcpu->PrintGenome(ctx, Genome(max_f_genotype->Properties().Get("genome")), filename);
    }
    
    delete testcpu;
    
    if (m_print_fitness_histo) {
      Avida::Output::FilePtr hdf = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filenames[1]);
      hdf->Write(update, "Update");
      hdf->Write(generation, "Generation");
      hdf->Write(fave / static_cast<double>(n), "Average Fitness");
      
      // now output the fitness histo
      for (int i = 0; i < histo.GetSize(); i++)
        hdf->WriteAnonymous(static_cast<double>(histo[i]) / static_cast<double>(nhist_tot));
      hdf->Endl();
      
      
      Avida::Output::FilePtr tdf = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filenames[2]);
      tdf->Write(update, "Update");
      tdf->Write(generation, "Generation");
      tdf->Write(fave / static_cast<double>(n), "Average Fitness");
      
      // now output the fitness histo
      for (int i = 0; i < histo_testCPU.GetSize(); i++)
        tdf->WriteAnonymous(static_cast<double>(histo_testCPU[i]) / static_cast<double>(nhist_tot_testCPU));
      tdf->Endl();
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
  cActionPrintCCladeCounts(cWorld* world, const cString& args, Feedback&)
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
    if (ctx.GetAnalyzeMode()) {
      ctx.Driver().Feedback().Error("PrintCCladeCount requires avida to be in run mode.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    if (m_world->GetConfig().TRACK_CCLADES.Get() == 0) {
      ctx.Driver().Feedback().Error("PrintCCladeCount requires coalescence clade tracking to be enabled.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    
    Apto::Map<int, int> cclade_count;  //A count for each clade in the population
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
      if (!cclade_count.Get(cclade_id, count))
        clade_ids.insert(cclade_id);
      cclade_count.Set(cclade_id, ++count);
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    if (!fp.is_open()) {
      ctx.Driver().Feedback().Error("PrintCCladeCount: Unable to open output file.");
      ctx.Driver().Abort(Avida::IO_ERROR);
    }
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
      cclade_count.Get(*sit, count);
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
  cActionPrintLogFitnessHistogram(cWorld* world, const cString& args, Feedback&)
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
  static Apto::Array<int> MakeHistogram(const Apto::Array<cOrganism*>& orgs, const Apto::Array<Systematics::GroupPtr>& gens,
                                        double min, double step, double& max, const cString& mode, cWorld* world,
                                        cAvidaContext& ctx)
  {
    //Set up histogram; extra columns prepended (non-viable, < m_hist_fmin) and appended ( > f_hist_fmax)
    //If the bin size is not a multiple of the step size, the last bin is expanded to make it a multiple.
    //All bins are [min, max)
    Apto::Array<int> histogram;
    int num_bins = static_cast<int>(ceil( (max - min) / step)) + 3;
    max  = min + (num_bins - 3) * step;
    histogram.Resize(num_bins, 0);
    cTestCPU* testcpu = world->GetHardwareManager().CreateTestCPU(ctx);
    
    
    // We calculate the fitness based on the current merit,
    // but with the true gestation time. Also, we set the fitness
    // to zero if the creature is not viable.
    for (int i = 0; i < gens.GetSize(); i++) {
      cCPUTestInfo test_info;
      double fitness = 0.0;
      if (mode == "TEST_CPU" || mode == "ACTUAL"){
        test_info.UseManualInputs(orgs[i]->GetOrgInterface().GetInputs());
        testcpu->TestGenome(ctx, test_info, Genome(gens[i]->Properties().Get("genome")));
      }
      
      if (mode == "TEST_CPU"){
        fitness = test_info.GetColonyFitness();
      }
      else if (mode == "CURRENT"){
        fitness = orgs[i]->GetPhenotype().GetFitness();
      }
      else if (mode == "ACTUAL"){
        fitness = (test_info.IsViable()) ?
        orgs[i]->GetPhenotype().GetMerit().CalcFitness(test_info.GetTestPhenotype().GetGestationTime()) : 0.0;
      } else {
        ctx.Driver().Feedback().Error("PrintLogFitnessHistogram::MakeHistogram: Invalid fitness mode requested.");
        ctx.Driver().Abort(Avida::INVALID_CONFIG);
      }
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
    Apto::Array<cOrganism*> orgs;
    Apto::Array<Systematics::GroupPtr> gens;
    
    for (int i = 0; i < pop.GetSize(); i++)
    {
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      Systematics::GroupPtr genotype = organism->SystematicsGroup("genotype");
      orgs.Push(organism);
      gens.Push(genotype);
    }
    
    Apto::Array<int> histogram = MakeHistogram(orgs, gens, m_hist_fmin, m_hist_fstep, m_hist_fmax, m_mode, m_world, ctx);
    
    
    //Output histogram
    Avida::Output::FilePtr hdf = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    hdf->Write(update, "Update");
    hdf->Write(generation, "Generation");
    
    for (int k = 0; k < histogram.GetSize(); k++)
      hdf->Write(histogram[k], GetHistogramBinLabel(k, m_hist_fmin, m_hist_fstep, m_hist_fmax));
    hdf->Endl();
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
  
public:
  cActionPrintRelativeFitnessHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
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
  
  static Apto::Array<int> MakeHistogram(const Apto::Array<cOrganism*>& orgs, const Apto::Array<Systematics::GroupPtr>& gens,
                                        double min, double step, double& max, const cString& mode, cWorld* world,
                                        cAvidaContext& ctx)
  {
    //Set up histogram; extra columns prepended (non-viable, < m_hist_fmin) and appended ( > f_hist_fmax)
    //If the bin size is not a multiple of the step size, the last bin is expanded to make it a multiple.
    //All bins are [min, max)
    Apto::Array<int> histogram;
    int num_bins = static_cast<int>(ceil( (max - min) / step)) + 3;
    max  = min + (num_bins - 3) * step;
    histogram.Resize(num_bins, 0);
    cTestCPU* testcpu = world->GetHardwareManager().CreateTestCPU(ctx);
    
    
    // We calculate the fitness based on the current merit,
    // but with the true gestation time. Also, we set the fitness
    // to zero if the creature is not viable.
    for (int i = 0; i < gens.GetSize(); i++){
      cCPUTestInfo test_info;
      double fitness = 0.0;
      double parent_fitness = 1.0;
      if (gens[i]->Properties().Get("parents").StringValue() != "") {
        cStringList parents((const char*)gens[i]->Properties().Get("parents").StringValue(), ',');
        
        Systematics::GroupPtr pbg = Systematics::Manager::Of(world->GetNewWorld())->ArbiterForRole("genotype")->Group(parents.Pop().AsInt());
        parent_fitness = Apto::StrAs(pbg->Properties().Get("fitness"));
      }
      
      if (mode == "TEST_CPU" || mode == "ACTUAL"){
        test_info.UseManualInputs( orgs[i]->GetOrgInterface().GetInputs() );
        testcpu->TestGenome(ctx, test_info, Genome(gens[i]->Properties().Get("genome")));
      }
      
      if (mode == "TEST_CPU"){
        fitness = test_info.GetColonyFitness();
      }
      else if (mode == "CURRENT"){
        fitness = orgs[i]->GetPhenotype().GetFitness();
      }
      else if (mode == "ACTUAL"){
        fitness = (test_info.IsViable()) ?
        orgs[i]->GetPhenotype().GetMerit().CalcFitness(test_info.GetTestPhenotype().GetGestationTime()) : 0.0;
      } else {
        ctx.Driver().Feedback().Error("MakeHistogram: Invalid fitness mode requested.");
        ctx.Driver().Abort(Avida::INVALID_CONFIG);
      }
      
      //Update the histogram
      if (parent_fitness <= 0.0) {
        ctx.Driver().Feedback().Error(cString("PrintRelativeFitness::MakeHistogram reports a parent fitness is zero.") + gens[i]->Properties().Get("parents").StringValue());
        ctx.Driver().Abort(Avida::INTERNAL_ERROR);
      }
      
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
    if (ctx.GetAnalyzeMode()) {
      ctx.Driver().Feedback().Error("PrintRelativeFitnessHistogram requires avida to be in run mode.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    //Gather data objects
    cPopulation& pop        = m_world->GetPopulation();
    const int    update     = m_world->GetStats().GetUpdate();
    const double generation = m_world->GetStats().SumGeneration().Average();
    Apto::Array<cOrganism*> orgs;
    Apto::Array<Systematics::GroupPtr> gens;
    
    for (int i = 0; i < pop.GetSize(); i++)
    {
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      Systematics::GroupPtr genotype = organism->SystematicsGroup("genotype");
      orgs.Push(organism);
      gens.Push(genotype);
    }
    
    Apto::Array<int> histogram = MakeHistogram(orgs, gens, m_hist_fmin, m_hist_fstep, m_hist_fmax, m_mode, m_world, ctx);
    
    
    //Output histogram
    Avida::Output::FilePtr hdf = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    hdf->Write(update, "Update");
    hdf->Write(generation, "Generation");
    
    for (int k = 0; k < histogram.GetSize(); k++)
      hdf->Write(histogram[k], GetHistogramBinLabel(k, m_hist_fmin, m_hist_fstep, m_hist_fmax));
    hdf->Endl();
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
  cActionPrintCCladeFitnessHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
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
    if (ctx.GetAnalyzeMode()) {
      ctx.Driver().Feedback().Error("PrintCCladeFitnessHistogram requires avida to be in run mode.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    if (m_world->GetConfig().TRACK_CCLADES.Get() == 0) {
      ctx.Driver().Feedback().Error("PrintCCladeFitnessHistogram requires coalescence clade tracking to be enabled.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    //Verify input parameters
    if ( (m_mode != "ACTUAL" && m_mode != "CURRENT" && m_mode != "TESTCPU") || m_hist_fmin > m_hist_fmax) {
      ctx.Driver().Feedback().Error("PrintCCladeFitnessHistogram: Check parameters.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    //Gather data objects
    cPopulation& pop        = m_world->GetPopulation();
    const int    update     = m_world->GetStats().GetUpdate();
    map< int, Apto::Array<cOrganism*> > org_map;  //Map of ccladeID to array of organism IDs
    map< int, Apto::Array<Systematics::GroupPtr> > gen_map;  //Map of ccladeID to array of genotype IDs
    
    //Collect clade information
    for (int i = 0; i < pop.GetSize(); i++){
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      Systematics::GroupPtr genotype = organism->SystematicsGroup("genotype");
      int cladeID = organism->GetCCladeLabel();
      
      map< int, Apto::Array<cOrganism*> >::iterator oit = org_map.find(cladeID);
      map< int, Apto::Array<Systematics::GroupPtr> >::iterator git = gen_map.find(cladeID);
      if (oit == org_map.end()) {
        //The clade is new
        org_map[cladeID].Resize(1); org_map[cladeID][0] = organism;
        gen_map[cladeID].Resize(1); gen_map[cladeID][0] = genotype;
      } else {
        //The clade is known
        oit->second.Push(organism);
        git->second.Push(genotype);
      }
    }
    
    //Create and print the histograms; this calls a static method in another action
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    ofstream& fp = df->OFStream();
    if (!fp.is_open()) {
      ctx.Driver().Feedback().Error("PrintCCladeFitnessHistogram: Unable to open output file.");
      ctx.Driver().Abort(Avida::IO_ERROR);
    }
    map< int, Apto::Array<cOrganism*> >::iterator oit = org_map.begin();
    map< int, Apto::Array<Systematics::GroupPtr> >::iterator git = gen_map.begin();
    for (; oit != org_map.end(); oit++, git++) {
      Apto::Array<int> hist =
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
  cActionPrintCCladeRelativeFitnessHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
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
    if (ctx.GetAnalyzeMode()) {
      ctx.Driver().Feedback().Error("PrintCCladeRelativeFitnessHistogram requires avida to be in run mode.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    if (m_world->GetConfig().TRACK_CCLADES.Get() == 0) {
      ctx.Driver().Feedback().Error("PrintCCladeRelativeFitnessHistogram requires coalescence clade tracking to be enabled.");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    //Verify input parameters
    if ( (m_mode != "ACTUAL" && m_mode != "CURRENT" && m_mode != "TESTCPU") || m_hist_fmin > m_hist_fmax) {
      ctx.Driver().Feedback().Error("PrintCCladeRelativeFitness: check parameters");
      ctx.Driver().Abort(Avida::INVALID_CONFIG);
    }
    
    ///Gather data objects
    cPopulation& pop        = m_world->GetPopulation();
    const int    update     = m_world->GetStats().GetUpdate();
    map< int, Apto::Array<cOrganism*> > org_map;  //Map of ccladeID to array of organism IDs
    map< int, Apto::Array<Systematics::GroupPtr> > gen_map;  //Map of ccladeID to array of genotype IDs
    
    //Collect clade information
    for (int i = 0; i < pop.GetSize(); i++) {
      if (pop.GetCell(i).IsOccupied() == false) continue;  //Skip unoccupied cells
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      Systematics::GroupPtr genotype = organism->SystematicsGroup("genotype");
      int cladeID = organism->GetCCladeLabel();
      
      map< int, Apto::Array<cOrganism*> >::iterator oit = org_map.find(cladeID);
      map< int, Apto::Array<Systematics::GroupPtr> >::iterator git = gen_map.find(cladeID);
      if (oit == org_map.end()) {
        // The clade is new
        org_map[cladeID].Resize(1); org_map[cladeID][0] = organism;
        gen_map[cladeID].Resize(1); gen_map[cladeID][0] = genotype;
      } else {
        // The clade is known
        oit->second.Push(organism);
        git->second.Push(genotype);
      }
    }
    
    //Create and print the histograms; this calls a static method in another action
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    ofstream& fp = df->OFStream();
    if (!fp.is_open()) {
      ctx.Driver().Feedback().Error("PrintCCladeRelativeFitnessHistogram: Unable to open output file.");
      ctx.Driver().Abort(Avida::IO_ERROR);      
    }
    map< int, Apto::Array<cOrganism*> >::iterator oit = org_map.begin();
    map< int, Apto::Array<Systematics::GroupPtr> >::iterator git = gen_map.begin();
    for (; oit != org_map.end(); oit++, git++) {
      Apto::Array<int> hist = cActionPrintRelativeFitnessHistogram::MakeHistogram( (oit->second), (git->second),
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
  cActionPrintGenomicSiteEntropy(cWorld* world, const cString& args, Feedback&) : cAction(world, args){
    cString largs = args;
    m_filename = (largs.GetSize()) ? largs.PopWord() : "GenomicSiteEntropy.dat";
  }
  
  static const cString GetDescription() { return "Arguments: [filename = \"GenomicSiteEntropyData.dat\"]";}
  
  void Process(cAvidaContext& ctx)
  {
    const int        num_insts  = m_world->GetHardwareManager().GetDefaultInstSet().GetSize();
    Apto::Array<cString> aligned;  //This will hold all of our aligned sequences
    
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
        ConstInstructionSequencePtr seq;
        seq.DynamicCastFrom(pop.GetCell(i).GetOrganism()->GetGenome().Representation());
        aligned.Push((const char*)seq->AsString());
      }
      AlignStringArray(aligned);  //Align our population genomes
    }
    
    //With all sequences aligned and stored, we can proceed to calculate per-site entropies
    if (!aligned.GetSize())
    {
      ctx.Driver().Feedback().Notify("cActionPrintGenomicSiteEntropy: No sequences available.  Abort.");
      return;
    }
    
    const int gen_size = aligned[0].GetSize();
    Apto::Array<double> site_entropy(gen_size);
    site_entropy.SetAll(0.0);
    
    Apto::Array<int> inst_count( (m_use_gap) ? num_insts + 1 : num_insts);  //Add an extra place if we're using gaps
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
        else inst_count[ Instruction(ch).GetOp() ]++;   //Update true instruction count
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
  void AlignStringArray(Apto::Array<cString>& unaligned)  //Taken from cAnalyze::CommandAnalyze
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
  
  void PrintPPG(ofstream& fot, Apto::SmartPtr<cPhenPlastGenotype> ppgen, int id, const cString& pid)
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
      Apto::Array<int> tasks = pp->GetLastTaskCount();
      for (int t = 0; t < tasks.GetSize(); t++)
        fot << tasks[t] << " ";
      Apto::Array<int> env_inputs = pp->GetEnvInputs();
      for (int e = 0; e < env_inputs.GetSize(); e++)
        fot << env_inputs[e] << " ";
      fot << endl;
      
    }
  }
  
public:
  cActionPrintPhenotypicPlasticity(cWorld* world, const cString& args, Feedback&)
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
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)this_path);
      ofstream& fot = df->OFStream();
      PrintHeader(fot);
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while((genotype = batch_it.Next())){
        Apto::SmartPtr<cPhenPlastGenotype> ppgen(new cPhenPlastGenotype(genotype->GetGenome(), m_num_trials, test_info, m_world, ctx));
        PrintPPG(fot, ppgen, genotype->GetID(), genotype->GetParents());
      }
    } else{  // Run mode
      cString this_path = m_filename + "-" + cStringUtil::Convert(m_world->GetStats().GetUpdate()) + ".dat";
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
      ofstream& fot = df->OFStream();
      PrintHeader(fot);
      
      Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
      Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
      while (it->Next()) {
        Systematics::GroupPtr bg = it->Get();
        Apto::SmartPtr<cPhenPlastGenotype> ppgen(new cPhenPlastGenotype(Genome(bg->Properties().Get("genome")), m_num_trials, test_info, m_world, ctx));
        PrintPPG(fot, ppgen, bg->ID(), (const char*)bg->Properties().Get("parents").StringValue());
      }
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
  cActionPrintTaskProbHistogram(cWorld* world, const cString& args, Feedback&)
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
    Avida::Output::FilePtr df;
    if (ctx.GetAnalyzeMode()) df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    else df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    ofstream& fot = df->OFStream();  //Setup output file
    if (m_first_run == true){
      PrintHeader(fot);
      m_first_run = false;
    }
    
    //Select runtime mode
    if (ctx.GetAnalyzeMode()){  // A N A L Y Z E    M O D E
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while((genotype = batch_it.Next())){                               //For each genotype
        Apto::Array<double> task_prob = genotype->GetTaskProbabilities();     //    get the taks probabilities
        int weight = (m_weighted) ? genotype->GetNumCPUs() : 1;          //    get the proper tally weighting
        for (int k = 0; k < task_prob.GetSize(); k++){                   //    For each task
          int bin_id = (task_prob[k] < 1.0) ? (int) ceil( ( task_prob[k] * 100 ) / 5 ) : 21;  // find the bin to put it into
          m_bins(k,bin_id) += weight;                                                   //   ... and tally it
        }
      }
    }
    else {  // E X P E R I M E N T    M O D E  (See above for explination)
      Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
      Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
      while (it->Next()) {
        Systematics::GroupPtr bg = it->Get();
        
        int weight = (m_weighted) ? bg->NumUnits() : 1;
        Apto::Array<double> task_prob = cPhenPlastUtil::GetTaskProbabilities(ctx, m_world, bg);
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
  
  inline bool HasPlasticTasks(Apto::Array<double> task_probs){
    for (int k = 0; k < task_probs.GetSize(); k++)
      if (task_probs[k] != 0 && task_probs[k] != 1) return true;
    return false;
  }
  
public:
  cActionPrintPlasticGenotypeSummary(cWorld* world, const cString& args, Feedback&)
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
    Avida::Output::FilePtr df;
    if (ctx.GetAnalyzeMode()) df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    else df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    ofstream& fot = df->OFStream();
    if (m_first_run == true){
      PrintHeader(fot);
      m_first_run = false;
    }
    double median = -1.0;           // Will hold the median phenotypic value (excluding 0.0)
    double task_median = -1.0;      // Will hold the median phenotypic entropy value of only those genotypes showing task plasticity
    Apto::Array<double> pp_entropy;      // Will hold the phenotypic entropy values greater than 0.0
    Apto::Array<double> pp_taskentropy;  // Will hold phenotypic entropy values for only those organisms with task plasticity
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
      Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
      Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
      pp_entropy.ResizeClear(num_genotypes);
      pp_taskentropy.ResizeClear(num_genotypes);
      while (it->Next()) {
        Systematics::GroupPtr bg = it->Get();
        int num = bg->NumUnits();
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
      Apto::QSort(pp_entropy, 0, num_plast_genotypes-1);
      int ndx    = num_plast_genotypes / 2;
      median     = (num_plast_genotypes % 2 == 1) ? pp_entropy[ndx] : (pp_entropy[ndx-1] + pp_entropy[ndx]) / 2.0;
      if (gen_task_plast > 0){      //Handle our second array of entropies if we need to
        Apto::QSort(pp_taskentropy, 0, gen_task_plast-1);
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
  }
  
};


/*
 This function goes through all genotypes currently present in the soup,
 and writes into an output file the average Hamming distance between the
 creatures in the population and a given reference genome.
 
 Parameters
 ref_creature_file (cString)
 Filename for the reference genome
 fname (cString)
 Name of file to create, defaults to 'genetic_distance.dat'
 */
class cActionPrintGeneticDistanceData : public cAction
{
private:
  Genome m_reference;
  InstructionSequencePtr m_r_seq;
  cString m_filename;
  
public:
  cActionPrintGeneticDistanceData(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args), m_filename("genetic_distance.dat")
  {
    cString creature_file;
    cString largs(args);
    
    // Load the genome of the reference creature
    creature_file = largs.PopWord();
    GenomePtr genome(Util::LoadGenomeDetailFile(creature_file, m_world->GetWorkingDir(), world->GetHardwareManager(), feedback));
    m_reference = *genome;
    m_r_seq.DynamicCastFrom(m_reference.Representation());
    
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <string ref_creature_file> [string fname='genetic_distance.dat']"; }
  
  void Process(cAvidaContext&)
  {
    double hamming_m1 = 0;
    double hamming_m2 = 0;
    int count = 0;
    int dom_dist = 0;
    
    // get the info for the dominant genotype
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    it->Next();
    Genome best_genome(it->Get()->Properties().Get("genome"));
    InstructionSequencePtr best_seq;
    best_seq.DynamicCastFrom(best_genome.Representation());
    dom_dist = InstructionSequence::FindHammingDistance(*m_r_seq, *best_seq);
    hamming_m1 += dom_dist;
    hamming_m2 += dom_dist*dom_dist;
    count += it->Get()->NumUnits();
    // now cycle over the remaining genotypes
    while ((it->Next())) {
      Genome cur_gen(it->Get()->Properties().Get("genome"));
      InstructionSequencePtr cur_seq;
      cur_seq.DynamicCastFrom(cur_gen.Representation());
      int dist = InstructionSequence::FindHammingDistance(*m_r_seq, *cur_seq);
      hamming_m1 += dist;
      hamming_m2 += dist*dist;
      count += it->Get()->NumUnits();
    }
    
    hamming_m1 /= static_cast<double>(count);
    hamming_m2 /= static_cast<double>(count);
    
    double hamming_best = InstructionSequence::FindHammingDistance(*m_r_seq, *best_seq);
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(hamming_m1, "Average Hamming Distance");
    df->Write(sqrt((hamming_m2 - hamming_m1*hamming_m1) / static_cast<double>(count)), "Standard Error");
    df->Write(hamming_best, "Best Genotype Hamming Distance");
    df->Endl();
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
  cActionPrintPopulationDistanceData(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_filename(""), m_save_genotypes(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_creature = largs.PopWord();
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_save_genotypes = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <string creature> [string fname=\"\"] [int save_genotypes=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("pop_distance-%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    double sum_fitness = 0;
    int sum_num_organisms = 0;
    
    // load the reference genome
    GenomePtr reference_genome;
    cUserFeedback feedback;
    reference_genome = Util::LoadGenomeDetailFile(m_creature, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback);
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
        case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }
    if (!reference_genome) return;
    
    InstructionSequencePtr r_seq;
    r_seq.DynamicCastFrom(reference_genome->Representation());
    
    // cycle over all genotypes
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    while ((it->Next())) {
      Systematics::GroupPtr bg = it->Get();
      const Genome genome(bg->Properties().Get("genome"));
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(genome.Representation());
      const int num_orgs = bg->NumUnits();
      
      // now output
      
      sum_fitness += (double)Apto::StrAs(bg->Properties().Get("fitness")) * num_orgs;
      sum_num_organisms += num_orgs;
      
      df->Write(bg->Properties().Get("name").StringValue(), "Genotype Name");
      df->Write((double)Apto::StrAs(bg->Properties().Get("fitness")), "Fitness");
      df->Write(num_orgs, "Abundance");
      df->Write(InstructionSequence::FindHammingDistance(*r_seq, *seq), "Hamming distance to reference");
      df->Write(InstructionSequence::FindEditDistance(*r_seq, *seq), "Levenstein distance to reference");
      df->Write(genome.AsString(), "Genome");
      
      // save into archive
      if (m_save_genotypes) {
        cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
        testcpu->PrintGenome(ctx, genome, cStringUtil::Stringf("archive/%s.org", (const char*)(bg->Properties().Get("name").StringValue())));
        delete testcpu;
      }
      
      df->Endl();
    }
    df->WriteRaw(cStringUtil::Stringf("# ave fitness from Test CPU's: %d\n", sum_fitness / sum_num_organisms));
    
  }
};


class cActionTestDominant : public cAction
{
private:
  cString m_filename;
  
public:
  cActionTestDominant(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("dom-test.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='dom-test.dat']"; }
  void Process(cAvidaContext& ctx)
  {
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    Systematics::GroupPtr bg = it->Next();
    Genome genome(bg->Properties().Get("genome"));
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(genome.Representation());
    
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
    cCPUTestInfo test_info;
    testcpu->TestGenome(ctx, test_info, genome);
    delete testcpu;
    
    cPhenotype& colony_phenotype = test_info.GetColonyOrganism()->GetPhenotype();
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(colony_phenotype.GetMerit().GetDouble(), "Merit");
    df->Write(colony_phenotype.GetGestationTime(), "Gestation Time");
    df->Write(colony_phenotype.GetFitness(), "Fitness");
    df->Write(1.0 / (0.1 + colony_phenotype.GetGestationTime()), "Reproduction Rate");
    df->Write(seq->GetSize(), "Genome Length");
    df->Write(colony_phenotype.GetCopiedSize(), "Copied Size");
    df->Write(colony_phenotype.GetExecutedSize(), "Executed Size");
    df->Endl();
  }
};


class cActionPrintTaskSnapshot : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintTaskSnapshot(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("tasks_%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    cPopulation& pop = m_world->GetPopulation();
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
    
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
      
      df->Write(i, "Cell Number");
      df->Write(sum_tasks_rewarded, "Number of Tasks Rewarded");
      df->Write(sum_tasks_all, "Total Number of Tasks Done");
      df->Write(divide_sum_tasks_rewarded, "Number of Rewarded Tasks on Divide");
      df->Write(divide_sum_tasks_all, "Number of Total Tasks on Divide");
      df->Write(parent_sum_tasks_rewarded, "Parent Number of Tasks Rewared");
      df->Write(parent_sum_tasks_all, "Parent Total Number of Tasks Done");
      df->Write(test_info.GetColonyFitness(), "Genotype Fitness");
      df->Write(organism->SystematicsGroup("genotype")->ID(), "Genotype ID");
      df->Endl();
    }
    
    delete testcpu;
  }
};

class cActionPrintAveNumTasks : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintAveNumTasks(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("ave_num_tasks.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
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
    
    df->WriteComment("Avida num tasks data");
    df->WriteTimeStamp();
    df->WriteComment("First column gives the current update, 2nd column gives the average number of tasks performed");
    df->WriteComment("by each organism in the current population that performs at least one task ");
    
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(pop_ave, "Ave num tasks done by single org that is doing at least one task");
    df->Endl();
  }
};


class cActionPrintViableTasksData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintViableTasksData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("viable_tasks.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='viable_tasks.dat']"; }
  void Process(cAvidaContext& ctx)
  {
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    cPopulation& pop = m_world->GetPopulation();
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    Apto::Array<int> tasks(num_tasks);
    tasks.SetAll(0);
    
    for (int i = 0; i < pop.GetSize(); i++) {
      if (!pop.GetCell(i).IsOccupied()) continue;
      if (pop.GetCell(i).GetOrganism()->GetTestFitness(ctx) > 0.0) {
        cPhenotype& phenotype = pop.GetCell(i).GetOrganism()->GetPhenotype();
        for (int j = 0; j < num_tasks; j++) if (phenotype.GetCurTaskCount()[j] > 0) tasks[j]++;
      }
    }
    
    df->WriteComment("Avida viable tasks data");
    df->WriteTimeStamp();
    df->WriteComment("First column gives the current update, next columns give the number");
    df->WriteComment("of organisms that have the particular task as a component of their merit");
    
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    for(int i = 0; i < tasks.GetSize(); i++) {
      df->WriteAnonymous(tasks[i]);
    }
    df->Endl();
  }
};



class cActionCalcConsensus : public cAction
{
private:
  int m_lines_saved;
  cString m_inst_set;
  
public:
  cActionCalcConsensus(cWorld* world, const cString& args, Feedback&)
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
    HashPropertyMap mg_props;
    cHardwareManager::SetupPropertyMap(mg_props, (const char*)m_inst_set);
    Genome mg(m_world->GetHardwareManager().GetInstSet((const char*)m_inst_set).GetHardwareType(), mg_props, InstructionSequencePtr(new InstructionSequence));
    const int num_inst = m_world->GetHardwareManager().GetInstSet((const char*)m_inst_set).GetSize();
    const int update = m_world->GetStats().GetUpdate();
    
    // Setup the histogtams...
    Apto::Array<cHistogram> inst_hist(MAX_GENOME_LENGTH);
    for (int i = 0; i < MAX_GENOME_LENGTH; i++) inst_hist[i].Resize(num_inst,-1);
    
    // Loop through all of the genotypes adding them to the histograms.
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    while ((it->Next())) {
      Systematics::GroupPtr bg = it->Get();
      const int num_organisms = bg->NumUnits();
      const Genome genome(bg->Properties().Get("genome"));
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(genome.Representation());
      const int length = seq->GetSize();
      if (Apto::StrAs(genome.Properties().Get("instset")) != m_inst_set) continue;
      
      // Place this genotype into the histograms.
      for (int j = 0; j < length; j++) {
        assert((*seq)[j].GetOp() < num_inst);
        inst_hist[j].Insert((*seq)[j].GetOp(), num_organisms);
      }
      
      // Mark all instructions beyond the length as -1 in histogram...
      for (int j = length; j < MAX_GENOME_LENGTH; j++) {
        inst_hist[j].Insert(-1, num_organisms);
      }
    }
    
    // Now, lets print something!
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "consensus.dat");
    Avida::Output::FilePtr df_abundance = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "consensus-abundance.dat");
    Avida::Output::FilePtr df_var = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "consensus-var.dat");
    Avida::Output::FilePtr df_entropy = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "consensus-entropy.dat");
    
    // Determine the length of the concensus genome
    int con_length;
    for (con_length = 0; con_length < MAX_GENOME_LENGTH; con_length++) {
      if (inst_hist[con_length].GetMode() == -1) break;
    }
    
    // Build the concensus genotype...
    InstructionSequencePtr con_genome_p;
    con_genome_p.DynamicCastFrom(mg.Representation());
    InstructionSequence& con_genome = *con_genome_p;
    con_genome = InstructionSequence(con_length);
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
        df_abundance->WriteAnonymous(count);
        df_var->WriteAnonymous(inst_hist[i].GetCountVariance());
        df_entropy->WriteAnonymous(entropy);
      }
    }
    
    // Put end-of-lines on the files.
    if (m_lines_saved > 0) {
      df_abundance->Endl();
      df_var->Endl();
      df_entropy->Endl();
    }
    
    // --- Study the consensus genome ---
    
    // Loop through genotypes again, and determine the average genetic distance.
    it = classmgr->ArbiterForRole("genotype")->Begin();
    cDoubleSum distance_sum;
    while ((it->Next())) {
      const int num_organisms = it->Get()->NumUnits();
      Genome cur_gen(it->Get()->Properties().Get("genome"));
      InstructionSequencePtr cur_seq;
      cur_seq.DynamicCastFrom(cur_gen.Representation());
      const int cur_dist = InstructionSequence::FindEditDistance(con_genome, *cur_seq);
      distance_sum.Add(cur_dist, num_organisms);
    }
    
    // Finally, gather last bits of data and print the results.
    // @TODO - find consensus bio group
    //    cGenotype* con_genotype = classmgr.FindGenotype(con_genome, -1);
    
    it = classmgr->ArbiterForRole("genotype")->Begin();
    Genome best_genome(it->Next()->Properties().Get("genome"));
    InstructionSequencePtr best_seq;
    best_seq.DynamicCastFrom(best_genome.Representation());
    const int best_dist = InstructionSequence::FindEditDistance(con_genome, *best_seq);
    
    const double ave_dist = distance_sum.Average();
    const double var_dist = distance_sum.Variance();
    const double complexity_base = static_cast<double>(con_genome.GetSize()) - total_entropy;
    
    cString con_name;
    con_name.Set("archive/%03d-consensus-u%i.gen", con_genome.GetSize(),update);
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
    testcpu->PrintGenome(ctx, mg, con_name);
    
    
    cCPUTestInfo test_info;
    testcpu->TestGenome(ctx, test_info, mg);
    delete testcpu;
    
    cPhenotype& colony_phenotype = test_info.GetColonyOrganism()->GetPhenotype();
    
    df->Write(update, "Update");
    df->Write(colony_phenotype.GetMerit().GetDouble(), "Merit");
    df->Write(colony_phenotype.GetGestationTime(), "Gestation Time");
    df->Write(colony_phenotype.GetFitness(), "Fitness");
    df->Write(1.0 / (0.1  + colony_phenotype.GetGestationTime()), "Reproduction Rate");
    df->Write(con_genome.GetSize(), "Length");
    df->Write(colony_phenotype.GetCopiedSize(), "Copied Size");
    df->Write(colony_phenotype.GetExecutedSize(), "Executed Size");
    df->Write(0, "Get Births");
    df->Write(0, "Breed True");
    df->Write(0, "Breed In");
    df->Write(0, "Abundance");
    df->Write(-1, "Tree Depth");
    df->Write(-1, "Genotype ID");
    df->Write(0, "Age (in updates)");
    df->Write(best_dist, "Best Distance");
    df->Write(ave_dist, "Average Distance");
    df->Write(var_dist, "Var Distance");
    df->Write(total_entropy, "Total Entropy");
    df->Write(complexity_base, "Complexity");
    df->Endl();
    //    }
    
    delete testcpu;
  }
};

/*! Calculate and print average edit distance of organisms within and among demes.
 
 Parameters:
 sample_size: Number of organism pairs to sample
 filename: File in which to output stats
 */
class cActionPrintEditDistance : public cAction {
public:
  cActionPrintEditDistance(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args)
  , m_sample_size(-1)
  , m_filename("edit_distance.dat") {
    cString largs(args);
    if(largs.GetSize()) { m_sample_size = static_cast<unsigned int>(largs.PopWord().AsInt()); }
    if(largs.GetSize()) {	m_filename = largs.PopWord();	}
  }
  
  static const cString GetDescription() { return "Arguments: [sample size [filename]]"; }
  
  /*! Calculate our various edit distances.
   
   We have two different measures that we want to look at:
   1) population-wide genetic variance
   2) within-deme genetic variance	 
   */
  void Process(cAvidaContext& ctx) {
    assert(m_world->GetPopulation().GetNumDemes() > 0);
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    // within deme edit distance:
    cDoubleSum within_deme_ed;
    std::vector<cOrganism*> organisms;
    organisms.reserve(m_world->GetPopulation().GetNumOrganisms());
    
    for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
      cDeme& deme = m_world->GetPopulation().GetDeme(i);
      for(int j=0; j<deme.GetSize(); ++j) {
        cOrganism* org = deme.GetOrganism(j);
        if(org != 0) {
          organisms.push_back(org);
        }
      }
      within_deme_ed.Add(average_edit_distance(organisms, ctx));
      organisms.clear();
    }
    
    // among deme edit distance:
    for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
      cOrganism* org = m_world->GetPopulation().GetCell(i).GetOrganism();
      if(org != 0) {
        organisms.push_back(org);
      }
    }
    double among_deme_ed = average_edit_distance(organisms, ctx);		
    
    df->Write(m_world->GetStats().GetUpdate(), "Update [update]");
    df->Write(within_deme_ed.Average(), "Mean deme edit distance [deme]");
    df->Write(among_deme_ed, "Mean population edit distance [population]");
    df->Endl();
  }
  
protected:
  //! Calculate the average edit distance of the given container of organisms.
  double average_edit_distance(std::vector<cOrganism*> organisms, cAvidaContext& ctx) {
    std::random_shuffle(organisms.begin(), organisms.end(), ctx.GetRandom());
    if(organisms.size() % 2) {
      organisms.pop_back();
    }
    
    unsigned int sample_pairs = organisms.size()/2;
    if(m_sample_size > 0) {
      sample_pairs = std::min(m_sample_size, sample_pairs);
    }
    
    cDoubleSum edit_distance;
    for(unsigned int i=0; i<sample_pairs; ++i) {
      cOrganism* a = organisms.back();
      organisms.pop_back();
      cOrganism* b = organisms.back();
      organisms.pop_back();
      
      ConstInstructionSequencePtr a_seq, b_seq;
      a_seq.DynamicCastFrom(a->GetGenome().Representation());
      b_seq.DynamicCastFrom(b->GetGenome().Representation());
      edit_distance.Add(InstructionSequence::FindEditDistance(*a_seq, *b_seq));
    }
    
    return edit_distance.Average();
  }
  
private:
  unsigned int m_sample_size; //!< Number of pairs of organisms to sample for diversity calculation.
  cString m_filename; //!< Filename in which to write the various edit distances.
};


class cActionDumpEnergyGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpEnergyGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_energy.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_energy = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().GetStoredEnergy() : 0.0;
        fp << cell_energy << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpExecutionRatioGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpExecutionRatioGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_exe_ratio.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_executionRatio = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().GetEnergyUsageRatio() : 1.0;
        fp << cell_executionRatio << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpCellDataGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpCellDataGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_cell_data.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_data = cell.GetCellData();
        fp << cell_data << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpFitnessGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpFitnessGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_fitness-%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        double fitness = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().GetFitness() : 0.0;
        fp << fitness << " ";
      }
      fp << endl;
    }
  }
};


class cActionDumpClassificationIDGrid : public cAction
{
private:
  cString m_filename;
  cString m_role;
  
public:
  cActionDumpClassificationIDGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename(""), m_role("genotype")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_role = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname_prefix='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename = "grid_class_id";
    filename.Set("%s-%d.dat", (const char*)filename, m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int id = (cell.IsOccupied() && cell.GetOrganism()->SystematicsGroup((const char*)m_role)) ? cell.GetOrganism()->SystematicsGroup((const char*)m_role)->ID() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpGenotypeColorGrid : public cAction
{
private:
  int m_num_colors;
  int m_threshold;
  cString m_filename;
  Apto::Array<int> m_genotype_chart;
  
public:
  cActionDumpGenotypeColorGrid(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_num_colors(12), m_threshold(10), m_filename(""), m_genotype_chart(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_num_colors = largs.PopWord().AsInt();
    if (largs.GetSize()) m_threshold = largs.PopWord().AsInt();
    if (largs.GetSize()) m_filename = largs.PopWord();
    
    m_genotype_chart.Resize(m_num_colors, 0);
  }
  
  static const cString GetDescription() { return "Arguments: [int num_colors=12] [string fname='']"; }
  
  void Process(cAvidaContext&)
  {
    // Update current entries in the color chart
    for (int i = 0; i < m_num_colors; i++) {
      if (m_genotype_chart[i] && FindPos(m_genotype_chart[i]) < 0) m_genotype_chart[i] = 0;
    }
    
    // Add new entries where possible
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    for (int i = 0; (it->Next()) && i < m_threshold; i++) {
      if (!isInChart(it->Get()->ID())) {
        // Add to the genotype chart
        for (int j = 0; j < m_num_colors; j++) {
          if (m_genotype_chart[j] == 0) {
            m_genotype_chart[j] = it->Get()->ID();
            break;
          }
        }
      }
    }
    
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_genotype_color-%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        Systematics::GroupPtr bg = (cell.IsOccupied()) ? cell.GetOrganism()->SystematicsGroup("genotype") : Systematics::GroupPtr(NULL);
        if (bg) {
          int color = 0;
          for (; color < m_num_colors; color++) if (m_genotype_chart[color] == bg->ID()) break;
          if (color == m_num_colors && (bool)Apto::StrAs(bg->Properties().Get("threshold"))) color++;
          fp << color << " ";
        } else {
          fp << "-1 ";
        }
      }
      fp << endl;
    }
  }
  
private:
  int FindPos(int gid)
  {
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    int i = 0;
    while ((it->Next()) && i < m_num_colors) {
      if (gid == it->Get()->ID()) return i;
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
  cActionDumpPhenotypeIDGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_phenotype_id.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int id = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().CalcID() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpIDGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpIDGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("id_grid.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int id = (cell.IsOccupied()) ? cell.GetOrganism()->GetID() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpVitalityGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpVitalityGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_dumps/vitality_grid.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        double id = (cell.IsOccupied()) ? cell.GetOrganism()->GetVitality() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpTargetGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpTargetGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    const int worldx = m_world->GetPopulation().GetWorldX();
    cString filename(m_filename);
    
    if (m_world->GetConfig().USE_AVATARS.Get()) {
      if (filename == "") filename.Set("grid_dumps/avatar_grid.%d.dat", m_world->GetStats().GetUpdate());
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
      ofstream& fp = df->OFStream();
      
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        for (int i = 0; i < worldx; i++) {
          cPopulationCell& cell = m_world->GetPopulation().GetCell(j * worldx + i);
          int target = -99;
          if (cell.HasAV()) {
            if (cell.HasPredAV()) target = cell.GetRandPredAV()->GetForageTarget();
            else target = cell.GetRandPreyAV()->GetForageTarget();
          } 
          fp << target << " ";
        }
        fp << endl;
      }
    }    
    
    else {
      if (filename == "") filename.Set("grid_dumps/target_grid.%d.dat", m_world->GetStats().GetUpdate());
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
      ofstream& fp = df->OFStream();
      
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        for (int i = 0; i < worldx; i++) {
          cPopulationCell& cell = m_world->GetPopulation().GetCell(j * worldx + i);
          int target = -99;
          if (cell.IsOccupied()) target = cell.GetOrganism()->GetForageTarget();
          fp << target << " ";
        }
        fp << endl;
      }
    }
  }
};

//DumpMaxResGrid intended for creating single output file of spatial resources, recording the max value (of any resource) when resources overlap
class cActionDumpMaxResGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpMaxResGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_dumps/max_res_grid.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        const Apto::Array<double> res_count = m_world->GetPopulation().GetCellResources(j * m_world->GetPopulation().GetWorldX() + i, ctx);
        double max_resource = 0.0;    
        // get the resource library
        const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
        // if more than one resource is available, return the resource with the most available in this spot 
        // (note that, with global resources, the GLOBAL total will evaluated)
        // we build regular resources on top of any hills, but replace any regular resources or hills with any walls or dens 
        double topo_height = 0.0;
        for (int h = 0; h < res_count.GetSize(); h++) {
          int hab_type = resource_lib.GetResource(h)->GetHabitat();
          if ((res_count[h] > max_resource) && (hab_type != 1) && (hab_type !=2)) max_resource = res_count[h];
          else if ((hab_type == 1 || hab_type == 4 || hab_type == 5) && res_count[h] > 0) topo_height = resource_lib.GetResource(h)->GetPlateau();
          // allow walls to trump everything else
          else if (hab_type == 2 && res_count[h] > 0) { 
            topo_height = resource_lib.GetResource(h)->GetPlateau();
            max_resource = 0.0;
            break;
          }
        }
        max_resource = max_resource + topo_height;
        fp << max_resource << " ";
      }
      fp << endl;
    }
  }
};


class cActionDumpSleepGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpSleepGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_sleep.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int i = 0; i < m_world->GetPopulation().GetWorldY(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldX(); j++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(i * m_world->GetPopulation().GetWorldX() + j);
        double cell_energy = (cell.IsOccupied()) ? cell.GetOrganism()->IsSleeping() : 0.0;
        fp << cell_energy << " ";
      }
      fp << endl;
    }
  }
};


class cActionDumpGenomeLengthGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpGenomeLengthGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_genome_length.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        int genome_length = 0;
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true)
        {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          ConstInstructionSequencePtr seq;
          seq.DynamicCastFrom(organism->GetGenome().Representation());
          genome_length = seq->GetSize();
        }
        else { genome_length = -1; }
        fp << genome_length << " ";
      }
      fp << endl;
    }
  }
};


class cActionDumpTaskGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpTaskGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_task.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    for (int i = 0; i < pop->GetWorldY(); i++) {
      for (int j = 0; j < pop->GetWorldX(); j++) {
        int task_sum = -1;
        int cell_num = i * pop->GetWorldX() + j;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          task_sum = 0;
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
  }
};



/* Dumps the task grid of the last task performed by each organism. */
class cActionDumpLastTaskGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpLastTaskGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_last_task.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    int task_id;      
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          task_id = organism->GetPhenotype().GetLastTaskID();
        } else {
          task_id = -1;
        }
        fp << m_world->GetStats().GetUpdate() << " " << cell_num << " "  << task_id << endl;
      }
    }
  }
};


//Dump the reaction grid from the last gestation cycle, so skip the 
//test cpu, and just use what the phenotype has. 
//LZ - dump task grid for only the hosts, skip the test cpu and just output the last
//gestation cycle tasks.
class cActionDumpHostTaskGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpHostTaskGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_task_hosts.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        int task_sum = 0;
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          cPhenotype& test_phenotype = organism->GetPhenotype();
          
          for (int k = 0; k < num_tasks; k++) {
            if (test_phenotype.GetLastHostTaskCount()[k] > 0) task_sum += static_cast<int>(pow(2.0, k));
          }
        }
        else { task_sum = -1; }
        fp << task_sum << " ";
      }
      fp << endl;
    }
  }
};


//LZ - dump the parasite tasks, ignoring the test cpu and just output what was in the phenotype for the
//last gestation of the parasite.
class cActionDumpParasiteTaskGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpParasiteTaskGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_task_parasite.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        int task_sum = 0;
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          if(organism->GetNumParasites() > 0)
          {
            cPhenotype& test_phenotype = organism->GetPhenotype();
            
            for (int k = 0; k < num_tasks; k++) {
              if (test_phenotype.GetLastParasiteTaskCount()[k] > 0) task_sum += static_cast<int>(pow(2.0, k));
            }
            
          }
          else { task_sum = -1; }
        }
        else { task_sum = -1; }
        fp << task_sum << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpHostTaskGridComma : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpHostTaskGridComma(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_task_hosts_comma.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    cPopulation* pop = &m_world->GetPopulation();
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        cString task_list = "";
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          cPhenotype& test_phenotype = organism->GetPhenotype();
          
          for (int k = 0; k < num_tasks; k++) {
            cString task_count_string = cStringUtil::Stringf("%d", test_phenotype.GetLastHostTaskCount()[k]);
            task_list += task_count_string;
            if (k != num_tasks -1)
              task_list += ",";            
          }
        }
        else { task_list = "-1"; }
        fp << task_list << " ";
      }
      fp << endl;
    }
  }
};


class cActionDumpParasiteTaskGridComma : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpParasiteTaskGridComma(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_task_parasites_comma.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    cPopulation* pop = &m_world->GetPopulation();
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        cString task_list = "";
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          if(organism->GetNumParasites() > 0)
          {
            cPhenotype& test_phenotype = organism->GetPhenotype();
            
            for (int k = 0; k < num_tasks; k++) {
              cString task_count_string = cStringUtil::Stringf("%d", test_phenotype.GetLastParasiteTaskCount()[k]);
              task_list += task_count_string;
              if (k != num_tasks -1)
                task_list += ",";
            }
          }
          else { task_list = "-1"; }
        }
        else { task_list = "-1"; }
        fp << task_list << " ";
      }
      fp << endl;
    }
  }
};


//LZ - dump the parasite virulence grid
class cActionDumpParasiteVirulenceGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpParasiteVirulenceGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_virulence.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        double virulence = 0;
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true) {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          if(organism->GetNumParasites() > 0)
          {
            Apto::Array<Systematics::UnitPtr> parasites = organism->GetParasites();
            Apto::SmartPtr<cParasite, Apto::InternalRCObject> parasite;
            parasite.DynamicCastFrom(parasites[0]);
            virulence = parasite->GetVirulence();
          }
          else { virulence = -1; }
        }
        else { virulence = -1; }
        fp << virulence << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpOffspringMigrationCounts : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpOffspringMigrationCounts(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("counts_offspring_migration.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    int num_demes = (&m_world->GetPopulation())->GetNumDemes();
    cMigrationMatrix* mig_mat = &m_world->GetMigrationMatrix();
    
    for(int row = 0; row < num_demes; row++){
      for(int col = 0; col < num_demes; col++){
        if((col+1) >= num_demes)
          fp << mig_mat->GetOffspringCountAt(row,col);
        else
          fp << mig_mat->GetOffspringCountAt(row,col) << ",";
      }
      fp << endl;
    }
    mig_mat->ResetOffspringCounts();
  }
};

class cActionDumpParasiteMigrationCounts : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpParasiteMigrationCounts(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("counts_parasite_migration.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    int num_demes = (&m_world->GetPopulation())->GetNumDemes();
    cMigrationMatrix* mig_mat = &m_world->GetMigrationMatrix();
    
    for(int row = 0; row < num_demes; row++){
      for(int col = 0; col < num_demes; col++){
        if((col+1) >= num_demes)
          fp << mig_mat->GetParasiteCountAt(row,col);
        else
          fp << mig_mat->GetParasiteCountAt(row,col) << ",";
      }
      fp << endl;
    }
    mig_mat->ResetParasiteCounts();
  }
};

//Dump the reaction grid from the last gestation cycle, so skip the
//test cpu, and just use what the phenotype has.
class cActionDumpReactionGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpReactionGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_reactions.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    const int num_tasks = m_world->GetEnvironment().GetNumTasks();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        int task_sum = 0;
        int cell_num = j * pop->GetWorldX() + i;
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
  }
};





/*
@MRR
Using the phenotype information, print the number of time each reaction
occured during the last gestation cyle
*/
class cActionPrintLastReactionCountGrid : public cAction
{
private:
  cString m_filename;
  bool first_time;
  
public:
  cActionPrintLastReactionCountGrid(cWorld* world, const cString& args, Feedback&) : 
    cAction(world, args)
    , m_filename("")
    , first_time(true)
  {
    cString largs(args);
    m_filename = (largs.GetSize()) ? largs.PopWord() : "last-reaction-count.dat";
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
  
    if (ctx.GetAnalyzeMode()){
      cerr << "cActionPrintLastReactionCount cannot be run in analyze mode.";
      m_world->GetDriver().Abort(INVALID_CONFIG);
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    std::ofstream& fout = df->OFStream();
    
    if (first_time){
      df->WriteComment("First column is update, second column is reaction name, subsequent columns are individual cells");
      df->WriteComment("-1 for a reaction count indicates the cell is not occupied.");
      df->FlushComments();
      first_time = false;
    }
    const int UNOCCUPIED = -1;
    
    cPopulation& pop = m_world->GetPopulation();
    cReactionLib& rlib = m_world->GetEnvironment().GetReactionLib();
    const int update = m_world->GetStats().GetUpdate();
    for (int react=0; react < rlib.GetSize(); react++){
      fout << update << " " << rlib.GetReaction(react)->GetName();
      for (int cell=0; cell < pop.GetSize(); cell++){  
        fout << " ";
        if (!pop.GetCell(cell).IsOccupied())
          fout << UNOCCUPIED;
        else
          fout << pop.GetCell(cell).GetOrganism()->GetPhenotype().GetLastReactionCount()[react];
      }
      df->Endl();
      df->Flush();
    }
  }
};


/*
@MRR
Using the phenotype information, print the number of time each reaction
occured during the current gestation cycle.  This may be incomplete
if the gestation cycle is still occuring.
*/
class cActionPrintCurrReactionCountGrid : public cAction
{
private:
  cString m_filename;
  bool first_time;
  
public:
  cActionPrintCurrReactionCountGrid(cWorld* world, const cString& args, Feedback&) : 
    cAction(world, args)
    , m_filename("")
    , first_time(true)
  {
    cString largs(args);
    m_filename = (largs.GetSize()) ? largs.PopWord() : "curr-reaction-count.dat";
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
  
    if (ctx.GetAnalyzeMode()){
      cerr << "cActionPrintLastReactionCount cannot be run in analyze mode.";
      m_world->GetDriver().Abort(INVALID_CONFIG);
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    std::ofstream& fout = df->OFStream();
    if (first_time){
      df->WriteComment("First column is update, second column is reaction name, subsequent columns are individual cells");
      df->WriteComment("-1 for a reaction count indicates the cell is not occupied.");
      df->FlushComments();
      first_time = false;
    }
    const int UNOCCUPIED = -1;
    
    cPopulation& pop = m_world->GetPopulation();
    cReactionLib& rlib = m_world->GetEnvironment().GetReactionLib();
    const int update = m_world->GetStats().GetUpdate();
    for (int react=0; react < rlib.GetSize(); react++){
      fout << update << " " << rlib.GetReaction(react)->GetName();
      for (int cell=0; cell < pop.GetSize(); cell++){  
        fout << " ";
        if (!pop.GetCell(cell).IsOccupied())
          fout << UNOCCUPIED;
        else
          fout << pop.GetCell(cell).GetOrganism()->GetPhenotype().GetCurReactionCount()[react];
      }
      df->Endl();
      df->Flush();
    }
  }
};






class cActionDumpGenotypeGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpGenotypeGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_genome.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        cString genome_seq("");
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true)
        {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          ConstInstructionSequencePtr seq;
          seq.DynamicCastFrom(organism->GetGenome().Representation());
          genome_seq = seq->AsString();
        }
        else { genome_seq = "-1"; }
        fp << genome_seq << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpHostGenotypeList : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpHostGenotypeList(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("host_genome_list.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        cString genome_seq("");
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true)
        {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          Genome host_genome(organism->Properties().Get("genome"));
          ConstInstructionSequencePtr seq;
          seq.DynamicCastFrom(host_genome.Representation());
          genome_seq = seq->AsString();
          fp << genome_seq << endl;
        }
      }
    }
  }
};

class cActionDumpParasiteGenotypeList : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpParasiteGenotypeList(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("parasite_genome_list.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        cString genome_seq("");
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true)
        {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          if (organism->GetNumParasites() > 0)
          {
            Apto::Array<Systematics::UnitPtr> parasites = organism->GetParasites();
            Apto::SmartPtr<cParasite, Apto::InternalRCObject> parasite;
            parasite.DynamicCastFrom(parasites[0]);
            genome_seq = parasite->UnitGenome().Representation()->AsString();
            fp << genome_seq << endl;
            
          }
        }
      }
    }
  }
};


class cActionDumpParasiteGenotypeGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpParasiteGenotypeGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_genome_parasite.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    cPopulation* pop = &m_world->GetPopulation();
    
    for (int j = 0; j < pop->GetWorldY(); j++) {
      for (int i = 0; i < pop->GetWorldX(); i++) {
        cString genome_seq("");
        int cell_num = j * pop->GetWorldX() + i;
        if (pop->GetCell(cell_num).IsOccupied() == true)
        {
          cOrganism* organism = pop->GetCell(cell_num).GetOrganism();
          if(organism->GetNumParasites() > 0)
          {
            Apto::Array<Systematics::UnitPtr> parasites = organism->GetParasites();
            Apto::SmartPtr<cParasite, Apto::InternalRCObject> parasite;
            parasite.DynamicCastFrom(parasites[0]);
            genome_seq = parasite->UnitGenome().Representation()->AsString();
          }
          else { genome_seq = "0"; }
        }
        else { genome_seq = "-1"; }
        fp << genome_seq << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpDonorGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpDonorGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_donor.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int donor = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().IsDonorLast() : -1;
        fp << donor << " ";
      }
      fp << endl;
    }
  }
};

class cActionDumpReceiverGrid : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpReceiverGrid(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_receiver.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
      for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(j * m_world->GetPopulation().GetWorldX() + i);
        int recv = (cell.IsOccupied()) ? cell.GetOrganism()->GetPhenotype().IsReceiver() : -1;
        fp << recv << " ";
      }
      fp << endl;
    }
  }
};

class cActionPrintNumDivides : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintNumDivides(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    /*Print num of successful divides for each or alive now. */
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("divides.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    fp << "# org_id,age,num_divides" << endl;
    
    const Apto::Array <cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
    for (int i = 0; i < live_orgs.GetSize(); i++) {  
      cOrganism* org = live_orgs[i];
      const int id = org->GetID();
      const int age = org->GetPhenotype().GetAge();
      const int num_divs = org->GetPhenotype().GetNumDivides();
      
      fp << id << "," << age << "," << num_divs;
      fp << endl;
    }
  }
};

class cActionPrintOrgLocData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintOrgLocData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    /*Print organism locations + other org data (for movies). */
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_dumps/org_loc.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    bool use_av = m_world->GetConfig().USE_AVATARS.Get();
    if (!use_av) fp << "# org_id,org_cellx,org_celly,org_forage_target,org_group_id,org_facing" << endl;
    else fp << "# org_id,org_cellx,org_celly,org_forage_target,org_group_id,org_facing,av_cellx,av_celly,av_facing" << endl;
    
    const int worldx = m_world->GetConfig().WORLD_X.Get();
    
    const Apto::Array <cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
    for (int i = 0; i < live_orgs.GetSize(); i++) {  
      cOrganism* org = live_orgs[i];
      const int id = org->GetID();
      const int loc = org->GetCellID();
      const int locx = loc % worldx;
      const int locy = loc / worldx;
      const int ft = org->GetForageTarget();
      const int faced_dir = org->GetFacedDir();
      int opinion = -1;
      if (org->HasOpinion()) opinion = org->GetOpinion().first;
      
      fp << id << "," << locx << "," << locy << "," << ft << "," <<  opinion << "," <<  faced_dir;
      if (use_av) {
        const int avloc = org->GetOrgInterface().GetAVCellID();
        const int avlocx = avloc % worldx;
        const int avlocy = avloc / worldx;
        const int avfaced_dir = org->GetOrgInterface().GetAVFacing();
        
        fp << "," << avlocx << "," << avlocy << "," << avfaced_dir;
      }
      fp << endl;
    }
  }
};

class cActionPrintPreyFlockingData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintPreyFlockingData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    /*Print data on prey neighborhood */
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();  
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_dumps/prey_flocking.%d.dat", m_world->GetStats().GetUpdate());    
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    bool use_av = m_world->GetConfig().USE_AVATARS.Get();
    if (!use_av) fp << "# org_id,org_cellx,org_celly,num_prey_neighbors,num_prey_this_cell" << endl;
    else fp << "# org_id,org_av_cellx,org_av_celly,num_neighbor_cells_with_prey,num_prey_this_cell" << endl;
    
    const int worldx = m_world->GetConfig().WORLD_X.Get();
    
    Apto::Array<int> neighborhood;
    const Apto::Array <cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
    for (int i = 0; i < live_orgs.GetSize(); i++) {
      cOrganism* org = live_orgs[i];
      if (!org->IsPreyFT()) continue;
      const int id = org->GetID();
      int num_neighbors = 0;
      neighborhood.Resize(0);
      if (!use_av) {
        const int loc = org->GetCellID();
        const int locx = loc % worldx;
        const int locy = loc / worldx;
        org->GetOrgInterface().GetNeighborhoodCellIDs(neighborhood);
        for (int j = 0; j < neighborhood.GetSize(); j++) {
          if (m_world->GetPopulation().GetCell(neighborhood[j]).IsOccupied()) {
            if (m_world->GetPopulation().GetCell(neighborhood[j]).GetOrganism()->IsPreyFT()) {
              num_neighbors++;
            }
          }
        }
        fp << id << "," << locx << "," << locy << "," << num_neighbors << "," << "1";
      }
      else {
        const int avloc = org->GetOrgInterface().GetAVCellID();
        const int num_prey_this_cell = m_world->GetPopulation().GetCell(avloc).GetNumPreyAV();
        const int avlocx = avloc % worldx;
        const int avlocy = avloc / worldx;
        org->GetOrgInterface().GetAVNeighborhoodCellIDs(neighborhood);
        for (int j = 0; j < neighborhood.GetSize(); j++) {
          if (m_world->GetPopulation().GetCell(neighborhood[j]).HasPreyAV()) num_neighbors++;
        }
        fp << id << "," << avlocx << "," << avlocy << "," << num_neighbors << "," << num_prey_this_cell;
      }
      fp << endl;
    }
  }
};

class cActionPrintOrgGuardData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintOrgGuardData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    /*Print organism locations + other org data (for movies). */
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("grid_dumps/org_loc_guard.%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    ofstream& fp = df->OFStream();
    
    bool use_av = m_world->GetConfig().USE_AVATARS.Get();
    if (!use_av) fp << "# org_id,org_cellx,org_celly,org_forage_target,org_group_id,org_facing,is_guard,num_guard_inst,on_den,r_bins_total,time_used,num_deposits,amount_deposited_total" << endl;
    else fp << "# org_id,org_cellx,org_celly,org_forage_target,org_group_id,org_facing,av_cellx,av_celly,av_facing,is_guard,num_guard_inst,on_den,r_bins_total,time_used,num_deposits,amount_deposited_total" << endl;
    
    const int worldx = m_world->GetConfig().WORLD_X.Get();
    
    const Apto::Array<cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
    for (int i = 0; i < live_orgs.GetSize(); i++) {
      cOrganism* org = live_orgs[i];
      const int id = org->GetID();
      const int loc = org->GetCellID();
      const int locx = loc % worldx;
      const int locy = loc / worldx;
      const int ft = org->GetForageTarget();
      const int faced_dir = org->GetFacedDir();
      int opinion = -1;
      if (org->HasOpinion()) opinion = org->GetOpinion().first;
      
      fp << id << "," << locx << "," << locy << "," << ft << "," <<  opinion << "," <<  faced_dir;
      if (use_av) {
        const int avloc = org->GetOrgInterface().GetAVCellID();
        const int avlocx = avloc % worldx;
        const int avlocy = avloc / worldx;
        const int avfaced_dir = org->GetOrgInterface().GetAVFacing();
        
        fp << "," << avlocx << "," << avlocy << "," << avfaced_dir;
      }    
      //Guard data:
      bool is_guard = org->IsGuard();
      fp << "," << is_guard;
      int num_guard_inst = org->GetNumGuard();
      fp << "," << num_guard_inst;
      
      //Find out if the organism is in a den:
      bool on_den = false;
      Apto::Array<double> res_count = m_world->GetPopulation().GetCellResources(loc, ctx);
      if (use_av) res_count = m_world->GetPopulation().GetCellResources(org->GetOrgInterface().GetAVCellID(), ctx);
      const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
      for (int i = 0; i < res_count.GetSize(); i++) {
        int hab_type = resource_lib.GetResource(i)->GetHabitat();
        if ((hab_type == 3 || hab_type == 4) && res_count[i] > 0) on_den = true;
      }
      
      fp << "," << on_den;
      fp << "," << org->GetRBinsTotal();
      fp << "," << org->GetPhenotype().GetTimeUsed();
      //Counter for number of deposits
      fp << "," << org->GetNumDeposits();
      fp << "," << org->GetAmountDeposited();
      fp << endl;
    }
  }
};

class cActionPrintDonationStats : public cAction
{
public:
  cActionPrintDonationStats(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().PrintDonationStats();
  }
};

class cActionPrintDemeAllStats : public cAction
{
public:
  cActionPrintDemeAllStats(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeAllStats(ctx);
  }
};

class cActionPrintDemeFitness : public cAction
{
public:
  cActionPrintDemeFitness(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx) {
    m_world->GetPopulation().PrintDemeFitness();
  }
};

class cActionPrintDemeLifeFitness : public cAction
{
public:
  cActionPrintDemeLifeFitness(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx) {
    m_world->GetPopulation().PrintDemeLifeFitness();
  }
};

class cActionPrintDemeTasks : public cAction
{
public:
  cActionPrintDemeTasks(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx) {
    m_world->GetPopulation().PrintDemeTasks();
  }
};

class cActionPrintDemesTotalAvgEnergy : public cAction {
public:
  cActionPrintDemesTotalAvgEnergy(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx) {
    m_world->GetPopulation().PrintDemeTotalAvgEnergy(ctx); 
  }
};

class cActionPrintDemeEnergySharingStats : public cAction
{
public:
  cActionPrintDemeEnergySharingStats(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().PrintDemeEnergySharingStats();
  }
};

class cActionPrintDemeEnergyDistributionStats : public cAction
{
public:
  cActionPrintDemeEnergyDistributionStats(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeEnergyDistributionStats(ctx); 
  }
};

class cActionPrintDemeDonorStats : public cAction
{
public:
  cActionPrintDemeDonorStats(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().PrintDemeDonor();
  }
};


class cActionPrintDemeSpacialEnergy : public cAction
{
public:
  cActionPrintDemeSpacialEnergy(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().PrintDemeSpatialEnergyData();
  }
};

class cActionPrintDemeSpacialSleep : public cAction
{
public:
  cActionPrintDemeSpacialSleep(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().PrintDemeSpatialSleepData();
  }
};

class cActionPrintDemeResources : public cAction
{
public:
  cActionPrintDemeResources(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeResource(ctx); 
  }
};

class cActionPrintDemeGlobalResources : public cAction
{
public:
  cActionPrintDemeGlobalResources(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemeGlobalResources(ctx); 
  }
};

class cActionSaveDemeFounders : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSaveDemeFounders(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("deme_founders-%d.dat", m_world->GetStats().GetUpdate());
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
    m_world->GetPopulation().DumpDemeFounders(df->OFStream());
  }
};

class cActionSetVerbose : public cAction
{
private:
  cString m_verbose;
  
public:
  cActionSetVerbose(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_verbose("")
  {
    cString largs(args);
    if (largs.GetSize()) m_verbose = largs.PopWord();
    m_verbose.ToUpper();
  }
  static const cString GetDescription() { return "Arguments: [string verbosity='']"; }
  void Process(cAvidaContext&)
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
  cActionPrintNumOrgsInDeme(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_org_count.dat");
    df->WriteComment("Avida deme resource data");
    df->WriteTimeStamp();
    
    cString UpdateStr = cStringUtil::Stringf( "deme_global_resources_%07i = [ ...", m_world->GetStats().GetUpdate());
    df->WriteRaw(UpdateStr);
    
    for (int d = 0; d < m_world->GetPopulation().GetNumDemes(); d++) {
      cDeme& deme = m_world->GetPopulation().GetDeme(d);
      df->WriteBlockElement(d, 0, 2);
      df->WriteBlockElement(deme.GetOrgCount(), 1, 2);
    }
    
    df->WriteRaw("];");
    df->Endl();
  }
};

//@JJB**
class cActionPrintDemesMeritsData : public cAction
{
public:
  cActionPrintDemesMeritsData(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().PrintDemesMeritsData();
  }
};

class cActionPrintDebug : public cAction
{
public:
  cActionPrintDebug(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
  }
};


//@CHC Mating type-related actions
//Prints counts of the number of organisms of each mating type alive in the population
class cActionPrintMatingTypeHistogram : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintMatingTypeHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else m_filename = "mating_type_histogram.dat";
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"mating_type_histogram.dat\"]"; }
  
  void Process(cAvidaContext&)
  {
    int type_counts[3] = {0,0,0};
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->WriteComment("Avida population mating type histogram");
    df->WriteTimeStamp();
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    cPopulation& pop = m_world->GetPopulation();
    for (int cell_num = 0; cell_num < pop.GetSize(); cell_num++) {
      //Count totals of each mating type
      if (pop.GetCell(cell_num).IsOccupied()) {
        type_counts[pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetMatingType()+1]++;
      }
    }
    df->Write(type_counts[0], "Mating type -1 (juvenile)");
    df->Write(type_counts[1], "Mating type 0 (female)");
    df->Write(type_counts[2], "Mating type 1 (male)");
    df->Endl(); 
  }
};

//Prints counts of the number of organisms of each mating type in the birth chamber
class cActionPrintBirthChamberMatingTypeHistogram : public cAction
{
private:
  cString m_filename;
  int m_hw_type;
  
public:
  cActionPrintBirthChamberMatingTypeHistogram(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename(""), m_hw_type(0)
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else m_filename = "birth_chamber_mating_type_histogram.dat";
    if (largs.GetSize()) m_hw_type = largs.PopWord().AsInt();
    else m_hw_type = 0;
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"birth_chamber_mating_type_histogram.dat\"] [int hwtype=0]"; }
  
  void Process(cAvidaContext&)
  {
    int type_counts[3] = {0,0,0};
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->WriteComment("Avida birth chamber mating type histogram");
    df->WriteTimeStamp();
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    
    type_counts[0] = m_world->GetPopulation().GetBirthChamber(m_hw_type).GetWaitingOffspringNumber(-1, m_hw_type);
    type_counts[1] = m_world->GetPopulation().GetBirthChamber(m_hw_type).GetWaitingOffspringNumber(0, m_hw_type);
    type_counts[2] = m_world->GetPopulation().GetBirthChamber(m_hw_type).GetWaitingOffspringNumber(1, m_hw_type);
    
    df->Write(type_counts[0], "Mating type -1 (juvenile)");
    df->Write(type_counts[1], "Mating type 0 (female)");
    df->Write(type_counts[2], "Mating type 1 (male)");
    df->Endl(); 
  }
};

//Prints data about the current mating display phenotypes of the population
class cActionPrintMatingDisplayData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintMatingDisplayData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else m_filename = "mating_display_data.dat";
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"mating_display_data.dat\"]"; }
  
  void Process(cAvidaContext&)
  {
    int display_sums[6] = {0, 0, 0, 0, 0, 0}; //[0-2] = display A values for juvenile/undefined mating type, females, and males
    //[3-5] = display B values for each sex
    int mating_type_sums[3] = {0, 0, 0}; //How many organisms of each mating type are present in the population
    double display_avgs[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    //Loop through the population and tally up the count values
    cPopulation& pop = m_world->GetPopulation();
    for (int cell_num = 0; cell_num < pop.GetSize(); cell_num++) {
      if (pop.GetCell(cell_num).IsOccupied()) {
        mating_type_sums[pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetMatingType()+1]++;
        display_sums[ pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetMatingType()+1 ] += pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetCurMatingDisplayA();
        display_sums[ pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetMatingType()+4 ] += pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetCurMatingDisplayB();
      }
    }
    
    if (mating_type_sums[0] > 0) display_avgs[0] = ((double) display_sums[0]) / ((double) mating_type_sums[0]);
    if (mating_type_sums[1] > 0) display_avgs[1] = ((double) display_sums[1]) / ((double) mating_type_sums[1]);
    if (mating_type_sums[2] > 0) display_avgs[2] = ((double) display_sums[2]) / ((double) mating_type_sums[2]); 
    if (mating_type_sums[0] > 0) display_avgs[3] = ((double) display_sums[3]) / ((double) mating_type_sums[0]); 
    if (mating_type_sums[1] > 0) display_avgs[4] = ((double) display_sums[4]) / ((double) mating_type_sums[1]); 
    if (mating_type_sums[2] > 0) display_avgs[5] = ((double) display_sums[5]) / ((double) mating_type_sums[2]); 
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->WriteComment("Avida population mating display data");
    df->WriteTimeStamp();
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(display_avgs[0], "Avg mating display A for mating type -1 (undefined)");
    df->Write(display_avgs[1], "Avg mating display A for mating type 0 (female)");
    df->Write(display_avgs[2], "Avg mating display A for mating type 1 (male)");
    df->Write(display_avgs[3], "Avg mating display B for mating type -1 (undefined)");
    df->Write(display_avgs[4], "Avg mating display B for mating type 0 (female)");
    df->Write(display_avgs[5], "Avg mating display B for mating type 1 (male)");
    df->Endl();
  }
};

//Prints data about the current mate preferences of females in the population
class cActionPrintFemaleMatePreferenceData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintFemaleMatePreferenceData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    else m_filename = "female_mate_preference_data.dat";
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"female_mate_preference_data.dat\"]"; }
  
  void Process(cAvidaContext&)
  {
    //Mating preferences:
    // 0 = random 
    // 1 = highest display A
    // 2 = highest display B
    // 3 = highest merit
    //IMPORTANT!: Modify next line according to how many types of mate preferences there are in the population
    int mate_pref_sums[4] = {0, 0, 0, 0};
    cPopulation &pop = m_world->GetPopulation();
    for (int cell_num = 0; cell_num < pop.GetSize(); cell_num++) {
      if (pop.GetCell(cell_num).IsOccupied()) {
        if (pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) {
          mate_pref_sums[pop.GetCell(cell_num).GetOrganism()->GetPhenotype().GetMatePreference()]++;
        }
      }
    }
    
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->WriteComment("Avida population female mate preference histogram");
    df->WriteTimeStamp();
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(mate_pref_sums[0], "Random");
    df->Write(mate_pref_sums[1], "Highest display A");
    df->Write(mate_pref_sums[2], "Highest display B");
    df->Write(mate_pref_sums[3], "Highest merit");
    df->Endl();
  }
};

class cActionPrintSoloTaskSnapshot : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintSoloTaskSnapshot(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "tasks-snap.dat"; else m_filename = largs.PopWord();
  }
  static const cString GetDescription() { return "Arguments: [string fname=\"tasks-snap.dat\"]"; }
  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintSoloTaskSnapshot(m_filename, ctx);
  }
};


//Prints data about the 'offspring' from the birth chamber that were chosen as mates during the current update
class cActionPrintSuccessfulMates : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintSuccessfulMates(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"mates/mates-XXXX.dat\"]"; }
  
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set( "mates/mates-%s.dat", (const char*)cStringUtil::Convert(m_world->GetStats().GetUpdate()));
    m_world->GetStats().PrintSuccessfulMates(filename);
  }
};

//Prints data about all the 'offspring' waiting in the birth chamber
class cActionPrintBirthChamber : public cAction
{
private:
  cString m_filename;
  int m_hw_type;
  
public:
  cActionPrintBirthChamber(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename(""), m_hw_type(0)
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_hw_type = largs.PopWord().AsInt();
    else m_hw_type = 0;
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"birth_chamber/bc-XXXX.dat\"] [int hwtype=0]"; }
  
  void Process(cAvidaContext&)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set( "birth_chamber/bc-%s.dat", (const char*)cStringUtil::Convert(m_world->GetStats().GetUpdate()));
    m_world->GetPopulation().GetBirthChamber(m_hw_type).PrintBirthChamber(filename, m_hw_type);
  }
};


//Prints data about all the 'offspring' waiting in the birth chamber
class cActionPrintDominantData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintDominantData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename("dominant.dat")
  {
    cString largs(args);
    largs.Trim();
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname=\"dominant.dat\"]"; }
  
  void Process(cAvidaContext&)
  {
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    df->WriteComment("Avida Dominant Data");
    df->WriteTimeStamp();
    
    df->Write(m_world->GetStats().GetUpdate(),     "Update");
    
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    Systematics::GroupPtr bg = it->Next();
    if (!bg) return;
    
    df->Write(bg->Properties().Get("ave_metabolic_rate").DoubleValue(),       "Average Merit of the Dominant Genotype");
    df->Write(bg->Properties().Get("ave_gestation_time").DoubleValue(),   "Average Gestation Time of the Dominant Genotype");
    df->Write(bg->Properties().Get("ave_fitness").DoubleValue(),     "Average Fitness of the Dominant Genotype");
    df->Write(bg->Properties().Get("ave_repro_rate").DoubleValue(),  "Repro Rate?");
    
    Genome gen(bg->Properties().Get("genome"));
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(gen.Representation());
    df->Write(seq->GetSize(),        "Size of Dominant Genotype");
    df->Write(bg->Properties().Get("ave_copy_size").DoubleValue(), "Copied Size of Dominant Genotype");
    df->Write(bg->Properties().Get("ave_exe_size").DoubleValue(), "Executed Size of Dominant Genotype");
    df->Write(bg->NumUnits(),   "Abundance of Dominant Genotype");
    df->Write(bg->Properties().Get("last_births").IntValue(),      "Number of Births");
    df->Write(bg->Properties().Get("last_breed_true").IntValue(),  "Number of Dominant Breed True?");
    df->Write(bg->Depth(),  "Dominant Gene Depth");
    df->Write(bg->Properties().Get("last_breed_in").IntValue(),    "Dominant Breed In");
    df->Write(bg->Properties().Get("max_fitness").DoubleValue(),     "Max Fitness?");
    df->Write(bg->ID(), "Genotype ID of Dominant Genotype");
    df->Write(bg->Properties().Get("name").StringValue(),        "Name of the Dominant Genotype");
    df->Endl();    
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
  action_lib->Register<cActionPrintCountData>("PrintCountData");
  action_lib->Register<cActionPrintMessageData>("PrintMessageData");
  action_lib->Register<cActionPrintMessageLog>("PrintMessageLog");
  action_lib->Register<cActionPrintRetMessageLog>("PrintRetMessageLog");
  action_lib->Register<cActionPrintInterruptData>("PrintInterruptData");
  action_lib->Register<cActionPrintTotalsData>("PrintTotalsData");
  action_lib->Register<cActionPrintThreadsData>("PrintThreadsData");
  action_lib->Register<cActionPrintTasksData>("PrintTasksData");
  action_lib->Register<cActionPrintSoloTaskSnapshot>("PrintSoloTaskSnapshot");
  action_lib->Register<cActionPrintHostTasksData>("PrintHostTasksData");
  action_lib->Register<cActionPrintParasiteTasksData>("PrintParasiteTasksData");
  action_lib->Register<cActionPrintTasksExeData>("PrintTasksExeData");
  action_lib->Register<cActionPrintNewTasksData>("PrintNewTasksData");
  action_lib->Register<cActionPrintNewReactionData>("PrintNewReactionData");
  action_lib->Register<cActionPrintNewTasksDataPlus>("PrintNewTasksDataPlus");
  action_lib->Register<cActionPrintTasksQualData>("PrintTasksQualData");
  action_lib->Register<cActionPrintResourceData>("PrintResourceData");
  action_lib->Register<cActionPrintSpatialResources>("PrintSpatialResources");
  action_lib->Register<cActionPrintResourceLocData>("PrintResourceLocData");
  action_lib->Register<cActionPrintResWallLocData>("PrintResWallLocData");
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
  action_lib->Register<cActionPrintNumDivides>("PrintNumDivides");
  
  action_lib->Register<cActionPrintPreyAverageData>("PrintPreyAverageData");
  action_lib->Register<cActionPrintPredatorAverageData>("PrintPredatorAverageData");
  action_lib->Register<cActionPrintTopPredatorAverageData>("PrintTopPredatorAverageData");
  action_lib->Register<cActionPrintPreyErrorData>("PrintPreyErrorData");
  action_lib->Register<cActionPrintPredatorErrorData>("PrintPredatorErrorData");
  action_lib->Register<cActionPrintTopPredatorErrorData>("PrintTopPredatorErrorData");
  action_lib->Register<cActionPrintPreyVarianceData>("PrintPreyVarianceData");
  action_lib->Register<cActionPrintPredatorVarianceData>("PrintPredatorVarianceData");
  action_lib->Register<cActionPrintTopPredatorVarianceData>("PrintTopPredatorVarianceData");
  action_lib->Register<cActionPrintPreyInstructionData>("PrintPreyInstructionData");
  action_lib->Register<cActionPrintPredatorInstructionData>("PrintPredatorInstructionData");
  action_lib->Register<cActionPrintTopPredatorInstructionData>("PrintTopPredatorInstructionData");
  action_lib->Register<cActionPrintPreyFromSensorInstructionData>("PrintPreyFromSensorInstructionData");
  action_lib->Register<cActionPrintPredatorFromSensorInstructionData>("PrintPredatorFromSensorInstructionData");
  action_lib->Register<cActionPrintTopPredatorFromSensorInstructionData>("PrintTopPredatorFromSensorInstructionData");
  action_lib->Register<cActionPrintGroupAttackData>("PrintGroupAttackData");
  action_lib->Register<cActionPrintKilledPreyFTData>("PrintKilledPreyFTData");
  action_lib->Register<cActionPrintAttacks>("PrintAttacks");
  
  action_lib->Register<cActionPrintFromMessageInstructionData>("PrintFromMessageInstructionData");
  
  action_lib->Register<cActionPrintMaleInstructionData>("PrintMaleInstructionData");
  action_lib->Register<cActionPrintFemaleInstructionData>("PrintFemaleInstructionData");
  action_lib->Register<cActionPrintSenseData>("PrintSenseData");
  action_lib->Register<cActionPrintSenseExeData>("PrintSenseExeData");
  action_lib->Register<cActionPrintInstructionData>("PrintInstructionData");
  action_lib->Register<cActionPrintInternalTasksData>("PrintInternalTasksData");
  action_lib->Register<cActionPrintInternalTasksQualData>("PrintInternalTasksQualData");
  action_lib->Register<cActionPrintSleepData>("PrintSleepData");
  action_lib->Register<cActionPrintCompetitionData>("PrintCompetitionData");
  action_lib->Register<cActionPrintDynamicMaxMinData>("PrintDynamicMaxMinData");
  action_lib->Register<cActionPrintMaleAverageData>("PrintMaleAverageData");
  action_lib->Register<cActionPrintFemaleAverageData>("PrintFemaleAverageData");
  action_lib->Register<cActionPrintMaleErrorData>("PrintMaleErrorData");
  action_lib->Register<cActionPrintFemaleErrorData>("PrintFemaleErrorData");
  action_lib->Register<cActionPrintMaleVarianceData>("PrintMaleVarianceData");
  action_lib->Register<cActionPrintFemaleVarianceData>("PrintFemaleVarianceData");
  
  // @WRE: Added printing of visit data
  action_lib->Register<cActionPrintCellVisitsData>("PrintCellVisitsData");
  
  // Population Out Files
  action_lib->Register<cActionPrintPhenotypeData>("PrintPhenotypeData");
  action_lib->Register<cActionPrintParasitePhenotypeData>("PrintParasitePhenotypeData");
  action_lib->Register<cActionPrintHostPhenotypeData>("PrintHostPhenotypeData");
  action_lib->Register<cActionPrintPhenotypeStatus>("PrintPhenotypeStatus");
  
  action_lib->Register<cActionPrintDemeTestamentStats>("PrintDemeTestamentStats");
  action_lib->Register<cActionPrintCurrentMeanDemeDensity>("PrintCurrentMeanDemeDensity");
  
  action_lib->Register<cActionPrintPredicatedMessages>("PrintPredicatedMessages");
  action_lib->Register<cActionPrintCellData>("PrintCellData");
  action_lib->Register<cActionPrintConsensusData>("PrintConsensusData");
  action_lib->Register<cActionPrintSimpleConsensusData>("PrintSimpleConsensusData");
  action_lib->Register<cActionPrintCurrentOpinions>("PrintCurrentOpinions");
  action_lib->Register<cActionPrintOpinionsSetPerDeme>("PrintOpinionsSetPerDeme");
  action_lib->Register<cActionPrintSynchronizationData>("PrintSynchronizationData");
  action_lib->Register<cActionPrintCurrentMeanDemeDensity>("PrintCurrentMeanDemeDensity");
  
  action_lib->Register<cActionPrintPredicatedMessages>("PrintPredicatedMessages");
  action_lib->Register<cActionPrintCellData>("PrintCellData");
  action_lib->Register<cActionPrintConsensusData>("PrintConsensusData");
  action_lib->Register<cActionPrintSimpleConsensusData>("PrintSimpleConsensusData");
  action_lib->Register<cActionPrintCurrentOpinions>("PrintCurrentOpinions");
  action_lib->Register<cActionPrintOpinionsSetPerDeme>("PrintOpinionsSetPerDeme");
  action_lib->Register<cActionPrintSynchronizationData>("PrintSynchronizationData");
  action_lib->Register<cActionPrintDetailedSynchronizationData>("PrintDetailedSynchronizationData");
  
  action_lib->Register<cActionPrintDonationStats>("PrintDonationStats");
  
  // kabooms output file
  action_lib->Register<cActionPrintKaboom>("PrintKaboom");
  action_lib->Register<cActionPrintQuorum>("PrintQuorum");
  
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
  action_lib->Register<cActionPrintDemeGermlineSequestration>("PrintDemeGermlineSequestration");
  action_lib->Register<cActionPrintDemeOrgGermlineSequestration>("PrintDemeOrgGermlineSequestration");
  action_lib->Register<cActionPrintDemeGLSFounders>("PrintDemeGLSFounders");
  action_lib->Register<cActionPrintDemeReactionDiversityReplicationData>("PrintDemeReactionDiversityReplicationData");
  action_lib->Register<cActionPrintDemeGermResourcesData>("PrintDemeGermResourcesData");
  action_lib->Register<cActionPrintWinningDeme>("PrintWinningDeme");
  action_lib->Register<cActionPrintDemeTreatableReplicationData>("PrintDemeTreatableReplicationData");
  action_lib->Register<cActionPrintDemeUntreatableReplicationData>("PrintDemeUntreatableReplicationData");
  action_lib->Register<cActionPrintDemeTreatableCount>("PrintDemeTreatableCount");
  action_lib->Register<cActionPrintDemeFitness>("PrintDemeFitnessData");
  action_lib->Register<cActionPrintDemeLifeFitness>("PrintDemeLifeFitnessData");
  action_lib->Register<cActionPrintDemeTasks>("PrintDemeTasksData");
  
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
  
  action_lib->Register<cActionPrintDemesTasksData>("PrintDemesTasksData"); //@JJB**
  action_lib->Register<cActionPrintDemesReactionsData>("PrintDemesReactionsData"); //@JJB**
  action_lib->Register<cActionPrintDemesMeritsData>("PrintDemesMeritsData"); //@JJB**
  action_lib->Register<cActionPrintDemesFitnessData>("PrintDemesFitnessData"); //@JJB**
  
  action_lib->Register<cActionPrintMultiProcessData>("PrintMultiProcessData");
  action_lib->Register<cActionPrintProfilingData>("PrintProfilingData");
  action_lib->Register<cActionPrintOrganismLocation>("PrintOrganismLocation");
  action_lib->Register<cActionPrintOrgLocData>("PrintOrgLocData");
  action_lib->Register<cActionPrintPreyFlockingData>("PrintPreyFlockingData");
  action_lib->Register<cActionPrintOrgGuardData>("PrintOrgGuardData");
  action_lib->Register<cActionPrintAgePolyethismData>("PrintAgePolyethismData");
  action_lib->Register<cActionPrintIntrinsicTaskSwitchingCostData>("PrintIntrinsicTaskSwitchingCostData");
  action_lib->Register<cActionPrintDenData>("PrintDenData");
  
  
  //Coalescence Clade Actions
  action_lib->Register<cActionPrintCCladeCounts>("PrintCCladeCounts");
  action_lib->Register<cActionPrintCCladeFitnessHistogram>("PrintCCladeFitnessHistogram");
  action_lib->Register<cActionPrintCCladeRelativeFitnessHistogram>("PrintCCladeRelativeFitnessHistogram");
  
  // Processed Data
  action_lib->Register<cActionPrintData>("PrintData");
  action_lib->Register<cActionPrintInstructionAbundanceHistogram>("PrintInstructionAbundanceHistogram");
  action_lib->Register<cActionPrintDepthHistogram>("PrintDepthHistogram");
  action_lib->Register<cActionPrintParasiteDepthHistogram>("PrintParasiteDepthHistogram");
  action_lib->Register<cActionPrintHostDepthHistogram>("PrintHostDepthHistogram");
  action_lib->Register<cActionEcho>("Echo");
  action_lib->Register<cActionPrintGenotypeAbundanceHistogram>("PrintGenotypeAbundanceHistogram");
  //  action_lib->Register<cActionPrintSpeciesAbundanceHistogram>("PrintSpeciesAbundanceHistogram");
  //  action_lib->Register<cActionPrintLineageTotals>("PrintLineageTotals");
  action_lib->Register<cActionPrintLineageCounts>("PrintLineageCounts");
  action_lib->Register<cActionPrintDominantGenotype>("PrintDominantGenotype");
  action_lib->Register<cActionPrintDominantGroupGenotypes>("PrintDominantGroupGenotypes");
  action_lib->Register<cActionPrintDominantForagerGenotypes>("PrintDominantForagerGenotypes");
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
  action_lib->Register<cActionDumpClassificationIDGrid>("DumpClassificationIDGrid");
  action_lib->Register<cActionDumpFitnessGrid>("DumpFitnessGrid");
  action_lib->Register<cActionDumpGenotypeColorGrid>("DumpGenotypeColorGrid");
  action_lib->Register<cActionDumpPhenotypeIDGrid>("DumpPhenotypeIDGrid");
  action_lib->Register<cActionDumpIDGrid>("DumpIDGrid");
  action_lib->Register<cActionDumpVitalityGrid>("DumpVitalityGrid");
  action_lib->Register<cActionDumpTargetGrid>("DumpTargetGrid");
  action_lib->Register<cActionDumpMaxResGrid>("DumpMaxResGrid");
  action_lib->Register<cActionDumpTaskGrid>("DumpTaskGrid");
  action_lib->Register<cActionDumpLastTaskGrid>("DumpLastTaskGrid");
  action_lib->Register<cActionDumpHostTaskGrid>("DumpHostTaskGrid");
  action_lib->Register<cActionDumpParasiteTaskGrid>("DumpParasiteTaskGrid");
  action_lib->Register<cActionDumpHostTaskGridComma>("DumpHostTaskGridComma");
  action_lib->Register<cActionDumpParasiteTaskGridComma>("DumpParasiteTaskGridComma");
  action_lib->Register<cActionDumpParasiteVirulenceGrid>("DumpParasiteVirulenceGrid");
  action_lib->Register<cActionDumpOffspringMigrationCounts>("DumpOffspringMigrationCounts"); 
  action_lib->Register<cActionDumpParasiteMigrationCounts>("DumpParasiteMigrationCounts"); 
  
  action_lib->Register<cActionDumpReactionGrid>("DumpReactionGrid");
  action_lib->Register<cActionPrintLastReactionCountGrid>("PrintLastReactionCountGrid");
  action_lib->Register<cActionPrintCurrReactionCountGrid>("PrintCurrReactionCountGrid");
  action_lib->Register<cActionDumpDonorGrid>("DumpDonorGrid");
  action_lib->Register<cActionDumpReceiverGrid>("DumpReceiverGrid");
  action_lib->Register<cActionDumpEnergyGrid>("DumpEnergyGrid");
  action_lib->Register<cActionDumpExecutionRatioGrid>("DumpExecutionRatioGrid");
  action_lib->Register<cActionDumpCellDataGrid>("DumpCellDataGrid");
  action_lib->Register<cActionDumpSleepGrid>("DumpSleepGrid");
  action_lib->Register<cActionDumpGenomeLengthGrid>("DumpGenomeLengthGrid");
  action_lib->Register<cActionDumpGenotypeGrid>("DumpGenotypeGrid");
  action_lib->Register<cActionDumpParasiteGenotypeGrid>("DumpParasiteGenotypeGrid");
  
  //Dump Genotype Lists
  action_lib->Register<cActionDumpHostGenotypeList>("DumpHostGenotypeList");
  action_lib->Register<cActionDumpParasiteGenotypeList>("DumpParasiteGenotypeList");
  
  
  action_lib->Register<cActionPrintNumOrgsKilledData>("PrintNumOrgsKilledData");
  action_lib->Register<cActionPrintMigrationData>("PrintMigrationData");
  
  action_lib->Register<cActionPrintReputationData>("PrintReputationData");
  action_lib->Register<cActionPrintDirectReciprocityData>("PrintDirectReciprocityData");
  action_lib->Register<cActionPrintStringMatchData>("PrintStringMatchData");
  action_lib->Register<cActionPrintShadedAltruists>("PrintShadedAltruists");
  
  action_lib->Register<cActionPrintGroupsFormedData>("PrintGroupsFormedData");
  action_lib->Register<cActionPrintGroupIds>("PrintGroupIds");
  action_lib->Register<cActionPrintGroupTolerance>("PrintGroupTolerance"); 
  action_lib->Register<cActionPrintGroupMTTolerance>("PrintGroupMTTolerance"); 
  action_lib->Register<cActionPrintToleranceInstructionData>("PrintToleranceInstructionData"); 
  action_lib->Register<cActionPrintToleranceData>("PrintToleranceData"); 
  action_lib->Register<cActionPrintTargets>("PrintTargets");
  action_lib->Register<cActionPrintMimicDisplays>("PrintMimicDisplays");
  action_lib->Register<cActionPrintTopPredTargets>("PrintTopPredTargets");
  
  action_lib->Register<cActionPrintHGTData>("PrintHGTData");
  
  action_lib->Register<cActionSetVerbose>("SetVerbose");
  action_lib->Register<cActionSetVerbose>("VERBOSE");
  
  action_lib->Register<cActionPrintNumOrgsInDeme>("PrintNumOrgsInDeme");
  action_lib->Register<cActionCalcConsensus>("CalcConsensus");
  action_lib->Register<cActionPrintEditDistance>("PrintEditDistance");
  
  //@CHC: Mating type-related actions	
  action_lib->Register<cActionPrintMatingTypeHistogram>("PrintMatingTypeHistogram");
  action_lib->Register<cActionPrintMatingDisplayData>("PrintMatingDisplayData");
  action_lib->Register<cActionPrintFemaleMatePreferenceData>("PrintFemaleMatePreferenceData");
  action_lib->Register<cActionPrintBirthChamberMatingTypeHistogram>("PrintBirthChamberMatingTypeHistogram");
  action_lib->Register<cActionPrintSuccessfulMates>("PrintSuccessfulMates");
  action_lib->Register<cActionPrintBirthChamber>("PrintBirthChamber");
  
  
  action_lib->Register<cActionPrintDominantData>("PrintDominantData");
  
};
