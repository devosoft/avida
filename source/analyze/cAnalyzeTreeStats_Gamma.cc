/*
 *  cAnalyzeTreeStats_Orig_Gamma.cc
 *  Avida@vallista
 *
 *  Created by Kaben Nanlohy on 2007.12.03.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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


cAnalyzeTreeStats_Orig_Gamma::cAnalyzeTreeStats_Orig_Gamma(cWorld* world)
: m_world(world)
, m_gen_array(0)
, m_g(0)
, m_gamma(0.)
{}

void cAnalyzeTreeStats_Orig_Gamma::LoadGenotypes(tList<cAnalyzeGenotype> &genotype_list){
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
void cAnalyzeTreeStats_Orig_Gamma::QSortGenotypes(void){
  QSortAGPhyloDepth(m_gen_array);
}
void cAnalyzeTreeStats_Orig_Gamma::CalculateInternodeDistances(void){
  m_g.Resize(1 + m_gen_array.GetSize());
  m_g[0] = 0;
  m_g[1] = m_gen_array[0]->GetDepth() - 0;
  for(int i = 1; i < m_gen_array.GetSize(); i++) {
    m_g[i+1] = m_gen_array[i]->GetDepth() - m_gen_array[i-1]->GetDepth();
  }  
}
void cAnalyzeTreeStats_Orig_Gamma::FixupInternodeDistances(void){
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
void cAnalyzeTreeStats_Orig_Gamma::CalculateGamma(void){
  // n: number of leaves, constant for a given tree.
  int n = m_gen_array.GetSize();
  
  if(n <= 2){
    if(m_world->GetVerbosity() >= VERBOSE_ON) {
      cerr << "Error: not enough genotypes in batch to calculate gamma - " << endl;
    }
    return;
  }

  unsigned long long T = 0;
  for(int j = 2; j <= n; j++) { T += j*m_g[j]; }
  
  // so: exterior summation
  unsigned long long so = 0;
  for(int i = 2; i <= n-1; i++) {
    // si: interior summation
    unsigned long long si = 1;
    for(int k = 2; k <= i; k++) { si += k*m_g[k]; }
    so += si;
  }
  
  m_gamma = ( ( (1./(n-2.)) * so ) - (T/2.) ) / ( T*sqrt( 1. / (12.*(n-2.)) ) );
}
double cAnalyzeTreeStats_Orig_Gamma::Gamma(void){
  return m_gamma;
}

void cAnalyzeTreeStats_Orig_Gamma::AnalyzeBatch(tList<cAnalyzeGenotype> &genotype_list){
  LoadGenotypes(genotype_list);
  QSortGenotypes();
  CalculateInternodeDistances();
  CalculateGamma();
}



// Comparison functions for qsort.
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

// Quicksort functions.
void QSortAGPhyloDepth(tArray<cAnalyzeGenotype *> &gen_array){
  gen_array.QSort(CompareAGPhyloDepth);
} 

void QSortAGUpdateBorn(tArray<cAnalyzeGenotype *> &gen_array){
  gen_array.QSort(CompareAGUpdateBorn);
}  



/* Rewrite */
cAnalyzeLineageFurcation::cAnalyzeLineageFurcation(
  cAnalyzeGenotype *parent,
  cAnalyzeGenotype *first_child,
  cAnalyzeGenotype *second_child
):m_parent(parent), m_first_child(first_child), m_second_child(second_child)
{
}
bool cAnalyzeLineageFurcation::operator==(const cAnalyzeLineageFurcation &in) const {
  if (m_parent != in.m_parent) { return false; }
  if (m_first_child != in.m_first_child) { return false; }
  if (m_second_child != in.m_second_child) { return false; }
  return true;
}

int FurcationTimePolicy_ParentBirth(cAnalyzeLineageFurcation &furcation){
  if(furcation.m_parent){ return furcation.m_parent->GetUpdateBorn();
  } else { return -1; }
}
int FurcationTimePolicy_FirstChildBirth(cAnalyzeLineageFurcation &furcation){
  if(furcation.m_first_child){ return furcation.m_first_child->GetUpdateBorn();
  } else { return -1; }
}
int FurcationTimePolicy_SecondChildBirth(cAnalyzeLineageFurcation &furcation){
  if(furcation.m_second_child){ return furcation.m_second_child->GetUpdateBorn();
  } else { return -1; }
}
int CompareInt(const void * _a, const void * _b){
  int a(*((int*)_a));
  int b(*((int*)_b));
  if(a > b){ return 1; }
  if(a < b){ return -1; }
  return 0;
}


