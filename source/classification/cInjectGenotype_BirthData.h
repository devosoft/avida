/*
 *  cInjectGenotype_BirthData.h
 *  Avida
 *
 *  Called "inject_genotype_birth_data.hh" prior to 11/30/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cInjectGenotype_BirthData_h
#define cInjectGenotype_BirthData_h

#ifndef cCountTracker_h
#include "cCountTracker.h"
#endif

class cInjectGenotype;

class cInjectGenotype_BirthData {
public:
  inline cInjectGenotype_BirthData(int in_update_born);
  ~cInjectGenotype_BirthData() { ; }

  cCountTracker birth_track;
  cCountTracker death_track;
  //cCountTracker breed_in_track;
  //cCountTracker breed_true_track;
  //cCountTracker breed_out_track;

  int update_born;      // Update genotype was first created
  int parent_id;        // ID of parent genotype
  //int parent_distance;  // Genetic distance from parent genotype
  int gene_depth;       // depth in the phylogenetic tree from ancestor

  int update_deactivated;      // If not, when did it get deactivated?
  cInjectGenotype * parent_genotype; // Pointer to parent genotype...
  //cSpecies * parent_species;
  int num_offspring_genotypes; // Num offspring genotypes still in memory.
};


#ifdef ENABLE_UNIT_TESTS
namespace nInjectGenotype_BirthData {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

inline cInjectGenotype_BirthData::cInjectGenotype_BirthData(int in_update_born)
: update_born(in_update_born)
, parent_id(-1)
, gene_depth(0)
, update_deactivated(-1)
, parent_genotype(NULL)
, num_offspring_genotypes(0)
{
}

#endif
