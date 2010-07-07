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
#include "cDataFile.h"
#include "cGenome.h"
#include "cStats.h"
#include "cStringUtil.h"
#include "cWorld.h"
#include "tDataCommandManager.h"


cBGGenotypeManager::cBGGenotypeManager(cWorld* world)
  : m_world(world)
  , m_coalescent(NULL)
  , m_best(0)
  , m_next_id(1)
  , m_dom_prev(-1)
  , m_dom_time(0)
  , m_active_count(0)
  , m_dcm(NULL)
{
}

cBGGenotypeManager::~cBGGenotypeManager()
{
  assert(m_historic.GetSize() == 0);
  assert(m_best == 0);
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

  tListIterator<cBGGenotype> list_it(m_historic);
  while (list_it.Next() != NULL) if (!list_it.Get()->GetReferenceCount()) removeGenotype(list_it.Get());
}


void cBGGenotypeManager::UpdateStats(cStats& stats)
{
  // @TODO - genotype manager should stash stats in cStats as a classification "role" stats object so that multiple roles can report stats
  
  // Clear out genotype sums...
  stats.SumGenotypeAge().Clear();
  stats.SumAbundance().Clear();
  stats.SumGenotypeDepth().Clear();
  stats.SumSize().Clear();
  stats.SumThresholdAge().Clear();
  
  double entropy = 0.0;
  
  for (int i = 0; i < m_active_sz.GetSize(); i++) {
    tListIterator<cBGGenotype> list_it(m_active_sz[i]);
    while (list_it.Next() != NULL) {
      cBGGenotype* bg = list_it.Get();
      const int abundance = bg->GetNumOrganisms();
      
      // Update stats...
      const int age = stats.GetUpdate() - bg->GetUpdateBorn();
      stats.SumGenotypeAge().Add(age, abundance);
      stats.SumAbundance().Add(abundance);
      stats.SumGenotypeDepth().Add(bg->GetDepth(), abundance);
      stats.SumSize().Add(bg->GetMetaGenome().GetGenome().GetSize(), abundance);
      
      // Calculate this genotype's contribution to entropy
      // - when p = 1.0, partial_ent calculation would return -0.0. This may propagate
      //   to the output stage, but behavior is dependent on compiler used and optimization
      //   level.  For consistent output, ensures that 0.0 is returned.
      const double p = ((double) abundance) / (double) stats.GetNumCreatures();
      const double partial_ent = (abundance == stats.GetNumCreatures()) ? 0.0 : -(p * Log(p)); 
      entropy += partial_ent;
      
      // Do any special calculations for threshold genotypes.
      if (bg->IsThreshold()) stats.SumThresholdAge().Add(age, abundance);
    }
  }
  
  stats.SetEntropy(entropy);
  stats.SetNumGenotypes(m_active_count);
  
  
  // Handle dominant genotype stats
  cBGGenotype* dom_genotype = getBest();
  if (dom_genotype == NULL) return;
  
  stats.SetDomMerit(dom_genotype->GetMerit());
  stats.SetDomGestation(dom_genotype->GetGestationTime());
// @TODO?  stats.SetDomReproRate(dom_genotype->GetReproRate());
  stats.SetDomFitness(dom_genotype->GetFitness());
  stats.SetDomCopiedSize(dom_genotype->GetCopiedSize());
  stats.SetDomExeSize(dom_genotype->GetExecutedSize());
  
  stats.SetDomSize(dom_genotype->GetMetaGenome().GetGenome().GetSize());
  stats.SetDomID(dom_genotype->GetID());
  stats.SetDomName(dom_genotype->GetName());
  
  stats.SetDomBirths(dom_genotype->GetThisBirths());
  stats.SetDomBreedTrue(dom_genotype->GetThisBreedTrue());
  stats.SetDomBreedIn(dom_genotype->GetThisBreedIn());
  stats.SetDomBreedOut(dom_genotype->GetThisBreedOut());
  
  stats.SetDomAbundance(dom_genotype->GetNumOrganisms());
  stats.SetDomGeneDepth(dom_genotype->GetDepth());
  stats.SetDomSequence(dom_genotype->GetMetaGenome().GetGenome().AsString());
  
}



cBioGroup* cBGGenotypeManager::GetBioGroup(int bg_id)
{
  for (int i = m_best; i >= 0; i--) {
    tListIterator<cBGGenotype> list_it(m_active_sz[i]);
    while (list_it.Next() != NULL) if (list_it.Get()->GetID() == bg_id) return list_it.Get();
  }
  
  tListIterator<cBGGenotype> list_it(m_historic);
  while (list_it.Next() != NULL) if (list_it.Get()->GetID() == bg_id) return list_it.Get();
  
  return NULL;
}

void cBGGenotypeManager::SaveBioGroups(cDataFile& df)
{
  // @TODO - Just dump historic for now.  Need structured output format to support top down save
  //         With a structured save (and save params passed through), a "structured population save" could be attained
  //         by simply calling the bio group save.  As it stands right now, cPopulation must decorate columns with additional
  //         data about active genotypes, yet the bio group interface really shouldn't know about active/inactive genotypes.
  //         Thus it is not proper to split bgm save into a save historic and save active.  Right now we'll just make
  //         cPopulation do the work.
  
  tListIterator<cBGGenotype> list_it(m_historic);
  while (list_it.Next() != NULL) {
    list_it.Get()->Save(df);
    df.Endl();
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
    m_active_count++;
  }
  
  return found;
}


