/*
 *  viewer/Driver.cc
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

#include "avida/viewer/Driver.h"

#include "avida/core/Context.h"
#include "avida/data/Manager.h"
#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Manager.h"
#include "avida/systematics/Manager.h"
#include "avida/viewer/Map.h"
#include "avida/viewer/Listener.h"

#include "avida/private/systematics/CladeArbiter.h"

#include "apto/rng.h"

#include "cAvidaContext.h"
#include "cEnvironment.h"
#include "cFile.h"
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


Avida::Viewer::Driver::Driver(cWorld* world, World* new_world)
: Apto::Thread(), m_world(world), m_new_world(new_world), m_pause_state(DRIVER_UNPAUSED), m_started(false), m_done(false)
, m_paused(false), m_pause_at(-2), m_map(NULL)
{
  GlobalObjectManager::Register(this);
}

Avida::Viewer::Driver::~Driver()
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


Avida::Viewer::Driver* Avida::Viewer::Driver::InitWithDirectory(const Apto::String& config_path)
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
  
  World* new_world = new World;
  cWorld* world = cWorld::Initialize(cfg, static_cast<const char*>(config_path), new_world, &feedback);
  
  for (int i = 0; i < feedback.GetNumMessages(); i++) {
    switch (feedback.GetMessageType(i)) {
      case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
      case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
      default: break;
    };
    cerr << feedback.GetMessage(i) << endl;
  }
  
  Apto::String path = Apto::FileSystem::PathAppend(config_path, "update");
  if (Apto::FileSystem::IsFile(path)) {
    // Open name file and read contents
    cFile file;
    file.Open((const char*)path, std::ios::in);
    cString line;
    if (!file.Eof() && file.ReadLine(line)) {
      world->GetStats().SetCurrentUpdate(line.AsInt());
    }
    file.Close();
  }

  
  if (!world) return NULL;

  Systematics::ManagerPtr systematics = Systematics::Manager::Of(new_world);
  systematics->RegisterArbiter(Systematics::ArbiterPtr(new Systematics::CladeArbiter(new_world, "clade")));

  
  return new Avida::Viewer::Driver(world, new_world);
}

int Avida::Viewer::Driver::CurrentUpdate() const
{
  return m_world->GetStats().GetUpdate();
}


int Avida::Viewer::Driver::NumOrganisms() const
{
  return m_world->GetPopulation().GetNumOrganisms();
}


void Avida::Viewer::Driver::InjectGenomeAt(GenomePtr genome, int x, int y, const Apto::String& name)
{
  m_mutex.Lock();
  m_inject_queue.Push(new InjectGenomeInfo(genome, x, y, name));
  m_mutex.Unlock();
}

bool Avida::Viewer::Driver::HasPendingInjects() const
{
  Apto::MutexAutoLock lock(m_mutex);
  return m_inject_queue.GetSize();
}

const Avida::Viewer::Driver::InjectGenomeInfo Avida::Viewer::Driver::PendingInject(int idx) const
{
  
  Apto::MutexAutoLock lock(m_mutex);
  Apto::List<InjectGenomeInfo*, Apto::DL>::ConstIterator it = m_inject_queue.Begin();
  it.Next();
  for (int i = 0; i < idx && it.Get(); i++, it.Next());
  if (it.Get()) return *(*it.Get());
  return InjectGenomeInfo();
}

int Avida::Viewer::Driver::WorldX()
{
  return m_world->GetPopulation().GetWorldX();
}

int Avida::Viewer::Driver::WorldY()
{
  return m_world->GetPopulation().GetWorldY();
}

bool Avida::Viewer::Driver::SetWorldSize(int x, int y)
{
  if (m_started) return false;
  m_world->GetConfig().WORLD_X.Set(x);
  m_world->GetConfig().WORLD_Y.Set(y);
  m_world->GetPopulation().ResizeCellGrid(x, y);
  return true;
}


double Avida::Viewer::Driver::MutationRate()
{
  return m_world->GetConfig().COPY_MUT_PROB.Get();
}

void Avida::Viewer::Driver::SetMutationRate(double rate)
{
  m_world->GetConfig().COPY_MUT_PROB.Set(rate);
  cPopulation& pop = m_world->GetPopulation();
  for (int i = 0; i < pop.GetSize(); i++) pop.GetCell(i).MutationRates().SetCopyMutProb(rate);
}


int Avida::Viewer::Driver::PlacementMode()
{
  return m_world->GetConfig().BIRTH_METHOD.Get();
}

void Avida::Viewer::Driver::SetPlacementMode(int mode)
{
  m_world->GetConfig().BIRTH_METHOD.Set(mode);
}


int Avida::Viewer::Driver::RandomSeed()
{
  return m_world->GetConfig().RANDOM_SEED.Get();
}

void Avida::Viewer::Driver::SetRandomSeed(int seed)
{
  m_world->GetConfig().RANDOM_SEED.Set(seed);
  m_world->GetRandom().ResetSeed(seed);
  
  // When resetting the random seed, the timeslicer also needs to be rebuilt, since it may use the RNG
  // Resizing the cell grid triggers the reconstruction of the timeslicer, so...
  m_world->GetPopulation().ResizeCellGrid(m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get());
}


double Avida::Viewer::Driver::ReactionValue(const Apto::String& name)
{
  return m_world->GetEnvironment().GetReactionValue((const char*)name);
}

void Avida::Viewer::Driver::SetReactionValue(const Apto::String& name, double value)
{
  cAvidaContext ctx(this, m_world->GetRandom());
  m_world->GetEnvironment().SetReactionValue(ctx, (const char*)name, value);
}


double Avida::Viewer::Driver::TestFitnessOfGroup(Avida::Systematics::GroupPtr group)
{
  Apto::RNG::AvidaRNG rng(100);
  cAvidaContext ctx(this, rng);
  return Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, group)->GetFitness();
}

double Avida::Viewer::Driver::TestGestationTimeOfGroup(Avida::Systematics::GroupPtr group)
{
  Apto::RNG::AvidaRNG rng(100);
  cAvidaContext ctx(this, rng);
  return Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, group)->GetGestationTime();
}

double Avida::Viewer::Driver::TestMetabolicRateOfGroup(Avida::Systematics::GroupPtr group)
{
  Apto::RNG::AvidaRNG rng(100);
  cAvidaContext ctx(this, rng);
  return Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, group)->GetMerit();
}

int Avida::Viewer::Driver::TestEnvironmentTriggerCountOfGroup(Avida::Systematics::GroupPtr group, Avida::Environment::ActionTriggerID action_id)
{
  Apto::RNG::AvidaRNG rng(100);
  cAvidaContext ctx(this, rng);
  Avida::Environment::ManagerPtr env = Avida::Environment::Manager::Of(m_new_world);
  return Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, group)->GetTaskCounts()[env->GetActionTrigger(action_id)->TempOrdering()];
}



void Avida::Viewer::Driver::Pause()
{
  m_mutex.Lock();
  m_pause_state = DRIVER_PAUSED;
  m_mutex.Unlock();
  m_pause_cv.Broadcast();
}

void Avida::Viewer::Driver::Resume()
{
  m_mutex.Lock();
  m_pause_state = DRIVER_UNPAUSED;
  m_mutex.Unlock();
  m_pause_cv.Broadcast();
}

void Avida::Viewer::Driver::Finish()
{
  m_mutex.Lock();
  m_done = true;
  m_mutex.Unlock();
  m_pause_cv.Broadcast();
}

void Avida::Viewer::Driver::Sync()
{
  m_mutex.Lock();
  if (m_pause_state == DRIVER_SYNCING) {
    m_pause_state = DRIVER_UNPAUSED;
  }
  m_mutex.Unlock();
  m_pause_cv.Broadcast();
}



void Avida::Viewer::Driver::Abort(AbortCondition condition)
{
  throw condition;
}


void Avida::Viewer::Driver::RegisterCallback(DriverCallback callback)
{
  m_callback = callback;
}

void Avida::Viewer::Driver::Run()
{
  m_callback(THREAD_START);
  
  // Support initial pause
  m_mutex.Lock();
  while (!m_done && m_pause_state == DRIVER_PAUSED) {
    m_paused = true;
    m_pause_cv.Wait(m_mutex);
  }
  m_paused = false;
  m_mutex.Unlock();
  
  if (m_done) return;
  
  m_started = true;
  
  try {
    cPopulation& population = m_world->GetPopulation();
    cStats& stats = m_world->GetStats();
    
    Data::ManagerPtr dm = m_world->GetDataManager();
    
    const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
    const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
    
    void (cPopulation::*ActiveProcessStep)(cAvidaContext& ctx, double step_size, int cell_id) = &cPopulation::ProcessStep;
    if (m_world->GetConfig().SPECULATIVE.Get() &&
        m_world->GetConfig().THREAD_SLICING_METHOD.Get() != 1 && !m_world->GetConfig().IMPLICIT_REPRO_END.Get()) {
      ActiveProcessStep = &cPopulation::ProcessStepSpeculative;
    }
    
    cAvidaContext ctx(this, m_world->GetRandom());
    Avida::Context new_ctx(this, &m_world->GetRandom());
    
    m_mutex.Lock();

    // Handle initial inject queue requests
    if (m_inject_queue.GetSize()) {
      Systematics::RoleClassificationHints hints;
      while (m_inject_queue.GetSize()) {
        InjectGenomeInfo* info = m_inject_queue.Pop();
        int cell_id = info->x + population.GetWorldX() * info->y;
        hints["clade"]["name"] = info->name;
        population.InjectGenome(cell_id, Systematics::Source(Systematics::DIVISION, "", true), *info->genome, ctx, 0, true, &hints);
        delete info;
      }
    }

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
        for (int i = 0; i < UD_size; i++) (population.*ActiveProcessStep)(ctx, step_size, population.ScheduleOrganism());
  //    }
      
      
      // end of update stats...
      population.ProcessPostUpdate(ctx);
      
      
      m_mutex.Lock();
      {
        // Handle inject queue requests
        if (m_inject_queue.GetSize()) {
          Systematics::RoleClassificationHints hints;
          while (m_inject_queue.GetSize()) {
            InjectGenomeInfo* info = m_inject_queue.Pop();
            int cell_id = info->x + population.GetWorldX() * info->y;
            hints["clade"]["name"] = info->name;
            population.InjectGenome(cell_id, Systematics::Source(Systematics::DIVISION, "", true), *info->genome, ctx, 0, true, &hints);
            delete info;
          }
        }
        
        // Listeners can be attached and detached asynchronously, must be locked while working with them
        if (m_map) m_map->UpdateMaps(population);
        for (Apto::Set<Listener*>::Iterator it = m_listeners.Begin(); it.Next();) {
          if ((*it.Get())->WantsMap()) {
            (*it.Get())->NotifyMap(m_map);
          }
          if ((*it.Get())->WantsUpdate()) (*it.Get())->NotifyUpdate(stats.GetUpdate());
        }
      }
      m_mutex.Unlock();
      
      
      // Do Point Mutations
      if (point_mut_prob > 0 ) {
        for (int i = 0; i < population.GetSize(); i++) {
          if (population.GetCell(i).IsOccupied()) {
            population.GetCell(i).GetOrganism()->GetHardware().PointMutate(ctx, point_mut_prob);
          }
        }
      }
      
      m_new_world->PerformUpdate(new_ctx, stats.GetUpdate());
      
      // Exit conditons...
      m_mutex.Lock();
      if (population.GetNumOrganisms() == 0 || stats.GetUpdate() == m_pause_at) {
        m_pause_state = DRIVER_PAUSED;
        m_pause_at = -2;
      }
      if (m_pause_state != DRIVER_PAUSED && m_sync_mode) {
        m_pause_state = DRIVER_SYNCING;
      }
      m_mutex.Unlock();
      
      for (Apto::Set<Listener*>::Iterator it = m_listeners.Begin(); it.Next();) {
        if ((*it.Get())->WantsState()) (*it.Get())->NotifyState(m_pause_state);
      }
      
      m_mutex.Lock();
      while (!m_done && m_pause_state != DRIVER_UNPAUSED) {
        m_paused = true;
        m_pause_cv.Wait(m_mutex);
      }
      m_paused = false;
    }
    m_mutex.Unlock();
  } catch (Avida::AbortCondition condition) {
    cerr << "abort: " << condition << endl;
  }
  m_callback(THREAD_END);
}


void Avida::Viewer::Driver::StdIOFeedback::Error(const char* fmt, ...)
{
  printf("error: ");
  va_list args;
  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);
  printf("\n");
}

void Avida::Viewer::Driver::StdIOFeedback::Warning(const char* fmt, ...)
{
  printf("warning: ");
  va_list args;
  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);
  printf("\n");
}

void Avida::Viewer::Driver::StdIOFeedback::Notify(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);
  printf("\n");
}


void Avida::Viewer::Driver::AttachListener(Listener* listener)
{
  m_mutex.Lock();
  m_listeners.Insert(listener);
  
  if (listener->WantsMap() && !m_map) m_map = new Map(m_world);
  m_mutex.Unlock();
}

void Avida::Viewer::Driver::DetachListener(Listener* listener)
{
  m_mutex.Lock();
  m_listeners.Remove(listener);
  m_mutex.Unlock();
}

void Avida::Viewer::Driver::AttachRecorder(Data::RecorderPtr recorder, bool concurrent_update)
{
  m_world->GetDataManager()->AttachRecorder(recorder, concurrent_update);
}

void Avida::Viewer::Driver::DetachRecorder(Data::RecorderPtr recorder)
{
  m_world->GetDataManager()->DetachRecorder(recorder);
}
