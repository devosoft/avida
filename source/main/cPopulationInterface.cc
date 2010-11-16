/*
 *  cPopulationInterface.cc
 *  Avida
 *
 *  Called "pop_interface.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
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

#include "cPopulationInterface.h"

#include "cDeme.h"
#include "cEnvironment.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cOrgSinkMessage.h"
#include "cOrgMessage.h"
#include "cPopulation.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cRandom.h"
#include "cInstSet.h"
#include "Platform.h"

#include <cassert>
#include <algorithm>
#include <iterator>

#ifndef NULL
#define NULL 0
#endif

#if AVIDA_PLATFORM(WINDOWS)
namespace std
{
  inline __int64  abs(__int64 i) { return _abs64(i); }
}
#endif


cPopulationInterface::cPopulationInterface(cWorld* world) 
: m_world(world)
, m_cell_id(-1)
, m_deme_id(-1)
, m_prevseen_cell_id(-1)
, m_prev_task_cell(-1)
, m_num_task_cells(0)
, m_hgt_support(NULL)
{
}

cPopulationInterface::~cPopulationInterface() {
	if(m_hgt_support) {
		delete m_hgt_support;
	}
}

cOrganism* cPopulationInterface::GetOrganism() {
	return GetCell()->GetOrganism();
}

cPopulationCell* cPopulationInterface::GetCell() { 
	return &m_world->GetPopulation().GetCell(m_cell_id);
}

cPopulationCell* cPopulationInterface::GetCellFaced() {
	return &GetCell()->GetCellFaced();
}

cDeme* cPopulationInterface::GetDeme() {
  return &m_world->GetPopulation().GetDeme(m_deme_id);
}

int cPopulationInterface::GetCellData() {
  return m_world->GetPopulation().GetCell(m_cell_id).GetCellData();
}

int cPopulationInterface::GetCellDataOrgID() {
  return m_world->GetPopulation().GetCell(m_cell_id).GetCellDataOrgID();
}

int cPopulationInterface::GetCellDataUpdate() {
  return m_world->GetPopulation().GetCell(m_cell_id).GetCellDataUpdate();
}

int cPopulationInterface::GetFacedCellData() {
  return m_world->GetPopulation().GetCell(m_cell_id).GetCellFaced().GetCellData();
}

int cPopulationInterface::GetFacedCellDataOrgID() {
  return m_world->GetPopulation().GetCell(m_cell_id).GetCellFaced().GetCellDataOrgID();
}

int cPopulationInterface::GetFacedCellDataUpdate() {
  return m_world->GetPopulation().GetCell(m_cell_id).GetCellFaced().GetCellDataUpdate();
}

void cPopulationInterface::SetCellData(const int newData) {
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  cell.SetCellData(cell.GetOrganism()->GetID(), newData);
}

bool cPopulationInterface::Divide(cAvidaContext& ctx, cOrganism* parent, const cGenome& offspring_genome)
{
  assert(parent != NULL);
  assert(m_world->GetPopulation().GetCell(m_cell_id).GetOrganism() == parent);
  return m_world->GetPopulation().ActivateOffspring(ctx, offspring_genome, parent);
}

cOrganism* cPopulationInterface::GetNeighbor()
{
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  
  return cell.ConnectionList().GetFirst()->GetOrganism();
}

bool cPopulationInterface::IsNeighborCellOccupied() {
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  return cell.ConnectionList().GetFirst()->IsOccupied();
}

int cPopulationInterface::GetNumNeighbors()
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  
  return cell.ConnectionList().GetSize();
}

void cPopulationInterface::GetNeighborhoodCellIDs(tArray<int>& list)
{
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  
  list.Resize(cell.ConnectionList().GetSize());
  tConstListIterator<cPopulationCell> it(cell.ConnectionList());
  int i = 0;
  while (it.Next() != NULL) list[i++] = it.Get()->GetID();
}

int cPopulationInterface::GetFacing()
{
	cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
	assert(cell.IsOccupied());
	return cell.GetFacing();
}

int cPopulationInterface::GetFacedCellID()
{
	cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id).GetCellFaced();
	return cell.GetID();
}

int cPopulationInterface::GetNeighborCellContents() {
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  return cell.ConnectionList().GetFirst()->GetCellData();
}

void cPopulationInterface::Rotate(int direction)
{
  cPopulationCell & cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
	
  if (direction >= 0) cell.ConnectionList().CircNext();
  else cell.ConnectionList().CircPrev();
}

int cPopulationInterface::GetInputAt(int& input_pointer)
{
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
  return cell.GetInputAt(input_pointer);
}

void cPopulationInterface::ResetInputs(cAvidaContext& ctx) 
{ 
  m_world->GetPopulation().GetCell(m_cell_id).ResetInputs(ctx); 
}

const tArray<int>& cPopulationInterface::GetInputs() const
{
  return m_world->GetPopulation().GetCell(m_cell_id).GetInputs();
}

const tArray<double> & cPopulationInterface::GetResources()
{
  return m_world->GetPopulation().GetCellResources(m_cell_id);
}

const tArray<double> & cPopulationInterface::GetDemeResources(int deme_id)
{
  return m_world->GetPopulation().GetDemeCellResources(deme_id, m_cell_id);
}

const tArray< tArray<int> >& cPopulationInterface::GetCellIdLists()
{
	return m_world->GetPopulation().GetCellIdLists();
}

void cPopulationInterface::UpdateResources(const tArray<double> & res_change)
{
  return m_world->GetPopulation().UpdateCellResources(res_change, m_cell_id);
}

void cPopulationInterface::UpdateDemeResources(const tArray<double> & res_change)
{
  return m_world->GetPopulation().UpdateDemeCellResources(res_change, m_cell_id);
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
  // const int num_demes = m_world->GetPopulation().GetNumDemes();
	
  // Spawn the current deme; no target ID will put it into a random deme.
  const int deme_id = m_world->GetPopulation().GetCell(m_cell_id).GetDemeID();
	
  m_world->GetPopulation().SpawnDeme(deme_id);
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

bool cPopulationInterface::InjectParasite(cOrganism* host, cBioUnit* parent, const cString& label, const cSequence& injected_code)
{
  assert(parent != NULL);
  assert(m_world->GetPopulation().GetCell(m_cell_id).GetOrganism() == host);
  
  return m_world->GetPopulation().ActivateParasite(host, parent, label, injected_code);
}

bool cPopulationInterface::UpdateMerit(double new_merit)
{
  return m_world->GetPopulation().UpdateMerit(m_cell_id, new_merit);
}

bool cPopulationInterface::TestOnDivide()
{
  return m_world->GetTestOnDivide();
}


/*! Internal-use method to consolidate message-sending code.
 */
