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

#include "avida/private/systematics/GenotypeArbiter.h"
#include "cEntryHandle.h"

#include "cPhenotype.h"
#include "cStringList.h"
#include "cStringUtil.h"
#include "cWorld.h"
#include "tDictionary.h"


cBGGenotype::cBGGenotype(cBGGenotypeManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents)
  : cBioGroup(in_id)
  , m_mgr(mgr)
  , m_handle(NULL)
  , m_src(founder->GetUnitSource())
  , m_src_args(founder->GetUnitSourceArgs())
  , m_genome(founder->GetGenome())
  , m_name("001-no_name")
  , m_threshold(false)
  , m_active(true)
  , m_generation_born(founder->GetPhenotype().GetGeneration())
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
{
  AddActiveReference();
  if (parents) {
    m_parents.Resize(parents->GetSize());
    for (int i = 0; i < m_parents.GetSize(); i++) {
      m_parents[i] = static_cast<cBGGenotype*>((*parents)[i]);
      m_parents[i]->AddPassiveReference();
      if (i > 0) m_parent_str += ",";
      m_parent_str += cStringUtil::Convert(m_parents[i]->GetID());
    }
  }
  if (m_parents.GetSize()) m_depth = m_parents[0]->GetDepth() + 1;
  if (m_src != SRC_ORGANISM_FILE_LOAD) m_breed_in.Inc();
  m_name.Set("%03d-no_name", m_genome.GetSequence().GetSize());
}


cBGGenotype::cBGGenotype(cBGGenotypeManager* mgr, int in_id, const tDictionary<cString>& props, cWorld* world)
: cBioGroup(in_id)
, m_mgr(mgr)
, m_handle(NULL)
, m_name("001-no_name")
, m_threshold(false)
, m_active(false)
, m_active_offspring_genotypes(0)
, m_num_organisms(0)
, m_last_num_organisms(0)
, m_total_organisms(0)
{
  if (props.HasEntry("src")) {
    m_src = (eBioUnitSource)props.Get("src").AsInt();
  } else {
    m_src = SRC_ORGANISM_FILE_LOAD;
  }
  m_src_args = props.Get("src_args");
  if (m_src_args == "(none)") m_src_args = "";
  
  m_genome.Load(props, world->GetHardwareManager());
  
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
  cStringList parents(m_parent_str,',');

  m_parents.Resize(parents.GetSize());
  for (int i = 0; i < m_parents.GetSize(); i++) {
    m_parents[i] = static_cast<cBGGenotype*>(m_mgr->GetBioGroup(parents.Pop().AsInt()));
    assert(m_parents[i]);
    m_parents[i]->AddPassiveReference();
  }
}



cBGGenotype::~cBGGenotype()
{
  delete m_handle;
  m_parents.Resize(0);
}

int cBGGenotype::GetRoleID() const
{
  return m_mgr->GetRoleID();
}


const cString& cBGGenotype::GetRole() const
{
  return m_mgr->GetRole();
}


cBioGroup* cBGGenotype::ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents)
{
  m_births.Inc();
  
  if (Matches(bu)) {
    m_breed_true.Inc();
    m_total_organisms++;
    m_num_organisms++;
    m_mgr->AdjustGenotype(this, m_num_organisms - 1, m_num_organisms);
    AddActiveReference();
    return this;
  }  
  
  m_breed_out.Inc();
  return m_mgr->ClassifyNewBioUnit(bu, parents);
}

void cBGGenotype::HandleBioUnitGestation(cBioUnit* bu)
{
  const cPhenotype& phenotype = bu->GetPhenotype();
  
  m_copied_size.Add(phenotype.GetCopiedSize());
  m_exe_size.Add(phenotype.GetExecutedSize());
  m_gestation_time.Add(phenotype.GetGestationTime());
  m_repro_rate.Add(1.0 / phenotype.GetGestationTime());
  m_merit.Add(phenotype.GetMerit().GetDouble());
  m_fitness.Add(phenotype.GetFitness());
}


void cBGGenotype::RemoveBioUnit(cBioUnit* bu)
{
  m_deaths.Inc();
  
  // Remove active reference
  m_a_refs--;
  assert(m_a_refs >= 0);
  
  m_num_organisms--;
  m_mgr->AdjustGenotype(this, m_num_organisms + 1, m_num_organisms);
}

