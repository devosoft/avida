/*
 *  cCountTracker.h
 *  Avida
 *
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

#ifndef cCountTracker_h
#define cCountTracker_h

class cCountTracker
{
private:
  int cur_count;
  int last_count;
  int total_count;
  
public:
  cCountTracker() { Clear(); }
  ~cCountTracker() { ; }

  inline const int& GetCur() const { return cur_count; }
  inline const int& GetLast() const { return last_count; }
  inline const int& GetTotal() const { return total_count; }

  inline void Inc() { cur_count++; total_count++; }
  inline void Dec() { cur_count--; }
  inline void Next() { last_count = cur_count; cur_count = 0; }
  inline void Clear() { cur_count = last_count = total_count = 0; }
};


#endif
