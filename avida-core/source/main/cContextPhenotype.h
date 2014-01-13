/*
 *  cHardwareBase.h
 *  Avida
 *
 *  Called "hardware_base.hh" prior to 11/17/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef cContextPhenotype_h
#define cContextPhenotype_h

#include "avida/core/Types.h"

#include <cassert>
#include <climits>
#include <iostream>


class cContextPhenotype
{
public:
  cContextPhenotype() : m_number_tasks(0), m_number_reactions(0) { };
  double m_cur_merit;
  Apto::Array<int> m_cur_task_count;
  Apto::Array<int> m_cur_reaction_count;
  int m_number_tasks;
  int m_number_reactions;

  void AddTaskCounts(int count, Apto::Array<int>& cur_task_count);
  Apto::Array<int>& GetTaskCounts() { return m_cur_task_count; }
  void AddReactionCounts(int count, Apto::Array<int>& cur_task_count);
  Apto::Array<int>& GetReactionCounts() { return m_cur_reaction_count; }

};


#endif
