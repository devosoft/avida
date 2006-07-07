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
#include "cCodeLabel.h"
#include "cGenome.h"
#include "cHardwareManager.h"
#include "cInstUtil.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
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
  
  const cString GetDescription() { return "Inject [string fname=\"START_CREATURE\"] [int cell_id=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cInstUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
    m_world->GetPopulation().Inject(genome, m_cell_id, m_merit, m_lineage_label, m_neutral_metric);
  }
};


/*
 Injects a randomly generated genome into the population.
 
 Parameters:
   length (integer) [required]
     Number of instructions in the randomly generated genome.
   cell ID (integer) default: 0
     The grid-point into which the organism should be placed.
   merit (double) default: -1
     The initial merit of the organism. If set to -1, this is ignored.
   lineage label (integer) default: 0
     An integer that marks all descendants of this organism.
   neutral metric (double) default: 0
     A double value that randomly drifts over time.
*/
class cActionInjectRandom : public cAction
{
private:
  int m_length;
  int m_cell_id;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectRandom(cWorld* world, const cString& args) : cAction(world, args), m_cell_id(0), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    m_length = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
  }
  
  const cString GetDescription() { return "InjectRandom <int length> [int cell_id=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cInstUtil::RandomGenome(ctx, m_length, m_world->GetHardwareManager().GetInstSet());
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
  
  const cString GetDescription() { return "InjectAll [string fname=\"START_CREATURE\"] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
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
  cActionInjectRange(cWorld* world, const cString& args)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
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
  
  const cString GetDescription() { return "InjectRange [string fname=\"START_CREATURE\"] [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectRange has invalid range!");
    } else {
      cGenome genome = cInstUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, i, m_merit, m_lineage_label, m_neutral_metric);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};


/*
 Injects identical organisms into a range of cells of the population.
 
 Parameters:
   sequence (string) [required]
     The genome sequence for this organism.
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
class cActionInjectSequence : public cAction
{
private:
  cString m_sequence;
  int m_cell_start;
  int m_cell_end;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectSequence(cWorld* world, const cString& args)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    m_sequence = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  const cString GetDescription() { return "InjectSequence [string fname=\"START_CREATURE\"] [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectRange has invalid range!");
    } else {
      cGenome genome(m_sequence);
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
   filename (string) [required]
     The filename of the genotype to load.
   label (string) [required]
     The parasite's inject target label.
   cell_start (int)
     First cell to inject into.
   cell_end (int)
     First cell *not* to inject into.
*/
class cActionInjectParasite : public cAction
{
private:
  cString m_filename;
  cCodeLabel m_label;
  int m_cell_start;
  int m_cell_end;
public:
  cActionInjectParasite(cWorld* world, const cString& args) : cAction(world, args), m_cell_start(0), m_cell_end(-1)
  {
    cString largs(args);
    m_filename = largs.PopWord();
    m_label.ReadString(largs.PopWord());
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  const cString GetDescription() { return "InjectParasite <string filename> <string label> [int cell_start=0] [int cell_end=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectParasite has invalid range!");
    } else {
      cGenome genome = cInstUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().InjectParasite(m_label, genome, i);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};


/*
 Injects identical parasites into a range of cells of the population.
 
 Parameters:
   filename_genome (string) [required]
     The filename of the genotype to load.
   filename_parasite (string) [required]
     The filename of the parasite to load.
   label (string) [required]
     The parasite's inject target label.
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
class cActionInjectParasitePair : public cAction
{
private:
  cString m_filename_genome;
  cString m_filename_parasite;
  cCodeLabel m_label;
  int m_cell_start;
  int m_cell_end;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectParasitePair(cWorld* world, const cString& args)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    m_filename_genome = largs.PopWord();
    m_filename_parasite = largs.PopWord();
    m_label.ReadString(largs.PopWord());
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  const cString GetDescription() { return "InjectParasitePair <string filename_genome> <string filename_parasite> <string label> [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectParasitePair has invalid range!");
    } else {
      cGenome genome = cInstUtil::LoadGenome(m_filename_genome, m_world->GetHardwareManager().GetInstSet());
      cGenome parasite = cInstUtil::LoadGenome(m_filename_parasite, m_world->GetHardwareManager().GetInstSet());
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, i, m_merit, m_lineage_label, m_neutral_metric);
        m_world->GetPopulation().InjectParasite(m_label, parasite, i);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};


/*
 Randomly removes a certain proportion of the population.
 
 Parameters:
  removal probability (double) default: 0.9
    The probability with which a single organism is removed.
*/
class cActionKillProb : public cAction
{
private:
  double m_killprob;
public:
  cActionKillProb(cWorld* world, const cString& args) : cAction(world, args), m_killprob(0.9)
  {
      cString largs(args);
      if (largs.GetSize()) m_killprob = largs.PopWord().AsDouble();
  }
  
  const cString GetDescription() { return "KillProb [double probability=0.9]"; }
  
  void Process(cAvidaContext& ctx)
  {
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
      if (cell.IsOccupied() == false) continue;
      if (ctx.GetRandom().P(m_killprob))  m_world->GetPopulation().KillOrganism(cell);
    }
  }
};


/*
 Randomly removes a certain proportion of the population.
 In principle, this event does the same thing as the KillProb action.
 However, instead of a probability, here one has to specify a rate. The
 rate has the same unit as fitness. So if the average fitness is 20000,
 then you remove 50% of the population on every update with a removal rate
 of 10000.
 
 Parameters:
   removal rate (double)
     The rate at which organisms are removed.
*/
class cActionKillRate : public cAction
{
private:
  double m_killrate;
public:
  cActionKillRate(cWorld* world, const cString& args) : cAction(world, args), m_killrate(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_killrate = largs.PopWord().AsDouble();
  }
  
