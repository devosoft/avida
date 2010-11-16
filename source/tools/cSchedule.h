/*
 *  cSchedule.h
 *  Avida
 *
 *  Called "schedule.hh" prior to 12/7/05.
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

#ifndef cSchedule_h
#define cSchedule_h

class cDeme;
class cMerit;
class cChangeList;

/**
 * This class is the base object to handle time-slicing. All other schedulers
 * are derived from this class.  This is a virtual class.
 *
 **/
class cSchedule
{
protected:
  int item_count;
  cChangeList* m_change_list;
  

  cSchedule(); // @not_implemented
  cSchedule(const cSchedule&); // @not_implemented
  cSchedule& operator=(const cSchedule&); // @not_implemented
  
public:
  cSchedule(int _item_count);
  virtual ~cSchedule();

  virtual bool OK() { return true; }
  virtual void Adjust(int item_id, const cMerit& merit, int deme_id = 0) = 0;
  virtual int GetNextID() = 0;
  virtual double GetStatus(int id) { return 0.0; }
  void SetChangeList(cChangeList *change_list);
  cChangeList *GetChangeList() { return m_change_list; }

  void SetSize(int _item_count);
};

#endif
