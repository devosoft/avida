/*
 *  cBGGenotypeManager.cc
 *  Avida
 *
 *  Created by David on 11/11/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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

#include "cBGGenotypeManager.h"

#include "cBGGenotype.h"
#include "cGenome.h"
#include "cStats.h"
#include "cStringUtil.h"
#include "cWorld.h"


inline void cBGGenotypeManager::resizeActiveList(int size)
{
  if (m_active_sz.GetSize() <= size) m_active_sz.Resize(size + 1);
}


cBioGroup* cBGGenotypeManager::ClassifyNewBioUnit(cBioUnit* bu) { return ClassifyNewBioUnit(bu, NULL); }

cBGGenotype* cBGGenotypeManager::ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents)
{
  int list_num = hashGenome(bu->GetMetaGenome().GetGenome());

  cBGGenotype* found = NULL;
  tListIterator<cBGGenotype> list_it(m_active_hash[list_num]);
  while (list_it.Next() != NULL) {
    if (list_it.Get()->Matches(bu)) {
      found = list_it.Get();
      found->NotifyNewBioUnit(bu);
      break;
    }
  }
  
  if (!found) {
    found = new cBGGenotype(this, m_next_id++, bu, m_world->GetStats().GetUpdate(), parents);
    m_active_hash[list_num].Push(found);
    resizeActiveList(found->GetNumOrganisms());
    m_active_sz[found->GetNumOrganisms()].Push(found);
    m_world->GetStats().AddGenotype();
  }
  
  return found;
}


unsigned int cBGGenotypeManager::hashGenome(const cGenome& genome) const
{
  unsigned int total = 0;
  
  for (int i = 0; i < genome.GetSize(); i++) {
    total += (genome[i].GetOp() + 3) * i;
  }
  
  return total % nBGGenotypeManager::HASH_SIZE;
}

cString cBGGenotypeManager::nameGenotype(int size, int num) const
{
  char alpha[6];
  
  for (int i = 4; i >= 0; i--) {
    alpha[i] = (num % 26) + 'a';
    num /= 26;
  }
  alpha[5] = '\0';
  
  return cStringUtil::Stringf("%03d-%s", size, alpha);
}
