//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cGenebank.h"

#include "cGenotype.h"
#include "cSpecies.h"
#include "cStats.h"
#include "cStringList.h"
#include "cTestUtil.h"
#include "cWorld.h"

using namespace std;


cGenebank::cGenebank(cWorld* world)
  : m_world(world), stats(world->GetStats())
{
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) {
    genotype_count[i] = 0;
  }

  genotype_control = new cGenotypeControl(world, *this);
  species_control  = new cSpeciesControl(world, *this);

}

cGenebank::~cGenebank()
{
  delete genotype_control;
  delete species_control;
}

void cGenebank::UpdateReset()
{
  static int genotype_dom_time = 0;
  static int prev_dom = -1;

  cGenotype * best_genotype = GetBestGenotype();

  species_control->Purge(stats);
  if (best_genotype && best_genotype->GetID() != prev_dom) {
    genotype_dom_time = 0;
    prev_dom = best_genotype->GetID();
  }
  else {
    genotype_dom_time++;
    if (genotype_dom_time == m_world->GetConfig().GENOTYPE_PRINT_DOM.Get()) {
      cString filename;
      filename.Set("genebank/%s", best_genotype->GetName()());
      cTestUtil::PrintGenome(m_world, best_genotype->GetGenome(), 
			     filename, best_genotype, stats.GetUpdate());
    }
  }

//   tArray<int> hist_array(15);
//   hist_array.SetAll(0);
//   int total_gens = 0;
  
//   for (int i = 0; i < nGenotype::HASH_SIZE; i++) {
//     int cur_val = active_genotypes[i].GetSize();
//     total_gens += cur_val;
//     if (cur_val < 15) hist_array[cur_val]++;
//     else cout << cur_val << " ";
//   }
//   cout << endl;
//   for (int i = 0; i < 15; i++) {
//     cout << i << " : " << hist_array[i] << endl;
//   }
//   cout << "Total genotypes: " << total_gens << endl;
}

cString cGenebank::GetLabel(int in_size, int in_num)
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


void cGenebank::AddGenotype(cGenotype * in_genotype, int list_num)
{
  assert( in_genotype != 0 );
  
  if (list_num < 0) list_num = FindCRC(in_genotype->GetGenome());
  
  active_genotypes[list_num].Push(in_genotype);
  genotype_control->Insert(*in_genotype);
  stats.AddGenotype(in_genotype->GetID());

  // Speciation... If we are creating a new genotype here, we must
  // initilize it to the species of its parent genotype.

  cSpecies * parent_species = NULL;
  if ( in_genotype->GetParentGenotype() != NULL) {
    parent_species = in_genotype->GetParentGenotype()->GetSpecies();
  }

  in_genotype->SetSpecies(parent_species);
  if (parent_species != NULL) parent_species->AddGenotype();
}


// Add a new genotype to the genebank.  Take in parent genotypes to track
// phylogenies (the second genotype is in the case of sexual organisms)

cGenotype * cGenebank::AddGenotype(const cGenome & in_genome,
				   cGenotype * parent_genotype,
				   cGenotype * parent2_genotype)
{
  // Make sure we dont have a NULL parent -- if we do, we should be calling
  // InjectGenotype().
  assert (parent_genotype != NULL);

  int list_num = FindCRC(in_genome);
  cGenotype * found_genotype = FindGenotype(in_genome,
			       parent_genotype->GetLineageLabel(), list_num);

  if (!found_genotype) {
    found_genotype = new cGenotype(m_world, stats.GetUpdate());
    found_genotype->SetGenome(in_genome);
    found_genotype->SetParent(parent_genotype, parent2_genotype);    
    AddGenotype(found_genotype, list_num);
  }

  return found_genotype;
}


// Add a new genotype that has been injected from the outside to the genebank.
//  Take in a lineage label to track clades.

cGenotype * cGenebank::InjectGenotype(const cGenome & in_genome,
				      int lineage_label)
{
  int list_num = FindCRC(in_genome);
  cGenotype * found_genotype =
    FindGenotype(in_genome, lineage_label, list_num);

  if (!found_genotype) {
    found_genotype = new cGenotype(m_world, stats.GetUpdate());
    found_genotype->SetGenome(in_genome);
    found_genotype->SetParent(NULL, NULL);
    found_genotype->SetLineageLabel(lineage_label);
    AddGenotype(found_genotype, list_num);
  }

  return found_genotype;
}

