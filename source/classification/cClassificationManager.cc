/*
 *  cClassificationManager.cc
 *  Avida
 *
 *  Created by David on 11/14/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cClassificationManager.h"

#include "cDataFile.h"
#include "cGenome.h"
#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cInjectGenotype.h"
#include "cLineage.h"
#include "cOrganism.h"
#include "cSpecies.h"
#include "cStats.h"
#include "cStringList.h"
#include "cTestCPU.h"
#include "cWorld.h"
#include "cWorldDriver.h"

using namespace std;

cClassificationManager::cClassificationManager(cWorld* world)
  : m_world(world)
  , m_genotype_ctl(new cGenotypeControl(world))
  , m_genotype_next_id(1)
  , m_genotype_dom_time(0)
  , m_genotype_prev_dom(-1)
  , m_species_ctl(new cSpeciesControl(world))
  , m_species_next_id(0)
  , m_inject_ctl(new cInjectGenotypeControl(world))
  , m_inject_next_id(1)
  , m_inject_dom_time(0)
  , m_inject_prev_dom(-1)
  , m_best_lineage(NULL)
  , m_max_fitness_lineage(NULL)
  , m_dominant_lineage(NULL)
  , m_lineage_next_id(0)
{
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) {
    m_genotype_count[i] = 0;
    m_inject_count[i] = 0;
  }
}

cClassificationManager::~cClassificationManager()
{
  delete m_genotype_ctl;
  delete m_species_ctl;
  delete m_inject_ctl;
}

void cClassificationManager::UpdateReset()
{
  cGenotype* best_genotype = GetBestGenotype();
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  m_species_ctl->Purge(m_world->GetStats());
  if (best_genotype && best_genotype->GetID() != m_genotype_prev_dom) {
    m_genotype_dom_time = 0;
    m_genotype_prev_dom = best_genotype->GetID();
  }
  else {
    m_genotype_dom_time++;
    if (m_genotype_dom_time == m_world->GetConfig().GENOTYPE_PRINT_DOM.Get()) {
      cString filename;
      filename.Set("archive/%s", static_cast<const char*>(best_genotype->GetName()));
      testcpu->PrintGenome(m_world->GetDefaultContext(), best_genotype->GetGenome(), 
                           filename, best_genotype, m_world->GetStats().GetUpdate());
    }
  }
  

  cInjectGenotype* best_inject_genotype = GetBestInjectGenotype();
  
  if (best_inject_genotype) {  // If there is a dominant parasite
    if (best_inject_genotype->GetID() != m_inject_prev_dom) {
      m_inject_dom_time = 0;
      m_inject_prev_dom = best_inject_genotype->GetID();
    } else {
      m_inject_dom_time++;
      if (m_inject_dom_time == m_world->GetConfig().GENOTYPE_PRINT_DOM.Get()) {
        cString filename;
        filename.Set("archive/%s", static_cast<const char*>(best_inject_genotype->GetName()));
        testcpu->PrintInjectGenome(m_world->GetDefaultContext(), best_inject_genotype, best_inject_genotype->GetGenome(), 
                               filename, m_world->GetStats().GetUpdate());
      }
    }
  }

  delete testcpu;
}

void cClassificationManager::AddGenotype(cGenotype* in_genotype, int list_num)
{
  assert( in_genotype != 0 );
  
  if (list_num < 0) list_num = FindCRC(in_genotype->GetGenome());
  
  m_active_genotypes[list_num].Push(in_genotype);
  m_genotype_ctl->Insert(*in_genotype);
  m_world->GetStats().AddGenotype();
  
  // Speciation... If we are creating a new genotype here, we must
  // initilize it to the species of its parent genotype.
  
  cSpecies* parent_species = NULL;
  if ( in_genotype->GetParentGenotype() != NULL) {
    parent_species = in_genotype->GetParentGenotype()->GetSpecies();
  }
  
  in_genotype->SetSpecies(parent_species);
  if (parent_species != NULL) parent_species->AddGenotype();
}


// Add a new genotype to the archive.  Take in parent genotypes to track
// phylogenies (the second genotype is in the case of sexual organisms)
cGenotype* cClassificationManager::GetGenotype(const cGenome& in_genome, cGenotype* parent1, cGenotype* parent2)
{
  // Make sure we dont have a NULL parent -- if we do, we should be calling
  // GetGenotypeInjected().
  // @DMB - cPopulation::ActivateOrganism calls this with NULL,NULL...   not sure if this is correct.
  //assert (parent1 != NULL);
  
  int list_num = FindCRC(in_genome);
  cGenotype* found_genotype = FindGenotype(in_genome, parent1->GetLineageLabel(), list_num);
  
  if (!found_genotype) {
    found_genotype = new cGenotype(m_world, m_world->GetStats().GetUpdate(), m_genotype_next_id++);
    found_genotype->SetGenome(in_genome);
    found_genotype->SetParent(parent1, parent2);    
    AddGenotype(found_genotype, list_num);
  }
  
  return found_genotype;
}


// Add a new genotype that has been injected from the outside to the archive.
//  Take in a lineage label to track clades.
cGenotype* cClassificationManager::GetGenotypeInjected(const cGenome& in_genome, int lineage_label)
{
  int list_num = FindCRC(in_genome);
  cGenotype* found_genotype = FindGenotype(in_genome, lineage_label, list_num);
  
  if (!found_genotype) {
    found_genotype = new cGenotype(m_world, m_world->GetStats().GetUpdate(), m_genotype_next_id++);
    found_genotype->SetGenome(in_genome);
    found_genotype->SetParent(NULL, NULL);
    found_genotype->SetLineageLabel(lineage_label);
    AddGenotype(found_genotype, list_num);
  }
  
  return found_genotype;
}

// Add a new genotype that has been injected from the outside to the archive.
//  Take in a lineage label to track clades.
cGenotype* cClassificationManager::GetGenotypeLoaded(const cGenome& in_genome, int update_born, int id_num)
{
  if (id_num >= 0) m_genotype_next_id = id_num;

  cGenotype* genotype = new cGenotype(m_world, update_born, m_genotype_next_id++);
  genotype->SetGenome(in_genome);
  genotype->SetParent(NULL, NULL);
  AddGenotype(genotype);
  
  return genotype;
}

const cGenotype* cClassificationManager::FindGenotype(const cGenome & in_genome,
                                          int lineage_label, int list_num) const
{
  if (list_num < 0) list_num = FindCRC(in_genome);
  
  tConstListIterator<cGenotype> list_it(m_active_genotypes[list_num]);
  
  // If the lineage label is -1, match any lineage...
  if (lineage_label == -1) {
    while (list_it.Next() != NULL) {
      if (list_it.GetConst()->GetGenome() == in_genome) break;
    }
  }
  
  // Otherwise, only match a genotype of the same lineage.
  else {
    while (list_it.Next() != NULL) {
      if (list_it.GetConst()->GetGenome() == in_genome &&
          list_it.GetConst()->GetLineageLabel() == lineage_label) break;
    }
  }
  
  return list_it.GetConst();
}

cGenotype* cClassificationManager::FindGenotype(const cGenome & in_genome,
                                    int lineage_label, int list_num)
{
  if (list_num < 0) list_num = FindCRC(in_genome);
  
  tListIterator<cGenotype> list_it(m_active_genotypes[list_num]);
  // If the lineage label is -1, match any lineage...
  if (lineage_label == -1) {
    while (list_it.Next() != NULL) {
      if (list_it.Get()->GetGenome() == in_genome) break;
    }
  }
  
  // Otherwise, only match a genotype of the same lineage.
  else {
    while (list_it.Next() != NULL) {
      if (list_it.Get()->GetGenome() == in_genome &&
          list_it.Get()->GetLineageLabel() == lineage_label) break;
    }
  }
  return list_it.Get();
}

void cClassificationManager::RemoveGenotype(cGenotype & in_genotype)
{
  // If we are supposed to defer analysis of this genotype, do so...
  if (in_genotype.GetDeferAdjust() == true) return;
  
  // If this genotype is still active, mark it no longer active and
  // take it out of the hash table so it doesn't have any new organisms
  // assigned to it.
  
  if (in_genotype.GetActive() == true) {
    int list_num = FindCRC(in_genotype.GetGenome());
    m_active_genotypes[list_num].Remove(&in_genotype);
    m_genotype_ctl->Remove(in_genotype);
    in_genotype.Deactivate(m_world->GetStats().GetUpdate());
    if (m_world->GetConfig().TRACK_MAIN_LINEAGE.Get()) {
      m_genotype_ctl->InsertHistoric(in_genotype);
    }
  }
  
  // If we are tracking the main lineage, we only want to delete a
  // genotype when all of its decendents have also died out.
  
  const int lineage_type = m_world->GetConfig().TRACK_MAIN_LINEAGE.Get();
  if (lineage_type > 0) {
    // If  there are more offspring genotypes, hold off on deletion...
    if (in_genotype.GetNumOffspringGenotypes() != 0) return;
    
    // If this is a dead end, delete it and recurse up...
    cGenotype * parent = in_genotype.GetParentGenotype();
    cGenotype * parent2 = in_genotype.GetParent2Genotype();
    
    if (parent != NULL) {
      parent->RemoveOffspringGenotype();
      
      // Test to see if we need to update the coalescent genotype.
      const int new_coal = m_genotype_ctl->UpdateCoalescent();
      m_world->GetStats().SetCoalescentGenotypeDepth(new_coal);
      
      if (parent->GetNumOrganisms() == 0) {
        // Regardless, run RemoveGenotype on the parent.
        RemoveGenotype(*parent);
      }
    }
    
    if (lineage_type == 2 && parent2 != NULL) {
      parent2->RemoveOffspringGenotype();
      
      // Test to see if we need to update the coalescent genotype.
      const int new_coal = m_genotype_ctl->UpdateCoalescent();
      m_world->GetStats().SetCoalescentGenotypeDepth(new_coal);
      
      if (parent2->GetNumOrganisms() == 0) {
        // Regardless, run RemoveGenotype on the parent.
        RemoveGenotype(*parent2);
      }
    }
    
    m_genotype_ctl->RemoveHistoric(in_genotype);
  }
  
  // Handle the relevent statistics...
  m_world->GetStats().RemoveGenotype(in_genotype.GetID(),
                                     in_genotype.GetParentID(), in_genotype.GetParentDistance(),
                                     in_genotype.GetDepth(), in_genotype.GetTotalOrganisms(),
                                     in_genotype.GetTotalParasites(),
                                     m_world->GetStats().GetUpdate() - in_genotype.GetUpdateBorn(),
                                     in_genotype.GetLength());
  if (in_genotype.GetThreshold()) {
    m_world->GetStats().RemoveThreshold();
  }
  
  
  // Speciation...  If a Threshold genotype was removed, the position of this
  // species in the active list will at least shift, and it is possible that
  // the species is made inactive, or removed all-togeather.  If it is a non-
  // threshold genotype, then the species will only be effected if this was
  // the last genotype of that species.
  
  cSpecies * cur_species = in_genotype.GetSpecies();
  if (cur_species) {
    
    // First, re-adjust the species.
    
    cur_species->RemoveGenotype();
    
    // Then, check to see how this species changes if it is a threshold.
    
    if (in_genotype.GetThreshold()) {
      cur_species->RemoveThreshold(in_genotype);
      
      // If we are out of thresholds, move this species to the inactive
      // list for now.  Otherwise, just adjust it.
      
      if (cur_species->GetNumThreshold() == 0) {
        m_species_ctl->SetInactive(*cur_species);
      }
      else {
        m_species_ctl->Adjust(*cur_species);
      }
    }
    
    // Finally, remove the species completely if it has no genotypes left.
    
    if (!cur_species->GetNumGenotypes()) {
      m_species_ctl->SetGarbage(*cur_species);
    }
  }
  
  
  delete &in_genotype;
}

void cClassificationManager::ThresholdGenotype(cGenotype & in_genotype)
{
  cSpecies * found_species = NULL;
  
  in_genotype.SetName( GetLabel(in_genotype.GetLength(), m_genotype_count[in_genotype.GetLength()]++) );
  in_genotype.SetThreshold();
  
  // If speciation is on, assign a species to the genotype now that it is
  // threshold.
  
  if (m_world->GetConfig().SPECIES_RECORDING.Get()) {
    // Record the old species to know if it changes.
    cSpecies * old_species = in_genotype.GetSpecies();
    
    // Determine the "proper" species.
    found_species = m_species_ctl->Find(in_genotype,
                                          m_world->GetConfig().SPECIES_RECORDING.Get());
    
    // If no species was found, create a new one.
    if (!found_species) {
      found_species = new cSpecies(m_world, in_genotype.GetGenome(), m_world->GetStats().GetUpdate(), m_species_next_id++);
      if (in_genotype.GetSpecies())
        found_species->SetParentID(in_genotype.GetSpecies()->GetID());
      m_species_ctl->SetActive(*found_species);
      m_world->GetStats().AddSpecies();
      
      // Since this is a new species, see if we should be printing it.
      
      if (m_world->GetConfig().SPECIES_PRINT.Get()) {
        cString filename;
        filename.Set("archive/spec-%04d", found_species->GetID());
        cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
        testcpu->PrintGenome(m_world->GetDefaultContext(), in_genotype.GetGenome(), filename,
                             &in_genotype, m_world->GetStats().GetUpdate());
        delete testcpu;
      }
    } else {
      // If we are not creating a new species, but are adding a threshold
      // to one which is currently in-active, make sure to move it back to
      // the active list.
      
      if (found_species->GetNumThreshold() == 0) {
        m_species_ctl->SetActive(*found_species);
      }
    }
    
    // Now that we know for sure what the species is, and that it is in
    // the proper list, setup both the species and the genotype.
    
    in_genotype.SetSpecies(found_species);
    found_species->AddThreshold(in_genotype);
    
    // Finally test to see if the species has been changed, and adjust
    // accordingly.
    
    if (found_species != old_species) {
      found_species->AddGenotype();
      if (old_species) {
        old_species->RemoveGenotype();
        if (old_species->GetNumGenotypes() == 0)
          m_species_ctl->SetGarbage(*old_species);
      }
    }
    else {
      if (found_species->GetNumThreshold() > 1) {
        m_species_ctl->Adjust(*found_species);
      }
    }
  }
  
  // Do the relevent statistics...
  if (m_world->GetConfig().SPECIES_RECORDING.Get()) {
    m_world->GetStats().AddThreshold(in_genotype.GetID(), in_genotype.GetName(),
                                     found_species->GetID());
  } else {
    m_world->GetStats().AddThreshold(in_genotype.GetID(), in_genotype.GetName());
  }
  
  // Print the genotype?
  if (m_world->GetConfig().GENOTYPE_PRINT.Get()) {
    cString filename;
    filename.Set("archive/%s", static_cast<const char*>(in_genotype.GetName()));
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    testcpu->PrintGenome(m_world->GetDefaultContext(), in_genotype.GetGenome(), filename,
                         &in_genotype, m_world->GetStats().GetUpdate());
    delete testcpu;
  }
}

bool cClassificationManager::AdjustGenotype(cGenotype & in_genotype)
{
  if (!m_genotype_ctl->Adjust(in_genotype)) return false;
  
  if ((in_genotype.GetNumOrganisms() >= m_world->GetConfig().THRESHOLD.Get() ||
       &in_genotype == m_genotype_ctl->GetBest()) &&
      !(in_genotype.GetThreshold())) {
    ThresholdGenotype(in_genotype);
  }
  
  return true;
}

bool cClassificationManager::SaveClone(ofstream& fp)
{
  // This method just save the counts at each size-class of genotypes.
  // The rest is reconstructable.
  
  // Save the numbers of organisms we're up to at each size.
  fp << MAX_CREATURE_SIZE << " ";
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) {
    fp << m_genotype_count[i] << " ";
  }
  
  // Save the numbers of organisms we're up to at each size.
  fp << MAX_CREATURE_SIZE << " ";
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) {
    fp << m_inject_count[i] << " ";
  }
  return true;
}

bool cClassificationManager::LoadClone(ifstream & fp)
{
  // This method just restores the counts at each size-class of genotypes.
  // The rest of the loading process should be handled elsewhere.
  
  // Load the numbers of organisms we're up to at each size.
  int max_size;
  fp >> max_size;
  assert (max_size <= MAX_CREATURE_SIZE); // MAX_CREATURE_SIZE too small
  for (int i = 0; i < max_size && i < MAX_CREATURE_SIZE; i++) {
    fp >> m_genotype_count[i];
  }
  
  fp >> max_size;
  assert (max_size <= MAX_CREATURE_SIZE); // MAX_CREATURE_SIZE too small
  for (int i = 0; i < max_size && i < MAX_CREATURE_SIZE; i++) {
    fp >> m_inject_count[i];
  }

  return true;
}

bool cClassificationManager::DumpTextSummary(ofstream& fp)
{
  m_genotype_ctl->Reset(0);
  for (int i = 0; i < m_genotype_ctl->GetSize(); i++) {
    cGenotype * genotype = m_genotype_ctl->Get(0);
    fp << genotype->GetGenome().AsString() << " "
      << genotype->GetNumOrganisms() << " "
      << genotype->GetID() << endl;
    m_genotype_ctl->Next(0);
  }
  
  return true;
}

bool cClassificationManager::PrintGenotypes(ofstream& fp, cString & data_fields,
                               int historic)
{
  bool print_id = false;
  bool print_parent_id = false;
  bool print_parent2_id = false;
  bool print_parent_dist = false;
  bool print_num_cpus = false;
  bool print_total_cpus = false;
  bool print_length = false;
  bool print_merit = false;
  bool print_gest_time = false;
  bool print_fitness = false;
  bool print_update_born = false;
  bool print_update_dead = false;
  bool print_depth = false;
  bool print_lineage = false;
  bool print_sequence = false;
  
  cStringList fields(data_fields, ',');
  if (fields.HasString("id") == true) print_id = true;
  if (fields.HasString("parent_id") == true) print_parent_id = true;
  if (fields.HasString("parent2_id") == true) print_parent2_id = true;
  if (fields.HasString("parent_dist") == true) print_parent_dist = true;
  if (fields.HasString("num_cpus") == true) print_num_cpus = true;
  if (fields.HasString("total_cpus") == true) print_total_cpus = true;
  if (fields.HasString("length") == true) print_length = true;
  if (fields.HasString("merit") == true) print_merit = true;
  if (fields.HasString("gest_time") == true) print_gest_time = true;
  if (fields.HasString("fitness") == true) print_fitness = true;
  if (fields.HasString("update_born") == true) print_update_born = true;
  if (fields.HasString("update_dead") == true) print_update_dead = true;
  if (fields.HasString("depth") == true) print_depth = true;
  if (fields.HasString("lineage") == true) print_lineage = true;
  if (fields.HasString("sequence") == true) print_sequence = true;
  if (fields.HasString("all") == true) {
    print_id = print_parent_id = print_parent2_id = print_parent_dist = true;
    print_num_cpus = print_total_cpus = print_length = print_merit = true;
    print_gest_time = print_fitness = print_update_born = true;
    print_update_dead = print_depth = print_lineage = print_sequence = true;
  }
  
  // Print all of the header information...
  fp << "#filetype genotype_data" << endl
    << "#format ";
  
  if (print_id == true) fp << "id ";
  if (print_parent_id == true) fp << "parent_id ";
  if (print_parent2_id == true) fp << "parent2_id ";
  if (print_parent_dist == true) fp << "parent_dist ";
  if (print_num_cpus == true) fp << "num_cpus ";
  if (print_total_cpus == true) fp << "total_cpus ";
  if (print_length == true) fp << "length ";
  if (print_merit == true) fp << "merit ";
  if (print_gest_time == true) fp << "gest_time ";
  if (print_fitness == true) fp << "fitness ";
  if (print_update_born == true) fp << "update_born ";
  if (print_update_dead == true) fp << "update_dead ";
  if (print_depth == true) fp << "depth ";
  if (print_lineage == true) fp << "lineage ";
  if (print_sequence == true) fp << "sequence ";  
  fp << endl;
  
  // Print extra information about what data is in this file...
  fp << "# Output is genotypes at update " << m_world->GetStats().GetUpdate();
  if (historic == -1) fp << " including ALL ancestors";
  else if (historic > 0) fp << " including ancestors for " 
    << historic << " updates.";
  fp << endl;
  
  // And give some information about what columns are available.
  int cur_col = 1;
  if (print_id) fp << "# " << cur_col++ << ": ID" << endl;
  if (print_parent_id) fp << "# " << cur_col++ << ": parent ID" << endl;
  if (print_parent2_id) fp << "# " << cur_col++ << ": parent2 ID" << endl;
  if (print_parent_dist) fp << "# " << cur_col++ << ": parent distance" << endl;
  if (print_num_cpus) fp << "# " << cur_col++ << ": number of orgranisms currently alive" << endl;
  if (print_total_cpus) fp << "# " << cur_col++ << ": total number of organisms that ever existed" << endl;
  if (print_length) fp << "# " << cur_col++ << ": length of genome" << endl;
  if (print_merit) fp << "# " << cur_col++ << ": merit" << endl;
  if (print_gest_time) fp << "# " << cur_col++ << ": gestation time" << endl;
  if (print_fitness) fp << "# " << cur_col++ << ": fitness" << endl;
  if (print_update_born) fp << "# " << cur_col++ << ": update born" << endl;
  if (print_update_dead) fp << "# " << cur_col++ << ": update deactivated" << endl;
  if (print_depth) fp << "# " << cur_col++ << ": depth in phylogentic tree" << endl;
  if (print_lineage) fp << "# " << cur_col++ << ": lineage label of genotype" << endl;
  if (print_sequence) fp << "# " << cur_col++ << ": genome of genotype" << endl;
  fp << endl;
  
  // Print the current population....
  m_genotype_ctl->Reset(0);
  for (int i = 0; i < m_genotype_ctl->GetSize(); i++) {
    cGenotype * genotype = m_genotype_ctl->Get(0);
    
    if (print_id)          fp << genotype->GetID() << " ";
    if (print_parent_id)   fp << genotype->GetParentID() << " ";
    if (print_parent2_id)  fp << genotype->GetAncestorID(1) << " ";
    if (print_parent_dist) fp << genotype->GetParentDistance() << " ";
    if (print_num_cpus)    fp << genotype->GetNumOrganisms() << " ";
    if (print_total_cpus)  fp << genotype->GetTotalOrganisms() << " ";
    if (print_length)      fp << genotype->GetLength() << " ";
    if (print_merit)       fp << genotype->GetMerit() << " ";
    if (print_gest_time)   fp << genotype->GetGestationTime() << " ";
    if (print_fitness)     fp << genotype->GetFitness() << " ";
    if (print_update_born) fp << genotype->GetUpdateBorn() << " ";
    if (print_update_dead) fp << genotype->GetUpdateDeactivated() << " ";
    if (print_depth)       fp << genotype->GetDepth() << " ";
    if (print_lineage)     fp << genotype->GetLineageLabel() << " "; 
    if (print_sequence)    fp << genotype->GetGenome().AsString() << " ";
    fp << endl;
    m_genotype_ctl->Next(0);
  }
  
  // Print the historic population if we are supposed to.
  if (historic == 0) return true;
  
  // Start by calculating the update we should start printing from...
  int start_update = 0;
  if (historic > 0) start_update = m_world->GetStats().GetUpdate() - historic;
  
  // Now loop through the remaining genotypes...
  m_genotype_ctl->ResetHistoric(0);
  for (int i = 0; i < m_genotype_ctl->GetHistoricCount(); i++) {
    // Get the next genotype.  Only print it if its in range...
    cGenotype * genotype = m_genotype_ctl->Get(0);
    if (genotype->GetUpdateDeactivated() < start_update) {
      m_genotype_ctl->Next(0);
      continue;
    }
    
    if (print_id)          fp << genotype->GetID() << " ";
    if (print_parent_id)   fp << genotype->GetParentID() << " ";
    if (print_parent2_id)  fp << genotype->GetAncestorID(1) << " ";
    if (print_parent_dist) fp << genotype->GetParentDistance() << " ";
    if (print_num_cpus)    fp << genotype->GetNumOrganisms() << " ";
    if (print_total_cpus)  fp << genotype->GetTotalOrganisms() << " ";
    if (print_length)      fp << genotype->GetLength() << " ";
    if (print_merit)       fp << genotype->GetMerit() << " ";
    if (print_gest_time)   fp << genotype->GetGestationTime() << " ";
    if (print_fitness)     fp << genotype->GetFitness() << " ";
    if (print_update_born) fp << genotype->GetUpdateBorn() << " ";
    if (print_update_dead) fp << genotype->GetUpdateDeactivated() << " ";
    if (print_depth)       fp << genotype->GetDepth() << " ";
    if (print_lineage)     fp << genotype->GetLineageLabel() << " "; 
    if (print_sequence)    fp << genotype->GetGenome().AsString() << " ";
    fp << endl;
    
    // Move to the next genotype...
    m_genotype_ctl->Next(0);
  }
  
  
  return true;
}

bool cClassificationManager::DumpDetailedSummary(ofstream& fp)
{
  m_genotype_ctl->Reset(0);
  DumpDetailHeading(fp);
  for (int i = 0; i < m_genotype_ctl->GetSize(); i++) {
    DumpDetailedEntry(m_genotype_ctl->Get(0), fp);
    m_genotype_ctl->Next(0);
  }
  
  return true;
}

bool cClassificationManager::DumpHistoricSummary(ofstream& fp, int back_dist)
{
  // Calculate the update we should start printing from...
  int start_update = 0;
  if (back_dist > 0) start_update = m_world->GetStats().GetUpdate() - back_dist;
  
  // Loop through all defunct genotypes that we're saving.
  DumpDetailHeading(fp);
  m_genotype_ctl->ResetHistoric(0);
  for (int i = 0; i < m_genotype_ctl->GetHistoricCount(); i++) {
    // Get the next genotype.  Only print it if its in range...
    cGenotype * cur_genotype = m_genotype_ctl->Get(0);
    if (cur_genotype->GetUpdateDeactivated() < start_update) {
      m_genotype_ctl->Next(0);
      continue;
    }
    DumpDetailedEntry(cur_genotype, fp);
    
    // Move to the next genotype...
    m_genotype_ctl->Next(0);
  }
  
  return true;
}

bool cClassificationManager::DumpDetailedSexSummary(ofstream& fp)
{
  m_genotype_ctl->Reset(0);
  DumpDetailSexHeading(fp);
  for (int i = 0; i < m_genotype_ctl->GetSize(); i++) {
    DumpDetailedSexEntry(m_genotype_ctl->Get(0), fp);
    m_genotype_ctl->Next(0);
  }
  
  return true;
}

bool cClassificationManager::DumpHistoricSexSummary(ofstream& fp)
{
  m_genotype_ctl->ResetHistoric(0);
  DumpDetailSexHeading(fp);
  for (int i = 0; i < m_genotype_ctl->GetHistoricCount(); i++) {
    DumpDetailedSexEntry(m_genotype_ctl->Get(0), fp);
    m_genotype_ctl->Next(0);
  }
  
  return true;
}

void cClassificationManager::DumpDetailHeading (ofstream& fp)
{
  fp << "#filetype genotype_data" << endl
  << "#format id parent_id parent_dist num_cpus total_cpus length merit gest_time fitness update_born update_dead depth sequence" << endl
  << endl
  << "#  1: ID" << endl
  << "#  2: parent ID" << endl
  << "#  3: parent distance" << endl
  << "#  4: number of organisms currently alive" << endl
  << "#  5: total number of organisms that ever existed" << endl
  << "#  6: length of genome" << endl
  << "#  7: merit" << endl
  << "#  8: gestation time" << endl
  << "#  9: fitness" << endl
  << "# 10: update born" << endl
  << "# 11: update deactivated" << endl
  << "# 12: depth in phylogentic tree" << endl
  << "# 13: genome of organism" << endl << endl;
}

void cClassificationManager::DumpDetailSexHeading (ofstream& fp)
{
  fp << "#filetype genotype_data" << endl
  << "#format id parent_id parent2_id parent_dist num_cpus total_cpus length merit gest_time fitness update_born update_dead depth sequence" << endl
  << endl
  << "#  1: ID" << endl
  << "#  2: parent 1 ID " << endl
  << "#  3: parent 2 ID" << endl
  << "#  4: parent 1 distance" << endl
  << "#  5: number of orgranisms currently alive" << endl
  << "#  6: total number of organisms that ever existed" << endl
  << "#  7: length of genome" << endl
  << "#  8: merit" << endl
  << "#  9: gestation time" << endl
  << "# 10: fitness" << endl
  << "# 11: update born" << endl
  << "# 12: update deactivated" << endl
  << "# 13: depth in phylogentic tree" << endl
  << "# 14: genome of organism" << endl << endl;
}

void cClassificationManager::DumpDetailedEntry(cGenotype* genotype, ofstream& fp)
{
  fp << genotype->GetID() << " "                //  1
  << genotype->GetParentID() << " "          //  2
                                             // << genotype->GetAncestorID(1) << " "          //  2b
  << genotype->GetParentDistance() << " "    //  3
  << genotype->GetNumOrganisms() << " "      //  4
  << genotype->GetTotalOrganisms() << " "    //  5
  << genotype->GetLength() << " "            //  6
  << genotype->GetMerit() << " "             //  7
  << genotype->GetGestationTime() << " "     //  8
  << genotype->GetFitness() << " "           //  9
  << genotype->GetUpdateBorn() << " "        // 10
  << genotype->GetUpdateDeactivated() << " " // 11
  << genotype->GetDepth() << " "             // 12
  << genotype->GetGenome().AsString() << " " // 13
  << endl;
}

void cClassificationManager::DumpDetailedSexEntry(cGenotype * genotype, ofstream& fp)
{
  fp << genotype->GetID() << " "                //  1
  << genotype->GetAncestorID(0) << " "       //  2
  << genotype->GetAncestorID(1) << " "       //  3
  << genotype->GetParentDistance() << " "    //  4
  << genotype->GetNumOrganisms() << " "      //  5
  << genotype->GetTotalOrganisms() << " "    //  6
  << genotype->GetLength() << " "            //  7
  << genotype->GetMerit() << " "             //  8
  << genotype->GetGestationTime() << " "     //  9
  << genotype->GetFitness() << " "           // 10
  << genotype->GetUpdateBorn() << " "        // 11
  << genotype->GetUpdateDeactivated() << " " // 12
  << genotype->GetDepth() << " "             // 13
  << genotype->GetGenome().AsString() << " " // 14
  << endl;
}



void cClassificationManager::AddInjectGenotype(cInjectGenotype* in_inject_genotype, int in_list_num)
{
  assert( in_inject_genotype != 0 );
  
  if ( in_list_num < 0 )
    in_list_num = FindCRC(in_inject_genotype->GetGenome()) % nInjectGenotype::HASH_SIZE;
  
  m_active_inject[in_list_num].Insert(*in_inject_genotype);
  m_inject_ctl->Insert(*in_inject_genotype);
}


cInjectGenotype* cClassificationManager::GetInjectGenotype(const cGenome& in_genome, cInjectGenotype* parent_genotype)
{
  int list_num = FindCRC(in_genome) % nInjectGenotype::HASH_SIZE;
  cInjectGenotype * found_genotype;
  
  found_genotype = m_active_inject[list_num].Find(in_genome);
  
  if (!found_genotype) {
    found_genotype = new cInjectGenotype(m_world, m_world->GetStats().GetUpdate(), m_inject_next_id++);
    found_genotype->SetGenome(in_genome);
    found_genotype->SetParent(parent_genotype);
    if(parent_genotype!=NULL)
    {
      parent_genotype->SetCanReproduce();
    }
    AddInjectGenotype( found_genotype, list_num );
  }
  return found_genotype;
}

cInjectGenotype* cClassificationManager::FindInjectGenotype(const cGenome& in_genome) const
{
  int list_num = FindCRC(in_genome) % nInjectGenotype::HASH_SIZE;
  return m_active_inject[list_num].Find(in_genome);
}

void cClassificationManager::RemoveInjectGenotype(cInjectGenotype& in_inject_genotype)
{
  // If this genotype is still active, mark it no longer active and
  // take it out of the hash table so it doesn't have any new organisms
  // assigned to it.
  
  if (in_inject_genotype.GetActive() == true) {
    int list_num = FindCRC(in_inject_genotype.GetGenome()) % nInjectGenotype::HASH_SIZE;
    m_active_inject[list_num].Remove(in_inject_genotype);
    m_inject_ctl->Remove(in_inject_genotype);
    //in_inject_genotype.Deactivate(stats.GetUpdate());
    if (m_world->GetConfig().TRACK_MAIN_LINEAGE.Get()) {
      m_inject_ctl->InsertHistoric(in_inject_genotype);
    }
  }
  
  delete &in_inject_genotype;
}

void cClassificationManager::ThresholdInjectGenotype(cInjectGenotype& in_inject_genotype)
{
  in_inject_genotype.SetName( GetLabel(in_inject_genotype.GetLength(),
                                       m_inject_count[in_inject_genotype.GetLength()]++) );
  in_inject_genotype.SetThreshold();
}

bool cClassificationManager::AdjustInjectGenotype(cInjectGenotype& in_inject_genotype)
{
  if (!m_inject_ctl->Adjust(in_inject_genotype)) return false;
  
  if ((in_inject_genotype.GetNumInjected() >= m_world->GetConfig().THRESHOLD.Get() ||
       &in_inject_genotype == m_inject_ctl->GetBest()) &&
      !(in_inject_genotype.GetThreshold())) {
    ThresholdInjectGenotype(in_inject_genotype);
  }
  
  return true;
}

bool cClassificationManager::DumpInjectTextSummary(ofstream& fp)
{
  m_inject_ctl->Reset(0);
  for (int i = 0; i < m_inject_ctl->GetSize(); i++) {
    cInjectGenotype * genotype = m_inject_ctl->Get(0);
    fp << genotype->GetGenome().AsString() << " "
      << genotype->GetNumInjected() << " "
      << genotype->GetID() << endl;
    m_inject_ctl->Next(0);
  }
  
  return true;
}

bool cClassificationManager::DumpInjectDetailedSummary(const cString & file, int update)
{
  m_inject_ctl->Reset(0);
  for (int i = 0; i < m_inject_ctl->GetSize(); i++) {
    DumpInjectDetailedEntry(m_inject_ctl->Get(0), file, update);
    m_inject_ctl->Next(0);
  }
  return true;
}

void cClassificationManager::DumpInjectDetailedEntry(cInjectGenotype * genotype, const cString & filename, int update)
{
  cDataFile & df = m_world->GetDataFile(filename);
  
  df.WriteComment( "Avida parasite dump data" );
  df.WriteTimeStamp();
  
  df.Write( genotype->GetID(),                 "parasite genotype ID");
  df.Write( genotype->GetName(),              "parasite genotype name");
  df.Write( genotype->GetParentID(),           "parasite parent ID");
  df.Write( genotype->GetNumInjected(),        "current number of injected creatures with this genotype");
  df.Write( genotype->GetTotalInjected(),      "total number of injected creatures with this genotype");
  df.Write( genotype->GetLength(),             "genotype length");
  df.Write( genotype->GetUpdateBorn(),         "update this genotype was born");
  df.Write( genotype->CanReproduce(),          "has this genotype reproduced?");
  df.Write( genotype->GetGenome().AsString(),  "genome of this genotype");
  df.Endl();
}


cLineage* cClassificationManager::AddLineage(double start_fitness, int parent_lin_id, int id, double lineage_stat1, double lineage_stat2)
{
  if (id < 0) id = m_lineage_next_id++;
  if (id >= m_lineage_next_id) m_lineage_next_id = id + 1;
  
  cLineage* new_lineage = new cLineage(start_fitness, parent_lin_id, id,
                                       m_world->GetStats().GetUpdate(), m_world->GetStats().GetGeneration(), lineage_stat1, lineage_stat2);

  // the best/ dominant lineage are automatically corrected
  // when a creature is added to this lineage
  m_lineage_list.push_back(new_lineage);  
  m_world->GetStats().AddLineage();

  return new_lineage;
}


void cClassificationManager::UpdateLineages()
{
  m_best_lineage = NULL;
  m_dominant_lineage = NULL;
  m_max_fitness_lineage = NULL;
  
  list<cLineage *>::iterator it = m_lineage_list.begin();
  list<cLineage *>::iterator del_it;
  
  while( it != m_lineage_list.end() ){
    bool del = false;
    
    // mark the lineage for removal if empty
    if ( (*it)->GetNumCreatures() == 0 ){
      del_it = it;
      del = true;
    }
    else { // otherwise it is a candidate for the best/ dominant/... lineage
      cAvidaContext& ctx = m_world->GetDefaultContext();
      if (!m_best_lineage || (*it)->GetAveFitness(ctx) > m_best_lineage->GetAveFitness(ctx))
        m_best_lineage = (*it);
      
      if ( !m_dominant_lineage ||
           (*it)->GetNumCreatures() > m_dominant_lineage->GetNumCreatures() )
        m_dominant_lineage = (*it);
      
      if ( !m_max_fitness_lineage ||
           (*it)->GetMaxFitness() > m_max_fitness_lineage->GetMaxFitness() )
        m_max_fitness_lineage = (*it);
      
    }
    // proceed to the next lineage
    it++;
    
    // now do the removal if necessary
    if ( del ){
      delete (*del_it); // delete the lineage
      m_lineage_list.erase( del_it ); // and remove its reference
    }
  }
  
#ifdef DEBUG
  if ( !m_lineage_list.empty() ){
    assert( m_dominant_lineage != 0 );
    assert( m_best_lineage != 0 );
    assert( m_max_fitness_lineage != 0 );
  }
#endif
}


cLineage* cClassificationManager::GetLineage(cAvidaContext& ctx, cGenotype* child_genotype,
                                             cGenotype* parent_genotype, cLineage* parent_lineage, int parent_lin_id)
{
  // Collect any information we can about the parent.
  double parent_fitness = 0.0;
  //  int parent_lin_id = 0;
  
  // at this point, the cpu has still the lineage from the
  // parent
  //  cLineage * parent_lineage = cpu->GetLineage();
  
#ifdef DEBUG
  if (parent_lineage != NULL){
    assert( parent_lin_id == parent_lineage->GetID() );
  }
#endif
  
  if (parent_genotype != NULL) {
    assert( parent_genotype->GetNumOrganisms() > 0 );
    parent_fitness = parent_genotype->GetTestColonyFitness(ctx);
  }
  double child_fitness = child_genotype->GetTestColonyFitness(ctx);
  cLineage * child_lineage = parent_lineage;
  bool create_lineage = false;
  double lineage_stat1 = child_fitness;
  double lineage_stat2 = child_fitness;
  
  // if we don't have a child lineage, we are probably dealing
  // with manual assignement of the lineage label
  if (child_lineage == NULL) {
    child_lineage = FindLineage( parent_lin_id );
    // lineage doesn't exist...
    if (child_lineage == NULL) {
      // create it
      child_lineage = AddLineage(child_fitness, -1, parent_lin_id, 0, 0);
      cString msg("Creating new lineage 'by hand'!\nRequested lineage label: ");
      msg += parent_lin_id;
      msg += ", actual lineage label: ";
      msg += child_lineage->GetID();
      m_world->GetDriver().NotifyComment(msg);
    }
  }
  // otherwise, check for conditions that cause the creation of a new lineage
  else {
    switch ( m_world->GetConfig().LINEAGE_CREATION_METHOD.Get() ) {
      case 0: // manual creation only
        break;
      case 1: // new lineage whenever a parent has offspring of greater fitness
        if ( child_fitness > parent_fitness ){
          create_lineage = true;
          lineage_stat1 = parent_fitness;
          lineage_stat2 = 0;
        }
        break;
      case 2: // new lineage whenever a new child exceeds the
              // currently highest fitness in the population
        if ( child_fitness > m_max_fitness_lineage->GetMaxFitness() ){
          create_lineage = true;
          lineage_stat1 = m_max_fitness_lineage->GetMaxFitness();
          lineage_stat2 = 0;
        }
        break;
      case 3: // new lineage whenever a new child exceeds the
              // highest fitness, or when it is a child of the
              // of the dominant lineage and exceeds that highest fitness
        if ( child_fitness > m_max_fitness_lineage->GetMaxFitness() ||
             ( parent_lineage == m_dominant_lineage
               && child_fitness > m_dominant_lineage->GetMaxFitness() ) ){
          create_lineage = true;
          lineage_stat1 = m_max_fitness_lineage->GetMaxFitness();
          lineage_stat2 = m_dominant_lineage->GetMaxFitness();
        }
        break;
      case 4: // new lineage whenever a new child exceeds the
              // fitness of the dominant creature (and the fitness of its own lineage)
        if (child_fitness > m_world->GetClassificationManager().GetBestGenotype()->GetTestColonyFitness(ctx)
            && child_fitness > parent_lineage->GetMaxFitness() ){
          create_lineage = true;
          lineage_stat1=m_world->GetClassificationManager().GetBestGenotype()->GetTestColonyFitness(ctx);
          lineage_stat2=parent_lineage->GetMaxFitness();
        }
        break;
      case 5: // new lineage whenever a new child exceeds the
              // fitness of the dominant lineage (and the fitness of its own lineage)
        if ( child_fitness > m_dominant_lineage->GetMaxFitness()
             && child_fitness > parent_lineage->GetMaxFitness() ){
          create_lineage = true;
          lineage_stat1=m_dominant_lineage->GetMaxFitness();
          lineage_stat2=parent_lineage->GetMaxFitness();
        }
        break;
      case 6: // new lineage whenever a new child exceeds the
              // fitness of its own lineage
        if ( child_fitness > parent_lineage->GetMaxFitness() ){
          create_lineage = true;
          lineage_stat1=parent_lineage->GetMaxFitness();
          lineage_stat2 = 0;
        }
        break;
      case 7: // new lineage whenever a new child exceeds the
              // maximum fitness ever attained by its parent lineage
        if (child_fitness > parent_lineage->GetMaxFitnessEver() ) {
          create_lineage = true;
          lineage_stat1 = parent_lineage->GetMaxFitnessEver();
          lineage_stat2 = 0;
        }
        break;
    }
  }
  if (create_lineage) {
    child_lineage = AddLineage(child_fitness, parent_lin_id, -1, lineage_stat1, lineage_stat2);
  }
  
  // add to the lineage
  child_lineage->AddCreature(ctx, child_genotype);
  
  // This would be nice, but the current Avida code doesn't allow for it.
  // Try to implement it in a new version...
  // update the cpu
  //  cpu->SetLineage( lineage );
  //  cpu->SetLineageLabel( lineage->GetID() );
  
  // test whether this makes the new lineage the best
  if (!m_best_lineage || child_lineage->GetAveFitness(ctx) > m_best_lineage->GetAveFitness(ctx))
    m_best_lineage = child_lineage;
  
  // test whether this makes the new lineage the dominant
  if (!m_dominant_lineage || child_lineage->GetNumCreatures() > m_dominant_lineage->GetNumCreatures())
    m_dominant_lineage = child_lineage;
  
  // test whether this makes the new lineage the one with the maximum fitness
  if (!m_max_fitness_lineage || child_lineage->GetMaxFitness() > m_max_fitness_lineage->GetMaxFitness())
    m_max_fitness_lineage = child_lineage;
  
  return child_lineage;
}


void cClassificationManager::RemoveLineageOrganism(cOrganism* org)
{
  cLineage* cur_lineage = org->GetLineage();
  
  if (cur_lineage) {
    // remove the creature
    if ( cur_lineage->RemoveCreature(m_world->GetDefaultContext(), org->GetGenotype())
         || cur_lineage == m_dominant_lineage
         || cur_lineage == m_best_lineage ) {
      // If this lineage no longer exists, tell stats...
      if (cur_lineage->GetNumCreatures() == 0) {
        m_world->GetStats().RemoveLineage( cur_lineage->GetID(),
                                           cur_lineage->GetParentID(),
                                           cur_lineage->GetUpdateBorn(),
                                           cur_lineage->GetGenerationBorn(),
                                           cur_lineage->GetTotalCreatures(),
                                           cur_lineage->GetTotalGenotypes(),
                                           cur_lineage->GetStartFitness(),
                                           cur_lineage->GetLineageStat1(),
                                           cur_lineage->GetLineageStat2() );
      }
    }
    
    // recalc the best/dominant lineage if necessary
    UpdateLineages();
    org->SetLineage( 0 );
    org->SetLineageLabel( -1 );
  }  
}


cLineage* cClassificationManager::FindLineage(int lineage_id) const
{
  list<cLineage *>::const_iterator it = m_lineage_list.begin();
  
  for (; it != m_lineage_list.end(); it++ ) if ( (*it)->GetID() == lineage_id ) break;
  
  if (it == m_lineage_list.end()) {
    cString msg("Lineage ");
    msg += lineage_id; msg += " not found.";
    m_world->GetDriver().NotifyComment(msg);
    return NULL;
  }
  
  return ( *it );
}


void cClassificationManager::PrintLineageTotals(const cString &filename, bool verbose)
{
  ofstream& fp = m_world->GetDataFileOFStream(filename);
  assert(fp.good());
  
  fp << m_world->GetStats().GetUpdate();
  const list<cLineage *> & lineage_list = GetLineageList();
  
  if (verbose) { // in verbose format, we print only those
                  // lineages that are actually present, but
                  // we print much more info
    fp << ": " << m_world->GetStats().SumGeneration().Average() << "\n";
    
    // let's calculate the average fitness in the population also
    double fitness_sum = 0;
    double fitness = 0;
    int total_num_organisms = 0;
    int num_organisms = 0;
    
    
    list<cLineage *>::const_iterator it = lineage_list.begin();
    for ( ; it != lineage_list.end(); it++ ){
      num_organisms = (*it)->GetNumCreatures();
      fitness = (*it)->GetAveFitness(m_world->GetDefaultContext());
      fitness_sum += fitness * num_organisms;
      total_num_organisms += num_organisms;
      fp << " "
        << (*it)->GetID()           << " "
        << (*it)->GetParentID()     << " "
        << (*it)->GetStartFitness() << " "
        << (*it)->GetMaxFitness()   << " "
        << fitness                  << " "
        << num_organisms            << " "
        << (*it)->GetTotalCreatures()    << "\n";
    }
    fp << "#Dom: " << GetDominantLineage()->GetID()
      << ", Max: " << GetMaxFitnessLineage()->GetID()
      << ", Best: " << GetBestLineage()->GetID()
      << "\n#Pop. fitness: " << fitness_sum / (double) total_num_organisms
      << "\n" << endl;
  }
  else {
    fp << " ";
    int cur_id = 0;
    
    list<cLineage *>::const_iterator it = lineage_list.begin();
    for ( ; it != lineage_list.end(); it++ ){
      int next_id = (*it)->GetID();
      // wind up the current id to the next id
      for ( ; cur_id < next_id; cur_id++ )
        fp << "-1 "; // output -1 when we don't have the data
      fp << (*it)->GetTotalCreatures() << " ";
      cur_id += 1;
    }
    fp << endl;
  }
}


void cClassificationManager::PrintLineageCurCounts(const cString & filename)
{
  ofstream& fp = m_world->GetDataFileOFStream(filename);
  assert(fp.good());
  
  fp << m_world->GetStats().GetUpdate() << " ";
  const list<cLineage *> & lineage_list = GetLineageList();
  
  list<cLineage *>::const_iterator it = lineage_list.begin();
  int cur_id = 0;
  for ( ; it != lineage_list.end(); it++ ){
    int next_id = (*it)->GetID();
    // wind up the current id to the next id
    for ( ; cur_id < next_id; cur_id++ )
      fp << "0 ";
    fp << (*it)->GetNumCreatures() << " ";
    cur_id += 1;
  }	
  fp << endl;
}

//@MRR Coalescence Clades
void cClassificationManager::LoadCCladeFounders(const cString& filename)
{
	ifstream fin(filename);
	if (!fin.is_open())
		m_world->GetDriver().RaiseFatalException(1, "Unable to open coalescence clade ids.");
	int id;
	fin >> id;
	while(!fin.eof())
	{
		m_cclade_ids.insert(id);
		fin >> id;
	}
	fin.close();
}

bool cClassificationManager::IsCCladeFounder(const int id) const
{
	set<int>::const_iterator it = m_cclade_ids.find(id);
	return (it == m_cclade_ids.end()) ? false : true;
}


unsigned int cClassificationManager::FindCRC(const cGenome & in_genome) const
{
  unsigned int total = 0;
  
  for (int i = 0; i < in_genome.GetSize(); i++) {
    total += (in_genome[i].GetOp() + 3) * i;
  }
  
  return total % nGenotype::HASH_SIZE;
}


unsigned int cClassificationManager::FindInjectCRC(const cGenome & in_genome) const
{
  unsigned int total = 13;
  int i;
  
  for (i = 0; i < in_genome.GetSize(); i++) {
    total *= in_genome[i].GetOp() + 10 + i << 6;
    total += 3;
  }
  
  return total;
}


cString cClassificationManager::GetLabel(int in_size, int in_num)
{
  char alpha[6];
  char full_name[12];
  int i;
  
  for (i = 4; i >= 0; i--) {
    alpha[i] = (in_num % 26) + 'a';
    in_num /= 26;
  }
  alpha[5] = '\0';
  
  sprintf(full_name, "%03d-%s", in_size, alpha);
  
  return full_name;
}


bool cClassificationManager::OK()
{
  bool ret_value = true;
  
  // Check components...
  if (!m_genotype_ctl->OK() || !m_species_ctl->OK()) { ret_value = false; }
  
  // Now to double check the numbers of genotypes (and threshold) for each
  // species.  This will only work if debug mode is on.
  
#ifdef DEBUG
  // Initialize debug routines in the species.
  
  cSpecies * cur_species = NULL;
  cSpecies * first_active = m_species_ctl->GetFirst();
  cSpecies * first_inactive = m_species_ctl->GetFirstInactive();
  cSpecies * first_garbage = m_species_ctl->GetFirstGarbage();
  
  while (cur_species != first_active) {
    if (!cur_species) cur_species = first_active;
    cur_species->debug_num_genotypes = 0;
    cur_species->debug_num_threshold = 0;
    cur_species = cur_species->GetNext();
  }
  
  cur_species = NULL;
  while (cur_species != first_inactive) {
    if (!cur_species) cur_species = first_inactive;
    cur_species->debug_num_genotypes = 0;
    cur_species->debug_num_threshold = 0;
    cur_species = cur_species->GetNext();
  }
  
  cur_species = NULL;
  while (cur_species != first_garbage) {
    if (!cur_species) cur_species = first_garbage;
    cur_species->debug_num_genotypes = 0;
    cur_species->debug_num_threshold = 0;
    cur_species = cur_species->GetNext();
  }
  
  
  // Check the species for each genotype and place results in the species.
  
  cGenotype * cur_gen = m_genotype_ctl->GetBest();
  for (int i = 0; i < m_genotype_ctl->GetSize(); i++) {
    if (cur_gen->GetSpecies()) {
      cur_gen->GetSpecies()->debug_num_genotypes++;
      if (cur_gen->GetThreshold()) {
        cur_gen->GetSpecies()->debug_num_threshold++;
      }
    }
    cur_gen = cur_gen->GetNext();
  }
  
  // Finally, make sure all the numbers match up.
  
  cur_species = NULL;
  while (cur_species != first_active) {
    if (!cur_species) cur_species = first_active;
    assert(cur_species->debug_num_genotypes == cur_species->GetNumGenotypes());
    assert(cur_species->debug_num_threshold == cur_species->GetNumThreshold());
    assert(cur_species->debug_num_genotypes >= cur_species->debug_num_threshold);
    
    assert (cur_species->debug_num_threshold);
    cur_species = cur_species->GetNext();
  }
  
  cur_species = NULL;
  while (cur_species != first_inactive) {
    if (!cur_species) cur_species = first_inactive;
    assert(cur_species->debug_num_genotypes == cur_species->GetNumGenotypes());
    assert(cur_species->debug_num_threshold == cur_species->GetNumThreshold());
    assert(cur_species->debug_num_threshold);
    assert(cur_species->debug_num_genotypes);
  }
  
  cur_species = NULL;
  while (cur_species != first_garbage) {
    if (!cur_species) cur_species = first_garbage;
    assert(cur_species->debug_num_genotypes == 0 &&
           cur_species->debug_num_threshold == 0);
  }
  
#endif
  
  assert (ret_value == true);
  
  return ret_value;
}
