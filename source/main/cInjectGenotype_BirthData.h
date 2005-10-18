//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENOTYPE_BIRTH_DATA_HH
#define INJECT_GENOTYPE_BIRTH_DATA_HH

#ifndef COUNT_TRACKER_HH
#include "cCountTracker.h"
#endif

class cInjectGenotype;
class cInjectGenotype_BirthData {
public:
  cInjectGenotype_BirthData(int in_update_born);
  ~cInjectGenotype_BirthData();

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

#endif
