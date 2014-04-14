/*
 *  biota/EventListener.h
 *  avida-core
 *
 *  Created by David on 4/10/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
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

#ifndef AvidaBiotaEventListener_h
#define AvidaBiotaEventListener_h

#include "avida/biota/Types.h"


namespace Avida {
  namespace Biota {

    // Biota::OrganismListener - Object that wishes to be notifed of organism events
    // --------------------------------------------------------------------------------------------------------------
    
    class EventListener
    {
    public:
      LIB_EXPORT virtual ~EventListener() = 0;
      
      LIB_EXPORT virtual void NotifyEvent(OrganismEvent event_type) = 0;
    };

  };
};

#endif
