/*
 *  cProbSchedule.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cProbSchedule_h
#define cProbSchedule_h

#ifndef cSchedule_h
#include "cSchedule.h"
#endif
#ifndef cWeightedIndex_h
#include "cWeightedIndex.h"
#endif

/**
 * The Probiblistic Schedule has the chance for an item to
 * be scheduled proportional to the merit of that item.
 **/

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
