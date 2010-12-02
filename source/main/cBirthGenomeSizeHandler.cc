/*
 *  cBirthGenomeSizeHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#include "cBirthGenomeSizeHandler.h"

#include "cBirthChamber.h"
#include "cGenome.h"


cBirthGenomeSizeHandler::~cBirthGenomeSizeHandler()
{
  for (int i = 0; i < m_entries.GetSize(); i++) {
    m_bc->ClearEntry(m_entries[i]);
  }
}


cBirthEntry* cBirthGenomeSizeHandler::SelectOffspring(cAvidaContext& ctx, const cGenome& offspring, cOrganism* parent)
{
  int offspring_length = offspring.GetSize();
  
  // If this is a new largest genome, increase the array size accordingly
  if (m_entries.GetSize() <= offspring_length) m_entries.Resize(offspring_length + 1);
  
  // Determine if we have an offspring of this length waiting already...
  if (!m_bc->ValidBirthEntry(m_entries[offspring_length])) {
    m_bc->StoreAsEntry(offspring, parent, m_entries[offspring_length]);
    return NULL; 				
  }
  
  return &(m_entries[offspring_length]);
}
