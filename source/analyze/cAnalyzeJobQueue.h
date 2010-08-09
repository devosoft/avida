/*
 *  cAnalyzeJobQueue.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cAnalyzeJobQueue_h
#define cAnalyzeJobQueue_h

#ifndef cAnalyzeJob
#include "cAnalyzeJob.h"
#endif
#ifndef cConditionVariable_h
#include "cConditionVariable.h"
#endif
#ifndef cMutex_h
#include "cMutex.h"
#endif
#ifndef cRandom_h
#include "cRandom.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cAnalyzeJobWorker;
class cWorld;


const int MT_RANDOM_POOL_SIZE = 128;
const int MT_RANDOM_INDEX_MASK = 0x7F;


class cAnalyzeJobQueue
{
  friend class cAnalyzeJobWorker;
  
private:
  cWorld* m_world;
  tList<cAnalyzeJob> m_queue;
  int m_last_jobid;
  cRandomMT* m_rng_pool[MT_RANDOM_POOL_SIZE];
  cMutex m_mutex;
  cConditionVariable m_cond;
  cConditionVariable m_term_cond;
  
  volatile int m_jobs;      // count of waiting jobs, used in condition variable constructs
  volatile int m_pending;   // count of currently executing jobs
  
  tArray<cAnalyzeJobWorker*> m_workers;


  void singleThreadedJobExecution(cAnalyzeJob* job);
  inline void queueJob(cAnalyzeJob* job);

  
  cAnalyzeJobQueue(); // @not_implemented
  cAnalyzeJobQueue(const cAnalyzeJobQueue&); // @not_implemented
  cAnalyzeJobQueue& operator=(const cAnalyzeJobQueue&); // @not_implemented
  

public:
  cAnalyzeJobQueue(cWorld* world);
  ~cAnalyzeJobQueue();

  void AddJob(cAnalyzeJob* job);
  void AddJobImmediate(cAnalyzeJob* job);

  void Start();
  void Execute();
  
  cRandom* GetRandom(int jobid) { return m_rng_pool[jobid & MT_RANDOM_INDEX_MASK]; } 
};

#endif
