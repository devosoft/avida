/*
 *  cResourceHistory.cpp
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

#include "cResourceHistory.h"

#include "cInitFile.h"
#include "cStringList.h"


cResourceHistory::cResourceHistory(const cResourceHistory& _in)
{
  (*this) = _in;
}

cResourceHistory& cResourceHistory::operator=(const cResourceHistory& _in)
{
  Apto::Array<sResourceHistoryEntry> m_entries(_in.m_entries.GetSize());
  for (int k=0; k < _in.m_entries.GetSize(); k++)
    m_entries[k] = _in.m_entries[k];
  return (*this);
}


int cResourceHistory::GetEntryForUpdate(int update, bool exact) const
{
  int entry = -1;
  
  if (exact) {
    for (int i = 0; i < m_entries.GetSize(); i++) {
      if (update == m_entries[i].update) {
        entry = i;
        break;
      }
    }
  } else {
    // Find the update that is closest to the born update, round down
    entry = 0;
    for (; entry < m_entries.GetSize(); entry++) 
      if (m_entries[entry].update > update) break;
      if (entry > 0) entry--;
  }

  return entry;
}


bool cResourceHistory::GetResourceAbundances(int update, GlobalResAmounts& levels, bool exact) const
{
  int entry = GetEntryForUpdate(update, exact);
  if (entry == -1) return false;
  
  levels.Resize(m_entries[entry].values.GetSize());
  for (int i = 0; i < levels.GetSize(); i++) {
    if (entry >= m_entries.GetSize()) levels[i] = 0.0;
    else levels[i] = m_entries[entry].values[i];
  }
  
  return true;
}

void cResourceHistory::AddEntry(int update, const GlobalResAmounts& values)
{
  // Note that this method does not currently validate that 'update' does not already exist as an entry
  // If this happens, incorrect resource levels may be returned upon retreival

  int new_entry = m_entries.GetSize();
  m_entries.Resize(new_entry + 1);
  m_entries[new_entry].update = update;
  m_entries[new_entry].values = values;
}

bool cResourceHistory::LoadFile(const cString& filename, const cString& working_dir)
{
  cInitFile file(filename, working_dir);
  
  if (!file.WasOpened()) {
    return false;
  }
  
  for (int line = 0; line < file.GetNumLines(); line++) {
    cStringList cur_line(file.GetLine(line));
    assert(cur_line.GetSize());
    
    sResourceHistoryEntry entry;
    if (cur_line.GetSize())
      entry.update = cur_line.Pop().AsInt();
    else
      return false;
    
    while (cur_line.GetSize()){
      entry.values.Push(cur_line.Pop().AsDouble());
    }
    
    m_entries[line] = entry;
  }
  
  return true;
}

