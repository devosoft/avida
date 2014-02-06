/*
 *  PopulationActions.cc
 *  Avida
 *
 *  Created by David on 6/25/06.
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

#include "PopulationActions.h"

#include "avida/core/Feedback.h"
#include "avida/core/InstructionSequence.h"
#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"

#include "avida/private/util/GenomeLoader.h"

#include "apto/stat/Accumulator.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cWorld.h"
#include "cOrganism.h"
#include "cEnvironment.h"
#include "cUserFeedback.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <set>

#include "stdlib.h"

using namespace Avida;


/*
 Injects a single organism into the population.
 
 Parameters:
 filename (string)
 The filename of the genotype to load.
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
  double m_neutral_metric;
  cString m_trace_filename;
public:
  cActionInject(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_id(0), m_merit(-1), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [int cell_id=0] [double merit=-1] [double neutral_metric=0] [string trace_filename]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_filename.GetSize() == 0) {
      cerr << "error: no organism file specified" << endl;
      return;
    }
    GenomePtr genome;
    cUserFeedback feedback;
    genome = Util::LoadGenomeDetailFile(m_filename, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback);
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
        case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }
    if (!genome) return;
    m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, m_cell_id, m_merit, m_neutral_metric, false);
    
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
  double m_neutral_metric;
public:
  cActionInjectRandom(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_id(0), m_merit(-1), m_neutral_metric(0)
  {
    cString largs(args);
    m_length = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <int length> [int cell_id=0] [double merit=-1] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
    HashPropertyMap props;
    cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
    Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence(m_length)));
    InstructionSequencePtr seq_p;
    seq_p.DynamicCastFrom(genome.Representation());
    InstructionSequence& seq = *seq_p;
    for (int i = 0; i < m_length; i++) seq[i] = is.GetRandomInst(ctx);
    m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "random", true), ctx, m_cell_id, m_merit, m_neutral_metric);
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
  double m_neutral_metric;
  int m_sex;
public:
  cActionInjectAllRandomRepro(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_merit(-1), m_neutral_metric(0), m_sex(0)
  {
    cString largs(args);
    m_length = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_sex = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int length> [double merit=-1] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++)
    {
      const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
      HashPropertyMap props;
      cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
      Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence(m_length + 1)));
      InstructionSequencePtr seq_p;
      seq_p.DynamicCastFrom(genome.Representation());
      InstructionSequence& seq = *seq_p;
      for (int j = 0; j < m_length; j++) {
        Instruction inst = is.GetRandomInst(ctx);
        while (is.GetRedundancy(inst) == 0) inst = is.GetRandomInst(ctx);
        seq[j] = inst;
      }
      if (m_sex) seq[m_length] = is.GetInst("repro-sex");
      else seq[m_length] = is.GetInst("repro");
      
      m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "random", true), ctx, i, m_merit, m_neutral_metric);
    }
  }
};

/*
 Injects identical organisms into all cells of the population.
 
 Parameters:
 filename (string)
 The filename of the genotype to load. 
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
  Apto::String m_instset;
  double m_merit;
  double m_neutral_metric;
public:
  cActionInjectAll(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args), m_merit(-1.0), m_neutral_metric(0)
  {

    Util::ArgSchema schema;
    
    // Entries
    schema.Define("filename", Util::STRING);
    schema.Define("instset", "");
    
    schema.Define("merit", -1.0);
    schema.Define("neutral_metric", 0.0);
    
    Util::Args* argc = Util::Args::Load((const char*)args, schema, ':', '=', &feedback);
    
    if (argc) {
      m_filename = argc->String(0);
      m_instset = argc->String(1);
      
      m_merit = argc->Double(0);
      m_neutral_metric = argc->Double(1);
      
    }
    
    delete argc;
  }
  
  static const cString GetDescription() { return "Arguments: <string filename> [instset=""] [double merit=-1] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_filename.GetSize() == 0) {
      cerr << "error: no organism file specified" << endl;
      return;
    }
    GenomePtr genome;
    cUserFeedback feedback;
    genome = Util::LoadGenomeDetailFile(m_filename, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback, m_instset);
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
        case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }
    if (!genome) return;
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++)
      m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_neutral_metric);
  }
};

/*
 Injects identical organisms into a range of cells of the population.
 
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
class cActionInjectRange : public cAction
{
private:
  cString m_filename;
  int m_cell_start;
  int m_cell_end;
  double m_merit;
  double m_neutral_metric;
public:
  cActionInjectRange(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [int cell_start=0] [int cell_end=-1] [double merit=-1] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_filename.GetSize() == 0) {
      cerr << "error: no organism file specified" << endl;
      return;
    }
    
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      ctx.Driver().Feedback().Warning("InjectRange has invalid range!");
    } else {
      GenomePtr genome;
      cUserFeedback feedback;
      genome = Util::LoadGenomeDetailFile(m_filename, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback);
      for (int i = 0; i < feedback.GetNumMessages(); i++) {
        switch (feedback.GetMessageType(i)) {
          case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
          case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
          default: break;
        };
        cerr << feedback.GetMessage(i) << endl;
      }
      if (!genome) return;
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_neutral_metric);
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
  double m_neutral_metric;
public:
  cActionInjectSequence(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_neutral_metric(0)
  {
    cString largs(args);
    m_sequence = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  static const cString GetDescription() { return "Arguments: <string sequence> [int cell_start=0] [int cell_end=-1] [double merit=-1] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      ctx.Driver().Feedback().Warning("InjectSequence has invalid range!");
    } else {
      const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
      HashPropertyMap props;
      cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
      Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence((const char*)m_sequence)));
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_neutral_metric);
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
  double m_neutral_metric;
  
  
public:
  cActionInjectSequenceWithDivMutRate(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_div_mut_rate(0.0), m_neutral_metric(0)
  {
    cString largs(args);
    m_sequence = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_div_mut_rate = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  static const cString GetDescription() { return "Arguments: <string sequence> [int cell_start=0] [int cell_end=-1] [double div_mut_rate=0] [double merit=-1] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      ctx.Driver().Feedback().Warning("InjectSequenceWithDivMutRate has invalid range!");
    } else {
      const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
      HashPropertyMap props;
      cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
      Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence((const char*)m_sequence)));
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_neutral_metric);
        m_world->GetPopulation().GetCell(i).GetOrganism()->MutationRates().SetDivMutProb(m_div_mut_rate);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};


/* Inject org into world with specified group_id and forager types */
class cActionInjectGroup : public cAction
{
private:
  cString m_filename;
  int m_cell_id;
  int m_group_id;
  int m_forager_type;
  bool m_trace;
  double m_merit;
  double m_neutral_metric;
  cString m_trace_filename;
public:
  cActionInjectGroup(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_id(0), m_group_id(m_world->GetConfig().DEFAULT_GROUP.Get()), m_forager_type(-1), m_trace(false),m_merit(-1), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_group_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_forager_type = largs.PopWord().AsInt();
    if (largs.GetSize()) m_trace = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [int cell_id=0] [int group_id=-1] [int forager_type=-1] [bool trace=false] [double merit=-1] [double neutral_metric=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_filename.GetSize() == 0) {
      cerr << "error: no organism file specified" << endl;
      return;
    }
    
    GenomePtr genome;
    cUserFeedback feedback;
    genome = Util::LoadGenomeDetailFile(m_filename, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback);
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
        case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }
    if (!genome) return;
    m_world->GetPopulation().InjectGroup(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, m_cell_id, m_merit, m_neutral_metric, m_group_id, m_forager_type, m_trace);
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
  cActionKillProb(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_killprob(0.9)
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
      if (ctx.GetRandom().P(m_killprob))  m_world->GetPopulation().KillOrganism(cell, ctx); 
    }
  }
};

