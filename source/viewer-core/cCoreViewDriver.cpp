/*
 *  cCoreViewDriver.cpp
 *  Avida
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#include "cCoreViewDriver.h"

#include "cAvidaContext.h"
#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cCoreViewListener.h"
#include "cDriverManager.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cStringList.h"
#include "cWorld.h"

#include <iostream>


cCoreViewDriver::cCoreViewDriver(cWorld* world) : m_world(world), m_done(false), m_paused(false)
{
  cDriverManager::Register(this);

}

cCoreViewDriver::~cCoreViewDriver()
{
  m_mutex.Lock();
  m_done = true;
  m_mutex.Unlock();
  m_pause_cv.Broadcast();
  Join();
  
  cDriverManager::Unregister(this);
  delete m_world;
}

void cCoreViewDriver::Run()
{
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
  
  cAvidaContext ctx(m_world, m_world->GetRandom());
  
  m_mutex.Lock();
  while (!m_done) {
    m_mutex.Unlock();
    if (cChangeList* change_list = population.GetChangeList()) change_list->Reset();
    
    m_world->GetEvents(ctx);
    if (m_done) break;  // Stop here if told to do so by an event.
    
    // Increment the Update.
    stats.IncCurrentUpdate();
    
    // Handle all data collection for previous update.
    if (stats.GetUpdate() > 0) {
      // Tell the stats object to do update calculations and printing.
      stats.ProcessUpdate();
    }
    
    
    // Process the update.
    const int UD_size = ave_time_slice * population.GetNumOrganisms();
    const double step_size = 1.0 / (double) UD_size;
    
    
    // Are we stepping through an organism?
//    if (m_info.GetStepOrganism() != -1) {  // Yes we are!
//      // Keep the viewer informed about the organism we are stepping through...
//      for (int i = 0; i < UD_size; i++) {
//        const int next_id = population.ScheduleOrganism();
//        if (next_id == m_info.GetStepOrganism()) {
//          DoUpdate();
//        }
//        population.ProcessStep(ctx, step_size, next_id);
//      }
//    }
//    else {
      for (int i = 0; i < UD_size; i++) population.ProcessStep(ctx, step_size, population.ScheduleOrganism());
//    }
    
    
    // end of update stats...
    population.ProcessPostUpdate(ctx);
    
    
    for (int i = 0; i < m_listeners.GetSize(); i++) {
      m_listeners[i]->NotifyUpdate(stats.GetUpdate());
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
    m_mutex.Lock();
    if (population.GetNumOrganisms() == 0) m_done = true;
    while (!m_done && m_pause_state == DRIVER_PAUSED) {
      m_paused = true;
      m_pause_cv.Wait(m_mutex);
    }
    m_paused = false;
  }  
  m_mutex.Unlock();
}


void cCoreViewDriver::RaiseException(const cString& in_string)
{
  std::cerr << "Error: " << in_string << std::endl;
}

void cCoreViewDriver::RaiseFatalException(int exit_code, const cString& in_string)
{
  std::cerr << "Error: " << in_string << "  Exiting..." << std::endl;
  exit(exit_code);
}

void cCoreViewDriver::NotifyComment(const cString& in_string)
{
  std::cout << in_string << std::endl;
}

void cCoreViewDriver::NotifyWarning(const cString& in_string)
{
  std::cout << "Warning: " << in_string << std::endl;
}
