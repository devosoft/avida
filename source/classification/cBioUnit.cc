/*
 *  cBioUnit.cc
 *  Avida
 *
 *  Created by David on 10/7/09.
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

#include "cBioUnit.h"

#include "cBioGroup.h"
#include "cString.h"
#include "tArrayMap.h"
#include "tArrayUtils.h"


cBioUnit::~cBioUnit()
{
  // Notify all groups that this bio unit has been terminated
  for (int i = 0; i < m_bio_groups.GetSize(); i++) m_bio_groups[i]->RemoveBioUnit(this);
}

cBioGroup* cBioUnit::GetBioGroup(const cString& role) const
{
  for (int i = 0; i < m_bio_groups.GetSize(); i++) if (m_bio_groups[i]->GetRole() == role) return m_bio_groups[i];
  return NULL;
}



void cBioUnit::SelfClassify(const tArray<const tArray<cBioGroup*>*>& parents)
{
  assert(parents.GetSize() > 0);
  
  if (parents.GetSize() == 1) {
    // Handle Asexual Organisms
    tArray<cBioGroup*> parent_group(1);
    
    // Loop through all parental groups and classify this bio unit with each one
    for (int i = 0; i < parents[0]->GetSize(); i++) {
      parent_group[0] = (*parents[0])[i];
      cBioGroup* group = (*parents[0])[i]->ClassifyNewBioUnit(this, &parent_group);
      if (group) m_bio_groups.Push(group);
    }
  } else {
    // Handle Sexual Organisms (multi-parent)
    tArrayMap<int, tArray<cBioGroup*> > parent_groups;
    
    // Sort groups into sets of parental groups by role_id
    for (int p = 0; p < parents.GetSize(); p++) {
      for (int g = 0; g < parents[p]->GetSize(); g++) {
        cBioGroup* group = (*parents[p])[g];
        parent_groups[group->GetRoleID()].Push(group);
      }
    }
    
    // Get the distinct set of group roles
    tArray<int> group_roles = parent_groups.GetKeys();
    
    // Handle group dependencies - right now this simply means ordered by role id
    tArrayUtils::QSort(group_roles);
    
    // Classify this bio unit with all distinct parental group roles
    for (int r = 0; r < group_roles.GetSize(); r++) {
      tArray<cBioGroup*>& pgrps = parent_groups[r];
      cBioGroup* group = pgrps[0]->ClassifyNewBioUnit(this, &pgrps);
      if (group) m_bio_groups.Push(group);
    }
  }
}

void cBioUnit::HandleGestation()
{
  for (int i = 0; i < m_bio_groups.GetSize(); i++) m_bio_groups[i]->HandleBioUnitGestation(this);
}

bool cBioUnit::IsParasite()
{
	if (GetUnitSource() == SRC_PARASITE_INJECT || GetUnitSource() == SRC_PARASITE_FILE_LOAD)
		return true;
	return false;
}
