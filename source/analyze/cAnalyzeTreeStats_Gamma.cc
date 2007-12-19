/*
 *  cAnalyzeTreeStats_Gamma.cc
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

#include "cAnalyzeTreeStats_Gamma.h"

#include "cAnalyze.h"
#include "cAnalyzeGenotype.h"
#include "tArray.h"
#include "cWorld.h"

#include <math.h>
#include <iostream>

using namespace std;


cAnalyzeTreeStats_Gamma::cAnalyzeTreeStats_Gamma(cWorld* world)
: m_world(world)
, m_gen_array(0)
, m_g(0)
, m_gamma(0.)
{}

void cAnalyzeTreeStats_Gamma::LoadGenotypes(tList<cAnalyzeGenotype> &genotype_list){
  cAnalyzeGenotype * genotype = NULL;
  tListIterator<cAnalyzeGenotype> batch_it(genotype_list);
  
  m_gen_array.Resize(genotype_list.GetSize());
  int array_pos = 0;
  batch_it.Reset();
  while ((genotype = batch_it.Next()) != NULL) {
    m_gen_array[array_pos] = genotype;
    array_pos++;
  }
}
int cAnalyzeTreeStats_Gamma::HeapSortGenotypes(void){
  return HeapSortAGPhyloDepth(m_gen_array);
}
void cAnalyzeTreeStats_Gamma::CalculateInternodeDistances(void){
  m_g.Resize(1 + m_gen_array.GetSize());
  m_g[0] = 0;
  m_g[1] = m_gen_array[0]->GetDepth() - 0;
  for(int i = 1; i < m_gen_array.GetSize(); i++) {
    m_g[i+1] = m_gen_array[i]->GetDepth() - m_gen_array[i-1]->GetDepth();
  }  
}
void cAnalyzeTreeStats_Gamma::FixupInternodeDistances(void){
  bool in_redundant_subsequence = false;
  int saved_g = -1;
  for(int i = 1; i < m_gen_array.GetSize(); i++) {
    /* if we are entering a redundant subsequence, save and then clear first redundant g. */
    if((m_gen_array[i]->GetDepth() == m_gen_array[i-1]->GetDepth()) && (!in_redundant_subsequence)) {
      saved_g = m_g[i];
      m_g[i] = 0;
      in_redundant_subsequence = true;
    }
    
    /* if we are exiting a redundant subsequence, restore the saved redundant g. */
    if((m_gen_array[i]->GetDepth() == m_gen_array[i-1]->GetDepth()) && (in_redundant_subsequence)) {
      m_g[i+1] = saved_g;
      saved_g = -1;
      in_redundant_subsequence = false;
    }
  }
  /* if we fell off the end of a redundant subsequence, restore the saved redundant g. */
  if(in_redundant_subsequence) {
    m_g[m_gen_array.GetSize()] = saved_g;
  }
}
void cAnalyzeTreeStats_Gamma::CalculateGamma(void){
  // n: number of leaves, constant for a given tree.
  int n = m_gen_array.GetSize();
  
  if(n <= 2){
    if(m_world->GetVerbosity() >= VERBOSE_ON) {
      cerr << "Error: not enough genotypes in batch to calculate gamma - " << endl;
    }
    return;
  }

  int T = 0;
  for(int j = 2; j <= n; j++) { T += j*m_g[j]; }
  
  // so: exterior summation
  int so = 0;
  for(int i = 2; i <= n-1; i++) {
    // si: interior summation
    int si = 0;
    for(int k = 2; k <= i; k++) { si += k*m_g[k]; }
    so += si;
  }
  
  m_gamma = ( ( (1./(n-2.)) * so ) - (T/2.) ) / ( T*sqrt( 1. / (12.*(n-2.)) ) );
}
double cAnalyzeTreeStats_Gamma::Gamma(void){
  return m_gamma;
}

void cAnalyzeTreeStats_Gamma::AnalyzeBatch(tList<cAnalyzeGenotype> &genotype_list){
  LoadGenotypes(genotype_list);
  HeapSortGenotypes();
  CalculateInternodeDistances();
  CalculateGamma();
}



// Comparison functions for heapsort.
int CompareAGPhyloDepth(const void * _a, const void * _b){
  cAnalyzeGenotype a(**((cAnalyzeGenotype**)_a));
  cAnalyzeGenotype b(**((cAnalyzeGenotype**)_b));
  if(a.GetDepth() > b.GetDepth()){ return 1; }
  if(a.GetDepth() < b.GetDepth()){ return -1; }
  return 0;
}
int CompareAGUpdateBorn(const void * _a, const void * _b){
  cAnalyzeGenotype a(**((cAnalyzeGenotype**)_a));
  cAnalyzeGenotype b(**((cAnalyzeGenotype**)_b));
  if(a.GetUpdateBorn() > b.GetUpdateBorn()){ return 1; }
  if(a.GetUpdateBorn() < b.GetUpdateBorn()){ return -1; }
  return 0;
}

// Heapsort functions.
int HeapSortAGPhyloDepth(tArray<cAnalyzeGenotype *> &gen_array){
  const int size = gen_array.GetSize();
  cAnalyzeGenotype *c_gen_array[size];
  
  /* Copy unsorted array from gen_array into c_gen_array. */
  for(int i = 0; i < size; i++){
    c_gen_array[i] = (gen_array[i]);
  }
  
  /* Heapsort c_gen_array. */
  int result = heapsort(c_gen_array, size, sizeof(cAnalyzeGenotype*), CompareAGPhyloDepth);
  
  /* If heapsort returned successfully, copy sorted array from c_gen_array into gen_array. */
  if(result == 0){
    for(int i = 0; i < size; i++){
      gen_array[i] = (c_gen_array[i]);
    }
  }
  return result;
}  
int HeapSortAGUpdateBorn(tArray<cAnalyzeGenotype *> &gen_array){
  const int size = gen_array.GetSize();
  cAnalyzeGenotype *c_gen_array[size];
  
  /* Copy unsorted array from gen_array into c_gen_array. */
  for(int i = 0; i < size; i++){
    c_gen_array[i] = (gen_array[i]);
  }
  
  /* Heapsort c_gen_array. */
  int result = heapsort(c_gen_array, size, sizeof(cAnalyzeGenotype*), CompareAGUpdateBorn);
  
  /* If heapsort returned successfully, copy sorted array from c_gen_array into gen_array. */
  if(result == 0){
    for(int i = 0; i < size; i++){
      gen_array[i] = (c_gen_array[i]);
    }
  }
  return result;
}  
