/*
 *  PopulationActions.cc
 *  Avida
 *
 *  Created by David on 6/25/06.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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
#include "cOrgMessagePredicate.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cWorld.h"
#include "cOrganism.h"
#include "cEnvironment.h"

#include <map>
#include <set>
#include <numeric>
#include <algorithm>

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
 Injects randomly generated genomes into the entire population, plus a repro inst on the end,
 with the caveat that any instructions set to 0 probability of mutating into the genome 
 are not included.
 
 Parameters:
   length (integer) [required]
     Number of instructions in the randomly generated genome (actual length will be +1 with repro).
   merit (double) default: -1
     The initial merit of the organism. If set to -1, this is ignored.
   lineage label (integer) default: 0
     An integer that marks all descendants of this organism.
   neutral metric (double) default: 0
     A double value that randomly drifts over time.
*/
class cActionInjectAllRandomRepro : public cAction
{
private:
  int m_length;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
  int m_sex;
public:
  cActionInjectAllRandomRepro(cWorld* world, const cString& args) : cAction(world, args), m_merit(-1), m_lineage_label(0), m_neutral_metric(0), m_sex(0)
  {
    cString largs(args);
    m_length = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_sex = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int length> [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
	  for (int i = 0; i < m_world->GetPopulation().GetSize(); i++)
	  {
	    cGenome genome;
	    if (m_sex)
	       genome = cGenomeUtil::RandomGenomeWithoutZeroRedundantsPlusReproSex(ctx, m_length, m_world->GetHardwareManager().GetInstSet());
	    else
	       genome = cGenomeUtil::RandomGenomeWithoutZeroRedundantsPlusRepro(ctx, m_length, m_world->GetHardwareManager().GetInstSet());
	    m_world->GetPopulation().Inject(genome, i, m_merit, m_lineage_label, m_neutral_metric);
	  }
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
      m_world->GetDriver().NotifyWarning("InjectSequence has invalid range!");
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
 Injects identical organisms into a range of cells of the population with a specified divide mut rate (per site).
 
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
class cActionInjectSequenceWithDivMutRate : public cAction
{
private:
  cString m_sequence;
  int m_cell_start;
  int m_cell_end;
  double m_merit;
  double m_div_mut_rate;
  int m_lineage_label;
  double m_neutral_metric;
  
  
public:
  cActionInjectSequenceWithDivMutRate(cWorld* world, const cString& args)
    : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_div_mut_rate(0.0), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    m_sequence = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_div_mut_rate = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  static const cString GetDescription() { return "Arguments: <string sequence> [int cell_start=0] [int cell_end=-1] [double div_mut_rate=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      m_world->GetDriver().NotifyWarning("InjectSequenceWithDivMutRate has invalid range!");
    } else {
      cGenome genome(m_sequence);
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, i, m_merit, m_lineage_label, m_neutral_metric);
	m_world->GetPopulation().GetCell(i).GetOrganism()->MutationRates().SetDivMutProb(m_div_mut_rate);
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



/*! Injects an organism into all demes in the population. 

Parameters:
filename (string):
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
class cActionInjectDemes : public cAction
{
private:
  cString m_filename;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
    cActionInjectDemes(cWorld* world, const cString& args) : cAction(world, args), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
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
    if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
      for(int i=1; i<m_world->GetPopulation().GetNumDemes(); ++i) {  // first org has already been injected
        m_world->GetPopulation().Inject(genome,
                                        m_world->GetPopulation().GetDeme(i).GetCellID(0),
                                        m_merit, m_lineage_label, m_neutral_metric);
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
      }
    } else {
      for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
        // WARNING: initial ancestor has already be injected into the population
        //           calling this will overwrite it.
        m_world->GetPopulation().Inject(genome,
                                        m_world->GetPopulation().GetDeme(i).GetCellID(0),
                                        m_merit, m_lineage_label, m_neutral_metric);
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
        
      }
    }
  }
};


/*! Injects one or more organisms into all demes in the population at a specified cell.
 *  Note: This gets the genotype from the germline, so germline use is required
 *  Note: one organism is inserted each time this is called, and it will stop
 *        when the given number of organisms has been injected.

  Parameters:
    - number of orgs (int): number of organisms to inject into each deme - default 1
    - nest cell (int): relative cell id into which organism is injected - default 0
    - merit (double): The initial merit of the organism. If set to -1, this is ignored - default -1
    - lineage label (integer): An integer that marks all descendants of this organism  - default 0
    - neutral metric (double):  A double value that randomly drifts over time - default 0

*/

class cActionInjectDemesFromNest : public cAction
{
private:
  int m_num_orgs;
  int m_nest_cellid;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
  
public:
  cActionInjectDemesFromNest(cWorld* world, const cString& args):
    cAction(world, args),
    m_num_orgs(1),
    m_nest_cellid(0),
    m_merit(-1),
    m_lineage_label(0),
    m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_num_orgs = largs.PopWord().AsInt();
    if (largs.GetSize()) m_nest_cellid = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();

    assert(m_num_orgs <= m_world->GetPopulation().GetSize());
    assert(m_nest_cellid > -1);
    assert(m_nest_cellid < m_world->GetPopulation().GetSize());
  } //End cActionInjectDemesFromNest constructor

  static const cString GetDescription() { return "Arguments: [int num_orgs=1] [int nest_cellid=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
    for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {

      // Each deme will have initially had one organism injected.  If this
      // is the first injection and energy is used, increment the injected
      // count (the initial injection wasn't counted) and skip the first deme
      // so that the energies don't get messed up.
      if( (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) &&
          (m_world->GetPopulation().GetDeme(i).GetInjectedCount() == 0) ) {
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
        continue;
      }

      if(m_world->GetPopulation().GetDeme(i).GetInjectedCount() < m_num_orgs) {
        m_world->GetPopulation().Inject(m_world->GetPopulation().GetDeme(i).GetGermline().GetLatest(),
                                        m_world->GetPopulation().GetDeme(i).GetCellID(m_nest_cellid),
                                        m_merit, m_lineage_label, m_neutral_metric);
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
      }

    } //End iterating through demes

  } //End Process()

}; //End cActionInjectDemesFromNest



/*! Injects one or more organisms into all demes in a randomly-chosen cell.
 *  Note: This gets the genotype from the germline, so germline use is required
 *  Note: one organism is inserted each time this is called, and it will stop
 *        when the given number of organisms has been injected.

  Parameters:
    - number of orgs (int): number of organisms to inject into each deme - default 1
    - merit (double): The initial merit of the organism. If set to -1, this is ignored - default -1
    - lineage label (integer): An integer that marks all descendants of this organism  - default 0
    - neutral metric (double):  A double value that randomly drifts over time - default 0

*/

class cActionInjectDemesRandom : public cAction
{
private:
  int m_num_orgs;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
  
public:
  cActionInjectDemesRandom(cWorld* world, const cString& args):
    cAction(world, args),
    m_num_orgs(1),
    m_merit(-1),
    m_lineage_label(0),
    m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_num_orgs = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    assert(m_num_orgs <= m_world->GetPopulation().GetSize());
  } //End cActionInjectDemesRandom constructor

  static const cString GetDescription() { return "Arguments: [int num_orgs=1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }

  void Process(cAvidaContext& ctx)
  {
    int target_cell, target_cellr;
    int deme_size;

    for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {

      // Each deme will have initially had one organism injected.  If this
      // is the first injection and energy is used, increment the injected
      // count (the initial injection wasn't counted) and skip the first deme
      // so that the energies don't get messed up.
      if( (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) &&
          (m_world->GetPopulation().GetDeme(i).GetInjectedCount() == 0) ) {
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
        continue;
      }

      if(m_world->GetPopulation().GetDeme(i).GetInjectedCount() < m_num_orgs) {
        target_cell = -1;
        target_cellr = -1;
        deme_size = m_world->GetPopulation().GetDeme(i).GetSize();

        // Find a random, unoccupied cell to use. Assumes one exists.
        do {
          target_cellr = m_world->GetRandom().GetInt(0, deme_size-1);
          target_cell = m_world->GetPopulation().GetDeme(i).GetCellID(target_cellr);
        } while (m_world->GetPopulation().GetCell(target_cell).IsOccupied());

        assert(target_cell > -1);
        assert(target_cell < m_world->GetPopulation().GetSize());

        m_world->GetPopulation().Inject(m_world->GetPopulation().GetDeme(i).GetGermline().GetLatest(),
                                        target_cell, m_merit,
                                        m_lineage_label, m_neutral_metric);
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
      } //End if there are still orgs to be inserted

    } //End iterating through demes

  } //End Process()

}; //End cActionInjectDemesRandom



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
public:
  cActionToggleRewardInstruction(cWorld* world, const cString& args) : cAction(world, args) {}
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetConfig().MERIT_BONUS_EFFECT.Set(-1* m_world->GetConfig().MERIT_BONUS_EFFECT.Get());
  }
};

/*
 In avida.cfg, when BASE_MERIT_METHOD is set to 6 (Merit prop. to num times MERIT_BONUS_INST is in genome), 
 the merit is incremented by MERIT_BONUS_EFFECT if MERIT_BONUS_EFFECT is positive and decremented by
 MERIT_BONUS_EFFECT if it is negative. For positive values the counting starts at 1, for negative values it starts
 at genome length. This event addes fitness valleys to this extremely simple counting-ones type of dynamic environment.
 Orgs that have #_merit_bonus_inst_in_genome >=FITNESS_VALLEY_START && <= FITNESS_VALLEY_STOP will have a fitness of one (the lowest).
 Example.   FITNESS_VALLEY_START = 5, FITNESS_VALLEY_STOP = 7. orgs with 5, 6, or 7 MERIT_BONUS_INST in their genome have fitness = 1.
 Specifically, this event creates these valleys or takes them away. 
*/

class cActionToggleFitnessValley : public cAction
{
private:
public:
  cActionToggleFitnessValley(cWorld* world, const cString& args) : cAction(world, args) {}
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    if(m_world->GetConfig().FITNESS_VALLEY.Get()) {m_world->GetConfig().FITNESS_VALLEY.Set(0);}
    else{m_world->GetConfig().FITNESS_VALLEY.Set(1);}
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

class cActionSetMigrationRate : public cAction
{
private:
    double m_rate;

public:
    cActionSetMigrationRate(cWorld* world, const cString& args) : cAction(world, args), m_rate(0.0)
    {
	cString largs(args);
	if(largs.GetSize()) m_rate = largs.PopWord().AsDouble();
    }

