/*
 *  output/Manager.h
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

#ifndef AvidaOutputManager_h
#define AvidaOutputManager_h

#include "apto/platform.h"
#include "avida/core/World.h"
#include "avida/output/Types.h"


namespace Avida {
  namespace Output {
    
    // Output::Manager - Manages output sockets (files, etc.) and their identifiers
    // --------------------------------------------------------------------------------------------------------------
    
    class Manager : public WorldFacet
    {
      friend class Socket;
    private:
      World* m_world;
      
      Apto::String m_output_path;
      
      mutable Apto::Mutex m_mutex;
      Apto::Map<OutputID, SocketWeakRef> m_sockets;
      Apto::Map<OutputID, SocketPtr> m_static_sockets;
      
    public:
      LIB_EXPORT Manager(const Apto::String& output_path);
      LIB_EXPORT ~Manager();
      
      LIB_EXPORT inline const Apto::String& OutputPath() const { return m_output_path; }
      
      LIB_EXPORT OutputID OutputIDFromPath(Apto::String path) const;

      LIB_EXPORT bool IsOpen(const OutputID& output_id) const;
      LIB_EXPORT bool Close(const OutputID& output_id);
      
      LIB_EXPORT void FlushAll();
      
      LIB_EXPORT bool AttachTo(World* world);
      LIB_EXPORT static ManagerPtr Of(World* world);
      
    public:
      LIB_EXPORT bool Serialize(ArchivePtr ar) const;
      
    public:
      LIB_LOCAL WorldFacetID UpdateBefore() const;
      LIB_LOCAL WorldFacetID UpdateAfter() const;
      
    private:
      LIB_EXPORT bool RegisterSocket(const OutputID& output_id, SocketWeakRef socket_ref);
      LIB_EXPORT bool RegisterStaticSocket(const OutputID& output_id, SocketPtr socket);
      LIB_EXPORT SocketPtr RetrieveStaticSocket(const OutputID& output_id);
      LIB_EXPORT void UnregisterSocket(const OutputID& output_id);
    };
    
  };
};

#endif
