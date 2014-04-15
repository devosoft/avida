/*
 *  hardware/Library.cc
 *  avida-core
 *
 *  Created by David on 2/1/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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

#include "avida/hardware/Library.h"


bool Avida::Hardware::Library::RegisterHardwareType(const Apto::String& hw_type_name, Util::ArgSchema& arg_schema,
                                                    HardwareCreateFunctor hw_create)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_hardware_types.Has(hw_type_name)) return false;
  
  HardwareType* hw_type = new HardwareType(arg_schema, hw_create);
  m_hardware_types.Set(hw_type_name, hw_type);
  
  return true;
}


int Avida::Hardware::Library::RegisterFeatureType(const Apto::String& type_name, Util::ArgSchema& arg_schema,
                                                   FeatureCreateFunctor feat_create, FeatureItemConfigFunctor feat_conf)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_feature_types.Has(type_name)) return -1;
  
  int feature_idx = m_feature_types.GetSize();
  FeatureType* feat_type = new FeatureType(arg_schema, feat_create, feat_conf);
  m_feature_types.Set(type_name, feat_type);
  
  return feature_idx;
}


Avida::Hardware::Library::Library()
{
  ;
}


Avida::Hardware::Library::~Library()
{
  for (Apto::Map<Apto::String, HardwareType*>::ValueIterator it = m_hardware_types.Values(); it.Next();) {
    delete (*it.Get());
  }
  for (Apto::Map<Apto::String, FeatureType*>::ValueIterator it = m_feature_types.Values(); it.Next();) {
    delete (*it.Get());
  }
}

