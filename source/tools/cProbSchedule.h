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

#ifndef cRandom_h
#include "cRandom.h"
#endif
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

class cProbSchedule : public cSchedule
{
private:
  cRandom m_rng;
  cWeightedIndex chart;
  
  
  cProbSchedule(const cProbSchedule&); // @not_implemented
  cProbSchedule& operator=(const cProbSchedule&); // @not_implemented

public:
  cProbSchedule(int num_cells, int seed) : cSchedule(num_cells), m_rng(seed), chart(num_cells) { ; }
  ~cProbSchedule() { ; }

  void Adjust(int item_id, const cMerit& merit);
  int GetNextID();
};


#ifdef ENABLE_UNIT_TESTS
namespace nProbSchedule {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
