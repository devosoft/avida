/*
 *  cResourceHistory.h
 *  Avida
 *
 *  Created by David Bryson on 10/27/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#ifndef cResourceHistory_h
#define cResourceHistory_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cResourceCount;
class cString;
template<class T> class tArray;


class cResourceHistory
{
private:
  struct sResourceHistoryEntry {
    int update;
    tArray<double> values;
  };
  
  tArray<sResourceHistoryEntry> m_entries;
  
  
  int getEntryForUpdate(int update, bool exact) const;
  
  
  cResourceHistory(const cResourceHistory&); // @not_implemented
  cResourceHistory& operator=(const cResourceHistory&); // @not_implemented
  
public:
  cResourceHistory() { ; }
  
  bool GetResourceCountForUpdate(int update, cResourceCount& rc, bool exact = false) const;
  bool GetResourceLevelsForUpdate(int update, tArray<double>& levels, bool exact = false) const;
  void AddEntry(int update, const tArray<double>& values);
  
  bool LoadFile(const cString& filename, const cString& working_dir);
};

#endif
