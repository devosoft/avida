/*
 *  cBGGenotypeManager.cc
 *  Avida
 *
 *  Created by David on 11/11/09.
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

#include "cBGGenotypeManager.h"

#include "cBGGenotype.h"
#include "cDataFile.h"
#include "cSequence.h"
#include "cStats.h"
#include "cStringUtil.h"
#include "cWorld.h"
#include "tArrayMap.h"
#include "tAutoRelease.h"
#include "tDataCommandManager.h"

using namespace AvidaTools;


cBGGenotypeManager::cBGGenotypeManager(cWorld* world)
  : m_world(world)
  , m_active_sz(1)
  , m_coalescent(NULL)
  , m_best(0)
  , m_next_id(1)
  , m_dom_prev(-1)
  , m_dom_time(0)
  , m_dcm(NULL)
{
}

cBGGenotypeManager::~cBGGenotypeManager()
{
  tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_sz[0].Iterator());
  while (list_it->Next() != NULL) {
    assert(list_it->Get()->GetActiveReferenceCount() == 0);
    removeGenotype(list_it->Get());
  }
  
  assert(m_historic.GetSize() == 0);
  assert(m_best == 0);
  delete m_dcm;
}


cBioGroup* cBGGenotypeManager::ClassifyNewBioUnit(cBioUnit* bu, tArrayMap<cString, cString>* hints) { return ClassifyNewBioUnit(bu, NULL, hints); }


void cBGGenotypeManager::UpdateReset()
{
  if (m_active_sz.GetSize() < nBGGenotypeManager::HASH_SIZE) {
    for (int i = 0; i < m_active_sz.GetSize(); i++) {
      tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_sz[i].Iterator());
      while (list_it->Next() != NULL) if (list_it->Get()->IsThreshold()) list_it->Get()->UpdateReset();
    }
  } else {
    for (int i = 0; i < nBGGenotypeManager::HASH_SIZE; i++) {
      tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_hash[i].Iterator());
      while (list_it->Next() != NULL) if (list_it->Get()->IsThreshold()) list_it->Get()->UpdateReset();
    }    
  }

  tAutoRelease<tIterator<cBGGenotype> > list_it(m_historic.Iterator());
  while (list_it->Next() != NULL) if (!list_it->Get()->GetReferenceCount()) this->removeGenotype(list_it->Get());
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
  int active_count = 0;
  for (int i = 1; i < m_active_sz.GetSize(); i++) {
    active_count += m_active_sz[i].GetSize();
    tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_sz[i].Iterator());
    while (list_it->Next() != NULL) {
      cBGGenotype* bg = list_it->Get();
      const int abundance = bg->GetNumUnits();
      
      // Update stats...
      const int age = stats.GetUpdate() - bg->GetUpdateBorn();
      stats.SumGenotypeAge().Add(age, abundance);
      stats.SumAbundance().Add(abundance);
      stats.SumGenotypeDepth().Add(bg->GetDepth(), abundance);
      stats.SumSize().Add(bg->GetGenome().GetSequence().GetSize(), abundance);
      
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
  stats.SetNumGenotypes(active_count, m_historic.GetSize());
  
  
  // Handle dominant genotype stats
  cBGGenotype* dom_genotype = getBest();
  if (dom_genotype == NULL) return;
  
  stats.SetDomMerit(dom_genotype->GetMerit());
  stats.SetDomGestation(dom_genotype->GetGestationTime());
  stats.SetDomReproRate(dom_genotype->GetReproRate());
  stats.SetDomFitness(dom_genotype->GetFitness());
  stats.SetDomCopiedSize(dom_genotype->GetCopiedSize());
  stats.SetDomExeSize(dom_genotype->GetExecutedSize());
  
  stats.SetDomSize(dom_genotype->GetGenome().GetSequence().GetSize());
  stats.SetDomID(dom_genotype->GetID());
  stats.SetDomName(dom_genotype->GetName());
  
  if (dom_genotype->IsThreshold()) {
    stats.SetDomBirths(dom_genotype->GetLastBirths());
    stats.SetDomBreedTrue(dom_genotype->GetLastBreedTrue());
    stats.SetDomBreedIn(dom_genotype->GetLastBreedIn());
    stats.SetDomBreedOut(dom_genotype->GetLastBreedOut());
  } else {
    stats.SetDomBirths(dom_genotype->GetThisBirths());
    stats.SetDomBreedTrue(dom_genotype->GetThisBreedTrue());
    stats.SetDomBreedIn(dom_genotype->GetThisBreedIn());
    stats.SetDomBreedOut(dom_genotype->GetThisBreedOut());
  }
  
  stats.SetDomAbundance(dom_genotype->GetNumUnits());
  stats.SetDomGeneDepth(dom_genotype->GetDepth());
  stats.SetDomSequence(dom_genotype->GetGenome().GetSequence().AsString());
  
}



cBioGroup* cBGGenotypeManager::GetBioGroup(int bg_id)
{
  for (int i = m_best; i >= 0; i--) {
    tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_sz[i].Iterator());
    while (list_it->Next() != NULL) if (list_it->Get()->GetID() == bg_id) return list_it->Get();
  }
  
  tAutoRelease<tIterator<cBGGenotype> > list_it(m_historic.Iterator());
  while (list_it->Next() != NULL) if (list_it->Get()->GetID() == bg_id) return list_it->Get();
  
  return NULL;
}


cBioGroup* cBGGenotypeManager::LoadBioGroup(const tDictionary<cString>& props)
{
  cBGGenotype* bg = new cBGGenotype(this, m_next_id++, props, m_world); 
  m_historic.Push(bg, &bg->m_handle);
  return bg;
}


void cBGGenotypeManager::SaveBioGroups(cDataFile& df)
{
  // @TODO - Just dump historic for now.  Need structured output format to support top down save
  //         With a structured save (and save params passed through), a "structured population save" could be attained
  //         by simply calling the bio group save.  As it stands right now, cPopulation must decorate columns with additional
  //         data about active genotypes, yet the bio group interface really shouldn't know about active/inactive genotypes.
  //         Thus it is not proper to split bgm save into a save historic and save active.  Right now we'll just make
  //         cPopulation do the work.
  
  tAutoRelease<tIterator<cBGGenotype> > list_it(m_historic.Iterator());
  while (list_it->Next() != NULL) {
    list_it->Get()->Save(df);
    df.Endl();
  }
}


tIterator<cBioGroup>* cBGGenotypeManager::Iterator()
{
  return new cGenotypeIterator(this);
}



cBGGenotype* cBGGenotypeManager::ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents, tArrayMap<cString, cString>* hints)
{
  int list_num = hashGenome(bu->GetGenome().GetSequence());
  
  cBGGenotype* found = NULL;

  cString gid_str;
  if (hints && hints->Get("id", gid_str)) {
    int gid = gid_str.AsInt();
    
    // Search all lists attempting to locate the referenced genotype by ID
    for (int i = 0; i < m_active_sz.GetSize() && !found; i++) {
      tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_sz[i].Iterator());
      while (list_it->Next() != NULL) {
        if (list_it->Get()->GetID() == gid) {
          found = list_it->Get();
          found->NotifyNewBioUnit(bu);
          break;
        }
      }
    }
    
    if (!found) {
      tAutoRelease<tIterator<cBGGenotype> > list_it(m_historic.Iterator());
      while (list_it->Next() != NULL) {
        if (list_it->Get()->GetID() == gid) {
          found = list_it->Get();
          m_active_hash[hashGenome(found->GetGenome().GetSequence())].Push(found);
          found->m_handle->Remove(); // Remove from historic list
          m_active_sz[found->GetNumUnits()].PushRear(found, &found->m_handle);
          found->NotifyNewBioUnit(bu);
          m_world->GetStats().AddGenotype();
          if (found->GetNumUnits() > m_best) {
            m_best = found->GetNumUnits();
            found->SetThreshold();
            found->SetName(nameGenotype(found->GetGenome().GetSequence().GetSize()));
            NotifyListeners(found, BG_EVENT_ADD_THRESHOLD);
          }          
        }
      }
    }
  } 
  
  // No hints or unable to locate hinted genome, search for a matching genotype
  if (!found) {
    tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_hash[list_num].Iterator());
    while (list_it->Next() != NULL) {
      if (list_it->Get()->Matches(bu)) {
        found = list_it->Get();
        found->NotifyNewBioUnit(bu);
        break;
      }
    }
  }
  
  // No matching genotype (hinted or otherwise), so create a new one
  if (!found) {
    found = new cBGGenotype(this, m_next_id++, bu, m_world->GetStats().GetUpdate(), parents);
    m_active_hash[list_num].Push(found);
    resizeActiveList(found->GetNumUnits());
    m_active_sz[found->GetNumUnits()].PushRear(found, &found->m_handle);
    m_world->GetStats().AddGenotype();
    if (found->GetNumUnits() > m_best) {
      m_best = found->GetNumUnits();
      found->SetThreshold();
      found->SetName(nameGenotype(found->GetGenome().GetSequence().GetSize()));
      NotifyListeners(found, BG_EVENT_ADD_THRESHOLD);
    }
  }
  
  return found;
}


void cBGGenotypeManager::AdjustGenotype(cBGGenotype* genotype, int old_size, int new_size)
{
  // Remove from old size list
  genotype->m_handle->Remove();
  if (m_coalescent == genotype) m_coalescent = NULL;

  // Handle best genotype pointer
  bool was_best = (old_size && old_size == m_best);
  if (was_best && m_active_sz[old_size].GetSize() == 0) {
    for (m_best--; m_best > 0; m_best--) if (m_active_sz[m_best].GetSize()) break;
  }
  
  // Handle defunct genotypes
  if (new_size == 0 && genotype->GetActiveReferenceCount() == 0) {
    removeGenotype(genotype);
    return;
  }
  
  // Add to new size list
  resizeActiveList(new_size);
  if (was_best && m_best == new_size) {
    // Special case to keep the current best genotype as best when shrinking to the same size as other genotypes
    m_active_sz[new_size].Push(genotype, &genotype->m_handle);
  } else {
    m_active_sz[new_size].PushRear(genotype, &genotype->m_handle);
    if (new_size > m_best) m_best = new_size;
  }
  
  if (!genotype->IsThreshold() && (new_size >= m_world->GetConfig().THRESHOLD.Get() || genotype == getBest())) {
    genotype->SetThreshold();
    genotype->SetName(nameGenotype(genotype->GetGenome().GetSequence().GetSize()));
    NotifyListeners(genotype, BG_EVENT_ADD_THRESHOLD);
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
  tAutoRelease<tDataEntryCommand<cBGGenotype> > dc(m_dcm->GetDataCommand(prop));
  return (!dc.IsNull());
}

cFlexVar cBGGenotypeManager::GetBioGroupProperty(const cBGGenotype* genotype, const cString& prop) const
{
  if (!m_dcm) buildDataCommandManager();
  tAutoRelease<tDataEntryCommand<cBGGenotype> > dc(m_dcm->GetDataCommand(prop));
  
  if (!dc.IsNull()) return dc->GetValue(genotype);
  
  return cFlexVar();
}



unsigned int cBGGenotypeManager::hashGenome(const cSequence& genome) const
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
    int list_num = hashGenome(genotype->GetGenome().GetSequence());
    m_active_hash[list_num].Remove(genotype);
    genotype->Deactivate(m_world->GetStats().GetUpdate());
    m_historic.Push(genotype, &genotype->m_handle);
  }

  if (genotype->IsThreshold()) {
    NotifyListeners(genotype, BG_EVENT_REMOVE_THRESHOLD);
    genotype->ClearThreshold();
  }
  
  if (genotype->GetPassiveReferenceCount()) return;
  
  const tArray<cBGGenotype*>& parents = genotype->GetParents();
  for (int i = 0; i < parents.GetSize(); i++) {
    parents[i]->RemovePassiveReference();
    updateCoalescent();
    
    // Pre-check for active genotypes to avoid recursion costs
    if (!parents[i]->GetActiveReferenceCount()) removeGenotype(parents[i]);
  }
  
  assert(genotype->m_handle);
  genotype->m_handle->Remove(); // Remove from historic list
  delete genotype;
}

void cBGGenotypeManager::updateCoalescent()
{
  if (m_coalescent && (m_coalescent->GetActiveReferenceCount() > 0 || m_coalescent->GetPassiveReferenceCount() > 1)) return;
  
  if (m_best == 0) {
    m_coalescent = NULL;
    m_world->GetStats().SetCoalescentGenotypeDepth(-1);
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
  m_world->GetStats().SetCoalescentGenotypeDepth(m_coalescent->GetDepth());
}

void cBGGenotypeManager::buildDataCommandManager() const
{
  m_dcm = new tDataCommandManager<cBGGenotype>;
  
#define ADD_PROP(NAME, TYPE, GET, DESC) \
  m_dcm->Add(NAME, new tDataEntryOfType<cBGGenotype, TYPE>(NAME, DESC, &cBGGenotype::GET));

  ADD_PROP("genome", cString (), GetGenomeString, "Genome");
  ADD_PROP("name", const cString& (), GetName, "Name");
  ADD_PROP("parents", const cString& (), GetParentString, "Parents");
  ADD_PROP("threshold", bool (), IsThreshold, "Threshold");  
  ADD_PROP("update_born", int (), GetUpdateBorn, "Update Born");
  ADD_PROP("fitness", double (), GetFitness, "Average Fitness");
  ADD_PROP("repro_rate", double (), GetReproRate, "Repro Rate");
  ADD_PROP("recent_births", int (), GetThisBirths, "Recent Births (during update)");
  ADD_PROP("recent_deaths", int (), GetThisDeaths, "Recent Deaths (during update)");
  ADD_PROP("recent_breed_true", int (), GetThisBreedTrue, "Recent Breed True (during update)");
  ADD_PROP("recent_breed_in", int (), GetThisBreedIn, "Recent Breed In (during update)");
  ADD_PROP("recent_breed_out", int (), GetThisBreedOut, "Recent Breed Out (during update)");
  ADD_PROP("total_organisms", int (), GetTotalOrganisms, "Total Organisms");
  ADD_PROP("last_births", int (), GetLastBirths, "Births (during last update)");
  ADD_PROP("last_breed_true", int (), GetLastBreedTrue, "Breed True (during last update)");
  ADD_PROP("last_breed_in", int (), GetLastBreedIn, "Breed In (during last update)");
  ADD_PROP("last_breed_out", int (), GetLastBreedOut, "Breed Out (during last update)");
}

cBioGroup* cBGGenotypeManager::cGenotypeIterator::Get() { return m_it->Get(); }


cBioGroup* cBGGenotypeManager::cGenotypeIterator::Next()
{
  if (!m_it->Next()) {
    for (m_sz_i--; m_sz_i > 0; m_sz_i--) {
      if (m_bgm->m_active_sz[m_sz_i].GetSize()) {
        delete m_it;
        m_it = m_bgm->m_active_sz[m_sz_i].Iterator();
        m_it->Next();
        break;
      }
    }
  }
  
  return m_it->Get();
}
