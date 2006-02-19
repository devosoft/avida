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
  void (T::*JobTask)() const;

public:
  tAnalyzeJob(T* target, void (T::*funJ)() const) : cAnalyzeJob(), m_target(target), JobTask(funJ) { ; }
  
  void Run() { (m_target->*JobTask)(); }
};

#endif
