/*
 *  PopulationActions.cc
 *  Avida
 *
 *  Created by David on 6/25/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "PopulationActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cCodeLabel.h"
#include "cGenome.h"
#include "cGenomeUtil.h"
#include "cHardwareManager.h"
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
  
  static const cString GetDescription() { return "Arguments: [string fname=\"START_CREATURE\"] [int cell_id=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cGenomeUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
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
  
  static const cString GetDescription() { return "Arguments: <int length> [int cell_id=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cGenomeUtil::RandomGenome(ctx, m_length, m_world->GetHardwareManager().GetInstSet());
    m_world->GetPopulation().Inject(genome, m_cell_id, m_merit, m_lineage_label, m_neutral_metric);
  }
};


/*
 Injects identical organisms into all cells of the population.
 
 Parameters:
   filename (string)
     The filename of the genotype to load.  If empty (or the keyword
     "START_CREATURE" is given) than the genotype specified in the genesis
     file under "START_CREATURE" is used.
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
  
  static const cString GetDescription() { return "Arguments: [string fname=\"START_CREATURE\"] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cGenome genome = cGenomeUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
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
  
  static const cString GetDescription() { return "Arguments: [string fname=\"START_CREATURE\"] [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectRange has invalid range!");
    } else {
      cGenome genome = cGenomeUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
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
  
  static const cString GetDescription() { return "Arguments: <string sequence> [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
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
  
  static const cString GetDescription() { return "Arguments: <string filename> <string label> [int cell_start=0] [int cell_end=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectParasite has invalid range!");
    } else {
      cGenome genome = cGenomeUtil::LoadGenome(m_filename, m_world->GetHardwareManager().GetInstSet());
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
  
  static const cString GetDescription() { return "Arguments: <string filename_genome> <string filename_parasite> <string label> [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectParasitePair has invalid range!");
    } else {
      cGenome genome = cGenomeUtil::LoadGenome(m_filename_genome, m_world->GetHardwareManager().GetInstSet());
      cGenome parasite = cGenomeUtil::LoadGenome(m_filename_parasite, m_world->GetHardwareManager().GetInstSet());
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
  
  static const cString GetDescription() { return "Arguments: [double probability=0.9]"; }
  
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
 In avida.cfg, when BASE_MERIT_METHOD is set to 6 (Merit prop. to num times MERIT_BONUS_INST is in genome), 
 the merit is incremented by MERIT_BONUS_EFFECT if MERIT_BONUS_EFFECT is positive and decremented by
 MERIT_BONUS_EFFECT if it is negative. For positive values the counting starts at 1, for negative values it starts
 at genome length. This event toggles MERIT_BONUS_EFFECT from positive to negative. This creates an extremely simple
 counting-ones type of dynamic environment.  
*/
class cActionToggleRewardInstruction : public cAction
{
private:
  double m_killprob;
public:
  cActionToggleRewardInstruction(cWorld* world, const cString& args) : cAction(world, args), m_killprob(0.9)
  {
    //pass
    //@JMC: m_killprob is meme that hitchiked when I used gabe's event as an example. need to clean it up. 
  }
  
