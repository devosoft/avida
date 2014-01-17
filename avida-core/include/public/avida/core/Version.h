/*
 *  core/Version.h
 *  avida-core
 *
 *  Created by David on 4/17/11.
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

#ifndef AvidaCoreVersion_h
#define AvidaCoreVersion_h

#include "apto/platform.h"
#include "apto/core/String.h"


namespace Avida {
  namespace Version {
    LIB_EXPORT inline const char* String() { return "2.14.0"; }
    LIB_EXPORT inline int Major() { return 2; }
    LIB_EXPORT inline int Minor() { return 14; }
    LIB_EXPORT inline int Patch() { return 0; }
    LIB_EXPORT inline const char* Tag() { return ""; }
    
    LIB_EXPORT Apto::String Banner();
    
    LIB_EXPORT bool CheckCompatibility(const char* version);
  };
};

#endif
