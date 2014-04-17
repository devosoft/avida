/*
 *  hardware/InstSet.cc
 *  avida-core
 *
 *  Created by David on 4/15/14.
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


#include "avida/hardware/InstSet.h"


Avida::Hardware::InstSet::~InstSet()
{
}


Avida::Instruction Avida::Hardware::InstSet::InstWithName(const Apto::String& name) const
{
  for (int i = 0; i < m_entries.GetSize(); i++) {
    if (m_inst_lib->NameOf(m_entries[i].lib_id) == name) {
      return Instruction(i);
    }
  }
  
  return InstError();
}

Apto::String Avida::Hardware::InstSet::FindBestMatch(const Apto::String& name) const
{
  int best_dist = 1024;
  Apto::String best_name("");
  
  for (int i = 0; i < m_entries.GetSize(); i++) {
    const Apto::String& cur_name = m_inst_lib->NameOf(m_entries[i].lib_id);
    const int cur_dist = Apto::EditDistance(cur_name, name);
    if (cur_dist < best_dist) {
      best_dist = cur_dist;
      best_name = cur_name;
    }
    if (cur_dist == 0) break;
  }
  
  return best_name;
}
