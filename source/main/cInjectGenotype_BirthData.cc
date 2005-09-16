//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENOTYPE_BIRTH_DATA_HH
#include "cInjectGenotype_BirthData.h"
#endif

#ifndef DEFS_HH
#include "defs.hh"
#endif

/////////////////////////
//  cInjectGenotype_BirthData
/////////////////////////

cInjectGenotype_BirthData::cInjectGenotype_BirthData(int in_update_born)
  : update_born(in_update_born)
  , parent_id(-1)
  , gene_depth(0)
  , update_deactivated(-1)
  , parent_genotype(NULL)
  , num_offspring_genotypes(0)
{
}

cInjectGenotype_BirthData::~cInjectGenotype_BirthData()
{
}
