/*
 *  cGenotype_BirthData.h
 *  Avida
 *
 *  Called "genotype_birth_data.hh" prior to 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cGenotype_BirthData_h
#define cGenotype_BirthData_h

#ifndef cCountTracker_h
#include "cCountTracker.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cGenotype;
class cSpecies;

class cGenotype_BirthData {
public:
  cGenotype_BirthData(int in_update_born);
  ~cGenotype_BirthData() { ; }

  cCountTracker birth_track;
  cCountTracker death_track;
  cCountTracker breed_in_track;
  cCountTracker breed_true_track;
  cCountTracker breed_out_track;

  int update_born;      // Update genotype was first created
  int parent_distance;  // Genetic distance from parent genotype
  int gene_depth;       // depth in the phylogenetic tree from ancestor
  int lineage_label;    // Unique label for the lineage of this genotype.

  int update_deactivated;       // If not, when did it get deactivated?
  cGenotype * parent_genotype;  // Pointer to parent genotype...
  cGenotype * parent2_genotype; // Pointer to secondary parent genotype...
  cSpecies * parent_species;
  int num_offspring_genotypes;  // Num offspring genotypes still in memory.

  // Ancestral IDs.  This array contains all of the information about the
  // ids of the ancestors.  It will have one entry if this is an asexual
  // population, otherwise:
  // [0]=parent1, [1]=parent2, [2]&[3]=grandparents 1, [4]&[5]=grandparents 2
  tArray<int> ancestor_ids;
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenotype_BirthData {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif
