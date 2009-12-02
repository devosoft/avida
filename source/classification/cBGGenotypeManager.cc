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


cBGGenotypeManager::cBGGenotypeManager(cWorld* world)
  : m_world(world)
  , m_next_id(1)
  , m_dom_prev(-1)
  , m_dom_time(0)
{
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) m_sz_count[i] = 0;
}

cBGGenotypeManager::~cBGGenotypeManager()
{
  // @TODO
}


cBioGroup* cBGGenotypeManager::ClassifyNewBioUnit(cBioUnit* bu) { return ClassifyNewBioUnit(bu, NULL); }


void cBGGenotypeManager::UpdateReset()
{
  if (m_active_sz.GetSize() < nBGGenotypeManager::HASH_SIZE) {
    for (int i = 0; i < m_active_sz.GetSize(); i++) {
      tListIterator<cBGGenotype> list_it(m_active_sz[i]);
      while (list_it.Next() != NULL) list_it.Get()->UpdateReset();
    }
  } else {
    for (int i = 0; i < nBGGenotypeManager::HASH_SIZE; i++) {
      tListIterator<cBGGenotype> list_it(m_active_hash[i]);
      while (list_it.Next() != NULL) list_it.Get()->UpdateReset();
    }    
  }
}


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


void cBGGenotypeManager::AdjustGenotype(cBGGenotype* genotype, int old_size, int new_size)
{
  // Remove from old size list
  m_active_sz[old_size].Remove(genotype);

  // Handle defunct genotypes
  if (new_size == 0 && genotype->GetActiveReferenceCount() == 0) {
    removeGenotype(genotype);
    return;
  }
  
  // Add to new size list
  resizeActiveList(new_size);
  m_active_sz[new_size].Push(genotype);
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

void cBGGenotypeManager::removeGenotype(cBGGenotype* genotype)
{
  if (genotype->GetActiveReferenceCount()) return;
  
  if (genotype->IsActive()) {
    int list_num = hashGenome(genotype->GetMetaGenome().GetGenome());
    m_active_hash[list_num].Remove(genotype);
    genotype->Deactivate();
    m_historic.Push(genotype);
  }
  
  if (genotype->IsThreshold()) {
    // @TODO handle threshold removal
    // m_world->GetStats().RemoveThreshold();
    // genotype->ClearThreshold();
  }
  
  if (genotype->GetPassiveReferenceCount()) return;
  
  const tArray<cBGGenotype*>& parents = genotype->GetParents();
  for (int i = 0; i < parents.GetSize(); i++) {
    parents[i]->RemovePassiveReference();
    // @TODO - update coalescent?
    removeGenotype(parents[i]);
  }
}
