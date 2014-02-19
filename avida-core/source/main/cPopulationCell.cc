/*
 *  cPopulationCell.cc
 *  Avida
 *
 *  Called "pop_cell.cc" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cPopulationCell.h"

#include "avida/core/Feedback.h"
#include "cOrganism.h"
#include "cWorld.h"
#include "cEnvironment.h"
#include "cPopulation.h"

#include <cmath>
#include <iterator>


cPopulationCell::cPopulationCell(const cPopulationCell& in_cell)
: m_world(in_cell.m_world)
, m_organism(in_cell.m_organism)
, m_hardware(in_cell.m_hardware)
, m_inputs(in_cell.m_inputs)
, m_cell_id(in_cell.m_cell_id)
{
  // Copy the mutation rates into a new structure
  m_mut_rates = new cMutationRates(*in_cell.m_mut_rates);
}

void cPopulationCell::operator=(const cPopulationCell& in_cell)
{
	if (this != &in_cell) {
		m_world = in_cell.m_world;
		m_organism = in_cell.m_organism;
		m_hardware = in_cell.m_hardware;
		m_inputs = in_cell.m_inputs;
		m_cell_id = in_cell.m_cell_id;
		
		// Copy the mutation rates, constructing the structure as necessary
		if (m_mut_rates == NULL)
			m_mut_rates = new cMutationRates(*in_cell.m_mut_rates);
		else
			m_mut_rates->Copy(*in_cell.m_mut_rates);
}

void cPopulationCell::ResetInputs(cAvidaContext& ctx)
{ 
  m_world->GetEnvironment().SetupInputs(ctx, m_inputs); 
}


void cPopulationCell::InsertOrganism(cOrganism* new_org, cAvidaContext& ctx) 
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
}

cOrganism * cPopulationCell::RemoveOrganism(cAvidaContext& ctx) 
{
  if (m_organism == NULL) return NULL;   // Nothing to do!
	
  // For the moment, the cell doesn't keep track of much...
  cOrganism * out_organism = m_organism;
  m_organism = NULL;
  m_hardware = NULL;
  return out_organism;
}
