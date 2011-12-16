/*
 *  systematics/Unit.h
 *  avida-core
 *
 *  Created by David on 12/14/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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

#ifndef AvidaSystematicsUnit_h
#define AvidaSystematicsUnit_h

#include "apto/platform.h"
#include "avida/core/Types.h"
#include "avida/systematics/Types.h"


namespace Avida {
  namespace Systematics {
    
    // Source
    // --------------------------------------------------------------------------------------------------------------
    
    struct Source
    {
    public:
      // Bit-fields {
      TransmissionType  transmission_type  :4;
      bool              external           :1;
      
      int               unused             :27;
      // }
      
      Apto::String arguments;
      
      
      // Methods
      LIB_EXPORT inline Source(TransmissionType t, const Apto::String& a, bool e = false)
        : transmission_type(t), external(e), unused(0), arguments(a) { ; }
    };

    
    // Unit
    // --------------------------------------------------------------------------------------------------------------
    
    class Unit
    {
    protected:
      GroupMembershipPtr m_groups;
      
    public:
      LIB_EXPORT inline Unit() : m_groups(new GroupMembership) { ; }
      LIB_EXPORT virtual ~Unit() = 0;
      
      LIB_EXPORT virtual Source UnitSource() const = 0;
      LIB_EXPORT virtual Apto::String UnitSourceArguments() const = 0;
      LIB_EXPORT virtual ConstGenomePtr GetGenome() const = 0;
      
      
      LIB_EXPORT inline ConstGroupMembershipPtr SystematicsGroupMembership() const { return m_groups; }
      LIB_EXPORT GroupPtr SystematicsGroup(const RoleID& role) const;
      
      LIB_EXPORT bool AddClassification(GroupPtr g);
      LIB_EXPORT void Reclassify(GroupPtr g);
      
      LIB_EXPORT void SelfClassify(ConstParentGroupsPtr parent_groups);
      
      LIB_EXPORT void HandleGestation();
    };
    
  };
};

#endif
