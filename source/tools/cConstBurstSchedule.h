/*
 *  cConstBurstSchedule.h
 *  Avida
 *
 *  Created by David on 3/23/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#ifndef cConstBurstSchedule_h
#define cConstBurstSchedule_h

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
class cConstBurstSchedule : public cSchedule
  {
  private:
    int m_burst_sz;
    tArray<bool> m_active;

    int m_cur_id;
    int m_burst_state;
    
    
    cConstBurstSchedule(); // @not_implemented
    cConstBurstSchedule(const cConstBurstSchedule&); // @not_implemented
    cConstBurstSchedule& operator=(const cConstBurstSchedule&); // @not_implemented
    
    
  public:
    cConstBurstSchedule(int item_sz, int burst_sz)
      : cSchedule(item_sz), m_burst_sz(burst_sz), m_active(item_sz), m_cur_id(0), m_burst_state(0)
    {
      m_active.SetAll(false);
    }
    ~cConstBurstSchedule() { ; }
    
    bool OK();
    void Adjust(int item_id, const cMerit& merit, int deme_id = 0);
    int GetNextID();
  };

#endif
