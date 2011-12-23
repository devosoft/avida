/*
 *  core/GeneticRepresentation.h
 *  avida-core
 *
 *  Created by David on 12/8/11.
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

#ifndef AvidaCoreGeneticRepresentation_h
#define AvidaCoreGeneticRepresentation_h

#include "avida/core/Types.h"


namespace Avida {
  
  // GeneticRepresentation
  // --------------------------------------------------------------------------------------------------------------
  
  class GeneticRepresentation
  {
  public:
    LIB_EXPORT virtual ~GeneticRepresentation() = 0;
    
    LIB_EXPORT virtual bool Process(GeneticRepresentationDispatchTable tbl, GeneticRepresentationPtr ptr);
    LIB_EXPORT virtual bool Process(ConstGeneticRepresentationDispatchTable tbl, ConstGeneticRepresentationPtr ptr) const;
    
    LIB_EXPORT virtual bool operator==(const GeneticRepresentation&) const = 0;
    LIB_EXPORT inline bool operator!=(const GeneticRepresentation& rhs) const { return !this->operator==(rhs); }
    
    LIB_EXPORT virtual Apto::String AsString() const = 0;
    
    LIB_EXPORT virtual GeneticRepresentationPtr Clone() const = 0;
    
    LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const = 0;
  };
  
};


#endif