const cGenotype * cGenebank::FindGenotype(const cGenome & in_genome,
				  int lineage_label, int list_num) const
{
  if (list_num < 0) list_num = FindCRC(in_genome);

  tConstListIterator<cGenotype> list_it(active_genotypes[list_num]);

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

cGenotype * cGenebank::FindGenotype(const cGenome & in_genome,
				    int lineage_label, int list_num)
{
  if (list_num < 0) list_num = FindCRC(in_genome);

  tListIterator<cGenotype> list_it(active_genotypes[list_num]);
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

void cGenebank::RemoveGenotype(cGenotype & in_genotype)
{
  // If we are supposed to defer analysis of this genotype, do so...
  if (in_genotype.GetDeferAdjust() == true) return;

  // If this genotype is still active, mark it no longer active and
  // take it out of the hash table so it doesn't have any new organisms
  // assigned to it.

  if (in_genotype.GetActive() == true) {
    int list_num = FindCRC(in_genotype.GetGenome());
    active_genotypes[list_num].Remove(&in_genotype);
    genotype_control->Remove(in_genotype);
    in_genotype.Deactivate(stats.GetUpdate());
    if (m_world->GetConfig().TRACK_MAIN_LINEAGE.Get()) {
      genotype_control->InsertHistoric(in_genotype);
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
      const int new_coal = genotype_control->UpdateCoalescent();
      stats.SetCoalescentGenotypeDepth(new_coal);
      // cout << "Set coalescent to " << found_gen->GetDepth() << endl;

      if (parent->GetNumOrganisms() == 0) {
	// Regardless, run RemoveGenotype on the parent.
	RemoveGenotype(*parent);
      }
    }

    if (lineage_type == 2 && parent2 != NULL) {
      parent2->RemoveOffspringGenotype();

      // Test to see if we need to update the coalescent genotype.
      const int new_coal = genotype_control->UpdateCoalescent();
      stats.SetCoalescentGenotypeDepth(new_coal);
      // cout << "Set coalescent to " << found_gen->GetDepth() << endl;

      if (parent2->GetNumOrganisms() == 0) {
	// Regardless, run RemoveGenotype on the parent.
	RemoveGenotype(*parent2);
      }
    }

    genotype_control->RemoveHistoric(in_genotype);
  }

  // Handle the relevent statistics...
  stats.RemoveGenotype(in_genotype.GetID(),
	      in_genotype.GetParentID(), in_genotype.GetParentDistance(),
	      in_genotype.GetDepth(), in_genotype.GetTotalOrganisms(),
              in_genotype.GetTotalParasites(),
	      stats.GetUpdate() - in_genotype.GetUpdateBorn(),
              in_genotype.GetLength());
  if (in_genotype.GetThreshold()) {
    stats.RemoveThreshold(in_genotype.GetID());
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
	species_control->SetInactive(*cur_species);
      }
      else {
	species_control->Adjust(*cur_species);
      }
    }

    // Finally, remove the species completely if it has no genotypes left.

    if (!cur_species->GetNumGenotypes()) {
      species_control->SetGarbage(*cur_species);
    }
  }


  delete &in_genotype;
}

void cGenebank::ThresholdGenotype(cGenotype & in_genotype)
{
  cSpecies * found_species = NULL;

  in_genotype.SetName( GetLabel(in_genotype.GetLength(),
				genotype_count[in_genotype.GetLength()]++) );
  in_genotype.SetThreshold();

  // If speciation is on, assign a species to the genotype now that it is
  // threshold.

  if (m_world->GetConfig().SPECIES_RECORDING.Get()) {
    // Record the old species to know if it changes.

    cSpecies * old_species = in_genotype.GetSpecies();

    // Determine the "proper" species.

    found_species = species_control->Find(in_genotype,
					  m_world->GetConfig().SPECIES_RECORDING.Get());

    // If no species was found, create a new one.

    if (!found_species) {
      found_species = new cSpecies(m_world, in_genotype.GetGenome(), stats.GetUpdate());
      if (in_genotype.GetSpecies())
	found_species->SetParentID(in_genotype.GetSpecies()->GetID());
      species_control->SetActive(*found_species);
      stats.AddSpecies(found_species->GetID());

      // Since this is a new species, see if we should be printing it.

      if (m_world->GetConfig().SPECIES_PRINT.Get()) {
	cString filename;
	filename.Set("genebank/spec-%04d", found_species->GetID());
	cTestUtil::PrintGenome(m_world, in_genotype.GetGenome(), filename,
			       &in_genotype, stats.GetUpdate());
      }
    }
    else {
      // If we are not creating a new species, but are adding a threshold
      // to one which is currently in-active, make sure to move it back to
      // the active list.

      if (found_species->GetNumThreshold() == 0) {
	species_control->SetActive(*found_species);
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
	  species_control->SetGarbage(*old_species);
      }
    }
    else {
      if (found_species->GetNumThreshold() > 1) {
	species_control->Adjust(*found_species);
      }
    }
  }

  // Do the relevent statistics...

  if (m_world->GetConfig().SPECIES_RECORDING.Get()) {
    stats.AddThreshold(in_genotype.GetID(), in_genotype.GetName()(),
			 found_species->GetID());
  } else {
    stats.AddThreshold(in_genotype.GetID(), in_genotype.GetName()());
  }

  // Print the genotype?

  if (m_world->GetConfig().GENOTYPE_PRINT.Get()) {
    cString filename;
    filename.Set("genebank/%s", in_genotype.GetName()());
    cTestUtil::PrintGenome(m_world, in_genotype.GetGenome(), filename,
			   &in_genotype, stats.GetUpdate());
  }
}

