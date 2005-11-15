//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cBirthChamber.h"

#include "tArray.h"
#include "functions.h"
#include "cClassificationManager.h"
#include "cGenome.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cOrganism.h"
#include "cTools.h"
#include "cWorld.h"

cBirthChamber::cBirthChamber(cWorld* world) : m_world(world)
{
  const int num_orgs = m_world->GetConfig().WORLD_X.Get() * m_world->GetConfig().WORLD_Y.Get();
  const int num_demes = m_world->GetConfig().NUM_DEMES.Get(); 
  local_wait_entry.Resize(num_orgs);
  deme_wait_entry.Resize(num_demes);
  for (int i = 0; i < num_orgs; i++) {
    local_wait_entry[i].is_waiting = false;
  }
  for (int i = 0; i < num_demes; i++) {
    deme_wait_entry[i].is_waiting = false;
  }
  global_wait_entry.is_waiting = false;
}

cBirthChamber::~cBirthChamber()
{
}

bool cBirthChamber::GetNeighborWaiting(const int & parent_id,
				       int world_x,
				       int world_y)
{
  for (int i=-1; i<=1; i++) {
    for (int j=-1; j<=1; j++) { 
      const int neighbor_id = GridNeighbor(parent_id, world_x, world_y, i, j);
      if (local_wait_entry[neighbor_id].is_waiting == true) {
	return true;
      }
    }
  }
  return false;
}


int cBirthChamber::PickRandRecGenome(const int & parent_id, 
				     int world_x,
				     int world_y)
{
  bool done = false; 
  while (done ==false) {
    int test_neighbor = (int) m_world->GetRandom().GetUInt(9); 
    int i = test_neighbor / 3 - 1; 
    int j = test_neighbor % 3 - 1;
    int test_loc = GridNeighbor(parent_id,world_x, world_y, i, j); 		
    if (local_wait_entry[test_loc].is_waiting == true) {
      return test_loc;
    }
  }

  return -1;
}

bool cBirthChamber::RegionSwap(cCPUMemory & genome0, 
			       cCPUMemory & genome1,
			       int start0, int end0, 
			       int start1, int end1)
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
   if( new_size0 < MIN_CREATURE_SIZE || new_size0 > MAX_CREATURE_SIZE ||
       new_size1 < MIN_CREATURE_SIZE || new_size1 > MAX_CREATURE_SIZE ){
     return false;
   } 

   if (size0 > 0 && size1 > 0) {
     cGenome cross0 = cGenomeUtil::Crop(genome0, start0, end0);
     cGenome cross1 = cGenomeUtil::Crop(genome1, start1, end1);
     genome0.Replace(start0, size0, cross1);
     genome1.Replace(start1, size1, cross0);
   }
   else if (size0 > 0) {
     cGenome cross0 = cGenomeUtil::Crop(genome0, start0, end0);
     genome1.Replace(start1, size1, cross0);
   }
   else if (size1 > 0) {
     cGenome cross1 = cGenomeUtil::Crop(genome1, start1, end1);
     genome0.Replace(start0, size0, cross1);
   }

   return true;
}

bool cBirthChamber::GenomeSwap(cCPUMemory & genome0,
	                       cCPUMemory & genome1,
                  	       double & merit0,
                  	       double & merit1)
{
  cCPUMemory & genome0_tmp = genome0;
  genome0 = genome1; 
  genome1 = genome0_tmp; 

  double & merit0_tmp = merit0; 
  merit0 = merit1; 
  merit1 = merit0_tmp;

	// This had no return value, added 9/23/2004 DMB
  return true;
}


bool cBirthChamber::DoAsexBirth(const cGenome & child_genome,
				cOrganism & parent,
				tArray<cOrganism *> & child_array,
				tArray<cMerit> & merit_array)
{
  // This is asexual who doesn't need to wait in the birth chamber
  // just build the child and return.
  child_array.Resize(1);
  child_array[0] = new cOrganism(m_world, child_genome);
  merit_array.Resize(1);
  merit_array[0] = parent.GetPhenotype().GetMerit();

  // Setup the genotype for the child
  cGenotype * child_genotype = parent.GetGenotype();
  
  if (parent.GetPhenotype().CopyTrue() == false) {
    // Add this genotype with only one parent since its asexual.
    child_genotype = m_world->GetClassificationManager().GetGenotype(child_genome, parent.GetGenotype(), NULL);
  }

  child_array[0]->SetGenotype(child_genotype);
  parent.GetGenotype()->SetBreedStats(*child_genotype);
    
  child_genotype->IncDeferAdjust();

  return true;
}

