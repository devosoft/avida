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

class cString;

namespace Avida {
  namespace Version {
    inline const char* String() { return "2.13.0 (TBD)"; }
    inline int Major() { return 2; }
    inline int Minor() { return 13; }
    inline int Patch() { return 0; }
    inline const char* Tag() { return "TDB"; }
    
    cString Banner();
    
    bool CheckCompatibility(const char* version);
  };
};

#endif
