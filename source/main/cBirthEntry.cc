/*
 *  cBirthEntry.cc
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

#include "cBirthEntry.h"
#include "cString.h"
#include "cStringUtil.h"
#include "cOrganism.h"
#include "cPhenotype.h"

cBirthEntry::cBirthEntry()
: m_mating_type(MATING_TYPE_JUVENILE)
, m_mating_display_a(0)
, m_mating_display_b(0)
, m_mate_preference(MATE_PREFERENCE_RANDOM)
, m_group_id(-1)
, timestamp(-1)
{
}

//This instructor is intended to be use to create a temporary birth entry from a parent that is
// about to divide sexually, just for record-keeping purposes; the birth entry should then be
// immediately destroyed
cBirthEntry::cBirthEntry(const Genome& _offspring, cOrganism* _parent, int _timestamp)
: m_mating_type(_parent->GetPhenotype().GetMatingType())
, m_mating_display_a(_parent->GetPhenotype().GetLastMatingDisplayA())
, m_mating_display_b(_parent->GetPhenotype().GetLastMatingDisplayB())
, m_mate_preference(_parent->GetPhenotype().GetMatePreference())
, m_group_id(-1)
, genome(_offspring)
, merit(_parent->GetPhenotype().GetMerit())
, timestamp(_timestamp)
{
  // Note: Not checking for energy because we don't want to clear out the parent's energy
  // for a temporary birth entry, otherwise things may get screwed up when the REAL offspring
  // is created

  // Similarly, I'm not setting the biogroups here because I don't want to add references to them,
  // since this birth entry is going to be destroyed anyway
  if (_parent->HasOpinion()) {
    m_group_id = _parent->GetOpinion().first;
  }
}


cBirthEntry::~cBirthEntry()
{
  if (groups) {
    for (int i = 0; i < groups->GetSize(); i++) {
      (*groups)[i]->RemoveActiveReference();
    }
  }
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
  //group
  cString result;
  
  result = genome.Representation()->AsString();
  result += " "; result += cStringUtil::Convert(timestamp);
  result += " "; result += cStringUtil::Convert(merit.GetDouble());
  result += " "; result += cStringUtil::Convert(m_mating_type);
  result += " "; result += cStringUtil::Convert(m_mate_preference);
  result += " "; result += cStringUtil::Convert(m_mating_display_a);
  result += " "; result += cStringUtil::Convert(m_mating_display_b);
  result += " "; result += cStringUtil::Convert(m_group_id);
  
  return result;
}

//Companion function for GetPhenotypeString() that tells what information is contained in each field
cString cBirthEntry::GetPhenotypeStringFormat()
{
  return "genome timestamp merit mating_type mate_preference mating_display_a mating_display_b group";
}

cBirthEntry& cBirthEntry::operator=(const cBirthEntry& _birth_entry)
{
  m_mating_type = _birth_entry.m_mating_type;
  m_mating_display_a = _birth_entry.m_mating_display_a;
  m_mating_display_b = _birth_entry.m_mating_display_b;
  m_parent_task_count = _birth_entry.m_parent_task_count;
  m_mate_preference = _birth_entry.m_mate_preference;
  m_group_id = _birth_entry.m_group_id;
  
  genome = _birth_entry.genome;
  energy4Offspring = _birth_entry.energy4Offspring;
  merit = _birth_entry.merit;
  timestamp = _birth_entry.timestamp;
  groups = _birth_entry.groups;
  
  // Creating a copy of this birth entry, make sure to add active references to group membership
  if (groups) {
    for (int i = 0; i < groups->GetSize(); i++) {
      (*groups)[i]->AddActiveReference();
    }
  }
  
  return *this;
}
