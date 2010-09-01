/*
 *  cReactionLib.cc
 *  Avida
 *
 *  Called "reaction_lib.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cReactionLib.h"

#include "cReaction.h"


cReactionLib::~cReactionLib()
{
  for (int i = 0; i < reaction_array.GetSize(); i++) {
    delete reaction_array[i];
  }
}


cReaction * cReactionLib::GetReaction(const cString & name) const
{
  for (int i = 0; i < reaction_array.GetSize(); i++) {
    if (reaction_array[i]->GetName() == name) return reaction_array[i];
  }
  return NULL;
}

cReaction * cReactionLib::GetReaction(int id) const
{
  return reaction_array[id];
}


cReaction * cReactionLib::AddReaction(const cString & name)
{
  // If this reaction already exists, just return it.
  cReaction * found_reaction = GetReaction(name);
  if (found_reaction != NULL) return found_reaction;

  // Create a new reaction...
  const int new_id = reaction_array.GetSize();
  cReaction * new_reaction = new cReaction(name, new_id);
  reaction_array.Resize(new_id + 1);
  reaction_array[new_id] = new_reaction;
  return new_reaction;
}
