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
#include "avida/output/File.h"

#include "avida/private/systematics/GenotypeArbiter.h"

#include "cHardwareManager.h"
#include "cStringList.h"
#include "cStringUtil.h"


static const Apto::BasicString<Apto::ThreadSafe> s_unit_prop_name_last_copied_size("last_copied_size");
static const Apto::BasicString<Apto::ThreadSafe> s_unit_prop_name_last_executed_size("last_executed_size");
static const Apto::BasicString<Apto::ThreadSafe> s_unit_prop_name_last_gestation_time("last_gestation_time");
static const Apto::BasicString<Apto::ThreadSafe> s_unit_prop_name_last_metabolic_rate("last_metabolic_rate");
static const Apto::BasicString<Apto::ThreadSafe> s_unit_prop_name_last_fitness("last_fitness");


static Avida::PropertyDescriptionMap s_prop_desc_map;

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_genome("genome");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_src_transmission_type("src_transmission_type");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_name("name");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_parents("parents");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_threshold("threshold");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_update_born("update_born");

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_ave_copy_size("ave_copy_size");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_ave_exe_size("ave_exe_size");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_ave_gestation_time("ave_gestation_time");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_ave_repro_rate("ave_repro_rate");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_ave_metabolic_rate("ave_metabolic_rate");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_ave_fitness("ave_fitness");

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_max_fitness("max_fitness");

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_recent_births("recent_births");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_recent_deaths("recent_deaths");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_recent_breed_true("recent_breed_true");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_recent_breed_in("recent_breed_in");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_recent_breed_out("recent_breed_out");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_recent_gestation_count("recent_gestation_count");

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_total_organisms("total_organisms");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_births("last_births");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_deaths("last_deaths");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_breed_true("last_breed_true");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_breed_in("last_breed_in");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_breed_out("last_breed_out");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_gestation_count("last_gestation_count");

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_birth_cell("last_birth_cell");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_group_id("last_group_id");
static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_forager_type("last_forager_type");

static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_total_gestation_count("total_gestation_count");


void Avida::Systematics::Genotype::Initialize()
{
#define DEFINE_PROP(NAME, DESC) s_prop_desc_map.Set(s_prop_name_ ## NAME, DESC);
  DEFINE_PROP(genome, "Genome");
  DEFINE_PROP(src_transmission_type, "Source Transmission Type");
  DEFINE_PROP(name, "Name");
  DEFINE_PROP(parents, "Parent IDs");
  DEFINE_PROP(threshold, "Threshold");
  DEFINE_PROP(update_born, "Update Born");
  
  DEFINE_PROP(ave_copy_size, "Average Copied Size");
  DEFINE_PROP(ave_exe_size, "Average Executed Size");
  DEFINE_PROP(ave_gestation_time, "Average Gestation Time");
  DEFINE_PROP(ave_repro_rate, "Average Repro Rate");
  DEFINE_PROP(ave_metabolic_rate, "Average Metabolic Rate");
  DEFINE_PROP(ave_fitness, "Average Fitness");
  
  DEFINE_PROP(max_fitness, "Maximum Fitness");
  
  DEFINE_PROP(recent_births, "Recent Births (during update)");
  DEFINE_PROP(recent_deaths, "Recent Deaths (during update)");
  DEFINE_PROP(recent_breed_true, "Recent Breed True (during update)");
  DEFINE_PROP(recent_breed_in, "Recent Breed In (during update)");
  DEFINE_PROP(recent_breed_out, "Recent Breed Out (during update)");
  DEFINE_PROP(recent_gestation_count, "Recent Gestation Count (during update)");
  
  DEFINE_PROP(total_organisms, "Total Organisms");
  DEFINE_PROP(last_births, "Births (during last update)");
  DEFINE_PROP(last_deaths, "Deaths (during last update)");
  DEFINE_PROP(last_breed_true, "Breed True (during last update)");
  DEFINE_PROP(last_breed_in, "Breed In (during last update)");
  DEFINE_PROP(last_breed_out, "Breed Out (during last update)");
  DEFINE_PROP(last_gestation_count, "Gestation Count (during last update)");
  
  DEFINE_PROP(last_birth_cell, "Last birth cell");
  DEFINE_PROP(last_group_id, "Last birth group");
  DEFINE_PROP(last_forager_type, "Last birth forager type");

  DEFINE_PROP(total_gestation_count, "Gestation Count (total)");
#undef DEFINE_PROP
}


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
  , m_generation_born(founder->Properties().Get("generation").IntValue())
  , m_update_born(update)
  , m_update_deactivated(-1)
  , m_depth(0)
  , m_active_offspring_genotypes(0)
  , m_num_organisms(1)
  , m_last_num_organisms(0)
  , m_total_organisms(1)
  , m_last_birth_cell(0)
  , m_last_group_id(-1)
  , m_last_forager_type(-1)
  , m_task_counts(mgr->NumEnvironmentActionTriggers())
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
      
