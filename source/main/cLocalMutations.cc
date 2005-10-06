//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LOCAL_MUTATIONS_HH
#include "cLocalMutations.h"
#endif

#ifndef MUTATION_HH
#include "cMutation.h"
#endif
#ifndef MUTATION_LIB_HH
#include "cMutationLib.h"
#endif
#ifndef nMutation_h
#include "nMutation.h"
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
    if (mut_array[i]->GetScope() == nMutation::SCOPE_PROP ||
	mut_array[i]->GetScope() == nMutation::SCOPE_SPREAD) {
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
