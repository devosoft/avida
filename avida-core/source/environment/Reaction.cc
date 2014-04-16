/*
 *  environment/Reaction.cc
 *  avida-core
 *
 *  Created by David on 2/20/14.
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

#include "avida/environment/Reaction.h"


Avida::Environment::ReactionProcess::~ReactionProcess() { ; }

Avida::Environment::ReactionProcess* Avida::Environment::Reaction::AddProcess()
{
  ReactionProcess* new_process = new ReactionProcess();
  m_process_list.PushRear(new_process);
  return new_process;
}

Avida::Environment::ReactionRequisite* Avida::Environment::Reaction::AddRequisite()
{
  ReactionRequisite* new_requisite = new ReactionRequisite();
  m_requisite_list.PushRear(new_requisite);
  return new_requisite;
}
