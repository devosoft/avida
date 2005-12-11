/*
 *  cEventManager.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cEventManager.h"

#include "cAnalyzeUtil.h"
#include "avida.h"
#include "cClassificationManager.h"
#include "cEnvironment.h"
#include "cEvent.h"
#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cInjectGenotype.h"
#include "cInstUtil.h"
#include "cLandscape.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cResource.h"
#include "cStats.h"
#include "cStringUtil.h"
#include "cTestCPU.h"
#include "cTestUtil.h"
#include "cTools.h"
#include "cWorld.h"
#include "cWorldDriver.h"

#include <ctype.h>           // for isdigit
#include <iostream>

using namespace std;


class cEvent_exit : public cEvent {
public:
  const cString GetName() const { return "exit"; }
  const cString GetDescription() const { return "exit"; }
  
  void Configure(cWorld* world, const cString& in_args) { ; }
  void Process(){
    m_world->GetDriver().SetDone();
  }
};

///// exit_if_generation_greater_than /////

/**
* Ends the Avida run when the current generation exceeds the
 * maximum generation given as parameter.
 *
 * Parameters:
 * max generation (int)
 *   The generation at which the run should be stopped.
 **/
class cEvent_exit_if_generation_greater_than : public cEvent {
private:
  int max_generation;
public:
  const cString GetName() const { return "exit_if_generation_greater_than"; }
  const cString GetDescription() const { return "exit_if_generation_greater_than  <int max_generation>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    max_generation = args.PopWord().AsInt();
  }
  ///// exit_if_generation_greater_than /////
  void Process(){
    if( m_world->GetStats().SumGeneration().Average() > max_generation ){
      m_world->GetDriver().SetDone();
    }
  }
};

///// exit_if_update_greater_than /////

/**
* Ends the Avida run when the current update exceeds the
 * maximum update given as parameter.
 *
 * Parameters:
 * max update (int)
 *   The update at which the run should be stopped.
 **/


class cEvent_exit_if_update_greater_than : public cEvent {
private:
  int max_update;
public:
  const cString GetName() const { return "exit_if_update_greater_than"; }
  const cString GetDescription() const { return "exit_if_update_greater_than  <int max_update>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    max_update = args.PopWord().AsInt();
  }
  ///// exit_if_update_greater_than /////
  void Process(){
    if( m_world->GetStats().GetUpdate() > max_update ){
      m_world->GetDriver().SetDone();
    }
  }
};

///// exit_if_ave_lineage_label_smaller /////

/**
* Halts the avida run if the current average lineage label is smaller
 * than the value given as parameter.
 *
 * Parameters:
 * lineage_label_crit_value (int)
 *   The critical value to which the average lineage label is compared.
 **/


class cEvent_exit_if_ave_lineage_label_smaller : public cEvent {
private:
  double lineage_label_crit_value;
public:
  const cString GetName() const { return "exit_if_ave_lineage_label_smaller"; }
  const cString GetDescription() const { return "exit_if_ave_lineage_label_smaller  <double lineage_label_crit_value>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    lineage_label_crit_value = args.PopWord().AsDouble();
  }
  ///// exit_if_ave_lineage_label_smaller /////
  void Process(){
    if( m_world->GetStats().GetAveLineageLabel() < lineage_label_crit_value ){
      m_world->GetDriver().SetDone();
    }
  }
};

///// exit_if_ave_lineage_label_larger /////

/**
* Halts the avida run if the current average lineage label is larger
 * than the value given as parameter.
 *
 * Parameters:
 * lineage_label_crit_value (int)
 *   The critical value to which the average lineage label is compared.
 **/


class cEvent_exit_if_ave_lineage_label_larger : public cEvent {
private:
  double lineage_label_crit_value;
public:
  const cString GetName() const { return "exit_if_ave_lineage_label_larger"; }
  const cString GetDescription() const { return "exit_if_ave_lineage_label_larger  <double lineage_label_crit_value>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    lineage_label_crit_value = args.PopWord().AsDouble();
  }
  ///// exit_if_ave_lineage_label_larger /////
  void Process(){
    if( m_world->GetStats().GetAveLineageLabel() > lineage_label_crit_value ){
      m_world->GetDriver().SetDone();
    }
  }
};

///// echo /////

/**
* Writes out a message. If no message is given, average update and
 * generation are written out.
 *
 * Parameters:
 * message (string)
 **/


class cEvent_echo : public cEvent {
private:
  cString mesg;
public:
  const cString GetName() const { return "echo"; }
  const cString GetDescription() const { return "echo  <cString mesg>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    mesg = args.PopWord();
  }
  ///// echo /////
  void Process(){
    if( mesg == "" ){
      mesg.Set("Echo : Update = %f\t AveGeneration = %f",
               m_world->GetStats().GetUpdate(), m_world->GetStats().SumGeneration().Average());
    }
    m_world->GetDriver().NotifyComment(mesg);
  }
};

///// print_data /////

/**
* Output user-defined data from the cStats object...
 *
 * Parameters:
 * filename (string)
 *   The name of the data file.
 * format
 *   A comma-seperated list of statistics to output.
 **/


class cEvent_print_data : public cEvent {
private:
  cString filename;
  cString format;
public:
    const cString GetName() const { return "print_data"; }
  const cString GetDescription() const { return "print_data  <cString filename> <cString format>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    filename = args.PopWord();
    format = args.PopWord();
  }
  ///// print_data /////
  void Process(){
    m_world->GetStats().PrintDataFile(filename, format, ',');
  }
};

///// print_average_data /////

/**
* Output various average quantities into datafile.
 *
 * Parameters:
 * filename (string) default: average.dat
   *   The name of the data file.
   **/


class cEvent_print_average_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_average_data"; }
  const cString GetDescription() const { return "print_average_data  [cString fname=\"average.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="average.dat"; else fname=args.PopWord();
  }
  ///// print_average_data /////
  void Process(){
    m_world->GetStats().PrintAverageData(fname);
  }
};

///// print_error_data /////

/**
* Prints out various data related to statistical errors.
 *
 * Parameters:
 * filename (string) default: error.dat
   *   The name of the data file.
   **/


class cEvent_print_error_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_error_data"; }
  const cString GetDescription() const { return "print_error_data  [cString fname=\"error.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="error.dat"; else fname=args.PopWord();
  }
  ///// print_error_data /////
  void Process(){
    m_world->GetStats().PrintErrorData(fname);
  }
};

///// print_variance_data /////

/**
* Prints out various variances.
 *
 * Parameters:
 * filename (string) default: variance.dat
   *   The name of the data file.
   **/


class cEvent_print_variance_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_variance_data"; }
  const cString GetDescription() const { return "print_variance_data  [cString fname=\"variance.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="variance.dat"; else fname=args.PopWord();
  }
  ///// print_variance_data /////
  void Process(){
    m_world->GetStats().PrintVarianceData(fname);
  }
};

///// print_dominant_data /////

/**
* Output various quantities related to the dominant organism.
 *
 * Parameters:
 * filename (string) default: dominant.dat
   *   The name of the data file.
   **/


class cEvent_print_dominant_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_dominant_data"; }
  const cString GetDescription() const { return "print_dominant_data  [cString fname=\"dominant.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="dominant.dat"; else fname=args.PopWord();
  }
  ///// print_dominant_data /////
  void Process(){
    m_world->GetStats().PrintDominantData(fname);
  }
};

///// print_stats_data /////

/**
* Output various statistical quantities.
 *
 * Parameters:
 * filename (string) default: stats.dat
   *   The name of the data file.
   **/


class cEvent_print_stats_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_stats_data"; }
  const cString GetDescription() const { return "print_stats_data  [cString fname=\"stats.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="stats.dat"; else fname=args.PopWord();
  }
  ///// print_stats_data /////
  void Process(){
    m_world->GetStats().PrintStatsData(fname);
  }
};

///// print_count_data /////

/**
* Output various counts, such as number of organisms etc.
 *
 * Parameters:
 * filename (string) default: count.dat
   *   The name of the data file.
   **/


class cEvent_print_count_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_count_data"; }
  const cString GetDescription() const { return "print_count_data  [cString fname=\"count.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="count.dat"; else fname=args.PopWord();
  }
  ///// print_count_data /////
  void Process(){
    m_world->GetStats().PrintCountData(fname);
  }
};

///// print_totals_data /////

/**
* Various total numbers.
 *
 * Parameters:
 * filename (string) default: totals.dat
   *   The name of the data file.
   **/


class cEvent_print_totals_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_totals_data"; }
  const cString GetDescription() const { return "print_totals_data  [cString fname=\"totals.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="totals.dat"; else fname=args.PopWord();
  }
  ///// print_totals_data /////
  void Process(){
    m_world->GetStats().PrintTotalsData(fname);
  }
};

///// print_tasks_data /////

/**
* Output the number of times the various tasks have been performed in the
 * last update.
 *
 * Parameters:
 * filename (string) default: tasks.dat
   *   The name of the data file.
   **/


class cEvent_print_tasks_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_tasks_data"; }
  const cString GetDescription() const { return "print_tasks_data  [cString fname=\"tasks.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="tasks.dat"; else fname=args.PopWord();
  }
  ///// print_tasks_data /////
  void Process(){
    m_world->GetStats().PrintTasksData(fname);
  }
};

///// print_tasks_exe_data /////

/**
**/


class cEvent_print_tasks_exe_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_tasks_exe_data"; }
  const cString GetDescription() const { return "print_tasks_exe_data  [cString fname=\"tasks_exe.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="tasks_exe.dat"; else fname=args.PopWord();
  }
  ///// print_tasks_exe_data /////
  void Process(){
    m_world->GetStats().PrintTasksExeData(fname);
  }
};

///// print_resource_data /////

/**
**/


class cEvent_print_resource_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_resource_data"; }
  const cString GetDescription() const { return "print_resource_data  [cString fname=\"resource.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="resource.dat"; else fname=args.PopWord();
  }
  ///// print_resource_data /////
  void Process(){
    m_world->GetStats().PrintResourceData(fname);
  }
};

///// print_time_data /////

/**
* Output time related data, such as update, generation, etc.
 *
 * Parameters:
 * filename (string) default: time.dat
   *   The name of the data file.
   **/


class cEvent_print_time_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_time_data"; }
  const cString GetDescription() const { return "print_time_data  [cString fname=\"time.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="time.dat"; else fname=args.PopWord();
  }
  ///// print_time_data /////
  void Process(){
    m_world->GetStats().PrintTimeData(fname);
  }
};

///// print_mutation_data /////

/**
**/


class cEvent_print_mutation_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_mutation_data"; }
  const cString GetDescription() const { return "print_mutation_data  [cString fname=\"mutation.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="mutation.dat"; else fname=args.PopWord();
  }
  ///// print_mutation_data /////
  void Process(){
    m_world->GetStats().PrintMutationData(fname);
  }
};

///// print_mutation_rate_data /////

/**
Output (regular and log) statistics about individual copy
 mutation rates (aver, stdev, skew, cur).
 Useful only when mutation rate is set per organism.
 **/


class cEvent_print_mutation_rate_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_mutation_rate_data"; }
  const cString GetDescription() const { return "print_mutation_rate_data  [cString fname=\"mutation_rates.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="mutation_rates.dat"; else fname=args.PopWord();
  }
  ///// print_mutation_rate_data /////
  void Process(){
    m_world->GetStats().PrintMutationRateData(fname);
  }
};

///// print_divide_mut_data /////

/**
Output (regular and log) statistics about individual, per site,
 rates divide mutation rates (aver, stdev, skew, cur).
 Use with multiple divide instuction set.
 **/


class cEvent_print_divide_mut_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_divide_mut_data"; }
  const cString GetDescription() const { return "print_divide_mut_data  [cString fname=\"divide_mut.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="divide_mut.dat"; else fname=args.PopWord();
  }
  ///// print_divide_mut_data /////
  void Process(){
    m_world->GetStats().PrintDivideMutData(fname);
  }
};

