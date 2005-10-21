//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_HH
#define ANALYZE_HH

#include <vector>

#ifndef DATA_FILE_MANAGER_HH
#include "cDataFileManager.h"
#endif
#ifndef GENOTYPE_BATCH_HH
#include "cGenotypeBatch.h"
#endif
#ifndef cRandom_h
#include "cRandom.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef STRING_LIST_HH
#include "cStringList.h"
#endif
#ifndef TLIST_HH
#include "tList.h"
#endif

#define MAX_BATCHES 2000

// cAnalyze            : The master analyze object.

class cGenotypeBatch; // array
template <class T> class tList; // aggregate
class cAnalyzeCommand;
class cAnalyzeFunction;
class cAnalyzeCommandDefBase;
class cString; // aggregate
class cDataFileManager; // aggregate
template <class T> class tDataEntryBase;
class cInstSet;
class cRandom; // aggregate
class cString; // aggregate
class cAnalyzeGenotype;
class cInitFile;
class cStringList; // aggregate
template <class T> class tDataEntryCommand;
template <class T> class tListIterator;
class cEnvironment;
class cWorld;

class cAnalyze {
private:
  int cur_batch;
  cGenotypeBatch batch[MAX_BATCHES];
  tList<cAnalyzeCommand> command_list;
  tList<cAnalyzeFunction> function_list;
  tList<cAnalyzeCommandDefBase> command_lib;
  cString variables[26];
  cString local_variables[26];
  cString arg_variables[10];

  cWorld* m_world;
  cInstSet& inst_set;

  // This is the storage for the resource information from resource.dat.  It 
  // is a pair of the update and a vector of the resource concentrations
  std::vector<std::pair<int, std::vector<double> > > resources;

  bool verbose;            // Should details be output to command line?
  int interactive_depth;   // How nested are we if in interactive mode?

  cDataFileManager data_file_manager;
  tList< tDataEntryBase<cAnalyzeGenotype> > genotype_data_list;

  cRandom random;

private:
  // Pop specific types of arguments from an arg list.
  cString PopDirectory(cString & in_string, const cString & default_dir);
  int PopBatch(const cString & in_string);
  cAnalyzeGenotype * PopGenotype(cString gen_desc, int batch_id=-1);
  cString & GetVariable(const cString & varname);

  // Other arg-list methods
  void LoadCommandList(cInitFile & init_file, tList<cAnalyzeCommand> & clist);
  void InteractiveLoadCommandList(tList<cAnalyzeCommand> & clist);
  void PreProcessArgs(cString & args);
  void ProcessCommands(tList<cAnalyzeCommand> & clist);

  void SetupGenotypeDataList();	
  void LoadGenotypeDataList(cStringList arg_list,
	    tList< tDataEntryCommand<cAnalyzeGenotype> > & output_list);
		      
  void AddLibraryDef(const cString & name, void (cAnalyze::*_fun)(cString));
  void AddLibraryDef(const cString & name,
	     void (cAnalyze::*_fun)(cString, tList<cAnalyzeCommand> &));
  cAnalyzeCommandDefBase * FindAnalyzeCommandDef(const cString & name);
  void SetupCommandDefLibrary();
  bool FunctionRun(const cString & fun_name, cString args);

  // Batch management...
  int BatchUtil_GetMaxLength(int batch_id=-1);

  // Comamnd helpers...
  void CommandDetail_Header(std::ostream & fp, int format_type,
            tListIterator< tDataEntryCommand<cAnalyzeGenotype> > & output_it,
            int time_step=-1);
  void CommandDetail_Body(std::ostream & fp, int format_type,
            tListIterator< tDataEntryCommand<cAnalyzeGenotype> > & output_it,
            int time_step=-1, int max_time=1);
  void CommandDetailAverage_Body(std::ostream & fp, int num_arguments,
            tListIterator< tDataEntryCommand<cAnalyzeGenotype> > & output_it);
  void CommandHistogram_Header(std::ostream & fp, int format_type,
            tListIterator< tDataEntryCommand<cAnalyzeGenotype> > & output_it);
  void CommandHistogram_Body(std::ostream & fp, int format_type,
            tListIterator< tDataEntryCommand<cAnalyzeGenotype> > & output_it);

private:
  // Loading methods...
  void LoadOrganism(cString cur_string);
  void LoadBasicDump(cString cur_string);
  void LoadDetailDump(cString cur_string);
  void LoadMultiDetail(cString cur_string);
  void LoadSequence(cString cur_string);
  void LoadDominant(cString cur_string);
  // Clears the current time oriented list of resources and loads in a new one
  // from a file specified by the user, or resource.dat by default.
  void LoadResources(cString cur_string);
  void LoadFile(cString cur_string);

