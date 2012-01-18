/*
 *  private/systematics/Genotype.cc
 *  Avida
 *
 *  Created by David on 11/5/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#include "avida/private/systematics/Genotype.h"

#include "avida/core/InstructionSequence.h"
#include "avida/core/Properties.h"

#include "avida/private/systematics/GenotypeArbiter.h"

#include "cDataFile.h"
#include "cStringList.h"
#include "cStringUtil.h"
#include "tDictionary.h"


Avida::Systematics::Genotype::Genotype(GenotypeArbiterPtr mgr, GroupID in_id, UnitPtr founder, Update update,
                             ConstGroupMembershipPtr parents)
  : Group(in_id)
  , m_mgr(mgr)
  , m_handle(NULL)
  , m_src(founder->UnitSource())
  , m_genome(founder->UnitGenome())
  , m_name("001-no_name")
  , m_threshold(false)
  , m_active(true)
  , m_generation_born(Apto::StrAs(founder->Properties().Get("generation")))
  , m_update_born(update)
  , m_update_deactivated(-1)
  , m_depth(0)
  , m_active_offspring_genotypes(0)
  , m_num_organisms(1)
  , m_last_num_organisms(0)
  , m_total_organisms(1)
  , m_provisional_stats(true)
  , m_last_birth_cell(0)
  , m_last_group_id(-1)
  , m_last_forager_type(-1)
  , m_task_counts(mgr->EnvironmentActionTriggerIDs().GetSize())
  , m_prop_map(NULL)
{
  AddActiveReference();
  if (parents) {
    m_parents.Resize(parents->GetSize());
    for (int i = 0; i < m_parents.GetSize(); i++) {
      GenotypePtr p;
      p.DynamicCastFrom((*parents)[i]);
      assert(p);
      m_parents[i] = p;
      m_parents[i]->AddPassiveReference();
      if (i > 0) m_parent_str += ",";
      m_parent_str += Apto::AsStr(m_parents[i]->ID());
      
      m_copied_size.Add(Apto::StrAs(p->Properties().Get("ave_copy_size")));
      m_exe_size.Add(Apto::StrAs(p->Properties().Get("ave_exe_size")));
      m_gestation_time.Add(Apto::StrAs(p->Properties().Get("ave_gestation_time")));
      m_repro_rate.Add(Apto::StrAs(p->Properties().Get("ave_repro_rate")));
      m_merit.Add(Apto::StrAs(p->Properties().Get("ave_metabolic_rate")));
      m_fitness.Add(Apto::StrAs(p->Properties().Get("ave_fitness")));
      
      // Collect all relevant action trigger counts
      for (int i = 0; i < m_mgr->EnvironmentActionTriggerIDs().GetSize(); i++) {
        PropertyID prop_id("environment.triggers.");
        prop_id += m_mgr->EnvironmentActionTriggerIDs()[i];
        prop_id += ".average";
        
        int task_count = Apto::StrAs(p->Properties().Get(prop_id));
        m_task_counts[i].Add(task_count);
      }
    }
  }
  if (m_parents.GetSize()) m_depth = m_parents[0]->Depth() + 1;
  if (!m_src.external) m_breed_in.Inc();
  
  InstructionSequencePtr seq;
  seq.DynamicCastFrom(m_genome.Representation());
  assert(seq);
  m_name = Apto::FormatStr("%03d-no_name", seq->GetSize());
}


Avida::Systematics::Genotype::Genotype(GenotypeArbiterPtr mgr, GroupID in_id, void* prop_p)
: Group(in_id)
, m_mgr(mgr)
, m_handle(NULL)
, m_name("001-no_name")
, m_threshold(false)
, m_active(false)
, m_update_born(-1)
, m_update_deactivated(-1)
, m_depth(0)
, m_active_offspring_genotypes(0)
, m_num_organisms(0)
, m_last_num_organisms(0)
, m_total_organisms(0)
, m_last_birth_cell(0)
, m_last_group_id(-1)
, m_last_forager_type(-1)
, m_task_counts(mgr->EnvironmentActionTriggerIDs().GetSize())
, m_prop_map(NULL)
{
  const tDictionary<cString>& props = *static_cast<const tDictionary<cString>*>(prop_p);
  
  m_src.transmission_type = DIVISION;
  m_src.external = true;
  m_src.arguments = props.Get("src_args");
  if (m_src.arguments == "(none)") m_src.arguments = "";
  
  PropertyMap prop_map;
  prop_map.Set(PropertyPtr(new StringProperty("instset", "Instruction Set", (const char*)props.Get("inst_set"))));
  m_genome = Avida::Genome(props.Get("hw_type").AsInt(), prop_map, GeneticRepresentationPtr(new InstructionSequence((const char*)props.Get("sequence"))));
  
  if (props.HasEntry("gen_born")) {
    m_generation_born = props.Get("gen_born").AsInt();
  } else {
    m_generation_born = -1;
  }
  assert(props.HasEntry("update_born"));
  m_update_born = props.Get("update_born").AsInt();
  if (props.HasEntry("update_deactivated")) {
    m_update_deactivated = props.Get("update_deactivated").AsInt();
  } else {
    m_update_deactivated = -1;
  }
  assert(props.HasEntry("depth"));  
  m_depth = props.Get("depth").AsInt();
  
  if (props.HasEntry("parents")) {
    m_parent_str = props.Get("parents");
  } else if (props.HasEntry("parent_id")) { // Backwards compatible load
    m_parent_str = props.Get("parent_id");
  }
  if (m_parent_str == "(none)") m_parent_str = "";
  cStringList parents((const char*)m_parent_str,',');
  
  m_parents.Resize(parents.GetSize());
  for (int i = 0; i < m_parents.GetSize(); i++) {
    GenotypePtr g;
    g.DynamicCastFrom(m_mgr->Group(parents.Pop().AsInt()));
    m_parents[i] = g;
    assert(m_parents[i]);
    m_parents[i]->AddPassiveReference();
  }
}


Avida::Systematics::Genotype::~Genotype()
{
  delete m_handle;
  delete m_prop_map;
}

Avida::Systematics::RoleID Avida::Systematics::Genotype::Role() const
{
  return m_mgr->Role();
}

Avida::Systematics::ArbiterPtr Avida::Systematics::Genotype::Arbiter() const
{
  return m_mgr;
}


Avida::Systematics::GroupPtr Avida::Systematics::Genotype::ClassifyNewUnit(UnitPtr u, ConstGroupMembershipPtr parents)
{
  m_births.Inc();
  
  if (Matches(u)) {
    m_breed_true.Inc();
    m_total_organisms++;
    m_num_organisms++;
    
    GenotypePtr g = thisPtr();
    m_mgr->AdjustGenotype(g, m_num_organisms - 1, m_num_organisms);
    AddActiveReference();
    return g;
  }  
  
  m_breed_out.Inc();
  return m_mgr->ClassifyNewUnit(u, parents);
}

void Avida::Systematics::Genotype::HandleUnitGestation(UnitPtr u)
{
  if (m_provisional_stats) {
    m_copied_size.Clear();
    m_exe_size.Clear();
    m_gestation_time.Clear();
    m_repro_rate.Clear();
    m_merit.Clear();
    m_fitness.Clear();
    
    for (int i = 0; i < m_task_counts.GetSize(); i++) m_task_counts[i].Clear();
    m_provisional_stats = false;
  }
  
  m_copied_size.Add(Apto::StrAs(u->Properties().Get("last_copied_size")));
  m_exe_size.Add(Apto::StrAs(u->Properties().Get("last_executed_size")));
  m_gestation_time.Add(Apto::StrAs(u->Properties().Get("last_gestation_time")));
  m_repro_rate.Add(1.0 / (double)Apto::StrAs(u->Properties().Get("last_gestation_time")));
  m_merit.Add(Apto::StrAs(u->Properties().Get("last_metabolic_rate")));
  m_fitness.Add(Apto::StrAs(u->Properties().Get("last_fitness")));

  // Collect all relevant action trigger counts
  for (int i = 0; i < m_mgr->EnvironmentActionTriggerIDs().GetSize(); i++) {
    PropertyID prop_id("environment.triggers.");
    prop_id += m_mgr->EnvironmentActionTriggerIDs()[i];
    prop_id += ".count";
    
    int task_count = Apto::StrAs(u->Properties().Get(prop_id));
    m_task_counts[i].Add(task_count);
  }
}


void Avida::Systematics::Genotype::RemoveUnit(UnitPtr)
{
  m_deaths.Inc();
  
  // Remove active reference
  m_a_refs--;
  assert(m_a_refs >= 0);
  
  m_num_organisms--;
  m_mgr->AdjustGenotype(thisPtr(), m_num_organisms + 1, m_num_organisms);
}


const Avida::PropertyMap& Avida::Systematics::Genotype::Properties() const
{
  if (!m_prop_map) setupPropertyMap();
  return *m_prop_map;
}

int Avida::Systematics::Genotype::Depth() const
{
  return m_depth;
}

int Avida::Systematics::Genotype::NumUnits() const
{
  return m_num_organisms;
}

bool Avida::Systematics::Genotype::Serialize(ArchivePtr) const
{
  // @TODO - serialize genotype
  return false;
}

bool Avida::Systematics::Genotype::LegacySave(void* dfp) const
{
  cDataFile& df = *static_cast<cDataFile*>(dfp);
  df.Write(m_id, "ID", "id");
  
  df.Write(m_src.AsString(), "Source", "src");
  
  df.Write(m_src.arguments.GetSize() ? (const char*)m_src.arguments : "(none)", "Source Args", "src_args");
  
  cString str("");
  if (m_parents.GetSize()) {
    str += cStringUtil::Stringf("%d", m_parents[0]->ID());
    for (int i = 1; i < m_parents.GetSize(); i++) {
      str += cStringUtil::Stringf(",%d", m_parents[i]->ID());
    }
  }
  df.Write((str.GetSize()) ? str : "(none)", "Parent ID(s)", "parents");
  
  df.Write(m_num_organisms, "Number of currently living organisms", "num_units");
  df.Write(m_total_organisms, "Total number of organisms that ever existed", "total_units");
  
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(m_genome.Representation());
  df.Write(seq->GetSize(), "Genome Length", "length");
  
  df.Write(m_merit.Average(), "Average Merit", "merit");
  df.Write(m_gestation_time.Average(), "Average Gestation Time", "gest_time");
  df.Write(m_fitness.Average(), "Average Fitness", "fitness");
  df.Write(m_generation_born, "Generation Born", "gen_born");
  df.Write(m_update_born, "Update Born", "update_born");
  df.Write(m_update_deactivated, "Update Deactivated", "update_deactivated");
  df.Write(m_depth, "Phylogenetic Depth", "depth");
  m_genome.LegacySave(dfp);
  
  return false;
}


void Avida::Systematics::Genotype::RemoveActiveReference()
{
  m_a_refs--;
  assert(m_a_refs >= 0);
  
  if (!m_a_refs) m_mgr->AdjustGenotype(thisPtr(), m_num_organisms, 0);
}



bool Avida::Systematics::Genotype::Matches(UnitPtr u)
{
  // Handle source branching
  switch (m_src.transmission_type) {
    case DIVISION:
    case DUPLICATION:
      switch (u->UnitSource().transmission_type) {
        case DIVISION:
        case DUPLICATION:
          break;
          
        case VERTICAL:
        case HORIZONTAL:
          return false;
          break;
          
        default:
          return false;
          break;          
      }
      break;
      
    case VERTICAL:
    case HORIZONTAL:
      switch (u->UnitSource().transmission_type) {
        case DIVISION:
        case DUPLICATION:
          return false;
          break;
          
        case VERTICAL:
        case HORIZONTAL:
          // Verify that the parasite inject label matches
          if (m_src.arguments != u->UnitSource().arguments) return false;
          break;
          
        default:
          return false;
          break;          
      }
      break;
      
    default:
      return false;
      break;
      
  }
  
  // Compare the genomes
  return (m_genome == u->UnitGenome());
}

void Avida::Systematics::Genotype::NotifyNewUnit(UnitPtr u)
{
  m_active = true;
  if (!u->UnitSource().external) {
    switch (u->UnitSource().transmission_type) {
      case DIVISION:
      case HORIZONTAL:
      case VERTICAL:
        m_breed_in.Inc();
        break;
        
      default:
        break;          
    }
  }
  m_total_organisms++;
  m_num_organisms++;

  m_mgr->AdjustGenotype(thisPtr(), m_num_organisms - 1, m_num_organisms);
  AddActiveReference();
}


void Avida::Systematics::Genotype::UpdateReset()
{
  m_last_num_organisms = m_num_organisms;
  m_births.Next();
  m_deaths.Next();
  m_breed_out.Next();
  m_breed_true.Next();
  m_breed_in.Next();
}


void Avida::Systematics::Genotype::setupPropertyMap() const
{
  if (m_prop_map) return;

  m_prop_map = new PropertyMap();
  
#define ADD_FUN_PROP(NAME, DESC, TYPE, VAL) m_prop_map->Set(PropertyPtr(new FunctorProperty<TYPE>(NAME, DESC, FunctorProperty<TYPE>::VAL)));
#define ADD_REF_PROP(NAME, DESC, TYPE, VAL) m_prop_map->Set(PropertyPtr(new ReferenceProperty<TYPE>(NAME, DESC, const_cast<TYPE&>(VAL))));
#define ADD_STR_PROP(NAME, DESC, VAL) m_prop_map->Set(PropertyPtr(new StringProperty(NAME, DESC, VAL)));
  
  ADD_FUN_PROP("genome", "Genome", Apto::String, GetFunctor(&m_genome, &Genome::AsString));
  ADD_STR_PROP("src_transmission_type", "Source Transmission Type", (int)m_src.transmission_type); 
  ADD_REF_PROP("name", "Name", Apto::String, m_name);
  ADD_REF_PROP("parents", "Parent IDs", Apto::String, m_parent_str);
  ADD_REF_PROP("threshold", "Threshold", bool, m_threshold);
  ADD_REF_PROP("update_born", "Update Born", int, m_update_born);
  
  ADD_FUN_PROP("ave_copy_size", "Average Copied Size", double, GetFunctor(&m_copied_size, &cDoubleSum::Average));
  ADD_FUN_PROP("ave_exe_size", "Average Executed Size", double, GetFunctor(&m_exe_size, &cDoubleSum::Average));
  ADD_FUN_PROP("ave_gestation_time", "Average Gestation Time", double, GetFunctor(&m_gestation_time, &cDoubleSum::Average));
  ADD_FUN_PROP("ave_repro_rate", "Average Repro Rate", double, GetFunctor(&m_repro_rate, &cDoubleSum::Average));
  ADD_FUN_PROP("ave_metabolic_rate", "Average Metabolic Rate", double, GetFunctor(&m_merit, &cDoubleSum::Average));
  ADD_FUN_PROP("ave_fitness", "Average Fitness", double, GetFunctor(&m_fitness, &cDoubleSum::Average));
  
  ADD_REF_PROP("recent_births", "Recent Births (during update)", int, m_births.GetCur());
  ADD_REF_PROP("recent_deaths", "Recent Deaths (during update)", int, m_deaths.GetCur());
  ADD_REF_PROP("recent_breed_true", "Recent Breed True (during update)", int, m_breed_true.GetCur());
  ADD_REF_PROP("recent_breed_in", "Recent Breed In (during update)", int, m_breed_in.GetCur());
  ADD_REF_PROP("recent_breed_out", "Recent Breed Out (during update)", int, m_breed_out.GetCur());
  
  ADD_REF_PROP("total_organisms", "Total Organisms", int, m_total_organisms);
  ADD_REF_PROP("last_births", "Births (during last update)", int, m_births.GetLast());
  ADD_REF_PROP("last_deaths", "Deaths (during last update)", int, m_deaths.GetLast());
  ADD_REF_PROP("last_breed_true", "Breed True (during last update)", int, m_breed_true.GetLast());
  ADD_REF_PROP("last_breed_in", "Breed In (during last update)", int, m_breed_in.GetLast());
  ADD_REF_PROP("last_breed_out", "Breed Out (during last update)", int, m_breed_out.GetLast());
  
  ADD_REF_PROP("last_birth_cell", "Last birth cell", int, m_last_birth_cell);
  ADD_REF_PROP("last_group_id", "Last birth group", int, m_last_group_id);
  ADD_REF_PROP("last_forager_type", "Last birth forager type", int, m_last_forager_type);
  
  // Collect all relevant action trigger counts
  for (int i = 0; i < m_mgr->EnvironmentActionTriggerIDs().GetSize(); i++) {
    PropertyID prop_id("environment.triggers.");
    prop_id += m_mgr->EnvironmentActionTriggerIDs()[i];
    prop_id += ".average";
    
    ADD_FUN_PROP(prop_id, Apto::String("Average ") + m_mgr->EnvironmentActionTriggerIDs()[i], double, GetFunctor(&m_task_counts[i], &cIntSum::Average));
  }
  
#undef ADD_FUN_PROP
#undef ADD_REF_PROP
#undef ADD_STR_PROP
}

inline Avida::Systematics::GenotypePtr Avida::Systematics::Genotype::thisPtr()
{
  AddReference(); // Explicitly add reference to internally created SmartPtr
  return GenotypePtr(this);
}