///// print_dom_parasite_data /////

/**
* Output various quantities related to the dominant parasite.
 *
 * Parameters:
 * filename (string) default: parasite.dat
   *   The name of the data file.
   **/


class cEvent_print_dom_parasite_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_dom_parasite_data"; }
  const cString GetDescription() const { return "print_dom_parasite_data  [cString fname=\"parasite.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="parasite.dat"; else fname=args.PopWord();
  }
  ///// print_dom_parasite_data /////
  void Process(){
    m_world->GetStats().PrintDominantParaData(fname);
  }
};

///// print_instruction_data /////

/**
Sum of the by-organisms counts of what instructions they _successfully_
 execute beteween birth and divide. Prior to their first divide, organisms
 report values for their parents.
 **/


class cEvent_print_instruction_data : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_instruction_data"; }
  const cString GetDescription() const { return "print_instruction_data  [cString fname=\"instruction.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="instruction.dat"; else fname=args.PopWord();
  }
  ///// print_instruction_data /////
  void Process(){
    m_world->GetStats().PrintInstructionData(fname);
  }
};

///// print_instruction_abundance_histogram /////

/**
*
 * Appends a line containing the bulk count (abundance) of
 * each instruction in the population onto a file.
 *
 * Parameters:
 * filename (string) default: "instruction_histogram.dat"
   *
   **/


class cEvent_print_instruction_abundance_histogram : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_instruction_abundance_histogram"; }
  const cString GetDescription() const { return "print_instruction_abundance_histogram  [cString filename=\"instruction_histogram.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename="instruction_histogram.dat"; else filename=args.PopWord();
  }
  ///// print_instruction_abundance_histogram /////
  void Process(){
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    cAnalyzeUtil::PrintInstructionAbundanceHistogram(m_world, fp);
  }
};

///// print_depth_histogram /////

/**
**/


class cEvent_print_depth_histogram : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_depth_histogram"; }
  const cString GetDescription() const { return "print_depth_histogram  [cString filename=\"depth_histogram.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename="depth_histogram.dat"; else filename=args.PopWord();
  }
  ///// print_depth_histogram /////
  void Process(){
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    cAnalyzeUtil::PrintDepthHistogram(m_world, fp);
  }
};

///// print_genotype_abundance_histogram /////

/**
* Writes out a genotype abundance histogram.
 *
 * Parameters:
 * filename (string) default: genotype_abundance_histogram.dat
   *   The name of the file into which the histogram is written.
   **/


class cEvent_print_genotype_abundance_histogram : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_genotype_abundance_histogram"; }
  const cString GetDescription() const { return "print_genotype_abundance_histogram  [cString filename=\"genotype_abundance_histogram.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename="genotype_abundance_histogram.dat"; else filename=args.PopWord();
  }
  ///// print_genotype_abundance_histogram /////
  void Process(){
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    cAnalyzeUtil::PrintGenotypeAbundanceHistogram(m_world, fp);
  }
};

///// print_species_abundance_histogram /////

/**
* Writes out a species abundance histogram.
 *
 * Parameters:
 * filename (string) default: species_abundance_histogram.dat
   *   The name of the file into which the histogram is written.
   **/


class cEvent_print_species_abundance_histogram : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_species_abundance_histogram"; }
  const cString GetDescription() const { return "print_species_abundance_histogram  [cString filename=\"species_abundance_histogram.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename="species_abundance_histogram.dat"; else filename=args.PopWord();
  }
  ///// print_species_abundance_histogram /////
  void Process(){
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    cAnalyzeUtil::PrintSpeciesAbundanceHistogram(m_world, fp);
  }
};

///// print_lineage_totals /////

/**
**/


class cEvent_print_lineage_totals : public cEvent {
private:
  cString fname;
  int verbose;
public:
    const cString GetName() const { return "print_lineage_totals"; }
  const cString GetDescription() const { return "print_lineage_totals  [cString fname=\"lineage_totals.dat\"] [int verbose=1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="lineage_totals.dat"; else fname=args.PopWord();
    if (args == "") verbose=1; else verbose=args.PopWord().AsInt();
  }
  ///// print_lineage_totals /////
  void Process(){
    if (!m_world->GetConfig().LOG_LINEAGES.Get()) {
      m_world->GetDataFileOFStream(fname) << "No lineage data available!" << endl;
      return;
    }
    m_world->GetClassificationManager().PrintLineageTotals(fname, verbose);
  }
};

///// print_lineage_counts /////

/**
**/


class cEvent_print_lineage_counts : public cEvent {
private:
  cString fname;
  int verbose;
public:
    const cString GetName() const { return "print_lineage_counts"; }
  const cString GetDescription() const { return "print_lineage_counts  [cString fname=\"lineage_counts.dat\"] [int verbose=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="lineage_counts.dat"; else fname=args.PopWord();
    if (args == "") verbose=0; else verbose=args.PopWord().AsInt();
  }
  ///// print_lineage_counts /////
  void Process(){
    if (!m_world->GetConfig().LOG_LINEAGES.Get()) {
      m_world->GetDataFileOFStream(fname) << "No lineage data available!" << endl;
      return;
    }
    if (verbose) {    // verbose mode is the same in both methods
      m_world->GetClassificationManager().PrintLineageTotals(fname, verbose);
      return;
    }
    m_world->GetClassificationManager().PrintLineageCurCounts(fname);
  }
};

///// print_dom /////

/**
* Write the currently dominant genotype to disk.
 *
 * Parameters:
 * filename (string)
 *   The name under which the genotype should be saved. If no
 *   filename is given, the genotype is saved into the directory
 *   archive, under the name that the archive has associated with
 *   this genotype.
 **/


class cEvent_print_dom : public cEvent {
private:
  cString in_filename;
public:
  const cString GetName() const { return "print_dom"; }
  const cString GetDescription() const { return "print_dom  [cString in_filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") in_filename=""; else in_filename=args.PopWord();
  }
  ///// print_dom /////
  void Process(){
    cGenotype * dom = m_world->GetClassificationManager().GetBestGenotype();
    cString filename(in_filename);
    if (filename == "") filename.Set("archive/%s", dom->GetName()());
    cTestUtil::PrintGenome(m_world, dom->GetGenome(), filename, dom, m_world->GetStats().GetUpdate());
  }
};

///// parasite_debug /////

//midget


class cEvent_parasite_debug : public cEvent {
private:
  cString in_filename;
public:
  const cString GetName() const { return "parasite_debug"; }
  const cString GetDescription() const { return "parasite_debug  [cString in_filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") in_filename=""; else in_filename=args.PopWord();
  }
  ///// parasite_debug /////
  void Process(){
    m_world->GetPopulation().ParasiteDebug();
  }
};

///// print_dom_parasite /////

/**
* Write the currently dominant injected genotype to disk.
 *
 * Parameters:
 * filename (string)
 *   The name under which the genotype should be saved. If no
 *   filename is given, the genotype is saved into the directory
 *   archive, under the name that the archive has associated with
 *   this genotype.
 **/


class cEvent_print_dom_parasite : public cEvent {
private:
  cString in_filename;
public:
  const cString GetName() const { return "print_dom_parasite"; }
  const cString GetDescription() const { return "print_dom_parasite  [cString in_filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") in_filename=""; else in_filename=args.PopWord();
  }
  ///// print_dom_parasite /////
  void Process(){
    cInjectGenotype * dom = m_world->GetClassificationManager().GetBestInjectGenotype();
    if (dom!=NULL) {
      cString filename(in_filename);
      if (filename == "") filename.Set("archive/%s", dom->GetName()());
      cTestUtil::PrintGenome(m_world, dom, dom->GetGenome(), filename, m_world->GetStats().GetUpdate()); }
  }
};

///// print_genotype_map /////

/**
* write a matrix of genotype ID's to a file (matlab format)
 **/


class cEvent_print_genotype_map : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_genotype_map"; }
  const cString GetDescription() const { return "print_genotype_map  [cString fname=\"genotype_map.m\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="genotype_map.m"; else fname=args.PopWord();
  }
  ///// print_genotype_map /////
  void Process(){
    m_world->GetStats().PrintGenotypeMap(fname);
  }
};

///// print_number_phenotypes /////

/**
Output file with number of phenotypes based on tasks executed
 for this update.  Executing a task any numbers of times is considered
 the same as executing it once.
 **/


class cEvent_print_number_phenotypes : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_number_phenotypes"; }
  const cString GetDescription() const { return "print_number_phenotypes  [cString fname=\"phenotype_count.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="phenotype_count.dat"; else fname=args.PopWord();
  }
  ///// print_number_phenotypes /////
  void Process(){
    m_world->GetPopulation().PrintPhenotypeData(fname);
  }
};

///// print_phenotype_status /////

/**
Prints merit status for all the organisms in the population.
 Used for testing/debuging. 
 **/


class cEvent_print_phenotype_status : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "print_phenotype_status"; }
  const cString GetDescription() const { return "print_phenotype_status  [cString fname=\"phenotype_status.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="phenotype_status.dat"; else fname=args.PopWord();
  }
  ///// print_phenotype_status /////
  void Process(){
    m_world->GetPopulation().PrintPhenotypeStatus(fname);
  }
};

///// save_population /////

/**
* Saves the full state of the population.
 *
 * Parameters:
 * filename (string) default: save_pop.*
   *   The name of the file into which the population should
   *   be saved. If it is not given, then the name 'save_pop.*'
   *   is used, with '*' replaced by the current update.
   **/


class cEvent_save_population : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "save_population"; }
  const cString GetDescription() const { return "save_population  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// save_population /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("save_pop.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetPopulation().SavePopulation(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// load_population /////

/**
* Loads the full state of the population.
 *
 * Parameters:
 * filename (string)
 *   The name of the file to open.
 **/


class cEvent_load_population : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "load_population"; }
  const cString GetDescription() const { return "load_population  <cString fname>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    fname = args.PopWord();
  }
  ///// load_population /////
  void Process(){
    ifstream fp(fname());
    m_world->GetPopulation().LoadPopulation(fp);
  }
};

///// save_clone /////

/**
**/


class cEvent_save_clone : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "save_clone"; }
  const cString GetDescription() const { return "save_clone  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// save_clone /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("clone.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetPopulation().SaveClone(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// load_clone /////

/**
**/


class cEvent_load_clone : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "load_clone"; }
  const cString GetDescription() const { return "load_clone  <cString fname>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    fname = args.PopWord();
  }
  ///// load_clone /////
  void Process(){
    ifstream fp(fname());
    m_world->GetPopulation().LoadClone(fp);
  }
};

///// load_dump_file /////

/**
* Sets up a population based on a dump file such as written out by
 * detail_pop. It is also possible to append a history file to the dump
 * file, in order to preserve the history of a previous run.
 **/


class cEvent_load_dump_file : public cEvent {
private:
  cString fname;
  int update;
public:
    const cString GetName() const { return "load_dump_file"; }
  const cString GetDescription() const { return "load_dump_file  <cString fname> [int update=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    fname = args.PopWord();
    if (args == "") update=-1; else update=args.PopWord().AsInt();
  }
  ///// load_dump_file /////
  void Process(){
    m_world->GetPopulation().LoadDumpFile(fname, update);
  }
};

///// dump_pop /////

/**
* Writes out a line of data for each genotype in the current population. The
 * line contains the genome as string, the number of organisms of that genotype,
 * and the genotype ID.
 *
 * Parameters:
 * filename (string) default: "dump.<update>"
   *   The name of the file into which the population dump should be written.
   **/