/*                                                                                                 
 Applies a fixed population bottleneck to the current population.                                  
 Parameters:                                                                                       
 bottleneck size (int) default: 100                                                                
 The size of the population bottleneck; kills every organism until the population is of the size specified. 
 */
class cActionApplyBottleneck : public cAction
{
private:
  int m_bottleneck_size;                                                                          
public:
  cActionApplyBottleneck(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_bottleneck_size(100)
  {
    cString largs(args);                                                                        
    if (largs.GetSize()) m_bottleneck_size = largs.PopWord().AsInt();                           
  }
  
  static const cString GetDescription() { return "Arguments: [int = 0]"; }                        
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& pop = m_world->GetPopulation();                                                
    int num_to_kill = pop.GetNumOrganisms() - m_bottleneck_size;
    
    while (num_to_kill > 0)
    {
      int target_cell = ctx.GetRandom().GetInt(0, pop.GetSize() - 1);
      cPopulationCell& cell = pop.GetCell(target_cell);                                         
      if (cell.IsOccupied())
      {
        pop.KillOrganism(cell, ctx);                                                          
        num_to_kill--;                                                                        
      }
    }
  }
};

class cActionKillDominantGenotype : public cAction
{
private:

public:
  cActionKillDominantGenotype(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    
  }
  
  static const cString GetDescription() { return "Arguments:"; }                        
  
  void Process(cAvidaContext& ctx)
  {
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    Systematics::GroupPtr dom_g = it->Next();
    int dom_id = dom_g->ID();
    
    const Apto::Array<cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
    
    Apto::Array<int, Apto::Smart> doomed_orgs;
    
    for (int i = 0; i < live_orgs.GetSize(); i++) {
      if (live_orgs[i]->SystematicsGroup("genotype")->ID() == dom_id) doomed_orgs.Push(live_orgs[i]->GetCellID());
    }
    cPopulation& pop = m_world->GetPopulation();                                                
    
    for (int j=0; j < doomed_orgs.GetSize(); j++) {
      pop.KillOrganism(ctx, doomed_orgs[j]);
    }
  }
};

