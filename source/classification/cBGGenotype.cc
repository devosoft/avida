/*
 *  cBGGenotype.cc
 *  Avida
 *
 *  Created by David on 11/5/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cBGGenotype.h"

#include "cBGGenotypeManager.h"
#include "cDataFile.h"
#include "cPhenotype.h"
#include "cStringList.h"
#include "cStringUtil.h"
#include "tDictionary.h"


cBGGenotype::cBGGenotype(cBGGenotypeManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents)
  : m_mgr(mgr)
  , m_src(founder->GetUnitSource())
  , m_src_args(founder->GetUnitSourceArgs())
  , m_genome(founder->GetMetaGenome())
  , m_name("001-no_name")
  , m_threshold(false)
  , m_active(true)
  , m_id(in_id)
  , m_generation_born(founder->GetPhenotype().GetGeneration())
  , m_update_born(update)
  , m_update_deactivated(-1)
  , m_depth(0)
  , m_active_offspring_genotypes(0)
  , m_num_organisms(1)
  , m_last_num_organisms(0)
  , m_total_organisms(1)
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
}


cBGGenotype::cBGGenotype(cBGGenotypeManager* mgr, int in_id, const tDictionary<cString>& props)
: m_mgr(mgr)
, m_name("001-no_name")
, m_threshold(false)
, m_active(false)
, m_id(in_id)
, m_active_offspring_genotypes(0)
, m_num_organisms(1)
, m_last_num_organisms(0)
, m_total_organisms(1)
{
  assert(props.HasEntry("src"));  
  m_src = (eBioUnitSource)props.Get("src").AsInt();
  assert(props.HasEntry("src_args"));  
  m_src_args = props.Get("src_args");
  
  m_genome.Load(props);
  
  assert(props.HasEntry("gen_born"));
  m_generation_born = props.Get("gen_born").AsInt();
  assert(props.HasEntry("update_born"));
  m_update_born = props.Get("update_born").AsInt();
  assert(props.HasEntry("update_deactivated"));
  m_update_deactivated = props.Get("update_deactivated").AsInt();
  assert(props.HasEntry("depth"));  
  m_depth = props.Get("depth").AsInt();
  
  m_parent_str = props.Get("parents");
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
  m_merit.Add(phenotype.GetMerit().GetDouble());
  m_fitness.Add(phenotype.GetFitness());
}


void cBGGenotype::RemoveBioUnit(cBioUnit* bu)
{
  m_deaths.Inc();
  RemoveActiveReference();
  m_num_organisms--;
  m_mgr->AdjustGenotype(this, m_num_organisms + 1, m_num_organisms);
}


const tArray<cString>& cBGGenotype::GetProperyList() const { return m_mgr->GetBioGroupPropertyList(); }
bool cBGGenotype::HasProperty(const cString& prop) const { return m_mgr->BioGroupHasProperty(prop); }
cFlexVar cBGGenotype::GetProperty(const cString& prop) const { return m_mgr->GetBioGroupProperty(this, prop); }



void cBGGenotype::Save(cDataFile& df)
{
  
  df.Write(m_id, "ID", "id");
  df.Write(Avida::BioUnitSourceMap[m_src], "Source", "src");
  df.Write(m_src_args, "Source Args", "src_args");

  cString str("");
  if (m_parents.GetSize()) {
    str += cStringUtil::Stringf("%d", m_parents[0]->GetID());
    for (int i = 1; i < m_parents.GetSize(); i++) {
      str += cStringUtil::Stringf(",%d", m_parents[i]->GetID());
    }
  }
  df.Write(str, "Parent ID(s)", "parents");
  
  df.Write(m_num_organisms, "Number of currently living organisms", "num_units");
  df.Write(m_total_organisms, "Total number of organisms that ever existed", "total_units");
  df.Write(m_genome.GetGenome().GetSize(), "Genome Length", "length");
  df.Write(m_merit.Average(), "Average Merit", "merit");
  df.Write(m_gestation_time.Average(), "Average Gestation Time", "gest_time");
  df.Write(m_fitness.Average(), "Average Fitness", "fitness");
  df.Write(m_generation_born, "Generation Born", "gen_born");
  df.Write(m_update_born, "Update Born", "update_born");
  df.Write(m_update_deactivated, "Update Deactivated", "update_deactivated");
  df.Write(m_depth, "Phylogenetic Depth", "depth");
  m_genome.Save(df);
}



bool cBGGenotype::Matches(cBioUnit* bu)
{
  // Handle source branching
  switch (m_src) {
    case SRC_DEME_COMPETE:
    case SRC_DEME_COPY:
    case SRC_DEME_GERMLINE:
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
  return (m_genome == bu->GetMetaGenome());
}

void cBGGenotype::NotifyNewBioUnit(cBioUnit* bu)
{
  m_breed_in.Inc();
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

