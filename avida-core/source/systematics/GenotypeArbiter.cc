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
#include "avida/environment/Manager.h"
#include "avida/output/File.h"

#include "avida/private/systematics/Genotype.h"

#include "cDoubleSum.h"

#include <cmath>


Avida::Systematics::GenotypeArbiter::GenotypeArbiter(World* world, const RoleID& role, int threshold, bool disable_class)
  : Arbiter(role)
  , m_threshold(threshold)
  , m_disable_class(disable_class)
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
  Avida::Environment::ManagerPtr env = Avida::Environment::Manager::Of(world);
  Avida::Environment::ConstActionTriggerIDSetPtr trigger_ids = env->GetActionTriggerIDs();
  m_env_action_average.Resize(trigger_ids->GetSize());
  m_env_action_count.Resize(trigger_ids->GetSize());
  int idx = 0;
  for (Avida::Environment::ConstActionTriggerIDSetIterator it = trigger_ids->Begin(); it.Next(); idx++) {
    m_env_action_average[idx] = Apto::FormatStr("environment.triggers.%s.average", (const char*)*it.Get());
    m_env_action_count[idx] = Apto::FormatStr("environment.triggers.%s.count", (const char*)*it.Get());
  }
  setupProvidedData(world);
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


void Avida::Systematics::GenotypeArbiter::PerformUpdate(Context&, Update current_update)
{
  m_cur_update = current_update + 1; // +1 since PerformUpdate happens at end of updates, but m_cur_update is used during
  
  if (m_active_sz.GetSize() < HASH_SIZE) {
    for (int i = 0; i < m_active_sz.GetSize(); i++) {
      Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_sz[i].Begin());
      while (list_it.Next() != NULL) if ((*list_it.Get())->IsThreshold()) (*list_it.Get())->UpdateReset();
    }
  } else {
    for (int i = 0; i < HASH_SIZE; i++) {
      Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_hash[i].Begin());
      while (list_it.Next() != NULL) if ((*list_it.Get())->IsThreshold()) (*list_it.Get())->UpdateReset();
    }    
  }

  Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_historic.Begin());
  while (list_it.Next() != NULL) if (!(*list_it.Get())->ReferenceCount()) removeGenotype(*list_it.Get());
}

void Avida::Systematics::GenotypeArbiter::PrintListStatus()
{
  Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it2(m_historic.Begin());
  printf("genotype_historic: ");
  while (list_it2.Next() != NULL) {
    printf("%d, ", (*list_it2.Get())->ID());
  }
  printf("\n\n");

}

bool Avida::Systematics::GenotypeArbiter::Serialize(ArchivePtr) const
{
  // @TOOD - serialize genotype arbiter
  assert(false);
  return false;
}

bool Avida::Systematics::GenotypeArbiter::LegacySave(void* dfp) const
{
  Apto::List<GenotypePtr, Apto::SparseVector>::ConstIterator list_it(m_historic.Begin());
  while (list_it.Next() != NULL) {
    (*list_it.Get())->LegacySave(dfp);
    static_cast<Avida::Output::File*>(dfp)->Endl();
  }
  return true;
}

