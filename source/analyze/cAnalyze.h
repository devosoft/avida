/*
 *  cAnalyze.h
 *  Avida
 *
 *  Called "analyze.hh" prior to 12/1/05.
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

#ifndef cAnalyze_h
#define cAnalyze_h

#include <iostream>

#include "cAnalyzeJobQueue.h"
#include "cAvidaContext.h"
#include "cGenotypeBatch.h"
#include "cFlexVar.h"
#include "cRandom.h"
#include "cString.h"
#include "cStringList.h"
#include "tList.h"
#include "tMatrix.h"
#include "tHashMap.h"


const int MAX_BATCHES = 2000;

class cAnalyzeCommand;
class cAnalyzeCommandDefBase;
class cAnalyzeFunction;
class cAnalyzeGenotype;
class cAnalyzeScreen;
class cCPUTestInfo;
class cEnvironment;
class cInitFile;
class cInstSet;
class cResourceHistory;
class cTestCPU;
class cWorld;
template <class T> class tDataEntry;
template <class T> class tDataEntryCommand;


class cAnalyze {
  friend class cAnalyzeScreen;

private:
  int cur_batch;

  /*
  FIXME : refactor "temporary_next_id". @kgn
  - Added as a quick way to provide unique serial ids, per organism, in COMPETE
    command. @kgn
  */
  int temporary_next_id;
  int temporary_next_update;
  void SetTempNextUpdate(int next){ temporary_next_update = next; }
  void SetTempNextID(int next){ temporary_next_id = next; }
  int GetTempNextUpdate(){ return temporary_next_update; }
  int GetTempNextID(){ return temporary_next_id; }

  cGenotypeBatch batch[MAX_BATCHES];
  tList<cAnalyzeCommand> command_list;
  tList<cAnalyzeFunction> function_list;
  tList<cAnalyzeCommandDefBase> command_lib;
  tArray<cString> variables;
  tArray<cString> local_variables;
  tArray<cString> arg_variables;

  bool exit_on_error;

  cWorld* m_world;
  cAvidaContext& m_ctx;
  cAnalyzeJobQueue m_jobqueue;

  // This is the storage for the resource information from resource.dat.
  cResourceHistory* m_resources;
  int m_resource_time_spent_offset; // The amount to offset the time spent when 
                                    // beginning, using resources that change

  int interactive_depth;  // How nested are we if in interactive mode?

  cRandom random;

  // These are a set of constants used to determine what type of comparisons should be done between an
  // organism and its parent to determine how it should be colored
  enum eFlexCompareTypes {
    FLEX_COMPARE_NONE  = 0,  // No comparisons should be done at all.
    FLEX_COMPARE_DIFF  = 1,  // Only track if a stat has changed, don't worry about direction.
    FLEX_COMPARE_MAX   = 2,  // Color higher values as beneficial, lower as harmful.
    FLEX_COMPARE_MIN   = 3,  // Color lower values as beneficial, higher as harmful.
    FLEX_COMPARE_DIFF2 = 4,  // Same as FLEX_COMPARE_DIFF, but 0 indicates trait is off.
    FLEX_COMPARE_MAX2  = 5,  // Same as FLEX_COMPARE_MAX, and 0 indicates trait is off.
    FLEX_COMPARE_MIN2  = 6   // Same as FLEX_COMPARE_MIN, BUT 0 still indicates off.
  };

  enum eFlexCompareResults {
    COMPARE_RESULT_OFF  = -2, // Has turned off since parent.
    COMPARE_RESULT_NEG  = -1, // Has gotten worse since parent.
    COMPARE_RESULT_SAME =  0, // No difference since parent.
    COMPARE_RESULT_POS  =  1, // Has improved since parent.
    COMPARE_RESULT_ON   =  2, // Has turned on since parent.
    COMPARE_RESULT_DIFF =  3  // Is different from parent (non qualtity).
  };


  cAnalyze(); // @not_implemented
  cAnalyze(const cAnalyze&); // @not_implemented
  cAnalyze& operator=(const cAnalyze&); // @not_implemented

public:
  cAnalyze(cWorld* world);
  ~cAnalyze();

  void RunFile(cString filename);
  void RunInteractive();
  bool Send(const cString &text_input);
  bool Send(const cStringList &list_input);
  
  int GetCurrentBatchID() { return cur_batch; }
  cGenotypeBatch& GetCurrentBatch() { return batch[cur_batch]; }
  cGenotypeBatch& GetBatch(int id) { assert(id >= 0 && id < MAX_BATCHES); return batch[id]; }
  cAnalyzeJobQueue& GetJobQueue() { return m_jobqueue; }
  
  void AlignCurrentBatch() { CommandAlign(""); }
  
  static void PopCommonCPUTestParameters(cWorld* in_world, cString& cur_string, cCPUTestInfo& test_info,
    cResourceHistory* in_resource_history = NULL, int in_resource_time_spent_offset = 0);
    
  // structure for phenotype statistics, used in CommandPrintPhenotypes
  struct p_stats {
    cBitArray phen_id;
    int cpu_count;
    int genotype_count;
    double total_length;
    double total_gest;
    int total_task_count;
    int total_task_performance_count;
  };
  
