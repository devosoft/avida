/*
 *  cBirthMateSelectHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#include "cBirthMateSelectHandler.h"

#include "cBirthChamber.h"
#include "cOrganism.h"


cBirthMateSelectHandler::~cBirthMateSelectHandler()
{
}


cBirthEntry* cBirthMateSelectHandler::SelectOffspring(cAvidaContext&, const Genome& offspring, cOrganism* parent)
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
