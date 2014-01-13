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

#include "tList.h"

class cAnalyzeGenotype;
class cWorld;

// Comparison functions for qsort.
int CompareAGPhyloDepth(cAnalyzeGenotype* const& a, cAnalyzeGenotype* const& b);
int CompareAGUpdateBorn(cAnalyzeGenotype* const& a, cAnalyzeGenotype* const& b);

// Quicksort functions.
void QSortAGPhyloDepth(Apto::Array<cAnalyzeGenotype *> &gen_array);
void QSortAGUpdateBorn(Apto::Array<cAnalyzeGenotype *> &gen_array);



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
  Apto::Array<cAnalyzeGenotype *> m_gen_array;
  Apto::Array<cAnalyzeLineageFurcation> m_furcations;
  Apto::Array<int> m_furcation_times;
  Apto::Array<int> m_internode_distances;
  double m_gamma;
public:
  cAnalyzeTreeStats_Gamma(cWorld* world);
  
  void LoadGenotypes(tList<cAnalyzeGenotype> &genotype_list);
  void MapIDToGenotypePos(Apto::Array<cAnalyzeGenotype *>&lineage, Apto::Map<int, int>& out_mapping);
  void Unlink(Apto::Array<cAnalyzeGenotype *>& lineage);
  void EstablishLinks(Apto::Array<cAnalyzeGenotype *>& lineage, Apto::Map<int, int>& out_mapping);
  void FindFurcations(
    Apto::Array<cAnalyzeGenotype *> &lineage,
    Apto::Array<cAnalyzeLineageFurcation> &out_furcations
  );
  void FindFurcationTimes(
    Apto::Array<cAnalyzeGenotype *> &lineage,
    int (*furcation_time_policy)(cAnalyzeLineageFurcation &furcation),
    Apto::Array<int> &out_furcation_times
  );
  void FindInternodeDistances(
    Apto::Array<int> &furcation_times,
    int end_time,
    Apto::Array<int> &out_internode_distances
  );
  double CalculateGamma(Apto::Array<int> &inode_dists);

  // Accessors.
  const Apto::Array<int> &FurcationTimes(void) const;
  const Apto::Array<int> &InternodeDistances(void) const;
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