    static const cString GetDescription() { return "Arguments: [double rate=0.0]"; }

    void Process(cAvidaContext& ctx)
    {
	m_world->GetConfig().MIGRATION_RATE.Set(m_rate);
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


/*! This action enables the tracking of all messages that are sent in each deme. */
class cActionTrackAllMessages : public cAction {
public:
  cActionTrackAllMessages(cWorld* world, const cString& args) : cAction(world, args) { }

  static const cString GetDescription() { return "No Arguments"; }
	
  void Process(cAvidaContext& ctx) {
		s_pred = new cOrgMessagePred_AllData(m_world);
		m_world->GetStats().AddMessagePredicate(s_pred);
  }
	
private:
	static cOrgMessagePred_AllData* s_pred;
};

cOrgMessagePred_AllData* cActionTrackAllMessages::s_pred=0;


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


/*! Assign a random identifier to the data for each cell and save those IDs for later
 use, respecting deme boundaries.
 
 This is a little hackish; feel free to modify.
 */
class cAssignRandomCellData : public cAction { 
public:
  typedef std::map<int, std::set<int> > DataMap;
	
  cAssignRandomCellData(cWorld* world, const cString& args) : cAction(world, args) { }
	
	virtual ~cAssignRandomCellData() { }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  virtual void Process(cAvidaContext& ctx) {
		deme_to_id.clear();
    for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
      cDeme& deme = m_world->GetPopulation().GetDeme(i);
      for(int j=0; j<deme.GetSize(); ++j) {
				// Assign random data to each cell:
        int d = m_world->GetRandom().GetInt(INT_MAX);
        deme.GetCell(j).SetCellData(d);
				// Save that data by deme in the map:
        deme_to_id[deme.GetID()].insert(d);
      }
    }
  }
  
  static bool IsCellDataInDeme(int data, const cDeme& deme) { 
    DataMap::iterator i = deme_to_id.find(deme.GetID());
    return i->second.find(data) != i->second.end();
  }
	
	static void ReplaceCellData(int old_data, int new_data, const cDeme& deme) {
		// Find all cells in the deme that hold the old data, and replace it with the new.
		for(int i=0; i<deme.GetSize(); ++i) {
			if(deme.GetCell(i).GetCellData() == old_data) {
				deme.GetCell(i).SetCellData(new_data);
			}
		}
		// Update the data map.
		DataMap::iterator i = deme_to_id.find(deme.GetID());
		i->second.erase(old_data);
		i->second.insert(new_data);
	}
	
	static const std::set<int>& GetDataInDeme(const cDeme& deme) {
		return deme_to_id[deme.GetID()];
	}
  
protected:
  static std::map<int, std::set<int> > deme_to_id; //!< Map of deme ID -> set of all cell data in that deme.
};

//! Definition for static data.
cAssignRandomCellData::DataMap cAssignRandomCellData::deme_to_id;


/*! An abstract base class to ease the development of new deme competition fitness functions.
 
 This base class does the bookkeeping associated with deme competitions, including calling a virtual
 function to calculate the fitness of each deme at the right time, collecting those fitness values in
 a vector, and finally invoking the deme competition.
 */
class cAbstractCompeteDemes : public cAction {
public:
  //! Constructor.
  cAbstractCompeteDemes(cWorld* world, const cString& args) : cAction(world, args) { }
  //! Destructor.
  virtual ~cAbstractCompeteDemes() { }
  
