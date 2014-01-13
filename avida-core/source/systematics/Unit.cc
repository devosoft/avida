/*
 *  systematics/Unit.cc
 *  avida-core
 *
 *  Created by David on 12/16/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/systematics/Unit.h"

#include "avida/systematics/Group.h"


Apto::String Avida::Systematics::Source::AsString() const
{
  Apto::String rtn;
  
  switch (transmission_type) {
    case DIVISION:
      rtn += "div";
      break;
    case DUPLICATION:
      rtn += "dup";
      break;
    case HORIZONTAL:
      rtn += "horz";
      break;
    case VERTICAL:
      rtn += "vert";
      break;
    default:
      rtn += "unknown";
      break;
  }
  
  rtn += ":";
  rtn += (external ? "ext" : "int");
  
  // @TODO - should add arguments to source string in the future
  
  return rtn;
}

Avida::Systematics::Unit::~Unit()
{
  for (int i = 0; i < m_groups->GetSize(); i++) {
    m_groups->Get(i)->RemoveUnit();
  }
}

Avida::Systematics::GroupPtr Avida::Systematics::Unit::SystematicsGroup(const RoleID& role) const
{
  for (int i = 0; i < m_groups->GetSize(); i++) if (m_groups->Get(i)->Role() == role) return m_groups->Get(i);
  return GroupPtr();
}


bool Avida::Systematics::Unit::AddClassification(GroupPtr g)
{
  for (int i = 0; i < m_groups->GetSize(); i++) if (m_groups->Get(i)->Role() == g->Role()) return false;
  m_groups->Push(g);
  return true;
}

void Avida::Systematics::Unit::Reclassify(GroupPtr g)
{
  for (int i = 0; i < m_groups->GetSize(); i++) {
    if (m_groups->Get(i)->Role() == g->Role()) {
      m_groups->Get(i) = g;
      return;
    }
  }
  m_groups->Push(g);  
}

void Avida::Systematics::Unit::SelfClassify(ConstConstParentGroupsPtr parent_groups)
{
  assert(parent_groups->GetSize() > 0);

  if (parent_groups->GetSize() == 1) {
    // Handle Asexual Organisms
    GroupMembershipPtr pg(new GroupMembership(1));
    
    // Loop through all parental groups and classify this bio unit with each one
    for (int i = 0; i < parent_groups->Get(0)->GetSize(); i++) {
      pg->Get(0) = parent_groups->Get(0)->Get(i);
      GroupPtr group = (const_cast<GroupPtr&>(parent_groups->Get(0)->Get(i)))->ClassifyNewUnit(thisPtr(), pg);
      if (group) m_groups->Push(group);
    }
  } else {
    // Handle Sexual Organisms (multi-parent)
    Apto::Map<RoleID, GroupMembershipPtr> pgs;
    
    // Sort groups into sets of parental groups by role_id
    for (int p = 0; p < parent_groups->GetSize(); p++) {
      for (int g = 0; g < parent_groups->Get(p)->GetSize(); g++) {
        GroupPtr group = parent_groups->Get(p)->Get(g);
        GroupMembershipPtr& pm = pgs[group->Role()];
        if (!pm) pm = GroupMembershipPtr(new GroupMembership);
        pm->Push(group);
      }
    }
    
    // Classify this bio unit with all distinct parental group roles
    for (Apto::Map<RoleID, GroupMembershipPtr>::ValueIterator role = pgs.Values(); (role.Next()); ) {
      GroupPtr group = (*role.Get())->Get(0)->ClassifyNewUnit(thisPtr(), *role.Get());
      if (group) m_groups->Push(group);
    }
  }
}

void Avida::Systematics::Unit::HandleGestation()
{
  for (int i = 0; i < m_groups->GetSize(); i++) m_groups->Get(i)->HandleUnitGestation(thisPtr());
}

inline Avida::Systematics::UnitPtr Avida::Systematics::Unit::thisPtr()
{
  AddReference(); // Explicitly add reference for newly created SmartPtr
  return UnitPtr(this);
}