bool cPopulationInterface::SendMessage(cOrgMessage& msg, cPopulationCell& rcell) {
	
	bool dropped=false;
	bool lost=false;
	
	static const double drop_prob = m_world->GetConfig().NET_DROP_PROB.Get();
  if((drop_prob > 0.0) && m_world->GetRandom().P(drop_prob)) {
		// message dropped
		GetDeme()->messageDropped();
		GetDeme()->messageSendFailed();
		dropped = true;
	}
	
	// Fail if the cell we're facing is not occupied.
  if(!rcell.IsOccupied()) {
		GetDeme()->messageSendFailed();
		lost = true;
	}
	
	// record this message, regardless of whether it's actually received.
	if(m_world->GetConfig().NET_LOG_MESSAGES.Get()) {
		m_world->GetStats().LogMessage(msg, dropped, lost);
	}
	
	if(dropped || lost) {
		return false;
	}
	
	cOrganism* recvr = rcell.GetOrganism();
  assert(recvr != 0);
  recvr->ReceiveMessage(msg);
	m_world->GetStats().SentMessage(msg);
	GetDeme()->IncMessageSent();
	GetDeme()->MessageSuccessfullySent(); // No idea what the difference is here...
  return true;
}

bool cPopulationInterface::SendMessage(cOrgMessage& msg, int cellid) {
  cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
	return SendMessage(msg, cell);	
}


/*! Send a message to the faced organism, failing if this cell does not have 
 neighbors or if the cell currently faced is not occupied.
 */
