/*
 *  cResourceLib.cc
 *  Avida
 *
 *  Called "resource_lib.cc" prior to 12/5/05.
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
    tArray<double> levels(m_resource_array.GetSize());
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

