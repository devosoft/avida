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
  
public:
  cAvidaContext(cRandom& rng) : m_rng(&rng) { ; }
  cAvidaContext(cRandom* rng) : m_rng(rng) { ; }
  ~cAvidaContext() { ; }
  
  void SetRandom(cRandom& rng) { m_rng = &rng; }  
  void SetRandom(cRandom* rng) { m_rng = rng; }  
  cRandom& GetRandom() { return *m_rng; }
};

#endif
