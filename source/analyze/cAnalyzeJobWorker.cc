/*
 *  cAnalyzeJobWorker.cc
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#include "cAnalyzeJobWorker.h"

#include "cAnalyzeJobQueue.h"
#include "cAvidaContext.h"
#include "cWorld.h"


void cAnalyzeJobWorker::Run()
{
  Apto::RNG::AvidaRNG rng;
  cAvidaContext ctx(&m_queue->m_world->GetDriver(), rng);
  ctx.SetAnalyzeMode();
  
  cAnalyzeJob* job = NULL;
  
  while (1) {
    m_queue->m_mutex.Lock();
    while (m_queue->m_jobs == 0) {
      m_queue->m_cond.Wait(m_queue->m_mutex);
    }
    job = m_queue->m_queue.Pop();
    m_queue->m_jobs--;
    m_queue->m_pending++; 
    m_queue->m_mutex.Unlock();
    
    if (job) {
      // Set RNG from the waiting pool and execute the job
      rng.ResetSeed(m_queue->GetSeedForJob(job->GetID()));
      job->Run(ctx);
      delete job;
      m_queue->m_mutex.Lock();
      int pending = --m_queue->m_pending;
      m_queue->m_mutex.Unlock();
      if (!pending) m_queue->m_term_cond.Signal();
    } else {
      // Terminate worker on NULL job receipt
      m_queue->m_mutex.Lock();
      int pending = --m_queue->m_pending;
      m_queue->m_mutex.Unlock();
      if (!pending) m_queue->m_term_cond.Signal();
      break;
    }
  }
}
