/*
 *  cPopulationCell.cc
 *  Avida
 *
 *  Called "pop_cell.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cPopulationCell.h"

#include "nHardware.h"
#include "cOrganism.h"
#include "cTools.h"

using namespace std;


cPopulationCell::cPopulationCell()
  : m_world(NULL)
  , organism(NULL)
  , mutation_rates(NULL)
  , organism_count(0)
{
}

cPopulationCell::cPopulationCell(const cPopulationCell& in_cell)
  : m_world(in_cell.m_world)
  , organism(in_cell.organism)
  , cell_id(in_cell.cell_id)
  , deme_id(in_cell.deme_id)
  , organism_count(in_cell.organism_count)
{
  for (int i = 0; i < nHardware::IO_SIZE; i++) input_array[i] = in_cell.input_array[i];
  mutation_rates = new cMutationRates(*in_cell.mutation_rates);
  tConstListIterator<cPopulationCell> conn_it(in_cell.connection_list);
  cPopulationCell * test_cell;
  while ( (test_cell = (cPopulationCell *) conn_it.Next()) != NULL) {
    connection_list.PushRear(test_cell);
  }
}

void cPopulationCell::operator=(const cPopulationCell& in_cell)
{
  m_world = in_cell.m_world;
  organism = in_cell.organism;
  for (int i = 0; i < nHardware::IO_SIZE; i++) input_array[i] = in_cell.input_array[i];
  cell_id = in_cell.cell_id;
  deme_id = in_cell.deme_id;
  organism_count = in_cell.organism_count;
  if (mutation_rates == NULL)
    mutation_rates = new cMutationRates(*in_cell.mutation_rates);
  else
    mutation_rates->Copy(*in_cell.mutation_rates);
  tConstListIterator<cPopulationCell> conn_it(in_cell.connection_list);
  cPopulationCell * test_cell;
  while ( (test_cell = (cPopulationCell *) conn_it.Next()) != NULL) {
    connection_list.PushRear(test_cell);
  }
}

void cPopulationCell::Setup(cWorld* world, int in_id, const cMutationRates& in_rates)
{
  m_world = world;
  cell_id = in_id;
  deme_id = -1;
  
  if (mutation_rates == NULL)
    mutation_rates = new cMutationRates(in_rates);
  else
    mutation_rates->Copy(in_rates);
}

void cPopulationCell::Rotate(cPopulationCell & new_facing)
{
  // @CAO Note, this breaks avida if new_facing is not in connection_list

#ifdef DEBUG
  int scan_count = 0;
#endif
  while (connection_list.GetFirst() != &new_facing) {
    connection_list.CircNext();
#ifdef DEBUG
    assert(++scan_count < connection_list.GetSize());
#endif
  }
}


int cPopulationCell::GetInputAt(int & input_pointer)
{
  if (input_pointer >= nHardware::IO_SIZE) input_pointer = 0;
  return input_array[input_pointer++];
}

int cPopulationCell::GetInput(int id)
{
  assert(id >= 0 && id < nHardware::IO_SIZE);
  return input_array[id];
}

void cPopulationCell::InsertOrganism(cOrganism & new_org)
{
  assert(&new_org != NULL);
  assert(new_org.GetGenotype() != NULL);
  assert(organism == NULL);

  // Adjust this cell's attributes to account for the new organism.
  organism = &new_org;
  organism_count++;

  // Adjust the organism's attributes to match this cell.
  organism->GetOrgInterface().SetCellID(cell_id);
}

cOrganism * cPopulationCell::RemoveOrganism()
{
  if (organism == NULL) return NULL;   // Nothing to do!

  // For the moment, the cell doesn't keep track of much...
  cOrganism * out_organism = organism;
  organism = NULL;
  return out_organism;
}


bool cPopulationCell::OK()
{
  // Nothing for the moment...
  return true;
}
