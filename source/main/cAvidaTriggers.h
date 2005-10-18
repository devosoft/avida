//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_TRIGGERS_HH
#define AVIDA_TRIGGERS_HH

#ifndef EVENT_TRIGGERS_HH
#include "cEventTriggers.h"
#endif
#ifndef STATS_HH
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