cAnalyzeTreeStats_Gamma::cAnalyzeTreeStats_Gamma(cWorld* world)
: m_world(world)
{
}

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

void cAnalyzeTreeStats_Gamma::MapIDToGenotypePos(
  tArray<cAnalyzeGenotype *> &lineage,
  tHashTable<int, int> &out_mapping
){
  out_mapping.ClearAll();
  for(int i = 0; i < lineage.GetSize(); i++){
    out_mapping.SetValue(lineage[i]->GetID(), i);
  }
}

void cAnalyzeTreeStats_Gamma::Unlink(tArray<cAnalyzeGenotype *> &lineage){
  for(int i = 0; i < lineage.GetSize(); i++){
    lineage[i]->Unlink();
  }
}

void cAnalyzeTreeStats_Gamma::EstablishLinks(
  tArray<cAnalyzeGenotype *> &lineage,
  tHashTable<int, int> &out_mapping
){
  this->Unlink(lineage);
  this->MapIDToGenotypePos(lineage, out_mapping);

  int parent_id(-1);
  int parent_index(-1);

  for(int i = 0; i < lineage.GetSize(); i++){
    parent_id = lineage[i]->GetParentID();
    if(parent_id >= 0){
      out_mapping.Find(parent_id, parent_index);
      lineage[parent_index]->LinkChild(*lineage[i]);
    }
  }
}

void cAnalyzeTreeStats_Gamma::FindFurcations(
  tArray<cAnalyzeGenotype *> &lineage,
  tArray<cAnalyzeLineageFurcation> &out_furcations
){
  cAnalyzeGenotype *parent(0);
  cAnalyzeLineageFurcation furcation(0,0,0);
  int child_list_size(0);

  out_furcations.Resize(0);
  for(int i = 0; i < lineage.GetSize(); i++){
    parent = lineage[i];

    child_list_size = parent->GetChildList().GetSize();
    if(child_list_size > 1){
      for(int j = 1; j < child_list_size; j++){
        furcation = cAnalyzeLineageFurcation(
          parent,
          parent->GetChildList().GetPos(j-1),
          parent->GetChildList().GetPos(j)
        );
        out_furcations.Push(furcation);
        if (m_world->GetVerbosity() >= VERBOSE_DETAILS){
          cout << "Furcation("
          << "parent_id=" << furcation.m_parent->GetID() << ", "
          << "elder_child_id=" << furcation.m_first_child->GetID() << ", "
          << "younger_child_id=" << furcation.m_second_child->GetID() << ")" << endl;
        }
      }
    }
  }
}

void cAnalyzeTreeStats_Gamma::FindFurcationTimes(
  tArray<cAnalyzeGenotype *> &lineage,
  int (*furcation_time_policy)(cAnalyzeLineageFurcation &furcation),
  tArray<int> &out_furcation_times
){
  /*
  furcation_time_policy is one of
    int FurcationTimePolicy_ParentBirth(cAnalyzeLineageFurcation &furcation);
    int FurcationTimePolicy_FirstChildBirth(cAnalyzeLineageFurcation &furcation);
    int FurcationTimePolicy_SecondChildBirth(cAnalyzeLineageFurcation &furcation);
  */
  tArray<cAnalyzeLineageFurcation> furcations;
  FindFurcations(lineage, furcations);

  int size = furcations.GetSize();
  out_furcation_times.Resize(size, 0);
  for(int i = 0; i < size; i++){
    out_furcation_times[i] = furcation_time_policy(furcations[i]);
    if (m_world->GetVerbosity() >= VERBOSE_DETAILS){
      cout << "FurcationTime("
      << "parent_id=" << furcations[i].m_parent->GetID() << ") = "
      << out_furcation_times[i] <<  endl;
    }
  }
  out_furcation_times.QSort(CompareInt);
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS){
    for(int i = 0; i < size; i++){
      cout << "SortedFurcationTime("
      << "num_lineages=" << i+2 << ") = "
      << out_furcation_times[i] <<  endl;
    }
  }
}

