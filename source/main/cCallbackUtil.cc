//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CALLBACK_UTIL_HH
#include "cCallbackUtil.h"
#endif

#ifndef AVIDA_HH
#include "avida.hh"
#endif
#ifndef AVIDA_DRIVER_BASE_HH
#include "cAvidaDriver_Base.h"
#endif
#ifndef BIRTH_CHAMBER_HH
#include "cBirthChamber.h"
#endif
#ifndef CONFIG_HH
#include "cConfig.h"
#endif
#ifndef ENVIRONMENT_HH
#include "cEnvironment.h"
#endif
#ifndef GENOTYPE_HH
#include "cGenotype.h"
#endif
#ifndef HARDWARE_BASE_HH
#include "cHardwareBase.h"
#endif
#ifndef HARDWARE_FACTORY_HH
#include "cHardwareFactory.h"
#endif
#ifndef ORG_MESSAGE_HH
#include "cOrgMessage.h"
#endif
#ifndef ORGANISM_HH
#include "cOrganism.h"
#endif
#ifndef POPULATION_HH
#include "cPopulation.h"
#endif
#ifndef POPULATION_CELL_HH
#include "cPopulationCell.h"
#endif
#ifndef POPULATION_INTERFACE_HH
#include "cPopulationInterface.h"
#endif
#ifndef TEST_CPU_HH
#include "cTestCPU.h"
#endif


using namespace std;


cHardwareBase * cCallbackUtil::CB_NewHardware(cPopulation * pop,
					      cOrganism * owner)
{
  cInstSet * inst_set;

  // Determine which inst_set we should be using...
  // If we're in a population, setup the inst_set properly.
  if (pop != NULL) inst_set = &(pop->GetEnvironment().GetInstSet());

  // Otherwise, use the test inst lib...
  else inst_set = cTestCPU::GetInstSet();

  // For the moment, we only have one hardware type...
  assert(inst_set != NULL && inst_set->OK());
  cHardwareBase * new_hardware =
    cHardwareFactory::Create(owner, inst_set, cConfig::GetHardwareType());

  return new_hardware;
}


void cCallbackUtil::CB_RecycleHardware(cHardwareBase * out_hardware)
{
  cHardwareFactory::Recycle(out_hardware);
}


// Callback for a divide inside of a population.  Return parent alive.
bool cCallbackUtil::CB_Divide(cPopulation * pop, int cell_id,
			      cOrganism * parent, cGenome & child_genome)
{
  assert(pop != NULL);
  assert(parent != NULL);
  assert(pop->GetCell(cell_id).GetOrganism() == parent);

  return pop->ActivateOffspring(child_genome, *parent);
}


// Callback for a divide outside of a population
bool cCallbackUtil::CB_TestDivide(cPopulation * pop, int cell_id,
				  cOrganism * parent, cGenome & child_genome)
{
  assert(pop == NULL);  // Must not be in a population...

  // Reset the parent from the divide
  parent->GetPhenotype().TestDivideReset(parent->GetGenome().GetSize());

  // @CAO in the future, we probably want to pass this child the test_cpu!

  return true;
}

// Callback to find out if we need to run a test CPU on all divides...
bool cCallbackUtil::CB_TestOnDivide(cPopulation * pop, int cell_id)
{
  assert(pop != NULL); // Must be in a population...
  return cConfig::GetTestOnDivide();
}

cOrganism * cCallbackUtil::CB_GetNeighbor(cPopulation * pop, int cell_id)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  return cell.ConnectionList().GetFirst()->GetOrganism();
}


int cCallbackUtil::CB_GetNumNeighbors(cPopulation * pop, int cell_id)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  return cell.ConnectionList().GetSize();
}


void cCallbackUtil::CB_Rotate(cPopulation * pop, int cell_id,
				    int direction)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  if (direction >= 0) cell.ConnectionList().CircNext();
  else cell.ConnectionList().CircPrev();
}


