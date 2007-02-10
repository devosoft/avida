/*
 *  cResourceLib.cc
 *  Avida
 *
 *  Called "resource_lib.cc" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

using namespace std;


cResourceLib::~cResourceLib()
{
  for (int i = 0; i < resource_array.GetSize(); i++) {
    delete resource_array[i];
  }
}

cResource * cResourceLib::AddResource(const cString & res_name)
{
  const int new_id = resource_array.GetSize();
  cResource * new_resource = new cResource(res_name, new_id);
  resource_array.Resize(new_id + 1);
  resource_array[new_id] = new_resource;
  return new_resource;
}

cResource * cResourceLib::GetResource(const cString & res_name) const
{
  for (int i = 0; i < resource_array.GetSize(); i++) {
    if (resource_array[i]->GetName() == res_name) return resource_array[i];
  }
  cerr << "Error: Unknown resource '" << res_name << "'." << endl;
  return NULL;
}

cResource * cResourceLib::GetResource(int id) const
{
  return resource_array[id];
}

bool cResourceLib::DoesResourceExist(const cString & res_name) 

/* Return boolean for if the named resource exists in the resource library */

{
  for (int i = 0; i < resource_array.GetSize(); i++) {
    if (resource_array[i]->GetName() == res_name) return true;
  }
  return false;
}

