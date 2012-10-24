/*
 *  cAnalyzeJobQueue.cc
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006-2011 Michigan State University. All rights reserved.
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

#include "cAnalyzeJobQueue.h"

#include "apto/platform.h"
#include "avida/Avida.h"
#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"

#include "cAnalyzeJobWorker.h"
#include "cWorld.h"


#if APTO_PLATFORM(WINDOWS) && defined(AddJob)
# undef AddJob
#endif

using namespace Avida;


cAnalyzeJobQueue::cAnalyzeJobQueue(cWorld* world)
: m_world(world), m_last_jobid(0), m_jobs(0), m_pending(0), m_workers(Apto::Platform::AvailableCPUs())
{
  const int max_workers = world->GetConfig().MAX_CONCURRENCY.Get();
  if (max_workers > 0 && max_workers < m_workers.GetSize()) m_workers.Resize(max_workers);
  
  m_job_seed_rng = new Apto::RNG::AvidaRNG(world->GetRandom().GetInt(world->GetRandom().MaxSeed()));
  
  if (m_workers.GetSize() > 1) {
    for (int i = 0; i < m_workers.GetSize(); i++) {
      m_workers[i] = new cAnalyzeJobWorker(this);
      m_workers[i]->Start();
    }
  } else {
    m_workers.Resize(0);
  }
}

cAnalyzeJobQueue::~cAnalyzeJobQueue()
{
  const int num_workers = m_workers.GetSize();
  
  m_mutex.Lock();
  
  // Clean out any waiting jobs
  cAnalyzeJob* job;
  while ((job = m_queue.Pop())) delete job;
  
  // Set job count so that all workers receive NULL jobs
  m_jobs = num_workers;
  
  m_mutex.Unlock();
  
  // Signal all workers to check job queue
  m_cond.Broadcast();
  
  for (int i = 0; i < num_workers; i++) {
    m_workers[i]->Join();
    delete m_workers[i];
  }
  
  delete m_job_seed_rng;
}

inline void cAnalyzeJobQueue::queueJob(cAnalyzeJob* job)
{
  if (m_workers.GetSize()) m_queue.PushRear(job);
  else singleThreadedJobExecution(job);
}

void cAnalyzeJobQueue::AddJob(cAnalyzeJob* job)
{
  Apto::MutexAutoLock lock(m_mutex);
  job->SetID(m_last_jobid++);
  queueJob(job);
  m_jobs++;
}

void cAnalyzeJobQueue::AddJobImmediate(cAnalyzeJob* job)
{
  m_mutex.Lock();
  job->SetID(m_last_jobid++);
  queueJob(job);
  m_jobs++;
  m_mutex.Unlock(); // should unlock prior to signaling condition variable
  m_cond.Signal();
}


void cAnalyzeJobQueue::Start()
{
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
    m_world->GetDriver().Feedback().Notify("waking worker threads...");

  m_cond.Broadcast();
}


void cAnalyzeJobQueue::Execute()
{
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
    m_world->GetDriver().Feedback().Notify("waking worker threads...");

  m_cond.Broadcast();
  
  // Wait for term signal
  m_mutex.Lock();
  while (m_jobs > 0 || m_pending > 0) {
    m_term_cond.Wait(m_mutex);
  }
  m_mutex.Unlock();

  if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
    m_world->GetDriver().Feedback().Notify("job queue complete");
}

void cAnalyzeJobQueue::singleThreadedJobExecution(cAnalyzeJob* job)
{
  Apto::RNG::AvidaRNG rng(GetSeedForJob(job->GetID()));
  cAvidaContext ctx(&m_world->GetDriver(), rng);
  job->Run(ctx);
  delete job;
}

