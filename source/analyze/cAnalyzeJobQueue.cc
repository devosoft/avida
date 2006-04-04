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

#include <iostream>
using namespace std;

cAnalyzeJobQueue::cAnalyzeJobQueue(cWorld* world) : m_world(world), m_last_jobid(0)
{
  for (int i = 0; i < MT_RANDOM_POOL_SIZE; i++) {
    m_rng_pool[i] = new cRandomMT(world->GetRandom().GetInt(0x7FFFFFFF));
  }
  
  pthread_mutex_init(&m_mutex, NULL);
}

cAnalyzeJobQueue::~cAnalyzeJobQueue()
{
  cAnalyzeJob* job;
  while (job = m_queue.Pop()) delete job;
  pthread_mutex_destroy(&m_mutex);
}

void cAnalyzeJobQueue::Execute()
{
  const int num_workers = 1; //m_world->GetConfig().ANALYZE_MT_CONCURRENCY.Get();
  
  cAnalyzeJobWorker* workers[num_workers];
  
  cout << "Going Multithreaded..." << endl;
  
  for (int i = 0; i < num_workers; i++) {
    workers[i] = new cAnalyzeJobWorker(this);
    workers[i]->Start();
  }

  for (int i = 0; i < num_workers; i++) workers[i]->Join();
}
