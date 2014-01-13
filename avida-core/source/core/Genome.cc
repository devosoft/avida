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

#include "cInstSet.h"
#include "cHardwareManager.h"

#include "cInitFile.h"
#include "cStringUtil.h"

static Apto::BasicString<Apto::ThreadSafe> s_prop_id_instset("instset");
static PropertyDescriptionMap s_prop_desc_map;

void cHardwareManager::Initialize()
{
  s_prop_desc_map.Set(s_prop_id_instset, "Instruction Set");
}

void cHardwareManager::SetupPropertyMap(PropertyMap& props, const Apto::String& instset)
{
  props.Define(PropertyPtr(new StringProperty(s_prop_id_instset, s_prop_desc_map, instset)));
}



Avida::Genome::Genome() : m_hw_type(-1) { ; }

Avida::Genome::Genome(HardwareTypeID hw, const PropertyMap& props, GeneticRepresentationPtr rep)
  : m_hw_type(hw), m_representation(rep)
{
  assert(rep);
  
  // Copy over properties
  m_props.SetValue(s_prop_id_instset, props.Get(s_prop_id_instset).StringValue());
}

Avida::Genome::Genome(const Apto::String& genome_str)
{
  // @TODO - unpack genome string more generally
  Apto::String str(genome_str);
  m_hw_type = Apto::StrAs(str.Pop(','));
  m_props.SetValue(s_prop_id_instset, str.Pop(','));
  m_representation = GeneticRepresentationPtr(new InstructionSequence(str));
}

Avida::Genome::Genome(const Genome& genome)
: m_hw_type(genome.m_hw_type), m_representation(genome.m_representation->Clone())
{
  m_props.SetValue(s_prop_id_instset, genome.m_props.Get(s_prop_id_instset).StringValue().Clone());
}


Apto::String Avida::Genome::AsString() const
{
  // @TODO - generate genome string more generally
  return Apto::FormatStr("%d,%s,%s", m_hw_type, (const char*)m_props.Get(s_prop_id_instset).StringValue(), (const char*)m_representation->AsString());
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
  
  m_props.SetValue(s_prop_id_instset, genome.m_props.Get(s_prop_id_instset).StringValue());

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
  Avida::Output::File& df = *static_cast<Avida::Output::File*>(dfp);
  df.Write(m_hw_type, "Hardware Type ID", "hw_type");
  df.Write(m_props.Get(s_prop_id_instset).StringValue(), "Inst Set Name" , "inst_set");
  df.Write(m_representation->AsString(), "Genome Sequence", "sequence");
  return false;
}



Avida::Genome::InstSetPropertyMap::InstSetPropertyMap() : m_inst_set(s_prop_id_instset, s_prop_desc_map, Apto::String("")) { ; }
Avida::Genome::InstSetPropertyMap::~InstSetPropertyMap() { ; }

int Avida::Genome::InstSetPropertyMap::GetSize() const { return 1; }
bool Avida::Genome::InstSetPropertyMap::Has(const PropertyID& p_id) const { return (p_id == s_prop_id_instset); }

const Avida::Property& Avida::Genome::InstSetPropertyMap::Get(const PropertyID& p_id) const
{
  assert(p_id.GetSize() == 7);
  if (p_id == s_prop_id_instset) return m_inst_set;

  return *s_default_prop;
}


bool Avida::Genome::InstSetPropertyMap::SetValue(const PropertyID& p_id, const Apto::String& prop_value)
{
  if (p_id == s_prop_id_instset) {
    return m_inst_set.SetValue(prop_value);
  }
  return false;
}


bool Avida::Genome::InstSetPropertyMap::SetValue(const PropertyID& p_id, const int prop_value) { return false; }
bool Avida::Genome::InstSetPropertyMap::SetValue(const PropertyID& p_id, const double prop_value) { return false; }

bool Avida::Genome::InstSetPropertyMap::operator==(const PropertyMap& p) const
{
  if (p.GetSize() == 1 && p.Has(s_prop_id_instset) && p.Get(s_prop_id_instset) == m_inst_set) return true;
  
  return false;
}

void Avida::Genome::InstSetPropertyMap::Define(PropertyPtr p) { ; }
bool Avida::Genome::InstSetPropertyMap::Remove(const PropertyID& p_id) { return false; }

Avida::ConstPropertyIDSetPtr Avida::Genome::InstSetPropertyMap::PropertyIDs() const
{
  PropertyIDSetPtr pidset(new PropertyIDSet);
  pidset->Insert(s_prop_id_instset);
  return pidset;
}

bool Avida::Genome::InstSetPropertyMap::Serialize(ArchivePtr) const
{
  // @TODO
  assert(false);
  return false;
}