class cEvent_dump_pop : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "dump_pop"; }
  const cString GetDescription() const { return "dump_pop  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// dump_pop /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("dump.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetClassificationManager().DumpTextSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// print_genotypes /////

/**
* This is a new version of "detail_pop" or "historic_dump".  It allows you to
 * output one line per genotype in memory where you get to choose what data
 * should be included.
 *
 * Parameters
 * data_fields (string)
 *   This must be a comma separated string of all data you wish to output.
 *   Options include: id, parent_id, parent2_id (for sex), parent_dist,
 *       num_cpus, total_cpus, length, merit, gest_time, fitness, update_born,
 *       update_dead, depth, lineage, sequence
 * historic (int) default: 0
   *   How many updates back of history should we include (-1 = all)
   * filename (string) default: "genotypes-<update>.dat"
     *   The name of the file into which the population dump should be written.
     **/


class cEvent_print_genotypes : public cEvent {
private:
  cString data_fields;
  int historic;
  cString fname;
public:
    const cString GetName() const { return "print_genotypes"; }
  const cString GetDescription() const { return "print_genotypes  [cString data_fields=\"all\"] [int historic=0] [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") data_fields="all"; else data_fields=args.PopWord();
    if (args == "") historic=0; else historic=args.PopWord().AsInt();
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// print_genotypes /////
  void Process(){
    cString filename = fname;
    if (filename == "") {
      filename.Set("genotypes-%d.dat", m_world->GetStats().GetUpdate());
    }
    m_world->GetClassificationManager().PrintGenotypes(m_world->GetDataFileOFStream(filename),
                                                          data_fields, historic);
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// detail_pop /////

/**
* Like dump_pop, but more detailed data is written out.
 *
 * Parameters:
 * filename (string) default: "detail_pop.<update>"
   *   The name of the file into which the population dump should be written.
   **/


class cEvent_detail_pop : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "detail_pop"; }
  const cString GetDescription() const { return "detail_pop  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// detail_pop /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("detail_pop.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetClassificationManager().DumpDetailedSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// detail_sex_pop /////

/**
* Like detail_pop, but for sexual populations. 
 * Info for both parents is writen out.
 *
 * Parameters:
 * filename (string) default: "detail_pop.<update>"
   *   The name of the file into which the population dump should be written.
   **/


class cEvent_detail_sex_pop : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "detail_sex_pop"; }
  const cString GetDescription() const { return "detail_sex_pop  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// detail_sex_pop /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("detail_pop.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetClassificationManager().DumpDetailedSexSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// detail_parasite_pop /////

/**
* Like dump_pop, but more detailed data is written out.
 *
 * Parameters:
 * filename (string) default: "detail_pop.<update>"
   *   The name of the file into which the population dump should be written.
   **/


class cEvent_detail_parasite_pop : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "detail_parasite_pop"; }
  const cString GetDescription() const { return "detail_parasite_pop  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// detail_parasite_pop /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("detail_parasite_pop.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetClassificationManager().DumpInjectDetailedSummary(filename, m_world->GetStats().GetUpdate());
  }
};

///// dump_historic_pop /////

/**
* Similar to detail_pop. However, only genotypes that are not in the
 * current population anymore are included. Genotypes that are not in
 * the line of descent of any of the current genotypes to the ultimate
 * ancestor are excluded.
 *
 * Parameters:
 * back_dist (int) default: -1
   *   How many updates back should we print?  -1 goes forever.  Use the
   *   distance to the last dump historic if you only want a "diff".
     * filename (string) default: "historic_dump.<update>"
       *   The name of the file into which the historic dump should be written.
       **/


class cEvent_dump_historic_pop : public cEvent {
private:
  int back_dist;
  cString fname;
public:
    const cString GetName() const { return "dump_historic_pop"; }
  const cString GetDescription() const { return "dump_historic_pop  [int back_dist=-1] [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") back_dist=-1; else back_dist=args.PopWord().AsInt();
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// dump_historic_pop /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("historic_dump.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetClassificationManager().DumpHistoricSummary(m_world->GetDataFileOFStream(filename), back_dist);
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_historic_sex_pop /////

/**
* Similar to dump_historic_pop, but for sexual populations. 
 * ID of both parents is writen out. 
 *
 * Parameters:
 * filename (string) default: "historic_dump.<update>"
   *   The name of the file into which the historic dump should be written.
   **/


class cEvent_dump_historic_sex_pop : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "dump_historic_sex_pop"; }
  const cString GetDescription() const { return "dump_historic_sex_pop  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// dump_historic_sex_pop /////
  void Process(){
    cString filename;
    if( fname == "" ){
      filename.Set("historic_dump.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetClassificationManager().DumpHistoricSexSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_memory /////

/**
* Dump the current memory state of all CPUs to a file.
 **/


class cEvent_dump_memory : public cEvent {
private:
  cString fname;
public:
  const cString GetName() const { return "dump_memory"; }
  const cString GetDescription() const { return "dump_memory  [cString fname=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname=""; else fname=args.PopWord();
  }
  ///// dump_memory /////
  void Process(){
    cString filename;
    if (fname == "") {
      filename.Set("memory_dump.%d", m_world->GetStats().GetUpdate());
    }
    m_world->GetPopulation().DumpMemorySummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// inject /////

/**
* Injects a single organism into the population.
 *
 * Parameters:
 * filename (string)
 *   The filename of the genotype to load. If this is left empty, or the keyword
 *   "START_CREATURE" is given, than the genotype specified in the genesis
 *   file under "START_CREATURE" is used.
 * cell ID (integer) default: 0
   *   The grid-point into which the organism should be placed.
   * merit (double) default: -1
     *   The initial merit of the organism. If set to -1, this is ignored.
     * lineage label (integer) default: 0
       *   An integer that marks all descendants of this organism.
       * neutral metric (double) default: 0
         *   A double value that randomly drifts over time.
         **/


class cEvent_inject : public cEvent {
private:
  cString fname;
  int cell_id;
  double merit;
  int lineage_label;
  double neutral_metric;
public:
    const cString GetName() const { return "inject"; }
  const cString GetDescription() const { return "inject  [cString fname=\"START_CREATURE\"] [int cell_id=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="START_CREATURE"; else fname=args.PopWord();
    if (args == "") cell_id=0; else cell_id=args.PopWord().AsInt();
    if (args == "") merit=-1; else merit=args.PopWord().AsDouble();
    if (args == "") lineage_label=0; else lineage_label=args.PopWord().AsInt();
    if (args == "") neutral_metric=0; else neutral_metric=args.PopWord().AsDouble();
  }
  ///// inject /////
  void Process(){
    if (fname == "START_CREATURE") fname = m_world->GetConfig().START_CREATURE.Get();
    cGenome genome =
      cInstUtil::LoadGenome(fname, m_world->GetHardwareManager().GetInstSet());
    m_world->GetPopulation().Inject(genome, cell_id, merit, lineage_label, neutral_metric);
  }
};

///// inject_all /////

/**
* Injects identical organisms into all cells of the population.
 *
 * Parameters:
 * filename (string)
 *   The filename of the genotype to load. If this is left empty, or the keyword
 *   "START_CREATURE" is given, than the genotype specified in the genesis
 *   file under "START_CREATURE" is used.
 * merit (double) default: -1
   *   The initial merit of the organism. If set to -1, this is ignored.
   * lineage label (integer) default: 0
     *   An integer that marks all descendants of this organism.
     * neutral metric (double) default: 0
       *   A double value that randomly drifts over time.
       **/


class cEvent_inject_all : public cEvent {
private:
  cString fname;
  double merit;
  int lineage_label;
  double neutral_metric;
public:
    const cString GetName() const { return "inject_all"; }
  const cString GetDescription() const { return "inject_all  [cString fname=\"START_CREATURE\"] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="START_CREATURE"; else fname=args.PopWord();
    if (args == "") merit=-1; else merit=args.PopWord().AsDouble();
    if (args == "") lineage_label=0; else lineage_label=args.PopWord().AsInt();
    if (args == "") neutral_metric=0; else neutral_metric=args.PopWord().AsDouble();
  }
  ///// inject_all /////
  void Process(){
    if (fname == "START_CREATURE") fname = m_world->GetConfig().START_CREATURE.Get();
    cGenome genome =
      cInstUtil::LoadGenome(fname, m_world->GetHardwareManager().GetInstSet());
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      m_world->GetPopulation().Inject(genome, i, merit, lineage_label, neutral_metric);
    }
    m_world->GetPopulation().SetSyncEvents(true);
  }
};

///// inject_range /////

/**
* Injects identical organisms into a range of cells of the population.
 *
 * Parameters:
 * filename (string)
 *   The filename of the genotype to load. If this is left empty, or the keyword
 *   "START_CREATURE" is given, than the genotype specified in the genesis
 *   file under "START_CREATURE" is used.
 * start_cell (int)
 *   First cell to inject into.
 * stop_cell (int)
 *   First cell *not* to inject into.
 * merit (double) default: -1
   *   The initial merit of the organism. If set to -1, this is ignored.
   * lineage label (integer) default: 0
     *   An integer that marks all descendants of this organism.
     * neutral metric (double) default: 0
       *   A double value that randomly drifts over time.
       *
       * Example:
       *   inject_range creature.gen 0 10
       *
       * Will inject 10 organisms into cells 0 through 9.
       **/


class cEvent_inject_range : public cEvent {
private:
  cString fname;
  int start_cell;
  int end_cell;
  double merit;
  int lineage_label;
  double neutral_metric;
public:
    const cString GetName() const { return "inject_range"; }
  const cString GetDescription() const { return "inject_range  [cString fname=\"START_CREATURE\"] [int start_cell=0] [int end_cell=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="START_CREATURE"; else fname=args.PopWord();
    if (args == "") start_cell=0; else start_cell=args.PopWord().AsInt();
    if (args == "") end_cell=-1; else end_cell=args.PopWord().AsInt();
    if (args == "") merit=-1; else merit=args.PopWord().AsDouble();
    if (args == "") lineage_label=0; else lineage_label=args.PopWord().AsInt();
    if (args == "") neutral_metric=0; else neutral_metric=args.PopWord().AsDouble();
  }
  ///// inject_range /////
  void Process(){
    if (fname == "START_CREATURE") fname = m_world->GetConfig().START_CREATURE.Get();
    if (end_cell == -1) end_cell = start_cell + 1;
    if (start_cell < 0 ||
        end_cell > m_world->GetPopulation().GetSize() ||
        start_cell >= end_cell) {
      m_world->GetDriver().NotifyWarning("inject_range has invalid range!");
    }
    else {
      cGenome genome =
      cInstUtil::LoadGenome(fname, m_world->GetHardwareManager().GetInstSet());
      for (int i = start_cell; i < end_cell; i++) {
        m_world->GetPopulation().Inject(genome, i, merit, lineage_label, neutral_metric);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};

///// inject_sequence /////

/**
* Injects identical organisms into a range of cells of the population.
 *
 * Parameters:
 * sequence (string)
 *   The genome sequence for this organism.  This is a mandatory argument.
 * start_cell (int)
 *   First cell to inject into.
 * stop_cell (int)
 *   First cell *not* to inject into.
 * merit (double) default: -1
   *   The initial merit of the organism. If set to -1, this is ignored.
   * lineage label (integer) default: 0
     *   An integer that marks all descendants of this organism.
     * neutral metric (double) default: 0
       *   A double value that randomly drifts over time.
       *
       * Example:
       *   inject_range ckdfhgklsahnfsaggdsgajfg 0 10 100
       *
       * Will inject 10 organisms into cells 0 through 9 with a merit of 100.
       **/


class cEvent_inject_sequence : public cEvent {
private:
  cString seq;
  int start_cell;
  int end_cell;
  double merit;
  int lineage_label;
  double neutral_metric;
public:
    const cString GetName() const { return "inject_sequence"; }
  const cString GetDescription() const { return "inject_sequence  <cString seq> [int start_cell=0] [int end_cell=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    seq = args.PopWord();
    if (args == "") start_cell=0; else start_cell=args.PopWord().AsInt();
    if (args == "") end_cell=-1; else end_cell=args.PopWord().AsInt();
    if (args == "") merit=-1; else merit=args.PopWord().AsDouble();
    if (args == "") lineage_label=0; else lineage_label=args.PopWord().AsInt();
    if (args == "") neutral_metric=0; else neutral_metric=args.PopWord().AsDouble();
  }
  ///// inject_sequence /////
  void Process(){
    if (end_cell == -1) end_cell = start_cell + 1;
    if (start_cell < 0 ||
        end_cell > m_world->GetPopulation().GetSize() ||
        start_cell >= end_cell) {
      m_world->GetDriver().NotifyWarning("inject_sequence has invalid range!");
    }
    else {
      cGenome genome(seq);
      for (int i = start_cell; i < end_cell; i++) {
        m_world->GetPopulation().Inject(genome, i, merit, lineage_label, neutral_metric);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};

///// inject_random /////

/**
* Injects a randomly generated genome into the population.
 *
 * Parameters:
 * length (integer) [required]
 *   Number of instructions in the randomly generated genome.
 * cell ID (integer) default: -1
   *   The grid-point into which the genome should be placed.  Default is random.
   * merit (double) default: -1
     *   The initial merit of the organism. If set to -1, this is ignored.
     * lineage label (integer) default: 0
       *   An integer that marks all descendants of this organism.
       * neutral metric (double) default: 0
         *   A double value that randomly drifts over time.
         **/


class cEvent_inject_random : public cEvent {
private:
  int length;
  int cell_id;
  double merit;
  int lineage_label;
  double neutral_metric;
public:
    const cString GetName() const { return "inject_random"; }
  const cString GetDescription() const { return "inject_random  <int length> [int cell_id=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    length = args.PopWord().AsInt();
    if (args == "") cell_id=-1; else cell_id=args.PopWord().AsInt();
    if (args == "") merit=-1; else merit=args.PopWord().AsDouble();
    if (args == "") lineage_label=0; else lineage_label=args.PopWord().AsInt();
    if (args == "") neutral_metric=0; else neutral_metric=args.PopWord().AsDouble();
  }
  ///// inject_random /////
  void Process(){
    if (cell_id == -1) cell_id = m_world->GetRandom().GetUInt(m_world->GetPopulation().GetSize());
    cGenome genome =
      cInstUtil::RandomGenome(length, m_world->GetHardwareManager().GetInstSet());
    m_world->GetPopulation().Inject(genome, cell_id, merit, lineage_label, neutral_metric);
  }
};

///// inject_range_parasite /////

/**
* Injects identical organisms into a range of cells of the population.
 *
 * Parameters:
 * filename (string)
 *   The filename of the genotype to load. If this is left empty, or the keyword
 *   "START_CREATURE" is given, than the genotype specified in the genesis
 *   file under "START_CREATURE" is used.
 * start_cell (int)
 *   First cell to inject into.
 * stop_cell (int)
 *   First cell *not* to inject into.
 * merit (double) default: -1
   *   The initial merit of the organism. If set to -1, this is ignored.
   * lineage label (integer) default: 0
     *   An integer that marks all descendants of this organism.
     * neutral metric (double) default: 0
       *   A double value that randomly drifts over time.
       *
       * Example:
       *   inject_range creature.gen 0 10
       *
       * Will inject 10 organisms into cells 0 through 9.
       **/


class cEvent_inject_range_parasite : public cEvent {
private:
  cString fname_parasite;
  int start_cell;
  int end_cell;
  double merit;
  int lineage_label;
  double neutral_metric;
  int mem_space;
public:
    const cString GetName() const { return "inject_range_parasite"; }
  const cString GetDescription() const { return "inject_range_parasite  [cString fname_parasite=\"organism.parasite\"] [int start_cell=0] [int end_cell=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0] [int mem_space=2]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname_parasite="organism.parasite"; else fname_parasite=args.PopWord();
    if (args == "") start_cell=0; else start_cell=args.PopWord().AsInt();
    if (args == "") end_cell=-1; else end_cell=args.PopWord().AsInt();
    if (args == "") merit=-1; else merit=args.PopWord().AsDouble();
    if (args == "") lineage_label=0; else lineage_label=args.PopWord().AsInt();
    if (args == "") neutral_metric=0; else neutral_metric=args.PopWord().AsDouble();
    if (args == "") mem_space=2; else mem_space=args.PopWord().AsInt();
  }
  ///// inject_range_parasite /////
  void Process(){
    if (fname_parasite == "START_CREATURE") fname_parasite = m_world->GetConfig().START_CREATURE.Get();
    if (end_cell == -1) end_cell = start_cell + 1;
    if (start_cell < 0 ||
        end_cell > m_world->GetPopulation().GetSize() ||
        start_cell >= end_cell) {
      m_world->GetDriver().NotifyWarning("inject_range has invalid range!");
    }
    else {
      cGenome genome_parasite =
      cInstUtil::LoadGenome(fname_parasite, m_world->GetHardwareManager().GetInstSet());
      for (int i = start_cell; i < end_cell; i++) {
        m_world->GetPopulation().Inject(genome_parasite, i, merit, lineage_label, neutral_metric, mem_space);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};

///// inject_range_pair /////

/**
* Injects identical organisms into a range of cells of the population.
 *
 * Parameters:
 * filename (string)
 *   The filename of the genotype to load. If this is left empty, or the keyword
 *   "START_CREATURE" is given, than the genotype specified in the genesis
 *   file under "START_CREATURE" is used.
 * start_cell (int)
 *   First cell to inject into.
 * stop_cell (int)
 *   First cell *not* to inject into.
 * merit (double) default: -1
   *   The initial merit of the organism. If set to -1, this is ignored.
   * lineage label (integer) default: 0
     *   An integer that marks all descendants of this organism.
     * neutral metric (double) default: 0
       *   A double value that randomly drifts over time.
       *
       * Example:
       *   inject_range creature.gen 0 10
       *
       * Will inject 10 organisms into cells 0 through 9.
       **/


class cEvent_inject_range_pair : public cEvent {
private:
  cString fname;
  cString fname_parasite;
  int start_cell;
  int end_cell;
  double merit;
  int lineage_label;
  double neutral_metric;
  int mem_space;
public:
    const cString GetName() const { return "inject_range_pair"; }
  const cString GetDescription() const { return "inject_range_pair  [cString fname=\"START_CREATURE\"] [cString fname_parasite=\"organism.parasite\"] [int start_cell=0] [int end_cell=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0] [int mem_space=2]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") fname="START_CREATURE"; else fname=args.PopWord();
    if (args == "") fname_parasite="organism.parasite"; else fname_parasite=args.PopWord();
    if (args == "") start_cell=0; else start_cell=args.PopWord().AsInt();
    if (args == "") end_cell=-1; else end_cell=args.PopWord().AsInt();
    if (args == "") merit=-1; else merit=args.PopWord().AsDouble();
    if (args == "") lineage_label=0; else lineage_label=args.PopWord().AsInt();
    if (args == "") neutral_metric=0; else neutral_metric=args.PopWord().AsDouble();
    if (args == "") mem_space=2; else mem_space=args.PopWord().AsInt();
  }
  ///// inject_range_pair /////
  void Process(){
    if (fname == "START_CREATURE") fname = m_world->GetConfig().START_CREATURE.Get();
    if (end_cell == -1) end_cell = start_cell + 1;
    if (start_cell < 0 ||
        end_cell > m_world->GetPopulation().GetSize() ||
        start_cell >= end_cell) {
      m_world->GetDriver().NotifyWarning("inject_range has invalid range!");
    }
    else {
      cGenome genome =
      cInstUtil::LoadGenome(fname, m_world->GetHardwareManager().GetInstSet());
      cGenome genome_parasite =
        cInstUtil::LoadGenome(fname_parasite, m_world->GetHardwareManager().GetInstSet());
      for (int i = start_cell; i < end_cell; i++) {
        m_world->GetPopulation().Inject(genome, i, merit, lineage_label, neutral_metric);
        m_world->GetPopulation().Inject(genome_parasite, i, merit, lineage_label, neutral_metric, mem_space);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};

///// zero_muts /////

/**
* This event will set all mutation rates to zero...
 **/


class cEvent_zero_muts : public cEvent {
private:
public:
  const cString GetName() const { return "zero_muts"; }
  const cString GetDescription() const { return "zero_muts"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// zero_muts /////
  void Process(){
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      m_world->GetPopulation().GetCell(i).MutationRates().Clear();
    }
  }
};

///// mod_copy_mut /////

/**
**/


class cEvent_mod_copy_mut : public cEvent {
private:
  double cmut_inc;
  int cell;
public:
    const cString GetName() const { return "mod_copy_mut"; }
  const cString GetDescription() const { return "mod_copy_mut  <double cmut_inc> [int cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    cmut_inc = args.PopWord().AsDouble();
    if (args == "") cell=-1; else cell=args.PopWord().AsInt();
  }
  ///// mod_copy_mut /////
  void Process(){
    const double new_cmut = m_world->GetConfig().COPY_MUT_PROB.Get() + cmut_inc;
    if (cell < 0) {   // cell == -1  -->  all
      for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
        m_world->GetPopulation().GetCell(i).MutationRates().SetCopyMutProb(new_cmut);
      }
      m_world->GetConfig().COPY_MUT_PROB.Set(new_cmut);
    } else {
      m_world->GetPopulation().GetCell(cell).MutationRates().SetCopyMutProb(new_cmut);
    }
  }
};

///// mod_div_mut /////

/**
**/


class cEvent_mod_div_mut : public cEvent {
private:
  double dmut_inc;
  int cell;
public:
    const cString GetName() const { return "mod_div_mut"; }
  const cString GetDescription() const { return "mod_div_mut  <double dmut_inc> [int cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    dmut_inc = args.PopWord().AsDouble();
    if (args == "") cell=-1; else cell=args.PopWord().AsInt();
  }
  ///// mod_div_mut /////
  void Process(){
    const double new_div_mut = m_world->GetConfig().DIV_MUT_PROB.Get() + dmut_inc;
    if (cell < 0) {   // cell == -1  -->  all
      for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
        m_world->GetPopulation().GetCell(i).MutationRates().SetDivMutProb(new_div_mut);
      }
      m_world->GetConfig().DIV_MUT_PROB.Set(new_div_mut);
    } else {
      m_world->GetPopulation().GetCell(cell).MutationRates().SetDivMutProb(new_div_mut);
    }
  }
};

///// set_copy_mut /////

/**
**/


class cEvent_set_copy_mut : public cEvent {
private:
  double cmut;
  int start_cell;
  int end_cell;
public:
    const cString GetName() const { return "set_copy_mut"; }
  const cString GetDescription() const { return "set_copy_mut  <double cmut> [int start_cell=-1] [int end_cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    cmut = args.PopWord().AsDouble();
    if (args == "") start_cell=-1; else start_cell=args.PopWord().AsInt();
    if (args == "") end_cell=-1; else end_cell=args.PopWord().AsInt();
  }
  ///// set_copy_mut /////
  void Process(){
    if (start_cell < 0) {   // start_cell == -1  -->  all
      m_world->GetConfig().COPY_MUT_PROB.Set(cmut);
      start_cell = 0;
      end_cell = m_world->GetPopulation().GetSize();
    }
    else if (end_cell < -1)  { // end_cell == -1 --> Only one cell!
      end_cell = start_cell + 1;
    }
    assert(start_cell >= 0 && start_cell < m_world->GetPopulation().GetSize());
    assert(end_cell > 0 && end_cell <= m_world->GetPopulation().GetSize());
    for (int i = start_cell; i < end_cell; i++) {
      m_world->GetPopulation().GetCell(i).MutationRates().SetCopyMutProb(cmut);
    }
  }
};

///// mod_point_mut /////

/**
**/


class cEvent_mod_point_mut : public cEvent {
private:
  double pmut_inc;
  int cell;
public:
    const cString GetName() const { return "mod_point_mut"; }
  const cString GetDescription() const { return "mod_point_mut  <double pmut_inc> [int cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    pmut_inc = args.PopWord().AsDouble();
    if (args == "") cell=-1; else cell=args.PopWord().AsInt();
  }
  ///// mod_point_mut /////
  void Process(){
    const double new_pmut = m_world->GetConfig().POINT_MUT_PROB.Get() + pmut_inc;
    if (cell < 0) {   // cell == -1   -->  all
      for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
        m_world->GetPopulation().GetCell(i).MutationRates().SetPointMutProb(new_pmut);
      }
      m_world->GetConfig().POINT_MUT_PROB.Set(new_pmut);
    } else {
      m_world->GetPopulation().GetCell(cell).MutationRates().SetPointMutProb(new_pmut);
    }
  }
};

///// set_point_mut /////

/**
**/


class cEvent_set_point_mut : public cEvent {
private:
  double pmut;
  int cell;
public:
    const cString GetName() const { return "set_point_mut"; }
  const cString GetDescription() const { return "set_point_mut  <double pmut> [int cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    pmut = args.PopWord().AsDouble();
    if (args == "") cell=-1; else cell=args.PopWord().AsInt();
  }
  ///// set_point_mut /////
  void Process(){
    if (cell < 0) {   // cell == -1   -->  all
      for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
        m_world->GetPopulation().GetCell(i).MutationRates().SetPointMutProb(pmut);
      }
      m_world->GetConfig().POINT_MUT_PROB.Set(pmut);
    } else {
      m_world->GetPopulation().GetCell(cell).MutationRates().SetPointMutProb(pmut);
    }
  }
};

///// calc_landscape /////

/**
**/


class cEvent_calc_landscape : public cEvent {
private:
  int landscape_dist;
public:
  const cString GetName() const { return "calc_landscape"; }
  const cString GetDescription() const { return "calc_landscape  [int landscape_dist=1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") landscape_dist=1; else landscape_dist=args.PopWord().AsInt();
  }
  ///// calc_landscape /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cAnalyzeUtil::CalcLandscape(m_world, landscape_dist, genome,
                                m_world->GetHardwareManager().GetInstSet());
  }
};

///// predict_w_landscape /////

/**
**/


class cEvent_predict_w_landscape : public cEvent {
private:
  cString datafile;
public:
  const cString GetName() const { return "predict_w_landscape"; }
  const cString GetDescription() const { return "predict_w_landscape  [cString datafile=\"land-predict.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") datafile="land-predict.dat"; else datafile=args.PopWord();
  }
  ///// predict_w_landscape /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.PredictWProcess(m_world->GetDataFileOFStream(datafile));
  }
};

///// predict_nu_landscape /////

/**
**/


class cEvent_predict_nu_landscape : public cEvent {
private:
  cString datafile;
public:
  const cString GetName() const { return "predict_nu_landscape"; }
  const cString GetDescription() const { return "predict_nu_landscape  [cString datafile=\"land-predict.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") datafile="land-predict.dat"; else datafile=args.PopWord();
  }
  ///// predict_nu_landscape /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.PredictNuProcess(m_world->GetDataFileOFStream(datafile));
  }
};

///// sample_landscape /////

/**
**/


class cEvent_sample_landscape : public cEvent {
private:
  int sample_size;
public:
  const cString GetName() const { return "sample_landscape"; }
  const cString GetDescription() const { return "sample_landscape  [int sample_size=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") sample_size=0; else sample_size=args.PopWord().AsInt();
  }
  ///// sample_landscape /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    if (sample_size == 0) sample_size = m_world->GetHardwareManager().GetInstSet().GetSize() - 1;
    landscape.SampleProcess(sample_size);
    landscape.PrintStats(m_world->GetDataFileOFStream("land-sample.dat"), m_world->GetStats().GetUpdate());
  }
};

///// random_landscape /////

/**
**/


class cEvent_random_landscape : public cEvent {
private:
  int landscape_dist;
  int sample_size;
  int min_found;
  int max_sample_size;
  bool print_if_found;
public:
    const cString GetName() const { return "random_landscape"; }
  const cString GetDescription() const { return "random_landscape  [int landscape_dist=1] [int sample_size=0] [int min_found=0] [int max_sample_size=0] [bool print_if_found=false]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") landscape_dist=1; else landscape_dist=args.PopWord().AsInt();
    if (args == "") sample_size=0; else sample_size=args.PopWord().AsInt();
    if (args == "") min_found=0; else min_found=args.PopWord().AsInt();
    if (args == "") max_sample_size=0; else max_sample_size=args.PopWord().AsInt();
    if (args == "") print_if_found=false; else print_if_found=args.PopWord();
  }
  ///// random_landscape /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.RandomProcess(sample_size, landscape_dist, min_found,
                            max_sample_size, print_if_found);
    landscape.PrintStats(m_world->GetDataFileOFStream("land-random.dat"), m_world->GetStats().GetUpdate());
  }
};

///// analyze_landscape /////

/**
**/


class cEvent_analyze_landscape : public cEvent {
private:
  int sample_size;
  int min_found;
  int max_sample_size;
public:
    const cString GetName() const { return "analyze_landscape"; }
  const cString GetDescription() const { return "analyze_landscape  [int sample_size=1000] [int min_found=0] [int max_sample_size=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") sample_size=1000; else sample_size=args.PopWord().AsInt();
    if (args == "") min_found=0; else min_found=args.PopWord().AsInt();
    if (args == "") max_sample_size=0; else max_sample_size=args.PopWord().AsInt();
  }
  ///// analyze_landscape /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cAnalyzeUtil::AnalyzeLandscape(m_world, genome, m_world->GetHardwareManager().GetInstSet(),
                     sample_size, min_found, max_sample_size,
                     m_world->GetStats().GetUpdate());
  }
};

