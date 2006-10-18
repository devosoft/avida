/*
 *  cPopulationInterface.cc
 *  Avida
 *
 *  Called "pop_interface.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cPopulationInterface.h"

#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cOrgMessage.h"
#include "cOrgSinkMessage.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cTestCPU.h"

#include <assert.h>

#ifndef NULL
#define NULL 0
#endif


bool cPopulationInterface::Divide(cAvidaContext& ctx, cOrganism* parent, cGenome& child_genome)
{
  assert(parent != NULL);
  assert(m_world->GetPopulation().GetCell(m_cell_id).GetOrganism() == parent);
  return m_world->GetPopulation().ActivateOffspring(ctx, child_genome, *parent);
}

cOrganism * cPopulationInterface::GetNeighbor()
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  
  return cell.ConnectionList().GetFirst()->GetOrganism();
}

int cPopulationInterface::GetNumNeighbors()
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  
  return cell.ConnectionList().GetSize();
}

void cPopulationInterface::Rotate(int direction)
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());

  if (direction >= 0) cell.ConnectionList().CircNext();
  else cell.ConnectionList().CircPrev();
}

int cPopulationInterface::GetInput()
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  return cell.GetInput();
}

int cPopulationInterface::GetInputAt(int& input_pointer)
{
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  return cell.GetInputAt(input_pointer);
}

int cPopulationInterface::Debug()
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  return cell.GetOrganism()->GetGenotype()->GetID();
}

const tArray<double> & cPopulationInterface::GetResources()
{
  return m_world->GetPopulation().GetCellResources(m_cell_id);
}

void cPopulationInterface::UpdateResources(const tArray<double> & res_change)
{
  return m_world->GetPopulation().UpdateCellResources(res_change, m_cell_id);
}

void cPopulationInterface::Die()
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  m_world->GetPopulation().KillOrganism(cell);
}

void cPopulationInterface::Kaboom(int distance)
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  m_world->GetPopulation().Kaboom(cell, distance);
}

void cPopulationInterface::SpawnDeme()
{
  const int num_demes = m_world->GetPopulation().GetNumDemes();

  // Spawn the current deme; no target ID will put it into a random deme.
  const int deme_id = m_world->GetPopulation().GetCell(m_cell_id).GetDemeID();

  m_world->GetPopulation().SpawnDeme(deme_id);
}

bool cPopulationInterface::SendMessage(cOrgMessage & mess)
{
  mess.SetSenderID(m_cell_id);
  mess.SetTime(m_world->GetStats().GetUpdate());
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  if(cell.ConnectionList().GetFirst() == NULL)
    return false;
  mess.SetRecipientID(cell.ConnectionList().GetFirst()->GetID());
  return cell.ConnectionList().GetFirst()->GetOrganism()->ReceiveMessage(mess);
}

cOrgSinkMessage* cPopulationInterface::NetReceive()
{
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  
  switch(m_world->GetConfig().NET_STYLE.Get())
  {
  case 1: // Receiver Facing
    {
      cOrganism* cur_neighbor = cell.ConnectionList().GetFirst()->GetOrganism();
      cOrgSinkMessage* msg = NULL;
      if (cur_neighbor != NULL && (msg = cur_neighbor->NetPop()) != NULL) return msg;
    }
    break;
    
  case 0: // Random Next - First Available
  default:
    {
      const int num_neighbors = cell.ConnectionList().GetSize();
      for (int i = 0; i < num_neighbors; i++) {
        cell.ConnectionList().CircNext();
        
        cOrganism* cur_neighbor = cell.ConnectionList().GetFirst()->GetOrganism();
        cOrgSinkMessage* msg = NULL;
        if (cur_neighbor != NULL && (msg = cur_neighbor->NetPop()) != NULL ) return msg;
      }
    }
    break;
  }
  
  return NULL;
}

bool cPopulationInterface::NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg)
{
  cOrganism* org = m_world->GetPopulation().GetCell(msg->GetSourceID()).GetOrganism();
  return (org != NULL && org->NetRemoteValidate(ctx, msg->GetOriginalValue()));
}

int cPopulationInterface::ReceiveValue()
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  
  const int num_neighbors = cell.ConnectionList().GetSize();
  for (int i = 0; i < num_neighbors; i++) {
    cell.ConnectionList().CircNext();
    
    cOrganism* cur_neighbor = cell.ConnectionList().GetFirst()->GetOrganism();
    if (cur_neighbor == NULL || cur_neighbor->GetSentActive() == false) {
      continue;
    }
    
    return cur_neighbor->RetrieveSentValue();
  }
  
  return 0;
}

void cPopulationInterface::SellValue(const int data, const int label, const int sell_price, const int org_id)
{
	m_world->GetPopulation().AddSellValue(data, label, sell_price, org_id, m_cell_id);
}

int cPopulationInterface::BuyValue(const int label, const int buy_price)
{
	int value = m_world->GetPopulation().BuyValue(label, buy_price, m_cell_id);
	return value;
}

bool cPopulationInterface::InjectParasite(cOrganism* parent, const cGenome& injected_code)
{
  assert(parent != NULL);
  assert(m_world->GetPopulation().GetCell(m_cell_id).GetOrganism() == parent);
  
  return m_world->GetPopulation().ActivateParasite(*parent, injected_code);
}

bool cPopulationInterface::UpdateMerit(double new_merit)
{
  return m_world->GetPopulation().UpdateMerit(m_cell_id, new_merit);
}

bool cPopulationInterface::TestOnDivide()
{
  return m_world->GetTestOnDivide();
}