bool cPopulationInterface::SendMessage(cOrgMessage& msg) {
	cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
	assert(cell.IsOccupied()); // This organism; sanity.
	
  cPopulationCell* rcell = cell.ConnectionList().GetFirst();
  assert(rcell != 0); // Cells should never be null.	
	return SendMessage(msg, *rcell);
}


/*! Send a message to the faced organism, failing if this cell does not have 
 neighbors or if the cell currently faced is not occupied. */
bool cPopulationInterface::BroadcastMessage(cOrgMessage& msg, int depth) {
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied()); // This organism; sanity.
	
	// Get the set of cells that are within range.
	std::set<cPopulationCell*> cell_set;
	cell.GetNeighboringCells(cell_set, depth);
	
	// Remove this cell from the set!
	cell_set.erase(&cell);
	
	// Now, send a message towards each cell:
	for(std::set<cPopulationCell*>::iterator i=cell_set.begin(); i!=cell_set.end(); ++i) {
		SendMessage(msg, **i);
	}
	return true;
}


bool cPopulationInterface::BcastAlarm(int jump_label, int bcast_range) {
  bool successfully_sent(false);
  cPopulationCell& scell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(scell.IsOccupied()); // This organism; sanity.
	
  const int ALARM_SELF = m_world->GetConfig().ALARM_SELF.Get(); // does an alarm affect the sender; 0=no  non-0=yes
  
  if(bcast_range > 1) { // multi-hop messaging
    cDeme& deme = m_world->GetPopulation().GetDeme(GetDemeID());
    for(int i = 0; i < deme.GetSize(); i++) {
      int possible_receiver_id = deme.GetCellID(i);
      cPopulationCell& rcell = m_world->GetPopulation().GetCell(possible_receiver_id);
			
      if(rcell.IsOccupied() && possible_receiver_id != GetCellID()) {
        //check distance
        pair<int, int> sender_pos = deme.GetCellPosition(GetCellID());
        pair<int, int> possible_receiver_pos = deme.GetCellPosition(possible_receiver_id);
        int hop_distance = max( abs(sender_pos.first  - possible_receiver_pos.first),
															 abs(sender_pos.second - possible_receiver_pos.second));
        if(hop_distance <= bcast_range) {
          // send alarm to organisms
          cOrganism* recvr = rcell.GetOrganism();
          assert(recvr != NULL);
          recvr->moveIPtoAlarmLabel(jump_label);
          successfully_sent = true;
        }
      }
    }
  } else { // single hop messaging
    for(int i = 0; i < scell.ConnectionList().GetSize(); i++) {
      cPopulationCell* rcell = scell.ConnectionList().GetPos(i);
      assert(rcell != NULL); // Cells should never be null.
			
      // Fail if the cell we're facing is not occupied.
      if(!rcell->IsOccupied())
        continue;
      cOrganism* recvr = rcell->GetOrganism();
      assert(recvr != NULL);
      recvr->moveIPtoAlarmLabel(jump_label);
      successfully_sent = true;
    }
  }
  
  if(ALARM_SELF) {
    scell.GetOrganism()->moveIPtoAlarmLabel(jump_label);
  }
  return successfully_sent;
}

void cPopulationInterface::DivideOrgTestamentAmongDeme(double value){
  cDeme* deme = GetDeme();
  for(int i = 0; i < deme->GetSize(); i++) {
    cPopulationCell& cell = deme->GetCell(i);
    if(cell.IsOccupied()) {
      cOrganism* org = cell.GetOrganism();
      org->GetPhenotype().EnergyTestament(value/deme->GetOrgCount());
    }
  }
}

/*! Send a flash to all neighboring organisms. */
void cPopulationInterface::SendFlash() {
  cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
  assert(cell.IsOccupied());
	
  for(int i=0; i<cell.ConnectionList().GetSize(); ++i) {
    cPopulationCell* neighbor = cell.ConnectionList().GetFirst();
    if(neighbor->IsOccupied()) {
      neighbor->GetOrganism()->ReceiveFlash();
    }
    cell.ConnectionList().CircNext();
  }
}

int cPopulationInterface::GetStateGridID(cAvidaContext& ctx)
{
  return ctx.GetRandom().GetUInt(m_world->GetEnvironment().GetNumStateGrids());
}

