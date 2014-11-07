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

#include "AvidaTools.h"

#include "avida/core/Feedback.h"
#include "avida/core/InstructionSequence.h"
#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"

#include "avida/private/util/GenomeLoader.h"

#include "apto/stat/Accumulator.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cCodeLabel.h"
#include "cDoubleSum.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHardwareStatusPrinter.h"
#include "cInstSet.h"
#include "cOrgMessagePredicate.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cWorld.h"
#include "cOrganism.h"
#include "cEnvironment.h"
#include "cUserFeedback.h"
#include "cArgSchema.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <set>

#include "stdlib.h"

using namespace Avida;
using namespace AvidaTools;


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
  int m_lineage_label;
  double m_neutral_metric;
  cString m_trace_filename;
public:
  cActionInject(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_id(0), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_trace_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [int cell_id=0] [double merit=-1] [int lineage_label=0] [double neutral_metric=0] [string trace_filename]"; }
  
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
    m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, m_cell_id, m_merit, m_lineage_label, m_neutral_metric, false);
    
    if (m_trace_filename.GetSize()) {
      HardwareTracerPtr tracer(new cHardwareStatusPrinter(m_world->GetNewWorld(), (const char*)m_trace_filename));
      m_world->GetPopulation().GetCell(m_cell_id).GetOrganism()->GetHardware().SetTrace(tracer);
    }
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
  cActionInjectRandom(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_id(0), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
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
    const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
    HashPropertyMap props;
    cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
    Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence(m_length)));
    InstructionSequencePtr seq_p;
    seq_p.DynamicCastFrom(genome.Representation());
    InstructionSequence& seq = *seq_p;
    for (int i = 0; i < m_length; i++) seq[i] = is.GetRandomInst(ctx);
    m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "random", true), ctx, m_cell_id, m_merit, m_lineage_label, m_neutral_metric); 
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
  cActionInjectAllRandomRepro(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_merit(-1), m_lineage_label(0), m_neutral_metric(0), m_sex(0)
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
      
      m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "random", true), ctx, i, m_merit, m_lineage_label, m_neutral_metric); 
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
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectAll(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args), m_merit(-1.0), m_lineage_label(0), m_neutral_metric(0)
  {

    cArgSchema schema(':','=');
    
    // Entries
    schema.AddEntry("filename", 0, cArgSchema::SCHEMA_STRING);
    schema.AddEntry("instset", 1, "");
    
    schema.AddEntry("merit", 0, -1.0);
    schema.AddEntry("neutral_metric", 1, 0.0);
    
    schema.AddEntry("lineage_label", 0, 0);
    
    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (argc) {
      m_filename = argc->GetString(0);
      m_instset = argc->GetString(1);
      
      m_merit = argc->GetDouble(0);
      m_neutral_metric = argc->GetDouble(1);
      
      m_lineage_label = argc->GetInt(0);
    }
    
    delete argc;
  }
  
  static const cString GetDescription() { return "Arguments: <string filename> [instset=""] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
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
      m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_lineage_label, m_neutral_metric); 
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
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectRange(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [int cell_start=0] [int cell_end=-1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
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
        m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_lineage_label, m_neutral_metric); 
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
  cActionInjectSequence(cWorld* world, const cString& args, Feedback&)
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
      ctx.Driver().Feedback().Warning("InjectSequence has invalid range!");
    } else {
      const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
      HashPropertyMap props;
      cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
      Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence((const char*)m_sequence)));
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_lineage_label, m_neutral_metric); 
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
  cActionInjectSequenceWithDivMutRate(cWorld* world, const cString& args, Feedback&)
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
      ctx.Driver().Feedback().Warning("InjectSequenceWithDivMutRate has invalid range!");
    } else {
      const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
      HashPropertyMap props;
      cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
      Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence((const char*)m_sequence)));
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_lineage_label, m_neutral_metric); 
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
  int m_lineage_label;
  double m_neutral_metric;
  cString m_trace_filename;
