/*
 *  cPopulationCell.cc
 *  Avida
 *
 *  Called "pop_cell.cc" prior to 12/5/05.
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

#include "cPopulationCell.h"

#include "cDoubleSum.h"
#include "nHardware.h"
#include "cOrganism.h"
#include "cWorld.h"
#include "cEnvironment.h"
#include "cPopulation.h"
#include "cDeme.h"

#include <cmath>
#include <iterator>

using namespace std;


cPopulationCell::cPopulationCell(const cPopulationCell& in_cell)
: m_world(in_cell.m_world)
, m_organism(in_cell.m_organism)
, m_hardware(in_cell.m_hardware)
, m_inputs(in_cell.m_inputs)
, m_cell_id(in_cell.m_cell_id)
, m_deme_id(in_cell.m_deme_id)
, m_cell_data(in_cell.m_cell_data)
, m_spec_state(in_cell.m_spec_state)
, m_hgt(0)
{
  // Copy the mutation rates into a new structure
  m_mut_rates = new cMutationRates(*in_cell.m_mut_rates);
	
  // Copy the connection list
  tConstListIterator<cPopulationCell> conn_it(in_cell.m_connections);
  cPopulationCell* test_cell;
  while ((test_cell = const_cast<cPopulationCell*>(conn_it.Next()))) m_connections.PushRear(test_cell);
	
	// copy the hgt information, if needed.
	if(in_cell.m_hgt) {
		InitHGTSupport();
		*m_hgt = *in_cell.m_hgt;
	}
}

void cPopulationCell::operator=(const cPopulationCell& in_cell)
{
	if(this != &in_cell) {
		m_world = in_cell.m_world;
		m_organism = in_cell.m_organism;
		m_hardware = in_cell.m_hardware;
		m_inputs = in_cell.m_inputs;
		m_cell_id = in_cell.m_cell_id;
		m_deme_id = in_cell.m_deme_id;
		m_cell_data = in_cell.m_cell_data;
		m_spec_state = in_cell.m_spec_state;
		
		// Copy the mutation rates, constructing the structure as necessary
		if (m_mut_rates == NULL)
			m_mut_rates = new cMutationRates(*in_cell.m_mut_rates);
		else
			m_mut_rates->Copy(*in_cell.m_mut_rates);
		
		// Copy the connection list
		tConstListIterator<cPopulationCell> conn_it(in_cell.m_connections);
		cPopulationCell* test_cell;
		while ((test_cell = const_cast<cPopulationCell*>(conn_it.Next()))) m_connections.PushRear(test_cell);
		
		// copy hgt information, if needed.
		delete m_hgt;
		m_hgt = 0;
		if(in_cell.m_hgt) {
			InitHGTSupport();
			*m_hgt = *in_cell.m_hgt;
		}
	}
}

void cPopulationCell::Setup(cWorld* world, int in_id, const cMutationRates& in_rates, int x, int y)
{
  m_world = world;
  m_cell_id = in_id;
  m_x = x;
  m_y = y;
  m_deme_id = -1;
  m_cell_data.contents = 0;
  m_cell_data.org_id = -1;
  m_cell_data.update = -1;
  m_spec_state = 0;
  
  if (m_mut_rates == NULL)
    m_mut_rates = new cMutationRates(in_rates);
  else
    m_mut_rates->Copy(in_rates);
}

void cPopulationCell::Rotate(cPopulationCell& new_facing)
{
  // @CAO Note, this breaks avida if new_facing is not in connection_list
	
  //@AWC if this cell contains a migrant then we assume new_facing is not in the connection list and bail out ...
  if(IsMigrant()){
    UnsetMigrant(); //@AWC -- unset the migrant flag for the next time this cell is used
    return;
  }
	
#ifdef DEBUG
  int scan_count = 0;
#endif
  while (m_connections.GetFirst() != &new_facing) {
    m_connections.CircNext();
#ifdef DEBUG
    assert(++scan_count < m_connections.GetSize());
#endif
  }
}

/*! This method recursively builds a set of cells that neighbor this cell, out to 
 the given depth.  The set must be passed in by-reference, as calls to this method 
 must share a common set of already-visited cells.
 */
void cPopulationCell::GetNeighboringCells(std::set<cPopulationCell*>& cell_set, int depth) const {
	typedef std::set<cPopulationCell*> cell_set_t;
  
  // For each cell in our connection list...
  tConstListIterator<cPopulationCell> i(m_connections);
  while(!i.AtEnd()) {
		// store the cell pointer, and check to see if we've already visited that cell...
    cPopulationCell* cell = i.Next();
		assert(cell != 0); // cells should never be null.
		std::pair<cell_set_t::iterator, bool> ins = cell_set.insert(cell);
		// and if so, recurse to it...
		if(ins.second && (depth > 1)) {
			cell->GetNeighboringCells(cell_set, depth-1);
		}
	}
}

