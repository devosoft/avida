/*
 *  cDeme.cc
 *  Avida
 *
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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
#include "cPopulationCell.h"
#include "cResource.h"


void cDeme::Setup(const tArray<int> & in_cells, int in_width)
{
  cell_ids = in_cells;
  birth_count = 0;
  org_count = 0;

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


void cDeme::Reset() 
{
  birth_count = 0; 
  _age = 0;
  //clear cell energy
  
  deme_resource_count.ReinitializeResources();
}

/*! Replacing this deme's germline has the effect of changing the deme's lineage.
There's still some work to do here; the lineage labels of the Genomes in the germline
are all messed up.

\todo Fix lineage labels in germlines.
*/
void cDeme::ReplaceGermline(const cGermline& germline) {
	_germline = germline;
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