public:
  cActionInjectGroup(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_id(0), m_group_id(m_world->GetConfig().DEFAULT_GROUP.Get()), m_forager_type(-1), m_trace(false),m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_cell_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_group_id = largs.PopWord().AsInt();
    if (largs.GetSize()) m_forager_type = largs.PopWord().AsInt();
    if (largs.GetSize()) m_trace = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_trace_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [int cell_id=0] [int group_id=-1] [int forager_type=-1] [bool trace=false] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
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
    m_world->GetPopulation().InjectGroup(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, m_cell_id, m_merit, m_lineage_label, m_neutral_metric, m_group_id, m_forager_type, m_trace); 
  
    if (m_trace_filename.GetSize()) {
      HardwareTracerPtr tracer(new cHardwareStatusPrinter(m_world->GetNewWorld(), (const char*)m_trace_filename));
      m_world->GetPopulation().GetCell(m_cell_id).GetOrganism()->GetHardware().SetTrace(tracer);
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
  cString m_label;
  int m_cell_start;
  int m_cell_end;
public:
  cActionInjectParasite(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_start(0), m_cell_end(-1)
  {
    cString largs(args);
    m_filename = largs.PopWord();
    m_label = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  static const cString GetDescription() { return "Arguments: <string filename> <string label> [int cell_start=0] [int cell_end=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      ctx.Driver().Feedback().Warning("InjectParasite has invalid range!");
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
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(genome->Representation());
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().InjectParasite(m_label, *seq, i);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};

class cActionInjectParasiteSequence : public cAction
{
private:
  cString m_sequence;
  cString m_label;
  int m_cell_start;
  int m_cell_end;
public:
  cActionInjectParasiteSequence(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_start(0), m_cell_end(-1)
  {
    cString largs(args);
    m_sequence = largs.PopWord();
    m_label = largs.PopWord();
    if (largs.GetSize()) m_cell_start = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cell_end = largs.PopWord().AsInt();
    
    if (m_cell_end == -1) m_cell_end = m_cell_start + 1;
  }
  
  static const cString GetDescription() { return "Arguments: <string sequence> [int cell_start=0] [int cell_end=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_cell_start < 0 || m_cell_end > m_world->GetPopulation().GetSize() || m_cell_start >= m_cell_end) {
      ctx.Driver().Feedback().Warning("InjectParasite has invalid range!");
    } else {
      cUserFeedback feedback;
      const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
      HashPropertyMap props;
      cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
      Genome genome(is.GetHardwareType(), props, GeneticRepresentationPtr(new InstructionSequence((const char*)m_sequence)));
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(genome.Representation());
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().InjectParasite(m_label, *seq, i);
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
  cString m_label;
  int m_cell_start;
  int m_cell_end;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectParasitePair(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_cell_start(0), m_cell_end(-1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    m_filename_genome = largs.PopWord();
    m_filename_parasite = largs.PopWord();
    m_label = largs.PopWord();
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
      ctx.Driver().Feedback().Warning("InjectParasitePair has invalid range!");
    } else {
      GenomePtr genome, parasite;
      cUserFeedback feedback;
      genome = Util::LoadGenomeDetailFile(m_filename_genome, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback);
      parasite = Util::LoadGenomeDetailFile(m_filename_parasite, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback);
      for (int i = 0; i < feedback.GetNumMessages(); i++) {
        switch (feedback.GetMessageType(i)) {
          case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
          case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
          default: break;
        };
        cerr << feedback.GetMessage(i) << endl;
      }
      if (!genome || !parasite) return;
      for (int i = m_cell_start; i < m_cell_end; i++) {
        m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_lineage_label, m_neutral_metric); 
        ConstInstructionSequencePtr seq;
        seq.DynamicCastFrom(parasite->Representation());
        m_world->GetPopulation().InjectParasite(m_label, *seq, i);
      }
      m_world->GetPopulation().SetSyncEvents(true);
    }
  }
};

/*! Injects an organism into all demes in the population. 
 
 Parameters:
 filename (string):
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
class cActionInjectDemes : public cAction
{
private:
  cString m_filename;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectDemes(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
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
    if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
      for(int i=1; i<m_world->GetPopulation().GetNumDemes(); ++i) {  // first org has already been injected
        m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx,
                                        m_world->GetPopulation().GetDeme(i).GetCellID(0),
                                        m_merit, m_lineage_label, m_neutral_metric); 
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
      }
    } else {
      for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
        // WARNING: initial ancestor has already be injected into the population
        //           calling this will overwrite it.
        m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx,
                                        m_world->GetPopulation().GetDeme(i).GetCellID(0),
                                        m_merit, m_lineage_label, m_neutral_metric); 
        m_world->GetPopulation().GetDeme(i).IncInjectedCount();
        
      }
    }
  }
};

/*! Injects an organism into all demes modulo a given number in the population. 
 
 Parameters:
 filename (string):
 The filename of the genotype to load.
 modulo default: 1 -- when the deme number modulo this number is 0, inject org
 cell ID (integer) default: 0
 The grid-point into which the organism should be placed.
 merit (double) default: -1
 The initial merit of the organism. If set to -1, this is ignored.
 lineage label (integer) default: 0
 An integer that marks all descendants of this organism.
 neutral metric (double) default: 0
 A double value that randomly drifts over time.
 */
class cActionInjectModuloDemes : public cAction
{
private:
  cString m_filename;
  int m_mod_num;
  double m_merit;
  int m_lineage_label;
  double m_neutral_metric;
public:
  cActionInjectModuloDemes(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_mod_num(1), m_merit(-1), m_lineage_label(0), m_neutral_metric(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_mod_num = largs.PopWord().AsInt();
    if (largs.GetSize()) m_merit = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_lineage_label = largs.PopWord().AsInt();
    if (largs.GetSize()) m_neutral_metric = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> [int mod_num = 1] [double merit=-1] [int lineage_label=0] [double neutral_metric=0]"; }
  
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
    if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
      for(int i=1; i<m_world->GetPopulation().GetNumDemes(); ++i) {  // first org has already been injected
        if (i % m_mod_num == 0) {
          m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx,
                                          m_world->GetPopulation().GetDeme(i).GetCellID(0),
                                          m_merit, m_lineage_label, m_neutral_metric); 
          m_world->GetPopulation().GetDeme(i).IncInjectedCount();
        }
      }
    } else {
      for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
        // WARNING: initial ancestor has already be injected into the population
        //           calling this will overwrite it.
        if (i==0 || (i % m_mod_num) ==0){
          m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx,
																					m_world->GetPopulation().GetDeme(i).GetCellID(0),
																					m_merit, m_lineage_label, m_neutral_metric);  
          m_world->GetPopulation().GetDeme(i).IncInjectedCount();
        }
        
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
  cActionInjectDemesFromNest(cWorld* world, const cString& args, Feedback&):
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
        m_world->GetPopulation().Inject(m_world->GetPopulation().GetDeme(i).GetGermline().GetLatest(), Systematics::Source(Systematics::DUPLICATION, "germline", true), ctx,
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
  cActionInjectDemesRandom(cWorld* world, const cString& args, Feedback&):
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
          target_cellr = ctx.GetRandom().GetInt(0, deme_size-1);
          target_cell = m_world->GetPopulation().GetDeme(i).GetCellID(target_cellr);
        } while (m_world->GetPopulation().GetCell(target_cell).IsOccupied());
        
        assert(target_cell > -1);
        assert(target_cell < m_world->GetPopulation().GetSize());
        
        m_world->GetPopulation().Inject(m_world->GetPopulation().GetDeme(i).GetGermline().GetLatest(), Systematics::Source(Systematics::DUPLICATION, "germline", true),
                                        ctx, target_cell, m_merit,
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
    
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
      
      if (!cell.HasAV()) continue;
      
      Apto::Array<double> cell_res;
      cell_res = m_world->GetPopulation().GetCellResources(i, ctx);
      
      for (int j = 0; j < cell_res.GetSize(); j++) {
        if ((resource_lib.GetResource(j)->GetHabitat() == 4 ||resource_lib.GetResource(j)->GetHabitat() == 3) && cell_res[j] > 0) {
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
  cActionRaidDen(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args), m_loss(0.0), m_res_id(0), m_units_per(1.0)
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
    
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
      
      Apto::Array<double> cell_res;
      cell_res = m_world->GetPopulation().GetCellResources(i, ctx);
      
      for (int j = 0; j < cell_res.GetSize(); j++) {
        if ((resource_lib.GetResource(j)->GetHabitat() == 4 || resource_lib.GetResource(j)->GetHabitat() == 3) && cell_res[j] > 0) {
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
          m_world->GetPopulation().UpdateCellResources(ctx, res_change, i);
          
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
  cActionSerialTransfer(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_size(1), m_ignore_deads(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_size = largs.PopWord().AsInt();
    if (largs.GetSize()) m_ignore_deads = largs.PopWord().AsInt();
    
    if (m_size < 0) m_size = 1;
  }
  
  static const cString GetDescription() { return "Arguments: [int transfer_size=1] [int ignore_deads=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().SerialTransfer(m_size, m_ignore_deads, ctx); 
  }
};

class cActionSetMigrationRate : public cAction
{
private:
  double m_rate;
  
public:
  cActionSetMigrationRate(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_rate(0.0)
  {
    cString largs(args);
    if(largs.GetSize()) m_rate = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: [double rate=0.0]"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetConfig().MIGRATION_RATE.Set(m_rate);
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


class cActionZeroMuts : public cAction
{
public:
  cActionZeroMuts(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&)
  {
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      m_world->GetPopulation().GetCell(i).MutationRates().Clear();
    }
		bool Set(const cString& entry, const cString& val);
		const cString cpy = "GERMLINE_COPY_MUT";
		const cString ins = "GERMLINE_INS_MUT";
		const cString del = "GERMLINE_DEL_MUT";
    const cString inst = "INST_POINT_MUT_PROB";
    
		const cString val = "0.0";
		
		m_world->GetConfig().Set(cpy, val);
		m_world->GetConfig().Set(ins, val);
		m_world->GetConfig().Set(del, val);
    m_world->GetConfig().Set(inst, val);
    
  }
};


/*! This action enables the tracking of all messages that are sent in each deme. */
class cActionTrackAllMessages : public cAction {
public:
  cActionTrackAllMessages(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { }
	
  static const cString GetDescription() { return "No Arguments"; }
	
  void Process(cAvidaContext&) {
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
  cActionCompeteDemes(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_type(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_type = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int type=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().CompeteDemes(ctx, m_type); 
  }
};


/*! Assign a random identifier to the data for each cell and save those IDs for later
 use, respecting deme boundaries.
 
 This is a little hackish; feel free to modify.
 */
class cAssignRandomCellData : public cAction { 
public:
	typedef std::vector<int> CellIDList;
  typedef std::map<int, std::set<int> > DataMap;
	
	//! Constructor.
	cAssignRandomCellData(cWorld* world, const cString& args, Feedback&) : cAction(world, args), _num_cells(0) {
		if(args.GetSize()) {
			cString largs(args);
			_num_cells = largs.PopWord().AsInt();
		}		
	}
  
	//! Destructor.
	virtual ~cAssignRandomCellData() { }
  
	//! Description of this event; only possible argument is the number of cells whose data is to be set.
  static const cString GetDescription() { return "Arguments: [num_cells=deme_size]"; }
  
	//! Process this event, setting the requested number of cell's data to a random value.
  virtual void Process(cAvidaContext& ctx) {
		for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
      cDeme& deme = m_world->GetPopulation().GetDeme(i);
			zero_cell_data(deme);
			if((_num_cells == 0) || (_num_cells >= deme.GetSize())) {
				// Assign random data to each cell:
				for(int j=0; j<deme.GetSize(); ++j) {
					int d = ctx.GetRandom().GetInt(INT_MAX);
					deme.GetCell(j).SetCellData(d);
					// Save that data by deme in the map:
					deme_to_id[deme.GetID()].insert(d);
				}
			} else {
				// Assign random data to exactly num_cells cells, with replacement:
				for(int j=0; j<_num_cells; ++j) {
					int cell = ctx.GetRandom().GetInt(deme.GetSize());
					int d = ctx.GetRandom().GetInt(INT_MAX);
					deme.GetCell(cell).SetCellData(d);
					// Save that data by deme in the map:
					deme_to_id[deme.GetID()].insert(d);
				}
			}
    }
  }
  
  static bool IsCellDataInDeme(int data, const cDeme& deme) { 
    DataMap::iterator i = deme_to_id.find(deme.GetID());
    return i->second.find(data) != i->second.end();
  }
	
	static int CellIDFromData(int data, const cDeme& deme) { 
		for(int i=0; i<deme.GetSize(); ++i) {
			if(deme.GetCell(i).GetCellData() == data) {
				return i;
			}
		}
		return -1;
  }
	
	static CellIDList ReplaceCellData(int old_data, int new_data, const cDeme& deme) {
		CellIDList cell_ids;
		// Find all cells in the deme that hold the old data, and replace it with the new.
		for(int i=0; i<deme.GetSize(); ++i) {
			if(deme.GetCell(i).GetCellData() == old_data) {
				deme.GetCell(i).SetCellData(new_data);
				cell_ids.push_back(i);
			}
		}
		// Update the data map.
		DataMap::iterator i = deme_to_id.find(deme.GetID());
		i->second.erase(old_data);
		i->second.insert(new_data);
		return cell_ids;
	}
	
	//! Replace the cell data in *one* cell.
	static void ReplaceCellData(cPopulationCell& cell, int new_data, const cDeme& deme) {		
		CellIDList cell_ids;
		// Find all cells in the deme that hold the cell's data:
		for(int i=0; i<deme.GetSize(); ++i) {
			if(deme.GetCell(i).GetCellData() == cell.GetCellData()) {
				cell_ids.push_back(i);
			}
		}
		
		// Only remove the data from the data map if there's exactly one cell with this data:
		DataMap::iterator i = deme_to_id.find(deme.GetID());
		if(cell_ids.size() == 1) {
			i->second.erase(cell.GetCellData());
		}
		
		// Reset the cell's data, and add the new data to the map:
		cell.SetCellData(new_data);
		i->second.insert(new_data);
	}
	
	static const std::set<int>& GetDataInDeme(const cDeme& deme) {
		return deme_to_id[deme.GetID()];
	}
  
protected:
	virtual void zero_cell_data(cDeme& deme) {
		deme_to_id[deme.GetID()].clear();
		for(int j=0; j<deme.GetSize(); ++j) {
			deme.GetCell(j).SetCellData(0);
		}
	}
	
  static std::map<int, std::set<int> > deme_to_id; //!< Map of deme ID -> set of all cell data in that deme.
	int _num_cells; //!< The number of cells in each deme whose cell-data is to be set.
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
  cAbstractCompeteDemes(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { }
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
      fitness.push_back(Fitness(m_world->GetPopulation().GetDeme(i), ctx));
      assert(fitness.back() >= 0.0);
    }
    m_world->GetPopulation().CompeteDemes(fitness, ctx); 
  }
	
  /*! Deme fitness function, to be overriden by specific types of deme competition.
   */
  virtual double Fitness(cDeme& deme, cAvidaContext& ctx) = 0;
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
  cAbstractMonitoringCompeteDemes(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback), _compete_period(100)
  {
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
      _update_fitness[i] += Update(m_world->GetPopulation().GetDeme(i), ctx); 
    }
    
    if((m_world->GetStats().GetUpdate() % _compete_period) == 0) {
      std::vector<double> fitness;
      for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
        fitness.push_back(pow(_update_fitness[i] + Fitness(m_world->GetPopulation().GetDeme(i), ctx), 2.0));
        assert(fitness.back() >= 0.0);
      }			
      m_world->GetPopulation().CompeteDemes(fitness, ctx); 
      _update_fitness.clear();
      Clear();
    }
  }
  
  //! Called on each action invocation, *including* immediately prior to fitness calculation.
  virtual double Update(cDeme& deme, cAvidaContext& ctx) = 0; 
  //! Called after demes compete, so that subclasses can clean up any state.
  virtual void Clear() { }
  
protected:
  int _compete_period; //!< Period at which demes compete.
  std::vector<double> _update_fitness; //!< Running sum of returns from Update(cDeme).
};


/*! Competes demes based on the networks they've constructed.
 */
class cActionCompeteDemesByNetwork : public cAbstractCompeteDemes {
public:
	//! Constructor.
	cActionCompeteDemesByNetwork(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback)
  {
	}
	
	//! Destructor.
	virtual ~cActionCompeteDemesByNetwork() { }
	
	//! Retrieve this class's description.
	static const cString GetDescription() { return "No arguments."; }
	
	//! Calculate the current fitness of this deme.
  virtual double Fitness(cDeme& deme, cAvidaContext&) {
		return deme.GetNetwork().Fitness();
	}
};


/*! Measure statistics of all deme networks.
 */
class cActionMeasureDemeNetworks : public cAction {
public:
	//! Constructor.
	cActionMeasureDemeNetworks(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
	}
	
	//! Retrieve this class's description.
	static const cString GetDescription() { return "No arguments."; }
	
	//! Called to process this event.
  virtual void Process(cAvidaContext&) {
		for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {			
			cDeme& deme = m_world->GetPopulation().GetDeme(i);
			m_world->GetStats().NetworkTopology(deme.GetNetwork().Measure());
		}
	}
	
protected:
};


/*! This class rewards for data distribution among organisms in a deme.
 
 Specifically, "data" injected into a single cell-data field in the deme should eventually
 be readable from another cell in the same deme.
 
 For injecting data, we use the AssignRandomCellData event (num_cells=1), and we use opinions
 to determine when data has reached another cell.
 
 This action does not examine efficiency of distributing data.
 */
class cActionDistributeData : public cAbstractCompeteDemes {
public:
	cActionDistributeData(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback)
  {
		world->GetStats().AddMessagePredicate(&m_message_counter);
	}
	
	//! Destructor.
	virtual ~cActionDistributeData() { }
	
	static const cString GetDescription() { return "No arguments."; }
	
	virtual void Process(cAvidaContext& ctx) {
		cAbstractCompeteDemes::Process(ctx);
		m_message_counter.Reset();
	}
  
	//! Calculate the current fitness of this deme.
	virtual double Fitness(cDeme& deme, cAvidaContext&) {
		return pow((double)received_data(deme) + 1.0, 2.0);
	}
	
protected:
	//! Return how many organisms have received the data, and then set their opinion correctly.	
	unsigned int received_data(const cDeme& deme) {
		// What is the data we're trying to distribute in this deme?
		int data = *cAssignRandomCellData::GetDataInDeme(deme).begin();
		
		// How many organisms in this deme are reflecting that piece of data?
		unsigned int count=0;
		for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
			if((org != 0) && org->HasOpinion() && (org->GetOpinion().first==data)) {
				++count;
			}
		}
		return count;
	}
	
	cOrgMessagePred_CountDemeMessages m_message_counter;
};


class cActionDistributeDataEfficiently : public cActionDistributeData {
public:
	cActionDistributeDataEfficiently(cWorld* world, const cString& args, Feedback& feedback)
  : cActionDistributeData(world, args, feedback)
  {
	}
	
	//! Destructor.
	virtual ~cActionDistributeDataEfficiently() { }
	
	static const cString GetDescription() { return "No arguments."; }
	
	//! Calculate the current fitness of this deme.
	virtual double Fitness(cDeme& deme, cAvidaContext&) {
		// First, get the number that have received the data (and set their opinion):
		unsigned int received = received_data(deme);
		
		// If not everyone has the data yet, we're done:
		if(received < (unsigned int)deme.GetSize()) {
			return pow((double)received + 1.0, 2.0);
		}
		
		// Now, reward for reducing the number of messages that were used:
		// The size of the deme is the theoretical minimum number of messages that could be used.
		double size = deme.GetSize() * 1000; // Scaled by 1000 (arbitrary) to get the fraction > 1.0.
		double msg_count = m_message_counter.GetMessageCount(deme);
		return pow(received + 1.0 + size / msg_count, 2.0);
	}	
};



class cActionDistributeDataCheaply : public cActionDistributeData {
public:
	cActionDistributeDataCheaply(cWorld* world, const cString& args, Feedback& feedback)
  : cActionDistributeData(world, args, feedback)
  {
		m_world->GetConfig().DEME_NETWORK_TOPOLOGY_FITNESS.Set(4); // link length sum
	}
	
	//! Destructor.
	virtual ~cActionDistributeDataCheaply() { }
	
	static const cString GetDescription() { return "No arguments."; }
	
	//! Calculate the current fitness of this deme.
	virtual double Fitness(cDeme& deme, cAvidaContext&) {
		// First, get the number that have received the data (and set their opinion):
		unsigned int received = received_data(deme);
		
		// If not everyone has the data yet, we're done:
		if(received < (unsigned int)deme.GetSize()) {
			return pow((double)received + 1.0, 2.0);
		}
		
		// sum the euclidean lengths of all links in the network:
		double link_length_sum = deme.GetNetwork().Fitness(false);
		return pow(received + 1.0 + 1000.0/link_length_sum, 2.0);
	}	
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
	
	/*! Returns a pair (support, have_opinion), where support is the number of organisms
	 within the deme that have set their opinion to the given value, and have_opinion is the
	 number of organisms that have set their opinion at all.
	 */
	virtual std::pair<unsigned int, int> support(const cDeme& deme, const cOrganism::Opinion opinion) {
		unsigned int support=0;
		int have_opinion=0;
		// For each organism in the deme:
		for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
			// if the org has an opinion, and it's the right value, count it:
			if((org != 0) && org->HasOpinion()) {
				++have_opinion;
				if(org->GetOpinion().first == opinion) {
					++support;
				}
			}
		}
		return std::make_pair(support, have_opinion);
	}
	
	/*! Returns the number of organisms that have set an opinion. */
	int count_opinions(const cDeme& deme) { 
		int have_opinion=0;
		// For each organism in the deme:
		for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
			// if the org has an opinion, and it's the right value, count it:
			if((org != 0) && org->HasOpinion()) {
				++have_opinion;
			}
		}
		return have_opinion;
	}
	
	/*! Returns the set of organism opinions */
	std::set<cOrganism::Opinion> unique_opinions(const cDeme& deme) {
		std::set<cOrganism::Opinion> opinions; 
		for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
			// if the org has an opinion, and it's the right value, count it:
			if((org != 0) && org->HasOpinion()) {
				opinions.insert(org->GetOpinion().first);
			}
		}
		return opinions;
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
	
  struct state {
    state(cOrganism::Opinion o) : opinion(o), current(0), max(0) { }
    void reset() { opinion = 0; current = 0; max = 0; }
    cOrganism::Opinion opinion;
    int current;
    int max;
  };
  
  typedef std::map<int, state> DemeState; //!< To support hold-times for consensus.	
  
  //! Constructor.
  cActionIteratedConsensus(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractMonitoringCompeteDemes(world, args, feedback)
  , _replace(1), _kill(1), _hold(1), _restrict_range(1), _dont_replace(0)
  {
    if(args.GetSize()) {
      cString largs(args);
      largs.PopWord(); //iterations
      _replace = largs.PopWord().AsInt(); // how many cell data are replaced on consensus (default 1)
      if(largs.GetSize()) {
        _kill = largs.PopWord().AsInt(); // whether to kill the owners of replaced cell data (default 1)
        if(largs.GetSize()) {
          _restrict_range = largs.PopWord().AsInt(); // whether replaced cell data is restricted to the range [min, max).
          if(largs.GetSize()) {
            _dont_replace = largs.PopWord().AsInt();
          }
        }
      }
    }
    
    // hold time is a configuration option, not an event parameter.
    _hold = m_world->GetConfig().CONSENSUS_HOLD_TIME.Get();
  }
  
  //! Destructor.
  virtual ~cActionIteratedConsensus() { }
  
  static const cString GetDescription() { return "Arguments: [int compete_period=100 [int replace_number=1 [int kill=1 [int restrict_range=1]]]]"; }
  
  //! Calculate the current fitness of this deme.
  virtual double Fitness(cDeme& deme, cAvidaContext& ctx) {
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
	 
	 If _kill is > 0, we also kill the organisms living in the cells that had their
	 data replaced - In all cases, the germline is used to repopulate the cell.
	 
	 Called during every update (depending on configuration).  Return values are
	 summed and included in final fitness calculation. 
	 
	 The intent behind this version of Update() is that for each consensus round, we
	 only reward *once* for each level of hold time that is reached.
	 */
	virtual double Update(cDeme& deme, cAvidaContext& ctx) { 
		// find the current maximally-supported opinion, and get this deme's
		// state information
		std::pair<unsigned int, cOrganism::Opinion> support = max_support(deme);
		DemeState::iterator st = _state.find(deme.GetID());
		
		// if we're not at consensus, set our current time to 0:
		if(support.first < static_cast<unsigned int>(deme.GetSize())) {
			if(st != _state.end()) { 
				st->second.current = 0;
			}
			return 0.0;
		}
		
		// ok, we're at consensus - is this the first time?
		if(st == _state.end()) {
			// need to add new state information:
			std::pair<DemeState::iterator, bool> ins = _state.insert(std::make_pair(deme.GetID(), state(support.second)));
			st = ins.first;
		} else if(support.second != st->second.opinion) {
			// is our current opinion different than our last?
			st->second.opinion = support.second;
			st->second.current = 0;
		}
		
		// since we're at consensus, unconditionally update this deme's consensus counter:
		++st->second.current;
		
		// has this opinion been held long enough?
		if(st->second.current >= _hold) {
			// yes; this is now a "true" consensus.
			ConsensusReached(deme, support, ctx); 
			// erase our state!
			_state.erase(st);
			// and reward by the size of the deme.
			return deme.GetSize();
		}
		
		// if this is a new level of consensus for this round, meaning that we haven't
		// before held consensus for this length of time, reward by the size of the deme.
		if(st->second.current > st->second.max) {
			st->second.max = st->second.current;
			return deme.GetSize();
		}
		
		return 0.0;
	}
	
	//! Called to reset state after demes compete.
	virtual void Clear() {
		_state.clear();
	}
	
protected:
	
	//! This method handles the clean-up of a deme once consensus has been reached.
	void ConsensusReached(cDeme& deme, std::pair<unsigned int, cOrganism::Opinion>& support, cAvidaContext& ctx) { 
		// Record that consensus occurred:
		m_world->GetStats().ConsensusReached(deme, support.second, cAssignRandomCellData::CellIDFromData(support.second, deme));
		
		// Exit early if we're not supposed to touch the cell data.
		if(_dont_replace) {
			return;
		}
		
		// Now, change the cell data for the value that was agreed upon:
		int min_data = 0;
		int max_data = INT_MAX;
		if(_restrict_range) {
			min_data = *cAssignRandomCellData::GetDataInDeme(deme).begin() + 1;
			max_data = *cAssignRandomCellData::GetDataInDeme(deme).rbegin() - 1;
		}
		cAssignRandomCellData::CellIDList cell_ids = 
		cAssignRandomCellData::ReplaceCellData(support.second, ctx.GetRandom().GetInt(min_data, max_data), deme);
		
		// Now reset the others:
		for(int i=1; i<_replace; ++i) {
			int cell_id = ctx.GetRandom().GetInt(deme.GetSize());
			int cell_data = deme.GetCell(cell_id).GetCellData();
			cAssignRandomCellData::CellIDList extra_cell_ids = 
			cAssignRandomCellData::ReplaceCellData(cell_data, ctx.GetRandom().GetInt(min_data, max_data), deme);
			cell_ids.insert(cell_ids.end(), extra_cell_ids.begin(), extra_cell_ids.end());
		}
    
		// Ok, if we're going to kill the organisms, do so:
		if(_kill) {
			// This is probably only compatible with the "old-style" germline
			for(cAssignRandomCellData::CellIDList::iterator i=cell_ids.begin(); i!=cell_ids.end(); ++i) {
				cPopulationCell& cell = deme.GetCell(*i);				
				if(cell.IsOccupied()) {
					if(m_world->GetConfig().DEMES_USE_GERMLINE.Get()) {
						m_world->GetPopulation().KillOrganism(cell, ctx); 
						m_world->GetPopulation().InjectGenome(*i, Systematics::Source(Systematics::DUPLICATION, "germline", true), deme.GetGermline().GetLatest(), ctx); 
					} else {
						Genome genome(cell.GetOrganism()->GetGenome());
						m_world->GetPopulation().KillOrganism(cell, ctx); 
						m_world->GetPopulation().InjectGenome(*i, Systematics::Source(Systematics::DUPLICATION, "random", true), genome, ctx); 
					}
					
					m_world->GetPopulation().DemePostInjection(deme, cell);
				}
			}
		}
	}
	
private:
	int _replace; //!< Number of cell datas that will be replaced on successful consensus.
	int _kill; //!< Whether organisms are killed (and then reinjected) upon reset.
	int _hold; //!< The number of updates that a deme must hold consensus for it to be counted.	
	int _restrict_range; //!< Whether or not new cell data are restricted to be within the range of current cell data.
	int _dont_replace; //!< For testing - if true, don't reset any cell data.
	DemeState _state; //!< Map of deme id -> (Opinion, held time) to support stability of consensus.
};


/*! An event to randomly kill an organism within each deme, replacing it with a new organism from the germline.
 */
class cActionReplaceFromGermline : public cAction {
public:
  cActionReplaceFromGermline(cWorld* world, const cString& args, Feedback&) : cAction(world, args), _p_kill(0.0), _update_cell_data(0) {
    cString largs(args);
    if(largs.GetSize()) {
      _p_kill = largs.PopWord().AsDouble();
      if(largs.GetSize()) {
        _update_cell_data = largs.PopWord().AsInt();
      }
    }
  }
  
  static const cString GetDescription() { return "Arguments: [double p(kill)=0.0 [int update_cell_data=0]]"; }
  
  //! Process this event, looping through each deme, randomly killing organisms, and replacing them from the germline.
  void Process(cAvidaContext& ctx) {
    if(_p_kill > 0.0) {
      for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
        if(ctx.GetRandom().P(_p_kill)) {
          cDeme& deme = m_world->GetPopulation().GetDeme(i);
          
          // Pick a cell at random, kill the occupant (if any), reset the cell data, and re-seed from the germline.
          cPopulationCell& cell = deme.GetCell(ctx.GetRandom().GetInt(deme.GetSize()));
          
          if(_update_cell_data) {
            // There are no restrictions on the ID for this cell:
            cAssignRandomCellData::ReplaceCellData(cell, ctx.GetRandom().GetInt(INT_MAX), deme);
          }
          
          // Kill any organism in that cell, re-seed the from the germline, and do the post-injection magic:
          assert(deme.GetGermline().Size()>0);
          m_world->GetPopulation().KillOrganism(cell, ctx); 
					m_world->GetPopulation().InjectGenome(cell.GetID(), Systematics::Source(Systematics::DUPLICATION, "germline", true), deme.GetGermline().GetLatest(), ctx); 
          m_world->GetPopulation().DemePostInjection(deme, cell);
        }
      }
    }
  }
  
private:
  double _p_kill; //!< probability that a single individual in each deme will be replaced from the germline.
  int _update_cell_data; //!< whether to update the cell data of killed individuals.	
};



/*! This class rewards a deme based on the number of opinions that have been set
 to a given value.
 
 There are three parameters that control this event:
 desired: the opinion we're counting.
 multiplicity: the number of times we want this opinion to be in the deme
 side: the "sidedness" of the fitness function, that is, "to which side of mult is
 there a fitness cliff?"  If side=-1, the cliff is to the left.  If side=1, the cliff
 is to the right.  If side=0, there is no cliff.
 */
class cActionCountOpinions : public cAbstractCompeteDemes, ConsensusSupport {
public:
	
	//! Constructor.
	cActionCountOpinions(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback), _desired(0), _mult(1), _side(0)
  {
		if(args.GetSize()) {
			cString largs(args);
			_desired = largs.PopWord().AsInt();
			if(largs.GetSize()) {
				_mult = largs.PopWord().AsInt();
				if(largs.GetSize()) {
					_side = largs.PopWord().AsInt();
				}
			}
		}		
	}
	
	//! Destructor.
	virtual ~cActionCountOpinions() { }
	
	static const cString GetDescription() { return "Arguments: [int desired_opinion=0 [int multiplicity=1 [int side=0]]]"; }
	
  /*! Fitness function.
	 
	 The idea here is that we want to reward each deme based on the number of constituents
	 that have set their opinion to the desired value.  We're going to set fitness == number
	 of organisms that have an opinion to get things jumpstarted.
   */
  virtual double Fitness(cDeme& deme, cAvidaContext&) { 
		// the number of individuals that have set their opinion to _desired:
		// s.first == support, s.second == have_opinion
		std::pair<int, int> s = support(deme, _desired);
		
		// encourage opinion setting:
		if(s.second < deme.GetSize()) {
			return s.second + 1; // small reward to help bootstrap.
		}
		
		// it's possible that we'll need to scale these values such that rewards to the
		// 'left' or 'right' of _mult are proportional to each other.  if so, this might help:
		//		double leftside_scale = 0.5 * deme.GetSize() / _mult;
		//		double rightside_scale = 0.5 * deme.GetSize() / (deme.GetSize() - _mult);
		//		
		//		double scaled = 0.0;
		//		if(_mult > s.first) {
		//			// leftside scaling
		//			scaled = std::abs(static_cast<double>(_mult - s.first)) * leftside_scale;
		//		} else if(_mult < s.first) {
		//			// rightside scaling
		//			scaled = std::abs(static_cast<double>(_mult - s.first)) * rightside_scale;
		//		}
		//		return pow(deme.GetSize() - scaled + 1, 2);
		
		switch(_side) {
			case -1: {
				// fitness cliff to the left of _mult.
				if(s.first >= _mult) {
					return pow((double)deme.GetSize() - (s.first - _mult) + 1, 2);					
				}
				break;
			}
			case 0: {
				// no fitness cliff.
				return pow(deme.GetSize() - std::abs(static_cast<double>(_mult - s.first)) + 1, 2);
			}
			case 1: {
				// fitness cliff to the right of _mult.
				if(s.first <= _mult) {
					return pow((double)deme.GetSize() - (_mult - s.first) + 1, 2);
				}
				break;
			}
			default: {
				assert(false);
			}
		}
		
		// we only get down here if we fell off the fitness cliff to either side of _mult.
		// we reward by the size of the deme 'cause all organisms did set their opinion.
		return deme.GetSize();
	}
	
private:
	cOrganism::Opinion _desired; //!< Opinion that will be rewarded.
	int _mult; //!< Desired multiplicity of the opinion that will be rewarded.
	int _side; //!< "sidedness" of the fitness function (two-sided==0, one-sided left==-1, one-sided right==1).
};


/*! This class rewards a deme based on the number of opinions the deme has. 
 Specifically, the user defines how many opinions it wants the deme to have (e.g., 5)
 and the deme then receives rewards for having at least one of those opinions (e.g., 1-5).
 
 There is one parameter that controls this event:
 opinion_count: number of opinions we want the deme to have. We assume the opinions 
 start at 1 and continue until the desired count is reached.
 */
class cActionCountMultipleOpinions : public cAbstractCompeteDemes, ConsensusSupport
{
public:
	
	typedef std::set<cOrganism::Opinion> opinion_set;
	
	//! Constructor.
	cActionCountMultipleOpinions(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback)
  {
		if(args.GetSize()) {
			cString largs(args);
			// Build the set of opinions that we will reward... does not include 0.
			for(int i=largs.PopWord().AsInt(); i>0; --i) {
				_rewarded.insert(i);
			}
		}		
	}
	
	//! Destructor.
	virtual ~cActionCountMultipleOpinions() { }
	
	static const cString GetDescription() { return "Arguments: [int opinion_count=0]"; }
	
  /*! Fitness function.
	 
	 The idea here is that we want to reward each deme based the number of opinions
	 expressed out of the desired set (e.g., for a count of 5 opinions, we want
	 at least one of 1, 2, 3, 4, 5 -- the others don't count.  
	 
	 We're going to set fitness == number of organisms that have an opinion to get 
	 things jumpstarted.
   */
  virtual double Fitness(cDeme& deme, cAvidaContext&) { 
		
		// How many opinions are set?
		int ocount = count_opinions(deme);
		
		// If not all opinions have been set, just return the number that have.
		// We want to encourage opinion-setting.
		if (ocount < deme.GetSize()) {
			return ocount + 1;
		}
		
		// Ok, great.  Everyone has set an opinion.  Now, reward based on
		// the size of the intersection between the rewarded opinions and the
		// unique opinions expressed in the deme.
		opinion_set intersection;
		opinion_set unique = unique_opinions(deme);
		std::set_intersection(unique.begin(), unique.end(), _rewarded.begin(), _rewarded.end(), 
													std::insert_iterator<opinion_set>(intersection, intersection.begin()));
		
		return intersection.size() + ocount + 1;
	}
	
private:
	opinion_set _rewarded; //!< Set of opinions that are rewarded.
};



/*! Compete demes event that evaluates based on 2 tasks (t1 & t2) as follows. 
 If min <= t1 <= max, then f = 1 + 1 + t2
 Else, f = 1
 */
class cActionDemeBalanceTwoTasks : public cAbstractCompeteDemes {
public:
	
	//! Constructor.
	cActionDemeBalanceTwoTasks(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback), _min(0), _max(0)
  {
		if(args.GetSize()) {
			cString largs(args);
			_min = largs.PopWord().AsInt();
			_max = largs.PopWord().AsInt();
		}		
	}
	
	//! Destructor.
	virtual ~cActionDemeBalanceTwoTasks() { }
	
	static const cString GetDescription() { return "Two arguments: min and max for not"; }
	
  /*! Fitness function.
	 Functions as described in the comments above.
	 */
  virtual double Fitness(cDeme& deme, cAvidaContext&) { 
		
		float val = 0.0;
		double performed_t1=0.0;
		double performed_t2=0.0;
		for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
			if(org != 0) {
				Apto::Array<int> reactions = org->GetPhenotype().GetCurReactionCount();
				assert(reactions.GetSize() > 1);
				
				if (reactions[0]) performed_t1++;
				if (reactions[1]) performed_t2++;
				
			}
		}
		
		// return something
		if ((_min <= performed_t1) && (performed_t1 <= _max)) {
			val = 2 + performed_t2;
		} else {
			val = 1;
		}
		return val;
	}
	
protected:
	int _min; //!< min for task 1 (not)
	int _max; //!< max for task 1 (not)
};

/*! Compete demes event that rewards for diversity of reactions that have been
 performed by the currently living individuals in each deme.
 */
class cActionDemeReactionDiversity : public cAbstractCompeteDemes {
public:
	
	//! Constructor.
	cActionDemeReactionDiversity(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback), _uniq_only(false)
  {
		if(args.GetSize()) {
			cString largs(args);
			_uniq_only = largs.PopWord().AsInt();
		}		
	}
	
	//! Destructor.
	virtual ~cActionDemeReactionDiversity() { }
	
	static const cString GetDescription() { return "No arguments."; }
	
  /*! Fitness function.
	 Here we reward the deme based on the number of unique reactions that have been
	 performed by the current individuals.
	 */
  virtual double Fitness(cDeme& deme, cAvidaContext&) { 
		std::set<int> uniq_reactions;
		int performed=0;
		for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
			if(org != 0) {
				bool performed_rx=false;
				//				Apto::Array<int> reactions = org->GetPhenotype().GetLastReactionCount();
				Apto::Array<int> reactions = org->GetPhenotype().GetCurReactionCount();
				for(int j=0; j<reactions.GetSize(); ++j) {
					if(reactions[j] > 0) {
						uniq_reactions.insert(j);
						performed_rx = true;
					}
				}
				if(performed_rx) {
					++performed;
				}
			}
		}
		
		if(_uniq_only) {
			return uniq_reactions.size() + 1.0;
		} else {
			if(performed < deme.GetSize()) {
				return performed + 1.0;
			} else {
				return uniq_reactions.size() + deme.GetSize() + 1.0;
			}
		} 
	}
	
protected:
	bool _uniq_only; //!< Whether to reward for uniqueness of reaction only.
};


/*! 
 */
class cActionPhenotypeMatch : public cAbstractCompeteDemes
{
public:
	
	
	//! Constructor.
	cActionPhenotypeMatch(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback)
  {
		if(args.GetSize()) {
			cString largs(args);
      std::ifstream ifs(largs.PopWord());
      
      string p;
      while (!ifs.eof()){
        ifs >> p;
        desired_phenotypes.push_back(p);
      }
      ifs.close();
		}	
	}
	
	//! Destructor.
	virtual ~cActionPhenotypeMatch() { }
	
	static const cString GetDescription() { return "Arguments: string file-name"; }
	
  /*! Fitness function.
	 For each organism, compare the actual phenotype to the desired
   phenotype by computing the euclidean distance between the vectors. Fitness is 1/(sum of Euclidean distances)
   
   */
  virtual double Fitness(cDeme& deme, cAvidaContext&) { 
    double fit = 1.0;
    
    // Fail if we don't have the right number of phenotypes.
    if (desired_phenotypes.size() != (unsigned long) deme.GetSize()) 
      return fit;
    
    // Put in check that the phenotype is the right length...?
    
    // Cycle through the organisms... 
    double total_dist = 0.0;
    for(int i=0; i<deme.GetSize(); ++i) {
			cOrganism* org = deme.GetOrganism(i);
      string p = desired_phenotypes[i];
			if(org != 0) {
        Apto::Array<int> reactions = org->GetPhenotype().GetCurReactionCount();
				for(int j=0; j<reactions.GetSize(); ++j) {
          char curp= p[j];
          int des =  atoi(&curp);
          int react = (int) reactions[j];
          total_dist += abs(des - react);
          if (des > react) total_dist += 1;
        }
      }
      else {
        for(unsigned long j=0; j<p.size(); ++j) {
          char curp= p[j];
          int des =  atoi(&curp);
          total_dist += des + 1;
        }
      }
    }
    fit = 1.0 + (1.0/total_dist);
    return fit;
	}
	
private:
  //!< The desired phenotypes of the organisms.
	vector<string> desired_phenotypes; 
  
};




/*! Unit-fitness compete demes method (use for control runs).
 */
class cActionUnitFitness : public cAbstractCompeteDemes {
public:
	
	//! Constructor.
	cActionUnitFitness(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback)
  {
	}
	
	//! Destructor.
	virtual ~cActionUnitFitness() { }
	
	static const cString GetDescription() { return "No arguments."; }
	
	virtual double Fitness(cDeme&, cAvidaContext&) { 
		return 1.0;
	}
};


/*!	This class competes demes based on the total number of times that a
 *	given task has been completed by an organism in the deme since the
 *  deme was initialized. This action takes one integer parameter representing
 *	the number of the task that is to be used for competition. If no parameter 
 *	is supplied, the class uses the first task defined in the environment file
 *	to compete the demes.
 */
class cActionCompeteDemesByTaskCount : public cAbstractCompeteDemes {
private:
  int _task_num;	// the task num to use when calculating fitness,
  // defaults to 0 (the first task)
public:
  cActionCompeteDemesByTaskCount(cWorld* world, const cString& args, Feedback& feedback) 
  : cAbstractCompeteDemes(world, args, feedback)
  {
    if (args.GetSize() > 0) {
      cString largs(args);
      _task_num = largs.PopWord().AsInt();
      assert(_task_num >= 0);
      assert(_task_num < m_world->GetEnvironment().GetNumTasks());
    } else {
      _task_num = 0;
    }
  }
  ~cActionCompeteDemesByTaskCount() {}
  
  static const cString GetDescription() { 
    return "Competes demes according to the number of times a given task has been completed within that deme"; 
  }
  
  virtual double Fitness(cDeme& deme, cAvidaContext& ctx) { 
    double fitness = pow(deme.GetCurTaskExeCount()[_task_num], 2.0);///deme.GetInjectedCount());
    if (fitness == 0.0) fitness = 0.1;
    return fitness;
  }
};

// Deme competition based on demes' merits with minor fitness bonuses for successful messaging and deme-IO, implemented for neural networking. @JJB
class cActionCompeteDemesByMerit : public cAbstractCompeteDemes
{
public:
  cActionCompeteDemesByMerit(cWorld* world, const cString& args, Feedback& feedback) : cAbstractCompeteDemes(world, args, feedback)
  {
  }
  
  ~cActionCompeteDemesByMerit() {}

  static const cString GetDescription() { return "Compete demes according to the each deme's current merit"; }

  virtual double Fitness(cDeme& deme, cAvidaContext& ctx)
  {
    deme.UpdateCurMerit();
    double messaging_bonus;
    double input_bonus;
    double output_bonus;

    if (deme.GetMessageSuccessfullySent() > 0) messaging_bonus = 1.5;
    else messaging_bonus = 1.0;

    if (deme.HasDoneInput()) input_bonus = 1.5;
    else input_bonus = 1.0;

    if (deme.HasDoneOutput()) output_bonus = 1.5;
    else output_bonus = 1.0;

    double fitness = deme.GetCurMerit().GetDouble() * messaging_bonus * input_bonus * output_bonus;
    return fitness;
  }
};

class cActionCompeteDemesByTaskCountAndEfficiency : public cAbstractCompeteDemes {
private:
  double _initial_deme_energy;
	int _task_num;	// the task num to use when calculating fitness,
	// defaults to 0 (the first task)
public:
	cActionCompeteDemesByTaskCountAndEfficiency(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback)
  {
  	cString largs(args);
    if (largs.GetSize() == 0) {
      cerr << "CompeteDemesByTaskCountAndEfficiency must be given an initial deme energy amount" << endl;
      exit(1);
    }
		
    _initial_deme_energy = largs.PopWord().AsDouble();
		//    cout << "initial deme energy = " << _initial_deme_energy << endl;
    assert(_initial_deme_energy > 0);
		
		if (largs.GetSize() > 1) {
			_task_num = largs.PopWord().AsInt();
			assert(_task_num >= 0);
			assert(_task_num < m_world->GetEnvironment().GetNumTasks());
		} else {
			_task_num = 0;
		}
	}
	~cActionCompeteDemesByTaskCountAndEfficiency() {}
	
	static const cString GetDescription() { 
		return "Competes demes according to the number of times a given task has been completed within that deme and the efficiency with which it was done"; 
	}
	
	virtual double Fitness(cDeme& deme, cAvidaContext& ctx) { 
    double energy_used = _initial_deme_energy - deme.CalculateTotalEnergy(ctx); 
		double fitness = 
		pow(deme.GetCurTaskExeCount()[_task_num] * (_initial_deme_energy/energy_used),2);
    if (fitness == 0.0) fitness = 0.1;
		//    cout  << "Deme " << deme.GetID() << ": used " << energy_used << " energy" 
		//          << " fitness=" << fitness << endl;
    return fitness;
	}
};


class cActionCompeteDemesByEnergyDistribution : public cAbstractCompeteDemes {
private:
	
public:
	cActionCompeteDemesByEnergyDistribution(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback)
  {
  }
	~cActionCompeteDemesByEnergyDistribution() { ; }
	
	static const cString GetDescription() { 
		return "Competes demes according to the distribution of energy among the organisms"; 
	}
	
	virtual double Fitness(cDeme& deme, cAvidaContext&) { 
		const int numcells = deme.GetSize();
		
		double min_energy = -1;
		double max_energy = -1;
		double current_energy;
		double fitness;
		
		cOrganism *org;
		
		for(int cellid = 0; cellid < numcells; cellid++) {
			org = deme.GetOrganism(cellid);
			fitness = 0.0001;
			
			if (org != 0) {
				current_energy = org->GetPhenotype().GetStoredEnergy();
				
				if((min_energy == -1) || (current_energy < min_energy)) {
					min_energy = current_energy;
				}
				
				if((max_energy == -1) || (current_energy > max_energy)) {
					max_energy = current_energy;
				}
			}
		}
		
		if((min_energy == -1) || (max_energy == -1)) {
			fitness = 0.0001;
		} else if (min_energy == max_energy) {
			fitness = 100 * max_energy;
		} else {
			fitness = 100 * (max_energy/pow((max_energy - min_energy),2));
		}
		
		return fitness;
	}
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
	cActionFlash(cWorld* world, const cString& args, Feedback&): cAction(world, args) { }
	
	//! Destructor.
	virtual ~cActionFlash() { }
	
	static const cString GetDescription() { return "No arguments"; }
	
	//! Process this event, sending a flash to each deme.
	virtual void Process(cAvidaContext&) {
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
  cActionSynchronization(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback) { ; }
  
	//! Destructor.
	virtual ~cActionSynchronization() { }
	
  //! Description of this event.
  static const cString GetDescription() { return "No Arguments"; }
	
  /*! Calculate the fitness based on how well the organisms in this deme have
	 synchronized their flashes.
   */
  virtual double Fitness(cDeme& deme, cAvidaContext&) { 
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



/*! Compete demes based on the ability of their constituent organisms
 to synchronize their flashes to a common period, and yet distribute themselves
 throughout phase-space (phase desynchronization).
 */
class cActionDesynchronization : public cActionSynchronization {
public:
  //! Constructor.
  cActionDesynchronization(cWorld* world, const cString& args, Feedback& feedback)
  : cActionSynchronization(world, args, feedback)
  {
  }
  
	//! Destructor.
	virtual ~cActionDesynchronization() { }
	
  //! Description of this event.
  static const cString GetDescription() { return "No Arguments"; }
  
	//! Calculate fitness based on how well organisms have spread throughout phase-space.
	virtual double Fitness(cDeme& deme, cAvidaContext&) { 
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
	cAbstractCompeteDemes_AttackKillAndEnergyConserve(cWorld* world, const cString& args, Feedback& feedback)
  : cAbstractCompeteDemes(world, args, feedback) { ; }
	
	static const cString GetDescription() { return "No Arguments"; }
  
	double Fitness(cDeme& deme, cAvidaContext& ctx) {  
		double eventsKilled = static_cast<double>(deme.GetEventsKilled());
		double totalEvents  = static_cast<double>(deme.GetEventsTotal());
		double energyRemaining = deme.CalculateTotalEnergy(ctx); 
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
 'sat-deme-predicate'...demes whose predicate has been satisfied; does not include movement or message predicates as those are organisms-level
 'perf-reactions' ...demes that have performed X number of each task are replicated
 'consume-res' ...demes that have consumed a sufficienct amount of resources
 
 */

class cActionReplicateDemes : public cAction
{
private:
  int m_rep_trigger;
public:
  cActionReplicateDemes(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args), m_rep_trigger(-1)
  {
    cString largs(args);
    cString in_trigger("full_deme");
    if (largs.GetSize()) in_trigger = largs.PopWord();
    
    if (in_trigger == "all") m_rep_trigger = DEME_TRIGGER_ALL;
    else if (in_trigger == "full_deme") m_rep_trigger = DEME_TRIGGER_FULL;
    else if (in_trigger == "corners") m_rep_trigger = DEME_TRIGGER_CORNERS;
    else if (in_trigger == "deme-age") m_rep_trigger = DEME_TRIGGER_AGE;
    else if (in_trigger == "birth-count") m_rep_trigger = DEME_TRIGGER_BIRTHS;
    else if (in_trigger == "sat-mov-pred") m_rep_trigger = DEME_TRIGGER_MOVE_PREDATORS;
    else if (in_trigger == "events-killed") m_rep_trigger = DEME_TRIGGER_GROUP_KILL;
    else if (in_trigger == "sat-msg-pred") m_rep_trigger = DEME_TRIGGER_MESSAGE_PREDATORS;
    else if (in_trigger == "sat-deme-predicate") m_rep_trigger = DEME_TRIGGER_PREDICATE;
    else if (in_trigger == "perf-reactions") m_rep_trigger = DEME_TRIGGER_PERFECT_REACTIONS;
    else if (in_trigger == "consume-res") m_rep_trigger = DEME_TRIGGER_CONSUME_RESOURCES;
    else {
      cString err("Unknown replication trigger '");
      err += in_trigger;
      err += "' in ReplicatDemes action.";
      feedback.Error(err);
      m_rep_trigger = DEME_TRIGGER_UNKNOWN;
      return;
    }
  }
  
  static const cString GetDescription() { return "Arguments: [string trigger=full_deme]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().ReplicateDemes(m_rep_trigger, ctx); 
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
 
 FYI: This model is appears to be similar to Traulsen's model of group selection:
 
 */

class cActionDivideDemes : public cAction
{
private:
public:
  cActionDivideDemes(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    // Nothing to do here yet....
  }
  
  static const cString GetDescription() { return "No arguments (yet!)"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().DivideDemes(ctx); 
  }
};


/*! Mix all organisms in the population.
 
 This event, in combination with a method for deme competition, can be used to model
 the different biologically-inspired approaches to group selection, specifically
 Wilson's model.
 */
class cActionMixPopulation : public cAction {
public:
	cActionMixPopulation(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
	}
	
	static const cString GetDescription() { return "No arguments."; }
	
	void Process(cAvidaContext& ctx) {
		m_world->GetPopulation().MixPopulation(ctx);
	}
};


/*
 Designed to serve as a control for the compete_demes. Each deme is 
 copied into itself and the parameters reset. 
 */
class cActionResetDemes : public cAction
{
public:
  cActionResetDemes(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext&)
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
  cActionCopyDeme(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_id1(0), m_id2(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_id1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_id2 = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int src_id> <int dest_id>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().CopyDeme(m_id1, m_id2, ctx); 
  }
};


class cActionNewTrial : public cAction
{
private:
public:
  cActionNewTrial(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
  }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().NewTrial(ctx);
  }
};


/* This action decays the number of points a deme has accumulated by 
 a percentage that is set in the configuration file. (hjg)*/
class cActionDecayPoints : public cAction
{
private:
  int m_decay_percent;
  
public:
  cActionDecayPoints(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_decay_percent(0)
  {
    cString largs(args);
    m_decay_percent = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "No Arguments"; }
  
  void Process(cAvidaContext&)
  {
    double decay_percent = (double) m_decay_percent / 100;
    double cur_points = 0;
    int sub_points = 0;
    
    // For each deme, subtract decay_percent of its points.
    for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
      cur_points = m_world->GetPopulation().GetDeme(i).GetNumberOfPoints(); 
      sub_points = (int) (cur_points * decay_percent); 
      m_world->GetPopulation().GetDeme(i).SubtractNumberOfPoints(sub_points); 
    }		
    
  }
};

class cActionCompeteOrganisms : public cAction
{
private:
  int m_type;
  int m_parents_survive;
public:
  cActionCompeteOrganisms(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_type(0), m_parents_survive(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_type = largs.PopWord().AsInt();
    if (largs.GetSize()) m_parents_survive = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int type=0] [int parents_survive=0] [double scaled_time=1.0] [int dynamic_scaling=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().CompeteOrganisms(ctx, m_type, m_parents_survive);
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
  cActionSeverGridCol(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
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
      ctx.Driver().Feedback().Warning(err);
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
 min_col: First column to start cutting from
 default = 0
 max_col: Last column to cut to
 default (or -1) = last column in population.
 */
class cActionSeverGridRow : public cAction
{
private:
  int m_id;
  int m_min;
  int m_max;
public:
  cActionSeverGridRow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
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
      ctx.Driver().Feedback().Warning(err);
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
  cActionJoinGridCol(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
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
      ctx.Driver().Feedback().Warning(err);
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
 Join the connections between cells along a row in an avida grid.
 
 Arguments:
 row_id:  indicates the number of rows above the join.
 default (or -1) = cut population in half
 min_col: First column to start cutting from
 default = 0
 max_col: Last row to cut to
 default (or -1) = last column in population.
 */
class cActionJoinGridRow : public cAction
{
private:
  int m_id;
  int m_min;
  int m_max;
public:
  cActionJoinGridRow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_id(-1), m_min(0), m_max(-1)
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
      ctx.Driver().Feedback().Warning(err);
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
  cActionConnectCells(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_a_x(-1), m_a_y(-1), m_b_x(-1), m_b_y(-1)
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
      ctx.Driver().Feedback().Warning("ConnectCells cell out of range");
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
  cActionDisconnectCells(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_a_x(-1), m_a_y(-1), m_b_x(-1), m_b_y(-1)
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
      ctx.Driver().Feedback().Warning("DisconnectCells cell out of range");
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

class cActionSwapRandomCells : public cAction
{
private:
  int num_swaps;
  
public:
  cActionSwapRandomCells(cWorld* world, const cString& args, Feedback&) : cAction(world, args), num_swaps(-1)
  {
    cString largs(args);
    if (largs.GetSize()) num_swaps = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int number swaps>"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int num_cells = m_world->GetPopulation().GetSize();
    
    for (int i=0; i<num_swaps;i++){
      int id1 = ctx.GetRandom().GetInt(0, num_cells);
      int id2 = ctx.GetRandom().GetInt(0, num_cells);
      //if the id's happen to be the same, it just won't swap
      if (!(id1 == id2)) {
        m_world->GetPopulation().SwapCells(id1, id2, ctx);
      }
    }
  }
};


class cActionPred_DemeResourceThresholdPredicate : public cAction {
private:
  cString resourceName;
	cString comparisonOperator;
	double threasholdValue;
	
public:
  cActionPred_DemeResourceThresholdPredicate(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
    cString largs(args);
		assert(largs.CountNumWords() == 3);
    if (largs.GetSize()) resourceName = largs.PopWord();
		if (largs.GetSize()) comparisonOperator = largs.PopWord();
		if (largs.GetSize()) threasholdValue = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: cString resourceName, cString comparisonOperator, double threasholdValue"; }
  
  void Process(cAvidaContext&)
  {
    cPopulation& pop = m_world->GetPopulation();
		const int numDemes = pop.GetNumDemes();
    for (int deme_id = 0; deme_id < numDemes; deme_id++) {
			pop.GetDeme(deme_id).AddDemeResourceThresholdPredicate(resourceName, comparisonOperator, threasholdValue);
    }
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
  cActionPred_DemeEventMoveCenter(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_times(1) {
    cString largs(args);
    if (largs.GetSize()) m_times = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int times=1]"; }
  
  void Process(cAvidaContext&)
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
  cActionPred_DemeEventMoveBetweenTargets(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_times(1) {
    cString largs(args);
    if (largs.GetSize()) m_times = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int times=1]"; }
  
  void Process(cAvidaContext&)
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
  cActionPred_DemeEventEventNUniqueIndividualsMovedIntoTarget(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_numorgs(1) {
    cString largs(args);
    if (largs.GetSize()) m_numorgs = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: [int numorgs=1]"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().AddDemePred("EventNUniqueIndividualsMovedIntoTarget", m_numorgs);
  }
};


/*
 Kill organisms in N randomly-chosen cells if the level of the given resource
 in the chosen cell is below the configured threshold
 
 Parameters:
 - The number of cells to kill (default: 0)
 - The name of the resource
 - The amount of resource below which to execute the kill (default: 0)
 */

class cActionKillNBelowResourceThreshold : public cAction
{
private:
  cString m_resname;
  int m_numkills;
  double m_threshold;
public:
  cActionKillNBelowResourceThreshold(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_numkills(0), m_threshold(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_numkills = largs.PopWord().AsInt();
    if (largs.GetSize()) m_resname = largs.PopWord();
    if (largs.GetSize()) m_threshold = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: [int numkills=0, string resource name, double threshold=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    double level;
    int target_cell;
    cPopulation& pop = m_world->GetPopulation();
    int res_id = m_world->GetPopulation().GetResourceCount().GetResourceCountID(m_resname);
    
    assert(res_id != -1);
    
    long cells_scanned = 0;
    long orgs_killed = 0;
    long cells_empty = 0;
    
    for(int i=0; i < m_numkills; i++) {
      target_cell = ctx.GetRandom().GetInt(0, m_world->GetPopulation().GetSize()-1);
      level = m_world->GetPopulation().GetResourceCount().GetSpatialResource(res_id).GetAmount(target_cell); 
      cells_scanned++;
      
      if(level < m_threshold) {
        cPopulationCell& cell = pop.GetCell(target_cell);
        if (cell.IsOccupied()) {
          pop.KillOrganism(cell, ctx); 
          orgs_killed++;
        } else {
          cells_empty++;
        }
      }
    }
    
    m_world->GetStats().AddNumCellsScannedAtKill(cells_scanned);
    m_world->GetStats().AddNumOrgsKilled(orgs_killed);
    m_world->GetStats().AddNumUnoccupiedCellAttemptedToKill(cells_empty);
    
  } //End Process()
};



/*
 Kill all organisms within a given radius of a randomly-chosen cell if the level
 of the given resource in the chosen cell is below the given threshold.  Currently
 only works for toruses and bounded grids.
 
 Parameters:
 - The number of kill radii to use (default: 0)
 - The radius of the kill zone (default: 0)
 - The name of the resource
 - The amount of resource below which to execute the kill (default: 0)
 - The fraction of orgs in the region to kill (1=all, 0.5 leave half) (default: 1) -- useful for controlling density with biofilms
 */

class cActionKillWithinRadiusBelowResourceThreshold : public cAction
{
private:
  int m_numradii;
  int m_radius;
  cString m_resname;
  double m_threshold;
  double m_kill_density;
public:
  cActionKillWithinRadiusBelowResourceThreshold(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_numradii(0), m_radius(0), m_threshold(0.0), m_kill_density(1.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_numradii = largs.PopWord().AsInt();
    if (largs.GetSize()) m_radius = largs.PopWord().AsInt();
    if (largs.GetSize()) m_resname = largs.PopWord();
    if (largs.GetSize()) m_threshold = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_kill_density = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: [int numradii=0, int radius=0, string resource name, double threshold=0, double killdensity=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
    
    assert(m_numradii >= 0);
    assert(m_radius >= 0);
    assert(m_radius <= world_x);
    assert(m_radius <= world_y);
    assert(m_threshold >= 0.0);
    assert(m_kill_density >= 0.0);
    assert(m_kill_density <= 1.0);
    assert(geometry == nGeometry::GRID || geometry == nGeometry::TORUS);
    
    cPopulation& pop = m_world->GetPopulation();
    int res_id = m_world->GetPopulation().GetResourceCount().GetResourceCountID(m_resname);
    
    assert(res_id != -1);
    
    long cells_scanned = 0;
    long orgs_killed = 0;
    long cells_empty = 0;
    
    for (int i = 0; i < m_numradii; i++) {
      
      int target_cell = ctx.GetRandom().GetInt(0, pop.GetSize()-1);
      double level = pop.GetResourceCount().GetSpatialResource(res_id).GetAmount(target_cell); 
      
      if(level < m_threshold) {
        const int current_row = target_cell / world_x;
        const int current_col = target_cell % world_x;
        
        for(int row = current_row - m_radius; row <= current_row + m_radius; row++) {
          
          if( ((row < 0) || (row >= world_y)) && (geometry == nGeometry::GRID) ) continue;
          
          for(int col = current_col - m_radius; col <= current_col + m_radius; col++) {
            if( ((col < 0) || (col >= world_x)) && (geometry == nGeometry::GRID) ) continue;
            
            cells_scanned++;
            int row_adj = 0;
            int col_adj = 0;
            
            if(geometry == nGeometry::TORUS) {
              row_adj = (row + world_y) % world_y;
              col_adj = (col + world_x) % world_x;
            } else if(geometry == nGeometry::GRID) {
              row_adj = row;
              col_adj = col;
            }
            
            int current_cell = (world_x * row_adj) + col_adj;
            cPopulationCell& cell = pop.GetCell(current_cell);
            
            if( (cell.IsOccupied()) && (ctx.GetRandom().P(m_kill_density)) ) {
              pop.KillOrganism(cell, ctx); 
              orgs_killed++;
            } else {
              cells_empty++;
            }
            
          }
        }
        
      }  // End if level at cell is below threshold
      
    } //End iterating through kill zones
    
    m_world->GetStats().AddNumCellsScannedAtKill(cells_scanned);
    m_world->GetStats().AddNumOrgsKilled(orgs_killed);
    m_world->GetStats().AddNumUnoccupiedCellAttemptedToKill(cells_empty);
    
  } //End Process()
};


/*
 Kill all organisms within a given radius of a randomly-chosen cell if the MEAN level
 of the given resource selected neighborhood is below the given threshold.  Currently
 only works for toruses and bounded grids.
 
 Parameters:
 - The number of kill radii to use (default: 0)
 - The radius of the kill zone (default: 0)
 - The name of the resource
 - The amount of resource below which to execute the kill (default: 0)
 - The fraction of orgs in the region to kill (1=all, 0.5 leave half) (default: 1) -- useful for controlling density with biofilms
 */

class cActionKillWithinRadiusMeanBelowResourceThreshold : public cAction
{
private:
  int m_numradii;
  int m_radius;
  cString m_resname;
  double m_threshold;
  double m_kill_density;
public:
  cActionKillWithinRadiusMeanBelowResourceThreshold(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_numradii(0), m_radius(0), m_threshold(0.0), m_kill_density(1.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_numradii = largs.PopWord().AsInt();
    if (largs.GetSize()) m_radius = largs.PopWord().AsInt();
    if (largs.GetSize()) m_resname = largs.PopWord();
    if (largs.GetSize()) m_threshold = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_kill_density = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: [int numradii=0, int radius=0, string resource name, double threshold=0, double killdensity=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
    
    assert(m_numradii >= 0);
    assert(m_radius >= 0);
    assert(m_radius <= world_x);
    assert(m_radius <= world_y);
    assert(m_threshold >= 0.0);
    assert(m_kill_density >= 0.0);
    assert(m_kill_density <= 1.0);
    assert(geometry == nGeometry::GRID || geometry == nGeometry::TORUS);
    
    cDoubleSum resourcesum;
    
    cPopulation& pop = m_world->GetPopulation();
    int res_id = m_world->GetPopulation().GetResourceCount().GetResourceCountID(m_resname);
    
    assert(res_id != -1);
    
    long cells_scanned = 0;
    long orgs_killed = 0;
    long cells_empty = 0;
    
    for (int i = 0; i < m_numradii; i++) {
      
      int target_cell = ctx.GetRandom().GetInt(0, pop.GetSize()-1);
      resourcesum.Clear();
      
      const int current_row = target_cell / world_x;
      const int current_col = target_cell % world_x;
      
      for(int row = current_row - m_radius; row <= current_row + m_radius; row++) {
        
        if( ((row < 0) || (row >= world_y)) && (geometry == nGeometry::GRID) ) continue;
        
        for(int col = current_col - m_radius; col <= current_col + m_radius; col++) {
          if( ((col < 0) || (col >= world_x)) && (geometry == nGeometry::GRID) ) continue;
          
          int row_adj = 0;
          int col_adj = 0;
          
          if(geometry == nGeometry::TORUS) {
            row_adj = (row + world_y) % world_y;
            col_adj = (col + world_x) % world_x;
          } else if(geometry == nGeometry::GRID) {
            row_adj = row;
            col_adj = col;
          }
          
          int current_cell = (world_x * row_adj) + col_adj;
          resourcesum.Add(pop.GetResourceCount().GetSpatialResource(res_id).GetAmount(current_cell));           
          cells_scanned++;
        }
      }
      
      if(resourcesum.Average() < m_threshold) {
        for(int row = current_row - m_radius; row <= current_row + m_radius; row++) {
          
          if( ((row < 0) || (row >= world_y)) && (geometry == nGeometry::GRID) ) continue;
          
          for(int col = current_col - m_radius; col <= current_col + m_radius; col++) {
            if( ((col < 0) || (col >= world_x)) && (geometry == nGeometry::GRID) ) continue;
            
            int row_adj = 0;
            int col_adj = 0;
            
            if(geometry == nGeometry::TORUS) {
              row_adj = (row + world_y) % world_y;
              col_adj = (col + world_x) % world_x;
            } else if(geometry == nGeometry::GRID) {
              row_adj = row;
              col_adj = col;
            }
            
            int current_cell = (world_x * row_adj) + col_adj;
            cPopulationCell& cell = pop.GetCell(current_cell);
            
            if( (cell.IsOccupied())  && (ctx.GetRandom().P(m_kill_density)) ) {
              pop.KillOrganism(cell, ctx); 
              orgs_killed++;
            } else {
              cells_empty++;
            }
            
          }
        }
        
      }
      
    } //End iterating through kill zones
    
    m_world->GetStats().AddNumCellsScannedAtKill(cells_scanned);
    m_world->GetStats().AddNumOrgsKilled(orgs_killed);
    m_world->GetStats().AddNumUnoccupiedCellAttemptedToKill(cells_empty);
    
  } //End Process()
};


/*
 Kill all organisms within a given radius of a randomly-chosen cell if the MEAN
 level of the given resource selected neighborhood is below the given
 threshold. The expected fraction of organisms killed within that neighborhood
 is determined by the level of a configured resource (range is [0,1]) at the
 chosen focal cell.  This allows levels of adversity to be "painted" in the
 environment or controlled by the production of this resource.  This action
 Currently only works for toruses and bounded grids.
 
 Parameters:
 - The number of kill radii to use (default: 0)
 - The radius of the kill zone (default: 0)
 - The name of the protective resource
 - The amount of resource below which to execute the kill (default: 0)
 - The name of the resource from which to determine the level of adversity
 
 BDC - 2011-02-17
 */

class cActionKillMeanBelowThresholdPaintable : public cAction
{
private:
  int m_numradii;
  int m_radius;
  cString m_resname;
  double m_threshold;
  cString m_adv_resname;
public:
  cActionKillMeanBelowThresholdPaintable(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_numradii(0), m_radius(0), m_threshold(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_numradii = largs.PopWord().AsInt();
    if (largs.GetSize()) m_radius = largs.PopWord().AsInt();
    if (largs.GetSize()) m_resname = largs.PopWord();
    if (largs.GetSize()) m_threshold = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_adv_resname = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [int numradii=0, int radius=0, string resource name, double threshold=0, string adversity resource name]"; }
  
  void Process(cAvidaContext& ctx)
  {
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
    
    assert(m_numradii >= 0);
    assert(m_radius >= 0);
    assert(m_radius <= world_x);
    assert(m_radius <= world_y);
    assert(m_threshold >= 0.0);
    assert(geometry == nGeometry::GRID || geometry == nGeometry::TORUS);
    
    cDoubleSum resourcesum;
    
    cPopulation& pop = m_world->GetPopulation();
    
    // Get the kill resource
    int res_id = m_world->GetPopulation().GetResourceCount().GetResourceCountID(m_resname);
    assert(res_id != -1);
    
    // Get the adversity resource
    int adv_res_id = m_world->GetPopulation().GetResourceCount().GetResourceCountID(m_adv_resname);
    assert(adv_res_id != -1);
    
    long cells_scanned = 0;
    long orgs_killed = 0;
    long cells_empty = 0;
    
    for (int i = 0; i < m_numradii; i++) {
      
      int target_cell = ctx.GetRandom().GetInt(0, pop.GetSize()-1);
      resourcesum.Clear();
      
      // Get the level of adversity.  Bounded by [0,1]
      double kill_prob = pop.GetResourceCount().GetSpatialResource(adv_res_id).GetAmount(target_cell);
      kill_prob = min(1.0, max(0.0, kill_prob));
      
      const int current_row = target_cell / world_x;
      const int current_col = target_cell % world_x;
      
      for(int row = current_row - m_radius; row <= current_row + m_radius; row++) {
        
        if( ((row < 0) || (row >= world_y)) && (geometry == nGeometry::GRID) ) continue;
        
        for(int col = current_col - m_radius; col <= current_col + m_radius; col++) {
          if( ((col < 0) || (col >= world_x)) && (geometry == nGeometry::GRID) ) continue;
          
          int row_adj = 0;
          int col_adj = 0;
          
          if(geometry == nGeometry::TORUS) {
            row_adj = (row + world_y) % world_y;
            col_adj = (col + world_x) % world_x;
          } else if(geometry == nGeometry::GRID) {
            row_adj = row;
            col_adj = col;
          }
          
          int current_cell = (world_x * row_adj) + col_adj;
          resourcesum.Add(pop.GetResourceCount().GetSpatialResource(res_id).GetAmount(current_cell));            
          cells_scanned++;
        }
      }
      
      if(resourcesum.Average() < m_threshold) {
        for(int row = current_row - m_radius; row <= current_row + m_radius; row++) {
          
          if( ((row < 0) || (row >= world_y)) && (geometry == nGeometry::GRID) ) continue;
          
          for(int col = current_col - m_radius; col <= current_col + m_radius; col++) {
            if( ((col < 0) || (col >= world_x)) && (geometry == nGeometry::GRID) ) continue;
            
            int row_adj = 0;
            int col_adj = 0;
            
            if(geometry == nGeometry::TORUS) {
              row_adj = (row + world_y) % world_y;
              col_adj = (col + world_x) % world_x;
            } else if(geometry == nGeometry::GRID) {
              row_adj = row;
              col_adj = col;
            }
            
            int current_cell = (world_x * row_adj) + col_adj;
            cPopulationCell& cell = pop.GetCell(current_cell);
            
            if( (cell.IsOccupied())  && (ctx.GetRandom().P(kill_prob)) ) {
              pop.KillOrganism(cell, ctx);
              orgs_killed++;
            } else {
              cells_empty++;
            }
            
          }
        }
        
      }
      
    } //End iterating through kill zones
    
    m_world->GetStats().AddNumCellsScannedAtKill(cells_scanned);
    m_world->GetStats().AddNumOrgsKilled(orgs_killed);
    m_world->GetStats().AddNumUnoccupiedCellAttemptedToKill(cells_empty);
    
  } //End Process()
};


/*
 Kill organisms within a given radius of a randomly-chosen cell if the level
 of the given resource in the chosen cells are below the given threshold.  Currently
 only works for toruses and bounded grids.
 
 Parameters:
 - The number of kill radii to use (default: 0)
 - The radius of the kill zone (default: 0)
 - The name of the resource
 - The amount of resource below which to execute the kill (default: 0)
 - The fraction of orgs in the region to kill (1=all, 0.5 leave half) (default: 1) -- useful for controlling density with biofilms
 */

class cActionKillWithinRadiusBelowResourceThresholdTestAll : public cAction {
private:
	int m_numradii;
	int m_radius;
	cString m_resname;
	double m_threshold;
	double m_kill_density;
public:
	cActionKillWithinRadiusBelowResourceThresholdTestAll(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_numradii(0), m_radius(0), m_threshold(0.0), m_kill_density(1.0)
	{
		cString largs(args);
		if (largs.GetSize()) m_numradii = largs.PopWord().AsInt();
		if (largs.GetSize()) m_radius = largs.PopWord().AsInt();
		if (largs.GetSize()) m_resname = largs.PopWord();
		if (largs.GetSize()) m_threshold = largs.PopWord().AsDouble();
		if (largs.GetSize()) m_kill_density = largs.PopWord().AsDouble();
	}
	
	static const cString GetDescription() { return "Arguments: [int numradii=0, int radius=0, string resource name, double threshold=0, double killdensity=1]"; }
	
	void Process(cAvidaContext& ctx)
	{
		const int world_x = m_world->GetPopulation().GetWorldX();
		const int world_y = m_world->GetPopulation().GetWorldY();
		const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
		
		assert(m_numradii >= 0);
		assert(m_radius >= 0);
		assert(m_radius <= world_x);
		assert(m_radius <= world_y);
		assert(m_threshold >= 0.0);
		assert(m_kill_density >= 0.0);
		assert(m_kill_density <= 1.0);
		assert(geometry == nGeometry::GRID || geometry == nGeometry::TORUS);
		
		cPopulation& pop = m_world->GetPopulation();
		int res_id = m_world->GetPopulation().GetResourceCount().GetResourceCountID(m_resname);
		
		assert(res_id != -1);
    
    long cells_scanned = 0;
    long orgs_killed = 0;
    long cells_empty = 0;
		
		for (int i = 0; i < m_numradii; i++) {
			
			int target_cell = ctx.GetRandom().GetInt(0, pop.GetSize()-1);
			const int current_row = target_cell / world_x;
			const int current_col = target_cell % world_x;
			
			for(int row = current_row - m_radius; row <= current_row + m_radius; row++) {
				
				if( ((row < 0) || (row >= world_y)) && (geometry == nGeometry::GRID) ) continue;
				
				for(int col = current_col - m_radius; col <= current_col + m_radius; col++) {
					if( ((col < 0) || (col >= world_x)) && (geometry == nGeometry::GRID) ) continue;
					
					int row_adj = 0;
					int col_adj = 0;
					
					if(geometry == nGeometry::TORUS) {
						row_adj = (row + world_y) % world_y;
						col_adj = (col + world_x) % world_x;
					} else if(geometry == nGeometry::GRID) {
						row_adj = row;
						col_adj = col;
					}
					
					int current_cell = (world_x * row_adj) + col_adj;
					cPopulationCell& cell = pop.GetCell(current_cell);
					cells_scanned++;
					
					double level = pop.GetResourceCount().GetSpatialResource(res_id).GetAmount(current_cell); 
					
					if(level < m_threshold) {
						if( (cell.IsOccupied()) && (ctx.GetRandom().P(m_kill_density)) ) {
							pop.KillOrganism(cell, ctx); 
							orgs_killed++;
						} else {
							cells_empty++;
						}
					}
				}
			}
		}
    
    m_world->GetStats().AddNumCellsScannedAtKill(cells_scanned);
    m_world->GetStats().AddNumOrgsKilled(orgs_killed);
    m_world->GetStats().AddNumUnoccupiedCellAttemptedToKill(cells_empty);
    
	}
};

/*
 Kill a percentage of organisms in all demes
 
 Parameters:
 - The percent of living organisms to kill (default: 0)
 */

class cActionKillDemePercent : public cAction
{
private:
  double m_pctkills;
public:
  cActionKillDemePercent(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_pctkills(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_pctkills = largs.PopWord().AsDouble();
    
    assert(m_pctkills >= 0);
    assert(m_pctkills <= 1);
  }
  
  static const cString GetDescription() { return "Arguments: [double pctkills=0.0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    int target_cell;
    cPopulation& pop = m_world->GetPopulation();
    
    long cells_scanned = 0;
    long orgs_killed = 0;
    long cells_empty = 0;
    
    for (int d = 0; d < pop.GetNumDemes(); d++) {
      
      cDeme &deme = pop.GetDeme(d);
      
      if(deme.IsTreatableNow()) {
        for (int c = 0; c < deme.GetWidth() * deme.GetHeight(); c++) {
          cells_scanned++;
          target_cell = deme.GetCellID(c); 
          cPopulationCell& cell = pop.GetCell(target_cell);
          
          if(ctx.GetRandom().P(m_pctkills)) {
            if(cell.IsOccupied()) {
              pop.KillOrganism(pop.GetCell(target_cell), ctx); 
              orgs_killed++;
            } else {
              cells_empty++; 
            }
          }      
          
        } //End iterating through all cells
        
      } //End if deme is treatable
      
    } //End iterating through all demes
    
    m_world->GetStats().AddNumCellsScannedAtKill(cells_scanned);
    m_world->GetStats().AddNumOrgsKilled(orgs_killed);
    m_world->GetStats().AddNumUnoccupiedCellAttemptedToKill(cells_empty);
    
  } //End Process()
};


/*
 Set the ages at which treatable demes can be treated
 
 Parameters:
 - 1+ age numbers at which a deme may be treated (int)
 */

class cActionSetDemeTreatmentAges : public cAction
{
private:
  std::set<int> treatment_ages;
public:
  static const cString GetDescription() { return "Arguments: <int treatment age>+"; }
  
  cActionSetDemeTreatmentAges(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    while (largs.GetSize()) {
      treatment_ages.insert(largs.PopWord().AsInt());
    }
  }
  
  void Process(cAvidaContext&)
  {
    for (int d = 0; d < m_world->GetPopulation().GetNumDemes(); d++) {
      cDeme& deme = m_world->GetPopulation().GetDeme(d);
      
      if(deme.isTreatable()) {
        for (std::set<int>::iterator it = treatment_ages.begin(); it != treatment_ages.end(); it++) {
          deme.AddTreatmentAge(*it);
        }
      }
      
    } //End iterating through demes
    
  } //End Process()
  
};


/*! Diffuse HGT genome fragments.
 
 If HGT is enabled, each cell will gradually (ok, ok - it'll happen quickly) build up
 a buffer of genome fragments.  This event triggers the diffusion of those fragments.
 
 For right now, we evaluate each cell in order; fix this if we start seeing diffusion
 artifacts.
 
 The actual code for performing the diffusion lives over in cPopulationCell::DiffuseGenomeFragments().
 */
class cActionDiffuseHGTGenomeFragments : public cAction {
public:
	static const cString GetDescription() { return "Arguments: <none>"; }
	
	//! Constructor.
	cActionDiffuseHGTGenomeFragments(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
	}
	
	//! Process this event.
	void Process(cAvidaContext&) {
		for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
			m_world->GetPopulation().GetCell(i).DiffuseGenomeFragments();
		}
	}
};

/*! Avidian conjugation.
 
 This event is an approximation of bacterial conjugation.  Behind the scenes, this
 is implemented as intra-lifetime HGT, and so requires that HGT be enabled.
 
 Each time this event runs, each individual in the population has a configurable
 probability of being the conjugate "donor."  If so, a genome fragment from that
 individual is deposited in a buffer in the receiving organism.  When the receiver
 next replicates, that fragment will be incorporated into its offspring.
 
 \todo I suppose that the fragment could be inserted at runtime, but I fear there
 would be... complications... to runtime changes to an organism's genome...
 */
class cActionAvidianConjugation : public cAction {
public:
  static const cString GetDescription() { return "Arguments: (prob. of donation)"; }
  
  //! Constructor.
  cActionAvidianConjugation(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args), m_donation_p(-1.0) {
    cString largs(args);
    if(largs.GetSize()) {
      m_donation_p = largs.PopWord().AsDouble();
    } 
    
    if((m_donation_p < 0.0) || (m_donation_p > 1.0)) {
      feedback.Error("Conjugate event must include probability of donation [0..1].");
    }
  }
  
  //! Process this event.
  void Process(cAvidaContext& ctx) {
    for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
      cOrganism* org = m_world->GetPopulation().GetCell(i).GetOrganism();			
      if(org && (m_donation_p > 0.0) && ctx.GetRandom().P(m_donation_p)) {
        org->GetOrgInterface().DoHGTDonation(ctx);
      }
    }
  }
  
private:
	double m_donation_p; //!< Per-individual probability of being a conjugate donor.
};


/* This action migrates a configurable number of organisms from one deme to another if
 the level of some global (deme) resource is above the configured threshold.
 
 Parameters: 3
 - name of the resource.  This must be a deme-level global resource.
 - threshold level for resource.  above this, organisms are migrated.
 - number of organisms to migrate to a randomly-chosen deme.
 
 */

class cActionMigrateDemes : public cAction
{
private:
  cString m_res;
  double m_thresh;
  int m_numorgs;
public:
  static const cString GetDescription() { return "Arguments: <string resource name><double failure_percent>"; }
  
  cActionMigrateDemes(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_thresh(0), m_numorgs(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res = largs.PopWord();
    if (largs.GetSize()) m_thresh = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_numorgs = largs.PopWord().AsInt();
    
    assert(m_thresh >= 0);
    assert(m_numorgs >= 0);
    
    assert(m_world->GetConfig().NUM_DEMES.Get() > 1);
    
    //Speculative execution will not work since we are moving organisms around.
    assert(m_world->GetConfig().SPECULATIVE.Get() == 0);
  }
  
  void Process(cAvidaContext& ctx)
  {
    int src_cellid, dest_cellid;
    
    for (int d = 0; d < m_world->GetPopulation().GetNumDemes(); d++) {
      cDeme& deme = m_world->GetPopulation().GetDeme(d);
      int deme_size = deme.GetWidth() * deme.GetHeight();
      
      const cResourceCount &res = deme.GetDemeResourceCount();
      const int resid = res.GetResourceByName(m_res);
      
      if(resid == -1) {
        //Resource doesn't exist for this deme.  This is a bad situation, but just go to next deme.
        cerr << "Error: Resource \"" << m_res << "\" not defined for this deme" << endl;
        continue;
      }
      
      if(res.Get(ctx, resid) >= m_thresh) {
        //Set the resource to zero
        deme.AdjustResource(ctx, resid, (-1 * res.Get(ctx, resid)));
        
        //Pick a deme to move to
        int target_demeid = ctx.GetRandom().GetInt(0, m_world->GetConfig().NUM_DEMES.Get()-1);
        cDeme& target_deme = m_world->GetPopulation().GetDeme(target_demeid);
        int target_deme_size = target_deme.GetWidth() * target_deme.GetHeight();
        
        //Migrate up to m_numorgs orgs
        for(int i = 0; i < m_numorgs; i++) {
          src_cellid = -1;
          dest_cellid = -1;
          
          int counter = 0;
          do {
            src_cellid = ctx.GetRandom().GetInt(0, (deme.GetWidth() * deme.GetHeight())-1);
            counter++;
          } while((counter < deme_size) && (!deme.GetCell(src_cellid).IsOccupied()));
          
          counter = 0;
          do {
            dest_cellid = ctx.GetRandom().GetInt(0, target_deme_size - 1);
            counter++;
          } while((counter < target_deme_size) && (target_deme.GetCell(dest_cellid).IsOccupied())); 
          
          if( (src_cellid != -1) && (dest_cellid != -1) ) {
            
            m_world->GetPopulation().MoveOrganisms(ctx, src_cellid, dest_cellid, -1);
            
            deme.DecOrgCount();
            target_deme.IncOrgCount();
          }
          
          //migrate the organism from src_cell to dest cell
        }
        
        m_world->GetStats().IncNumMigrations();
        
      }
      
    } //End iterating through demes
    
  }
};

/* Record condensed trace files for particular orgs. */
class cActionPrintMiniTraces : public cAction
{
private:
  bool m_random;
  bool m_save_dominants;
  bool m_save_groups;
  bool m_save_foragers;
  int m_orgs_per;
  int m_max_samples;
  bool m_print_genomes;
  bool m_print_reacs;
  
public:
  cActionPrintMiniTraces(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args), m_random(false), m_save_dominants(false), m_save_groups(false), m_save_foragers(false), m_orgs_per(1), m_max_samples(0), 
  m_print_genomes(true), m_print_reacs(false)
  {
    cArgSchema schema(':','=');
    
    // Entries
    schema.AddEntry("random", 0, 0, 1, 0);
    schema.AddEntry("save_dominants", 1, 0, 1, 0);
    schema.AddEntry("save_groups", 2, 0, 1, 0);
    schema.AddEntry("save_foragers", 3, 0, 1, 0);
    schema.AddEntry("orgs_per", 4, 1);
    schema.AddEntry("max_samples", 5, 0); // recommended if using save_groups and restrict to defined is not set
    schema.AddEntry("print_genomes", 6, 0, 1, 1);
    schema.AddEntry("print_reacs", 7, 0, 1, 0);
    
    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (args) {
      m_random = argc->GetInt(0);
      m_save_dominants = argc->GetInt(1);
      m_save_groups = argc->GetInt(2);
      m_save_foragers = argc->GetInt(3);
      m_orgs_per = argc->GetInt(4);
      m_max_samples = argc->GetInt(5);
      m_print_genomes = argc->GetInt(6);
      m_print_reacs = argc->GetInt(7);
    }
  }
  
  static const cString GetDescription() { return "Arguments: [boolean random=0] [boolean save_dominants=0] [boolean save_groups=0] [boolean save_foragers=0] [int orgs_per=1] [int max_samples=0] [boolean print_genomes=1] [boolean print_reacs = 0]"; }
  
  void Process(cAvidaContext&)
  {
    Apto::Array<int, Apto::Smart> target_bgs;
    target_bgs.Resize(0);
    if (m_random) target_bgs = m_world->GetPopulation().SetRandomTraceQ(m_max_samples);
    else target_bgs = m_world->GetPopulation().SetTraceQ(m_save_dominants, m_save_groups, m_save_foragers, m_orgs_per, m_max_samples);
    m_world->GetPopulation().SetMiniTraceQueue(target_bgs, m_print_genomes, m_print_reacs);
  }
};

/* Record condensed trace files for particular orgs. */
class cActionPrintMicroTraces : public cAction
{
private:
  bool m_random;
  bool m_rand_prey;
  bool m_rand_pred;
  int m_next_prey;
  int m_next_pred;
  bool m_save_dominants;
  bool m_save_groups;
  bool m_save_foragers;
  int m_orgs_per;
  int m_max_samples;
  bool m_print_genomes;
  bool m_print_reacs;
  
public:
  cActionPrintMicroTraces(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args), m_random(false), m_rand_prey(false), m_rand_pred(false), m_next_prey(false), m_next_pred(false), m_save_dominants(false), m_save_groups(false), m_save_foragers(false), m_orgs_per(1), m_max_samples(0), m_print_genomes(true), m_print_reacs(false)
  {
    cArgSchema schema(':','=');
    
    // Entries
    schema.AddEntry("random", 0, 0, 1, 0);
    schema.AddEntry("rand_prey", 1, 0, 1, 0);
    schema.AddEntry("rand_pred", 2, 0, 1, 0);
    schema.AddEntry("next_prey", 3, 0);
    schema.AddEntry("next_pred", 4, 0);
    schema.AddEntry("save_dominants", 5, 0, 1, 0);
    schema.AddEntry("save_groups", 6, 0, 1, 0);
    schema.AddEntry("save_foragers", 7, 0, 1, 0);
    schema.AddEntry("orgs_per", 8, 1);
    schema.AddEntry("max_samples", 9, 0); // recommended if using save_groups and restrict to defined is not set
    schema.AddEntry("print_genomes", 10, 0, 1, 0);
    schema.AddEntry("print_reacs", 11, 0, 1, 0);
    
    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (args) {
      m_random = argc->GetInt(0);
      m_rand_prey = argc->GetInt(1);
      m_rand_pred = argc->GetInt(2);
      m_next_prey = argc->GetInt(3);
      m_next_pred = argc->GetInt(4);
      m_save_dominants = argc->GetInt(5);
      m_save_groups = argc->GetInt(6);
      m_save_foragers = argc->GetInt(7);
      m_orgs_per = argc->GetInt(8);
      m_max_samples = argc->GetInt(9);
      m_print_genomes = argc->GetInt(10);
      m_print_reacs = argc->GetInt(11);
    }
  }
  
  static const cString GetDescription() { return "Arguments: [boolean random=0] [boolean rand_prey=0] [boolean rand_pred=0] [int next_prey=0] [int next_pred=0] [boolean save_dominants=0] [boolean save_groups=0] [boolean save_foragers=0] [int orgs_per=1] [int max_samples=0] [boolean print_genomes=0] [boolean print_reacs=0]"; }
  
  void Process(cAvidaContext& ctx)
  { 
    Apto::Array<int, Apto::Smart> target_bgs;
    target_bgs.Resize(0);
    if (m_next_prey || m_next_pred) {
      if (m_next_prey) m_world->GetPopulation().SetNextPreyQ(m_next_prey, m_print_genomes, m_print_reacs, true);
      if (m_next_pred) m_world->GetPopulation().SetNextPredQ(m_next_pred, m_print_genomes, m_print_reacs, true);
    }
    if (m_rand_prey || m_rand_pred) {
      if (m_rand_prey) target_bgs = m_world->GetPopulation().SetRandomPreyTraceQ(m_max_samples);
      if (m_rand_pred) {
        if (target_bgs.GetSize() == 0) target_bgs = m_world->GetPopulation().SetRandomPredTraceQ(m_max_samples);
        else {
          Apto::Array<int, Apto::Smart> pred_bgs = m_world->GetPopulation().SetRandomPredTraceQ(m_max_samples);
          for (int i = 0; i < pred_bgs.GetSize(); i++) target_bgs.Push(pred_bgs[i]); 
        }
      }
    }
    else if (m_random) target_bgs = m_world->GetPopulation().SetRandomTraceQ(m_max_samples);
    else target_bgs = m_world->GetPopulation().SetTraceQ(m_save_dominants, m_save_groups, m_save_foragers, m_orgs_per, m_max_samples);
    
    if (target_bgs.GetSize() > 0) m_world->GetPopulation().AppendMiniTraces(target_bgs, m_print_genomes, m_print_reacs);
  }
};

/* Record condensed trace files for pre-specificied genotypes. */
class cActionLoadMiniTraceQ : public cAction
{
private:
  cString m_filename; 
  int m_orgs_per;
  bool m_print_genomes;
  bool m_print_reacs;
  
public:
  cActionLoadMiniTraceQ(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args), m_filename(""), m_orgs_per(1), m_print_genomes(true), m_print_reacs(false)
  {
    cArgSchema schema(':','=');
    
    // Entries
    schema.AddEntry("file", 0, "genotype_ids");
    schema.AddEntry("orgs_per", 0, 1);
    schema.AddEntry("print_genomes", 1, 0, 1, 1);
    schema.AddEntry("print_reacs", 2, 0, 1, 0);

    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (args) {
      m_filename = argc->GetString(0);
      m_orgs_per = argc->GetInt(0);
      m_print_genomes = argc->GetInt(1);
      m_print_reacs = argc->GetInt(2);
    }
  }
  
  static const cString GetDescription() { return "Arguments: <cString fname> [int orgs_per=1] [boolean print_genomes=1] [boolean print_reacs=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().LoadMiniTraceQ(m_filename, m_orgs_per, m_print_genomes, false);
  }
};

/* Record and print some data up to first reproduction for every org alive now. */
// will pring nothing if org dies or run ends prior to first birth
class cActionPrintReproData : public cAction
{
private:
  
public:
  cActionPrintReproData(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args)
  {
  }
  
  static const cString GetDescription() { return "Arguments: ''"; }

  void Process(cAvidaContext& ctx)
  { 
    const Apto::Array<cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
    for (int i = 0; i < live_orgs.GetSize(); i++) {  
      m_world->GetPopulation().AppendRecordReproQ(live_orgs[i]);
    }
  }
};

/*   Record and print some nav data up to first reproduction for best of orgs alive now, including trace execution,
  locations, and facings. Will print these data for the org among those with the highest reaction achieved by
  time of reproduction in shortest amount of time (as measured by cycles). Will print nothing if any of the 
  candidate orgs are still alive when avida exits and no FlushTopNavTrace events were called.
  Meant for use in behavioral trials where call to this event happens at start of update 0 of orgs lives.
*/
class cActionPrintTopNavTrace : public cAction
{
private:
  
public:
  cActionPrintTopNavTrace(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args)
  {
  }
  
  static const cString GetDescription() { return "Arguments: ''"; }

  void Process(cAvidaContext& ctx)
  { 
    m_world->GetPopulation().SetTopNavQ();
  }
};

class cActionPrintNavTrace : public cAction
{
private:
  
public:
  cActionPrintNavTrace(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args)
  {
  }
  
  static const cString GetDescription() { return "Arguments: ''"; }

  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().SetNavTrace(true);
    m_world->GetPopulation().SetTopNavQ();
  }
};

/* Force Printing of current TopNacTrace even if orgs still being tracked. */
class cActionFlushTopNavTrace : public cAction
{
private:
  
public:
  cActionFlushTopNavTrace(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args)
  {
  }
  
  static const cString GetDescription() { return "Arguments: ''"; }

  void Process(cAvidaContext& ctx)
  { 
    m_world->GetStats().PrintTopNavTrace(true);
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


/*
 Modifies an instruction's prob of failure during a run  */
class cActionSetProbFail : public cAction
{
private:
	cString m_inst_name;
	double m_prob;
public:
	cActionSetProbFail(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
	{
		cString largs(args);
		m_inst_name = largs.PopWord();
		
		//Make sure that the instruction the user has specified is in the instruction set
		assert(world->GetHardwareManager().GetDefaultInstSet().InstInSet(m_inst_name));
    
		//Get the new redundancy
		m_prob = largs.PopWord().AsInt();
	}
	
	static const cString GetDescription() { return "Arguments: <char instruction> <int prob_fail>"; }
	
	void Process(cAvidaContext& ctx)
	{
		//cInstSet& is = m_world->GetHardwareManager().GetInstSet(m_world->GetHardwareManager().GetDefaultInstSet().GetInstSetName());
		cInstSet& is = m_world->GetHardwareManager().GetInstSet("(default)");
		Instruction inst = is.GetInst(m_inst_name);
		is.SetProbFail(inst, m_prob);
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
  action_lib->Register<cActionInjectModuloDemes>("InjectModuloDemes");
  action_lib->Register<cActionInjectDemesFromNest>("InjectDemesFromNest");
  action_lib->Register<cActionInjectDemesRandom>("InjectDemesRandom");
	
  action_lib->Register<cActionInjectGroup>("InjectGroup");
  action_lib->Register<cActionInjectParasite>("InjectParasite");
  action_lib->Register<cActionInjectParasiteSequence>("InjectParasiteSequence");
  action_lib->Register<cActionInjectParasitePair>("InjectParasitePair");
  
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
  action_lib->Register<cActionSerialTransfer>("SerialTransfer");
  action_lib->Register<cActionReplaceFromGermline>("ReplaceFromGermline");
  
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
  action_lib->Register<cActionMixPopulation>("MixPopulation");
	
  action_lib->Register<cActionDecayPoints>("DecayPoints");
  
  action_lib->Register<cActionFlash>("Flash");
  
  action_lib->Register<cActionSetRedundancy>("SetRedundancy");
  action_lib->Register<cActionSetProbFail>("SetProbFail");
	
  /****AbstractCompeteDemes sub-classes****/
	
  action_lib->Register<cAbstractCompeteDemes_AttackKillAndEnergyConserve>("CompeteDemes_AttackKillAndEnergyConserve");
  action_lib->Register<cAssignRandomCellData>("AssignRandomCellData");
  action_lib->Register<cActionMeasureDemeNetworks>("MeasureDemeNetworks");
  action_lib->Register<cActionDistributeData>("DistributeData");
  action_lib->Register<cActionDistributeDataEfficiently>("DistributeDataEfficiently");
  action_lib->Register<cActionDistributeDataCheaply>("DistributeDataCheaply");
  
  action_lib->Register<cActionCompeteDemesByNetwork>("CompeteDemesByNetwork");
  action_lib->Register<cActionIteratedConsensus>("IteratedConsensus");
  action_lib->Register<cActionCountOpinions>("CountOpinions");
  action_lib->Register<cActionCountMultipleOpinions>("CountMultipleOpinions");
  action_lib->Register<cActionDemeReactionDiversity>("DemeReactionDiversity");
  action_lib->Register<cActionDemeBalanceTwoTasks>("DemeBalanceTwoTasks");
  action_lib->Register<cActionSynchronization>("Synchronization");
  action_lib->Register<cActionDesynchronization>("Desynchronization");
  action_lib->Register<cActionPhenotypeMatch>("PhenotypeMatch");
  action_lib->Register<cActionUnitFitness>("UnitFitness");
	
  action_lib->Register<cActionNewTrial>("NewTrial");
  action_lib->Register<cActionCompeteOrganisms>("CompeteOrganisms");
  
  action_lib->Register<cActionSeverGridCol>("SeverGridCol");
  action_lib->Register<cActionSeverGridRow>("SeverGridRow");
  action_lib->Register<cActionJoinGridCol>("JoinGridCol");
  action_lib->Register<cActionJoinGridRow>("JoinGridRow");
	
  action_lib->Register<cActionConnectCells>("ConnectCells");
  action_lib->Register<cActionDisconnectCells>("DisconnectCells");
  action_lib->Register<cActionSwapCells>("SwapCells");
  action_lib->Register<cActionSwapRandomCells>("SwapRandomCells");
	
  action_lib->Register<cActionCompeteDemesByTaskCount>("CompeteDemesByTaskCount");
  action_lib->Register<cActionCompeteDemesByTaskCountAndEfficiency>("CompeteDemesByTaskCountAndEfficiency");
  action_lib->Register<cActionCompeteDemesByEnergyDistribution>("CompeteDemesByEnergyDistribution");
  action_lib->Register<cActionCompeteDemesByMerit>("CompeteDemesByMerit");
	
  /* deme predicate*/
  action_lib->Register<cActionPred_DemeEventMoveCenter>("Pred_DemeEventMoveCenter");
  action_lib->Register<cActionPred_DemeEventMoveBetweenTargets>("Pred_DemeEventMoveBetweenTargets");
  action_lib->Register<cActionPred_DemeEventEventNUniqueIndividualsMovedIntoTarget>("Pred_DemeEventNUniqueIndividualsMovedIntoTarget");
  action_lib->Register<cActionPred_DemeResourceThresholdPredicate>("DemeResourceThresholdPredicate");
  
  action_lib->Register<cActionKillNBelowResourceThreshold>("KillNBelowResourceThreshold");
  action_lib->Register<cActionKillWithinRadiusBelowResourceThreshold>("KillWithinRadiusBelowResourceThreshold");
  action_lib->Register<cActionKillWithinRadiusMeanBelowResourceThreshold>("KillWithinRadiusMeanBelowResourceThreshold");
  action_lib->Register<cActionKillWithinRadiusBelowResourceThresholdTestAll>("KillWithinRadiusBelowResourceThresholdTestAll");
  action_lib->Register<cActionKillMeanBelowThresholdPaintable>("KillMeanBelowThresholdPaintable");
	
  action_lib->Register<cActionDiffuseHGTGenomeFragments>("DiffuseHGTGenomeFragments");
  action_lib->Register<cActionAvidianConjugation>("AvidianConjugation");
  
  action_lib->Register<cActionPrintMiniTraces>("PrintMiniTraces");
  action_lib->Register<cActionPrintMicroTraces>("PrintMicroTraces");
  action_lib->Register<cActionLoadMiniTraceQ>("LoadMiniTraceQ");
  action_lib->Register<cActionPrintReproData>("PrintReproData");
  action_lib->Register<cActionPrintTopNavTrace>("PrintTopNavTrace");
  action_lib->Register<cActionPrintTopNavTrace>("PrintNavTrace");
  action_lib->Register<cActionFlushTopNavTrace>("FlushTopNavTrace");

  action_lib->Register<cActionRemovePredators>("RemovePredators");
}