  static const cString GetDescription() { return "Arguments: [double probability=0.9]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetConfig().MERIT_BONUS_EFFECT.Set(-1* m_world->GetConfig().MERIT_BONUS_EFFECT.Get());
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
  
  static const cString GetDescription() { return "Arguments: <double probability>"; }
  
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
  
  static const cString GetDescription() { return "Arguments: [int x1=0] [int y1=0] [int x2=0] [int y2=0]"; }
  
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
  
  static const cString GetDescription() { return "Arguments: [int transfer_size=1] [int ignore_deads=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().SerialTransfer(m_size, m_ignore_deads);
  }
};


class cActionSetMutProb : public cAction
{
private:
  enum { POINT, COPY, INS, DEL, DIV, DIVIDE, D_INS, D_DEL, PARENT, INJECT, I_INS, I_DEL } m_mut_type;
  double m_prob;
  int m_start;
  int m_end;
  bool m_setconf;
  
public:
  cActionSetMutProb(cWorld* world, const cString& args) : cAction(world, args), m_prob(0.0), m_start(-1), m_end(-1), m_setconf(false)
  {
    cString mutstr("COPY");

    cString largs(args);
    if (largs.GetSize()) mutstr = largs.PopWord().ToUpper();
    if (largs.GetSize()) m_prob = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_end = largs.PopWord().AsInt();
    
    if (mutstr == "POINT") m_mut_type = POINT;
    else if (mutstr == "COPY") m_mut_type = COPY;
    else if (mutstr == "INS" || mutstr == "INSERT") m_mut_type = INS;
    else if (mutstr == "DEL" || mutstr == "DELETE") m_mut_type = DEL;
    else if (mutstr == "DIV") m_mut_type = DIV;
    else if (mutstr == "DIVIDE") m_mut_type = DIVIDE;
    else if (mutstr == "DIVIDE_INS") m_mut_type = D_INS;
    else if (mutstr == "DIVIDE_DEL") m_mut_type = D_DEL;
    else if (mutstr == "PARENT") m_mut_type = PARENT;
    else if (mutstr == "INJECT") m_mut_type = INJECT;
    else if (mutstr == "INJECT_INS") m_mut_type = I_INS;
    else if (mutstr == "INJECT_DEL") m_mut_type = I_DEL;

    if (m_start < 0) { // start == -1  -->  all
      m_setconf = true;
      m_start = 0;
      m_end = m_world->GetPopulation().GetSize();
    }
    if (m_end < 0)  m_end = m_start + 1; // end == -1 --> Only one cell!
    if (m_end < m_start) { // swap order
      int temp = m_start;
      m_start = m_end;
      m_end = temp;
    }
    if (m_end > m_world->GetPopulation().GetSize()) m_end = m_world->GetPopulation().GetSize();
  }
  
  static const cString GetDescription() { return "Arguments: [string mut_type='copy'] [double prob=0.0] [int start_cell=-1] [int end_cell=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_setconf) {
      switch (m_mut_type) {
        case POINT: m_world->GetConfig().POINT_MUT_PROB.Set(m_prob); break;
        case COPY: m_world->GetConfig().COPY_MUT_PROB.Set(m_prob); break;
        case INS: m_world->GetConfig().INS_MUT_PROB.Set(m_prob); break;
        case DEL: m_world->GetConfig().DEL_MUT_PROB.Set(m_prob); break;
        case DIV: m_world->GetConfig().DIV_MUT_PROB.Set(m_prob); break;
        case DIVIDE: m_world->GetConfig().DIVIDE_MUT_PROB.Set(m_prob); break;
        case D_INS: m_world->GetConfig().DIVIDE_INS_PROB.Set(m_prob); break;
        case D_DEL: m_world->GetConfig().DIVIDE_DEL_PROB.Set(m_prob); break;
        case PARENT: m_world->GetConfig().PARENT_MUT_PROB.Set(m_prob); break;
        case INJECT: m_world->GetConfig().INJECT_MUT_PROB.Set(m_prob); break;
        case I_INS: m_world->GetConfig().INJECT_INS_PROB.Set(m_prob); break;
        case I_DEL: m_world->GetConfig().INJECT_DEL_PROB.Set(m_prob); break;
        default:
          return;
      }
    }

    switch (m_mut_type) {
      case POINT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointMutProb(m_prob); break;
      case COPY: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyMutProb(m_prob); break;
      case INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInsMutProb(m_prob); break;
      case DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDelMutProb(m_prob); break;
      case DIV: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivMutProb(m_prob); break;
      case DIVIDE: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideMutProb(m_prob); break;
      case D_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideInsProb(m_prob); break;
      case D_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideDelProb(m_prob); break;
      case PARENT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentMutProb(m_prob); break;
      case INJECT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectMutProb(m_prob); break;
      case I_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectInsProb(m_prob); break;
      case I_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectDelProb(m_prob); break;
      default:
        return;
    }
  }
};

