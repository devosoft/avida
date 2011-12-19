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
      GenotypeArbiterPtr m_mgr;
      Apto::List<GenotypePtr, Apto::SparseVector>::EntryHandle* m_handle;
      
      Source m_src;
      Genome m_genome;
      Apto::String m_name;
      
      struct {
        bool m_threshold:1;
        bool m_active:1;
      };
      
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
      
      cDoubleSum m_copied_size;
      cDoubleSum m_exe_size;
      cDoubleSum m_gestation_time;
      cDoubleSum m_repro_rate;
      cDoubleSum m_merit;
      cDoubleSum m_fitness;
      
      int m_last_birth_cell;
      int m_last_group_id;
      int m_last_forager_type;
      
      Genotype(GenotypeArbiterPtr mgr, int in_id, UnitPtr founder, int update, ConstGroupMembershipPtr parents);
      
    public:
      ~Genotype();
      
      // Systematics::Group Interface Methods
      RoleID Role() const;
      ArbiterPtr Arbiter() const;
      
      GroupPtr ClassifyNewUnit(UnitPtr u, ConstGroupMembershipPtr parent_groups);
      void HandleUnitGestation(UnitPtr u);
      void RemoveUnit(UnitPtr u);
      
      int Depth() const;
      int NumUnits() const;
      
      ConstPropertyIDSetPtr PropertyIDs() const;
      const PropertyMap& Properties() const;
      
      bool Serialize(ArchivePtr ar) const;
      

      void RemoveActiveReference();
      
      
      // Genotype Specific Methods
      inline const Source& GetSource() const { return m_src; }
      inline const Genome& GetGenome() const { return m_genome; }
      inline Apto::String GetGenomeString() const { return m_genome.AsString(); }      
      
      inline const Apto::String& GetName() const { return m_name; }
      inline void SetName(const Apto::String& name) { m_name = name; }
      
      inline bool IsThreshold() const { return m_threshold; }
      inline bool IsActive() const { return m_active; }
      
      inline int GetUpdateBorn() const { return m_update_born; }
      inline int GetUpdateDeactivated() const { return m_update_deactivated; }
      
      inline const Apto::String& GetParentString() const { return m_parent_str; }
      
      inline void SetThreshold() { m_threshold = true; }
      inline void ClearThreshold() { m_threshold = false; }
      
      inline void Deactivate(int update) { m_active = false; m_update_deactivated = update; }
      
      inline const Apto::Array<GenotypePtr> GetParents() const { return m_parents; }
      
      bool Matches(UnitPtr u);
      void NotifyNewUnit(UnitPtr u);
      
      void UpdateReset();

      // ... convert to properties
      inline int GetTotalOrganisms() const { return m_total_organisms; }
      
      inline int GetLastBirths() const { return m_births.GetLast(); }
      inline int GetLastBreedIn() const { return m_breed_in.GetLast(); }
      inline int GetLastBreedTrue() const { return m_breed_true.GetLast(); }
      inline int GetLastBreedOut() const { return m_breed_out.GetLast(); }
      
      inline int GetThisBirths() const { return m_births.GetCur(); }
      inline int GetThisDeaths() const { return m_deaths.GetCur(); }
      inline int GetThisBreedIn() const { return m_breed_in.GetCur(); }
      inline int GetThisBreedTrue() const { return m_breed_true.GetCur(); }
      inline int GetThisBreedOut() const { return m_breed_out.GetCur(); }
      
      inline double GetCopiedSize() const { return m_copied_size.Average(); }
      inline double GetExecutedSize() const { return m_exe_size.Average(); }
      inline double GetGestationTime() const { return m_gestation_time.Average(); }
      inline double GetReproRate() const { return m_repro_rate.Average(); }
      inline double GetMerit() const { return m_merit.Average(); }
      inline double GetFitness() const { return m_fitness.Average(); }
      
      inline int GetLastBirthCell() const { return m_last_birth_cell; }
      inline int GetLastGroupID() const { return m_last_group_id; } 
      inline int GetLastForagerType() const { return m_last_forager_type; } 
      
      inline void SetLastBirthCell(int birth_cell) { m_last_birth_cell = birth_cell; }
      inline void SetLastGroupID(int group_id) { m_last_group_id = group_id; }
      inline void SetLastForagerType(int forager_type) { m_last_forager_type = forager_type; }
    };

  };
};

#endif
