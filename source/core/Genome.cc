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
#include "avida/core/InstructionSequence.h"

#include "cInstSet.h"
#include "cHardwareManager.h"

#include "cDataFile.h"
#include "cInitFile.h"
#include "cStringUtil.h"


Avida::Genome::Genome() : m_hw_type(-1) { ; }

Avida::Genome::Genome(HardwareTypeID hw, const PropertyMap& props, GeneticRepresentationPtr rep)
  : m_hw_type(hw), m_representation(rep)
{
  assert(rep);
  
  // Copy over properties
  PropertyMap::PropertyIDIterator it = props.PropertyIDs();
  while (it.Next()) m_props.Define(PropertyPtr(new StringProperty(props.Get(*it.Get()))));
}

Avida::Genome::Genome(const Apto::String& genome_str)
{
  // @TODO - unpack genome string more generally
  Apto::String str(genome_str);
  m_hw_type = Apto::StrAs(str.Pop(','));
  cHardwareManager::SetupPropertyMap(m_props, str.Pop(','));
  m_representation = GeneticRepresentationPtr(new InstructionSequence(str));
}

Avida::Genome::Genome(const Genome& genome)
: m_hw_type(genome.m_hw_type), m_representation(genome.m_representation->Clone())
{
  PropertyMap::PropertyIDIterator it = genome.m_props.PropertyIDs();
  while (it.Next()) m_props.Define(PropertyPtr(new StringProperty(genome.m_props.Get(*it.Get()))));
}


Apto::String Avida::Genome::AsString() const
{
  // @TODO - generate genome string more generally
  return Apto::FormatStr("%d,%s,%s", m_hw_type, (const char*)m_props.Get("instset").StringValue(), (const char*)m_representation->AsString());
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
  m_hw_type = genome.m_hw_type;
  
  // @TODO - should clear internal properties first
  PropertyMap::PropertyIDIterator it = genome.m_props.PropertyIDs();
  while (it.Next()) m_props.Define(PropertyPtr(new StringProperty(genome.m_props.Get(*it.Get()))));

  m_representation = genome.m_representation->Clone();
  
  return *this;
}

bool Avida::Genome::Serialize(ArchivePtr) const
{
  // @TODO - genome serialize
  assert(false);
  return false;
}

Avida::GenomePtr Avida::Genome::Deserialize(ArchivePtr)
{
  // @TODO - genome deserialize
  assert(false);
  return GenomePtr();
}

bool Avida::Genome::LegacySave(void* dfp) const
{
  cDataFile& df = *static_cast<cDataFile*>(dfp);
  df.Write(m_hw_type, "Hardware Type ID", "hw_type");
  df.Write(m_props.Get("instset").StringValue(), "Inst Set Name" , "inst_set");
  df.Write(m_representation->AsString(), "Genome Sequence", "sequence");
  return false;
}

