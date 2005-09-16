//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cAvidaDriver_Population.h"

#include "cAvidaTriggers.h"
#include "cCallbackUtil.h"
#include "change_list.hh"
#include "cConfig.h"
#include "cEventFactoryManager.h"
#include "cEventList.h"
#include "cGenebank.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cHardwareFactory.h"
#include "init_file.hh"
#include "organism.hh"
#include "population.hh"
#include "population_cell.hh"
#include "cPopulationEventFactory.h"
#include "cPopulationInterface.h"
#include "string.hh"
#include "tools.hh"

#include <iostream>
#include <iomanip>

using namespace std;

/////////////////////////////
//  cAvidaDriver_Population
/////////////////////////////

cAvidaDriver_Population::cAvidaDriver_Population(cEnvironment & environment, cChangeList * change_list)
  : population(NULL), event_manager(NULL), event_list(NULL)
{
  // Setup Population    
  cPopulationInterface default_interface;
  default_interface.SetFun_NewHardware(&cCallbackUtil::CB_NewHardware);
  default_interface.SetFun_Recycle(&cCallbackUtil::CB_RecycleHardware);
  default_interface.SetFun_Divide(&cCallbackUtil::CB_Divide);
  default_interface.SetFun_TestOnDivide(&cCallbackUtil::CB_TestOnDivide);
  default_interface.SetFun_GetNeighbor(&cCallbackUtil::CB_GetNeighbor);
  default_interface.SetFun_NumNeighbors(&cCallbackUtil::CB_GetNumNeighbors);
  default_interface.SetFun_Rotate(&cCallbackUtil::CB_Rotate);
  default_interface.SetFun_Breakpoint(&cCallbackUtil::CB_Breakpoint);
  default_interface.SetFun_TestFitness(&cCallbackUtil::CB_TestFitness);
  default_interface.SetFun_GetInput(&cCallbackUtil::CB_GetInput);
  default_interface.SetFun_GetInputAt(&cCallbackUtil::CB_GetInputAt);
  default_interface.SetFun_Debug(&cCallbackUtil::CB_Debug);
  default_interface.SetFun_GetResources(&cCallbackUtil::CB_GetResources);
  default_interface.SetFun_UpdateResources(&cCallbackUtil::CB_UpdateResources);
  default_interface.SetFun_KillCell(&cCallbackUtil::CB_KillCell);
  default_interface.SetFun_KillSurroundCell(&cCallbackUtil::CB_KillSurroundCell);
  default_interface.SetFun_SendMessage(&cCallbackUtil::CB_SendMessage);
  default_interface.SetFun_ReceiveValue(&cCallbackUtil::CB_ReceiveValue);
  default_interface.SetFun_InjectParasite(&cCallbackUtil::CB_InjectParasite);
  default_interface.SetFun_UpdateMerit(&cCallbackUtil::CB_UpdateMerit);

  population = new cPopulation(default_interface, environment, change_list);

  //Setup Event List
  event_manager = new cEventFactoryManager;
  cStats & stats = population->GetStats();
  event_list = new cEventList( event_manager, new cAvidaTriggers(stats) );
  
  // in principle, one could add more than one event factory here.
  // however, this is not a good idea, because the automatic documentation
  // system cannot cope with this at this point. Claus
  event_manager->AddFactory(new cPopulationEventFactory(population));

  ReadEventListFile(cConfig::GetEventFilename());
  
  // Make sure the directory 'genebank' exits!
  cTools::MkDir("genebank", true);
}

cAvidaDriver_Population::~cAvidaDriver_Population()
{
  if (population != NULL) delete population;
}

void cAvidaDriver_Population::Run()
{
  assert( population != NULL );

  // Process until done...
  while ( !ProcessUpdate() ) {} 
}

