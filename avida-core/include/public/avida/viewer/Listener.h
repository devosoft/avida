/*
 *  viewer/Listener.h
 *  avida-core
 *
 *  Created by David on 11/11/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#ifndef AvidaViewerListener_h
#define AvidaViewerListener_h

#include "apto/platform.h"
#include "avida/viewer/Types.h"


namespace Avida {
  namespace Viewer {
    
    class Listener
    {
    public:
      LIB_EXPORT virtual ~Listener() = 0;
      
      LIB_EXPORT virtual bool WantsMap() = 0;
      LIB_EXPORT virtual bool WantsState() = 0;
      LIB_EXPORT virtual bool WantsUpdate() = 0;
      
      LIB_EXPORT virtual void NotifyMap(Map* map);
      LIB_EXPORT virtual void NotifyState(DriverPauseState state);
      LIB_EXPORT virtual void NotifyUpdate(int update);
    };
    
  };
};

#endif
