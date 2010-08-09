/*
 *  Cintegratedschedule.h
 *  Avida
 *
 *  Called "integrated_schedule.hh" prior to 12/7/05.
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

#ifndef cIntegratedSchedule_h
#define cIntegratedSchedule_h

#ifndef cSchedule_h
#include "cSchedule.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cDeme;
class cIntegratedScheduleNode;
class cMerit;

/**
* The cIntegratedSchedule method relies on breaking up all merits into
 * sums of powers of 2 (i.e. using the binary representation of the merit).
 * All items with merits in the highest power of two will get the most
 * time, and subsequent merit components will have time divided,
 * continuing recursively.  The simplest way of doing this while maximizing
 * evenness of distribution of time slices is to simply alternate executing
 * the best, and everything else (where in everything else we again alternate
 * with the best of this sub-list recursively).
 **/
class cIntegratedSchedule : public cSchedule
{
private:
  tArray<cIntegratedScheduleNode *> node_array;
  int num_active_nodes;
  cMerit * merit_chart;

  void InsertNode(int node_id);
  void RemoveNode(int node_id);
  void ResizeNodes(int new_size);

  cIntegratedSchedule(); // @not_implemented
  cIntegratedSchedule(const cIntegratedSchedule&); // @not_implemented
  cIntegratedSchedule& operator=(const cIntegratedSchedule&); // @not_implemented

public:
  cIntegratedSchedule(int _item_count);
  ~cIntegratedSchedule();

  virtual void Adjust(int item_id, const cMerit& merit, int deme_id);
  
  int GetNextID();
  double GetStatus(int id);

  bool OK();
};

#endif
