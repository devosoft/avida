/*
 *  PrintActions.cc
 *  Avida
 *
 *  Created by David on 5/11/06.
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

#include "PrintActions.h"

#include "avida/core/EventList.h"
#include "avida/core/Feedback.h"
#include "avida/core/InstructionSequence.h"
#include "avida/core/Library.h"
#include "avida/core/Properties.h"
#include "avida/core/UniverseDriver.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include "avida/data/Recorder.h"
#include "avida/output/File.h"
#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"
#include "avida/util/ArgParser.h"

using namespace Avida;


class ActionPrintDominantData : public EventAction
{
private:
  Universe* m_universe;
  Util::Args* m_args;

  
public:
  ActionPrintDominantData(Universe* universe, Util::Args* args) : m_universe(universe), m_args(args) { ; }
  
  static Util::ArgSchema Schema()
  {
    Util::ArgSchema schema;
    schema.Define("filename", "dominant.dat");
    
    return schema;
  }
  
  static EventAction* Create(Universe* universe, Context& ctx, Util::Args* args)
  {
    (void)ctx;
    return new ActionPrintDominantData(universe, args);
  }
  
  
  void Process(Avida::Context& ctx, Update current_update)
  {
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_universe, m_args->String(0));
    
    df->WriteComment("Avida Dominant Data");
    df->WriteTimeStamp();
    
    df->Write((int)current_update,     "Update");
    
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_universe);
    Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
    Systematics::GroupPtr bg = it->Next();
    if (!bg) return;
    
    df->Write(bg->Properties().Get("ave_metabolic_rate").DoubleValue(),       "Average Merit of the Dominant Genotype");
    df->Write(bg->Properties().Get("ave_gestation_time").DoubleValue(),   "Average Gestation Time of the Dominant Genotype");
    df->Write(bg->Properties().Get("ave_fitness").DoubleValue(),     "Average Fitness of the Dominant Genotype");
    df->Write(bg->Properties().Get("ave_repro_rate").DoubleValue(),  "Repro Rate?");
    
    Genome gen(bg->Properties().Get("genome"));
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(gen.Representation());
    df->Write(seq->GetSize(),        "Size of Dominant Genotype");
    df->Write(bg->Properties().Get("ave_copy_size").DoubleValue(), "Copied Size of Dominant Genotype");
    df->Write(bg->Properties().Get("ave_exe_size").DoubleValue(), "Executed Size of Dominant Genotype");
    df->Write(bg->NumUnits(),   "Abundance of Dominant Genotype");
    df->Write(bg->Properties().Get("last_births").IntValue(),      "Number of Births");
    df->Write(bg->Properties().Get("last_breed_true").IntValue(),  "Number of Dominant Breed True?");
    df->Write(bg->Depth(),  "Dominant Gene Depth");
    df->Write(bg->Properties().Get("last_breed_in").IntValue(),    "Dominant Breed In");
    df->Write(bg->Properties().Get("max_fitness").DoubleValue(),     "Max Fitness?");
    df->Write(bg->ID(), "Genotype ID of Dominant Genotype");
    df->Write(bg->Properties().Get("name").StringValue(),        "Name of the Dominant Genotype");
    df->Endl();    
  }
};






void RegisterPrintActions(cActionLibrary* action_lib)
{
  // Stats Out Files
//  action_lib->Register<cActionPrintAverageData>("PrintAverageData");
//  action_lib->Register<cActionPrintErrorData>("PrintErrorData");
//  action_lib->Register<cActionPrintVarianceData>("PrintVarianceData");
//  action_lib->Register<cActionPrintCountData>("PrintCountData");
//  action_lib->Register<cActionPrintTotalsData>("PrintTotalsData");
//  action_lib->Register<cActionPrintTasksData>("PrintActionData");
//  action_lib->Register<cActionPrintTasksQualData>("PrintActionQualData");
//  action_lib->Register<cActionPrintReactionData>("PrintReactionData");
//  action_lib->Register<cActionPrintTimeData>("PrintTimeData");
//  action_lib->Register<cActionPrintExtendedTimeData>("PrintExtendedTimeData");
//  action_lib->Register<cActionPrintMutationRateData>("PrintMutationRateData");
//  action_lib->Register<cActionPrintDivideMutData>("PrintDivideMutData");
  Core::Library::Instance().RegisterEventType("PrintDominantData", ActionPrintDominantData::Schema(), &ActionPrintDominantData::Create);
  
}