  /*! Compete all demes with each other.
   
   For deme competition, what we do here is iterate over every deme and calculate its 
   fitness according to the subclass-defined fitness function.  The resulting vector of
   fitness values is then passed to cPopulation::CompeteDemes for fitness-proportional
   selection, and then each deme is piped through the standard battery of config options.
   
   Note that each fitness value must be >= 0.0, and the sum of all fitnesses must 
   be > 0.0.
   */
  virtual void Process(cAvidaContext& ctx) {
    std::vector<double> fitness;
    for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
      fitness.push_back(Fitness(m_world->GetPopulation().GetDeme(i)));
      assert(fitness.back() >= 0.0);
    }
    m_world->GetPopulation().CompeteDemes(fitness);
  }

  /*! Deme fitness function, to be overriden by specific types of deme competition.
   */
  virtual double Fitness(const cDeme& deme) = 0;
};


/*! An abstract base class to ease the development of new deme competition fitness functions 
 that require a per-update monitoring of each deme.
 
 The idea here is that in addition to normal deme competition, we also have periodic housekeeping to do.
 This housekeeping may or may not influence the final fitness value (once deme competition is triggered).
 
 In the events file, when using any deme competition strategy that uses cAbstractMonitoringCompeteDemes,
 the execution of the event corresponds to when the Update(...) method will be called, while the integer parameter
 to the event is the period (in updates) of deme competition.  So this:
 
 u 1:1:end SomeFormOfCompeteDemes 400
 
 will call Update(...) every update, but only calls Fitness(...) every 400 updates.
 
 WARNING: The compete_period argument MUST be an even multiple of the update period!
 If it isn't, competition won't ever be triggered.  We're not preventing this, 'cause
 there might be good reasons you'd want that...
 */
class cAbstractMonitoringCompeteDemes : public cAbstractCompeteDemes {
public:
	//! Constructor.
	cAbstractMonitoringCompeteDemes(cWorld* world, const cString& args) : cAbstractCompeteDemes(world, args), _compete_period(100) {
		if(args.GetSize()) {
			cString largs(args);
			_compete_period = largs.PopWord().AsInt();
		}
  }
  
	//! Destructor.
	virtual ~cAbstractMonitoringCompeteDemes() { }
	
