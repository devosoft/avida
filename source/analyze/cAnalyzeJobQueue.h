/*
 *  cAnalyzeJobQueue.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cAnalyzeJobQueue_h
#define cAnalyzeJobQueue_h

#ifndef cAnalyzeJob
#include "cAnalyzeJob.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif
#ifndef cRandom_h
#include "cRandom.h"
#endif

#include <pthread.h>

class cWorld;


const int MT_RANDOM_POOL_SIZE = 16;
const int MT_RANDOM_INDEX_MASK = 0xF;

class cAnalyzeJobQueue
{
  friend class cAnalyzeJobWorker;
  
private:
  cWorld* m_world;
  tList<cAnalyzeJob> m_queue;
  int m_last_jobid;
  cRandomMT* m_rng_pool[MT_RANDOM_POOL_SIZE];
  pthread_mutex_t m_mutex;
  

  cAnalyzeJobQueue(const cAnalyzeJobQueue&); // @not_implemented
  cAnalyzeJobQueue& operator=(const cAnalyzeJobQueue&); // @not_implemented
  
public:
    cAnalyzeJobQueue(cWorld* world);
  ~cAnalyzeJobQueue();

  void AddJob(cAnalyzeJob* job) { job->SetID(m_last_jobid++); m_queue.PushRear(job); } // @DMB - Warning: NOT thread safe

  void Execute();
  
  cRandom* GetRandom(int jobid) { return m_rng_pool[jobid & MT_RANDOM_INDEX_MASK]; } 
};

#endif