class cActionModMutProb : public cAction
{
private:
  enum { POINT, COPY, INS, DEL, DIV, DIVIDE, D_INS, D_DEL, PARENT, INJECT, I_INS, I_DEL } m_mut_type;
  double m_prob;
  int m_start;
  int m_end;
  bool m_setconf;
  
public:
  cActionModMutProb(cWorld* world, const cString& args) : cAction(world, args), m_prob(0.0), m_start(-1), m_end(-1), m_setconf(false)
  {
      cString mutstr("COPY");
      
      cString largs(args);
      if (largs.GetSize()) mutstr = largs.PopWord().ToUpper();
      if (largs.GetSize()) m_prob = largs.PopWord().AsDouble();
      if (largs.GetSize()) m_start = largs.PopWord().AsInt();
      if (largs.GetSize()) m_end = largs.PopWord().AsInt();
      
      if (mutstr == "POINT") m_mut_type = POINT;
      else if (mutstr == "COPY") m_mut_type = COPY;
      else if (mutstr == "INS" || mutstr == "INSERT") m_mut_type = INS;
      else if (mutstr == "DEL" || mutstr == "DELETE") m_mut_type = DEL;
      else if (mutstr == "DIV") m_mut_type = DIV;
      else if (mutstr == "DIVIDE") m_mut_type = DIVIDE;
      else if (mutstr == "DIVIDE_INS") m_mut_type = D_INS;
      else if (mutstr == "DIVIDE_DEL") m_mut_type = D_DEL;
      else if (mutstr == "PARENT") m_mut_type = PARENT;
      else if (mutstr == "INJECT") m_mut_type = INJECT;
      else if (mutstr == "INJECT_INS") m_mut_type = I_INS;
      else if (mutstr == "INJECT_DEL") m_mut_type = I_DEL;
      
      if (m_start < 0) { // start == -1  -->  all
        m_setconf = true;
        m_start = 0;
        m_end = m_world->GetPopulation().GetSize();
      }
      if (m_end < 0)  m_end = m_start + 1; // end == -1 --> Only one cell!
      if (m_end < m_start) { // swap order
        int temp = m_start;
        m_start = m_end;
        m_end = temp;
      }
      if (m_end > m_world->GetPopulation().GetSize()) m_end = m_world->GetPopulation().GetSize();
  }
  
  static const cString GetDescription() { return "Arguments: [string mut_type='copy'] [double prob=0.0] [int start_cell=-1] [int end_cell=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    double prob = m_prob;

    switch (m_mut_type) {
      case POINT: prob += m_world->GetConfig().POINT_MUT_PROB.Get(); break;
      case COPY: prob += m_world->GetConfig().COPY_MUT_PROB.Get(); break;
      case INS: prob += m_world->GetConfig().INS_MUT_PROB.Get(); break;
      case DEL: prob += m_world->GetConfig().DEL_MUT_PROB.Get(); break;
      case DIV: prob += m_world->GetConfig().DIV_MUT_PROB.Get(); break;
      case DIVIDE: prob += m_world->GetConfig().DIVIDE_MUT_PROB.Get(); break;
      case D_INS: prob += m_world->GetConfig().DIVIDE_INS_PROB.Get(); break;
      case D_DEL: prob += m_world->GetConfig().DIVIDE_DEL_PROB.Get(); break;
      case PARENT: prob += m_world->GetConfig().PARENT_MUT_PROB.Get(); break;
      case INJECT: prob += m_world->GetConfig().INJECT_MUT_PROB.Get(); break;
      case I_INS: prob += m_world->GetConfig().INJECT_INS_PROB.Get(); break;
      case I_DEL: prob += m_world->GetConfig().INJECT_DEL_PROB.Get(); break;
      default:
        return;
    }
    
    if (m_setconf) {
      switch (m_mut_type) {
        case POINT: m_world->GetConfig().POINT_MUT_PROB.Set(prob); break;
        case COPY: m_world->GetConfig().COPY_MUT_PROB.Set(prob); break;
        case INS: m_world->GetConfig().INS_MUT_PROB.Set(prob); break;
        case DEL: m_world->GetConfig().DEL_MUT_PROB.Set(prob); break;
        case DIV: m_world->GetConfig().DIV_MUT_PROB.Set(prob); break;
        case DIVIDE: m_world->GetConfig().DIVIDE_MUT_PROB.Set(prob); break;
        case D_INS: m_world->GetConfig().DIVIDE_INS_PROB.Set(prob); break;
        case D_DEL: m_world->GetConfig().DIVIDE_DEL_PROB.Set(prob); break;
        case PARENT: m_world->GetConfig().PARENT_MUT_PROB.Set(prob); break;
        case INJECT: m_world->GetConfig().INJECT_MUT_PROB.Set(prob); break;
        case I_INS: m_world->GetConfig().INJECT_INS_PROB.Set(prob); break;
        case I_DEL: m_world->GetConfig().INJECT_DEL_PROB.Set(prob); break;
        default:
          return;
      }
    }
    
    switch (m_mut_type) {
      case POINT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointMutProb(prob); break;
      case COPY: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyMutProb(prob); break;
      case INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInsMutProb(prob); break;
      case DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDelMutProb(prob); break;
      case DIV: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivMutProb(prob); break;
      case DIVIDE: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideMutProb(prob); break;
      case D_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideInsProb(prob); break;
      case D_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideDelProb(prob); break;
      case PARENT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentMutProb(prob); break;
      case INJECT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectMutProb(prob); break;
      case I_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectInsProb(prob); break;
      case I_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectDelProb(prob); break;
      default:
        return;
    }
  }
};


