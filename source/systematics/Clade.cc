/*
 *  private/systematics/Clade.cc
 *  Avida
 *
 *  Created by David on 7/30/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
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

#include "avida/private/systematics/Clade.h"

#include "avida/core/Properties.h"
#include "avida/output/File.h"

#include "avida/private/systematics/CladeArbiter.h"


static Avida::PropertyDescriptionMap s_prop_desc_map;

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_name("name");



static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_total_organisms("total_organisms");


void Avida::Systematics::Clade::Initialize()
{
#define DEFINE_PROP(NAME, DESC) s_prop_desc_map.Set(s_prop_name_ ## NAME, DESC);
  DEFINE_PROP(name, "Name");
  
  DEFINE_PROP(total_organisms, "Total Organisms");
  
#undef DEFINE_PROP
}


Avida::Systematics::Clade::Clade(CladeArbiterPtr mgr, GroupID in_id, const Apto::String& name, bool create_empty)
: Group(in_id)
, m_mgr(mgr)
, m_handle(NULL)
, m_name(name)
, m_num_organisms(create_empty ? 0 : 1)
, m_last_num_organisms(0)
, m_total_organisms(create_empty ? 0 : 1)
, m_prop_map(NULL)
{
  if (!create_empty) AddActiveReference();
}




Avida::Systematics::Clade::~Clade()
{
  delete m_prop_map;
}

Avida::Systematics::RoleID Avida::Systematics::Clade::Role() const
{
  return m_mgr->Role();
}

Avida::Systematics::ArbiterPtr Avida::Systematics::Clade::Arbiter() const
{
  return m_mgr;
}


Avida::Systematics::GroupPtr Avida::Systematics::Clade::ClassifyNewUnit(UnitPtr u, ConstGroupMembershipPtr parents)
{
  (void)u;
  (void)parents;
  
  m_total_organisms++;
  m_num_organisms++;
  
  // @TODO how to handle sexual organisms?
  
  CladePtr g = thisPtr();
  m_mgr->AdjustClade(g, m_num_organisms - 1, m_num_organisms);
  AddActiveReference();
  return g;
}

void Avida::Systematics::Clade::HandleUnitGestation(UnitPtr u)
{
  (void)u;
}


void Avida::Systematics::Clade::RemoveUnit()
{
  // Remove active reference
  m_a_refs--;
  assert(m_a_refs >= 0);
  
  m_num_organisms--;
  m_mgr->AdjustClade(thisPtr(), m_num_organisms + 1, m_num_organisms);
}


const Avida::PropertyMap& Avida::Systematics::Clade::Properties() const
{
  if (!m_prop_map) setupPropertyMap();
  return *m_prop_map;
}

int Avida::Systematics::Clade::Depth() const
{
  return 0;
}

int Avida::Systematics::Clade::NumUnits() const
{
  return m_num_organisms;
}

bool Avida::Systematics::Clade::Serialize(ArchivePtr) const
{
  // @TODO - serialize genotype
  return false;
}

bool Avida::Systematics::Clade::LegacySave(void* dfp) const
{
  Avida::Output::File& df = *static_cast<Avida::Output::File*>(dfp);
  df.Write(m_name, "Clade Name", "name");
  return false;
}



void Avida::Systematics::Clade::RemoveActiveReference() const
{
  m_a_refs--;
  assert(m_a_refs >= 0);
  
  Clade* nc_this = const_cast<Clade*>(this);
  if (!m_a_refs) m_mgr->AdjustClade(nc_this->thisPtr(), m_num_organisms, 0);
}



void Avida::Systematics::Clade::NotifyNewUnit(UnitPtr u)
{
  (void)u;
  
  m_total_organisms++;
  m_num_organisms++;
  
  m_mgr->AdjustClade(thisPtr(), m_num_organisms - 1, m_num_organisms);
  AddActiveReference();
}


void Avida::Systematics::Clade::UpdateReset()
{
  m_last_num_organisms = m_num_organisms;
}


void Avida::Systematics::Clade::setupPropertyMap() const
{
  if (m_prop_map) return;
  
  m_prop_map = new HashPropertyMap();
  
#define ADD_REF_PROP(NAME, TYPE, VAL) m_prop_map->Define(PropertyPtr(new ReferenceProperty<TYPE>(s_prop_name_ ## NAME, s_prop_desc_map, const_cast<TYPE&>(VAL))));
  
  ADD_REF_PROP(name, Apto::String, m_name);
  
  ADD_REF_PROP(total_organisms, int, m_total_organisms);
  
#undef ADD_REF_PROP
}

inline Avida::Systematics::CladePtr Avida::Systematics::Clade::thisPtr()
{
  AddReference(); // Explicitly add reference to internally created SmartPtr
  return CladePtr(this);
}