	/*! Update each deme, and possibly compete them.

	 Calls Update(...) on every execution, but only calls Fitness(...) when the current
	 update is an even multiple of the competition period.
	 */	
	virtual void Process(cAvidaContext& ctx) {
		_update_fitness.resize(m_world->GetPopulation().GetNumDemes());													 
    for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
			_update_fitness[i] += Update(m_world->GetPopulation().GetDeme(i));
		}
		
		if((m_world->GetStats().GetUpdate() % _compete_period) == 0) {
			std::vector<double> fitness;
			for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
				fitness.push_back(pow(_update_fitness[i] + Fitness(m_world->GetPopulation().GetDeme(i)), 2.0));
				assert(fitness.back() >= 0.0);
			}			
			m_world->GetPopulation().CompeteDemes(fitness);
			_update_fitness.clear();
		}
	}
	
	//! Called on each action invocation, *including* immediately prior to fitness calculation.
	virtual double Update(cDeme& deme) = 0;
	
protected:
	int _compete_period; //!< Period at which demes compete.
	std::vector<double> _update_fitness; //!< Running sum of returns from Update(cDeme).
};


/*! This class contains methods that are useful for consensus-related problems.
 */
class ConsensusSupport {
public:
	ConsensusSupport() { }
	virtual ~ConsensusSupport() { }
	
	/*! Returns a pair (support, opinion), where support is equal to the maximum 
	 number of organisms within the deme that have set their opinion to the same value,
	 and opinion is the value that they have set their (shared) opinion to.
	 */	 
	virtual std::pair<unsigned int, cOrganism::Opinion> max_support(const cDeme& deme) {
		typedef std::vector<cOrganism::Opinion> OpinionList;
		OpinionList opinions;
		// For each organism in the deme:
		for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
			if((org != 0) && org->HasOpinion()) {
				// Get its current opinion (we don't care about the date here):
				opinions.push_back(org->GetOpinion().first);
			}
		}
		
		// Go through the list of opinions, count & filter:
		typedef std::map<cOrganism::Opinion, unsigned int> OpinionMap;
		unsigned int support=0;
		cOrganism::Opinion opinion=0;
		OpinionMap opinion_counts;
		for(OpinionList::iterator i=opinions.begin(); i!=opinions.end(); ++i) {
			// filter:
			if(cAssignRandomCellData::IsCellDataInDeme(*i, deme)) {
				// count:
				++opinion_counts[*i];
				// find support:
				if(opinion_counts[*i] > support) {
					support = opinion_counts[*i];
					opinion = *i;
				}
			}
		}		
		return std::make_pair(support, opinion);
	}	
};


/*! This class rewards for solving the Iterated Consensus Dilemma, where organisms
 are to repeatedly reach consensus on one of a set of values.
 
 Each organism is initialized with access to a single value, so the first part of
 the ICD is to distribute knowledge of all possible values.  The second part of 
 the ICD is to determine which of the possible values should be selected by the
 group.  The third, and final, part of the ICD is for the group to iterate this
 problem, so that consensus on as many values as possible is reached in the shortest
 amount of time.
 */
class cActionIteratedConsensus : public cAbstractMonitoringCompeteDemes, ConsensusSupport {
public:
	cActionIteratedConsensus(cWorld* world, const cString& args) : cAbstractMonitoringCompeteDemes(world, args), _replace(0) {
		if(args.GetSize()) {
			cString largs(args);
			_replace = largs.PopWord().AsInt();
		}
	}
	
	//! Destructor.
	virtual ~cActionIteratedConsensus() { }
	
	static const cString GetDescription() { return "Arguments: [int compete_period=100 [int replace_number=0]]"; }
	
	//! Calculate the current fitness of this deme.
  virtual double Fitness(const cDeme& deme) {
		return max_support(deme).first + 1;
	}
	
	/*! Determine if the organisms in this deme have reached consensus, and if so,
	 record that fact and reset so that they can "iterate" (try to reach consensus
	 again).  This version includes protection against resetting to an "easy" value,
	 removing the ability for organisms to continually filter in one direction to
	 solve ICD.
	 
	 Resets always reset the cell data of the agreed-upon value, and if _replace is
	 greater than zero, we also reset the cell data of other cells, which are
	 selected at random with replacement.
	 
	 Called during every update (depending on configuration).  Return values are
	 summed and included in final fitness calculation. */
	virtual double Update(cDeme& deme) {
		std::pair<unsigned int, cOrganism::Opinion> support = max_support(deme);
		
		// Have all organisms in this deme reached consensus?
		if(support.first == static_cast<unsigned int>(deme.GetSize())) {
			// Yes; change the cell data for the value that was agreed upon:
			int min_data = *cAssignRandomCellData::GetDataInDeme(deme).begin();
			int max_data = *cAssignRandomCellData::GetDataInDeme(deme).rbegin();
			cAssignRandomCellData::ReplaceCellData(support.second, m_world->GetRandom().GetInt(min_data+1, max_data-1), deme);
			
			// Now reset the others:
			for(int i=0; i<_replace; ++i) {
				int cell_id = m_world->GetRandom().GetInt(deme.GetSize());
				int cell_data = deme.GetCell(cell_id).GetCellData();
				cAssignRandomCellData::ReplaceCellData(cell_data, m_world->GetRandom().GetInt(min_data+1, max_data-1), deme);
			}
			
			return deme.GetSize();
		}
		return 0.0;
	}
	
private:
	int _replace; //!< Number of cell datas that will be replaced on successful consensus.
};


/*! Send an artificial flash to a single organism in each deme in the population
 at a specified period.
 
 This action sends a single flash to the organism in the center of each deme.  It
 is meant to be sort of a "pacecar" flash, and is useful for analysis.  This action
 is also capable of having a per-deme flash period, which should assist in evolving
 organisms that can adapt their flash period.
 */
