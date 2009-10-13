/*
 *  cPopulationInterface.cc
 *  Avida
 *
 *  Called "pop_interface.cc" prior to 12/5/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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
#include "cGenotype.h"
#include "cGenomeUtil.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cOrgSinkMessage.h"
#include "cOrgMessage.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cTestCPU.h"

#include <cassert>
#include <algorithm>
#include <iterator>

#ifndef NULL
#define NULL 0
#endif


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

void cPopulationInterface::SetCellData(const int newData) {
  m_world->GetPopulation().GetCell(m_cell_id).SetCellData(newData);
}

bool cPopulationInterface::Divide(cAvidaContext& ctx, cOrganism* parent, const cMetaGenome& offspring_genome)
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

bool cPopulationInterface::InjectParasite(cOrganism* parent, const cCodeLabel& label, const cGenome& injected_code)
{
  assert(parent != NULL);
  assert(m_world->GetPopulation().GetCell(m_cell_id).GetOrganism() == parent);
  
  return m_world->GetPopulation().ActivateParasite(*parent, label, injected_code);
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
	static const double drop_prob = m_world->GetConfig().NET_DROP_PROB.Get();
  if((drop_prob > 0.0) && m_world->GetRandom().P(drop_prob)) {
		// message dropped
		GetDeme()->messageDropped();
		GetDeme()->messageSendFailed();
		return false;
	}
	
  // Fail if the cell we're facing is not occupied.
  if(!rcell.IsOccupied()) {
		GetDeme()->messageSendFailed();
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
	cPopulationCell& that_cell = deme->GetCell(x % deme->GetWidth(), y % deme->GetHeight());
	deme->GetNetwork().Connect(*this_cell, that_cell, weight);
}

/*! Link this organism's cell to the cell with index idx.
 */
void cPopulationInterface::CreateLinkByIndex(int idx, double weight) {
	cDeme* deme = GetDeme(); assert(deme);
	cPopulationCell* this_cell = GetCell(); assert(this_cell);
	cPopulationCell& that_cell = deme->GetCell(idx % deme->GetSize());
	deme->GetNetwork().Connect(*this_cell, that_cell, weight);
}


/*! Perform an HGT mutation on this offspring. 
 
 HGT mutations are location-dependent, hence they are implemented here as opposed to
 the CPU or organism.
 
 If this method is called, an HGT mutation of some kind is imminent.  All that's left
 is to actually *do* the mutation.  We only do *one* HGT mutation each time this method
 is called.
 
 \todo HGT should prefer more similar and older fragments.
 \todo Enable replacement mutations.
 */
void cPopulationInterface::DoHGTMutation(cAvidaContext& ctx, cGenome& offspring) {
	// get this organism's cell:
	cPopulationCell& cell = m_world->GetPopulation().GetCell(m_cell_id);
	
	// do we have any fragments available?
	if(cell.CountGenomeFragments() == 0) { return; }
	
	// randomly select the genome fragment for HGT:
	typedef cPopulationCell::fragment_list_type fragment_list_type;
	fragment_list_type& fragments = cell.GetFragments();
	fragment_list_type::iterator f=fragments.begin();
	std::advance(f, ctx.GetRandom().GetInt(fragments.size()));
	
	// find the location within this organism's genome that best matches the selected fragment:
	cGenomeUtil::substring_match ssm = cGenomeUtil::FindBestSubstringMatch(cell.GetOrganism()->GetGenome(), *f);
	
	// there are (currently) two supported types of HGT mutations: insertions & replacements.
	// which one are we doing?
	if(ctx.GetRandom().P(m_world->GetConfig().HGT_INSERTION_MUT_P.Get())) {
		// insertion: insert the fragment at the final location of the match:
		offspring.Insert(ssm.position, *f);
	} else {
		// replacement: replace up to fragment size instructions in the genome.

		// replacement counts forward, so let's get the starting index of where the
		// fragment needs to go.  *inclusive* of the final match position (so +1), and
		// a floor at 0.
		int start = std::max(ssm.position-f->GetSize()+1, 0);

		offspring.Replace(start, ssm.position-start+1, *f);
	}
	
	// resource utilization, cleanup, and stats tracking:
	m_world->GetPopulation().AdjustHGTResource(-f->GetSize());
	m_world->GetStats().GenomeFragmentInserted(cell.GetOrganism(), *f);
	fragments.erase(f);
	
	// old code from a previous version of hgt, kept around for reference:
	//	// calculate the best match for each fragment:
	//	cGenomeUtil::substring_match_list_type match_list;
	//	for(fragment_list_type::iterator i=fragments.begin(); i!=fragments.end(); ++i) {
	//		match_list.push_back(cGenomeUtil::FindBestSubstringMatch(cell.GetOrganism()->GetGenome(), *i));
	//	}	
	
	//	for(fragment_list_type::iterator i=fragments.begin(); i!=fragments.end(); ++i) {
	//		int d = std::max(cGenomeUtil::FindHammingDistance(nearby, *i), 1);
	//		if(m_world->GetRandom().P(m_world->GetConfig().HGT_INSERTION_PROB.Get() / d)) {
	//			m_hardware->InsertGenomeFragment(*i);
	//			m_world->GetPopulation().AdjustHGTResource(-i->GetSize());
	//			m_world->GetStats().GenomeFragmentInserted(this, *i);
	//			fragments.erase(i);
	//			return true;
	//		}
	//	}
}
