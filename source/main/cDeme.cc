/*
 *  cDeme.cc
 *  Avida
 *
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#include "cDeme.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cResource.h"
#include "cWorld.h"

void cDeme::Setup(int id, const tArray<int> & in_cells, int in_width, cWorld* world)
{
  _id = id;
  cell_ids = in_cells;
  birth_count = 0;
  org_count = 0;
  m_world = world;

  _current_merit = 1.0;
  _next_merit = 1.0;
  
  // If width is negative, set it to the full number of cells.
  width = in_width;
  if (width < 1) width = cell_ids.GetSize();
  
  // drain spacial energy resources and place energy in cells
}


int cDeme::GetCellID(int x, int y) const
{
  assert(x >= 0 && x < GetWidth());
  assert(y >= 0 && y < GetHeight());

  const int pos = y * width + x;
  return cell_ids[pos];
}


/*! Note that for this method to work, we blatantly assume that IDs are in
monotonically increasing order!! */
std::pair<int, int> cDeme::GetCellPosition(int cellid) const 
{
  assert(cell_ids.GetSize()>0);
  assert(GetWidth() > 0);
  cellid -= cell_ids[0];
  return std::make_pair(cellid % GetWidth(), cellid / GetWidth());
}

void cDeme::ProcessUpdate() {
  for(int i = 0; i < cell_events.GetSize(); i++) {
    cDemeCellEvent& event = cell_events[i];
    if(event.GetDelay() == _age) {
      event.ActivateEvent(m_world); //start event
      int eventCell = event.GetNextEventCellID();
      while(eventCell != -1) {
        // place event ID in cells' data
        m_world->GetPopulation().GetCell(GetCellID(eventCell)).SetCellData(event.GetEventID());
        eventCell = event.GetNextEventCellID();
      }
    } else if(event.GetDelay()+event.GetDuration() == _age) {
      int eventCell = event.GetNextEventCellID();
      while(eventCell != -1) {
        if(event.GetEventID() == m_world->GetPopulation().GetCell(GetCellID(eventCell)).GetCellData()) { // eventID == CellData
          //set cell data to 0
          m_world->GetPopulation().GetCell(GetCellID(eventCell)).SetCellData(0);
          eventCell = event.GetNextEventCellID();
        }
      }
      event.DeactivateEvent();  //event over
    }
  }
  ++_age;
}

void cDeme::Reset(int previous_generation, bool resetResources)
{
  deme_time_used = 0;
  birth_count = 0;
  _age = 0;
  generation = previous_generation + 1;
  if(resetResources)
    deme_resource_count.ReinitializeResources();
}

void cDeme::Reset(double deme_energy, int previous_generation, bool resetResources)
{
  assert(m_world->GetConfig().ENERGY_ENABLED.Get());
  assert(org_count>0);
  
  total_org_energy = deme_energy;
  if(total_org_energy < 0.0)
    total_org_energy = 0.0;
  
  // split deme energy evenly between organisms in deme
  for (int i=0; i<GetSize(); i++) {
    int cellid = GetCellID(i);
    cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
    if(cell.IsOccupied()) {
      cOrganism* organism = cell.GetOrganism();
      cPhenotype& phenotype = organism->GetPhenotype();
      phenotype.SetEnergy(phenotype.GetStoredEnergy() + total_org_energy/static_cast<double>(org_count));
      phenotype.SetMerit(cMerit(cMerit::EnergyToMerit(phenotype.GetStoredEnergy() * phenotype.GetEnergyUsageRatio(), m_world)));
    }
  }
  Reset(previous_generation, resetResources);
}


/*! Check every cell in this deme for a living organism.  If found, kill it. */
void cDeme::KillAll()
{
  for (int i=0; i<GetSize(); ++i) {
    cPopulationCell& cell = m_world->GetPopulation().GetCell(cell_ids[i]);
    if(cell.IsOccupied()) {
      m_world->GetPopulation().KillOrganism(cell);
    }
  }
}


