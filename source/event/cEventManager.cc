/*
 *  cEventManager.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cEventManager.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyzeUtil.h"
#include "cAvidaContext.h"
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
  
  void Configure(cWorld* world, const cString& in_args) { m_world = world; }
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
    
    cAvidaContext& ctx = m_world->GetDefaultContext();
    
    cGenome genome =
      cInstUtil::RandomGenome(ctx, length, m_world->GetHardwareManager().GetInstSet());
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
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.PredictWProcess(ctx, m_world->GetDataFileOFStream(datafile));
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
  void Process() {
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cGenome& genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.PredictNuProcess(ctx, m_world->GetDataFileOFStream(datafile));
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
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cGenome& genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.HillClimb(ctx, m_world->GetDataFileOFStream("hillclimb.dat"));
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
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cGenome& genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.HillClimb_Neut(ctx, m_world->GetDataFileOFStream("hillclimb.dat"));
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
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cGenome& genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cLandscape landscape(m_world, genome, m_world->GetHardwareManager().GetInstSet());
    landscape.HillClimb_Rand(ctx, m_world->GetDataFileOFStream("hillclimb.dat"));
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
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    testcpu->TestThreads(ctx, m_world->GetClassificationManager().GetBestGenotype()->GetGenome());
    delete testcpu;
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
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    testcpu->PrintThreads(ctx, m_world->GetClassificationManager().GetBestGenotype()->GetGenome() );
    delete testcpu;
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

class cEventAction : public cEvent
{
private:
  cAction* m_action;

public:
  cEventAction(cWorld* world, cAction* action, const cString& args)
    : m_action(action) { Configure(world, args); }
  ~cEventAction() { delete m_action; }
  
  const cString GetName() const { return "action wrapper"; }
  const cString GetDescription() const { return "action wrapper - description not available"; }
  
  void Configure(cWorld* world, const cString& in_args) { m_world = world; m_args = in_args; }
  void Process()
  {
    cAvidaContext& ctx = m_world->GetDefaultContext();
    m_action->Process(ctx);
  }
};



#define REGISTER(EVENT_NAME) Register<cEvent_ ## EVENT_NAME>(#EVENT_NAME)

cEventManager::cEventManager(cWorld* world) : m_world(world)
{
  REGISTER(exit);
  REGISTER(exit_if_generation_greater_than);
  REGISTER(exit_if_update_greater_than);
  REGISTER(exit_if_ave_lineage_label_smaller);
  REGISTER(exit_if_ave_lineage_label_larger);
  
  REGISTER(inject);
  REGISTER(inject_all);
  REGISTER(inject_range);
  REGISTER(inject_sequence);
  REGISTER(inject_random);
  REGISTER(inject_range_parasite);
  REGISTER(inject_range_pair);
  REGISTER(zero_muts);
  REGISTER(mod_copy_mut);
  REGISTER(mod_div_mut);
  REGISTER(set_copy_mut);
  REGISTER(mod_point_mut);
  REGISTER(set_point_mut);
  REGISTER(calc_landscape);
  REGISTER(predict_w_landscape);
  REGISTER(predict_nu_landscape);
  REGISTER(pairtest_landscape);
  REGISTER(test_dom);
  REGISTER(analyze_population);

  REGISTER(print_genetic_distance_data);
  REGISTER(genetic_distance_pop_dump);
  REGISTER(task_snapshot);
  REGISTER(print_viable_tasks_data);
  
  REGISTER(apocalypse);
  REGISTER(kill_rectangle);
  REGISTER(rate_kill);
  REGISTER(serial_transfer);
  REGISTER(hillclimb);
  REGISTER(hillclimb_neut);
  REGISTER(hillclimb_rand);
  REGISTER(compete_demes);
  REGISTER(reset_demes);
  REGISTER(print_deme_stats);
  REGISTER(copy_deme);
  
  REGISTER(calc_consensus);
  REGISTER(test_size_change_robustness);
  REGISTER(test_threads);
  REGISTER(print_threads);
  REGISTER(dump_fitness_grid);
  REGISTER(dump_genotype_grid);
  REGISTER(dump_task_grid);
  REGISTER(dump_donor_grid);
  REGISTER(dump_receiver_grid);
  REGISTER(print_tree_depths);
  REGISTER(sever_grid_col);
  REGISTER(sever_grid_row);
  REGISTER(join_grid_col);
  REGISTER(join_grid_row);
  REGISTER(connect_cells);
  REGISTER(disconnect_cells);
  REGISTER(inject_resource);
  REGISTER(set_resource);
  REGISTER(inject_scaled_resource);
  REGISTER(outflow_scaled_resource);
  REGISTER(set_reaction_value);
  REGISTER(set_reaction_value_mult);
  REGISTER(set_reaction_inst);
}

cEvent* cEventManager::ConstructEvent(const cString name, const cString & args)
{
  cEvent* event = Create(name);
  
  if (event != NULL) {
    event->Configure(m_world, args);
  } else {
    cAction* action = m_world->GetActionLibrary().Create(name, m_world, args);
    if (action != NULL) event = new cEventAction(m_world, action, args);
  }
  
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
