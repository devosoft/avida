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
#include "apto/core/StringUtils.h"
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
    
    LIB_EXPORT virtual const PropertyMap& Properties() const = 0;
    
    LIB_EXPORT virtual bool AttachProperty(const Property& prop) = 0;
    
    LIB_EXPORT virtual ConstArchiveObjectIDSetPtr SubObjectIDs() const = 0;
    LIB_EXPORT virtual ConstArchivePtr SubObject(ArchiveObjectID) const = 0;
    
    LIB_EXPORT virtual ArchivePtr DefineSubObject(ArchiveObjectID obj_id) = 0;
  };
    
};

#endif
