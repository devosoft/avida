/*
 *  private/systematics/SexualAncestry.cc
 *  Avida
 *
 *  Created by David on 7/7/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#include "avida/private/systematics/SexualAncestry.h"

#include "avida/core/Properties.h"
#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"

const Apto::String Avida::Systematics::SexualAncestry::ObjectKey("Avida::Systematics::SexualAncestry");

Avida::Systematics::SexualAncestry::SexualAncestry(GroupPtr g)
{
  m_id = g->ID();
  m_ancestor_ids[0] = -1;
  m_ancestor_ids[1] = -1;
  m_ancestor_ids[2] = -1;
  m_ancestor_ids[3] = -1;
  m_ancestor_ids[4] = -1;
  m_ancestor_ids[5] = -1;
  
  if (!g->Properties().Has("parents")) return;
  
  ArbiterPtr arbiter = g->Arbiter();
  Apto::Array<GroupPtr> parents;
  Apto::String parent_str(g->Properties().Get("parents").StringValue());
  while (parent_str.GetSize()) {
    parents.Push(arbiter->Group(Apto::StrAs(parent_str.Pop(','))));
  }
  
  
  if (parents.GetSize() > 0) {
    GroupPtr p1 = parents[0];
    SexualAncestryPtr p1sa = p1->GetData<SexualAncestry>();
    if (!p1sa) {
      p1sa = SexualAncestryPtr(new SexualAncestry(p1));
      p1->AttachData(p1sa);
    }
    
    m_ancestor_ids[0] = p1->ID();
    m_ancestor_ids[2] = p1sa->GetAncestorID(0);
    m_ancestor_ids[3] = p1sa->GetAncestorID(1);
  }

  if (parents.GetSize() > 1) {
    GroupPtr p2 = parents[1];
    SexualAncestryPtr p2sa = p2->GetData<SexualAncestry>();
    if (!p2sa) {
      p2sa = SexualAncestryPtr(new SexualAncestry(p2));
      p2->AttachData(p2sa);
    }
    
    m_ancestor_ids[1] = p2->ID();
    m_ancestor_ids[4] = p2sa->GetAncestorID(0);
    m_ancestor_ids[5] = p2sa->GetAncestorID(1);
  }
}


bool Avida::Systematics::SexualAncestry::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


int Avida::Systematics::SexualAncestry::GetPhyloDistance(GroupPtr g) const
{
  SexualAncestryPtr tsa = g->GetData<SexualAncestry>();
  if (!tsa) {
    tsa = SexualAncestryPtr(new SexualAncestry(g));
    g->AttachData(tsa);
  }
  

  if (m_id == tsa->GetID()) return 0;

  if (m_id == tsa->GetAncestorID(0) ||  // Parent of test
      m_id == tsa->GetAncestorID(1) ||  // Parent of test
      tsa->GetID() == GetAncestorID(0) ||  // Child of test
      tsa->GetID() == GetAncestorID(1)     // Child of test
      ) {
    return 1;
  }
  
  if (m_id == tsa->GetAncestorID(2) ||  // Grandparent of test
      m_id == tsa->GetAncestorID(3) ||  // Grandparent of test
      m_id == tsa->GetAncestorID(4) ||  // Grandparent of test
      m_id == tsa->GetAncestorID(5) ||  // Grandparent of test
      tsa->GetID() == GetAncestorID(2) ||  // Grandchild of test
      tsa->GetID() == GetAncestorID(3) ||  // Grandchild of test
      tsa->GetID() == GetAncestorID(4) ||  // Grandchild of test
      tsa->GetID() == GetAncestorID(5) ||  // Grandchild of test
      GetAncestorID(0) == tsa->GetAncestorID(0) || // Sibling of test
      GetAncestorID(0) == tsa->GetAncestorID(1) || // Sibling of test
      GetAncestorID(1) == tsa->GetAncestorID(0) || // Sibling of test
      GetAncestorID(1) == tsa->GetAncestorID(1)    // Sibling of test
      ) {
    return 2;
  }
  if (GetAncestorID(0) == tsa->GetAncestorID(2) || // Uncle of test
      GetAncestorID(0) == tsa->GetAncestorID(3) || // Uncle of test
      GetAncestorID(0) == tsa->GetAncestorID(4) || // Uncle of test
      GetAncestorID(0) == tsa->GetAncestorID(5) || // Uncle of test
      GetAncestorID(1) == tsa->GetAncestorID(2) || // Uncle of test
      GetAncestorID(1) == tsa->GetAncestorID(3) || // Uncle of test
      GetAncestorID(1) == tsa->GetAncestorID(4) || // Uncle of test
      GetAncestorID(1) == tsa->GetAncestorID(5) || // Uncle of test
      tsa->GetAncestorID(0) == GetAncestorID(2) || // Nephew of test
      tsa->GetAncestorID(0) == GetAncestorID(3) || // Nephew of test
      tsa->GetAncestorID(0) == GetAncestorID(4) || // Nephew of test
      tsa->GetAncestorID(0) == GetAncestorID(5) || // Nephew of test
      tsa->GetAncestorID(1) == GetAncestorID(2) || // Nephew of test
      tsa->GetAncestorID(1) == GetAncestorID(3) || // Nephew of test
      tsa->GetAncestorID(1) == GetAncestorID(4) || // Nephew of test
      tsa->GetAncestorID(1) == GetAncestorID(5)    // Nephew of test
      ) {
    return 3;
  }
  
  if (GetAncestorID(2) == tsa->GetAncestorID(2) || // First Cousins
      GetAncestorID(2) == tsa->GetAncestorID(3) ||
      GetAncestorID(2) == tsa->GetAncestorID(4) ||
      GetAncestorID(2) == tsa->GetAncestorID(5) ||
      GetAncestorID(3) == tsa->GetAncestorID(2) ||
      GetAncestorID(3) == tsa->GetAncestorID(3) ||
      GetAncestorID(3) == tsa->GetAncestorID(4) ||
      GetAncestorID(3) == tsa->GetAncestorID(5) ||
      GetAncestorID(4) == tsa->GetAncestorID(2) ||
      GetAncestorID(4) == tsa->GetAncestorID(3) ||
      GetAncestorID(4) == tsa->GetAncestorID(4) ||
      GetAncestorID(4) == tsa->GetAncestorID(5) ||
      GetAncestorID(5) == tsa->GetAncestorID(2) ||
      GetAncestorID(5) == tsa->GetAncestorID(3) ||
      GetAncestorID(5) == tsa->GetAncestorID(4) ||
      GetAncestorID(5) == tsa->GetAncestorID(5)
      ) {
    return 4;
  }
  
  return 5;
}
