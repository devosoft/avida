/*
 *  tAnalyzeJob.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef tAnalyzeJob_h
#define tAnalyzeJob_h

#ifndef cAnalyzeJob_h
#include "cAnalyzeJob.h"
#endif

template <class T> class tAnalyzeJob : public cAnalyzeJob
{
protected:
  T* m_target;
  void (T::*JobTask)(cAvidaContext&);

public:
  tAnalyzeJob(T* target, void (T::*funJ)(cAvidaContext&)) : cAnalyzeJob(), m_target(target), JobTask(funJ) { ; }
  
  void Run(cAvidaContext& ctx)
  {
    (m_target->*JobTask)(ctx);
  }
};

#endif