///// pairtest_landscape /////

/**
* If sample_size = 0, pairtest the full landscape.
 **/


class cEvent_pairtest_landscape : public cEvent {
private:
  int sample_size;
public:
  const cString GetName() const { return "pairtest_landscape"; }
  const cString GetDescription() const { return "pairtest_landscape  [int sample_size=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") sample_size=0; else sample_size=args.PopWord().AsInt();
  }
  ///// pairtest_landscape /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cAnalyzeUtil::PairTestLandscape(m_world, genome, m_world->GetHardwareManager().GetInstSet(),
                                    sample_size, m_world->GetStats().GetUpdate());
  }
};

///// test_dom /////

/**
**/


class cEvent_test_dom : public cEvent {
private:
public:
  const cString GetName() const { return "test_dom"; }
  const cString GetDescription() const { return "test_dom"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args; }
  ///// test_dom /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cAnalyzeUtil::TestGenome(m_world, genome, m_world->GetHardwareManager().GetInstSet(),
                             m_world->GetDataFileOFStream("dom-test.dat"), m_world->GetStats().GetUpdate());
  }
};

///// analyze_population /////

/**
**/


class cEvent_analyze_population : public cEvent {
private:
  double sample_prob;
  int landscape;
  int save_genotype;
  cString filename;
public:
    const cString GetName() const { return "analyze_population"; }
  const cString GetDescription() const { return "analyze_population  [double sample_prob=1] [int landscape=0] [int save_genotype=0] [cString filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") sample_prob=1; else sample_prob=args.PopWord().AsDouble();
    if (args == "") landscape=0; else landscape=args.PopWord().AsInt();
    if (args == "") save_genotype=0; else save_genotype=args.PopWord().AsInt();
    if (args == "") filename=""; else filename=args.PopWord();
  }
  ///// analyze_population /////
  void Process(){
    if (filename == "") filename.Set("population_info_%d.dat",m_world->GetStats().GetUpdate());
    cAnalyzeUtil::AnalyzePopulation(m_world, m_world->GetDataFileOFStream(filename), sample_prob,
                                    landscape, save_genotype);
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// print_detailed_fitness_data /////

/**
**/


class cEvent_print_detailed_fitness_data : public cEvent {
private:
  int save_max_f_genotype;
  int print_fitness_histo;
  double hist_fmax;
  double hist_fstep;
  cString filename;
  cString filename2;
  cString filename3;
public:
    const cString GetName() const { return "print_detailed_fitness_data"; }
  const cString GetDescription() const { return "print_detailed_fitness_data  [int save_max_f_genotype=0] [int print_fitness_histo=0] [double hist_fmax=1] [double hist_fstep=0.1] [cString filename=\"fitness.dat\"] [cString filename2=\"fitness_histos.dat\"] [cString filename3=\"fitness_histos_testCPU.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") save_max_f_genotype=0; else save_max_f_genotype=args.PopWord().AsInt();
    if (args == "") print_fitness_histo=0; else print_fitness_histo=args.PopWord().AsInt();
    if (args == "") hist_fmax=1; else hist_fmax=args.PopWord().AsDouble();
    if (args == "") hist_fstep=0.1; else hist_fstep=args.PopWord().AsDouble();
    if (args == "") filename="fitness.dat"; else filename=args.PopWord();
    if (args == "") filename2="fitness_histos.dat"; else filename2=args.PopWord();
    if (args == "") filename3="fitness_histos_testCPU.dat"; else filename3=args.PopWord();
  }
  ///// print_detailed_fitness_data /////
  void Process(){
    cAnalyzeUtil::PrintDetailedFitnessData(m_world, filename, filename2, filename3, save_max_f_genotype, print_fitness_histo, hist_fmax, hist_fstep );
  }
};