//      m_copied_size.Add(p->Properties().Get(s_prop_name_ave_copy_size));
//      m_exe_size.Add(p->Properties().Get(s_prop_name_ave_exe_size));
//      m_gestation_time.Add(p->Properties().Get(s_prop_name_ave_gestation_time));
//      m_repro_rate.Add(p->Properties().Get(s_prop_name_ave_repro_rate));
//      m_merit.Add(p->Properties().Get(s_prop_name_ave_metabolic_rate));
//      m_fitness.Add(p->Properties().Get(s_prop_name_ave_fitness));
      
      // Collect all relevant action trigger counts
//      for (int i = 0; i < m_mgr->EnvironmentActionTriggerAverageIDs().GetSize(); i++) {
//        m_task_counts[i].Add(static_cast<int>(p->Properties().Get(m_mgr->EnvironmentActionTriggerAverageIDs()[i])));
//      }
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
, m_task_counts(mgr->NumEnvironmentActionTriggers())
, m_prop_map(NULL)
{
  Apto::Map<Apto::String, Apto::String>& props = *(*static_cast<Apto::SmartPtr<Apto::Map<Apto::String, Apto::String> >*>(prop_p));
  
  m_src.transmission_type = DIVISION;
  m_src.external = true;
  m_src.arguments = props.Get("src_args");
  if (m_src.arguments == "(none)") m_src.arguments = "";
  
  HashPropertyMap prop_map;
  cString inst_set = (const char*)props.Get("inst_set");
  if (inst_set == "") inst_set = "(default)";
  
  cHardwareManager::SetupPropertyMap(prop_map, (const char*)inst_set);
  m_genome = Avida::Genome(Apto::StrAs(props.Get("hw_type")), prop_map, GeneticRepresentationPtr(new InstructionSequence((const char*)props.Get("sequence"))));
  
  if (props.Has("gen_born")) {
    m_generation_born = Apto::StrAs(props.Get("gen_born"));
  } else {
    m_generation_born = -1;
  }
  assert(props.Has("update_born"));
  m_update_born = Apto::StrAs(props.Get("update_born"));
  if (props.Has("update_deactivated")) {
    m_update_deactivated = Apto::StrAs(props.Get("update_deactivated"));
  } else {
    m_update_deactivated = -1;
  }
  assert(props.Has("depth"));
  m_depth = Apto::StrAs(props.Get("depth"));
  
  if (props.Has("parents")) {
    m_parent_str = (const char*)props.Get("parents");
  } else if (props.Has("parent_id")) { // Backwards compatible load
    m_parent_str = (const char*)props.Get("parent_id");
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
  m_gestation_count.Inc();
  
  m_copied_size.Add(u->Properties().Get(s_unit_prop_name_last_copied_size));
  m_exe_size.Add(u->Properties().Get(s_unit_prop_name_last_executed_size));
  
  double last_gestation_time = u->Properties().Get(s_unit_prop_name_last_gestation_time);
  m_gestation_time.Add(last_gestation_time);
  m_repro_rate.Add(1.0 / last_gestation_time);
  m_merit.Add(u->Properties().Get(s_unit_prop_name_last_metabolic_rate));
  m_fitness.Add(u->Properties().Get(s_unit_prop_name_last_fitness));

  // Collect all relevant action trigger counts
//  for (int i = 0; i < m_mgr->EnvironmentActionTriggerCountIDs().GetSize(); i++) {
//    m_task_counts[i].Add(static_cast<int>(u->Properties().Get(m_mgr->EnvironmentActionTriggerCountIDs()[i])));
//  }
}


void Avida::Systematics::Genotype::RemoveUnit()
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
  Avida::Output::File& df = *static_cast<Avida::Output::File*>(dfp);
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


void Avida::Systematics::Genotype::RemoveActiveReference() const
{
  m_a_refs--;
  assert(m_a_refs >= 0);
  
  Genotype* nc_this = const_cast<Genotype*>(this);
  if (!m_a_refs) m_mgr->AdjustGenotype(nc_this->thisPtr(), m_num_organisms, 0);
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
  m_gestation_count.Next();
}


void Avida::Systematics::Genotype::setupPropertyMap() const
{
  if (m_prop_map) return;

  m_prop_map = new HashPropertyMap();
  
#define ADD_FUN_PROP(NAME, TYPE, VAL) m_prop_map->Define(PropertyPtr(new FunctorProperty<TYPE>(s_prop_name_ ## NAME, s_prop_desc_map, FunctorProperty<TYPE>::VAL)));
#define ADD_REF_PROP(NAME, TYPE, VAL) m_prop_map->Define(PropertyPtr(new ReferenceProperty<TYPE>(s_prop_name_ ## NAME, s_prop_desc_map, const_cast<TYPE&>(VAL))));
#define ADD_STR_PROP(NAME, VAL) m_prop_map->Define(PropertyPtr(new StringProperty(s_prop_name_ ## NAME, s_prop_desc_map, VAL)));
  
  ADD_FUN_PROP(genome, Apto::String, GetFunctor(&m_genome, &Genome::AsString));
  ADD_STR_PROP(src_transmission_type, (int)m_src.transmission_type);
  ADD_REF_PROP(name, Apto::String, m_name);
  ADD_REF_PROP(parents, Apto::String, m_parent_str);
  ADD_REF_PROP(threshold, bool, m_threshold);
  ADD_REF_PROP(update_born, int, m_update_born);
  
  ADD_FUN_PROP(ave_copy_size, double, GetFunctor(&m_copied_size, &cDoubleSum::Average));
  ADD_FUN_PROP(ave_exe_size, double, GetFunctor(&m_exe_size, &cDoubleSum::Average));
  ADD_FUN_PROP(ave_gestation_time, double, GetFunctor(&m_gestation_time, &cDoubleSum::Average));
  ADD_FUN_PROP(ave_repro_rate, double, GetFunctor(&m_repro_rate, &cDoubleSum::Average));
  ADD_FUN_PROP(ave_metabolic_rate, double, GetFunctor(&m_merit, &cDoubleSum::Average));
  ADD_FUN_PROP(ave_fitness, double, GetFunctor(&m_fitness, &cDoubleSum::Average));

  ADD_FUN_PROP(max_fitness, double, GetFunctor(&m_fitness, &cDoubleSum::Max));
  
  ADD_REF_PROP(recent_births, int, m_births.GetCur());
  ADD_REF_PROP(recent_deaths, int, m_deaths.GetCur());
  ADD_REF_PROP(recent_breed_true, int, m_breed_true.GetCur());
  ADD_REF_PROP(recent_breed_in, int, m_breed_in.GetCur());
  ADD_REF_PROP(recent_breed_out, int, m_breed_out.GetCur());
  ADD_REF_PROP(recent_gestation_count, int, m_gestation_count.GetCur());
  
  ADD_REF_PROP(total_organisms, int, m_total_organisms);
  ADD_REF_PROP(last_births, int, m_births.GetLast());
  ADD_REF_PROP(last_deaths, int, m_deaths.GetLast());
  ADD_REF_PROP(last_breed_true, int, m_breed_true.GetLast());
  ADD_REF_PROP(last_breed_in, int, m_breed_in.GetLast());
  ADD_REF_PROP(last_breed_out, int, m_breed_out.GetLast());
  ADD_REF_PROP(last_gestation_count, int, m_gestation_count.GetLast());
  
  ADD_REF_PROP(last_birth_cell, int, m_last_birth_cell);
  ADD_REF_PROP(last_group_id, int, m_last_group_id);
  ADD_REF_PROP(last_forager_type, int, m_last_forager_type);

  ADD_REF_PROP(total_gestation_count, int, m_gestation_count.GetTotal());

  // Collect all relevant action trigger counts
  for (int i = 0; i < m_mgr->EnvironmentActionTriggerAverageIDs().GetSize(); i++) {
    m_prop_map->Define(PropertyPtr(new FunctorProperty<double>(m_mgr->EnvironmentActionTriggerAverageIDs()[i], s_prop_desc_map, FunctorProperty<double>::GetFunctor(&m_task_counts[i], &Apto::Stat::Accumulator<int>::Mean))));
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
