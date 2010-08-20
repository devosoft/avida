/*
 *  cSexualAncestry.cc
 *  Avida
 *
 *  Created by David on 7/7/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#include "cSexualAncestry.h"

#include "cBGGenotype.h"


cSexualAncestry::cSexualAncestry(cBioGroup* bg)
{
  m_id = bg->GetID();
  m_ancestor_ids[0] = -1;
  m_ancestor_ids[1] = -1;
  m_ancestor_ids[2] = -1;
  m_ancestor_ids[3] = -1;
  m_ancestor_ids[4] = -1;
  m_ancestor_ids[5] = -1;
  
  assert(dynamic_cast<cBGGenotype*>(bg));
  cBGGenotype* genotype = (cBGGenotype*)bg;
  
  if (genotype->GetParents().GetSize() > 0) {
    cBGGenotype* p1 = genotype->GetParents()[0];
    cSexualAncestry* p1sa = p1->GetData<cSexualAncestry>();
    if (!p1) {
      p1sa = new cSexualAncestry(p1);
      p1->AttachData(p1sa);
    }
    
    m_ancestor_ids[0] = p1->GetID();
    m_ancestor_ids[2] = p1sa->GetAncestorID(0);
    m_ancestor_ids[3] = p1sa->GetAncestorID(1);
  }

  if (genotype->GetParents().GetSize() > 1) {
    cBGGenotype* p2 = genotype->GetParents()[1];
    cSexualAncestry* p2sa = p2->GetData<cSexualAncestry>();
    if (!p2sa) {
      p2sa = new cSexualAncestry(p2);
      p2->AttachData(p2sa);
    }
    
    m_ancestor_ids[1] = p2->GetID();
    m_ancestor_ids[4] = p2sa->GetAncestorID(0);
    m_ancestor_ids[5] = p2sa->GetAncestorID(1);
  }
}

int cSexualAncestry::GetPhyloDistance(cBioGroup* bg) const
{
  assert(dynamic_cast<cBGGenotype*>(bg));
  cSexualAncestry* tsa = bg->GetData<cSexualAncestry>();
  if (!tsa) {
    tsa = new cSexualAncestry(bg);
    bg->AttachData(tsa);
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
