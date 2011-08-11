/*
 *  core/Archive.h
 *  avida-core
 *
 *  Created by David on 8/10/11.
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

#ifndef AvidaCoreArchive_h
#define AvidaCoreArchive_h

#include "apto/platform.h"
#include "avida/core/Types.h"


namespace Avida {
  
  // Archive - abstract serialized state container
  // --------------------------------------------------------------------------------------------------------------
  
  class Archive
  {
  public:
    LIB_EXPORT virtual ~Archive() { ; }
    
    LIB_EXPORT virtual ArchiveObjectID ObjectID() const = 0;
    LIB_EXPORT virtual ArchiveObjectType ObjectType() const = 0;
    LIB_EXPORT virtual int Version() const = 0;
    
    LIB_EXPORT virtual void SetObjectType(ArchiveObjectType obj_type) = 0;
    LIB_EXPORT virtual void SetVersion(int version) = 0;
    
    LIB_EXPORT virtual ConstArchivePropertyIDSetPtr PropertyIDs() const = 0;
    LIB_EXPORT virtual ConstArchivePropertyPtr Property(ArchivePropertyID prop_id) const = 0;
    
    template <typename T>
    LIB_EXPORT inline bool DefineProperty(ArchivePropertyID prop_id, T prop_value);
    LIB_EXPORT virtual bool DefineProperty(ArchivePropertyID prop_id, ArchivePropertyType prop_type, Apto::String prop_value) = 0;
    
    LIB_EXPORT virtual ConstArchiveObjectIDSetPtr SubObjectIDs() const = 0;
    LIB_EXPORT virtual ConstArchivePtr SubObject(ArchiveObjectID) const = 0;
    
    LIB_EXPORT virtual ArchivePtr DefineSubObject(ArchiveObjectID obj_id) = 0;
  };
  
  template <typename T>
  inline bool Archive::DefineProperty(ArchivePropertyID prop_id, T prop_value)
  {
    return this->DefineProperty(prop_id, ArchivePropertyTraits<T>::Type, prop_value);
  }
  
  
  // ArchiveProperty
  // --------------------------------------------------------------------------------------------------------------
  
  class ArchiveProperty
  {
  public:
    LIB_EXPORT virtual ~ArchiveProperty() { ; }
    
    LIB_EXPORT virtual ArchivePropertyID PropertyID() const = 0;
    LIB_EXPORT virtual ArchivePropertyType Type() const = 0;
    LIB_EXPORT virtual Apto::String Value() const = 0;
  };
  
  
  
  // ArchivePropertyTraits Specializations
  // --------------------------------------------------------------------------------------------------------------
  
  template <> struct ArchivePropertyTraits<int>
  {
    static const ArchivePropertyType Type;
  };
  
  template <> struct ArchivePropertyTraits<double>
  {
    static const ArchivePropertyType Type;
  };
  
  template <> struct ArchivePropertyTraits<Apto::String>
  {
    static const ArchivePropertyType Type;
  };
  
};

#endif
