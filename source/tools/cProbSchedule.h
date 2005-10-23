//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef PROB_SCHEDULE_HH
#define PROB_SCHEDULE_HH

#ifndef SCHEDULE_HH
#include "cSchedule.h"
#endif
#ifndef WEIGHTED_INDEX_HH
#include "cWeightedIndex.h"
#endif

/**
 * The Probiblistic Schedule has the chance for an item to
 * be scheduled proportional to the merit of that item.
 **/

class cWeightedIndex; // aggregate
class cMerit;
class cWorld;

class cProbSchedule : public cSchedule {
private:
  cWorld* m_world;
  cWeightedIndex chart;
public:
  cProbSchedule(cWorld* world, int num_cells);
  ~cProbSchedule();

  void Adjust(int item_id, const cMerit & merit);
  int GetNextID();
};

#endif
