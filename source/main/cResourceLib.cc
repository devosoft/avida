/*
 *  cResourceLib.cc
 *  Avida
 *
 *  Called "resource_lib.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
