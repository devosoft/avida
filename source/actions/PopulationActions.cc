/*
 *  PopulationActions.cc
 *  Avida
 *
 *  Created by David on 6/25/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "PopulationActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cGenome.h"
#include "cHardwareManager.h"
#include "cInstUtil.h"
#include "cPopulation.h"
#include "cWorld.h"


/*
 Injects a single organism into the population.
 
 Parameters:
   filename (string)
     The filename of the genotype to load. If this is left empty, or the keyword
     "START_CREATURE" is given, than the genotype specified in the genesis
     file under "START_CREATURE" is used.
   cell ID (integer) default: 0
     The grid-point into which the organism should be placed.
   merit (double) default: -1
     The initial merit of the organism. If set to -1, this is ignored.
   lineage label (integer) default: 0
     An integer that marks all descendants of this organism.
   neutral metric (double) default: 0
     A double value that randomly drifts over time.
*/
class cActionInject : public cAction
{
private:
  cString m_filename;
  int m_cell_id;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInject(cWorld* world, const cString& args) : cAction(world, args), m_cell_id(0), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (!largs.GetSize()) m_filename = "START_CREATURE"; else m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();

    if (m_filename == "START_CREATURE") m_filename = m_world->GetConfig().START_CREATURE.Get();
}
  
  const cString GetDescription() { return "Inject [cString fname=\"START_CREATURE\"] [int cell_id=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cInstUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
    m_world->GetPopulation().Inject(genome, m_cell_id, m_merit, m_lineage_label, m_neutral_metric);
  }
};


/*
 Injects identical organisms into all cells of the population.
 
 Parameters:
   filename (string)
     The filename of the genotype to load. If this is left empty, or the keyword
     "START_CREATURE" is given, than the genotype specified in the genesis
     file under "START_CREATURE" is used.
   cell ID (integer) default: 0
     The grid-point into which the organism should be placed.
   merit (double) default: -1
     The initial merit of the organism. If set to -1, this is ignored.
   lineage label (integer) default: 0
     An integer that marks all descendants of this organism.
   neutral metric (double) default: 0
     A double value that randomly drifts over time.
*/
class cActionInjectAll : public cAction
{
private:
  cString m_filename;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectAll(cWorld* world, const cString& args) : cAction(world, args), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (!largs.GetSize()) m_filename = "START_CREATURE"; else m_filename = largs.PopWord();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_filename == "START_CREATURE") m_filename = m_world->GetConfig().START_CREATURE.Get();
  }
  
  const cString GetDescription() { return "InjectAll [cString fname=\"START_CREATURE\"] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cInstUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++)
      m_world->GetPopulation().Inject(genome, i, m_merit, m_lineage_label, m_neutral_metric);
  }
};

/*
 Injects identical organisms into a range of cells of the population.
 
 Parameters:
   filename (string)
     The filename of the genotype to load. If this is left empty, or the keyword
     "START_CREATURE" is given, than the genotype specified in the genesis
     file under "START_CREATURE" is used.
   cell_start (int)
     First cell to inject into.
   cell_end (int)
     First cell *not* to inject into.
   merit (double) default: -1
     The initial merit of the organism. If set to -1, this is ignored.
   lineage label (integer) default: 0
     An integer that marks all descendants of this organism.
   neutral metric (double) default: 0
     A double value that randomly drifts over time.
*/
class cActionInjectRange : public cAction
{
private:
  cString m_filename;
  int m_cell_start;
  int m_cell_end;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectRange(cWorld* world, const cString& args) : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (!largs.GetSize()) m_filename = "START_CREATURE"; else m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_filename == "START_CREATURE") m_filename = m_world->GetConfig().START_CREATURE.Get();
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  const cString GetDescription() { return "InjectRange [cString fname=\"START_CREATURE\"] [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cInstUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectRange has invalid range!");
    } else {
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, i, m_merit, m_lineage_label, m_neutral_metric);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};


/*
 Injects identical parasites into a range of cells of the population.
 
 Parameters:
   filename (string)
     The filename of the genotype to load.
   cell_start (int)
     First cell to inject into.
   cell_end (int)
     First cell *not* to inject into.
   merit (double) default: -1
     The initial merit of the organism. If set to -1, this is ignored.
   lineage label (integer) default: 0
     An integer that marks all descendants of this organism.
   neutral metric (double) default: 0
     A double value that randomly drifts over time.
*/
class cActionInjectParasiteRange : public cAction
{
private:
  cString m_filename;
  int m_cell_start;
  int m_cell_end;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectParasiteRange(cWorld* world, const cString& args) : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
      cString largs(args);
      if (!largs.GetSize()) m_filename = "default.para"; else m_filename = largs.PopWord();
      if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
      if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
      if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
      if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
      if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
      
      if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  const cString GetDescription() { return "InjectRange <cString filename=\"START_CREATURE\"> [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cInstUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectParasiteRange has invalid range!");
    } else {
      for (int i = m_cell_start; i < m_cell_end; i++) {
        // @DMB 
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};



void RegisterPopulationActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionInject>("Inject");
  action_lib->Register<cActionInjectAll>("InjectAll");
  action_lib->Register<cActionInjectRange>("InjectRange");


  // @DMB - The following actions are DEPRECATED aliases - These will be removed in 2.7.
  action_lib->Register<cActionInject>("inject");
  action_lib->Register<cActionInjectAll>("inject_all");
  action_lib->Register<cActionInjectRange>("inject_range");
}