class cActionFlash : public cAction {
public:
	cActionFlash(cWorld* world, const cString& args): cAction(world, args) { }
	
	//! Destructor.
	virtual ~cActionFlash() { }
	
	static const cString GetDescription() { return "No arguments"; }
	
	//! Process this event, sending a flash to each deme.
	virtual void Process(cAvidaContext& ctx) {
		for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
			SendFlash(m_world->GetPopulation().GetDeme(i));
		}
	}
								
	//! Send a flash to a single organism in the given deme.							
	void SendFlash(cDeme& deme) {
		cOrganism* org = deme.GetOrganism(deme.GetSize()/2);
		if(org != 0) {
			org->ReceiveFlash();
		}
	}
};


/*! Compete demes based on the ability of their constituent organisms
 to synchronize their flashes to a common phase and period.
 */
class cActionSynchronization : public cAbstractCompeteDemes {
public:
  //! Constructor.
  cActionSynchronization(cWorld* world, const cString& args) : cAbstractCompeteDemes(world, args) { }
  
	//! Destructor.
	virtual ~cActionSynchronization() { }
	
  //! Description of this event.
  static const cString GetDescription() { return "No Arguments"; }
	
  /*! Calculate the fitness based on how well the organisms in this deme have
	 synchronized their flashes.
   */
  virtual double Fitness(const cDeme& deme) {
		cStats::PopulationFlashes::const_iterator deme_flashes = m_world->GetStats().GetFlashTimes().find(deme.GetID());
		if(deme_flashes == m_world->GetStats().GetFlashTimes().end()) {
			// Hm, nothing to see here.  We're done.
			return 1.0;
		}
		
		// Now, chop off all flashes that are outside of the SYNC_FITNESS_WINDOW:
		cStats::DemeFlashes window_flashes;
		copy_flashes(window_flashes, deme_flashes->second, m_world->GetConfig().SYNC_FITNESS_WINDOW.Get());
		
		// Make sure that everyone's flashed:
		if(window_flashes.size() < (unsigned int)deme.GetSize()) {
			return 1.0 + window_flashes.size();
		}
    
    // Since everyone has flashed, fitness is the difference between max and average
    // number of flashes that occurred during the same update squared, added to
		// the size of the deme.
		std::vector<unsigned int> flash_counts;
		count_flashes(window_flashes, flash_counts);
		
		double max = *std::max_element(flash_counts.begin(), flash_counts.end());
		double mean = std::accumulate(flash_counts.begin(), flash_counts.end(), 0.0) / flash_counts.size();
		
		return 1.0 + deme.GetSize() + max - mean;
  }
	
protected:
	//! Copy flashes that occured in the last window updates from source to target.
	void copy_flashes(cStats::DemeFlashes& target, const cStats::DemeFlashes& source, int window) {
		for(cStats::DemeFlashes::const_iterator i=source.begin(); i!=source.end(); ++i) {
			for(cStats::CellFlashes::const_iterator j=i->second.begin(); j!=i->second.end(); ++j) {
				if(*j > (m_world->GetStats().GetUpdate() - window)) {
					target[i->first].push_back(*j);
				}
			}
		}
	}
	
	//! Calculate the number of flashes that have occurred during each update.
	void count_flashes(const cStats::DemeFlashes& flashes, std::vector<unsigned int>& flash_count) {
		flash_count.clear();
		flash_count.resize((unsigned int)m_world->GetConfig().SYNC_FITNESS_WINDOW.Get(), 0);
		for(cStats::DemeFlashes::const_iterator i=flashes.begin(); i!=flashes.end(); ++i) {
			for(cStats::CellFlashes::const_iterator j=i->second.begin(); j!=i->second.end(); ++j) {
				++flash_count[m_world->GetStats().GetUpdate()-*j];
			}
		}		
	}
};


/*! Compete demes based on the number of times they've completed the echo task.
    Fitness is 2^#echos
 */
class cActionCompeteDemesEcho : public cAbstractCompeteDemes {
public:
  //! Constructor.
  cActionCompeteDemesEcho(cWorld* world, const cString& args) : cAbstractCompeteDemes(world, args) { }
  
	//! Destructor.
	virtual ~cActionCompeteDemesEcho() { }
	
  //! Description of this event.
  static const cString GetDescription() { return "No Arguments"; }
	
  virtual double Fitness(const cDeme& deme) {
    int num_echos = 0;
    const int num_task = m_world->GetEnvironment().GetNumTasks();

    for(int i=0; i < deme.GetSize(); i++) {
      int cur_cell = deme.GetCellID(i);
      
      // Since we only count echos from living organisms, this also creates a pressure
      // for all of the organisms to stay alive
      if (m_world->GetPopulation().GetCell(cur_cell).IsOccupied() == false) continue;
      
      cPhenotype & phenotype = m_world->GetPopulation().GetCell(cur_cell).GetOrganism()->GetPhenotype();
      
      for (int j = 0; j < num_task; j++) {        
        if( (strcasecmp(m_world->GetEnvironment().GetTask(j).GetName(), "echo") == 0) &&
           (phenotype.GetLastTaskCount()[j] > 0) ) {
          num_echos += phenotype.GetLastTaskCount()[j];
        }
      }
      
    }
    
    return (double) (2^num_echos);
  } 
  
}; //End cActionCompeteDemesEcho


