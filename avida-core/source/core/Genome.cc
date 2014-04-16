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
#include "avida/output/File.h"


Avida::Genome::Genome() : m_hw_config(INVALID_HARDWARE_CONFIG_ID) { ; }

Avida::Genome::Genome(HardwareConfigID hw, const BiotaTraitSet& traits, GeneticRepresentationPtr rep)
  : m_hw_config(hw), m_traits(traits), m_representation(rep)
{
  assert(rep);
}

Avida::Genome::Genome(const Apto::String& genome_str)
{
  // @TODO - unpack genome string more generally
//  Apto::String str(genome_str);
//  m_hw_type = Apto::StrAs(str.Pop(','));
//  m_representation = GeneticRepresentationPtr(new InstructionSequence(str));

  assert(false);
}

Avida::Genome::Genome(const Genome& genome)
: m_hw_config(genome.m_hw_config), m_traits(genome.m_traits), m_representation(genome.m_representation->Clone())
{
  // Copy over epigenetic objects
  for (Apto::Map<Apto::String, EpigeneticObject*>::ConstIterator it = genome.m_epigenetic_objs.Begin(); it.Next();) {
    Apto::String key = it.Get()->Value1();
    EpigeneticObject* obj = *it.Get()->Value2();
    
    m_epigenetic_objs.Set(key, obj->Clone());
  }
}


Apto::String Avida::Genome::AsString() const
{
  // @TODO - generate genome string more generally
//  return Apto::FormatStr("%d,%s,%s", m_hw_type, (const char*)m_props.Get(s_prop_id_instset).StringValue(), (const char*)m_representation->AsString());
  assert(false);
  return "";
}

bool Avida::Genome::operator==(const Genome& genome) const
{
  // Simple hardware type comparision
  if (m_hw_config != genome.m_hw_config) return false;
  if (m_traits != genome.m_traits) return false;
  
  assert(m_representation);
  assert(genome.m_representation);
  if (*m_representation != *genome.m_representation) return false;

  // Compare epigenetic objects
  assert(false);
  
  return true;
}

Avida::Genome& Avida::Genome::operator=(const Genome& genome)
{
  m_hw_config = genome.m_hw_config;
  m_traits = genome.m_traits;

  m_representation = genome.m_representation->Clone();

  // Copy over epigenetic objects
  for (Apto::Map<Apto::String, EpigeneticObject*>::ConstIterator it = genome.m_epigenetic_objs.Begin(); it.Next();) {
    Apto::String key = it.Get()->Value1();
    EpigeneticObject* obj = *it.Get()->Value2();
    
    m_epigenetic_objs.Set(key, obj->Clone());
  }

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
  Avida::Output::File& df = *static_cast<Avida::Output::File*>(dfp);
  df.Write(m_hw_type, "Hardware Type ID", "hw_type");
  df.Write(m_representation->AsString(), "Genome Sequence", "sequence");
  return false;
}




