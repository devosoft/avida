/*
 *  structure/Coord.h
 *  avida-core
 *
 *  Created by David on 1/25/13.
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

#ifndef AvidaStructureCoord_h
#define AvidaStructureCoord_h

#include "avida/structure/Types.h"

namespace Avida {
  namespace Structure {
    
    // Coord
    // --------------------------------------------------------------------------------------------------------------
    
    struct Coord
    {
    public:
      union {
        float x;
        int index;
      };
      float y;
      float z;
      
      LIB_EXPORT inline Coord() : x(0.0f), y(0.0f), z(0.0f) { ; }
      LIB_EXPORT inline Coord(float in_x, float in_y, float in_z = 0.0f) : x(in_x), y(in_y), z(in_z) { ; }
      LIB_EXPORT inline explicit Coord(int in_index) : index(in_index), y(0.0f), z(0.0f) { ; }
    };
    
  };
};

#endif
