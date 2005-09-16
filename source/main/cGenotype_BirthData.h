//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GENOTYPE_BIRTH_DATA_HH
#define GENOTYPE_BIRTH_DATA_HH

#ifndef COUNT_TRACKER_HH
#include "count_tracker.hh"
#endif

#ifndef TARRAY_HH
#include "tArray.hh"
#endif

class cGenotype;
class cSpecies;
class cGenotype_BirthData {
public:
  cGenotype_BirthData(int in_update_born);
  ~cGenotype_BirthData();

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

#endif
