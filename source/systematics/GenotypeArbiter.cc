/*
 *  private/systematics/GenotypeArbiter.cc
 *  Avida
 *
 *  Created by David on 11/11/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#include "avida/private/systematics/GenotypeArbiter.h"

#include "avida/core/InstructionSequence.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include "avida/private/systematics/Genotype.h"


Avida::Systematics::GenotypeArbiter::GenotypeArbiter(int threshold)
  : m_threshold(threshold)
  , m_active_sz(1)
  , m_coalescent(NULL)
  , m_best(0)
  , m_next_id(1)
  , m_dom_prev(-1)
  , m_dom_time(0)
  , m_cur_update(-1)
  , m_tot_genotypes(0)
  , m_coalescent_depth(-1)
{
}

Avida::Systematics::GenotypeArbiter::~GenotypeArbiter()
{
  // @TODO make sure this gets cleaned up last
  Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_sz[0].Begin());
  while (list_it.Next() != NULL) {
    assert((*list_it.Get())->ActiveReferenceCount() == 0);
    removeGenotype(*list_it.Get());
  }
  
  assert(m_historic.GetSize() == 0);
  assert(m_best == 0);
}


Avida::Systematics::GroupPtr Avida::Systematics::GenotypeArbiter::ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints)
{
  return ClassifyNewUnit(u, ConstGroupMembershipPtr(NULL), hints);
}


void Avida::Systematics::GenotypeArbiter::PerformUpdate(Context& ctx, Update current_update)
{
  m_cur_update = current_update;
  
  if (m_active_sz.GetSize() < nBGGenotypeManager::HASH_SIZE) {
    for (int i = 0; i < m_active_sz.GetSize(); i++) {
      Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_sz[i].Begin());
      while (list_it.Next() != NULL) if ((*list_it.Get())->IsThreshold()) (*list_it.Get())->UpdateReset();
    }
  } else {
    for (int i = 0; i < nBGGenotypeManager::HASH_SIZE; i++) {
      Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_hash[i].Begin());
      while (list_it.Next() != NULL) if ((*list_it.Get())->IsThreshold()) (*list_it.Get())->UpdateReset();
    }    
  }

  Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_historic.Begin());
  while (list_it.Next() != NULL) if (!(*list_it.Get())->ReferenceCount()) removeGenotype(*list_it.Get());
}



Avida::Systematics::GroupPtr Avida::Systematics::GenotypeArbiter::Group(GroupID g_id)
{
  for (int i = m_best; i >= 0; i--) {
    Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_sz[i].Begin());
    while (list_it.Next() != NULL) if ((*list_it.Get())->ID() == g_id) return *list_it.Get();
  }
  
  Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_historic.Begin());
  while (list_it.Next() != NULL) if ((*list_it.Get())->ID() == g_id) return *list_it.Get();
  
  return GroupPtr(NULL);
}



Avida::Systematics::Arbiter::IteratorPtr Avida::Systematics::GenotypeArbiter::Begin()
{
  GenotypeArbiterPtr a(this);
  AddReference();  // explictly add reference, since this is internally creating a smart pointer to itself
  return IteratorPtr(new GenotypeIterator(a));
}




Avida::Data::ConstDataSetPtr Avida::Systematics::GenotypeArbiter::Provides() const
{
  if (!m_provides) {
    Data::DataSetPtr provides(new Apto::Set<Apto::String>);
    for (Apto::Map<Data::DataID, ProvidedData>::KeyIterator it = m_provided_data.Keys(); it.Next();) {
      provides->Insert(*it.Get());
    }
    m_provides = provides;
  }
  return m_provides;
}

void Avida::Systematics::GenotypeArbiter::UpdateProvidedValues(Update current_update)
{
//
//  // Clear out genotype sums...
//  stats.SumGenotypeAge().Clear();
//  stats.SumAbundance().Clear();
//  stats.SumGenotypeDepth().Clear();
//  stats.SumSize().Clear();
//  stats.SumThresholdAge().Clear();
//  
//  double entropy = 0.0;
//  int active_count = 0;
//  for (int i = 1; i < m_active_sz.GetSize(); i++) {
//    active_count += m_active_sz[i].GetSize();
//    tAutoRelease<tIterator<cBGGenotype> > list_it(m_active_sz[i].Iterator());
//    while (list_it->Next() != NULL) {
//      cBGGenotype* bg = list_it->Get();
//      const int abundance = bg->GetNumUnits();
//      
//      // Update stats...
//      const int age = stats.GetUpdate() - bg->GetUpdateBorn();
//      stats.SumGenotypeAge().Add(age, abundance);
//      stats.SumAbundance().Add(abundance);
//      stats.SumGenotypeDepth().Add(bg->GetDepth(), abundance);
//      stats.SumSize().Add(bg->GetGenome().GetSequence().GetSize(), abundance);
//      
//      // Calculate this genotype's contribution to entropy
//      // - when p = 1.0, partial_ent calculation would return -0.0. This may propagate
//      //   to the output stage, but behavior is dependent on compiler used and optimization
//      //   level.  For consistent output, ensures that 0.0 is returned.
//      const double p = ((double) abundance) / (double) stats.GetNumCreatures();
//      const double partial_ent = (abundance == stats.GetNumCreatures()) ? 0.0 : -(p * Log(p)); 
//      entropy += partial_ent;
//      
//      // Do any special calculations for threshold genotypes.
//      if (bg->IsThreshold()) stats.SumThresholdAge().Add(age, abundance);
//    }
//  }
//  
//  stats.SetEntropy(entropy);
//  stats.SetNumGenotypes(active_count, m_historic.GetSize());
//  
//  
//  // Handle dominant genotype stats
//  cBGGenotype* dom_genotype = getBest();
//  if (dom_genotype == NULL) return;
//  
//  stats.SetDomMerit(dom_genotype->GetMerit());
//  stats.SetDomGestation(dom_genotype->GetGestationTime());
//  stats.SetDomReproRate(dom_genotype->GetReproRate());
//  stats.SetDomFitness(dom_genotype->GetFitness());
//  stats.SetDomCopiedSize(dom_genotype->GetCopiedSize());
//  stats.SetDomExeSize(dom_genotype->GetExecutedSize());
//  
//  stats.SetDomSize(dom_genotype->GetGenome().GetSequence().GetSize());
//  stats.SetDomID(dom_genotype->GetID());
//  stats.SetDomName(dom_genotype->GetName());
//  
//  if (dom_genotype->IsThreshold()) {
//    stats.SetDomBirths(dom_genotype->GetLastBirths());
//    stats.SetDomBreedTrue(dom_genotype->GetLastBreedTrue());
//    stats.SetDomBreedIn(dom_genotype->GetLastBreedIn());
//    stats.SetDomBreedOut(dom_genotype->GetLastBreedOut());
//  } else {
//    stats.SetDomBirths(dom_genotype->GetThisBirths());
//    stats.SetDomBreedTrue(dom_genotype->GetThisBreedTrue());
//    stats.SetDomBreedIn(dom_genotype->GetThisBreedIn());
//    stats.SetDomBreedOut(dom_genotype->GetThisBreedOut());
//  }
//  
//  stats.SetDomAbundance(dom_genotype->GetNumUnits());
//  stats.SetDomGeneDepth(dom_genotype->GetDepth());
//  stats.SetDomSequence(dom_genotype->GetGenome().GetSequence().AsString());
//  
//  stats.SetDomLastBirthCell(dom_genotype->GetLastBirthCell());
//  stats.SetDomLastGroup(dom_genotype->GetLastGroupID());
//  stats.SetDomLastForagerType(dom_genotype->GetLastForagerType());
//  
}


Avida::Data::PackagePtr Avida::Systematics::GenotypeArbiter::GetProvidedValue(const Data::DataID& data_id) const
{
  Data::PackagePtr rtn;
  ProvidedData data_entry;
  if (m_provided_data.Get(data_id, data_entry)) {
    rtn = data_entry.GetData();
  }
  assert(rtn);
  
  return rtn;
}


Apto::String Avida::Systematics::GenotypeArbiter::DescribeProvidedValue(const Data::DataID& data_id) const
{
  ProvidedData data_entry;
  Apto::String rtn;
  if (m_provided_data.Get(data_id, data_entry)) {
    rtn = data_entry.description;
  }
  assert(rtn != "");
  return rtn;
}



Avida::Systematics::GenotypePtr Avida::Systematics::GenotypeArbiter::ClassifyNewUnit(UnitPtr u,
                                                                                     ConstGroupMembershipPtr parents,
                                                                                     const ClassificationHints* hints)
{
  
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(u->Genome()->Representation());
  assert(seq);
  int list_num = hashGenome(*seq);
  
  GenotypePtr found;

  Apto::String gid_str;
  if (hints && hints->Get("id", gid_str)) {
    int gid = Apto::StrAs(gid_str);
    
    // Search all lists attempting to locate the referenced genotype by ID
    for (int i = 0; i < m_active_sz.GetSize() && !found; i++) {
      Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_sz[i].Begin());
      while (list_it.Next() != NULL) {
        if ((*list_it.Get())->ID() == gid) {
          found = *list_it.Get();
          found->NotifyNewUnit(u);
          break;
        }
      }
    }
    
    if (!found) {
      Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_historic.Begin());
      while (list_it.Next() != NULL) {
        if ((*list_it.Get())->ID() == gid) {
          found = *list_it.Get();
          seq.DynamicCastFrom(found->GetGenome().Representation());
          assert(seq);
          
          m_active_hash[hashGenome(*seq)].Push(found);
          found->m_handle->Remove(); // Remove from historic list
          m_active_sz[found->NumUnits()].PushRear(found, &found->m_handle);
          found->NotifyNewUnit(u);
          m_tot_genotypes++;
          if (found->NumUnits() > m_best) {
            m_best = found->NumUnits();
            found->SetThreshold();
            found->SetName(nameGenotype(seq->GetSize()));
            notifyListeners(found, EVENT_ADD_THRESHOLD);
          }          
        }
      }
    }
  } 
  
  // No hints or unable to locate hinted genome, search for a matching genotype
  if (!found) {
    Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_hash[list_num].Begin());
    while (list_it.Next() != NULL) {
      if ((*list_it.Get())->Matches(u)) {
        found = *list_it.Get();
        found->NotifyNewUnit(u);
        break;
      }
    }
  }
  
  // No matching genotype (hinted or otherwise), so create a new one
  if (!found) {
    GenotypeArbiterPtr a(this);
    AddReference(); // explictly add reference, since this is internally creating a smart pointer to itself
    
    found = GenotypePtr(new Genotype(a, m_next_id++, u, m_cur_update, parents));
    m_active_hash[list_num].Push(found);
    resizeActiveList(found->NumUnits());
    m_active_sz[found->NumUnits()].PushRear(found, &found->m_handle);
    m_tot_genotypes++;
    if (found->NumUnits() > m_best) {
      m_best = found->NumUnits();
      found->SetThreshold();
      seq.DynamicCastFrom(found->GetGenome().Representation());
      assert(seq);
      found->SetName(nameGenotype(seq->GetSize()));
      notifyListeners(found, EVENT_ADD_THRESHOLD);
    }
  }
  
  return found;
}


void Avida::Systematics::GenotypeArbiter::AdjustGenotype(GenotypePtr genotype, int old_size, int new_size)
{
  // Remove from old size list
  genotype->m_handle->Remove();
  if (m_coalescent == genotype) m_coalescent = GenotypePtr(NULL);

  // Handle best genotype pointer
  bool was_best = (old_size && old_size == m_best);
  if (was_best && m_active_sz[old_size].GetSize() == 0) {
    for (m_best--; m_best > 0; m_best--) if (m_active_sz[m_best].GetSize()) break;
  }
  
  // Handle defunct genotypes
  if (new_size == 0 && genotype->ActiveReferenceCount() == 0) {
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
  
  if (!genotype->IsThreshold() && (new_size >= m_threshold || genotype == getBest())) {
    genotype->SetThreshold();
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(genotype->GetGenome().Representation());
    assert(seq);
    genotype->SetName(nameGenotype(seq->GetSize()));
    notifyListeners(genotype, EVENT_ADD_THRESHOLD);
  }
}

template <class T> Avida::Data::PackagePtr Avida::Systematics::GenotypeArbiter::packageData(const T& val) const
{
  return Data::PackagePtr(new Data::Wrap<T>(val));
}

Avida::Data::ProviderPtr Avida::Systematics::GenotypeArbiter::activateProvider(World* world) 
{
  Data::ProviderPtr p(this);
  AddReference(); // explictly add reference, since this is internally creating a smart pointer to itself
  return p;
}


void Avida::Systematics::GenotypeArbiter::setupProvidedData(World* world)
{
  // Setup functors and references for use in the PROVIDE macro
  Data::ProviderActivateFunctor activate(this, &GenotypeArbiter::activateProvider);
  Data::ManagerPtr mgr = Data::Manager::Of(world);
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<const int&> > intStat(this, &GenotypeArbiter::packageData<int>);
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<const double&> > doubleStat(this, &GenotypeArbiter::packageData<double>);

  // Define PROVIDE macro to simplify instantiating new provided data
#define PROVIDE(name, desc, type, val) { \
  m_provided_data[Apto::String("systematics.") + Role() + "." + name] = ProvidedData(desc, Apto::BindFirst(type ## Stat, val));\
  mgr->Register(name, activate); \
}

  PROVIDE("", "", int, );
  
}



unsigned int Avida::Systematics::GenotypeArbiter::hashGenome(const InstructionSequence& genome) const
{
  unsigned int total = 0;
  
  for (int i = 0; i < genome.GetSize(); i++) {
    total += (genome[i].GetOp() + 3) * i;
  }
  
  return total % nBGGenotypeManager::HASH_SIZE;
}

Apto::String Avida::Systematics::GenotypeArbiter::nameGenotype(int size)
{
  if (m_sz_count.GetSize() <= size) m_sz_count.Resize(size + 1, 0);
  int num = m_sz_count[size]++;
  
  char alpha[6];
  
  for (int i = 4; i >= 0; i--) {
    alpha[i] = (num % 26) + 'a';
    num /= 26;
  }
  alpha[5] = '\0';
  
  return Apto::FormatStr("%03d-%s", size, alpha);
}

void Avida::Systematics::GenotypeArbiter::removeGenotype(GenotypePtr genotype)
{
  if (genotype->ActiveReferenceCount()) return;    
  
  if (genotype->IsActive()) {
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(genotype->GetGenome().Representation());
    int list_num = hashGenome(*seq);
    m_active_hash[list_num].Remove(genotype);
    genotype->Deactivate(m_cur_update);
    m_historic.Push(genotype, &genotype->m_handle);
  }

  if (genotype->IsThreshold()) {
    notifyListeners(genotype, EVENT_REMOVE_THRESHOLD);
    genotype->ClearThreshold();
  }
  
  if (genotype->PassiveReferenceCount()) return;
  
  const Apto::Array<GenotypePtr>& parents = genotype->GetParents();
  for (int i = 0; i < parents.GetSize(); i++) {
    parents[i]->RemovePassiveReference();
    updateCoalescent();
    
    // Pre-check for active genotypes to avoid recursion costs
    if (!parents[i]->ActiveReferenceCount()) removeGenotype(parents[i]);
  }
  
  assert(genotype->m_handle);
  genotype->m_handle->Remove(); // Remove from historic list
  delete genotype;
}

void Avida::Systematics::GenotypeArbiter::updateCoalescent()
{
  if (m_coalescent && (m_coalescent->ActiveReferenceCount() > 0 || m_coalescent->PassiveReferenceCount() > 1)) return;
  
  if (m_best == 0) {
    m_coalescent = GenotypePtr(NULL);
    m_coalescent_depth = -1;
    return;
  }
  
  // @TODO - update coalescent assumes asexual population
  GenotypePtr test_gen = getBest();
  GenotypePtr found_gen = test_gen;
  GenotypePtr parent_gen = (found_gen->GetParents().GetSize()) ? (found_gen->GetParents()[0]) : GenotypePtr(NULL);

  while (parent_gen) {
    if (test_gen->ActiveReferenceCount() > 0 || test_gen->PassiveReferenceCount() > 1) found_gen = test_gen;
    
    test_gen = parent_gen;
    parent_gen = (test_gen->GetParents().GetSize()) ? (test_gen->GetParents()[0]) : GenotypePtr(NULL);
  }
  
  m_coalescent = found_gen;
  m_coalescent_depth = m_coalescent->Depth();
}


Avida::Systematics::GroupPtr Avida::Systematics::GenotypeArbiter::GenotypeIterator::Get() { return *m_it.Get(); }


Avida::Systematics::GroupPtr Avida::Systematics::GenotypeArbiter::GenotypeIterator::Next()
{
  if (!m_it.Next()) {
    for (m_sz_i--; m_sz_i > 0; m_sz_i--) {
      if (m_bgm->m_active_sz[m_sz_i].GetSize()) {
        m_it = m_bgm->m_active_sz[m_sz_i].Begin();
        m_it.Next();
        break;
      }
    }
  }
  
  return *m_it.Get();
}