  const cString GetDescription() { return "KillRate <double probability>"; }
  
  void Process(cAvidaContext& ctx)
  {
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if (ave_merit <= 0) ave_merit = 1; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();
    const double kill_prob = m_killrate / ave_merit;
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
      if (cell.IsOccupied() == false) continue;
      if (ctx.GetRandom().P(kill_prob))  m_world->GetPopulation().KillOrganism(cell);
    }
  }
};


/*
 Kills all cell in a rectangle.
 
 Parameters:
   cell [X1][Y1][x2][Y2] (integer) default: 0
     The start and stoping grid-points into which the organism should be killed.
*/
class cActionKillRectangle : public cAction
{
private:
  int m_x1;
  int m_y1;
  int m_x2;
  int m_y2;
public:
  cActionKillRectangle(cWorld* world, const cString& args) : cAction(world, args), m_x1(0), m_y1(0), m_x2(0), m_y2(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_x1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_y1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_x2 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_y2 = largs.PopWord().AsInt();

    /* Be sure the user entered a valid range */
    if (m_x1 < 0) {
      m_x1 = 0;
    } else if (m_x1 > m_world->GetPopulation().GetWorldX() - 1) {
      m_x1 = m_world->GetPopulation().GetWorldX() - 1;
    }
    if (m_x2 < 0) {
      m_x2 = 0;
    } else if (m_x2 > m_world->GetPopulation().GetWorldX() - 1) {
      m_x2 = m_world->GetPopulation().GetWorldX() - 1;
    }
    if (m_y1 < 0) {
      m_y1 = 0;
    } else if (m_y1 > m_world->GetPopulation().GetWorldY() - 1) {
      m_y1 = m_world->GetPopulation().GetWorldY() - 1;
    }
    if (m_y2 < 0) {
      m_y2 = 0;
    } else if (m_y2 > m_world->GetPopulation().GetWorldY() - 1) {
      m_y2 = m_world->GetPopulation().GetWorldY() - 1;
    }

    /* Account for a rectangle that crosses over the Zero X or Y cell */
    if (m_x2 < m_x1) {
      m_x2 = m_x2 + m_world->GetPopulation().GetWorldX();
    }
    if (m_y2 < m_y1) {
      m_y2 = m_y2 + m_world->GetPopulation().GetWorldY();
    }
  }
  
  const cString GetDescription() { return "KillRectangle [int x1=0] [int y1=0] [int x2=0] [int y2=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& pop = m_world->GetPopulation();
    for (int i = m_y1; i <= m_y2; i++) {
      for (int j = m_x1; j <= m_x2; j++) {
        int loc = (i % pop.GetWorldY()) * pop.GetWorldX() + (j % pop.GetWorldX());
        cPopulationCell& cell = pop.GetCell(loc);
        if (cell.IsOccupied()) pop.KillOrganism(cell);
      }
    }
    m_world->GetPopulation().SetSyncEvents(true);
  }
};


/*
 This event does again the same thing as KillProb. However, now
 the number of organisms to be retained can be specified
 exactly. Also, it is possible to specify whether any of these
 organisms may be dead or not.
 
 Parameters:
   transfer size (int) default: 1
     The number of organisms to retain. If there are fewer living
     organisms than the specified transfer size, then all living
     organisms are retained.
   ignore deads (int) default: 1
     When set to 1, only living organisms are retained. Otherwise,
     every type of organism can be retained.
*/
class cActionSerialTransfer : public cAction
{
private:
  int m_size;
  int m_ignore_deads;
public:
  cActionSerialTransfer(cWorld* world, const cString& args) : cAction(world, args), m_size(1), m_ignore_deads(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_size = largs.PopWord().AsInt();
    if (largs.GetSize()) m_ignore_deads = largs.PopWord().AsInt();
    
    if (m_size < 0) m_size = 1;
  }
  
  const cString GetDescription() { return "SerialTransfer [int transfer_size=1] [int ignore_deads=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().SerialTransfer(m_size, m_ignore_deads);
  }
};



void RegisterPopulationActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionInject>("Inject");
  action_lib->Register<cActionInjectRandom>("InjectRandom");
  action_lib->Register<cActionInjectAll>("InjectAll");
  action_lib->Register<cActionInjectRange>("InjectRange");
  action_lib->Register<cActionInjectSequence>("InjectSequence");

  action_lib->Register<cActionInjectParasite>("InjectParasite");
  action_lib->Register<cActionInjectParasitePair>("InjectParasitePair");

  action_lib->Register<cActionKillProb>("KillProb");
  action_lib->Register<cActionKillProb>("KillRate");
  action_lib->Register<cActionKillRectangle>("KillRectangle");
  action_lib->Register<cActionSerialTransfer>("SerialTransfer");


  // @DMB - The following actions are DEPRECATED aliases - These will be removed in 2.7.
  action_lib->Register<cActionInject>("inject");
  action_lib->Register<cActionInjectRandom>("inject_random");
  action_lib->Register<cActionInjectAll>("inject_all");
  action_lib->Register<cActionInjectRange>("inject_range");
  action_lib->Register<cActionInject>("inject_sequence");

  action_lib->Register<cActionKillProb>("apocalypse");
  action_lib->Register<cActionKillRate>("rate_kill");
  action_lib->Register<cActionKillRectangle>("kill_rectangle");
  action_lib->Register<cActionSerialTransfer>("serial_transfer");
}
