/*
 *  cGenotype_BirthData.cc
 *  Avida
 *
 *  Called "genotype_birth_data.cc" prior to 11/30/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#include "cGenotype_BirthData.h"

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
