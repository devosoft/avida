/*
 *  private/systematics/CladeArbiter.h
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

#ifndef AvidaSystematicsCladeArbiter_h
#define AvidaSystematicsCladeArbiter_h

#include "avida/core/Properties.h"
#include "avida/data/Provider.h"
#include "avida/environment/Types.h"
#include "avida/systematics/Arbiter.h"

#include "avida/private/systematics/Clade.h"


namespace Avida {
  namespace Systematics {
    
    // Genotype
    // --------------------------------------------------------------------------------------------------------------
    
    class CladeArbiter : public Arbiter, public Data::Provider
    {
      friend class Clade;
      
    private:
      
      // Internal Data Structures
      Apto::Map<Apto::String, CladePtr> m_clades;
      int m_next_id;
      Update m_cur_update;
      
      // Stats
      int m_tot_clades;
            
      double m_ave_abundance;
      double m_stderr_abundance;
      double m_var_abundance;
      
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
      CladeArbiter(World* world, const RoleID& role);
      ~CladeArbiter();
      
      GroupPtr GroupWithName(const Apto::String& name);
      
      // Arbiter Interface Methods
      GroupPtr ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints);
      GroupPtr Group(GroupID g_id);
      
      void PerformUpdate(Context& ctx, Update current_update);
      
      bool Serialize(ArchivePtr ar) const;
      GroupPtr LegacyLoad(void* props);
      
      IteratorPtr Begin();
      
      
      // Data::Provider
      Data::ConstDataSetPtr Provides() const;
      void UpdateProvidedValues(Update current_update);
      Data::PackagePtr GetProvidedValue(const Data::DataID& data_id) const;
      Apto::String DescribeProvidedValue(const Data::DataID& data_id) const;

    private:
      // Methods called by Clade
      void AdjustClade(CladePtr clade, int old_size, int new_size);
      
    private:
      void setupProvidedData(World* world);
      template <class T> Data::PackagePtr packageData(const T&) const;
      Data::ProviderPtr activateProvider(World*);
      
      void removeClade(CladePtr genotype);
      
      inline CladeArbiterPtr thisPtr();
      
      class CladeIterator : public Iterator
      {
      private:
        CladeArbiterPtr m_bgm;
        Apto::Map<Apto::String, CladePtr>::ValueIterator m_it;
        
      public:
        CladeIterator(CladeArbiterPtr bgm)
        : m_bgm(bgm), m_it(m_bgm->m_clades.Values()) { ; }
        ~CladeIterator() { ; }
        
        GroupPtr Get();
        GroupPtr Next();
      };
    };
    
    
  };
};

#endif