private:
  // Pop specific types of arguments from an arg list.
  cString PopDirectory(cString  in_string, const cString default_dir);
  int PopBatch(const cString& in_string);
  cAnalyzeGenotype* PopGenotype(cString gen_desc, int batch_id = -1);
  cString & GetVariable(const cString & varname);
  
  // Other arg-list methods
  int LoadCommandList(cInitFile& init_file, tList<cAnalyzeCommand>& clist, int start_line = 0);
  void InteractiveLoadCommandList(tList<cAnalyzeCommand>& clist);
  void PreProcessArgs(cString& args);
  void ProcessCommands(tList<cAnalyzeCommand>& clist);
  
  // Helper functions for printing to HTML files...
  void HTMLPrintStat(const cFlexVar& value, std::ostream& fp, int compare=0,
                     const cString& cell_flags="align=center", const cString& null_text = "0", bool print_text = true);
  int CompareFlexStat(const cFlexVar& org_stat, const cFlexVar& parent_stat, int compare_type = FLEX_COMPARE_MAX);
  
  
  void AddLibraryDef(const cString & name, void (cAnalyze::*_fun)(cString));
  void AddLibraryDef(const cString & name, void (cAnalyze::*_fun)(cString, tList<cAnalyzeCommand> &));
  cAnalyzeCommandDefBase* FindAnalyzeCommandDef(const cString& name);
  void SetupCommandDefLibrary();
  bool FunctionRun(const cString& fun_name, cString args);
  
  // Batch management...
  int BatchUtil_GetMaxLength(int batch_id = -1);
  
  // Command helpers...
  void CommandDetail_Header(std::ostream& fp, int format_type,
                            tListIterator< tDataEntryCommand<cAnalyzeGenotype> >& output_it, int time_step = -1);
  void CommandDetail_Body(std::ostream& fp, int format_type,
                          tListIterator< tDataEntryCommand<cAnalyzeGenotype> > & output_it,
                          int time_step = -1, int max_time = 1);
  void CommandDetailAverage_Body(std::ostream& fp, int num_arguments,
                                 tListIterator< tDataEntryCommand<cAnalyzeGenotype> >& output_it);
  void CommandHistogram_Header(std::ostream& fp, int format_type,
                               tListIterator< tDataEntryCommand<cAnalyzeGenotype> >& output_it);
  void CommandHistogram_Body(std::ostream& fp, int format_type,
                             tListIterator< tDataEntryCommand<cAnalyzeGenotype> >& output_it);
  static int PStatsComparator(const void * elem1, const void * elem2);  // must be static for qsort to accept it
  
  // Loading methods...
  void LoadOrganism(cString cur_string);
  void LoadSequence(cString cur_string);
  // Clears the current time oriented list of resources and loads in a new one
  // from a file specified by the user, or resource.dat by default.
  void LoadResources(cString cur_string);
  void LoadFile(cString cur_string);
  
  // Reduction and Sampling
  void CommandFilter(cString cur_string);
  void FindGenotype(cString cur_string);
  void FindOrganism(cString cur_string);
  void FindLineage(cString cur_string);
  void FindSexLineage(cString cur_string);
  void FindClade(cString cur_string);
  void FindLastCommonAncestor(cString cur_string);
  void SampleOrganisms(cString cur_string);
  void SampleGenotypes(cString cur_string);
  void KeepTopGenotypes(cString cur_string);
  void TruncateLineage(cString cur_string);
  void SampleOffspring(cString cur_string);
  
  // Direct Output Commands...
  void CommandPrint(cString cur_string);
  void CommandTrace(cString cur_string);
  void CommandTraceWithResources(cString cur_string);
  void CommandPrintTasks(cString cur_string);
  void CommandPrintTasksQuality(cString cur_string);
  void CommandDetail(cString cur_string);
  void CommandDetailTimeline(cString cur_string);
  void CommandDetailBatches(cString cur_string);
  void CommandDetailAverage(cString cur_string);
  void CommandDetailIndex(cString cur_string);
  void CommandHistogram(cString cur_string);
  
  // Population Analysis Commands...
  void CommandPrintPhenotypes(cString cur_string);
  void CommandPrintDiversity(cString cur_string);
  void CommandPrintDistances(cString cur_String);
  void CommandPrintTreeStats(cString cur_string);
  void CommandPrintCumulativeStemminess(cString cur_string);
  void CommandPrintGamma(cString cur_string);
  void PhyloCommunityComplexity(cString cur_string);
  void AnalyzeCommunityComplexity(cString cur_string);
  void CommandPrintResourceFitnessMap(cString cur_string);
  
  // Individual Organism Analysis...
  void CommandFitnessMatrix(cString cur_string);
  void CommandMapTasks(cString cur_string);
  void CommandCalcFunctionalModularity(cString cur_string);
  void CommandAverageModularity(cString cur_string);
  void CommandAnalyzeModularity(cString cur_string);
  void CommandAnalyzeRedundancyByInstFailure(cString cur_string);
  void CommandMapMutations(cString cur_string);
  void CommandMapDepth(cString cur_string);
  void CommandPairwiseEntropy(cString cur_string);
  
  // Population Comparison Commands...
  void CommandHamming(cString cur_string);
  void CommandLevenstein(cString cur_string);
  void CommandSpecies(cString cur_string);
  void CommandRecombine(cString cur_string);
  
  // Lineage Analysis Commands...
  void CommandAlign(cString cur_string);
  void AnalyzeNewInfo(cString cur_string);  
  void MutationRevert(cString cur_string);
  
  // Build Input Files for Avida
  void WriteClone(cString cur_string);
  void WriteInjectEvents(cString cur_string);
  void WriteCompetition(cString cur_string);
  
  // Automated analysis...
  void AnalyzeMuts(cString cur_string);
  void AnalyzeInstructions(cString cur_string);
  void AnalyzeInstPop(cString cur_string);
  void AnalyzeBranching(cString cur_string);
  void AnalyzeMutationTraceback(cString cur_string);
  void AnalyzeComplexity(cString cur_string);
  void AnalyzeFitnessLandscapeTwoSites(cString cur_string);
  void AnalyzeComplexityTwoSites(cString cur_string);
  void AnalyzeKnockouts(cString cur_string);
  void AnalyzePopComplexity(cString cur_string);
  void AnalyzeMateSelection(cString cur_string);
  void AnalyzeComplexityDelta(cString cur_string);
  
  // Environment Manipulation
  void EnvironmentSetup(cString cur_string);
  
  // Documentation...
  void CommandHelpfile(cString cur_string);
  
  // Control...
  void VarSet(cString cur_string);
  void ConfigGet(cString cur_string);
  void ConfigSet(cString cur_string);
  void BatchSet(cString cur_string);
  void BatchName(cString cur_string);
  void BatchTag(cString cur_string);
  void BatchPurge(cString cur_string);
  void BatchDuplicate(cString cur_string);
  void BatchRecalculate(cString cur_string);
  void BatchRecalculateWithArgs(cString cur_string);
  void BatchRename(cString cur_string);
  void CloseFile(cString cur_string);
  void PrintStatus(cString cur_string);
  void PrintDebug(cString cur_string);
  void PrintTestInfo(cString cur_string);
  void IncludeFile(cString cur_string);
  void CommandSystem(cString cur_string);
  void CommandInteractive(cString cur_string);
  
  // Uncategorized...
  void BatchCompete(cString cur_string);
  
  // Functions...
  void FunctionCreate(cString cur_string, tList<cAnalyzeCommand>& clist);
  
  // Looks up the resource concentrations that are the closest to the
  // given update and then fill in those concentrations into the environment.
  void FillResources(cTestCPU* testcpu, int update);
  
  // Analyze the entropy of genotype under default environment
  double AnalyzeEntropy(cAnalyzeGenotype* genotype, double mut_rate);
  
  // Analyze the entropy of child given parent and default environment
  double AnalyzeEntropyGivenParent(cAnalyzeGenotype* genotype, cAnalyzeGenotype* parent, double mut_rate);
  
  // Calculate the increased information in genotype1 comparing to genotype2 
  // line by line. If information in genotype1 is less than that in genotype2 
  // in a line, increasing is 0. Usually this is used for child-parent comparison.
  double IncreasedInfo(cAnalyzeGenotype* genotype1, cAnalyzeGenotype* genotype2, double mut_rate);
  
  //Calculate covarying information between pairs of sites
  tMatrix<double> AnalyzeEntropyPairs(cAnalyzeGenotype* genotype, double mut_rate);
  

  // Flow Control...
  void CommandForeach(cString cur_string, tList<cAnalyzeCommand>& clist);
  void CommandForRange(cString cur_string, tList<cAnalyzeCommand>& clist);
};

#endif
