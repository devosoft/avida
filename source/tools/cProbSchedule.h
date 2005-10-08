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
#include "weighted_index.hh"
#endif

/**
 * The Probiblistic Schedule has the chance for an item to
 * be scheduled proportional to the merit of that item.
 **/

class cWeightedIndex; // aggregate
class cMerit;

class cProbSchedule : public cSchedule {
private:
  cWeightedIndex chart;
public:
  cProbSchedule(int num_cells);
  ~cProbSchedule();

  void Adjust(int item_id, const cMerit & merit);
  int GetNextID();
};

#endif