///// print_genetic_distance_data /////

/**
**/


class cEvent_print_genetic_distance_data : public cEvent {
private:
  cString creature_name;
  cString filename;
public:
    const cString GetName() const { return "print_genetic_distance_data"; }
  const cString GetDescription() const { return "print_genetic_distance_data  [cString creature_name=\"\"] [cString filename=\"genetic_distance.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") creature_name=""; else creature_name=args.PopWord();
    if (args == "") filename="genetic_distance.dat"; else filename=args.PopWord();
  }
  ///// print_genetic_distance_data /////
  void Process(){
    if( creature_name == "" || creature_name == "START_CREATURE" )
      creature_name = m_world->GetConfig().START_CREATURE.Get();
    cAnalyzeUtil::PrintGeneticDistanceData(m_world, m_world->GetDataFileOFStream(filename), creature_name);
  }
};

///// genetic_distance_pop_dump /////

/**
**/


class cEvent_genetic_distance_pop_dump : public cEvent {
private:
  cString creature_name;
  cString filename;
  int save_genotype;
public:
    const cString GetName() const { return "genetic_distance_pop_dump"; }
  const cString GetDescription() const { return "genetic_distance_pop_dump  [cString creature_name=\"\"] [cString filename=\"\"] [int save_genotype=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") creature_name=""; else creature_name=args.PopWord();
    if (args == "") filename=""; else filename=args.PopWord();
    if (args == "") save_genotype=0; else save_genotype=args.PopWord().AsInt();
  }
  ///// genetic_distance_pop_dump /////
  void Process() {
    if (creature_name == "" || creature_name == "START_CREATURE")
      creature_name = m_world->GetConfig().START_CREATURE.Get();
    if (filename == "" || filename == "AUTO")
      filename.Set("pop_dump_%d.dat",m_world->GetStats().GetUpdate());
    cAnalyzeUtil::GeneticDistancePopDump(m_world, m_world->GetDataFileOFStream(filename),
                                         creature_name, save_genotype);
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// task_snapshot /////

/**
**/


class cEvent_task_snapshot : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "task_snapshot"; }
  const cString GetDescription() const { return "task_snapshot  [cString filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename = "";
    else filename = args.PopWord();
  }
  ///// task_snapshot /////
  void Process(){
    if (filename == "") filename.Set("tasks_%d.dat",m_world->GetStats().GetUpdate());
    cAnalyzeUtil::TaskSnapshot(m_world, m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// print_viable_tasks_data /////

/**
**/


class cEvent_print_viable_tasks_data : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_viable_tasks_data"; }
  const cString GetDescription() const { return "print_viable_tasks_data  [cString filename=\"viable_tasks.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename="viable_tasks.dat"; else filename=args.PopWord();
  }
  ///// print_viable_tasks_data /////
  void Process(){
    cAnalyzeUtil::PrintViableTasksData(m_world, m_world->GetDataFileOFStream(filename));
  }
};

///// apocalypse /////

/**
* Randomly removes a certain proportion of the population.
 *
 * Parameters:
 * removal probability (double) default: 0.9
   *   The probability with which a single organism is removed.
   **/


class cEvent_apocalypse : public cEvent {
private:
  double kill_prob;
public:
  const cString GetName() const { return "apocalypse"; }
  const cString GetDescription() const { return "apocalypse  [double kill_prob=.9]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") kill_prob=.9; else kill_prob=args.PopWord().AsDouble();
  }
  ///// apocalypse /////
  void Process(){
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell & cell = m_world->GetPopulation().GetCell(i);
      if (cell.IsOccupied() == false) continue;
      if (m_world->GetRandom().P(kill_prob))  m_world->GetPopulation().KillOrganism(cell);
    }
  }
};

