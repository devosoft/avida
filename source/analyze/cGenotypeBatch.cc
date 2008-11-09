/*
 *  cGenotypeBatch.cc
 *  Avida
 *
 *  Created by David Bryson on 11/3/08.
 *  Copyright 2008 Michigan State University. All rights reserved.
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

#include "cGenotypeBatch.h"

#include "cAnalyzeGenotype.h"
#include "cDriverManager.h"
#include "cDriverStatusConduit.h"
#include "cRandom.h"
#include "tSmartArray.h"


cGenotypeBatch::~cGenotypeBatch()
{
  tListIterator<cAnalyzeGenotype> it(m_list);
  cAnalyzeGenotype* genotype = NULL;
  while ((genotype = it.Next())) delete genotype;
}


cAnalyzeGenotype* cGenotypeBatch::FindGenotypeNumCPUs() const
{
  return new cAnalyzeGenotype(*(m_list.FindMax(&cAnalyzeGenotype::GetNumCPUs)));
}

cAnalyzeGenotype* cGenotypeBatch::PopGenotypeNumCPUs()
{
  clearFlags();
  return m_list.PopMax(&cAnalyzeGenotype::GetNumCPUs);
}


cAnalyzeGenotype* cGenotypeBatch::FindGenotypeTotalCPUs() const
{
  return new cAnalyzeGenotype(*(m_list.FindMax(&cAnalyzeGenotype::GetTotalCPUs)));
}

cAnalyzeGenotype* cGenotypeBatch::PopGenotypeTotalCPUs()
{
  clearFlags();
  return m_list.PopMax(&cAnalyzeGenotype::GetTotalCPUs);
}


cAnalyzeGenotype* cGenotypeBatch::FindGenotypeMetabolicRate() const
{
  return new cAnalyzeGenotype(*(m_list.FindMax(&cAnalyzeGenotype::GetMerit)));
}

cAnalyzeGenotype* cGenotypeBatch::PopGenotypeMetabolicRate()
{
  clearFlags();
  return m_list.PopMax(&cAnalyzeGenotype::GetMerit);
}


cAnalyzeGenotype* cGenotypeBatch::FindGenotypeFitness() const
{
  return new cAnalyzeGenotype(*(m_list.FindMax(&cAnalyzeGenotype::GetFitness)));
}

cAnalyzeGenotype* cGenotypeBatch::PopGenotypeFitness()
{
  clearFlags();
  return m_list.PopMax(&cAnalyzeGenotype::GetFitness);
}


cAnalyzeGenotype* cGenotypeBatch::FindGenotypeID(int gid) const
{
  return new cAnalyzeGenotype(*(m_list.FindValue(&cAnalyzeGenotype::GetID, gid)));
}

cAnalyzeGenotype* cGenotypeBatch::PopGenotypeID(int gid)
{
  clearFlags();
  return m_list.PopValue(&cAnalyzeGenotype::GetID, gid);
}


cAnalyzeGenotype* cGenotypeBatch::FindGenotypeRandom(cRandom& rng) const
{
  if (m_list.GetSize() == 0) return NULL;
  
  return new cAnalyzeGenotype(*(m_list.GetPos(rng.GetUInt(m_list.GetSize()))));
}

cAnalyzeGenotype* cGenotypeBatch::PopGenotypeRandom(cRandom& rng)
{
  if (m_list.GetSize() == 0) return NULL;

  clearFlags();
  return m_list.PopPos(rng.GetUInt(m_list.GetSize()));
}


cGenotypeBatch* cGenotypeBatch::FindLineage(cAnalyzeGenotype* end_genotype) const
{
  if ((end_genotype)) return FindLineage(end_genotype->GetID());
  
  return new cGenotypeBatch;
}

cGenotypeBatch* cGenotypeBatch::FindLineage(int end_genotype_id) const
{
  cGenotypeBatch* batch = new cGenotypeBatch;
  cAnalyzeGenotype* found_gen = FindGenotypeID(end_genotype_id);
  
  while ((found_gen)) {
    batch->m_list.Push(found_gen);
    batch->m_lineage_head = found_gen;
    found_gen = FindGenotypeID(found_gen->GetParentID());
  }
  
  batch->m_is_lineage = true;
  
  return batch;
}


cGenotypeBatch* cGenotypeBatch::FindClade(cAnalyzeGenotype* start_genotype) const
{
  if ((start_genotype)) return FindClade(start_genotype->GetID());
  
  return new cGenotypeBatch;
}

cGenotypeBatch* cGenotypeBatch::FindClade(int start_genotype_id) const
{
  cGenotypeBatch* batch = new cGenotypeBatch;
  tList<cAnalyzeGenotype> list(m_list);
  tSmartArray<int> scan_list;
  cAnalyzeGenotype* found_gen = FindGenotypeID(start_genotype_id);
 
  if ((found_gen)) {
    batch->m_list.Push(found_gen);
    batch->m_clade_head = found_gen;
    scan_list.Push(found_gen->GetID());
  }
  
  while (scan_list.GetSize()) {
    int parent_id = scan_list.Pop();
    
    // Seach for all of the offspring of this genotype...
    tListIterator<cAnalyzeGenotype> it(list);
    while ((found_gen = it.Next()) != NULL) {
      if (found_gen->GetParentID() == parent_id) {
        it.Remove();
        scan_list.Push(found_gen->GetID());
        batch->m_list.Push(new cAnalyzeGenotype(*found_gen));
      }
    }
  }

  return batch;
}


void cGenotypeBatch::RemoveClade(cAnalyzeGenotype* start_genotype)
{
  if ((start_genotype)) RemoveClade(start_genotype->GetID());
}

void cGenotypeBatch::RemoveClade(int start_genotype_id)
{
  if (m_is_lineage) {
    tListIterator<cAnalyzeGenotype> it(m_list);
    cAnalyzeGenotype* genotype = NULL;
    
    while ((genotype = it.Next())) {
      if (genotype->GetID() == start_genotype_id) {
        it.Remove();
        delete genotype;
        break;
      }
    }
    while ((genotype = it.Next())) { it.Remove(); delete genotype; }
  } else {
    tSmartArray<int> scan_list;
    cAnalyzeGenotype* found_gen = PopGenotypeID(start_genotype_id);
    
    if ((found_gen)) scan_list.Push(found_gen->GetID());
    
    while (scan_list.GetSize()) {
      int parent_id = scan_list.Pop();
      
      // Seach for all of the offspring of this genotype...
      tListIterator<cAnalyzeGenotype> it(m_list);
      while ((found_gen = it.Next()) != NULL) {
        if (found_gen->GetParentID() == parent_id) {
          it.Remove();
          scan_list.Push(found_gen->GetID());
          delete found_gen;
        }
      }
    }
  }
}