Avida::Systematics::GroupPtr Avida::Systematics::GenotypeArbiter::LegacyLoad(void* props)
{
  GenotypePtr g(new Genotype(thisPtr(), m_next_id++, props));
  m_historic.Push(g, &g->m_handle);
  return g;
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
  return IteratorPtr(new GenotypeIterator(thisPtr()));
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
  cDoubleSum sum_age;
  cDoubleSum sum_abundance;
  cDoubleSum sum_depth;
  cDoubleSum sum_size;
  cDoubleSum sum_threshold_age;
  
  // Pre-calculate the total number of units that are currently active (used in entropy calculation)
  int tot_units = 0;
  for (int i = 1; i < m_active_sz.GetSize(); i++) {
    Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_sz[i].Begin());
    while (list_it.Next()) tot_units += (*list_it.Get())->NumUnits();
  }
  
  // Loop through all genotypes collecting statistics
  m_entropy = 0.0;
  int active_count = 0;
  for (int i = 1; i < m_active_sz.GetSize(); i++) {
    active_count += m_active_sz[i].GetSize();
    Apto::List<GenotypePtr, Apto::SparseVector>::Iterator list_it(m_active_sz[i].Begin());
    while (list_it.Next()) {
      GenotypePtr bg = *list_it.Get();
      const int abundance = bg->NumUnits();
      
      // Update stats...
      const int age = current_update - bg->GetUpdateBorn();
      sum_age.Add(age, abundance);
      sum_abundance.Add(abundance);
      sum_depth.Add(bg->Depth(), abundance);
      
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(bg->GroupGenome().Representation());
      assert(seq);
      sum_size.Add(seq->GetSize(), abundance);
      
      // Calculate this genotype's contribution to entropy
      // - when p = 1.0, partial_ent calculation would return -0.0. This may propagate
      //   to the output stage, but behavior is dependent on compiler used and optimization
      //   level.  For consistent output, ensures that 0.0 is returned.
      const double p = ((double) abundance) / (double) tot_units;
      const double partial_ent = (abundance == tot_units) ? 0.0 : -(p * log(p)); 
      m_entropy += partial_ent;
      
      // Do any special calculations for threshold genotypes.
      if (bg->IsThreshold()) sum_threshold_age.Add(age, abundance);
    }
  }
  
  // Stash all stats so that the can be retrieved using the provider mechanisms
  m_num_genotypes = active_count;
  m_num_historic_genotypes = m_historic.GetSize();
  
  m_ave_age = sum_age.Average();
  m_ave_abundance = sum_abundance.Average();
  m_ave_depth = sum_depth.Average();
  m_ave_size = sum_size.Average();
  m_ave_threshold_age = sum_threshold_age.Average();
  
  m_stderr_age = sum_age.StdError();
  m_stderr_abundance = sum_abundance.StdError();
  m_stderr_depth = sum_depth.StdError();
  m_stderr_size = sum_size.StdError();
  m_stderr_threshold_age = sum_threshold_age.StdError();
  
  m_var_age = sum_age.Variance();
  m_var_abundance = sum_abundance.Variance();
  m_var_depth = sum_depth.Variance();
  m_var_size = sum_size.Variance();
  m_var_threshold_age = sum_threshold_age.Variance();
  
  m_dom_id = (getBest()) ? getBest()->ID() : -1;  
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
  seq.DynamicCastFrom(u->UnitGenome().Representation());
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
          seq.DynamicCastFrom(found->GroupGenome().Representation());
          assert(seq);
          
          m_active_hash[hashGenome(*seq)].Push(found);
          found->m_handle->Remove(); // Remove from historic list
          resizeActiveList(found->NumUnits());
          m_active_sz[found->NumUnits()].PushRear(found, &found->m_handle);
          found->Reactivate();
          found->NotifyNewUnit(u);
          m_tot_genotypes++;
          if (found->NumUnits() > m_best) {
            m_best = found->NumUnits();
            found->SetThreshold();
            found->SetName(nameGenotype(seq->GetSize()));
            m_num_threshold++;
            m_tot_threshold++;
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
    if (!m_disable_class) { // It's not enabled, so keep the parents
      found = GenotypePtr(new Genotype(thisPtr(), m_next_id++, u, m_cur_update, parents));
    } else {
      found = GenotypePtr(new Genotype(thisPtr(), m_next_id++, u, m_cur_update, ConstGroupMembershipPtr(NULL)));
    }
    m_active_hash[list_num].Push(found);
    resizeActiveList(found->NumUnits());
    m_active_sz[found->NumUnits()].PushRear(found, &found->m_handle);
    m_tot_genotypes++;
    if (found->NumUnits() > m_best) {
      m_best = found->NumUnits();
      found->SetThreshold();
      seq.DynamicCastFrom(found->GroupGenome().Representation());
      assert(seq);
      found->SetName(nameGenotype(seq->GetSize()));
      m_num_threshold++;
      m_tot_threshold++;
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
    ConstInstructionSequencePtr seq;
    seq.DynamicCastFrom(genotype->GroupGenome().Representation());
    assert(seq);
    genotype->SetName(nameGenotype(seq->GetSize()));
    m_num_threshold++;
    m_tot_threshold++;
    notifyListeners(genotype, EVENT_ADD_THRESHOLD);
  }
}

template <class T> Avida::Data::PackagePtr Avida::Systematics::GenotypeArbiter::packageData(const T& val) const
{
  return Data::PackagePtr(new Data::Wrap<T>(val));
}

Avida::Data::ProviderPtr Avida::Systematics::GenotypeArbiter::activateProvider(World*) 
{
  return thisPtr();
}


