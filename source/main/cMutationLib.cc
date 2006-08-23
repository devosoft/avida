/*
 *  cMutationLib.cc
 *  Avida
 *
 *  Called "mutation_lib.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
