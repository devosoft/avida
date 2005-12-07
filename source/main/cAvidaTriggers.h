/*
 *  cAvidaTriggers.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
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
class cAvidaTriggers : public cEventTriggers {
private:
  cStats & stats;
private:
  // disabled copy constructor.
  cAvidaTriggers(const cAvidaTriggers &);
public:
  cAvidaTriggers(cStats & _stats) : stats(_stats) { ; }
  double GetUpdate() const { return (double) stats.GetUpdate(); }
  double GetGeneration() const { return stats.SumGeneration().Average(); }
};

#endif
