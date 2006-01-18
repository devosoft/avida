/*
 *  cAvidaTriggers.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAvidaTriggers_h
#define cAvidaTriggers_h

#ifndef cEventTriggers_h
#include "cEventTriggers.h"
#endif
#ifndef cStats_h
#include "cStats.h"
#endif

/**
 * A small class that determines the correct triggers for the event list.
 * Needed in order to separate the event business from the rest.
 */
class cAvidaTriggers : public cEventTriggers
{
private:
  cStats& m_stats;

  
  cAvidaTriggers(); // @not_implemented
  cAvidaTriggers(const cAvidaTriggers&); // @not_implemented
  cAvidaTriggers& operator=(const cAvidaTriggers&); // @not_implemented

public:
  cAvidaTriggers(cStats& stats) : m_stats(stats) { ; }

  double GetUpdate() const { return (double) m_stats.GetUpdate(); }
  double GetGeneration() const { return m_stats.SumGeneration().Average(); }
};

#endif
