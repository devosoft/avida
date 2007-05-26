/*
 *  cDefaultRunDriver.cc
 *  Avida
 *
 *  Created by David on 12/11/05.
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

#include "cDefaultRunDriver.h"

#include "cAvidaContext.h"
#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cDriverManager.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"

#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;


cDefaultRunDriver::cDefaultRunDriver(cWorld* world) : m_world(world), m_done(false)
{
  cDriverManager::Register(static_cast<cAvidaDriver*>(this));
  world->SetDriver(this);
}

cDefaultRunDriver::~cDefaultRunDriver()
{
  cDriverManager::Unregister(static_cast<cAvidaDriver*>(this));
  delete m_world;
}


void cDefaultRunDriver::Run()
{
  cClassificationManager& classmgr = m_world->GetClassificationManager();
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
  
  cAvidaContext& ctx = m_world->GetDefaultContext();

  while (!m_done) {
    if (cChangeList* change_list = population.GetChangeList()) {
      change_list->Reset();
    }
    
    m_world->GetEvents(ctx);
    if (m_done == true) break;
    
    // Increment the Update.
    stats.IncCurrentUpdate();
    
    // Handle all data collection for previous update.
    if (stats.GetUpdate() > 0) {
      // Tell the stats object to do update calculations and printing.
      stats.ProcessUpdate();
      
      // Update all the genotypes for the end of this update.
      for (cGenotype * cur_genotype = classmgr.ResetThread(0);
           cur_genotype != NULL && cur_genotype->GetThreshold();
           cur_genotype = classmgr.NextGenotype(0)) {
        cur_genotype->UpdateReset();
      }
    }
    
    
    // Process the update.
    const int UD_size = ave_time_slice * population.GetNumOrganisms();
    const double step_size = 1.0 / (double) UD_size;
    
    for (int i = 0; i < UD_size; i++) {
      if (population.GetNumOrganisms() == 0) {
        m_done = true;
        break;
      }
      population.ProcessStep(ctx, step_size);
    }
    
    // end of update stats...
    population.CalcUpdateStats();
    
    // Process the update for each deme.
    for(int i=0; i<population.GetNumDemes(); ++i) {
      population.GetDeme(i).ProcessUpdate();
    }
    
    // No viewer; print out status for this update....
    if (m_world->GetVerbosity() > VERBOSE_SILENT) {
      cout.setf(ios::left);
      cout.setf(ios::showpoint);
      cout << "UD: " << setw(6) << stats.GetUpdate() << "  "
        << "Gen: " << setw(9) << setprecision(7) << stats.SumGeneration().Average() << "  "
        << "Fit: " << setw(9) << setprecision(7) << stats.GetAveFitness() << "  "
        << "Orgs: " << setw(6) << population.GetNumOrganisms() << "  "
        << "Thrd: " << setw(6) << stats.GetNumThreads() << "  "
        << "Para: " << stats.GetNumParasites()
        << endl;
    }
    
    
    // Do Point Mutations
    if (point_mut_prob > 0 ) {
      for (int i = 0; i < population.GetSize(); i++) {
        if (population.GetCell(i).IsOccupied()) {
          population.GetCell(i).GetOrganism()->GetHardware().PointMutate(ctx, point_mut_prob);
        }
      }
    }
    
    // Exit conditons...
    if (population.GetNumOrganisms() == 0) m_done = true;
  }
}

void cDefaultRunDriver::RaiseException(const cString& in_string)
{
  cerr << "Error: " << in_string << endl;
}

void cDefaultRunDriver::RaiseFatalException(int exit_code, const cString& in_string)
{
  cerr << "Error: " << in_string << "  Exiting..." << endl;
  exit(exit_code);
}

void cDefaultRunDriver::NotifyComment(const cString& in_string)
{
  cout << in_string << endl;
}

void cDefaultRunDriver::NotifyWarning(const cString& in_string)
{
  cout << "Warning: " << in_string << endl;
}
