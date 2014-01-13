/*
 *  private/systematics/Clade.h
 *  Avida
 *
 *  Created by David on 7/30/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
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

#ifndef AvidaSystematicsClade_h
#define AvidaSystematicsClade_h

#include "avida/systematics/Group.h"
#include "avida/systematics/Unit.h"


namespace Avida {
  namespace Systematics {
    
    // Class Declaractions
    // --------------------------------------------------------------------------------------------------------------
    
    class Clade;
    class CladeArbiter;
    
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SmartPtr<Clade, Apto::InternalRCObject> CladePtr;
    typedef Apto::SmartPtr<CladeArbiter, Apto::InternalRCObject> CladeArbiterPtr;
    
    
    // Clade
    // --------------------------------------------------------------------------------------------------------------
    
    class Clade : public Group
    {
      friend class CladeArbiter;
    private:
      mutable CladeArbiterPtr m_mgr;
      Apto::List<CladePtr, Apto::SparseVector>::EntryHandle* m_handle;
      
      Apto::String m_name;
      
      int m_num_organisms;
      int m_last_num_organisms;
      int m_total_organisms;
            
      mutable PropertyMap* m_prop_map;
      
      
    public:
      ~Clade();
      
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
                  
      
    private:
      // Methods called by CladeArbiter
      Clade(CladeArbiterPtr mgr, GroupID in_id, const Apto::String& name, bool create_empty = false);
      
      void NotifyNewUnit(UnitPtr u);
      void UpdateReset();
      
      inline const Apto::String& Name() const { return m_name; }
      
    private:
      void setupPropertyMap() const;
      inline CladePtr thisPtr();
    };
    
  };
};

#endif