///// kill_rectangle /////

/**
* Kills all cell in a rectangle.
 *
 * Parameters:
 * cell [X1][Y1][x2][Y2] (integer) default: 0
   *   The start and stoping grid-points into which the organism should
   be killed.
   **/


class cEvent_kill_rectangle : public cEvent {
private:
  int cell_X1;
  int cell_Y1;
  int cell_X2;
  int cell_Y2;
public:
    const cString GetName() const { return "kill_rectangle"; }
  const cString GetDescription() const { return "kill_rectangle  [int cell_X1=0] [int cell_Y1=0] [int cell_X2=0] [int cell_Y2=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") cell_X1=0; else cell_X1=args.PopWord().AsInt();
    if (args == "") cell_Y1=0; else cell_Y1=args.PopWord().AsInt();
    if (args == "") cell_X2=0; else cell_X2=args.PopWord().AsInt();
    if (args == "") cell_Y2=0; else cell_Y2=args.PopWord().AsInt();
  }
  ///// kill_rectangle /////
  void Process(){
    int i, j, loc;
    /* Be sure the user entered a valid range */
    if (cell_X1 < 0) {
      cell_X1 = 0;
    } else if (cell_X1 > m_world->GetPopulation().GetWorldX() - 1) {
      cell_X1 = m_world->GetPopulation().GetWorldX() - 1;
    }
    if (cell_X2 < 0) {
      cell_X2 = 0;
    } else if (cell_X2 > m_world->GetPopulation().GetWorldX() - 1) {
      cell_X2 = m_world->GetPopulation().GetWorldX() - 1;
    }
    if (cell_Y1 < 0) {
      cell_Y1 = 0;
    } else if (cell_Y1 > m_world->GetPopulation().GetWorldY() - 1) {
      cell_Y1 = m_world->GetPopulation().GetWorldY() - 1;
    }
    if (cell_Y2 < 0) {
      cell_Y2 = 0;
    } else if (cell_Y2 > m_world->GetPopulation().GetWorldY() - 1) {
      cell_Y2 = m_world->GetPopulation().GetWorldY() - 1;
    }
    /* Account for a rectangle that crosses over the Zero X or Y cell */
    if (cell_X2 < cell_X1) {
      cell_X2 = cell_X2 + m_world->GetPopulation().GetWorldX();
    }
    if (cell_Y2 < cell_Y1) {
      cell_Y2 = cell_Y2 + m_world->GetPopulation().GetWorldY();
    }
    for (i = cell_Y1; i <= cell_Y2; i++) {
      for (j = cell_X1; j <= cell_X2; j++) {
        loc = (i % m_world->GetPopulation().GetWorldY()) * m_world->GetPopulation().GetWorldX() +
        (j % m_world->GetPopulation().GetWorldX());
        cPopulationCell & cell = m_world->GetPopulation().GetCell(loc);
        if (cell.IsOccupied() == true) {
          m_world->GetPopulation().KillOrganism(cell);
        }
      }
    }
    m_world->GetPopulation().SetSyncEvents(true);
  }
};

///// rate_kill /////

/**
* Randomly removes a certain proportion of the population.
 * In principle, this event does the same thing as the apocalypse event.
 * However, instead of a probability, here one has to specify a rate. The
 * rate has the same unit as fitness. So if the average fitness is 20000,
 * then you remove 50% of the population on every update with a removal rate
 * of 10000.
 *
 * Parameters:
 * removal rate (double)
 *   The rate at which organisms are removed.
 **/


class cEvent_rate_kill : public cEvent {
private:
  double kill_rate;
public:
  const cString GetName() const { return "rate_kill"; }
  const cString GetDescription() const { return "rate_kill  <double kill_rate>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    kill_rate = args.PopWord().AsDouble();
  }
  ///// rate_kill /////
  void Process(){
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if ( ave_merit <= 0 )
      ave_merit = 1; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();
    const double kill_prob = kill_rate / ave_merit;
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell & cell = m_world->GetPopulation().GetCell(i);
      if (cell.IsOccupied() == false) continue;
      if (m_world->GetRandom().P(kill_prob))  m_world->GetPopulation().KillOrganism(cell);
    }
  }
};

///// serial_transfer /////

/**
* This event does again the same thing as apocalypse. However, now
 * the number of organisms to be retained can be specified
 * exactly. Also, it is possible to specify whether any of these
 * organisms may be dead or not.
 *
 * Parameters:
 * transfer size (int) default: 1
   *   The number of organisms to retain. If there are fewer living
   *   organisms than the specified transfer size, then all living
   *   organisms are retained.
   * ignore deads (int) default: 1
     *   When set to 1, only living organisms are retained. Otherwise,
     *   every type of organism can be retained.
     **/


class cEvent_serial_transfer : public cEvent {
private:
  int transfer_size;
  int ignore_deads;
public:
    const cString GetName() const { return "serial_transfer"; }
  const cString GetDescription() const { return "serial_transfer  [int transfer_size=1] [int ignore_deads=1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") transfer_size=1; else transfer_size=args.PopWord().AsInt();
    if (args == "") ignore_deads=1; else ignore_deads=args.PopWord().AsInt();
  }
  ///// serial_transfer /////
  void Process(){
    m_world->GetPopulation().SerialTransfer( transfer_size, ignore_deads );
  }
};

///// hillclimb /////

/**
* Does a hill climb with the dominant genotype.
 **/


class cEvent_hillclimb : public cEvent {
private:
public:
  const cString GetName() const { return "hillclimb"; }
  const cString GetDescription() const { return "hillclimb"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args; }
  ///// hillclimb /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.HillClimb(m_world->GetDataFileOFStream("hillclimb.dat"));
    m_world->GetDataFileManager().Remove("hillclimb.dat");
  }
};

///// hillclimb_neut /////

/**
**/


class cEvent_hillclimb_neut : public cEvent {
private:
public:
  const cString GetName() const { return "hillclimb_neut"; }
  const cString GetDescription() const { return "hillclimb_neut"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args; }
  ///// hillclimb_neut /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.HillClimb_Neut(m_world->GetDataFileOFStream("hillclimb.dat"));
    m_world->GetDataFileManager().Remove("hillclimb.dat");
  }
};

///// hillclimb_rand /////

/**
**/


class cEvent_hillclimb_rand : public cEvent {
private:
public:
  const cString GetName() const { return "hillclimb_rand"; }
  const cString GetDescription() const { return "hillclimb_rand"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args; }
  ///// hillclimb_rand /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.HillClimb_Rand(m_world->GetDataFileOFStream("hillclimb.dat"));
    m_world->GetDataFileManager().Remove("hillclimb.dat");
  }
};

///// compete_demes /////

/**
* Compete all of the demes using a basic genetic algorithm approach. Fitness
 * of each deme is determined differently depending on the competition_type: 
 * 0: deme fitness = 1 (control, random deme selection)
 * 1: deme fitness = number of births since last competition (default) 
 * 2: deme fitness = average organism fitness at the current update
 * 3: deme fitness = average mutation rate at the current update
 * Merit can optionally be passed in.
 **/


class cEvent_compete_demes : public cEvent {
private:
  int competition_type;
public:
  const cString GetName() const { return "compete_demes"; }
  const cString GetDescription() const { return "compete_demes  [int competition_type=1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") competition_type=1; else competition_type=args.PopWord().AsInt();
  }
  ///// compete_demes /////
  void Process(){
    m_world->GetPopulation().CompeteDemes(competition_type);
  }
};

///// reset_demes /////

/**
* Designed to serve as a control for the compete_demes. Each deme is 
 * copied into itself and the parameters reset. 
 **/


class cEvent_reset_demes : public cEvent {
private:
public:
  const cString GetName() const { return "reset_demes"; }
  const cString GetDescription() const { return "reset_demes"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// reset_demes /////
  void Process(){
    m_world->GetPopulation().ResetDemes();
  }
};

///// print_deme_stats /////

/**
* Print stats about individual demes
 **/


class cEvent_print_deme_stats : public cEvent {
private:
public:
  const cString GetName() const { return "print_deme_stats"; }
  const cString GetDescription() const { return "print_deme_stats"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// print_deme_stats /////
  void Process(){
    m_world->GetPopulation().PrintDemeStats();
  }
};

///// copy_deme /////

/**
* Takes two numbers as arguments and copies the contents of the first deme
 * listed into the second.
 **/


class cEvent_copy_deme : public cEvent {
private:
  int deme1_id;
  int deme2_id;
public:
    const cString GetName() const { return "copy_deme"; }
  const cString GetDescription() const { return "copy_deme  <int deme1_id> <int deme2_id>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    deme1_id = args.PopWord().AsInt();
    deme2_id = args.PopWord().AsInt();
  }
  ///// copy_deme /////
  void Process(){
    m_world->GetPopulation().CopyDeme(deme1_id, deme2_id);
  }
};

///// calc_consensus /////

/**
* Calculates the consensus sequence.
 *
 * Parameters:
 * lines saved (integer) default: 0
   *    ???
   **/


class cEvent_calc_consensus : public cEvent {
private:
  int lines_saved;
public:
  const cString GetName() const { return "calc_consensus"; }
  const cString GetDescription() const { return "calc_consensus  [int lines_saved=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") lines_saved=0; else lines_saved=args.PopWord().AsInt();
  }
  ///// calc_consensus /////
  void Process(){
    cAnalyzeUtil::CalcConsensus(m_world, lines_saved);
  }
};

///// test_size_change_robustness /////

/**
**/


class cEvent_test_size_change_robustness : public cEvent {
private:
  int num_trials;
  cString filename;
public:
    const cString GetName() const { return "test_size_change_robustness"; }
  const cString GetDescription() const { return "test_size_change_robustness  [int num_trials=100] [cString filename=\"size_change.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") num_trials=100; else num_trials=args.PopWord().AsInt();
    if (args == "") filename="size_change.dat"; else filename=args.PopWord();
  }
  ///// test_size_change_robustness /////
  void Process(){
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    cAnalyzeUtil::TestInsSizeChangeRobustness(m_world, fp,
                                              m_world->GetHardwareManager().GetInstSet(),
                                              m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                                              num_trials, m_world->GetStats().GetUpdate());
  }
};

///// test_threads /////

/**
**/


