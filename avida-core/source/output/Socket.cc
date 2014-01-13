/*
 *  output/Socket.cc
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

#include "avida/output/Socket.h"

#include "avida/output/Manager.h"


Avida::Output::Socket::Socket(World* world, const OutputID& output_id) : m_world(world), m_output_id(output_id), m_is_static(false)
{
  Output::Manager::Of(m_world)->RegisterSocket(m_output_id, this);
}


Avida::Output::Socket::~Socket()
{
  if (!m_is_static) Output::Manager::Of(m_world)->UnregisterSocket(m_output_id);
}


bool Avida::Output::Socket::registerAsStatic()
{
  SocketPtr thisPtr(this);
  this->AddReference();
  bool success = Output::Manager::Of(m_world)->RegisterStaticSocket(m_output_id, thisPtr);
  if (success) m_is_static = true;
  return success;
}

Avida::Output::SocketPtr Avida::Output::Socket::RetrieveStatic(ManagerPtr mgr, const OutputID& output_id)
{
  return mgr->RetrieveStaticSocket(output_id);
}
