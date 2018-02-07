/*
 *  cResourceHistory.h
 *  Avida
 *
 *  Created by David Bryson on 10/27/08.
 *  Copyright 2008-2011 Michigan State University. All rights reserved.
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

#include "avida/core/Types.h"
#include "resources/Types.h"


class cAvidaContext;
class cString;

class cResourceHistory
  {
  protected:
    struct sResourceHistoryEntry {
      int update;
      ResAmounts values;
    };
    
  protected:
    
    Apto::Array<sResourceHistoryEntry> m_entries;
    int GetEntryForUpdate(int update, bool exact) const;
    
    
    
    
  public:
    cResourceHistory()
    { ; }
    
    cResourceHistory(const cResourceHistory&);
    cResourceHistory& operator=(const cResourceHistory&);
    
    bool Empty() const { return m_entries.GetSize() == 0;}
    
    bool GetResourceAbundances(int update, ResAmounts& levels, bool exact = false) const;
    void AddEntry(int update, const ResAmounts& values);
    
    bool LoadFile(const cString& filename, const cString& working_dir);
  };

#endif
