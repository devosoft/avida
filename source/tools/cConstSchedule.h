/*
 *  cConstSchedule.h
 *  Avida
 *
 *  Called "const_schedule.hh" prior to 12/2/05.
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

#ifndef cConstSchedule_h
#define cConstSchedule_h

#ifndef cSchedule_h
#include "cSchedule.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cMerit;

/**
 * This class rotates between all items to schedule, giving each equal time.
 **/
class cConstSchedule : public cSchedule
{
private:
  int last_id;
  tArray<bool> is_active;
  
  cConstSchedule(); // @not_implemented
  cConstSchedule(const cConstSchedule&); // @not_implemented
  cConstSchedule& operator=(const cConstSchedule&); // @not_implemented

public:
  cConstSchedule(int _item_count)
    : cSchedule(_item_count), last_id(0), is_active(_item_count)
  {
    is_active.SetAll(false);
  }
  ~cConstSchedule() { ; }

  bool OK();

  virtual void Adjust(int item_id, const cMerit& merit, int deme_id = 0);

  int GetNextID();
};


#endif
