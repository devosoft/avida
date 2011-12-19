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
#include "avida/systematics/Arbiter.h"
#include "avida/private/systematics/Genotype.h"

#include "tIterator.h"
#include "tManagedPointerArray.h"
#include "tSparseVectorList.h"

namespace Avida {
  class Sequence;
}
class cWorld;


namespace nBGGenotypeManager {
  const int HASH_SIZE = 3203;
}



namespace Avida {
  namespace Systematics {
    
    // Genotype
    // --------------------------------------------------------------------------------------------------------------
    
    class GenotypeArbiter : public Arbiter, public Data::Provider
    {
    public:
      enum {
        EVENT_ADD_THRESHOLD,
        EVENT_REMOVE_THRESHOLD
      };
      
    private:
      // Config Settings
      int m_threshold;
      
      // Internal Data Structures
      Apto::List<GenotypePtr, Apto::SparseVector> m_active_hash[nBGGenotypeManager::HASH_SIZE];
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
      int m_coalescent_depth;
      

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
      GenotypeArbiter(int threshold);
      ~GenotypeArbiter();
      
      // Arbiter Interface Methods
      GroupPtr ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints);
      GroupPtr Group(GroupID g_id);
      
      void PerformUpdate(Context& ctx, Update current_update);
      
      bool Serialize(ArchivePtr ar) const;
      
      IteratorPtr Begin();
      
      
      // Data::Provider
      Data::ConstDataSetPtr Provides() const;
      void UpdateProvidedValues(Update current_update);
      Data::PackagePtr GetProvidedValue(const Data::DataID& data_id) const;
      Apto::String DescribeProvidedValue(const Data::DataID& data_id) const;
      
      
      // Genotype Manager Methods
      GenotypePtr ClassifyNewUnit(UnitPtr bu, ConstGroupMembershipPtr parents, const ClassificationHints* hints = NULL);
      void AdjustGenotype(GenotypePtr genotype, int old_size, int new_size);
      
      
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