void cCallbackUtil::CB_Breakpoint()
{
  cAvidaDriver_Base::main_driver->SignalBreakpoint();
}


double cCallbackUtil::CB_TestFitness(cPopulation * pop, int cell_id)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  return cell.GetOrganism()->GetGenotype()->GetTestFitness();
}


int cCallbackUtil::CB_GetInput(cPopulation * pop, int cell_id)
{
  if (pop == NULL) return cTestCPU::GetInput();
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());
  return cell.GetInput();
}

int cCallbackUtil::CB_GetInputAt(cPopulation * pop, int cell_id, int & input_pointer)
{
  if (pop == NULL) return cTestCPU::GetInputAt(input_pointer);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());
  return cell.GetInputAt(input_pointer);
}

int cCallbackUtil::CB_Debug(cPopulation * pop, int cell_id)
{
  if (pop == NULL) return -1;
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());
  return cell.GetOrganism()->GetGenotype()->GetID();
}


const tArray<double> &
cCallbackUtil::CB_GetResources(cPopulation * pop, int cell_id)
{
  if (pop == NULL) return cTestCPU::GetResources();

  // @CAO for the moment, ignore the possibility of local resources...
  return pop->GetCellResources(cell_id);
}


void cCallbackUtil::CB_UpdateResources(cPopulation * pop, int cell_id,
				       const tArray<double> & res_change)
{
  if (pop == NULL) return cTestCPU::UpdateResources(res_change);

  // @CAO for the moment, ignore the possibility of local resources...
  return pop->UpdateCellResources(res_change, cell_id);
}


void cCallbackUtil::CB_KillCell(cPopulation * pop, int death_id)
{
  assert(pop != NULL);
  cPopulationCell & death_cell = pop->GetCell(death_id);
  pop->KillOrganism(death_cell);
}

void cCallbackUtil::CB_KillSurroundCell(cPopulation * pop, int commie_id)
{
	assert (pop != NULL);
	cPopulationCell & death_cell = pop->GetCell(commie_id);
	pop->Kaboom(death_cell);
}

bool cCallbackUtil::CB_SendMessage(cPopulation * pop, int cell_id, cOrgMessage & mess)
{
  mess.SetSenderID(cell_id);
  mess.SetTime(pop->GetUpdate());
  cPopulationCell & cell = pop->GetCell(cell_id);
  if(cell.ConnectionList().GetFirst() == NULL)
    return false;
  mess.SetRecipientID(cell.ConnectionList().GetFirst()->GetID());
  return cell.ConnectionList().GetFirst()->GetOrganism()->ReceiveMessage(mess);
}

int cCallbackUtil::CB_ReceiveValue(cPopulation * pop, int cell_id)
{
  if (pop == NULL) return cTestCPU::GetReceiveValue();

  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  const int num_neighbors = cell.ConnectionList().GetSize();
  for (int i = 0; i < num_neighbors; i++) {
	cPopulationCell & cell = pop->GetCell(cell_id);
	cell.ConnectionList().CircNext();

    cOrganism * cur_neighbor = cell.ConnectionList().GetFirst()->GetOrganism();
    if (cur_neighbor == NULL || cur_neighbor->GetSentActive() == false) {
      continue;
    }

    return cur_neighbor->RetrieveSentValue();
  }

  return 0;

}

bool cCallbackUtil::CB_InjectParasite(cPopulation * pop, int cell_id, cOrganism * parent,
				      const cGenome & injected_code)
{
  assert(pop != NULL);
  assert(parent != NULL);
  assert(pop->GetCell(cell_id).GetOrganism() == parent);

  return pop->ActivateInject(*parent, injected_code);
}

bool cCallbackUtil::CB_UpdateMerit(cPopulation * pop, int cell_id,
				   double new_merit)
{
  assert(pop != NULL);
  return pop->UpdateMerit(cell_id, new_merit);
}
