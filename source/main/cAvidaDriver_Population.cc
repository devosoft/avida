/*
 *  cAvidaDriver_Popultion.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cAvidaDriver_Population.h"

#include "cChangeList.h"
#include "cPopulation.h"
#include "cString.h"
#include "cClassificationManager.h"
#include "cGenotype.h"
#include "cPopulationCell.h"
#include "cOrganism.h"
#include "cStats.h"
#include "cHardwareBase.h"

#include <iostream>
#include <iomanip>

using namespace std;


void cAvidaDriver_Population::Run()
{
  assert( m_world != NULL );
  
  // Process until done...
  while ( !ProcessUpdate() ) {} 
}

bool cAvidaDriver_Population::ProcessUpdate()
{
  if (cChangeList* change_list = m_world->GetPopulation().GetChangeList()) {
    change_list->Reset();
  }
  
  m_world->GetEvents();
  if (done_flag == true) return true;
  
  // Increment the Update.
  cStats& stats = m_world->GetStats();
  stats.IncCurrentUpdate();
  
  cPopulation* population = &m_world->GetPopulation();
  
  // Handle all data collection for previous update.
  if (stats.GetUpdate() > 0) {
    // Tell the stats object to do update calculations and printing.
    stats.ProcessUpdate();
    
    // Update all the genotypes for the end of this update.
    cClassificationManager& classmgr = m_world->GetClassificationManager();
    for (cGenotype * cur_genotype = classmgr.ResetThread(0);
         cur_genotype != NULL && cur_genotype->GetThreshold();
         cur_genotype = classmgr.NextGenotype(0)) {
      cur_genotype->UpdateReset();
    }
  }
  
  ProcessOrganisms();
  
  // Do Point Mutations
  if (m_world->GetConfig().POINT_MUT_PROB.Get() > 0 ) {
    for (int i = 0; i < population->GetSize(); i++) {
      if (population->GetCell(i).IsOccupied()) {
        population->GetCell(i).GetOrganism()->GetHardware().PointMutate(m_world->GetConfig().POINT_MUT_PROB.Get());
      }
    }
  }
  
  
#ifdef DEBUG_CHECK_OK
  // If we're in a debug mode, make sure the population is OK.
  if (population->OK() == false) {
    g_debug.Warning("Population::OK() is failing.");
  }
#endif
  
  // Exit conditons...
  if (population->GetNumOrganisms() == 0) done_flag = true;
  
  return done_flag;
}


void cAvidaDriver_Population::NotifyUpdate()
{
  // Nothing here for now.  This method should be overloaded and only
  // run if there is no proper viewer.
}


void cAvidaDriver_Population::ProcessOrganisms()
{
  cPopulation* population = &m_world->GetPopulation();
  
  // Process the update.
  const int UD_size = m_world->GetConfig().AVE_TIME_SLICE.Get() * population->GetNumOrganisms();
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
  cStats & stats = m_world->GetStats();
  cout.setf(ios::left);
  cout.setf(ios::showpoint);
  cout << "UD: " << setw(6) << stats.GetUpdate() << "  "
    << "Gen: " << setw(9) << setprecision(7) << stats.SumGeneration().Average() << "  "
    << "Fit: " << setw(9) << setprecision(7) << stats.GetAveFitness() << "  "
    << "Size: " << population->GetNumOrganisms()
    << endl;
}
