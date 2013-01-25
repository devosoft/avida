/*
 *  output/Socket.h
 *  avida-core
 *
 *  Created by David on 1/16/13.
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

#ifndef AvidaOutputSocket_h
#define AvidaOutputSocket_h

#include "apto/platform.h"
#include "avida/output/Types.h"


namespace Avida {
  namespace Output {
    
    // Output::Socket - Protocol defining interface for output sockets that can be managed by the output manager
    // --------------------------------------------------------------------------------------------------------------
    
    class Socket : public Apto::RefCountObject<Apto::ThreadSafe>
    {
    protected:
      World* m_world;
      OutputID m_output_id;

    private:
      bool m_is_static;

    public:
      LIB_EXPORT Socket(World* world, const OutputID& output_id);
      LIB_EXPORT virtual ~Socket() = 0;
      
      LIB_EXPORT virtual void Flush() = 0;
      
    protected:
      LIB_EXPORT bool registerAsStatic();
      LIB_EXPORT static SocketPtr RetrieveStatic(ManagerPtr mgr, const OutputID& output_id);
    };
    
  };
};

#endif