class cActionZeroMuts : public cAction
{
public:
  cActionZeroMuts(cWorld* world, const cString& args) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext& ctx)
  {
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      m_world->GetPopulation().GetCell(i).MutationRates().Clear();
    }
  }
};


/*
 Compete all of the demes using a basic genetic algorithm approach. Fitness
 of each deme is determined differently depending on the competition_type: 
 0: deme fitness = 1 (control, random deme selection)
 1: deme fitness = number of births since last competition (default) 
 2: deme fitness = average organism fitness at the current update (uses parent's fitness, so
                     does not work with donations)
 3: deme fitness = average mutation rate at the current update
 4: deme fitness = strong rank selection on (parents) fitness (2^-deme fitness rank)
 5: deme fitness = average organism life (current, not parents) fitness (works with donations)
 6: deme fitness = strong rank selection on life (current, not parents) fitness
*/

class cActionCompeteDemes : public cAction
{
private:
  int m_type;
public:
  cActionCompeteDemes(cWorld* world, const cString& args) : cAction(world, args), m_type(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_type = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int type=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().CompeteDemes(m_type);
  }
};


/* This Action will check if any demes have met the critera to be replicated
   and do so.  There are several bases this can be checked on:

    'all'       - ...all non-empty demes in the population.
    'full_deme' - ...demes that have been filled up.
    'corners'   - ...demes with upper left and lower right corners filled.
*/

class cActionReplicateDemes : public cAction
{
private:
  int m_rep_trigger;
public:
  cActionReplicateDemes(cWorld* world, const cString& args) : cAction(world, args), m_rep_trigger(-1)
  {
    cString largs(args);
    cString in_trigger("full_deme");
    if (largs.GetSize()) in_trigger = largs.PopWord();

    if (in_trigger == "all") m_rep_trigger = 0;
    else if (in_trigger == "full_deme") m_rep_trigger = 1;
    else if (in_trigger == "corners") m_rep_trigger = 2;
    else {
      cString err("Unknown replication trigger '");
      err += in_trigger;
      err += "' in ReplicatDemes action.";
      m_world->GetDriver().RaiseException(err);
      return;
    }
  }
  
  static const cString GetDescription() { return "Arguments: [string trigger=full_deme]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().ReplicateDemes(m_rep_trigger);
  }
};


