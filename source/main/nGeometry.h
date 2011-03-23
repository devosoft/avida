/*
 *  nGeometry.h
 *  Avida
 *
 *  Created by David on 10/6/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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
 */

/*! Namespace to hold possible values of the world grids geometry */

#ifndef nGeometry_h
#define nGeometry_h

namespace nGeometry {
  enum {
    GLOBAL = 0,
    GRID,
    TORUS,
    CLIQUE,
    HEX,
		PARTIAL,
		LATTICE,
		RANDOM_CONNECTED,
		SCALE_FREE
  };
}

#endif
