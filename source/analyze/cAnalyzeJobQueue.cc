/*
 *  cAnalyzeJobQueue.cc
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cAnalyzeJobQueue.h"

#include "cAnalyzeJobWorker.h"
#include "cWorld.h"

cAnalyzeJobQueue::~cAnalyzeJobQueue()
{
  cAnalyzeJob* job;
  while (job = m_queue.Pop()) delete job;
}

void cAnalyzeJobQueue::Execute()
{
  const int num_workers = m_world->GetConfig().ANALYZE_MT_CONCURRENCY.Get();
  
  cAnalyzeJobWorker* workers[num_workers];
  
  for (int i = 0; i < num_workers; i++) {
    workers[i] = new cAnalyzeJobWorker(this);
    workers[i]->Start();
  }

  for (int i = 0; i < num_workers; i++) workers[i]->Join();
}
