/*
 *  cGenotype_BirthData.cc
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#include "cGenotype_BirthData.h"

#include "defs.h"


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
