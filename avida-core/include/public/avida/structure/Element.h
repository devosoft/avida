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
    

  
    // ElementIteratorBase
    // --------------------------------------------------------------------------------------------------------------

    class ElementIteratorBase
    {
    public:
      LIB_EXPORT virtual ~ElementIteratorBase() = 0;
      
      LIB_EXPORT virtual ElementPtr Next() = 0;
      LIB_EXPORT virtual ElementPtr Get() = 0;
      
      LIB_EXPORT virtual ElementIteratorBase* Clone() const = 0;
    };
    
    
    
    // ElementIterator
    // --------------------------------------------------------------------------------------------------------------
    
    class ElementIterator
    {
    private:
      ElementIteratorBase* m_it;
      
    public:
      LIB_EXPORT inline ElementIterator(ElementIteratorBase* it) : m_it(it) { ; }
      LIB_EXPORT inline ElementIterator(const ElementIterator& it) : m_it(it.m_it->Clone()) { ; }
      LIB_EXPORT inline ~ElementIterator() { delete m_it; }
      
      LIB_EXPORT inline ElementIterator& operator=(const ElementIterator& rhs)
      {
        delete m_it; m_it = rhs.m_it->Clone(); return *this;
      }
      
      LIB_EXPORT inline ElementPtr Next() { return m_it->Next(); }
      LIB_EXPORT inline ElementPtr Get() { return m_it->Get(); }
    };
    
  };
};

#endif
