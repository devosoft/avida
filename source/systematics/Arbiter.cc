/*
 *  systematics/Arbiter.cc
 *  avida-core
 *
 *  Created by David on 12/16/11.
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

#include "avida/systematics/Arbiter.h"

#include "avida/systematics/Group.h"
#include "avida/systematics/Listener.h"
#include "avida/systematics/Unit.h"


Avida::Systematics::Arbiter::~Arbiter() { ; }

void Avida::Systematics::Arbiter::notifyListeners(GroupPtr g, EventType t, UnitPtr u)
{
  for (Apto::Set<Listener*>::Iterator it = m_listeners.Begin(); (it.Next()); ) (*it.Get())->Notify(g, t, u);
}

Avida::Systematics::Arbiter::Iterator::~Iterator() { ; }

bool Avida::Systematics::Arbiter::Serialize(ArchivePtr) const
{
  // @TODO - serialize arbiter
  return false;
}

bool Avida::Systematics::Arbiter::LegacySave(void*) const
{
  return false;
}

Avida::Systematics::GroupPtr Avida::Systematics::Arbiter::LegacyLoad(void*)
{
  return GroupPtr();
}
