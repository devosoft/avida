/*
 *  cAnalyzeTreeStats_Gamma.h
 *  Avida@vallista
 *
 *  Created by Kaben Nanlohy on 2007.12.03.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cAnalyzeTreeStats_Gamma_h
#define cAnalyzeTreeStats_Gamma_h

#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cAnalyzeGenotype;
class cWorld;

class cAnalyzeTreeStats_Gamma {
public:
  cWorld* m_world;
  tArray<cAnalyzeGenotype *> m_gen_array;
  tArray<int> m_g;
  double m_gamma;
public:
  cAnalyzeTreeStats_Gamma(cWorld* world);
  
  void LoadGenotypes(tList<cAnalyzeGenotype> &genotype_list);
  void QSortGenotypes(void);
  void CalculateInternodeDistances(void);
  void FixupInternodeDistances(void);
  void CalculateGamma(void);
  
  double Gamma(void);
  
  // Commands.
  void AnalyzeBatch(tList<cAnalyzeGenotype> &genotype_list);
};

// Comparison functions for qsort.
int CompareAGPhyloDepth(const void * _a, const void * _b);
int CompareAGUpdateBorn(const void * _a, const void * _b);
// Quicksort functions.
void QSortAGPhyloDepth(tArray<cAnalyzeGenotype *> &gen_array);
void QSortAGUpdateBorn(tArray<cAnalyzeGenotype *> &gen_array);

#endif
