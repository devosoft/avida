/*
 *  cAnalyzeJobWorker.cc
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
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
