/*
 *  cPopulationInterface.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cPopulationInterface.h"

#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cOrgMessage.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cTestCPU.h"

#include <assert.h>

#ifndef NULL
#define NULL 0
#endif


cHardwareBase* cPopulationInterface::NewHardware(cOrganism * owner)
{
  return m_world->GetHardwareManager().Create(owner);
}

bool cPopulationInterface::Divide(cOrganism * parent, cGenome & child_genome)
{
  if (InTestPop()) {
    parent->GetPhenotype().TestDivideReset(parent->GetGenome().GetSize());
    // @CAO in the future, we probably want to pass this child the test_cpu!
    return true;
  } else {
    assert(parent != NULL);
    assert(m_world->GetPopulation().GetCell(cell_id).GetOrganism() == parent);
    return m_world->GetPopulation().ActivateOffspring(child_genome, *parent);
  }
}

cOrganism * cPopulationInterface::GetNeighbor()
{
  if (InTestPop()) return NULL;

  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());
  
  return cell.ConnectionList().GetFirst()->GetOrganism();
}

int cPopulationInterface::GetNumNeighbors()
{
  if (InTestPop()) return 0;
  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());
  
  return cell.ConnectionList().GetSize();
}

void cPopulationInterface::Rotate(int direction)
{
  if (InTestPop()) return;

  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());

  if (direction >= 0) cell.ConnectionList().CircNext();
  else cell.ConnectionList().CircPrev();
}

double cPopulationInterface::TestFitness()
{
  if (InTestPop()) return -1.0;
  
  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());
  
  return cell.GetOrganism()->GetGenotype()->GetTestFitness();
}

int cPopulationInterface::GetInput()
{
  if (InTestPop()) return m_world->GetTestCPU().GetInput();
  
  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());
  return cell.GetInput();
}

int cPopulationInterface::GetInputAt(int& input_pointer)
{
  if (InTestPop()) return m_world->GetTestCPU().GetInputAt(input_pointer);
  
  cPopulationCell& cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());
  return cell.GetInputAt(input_pointer);
}

int cPopulationInterface::Debug()
{
  if (InTestPop()) return -1;
  
  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());
  return cell.GetOrganism()->GetGenotype()->GetID();
}

const tArray<double> & cPopulationInterface::GetResources()
{
  if (InTestPop()) return m_world->GetTestCPU().GetResources();  
  return m_world->GetPopulation().GetCellResources(cell_id);
}

void cPopulationInterface::UpdateResources(const tArray<double> & res_change)
{
  if (InTestPop()) return;
  return m_world->GetPopulation().UpdateCellResources(res_change, cell_id);
}

void cPopulationInterface::Die()
{
  if (InTestPop()) return;
  
  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  m_world->GetPopulation().KillOrganism(cell);
}

void cPopulationInterface::Kaboom()
{
  if (InTestPop()) return;
  
  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
	m_world->GetPopulation().Kaboom(cell);
}

bool cPopulationInterface::SendMessage(cOrgMessage & mess)
{
  if (InTestPop()) return false;
  
  mess.SetSenderID(cell_id);
  mess.SetTime(m_world->GetStats().GetUpdate());
  cPopulationCell& cell = m_world->GetPopulation().GetCell(cell_id);
  if(cell.ConnectionList().GetFirst() == NULL)
    return false;
  mess.SetRecipientID(cell.ConnectionList().GetFirst()->GetID());
  return cell.ConnectionList().GetFirst()->GetOrganism()->ReceiveMessage(mess);
}

int cPopulationInterface::ReceiveValue()
{
  if (InTestPop()) return m_world->GetTestCPU().GetReceiveValue();
  
  cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
  assert(cell.IsOccupied());
  
  const int num_neighbors = cell.ConnectionList().GetSize();
  for (int i = 0; i < num_neighbors; i++) {
    cPopulationCell & cell = m_world->GetPopulation().GetCell(cell_id);
    cell.ConnectionList().CircNext();
    
    cOrganism * cur_neighbor = cell.ConnectionList().GetFirst()->GetOrganism();
    if (cur_neighbor == NULL || cur_neighbor->GetSentActive() == false) {
      continue;
    }
    
    return cur_neighbor->RetrieveSentValue();
  }
  
  return 0;
}

bool cPopulationInterface::InjectParasite(cOrganism * parent, const cGenome & injected_code)
{
  if (InTestPop()) return false;
  
  assert(parent != NULL);
  assert(m_world->GetPopulation().GetCell(cell_id).GetOrganism() == parent);
  
  return m_world->GetPopulation().ActivateInject(*parent, injected_code);
}

bool cPopulationInterface::UpdateMerit(double new_merit)
{
  if (InTestPop()) return false;
  return m_world->GetPopulation().UpdateMerit(cell_id, new_merit);
}

