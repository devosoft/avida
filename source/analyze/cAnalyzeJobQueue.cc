/*
 *  cAnalyzeJobQueue.cc
 *  Avida
 *
 *  Created by David on 2/18/06.
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

#include "cAnalyzeJobQueue.h"

#include "cAnalyzeJobWorker.h"
#include "cWorld.h"
#include "cWorldDriver.h"


#include "defs.h"


cAnalyzeJobQueue::cAnalyzeJobQueue(cWorld* world)
: m_world(world), m_last_jobid(0), m_jobs(0), m_pending(0), m_workers(world->GetConfig().MT_CONCURRENCY.Get())
{
  for (int i = 0; i < MT_RANDOM_POOL_SIZE; i++) {
    m_rng_pool[i] = new cRandomMT(world->GetRandom().GetInt(0x7FFFFFFF));
  }
  
  pthread_mutex_init(&m_mutex, NULL);
  pthread_cond_init(&m_cond, NULL);
  pthread_cond_init(&m_term_cond, NULL);

  for (int i = 0; i < m_workers.GetSize(); i++) {
    m_workers[i] = new cAnalyzeJobWorker(this);
    m_workers[i]->Start();
  }
}

cAnalyzeJobQueue::~cAnalyzeJobQueue()
{
  const int num_workers = m_workers.GetSize();
  
  pthread_mutex_lock(&m_mutex);
  
  // Clean out any waiting jobs
  cAnalyzeJob* job;
  while ((job = m_queue.Pop())) delete job;
  
  // Set job count so that all workers receive NULL jobs
  m_jobs = num_workers;
  
  pthread_mutex_unlock(&m_mutex);
  
  // Signal all workers to check job queue
  pthread_cond_broadcast(&m_cond);
  
  for (int i = 0; i < num_workers; i++) {
    m_workers[i]->Join();
    delete m_workers[i];
  }
  
  pthread_mutex_destroy(&m_mutex);
  pthread_cond_destroy(&m_cond);
}

void cAnalyzeJobQueue::AddJob(cAnalyzeJob* job)
{
  pthread_mutex_lock(&m_mutex);
  job->SetID(m_last_jobid++);
  m_queue.PushRear(job);
  m_jobs++;
  pthread_mutex_unlock(&m_mutex);
}

void cAnalyzeJobQueue::AddJobImmediate(cAnalyzeJob* job)
{
  pthread_mutex_lock(&m_mutex);
  job->SetID(m_last_jobid++);
  m_queue.PushRear(job);
  m_jobs++;
  pthread_mutex_unlock(&m_mutex);
  pthread_cond_signal(&m_cond);
}


void cAnalyzeJobQueue::Start()
{
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
    m_world->GetDriver().NotifyComment("waking worker threads...");

  pthread_cond_broadcast(&m_cond);
}


void cAnalyzeJobQueue::Execute()
{
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
    m_world->GetDriver().NotifyComment("waking worker threads...");

  pthread_cond_broadcast(&m_cond);
  
  // Wait for term signal
  pthread_mutex_lock(&m_mutex);
  while (m_jobs > 0 || m_pending > 0) {
    pthread_cond_wait(&m_term_cond, &m_mutex);
  }
  pthread_mutex_unlock(&m_mutex);

  if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
    m_world->GetDriver().NotifyComment("job queue complete");
}
