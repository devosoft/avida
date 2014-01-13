/*
 *  core/GlobalObject.h
 *  avida-core
 *
 *  Created by David on 12/11/05.
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

#ifndef AvidaCoreGlobalObject_h
#define AvidaCoreGlobalObject_h

#include "apto/platform.h"


namespace Avida {
  
  // GlobalObject - protocol for globally registered objects
  // --------------------------------------------------------------------------------------------------------------
  
  class GlobalObject
  {
  public:
    LIB_EXPORT virtual ~GlobalObject() = 0;
  };
  

  // Global Object Manager
  // --------------------------------------------------------------------------------------------------------------
  
  namespace GlobalObjectManager {
    LIB_EXPORT void Register(GlobalObject* obj);
    LIB_EXPORT void Unregister(GlobalObject* obj);
    
    LIB_LOCAL void Initialize();
  };
};


#endif
