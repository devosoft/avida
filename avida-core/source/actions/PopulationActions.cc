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
    genome = Util::LoadGenomeDetailFile(m_filename, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback);
    if (!genome) return;
    m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, m_cell_id, m_merit, m_neutral_metric, false);
    
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
    genome = Util::LoadGenomeDetailFile(m_filename, m_world->GetWorkingDir(), m_world->GetHardwareManager(), feedback, m_instset);
    if (!genome) return;
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++)
      m_world->GetPopulation().Inject(*genome, Systematics::Source(Systematics::DIVISION, "", true), ctx, i, m_merit, m_neutral_metric);
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


void RegisterPopulationActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionInject>("Inject");
  action_lib->Register<cActionInjectAll>("InjectAll");
  action_lib->Register<cActionInjectSequence>("InjectSequence");
}
