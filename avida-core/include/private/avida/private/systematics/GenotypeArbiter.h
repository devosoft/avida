/*
 *  private/systematics/GenotypeArbiter.h
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

#ifndef AvidaSystematicsGenotypeArbiter_h
#define AvidaSystematicsGenotypeArbiter_h

#include "avida/core/Properties.h"
#include "avida/data/Provider.h"
#include "avida/environment/Types.h"
#include "avida/systematics/Arbiter.h"

#include "avida/private/systematics/Genotype.h"


namespace Avida {
  namespace Systematics {
    
    // Genotype
    // --------------------------------------------------------------------------------------------------------------
    
    class GenotypeArbiter : public Arbiter, public Data::Provider
    {
      friend class Genotype;
    public:
      enum {
        EVENT_ADD_THRESHOLD,
        EVENT_REMOVE_THRESHOLD
      };
      
      static const int HASH_SIZE = 3203;
      
    private:
      // Config Settings
      int m_threshold;
      bool m_disable_class;
      
      // Internal Data Structures
      Apto::List<GenotypePtr, Apto::SparseVector> m_active_hash[HASH_SIZE];
      Apto::Array<Apto::List<GenotypePtr, Apto::SparseVector>, Apto::ManagedPointer> m_active_sz;
      Apto::List<GenotypePtr, Apto::SparseVector> m_historic;
      GenotypePtr m_coalescent;
      int m_best;
      int m_next_id;
      int m_dom_prev;
      int m_dom_time;
      Apto::Array<int> m_sz_count;
      
      Update m_cur_update;
      
      // Stats
      int m_tot_genotypes;
      
      int m_num_genotypes;
      int m_num_historic_genotypes;
      
      int m_num_threshold;
      int m_tot_threshold;
      
      int m_coalescent_depth;
      
      double m_ave_age;
      double m_ave_abundance;
      double m_ave_depth;
      double m_ave_size;
      double m_ave_threshold_age;

      double m_stderr_age;
      double m_stderr_abundance;
      double m_stderr_depth;
      double m_stderr_size;
      double m_stderr_threshold_age;

      double m_var_age;
      double m_var_abundance;
      double m_var_depth;
      double m_var_size;
      double m_var_threshold_age;

      double m_entropy;
      
      int m_dom_id;
      
      Apto::Array<PropertyID> m_env_action_average;
      Apto::Array<PropertyID> m_env_action_count;
      

      struct ProvidedData
      {
        Apto::String description;
        Apto::Functor<Data::PackagePtr, Apto::NullType> GetData;
        
        ProvidedData() { ; }
        ProvidedData(const Apto::String& desc, Apto::Functor<Data::PackagePtr, Apto::NullType> func)
          : description(desc), GetData(func) { ; } 
      };
      Apto::Map<Data::DataID, ProvidedData> m_provided_data;
      mutable Data::ConstDataSetPtr m_provides;
      
      
    public:
      GenotypeArbiter(World* world, const RoleID& role, int threshold, bool disable_class = false);
      ~GenotypeArbiter();
      
      // Arbiter Interface Methods
      GroupPtr ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints);
      GroupPtr Group(GroupID g_id);
      
      void PerformUpdate(Context& ctx, Update current_update);
      
      bool Serialize(ArchivePtr ar) const;
      bool LegacySave(void* df) const;
      GroupPtr LegacyLoad(void* props);
      
      IteratorPtr Begin();
      
      
      // Data::Provider
      Data::ConstDataSetPtr Provides() const;
      void UpdateProvidedValues(Update current_update);
      Data::PackagePtr GetProvidedValue(const Data::DataID& data_id) const;
      Apto::String DescribeProvidedValue(const Data::DataID& data_id) const;
      
      
    private:
      // Methods called by Genotype
      GenotypePtr ClassifyNewUnit(UnitPtr bu, ConstGroupMembershipPtr parents, const ClassificationHints* hints = NULL);
      void AdjustGenotype(GenotypePtr genotype, int old_size, int new_size);
      
      inline int NumEnvironmentActionTriggers() const { return m_env_action_count.GetSize(); }
      inline const Apto::Array<PropertyID>& EnvironmentActionTriggerAverageIDs() const { return m_env_action_average; }
      inline const Apto::Array<PropertyID>& EnvironmentActionTriggerCountIDs() const { return m_env_action_count; }
      
      void PrintListStatus();
      
    private:
      void setupProvidedData(World* world);
      template <class T> Data::PackagePtr packageData(const T&) const;
      Data::ProviderPtr activateProvider(World*);
      
      unsigned int hashGenome(const InstructionSequence& genome) const;
      Apto::String nameGenotype(int size);
      
      void removeGenotype(GenotypePtr genotype);
      void updateCoalescent();
      
      inline void resizeActiveList(int size);
      inline GenotypePtr getBest();
      
      inline GenotypeArbiterPtr thisPtr();
      
      class GenotypeIterator : public Iterator
      {
      private:
        GenotypeArbiterPtr m_bgm;
        int m_sz_i;
        Apto::List<GenotypePtr, Apto::SparseVector>::Iterator m_it;
                
      public:
        GenotypeIterator(GenotypeArbiterPtr bgm)
          : m_bgm(bgm), m_sz_i(bgm->m_best), m_it(m_bgm->m_active_sz[m_sz_i].Begin()) { ; }
        ~GenotypeIterator() { ; }
        
        GroupPtr Get();
        GroupPtr Next();
      };
    };


    inline void GenotypeArbiter::resizeActiveList(int size)
    {
      if (m_active_sz.GetSize() <= size) m_active_sz.Resize(size + 1);
    }

    inline GenotypePtr GenotypeArbiter::getBest()
    {
      return (m_best) ? m_active_sz[m_best].GetFirst() : GenotypePtr(NULL);
    }

  };
};

#endif
