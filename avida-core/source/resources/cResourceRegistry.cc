/*
 *  cResourceRegistry.cc
 *  Avida
 *
 *  Called "resource_lib.cc" prior to 12/5/05.
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

#include "cResourceRegistry.h"

#include "cResource.h"
#include "cResourceHistory.h"

using namespace std;


cResourceRegistry::~cResourceRegistry()
{
  for (int i = 0; i < m_resource_array.GetSize(); i++) delete m_resource_array[i];
  delete m_initial_levels;
}

cResource* cResourceRegistry::AddResource(const cString& res_name)
{
  if (m_initial_levels) return NULL; // Initial levels calculated, cannot add more resources
  
  const int new_id = m_resource_array.GetSize();
  cResource* new_resource = new cResource(res_name, new_id);
  m_resource_array.Resize(new_id + 1);
  m_resource_array[new_id] = new_resource;
  
  return new_resource;
}

cResource* cResourceRegistry::GetResource(const cString& res_name) const
{
  for (int i = 0; i < m_resource_array.GetSize(); i++) {
    if (m_resource_array[i]->GetName() == res_name) return m_resource_array[i];
  }
  cerr << "Error: Unknown resource '" << res_name << "'." << endl;
  return NULL;
}


const cResourceHistory& cResourceRegistry::GetInitialResourceLevels() const
{
  if (!m_initial_levels) {
    Apto::Array<double> levels(m_resource_array.GetSize());
    for (int i = 0; i < m_resource_array.GetSize(); i++) levels[i] = m_resource_array[i]->GetInitial();
    m_initial_levels = new cResourceHistory;
    m_initial_levels->AddEntry(0, levels);
  }
 
  return *m_initial_levels;
}


bool cResourceRegistry::DoesResourceExist(const cString& res_name) 
{
  for (int i = 0; i < m_resource_array.GetSize(); i++) if (m_resource_array[i]->GetName() == res_name) return true;
  return false;
}

/* This assigns an index to a resource within its own type (deme vs. non-deme)
 * If the resource already has a positive id nothing will be assigned.
 * (Enforced by cResource::SetIndex())
 *
 * Population resource counts include all non-deme resources, regardless of geometry.
 * Deme resource counts include all deme resources, regardless of geometry.
 */
void cResourceRegistry::SetResourceIndex(cResource* res)
{
  bool is_deme = res->GetDemeResource();
  
  if (is_deme) {
    res->SetIndex(m_num_deme_resources);
    m_num_deme_resources++;
  }
  else {
    res->SetIndex(m_resource_array.GetSize() - 1 - m_num_deme_resources);
  }
}




void cResourceRegistry::UpdateResources(cAvidaContext& ctx, const Apto::Array<double> & res_change)
{
  m_resource_count.Modify(ctx, res_change);
}

void cResourceRegistry::UpdateResource(cAvidaContext& ctx, int res_index, double change)
{
  m_resource_count.Modify(ctx, res_index, change);
}

void cResourceRegistry::UpdateCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id)
{
  m_resource_count.ModifyCell(ctx, res_change, cell_id);
}



void cResourceRegistry::SetResource(cAvidaContext& ctx, int res_index, double new_level)
{
  m_resource_count.Set(ctx, res_index, new_level);
}

/* This version of SetResource takes the name of the resource.
 * If a resource by this name does not exist, it does nothing.
 * Otherwise, it sets the resource to the new level, 
 * calling the index version of SetResource().
 */
void cResourceRegistry::SetResource(cAvidaContext& ctx, const cString res_name, double new_level)
{
  cResource* res = GetResource(res_name);
  if (res != NULL) SetResource(ctx, res->GetIndex(), new_level);
}

/* This method sets the inflow of the named resource.
 * It changes this value in the environment, then updates it in the
 * actual population's resource count.
 */
void cResourceRegistry::SetResourceInflow(const cString res_name, double _inflow)
{
  cResource* found_resource = GetResource(res_name);
  assert(found_resource);
  found_resource->SetInflow(_inflow);
  m_resource_count.SetInflow(res_name, _inflow);
}

/* This method sets the outflow of the named resource.
 * It changes this value in the enviroment, then updates the
 * decay rate in the resource count (to 1 - the given outflow, as 
 * outflow is different than decay).
 */
void cResourceRegistry::SetResourceOutflow(const cString res_name, double _outflow)
{
  cResource* found_resource = GetResource(res_name);
  assert(found_resource);
  found_resource->SetInflow(_outflow);
  m_resource_count.SetDecay(res_name, 1 - _outflow);
}


