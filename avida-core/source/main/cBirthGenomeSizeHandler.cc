/*
 *  cBirthGenomeSizeHandler.cc
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

#include "cBirthGenomeSizeHandler.h"

#include "avida/core/InstructionSequence.h"
#include "avida/core/Genome.h"

#include "cBirthChamber.h"

using namespace Avida;


cBirthGenomeSizeHandler::~cBirthGenomeSizeHandler()
{
}


cBirthEntry* cBirthGenomeSizeHandler::SelectOffspring(cAvidaContext&, const Genome& offspring, cOrganism* parent)
{
  ConstInstructionSequencePtr offspring_seq_p;
  ConstGeneticRepresentationPtr offspring_rep_p = offspring.Representation();
  offspring_seq_p.DynamicCastFrom(offspring_rep_p);
  const InstructionSequence& offspring_seq = *offspring_seq_p;
  
  int offspring_length = offspring_seq.GetSize();
  
  // If this is a new largest genome, increase the array size accordingly
  if (m_entries.GetSize() <= offspring_length) m_entries.Resize(offspring_length + 1);
  
  // Determine if we have an offspring of this length waiting already...
  if (!m_bc->ValidBirthEntry(m_entries[offspring_length])) {
    m_bc->StoreAsEntry(offspring, parent, m_entries[offspring_length]);
    return NULL; 				
  }
  
  return &(m_entries[offspring_length]);
}
