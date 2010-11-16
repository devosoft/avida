/*
 *  cBirthMateSelectHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#include "cBirthMateSelectHandler.h"

#include "cBirthChamber.h"
#include "cOrganism.h"


cBirthMateSelectHandler::~cBirthMateSelectHandler()
{
  for (int i = 0; i < m_entries.GetSize(); i++) {
    m_bc->ClearEntry(m_entries[i]);
  }
  m_bc->ClearEntry(m_non_ms_entry);
}


cBirthEntry* cBirthMateSelectHandler::SelectOffspring(cAvidaContext& ctx, const cGenome& offspring, cOrganism* parent)
{
  int mate_id = parent->GetPhenotype().MateSelectID();
  
  if (mate_id >= 0) {
    // If this is a new mate id, increase the array size accordingly
    if (m_entries.GetSize() <= mate_id) m_entries.Resize(mate_id + 1);
    
    // Determine if we have an offspring of this length waiting already...
    if (!m_bc->ValidBirthEntry(m_entries[mate_id])) {
      m_bc->StoreAsEntry(offspring, parent, m_entries[mate_id]);
      return NULL; 				
    }
    return &(m_entries[mate_id]);
  }
  
  // Non-mate select offspring handled globally
  
  if (!m_bc->ValidBirthEntry(m_non_ms_entry)) {
    m_bc->StoreAsEntry(offspring, parent, m_non_ms_entry);
    return NULL;
  }
  
  return &m_non_ms_entry;
}