/* Rotate an organism to face the neighbor with the highest reputation */
void cPopulationInterface::RotateToGreatestReputation() 
{
	
	cPopulationCell& cell = m_world->GetPopulation().GetCell(GetCellID());
	int high_rep=-1; 
	vector <int> high_rep_orgs;
	
	
	// loop to find the max reputation
	for(int i=0; i<cell.ConnectionList().GetSize(); ++i) {
		const cPopulationCell* faced_cell = cell.ConnectionList().GetFirst();
		// cell->organism, if occupied, check reputation, etc.
		if (IsNeighborCellOccupied()) {
			cOrganism* cur_neighbor = faced_cell->GetOrganism();
			
			// if it has high reputation	
			if (cur_neighbor->GetReputation() >= high_rep) {
				if (cur_neighbor->GetReputation() > high_rep) {
					high_rep = cur_neighbor->GetReputation();
					high_rep_orgs.clear();
				}
				high_rep_orgs.push_back(cur_neighbor->GetID()); 
			} 
		}
		
		// check the next neighbor
		cell.ConnectionList().CircNext();
	}
	
	// Pick an organism to donate to
	
	if (high_rep_orgs.size() > 0) {
		unsigned int rand_num = m_world->GetRandom().GetUInt(0, high_rep_orgs.size()); 
		int high_org_id = high_rep_orgs[rand_num];
		
		for(int i=0; i<cell.ConnectionList().GetSize(); ++i) {
			const cPopulationCell* faced_cell = cell.ConnectionList().GetFirst();
			
			if (IsNeighborCellOccupied()) {
				
				cOrganism* cur_neighbor = faced_cell->GetOrganism();
				
				// if it has high reputation	
				if (cur_neighbor->GetID() == high_org_id) {
					break;
				}
			}
			
			cell.ConnectionList().CircNext();
			
		}
	}
	
}

/* Rotate an organism to face the neighbor with the highest reputation 
 where the neighbor has a different tag than the organism*/
void cPopulationInterface::RotateToGreatestReputationWithDifferentTag(int tag) 
{
	
	cPopulationCell& cell = m_world->GetPopulation().GetCell(GetCellID());
	int high_rep=-1; 
	vector <int> high_rep_orgs;
	
	// loop to find the max reputation
	for(int i=0; i<cell.ConnectionList().GetSize(); ++i) {
		const cPopulationCell* faced_cell = cell.ConnectionList().GetFirst();
		// cell->organism, if occupied, check reputation, etc.
		if (IsNeighborCellOccupied()) {
			cOrganism* cur_neighbor = faced_cell->GetOrganism();
			
			// if it has high reputation	
			if ((cur_neighbor->GetTagLabel() != tag) && (cur_neighbor->GetReputation() >= high_rep)) {
				if (cur_neighbor->GetReputation() > high_rep) {
					high_rep = cur_neighbor->GetReputation();
					high_rep_orgs.clear();
				}
				high_rep_orgs.push_back(cur_neighbor->GetID()); 
			} 
		}
		
		// check the next neighbor
		cell.ConnectionList().CircNext();
	}
	
	// Pick an organism to donate to
	
	if (high_rep_orgs.size() > 0) {
		unsigned int rand_num = m_world->GetRandom().GetUInt(0, high_rep_orgs.size()); 
		int high_org_id = high_rep_orgs[rand_num];
		
		for(int i=0; i<cell.ConnectionList().GetSize(); ++i) {
			const cPopulationCell* faced_cell = cell.ConnectionList().GetFirst();
			
			if (IsNeighborCellOccupied()) {
				
				cOrganism* cur_neighbor = faced_cell->GetOrganism();
				
				// if it has high reputation	
				if (cur_neighbor->GetID() == high_org_id) {
					break;
				}
			}
			
			cell.ConnectionList().CircNext();
			
		}
		
		
		
	}
	
}

/* Rotate an organism to face the neighbor with the highest reputation 
 where the neighbor has a different tag than the organism*/
