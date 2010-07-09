/*
 *  cMutationLib.cc
 *  Avida
 *
 *  Called "mutation_lib.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cMutationLib.h"

#include "cMutation.h"
#include "nMutation.h"


cMutationLib::cMutationLib()
  : trigger_list_array(nMutation::NUM_TRIGGERS)
{
}

cMutationLib::~cMutationLib()
{
  // Get rid of the trigger lists...  This is not required, but since we
  // are deleting the mutations after this, we should first remove all
  // references to them.
  trigger_list_array.Resize(0);

  // Now delete the actual mutations.
  for (int i = 0; i < mutation_array.GetSize(); i++) {
    delete mutation_array[i];
  }
}

cMutation * cMutationLib::AddMutation(const cString & name, int trigger,
				      int scope, int type, double rate)
{
  // Build the new mutation and drop it in the proper trigger list.
  const int id = GetSize();
  mutation_array.Resize(id+1);
  mutation_array[id] = new cMutation(name, id, trigger, scope, type, rate);
  trigger_list_array[trigger].Push(mutation_array[id]);

  return mutation_array[id];
}
