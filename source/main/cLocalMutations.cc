/*
 *  cLocalMutations.cc
 *  Avida
 *
 *  Called "local_mutations.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cLocalMutations.h"

#include "cMutation.h"
#include "cMutationLib.h"
#include "nMutation.h"


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
