/*
 *  core/Properties.h
 *  avida-core
 *
 *  Created by David on 12/7/11.
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

#ifndef AvidaCoreProperties_h
#define AvidaCoreProperties_h

#include "apto/platform.h"
#include "apto/core/StringUtils.h"
#include "avida/core/Types.h"


namespace Avida {
  
  // Property
  // --------------------------------------------------------------------------------------------------------------
  
  class Property
  {
  private:
    PropertyID m_id;
    const PropertyTypeID& m_type_id;
    Apto::String m_value;
    
  public:
    template <typename T> LIB_EXPORT Property(const PropertyID& prop_id, const T& prop_value)
      : m_id(prop_id), m_type_id(PropertyTraits<T>::Type), m_value(Apto::AsStr(prop_value)) { ; }
    LIB_EXPORT inline Property(const PropertyID& prop_id, const PropertyTypeID& type_id, const Apto::String& prop_value)
      : m_id(prop_id), m_type_id(type_id), m_value(prop_value) { ; }
    
    LIB_EXPORT inline const PropertyID& PropertyID() const { return m_id; }
    LIB_EXPORT inline const PropertyTypeID& Type() const { return m_type_id; }
    LIB_EXPORT inline const Apto::String& Value() const { return m_value; }
  };
  

  
  // PropertyTraits Specializations
  // --------------------------------------------------------------------------------------------------------------
  
  template <> struct PropertyTraits<int>
  {
    static const PropertyTypeID Type;
  };
  
  template <> struct PropertyTraits<double>
  {
    static const PropertyTypeID Type;
  };
  
  template <> struct PropertyTraits<Apto::String>
  {
    static const PropertyTypeID Type;
  };
  
};

#endif
