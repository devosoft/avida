//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GENOTYPE_BIRTH_DATA_HH
#include "genotype_birth_data.hh"
#endif

#ifndef DEFS_HH
#include "defs.hh"
#endif

/////////////////////////
//  cGenotype_BirthData
/////////////////////////

cGenotype_BirthData::cGenotype_BirthData(int in_update_born)
  : update_born(in_update_born)
  , parent_distance(-1)
  , gene_depth(0)
  , update_deactivated(-1)
  , parent_genotype(NULL)
  , parent_species(NULL)
  , num_offspring_genotypes(0)
{
  // @CAO: we should do a test to see if we have a sexual population.  For now
  // we will assume we do.
  ancestor_ids.Resize(6);
  ancestor_ids.SetAll(-1);
}

cGenotype_BirthData::~cGenotype_BirthData()
{
}
