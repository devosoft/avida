/*
 *  cBirthEntry.cc
 *  Avida
 *
 *  Called "birth_chamber.cc" prior to 12/2/05.
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

#include "cBirthEntry.h"
#include "cString.h"
#include "cStringUtil.h"

cBirthEntry::cBirthEntry()
: timestamp(-1)
, m_mating_type(MATING_TYPE_JUVENILE)
, m_mate_preference(MATE_PREFERENCE_RANDOM)
, m_mating_display_a(0)
, m_mating_display_b(0)
{
}

//Returns a string representation of a birth entry's information (primarily used for print actions
// that output information about the offspring in the birth chamber)
cString cBirthEntry::GetPhenotypeString()
{
  //genome
  //timestamp
  //merit
  //mating_type
  //mate_preference
  //mating_display_a
  //mating_display_b
  cString result;
  
  result = genome.GetSequence().AsString();
  result += " "; result += cStringUtil::Convert(timestamp);
  result += " "; result += cStringUtil::Convert(merit.GetDouble());
  result += " "; result += cStringUtil::Convert(m_mating_type);
  result += " "; result += cStringUtil::Convert(m_mate_preference);
  result += " "; result += cStringUtil::Convert(m_mating_display_a);
  result += " "; result += cStringUtil::Convert(m_mating_display_b);
  
  return result;
}

//Companion function for GetPhenotypeString() that tells what information is contained in each field
cString cBirthEntry::GetPhenotypeStringFormat()
{
  return "genome timestamp merit mating_type mate_preference mating_display_a mating_display_b";
}

cBirthEntry& cBirthEntry::operator=(const cBirthEntry& _birth_entry)
{
  m_mating_type = _birth_entry.m_mating_type;
  m_mating_display_a = _birth_entry.m_mating_display_a;
  m_mating_display_b = _birth_entry.m_mating_display_b;
  m_parent_task_count = _birth_entry.m_parent_task_count;
  m_mate_preference = _birth_entry.m_mate_preference;
  
  genome = _birth_entry.genome;
  energy4Offspring = _birth_entry.energy4Offspring;
  merit = _birth_entry.merit;
  timestamp = _birth_entry.timestamp;
  groups = _birth_entry.groups;
  
  return *this;
}