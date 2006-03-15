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

class cAvidaContext
{
private:
  int m_rng_id;
  
public:
  cAvidaContext(int rng_id) : m_rng_id(rng_id) { ; }
  ~cAvidaContext() { ; }
  
  int GetRandomID() { return m_rng_id; }
};

#endif
