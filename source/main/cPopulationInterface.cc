//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_INTERFACE_HH
#include "cPopulationInterface.h"
#endif

#include <assert.h>

#ifndef NULL
#define NULL 0
#endif

cPopulationInterface::cPopulationInterface()
  : population(NULL)
  , cell_id(-1)
  , fun_new_hardware(NULL)
  , fun_recycle(NULL)
  , fun_divide(NULL)
  , fun_test_on_divide(NULL)
  , fun_get_neighbor(NULL)
  , fun_birth_chamber(NULL)
  , fun_num_neighbors(NULL)
  , fun_rotate(NULL)
  , fun_breakpoint(NULL)
  , fun_test_fitness(NULL)
  , fun_get_input(NULL)
  , fun_get_input_at(NULL)
  , fun_debug(NULL)
  , fun_get_resources(NULL)
  , fun_update_resources(NULL)
  , fun_kill_cell(NULL)
  , fun_kill_surround_cell(NULL)
  , fun_send_message(NULL)
  , fun_receive_value(NULL)
  , fun_inject_parasite(NULL)
  , fun_update_merit(NULL)
{
}


cPopulationInterface::~cPopulationInterface()
{
}

void cPopulationInterface::CopyCallbacks(cPopulationInterface & in_interface)
{
  fun_new_hardware     = in_interface.fun_new_hardware;
  fun_recycle          = in_interface.fun_recycle;
  fun_divide           = in_interface.fun_divide;
  fun_test_on_divide   = in_interface.fun_test_on_divide;
  fun_get_neighbor     = in_interface.fun_get_neighbor;
  fun_birth_chamber    = in_interface.fun_birth_chamber;
  fun_num_neighbors    = in_interface.fun_num_neighbors;
  fun_rotate           = in_interface.fun_rotate;
  fun_breakpoint       = in_interface.fun_breakpoint;
  fun_test_fitness     = in_interface.fun_test_fitness;
  fun_get_input        = in_interface.fun_get_input;
  fun_debug            = in_interface.fun_debug;
  fun_get_resources    = in_interface.fun_get_resources;
  fun_update_resources = in_interface.fun_update_resources;
  fun_kill_cell        = in_interface.fun_kill_cell;
  fun_kill_surround_cell = in_interface.fun_kill_surround_cell;
  fun_send_message     = in_interface.fun_send_message;
  fun_receive_value    = in_interface.fun_receive_value;
  fun_inject_parasite  = in_interface.fun_inject_parasite;
  fun_update_merit     = in_interface.fun_update_merit;
}

cHardwareBase * cPopulationInterface::NewHardware(cOrganism * owner)
{
  assert(fun_new_hardware != NULL);  // All interfaces must have a NewHardware!
  return (*fun_new_hardware)(population, owner);
}


void cPopulationInterface::RecycleHardware(cHardwareBase * out_hardware)
{
  assert(fun_recycle != NULL);  // All interfaces must have a RecycleHArdware!
  (*fun_recycle)(out_hardware);
}


bool cPopulationInterface::Divide(cOrganism * parent, cGenome & child_genome)
{
  assert (fun_divide != NULL);       // All interfaces must have a Divide!
  return (*fun_divide)(population, cell_id, parent, child_genome);  
}

bool cPopulationInterface::TestOnDivide() const
{
  if (population == NULL) return false;
  assert(fun_test_on_divide != NULL);
  return (*fun_test_on_divide)(population, cell_id);
}

cOrganism * cPopulationInterface::GetNeighbor()
{
  if (fun_get_neighbor == NULL) return NULL;

  assert(cell_id >= 0);
  return (*fun_get_neighbor)(population, cell_id);
}

cBirthChamber & cPopulationInterface::GetBirthChamber()
{
  assert(fun_birth_chamber != NULL);
  
  return (*fun_birth_chamber)(population, cell_id);
}

int cPopulationInterface::GetNumNeighbors()
{
  if (fun_num_neighbors == NULL) return 0;

  assert(cell_id >= 0);
  return (*fun_num_neighbors)(population, cell_id);
}

void cPopulationInterface::Rotate(int direction)
{
  if (fun_rotate == NULL) return;

  assert(cell_id >= 0);
  (*fun_rotate)(population, cell_id, direction);
}

void cPopulationInterface::Breakpoint()
{
  if (fun_breakpoint == NULL) return;
  (*fun_breakpoint)();
}

double cPopulationInterface::TestFitness()
{
  if (fun_test_fitness == NULL) return -1.0;
  return (*fun_test_fitness)(population, cell_id);
}

int cPopulationInterface::GetInput()
{
  assert(fun_get_input != NULL);
  return (*fun_get_input)(population, cell_id);
}

int cPopulationInterface::GetInputAt(int & input_pointer)
{
  assert(fun_get_input_at != NULL);
  return (*fun_get_input_at)(population, cell_id, input_pointer);
}

int cPopulationInterface::Debug()
{
  assert(fun_debug != NULL);
  return (*fun_debug)(population, cell_id);
}

const tArray<double> & cPopulationInterface::GetResources()
{
  assert(fun_get_resources != NULL);
  return (*fun_get_resources)(population, cell_id);
}

void cPopulationInterface::UpdateResources(const tArray<double> & res_change)
{
  assert(fun_update_resources != NULL);
  (*fun_update_resources)(population, cell_id, res_change);
}

void cPopulationInterface::Die()
{
  if (fun_kill_cell == NULL) return;
  (*fun_kill_cell)(population, cell_id);
}

void cPopulationInterface::Kaboom()
{
	if (fun_kill_surround_cell == NULL) return;
	(*fun_kill_surround_cell)(population, cell_id);
}

bool cPopulationInterface::SendMessage(cOrgMessage & mess)
{
  if (fun_send_message == NULL) return false;
  return (*fun_send_message)(population, cell_id, mess);
}

int cPopulationInterface::ReceiveValue()
{
  assert(fun_receive_value != NULL);
  return (*fun_receive_value)(population, cell_id);
}

bool cPopulationInterface::InjectParasite(cOrganism * parent, 
					  const cGenome & injected_code)
{
  if (fun_inject_parasite == NULL) return false;
  return (*fun_inject_parasite)(population, cell_id, parent, injected_code);
}

bool cPopulationInterface::UpdateMerit(double new_merit)
{
  if (fun_update_merit == NULL) return false;
  return (*fun_update_merit)(population, cell_id, new_merit);
}
