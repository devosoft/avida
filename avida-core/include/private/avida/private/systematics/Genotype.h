/*
 *  private/systematics/Genotype.h
 *  Avida
 *
 *  Created by David on 11/5/09.
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

#ifndef AvidaSystematicsGenotype_h
#define AvidaSystematicsGenotype_h

#include "avida/core/Genome.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Unit.h"

#include "apto/stat/Accumulator.h"

#include "cCountTracker.h"
#include "cDoubleSum.h"


namespace Avida {
  namespace Systematics {
    
    // Class Declaractions
    // --------------------------------------------------------------------------------------------------------------
    
    class Genotype;
    class GenotypeArbiter;
    
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SmartPtr<Genotype, Apto::InternalRCObject> GenotypePtr;
    typedef Apto::SmartPtr<GenotypeArbiter, Apto::InternalRCObject> GenotypeArbiterPtr;
    
    
    // Genotype
    // --------------------------------------------------------------------------------------------------------------
    
    class Genotype : public Group
    {
      friend class GenotypeArbiter;
    private:
      mutable GenotypeArbiterPtr m_mgr;
      Apto::List<GenotypePtr, Apto::SparseVector>::EntryHandle* m_handle;
      
      Source m_src;
      Genome m_genome;
      Apto::String m_name;
      
      bool m_threshold;
      bool m_active;
      
      int m_generation_born;
      int m_update_born;
      int m_update_deactivated;
      int m_depth;
      int m_active_offspring_genotypes;
      int m_num_organisms;
      int m_last_num_organisms;
      int m_total_organisms;
      
      Apto::Array<GenotypePtr> m_parents;
      Apto::String m_parent_str;
      
      cCountTracker m_births;
      cCountTracker m_deaths;
      cCountTracker m_breed_in;
      cCountTracker m_breed_true;
      cCountTracker m_breed_out;

      cCountTracker m_gestation_count;
            
      cDoubleSum m_copied_size;
      cDoubleSum m_exe_size;
      cDoubleSum m_gestation_time;
      cDoubleSum m_repro_rate;
      cDoubleSum m_merit;
      cDoubleSum m_fitness;
            
      int m_last_birth_cell;
      int m_last_group_id;
      int m_last_forager_type;
      
      Apto::Array<Apto::Stat::Accumulator<int> > m_task_counts;
      
      mutable PropertyMap* m_prop_map;
      
      
    public:
      ~Genotype();
      
      static void Initialize();
      
      
      // Systematics::Group Interface Methods
      RoleID Role() const;
      ArbiterPtr Arbiter() const;
      
      GroupPtr ClassifyNewUnit(UnitPtr u, ConstGroupMembershipPtr parent_groups);
      void HandleUnitGestation(UnitPtr u);
      void RemoveUnit();
      
      int Depth() const;
      int NumUnits() const;
      
      const PropertyMap& Properties() const;
      
      bool Serialize(ArchivePtr ar) const;
      bool LegacySave(void* df) const;

      void RemoveActiveReference() const;
      

      // Genotype Specific Methods
      bool Matches(UnitPtr u);
      
      
      // ???      
      inline void SetLastBirthCell(int birth_cell) { m_last_birth_cell = birth_cell; }
      inline void SetLastGroupID(int group_id) { m_last_group_id = group_id; }
      inline void SetLastForagerType(int forager_type) { m_last_forager_type = forager_type; }
      
      
    private:
      // Methods called by GenotypeArbiter
      Genotype(GenotypeArbiterPtr mgr, GroupID in_id, UnitPtr founder, Update update, ConstGroupMembershipPtr parents);
      Genotype(GenotypeArbiterPtr mgr, GroupID in_id, void* props);

      void NotifyNewUnit(UnitPtr u);
      void UpdateReset();

      inline const Genome& GroupGenome() const { return m_genome; }
      inline const Apto::Array<GenotypePtr> Parents() const { return m_parents; }
      
      inline void SetName(const Apto::String& name) { m_name = name; }
      
      inline bool IsThreshold() const { return m_threshold; }
      inline bool IsActive() const { return m_active; }
      
      inline int GetUpdateBorn() const { return m_update_born; }
      
      inline void SetThreshold() { m_threshold = true; }
      inline void ClearThreshold() { m_threshold = false; }
      
      inline void Deactivate(int update) { m_active = false; m_update_deactivated = update; }
      inline void Reactivate() { m_active = true; m_update_deactivated = -1; }
            
    private:
      void setupPropertyMap() const;
      inline GenotypePtr thisPtr();
    };

  };
};

#endif