void cBGGenotype::RemoveActiveReference()
{
  m_a_refs--;
  assert(m_a_refs >= 0);
  
  if (!m_a_refs) m_mgr->AdjustGenotype(this, m_num_organisms, 0);
}


const tArray<cString>& cBGGenotype::GetProperyList() const { return m_mgr->GetBioGroupPropertyList(); }
bool cBGGenotype::HasProperty(const cString& prop) const { return m_mgr->BioGroupHasProperty(prop); }
cFlexVar cBGGenotype::GetProperty(const cString& prop) const { return m_mgr->GetBioGroupProperty(this, prop); }



void cBGGenotype::Save(cDataFile& df)
{
  
  df.Write(m_id, "ID", "id");
  df.Write(Avida::BioUnitSourceMap[m_src], "Source", "src");
  df.Write(m_src_args.GetSize() ? m_src_args : "(none)", "Source Args", "src_args");

  cString str("");
  if (m_parents.GetSize()) {
    str += cStringUtil::Stringf("%d", m_parents[0]->GetID());
    for (int i = 1; i < m_parents.GetSize(); i++) {
      str += cStringUtil::Stringf(",%d", m_parents[i]->GetID());
    }
  }
  df.Write((str.GetSize()) ? str : "(none)", "Parent ID(s)", "parents");
  
  df.Write(m_num_organisms, "Number of currently living organisms", "num_units");
  df.Write(m_total_organisms, "Total number of organisms that ever existed", "total_units");
  df.Write(m_genome.GetSequence().GetSize(), "Genome Length", "length");
  df.Write(m_merit.Average(), "Average Merit", "merit");
  df.Write(m_gestation_time.Average(), "Average Gestation Time", "gest_time");
  df.Write(m_fitness.Average(), "Average Fitness", "fitness");
  df.Write(m_generation_born, "Generation Born", "gen_born");
  df.Write(m_update_born, "Update Born", "update_born");
  df.Write(m_update_deactivated, "Update Deactivated", "update_deactivated");
  df.Write(m_depth, "Phylogenetic Depth", "depth");
  m_genome.Save(df);
}


void cBGGenotype::DepthSave(cDataFile& df)
{
  df.Write(m_id, "ID", "genotype_id");
  df.Write(m_num_organisms, "Number of currently living organisms", "num_units");
  df.Write(m_depth, "Phylogenetic Depth", "depth");
}

bool cBGGenotype::Matches(cBioUnit* bu)
{
  // Handle source branching
  switch (m_src) {
    case SRC_DEME_COMPETE:
    case SRC_DEME_COPY:
    case SRC_DEME_GERMLINE:
    case SRC_DEME_RANDOM:
    case SRC_DEME_REPLICATE:
    case SRC_DEME_SPAWN:
    case SRC_ORGANISM_COMPETE:
    case SRC_ORGANISM_DIVIDE:
    case SRC_ORGANISM_FILE_LOAD:
    case SRC_ORGANISM_RANDOM:
      switch (bu->GetUnitSource()) {
        case SRC_DEME_COMPETE:
        case SRC_DEME_COPY:
        case SRC_DEME_GERMLINE:
        case SRC_DEME_RANDOM:
        case SRC_DEME_REPLICATE:
        case SRC_DEME_SPAWN:
        case SRC_ORGANISM_COMPETE:
        case SRC_ORGANISM_DIVIDE:
        case SRC_ORGANISM_FILE_LOAD:
        case SRC_ORGANISM_RANDOM:
          break;
          
        case SRC_PARASITE_FILE_LOAD:
        case SRC_PARASITE_INJECT:
          return false;
          break;
          
        default:
          assert(false);
          break;          
      }
      break;
      
    case SRC_PARASITE_FILE_LOAD:
    case SRC_PARASITE_INJECT:
      switch (bu->GetUnitSource()) {
        case SRC_DEME_COMPETE:
        case SRC_DEME_COPY:
        case SRC_DEME_GERMLINE:
        case SRC_DEME_RANDOM:
        case SRC_DEME_REPLICATE:
        case SRC_DEME_SPAWN:
        case SRC_ORGANISM_COMPETE:
        case SRC_ORGANISM_DIVIDE:
        case SRC_ORGANISM_FILE_LOAD:
        case SRC_ORGANISM_RANDOM:
          return false;
          break;
          
        case SRC_PARASITE_FILE_LOAD:
        case SRC_PARASITE_INJECT:
          // Verify that the parasite inject label matches
          if (m_src_args != bu->GetUnitSourceArgs()) return false;
          break;
          
        default:
          assert(false);
          break;          
      }
      break;
      
    default:
      assert(false);
      break;
      
  }
  
  // Compare the genomes
  return (m_genome == bu->GetGenome());
}

