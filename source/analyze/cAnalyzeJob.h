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

class cAvidaContext;

class cAnalyzeJob
{
private:
  int m_id;
  
public:
  cAnalyzeJob() : m_id(0) { ; }
  virtual ~cAnalyzeJob() { ; }
  
  void SetID(int newid) { m_id = newid; }
  int GetID() { return m_id; }
  
  virtual void Run(cAvidaContext& ctx) = 0;
};


#ifdef ENABLE_UNIT_TESTS
namespace nAnalyzeJob {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