class cEvent_test_threads : public cEvent {
private:
public:
  const cString GetName() const { return "test_threads"; }
  const cString GetDescription() const { return "test_threads"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  
  ///// test_threads /////
  void Process(){
    m_world->GetTestCPU().TestThreads(m_world->GetClassificationManager().GetBestGenotype()->GetGenome());
  }
};

///// print_threads /////

/**
**/


class cEvent_print_threads : public cEvent {
private:
public:
  const cString GetName() const { return "print_threads"; }
  const cString GetDescription() const { return "print_threads"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// print_threads /////
  void Process(){
    m_world->GetTestCPU().PrintThreads( m_world->GetClassificationManager().GetBestGenotype()->GetGenome() );
  }
};

///// dump_fitness_grid /////

/**
* Writes out all fitness values of the organisms currently in the
 * population.
 *
 * The output file is called "fgrid.*.out", where '*' is replaced by the
 * number of the current update.
 **/


class cEvent_dump_fitness_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_fitness_grid"; }
  const cString GetDescription() const { return "dump_fitness_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_fitness_grid /////
  void Process(){
    cString filename;
    filename.Set("fgrid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        double fitness = (cell.IsOccupied()) ?
          cell.GetOrganism()->GetGenotype()->GetFitness() : 0.0;
        fp << fitness << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_genotype_grid /////

/**
* Writes out all genotype id values of the organisms currently in the
 * population.
 *
 * The output file is called "idgrid.*.out", where '*' is replaced by the
 * number of the current update.
 **/


class cEvent_dump_genotype_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_genotype_grid"; }
  const cString GetDescription() const { return "dump_genotype_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_genotype_grid /////
  void Process(){
    cString filename;
    filename.Set("idgrid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        int id = (cell.IsOccupied()) ?
          cell.GetOrganism()->GetGenotype()->GetID() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_task_grid /////

/**
* Writes out a grid of tasks done by each organism
 * Tasks are encoded as a binary string first, and then converted into a
 * base 10 number 
 **/


class cEvent_dump_task_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_task_grid"; }
  const cString GetDescription() const { return "dump_task_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_task_grid /////
  void Process(){
    cString filename;
    filename.Set("task_grid_%d.dat", m_world->GetStats().GetUpdate());
    cAnalyzeUtil::TaskGrid(m_world, m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_donor_grid /////

/**
* Writes out the grid of donor organisms in the population
 * 
 * The output file is called "donor_grid.*.out", where '*' is replaced by the
 * number of the current update.
 **/   


class cEvent_dump_donor_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_donor_grid"; }
  const cString GetDescription() const { return "dump_donor_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  
  ///// dump_donor_grid /////
  void Process(){
    cString filename;
    filename.Set("donor_grid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {  
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        int donor = cell.IsOccupied() ?  
          cell.GetOrganism()->GetPhenotype().IsDonorLast() : -1;    
        fp << donor << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_receiver_grid /////

/**
* Writes out the grid of organisms which received merit in the population
 *
 * The output file is called "receiver_grid.*.out", where '*' is replaced by the
 * number of the current update.
 **/


class cEvent_dump_receiver_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_receiver_grid"; }
  const cString GetDescription() const { return "dump_receiver_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_receiver_grid /////
  void Process(){
    cString filename;
    filename.Set("receiver_grid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        int receiver = cell.IsOccupied() ?
          cell.GetOrganism()->GetPhenotype().IsReceiver() : -1;
        fp << receiver << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// print_tree_depths /////

/**
* Reconstruction of phylogenetic trees.
 **/


class cEvent_print_tree_depths : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_tree_depths"; }
  const cString GetDescription() const { return "print_tree_depths  [cString filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename=""; else filename=args.PopWord();
  }
  ///// print_tree_depths /////
  void Process(){
    if (filename == "") filename.Set("tree_depth.%d.dat", m_world->GetStats().GetUpdate());
    cAnalyzeUtil::PrintTreeDepths(m_world, m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// sever_grid_col /////

/**
* Remove the connections between cells along a column in an avida grid.
 * Arguments:
 *  col_id:  indicates the number of columns to the left of the cut.
 *           default (or -1) = cut population in half
 *  min_row: First row to start cutting from
 *           default = 0
 *  max_row: Last row to cut to
 *           default (or -1) = last row in population.
 **/


class cEvent_sever_grid_col : public cEvent {
private:
  int col_id;
  int min_row;
  int max_row;
public:
    const cString GetName() const { return "sever_grid_col"; }
  const cString GetDescription() const { return "sever_grid_col  [int col_id=-1] [int min_row=0] [int max_row=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") col_id=-1; else col_id=args.PopWord().AsInt();
    if (args == "") min_row=0; else min_row=args.PopWord().AsInt();
    if (args == "") max_row=-1; else max_row=args.PopWord().AsInt();
  }
  ///// sever_grid_col /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (col_id == -1) col_id = world_x / 2;
    if (max_row == -1) max_row = world_y;
    if (col_id < 0 || col_id >= world_x) {
      cerr << "Event Error: Column ID " << col_id
      << " out of range for sever_grid_col" << endl;
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int row_id = min_row; row_id < max_row; row_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);
      int idA0 = GridNeighbor(idA, world_x, world_y,  0, -1);
      int idA1 = GridNeighbor(idA, world_x, world_y,  0,  1);
      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y, -1,  1);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB0));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB1));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA0));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA1));
    }
  }
};

///// sever_grid_row /////

/**
* Remove the connections between cells along a column in an avida grid.
 * Arguments:
 *  row_id:  indicates the number of rows above the cut.
 *           default (or -1) = cut population in half
 *  min_col: First row to start cutting from
 *           default = 0
 *  max_col: Last row to cut to
 *           default (or -1) = last row in population.
 **/


class cEvent_sever_grid_row : public cEvent {
private:
  int row_id;
  int min_col;
  int max_col;
public:
    const cString GetName() const { return "sever_grid_row"; }
  const cString GetDescription() const { return "sever_grid_row  [int row_id=-1] [int min_col=0] [int max_col=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") row_id=-1; else row_id=args.PopWord().AsInt();
    if (args == "") min_col=0; else min_col=args.PopWord().AsInt();
    if (args == "") max_col=-1; else max_col=args.PopWord().AsInt();
  }
  ///// sever_grid_row /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (row_id == -1) row_id = world_y / 2;
    if (max_col == -1) max_col = world_x;
    if (row_id < 0 || row_id >= world_y) {
      cerr << "Event Error: Row ID " << row_id
      << " out of range for sever_grid_row" << endl;
      return;
    }
    // Loop through all of the cols and make the cut on each...
    for (int col_id = min_col; col_id < max_col; col_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y,  0, -1);
      int idA0 = GridNeighbor(idA, world_x, world_y, -1,  0);
      int idA1 = GridNeighbor(idA, world_x, world_y,  1,  0);
      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y,  1, -1);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB0));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB1));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA0));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA1));
    }
  }
};

///// join_grid_col /////

/**
* Join the connections between cells along a column in an avida grid.
 * Arguments:
 *  col_id:  indicates the number of columns to the left of the joining.
 *           default (or -1) = join population halves.
 *  min_row: First row to start joining from
 *           default = 0
 *  max_row: Last row to join to
 *           default (or -1) = last row in population.
 **/


class cEvent_join_grid_col : public cEvent {
private:
  int col_id;
  int min_row;
  int max_row;
public:
    const cString GetName() const { return "join_grid_col"; }
  const cString GetDescription() const { return "join_grid_col  [int col_id=-1] [int min_row=0] [int max_row=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") col_id=-1; else col_id=args.PopWord().AsInt();
    if (args == "") min_row=0; else min_row=args.PopWord().AsInt();
    if (args == "") max_row=-1; else max_row=args.PopWord().AsInt();
  }
  ///// join_grid_col /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (col_id == -1) col_id = world_x / 2;
    if (max_row == -1) max_row = world_y;
    if (col_id < 0 || col_id >= world_x) {
      cerr << "Event Error: Column ID " << col_id
      << " out of range for join_grid_col" << endl;
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int row_id = min_row; row_id < max_row; row_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      cPopulationCell & cellA0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0, -1));
      cPopulationCell & cellA1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0,  1));
      cPopulationCell & cellB0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
      cPopulationCell & cellB1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  1));
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
      if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);
      if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
    }
  }
};

///// join_grid_row /////

/**
* Remove the connections between cells along a column in an avida grid.
 * Arguments:
 *  row_id:  indicates the number of rows abovef the cut.
 *           default (or -1) = cut population in half
 *  min_col: First row to start cutting from
 *           default = 0
 *  max_col: Last row to cut to
 *           default (or -1) = last row in population.
 **/


class cEvent_join_grid_row : public cEvent {
private:
  int row_id;
  int min_col;
  int max_col;
public:
    const cString GetName() const { return "join_grid_row"; }
  const cString GetDescription() const { return "join_grid_row  [int row_id=-1] [int min_col=0] [int max_col=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") row_id=-1; else row_id=args.PopWord().AsInt();
    if (args == "") min_col=0; else min_col=args.PopWord().AsInt();
    if (args == "") max_col=-1; else max_col=args.PopWord().AsInt();
  }
  ///// join_grid_row /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (row_id == -1) row_id = world_y / 2;
    if (max_col == -1) max_col = world_x;
    if (row_id < 0 || row_id >= world_y) {
      cerr << "Event Error: Row ID " << row_id
      << " out of range for join_grid_row" << endl;
      return;
    }
    // Loop through all of the cols and make the cut on each...
    for (int col_id = min_col; col_id < max_col; col_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y,  0, -1);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      cPopulationCell & cellA0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  0));
      cPopulationCell & cellA1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1,  0));
      cPopulationCell & cellB0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
      cPopulationCell & cellB1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1, -1));
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
      if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);
      if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
    }
  }
};

///// connect_cells /////

/**
* Connects a pair of specified cells.
 * Arguments:
 *  cellA_x, cellA_y, cellB_x, cellB_y
 **/


class cEvent_connect_cells : public cEvent {
private:
  int cellA_x;
  int cellA_y;
  int cellB_x;
  int cellB_y;
public:
    const cString GetName() const { return "connect_cells"; }
  const cString GetDescription() const { return "connect_cells  <int cellA_x> <int cellA_y> <int cellB_x> <int cellB_y>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    cellA_x = args.PopWord().AsInt();
    cellA_y = args.PopWord().AsInt();
    cellB_x = args.PopWord().AsInt();
    cellB_y = args.PopWord().AsInt();
  }
  ///// connect_cells /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (cellA_x < 0 || cellA_x >= world_x ||
        cellA_y < 0 || cellA_y >= world_y ||
        cellB_x < 0 || cellB_x >= world_x ||
        cellB_y < 0 || cellB_y >= world_y) {
      cerr << "Event 'connect_cells' cell out of range." << endl;
      return;
    }
    int idA = cellA_y * world_x + cellA_x;
    int idB = cellB_y * world_x + cellB_x;
    cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
    cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
    tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
    tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
    cellA_list.PushRear(&cellB);
    cellB_list.PushRear(&cellA);
  }
};

///// disconnect_cells /////

/**
* Connects a pair of specified cells.
 * Arguments:
 *  cellA_x, cellA_y, cellB_x, cellB_y
 **/


class cEvent_disconnect_cells : public cEvent {
private:
  int cellA_x;
  int cellA_y;
  int cellB_x;
  int cellB_y;
public:
    const cString GetName() const { return "disconnect_cells"; }
  const cString GetDescription() const { return "disconnect_cells  <int cellA_x> <int cellA_y> <int cellB_x> <int cellB_y>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    cellA_x = args.PopWord().AsInt();
    cellA_y = args.PopWord().AsInt();
    cellB_x = args.PopWord().AsInt();
    cellB_y = args.PopWord().AsInt();
  }
  ///// disconnect_cells /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (cellA_x < 0 || cellA_x >= world_x ||
        cellA_y < 0 || cellA_y >= world_y ||
        cellB_x < 0 || cellB_x >= world_x ||
        cellB_y < 0 || cellB_y >= world_y) {
      cerr << "Event 'connect_cells' cell out of range." << endl;
      return;
    }
    int idA = cellA_y * world_x + cellA_x;
    int idB = cellB_y * world_x + cellB_x;
    cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
    cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
    tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
    tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
    cellA_list.Remove(&cellB);
    cellB_list.Remove(&cellA);
  }
};

///// inject_resource /////

/**
* Inject (add) a specified amount of a specified resource.
 **/


class cEvent_inject_resource : public cEvent {
private:
  cString res_name;
  double res_count;
public:
    const cString GetName() const { return "inject_resource"; }
  const cString GetDescription() const { return "inject_resource  <cString res_name> <double res_count>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_count = args.PopWord().AsDouble();
  }
  ///// inject_resource /////
  void Process(){
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    int res_id = res_lib.GetResource(res_name)->GetID();
    m_world->GetPopulation().UpdateResource(res_id, res_count);
  }
};

