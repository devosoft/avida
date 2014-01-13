/*
 *  core/WorldDriver.h
 *  avida-core
 *
 *  Created by David on 12/10/05.
 *  Copyright 2005-2011 Michigan State University. All rights reserved.
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

#ifndef AvidaCoreWorldDriver_h
#define AvidaCoreWorldDriver_h

#include "apto/platform.h"
#include "avida/core/GlobalObject.h"
#include "avida/core/Types.h"


namespace Avida {  
  
  // WorldDriver - protocol defining the interface for objects that drive world execution
  // --------------------------------------------------------------------------------------------------------------
  
  class WorldDriver : public virtual GlobalObject
  {
  public:
    LIB_EXPORT virtual ~WorldDriver() { ; }
    
    // Actions
    LIB_EXPORT virtual void Pause() = 0;
    LIB_EXPORT virtual void Finish() = 0;
    LIB_EXPORT virtual void Abort(AbortCondition condition) = 0;
    
    // Facilities
    LIB_EXPORT virtual Avida::Feedback& Feedback() = 0;
    
    // Callback methods
    LIB_EXPORT virtual void RegisterCallback(DriverCallback callback) = 0;
  };
};

#endif
