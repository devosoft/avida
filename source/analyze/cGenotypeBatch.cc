/*
 *  cGenotypeBatch.cc
 *  Avida
 *
 *  Created by David Bryson on 11/3/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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


cAnalyzeGenotype* cGenotypeBatch::FindOrganismRandom(cRandom& rng) const
{
  if (m_list.GetSize() == 0) return NULL;
  
  int num_orgs = m_list.Count(&cAnalyzeGenotype::GetNumCPUs);
  while (true) {
    cAnalyzeGenotype* genotype = m_list.FindSummedValue(rng.GetUInt(num_orgs), &cAnalyzeGenotype::GetNumCPUs);
    if (genotype->GetNumCPUs()) return new cAnalyzeGenotype(*genotype);
  }
  
  return NULL;
}

cAnalyzeGenotype* cGenotypeBatch::PopOrganismRandom(cRandom& rng)
{
  if (m_list.GetSize() == 0) return NULL;

  int num_orgs = m_list.Count(&cAnalyzeGenotype::GetNumCPUs);
  while (true) {
    cAnalyzeGenotype* genotype = m_list.FindSummedValue(rng.GetUInt(num_orgs), &cAnalyzeGenotype::GetNumCPUs);
    if (genotype->GetNumCPUs()) {
      genotype->SetNumCPUs(genotype->GetNumCPUs() - 1);
      return new cAnalyzeGenotype(*genotype);
    }
  }
  
  return m_list.PopPos(rng.GetUInt(m_list.GetSize()));
}


cAnalyzeGenotype* cGenotypeBatch::FindLastCommonAncestor()
{
  // Assumes that the batch contains a population and all of its common ancestors
  // Finds the last common ancestor among all current organisms that are still alive,
  // i.e. have an update_died of -1.
  
  // Connect each genotype to its parent.
  tListIterator<cAnalyzeGenotype> it(m_list);
  tListIterator<cAnalyzeGenotype> parent_it(m_list);
  cAnalyzeGenotype* on_child = NULL;
  while ((on_child = it.Next())) {
    parent_it.Reset();
    cAnalyzeGenotype* on_parent = NULL;
    while ((on_parent = parent_it.Next())) {
      if (on_child->GetParentID() == on_parent->GetID()) {
        on_child->LinkParent(on_parent);
        break;
      }
    }
  }
    
  // Find the genotype without a parent (there should only be one)
  it.Reset();
  cAnalyzeGenotype* lca = NULL;
  cAnalyzeGenotype* test_lca = NULL;
  while ((test_lca = it.Next())) {
    if (!test_lca->GetParent()) {
      // It is an error to get two genotypes without a parent
      if (lca) return NULL;
      lca = test_lca;
    }
  }
  
  // Follow the children from this parent until we find a genotype with more than one child.
  // This is the last common ancestor.
  while (lca->GetChildList().GetSize() == 1) {
    lca = lca->GetChildList().Pop();
  }
  
  return new cAnalyzeGenotype(*lca);
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
    
  return batch;
}


cGenotypeBatch* cGenotypeBatch::FindSexLineage(cAnalyzeGenotype* end_genotype, bool use_genome_size) const
{
  if ((end_genotype)) return FindSexLineage(end_genotype->GetID(), use_genome_size);
  
  return new cGenotypeBatch;
}

cGenotypeBatch* cGenotypeBatch::FindSexLineage(int end_genotype_id, bool use_genome_size) const
{
  cGenotypeBatch* batch = new cGenotypeBatch;
  cAnalyzeGenotype* found_gen = FindGenotypeID(end_genotype_id);
  
  if (!found_gen) return batch;

  
  cAnalyzeGenotype* gen_p1 = NULL;
  cAnalyzeGenotype* gen_p2 = NULL;
  
  // Construct a list of genotypes found...  
  tListPlus<cAnalyzeGenotype> src_list(m_list);
  tListPlus<cAnalyzeGenotype>& trgt_list = batch->m_list;
  trgt_list.Push(found_gen);
  int next_id1 = found_gen->GetParentID();
  int next_id2 = found_gen->GetParent2ID();
  
  bool found_p1 = true;
  bool found_p2 = true;
  
  while (found_p1 && found_p2) {
    found_p1 = false;
    found_p2 = false;
    
    // Look for the secondary parent first....
    tListIterator<cAnalyzeGenotype> src_it(src_list);
    while ((gen_p2 = src_it.Next())) {
      if (gen_p2->GetID() == next_id2) {
        src_it.Remove();
        trgt_list.Push(new cAnalyzeGenotype(*gen_p2));
        found_p2 = true;
        break;
      }
    }
    
    // Secondary parent may have already been found
    if (!found_p2) {
      tListIterator<cAnalyzeGenotype> trgt_it(trgt_list);
      while ((gen_p2 = trgt_it.Next())) {
        if (gen_p2->GetID() == next_id2) {
          found_p2 = true;
          break;
        }
      }
    }
    
    // If the secondary parent still has not been found, proceed no further
    if (!found_p2) break;
    
    // Next, look for the primary parent...
    src_it.Reset();
    while ((gen_p1 = src_it.Next())) {
      if (gen_p1->GetID() == next_id1) {
        src_it.Remove();
        trgt_list.Push(new cAnalyzeGenotype(*gen_p1));
        
        // if finding lineages by parental length, may have to swap 
        if (use_genome_size && gen_p1->GetLength() < gen_p2->GetLength()) { 
          cAnalyzeGenotype* temp = gen_p1; 
          gen_p1 = gen_p2; 
          gen_p2 = temp; 
        }
        next_id1 = gen_p1->GetParentID();
        next_id2 = gen_p2->GetParent2ID();
        found_p1 = true;
        break;
      }
    }
    
    // If the primary parent was not found, it may already have been placed in the target list as a secondary parent... 
    if (!found_p1) {
      tListIterator<cAnalyzeGenotype> trgt_it(trgt_list);
      while ((gen_p1 = trgt_it.Next()) != NULL) {
        if (gen_p1->GetID() == next_id1) {
          // Don't move to found list, since its already there, but update to the next ids.
          // if finding lineages by parental length, may have to swap
          if (use_genome_size && gen_p1->GetLength() < gen_p2->GetLength()) {
            cAnalyzeGenotype* temp = gen_p1;
            gen_p1 = gen_p2;
            gen_p2 = temp;
          }
          next_id1 = gen_p1->GetParentID();
          next_id2 = gen_p1->GetParent2ID();
          found_p1 = true;
          break;
        }
      }
    }    
  }
  
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


void cGenotypeBatch::PruneExtinctGenotypes()
{
  cAnalyzeGenotype* genotype = NULL;
  tListIterator<cAnalyzeGenotype> it(m_list);
  
  while ((genotype = it.Next())) {
    if (genotype->GetNumCPUs() == 0) {
      it.Remove();
      delete genotype;
    }
  }
  
  clearFlags();
}

void cGenotypeBatch::PruneNonViableGenotypes()
{
  cAnalyzeGenotype* genotype = NULL;
  tListIterator<cAnalyzeGenotype> it(m_list);
  
  while ((genotype = it.Next())) {
    if (!genotype->GetViable()) {
      it.Remove();
      delete genotype;
    }
  }
  
  clearFlags();
}