bool cGenebank::AdjustGenotype(cGenotype & in_genotype)
{
  if (!genotype_control->Adjust(in_genotype)) return false;

  if ((in_genotype.GetNumOrganisms() >= m_world->GetConfig().THRESHOLD.Get() ||
       &in_genotype == genotype_control->GetBest()) &&
      !(in_genotype.GetThreshold())) {
    ThresholdGenotype(in_genotype);
  }

  return true;
}

bool cGenebank::SaveClone(ofstream & fp)
{
  // This method just save the counts at each size-class of genotypes.
  // The rest is reconstructable.

  // Save the numbers of organisms we're up to at each size.
  fp << MAX_CREATURE_SIZE << " ";
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) {
    fp << genotype_count[i] << " ";
  }

  return true;
}

bool cGenebank::LoadClone(ifstream & fp)
{
  // This method just restores the counts at each size-class of genotypes.
  // The rest of the loading process should be handled elsewhere.

  // Load the numbers of organisms we're up to at each size.
  int max_size;
  fp >> max_size;
  assert (max_size <= MAX_CREATURE_SIZE); // MAX_CREATURE_SIZE too small
  for (int i = 0; i < max_size && i < MAX_CREATURE_SIZE; i++) {
    fp >> genotype_count[i];
  }

  return true;
}

bool cGenebank::DumpTextSummary(ofstream & fp)
{
  genotype_control->Reset(0);
  for (int i = 0; i < genotype_control->GetSize(); i++) {
    cGenotype * genotype = genotype_control->Get(0);
    fp << genotype->GetGenome().AsString() << " "
       << genotype->GetNumOrganisms() << " "
       << genotype->GetID() << endl;
    genotype_control->Next(0);
  }

  return true;
}

