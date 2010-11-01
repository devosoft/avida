/*
 *  cResourceHistory.cpp
 *  Avida
 *
 *  Created by David Bryson on 10/27/08.
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

#include "cResourceHistory.h"

#include "cInitFile.h"
#include "cResourceCount.h"
#include "cStringList.h"


int cResourceHistory::getEntryForUpdate(int update, bool exact) const
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
    for (; entry < m_entries.GetSize(); entry++) if (m_entries[entry].update > update) break;
    if (entry > 0) entry--;
  }

  return entry;
}

bool cResourceHistory::GetResourceCountForUpdate(int update, cResourceCount& rc, bool exact) const
{
  int entry = getEntryForUpdate(update, exact);
  if (entry == -1) return false;
      
  for (int i = 0; i < rc.GetSize(); i++) {
    if (entry >= m_entries.GetSize() || i >= m_entries[entry].values.GetSize()) {
			rc.Set(i, 0.0);
		}
    else {
			rc.Set(i, m_entries[entry].values[i]);
		}
  }
  
  return true;
}

bool cResourceHistory::GetResourceLevelsForUpdate(int update, tArray<double>& levels, bool exact) const
{
  int entry = getEntryForUpdate(update, exact);
  if (entry == -1) return false;
  
  levels.Resize(m_entries[entry].values.GetSize());
  for (int i = 0; i < levels.GetSize(); i++) {
    if (entry >= m_entries.GetSize()) levels[i] = 0.0;
    else levels[i] = m_entries[entry].values[i];
  }
  
  return true;
}

void cResourceHistory::AddEntry(int update, const tArray<double>& values)
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
//    tConstListIterator<cString> err_it(file.GetErrors());
//    const cString* errstr = NULL;
//    while ((errstr = err_it.Next())) m_world->GetDriver().RaiseException(*errstr);
//    m_world->GetDriver().RaiseFatalException(1, cString("Could not open instruction set '") + filename + "'.");
    return false;
  }
  
  m_entries.Resize(file.GetNumLines());
  for (int line = 0; line < file.GetNumLines(); line++) {
    cStringList cur_line(file.GetLine(line));
    assert(cur_line.GetSize());
    
    m_entries[line].update = cur_line.Pop().AsInt();
    
    int num_values = cur_line.GetSize();
    m_entries[line].values.Resize(num_values);
    for (int i = 0; i < num_values; i++) m_entries[line].values[i] = cur_line.Pop().AsDouble();
  }
  
  return true;
}

