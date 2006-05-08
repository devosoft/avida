/*
 *  cAnalyzeJobWorker.h
 *  Avida
 *
 *  Created by David on 2/18/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cAnalyzeJobWorker_h
#define cAnalyzeJobWorker_h

#ifndef cThread_h
#include "cThread.h"
#endif

class cAnalyzeJobQueue;


class cAnalyzeJobWorker : public cThread
{
private:
  cAnalyzeJobQueue* m_queue;
  
  void Run();

public:
  cAnalyzeJobWorker(cAnalyzeJobQueue* queue) : m_queue(queue) { ; }  
};


#ifdef ENABLE_UNIT_TESTS
namespace nAnalyzeJobWorker {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