bool cGenebank::PrintGenotypes(ofstream & fp, cString & data_fields,
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
  fp << "# Output is genotypes at update " << stats.GetUpdate();
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
  genotype_control->Reset(0);
  for (int i = 0; i < genotype_control->GetSize(); i++) {
    cGenotype * genotype = genotype_control->Get(0);

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
    genotype_control->Next(0);
  }

  // Print the historic population if we are supposed to.
  if (historic == 0) return true;

  // Start by calculating the update we should start printing from...
  int start_update = 0;
  if (historic > 0) start_update = stats.GetUpdate() - historic;

  // Now loop through the remaining genotypes...
  genotype_control->ResetHistoric(0);
  for (int i = 0; i < genotype_control->GetHistoricCount(); i++) {
    // Get the next genotype.  Only print it if its in range...
    cGenotype * genotype = genotype_control->Get(0);
    if (genotype->GetUpdateDeactivated() < start_update) {
      genotype_control->Next(0);
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
    genotype_control->Next(0);
  }

  
  return true;
}

bool cGenebank::DumpDetailedSummary(ofstream & fp)
{
  genotype_control->Reset(0);
  DumpDetailHeading(fp);
  for (int i = 0; i < genotype_control->GetSize(); i++) {
    DumpDetailedEntry(genotype_control->Get(0), fp);
    genotype_control->Next(0);
  }

  return true;
}

bool cGenebank::DumpHistoricSummary(ofstream & fp, int back_dist)
{
  // Calculate the update we should start printing from...
  int start_update = 0;
  if (back_dist > 0) start_update = stats.GetUpdate() - back_dist;

  // Loop through all defunct genotypes that we're saving.
  DumpDetailHeading(fp);
  genotype_control->ResetHistoric(0);
  for (int i = 0; i < genotype_control->GetHistoricCount(); i++) {
    // Get the next genotype.  Only print it if its in range...
    cGenotype * cur_genotype = genotype_control->Get(0);
    if (cur_genotype->GetUpdateDeactivated() < start_update) {
      genotype_control->Next(0);
      continue;
    }
    DumpDetailedEntry(cur_genotype, fp);

    // Move to the next genotype...
    genotype_control->Next(0);
  }

  return true;
}

bool cGenebank::DumpDetailedSexSummary(ofstream & fp)
{
  genotype_control->Reset(0);
  DumpDetailSexHeading(fp);
  for (int i = 0; i < genotype_control->GetSize(); i++) {
    DumpDetailedSexEntry(genotype_control->Get(0), fp);
    genotype_control->Next(0);
  }

  return true;
}

bool cGenebank::DumpHistoricSexSummary(ofstream & fp)
{
  genotype_control->ResetHistoric(0);
  DumpDetailSexHeading(fp);
  for (int i = 0; i < genotype_control->GetHistoricCount(); i++) {
    DumpDetailedSexEntry(genotype_control->Get(0), fp);
    genotype_control->Next(0);
  }

  return true;
}

void cGenebank::DumpDetailHeading (ofstream & fp)
{
  fp << "#filetype genotype_data" << endl
     << "#format id parent_id parent_dist num_cpus total_cpus length merit gest_time fitness update_born update_dead depth sequence" << endl
     << endl
     << "#  1: ID" << endl
     << "#  2: parent ID" << endl
     << "#  3: parent distance" << endl
     << "#  4: number of orgranisms currently alive" << endl
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

void cGenebank::DumpDetailSexHeading (ofstream & fp)
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

void cGenebank::DumpDetailedEntry(cGenotype * genotype, ofstream & fp)
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

void cGenebank::DumpDetailedSexEntry(cGenotype * genotype, ofstream & fp)
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

bool cGenebank::OK()
{
  bool ret_value = true;

  // Check components...

  if (!genotype_control->OK() || !species_control->OK()) {
    ret_value = false;
  }

  // Now to double check the numbers of genotypes (and threshold) for each
  // species.  This will only work if debug mode is on.

#ifdef DEBUG
  // Initialize debug routines in the species.

  cSpecies * cur_species = NULL;
  cSpecies * first_active = species_control->GetFirst();
  cSpecies * first_inactive = species_control->GetFirstInactive();
  cSpecies * first_garbage = species_control->GetFirstGarbage();

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

  cGenotype * cur_gen = genotype_control->GetBest();
  for (int i = 0; i < genotype_control->GetSize(); i++) {
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

int cGenebank::CountNumCreatures()
{
  int i;
  int total = 0;

  genotype_control->Reset(0);
  for (i = 0; i < genotype_control->GetSize(); i++) {
    total += genotype_control->Get(0)->GetNumOrganisms();
    genotype_control->Next(0);
  }

  return total;
}


unsigned int cGenebank::FindCRC(const cGenome & in_genome) const
{
  unsigned int total = 0;

  for (int i = 0; i < in_genome.GetSize(); i++) {
    total += (in_genome[i].GetOp() + 3) * i;
  }

  return total % nGenotype::HASH_SIZE;
}

void cGenebank::SpeciesTest(char * message, cGenotype & genotype)
{
//  cSpecies * cur_species = genotype.GetSpecies();

//    if (cur_species) {
//      g_debug.Comment("UD %d: %s on genotype [%d] (size %d) of species [%d]",
//  		    stats.GetUpdate(), message, genotype.GetID(),
//  		    genotype.GetNumOrganisms(), cur_species->GetID());
//    } else {
//      g_debug.Comment("UD %d: %s on genotype [%d] (size %d) (no species)",
//  	    stats.GetUpdate(), message, genotype.GetID(),
//  	    genotype.GetNumOrganisms());
//    }

//    if (cur_species) {
//      g_debug.Comment("   Species [%d] has %d gen and %d thresh. (list %d)",
//  	    cur_species->GetID(), cur_species->GetNumGenotypes(),
//  	    cur_species->GetNumThreshold(), cur_species->GetQueueType());
//    }
}
