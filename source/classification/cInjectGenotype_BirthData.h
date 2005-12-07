/*
 *  cInjectGenotype_BirthData.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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
