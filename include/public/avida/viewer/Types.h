/*
 *  viewer/Types.h
 *  avida-core
 *
 *  Created by David on 1/3/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
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

#ifndef AvidaViewerTypes_h
#define AvidaViewerTypes_h

#include "apto/core.h"

namespace Avida {
  namespace Viewer {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class ClassificationInfo;
    struct Color;
    class Driver;
    class Freezer;
    struct FreezerID;
    class Graphic;
    struct GraphicObject;
    class GraphicsContext;
    class HardwareSnapshot;
    class Listener;
    class Map;
    class OrganismTrace;
    
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SmartPtr<Freezer> FreezerPtr;
    typedef Apto::SmartPtr<Graphic> GraphicPtr;
    typedef Apto::SmartPtr<const Graphic> ConstGraphicPtr;
    typedef Apto::SmartPtr<OrganismTrace> OrganismTracePtr;
    

    // Enumerations
    // --------------------------------------------------------------------------------------------------------------
    
    enum DriverPauseState {
      DRIVER_PAUSED,
      DRIVER_SYNCING,
      DRIVER_UNPAUSED
    };    
    
    enum GraphicShape { SHAPE_NONE, SHAPE_OVAL, SHAPE_RECT, SHAPE_CURVE };
    
  };
};

#endif