class cActionAttackDen : public cAction
{
private:
  double m_killprob;
  int m_juvs_per;
public:
  cActionAttackDen(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_killprob(0.0), m_juvs_per(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_killprob = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_juvs_per = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [double probability=0.0] [int juvs_per_adult=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_world->GetConfig().USE_AVATARS.Get() <= 0) m_world->GetDriver().Feedback().Error("Den AttackDen requires use of avatars.");
    int juv_age = m_world->GetConfig().JUV_PERIOD.Get();
    
    const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
    
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
      
      if (!cell.HasAV()) continue;
      
      Apto::Array<double> cell_res;
      cell_res = m_world->GetPopulation().GetResources().GetCellResources(i, ctx);
      
      for (int j = 0; j < cell_res.GetSize(); j++) {
        if ((resource_lib.GetResDef(j)->GetHabitat() == 4 ||resource_lib.GetResDef(j)->GetHabitat() == 3) && cell_res[j] > 0) {
          // for every x juvs, we require 1 adult...otherwise use killprob on the rest
          Apto::Array<cOrganism*> cell_avs = cell.GetCellAVs();    // cell avs are already randomized
          Apto::Array<cOrganism*> juvs;
          juvs.Resize(0);
          int num_juvs = 0;
          int num_guards = 0;
          for (int k = 0; k < cell_avs.GetSize(); k++) {
            if (cell_avs[k]->GetPhenotype().GetTimeUsed() < juv_age) { 
              num_juvs++;
              juvs.Push(cell_avs[k]);
            }
            else if (cell_avs[k]->IsGuard()) num_guards++;
          }
          if (num_juvs == 0) break;
          int guarded_juvs = num_guards * m_juvs_per;
          int unguarded_juvs = num_juvs - guarded_juvs;
          for (int k = 0; k < unguarded_juvs; k++) {
              if (ctx.GetRandom().P(m_killprob)){
                  juvs[k]->Die(ctx);
                  m_world->GetStats().IncJuvKilled();
              }
          }
          break;  // only do this once if two dens overlap
        }
      }
    }
  }
};

class cActionRaidDen : public cAction
{
private:
  double m_loss;
  int m_res_id;
  double m_units_per;
public:
  cActionRaidDen(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_loss(0.0), m_res_id(0), m_units_per(1.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_loss = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_res_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_units_per = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: [double loss=0.0] [int res_id=1] [double units_per_adult=1.0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_world->GetConfig().USE_AVATARS.Get() <= 0) m_world->GetDriver().Feedback().Error("Den RaidDen requires use of avatars.");
    int juv_age = m_world->GetConfig().JUV_PERIOD.Get();
    
    const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
    
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
      
      Apto::Array<double> cell_res;
      cell_res = m_world->GetPopulation().GetResources().GetCellResources(i, ctx);
      
      for (int j = 0; j < cell_res.GetSize(); j++) {
        if ((resource_lib.GetResDef(j)->GetHabitat() == 4 || resource_lib.GetResDef(j)->GetHabitat() == 3) && cell_res[j] > 0) {
          if (cell_res[m_res_id] <= 0) break;
          
          // for every x units of res, we require 1 adult guard...otherwise apply outflow to rest
          int num_guards = 0;
          Apto::Array<cOrganism*> cell_avs = cell.GetCellAVs();
          for (int k = 0; k < cell_avs.GetSize(); k++) {
            if (cell_avs[k]->GetPhenotype().GetTimeUsed() >= juv_age) num_guards++;
          }
          
          double guarded_res = num_guards * m_units_per;
          double unguarded_res = cell_res[m_res_id] - guarded_res;
          
          Apto::Array<double> res_change(cell_res.GetSize());
          res_change.SetAll(0.0);
          res_change[m_res_id] = -1 * unguarded_res * m_loss;          
          m_world->GetPopulation().GetResources().UpdateCellResources(ctx, res_change, i);
          
          break;  // only do this once if two dens overlap
        }
      }
    }
  }
};

/*
 Kills a fraction of organisms in the population in sequence.
 
 Parameters:
 fraction killed (double) default: 0.01
 
 */