bool cAvidaDriver_Population::ProcessUpdate()
{
  if (cChangeList *change_list = population->GetChangeList()) {
    change_list->Reset();
  }

  GetEvents();
  if (done_flag == true) return true;

  // Increment the Update.
  cStats & stats = population->GetStats();
  stats.IncCurrentUpdate();

  // Handle all data collection for previous update.
  if (stats.GetUpdate() > 0) {
    // Tell the stats object to do update calculations and printing.
    stats.ProcessUpdate();

    // Update all the genotypes for the end of this update.
    cGenebank & genebank = population->GetGenebank();
    for (cGenotype * cur_genotype = genebank.ResetThread(0);
	 cur_genotype != NULL && cur_genotype->GetThreshold();
	 cur_genotype = genebank.NextGenotype(0)) {
      cur_genotype->UpdateReset();
    }
  }

  ProcessOrganisms();

  // Do Point Mutations
  if (cConfig::GetPointMutProb() > 0 ) {
    for (int i = 0; i < population->GetSize(); i++) {
      if (population->GetCell(i).IsOccupied()) {
	population->GetCell(i).GetOrganism()->
	  GetHardware().PointMutate(cConfig::GetPointMutProb());
      }
    }
  }
  

#ifdef DEBUG_CHECK_OK
  // If we're in a debug mode, make sure the population is OK.
  if (population->OK() == false) {
    g_debug.Warning("Population::OK() is failing.");
  }
#endif

  // Do any cleanup in the hardware factory...
  cHardwareFactory::Update();
  
  // Exit conditons...
  if (population->GetNumOrganisms() == 0) done_flag = true;
  
  return done_flag;
}


void cAvidaDriver_Population::NotifyUpdate()
{
  // Nothing here for now.  This method should be overloaded and only
  // run if there is no proper viewer.
}


void cAvidaDriver_Population::GetEvents()
{
  assert(population != NULL);

  if (population->GetSyncEvents() == true) {
    SyncEventList();
    population->SetSyncEvents(false);
  }
  event_list->Process();
}

void cAvidaDriver_Population::ProcessOrganisms()
{
  // Process the update.
  const int UD_size =
    cConfig::GetAveTimeslice() * population->GetNumOrganisms();
  const double step_size = 1.0 / (double) UD_size;

  for (int i = 0; i < UD_size; i++) {
    if (population->GetNumOrganisms() == 0) {
      done_flag = true;
      break;
    }
    population->ProcessStep(step_size);
  }

  // end of update stats...
  population->CalcUpdateStats();

  // No viewer; print out status for this update....
  cStats & stats = population->GetStats();
  cout.setf(ios::left);
  cout.setf(ios::showpoint);
  cout << "UD: " << setw(6) << stats.GetUpdate() << "  "
       << "Gen: " << setw(9) << setprecision(7) << stats.SumGeneration().Average() << "  "
       << "Fit: " << setw(9) << setprecision(7) << stats.GetAveFitness() << "  "
       << "Size: " << population->GetNumOrganisms()
       << endl;
}


void cAvidaDriver_Population::
ReadEventListFile(const cString & filename)
{
  cInitFile event_file(filename);

  // Load in the proper event list and set it up.
  event_file.Load();
  event_file.Compress();

  // Loop through the line_list and change the lines to events.
  for (int line_id = 0; line_id < event_file.GetNumLines(); line_id++) {
    event_list->AddEventFileFormat(event_file.GetLine(line_id));
  }

  ////////// Adding events from the Genesis file //////////
  // Max Updates and/or Max Generations
  if( cConfig::GetEndConditionMode() == 0 ){
    if( cConfig::GetMaxUpdates() >= 0 ){
      event_list->AddEvent( cEventTriggers::UPDATE, cConfig::GetMaxUpdates(),
			    cEventTriggers::TRIGGER_ONCE,
			    cEventTriggers::TRIGGER_END, "exit", "");
    }
    if( cConfig::GetMaxGenerations() >= 0 ){
      event_list->AddEvent( cEventTriggers::GENERATION,
			    cConfig::GetMaxGenerations(),
			    cEventTriggers::TRIGGER_ONCE,
			    cEventTriggers::TRIGGER_END, "exit", "");
    }
  }else{
    if( cConfig::GetMaxUpdates() >= 0 && cConfig::GetMaxGenerations() >= 0 ){
      cString max_gen_str;
      max_gen_str.Set("%d",cConfig::GetMaxGenerations());
      cString max_update_str;
      max_update_str.Set("%d",cConfig::GetMaxUpdates());
      event_list->AddEvent( cEventTriggers::UPDATE, cConfig::GetMaxUpdates(),
			    1, cEventTriggers::TRIGGER_END,
			    "exit_if_generation_greater_than", max_gen_str);
      event_list->AddEvent( cEventTriggers::GENERATION,
			    cConfig::GetMaxGenerations(),
			    .01, cEventTriggers::TRIGGER_END,
			    "exit_if_update_greater_than", max_update_str);
    }
  }

}

void cAvidaDriver_Population::SyncEventList()
{
  if( event_list != NULL ){
    event_list->Sync();
  }
}
