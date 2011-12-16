/*
 *  systematics/Types.h
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

#ifndef AvidaSystematicsTypes_h
#define AvidaSystematicsTypes_h

#include "apto/core.h"

namespace Avida {
  namespace Systematics {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class Arbiter;
    class Group;
    class Manager;
    struct Source;
    class Unit;
    
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------

    typedef Apto::Map<Apto::String, Apto::String> ClassificationHints;
    
    typedef int GroupID;
    typedef Apto::SmartPtr<Group> GroupPtr;
    
    typedef Apto::Array<GroupPtr> GroupMembership;
    typedef Apto::SmartPtr<GroupMembership> GroupMembershipPtr;
    typedef Apto::SmartPtr<const GroupMembership> ConstGroupMembershipPtr;
    
    typedef Apto::Array<ConstGroupMembershipPtr> ParentGroups;
    typedef Apto::SmartPtr<ParentGroups> ParentGroupsPtr;
    typedef Apto::SmartPtr<const ParentGroups> ConstParentGroupsPtr;
    
    typedef Apto::String RoleID;
    
    
    enum TransmissionType {
      DIVISION,
      DUPLICATION,
      VERTICAL,
      HORIZONTAL
    };
    
  };
};

#endif