/*! Compete demes based on the ability of their constituent organisms
 to synchronize their flashes to a common period, and yet distribute themselves
 throughout phase-space (phase desynchronization).
  */
class cActionDesynchronization : public cActionSynchronization {
public:
  //! Constructor.
  cActionDesynchronization(cWorld* world, const cString& args) : cActionSynchronization(world, args) {
  }
  
	//! Destructor.
	virtual ~cActionDesynchronization() { }
	
  //! Description of this event.
  static const cString GetDescription() { return "No Arguments"; }
  
	//! Calculate fitness based on how well organisms have spread throughout phase-space.
	virtual double Fitness(const cDeme& deme) {
		cStats::PopulationFlashes::const_iterator deme_flashes = m_world->GetStats().GetFlashTimes().find(deme.GetID());
		if(deme_flashes == m_world->GetStats().GetFlashTimes().end()) {
			// Hm, nothing to see here.  We're done.
			return 1.0;
		}
		
		// Now, chop off all flashes that are outside of the SYNC_FITNESS_WINDOW:
		cStats::DemeFlashes window_flashes;
		copy_flashes(window_flashes, deme_flashes->second, m_world->GetConfig().SYNC_FITNESS_WINDOW.Get());
		
		// Make sure that everyone's flashed:
		if(window_flashes.size() < (unsigned int)deme.GetSize()) {
			return 1.0 + window_flashes.size();
		}
		
		// Since everyone has flashed, fitness is the size of the deme minus the max
		// number of organisms that have flashed during the same update.
		std::vector<unsigned int> flash_counts;
		count_flashes(window_flashes, flash_counts);
		
		double max = *std::max_element(flash_counts.begin(), flash_counts.end());
		
		return 1.0 + 2*deme.GetSize() - max;
  }
};


class cAbstractCompeteDemes_AttackKillAndEnergyConserve : public cAbstractCompeteDemes {

  public:
    cAbstractCompeteDemes_AttackKillAndEnergyConserve(cWorld* world, const cString& args) : cAbstractCompeteDemes(world, args) { }

    static const cString GetDescription() { return "No Arguments"; }
  
    double Fitness(const cDeme& deme) {    
      double eventsKilled = static_cast<double>(deme.GetEventsKilled());
      double totalEvents  = static_cast<double>(deme.GetEventsTotal());
      double energyRemaining = deme.CalculateTotalEnergy();
      double initialEnergy = deme.CalculateTotalInitialEnergyResources();
      double fitnessOfDeme = ((eventsKilled / totalEvents) + (energyRemaining / initialEnergy)) / 2.0;
      return fitnessOfDeme;
    }
};


