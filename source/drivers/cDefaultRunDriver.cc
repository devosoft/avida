/*
 *  cDefaultRunDriver.cc
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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
#include "cBGGenotype.h"
#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cDriverManager.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>

using namespace std;


cDefaultRunDriver::cDefaultRunDriver(cWorld* world) : m_world(world), m_done(false), 
m_fastforward(false),m_last_generation(0),  m_generation_same_update_count(0) 
{
  cDriverManager::Register(this);
  world->SetDriver(this);
  
  // Save this config variable
  m_generation_update_fastforward_threshold = m_world->GetConfig().FASTFORWARD_UPDATES.Get();
  m_population_fastforward_threshold = m_world->GetConfig().FASTFORWARD_NUM_ORGS.Get();
}

cDefaultRunDriver::~cDefaultRunDriver()
{
  cDriverManager::Unregister(this);
  delete m_world;
}


void cDefaultRunDriver::Run()
{
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
  
  void (cPopulation::*ActiveProcessStep)(cAvidaContext& ctx, double step_size, int cell_id) = &cPopulation::ProcessStep;
  if (m_world->GetConfig().SPECULATIVE.Get() &&
      m_world->GetConfig().THREAD_SLICING_METHOD.Get() != 1 && !m_world->GetConfig().IMPLICIT_REPRO_END.Get()) {
    ActiveProcessStep = &cPopulation::ProcessStepSpeculative;
  }
  
  cAvidaContext& ctx = m_world->GetDefaultContext();
  
  while (!m_done) {
    if (cChangeList* change_list = population.GetChangeList()) {
      change_list->Reset();
    }
    
    m_world->GetEvents(ctx);
    if(m_done == true) break;
    
    // Increment the Update.
    stats.IncCurrentUpdate();
    
    // Handle all data collection for previous update.
    if (stats.GetUpdate() > 0) {
      // Tell the stats object to do update calculations and printing.
      stats.ProcessUpdate();
    }
    
    // don't process organisms if we are in fast-forward mode. -- @JEB
    if (!GetFastForward()) {
      // Process the update.
			// query the world to calculate the exact size of this update:
      const int UD_size = m_world->CalculateUpdateSize();
      const double step_size = 1.0 / (double) UD_size;
      
      for (int i = 0; i < UD_size; i++) {
        if(population.GetNumOrganisms() == 0) {
          break;
        }
        (population.*ActiveProcessStep)(ctx, step_size, population.ScheduleOrganism());
      }
    }
    
    // end of update stats...
    population.ProcessPostUpdate(ctx);

		m_world->ProcessPostUpdate(ctx);
        
    // No viewer; print out status for this update....
    if (m_world->GetVerbosity() > VERBOSE_SILENT) {
      cout.setf(ios::left);
      cout.setf(ios::showpoint);
      cout << "UD: " << setw(6) << stats.GetUpdate() << "  ";
      cout << "Gen: " << setw(9) << setprecision(7) << stats.SumGeneration().Average() << "  ";
      cout << "Fit: " << setw(9) << setprecision(7) << stats.GetAveFitness() << "  ";
      cout << "Orgs: " << setw(6) << population.GetNumOrganisms() << "  ";
      if (m_world->GetPopulation().GetNumDemes() > 1) cout << "Demes: " << setw(4) << stats.GetNumOccupiedDemes() << " ";
      if (m_world->GetVerbosity() == VERBOSE_ON || m_world->GetVerbosity() == VERBOSE_DETAILS) {
        cout << "Merit: " << setw(9) << setprecision(7) << stats.GetAveMerit() << "  ";
        cout << "Thrd: " << setw(6) << stats.GetNumThreads() << "  ";
        cout << "Para: " << stats.GetNumParasites() << "  ";
        cout << "GenEntr: " << stats.GetEntropy() << "  ";
      }
      if (m_world->GetVerbosity() >= VERBOSE_DEBUG) {
        cout << "Spec: " << setw(6) << setprecision(4) << stats.GetAveSpeculative() << "  ";
        cout << "SWst: " << setw(6) << setprecision(4) << (((double)stats.GetSpeculativeWaste() / (double)m_world->CalculateUpdateSize()) * 100.0) << "%  ";
        cout << "GSz: " << setw(4) << setprecision(3) << (double)((stats.GetNumGenotypes() + stats.GetNumGenotypesHistoric()) * sizeof(cBGGenotype)) / 1048576.0 << "m";
      }

      cout << endl;
    }
    
    
    // Do Point Mutations
    if (point_mut_prob > 0 ) {
      for (int i = 0; i < population.GetSize(); i++) {
        if (population.GetCell(i).IsOccupied()) {
          population.GetCell(i).GetOrganism()->GetHardware().PointMutate(ctx, point_mut_prob);
        }
      }
    }
    
    // Keep track of changes in generation for fast-forward purposes
    UpdateFastForward(stats.GetGeneration(),stats.GetNumCreatures());
    
    // Exit conditons...
    if((population.GetNumOrganisms()==0) && m_world->AllowsEarlyExit()) {
			m_done = true;
		}
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

void cDefaultRunDriver::UpdateFastForward (double inGeneration, int population)
{
  if (bool(m_population_fastforward_threshold))
  {
    if (population >= m_population_fastforward_threshold) m_fastforward = true;
    else m_fastforward = false;
  }
  if (!m_generation_update_fastforward_threshold) return;
  
  if (inGeneration == m_last_generation)
  {
    m_generation_same_update_count++;
    if (m_generation_same_update_count >= m_generation_update_fastforward_threshold) m_fastforward = true;
  }
  else
  {
    m_generation_same_update_count = 0;
    m_last_generation = inGeneration;
  }
}
