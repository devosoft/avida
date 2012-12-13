/*
 *  private/systematics/SexualAncestry.h
 *  Avida
 *
 *  Created by David on 7/7/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#ifndef AvidaSystematicsSexualAncestry_h
#define AvidaSystematicsSexualAncestry_h

#include "apto/platform.h"
#include "avida/systematics/Group.h"

#include <cassert>


namespace Avida {
  namespace Systematics {
    
    // Class Declaractions
    // --------------------------------------------------------------------------------------------------------------
    
    class SexualAncestry;
    
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SmartPtr<SexualAncestry> SexualAncestryPtr;
    
    
    // SexualAncestry
    // --------------------------------------------------------------------------------------------------------------
    
    class SexualAncestry : public GroupData
    {
    public:
      static const Apto::String ObjectKey;

    private:
      int m_id;
      int m_ancestor_ids[6];
      
    public:
      LIB_LOCAL SexualAncestry(GroupPtr g);
      
      LIB_LOCAL int GetID() const { return m_id; }
      LIB_LOCAL int GetAncestorID(int idx) const { assert(idx < 6); return m_ancestor_ids[idx]; }
      
      LIB_LOCAL int GetPhyloDistance(GroupPtr g) const;
      
      LIB_LOCAL bool Serialize(ArchivePtr ar) const;
    };

  };
};

#endif