void cPopulationInterface::RotateToGreatestReputationWithDifferentLineage(int line) 
{
	
	cPopulationCell& cell = m_world->GetPopulation().GetCell(GetCellID());
	int high_rep=-1; 
	vector <int> high_rep_orgs;
	
	// loop to find the max reputation
	for(int i=0; i<cell.ConnectionList().GetSize(); ++i) {
		const cPopulationCell* faced_cell = cell.ConnectionList().GetFirst();
		// cell->organism, if occupied, check reputation, etc.
		if (IsNeighborCellOccupied()) {
			cOrganism* cur_neighbor = faced_cell->GetOrganism();
			
			// if it has high reputation	
			if ((cur_neighbor->GetLineageLabel() != line) && (cur_neighbor->GetReputation() >= high_rep)) {
				if (cur_neighbor->GetReputation() > high_rep) {
					high_rep = cur_neighbor->GetReputation();
					high_rep_orgs.clear();
				}
				high_rep_orgs.push_back(cur_neighbor->GetID()); 
			} 
		}
		
		// check the next neighbor
		cell.ConnectionList().CircNext();
	}
	
	// Pick an organism to donate to
	
	if (high_rep_orgs.size() > 0) {
		unsigned int rand_num = m_world->GetRandom().GetUInt(0, high_rep_orgs.size()); 
		int high_org_id = high_rep_orgs[rand_num];
		
		for(int i=0; i<cell.ConnectionList().GetSize(); ++i) {
			const cPopulationCell* faced_cell = cell.ConnectionList().GetFirst();
			
			if (IsNeighborCellOccupied()) {
				
				cOrganism* cur_neighbor = faced_cell->GetOrganism();
				
				// if it has high reputation	
				if (cur_neighbor->GetID() == high_org_id) {
					break;
				}
			}
			
			cell.ConnectionList().CircNext();
			
		}
	}	
}

/*! Link this organism's cell to the cell it is currently facing.
 */
void cPopulationInterface::CreateLinkByFacing(double weight) {
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	cPopulationCell* that_cell = GetCellFaced(); assert(that_cell);
	deme->GetNetwork().Connect(*this_cell, *that_cell, weight);
}

/*! Link this organism's cell to the cell with coordinates (x,y).
 */
void cPopulationInterface::CreateLinkByXY(int x, int y, double weight) {
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	// the static casts here are to fix a problem with -2^31 being sent in as a 
	// cell coordinate.  the problem is that a 2s-complement int can hold a negative
	// number whose absolute value is too large for the int to hold.  when this happens,
	// abs returns the value unmodified.
	int cellx = std::abs(static_cast<long long int>(x)) % deme->GetWidth();
	int celly = std::abs(static_cast<long long int>(y)) % deme->GetHeight();
	assert(cellx >= 0);
	assert(cellx < deme->GetWidth());
	assert(celly >= 0);
	assert(celly < deme->GetHeight());
	deme->GetNetwork().Connect(*this_cell, deme->GetCell(cellx, celly), weight);
}

/*! Link this organism's cell to the cell with index idx.
 */
void cPopulationInterface::CreateLinkByIndex(int idx, double weight) {
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	// the static casts here are to fix a problem with -2^31 being sent in as a 
	// cell coordinate.  the problem is that a 2s-complement int can hold a negative
	// number whose absolute value is too large for the int to hold.  when this happens,
	// abs returns the value unmodified.
	int that_cell = std::abs(static_cast<long long int>(idx)) % deme->GetSize();
	assert(that_cell >= 0);
	assert(that_cell < deme->GetSize());
	deme->GetNetwork().Connect(*this_cell, deme->GetCell(that_cell), weight);
}

/*! Broadcast a message to all organisms that are connected by this network.
 */
bool cPopulationInterface::NetworkBroadcast(cOrgMessage& msg) {	
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	deme->GetNetwork().BroadcastToNeighbors(*this_cell, msg, this);
	return true;
}

/*! Unicast a message to the current selected organism.
 */
bool cPopulationInterface::NetworkUnicast(cOrgMessage& msg) {
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	deme->GetNetwork().Unicast(*this_cell, msg, this);
	return true;
}

/*! Rotate to select a new network link.
 */
bool cPopulationInterface::NetworkRotate(int x) {
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	deme->GetNetwork().Rotate(*this_cell, x);
	return true;
}

/*! Select a new network link.
 */