/*! Replacing this deme's germline has the effect of changing the deme's lineage.
There's still some work to do here; the lineage labels of the Genomes in the germline
are all messed up.
*/
void cDeme::ReplaceGermline(const cGermline& germline) {
	_germline = germline;
}


/*! If this method is called, this is the "parent" deme.  As with individuals,
we need to rotate the heritable merit to the current merit.
*/
void cDeme::UpdateDemeMerit() {
  assert(_next_merit.GetDouble()>=1.0);
  _current_merit = _next_merit;
  _next_merit = 1.0;
}


/*! Update this deme's merit from the given source.
*/
void cDeme::UpdateDemeMerit(cDeme& source) {
  _current_merit = source.GetHeritableDemeMerit();
  _next_merit = 1.0;
  assert(_current_merit.GetDouble()>=1.0);
}


void cDeme::ModifyDemeResCount(const tArray<double> & res_change, const int absolute_cell_id) {
  // find relative cell_id in deme resource count
  const int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  deme_resource_count.ModifyCell(res_change, relative_cell_id);
}

void cDeme::SetupDemeRes(int id, cResource * res, int verbosity) {
  const double decay = 1.0 - res->GetOutflow();
  //addjust the resources cell list pointer here if we want CELL env. commands to be replicated in each deme
  
  deme_resource_count.Setup(id, res->GetName(), res->GetInitial(), 
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(), 
                           res->GetYGravity(), res->GetInflowX1(), 
                           res->GetInflowX2(), res->GetInflowY1(), 
                           res->GetInflowY2(), res->GetOutflowX1(), 
                           res->GetOutflowX2(), res->GetOutflowY1(), 
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           verbosity);
                           
  if(res->GetEnergyResource()) {
    energy_res_ids.Push(id);
  }
}

double cDeme::GetAndClearCellEnergy(int absolute_cell_id) {
  assert(cell_ids[0] <= absolute_cell_id);
  assert(absolute_cell_id <= cell_ids[cell_ids.GetSize()-1]);

  double total_energy = 0.0;
  int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  tArray<double> cell_resources = deme_resource_count.GetCellResources(relative_cell_id);

  // sum all energy resources
  for(int i = 0; i < energy_res_ids.GetSize(); i++) {
    if(cell_resources[energy_res_ids[i]] > 0.0) {
      total_energy += cell_resources[energy_res_ids[i]];
      cell_resources[energy_res_ids[i]] *= -1.0;
    }
  }
  // set energy resources to zero
  deme_resource_count.ModifyCell(cell_resources, relative_cell_id);
  return total_energy;
}

void cDeme::GiveBackCellEnergy(int absolute_cell_id, double value) {
  assert(cell_ids[0] <= absolute_cell_id);
  assert(absolute_cell_id <= cell_ids[cell_ids.GetSize()-1]);

  int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  tArray<double> cell_resources = deme_resource_count.GetCellResources(relative_cell_id);

  double amount_per_resource = value / energy_res_ids.GetSize();
  
  // put back energy resources evenly
  for(int i = 0; i < energy_res_ids.GetSize(); i++) {
    cell_resources[energy_res_ids[i]] += amount_per_resource;
  }
  deme_resource_count.ModifyCell(cell_resources, relative_cell_id);
}

void cDeme::SetCellEvent(int x1, int y1, int x2, int y2, int delay, int duration) {
  cDemeCellEvent demeEvent = cDemeCellEvent(x1, y1, x2, y2, delay, duration, width);
  cell_events.Push(demeEvent);
}

double cDeme::CalculateTotalEnergy() {
  assert(m_world->GetConfig().ENERGY_ENABLED.Get());
    
  double energy_sum = 0.0;
  for (int i=0; i<GetSize(); i++) {
    int cellid = GetCellID(i);
    cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
    if(cell.IsOccupied()) {
      cOrganism* organism = cell.GetOrganism();
      cPhenotype& phenotype = organism->GetPhenotype();
      energy_sum += phenotype.GetStoredEnergy();
    }
  }
  return energy_sum;
}
