//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATION_LIB_HH
#include "cMutationLib.h"
#endif

#ifndef MUTATION_HH
#include "cMutation.h"
#endif
#ifndef nMutation_h
#include "nMutation.h"
#endif

//////////////////
//  cMutationLib
//////////////////

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
