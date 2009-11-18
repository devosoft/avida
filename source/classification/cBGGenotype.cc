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


cBGGenotype::cBGGenotype(cBGGenotypeManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents)
  : m_mgr(mgr)
  , m_src(founder->GetUnitSource())
  , m_src_args(founder->GetUnitSourceArgs())
  , m_genome(founder->GetMetaGenome())
  , m_name("001-no_name")
  , m_threshold(false)
  , m_active(true)
  , m_id(in_id)
  , m_update_born(update)
  , m_update_deactivated(-1)
  , m_depth(0)
  , m_active_offspring_genotypes(0)
  , m_num_organisms(1)
  , m_last_num_organisms(0)
  , m_total_organisms(1)
{
  if (parents) {
    m_parents.Resize(parents->GetSize());
    for (int i = 0; i < m_parents.GetSize(); i++) {
      m_parents[i] = (*parents)[i];
      m_parents[i]->AddReference();
    }
  }
  if (m_parents.GetSize()) m_depth = m_parents[0]->GetDepth() + 1;
}

cBGGenotype::~cBGGenotype()
{
  for (int i = 0; i < m_parents.GetSize(); i++) m_parents[i]->RemoveReference();
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
    m_mgr->AdjustGenotype(this, m_num_organisms++, m_num_organisms);
    
    return this;
  }  
  
  m_breed_out.Inc();
  return m_mgr->ClassifyNewBioUnit(bu, parents);
}


void cBGGenotype::RemoveBioUnit(cBioUnit* bu)
{
  m_deaths.Inc();
  m_mgr->AdjustGenotype(this, m_num_organisms--, m_num_organisms);
}


bool cBGGenotype::Matches(cBioUnit* bu)
{
  // Handle source branching
  switch (m_src) {
    case SRC_DEME_GERMLINE:
    case SRC_DEME_REPLICATE:
    case SRC_ORGANISM_COMPETE:
    case SRC_ORGANISM_DIVIDE:
    case SRC_ORGANISM_FILE_LOAD:
    case SRC_ORGANISM_RANDOM:
      switch (bu->GetUnitSource()) {
        case SRC_DEME_GERMLINE:
        case SRC_DEME_REPLICATE:
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
        case SRC_DEME_GERMLINE:
        case SRC_DEME_REPLICATE:
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

void cBGGenotype::UpdateReset()
{
  m_last_num_organisms = m_num_organisms;
  m_births.Next();
  m_deaths.Next();
  m_breed_out.Next();
  m_breed_true.Next();
  m_breed_in.Next();
}

