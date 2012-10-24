/*
 *  cAnalyzeJobQueue.h
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

#ifndef cAnalyzeJobQueue_h
#define cAnalyzeJobQueue_h

#include "apto/core.h"
#include "apto/platform.h"

#include "cAnalyzeJob.h"
#include "tList.h"

class cAnalyzeJobWorker;
class cWorld;

#if APTO_PLATFORM(WINDOWS) && defined(AddJob)
# undef AddJob
#endif


const int MT_RANDOM_INDEX_MASK = 0x7F;


class cAnalyzeJobQueue
{
  friend class cAnalyzeJobWorker;
  
private:
  cWorld* m_world;
  tList<cAnalyzeJob> m_queue;
  int m_last_jobid;
  Apto::Random* m_job_seed_rng;
  Apto::Mutex m_mutex;
  Apto::ConditionVariable m_cond;
  Apto::ConditionVariable m_term_cond;
  
  volatile int m_jobs;      // count of waiting jobs, used in condition variable constructs
  volatile int m_pending;   // count of currently executing jobs
  
  Apto::Array<cAnalyzeJobWorker*> m_workers;


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
  
  int GetSeedForJob(int jobid) { Apto::MutexAutoLock lock(m_mutex); return m_job_seed_rng->GetInt(m_job_seed_rng->MaxSeed()); }
};

#endif