bool cPopulationInterface::NetworkSelect(int x) {
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	deme->GetNetwork().Select(*this_cell, x);
	return true;
}

/*! Called when this individual is the donor organism during conjugation.
 
 This method causes this individual to "donate" a fragment of its own genome to
 another organism selected from the population.
 */
void cPopulationInterface::DoHGTDonation(cAvidaContext& ctx) {
	cPopulationCell* target=0;
	
	switch(m_world->GetConfig().HGT_CONJUGATION_METHOD.Get()) {
		case 0: { // selected at random from neighborhood
			std::set<cPopulationCell*> occupied_cell_set;
			GetCell()->GetOccupiedNeighboringCells(occupied_cell_set, 1);
			if(occupied_cell_set.size()==0) {
				// nothing to do here, there are no neighbors
				return;
			}
			std::set<cPopulationCell*>::iterator selected=occupied_cell_set.begin();
			std::advance(selected, ctx.GetRandom().GetInt(occupied_cell_set.size()));
			target = *selected;
			break;
		}
		case 1: { // faced individual
			target = GetCellFaced();
			if(!target->IsOccupied()) {
				// nothing to do, we're facing an empty cell.
				return;
			}
			break;
		}
		default: {
			m_world->GetDriver().RaiseFatalException(1, "HGT_CONJUGATION_METHOD is set to an invalid value.");
			break;
		}
	}
	assert(target != 0);
	fragment_list_type fragments;
	cGenomeUtil::RandomSplit(ctx, 
													 m_world->GetConfig().HGT_FRAGMENT_SIZE_MEAN.Get(),
													 m_world->GetConfig().HGT_FRAGMENT_SIZE_VARIANCE.Get(),
													 GetOrganism()->GetGenome().GetSequence(),
													 fragments);
	target->GetOrganism()->GetOrgInterface().ReceiveHGTDonation(fragments[ctx.GetRandom().GetInt(fragments.size())]);
}


/*! Called when this organism "requests" an HGT conjugation.
 
 Technically, organisms don't request an HGT conjugation.  However, this provides
 an alternative to population-level conjugational events.  Specifically, whenever
 an organism replicates, there is the possibility that its offspring conjugates
 with another organism in the population -- that is what we check here.
 
 This method is closely related to HGT donation, except here we're looking for
 the donatOR, instead of the donatEE.
 */
void cPopulationInterface::DoHGTConjugation(cAvidaContext& ctx) {
	cPopulationCell* source=0;
	
	switch(m_world->GetConfig().HGT_CONJUGATION_METHOD.Get()) {
		case 0: { // selected at random from neighborhood
			std::set<cPopulationCell*> occupied_cell_set;
			GetCell()->GetOccupiedNeighboringCells(occupied_cell_set, 1);
			if(occupied_cell_set.size()==0) {
				// nothing to do here, there are no neighbors
				return;
			}
			std::set<cPopulationCell*>::iterator selected=occupied_cell_set.begin();
			std::advance(selected, ctx.GetRandom().GetInt(occupied_cell_set.size()));
			source = *selected;
			break;
		}
		case 1: { // faced individual
			source = GetCellFaced();
			if(!source->IsOccupied()) {
				// nothing to do, we're facing an empty cell.
				return;
			}
			break;
		}
		default: {
			m_world->GetDriver().RaiseFatalException(1, "HGT_CONJUGATION_METHOD is set to an invalid value.");
			break;
		}
	}
	assert(source != 0);
	fragment_list_type fragments;
	cGenomeUtil::RandomSplit(ctx, 
													 m_world->GetConfig().HGT_FRAGMENT_SIZE_MEAN.Get(),
													 m_world->GetConfig().HGT_FRAGMENT_SIZE_VARIANCE.Get(),
													 source->GetOrganism()->GetGenome().GetSequence(),
													 fragments);
	ReceiveHGTDonation(fragments[ctx.GetRandom().GetInt(fragments.size())]);	
}


/*! Perform an HGT mutation on this offspring. 
 
 HGT mutations are location-dependent, hence they are piped through the populatin
 interface as opposed to being implemented in the CPU or organism.

 There is the possibility that more than one HGT mutation occurs when this method 
 is called.
 */
