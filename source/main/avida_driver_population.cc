//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_DRIVER_POPULATION_HH
#include "avida_driver_population.hh"
#endif

#ifndef AVIDA_TRIGGERS_HH
#include "avida_triggers.hh"
#endif
#ifndef CALLBACK_UTIL_HH
#include "callback_util.hh"
#endif
#ifndef CONFIG_HH
#include "config.hh"
#endif
#ifndef EVENT_FACTORY_MANAGER_HH
#include "event_factory_manager.hh"
#endif
#ifndef EVENT_LIST_HH
#include "event_list.hh"
#endif
#ifndef GENEBANK_HH
#include "genebank.hh"
#endif
#ifndef GENOTYPE_HH
#include "genotype.hh"
#endif
#ifndef HARDWARE_BASE_HH
#include "hardware_base.hh"
#endif
#ifndef HARDWARE_FACTORY_HH
#include "hardware_factory.hh"
#endif
#ifndef INIT_FILE_HH
#include "init_file.hh"
#endif
#ifndef ORGANISM_HH
#include "organism.hh"
#endif
#ifndef POPULATION_HH
#include "population.hh"
#endif
#ifndef POPULATION_CELL_HH
#include "population_cell.hh"
#endif
#ifndef POPULATION_EVENT_FACTORY_HH
#include "population_event_factory.hh"
#endif
#ifndef POPULATION_INTERFACE_HH
#include "population_interface.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TOOLS_HH
#include "tools.hh"
#endif

#include <iostream>

using namespace std;

/////////////////////////////
//  cAvidaDriver_Population
/////////////////////////////

cAvidaDriver_Population::cAvidaDriver_Population(cEnvironment & environment)
  : population(NULL)
  , event_manager(NULL)
  , event_list(NULL)
{
  // Setup Population
  cout << "Initializing Population..." << flush;
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

  population = new cPopulation(default_interface, environment);
  cout << " ...done" << endl;

  //Setup Event List
  cout<<"Initializing Event Factory Manager..."<<flush;
  event_manager = new cEventFactoryManager;
  cStats & stats = population->GetStats();
  event_list = new cEventList( event_manager, new cAvidaTriggers(stats) );
  cout<<"...Factories..."<<flush;

  // in principle, one could add more than one event factory here.
  // however, this is not a good idea, because the automatic documentation
  // system cannot cope with this at this point. Claus
  event_manager->AddFactory(new cPopulationEventFactory(population));
  cout<<" ...done"<<endl;

  cout<<"Reading Event List File..."<<flush;
  ReadEventListFile(cConfig::GetEventFilename());
  cout<<" ...done"<<endl;

  // Make sure the directory 'genebank' exits!
  cTools::MkDir("genebank", true);
}

cAvidaDriver_Population::~cAvidaDriver_Population()
{
  if (population != NULL) delete population;
}

void cAvidaDriver_Population::Run()
{
  // cout << "DEBUG: Turning control over to driver..." << endl;

  assert( population != NULL );

  // Process until done...
  while ( !ProcessUpdate() ) {} 
}

bool cAvidaDriver_Population::ProcessUpdate()
{
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
  //  cout << "DEBUG: Running viewer-less update..." << endl;

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
  cout << "UD: "  << stats.GetUpdate() << "\t"
       << "Gen: " << stats.SumGeneration().Average() << "\t"
       << "Fit: " << stats.GetAveFitness() << "\t"
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
