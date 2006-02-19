/*
 *  cAnalyzeJob.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cAnalyzeJob_h
#define cAnalyzeJob_h

class cAnalyzeJob
{
public:
  cAnalyzeJob() { ; }
  virtual ~cAnalyzeJob() { ; }
  
  virtual void Run() = 0;
};

#endif