void cBGGenotypeManager::AdjustGenotype(cBGGenotype* genotype, int old_size, int new_size)
{
  // Remove from old size list
  m_active_sz[old_size].Remove(genotype);

  if (old_size == m_best && m_active_sz[old_size].GetSize() == 0) {
    for (m_best--; m_best > 0; m_best--) if (m_active_sz[m_best].GetSize()) break;
  }
  
  // Handle defunct genotypes
  if (new_size == 0 && genotype->GetActiveReferenceCount() == 0) {
    removeGenotype(genotype);
    return;
  }
  
  // Add to new size list
  resizeActiveList(new_size);
  m_active_sz[new_size].Push(genotype);
  if (new_size > m_best) m_best = new_size;
  
  if (!genotype->IsThreshold() && (new_size >= m_world->GetConfig().THRESHOLD.Get() || genotype == getBest())) {
    genotype->SetThreshold();
    genotype->SetName(nameGenotype(new_size));
    
    // @TODO - handle threshold triggers
  }
}



const tArray<cString>& cBGGenotypeManager::GetBioGroupPropertyList() const
{
  if (!m_dcm) buildDataCommandManager();
  return m_dcm->GetEntryNames();
}

bool cBGGenotypeManager::BioGroupHasProperty(const cString& prop) const
{
  if (!m_dcm) buildDataCommandManager();
  return (m_dcm->GetDataCommand(prop));
}

cFlexVar cBGGenotypeManager::GetBioGroupProperty(const cBGGenotype* genotype, const cString& prop) const
{
  if (!m_dcm) buildDataCommandManager();
  tDataEntryCommand<cBGGenotype>* dc = m_dcm->GetDataCommand(prop);
  
  if (dc) return dc->GetValue(genotype);
  
  return cFlexVar();
}



unsigned int cBGGenotypeManager::hashGenome(const cGenome& genome) const
{
  unsigned int total = 0;
  
  for (int i = 0; i < genome.GetSize(); i++) {
    total += (genome[i].GetOp() + 3) * i;
  }
  
  return total % nBGGenotypeManager::HASH_SIZE;
}

cString cBGGenotypeManager::nameGenotype(int size)
{
  if (m_sz_count.GetSize() <= size) m_sz_count.Resize(size + 1, 0);
  int num = m_sz_count[size]++;
  
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
    genotype->Deactivate(m_world->GetStats().GetUpdate());
    m_historic.Push(genotype);
    m_active_count--;
  }
  
  if (genotype->IsThreshold()) {
    // @TODO handle threshold removal
    m_world->GetStats().RemoveThreshold();
    genotype->ClearThreshold();
  }
  
  if (genotype->GetPassiveReferenceCount()) return;
  
  const tArray<cBGGenotype*>& parents = genotype->GetParents();
  for (int i = 0; i < parents.GetSize(); i++) {
    parents[i]->RemovePassiveReference();
    updateCoalescent();
    removeGenotype(parents[i]);
  }
  
  m_historic.Remove(genotype);
  delete genotype;
}

void cBGGenotypeManager::updateCoalescent()
{
  if (m_coalescent && (m_coalescent->GetActiveReferenceCount() > 0 || m_coalescent->GetPassiveReferenceCount() > 1)) return;
  
  if (m_best == 0) {
    m_coalescent = NULL;
    // m_world->GetStats().SetCoalescentGenotypeDepth(-1);
    return;
  }
  
  // @TODO - update coalescent assumes asexual population
  cBGGenotype* test_gen = getBest();
  cBGGenotype* found_gen = test_gen;
  cBGGenotype* parent_gen = (found_gen->GetParents().GetSize()) ? found_gen->GetParents()[0] : NULL;

  while (parent_gen != NULL) {
    if (test_gen->GetActiveReferenceCount() > 0 || test_gen->GetPassiveReferenceCount() > 1) found_gen = test_gen;
    
    test_gen = parent_gen;
    parent_gen = (test_gen->GetParents().GetSize()) ? test_gen->GetParents()[0] : NULL;
  }
  
  m_coalescent = found_gen;
  // m_world->GetStats().SetCoalescentGenotypeDepth(m_coalescent->GetDepth());
}

void cBGGenotypeManager::buildDataCommandManager() const
{
  m_dcm = new tDataCommandManager<cBGGenotype>;
  
#define ADD_PROP(NAME, TYPE, GET, DESC) \
  m_dcm->Add(NAME, new tDataEntryOfType<cBGGenotype, TYPE>(NAME, DESC, &cBGGenotype::GET));

  ADD_PROP("genome", cString (), GetGenomeString, "Genome");
  ADD_PROP("parents", const cString& (), GetParentString, "Parents");
  ADD_PROP("update_born", int (), GetUpdateBorn, "Update Born");
}
