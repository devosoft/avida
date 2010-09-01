/*
 *  cProbSchedule.h
 *  Avida
 *
 *  Called "prob_schedule.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

class cDeme;
class cMerit;

/**
 * The Probiblistic Schedule has the chance for an item to
 * be scheduled proportional to the merit of that item.
 **/
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

  virtual void Adjust(int item_id, const cMerit& merit, int deme_id = 0);

  int GetNextID();
};

#endif
