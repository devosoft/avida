/*
 *  viewer-core/Driver.cc
 *  Avida
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 */

#include "avida/viewer-core/Driver.h"

#include "avida/data/Manager.h"
#include "avida/viewer-core/Map.h"
#include "avida/viewer-core/Listener.h"

#include "cAvidaContext.h"
#include "cClassificationManager.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cStringList.h"
#include "cUserFeedback.h"
#include "cWorld.h"

#include <iostream>


Avida::CoreView::Driver::Driver(cWorld* world)
: Apto::Thread(), m_world(world), m_pause_state(DRIVER_UNPAUSED), m_done(false), m_paused(false), m_map(NULL)
{
  GlobalObjectManager::Register(this);
}

Avida::CoreView::Driver::~Driver()
{
  m_mutex.Lock();
  m_done = true;
  m_mutex.Unlock();
  m_pause_cv.Broadcast();
  Join();
  
  delete m_map;
  
  GlobalObjectManager::Unregister(this);
  delete m_world;
}


Avida::CoreView::Driver* Avida::CoreView::Driver::InitWithDirectory(const Apto::String& config_path)
{
  cAvidaConfig* cfg = new cAvidaConfig;
  
  cUserFeedback feedback;
  if (!cfg->Load("avida.cfg", static_cast<const char*>(config_path), &feedback, NULL, false)) {
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
        case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }
    
    return NULL;
  }
  
  cWorld* world = cWorld::Initialize(cfg, static_cast<const char*>(config_path), &feedback);
  
  for (int i = 0; i < feedback.GetNumMessages(); i++) {
    switch (feedback.GetMessageType(i)) {
      case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
      case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
      default: break;
    };
    cerr << feedback.GetMessage(i) << endl;
  }
  
  
  if (!world) return NULL;
  return new Avida::CoreView::Driver(world);  
}



void Avida::CoreView::Driver::Run()
{
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  Data::Manager& dm = m_world->GetDataManager();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get() +
                                m_world->GetConfig().POINT_INS_PROB.Get() +
                                m_world->GetConfig().POINT_DEL_PROB.Get();
  
  cAvidaContext ctx(m_world, m_world->GetRandom());
  
  m_mutex.Lock();
  while (!m_done) {
    m_mutex.Unlock();
    
    m_world->GetEvents(ctx);
    if (m_done) break;  // Stop here if told to do so by an event.
    
    // Increment the Update.
    stats.IncCurrentUpdate();
    
    // Handle all data collection for previous update.
    if (stats.GetUpdate() > 0) {
      // Tell the stats object to do update calculations and printing.
      stats.ProcessUpdate();
      dm.UpdateState(stats.GetUpdate());
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
    
    
    if (m_map) m_map->UpdateMaps(population);
    for (Apto::Set<Listener*>::Iterator it = m_listeners.Begin(); it.Next();) {
      if ((*it.Get())->WantsMap()) {
        (*it.Get())->NotifyMap(m_map);
      }
      if ((*it.Get())->WantsUpdate()) (*it.Get())->NotifyUpdate(stats.GetUpdate());
    }
    
    
    // Do Point Mutations
    if (point_mut_prob > 0 ) {
      for (int i = 0; i < population.GetSize(); i++) {
        if (population.GetCell(i).IsOccupied()) {
          population.GetCell(i).GetOrganism()->GetHardware().PointMutate(ctx);
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


void Avida::CoreView::Driver::RaiseException(const cString& in_string)
{
  std::cerr << "Error: " << in_string << std::endl;
}

void Avida::CoreView::Driver::RaiseFatalException(int exit_code, const cString& in_string)
{
  std::cerr << "Error: " << in_string << "  Exiting..." << std::endl;
  exit(exit_code);
}

void Avida::CoreView::Driver::NotifyComment(const cString& in_string)
{
  std::cout << in_string << std::endl;
}

void Avida::CoreView::Driver::NotifyWarning(const cString& in_string)
{
  std::cout << "Warning: " << in_string << std::endl;
}


void Avida::CoreView::Driver::AttachListener(Listener* listener)
{
  m_listeners.Insert(listener);
  
  if (listener->WantsMap() && !m_map) m_map = new Map(m_world);
}

void Avida::CoreView::Driver::AttachRecorder(Data::RecorderPtr recorder)
{
  m_world->GetDataManager().AttachRecorder(recorder);
}

void Avida::CoreView::Driver::DetachRecorder(Data::RecorderPtr recorder)
{
  m_world->GetDataManager().DetachRecorder(recorder);
}
