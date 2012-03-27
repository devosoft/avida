/*
 *  data/Package.cc
 *  avida-core
 *
 *  Created by David on 10/24/11.
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

#include "avida/data/Package.h"

#include <limits>

// Data::Package
// --------------------------------------------------------------------------------------------------------------

Avida::Data::Package::~Package() { ; }

bool Avida::Data::Package::IsAggregate() const { return false; }
Apto::String Avida::Data::Package::GetAggregateDescriptor() const { return Apto::String(); }
int Avida::Data::Package::NumComponents() const { return 0; }
Avida::Data::PackagePtr Avida::Data::Package::GetComponent(int) { return PackagePtr(); }
Avida::Data::ConstPackagePtr Avida::Data::Package::GetComponent(int) const { return ConstPackagePtr(); }


// Data::ArrayPackage
// --------------------------------------------------------------------------------------------------------------

bool Avida::Data::ArrayPackage::BoolValue() const { return (m_entries.GetSize() != 0); }
int Avida::Data::ArrayPackage::IntValue() const { return m_entries.GetSize(); }
double Avida::Data::ArrayPackage::DoubleValue() const { return std::numeric_limits<double>::quiet_NaN(); }


Apto::String Avida::Data::ArrayPackage::StringValue() const
{
  Apto::String rtn;
  
  if (!m_entries.GetSize()) return rtn;
  
  rtn += "'";
  rtn += m_entries[0]->StringValue();
  rtn += "'";
  
  for (int i = 1; i < m_entries.GetSize(); i++) {
    rtn += ",'";
    rtn += m_entries[i]->StringValue();
    rtn += "'";
  }
  
  return rtn;
}


bool Avida::Data::ArrayPackage::IsAggregate() const { return true; }


Apto::String Avida::Data::ArrayPackage::GetAggregateDescriptor() const
{
  Apto::String desc("array(");
  desc += Apto::AsStr(m_entries.GetSize());
  desc += ")";
  return desc;
}

int Avida::Data::ArrayPackage::NumComponents() const { return m_entries.GetSize(); }

Avida::Data::PackagePtr Avida::Data::ArrayPackage::GetComponent(int index)
{
  return m_entries[index];
}

Avida::Data::ConstPackagePtr Avida::Data::ArrayPackage::GetComponent(int index) const
{
  const PackagePtr& pptr = m_entries[index];
  return const_cast<PackagePtr&>(pptr);
}