/*
   This action will determine if any demes have filled up, and if so move half
   of the members into a new deme.  Specifically, it will leave the even
   numbered cells (0,2,4, etc.) and take the odd numbered ones (1,3,5, etc.)

   @CAO This next part should be configurable
   All replicated organisms will have their merit recalculated given the full
   list of completed tasks, and assigned to all offspring *and* all parents.

   This action should be used in combination with:
      BIRTH_METHOD 8 (always repoduce into id+1)
      BASE_MERIT_METHOD 0 (Constant base merit)
      BASE_CONST_MERIT 0 (Use a base merit of zero, hence all merits = 0)

   These settings will make sure that all merit will be set by this action.
*/

class cActionDivideDemes : public cAction
{
private:
public:
  cActionDivideDemes(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    // Nothing to do here yet....
  }
  
  static const cString GetDescription() { return "No arguments (yet!)"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().DivideDemes();
  }
};


/*
 Designed to serve as a control for the compete_demes. Each deme is 
 copied into itself and the parameters reset. 
*/
class cActionResetDemes : public cAction
{
public:
  cActionResetDemes(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().ResetDemes();
  }
};


class cActionCopyDeme : public cAction
{
private:
  int m_id1;
  int m_id2;
public:
  cActionCopyDeme(cWorld* world, const cString& args) : cAction(world, args), m_id1(0), m_id2(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_id1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_id2 = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int src_id> <int dest_id>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().CopyDeme(m_id1, m_id2);
  }
};


/*
 Remove the connections between cells along a column in an avida grid.
 
 Arguments:
   col_id:  indicates the number of columns to the left of the cut.
            default (or -1) = cut population in half
   min_row: First row to start cutting from
            default = 0
   max_row: Last row to cut to
            default (or -1) = last row in population.
*/
class cActionSeverGridCol : public cAction
{
private:
  int m_id;
  int m_min;
  int m_max;
public:
  cActionSeverGridCol(cWorld* world, const cString& args) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
  {
    cString largs(args);
    if (largs.GetSize()) m_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_min = largs.PopWord().AsInt();
    if (largs.GetSize()) m_max = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int col_id=-1] [int min_row=0] [int max_row=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (m_id == -1) m_id = world_x / 2;
    if (m_max == -1) m_max = world_y;
    if (m_id < 0 || m_id >= world_x) {
      cString err = cStringUtil::Stringf("Column ID %d out of range for SeverGridCol", m_id);
      m_world->GetDriver().RaiseException(err);
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int row_id = m_min; row_id < m_max; row_id++) {
      int idA = row_id * world_x + m_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);
      int idA0 = GridNeighbor(idA, world_x, world_y,  0, -1);
      int idA1 = GridNeighbor(idA, world_x, world_y,  0,  1);
      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y, -1,  1);
      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);
      tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
      tList<cPopulationCell>& cellB_list = cellB.ConnectionList();
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

/*
 Remove the connections between cells along a column in an avida grid.

 Arguments:
   row_id:  indicates the number of rows above the cut.
            default (or -1) = cut population in half
   min_col: First row to start cutting from
            default = 0
   max_col: Last row to cut to
            default (or -1) = last row in population.
*/
class cActionSeverGridRow : public cAction
{
private:
  int m_id;
  int m_min;
  int m_max;
public:
  cActionSeverGridRow(cWorld* world, const cString& args) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
  {
    cString largs(args);
    if (largs.GetSize()) m_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_min = largs.PopWord().AsInt();
    if (largs.GetSize()) m_max = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int row_id=-1] [int min_col=0] [int max_col=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (m_id == -1) m_id = world_y / 2;
    if (m_max == -1) m_max = world_x;
    if (m_id < 0 || m_id >= world_y) {
      cString err = cStringUtil::Stringf("Row ID %d out of range for SeverGridRow", m_id);
      m_world->GetDriver().RaiseException(err);
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int col_id = m_min; col_id < m_max; col_id++) {
      int idA = col_id * world_y + m_id;
      int idB  = GridNeighbor(idA, world_x, world_y,  0, -1);
      int idA0 = GridNeighbor(idA, world_x, world_y, -1,  0);
      int idA1 = GridNeighbor(idA, world_x, world_y,  1,  0);
      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y,  1, -1);
      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);
      tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
      tList<cPopulationCell>& cellB_list = cellB.ConnectionList();
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB0));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB1));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA0));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA1));
    }
  }
};


