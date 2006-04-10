/*
 *  cAvidaContext.h
 *  Avida
 *
 *  Created by David on 3/13/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cAvidaContext_h
#define cAvidaContext_h

class cRandom;

class cAvidaContext
{
private:
  cRandom* m_rng;
  bool m_analyze;
  
public:
  cAvidaContext(cRandom& rng) : m_rng(&rng), m_analyze(false) { ; }
  cAvidaContext(cRandom* rng) : m_rng(rng), m_analyze(false) { ; }
  ~cAvidaContext() { ; }
  
  void SetRandom(cRandom& rng) { m_rng = &rng; }  
  void SetRandom(cRandom* rng) { m_rng = rng; }  
  cRandom& GetRandom() { return *m_rng; }
  
  void SetAnalyzeMode() { m_analyze = true; }
  void ClearAnalyzeMode() { m_analyze = false; }
  bool GetAnalyzeMode() { return m_analyze; }
};

#endif
