/*
 *  data/Provider.cc
 *  avida-core
 *
 *  Created by David on 10/10/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/data/Provider.h"

#include "avida/data/Package.h"
#include "avida/data/Util.h"


bool Avida::Data::Provider::SupportsConcurrentUpdate() const
{
  return false;
}


Avida::Data::PackagePtr Avida::Data::ArgumentedProvider::GetProvidedValuesForArguments(const DataID& data_id,
                                                                                       ConstArgumentSetPtr args) const
{
  Apto::SmartPtr<ArrayPackage, Apto::InternalRCObject> package(new ArrayPackage);
  for (ArgumentSet::ConstIterator it = args->Begin(); it.Next();) {
    PackagePtr comp = this->GetProvidedValueForArgument(data_id, *it.Get());
    if (!comp) continue;
    package->AddComponent(comp);
  }
  return package;
}

Avida::Data::PackagePtr Avida::Data::ArgumentedProvider::GetProvidedValue(const DataID& data_id) const
{
  PackagePtr pkg;
  Apto::String argument;
  
  if (IsStandardID(data_id)) {
    return GetProvidedValueForArgument(data_id, argument);
  } else if (IsArgumentedID(data_id)) {    
    // Find start of argument
    int start_idx = -1;
    for (int i = 0; i < data_id.GetSize(); i++) {
      if (data_id[i] == '[') {
        start_idx = i + 1;
        break;
      }
    }
    if (start_idx != -1) {
      // Separate argument from incoming requested data id
      argument = data_id.Substring(start_idx, data_id.GetSize() - start_idx - 1);
      DataID raw_id = data_id.Substring(0, start_idx) + "]";
      return GetProvidedValueForArgument(raw_id, argument);
    }
  }
  
  return pkg;
}