/*
 Join the connections between cells along a column in an avida grid.

 Arguments:
   col_id:  indicates the number of columns to the left of the joining.
            default (or -1) = join population halves.
   min_row: First row to start joining from
            default = 0
   max_row: Last row to join to
            default (or -1) = last row in population.
*/
class cActionJoinGridCol : public cAction
{
private:
  int m_id;
  int m_min;
  int m_max;
public:
  cActionJoinGridCol(cWorld* world, const cString& args) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
  {
    cString largs(args);
    if (largs.GetSize()) m_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_min = largs.PopWord().AsInt();
    if (largs.GetSize()) m_max = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int col_id=-1] [int min_row=0] [int max_row=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (m_id == -1) m_id = world_x / 2;
    if (m_max == -1) m_max = world_y;
    if (m_id < 0 || m_id >= world_x) {
      cString err = cStringUtil::Stringf("Column ID %d out of range for JoinGridCol", m_id);
      m_world->GetDriver().RaiseException(err);
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int row_id = m_min; row_id < m_max; row_id++) {
      int idA = row_id * world_x + m_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);
      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);
      cPopulationCell& cellA0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0, -1));
      cPopulationCell& cellA1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0,  1));
      cPopulationCell& cellB0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
      cPopulationCell& cellB1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  1));
      tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
      tList<cPopulationCell>& cellB_list = cellB.ConnectionList();
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
      if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);
      if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
    }
  }
};


/*
 Remove the connections between cells along a column in an avida grid.

 Arguments:
   row_id:  indicates the number of rows abovef the cut.
            default (or -1) = cut population in half
   min_col: First row to start cutting from
            default = 0
   max_col: Last row to cut to
            default (or -1) = last row in population.
*/
class cActionJoinGridRow : public cAction
{
private:
  int m_id;
  int m_min;
  int m_max;
public:
  cActionJoinGridRow(cWorld* world, const cString& args) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
  {
    cString largs(args);
    if (largs.GetSize()) m_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_min = largs.PopWord().AsInt();
    if (largs.GetSize()) m_max = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int row_id=-1] [int min_col=0] [int max_col=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (m_id == -1) m_id = world_y / 2;
    if (m_max == -1) m_max = world_x;
    if (m_id < 0 || m_id >= world_y) {
      cString err = cStringUtil::Stringf("Row ID %d out of range for JoinGridRow", m_id);
      m_world->GetDriver().RaiseException(err);
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int col_id = m_min; col_id < m_max; col_id++) {
      int idA = col_id * world_y + m_id;
      int idB  = GridNeighbor(idA, world_x, world_y, 0, -1);
      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);
      cPopulationCell& cellA0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  0));
      cPopulationCell& cellA1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1,  0));
      cPopulationCell& cellB0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
      cPopulationCell& cellB1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1, -1));
      tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
      tList<cPopulationCell>& cellB_list = cellB.ConnectionList();
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
      if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);
      if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
    }
  }
};


class cActionConnectCells : public cAction
{
private:
  int m_a_x;
  int m_a_y;
  int m_b_x;
  int m_b_y;
public:
  cActionConnectCells(cWorld* world, const cString& args) : cAction(world, args), m_a_x(-1), m_a_y(-1), m_b_x(-1), m_b_y(-1)
  {
    cString largs(args);
    if (largs.GetSize()) m_a_x = largs.PopWord().AsInt();
    if (largs.GetSize()) m_a_y = largs.PopWord().AsInt();
    if (largs.GetSize()) m_b_x = largs.PopWord().AsInt();
    if (largs.GetSize()) m_b_y = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int cellA_x> <int cellA_y> <int cellB_x> <int cellB_y>"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (m_a_x < 0 || m_a_x >= world_x || m_a_y < 0 || m_a_y >= world_y ||
        m_b_x < 0 || m_b_x >= world_x || m_b_y < 0 || m_b_y >= world_y) {
      m_world->GetDriver().RaiseException("ConnectCells cell out of range");
      return;
    }
    int idA = m_a_y * world_x + m_a_x;
    int idB = m_b_y * world_x + m_b_x;
    cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
    cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);
    tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
    tList<cPopulationCell>& cellB_list = cellB.ConnectionList();
    cellA_list.PushRear(&cellB);
    cellB_list.PushRear(&cellA);
  }
};


