/*
 *  output/Manager.cc
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

#include "avida/output/Manager.h"

#include "avida/output/Socket.h"

Avida::Output::Manager::Manager(const Apto::String& output_path) : m_world(NULL)
{
  m_output_path = output_path;
  m_output_path.Trim();
  
  m_output_path = Apto::FileSystem::GetAbsolutePath(Apto::String(m_output_path), Apto::FileSystem::GetCWD());

  if (m_output_path.GetSize() > 0) {
    char dir_tail = m_output_path[m_output_path.GetSize() - 1];
    if (dir_tail != '\\' && dir_tail != '/') m_output_path += "/";
    Apto::FileSystem::MkDir(Apto::String(m_output_path));
  }
}

Avida::Output::Manager::~Manager() { ; }


Avida::Output::OutputID Avida::Output::Manager::OutputIDFromPath(Apto::String path) const
{
  path.Trim();
  
  // Determine directory prefix, default being the current data directory
  Apto::String dir_prefix(m_output_path);
  if ((path[0] == '.' &&             // Must start with '.' to trigger further testing
       (path.GetSize() == 1 ||       // If string is exactly "."
        (path.GetSize() > 1 &&       //   or if it ".." or "./" or ".\"
         (path[1] == '.' || path[1] == '/' || path[1] == '\\')
         )
        )
       ) || path[0] == '/'           // If it is an absolute path, ignore prefix as well
      )
  {
    // Treat path as current working directory relative
    dir_prefix = Apto::FileSystem::GetCWD();
  }
  
  // Create directory structure, as necessary
  int i = 0;
  while (i < path.GetSize()) {
    int d = path.Find('/', i);
    int b = path.Find('\\', i);
    
    // If a backslash is found, and earlier then next forward slash, use it instead
    if (b != -1 && b < d) d = b;
    
    // Exit if neither slash is found
    if (d == -1) break;
    
    // If directory name is not null
    if (d - i > 0) {
      Apto::String dir = path.Substring(i, d - i);
      // Create if  that this directory is not a relative path component
      if (dir.GetSize() > 2 || (dir != "." && dir != ".."))
        Apto::FileSystem::MkDir(Apto::String(dir_prefix + path.Substring(0, d)));
    }
    
    // Adjust next directory name starting point
    i = d + 1;
  }
  
  path = Apto::FileSystem::GetAbsolutePath(path, dir_prefix);

  return path;
}


bool Avida::Output::Manager::IsOpen(const OutputID& output_id) const
{
  Apto::MutexAutoLock lock(m_mutex);
  return m_sockets.Has(output_id);
}

bool Avida::Output::Manager::Close(const OutputID& output_id)
{
  Apto::MutexAutoLock lock(m_mutex);
  m_sockets.Remove(output_id);
  return m_static_sockets.Remove(output_id);
}

void Avida::Output::Manager::FlushAll()
{
  m_mutex.Lock();
  for (Apto::Map<OutputID, SocketWeakRef>::ValueIterator it = m_sockets.Values(); it.Next();) {
    (*it.Get())->Flush();
  }
  m_mutex.Unlock();
}


bool Avida::Output::Manager::AttachTo(World* world)
{
  if (m_world) return false;
  
  WorldFacetPtr ptr(this);
  AddReference();  // explictly add reference, since this is internally creating a smart pointer to itself
  
  if (world->AttachFacet(Reserved::OutputManagerFacetID, ptr)) {
    m_world = world;
    return true;
  }
  return false;
}


Avida::Output::ManagerPtr Avida::Output::Manager::Of(World* world)
{
  ManagerPtr manager;
  manager.DynamicCastFrom(world->OutputManager());
  return manager;
}


bool Avida::Output::Manager::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::WorldFacetID Avida::Output::Manager::UpdateBefore() const
{
  return "";
}

Avida::WorldFacetID Avida::Output::Manager::UpdateAfter() const
{
  return "";
}


bool Avida::Output::Manager::RegisterSocket(const OutputID& output_id, SocketWeakRef socket_ref)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_sockets.Has(output_id)) return false;
  
  m_sockets.Set(output_id, socket_ref);
  return true;
}


bool Avida::Output::Manager::RegisterStaticSocket(const OutputID& output_id, SocketPtr socket)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_sockets.Has(output_id) && m_sockets.Get(output_id) != Apto::GetInternalPtr(socket)) return false;
  
  m_sockets.Set(output_id, Apto::GetInternalPtr(socket));
  m_static_sockets.Set(output_id, socket);
  return true;
}

Avida::Output::SocketPtr Avida::Output::Manager::RetrieveStaticSocket(const OutputID& output_id)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  return m_static_sockets.Get(output_id);
}



void Avida::Output::Manager::UnregisterSocket(const OutputID& output_id)
{
  m_mutex.Lock();
  if (!m_static_sockets.Has(output_id)) m_sockets.Remove(output_id);
  m_mutex.Unlock();
}

