/*
 *  cProbSchedule.h
 *  Avida
 *
 *  Called "prob_schedule.hh" prior to 12/7/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cProbSchedule_h
#define cProbSchedule_h

#include "apto/rng.h"

#include "cSchedule.h"
#include "cWeightedIndex.h"

class cDeme;
class cMerit;

/**
 * The Probiblistic Schedule has the chance for an item to
 * be scheduled proportional to the merit of that item.
 **/
class cProbSchedule : public cSchedule
{
private:
  Apto::Random* m_rng;
  cWeightedIndex chart;
  
  
  cProbSchedule(const cProbSchedule&); // @not_implemented
  cProbSchedule& operator=(const cProbSchedule&); // @not_implemented

public:
  cProbSchedule(int num_cells, int seed) : cSchedule(num_cells), m_rng(new Apto::RNG::AvidaRNG(seed)), chart(num_cells) { ; }
  ~cProbSchedule() { delete m_rng; }

  virtual void Adjust(int item_id, const cMerit& merit, int deme_id = 0);

  int GetNextID();
};

#endif
