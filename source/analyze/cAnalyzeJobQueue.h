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

#include <pthread.h>

class cWorld;


class cAnalyzeJobQueue
{
  friend class cAnalyzeJobWorker;
  
private:
  cWorld* m_world;
  tList<cAnalyzeJob> m_queue;
  pthread_mutex_t m_mutex;

  cAnalyzeJobQueue(const cAnalyzeJobQueue&); // @not_implemented
  cAnalyzeJobQueue& operator=(const cAnalyzeJobQueue&); // @not_implemented
  
public:
  cAnalyzeJobQueue(cWorld* world) : m_world(world) { pthread_mutex_init(&m_mutex, NULL); }
  ~cAnalyzeJobQueue();

  void AddJob(cAnalyzeJob* job) { m_queue.PushRear(job); } // @DMB - warning: this method is NOT thread safe

  void Execute();
};

#endif
