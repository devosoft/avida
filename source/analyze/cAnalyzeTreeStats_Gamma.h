/*
 *  cAnalyzeTreeStats_Gamma.h
 *  Avida@vallista
 *
 *  Created by Kaben Nanlohy on 2007.12.03.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cAnalyzeTreeStats_Gamma_h
#define cAnalyzeTreeStats_Gamma_h

#include "apto/core.h"

#include "tArray.h"
#include "tList.h"

class cAnalyzeGenotype;
class cWorld;

// Comparison functions for qsort.
int CompareAGPhyloDepth(const void * _a, const void * _b);
int CompareAGUpdateBorn(const void * _a, const void * _b);
// Quicksort functions.
void QSortAGPhyloDepth(tArray<cAnalyzeGenotype *> &gen_array);
void QSortAGUpdateBorn(tArray<cAnalyzeGenotype *> &gen_array);



/* Rewrite */
class cAnalyzeLineageFurcation {
public:
  cAnalyzeGenotype *m_parent;
  cAnalyzeGenotype *m_first_child;
  cAnalyzeGenotype *m_second_child;
public:
  cAnalyzeLineageFurcation(
    cAnalyzeGenotype *parent = 0,
    cAnalyzeGenotype *first_child = 0,
    cAnalyzeGenotype *second_child = 0
  );
  /* This equality operator compares pointers -- this is bad form, but propagated from a similar equality operator in cAnalyzeGenotype. */
  bool operator==(const cAnalyzeLineageFurcation &in) const ;
};

class cAnalyzeTreeStats_Gamma {
public:
  cWorld* m_world;
  tArray<cAnalyzeGenotype *> m_gen_array;
  tArray<cAnalyzeLineageFurcation> m_furcations;
  tArray<int> m_furcation_times;
  tArray<int> m_internode_distances;
  double m_gamma;
public:
  cAnalyzeTreeStats_Gamma(cWorld* world);
  
  void LoadGenotypes(tList<cAnalyzeGenotype> &genotype_list);
  void MapIDToGenotypePos(tArray<cAnalyzeGenotype *>&lineage, Apto::Map<int, int>& out_mapping);
  void Unlink(tArray<cAnalyzeGenotype *>& lineage);
  void EstablishLinks(tArray<cAnalyzeGenotype *>& lineage, Apto::Map<int, int>& out_mapping);
  void FindFurcations(
    tArray<cAnalyzeGenotype *> &lineage,
    tArray<cAnalyzeLineageFurcation> &out_furcations
  );
  void FindFurcationTimes(
    tArray<cAnalyzeGenotype *> &lineage,
    int (*furcation_time_policy)(cAnalyzeLineageFurcation &furcation),
    tArray<int> &out_furcation_times
  );
  void FindInternodeDistances(
    tArray<int> &furcation_times,
    int end_time,
    tArray<int> &out_internode_distances
  );
  double CalculateGamma(tArray<int> &inode_dists);

  // Accessors.
  const tArray<int> &FurcationTimes(void) const;
  const tArray<int> &InternodeDistances(void) const;
  double Gamma(void);
  
  // Commands.
  void AnalyzeBatch(
    tList<cAnalyzeGenotype> &genotype_list,
    int end_time,
    int furcation_time_convention
  );    
};
  
int FurcationTimePolicy_ParentBirth(cAnalyzeLineageFurcation &furcation);
int FurcationTimePolicy_FirstChildBirth(cAnalyzeLineageFurcation &furcation);
int FurcationTimePolicy_SecondChildBirth(cAnalyzeLineageFurcation &furcation);

#endif