void cPopulationInterface::DoHGTMutation(cAvidaContext& ctx, cGenome& offspring) {
	InitHGTSupport();
	
	// first, gather up all the fragments that we're going to be inserting into this offspring:
	// these come from a per-replication conjugational event:
	if((m_world->GetConfig().HGT_CONJUGATION_P.Get() > 0.0)
		 && (ctx.GetRandom().P(m_world->GetConfig().HGT_CONJUGATION_P.Get()))) {
		DoHGTConjugation(ctx);
	}	
	
	// the pending list includes both the fragments selected via the above process,
	// as well as from population-level conjugational events (see cPopulationActions.cc:cActionAvidianConjugation).
	fragment_list_type& fragments = m_hgt_support->_pending;

	// these come from "natural" competence (ie, eating the dead):
	if((m_world->GetConfig().HGT_COMPETENCE_P.Get() > 0.0)
		 && (ctx.GetRandom().P(m_world->GetConfig().HGT_COMPETENCE_P.Get()))) {
		
		// get this organism's cell:
		cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
		
		// the hgt source controls where the genetic material for HGT comes from.
		switch(m_world->GetConfig().HGT_SOURCE.Get()) {
			case 0: { // source is other genomes, nothing to do here (default)
				break;
			}
			case 1: { // source is the parent (a control)
				// this is a little hackish, but this is the cleanest way to make sure
				// that all downstream stuff works right.
				cell.ClearFragments();
				cell.AddGenomeFragments(cell.GetOrganism()->GetGenome().GetSequence());
				break;
			}
			default: { // error
				m_world->GetDriver().RaiseFatalException(1, "HGT_SOURCE is set to an invalid value.");
				break;
			}
		}
		
		// do we have any fragments available?
		if(cell.CountGenomeFragments() > 0) {
			// add a randomly-selected fragment to the list of fragments to be HGT'd,
			// remove it from the cell, and adjust the level of HGT resource.
			fragment_list_type::iterator selected=cell.GetFragments().begin();
			std::advance(selected, ctx.GetRandom().GetInt(cell.GetFragments().size()));
			fragments.insert(fragments.end(), *selected);			
			m_world->GetPopulation().AdjustHGTResource(-selected->GetSize());
			cell.GetFragments().erase(selected);
		}
	}
	
	// now, for each fragment being HGT'd, figure out where to put it:
	for(fragment_list_type::iterator i=fragments.begin(); i!=fragments.end(); ++i) {
		cGenomeUtil::substring_match location;
		switch(m_world->GetConfig().HGT_FRAGMENT_SELECTION.Get()) {
			case 0: { // random selection
				HGTMatchPlacement(ctx, offspring.GetSequence(), i, location);
				break;
			}
			case 1: { // random selection with redundant instruction trimming
				HGTTrimmedPlacement(ctx, offspring.GetSequence(), i, location);
				break;
			}
			case 2: { // random selection and random placement
				HGTRandomPlacement(ctx, offspring.GetSequence(), i, location);
				break;
			}
			default: { // error
				m_world->GetDriver().RaiseFatalException(1, "HGT_FRAGMENT_SELECTION is set to an invalid value.");
				break;
			}
		}
		
		// at this stage, we have a fragment and a location we're going to put it.
		// there are various transformations to this fragment that we could perform,
		// more as controls than anything else.
		switch(m_world->GetConfig().HGT_FRAGMENT_XFORM.Get()) {
			case 0: { // no transformation.
				break;
			}
			case 1: { // random shuffle of the instructions in the fragment.
				cGenomeUtil::RandomShuffle(ctx, *i);
				break;
			}
			case 2: { // replace the instructions in the fragment with random instructions.
				const cInstSet& instset = m_world->GetHardwareManager().GetInstSet(offspring.GetInstSet());
				for(int j=0; j<i->GetSize(); ++j) {
					(*i)[j] = instset.GetRandomInst(ctx);
				}
				break;
			}
			default: { // error
				m_world->GetDriver().RaiseFatalException(1, "HGT_FRAGMENT_XFORM is set to an invalid value.");
				break;
			}				
		}
		
		// do the mutation; we currently support insertions and replacements, but this can
		// be extended in the same way as fragment selection if need be.
		if(ctx.GetRandom().P(m_world->GetConfig().HGT_INSERTION_MUT_P.Get())) {
			// insert the fragment just after the final location:
			offspring.GetSequence().Insert(location.end, *i);
		} else {
			// replacement: replace [begin,end) instructions in the genome with the fragment,
			// respecting circularity.
			offspring.GetSequence().Replace(*i, location.begin, location.end);
		}
		
		// stats tracking:
		m_world->GetStats().GenomeFragmentInserted(GetOrganism(), *i, location);
	}
	
	// clean-up; be sure to empty the pending list so that we don't end up doing an HGT
	// operation multiple times on the same fragment.
	fragments.clear();
}


