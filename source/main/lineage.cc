//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LINEAGE_HH
#include "lineage.hh"
#endif
#ifndef GENOTYPE_HH
#include "cGenotype.h"
#endif

using namespace std;


bool gt_gentype::operator()(const cGenotype * g1, const cGenotype * g2) const
{
  return g1->GetID() > g2->GetID();
}

int cLineage::m_max_id = 0;

cLineage::cLineage(double start_fitness, int parent_id, int id, int update,
		   double generation,  double lineage_stat1, double lineage_stat2)
  : m_parent_id(parent_id)
  , m_update_born(update)
  , m_num_CPUs(0)
  , m_total_CPUs(0)
  , m_total_genotypes(0)
  , m_generation_born(generation)
  , m_lineage_stat1(lineage_stat1)
  , m_lineage_stat2(lineage_stat2)
{
  if ( id < 0 )
    m_id = m_max_id++;
  else{
    m_id = id;
    if ( id >= m_max_id )
      m_max_id = id + 1;
  }
  m_start_fitness = m_max_fitness = m_max_fitness_ever = start_fitness;
  m_ave_fitness = 0;
  m_ave_fitness_changed = true;
  m_threshold = false;
}

cLineage::~cLineage()
{
}

void cLineage::AddCreature(cGenotype * genotype)
{
  // add the new genotype to the map if necessary, otherwise
  // find its position
  pair<map<const cGenotype *, int, gt_gentype>::iterator, bool> p =
    m_genotype_map.insert( pair<const cGenotype*, int>( genotype, 0 ) );

  // did we add a new genotype?
  if ( ( p.second ) == true )
    m_total_genotypes += 1;

  // increase the count for the given genotype by one.
  (*(p.first)).second += 1;

  // The above lines could be written like that:
  //     m_genotype_map[ genotype ] += 1;
  // if we didn't want to count how often we add a new genotype

  double fitness = genotype->GetTestColonyFitness();

  // adjust the current maximum fitness
  if ( fitness > m_max_fitness )
    m_max_fitness = fitness;
   // adjust overall maxiumum fitness
  if ( fitness> m_max_fitness_ever )
    m_max_fitness_ever = fitness;
  // the average fitness has changed as well
  m_ave_fitness_changed = true;

  m_num_CPUs++;
  m_total_CPUs++;
}

bool cLineage::RemoveCreature(cGenotype * genotype)
{
  // we return true if the removal of this creature triggers a
  // recalculation of the best lineage
  bool result = false;

  map<const cGenotype *, int, gt_gentype>::iterator cur = m_genotype_map.find( genotype );

  // is the genotype part of the map?
  if ( cur == m_genotype_map.end() ){
    cerr << "Removing creature from lineage whose genotype is not part of the lineage!" << endl;
    cerr << "  " << genotype->GetName()() << " " << GetID() << endl;
    return false;
  }

  // yes, decrease the count
  (*cur).second -= 1;
  // and adjust the average fitness
  m_ave_fitness_changed = true;

  double fitness = genotype->GetTestColonyFitness();

  // did we reach zero?
  if ( (*cur).second == 0 ){
    // yes, remove the entry
    m_genotype_map.erase( cur );
    // make sure that the maximum fitness is correct
    if (fitness == m_max_fitness) CalcCurrentFitness();
    // adjust the return value
    result = true;
  }

  // and now the total cpu count
  m_num_CPUs--;

  return result;
}

int cLineage::CountNumCreatures() const
{
  int creature_count = 0;

  map<const cGenotype *, int, gt_gentype>::const_iterator it =
    m_genotype_map.begin();

  for ( ; it!=m_genotype_map.end(); it++ )
    creature_count += (*it).second;

  return creature_count;
}

void cLineage::CalcCurrentFitness() const
{
  m_max_fitness = 0.0;
  map<const cGenotype *, int, gt_gentype>::const_iterator it =
    m_genotype_map.begin();

  // we calculate the average fitness as well, since it is so easy.
  m_ave_fitness = 0;
  for ( ; it!=m_genotype_map.end(); it++ ){
    double fitness = (*it).first->GetTestColonyFitness();
    if (fitness > m_max_fitness) m_max_fitness = fitness;
    if (fitness > m_max_fitness_ever) m_max_fitness_ever = fitness;
    m_ave_fitness += fitness * (*it).second;
  }
  m_ave_fitness /= (double) m_num_CPUs;
  m_ave_fitness_changed = false;
}


