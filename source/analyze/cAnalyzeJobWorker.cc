/*
 *  cAnalyzeJobWorker.cc
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

#include "cAnalyzeJobWorker.h"

#include "cAnalyzeJobQueue.h"
#include "cAvidaContext.h"


void cAnalyzeJobWorker::Run()
{
  cAvidaContext ctx(NULL);
  ctx.SetAnalyzeMode();
  
  cAnalyzeJob* job = NULL;
  
  while (1) {
    pthread_mutex_lock(&m_queue->m_mutex);
    while (m_queue->m_jobs == 0) {
      pthread_cond_wait(&m_queue->m_cond, &m_queue->m_mutex);
    }
    job = m_queue->m_queue.Pop();
    m_queue->m_jobs--;
    m_queue->m_pending++; 
    pthread_mutex_unlock(&m_queue->m_mutex);
    
    if (job) {
      // Set RNG from the waiting pool and execute the job
      ctx.SetRandom(m_queue->GetRandom(job->GetID()));
      job->Run(ctx);
      delete job;
      pthread_mutex_lock(&m_queue->m_mutex);
      int pending = --m_queue->m_pending;
      pthread_mutex_unlock(&m_queue->m_mutex);
      if (!pending) pthread_cond_signal(&m_queue->m_term_cond);
    } else {
      // Terminate worker on NULL job receipt
      pthread_mutex_lock(&m_queue->m_mutex);
      int pending = --m_queue->m_pending;
      pthread_mutex_unlock(&m_queue->m_mutex);
      if (!pending) pthread_cond_signal(&m_queue->m_term_cond);
      break;
    }
  }
  
  pthread_exit(NULL);
}
