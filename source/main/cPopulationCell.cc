/*
 *  cPopulationCell.cc
 *  Avida
 *
 *  Called "pop_cell.cc" prior to 12/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#include "nHardware.h"
#include "cOrganism.h"
#include "cTools.h"
#include "cTools.h"
#include "cWorld.h"
#include "cEnvironment.h"
#include "cPopulation.h"
#include "cDeme.h"

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
{
  // Copy the mutation rates into a new structure
  m_mut_rates = new cMutationRates(*in_cell.m_mut_rates);

  // Copy the connection list
  tConstListIterator<cPopulationCell> conn_it(in_cell.m_connections);
  cPopulationCell* test_cell;
  while ((test_cell = const_cast<cPopulationCell*>(conn_it.Next()))) m_connections.PushRear(test_cell);
}

void cPopulationCell::operator=(const cPopulationCell& in_cell)
{
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
}

void cPopulationCell::Setup(cWorld* world, int in_id, const cMutationRates& in_rates, int x, int y)
{
  m_world = world;
  m_cell_id = in_id;
  m_x = x;
  m_y = y;
  m_deme_id = -1;
  m_cell_data = 0;
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
  assert(new_org->GetGenotype() != NULL);
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
      m_organism->GetPhenotype().ReduceEnergy(-1.0 * uptake_energy);
      m_organism->GetPhenotype().SetMerit(cMerit(cMerit::EnergyToMerit(m_organism->GetPhenotype().GetStoredEnergy() * m_organism->GetPhenotype().GetEnergyUsageRatio(), m_world)));
    }
  }
}

cOrganism * cPopulationCell::RemoveOrganism()
{
  if (m_organism == NULL) return NULL;   // Nothing to do!

  // For the moment, the cell doesn't keep track of much...
  cOrganism * out_organism = m_organism;
  if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1 && m_world->GetConfig().FRAC_ENERGY_TRANSFER.Get() > 0.0) {
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