  // Reduction
  void FindGenotype(cString cur_string);
  void FindOrganism(cString cur_string);
  void FindLineage(cString cur_string);
  void FindSexLineage(cString cur_string);
  void FindClade(cString cur_string);
  void SampleOrganisms(cString cur_string);
  void SampleGenotypes(cString cur_string);
  void KeepTopGenotypes(cString cur_string);

  // Direct Output Commands...
  void CommandPrint(cString cur_string);
  void CommandTrace(cString cur_string);
  void CommandTraceWithResources(cString cur_string);
  void CommandPrintTasks(cString cur_string);
  void CommandDetail(cString cur_string);
  void CommandDetailTimeline(cString cur_string);
  void CommandDetailBatches(cString cur_string);
  void CommandDetailAverage(cString cur_string);
  void CommandDetailIndex(cString cur_string);
  void CommandHistogram(cString cur_string);

  // Population Analysis Commands...
  void CommandPrintPhenotypes(cString cur_string);
  void CommandPrintDiversity(cString cur_string);
  void PhyloCommunityComplexity(cString cur_string);
  void CommunityComplexity(cString cur_string);

  // Individual Organism Analysis...
  void CommandLandscape(cString cur_string);
  void CommandFitnessMatrix(cString cur_string);
  void CommandMapTasks(cString cur_string);
  void CommandAverageModularity(cString cur_string);
  void CommandMapMutations(cString cur_string);
  void CommandMapDepth(cString cur_string);

  // Population Comparison Commands...
  void CommandHamming(cString cur_string);
  void CommandLevenstein(cString cur_string);
  void CommandSpecies(cString cur_string);
  void CommandRecombine(cString cur_string);

  // Lineage Analysis Commands...
  void CommandAlign(cString cur_string);
  void AnalyzeNewInfo(cString cur_string);   

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
  void AnalyzePopComplexity(cString cur_string);
  void AnalyzeEpistasis(cString cur_string);
  void AnalyzeMateSelection(cString cur_string);

  // Environment Manipulation
  void EnvironmentSetup(cString cur_string);

  // Documentation...
  void CommandHelpfile(cString cur_string);
  void CommandDocfile(cString cur_string);

  // Control...
  void VarSet(cString cur_string);
  void BatchSet(cString cur_string);
  void BatchName(cString cur_string);
  void BatchTag(cString cur_string);
  void BatchPurge(cString cur_string);
  void BatchDuplicate(cString cur_string);
  void BatchRecalculate(cString cur_string);
  void BatchRename(cString cur_string);
  void PrintStatus(cString cur_string);
  void PrintDebug(cString cur_string);
  void ToggleVerbose(cString cur_string);
  void IncludeFile(cString cur_string);
  void CommandSystem(cString cur_string);
  void CommandInteractive(cString cur_string);
  void PrintTestCPUResources(cString cur_string);

  // Functions...
  void FunctionCreate(cString cur_string, tList<cAnalyzeCommand> & clist);
  // Looks up the resource concentrations that are the closest to the
  // given update and then fill in those concentrations into the environment.
  void FillResources(int update);
  // Analyze the entropy of genotype under default environment
  double AnalyzeEntropy(cAnalyzeGenotype * genotype, double mut_rate);
  // Analyze the entropy of child given parent and default environment
  double AnalyzeEntropyGivenParent(cAnalyzeGenotype * genotype, 
				   cAnalyzeGenotype * parent, 
				   double mut_rate);
  // Calculate the increased information in genotype1 comparing to genotype2 
  // line by line. If information in genotype1 is less than that in genotype2 
  // in a line, increasing is 0. Usually this is used for child-parent comparison.
  double IncreasedInfo(cAnalyzeGenotype * genotype1, 
		       cAnalyzeGenotype * genotype2, 
		       double mut_rate);
  
  // Flow Control...
  void CommandForeach(cString cur_string, tList<cAnalyzeCommand> & clist);
  void CommandForRange(cString cur_string, tList<cAnalyzeCommand> & clist);

private:
  // disabled copy constructor.
  cAnalyze(const cAnalyze &);
public:
  cAnalyze(cWorld* world);
  ~cAnalyze();

  void RunInteractive();
};

#endif
