/*
 *  cBirthChamber.cc
 *  Avida
 *
 *  Called "birth_chamber.cc" prior to 12/2/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include "cBirthChamber.h"

#include "cAvidaContext.h"
#include "cBirthDemeHandler.h"
#include "cBirthGenomeSizeHandler.h"
#include "cBirthGlobalHandler.h"
#include "cBirthGridLocalHandler.h"
#include "cBirthMateSelectHandler.h"
#include "cBirthNeighborhoodHandler.h"
#include "cClassificationManager.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cOrganism.h"
#include "cTools.h"
#include "cWorld.h"
#include "cStats.h"
#include "tArray.h"
#include "functions.h"


cBirthSelectionHandler* cBirthChamber::getSelectionHandler(int hw_type)
{
  cBirthSelectionHandler* handler = NULL;
  if (!m_handler_map.Get(hw_type, handler)) {
    const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();
    
    if (m_world->GetConfig().NUM_DEMES.Get() > 1) {
      // Deme local takes priority, and manages the sub handlers
      handler = new cBirthDemeHandler(m_world, this);    
    } else if (birth_method < NUM_LOCAL_POSITION_CHILD || birth_method == POSITION_CHILD_PARENT_FACING) { 
      // ... else check if the birth method is one of the local ones... 
      if (m_world->GetConfig().LEGACY_GRID_LOCAL_SELECTION.Get()) {
        handler = new cBirthGridLocalHandler(m_world, this);
      } else {
        handler = new cBirthNeighborhoodHandler(m_world, this);
      }
    } else if (m_world->GetConfig().SAME_LENGTH_SEX.Get() != 0) {
      // ... else check if recombination must be with organisms of the same length
      handler = new cBirthGenomeSizeHandler(this);
    } else if (m_world->GetConfig().ALLOW_MATE_SELECTION.Get()) {
      // ... else check if we have mate selection
      handler = new cBirthMateSelectHandler(this);
    } else {
      // If everything failed until this point, use default global.
      handler = new cBirthGlobalHandler(this);
    }
    
    m_handler_map.Set(hw_type, handler);
  }
  
  return handler;
}

bool cBirthChamber::ValidBirthEntry(const cBirthEntry& entry) const
{
  // If there is no organism in the entry, return false.
  if (entry.timestamp == -1) return false;

  // If there is an organism, determine if it is still alive.
  const int max_wait_time = m_world->GetConfig().MAX_BIRTH_WAIT_TIME.Get();

  // If the max_wait_time is -1, there is no timeout, so its alive.
  if (max_wait_time == -1) return true;

  // Otherwise, check if few enough updates have gone by...
  const int cur_update = m_world->GetStats().GetUpdate();
  const int max_update = entry.timestamp + max_wait_time;

  if (cur_update > max_update) return false;  // Too many updates...

  return true;
}

void cBirthChamber::StoreAsEntry(const cMetaGenome& offspring, cOrganism* parent, cBirthEntry& entry) const
{
  cGenotype* parent_genotype = parent->GetGenotype();
  parent_genotype->IncDeferAdjust();
  entry.genome = offspring;
  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    entry.energy4Offspring = parent->GetPhenotype().ExtractParentEnergy();
    entry.merit = parent->GetPhenotype().ConvertEnergyToMerit(entry.energy4Offspring);
  } else {
    entry.merit = parent->GetPhenotype().GetMerit();
  }
  entry.parent_genotype = parent_genotype;
  entry.timestamp = m_world->GetStats().GetUpdate();  
}

bool cBirthChamber::RegionSwap(cGenome& genome0, cGenome& genome1, int start0, int end0, int start1, int end1)
{
   assert( start0 >= 0  &&  start0 < genome0.GetSize() );
   assert( end0   >= 0  &&  end0   < genome0.GetSize() );
   assert( start1 >= 0  &&  start1 < genome1.GetSize() );
   assert( end1   >= 0  &&  end1   < genome1.GetSize() );
            
   // Calculate size of sections crossing over...
   int size0 = end0 - start0;
   int size1 = end1 - start1;
            
   int new_size0 = genome0.GetSize() - size0 + size1;
   int new_size1 = genome1.GetSize() - size1 + size0;
      
   // Don't Crossover if offspring will be illegal!!!
   if (new_size0 < MIN_CREATURE_SIZE || new_size0 > MAX_CREATURE_SIZE ||
       new_size1 < MIN_CREATURE_SIZE || new_size1 > MAX_CREATURE_SIZE) {
     return false;
   } 

   if (size0 > 0 && size1 > 0) {
     cGenome cross0 = cGenomeUtil::Crop(genome0, start0, end0);
     cGenome cross1 = cGenomeUtil::Crop(genome1, start1, end1);
     genome0.Replace(start0, size0, cross1);
     genome1.Replace(start1, size1, cross0);
   } else if (size0 > 0) {
     cGenome cross0 = cGenomeUtil::Crop(genome0, start0, end0);
     genome1.Replace(start1, size1, cross0);
   } else if (size1 > 0) {
     cGenome cross1 = cGenomeUtil::Crop(genome1, start1, end1);
     genome0.Replace(start0, size0, cross1);
   }

   return true;
}

void cBirthChamber::GenomeSwap(cGenome& genome0, cGenome& genome1, double& merit0, double& merit1)
{
  cGenome genome0_tmp = genome0;
  genome0 = genome1; 
  genome1 = genome0_tmp; 

  double merit0_tmp = merit0; 
  merit0 = merit1; 
  merit1 = merit0_tmp;
}


bool cBirthChamber::DoAsexBirth(cAvidaContext& ctx, const cMetaGenome& offspring, cOrganism& parent,
                                tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array)
{
  // This is asexual who doesn't need to wait in the birth chamber
  // just build the child and return.
  child_array.Resize(1);
  child_array[0] = new cOrganism(m_world, ctx, offspring);
  merit_array.Resize(1);
  
  if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    // calculate energy to be given to child
    double child_energy = parent.GetPhenotype().ExtractParentEnergy();
        
    // set child energy & merit
    child_array[0]->GetPhenotype().SetEnergy(child_energy);
    merit_array[0] = child_array[0]->GetPhenotype().ConvertEnergyToMerit(child_array[0]->GetPhenotype().GetStoredEnergy());
		if(merit_array[0].GetDouble() <= 0.0) {  // do not allow zero merit
			delete child_array[0];  // MAKE SURE THIS GETS DONE! Otherwise, memory leak.	
			child_array.Resize(0);
			merit_array.Resize(0);
			return false;
		}
  } else {
    merit_array[0] = parent.GetPhenotype().GetMerit();
  }

  // Setup the genotype for the child
  cGenotype* child_genotype = parent.GetGenotype();
  
  if (parent.GetPhenotype().CopyTrue() == false) {
    // Add this genotype with only one parent since its asexual.
    child_genotype = m_world->GetClassificationManager().GetGenotype(offspring.GetGenome(), parent.GetGenotype(), NULL);
  }

  child_array[0]->SetGenotype(child_genotype);
  parent.GetGenotype()->SetBreedStats(*child_genotype);
    
  child_genotype->IncDeferAdjust();

  return true;
}

bool cBirthChamber::DoPairAsexBirth(cAvidaContext& ctx, const cBirthEntry& old_entry, const cMetaGenome& new_genome,
                                    cOrganism& parent, tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array)
{
  // Build both child organisms...
  child_array.Resize(2);
  child_array[0] = new cOrganism(m_world, ctx, old_entry.genome);
  child_array[1] = new cOrganism(m_world, ctx, new_genome);

  // Setup the merits for both children...
  merit_array.Resize(2);
  merit_array[0] = old_entry.merit;
  merit_array[1] = parent.GetPhenotype().GetMerit();

  // Setup the genotypes for both children...
  SetupGenotypeInfo(child_array[0], old_entry.parent_genotype, NULL);
  SetupGenotypeInfo(child_array[1], parent.GetGenotype(), NULL);

  // We are now also done with the parent genotype that we were saving in
  // the birth chamber, so we can un-defer that one.

  old_entry.parent_genotype->DecDeferAdjust();
  m_world->GetClassificationManager().AdjustGenotype(*old_entry.parent_genotype);

  return true;
}



void cBirthChamber::DoBasicRecombination(cAvidaContext& ctx, cGenome& genome0, cGenome& genome1,
                                         double& merit0, double& merit1)
{
  double start_frac = ctx.GetRandom().GetDouble();
  double end_frac = ctx.GetRandom().GetDouble();
  if (start_frac > end_frac) nFunctions::Swap(start_frac, end_frac);
    
  // calculate the proportion of the genome  that will be swapped
  double cut_frac = end_frac - start_frac;
  double stay_frac = 1.0 - cut_frac;

  int start0 = (int) (start_frac * (double) genome0.GetSize());
  int end0   = (int) (end_frac * (double) genome0.GetSize());
  int start1 = (int) (start_frac * (double) genome1.GetSize());
  int end1   = (int) (end_frac * (double) genome1.GetSize());

  RegionSwap(genome0, genome1, start0, end0, start1, end1);

  // Adjust the merits....
  double tmp_merit0 = merit0 * stay_frac + merit1 * cut_frac;
  merit1 = merit1 * stay_frac + merit0 * cut_frac;
  merit0 = tmp_merit0;

  // Majority of the genome should stay in the offspring
  if (stay_frac < cut_frac) {
     GenomeSwap(genome0, genome1, merit0, merit1); 
  } 
}

void cBirthChamber::DoModularContRecombination(cAvidaContext& ctx, cGenome& genome0, cGenome& genome1,
                                               double& merit0, double& merit1)
{
  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();

  int start_module = (int) (ctx.GetRandom().GetDouble() * num_modules);
  int end_module = (int) (ctx.GetRandom().GetDouble() * num_modules);

  double start_frac = ((double) start_module) / (double) num_modules;
  double end_frac = ((double) end_module) / (double) num_modules;

  if (start_frac > end_frac) nFunctions::Swap(start_frac, end_frac);
	    
  // calculate the proportion of the genome  that will be swapped
  double cut_frac = end_frac - start_frac;
  double stay_frac = 1.0 - cut_frac;

  int start0 = (int) (start_frac * (double) genome0.GetSize());
  int end0   = (int) (end_frac * (double) genome0.GetSize());
  int start1 = (int) (start_frac * (double) genome1.GetSize());
  int end1   = (int) (end_frac * (double) genome1.GetSize());

  RegionSwap(genome0, genome1, start0, end0, start1, end1);

  // Adjust the merits....
  double tmp_merit0 = merit0 * stay_frac + merit1 * cut_frac;
  merit1 = merit1 * stay_frac + merit0 * cut_frac;
  merit0 = tmp_merit0;

  // Majority of the genome should stay in the offspring
  if (stay_frac < cut_frac) {
     GenomeSwap(genome0, genome1, merit0, merit1); 
  } 
}

void cBirthChamber::DoModularNonContRecombination(cAvidaContext& ctx, cGenome& genome0, cGenome& genome1,
                                                  double& merit0, double& merit1)
{
  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();

  int swap_count = 0;
  for (int i = 0; i < num_modules; i++) {
    if (ctx.GetRandom().GetDouble() < 0.5) {
      swap_count++;
      double start_frac = ((double) i) / (double) num_modules;
      double end_frac = ((double) i+1) / (double) num_modules;
      int start0 = (int) (start_frac * (double) genome0.GetSize());
      int end0   = (int) (end_frac * (double) genome0.GetSize());
      int start1 = (int) (start_frac * (double) genome1.GetSize());
      int end1   = (int) (end_frac * (double) genome1.GetSize());

      RegionSwap(genome0, genome1, start0, end0, start1, end1);  
    }
  }

  double cut_frac = ((double) swap_count) / (double) num_modules;
  double stay_frac = 1.0 - cut_frac;

  // Adjust the merits....
  double tmp_merit0 = merit0 * stay_frac + merit1 * cut_frac;
  merit1 = merit1 * stay_frac + merit0 * cut_frac;
  merit0 = tmp_merit0;

  // Majority of the genome should stay in the offspring
  if (stay_frac < cut_frac) {
     GenomeSwap(genome0, genome1, merit0, merit1); 
  } 
}

void cBirthChamber::DoModularShuffleRecombination(cAvidaContext& ctx, cGenome& genome0, cGenome& genome1,
                                                   double& merit0, double& merit1)
{
  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();
  tArray<bool> swapped_region(num_modules);
  swapped_region.SetAll(false);

  int swap_count = 0;
  for (int mod0 = 0; mod0 < num_modules; mod0++) {
    if (ctx.GetRandom().GetDouble() < 0.5) {
      swap_count++;

      // Collect start and end info for current module
      double start0_frac = ((double) mod0) / (double) num_modules;
      double end0_frac = ((double) mod0+1) / (double) num_modules;
      int start0 = (int) (start0_frac * (double) genome0.GetSize());
      int end0   = (int) (end0_frac * (double) genome0.GetSize());

      // Pick module from other genome...
      int mod1 = ctx.GetRandom().GetUInt(num_modules);
      while (swapped_region[mod1] == true) {
        mod1 = ctx.GetRandom().GetUInt(num_modules);
      }
      swapped_region[mod1] = true;

      // Collect start and end info for other module
      double start1_frac = ((double) mod1) / (double) num_modules;
      double end1_frac = ((double) mod1+1) / (double) num_modules;
      int start1 = (int) (start1_frac * (double) genome1.GetSize());
      int end1   = (int) (end1_frac * (double) genome1.GetSize());

      // Do the swap.
      RegionSwap(genome0, genome1, start0, end0, start1, end1);  
    }
  }

  double cut_frac = ((double) swap_count) / (double) num_modules;
  double stay_frac = 1.0 - cut_frac;

  // Adjust the merits....
  double tmp_merit0 = merit0 * stay_frac + merit1 * cut_frac;
  merit1 = merit1 * stay_frac + merit0 * cut_frac;
  merit0 = tmp_merit0;

  // Majority of the genome should stay in the offspring
  if (stay_frac < cut_frac) {
     GenomeSwap(genome0, genome1, merit0, merit1); 
  } 
}


void cBirthChamber::SetupGenotypeInfo(cOrganism* organism, cGenotype* parent0, cGenotype* parent1)
{
  // Setup the genotypes for both children...
  cGenotype* child_genotype =
    m_world->GetClassificationManager().GetGenotype(organism->GetGenome(), parent0, parent1);

  organism->SetGenotype(child_genotype);
  parent0->SetBreedStats(*child_genotype);
  
  // Defer the child genotype from being adjusted until after the child
  // has been placed into the population.
  child_genotype->IncDeferAdjust();
}

bool cBirthChamber::SubmitOffspring(cAvidaContext& ctx, const cMetaGenome& offspring, cOrganism* parent,
                                    tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array)
{
  cPhenotype& parent_phenotype = parent->GetPhenotype();

  if (parent_phenotype.DivideSex() == false) {
    return DoAsexBirth(ctx, offspring, *parent, child_array, merit_array);
  }

  // If we make it this far, this must be a sexual or a "waiting" asexual
  // organism (which is the same as sexual with 0 recombination points)
  
  // Find a waiting entry (locally or globally)
  cBirthEntry* old_entry = getSelectionHandler(offspring.GetHardwareType())->SelectOffspring(ctx, offspring, parent);

  // If we couldn't find a waiting entry, this one was saved -- stop here!
  if (old_entry == NULL) return false;

  // We have now found a waiting entry.  Mark it no longer waiting and use it.
  old_entry->timestamp = -1;

  // If we are NOT recombining, handle that here.
  if (parent_phenotype.CrossNum() == 0 || 
      ctx.GetRandom().GetDouble() > m_world->GetConfig().RECOMBINATION_PROB.Get()) {
    return DoPairAsexBirth(ctx, *old_entry, offspring, *parent, child_array, merit_array);
  }
  // If we made it this far, RECOMBINATION will happen!
  cGenome genome0 = old_entry->genome.GetGenome();
  cGenome genome1 = offspring.GetGenome();
  double meritOrEnergy0;
  double meritOrEnergy1;

  if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    meritOrEnergy0 = old_entry->energy4Offspring;
    meritOrEnergy1 = parent_phenotype.ExtractParentEnergy();
  } else {
    meritOrEnergy0 = old_entry->merit.GetDouble();
    meritOrEnergy1 = parent_phenotype.GetMerit().GetDouble();
  }

  // Check the modular recombination settings.  There are three variables:
  //  1: How many modules?  (0 = non-modular)
  //  2: Are the recombination regions continuous? (only used if modular)
  //  3: Can modules be shuffled during recombination? (only if non-continuous)

  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();
  const int continuous_regions = m_world->GetConfig().CONT_REC_REGS.Get();
  const int shuffle_regions = !m_world->GetConfig().CORESPOND_REC_REGS.Get();

  // If we are NOT modular...
  if (num_modules == 0) {
    DoBasicRecombination(ctx, genome0, genome1, meritOrEnergy0, meritOrEnergy1);
  }

  // If we ARE modular, and continuous...
  else if (continuous_regions == 1) {
    DoModularContRecombination(ctx, genome0, genome1, meritOrEnergy0, meritOrEnergy1);
  }

  // If we are NOT continuous, but NO shuffling...
  else if (shuffle_regions == 0) {
    DoModularNonContRecombination(ctx, genome0, genome1, meritOrEnergy0, meritOrEnergy1);
  }

  // If there IS shuffling (NON-continuous required)
  else {
    DoModularShuffleRecombination(ctx, genome0, genome1, meritOrEnergy0, meritOrEnergy1);
  }

  // Should there be a 2-fold cost to sex?

  const int two_fold_cost = m_world->GetConfig().TWO_FOLD_COST_SEX.Get();

  cGenotype* parent0_genotype = old_entry->parent_genotype;
  cGenotype* parent1_genotype = parent->GetGenotype();
  
  const int hw_type = offspring.GetHardwareType();
  const int inst_set_id = offspring.GetInstSetID();

  if (two_fold_cost == 0) {	// Build the two organisms.
    child_array.Resize(2);
    child_array[0] = new cOrganism(m_world, ctx, hw_type, inst_set_id, genome0);
    child_array[1] = new cOrganism(m_world, ctx, hw_type, inst_set_id, genome1);
    
    if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
      child_array[0]->GetPhenotype().SetEnergy(meritOrEnergy0);
      child_array[1]->GetPhenotype().SetEnergy(meritOrEnergy1);
      meritOrEnergy0 = child_array[0]->GetPhenotype().ConvertEnergyToMerit(child_array[0]->GetPhenotype().GetStoredEnergy());
      meritOrEnergy1 = child_array[1]->GetPhenotype().ConvertEnergyToMerit(child_array[1]->GetPhenotype().GetStoredEnergy());
    }
    
    merit_array.Resize(2);
    merit_array[0] = meritOrEnergy0;
    merit_array[1] = meritOrEnergy1;
    
    // Setup the genotypes for both children...
    SetupGenotypeInfo(child_array[0], parent0_genotype, parent1_genotype);
    SetupGenotypeInfo(child_array[1], parent1_genotype, parent0_genotype);

  }
  else { 			// Build only one organism	
    child_array.Resize(1);
    merit_array.Resize(1);

    if (ctx.GetRandom().GetDouble() < 0.5) {
      child_array[0] = new cOrganism(m_world, ctx, hw_type, inst_set_id, genome0);
      if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
        child_array[0]->GetPhenotype().SetEnergy(meritOrEnergy0);
        meritOrEnergy0 = child_array[0]->GetPhenotype().ConvertEnergyToMerit(child_array[0]->GetPhenotype().GetStoredEnergy());
      }
      merit_array[0] = meritOrEnergy0;

      // Setup the genotype for the child...
      SetupGenotypeInfo(child_array[0], parent0_genotype, parent1_genotype);
    } 
    else {
      child_array[0] = new cOrganism(m_world, ctx, hw_type, inst_set_id, genome1);
      if(m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
        child_array[0]->GetPhenotype().SetEnergy(meritOrEnergy1);
        meritOrEnergy1 = child_array[1]->GetPhenotype().ConvertEnergyToMerit(child_array[1]->GetPhenotype().GetStoredEnergy());
      }
      merit_array[0] = meritOrEnergy1;

      // Setup the genotype for the child...
      SetupGenotypeInfo(child_array[0], parent1_genotype, parent0_genotype);
    }
  } 

  // We are now also done with the parent genotype that we were saving in
  // the birth chamber, so we can un-defer that one.
  parent0_genotype->DecDeferAdjust();
  m_world->GetClassificationManager().AdjustGenotype(*parent0_genotype);

  return true;
}