///// set_resource /////

/**
* Set the resource amount to a specific level
 **/


class cEvent_set_resource : public cEvent {
private:
  cString res_name;
  double res_count;
public:
    const cString GetName() const { return "set_resource"; }
  const cString GetDescription() const { return "set_resource  <cString res_name> <double res_count>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_count = args.PopWord().AsDouble();
  }
  ///// set_resource /////
  void Process(){
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    cResource * found_resource = res_lib.GetResource(res_name);
    if (found_resource != NULL) {
      m_world->GetPopulation().SetResource(found_resource->GetID(), res_count);
    }
  }
};

///// inject_scaled_resource /////

/**
* Inject (add) a specified amount of a specified resource, scaled by
 * the current average merit divided by the average time slice.
 **/


class cEvent_inject_scaled_resource : public cEvent {
private:
  cString res_name;
  double res_count;
public:
    const cString GetName() const { return "inject_scaled_resource"; }
  const cString GetDescription() const { return "inject_scaled_resource  <cString res_name> <double res_count>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_count = args.PopWord().AsDouble();
  }
  ///// inject_scaled_resource /////
  void Process(){
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if ( ave_merit <= 0 )
      ave_merit = 1; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    int res_id = res_lib.GetResource(res_name)->GetID();
    m_world->GetPopulation().UpdateResource(res_id, res_count/ave_merit);
  }
};


///// outflow_scaled_resource /////

/**
* Removes a specified percentage of a specified resource, scaled by
 * the current average merit divided by the average time slice.
 **/
class cEvent_outflow_scaled_resource : public cEvent {
private:
  cString res_name;
  double res_perc;
public:
    const cString GetName() const { return "outflow_scaled_resource"; }
  const cString GetDescription() const { return "outflow_scaled_resource  <cString res_name> <double res_perc>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_perc = args.PopWord().AsDouble();
  }
  void Process()
  {
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if ( ave_merit <= 0 )
      ave_merit = 1; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    int res_id = res_lib.GetResource(res_name)->GetID();
    double res_level = m_world->GetPopulation().GetResource(res_id);
    // a quick calculation shows that this formula guarantees that
    // the equilibrium level when resource is not used is independent
    // of the average merit
    double scaled_perc = 1/(1+ave_merit*(1-res_perc)/res_perc);
    res_level -= res_level*scaled_perc;
    m_world->GetPopulation().SetResource(res_id, res_level);
  }
};


///// set_reaction_value /////

/**
* Set the value associated with a reaction to a specific level
 **/
class cEvent_set_reaction_value : public cEvent {
private:
  cString reaction_name;
  double reaction_value;
public:
    const cString GetName() const { return "set_reaction_value"; }
  const cString GetDescription() const { return "set_reaction_value  <cString reaction_name> <double reaction_value>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    reaction_name = args.PopWord();
    reaction_value = args.PopWord().AsDouble();
  }
  void Process()
  {
    m_world->GetEnvironment().SetReactionValue(reaction_name, reaction_value);
  }
};


///// set_reaction_value_mult /////

/**
* Change the value of the reaction by multiplying it with the imput number
 **/
class cEvent_set_reaction_value_mult : public cEvent {
private:
  cString reaction_name;
  double value_mult;
public:
    const cString GetName() const { return "set_reaction_value_mult"; }
  const cString GetDescription() const { return "set_reaction_value_mult  <cString reaction_name> <double value_mult>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    reaction_name = args.PopWord();
    value_mult = args.PopWord().AsDouble();
  }
  void Process()
  {
    m_world->GetEnvironment().SetReactionValueMult(reaction_name, value_mult);
  }
};


///// set_reaction_inst /////

/**
* Change the instruction triggered by the task
 **/
class cEvent_set_reaction_inst : public cEvent {
private:
  cString reaction_name;
  cString inst_name;
public:
    const cString GetName() const { return "set_reaction_inst"; }
  const cString GetDescription() const { return "set_reaction_inst <cString reaction_name> <cString inst_name>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    reaction_name = args.PopWord();
    inst_name = args.PopWord();
  }
  void Process()
  {
    m_world->GetEnvironment().SetReactionInst(reaction_name, inst_name);
  }
};

cEventManager::cEventManager(cWorld* world) : m_world(world)
{
  Register<cEvent_exit>("exit");
  Register<cEvent_exit_if_generation_greater_than>("exit_if_generation_greater_than");
  Register<cEvent_exit_if_update_greater_than>("exit_if_update_greater_than");
  Register<cEvent_exit_if_ave_lineage_label_smaller>("exit_if_ave_lineage_label_smaller");
  Register<cEvent_exit_if_ave_lineage_label_larger>("exit_if_ave_lineage_label_larger");
  Register<cEvent_echo>("echo");
  Register<cEvent_print_data>("print_data");
  Register<cEvent_print_average_data>("print_average_data");
  Register<cEvent_print_error_data>("print_error_data");
  Register<cEvent_print_variance_data>("print_variance_data");
  Register<cEvent_print_dominant_data>("print_dominant_data");
  Register<cEvent_print_stats_data>("print_stats_data");
  Register<cEvent_print_count_data>("print_count_data");
  Register<cEvent_print_totals_data>("print_totals_data");
  Register<cEvent_print_tasks_data>("print_tasks_data");
  Register<cEvent_print_tasks_exe_data>("print_tasks_exe_data");
  Register<cEvent_print_resource_data>("print_resource_data");
  Register<cEvent_print_time_data>("print_time_data");
  Register<cEvent_print_mutation_data>("print_mutation_data");
  Register<cEvent_print_mutation_rate_data>("print_mutation_rate_data");
  Register<cEvent_print_divide_mut_data>("print_divide_mut_data");
  Register<cEvent_print_dom_parasite_data>("print_dom_parasite_data");
  Register<cEvent_print_instruction_data>("print_instruction_data");
  Register<cEvent_print_instruction_abundance_histogram>("print_instruction_abundance_histogram");
  Register<cEvent_print_depth_histogram>("print_depth_histogram");
  Register<cEvent_print_genotype_abundance_histogram>("print_genotype_abundance_histogram");
  Register<cEvent_print_species_abundance_histogram>("print_species_abundance_histogram");
  Register<cEvent_print_lineage_totals>("print_lineage_totals");
  Register<cEvent_print_lineage_counts>("print_lineage_counts");
  Register<cEvent_print_dom>("print_dom");
  Register<cEvent_parasite_debug>("parasite_debug");
  Register<cEvent_print_dom_parasite>("print_dom_parasite");
  Register<cEvent_print_genotype_map>("print_genotype_map");
  Register<cEvent_print_number_phenotypes>("print_number_phenotypes");
  Register<cEvent_print_phenotype_status>("print_phenotype_status");
  Register<cEvent_save_population>("save_population");
  Register<cEvent_load_population>("load_population");
  Register<cEvent_save_clone>("save_clone");
  Register<cEvent_load_clone>("load_clone");
  Register<cEvent_load_dump_file>("load_dump_file");
  Register<cEvent_dump_pop>("dump_pop");
  Register<cEvent_print_genotypes>("print_genotypes");
  Register<cEvent_detail_pop>("detail_pop");
  Register<cEvent_detail_sex_pop>("detail_sex_pop");
  Register<cEvent_detail_parasite_pop>("detail_parasite_pop");
  Register<cEvent_dump_historic_pop>("dump_historic_pop");
  Register<cEvent_dump_historic_sex_pop>("dump_historic_sex_pop");
  Register<cEvent_dump_memory>("dump_memory");
  Register<cEvent_inject>("inject");
  Register<cEvent_inject_all>("inject_all");
  Register<cEvent_inject_range>("inject_range");
  Register<cEvent_inject_sequence>("inject_sequence");
  Register<cEvent_inject_random>("inject_random");
  Register<cEvent_inject_range_parasite>("inject_range_parasite");
  Register<cEvent_inject_range_pair>("inject_range_pair");
  Register<cEvent_zero_muts>("zero_muts");
  Register<cEvent_mod_copy_mut>("mod_copy_mut");
  Register<cEvent_mod_div_mut>("mod_div_mut");
  Register<cEvent_set_copy_mut>("set_copy_mut");
  Register<cEvent_mod_point_mut>("mod_point_mut");
  Register<cEvent_set_point_mut>("set_point_mut");
  Register<cEvent_calc_landscape>("calc_landscape");
  Register<cEvent_predict_w_landscape>("predict_w_landscape");
  Register<cEvent_predict_nu_landscape>("predict_nu_landscape");
  Register<cEvent_sample_landscape>("sample_landscape");
  Register<cEvent_random_landscape>("random_landscape");
  Register<cEvent_analyze_landscape>("analyze_landscape");
  Register<cEvent_pairtest_landscape>("pairtest_landscape");
  Register<cEvent_test_dom>("test_dom");
  Register<cEvent_analyze_population>("analyze_population");
  Register<cEvent_print_detailed_fitness_data>("print_detailed_fitness_data");
  Register<cEvent_print_genetic_distance_data>("print_genetic_distance_data");
  Register<cEvent_genetic_distance_pop_dump>("genetic_distance_pop_dump");
  Register<cEvent_task_snapshot>("task_snapshot");
  Register<cEvent_print_viable_tasks_data>("print_viable_tasks_data");
  Register<cEvent_apocalypse>("apocalypse");
  Register<cEvent_kill_rectangle>("kill_rectangle");
  Register<cEvent_rate_kill>("rate_kill");
  Register<cEvent_serial_transfer>("serial_transfer");
  Register<cEvent_hillclimb>("hillclimb");
  Register<cEvent_hillclimb_neut>("hillclimb_neut");
  Register<cEvent_hillclimb_rand>("hillclimb_rand");
  Register<cEvent_compete_demes>("compete_demes");
  Register<cEvent_reset_demes>("reset_demes");
  Register<cEvent_print_deme_stats>("print_deme_stats");
  Register<cEvent_copy_deme>("copy_deme");
  Register<cEvent_calc_consensus>("calc_consensus");
  Register<cEvent_test_size_change_robustness>("test_size_change_robustness");
  Register<cEvent_test_threads>("test_threads");
  Register<cEvent_print_threads>("print_threads");
  Register<cEvent_dump_fitness_grid>("dump_fitness_grid");
  Register<cEvent_dump_genotype_grid>("dump_genotype_grid");
  Register<cEvent_dump_task_grid>("dump_task_grid");
  Register<cEvent_dump_donor_grid>("dump_donor_grid");
  Register<cEvent_dump_receiver_grid>("dump_receiver_grid");
  Register<cEvent_print_tree_depths>("print_tree_depths");
  Register<cEvent_sever_grid_col>("sever_grid_col");
  Register<cEvent_sever_grid_row>("sever_grid_row");
  Register<cEvent_join_grid_col>("join_grid_col");
  Register<cEvent_join_grid_row>("join_grid_row");
  Register<cEvent_connect_cells>("connect_cells");
  Register<cEvent_disconnect_cells>("disconnect_cells");
  Register<cEvent_inject_resource>("inject_resource");
  Register<cEvent_set_resource>("set_resource");
  Register<cEvent_inject_scaled_resource>("inject_scaled_resource");
  Register<cEvent_outflow_scaled_resource>("outflow_scaled_resource");
  Register<cEvent_set_reaction_value>("set_reaction_value");
  Register<cEvent_set_reaction_value_mult>("set_reaction_value_mult");
  Register<cEvent_set_reaction_inst>("set_reaction_inst");
}

cEvent* cEventManager::ConstructEvent(const cString name, const cString & args)
{
  cEvent* event = Create(name);
  event->Configure(m_world, args);
  return event;
}

void cEventManager::PrintAllEventDescriptions()
{
  tArray<cEvent*> events;
  CreateAll(events);
  
  for (int i = 0; i < events.GetSize(); i++) {
    cout << events[i]->GetDescription() << endl;
    delete events[i];
  }
}
