//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LOCAL_MUTATIONS_HH
#include "local_mutations.hh"
#endif

#ifndef MUTATION_HH
#include "mutation.hh"
#endif
#ifndef MUTATION_LIB_HH
#include "mutation_lib.hh"
#endif
#ifndef MUTATION_MACROS_HH
#include "mutation_macros.hh"
#endif

/////////////////////
//  cLocalMutations
/////////////////////

cLocalMutations::cLocalMutations(const cMutationLib & _lib, int genome_length)
  : mut_lib(_lib)
  , rates(_lib.GetSize())
{
  // Setup the rates for this specifc organism.
  const tArray<cMutation *> & mut_array = mut_lib.GetMutationArray();
  for (int i = 0; i < rates.GetSize(); i++) {
    if (mut_array[i]->GetScope() == MUTATION_SCOPE_PROP ||
	mut_array[i]->GetScope() == MUTATION_SCOPE_SPREAD) {
      rates[i] = mut_array[i]->GetRate() / (double) genome_length;
    }
    else {
      rates[i] = mut_array[i]->GetRate();
    }
  }

  // Setup the mutation count array.
  counts.Resize(mut_lib.GetSize(), 0);
}

cLocalMutations::~cLocalMutations()
{
}