/* This Action will check if any demes have met the critera to be replicated
   and do so.  There are several bases this can be checked on:

    'all'       - ...all non-empty demes in the population.
    'full_deme' - ...demes that have been filled up.
    'corners'   - ...demes with upper left and lower right corners filled.
    'deme-age'  - ...demes that are a certain age
    'birth-count' ...demes that have had a certain number of births.
    'sat-mov-pred'  - ...demes whose movement predicate was previously satisfied
    'events-killed' ...demes that have killed a certian number of events
    'sat-msg-pred'  - ...demes whose message predicate was previously satisfied
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
    else if (in_trigger == "deme-age") m_rep_trigger = 3;
    else if (in_trigger == "birth-count") m_rep_trigger = 4;
    else if (in_trigger == "sat-mov-pred") m_rep_trigger = 5;
    else if (in_trigger == "events-killed") m_rep_trigger = 6;
    else if (in_trigger == "sat-msg-pred") m_rep_trigger = 7;
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


class cActionNewTrial : public cAction
{
private:
public:
  cActionNewTrial(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
  }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().NewTrial();
  }
};

class cActionCompeteOrganisms : public cAction
{
private:
  int m_type;
  int m_parents_survive;
public:
  cActionCompeteOrganisms(cWorld* world, const cString& args) : cAction(world, args), m_type(0), m_parents_survive(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_type = largs.PopWord().AsInt();
    if (largs.GetSize()) m_parents_survive = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int type=0] [int parents_survive=0] [double scaled_time=1.0] [int dynamic_scaling=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().CompeteOrganisms(m_type, m_parents_survive);
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
      //col is always the same -- compute which row to make the cut
      int idA = row_id * world_x + m_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);

      int idA0 = GridNeighbor(idA, world_x, world_y,  0, -1);
      int idA1 = GridNeighbor(idA, world_x, world_y,  0,  1);

      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y, -1,  1);

      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);

      #ifdef DEBUG
      int temp_x = 0, temp_y = 0;
      cellA.GetPosition(temp_x,temp_y);
      cerr << "cellA: " << temp_x << " " << temp_y << endl;
      cellB.GetPosition(temp_x,temp_y);
      cerr << "cellB: " << temp_x << " " << temp_y << endl;
      #endif

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
 Remove the connections between cells along a row in an avida grid.

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

    // Loop through all of the cols and make the cut on each...
    for (int col_id = m_min; col_id < m_max; col_id++) {
      //row is always the same -- only the column changes -- could also do this in the loop
      int idA = m_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y,  0, -1);

      int idA0 = GridNeighbor(idA, world_x, world_y, -1,  0);
      int idA1 = GridNeighbor(idA, world_x, world_y,  1,  0);

      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y,  1, -1);
      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);
      
      #ifdef DEBUG
      int temp_x = 0, temp_y = 0;
      cellA.GetPosition(temp_x,temp_y);
      cerr << "cellA: " << temp_x << " " << temp_y << endl;
      cellB.GetPosition(temp_x,temp_y);
      cerr << "cellB: " << temp_x << " " << temp_y << endl;
      #endif

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
    const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
    if (m_id == -1) m_id = world_x / 2;
    if (m_max == -1) m_max = world_y;
    if (m_id < 0 || m_id >= world_x) {
      cString err = cStringUtil::Stringf("Column ID %d out of range for JoinGridCol", m_id);
      m_world->GetDriver().RaiseException(err);
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int row_id = m_min; row_id < m_max; row_id++) {
      //compute which cells to be joined -- grab them from the population
      int idA = row_id * world_x + m_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);
      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);

      //grab the cell lists
      tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
      tList<cPopulationCell>& cellB_list = cellB.ConnectionList();

      //these cells are always joined
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);

      //make sure we don't break the bounded grid at the top
      if((nGeometry::GRID == geometry && row_id != 0) || nGeometry::GRID != geometry){
	cPopulationCell& cellA0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0, -1));
	cPopulationCell& cellB0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
	if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
	if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      }

      //make sure we don't break the bounded grid at the bottom
      if((nGeometry::GRID == geometry && row_id != (world_y-1)) || nGeometry::GRID != geometry){
	cPopulationCell& cellA1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0,  1));
	cPopulationCell& cellB1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  1));
	if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
	if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
      }
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
    const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
    if (m_id == -1) m_id = world_y / 2;
    if (m_max == -1) m_max = world_x;
    if (m_id < 0 || m_id >= world_y) {
      cString err = cStringUtil::Stringf("Row ID %d out of range for JoinGridRow", m_id);
      m_world->GetDriver().RaiseException(err);
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int col_id = m_min; col_id < m_max; col_id++) {
      //compute which cells are beoing joined and grab them
      int idA = m_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y, 0, -1);
      cPopulationCell& cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell& cellB = m_world->GetPopulation().GetCell(idB);

      //grab the cell lists
      tList<cPopulationCell>& cellA_list = cellA.ConnectionList();
      tList<cPopulationCell>& cellB_list = cellB.ConnectionList();

      //these cells are always joined
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);

      //make sure we don't break the bounded grid on the left
      if((nGeometry::GRID == geometry && col_id != 0) || nGeometry::GRID != geometry){
	cPopulationCell& cellA0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  0));
	cPopulationCell& cellB0 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
	if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
	if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      }

      //make cure we don't break the bounded grid on the right
      if((nGeometry::GRID == geometry && col_id != (world_x-1)) || nGeometry::GRID != geometry){
	cPopulationCell& cellA1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1,  0));
	cPopulationCell& cellB1 = m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1, -1));
	if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
	if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
      }
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

class cActionSwapCells : public cAction
{
private:
  int id1;
  int id2;

public:
  cActionSwapCells(cWorld* world, const cString& args) : cAction(world, args), id1(-1), id2(-1)
  {
    cString largs(args);
    if (largs.GetSize()) id1 = largs.PopWord().AsInt();
    if (largs.GetSize()) id2 = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int cell_id1> <int cell_id2>"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int num_cells = m_world->GetPopulation().GetSize();
    if (id1 < 0 || id1 >= num_cells ||
	id2 < 0 || id2 >= num_cells) {
      m_world->GetDriver().RaiseException("SwapCells cell ID out of range");
      return;
    }
    if (id1 == id2) {
      m_world->GetDriver().NotifyWarning("SwapCells cell IDs identical");
    }

    cPopulationCell& cell1 = m_world->GetPopulation().GetCell(id1);
    cPopulationCell& cell2 = m_world->GetPopulation().GetCell(id2);
    m_world->GetPopulation().SwapCells(cell1, cell2);
  }
};

/*
 Added predicate to all demes that is satisified when an organism reaches the center of an event
*/
class cActionPred_DemeEventMoveCenter : public cAction
{
private:
  int m_times;

public:
  cActionPred_DemeEventMoveCenter(cWorld* world, const cString& args) : cAction(world, args), m_times(1) {
    cString largs(args);
    if (largs.GetSize()) m_times = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int times=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().AddDemePred("EventMovedIntoCenter", m_times);
  }
};


/*
 Added predicate to all demes that is satisified when an organism reaches a target cell
 modified cActionPred_DemeEventMoveCenter
*/
class cActionPred_DemeEventMoveBetweenTargets : public cAction
{
private:
  int m_times;

public:
  cActionPred_DemeEventMoveBetweenTargets(cWorld* world, const cString& args) : cAction(world, args), m_times(1) {
    cString largs(args);
    if (largs.GetSize()) m_times = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int times=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().AddDemePred("EventMovedBetweenTargets", m_times);
  }
};


/*
 Added predicate to all demes that is satisified when a number of organisms
 reach a target cell
*/
class cActionPred_DemeEventEventNUniqueIndividualsMovedIntoTarget : public cAction
{
private:
  int m_numorgs;

public:
  cActionPred_DemeEventEventNUniqueIndividualsMovedIntoTarget(cWorld* world, const cString& args) : cAction(world, args), m_numorgs(1) {
    cString largs(args);
    if (largs.GetSize()) m_numorgs = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int numorgs=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().AddDemePred("EventNUniqueIndividualsMovedIntoTarget", m_numorgs);
  }
};



void RegisterPopulationActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionInject>("Inject");
  action_lib->Register<cActionInjectRandom>("InjectRandom");
  action_lib->Register<cActionInjectAllRandomRepro>("InjectAllRandomRepro");
  action_lib->Register<cActionInjectAll>("InjectAll");
  action_lib->Register<cActionInjectRange>("InjectRange");
  action_lib->Register<cActionInjectSequence>("InjectSequence");
  action_lib->Register<cActionInjectSequenceWithDivMutRate>("InjectSequenceWDivMutRate");
  action_lib->Register<cActionInjectDemes>("InjectDemes");
  action_lib->Register<cActionInjectDemesFromNest>("InjectDemesFromNest");
  action_lib->Register<cActionInjectDemesRandom>("InjectDemesRandom");

  action_lib->Register<cActionInjectParasite>("InjectParasite");
  action_lib->Register<cActionInjectParasitePair>("InjectParasitePair");

  action_lib->Register<cActionKillProb>("KillProb");
  action_lib->Register<cActionToggleRewardInstruction>("ToggleRewardInstruction");
  action_lib->Register<cActionToggleFitnessValley>("ToggleFitnessValley");
  action_lib->Register<cActionKillProb>("KillRate");
  action_lib->Register<cActionKillRectangle>("KillRectangle");
  action_lib->Register<cActionSerialTransfer>("SerialTransfer");

  action_lib->Register<cActionSetMigrationRate>("SetMigrationRate");
  action_lib->Register<cActionSetMutProb>("SetMutProb");
  action_lib->Register<cActionModMutProb>("ModMutProb");
  action_lib->Register<cActionZeroMuts>("ZeroMuts");

	action_lib->Register<cActionTrackAllMessages>("TrackAllMessages");
	
  action_lib->Register<cActionCompeteDemes>("CompeteDemes");
  action_lib->Register<cActionReplicateDemes>("ReplicateDemes");
  action_lib->Register<cActionDivideDemes>("DivideDemes");
  action_lib->Register<cActionResetDemes>("ResetDemes");
  action_lib->Register<cActionCopyDeme>("CopyDeme");

	action_lib->Register<cActionFlash>("Flash");
	
	/****AbstractCompeteDemes sub-classes****/
		
  action_lib->Register<cAbstractCompeteDemes_AttackKillAndEnergyConserve>("CompeteDemes_AttackKillAndEnergyConserve");
  action_lib->Register<cAssignRandomCellData>("AssignRandomCellData");
  action_lib->Register<cActionIteratedConsensus>("IteratedConsensus");
	action_lib->Register<cActionSynchronization>("Synchronization");
	action_lib->Register<cActionDesynchronization>("Desynchronization");
  action_lib->Register<cActionCompeteDemesEcho>("CompeteDemesEcho");
	
  action_lib->Register<cActionNewTrial>("NewTrial");
  action_lib->Register<cActionCompeteOrganisms>("CompeteOrganisms");
  
  action_lib->Register<cActionSeverGridCol>("SeverGridCol");
  action_lib->Register<cActionSeverGridRow>("SeverGridRow");
  action_lib->Register<cActionJoinGridCol>("JoinGridCol");
  action_lib->Register<cActionJoinGridRow>("JoinGridRow");

  action_lib->Register<cActionConnectCells>("ConnectCells");
  action_lib->Register<cActionDisconnectCells>("DisconnectCells");
  action_lib->Register<cActionSwapCells>("SwapCells");

  // @DMB - The following actions are DEPRECATED aliases - These will be removed in 2.7.
  action_lib->Register<cActionInject>("inject");
  action_lib->Register<cActionInjectRandom>("inject_random");
  action_lib->Register<cActionInjectAllRandomRepro>("inject_all_random_repro");
  action_lib->Register<cActionInjectAll>("inject_all");
  action_lib->Register<cActionInjectRange>("inject_range");
  action_lib->Register<cActionInjectSequence>("inject_sequence");

  action_lib->Register<cActionKillProb>("apocalypse");
  action_lib->Register<cActionKillRate>("rate_kill");
  action_lib->Register<cActionKillRectangle>("kill_rectangle");
  action_lib->Register<cActionSerialTransfer>("serial_transfer");

  action_lib->Register<cActionZeroMuts>("zero_muts");
  
  action_lib->Register<cActionCompeteDemes>("compete_demes");
  action_lib->Register<cActionReplicateDemes>("replicate_demes");
  action_lib->Register<cActionResetDemes>("reset_demes");
  action_lib->Register<cActionCopyDeme>("copy_deme");
  
  action_lib->Register<cActionCompeteDemes>("new_trial");
  action_lib->Register<cActionCompeteDemes>("compete_organisms");
  
  action_lib->Register<cActionSeverGridCol>("sever_grid_col");
  action_lib->Register<cActionSeverGridRow>("sever_grid_row");
  action_lib->Register<cActionJoinGridCol>("join_grid_col");
  action_lib->Register<cActionJoinGridRow>("join_grid_row");

  action_lib->Register<cActionConnectCells>("connect_cells");
  action_lib->Register<cActionDisconnectCells>("disconnect_cells");
  action_lib->Register<cActionSwapCells>("swap_cells");

  action_lib->Register<cActionPred_DemeEventMoveCenter>("Pred_DemeEventMoveCenter");
  action_lib->Register<cActionPred_DemeEventMoveBetweenTargets>("Pred_DemeEventMoveBetweenTargets");
  action_lib->Register<cActionPred_DemeEventEventNUniqueIndividualsMovedIntoTarget>("Pred_DemeEventNUniqueIndividualsMovedIntoTarget");
}