/*! Recursively build a set of occupied cells that neighbor this one, out to the given depth.
*/
void cPopulationCell::GetOccupiedNeighboringCells(std::set<cPopulationCell*>& occupied_cell_set, int depth) const {
	// we'll do this the easy way, and just filter the neighbor set.
	std::set<cPopulationCell*> cell_set;
	GetNeighboringCells(cell_set, depth);
	for(std::set<cPopulationCell*>::iterator i=cell_set.begin(); i!=cell_set.end(); ++i) {
		if((*i)->IsOccupied()) {
			occupied_cell_set.insert(*i);
		}
	}
}

/*! These values are chosen so as to make loops on the facing 'easy'.
 111 = NE
 101 = E
 100 = SE
 000 = S
 001 = SW
 011 = W
 010 = NW
 110 = N
 
 Facing is determined by the relative positions of this cell and the cell that 
 is currently faced. Note that we cannot differentiate between directions on a 2x2 
 torus.
 */
int cPopulationCell::GetFacing()
{
  // This whole function is a hack.
	cPopulationCell* faced = ConnectionList().GetFirst();
	
	int x=0,y=0,lr=0,du=0;
	faced->GetPosition(x,y);
  
	if((x==m_x-1) || (x>m_x+1))
		lr = -1; //left
	else if((x==m_x+1) || (x<m_x-1))
		lr = 1; //right
	
	if((y==m_y-1) || (y>m_y+1))
		du = -1; //down
	else if((y==m_y+1) || (y<m_y-1))
		du = 1; //up
  
	// This is hackish.
	// If you change these return values then the directional send tasks, like sent-north, need to be updated.
	if(lr==0 && du==-1) return 0; //S
	if(lr==-1 && du==-1) return 1; //SW
	if(lr==-1 && du==0) return 3; //W
	if(lr==-1 && du==1) return 2; //NW
	if(lr==0 && du==1) return 6; //N
	if(lr==1 && du==1) return 7; //NE
	if(lr==1 && du==0) return 5; //E
	if(lr==1 && du==-1) return 4; //SE
  
	assert(false);
  
  return 0;
}

void cPopulationCell::ResetInputs(cAvidaContext& ctx) 
{ 
  m_world->GetEnvironment().SetupInputs(ctx, m_inputs); 
}


void cPopulationCell::InsertOrganism(cOrganism* new_org)
{
  assert(new_org != NULL);
  assert(m_organism == NULL);
	
  // Adjust this cell's attributes to account for the new organism.
  m_organism = new_org;
  m_hardware = &new_org->GetHardware();
  m_world->GetStats().AddSpeculativeWaste(m_spec_state);
  m_spec_state = 0;
	
  // Adjust the organism's attributes to match this cell.
  m_organism->GetOrgInterface().SetCellID(m_cell_id);
  m_organism->GetOrgInterface().SetDemeID(m_deme_id);
	
  // If this organism is new, set the previously-seen cell id
  if(m_organism->GetOrgInterface().GetPrevSeenCellID() == -1) {
    m_organism->GetOrgInterface().SetPrevSeenCellID(m_cell_id);
  }
  
  if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1 && m_world->GetConfig().FRAC_ENERGY_TRANSFER.Get() > 0.0) {
    // uptake all the cells energy
    double uptake_energy = UptakeCellEnergy(1.0);
    if(uptake_energy != 0.0) {
      // update energy and merit
      cPhenotype& phenotype = m_organism->GetPhenotype();
      phenotype.ReduceEnergy(-1.0 * uptake_energy);
      phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy() * phenotype.GetEnergyUsageRatio())));
    }
  }
}

cOrganism * cPopulationCell::RemoveOrganism()
{
  if (m_organism == NULL) return NULL;   // Nothing to do!
	
  // For the moment, the cell doesn't keep track of much...
  cOrganism * out_organism = m_organism;
  if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1 && m_world->GetConfig().FRAC_ENERGY_TRANSFER.Get() > 0.0
		 && m_world->GetConfig().FRAC_ENERGY_DECAY_AT_DEME_BIRTH.Get() != 1.0) { // hack
    m_world->GetPopulation().GetDeme(m_deme_id).GiveBackCellEnergy(m_cell_id, m_organism->GetPhenotype().GetStoredEnergy() * m_world->GetConfig().FRAC_ENERGY_TRANSFER.Get());
  }
  m_organism = NULL;
  m_hardware = NULL;
  return out_organism;
}

