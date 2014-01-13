/*
 *  structure/Element.h
 *  avida-core
 *
 *  Created by David on 1/24/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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

#ifndef AvidaStructureElement_h
#define AvidaStructureElement_h

#include "avida/structure/Types.h"

namespace Avida {
  namespace Structure {
    
    // Element
    // --------------------------------------------------------------------------------------------------------------
    
    class Element : public virtual Apto::RefCountObject<Apto::ThreadSafe>
    {
      friend class Controller;
      
    protected:
      Controller* m_structure;
      
    public:
      LIB_EXPORT inline Element() : m_structure(NULL) { ; }
      
      LIB_EXPORT inline Controller* StructureContoller() { return m_structure; }
      LIB_EXPORT inline const Controller* StructureController() const { return m_structure; }
      
      LIB_EXPORT virtual void ProcessTimeStep(Context& ctx, Update current_update) = 0;
      
    private:
      LIB_EXPORT inline void SetStructureController(Controller* structure) { m_structure = structure; }
    };
    
  };
};

#endif
