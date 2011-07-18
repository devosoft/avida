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

class cString;


namespace Avida {  
  
  // WorldDriver - protocol defining the interface for objects that drive world execution
  // --------------------------------------------------------------------------------------------------------------
  
  class WorldDriver : public virtual GlobalObject
  {
  public:
    LIB_EXPORT virtual ~WorldDriver() { ; }
    
    // Driver Actions
    LIB_EXPORT virtual void SignalBreakpoint() = 0;
    LIB_EXPORT virtual void SetDone() = 0;
    LIB_EXPORT virtual void SetPause() = 0;

    
    // Legacy Methods
    // --------------------------------------------------------------------------------------------------------------

    LIB_LOCAL virtual void RaiseException(const cString& in_string) = 0;
    LIB_LOCAL virtual void RaiseFatalException(int exit_code, const cString& in_string) = 0;
    
    LIB_LOCAL virtual void NotifyComment(const cString& in_string) = 0;
    LIB_LOCAL virtual void NotifyWarning(const cString& in_string) = 0;

    LIB_LOCAL virtual bool IsInteractive() { return false; }
    
    
    // Fast-forward through epochs when no replication is happening -- @JEB
    // These are only implemented in the DefaultWorldDriver
    LIB_LOCAL virtual void ClearFastForward() { }
    LIB_LOCAL virtual bool GetFastForward() { return false; }
  };
};

#endif