double cPopulationCell::UptakeCellEnergy(double frac_to_uptake) {
  assert(0.0 <= frac_to_uptake);
  assert(frac_to_uptake <= 1.0);
	
  double cell_energy = m_world->GetPopulation().GetDeme(m_deme_id).GetAndClearCellEnergy(m_cell_id);  
  double uptakeAmount = cell_energy * frac_to_uptake;
  cell_energy -= uptakeAmount;
  m_world->GetPopulation().GetDeme(m_deme_id).GiveBackCellEnergy(m_cell_id, cell_energy);
  return uptakeAmount;
}


bool cPopulationCell::OK()
{
  // Nothing for the moment...
  return true;
}

/*! Diffuse genome fragments from this cell to its neighbors.
 
 NOTE: This method is for OUTGOING diffusion only.
 
 There are many possible ways in which genome fragments could be diffused.  We'll
 put in the framework to support those other mechanisms, but we're not going to 
 worry about this until we need it.  Not terribly interested in recreating an
 artificial chemistry here...
 */
void cPopulationCell::DiffuseGenomeFragments() {
	InitHGTSupport();
	
	switch(m_world->GetConfig().HGT_DIFFUSION_METHOD.Get()) {
			case 0: { // none
				break;
			}
			default: {
				m_world->GetDriver().RaiseFatalException(-1, "Unrecognized diffusion type in cPopulationCell::DiffuseGenomeFragments().");
			}
	}
}

/*! Add fragments from the passed-in genome to the HGT fragments contained in this cell.
 
 Split the passed-in genome into fragments according to a normal distribution specified
 by HGT_FRAGMENT_SIZE_MEAN and HGT_FRAGMENT_SIZE_VARIANCE.  These fragments are added
 to this cell's fragment list.
 
 As a safety measure, we also remove old fragments to conserve memory.  Specifically, we
 remove old fragments until at most HGT_MAX_FRAGMENTS_PER_CELL fragments remain.
 */
void cPopulationCell::AddGenomeFragments(const cSequence& genome) {
	assert(genome.GetSize()>0); // oh, sweet sanity.
	InitHGTSupport();
	
	m_world->GetPopulation().AdjustHGTResource(genome.GetSize());
	
	cAvidaContext ctx(m_world, m_world->GetRandom());
	cGenomeUtil::RandomSplit(ctx, 
													 m_world->GetConfig().HGT_FRAGMENT_SIZE_MEAN.Get(),
													 m_world->GetConfig().HGT_FRAGMENT_SIZE_VARIANCE.Get(),
													 genome,
													 m_hgt->fragments);
	
	// pop off the front of this cell's buffer until we have <= HGT_MAX_FRAGMENTS_PER_CELL.
	while(m_hgt->fragments.size()>(unsigned int)m_world->GetConfig().HGT_MAX_FRAGMENTS_PER_CELL.Get()) {
		m_world->GetPopulation().AdjustHGTResource(-m_hgt->fragments.front().GetSize());
		m_hgt->fragments.pop_front();
	}
}

/*! Retrieve the number of genome fragments currently found in this cell.
 */
unsigned int cPopulationCell::CountGenomeFragments() const {
	if(IsHGTInitialized()) {
		return m_hgt->fragments.size();
	} else {
		return 0;
	}
}

/*! Remove and return a random genome fragment.
 */
cSequence cPopulationCell::PopGenomeFragment() {
	assert(m_hgt!=0);
	fragment_list_type::iterator i = m_hgt->fragments.begin();
	std::advance(i, m_world->GetRandom().GetUInt(0, m_hgt->fragments.size()));	
	cSequence tmp = *i;
	m_hgt->fragments.erase(i);
	return tmp;
}

/*! Retrieve the list of fragments from this cell.
 */
cPopulationCell::fragment_list_type& cPopulationCell::GetFragments() {
	InitHGTSupport();
	return m_hgt->fragments;
}

/*!	Clear all fragments from this cell, adjust resources as required.
 */
void cPopulationCell::ClearFragments() {
	InitHGTSupport();
	for(fragment_list_type::iterator i=m_hgt->fragments.begin(); i!=m_hgt->fragments.end(); ++i) {
		m_world->GetPopulation().AdjustHGTResource(-i->GetSize());
	}
	m_hgt->fragments.clear();
}

void cPopulationCell::SetCellData(int data, int org_id)
{
  m_cell_data.contents = data;
  m_cell_data.org_id = org_id;
  m_cell_data.update = m_world->GetStats().GetUpdate();
}