void cAnalyzeTreeStats_Gamma::FindInternodeDistances(
  tArray<int> &furcation_times,
  int end_time,
  tArray<int> &out_internode_distances
){
  int size = furcation_times.GetSize();
  out_internode_distances.Resize(size, 0);

  for(int i = 0; i < size-1; i++){
    out_internode_distances[i] = furcation_times[i+1] - furcation_times[i];
  }
  out_internode_distances[size-1] = end_time - furcation_times[size-1];

  if (m_world->GetVerbosity() >= VERBOSE_DETAILS){
    for(int i = 0; i < size; i++){
      cout << "g_" << i+2 << " = InternodeDistance("
      << "num_lineages=" << i+2 << ") = "
      << out_internode_distances[i] <<  endl;
    }
  }
}

double cAnalyzeTreeStats_Gamma::CalculateGamma(tArray<int> &inode_dists){
  // n: number of leaves, constant for a given tree.
  int n = inode_dists.GetSize() + 1;
  
  if(n <= 2){
    if(m_world->GetVerbosity() >= VERBOSE_ON) {
      cerr << "Error: not enough genotypes in batch to calculate gamma - " << endl;
    }
    return nan("0");
  }

  /*
   For convenience, make a copy of m_g with the indices offset.
   This permits use of the same indices as in the formula for gamma.
   */
  tArray<int> g = tArray<int>(2, 0) + inode_dists;
  
  int T = 0;
  for(int j = 2; j <= n; j++) { T += j*g[j]; }

  // si: interior summation values, cached
  tArray<int> si(n, 0);
  for(int k = 2; k <= n-1; k++) { si[k] = k*g[k] + si[k-1]; }    

  // so: exterior summation
  int so = 0;
  for(int i = 2; i <= n-1; i++) { so += si[i]; }
  
  m_gamma = ( ( (1./(n-2.)) * so ) - (T/2.) ) / ( T*sqrt( 1. / (12.*(n-2.)) ) );
  
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS){
    for(int i = 2; i <= n; i++){
      cout << i << "*g_" << i << " = " << i*g[i] << endl;
    }
    for(int i = 2; i <= n-1; i++){
      cout << "sum(k=2.." << i << ", k*g_k) = " << si[i] << endl;
    }
    cout << "double_sum = " << so << endl;
    cout << "T = " << T << endl;
    cout << "gamma_numerator = " << ( ( (1./(n-2.)) * so ) - (T/2.) ) << endl;
    cout << "gamma_denominator = " << ( T*sqrt( 1. / (12.*(n-2.)) ) ) << endl;
    cout << "gamma = " << m_gamma << endl;
  }
  
  return m_gamma;
}

double cAnalyzeTreeStats_Gamma::Gamma(void){
  return m_gamma;
}

void cAnalyzeTreeStats_Gamma::AnalyzeBatch(
  tList<cAnalyzeGenotype> &genotype_list,
  int end_time,
  int furcation_time_convention
){
  tHashTable<int, int> mapping;
  tArray<int> furcation_times;
  tArray<int> internode_distances;

  int (*furcation_time_policy)(cAnalyzeLineageFurcation &furcation);
  furcation_time_policy = 0;
  if (furcation_time_convention == 1){
    furcation_time_policy = FurcationTimePolicy_ParentBirth;
  } else if (furcation_time_convention == 2){
    furcation_time_policy = FurcationTimePolicy_FirstChildBirth;
  } else if (furcation_time_convention == 3){
    furcation_time_policy = FurcationTimePolicy_SecondChildBirth;
  } else {
    /* Bad furcation time convention specified. */
    if(m_world->GetVerbosity() >= VERBOSE_ON) {
      cerr << "Error: Bad furcation time convention specified." << endl;
      cerr << " - choices are" << endl;
      cerr << " - 1: Use parent's birth time" << endl;
      cerr << " - 2: Use elder child's birth time" << endl;
      cerr << " - 3: Use younger child's birth time" << endl;
    }
  }

  LoadGenotypes(genotype_list);
  EstablishLinks(m_gen_array, mapping);
  FindFurcationTimes(m_gen_array, furcation_time_policy, furcation_times);

  if (end_time < furcation_times[furcation_times.GetSize() - 1]){
    /* Bad furcation time convention specified. */
    if(m_world->GetVerbosity() >= VERBOSE_ON) {
      cerr << "Error: Lineage end time comes before last furcation." << endl;
    }
  }

  FindInternodeDistances(furcation_times, end_time, internode_distances);
  CalculateGamma(internode_distances);
}

