//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REACTION_LIB_HH
#include "reaction_lib.hh"
#endif

#ifndef REACTION_HH
#include "reaction.hh"
#endif

//////////////////
//  cReactionLib
//////////////////

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