void cBGGenotype::NotifyNewBioUnit(cBioUnit* bu)
{
  m_active = true;
  switch (bu->GetUnitSource()) {
    case SRC_DEME_COMPETE:
    case SRC_DEME_COPY:
    case SRC_DEME_GERMLINE:
    case SRC_DEME_RANDOM:
    case SRC_DEME_REPLICATE:
    case SRC_DEME_SPAWN:
    case SRC_ORGANISM_COMPETE:
    case SRC_ORGANISM_FILE_LOAD:
    case SRC_ORGANISM_RANDOM:
    case SRC_PARASITE_FILE_LOAD:
      break;
      
    case SRC_ORGANISM_DIVIDE:
    case SRC_PARASITE_INJECT:
      m_breed_in.Inc();
      break;
      
    default:
      break;          
  }
  m_total_organisms++;
  m_num_organisms++;
  m_mgr->AdjustGenotype(this, m_num_organisms - 1, m_num_organisms);
  AddActiveReference();
}


void cBGGenotype::UpdateReset()
{
  m_last_num_organisms = m_num_organisms;
  m_births.Next();
  m_deaths.Next();
  m_breed_out.Next();
  m_breed_true.Next();
  m_breed_in.Next();
}


void cBGGenotypeManager::buildDataCommandManager() const
{
  m_dcm = new tDataCommandManager<cBGGenotype>;
  
#define ADD_PROP(NAME, TYPE, GET, DESC) \
m_dcm->Add(NAME, new tDataEntryOfType<cBGGenotype, TYPE>(NAME, DESC, &cBGGenotype::GET));
  
  ADD_PROP("genome", cString (), GetGenomeString, "Genome");
  ADD_PROP("name", const cString& (), GetName, "Name");
  ADD_PROP("parents", const cString& (), GetParentString, "Parents");
  ADD_PROP("threshold", bool (), IsThreshold, "Threshold");  
  ADD_PROP("update_born", int (), GetUpdateBorn, "Update Born");
  ADD_PROP("fitness", double (), GetFitness, "Average Fitness");
  ADD_PROP("repro_rate", double (), GetReproRate, "Repro Rate");
  ADD_PROP("recent_births", int (), GetThisBirths, "Recent Births (during update)");
  ADD_PROP("recent_deaths", int (), GetThisDeaths, "Recent Deaths (during update)");
  ADD_PROP("recent_breed_true", int (), GetThisBreedTrue, "Recent Breed True (during update)");
  ADD_PROP("recent_breed_in", int (), GetThisBreedIn, "Recent Breed In (during update)");
  ADD_PROP("recent_breed_out", int (), GetThisBreedOut, "Recent Breed Out (during update)");
  ADD_PROP("total_organisms", int (), GetTotalOrganisms, "Total Organisms");
  ADD_PROP("last_births", int (), GetLastBirths, "Births (during last update)");
  ADD_PROP("last_breed_true", int (), GetLastBreedTrue, "Breed True (during last update)");
  ADD_PROP("last_breed_in", int (), GetLastBreedIn, "Breed In (during last update)");
  ADD_PROP("last_breed_out", int (), GetLastBreedOut, "Breed Out (during last update)");
  ADD_PROP("last_birth_cell", int (), GetLastBirthCell, "Last birth cell");
  ADD_PROP("last_group_id", int (), GetLastGroupID, "Last birth group");
  ADD_PROP("last_forager_type", int (), GetLastForagerType, "Last birth forager type");
}

