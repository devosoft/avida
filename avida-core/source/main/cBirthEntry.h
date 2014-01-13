/*
 *  cBirthEntry.h
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

#ifndef cBirthEntry_h
#define cBirthEntry_h

#include "avida/core/Genome.h"
#include "avida/systematics/Types.h"

#include "cMerit.h"
#include "cString.h"

class cBioGroup;
class cOrganism;

using namespace Avida;

class cBirthEntry
{
private:
  int m_mating_type;
  int m_mating_display_a;
  int m_mating_display_b;
  int m_mate_preference;
  int m_group_id;
  Apto::Array<int> m_parent_task_count;
public:
  Genome genome;
  double energy4Offspring;
  cMerit merit;
  int timestamp; // -1 if empty
  Systematics::GroupMembershipPtr groups;
  
  cBirthEntry();
  cBirthEntry(const Genome& _offspring, cOrganism* _parent, int _timestamp);
  ~cBirthEntry();
  
  //Accessor functions
  int GetMatingType() { return m_mating_type; }
  int GetParentTaskCount(int which_task) { return m_parent_task_count[which_task]; }
  const Apto::Array<int>& GetParentTaskCount() const { return m_parent_task_count; }
  int GetMatingDisplayA() const { return m_mating_display_a; } 
  int GetMatingDisplayB() const { return m_mating_display_b; } 
  int GetMatePreference() const { return m_mate_preference; }
  int GetGroupID() const { return m_group_id; }
  
  void SetMatingType(int _mating_type) { m_mating_type = _mating_type; } //@CHC
  void SetParentTaskCount(Apto::Array<int> _parent_task_count) { m_parent_task_count = _parent_task_count; } //@CHC
  void SetMatingDisplayA(int _mating_display_a) { m_mating_display_a = _mating_display_a; } //@CHC
  void SetMatingDisplayB(int _mating_display_b) { m_mating_display_b = _mating_display_b; } //@CHC
  void SetMatePreference(int _mate_preference) { m_mate_preference = _mate_preference; }
  void SetGroupID(int _group_id) { m_group_id = _group_id; }
  
  //Other functions
  cString GetPhenotypeString();
  static cString GetPhenotypeStringFormat();
  
  //Operators
  cBirthEntry& operator=(const cBirthEntry& _birth_entry);
  
};

#endif