class cActionKillFractionInSequence : public cAction
{
private:
  double m_killFraction;
  int m_windowLength;
  int killIndex;
  bool applyAction;
  int updateSinceLastContextSwitch;
public:
  cActionKillFractionInSequence(cWorld* world, const cString& args, Feedback&) : 
  cAction(world, args), 
  m_killFraction(0.01), 
  m_windowLength(1000), 
  killIndex(0),
  applyAction(true),
  updateSinceLastContextSwitch(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_killFraction = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_windowLength = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [double fraction=0.01]"; }
  
  void Process(cAvidaContext& ctx)
  {
    ++updateSinceLastContextSwitch;
    
    if(updateSinceLastContextSwitch >= static_cast<int>(m_windowLength/2.0)) {
      // switch context
      applyAction = !applyAction;
      updateSinceLastContextSwitch = 0;
      cerr << "applied? " << applyAction<<endl;
    }
    
    if(!applyAction)
      return;
    
    cPopulation& pop = m_world->GetPopulation();
    const int numOrgsInPop = pop.GetNumOrganisms();
    int organismsToKill = static_cast<int>(numOrgsInPop * m_killFraction);
    int oldKillIndex = killIndex;
    
    while(organismsToKill > 0) {
      cPopulationCell& cell = pop.GetCell(killIndex);
      if (cell.IsOccupied()) {
        pop.KillOrganism(cell, ctx); 
        --organismsToKill;
      }
      killIndex = (killIndex + 1) % pop.GetSize();
      if(killIndex == oldKillIndex)
        assert(false);  // trying to kill organisms that don't exist
    }
  }
};

/*
 Kills a fraction of organisms in the population in sequence.
 
 Parameters:
 fraction killed (double) default: 0.01
 
 */
class cActionKillFractionInSequence_PopLimit : public cAction
{
private:
  double m_killFraction;
  int m_popSize;
  int killIndex;
  
public:
  cActionKillFractionInSequence_PopLimit(cWorld* world, const cString& args, Feedback&) : 
  cAction(world, args), 
  m_killFraction(0.01), 
  m_popSize(1000),
  killIndex(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_killFraction = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_popSize = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [double fraction=0.01]"; }
  
  void Process(cAvidaContext& ctx)
  {
    double avgGestation = m_world->GetStats().GetAveGestation();
    double instPerUpdate = m_world->GetConfig().AVE_TIME_SLICE.Get();
    double growthRate = floor(instPerUpdate/avgGestation * 100.0 + 0.5) / 100.0;
    
    cPopulation& pop = m_world->GetPopulation();
    const int numOrgsInPop = pop.GetNumOrganisms();
    if(numOrgsInPop < m_popSize)
      return;
    
    int organismsToKill = static_cast<int>(numOrgsInPop * growthRate);
    int oldKillIndex = killIndex;
    cerr<< "growth rate: " << growthRate << "  kill: " << organismsToKill <<endl;
    while(organismsToKill > 0) {
      cPopulationCell& cell = pop.GetCell(killIndex);
      if (cell.IsOccupied()) {
        pop.KillOrganism(cell, ctx); 
        --organismsToKill;
      }
      killIndex = (killIndex + 1) % pop.GetSize();
      if(killIndex == oldKillIndex)
        assert(false);  // trying to kill organisms that don't exist
    }
  }
};

/*
 Randomly removes a certain proportion of the population whose genomes contain a specified
 number (or more) of a certain type of instruction.  E.g., the default is to remove 90% of 
 organisms containing 5 or more nand instructions.
 
 Parameters:
 removal probability (double) default: 0.9
 The probability with which a single organism is removed.
 instruction type (string) default: "nand"
 The type of instruction in question.
 threshold level (int) default: 5
 The threshold for instruction instances.
 */
class cActionKillInstLimit : public cAction
{
private:
  double m_killprob;
  cString m_inst;
  double m_limit;
public:
  cActionKillInstLimit(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_killprob(0.9), m_inst("nand"), m_limit(5)
  {
    cString largs(args);
    if (largs.GetSize()) m_killprob = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_inst = largs.PopWord();
    if (largs.GetSize()) m_limit = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [double probability=0.9] [cString inst=nand] [int limit=5]"; }
  
  void Process(cAvidaContext& ctx)
  {
    int count;
    int totalkilled = 0;
    
    // for each organism in the population...
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
      if (cell.IsOccupied() == false) continue;
      
      // count the number of target instructions in the genome
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(cell.GetOrganism()->GetGenome().Representation());
			count = seq->CountInst(m_world->GetHardwareManager().GetInstSet(cell.GetOrganism()->GetGenome().Properties().Get("instset").StringValue()).GetInst(m_inst));
      
      // decide if it should be killed or not, based on the count and a the kill probability
      if (count >= m_limit) {
        if (ctx.GetRandom().P(m_killprob))  {
          m_world->GetPopulation().KillOrganism(cell, ctx); 
          totalkilled++;
        }
      }
    }
    // could keep track of the total number killed for statistics; in testing simply printed it out
    // cout << "total killed = " << totalkilled << endl;
  }
};


/*
 Randomly removes a certain proportion of the population whose genomes contain at least m_limit instances of 
 each of two specified instructions.  Default is to remove 90% of organisms containing at least one NAND and 
 at least one NOR instruction.   To survive, organisms need to use one or the other, but not both.
 
 Parameters:
 removal probability (double) default: 0.9
 The probability with which a single organism is removed.
 instruction type1 (string) default: "nand"
 The type of instruction in question.
 instruction type2 (string) default: "nor"
 The type of instruction in question.
 threshold level (int) default: 1
 The threshold for instruction instances.
 */
class cActionKillInstPair : public cAction {
private:
	double m_killprob;
	cString m_inst1;
	cString m_inst2;
	double m_limit;
public:
	cActionKillInstPair(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_killprob(0.9), m_inst1("nand"), m_inst2("nor"), m_limit(1)
	{
		cString largs(args);
		if (largs.GetSize()) m_killprob = largs.PopWord().AsDouble();
		if (largs.GetSize()) m_inst1 = largs.PopWord();
		if (largs.GetSize()) m_inst2 = largs.PopWord();
		if (largs.GetSize()) m_limit = largs.PopWord().AsInt();
	}
	
	static const cString GetDescription() { return "Arguments: [double probability=0.9] [cString inst1=nand] [cString inst2=nor] [int limit=1]"; }
	
	void Process(cAvidaContext& ctx)
	{
		int count1, count2;
		int totalkilled = 0;
		
		// for each organism in the population...
		for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
			cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
			if (cell.IsOccupied() == false) continue;
			
			// get the number of instructions of each type.
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(cell.GetOrganism()->GetGenome().Representation());
      Apto::String instset(cell.GetOrganism()->GetGenome().Properties().Get("instset").StringValue());
			count1 = seq->CountInst(m_world->GetHardwareManager().GetInstSet(instset).GetInst(m_inst1));
			count2 = seq->CountInst(m_world->GetHardwareManager().GetInstSet(instset).GetInst(m_inst2));
			
			// decide if it should be killed or not, based on the two counts and a the kill probability
			if ((count1 >= m_limit) && (count2 >= m_limit)) {
				if (ctx.GetRandom().P(m_killprob))  {
					m_world->GetPopulation().KillOrganism(cell, ctx); 
					totalkilled++;
				}
			}
		}
		// could keep track of the total number killed for statistics; in testing simply printed it out
		// cout << "total killed = " << totalkilled << endl;
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
  cActionToggleRewardInstruction(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {}
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext&)
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
  cActionToggleFitnessValley(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {}
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext&)
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
  cActionKillRate(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_killrate(0.0)
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
      if (ctx.GetRandom().P(kill_prob))  m_world->GetPopulation().KillOrganism(cell, ctx); 
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
  cActionKillRectangle(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_x1(0), m_y1(0), m_x2(0), m_y2(0)
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
        if (cell.IsOccupied()) pop.KillOrganism(cell, ctx); 
      }
    }
    m_world->GetPopulation().SetSyncEvents(true);
  }
};




class cActionSetMutProb : public cAction
{
private:
  enum {
    POINT,
    C_MUT, C_INS, C_DEL, C_UNIFORM, C_SLIP,
    DS_MUT, DS_INS, DS_DEL, DS_UNIFORM, DS_SLIP,
    D1_MUT, D1_INS, D1_DEL, D1_UNIFORM, D1_SLIP,
    P_MUT, P_INS, P_DEL, DEATH, PNT_MUT, PNT_INS, PNT_DEL,
    I_MUT, I_INS, I_DEL
  } m_mut_type;
  