class cActionDisconnectCells : public cAction
{
private:
  int m_a_x;
  int m_a_y;
  int m_b_x;
  int m_b_y;
public:
  cActionDisconnectCells(cWorld* world, const cString& args) : cAction(world, args), m_a_x(-1), m_a_y(-1), m_b_x(-1), m_b_y(-1)
  {
    cString largs(args);
    if (largs.GetSize()) m_a_x = largs.PopWord().AsInt();
    if (largs.GetSize()) m_a_y = largs.PopWord().AsInt();
    if (largs.GetSize()) m_b_x = largs.PopWord().AsInt();
    if (largs.GetSize()) m_b_y = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int cellA_x> <int cellA_y> <int cellB_x> <int cellB_y>"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (m_a_x < 0 || m_a_x >= world_x || m_a_y < 0 || m_a_y >= world_y ||
        m_b_x < 0 || m_b_x >= world_x || m_b_y < 0 || m_b_y >= world_y) {
      m_world->GetDriver().RaiseException("DisconnectCells cell out of range");
      return;
    }
    int idA = m_a_y * world_x + m_a_x;
    int idB = m_b_y * world_x + m_b_x;
    cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
    cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);
    tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
    tList<cPopulationCell>& cellB_list = cellB.ConnectionList();
    cellA_list.Remove(&cellB);
    cellB_list.Remove(&cellA);
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
  action_lib->Register<cActionToggleRewardInstruction>("ToggleRewardInstruction");
  action_lib->Register<cActionKillProb>("KillRate");
  action_lib->Register<cActionKillRectangle>("KillRectangle");
  action_lib->Register<cActionSerialTransfer>("SerialTransfer");

  action_lib->Register<cActionSetMutProb>("SetMutProb");
  action_lib->Register<cActionModMutProb>("ModMutProb");
  action_lib->Register<cActionZeroMuts>("ZeroMuts");

  action_lib->Register<cActionCompeteDemes>("CompeteDemes");
  action_lib->Register<cActionReplicateDemes>("ReplicateDemes");
  action_lib->Register<cActionDivideDemes>("DivideDemes");
  action_lib->Register<cActionResetDemes>("ResetDemes");
  action_lib->Register<cActionCopyDeme>("CopyDeme");
  
  action_lib->Register<cActionSeverGridCol>("SeverGridCol");
  action_lib->Register<cActionSeverGridRow>("SeverGridRow");
  action_lib->Register<cActionJoinGridCol>("JoinGridCol");
  action_lib->Register<cActionJoinGridRow>("JoinGridRow");

  action_lib->Register<cActionConnectCells>("ConnectCells");
  action_lib->Register<cActionDisconnectCells>("DisconnectCells");

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

  action_lib->Register<cActionZeroMuts>("zero_muts");
  
  action_lib->Register<cActionCompeteDemes>("compete_demes");
  action_lib->Register<cActionReplicateDemes>("replicate_demes");
  action_lib->Register<cActionResetDemes>("reset_demes");
  action_lib->Register<cActionCopyDeme>("copy_deme");
  
  action_lib->Register<cActionSeverGridCol>("sever_grid_col");
  action_lib->Register<cActionSeverGridRow>("sever_grid_row");
  action_lib->Register<cActionJoinGridCol>("join_grid_col");
  action_lib->Register<cActionJoinGridRow>("join_grid_row");

  action_lib->Register<cActionConnectCells>("connect_cells");
  action_lib->Register<cActionDisconnectCells>("disconnect_cells");
}