bool cBirthChamber::DoPairAsexBirth(const cBirthEntry & old_entry,
				    const cGenome & new_genome,
				    cOrganism & parent,
				    tArray<cOrganism *> & child_array,
				    tArray<cMerit> & merit_array)
{
  // Build both child organisms...
  child_array.Resize(2);
  child_array[0] = new cOrganism(m_world, old_entry.genome);
  child_array[1] = new cOrganism(m_world, new_genome);

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

cBirthChamber::cBirthEntry *
  cBirthChamber::FindSexSizeWaiting(const cGenome & child_genome,
				    cOrganism & parent)
{
  const int child_length = child_genome.GetSize();

  // If this is a new largest genome, increase the array size.
  if (size_wait_entry.GetSize() <= child_length) {
    int old_wait_size = size_wait_entry.GetSize();
    size_wait_entry.Resize(child_length + 1);
    for (int i = old_wait_size; i <= child_length; i++) {
      size_wait_entry[i].is_waiting = false;
    }
  }

  // Determine if we have an offspring of this length waiting already...
  if (size_wait_entry[child_length].is_waiting == false) {
    cGenotype * parent_genotype = parent.GetGenotype();
    parent_genotype->IncDeferAdjust();
    size_wait_entry[child_length].genome = child_genome;
    size_wait_entry[child_length].merit = parent.GetPhenotype().GetMerit();
    size_wait_entry[child_length].parent_genotype = parent_genotype;
    size_wait_entry[child_length].is_waiting = true;
    return NULL; 				
  }

  // There is already a child waiting -- do crossover between the two.
  return &( size_wait_entry[child_length] ); 
}

cBirthChamber::cBirthEntry *
  cBirthChamber::FindSexMateSelectWaiting(const cGenome & child_genome,
					  cOrganism & parent)
{
  const int mate_id = parent.GetPhenotype().MateSelectID();

  // If this is a new largest ID, increase the array size.
  if (mate_select_wait_entry.GetSize() <= mate_id) {
    int old_wait_size = mate_select_wait_entry.GetSize();
    mate_select_wait_entry.Resize(mate_id + 1);
    for (int i = old_wait_size; i <= mate_id; i++) {
      mate_select_wait_entry[i].is_waiting = false;
    }
  }

  // Determine if we have an offspring of this length waiting already...
  if (mate_select_wait_entry[mate_id].is_waiting == false) {
    cGenotype * parent_genotype = parent.GetGenotype();
    parent_genotype->IncDeferAdjust();
    mate_select_wait_entry[mate_id].genome = child_genome;
    mate_select_wait_entry[mate_id].merit = parent.GetPhenotype().GetMerit();
    mate_select_wait_entry[mate_id].parent_genotype = parent_genotype;
    mate_select_wait_entry[mate_id].is_waiting = true;
    return NULL;
  }

  // There is already a child waiting -- do crossover between the two.
  return &( mate_select_wait_entry[mate_id] ); 
}

cBirthChamber::cBirthEntry *
  cBirthChamber::FindSexLocalWaiting(const cGenome & child_genome,
				   cOrganism & parent)
{
  // Collect some info for building the child.
  const int world_x = m_world->GetConfig().WORLD_X.Get();
  const int world_y = m_world->GetConfig().WORLD_Y.Get();
  const int parent_id = parent.PopInterface().GetCellID();
  
  // If nothing is waiting, store child locally.
  if (GetNeighborWaiting(parent_id, world_x, world_y) == false) { 
    cGenotype * parent_genotype = parent.GetGenotype();
    parent_genotype->IncDeferAdjust();
    local_wait_entry[parent_id].genome = child_genome;
    local_wait_entry[parent_id].merit = parent.GetPhenotype().GetMerit();
    local_wait_entry[parent_id].parent_genotype = parent_genotype;
    local_wait_entry[parent_id].is_waiting = true;
    return NULL; 				
  }

  // There is already a child waiting -- do crossover between the two.
  int found_location = PickRandRecGenome(parent_id, world_x, world_y);
  return &( local_wait_entry[found_location] ); 
}

cBirthChamber::cBirthEntry *
  cBirthChamber::FindSexDemeWaiting(const cGenome & child_genome,
				   cOrganism & parent)
{
  // Collect some info for building the child.
  const int world_x = m_world->GetConfig().WORLD_X.Get();
  const int world_y = m_world->GetConfig().WORLD_Y.Get();
  const int num_demes = m_world->GetConfig().NUM_DEMES.Get();
  const int parent_id = parent.PopInterface().GetCellID();
  
  const int parent_deme = (int) parent_id/(world_y*world_x/num_demes);

  // If nothing is waiting, store child locally.
  if (deme_wait_entry[parent_deme].is_waiting == false) { 
    cGenotype * parent_genotype = parent.GetGenotype();
    parent_genotype->IncDeferAdjust();
    deme_wait_entry[parent_deme].genome = child_genome;
    deme_wait_entry[parent_deme].merit = parent.GetPhenotype().GetMerit();
    deme_wait_entry[parent_deme].parent_genotype = parent_genotype;
    deme_wait_entry[parent_deme].is_waiting = true;
    return NULL; 				
  }

  // There is already a child waiting -- do crossover between the two.
  return &( deme_wait_entry[parent_deme] );

}

cBirthChamber::cBirthEntry * cBirthChamber::FindSexGlobalWaiting(const cGenome & child_genome,
						  cOrganism & parent)
{
  // If no other child is waiting, store this one.
  if (global_wait_entry.is_waiting == false){
    cGenotype * parent_genotype = parent.GetGenotype();
    parent_genotype->IncDeferAdjust();
    global_wait_entry.genome = child_genome; 
    global_wait_entry.merit = parent.GetPhenotype().GetMerit();
    global_wait_entry.parent_genotype = parent_genotype;
    global_wait_entry.is_waiting = true;
    return NULL;
  }

  // There is already a child waiting -- do crossover between the two.

  return &global_wait_entry;
}

void cBirthChamber::DoBasicRecombination(cCPUMemory & genome0,
					 cCPUMemory & genome1, 
					 double & merit0, double & merit1)
{
  double start_frac = m_world->GetRandom().GetDouble();
  double end_frac = m_world->GetRandom().GetDouble();
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

void cBirthChamber::DoModularContRecombination(cCPUMemory & genome0,
					       cCPUMemory & genome1, 
					       double & merit0,
					       double & merit1)
{
  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();

  int start_module = (int) (m_world->GetRandom().GetDouble() * num_modules);
  int end_module = (int) (m_world->GetRandom().GetDouble() * num_modules);

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

void cBirthChamber::DoModularNonContRecombination(cCPUMemory & genome0,
						  cCPUMemory & genome1, 
						  double & merit0,
						  double & merit1)
{
  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();

  int swap_count = 0;
  for (int i = 0; i < num_modules; i++) {
    if (m_world->GetRandom().GetDouble() < 0.5) {
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

void cBirthChamber::DoModularShuffleRecombination(cCPUMemory & genome0,
						  cCPUMemory & genome1, 
						  double & merit0,
						  double & merit1)
{
  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();
  tArray<bool> swapped_region(num_modules);
  swapped_region.SetAll(false);

  int swap_count = 0;
  for (int mod0 = 0; mod0 < num_modules; mod0++) {
    if (m_world->GetRandom().GetDouble() < 0.5) {
      swap_count++;

      // Collect start and end info for current module
      double start0_frac = ((double) mod0) / (double) num_modules;
      double end0_frac = ((double) mod0+1) / (double) num_modules;
      int start0 = (int) (start0_frac * (double) genome0.GetSize());
      int end0   = (int) (end0_frac * (double) genome0.GetSize());

      // Pick module from other genome...
      int mod1 = m_world->GetRandom().GetUInt(num_modules);
      while (swapped_region[mod1] == true) {
	mod1 = m_world->GetRandom().GetUInt(num_modules);
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

bool cBirthChamber::SubmitOffspring(const cGenome & child_genome,
				    cOrganism & parent,
				    tArray<cOrganism *> & child_array,
				    tArray<cMerit> & merit_array)
{
  cPhenotype & parent_phenotype = parent.GetPhenotype();

  if (parent_phenotype.DivideSex() == false) {
    return DoAsexBirth(child_genome, parent, child_array, merit_array);
  }

  // If we make it this far, this must be a sexual or a "waiting" asexual
  // organism (which is the same as sexual with 0 recombination points)

  const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();
  
  // Find a waiting entry (locally or globally)
  cBirthEntry * old_entry = NULL;
  // First check if the birth method is one of the local ones... 
  if (birth_method < NUM_LOCAL_POSITION_CHILD) { 
    old_entry = FindSexLocalWaiting(child_genome, parent);
  }
  // ... then check if population is split into demes
  else if (birth_method == POSITION_CHILD_DEME_RANDOM) {
    old_entry = FindSexDemeWaiting(child_genome, parent);
  }

  // If none of the previous conditions were met, it must be global.
  // ...check if recombination must be with organisms of the same length
  else if (m_world->GetConfig().SAME_LENGTH_SEX.Get() != 0) {
    old_entry = FindSexSizeWaiting(child_genome, parent);
  }

  // ...check if we have mate selection
  else if (parent_phenotype.MateSelectID() >= 0) {
    old_entry = FindSexMateSelectWaiting(child_genome, parent);
  }

  // If everything failed until this point, use default global.
  else {
    old_entry = FindSexGlobalWaiting(child_genome, parent);
  }

  // If we couldn't find a waiting entry, this one was saved -- stop here!
  if (old_entry == NULL) {
    return false;
  }

  // We have now found a waiting entry.  Mark it no longer waiting and use it.
  old_entry->is_waiting = false;

  // If we are NOT recombining, handle that here.
  if (parent_phenotype.CrossNum() == 0 || 
      m_world->GetRandom().GetDouble() > m_world->GetConfig().RECOMBINATION_PROB.Get()) {
    return DoPairAsexBirth(*old_entry, child_genome, parent, 
			   child_array, merit_array);
  }

  // If we made it this far, RECOMBINATION will happen!
  cCPUMemory genome0 = old_entry->genome;
  cCPUMemory genome1 = child_genome;
  double merit0 = old_entry->merit.GetDouble();
  double merit1 = parent_phenotype.GetMerit().GetDouble();


  // Check the modular recombination settings.  There are three variables:
  //  1: How many modules?  (0 = non-modular)
  //  2: Are the recombination regions continuous? (only used if modular)
  //  3: Can modules be shuffled during recombination? (only if non-continuous)

  const int num_modules = m_world->GetConfig().MODULE_NUM.Get();
  const int continuous_regions = m_world->GetConfig().CONT_REC_REGS.Get();
  const int shuffle_regions = !m_world->GetConfig().CORESPOND_REC_REGS.Get();

  // If we are NOT modular...
  if (num_modules == 0) {
    DoBasicRecombination(genome0, genome1, merit0, merit1);
  }

  // If we ARE modular, and continuous...
  else if (continuous_regions == 1) {
    DoModularContRecombination(genome0, genome1, merit0, merit1);
  }

  // If we are NOT continuous, but NO shuffling...
  else if (shuffle_regions == 0) {
    DoModularNonContRecombination(genome0, genome1, merit0, merit1);
  }

  // If there IS shuffling (NON-continuous required)
  else {
    DoModularShuffleRecombination(genome0, genome1, merit0, merit1);
  }

  // Should there be a 2-fold cost to sex?

  const int two_fold_cost = m_world->GetConfig().TWO_FOLD_COST_SEX.Get();

  cGenotype * parent0_genotype = old_entry->parent_genotype;
  cGenotype * parent1_genotype = parent.GetGenotype();

  if (two_fold_cost == 0) {	// Build the two organisms.
    child_array.Resize(2);
    child_array[0] = new cOrganism(m_world, genome0);
    child_array[1] = new cOrganism(m_world, genome1);
    
    merit_array.Resize(2);
    merit_array[0] = merit0;
    merit_array[1] = merit1;

    // Setup the genotypes for both children...
    SetupGenotypeInfo(child_array[0], parent0_genotype, parent1_genotype);
    SetupGenotypeInfo(child_array[1], parent1_genotype, parent0_genotype);

  }
  else { 			// Build only one organism	
    child_array.Resize(1);
    merit_array.Resize(1);

    if (m_world->GetRandom().GetDouble() < 0.5) {
      child_array[0] = new cOrganism(m_world, genome0);
      merit_array[0] = merit0;

      // Setup the genotype for the child...
      SetupGenotypeInfo(child_array[0], parent0_genotype, parent1_genotype);
    } 
    else {
      child_array[0] = new cOrganism(m_world, genome1);
      merit_array[0] = merit1;

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