void Avida::Systematics::GenotypeArbiter::setupProvidedData(World* world)
{
  // Setup functors and references for use in the PROVIDE macro
  Data::ProviderActivateFunctor activate(this, &GenotypeArbiter::activateProvider);
  Data::ManagerPtr mgr = Data::Manager::Of(world);
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<const int&> > intStat(this, &GenotypeArbiter::packageData<int>);
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<const double&> > doubleStat(this, &GenotypeArbiter::packageData<double>);

  // Define PROVIDE macro to simplify instantiating new provided data
#define PROVIDE(name, desc, type, val) { Apto::String pvn = Apto::String("systematics.") + Role() + "." + name; \
  m_provided_data[pvn] = ProvidedData(desc, Apto::BindFirst(type ## Stat, val));\
  mgr->Register(pvn, activate); \
}

  PROVIDE("total", "Total Number of Genotypes", int, m_tot_genotypes);
  PROVIDE("current", "Number of Current Genotypes", int, m_num_genotypes);
  PROVIDE("ancestral", "Number of Ancestral Genotypes", int, m_num_historic_genotypes);

  PROVIDE("total_threshold", "Total Number of Threshold Genotypes", int, m_tot_threshold);
  PROVIDE("current_threshold", "Number of Current Threshold Genotypes", int, m_num_threshold);

  PROVIDE("coalescent_depth", "Coalescent Depth", int, m_coalescent_depth);
  
  PROVIDE("ave_age", "Average Age", double, m_ave_age);
  PROVIDE("ave_abundance", "Average Abundance", double, m_ave_abundance);
  PROVIDE("ave_depth", "Average Depth", double, m_ave_depth);
  PROVIDE("ave_size", "Average ", double, m_ave_size);
  PROVIDE("ave_threshold_age", "Average Threshold Age", double, m_ave_threshold_age);
  
  PROVIDE("stderr_age", "Age Standard Error", double, m_stderr_age);
  PROVIDE("stderr_abundance", "Abundance Standard Error", double, m_stderr_abundance);
  PROVIDE("stderr_depth", "Depth Standard Error", double, m_stderr_depth);
  PROVIDE("stderr_size", "Size Standard Error", double, m_stderr_size);
  PROVIDE("stderr_threshold_age", "Threshold Age Standard Error", double, m_stderr_threshold_age);
  
  PROVIDE("var_age", "Age Variance", double, m_var_age);
  PROVIDE("var_abundance", "Abundance Variance", double, m_var_abundance);
  PROVIDE("var_depth", "Depth Variance", double, m_var_depth);
  PROVIDE("var_size", "Size Variance", double, m_var_size);
  PROVIDE("var_threshold_age", "Threshold Age Variance", double, m_var_threshold_age);
  
  PROVIDE("entropy", "Genotypic Entropy", double, m_entropy);
  
  PROVIDE("dominant_id", "Dominant Genotype ID", int, m_dom_id);
}



unsigned int Avida::Systematics::GenotypeArbiter::hashGenome(const InstructionSequence& genome) const
{
  unsigned int total = 0;
  
  for (int i = 0; i < genome.GetSize(); i++) {
    total += (genome[i].GetOp() + 3) * i;
  }
  
  return total % HASH_SIZE;
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
    ConstInstructionSequencePtr seq;
    seq.DynamicCastFrom(genotype->GroupGenome().Representation());
    int list_num = hashGenome(*seq);
    m_active_hash[list_num].Remove(genotype);
    genotype->Deactivate(m_cur_update);
    m_historic.Push(genotype, &genotype->m_handle);
  }

  if (genotype->IsThreshold()) {
    m_num_threshold--;
    notifyListeners(genotype, EVENT_REMOVE_THRESHOLD);
    genotype->ClearThreshold();
  }
  
  if (genotype->PassiveReferenceCount()) return;
    
  const Apto::Array<GenotypePtr>& parents = genotype->Parents();
  for (int i = 0; i < parents.GetSize(); i++) {
    parents[i]->RemovePassiveReference();
    updateCoalescent();
    
    // Pre-check for active genotypes to avoid recursion costs
    if (!parents[i]->ActiveReferenceCount()) removeGenotype(parents[i]);
  }
  
  assert(genotype->m_handle);
  genotype->m_handle->Remove(); // Remove from historic list
  
  delete genotype->m_handle;
  genotype->m_handle = NULL;
}

void Avida::Systematics::GenotypeArbiter::updateCoalescent()
{
  if (m_coalescent && (m_coalescent->ActiveReferenceCount() > 0 || m_coalescent->PassiveReferenceCount() > 1)) return;
  
  if (m_best == 0) {
    m_coalescent = GenotypePtr(NULL);
    m_coalescent_depth = -1;
    return;
  }
  
  // @note - update coalescent assumes asexual population
  GenotypePtr test_gen = getBest();
  GenotypePtr found_gen = test_gen;
  GenotypePtr parent_gen = (found_gen->Parents().GetSize()) ? (found_gen->Parents()[0]) : GenotypePtr(NULL);

  while (parent_gen) {
    if (test_gen->ActiveReferenceCount() > 0 || test_gen->PassiveReferenceCount() > 1) found_gen = test_gen;
    
    test_gen = parent_gen;
    parent_gen = (test_gen->Parents().GetSize()) ? (test_gen->Parents()[0]) : GenotypePtr(NULL);
  }
  
  m_coalescent = found_gen;
  m_coalescent_depth = m_coalescent->Depth();
}


inline Avida::Systematics::GenotypeArbiterPtr Avida::Systematics::GenotypeArbiter::thisPtr()
{
  AddReference(); // Explicitly add reference for newly created SmartPtr
  return GenotypeArbiterPtr(this);
}


Avida::Systematics::GroupPtr Avida::Systematics::GenotypeArbiter::GenotypeIterator::Get()
{
  return m_it.Get() ? (GroupPtr)*m_it.Get() : GroupPtr(NULL);
}


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
  
  return m_it.Get() ? (GroupPtr)*m_it.Get() : GroupPtr(NULL);
}
