/*
 *  core/Genome.cc
 *  avida-core
 *
 *  Created by David on 12/21/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#include "avida/core/Genome.h"

#include "apto/core/Set.h"
#include "avida/core/Feedback.h"

#include "cInstSet.h"
#include "cHardwareManager.h"

#include "cDataFile.h"
#include "cInitFile.h"
#include "cStringUtil.h"
#include "tDictionary.h"


Avida::Genome::Genome() : m_hw_type(-1) { ; }

Avida::Genome::Genome(HardwareTypeID hw, const PropertyMap& props, GeneticRepresentationPtr rep)
  : m_hw_type(hw), m_representation(rep)
{
  assert(rep);
  
  // Copy over properties
  PropertyMap::PropertyIDIterator it = props.PropertyIDs();
  while (it.Next()) m_props.Set(PropertyPtr(new StringProperty(props.Get(*it.Get()))));
}

Avida::Genome::Genome(const Apto::String& genome_str)
{
  // @TODO - unpack genome string
  assert(false);
}


Apto::String Avida::Genome::AsString() const
{
  // @TODO - generate genome string
//  return Apto::FormatStr("%d,%s,%s", m_hw_type, (const char*)m_inst_set, (const char*)m_rep->AsString());
  return "";
}

bool Avida::Genome::operator==(const Genome& genome) const
{
  // Simple hardware type comparision
  if (m_hw_type != genome.m_hw_type) return false;
  if (m_props != genome.m_props) return false;
  
  assert(m_representation);
  assert(genome.m_representation);
  if (*m_representation != *genome.m_representation) return false;

  return true;
}

Avida::Genome& Avida::Genome::operator=(const Genome& genome)
{
  // @TODO - genome operator=
  assert(false);
  return *this;
}

bool Avida::Genome::Serialize(ArchivePtr ar) const
{
  // @TODO - genome serialize
  assert(false);
  return false;
}

Avida::GenomePtr Avida::Genome::Deserialize(ArchivePtr ar)
{
  // @TODO - genome deserialize
  assert(false);
  return GenomePtr();
}