/*! Place the fragment at the location of best match.
 */
void cPopulationInterface::HGTMatchPlacement(cAvidaContext& ctx, const cSequence& offspring,
																						 fragment_list_type::iterator& selected,
																						 substring_match& location) {
	// find the location within the offspring's genome that best matches the selected fragment:
	location = cGenomeUtil::FindUnbiasedCircularMatch(ctx, offspring, *selected);
}


/*! Place the fragment at the location of best match, with redundant instructions trimmed.

 In this fragment selection method, the
 match location within the genome is calculated on a "trimmed" fragment.  Specifically,
 the trimmed fragment has all duplicate instructions at its end removed prior to the match.
 
 Mutations to the offspring are still performed using the entire fragment, so this effectively
 increases the insertion rate.  E.g., hgt(abcde, abcccc) -> abccccde.
 */
void cPopulationInterface::HGTTrimmedPlacement(cAvidaContext& ctx, const cSequence& offspring,
																											 fragment_list_type::iterator& selected,
																											 substring_match& location) {
	// copy the selected fragment, trimming redundant instructions at the end:
	cSequence trimmed(*selected);
	while((trimmed.GetSize() >= 2) && (trimmed[trimmed.GetSize()-1] == trimmed[trimmed.GetSize()-2])) {
		trimmed.Remove(trimmed.GetSize()-1);
	}
	
	// find the location within the offspring's genome that best matches the selected fragment:
	location = cGenomeUtil::FindUnbiasedCircularMatch(ctx, offspring, trimmed);
}


/*! Place the fragment at a random location.
 
 Here we select a random location for the fragment within the offspring.
 The beginning of the fragment location is selected at random, while the end is selected a
 random distance (up to the length of the selected fragment * 2) instructions away.
 */
void cPopulationInterface::HGTRandomPlacement(cAvidaContext& ctx, const cSequence& offspring,
																											fragment_list_type::iterator& selected,
																											substring_match& location) {
	// select a random location within the offspring's genome for this fragment to be
	// inserted:
	location.begin = ctx.GetRandom().GetUInt(offspring.GetSize());
	location.end = location.begin + ctx.GetRandom().GetUInt(selected->GetSize()*2);
	location.size = offspring.GetSize();
	location.resize(offspring.GetSize());
}

/*! Called when this organism is the receiver of an HGT donation.
 */
void cPopulationInterface::ReceiveHGTDonation(const cSequence& fragment) {
	InitHGTSupport();
	m_hgt_support->_pending.push_back(fragment);
}


void cPopulationInterface::Move(cAvidaContext& ctx, int src_id, int dest_id)
{
  m_world->GetPopulation().MoveOrganisms(ctx, src_id, dest_id);
}


void cPopulationInterface::JoinGroup(int group_id)
{
  m_world->GetPopulation().JoinGroup(group_id);
}

void cPopulationInterface::LeaveGroup(int group_id)
{
  m_world->GetPopulation().LeaveGroup(group_id);
}


void cPopulationInterface::BeginSleep()
{
  if(m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1)
    m_world->GetPopulation().AddBeginSleep(m_cell_id, m_world->GetStats().GetUpdate());
  GetDeme()->IncSleepingCount();
}

void cPopulationInterface::EndSleep()
{
  if(m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1)
    m_world->GetPopulation().AddEndSleep(m_cell_id, m_world->GetStats().GetUpdate());
  GetDeme()->DecSleepingCount();
}
