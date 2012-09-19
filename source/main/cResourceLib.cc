/*
 *  cResourceLib.cc
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

#include "cResourceLib.h"

#include "cResource.h"
#include "cResourceHistory.h"

using namespace std;


cResourceLib::~cResourceLib()
{
  for (int i = 0; i < m_resource_array.GetSize(); i++) delete m_resource_array[i];
  delete m_initial_levels;
}

cResource* cResourceLib::AddResource(const cString& res_name)
{
  if (m_initial_levels) return NULL; // Initial levels calculated, cannot add more resources
  
  const int new_id = m_resource_array.GetSize();
  cResource* new_resource = new cResource(res_name, new_id);
  m_resource_array.Resize(new_id + 1);
  m_resource_array[new_id] = new_resource;
  
  return new_resource;
}

cResource* cResourceLib::GetResource(const cString& res_name) const
{
  for (int i = 0; i < m_resource_array.GetSize(); i++) {
    if (m_resource_array[i]->GetName() == res_name) return m_resource_array[i];
  }
  cerr << "Error: Unknown resource '" << res_name << "'." << endl;
  return NULL;
}


const cResourceHistory& cResourceLib::GetInitialResourceLevels() const
{
  if (!m_initial_levels) {
    Apto::Array<double> levels(m_resource_array.GetSize());
    for (int i = 0; i < m_resource_array.GetSize(); i++) levels[i] = m_resource_array[i]->GetInitial();
    m_initial_levels = new cResourceHistory;
    m_initial_levels->AddEntry(0, levels);
  }
 
  return *m_initial_levels;
}


bool cResourceLib::DoesResourceExist(const cString& res_name) 
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
void cResourceLib::SetResourceIndex(cResource* res)
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