  double m_prob;
  int m_start;
  int m_end;
  bool m_setconf;
  
public:
  cActionSetMutProb(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_prob(0.0), m_start(-1), m_end(-1), m_setconf(false)
  {
    cString mutstr("COPY_MUT");
    
    cString largs(args);
    if (largs.GetSize()) mutstr = largs.PopWord().ToUpper();
    if (largs.GetSize()) m_prob = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_end = largs.PopWord().AsInt();
    
    
    if (mutstr == "POINT") m_mut_type = POINT;
    
    else if (mutstr == "COPY_MUT") m_mut_type = C_MUT;
    else if (mutstr == "COPY_INS") m_mut_type = C_INS;
    else if (mutstr == "COPY_DEL") m_mut_type = C_DEL;
    else if (mutstr == "COPY_UNIFORM") m_mut_type = C_UNIFORM;
    else if (mutstr == "COPY_SLIP") m_mut_type = C_SLIP;
    
    else if (mutstr == "DIV_MUT") m_mut_type = DS_MUT;
    else if (mutstr == "DIV_INS") m_mut_type = DS_INS;
    else if (mutstr == "DIV_DEL") m_mut_type = DS_DEL;
    else if (mutstr == "DIV_UNIFORM") m_mut_type = DS_UNIFORM;
    else if (mutstr == "DIV_SLIP") m_mut_type = DS_SLIP;
    
    else if (mutstr == "DIVIDE_MUT") m_mut_type = D1_MUT;
    else if (mutstr == "DIVIDE_INS") m_mut_type = D1_INS;
    else if (mutstr == "DIVIDE_DEL") m_mut_type = D1_DEL;
    else if (mutstr == "DIVIDE_UNIFORM") m_mut_type = D1_UNIFORM;
    else if (mutstr == "DIVIDE_SLIP") m_mut_type = D1_SLIP;
    
    else if (mutstr == "PARENT_MUT") m_mut_type = P_MUT;
    else if (mutstr == "PARENT_INS") m_mut_type = P_INS;
    else if (mutstr == "PARENT_DEL") m_mut_type = P_DEL;
    else if (mutstr == "DEATH") m_mut_type = DEATH;
    else if (mutstr == "POINT_MUT") m_mut_type = PNT_MUT;
    else if (mutstr == "POINT_INS") m_mut_type = PNT_INS;
    else if (mutstr == "POINT_DEL") m_mut_type = PNT_DEL;
    else if (mutstr == "INJECT_MUT") m_mut_type = I_MUT;
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
  
  static const cString GetDescription() { return "Arguments: [string mut_type='COPY_MUT'] [double prob=0.0] [int start_cell=-1] [int end_cell=-1]"; }
  
  void Process(cAvidaContext&)
  {
    if (m_setconf) {
      switch (m_mut_type) {
        case POINT: m_world->GetConfig().POINT_MUT_PROB.Set(m_prob); break;
          
        case C_MUT: m_world->GetConfig().COPY_MUT_PROB.Set(m_prob); break;
        case C_INS: m_world->GetConfig().COPY_INS_PROB.Set(m_prob); break;
        case C_DEL: m_world->GetConfig().COPY_DEL_PROB.Set(m_prob); break;
        case C_UNIFORM: m_world->GetConfig().COPY_UNIFORM_PROB.Set(m_prob); break;
        case C_SLIP: m_world->GetConfig().COPY_SLIP_PROB.Set(m_prob); break;
          
        case DS_MUT: m_world->GetConfig().DIV_MUT_PROB.Set(m_prob); break;
        case DS_INS: m_world->GetConfig().DIV_INS_PROB.Set(m_prob); break;
        case DS_DEL: m_world->GetConfig().DIV_DEL_PROB.Set(m_prob); break;
        case DS_UNIFORM: m_world->GetConfig().DIV_UNIFORM_PROB.Set(m_prob); break;
        case DS_SLIP: m_world->GetConfig().DIV_SLIP_PROB.Set(m_prob); break;
          
        case D1_MUT: m_world->GetConfig().DIVIDE_MUT_PROB.Set(m_prob); break;
        case D1_INS: m_world->GetConfig().DIVIDE_INS_PROB.Set(m_prob); break;
        case D1_DEL: m_world->GetConfig().DIVIDE_DEL_PROB.Set(m_prob); break;
        case D1_UNIFORM: m_world->GetConfig().DIVIDE_UNIFORM_PROB.Set(m_prob); break;
        case D1_SLIP: m_world->GetConfig().DIVIDE_SLIP_PROB.Set(m_prob); break;
          
        case P_MUT: m_world->GetConfig().PARENT_MUT_PROB.Set(m_prob); break;
        case P_INS: m_world->GetConfig().PARENT_INS_PROB.Set(m_prob); break;
        case P_DEL: m_world->GetConfig().PARENT_DEL_PROB.Set(m_prob); break;
        case DEATH: m_world->GetConfig().DEATH_PROB.Set(m_prob); break;
        case PNT_MUT: m_world->GetConfig().POINT_MUT_PROB.Set(m_prob); break;
        case PNT_INS: m_world->GetConfig().POINT_INS_PROB.Set(m_prob); break;
        case PNT_DEL: m_world->GetConfig().POINT_DEL_PROB.Set(m_prob); break;
        case I_MUT: m_world->GetConfig().INJECT_MUT_PROB.Set(m_prob); break;
        case I_INS: m_world->GetConfig().INJECT_INS_PROB.Set(m_prob); break;
        case I_DEL: m_world->GetConfig().INJECT_DEL_PROB.Set(m_prob); break;
        default:
          return;
      }
    }
    
    switch (m_mut_type) {
        
      case C_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyMutProb(m_prob); break;
      case C_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyInsProb(m_prob); break;
      case C_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyDelProb(m_prob); break;
      case C_UNIFORM: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyUniformProb(m_prob); break;
      case C_SLIP: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopySlipProb(m_prob); break;
        
      case DS_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivMutProb(m_prob); break;
      case DS_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivInsProb(m_prob); break;
      case DS_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivDelProb(m_prob); break;
      case DS_UNIFORM: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivUniformProb(m_prob); break;
      case DS_SLIP: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivSlipProb(m_prob); break;
        
      case D1_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideMutProb(m_prob); break;
      case D1_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideInsProb(m_prob); break;
      case D1_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideDelProb(m_prob); break;
      case D1_UNIFORM: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideUniformProb(m_prob); break;
      case D1_SLIP: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideSlipProb(m_prob); break;
        
      case P_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentMutProb(m_prob); break;
      case P_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentInsProb(m_prob); break;
      case P_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentDelProb(m_prob); break;
      case DEATH: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDeathProb(m_prob); break;
      case PNT_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointMutProb(m_prob); break;
      case PNT_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointInsProb(m_prob); break;
      case PNT_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointDelProb(m_prob); break;
      case I_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectMutProb(m_prob); break;
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
  enum {
    POINT,
    C_MUT, C_INS, C_DEL, C_UNIFORM, C_SLIP,
    DS_MUT, DS_INS, DS_DEL, DS_UNIFORM, DS_SLIP,
    D1_MUT, D1_INS, D1_DEL, D1_UNIFORM, D1_SLIP,
    P_MUT, P_INS, P_DEL, DEATH, PNT_MUT, PNT_INS, PNT_DEL,
    I_MUT, I_INS, I_DEL
  } m_mut_type;
  
  double m_prob;
  int m_start;
  int m_end;
  bool m_setconf;
  
public:
  cActionModMutProb(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_prob(0.0), m_start(-1), m_end(-1), m_setconf(false)
  {
    cString mutstr("COPY_MUT");
    
    cString largs(args);
    if (largs.GetSize()) mutstr = largs.PopWord().ToUpper();
    if (largs.GetSize()) m_prob = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_end = largs.PopWord().AsInt();
    
    if (mutstr == "POINT") m_mut_type = POINT;
    
    else if (mutstr == "COPY_MUT") m_mut_type = C_MUT;
    else if (mutstr == "COPY_INS") m_mut_type = C_INS;
    else if (mutstr == "COPY_DEL") m_mut_type = C_DEL;
    else if (mutstr == "COPY_UNIFORM") m_mut_type = C_UNIFORM;
    else if (mutstr == "COPY_SLIP") m_mut_type = C_SLIP;
    
    else if (mutstr == "DIV_MUT") m_mut_type = DS_MUT;
    else if (mutstr == "DIV_INS") m_mut_type = DS_INS;
    else if (mutstr == "DIV_DEL") m_mut_type = DS_DEL;
    else if (mutstr == "DIV_UNIFORM") m_mut_type = DS_UNIFORM;
    else if (mutstr == "DIV_SLIP") m_mut_type = DS_SLIP;
    
    else if (mutstr == "DIVIDE_MUT") m_mut_type = D1_MUT;
    else if (mutstr == "DIVIDE_INS") m_mut_type = D1_INS;
    else if (mutstr == "DIVIDE_DEL") m_mut_type = D1_DEL;
    else if (mutstr == "DIVIDE_UNIFORM") m_mut_type = D1_UNIFORM;
    else if (mutstr == "DIVIDE_SLIP") m_mut_type = D1_SLIP;
    
    else if (mutstr == "PARENT_MUT") m_mut_type = P_MUT;
    else if (mutstr == "PARENT_INS") m_mut_type = P_INS;
    else if (mutstr == "PARENT_DEL") m_mut_type = P_DEL;
    else if (mutstr == "DEATH") m_mut_type = DEATH;
    else if (mutstr == "POINT_MUT") m_mut_type = PNT_MUT;
    else if (mutstr == "POINT_INS") m_mut_type = PNT_INS;
    else if (mutstr == "POINT_DEL") m_mut_type = PNT_DEL;
    else if (mutstr == "INJECT_MUT") m_mut_type = I_MUT;
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
  
  static const cString GetDescription() { return "Arguments: [string mut_type='COPY_MUT'] [double prob=0.0] [int start_cell=-1] [int end_cell=-1]"; }
  
  void Process(cAvidaContext&)
  {
    double prob = m_prob;
    
    switch (m_mut_type) {
      case POINT: prob += m_world->GetConfig().POINT_MUT_PROB.Get(); break;
        
      case C_MUT: prob += m_world->GetConfig().COPY_MUT_PROB.Get(); break;
      case C_INS: prob += m_world->GetConfig().COPY_INS_PROB.Get(); break;
      case C_DEL: prob += m_world->GetConfig().COPY_DEL_PROB.Get(); break;
      case C_UNIFORM: prob += m_world->GetConfig().COPY_MUT_PROB.Get(); break;
      case C_SLIP: prob += m_world->GetConfig().COPY_MUT_PROB.Get(); break;
        
      case DS_MUT: prob += m_world->GetConfig().DIV_MUT_PROB.Get(); break;
      case DS_INS: prob += m_world->GetConfig().DIV_INS_PROB.Get(); break;
      case DS_DEL: prob += m_world->GetConfig().DIV_DEL_PROB.Get(); break;
      case DS_UNIFORM: prob += m_world->GetConfig().DIV_MUT_PROB.Get(); break;
      case DS_SLIP: prob += m_world->GetConfig().DIV_MUT_PROB.Get(); break;
        
      case D1_MUT: prob += m_world->GetConfig().DIVIDE_MUT_PROB.Get(); break;
      case D1_INS: prob += m_world->GetConfig().DIVIDE_INS_PROB.Get(); break;
      case D1_DEL: prob += m_world->GetConfig().DIVIDE_DEL_PROB.Get(); break;
      case D1_UNIFORM: prob += m_world->GetConfig().DIVIDE_MUT_PROB.Get(); break;
      case D1_SLIP: prob += m_world->GetConfig().DIVIDE_MUT_PROB.Get(); break;
        
      case P_MUT: prob += m_world->GetConfig().PARENT_MUT_PROB.Get(); break;
      case P_INS: prob += m_world->GetConfig().PARENT_INS_PROB.Get(); break;
      case P_DEL: prob += m_world->GetConfig().PARENT_DEL_PROB.Get(); break;
      case DEATH: prob += m_world->GetConfig().DEATH_PROB.Get(); break;
      case PNT_MUT: prob += m_world->GetConfig().POINT_MUT_PROB.Get(); break;
      case PNT_INS: prob += m_world->GetConfig().POINT_INS_PROB.Get(); break;
      case PNT_DEL: prob += m_world->GetConfig().POINT_DEL_PROB.Get(); break;
      case I_MUT: prob += m_world->GetConfig().INJECT_MUT_PROB.Get(); break;
      case I_INS: prob += m_world->GetConfig().INJECT_INS_PROB.Get(); break;
      case I_DEL: prob += m_world->GetConfig().INJECT_DEL_PROB.Get(); break;
      default:
        return;
    }
    
    if (m_setconf) {
      switch (m_mut_type) {
        case POINT: m_world->GetConfig().POINT_MUT_PROB.Set(prob); break;
          
        case C_MUT: m_world->GetConfig().COPY_MUT_PROB.Set(prob); break;
        case C_INS: m_world->GetConfig().COPY_INS_PROB.Set(prob); break;
        case C_DEL: m_world->GetConfig().COPY_DEL_PROB.Set(prob); break;
        case C_UNIFORM: m_world->GetConfig().COPY_UNIFORM_PROB.Set(prob); break;
        case C_SLIP: m_world->GetConfig().COPY_SLIP_PROB.Set(prob); break;
          
        case DS_MUT: m_world->GetConfig().DIV_MUT_PROB.Set(prob); break;
        case DS_INS: m_world->GetConfig().DIV_INS_PROB.Set(prob); break;
        case DS_DEL: m_world->GetConfig().DIV_DEL_PROB.Set(prob); break;
        case DS_UNIFORM: m_world->GetConfig().DIV_UNIFORM_PROB.Set(prob); break;
        case DS_SLIP: m_world->GetConfig().DIV_SLIP_PROB.Set(prob); break;
          
        case D1_MUT: m_world->GetConfig().DIVIDE_MUT_PROB.Set(prob); break;
        case D1_INS: m_world->GetConfig().DIVIDE_INS_PROB.Set(prob); break;
        case D1_DEL: m_world->GetConfig().DIVIDE_DEL_PROB.Set(prob); break;
        case D1_UNIFORM: m_world->GetConfig().DIVIDE_UNIFORM_PROB.Set(prob); break;
        case D1_SLIP: m_world->GetConfig().DIVIDE_SLIP_PROB.Set(prob); break;
          
        case P_MUT: m_world->GetConfig().PARENT_MUT_PROB.Set(m_prob); break;
        case P_INS: m_world->GetConfig().PARENT_INS_PROB.Set(m_prob); break;
        case P_DEL: m_world->GetConfig().PARENT_DEL_PROB.Set(m_prob); break;
        case DEATH: m_world->GetConfig().DEATH_PROB.Set(m_prob); break;
        case PNT_MUT: m_world->GetConfig().POINT_MUT_PROB.Set(m_prob); break;
        case PNT_INS: m_world->GetConfig().POINT_INS_PROB.Set(m_prob); break;
        case PNT_DEL: m_world->GetConfig().POINT_DEL_PROB.Set(m_prob); break;
        case I_MUT: m_world->GetConfig().INJECT_MUT_PROB.Set(prob); break;
        case I_INS: m_world->GetConfig().INJECT_INS_PROB.Set(prob); break;
        case I_DEL: m_world->GetConfig().INJECT_DEL_PROB.Set(prob); break;
        default:
          return;
      }
    }
    
    switch (m_mut_type) {
      case C_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyMutProb(prob); break;
      case C_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyInsProb(prob); break;
      case C_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyDelProb(prob); break;
      case C_UNIFORM: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopyUniformProb(prob); break;
      case C_SLIP: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetCopySlipProb(prob); break;
        
      case DS_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivMutProb(prob); break;
      case DS_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivInsProb(prob); break;
      case DS_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivDelProb(prob); break;
      case DS_UNIFORM: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivUniformProb(prob); break;
      case DS_SLIP: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivSlipProb(prob); break;
        
      case D1_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideMutProb(prob); break;
      case D1_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideInsProb(prob); break;
      case D1_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideDelProb(prob); break;
      case D1_UNIFORM: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideUniformProb(prob); break;
      case D1_SLIP: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDivideSlipProb(prob); break;
        
        
      case P_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentMutProb(prob); break;
      case P_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentInsProb(prob); break;
      case P_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetParentDelProb(prob); break;
      case DEATH: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetDeathProb(prob); break;
      case PNT_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointMutProb(prob); break;
      case PNT_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointInsProb(prob); break;
      case PNT_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetPointDelProb(prob); break;
      case I_MUT: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectMutProb(prob); break;
      case I_INS: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectInsProb(prob); break;
      case I_DEL: for (int i = m_start; i < m_end; i++) m_world->GetPopulation().GetCell(i).MutationRates().SetInjectDelProb(prob); break;
      default:
        return;
    }
  }
};






class cActionSwapCells : public cAction
{
private:
  int id1;
  int id2;
  
public:
  cActionSwapCells(cWorld* world, const cString& args, Feedback&) : cAction(world, args), id1(-1), id2(-1)
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
      ctx.Driver().Feedback().Warning("SwapCells cell ID out of range");
      return;
    }
    if (id1 == id2) {
      ctx.Driver().Feedback().Warning("SwapCells cell IDs identical");
    }
    
    m_world->GetPopulation().SwapCells(id1, id2, ctx); 
  }
};










class cActionRemovePredators : public cAction
{
private:
  
public:
  cActionRemovePredators(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args)
  {
  }
  
  static const cString GetDescription() { return "Arguments: ''"; }

  void Process(cAvidaContext& ctx)
  { 
    m_world->GetPopulation().RemovePredators(ctx);
  }
};

/*
 Modifies an instruction's redundancy during a run 
 Parameters:
 instruction [required] = which instruction to modify
 redundancy [required] = new redundancy value
 (@CHC, March 2, 2012)
 */
class cActionSetRedundancy : public cAction
{
private:
	cString m_inst_name;
	double m_redundancy;
public:
	cActionSetRedundancy(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
	{
		cString largs(args);
		m_inst_name = largs.PopWord();
		
		//Make sure that the instruction the user has specified is in the instruction set
		assert(world->GetHardwareManager().GetDefaultInstSet().InstInSet(m_inst_name));
    
		//Get the new redundancy
		m_redundancy = largs.PopWord().AsInt();
	}
	
	static const cString GetDescription() { return "Arguments: <char instruction> <int redundancy>"; }
	
	void Process(cAvidaContext& ctx)
	{
		//cInstSet& is = m_world->GetHardwareManager().GetInstSet(m_world->GetHardwareManager().GetDefaultInstSet().GetInstSetName());
		cInstSet& is = m_world->GetHardwareManager().GetInstSet("(default)");
		Instruction inst = is.GetInst(m_inst_name);
		is.SetRedundancy(inst, m_redundancy);
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
	
  action_lib->Register<cActionInjectGroup>("InjectGroup");
  
  action_lib->Register<cActionKillInstLimit>("KillInstLimit");
  action_lib->Register<cActionKillInstPair>("KillInstPair");
  action_lib->Register<cActionKillProb>("KillProb");
  action_lib->Register<cActionKillProb>("KillProb");
  action_lib->Register<cActionApplyBottleneck>("ApplyBottleneck");
  action_lib->Register<cActionKillDominantGenotype>("KillDominantGenotype");
  action_lib->Register<cActionAttackDen>("AttackDen");
  action_lib->Register<cActionRaidDen>("RaidDen");
  action_lib->Register<cActionKillFractionInSequence>("KillFractionInSequence");
  action_lib->Register<cActionKillFractionInSequence_PopLimit>("KillFractionInSequence_PopLimit");
	
	
  action_lib->Register<cActionToggleRewardInstruction>("ToggleRewardInstruction");
  action_lib->Register<cActionToggleFitnessValley>("ToggleFitnessValley");
  action_lib->Register<cActionKillProb>("KillRate");
  action_lib->Register<cActionKillRectangle>("KillRectangle");
  
  action_lib->Register<cActionSetMutProb>("SetMutProb");
  action_lib->Register<cActionModMutProb>("ModMutProb");
	
  action_lib->Register<cActionSetRedundancy>("SetRedundancy");
	
	
  action_lib->Register<cActionSwapCells>("SwapCells");
	
	
  action_lib->Register<cActionRemovePredators>("RemovePredators");
}
