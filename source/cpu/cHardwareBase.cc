/*
 *  cHardwareBase.cc
 *  Avida
 *
 *  Called "hardware_base.cc" prior to 11/17/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#include "cHardwareBase.h"

#include "cAvidaContext.h"
#include "cCodeLabel.h"
#include "cCPUTestInfo.h"
#include "cHardwareManager.h"
#include "cHeadCPU.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cRandom.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "nHardware.h"
#include "tArrayUtils.h"

using namespace AvidaTools;


cHardwareBase::cHardwareBase(cWorld* world, cOrganism* in_organism, cInstSet* inst_set)
: m_world(world), m_organism(in_organism), m_inst_set(inst_set), m_tracer(NULL)
, m_has_costs(inst_set->HasCosts()), m_has_ft_costs(inst_set->HasFTCosts())
, m_has_energy_costs(m_inst_set->HasEnergyCosts())
{
	m_task_switching_cost=0;
	int switch_cost =  world->GetConfig().TASK_SWITCH_PENALTY.Get();
	m_has_any_costs = (m_has_costs | m_has_ft_costs | m_has_energy_costs | switch_cost);
  m_implicit_repro_active = (m_world->GetConfig().IMPLICIT_REPRO_TIME.Get() ||
                             m_world->GetConfig().IMPLICIT_REPRO_CPU_CYCLES.Get() ||
                             m_world->GetConfig().IMPLICIT_REPRO_BONUS.Get() ||
                             m_world->GetConfig().IMPLICIT_REPRO_END.Get() ||
                             m_world->GetConfig().IMPLICIT_REPRO_ENERGY.Get());
	
  assert(m_organism != NULL);
}


void cHardwareBase::Reset(cAvidaContext& ctx)
{
  m_organism->HardwareReset(ctx);
  
  m_inst_cost = 0;
  
  const int num_inst_cost = m_inst_set->GetSize();
  
  if (m_has_ft_costs) {
    m_inst_ft_cost.Resize(num_inst_cost);
    for (int i = 0; i < num_inst_cost; i++) m_inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
  
  if (m_has_energy_costs) {
    m_inst_energy_cost.Resize(num_inst_cost);
    for (int i = 0; i < num_inst_cost; i++) m_inst_energy_cost[i] = m_inst_set->GetEnergyCost(cInstruction(i));
  }
  
  internalReset();
}

int cHardwareBase::calcExecutedSize(const int parent_size)
{
  int executed_size = 0;
  const cCPUMemory& memory = GetMemory();
  for (int i = 0; i < parent_size; i++) {
    if (memory.FlagExecuted(i)) executed_size++;
  }  
  return executed_size;
}

bool cHardwareBase::Divide_CheckViable(cAvidaContext& ctx, const int parent_size, const int child_size, bool using_repro)
{
#define ORG_FAULT(error) if (ctx.OrgFaultReporting()) m_organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR, error)
  
  // Make sure the organism is okay with dividing now...
  // Moved to end of function @LZ

  // Make sure that neither parent nor child will be below the minimum size.  
  const int genome_size = m_organism->GetGenome().GetSize();
  const double size_range = m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get();
  const int min_size = Max(MIN_GENOME_LENGTH, static_cast<int>(genome_size / size_range));
  const int max_size = Min(MAX_GENOME_LENGTH, static_cast<int>(genome_size * size_range));
  
  if (child_size < min_size || child_size > max_size) {
    ORG_FAULT(cStringUtil::Stringf("Invalid offspring length (%d)", child_size));
    return false; // (divide fails)
  }
  if (parent_size < min_size || parent_size > max_size) {
    ORG_FAULT(cStringUtil::Stringf("Invalid post-divide length (%d)",parent_size));
    return false; // (divide fails)
  }
  
  // Absolute minimum and maximum child/parent size limits -- @JEB
  const int max_genome_size = m_world->GetConfig().MAX_GENOME_SIZE.Get();
  const int min_genome_size = m_world->GetConfig().MIN_GENOME_SIZE.Get();
  if ( (min_genome_size && (child_size < min_genome_size)) || (max_genome_size && (child_size > max_genome_size)) ) {
    ORG_FAULT(cStringUtil::Stringf("Invalid absolute offspring length (%d)",child_size));
    return false; // (divide fails)
  }
  
  if ( (min_genome_size && (parent_size < min_genome_size)) || (max_genome_size && (parent_size > max_genome_size)) ) {
    ORG_FAULT(cStringUtil::Stringf("Invalid absolute post-divide length (%d)",parent_size));
    return false; // (divide fails)
  }
  
  // Count the number of lines executed in the parent, and make sure the
  // specified fraction has been reached.
  
  const int executed_size = calcExecutedSize(parent_size);
  const int min_exe_lines = static_cast<int>(parent_size * m_world->GetConfig().MIN_EXE_LINES.Get());
  if (executed_size < min_exe_lines) {
    ORG_FAULT(cStringUtil::Stringf("Too few executed lines (%d < %d)", executed_size, min_exe_lines));
    return false; // (divide fails)
  }
  
  // Repro organisms mark their entire genomes as copied
  int copied_size = parent_size;
  if (!using_repro) {
    // Normal organisms check to see how much was copied
    copied_size = calcCopiedSize(parent_size, child_size); // Fails for REPRO organisms
    const int min_copied = static_cast<int>(child_size * m_world->GetConfig().MIN_COPIED_LINES.Get());
    
    if (copied_size < min_copied) {
      ORG_FAULT(cStringUtil::Stringf("Too few copied commands (%d < %d)", copied_size, min_copied));
      return false; // (divide fails)
    }
  }

  if (m_world->GetConfig().USE_FORM_GROUPS.Get()) {
    if (!m_organism->HasOpinion()) {
      if (m_world->GetConfig().DEFAULT_GROUP.Get() != -1) {
        m_organism->SetOpinion(m_world->GetConfig().DEFAULT_GROUP.Get());
      } else {
        // No default group, so divide fails (group opinion is required by cPopulation::ActivateOffspring)
        return false;
      }
    }
  }
  
	if (m_organism->Divide_CheckViable() == false) 
	{
		if (m_world->GetConfig().DIVIDE_FAILURE_RESETS.Get())
		{
			internalResetOnFailedDivide();
		}
		
		return false; // (divide fails)
	}

  
  // Save the information we collected here...
  cPhenotype& phenotype = m_organism->GetPhenotype();
  phenotype.SetLinesExecuted(executed_size);
  phenotype.SetLinesCopied(copied_size);
  
  // Determine the fitness of this organism as compared to its parent...
  if (m_world->GetTestSterilize() && !phenotype.IsInjected()) {
    const int merit_base = phenotype.CalcSizeMerit();
    const double cur_fitness = merit_base * phenotype.GetCurBonus() / phenotype.GetTimeUsed();
    const double fitness_ratio = cur_fitness / phenotype.GetLastFitness();
    const tArray<int>& childtasks = phenotype.GetCurTaskCount();
    const tArray<int>& parenttasks = phenotype.GetLastTaskCount();
    
    bool sterilize = false;
    
    if (fitness_ratio < nHardware::FITNESS_NEUTRAL_MIN) {
      if (ctx.GetRandom().P(m_organism->GetSterilizeNeg())) sterilize = true;
    } else if (fitness_ratio <= nHardware::FITNESS_NEUTRAL_MAX) {
      if (ctx.GetRandom().P(m_organism->GetSterilizeNeut())) sterilize = true;
    } else {
      if (ctx.GetRandom().P(m_organism->GetSterilizePos())) sterilize = true;
    }
    
    // for sterilize task loss *SLG
    if (ctx.GetRandom().P(m_organism->GetSterilizeTaskLoss()))
    {
      bool del = false;
      bool added = false;
      for (int i=0; i<childtasks.GetSize(); i++)
      {
        if (childtasks[i] > parenttasks[i]) {
          added = true;
          break;
        }
        else if (childtasks[i] < parenttasks[i])
          del = true;
      }
      sterilize = (del & !added);
    }
    
    if (sterilize) {
      // Don't let this organism have this or any more children!
      phenotype.IsFertile() = false;
      return false;
    }    
  }
  
  return true; // (divide succeeds!)
#undef ORG_FAULT
}


/*
 Return the number of mutations that occur on divide.  AWC 06/29/06
 Limit the number of mutations that occur to be less than or equal to maxmut (defaults to INT_MAX)
 */
int cHardwareBase::Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier, const int maxmut)
{
  int max_genome_size = m_world->GetConfig().MAX_GENOME_SIZE.Get();
  int min_genome_size = m_world->GetConfig().MIN_GENOME_SIZE.Get();
  if (!max_genome_size || max_genome_size > MAX_GENOME_LENGTH) max_genome_size = MAX_GENOME_LENGTH;
  if (!min_genome_size || min_genome_size < MIN_GENOME_LENGTH) min_genome_size = MIN_GENOME_LENGTH;
  
  int totalMutations = 0;
  cSequence& offspring_genome = m_organism->OffspringGenome().GetSequence();
  
  m_organism->GetPhenotype().SetDivType(mut_multiplier);
  
  // @JEB 
  // All slip mutations should happen first, so that there is a chance
  // of getting a point mutation within one copy in the same divide.
  
  // Divide Slip Mutations - NOT COUNTED.
  if (m_organism->TestDivideSlip(ctx)) doSlipMutation(ctx, offspring_genome);
  
  // Poisson Slip Mutations - NOT COUNTED
  unsigned int num_poisson_slip = m_organism->NumDividePoissonSlip(ctx);
  for (unsigned int i = 0; i < num_poisson_slip; i++) { doSlipMutation(ctx, offspring_genome);  }
  
  // Slip Mutations (per site) - NOT COUNTED
  if (m_organism->GetDivSlipProb() > 0) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), 
                                                  m_organism->GetDivSlipProb() / mut_multiplier);
    for (int i = 0; i < num_mut; i++) doSlipMutation(ctx, offspring_genome);
  }
	
	// HGT Mutations - NOT COUNTED
	// HGT is a location-dependent random process; each type is tested over in
	// cPopulationInterface.
	if(m_world->GetConfig().ENABLE_HGT.Get()) {
		m_organism->GetOrgInterface().DoHGTMutation(ctx, m_organism->OffspringGenome());
	}
  
  // Divide Mutations
  if (m_organism->TestDivideMut(ctx) && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
    char before_mutation = offspring_genome[mut_line].GetSymbol();
    offspring_genome[mut_line] = m_inst_set->GetRandomInst(ctx);
    offspring_genome.GetMutationSteps().AddSubstitutionMutation(mut_line, before_mutation, offspring_genome[mut_line].GetSymbol());
    totalMutations++;
  }
  
  
  // Poisson Divide Mutations
  unsigned int num_poisson_mut = m_organism->NumDividePoissonMut(ctx);
  for (unsigned int i=0; i<num_poisson_mut; i++)
  {
    if (totalMutations >= maxmut) break;
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
    char before_mutation = offspring_genome[mut_line].GetSymbol();
    offspring_genome[mut_line] = m_inst_set->GetRandomInst(ctx);
    offspring_genome.GetMutationSteps().AddSubstitutionMutation(mut_line, before_mutation, offspring_genome[mut_line].GetSymbol());
    totalMutations++;
  }
  
  
  // Divide Insertions
  if (m_organism->TestDivideIns(ctx) && offspring_genome.GetSize() < max_genome_size && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize() + 1);
    offspring_genome.Insert(mut_line, m_inst_set->GetRandomInst(ctx));
    offspring_genome.GetMutationSteps().AddInsertionMutation(mut_line, offspring_genome[mut_line].GetSymbol());
    totalMutations++;
  }
  
  
  // Poisson Divide Insertions
  unsigned int num_poisson_ins = m_organism->NumDividePoissonIns(ctx);
  for (unsigned int i=0; i<num_poisson_ins; i++)
  {
    if (offspring_genome.GetSize() >= max_genome_size) break;
    if (totalMutations >= maxmut) break;
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize() + 1);
    offspring_genome.Insert(mut_line, m_inst_set->GetRandomInst(ctx));
    offspring_genome.GetMutationSteps().AddInsertionMutation(mut_line, offspring_genome[mut_line].GetSymbol());
    totalMutations++;
  }
  
  
  // Divide Deletions
  if (m_organism->TestDivideDel(ctx) && offspring_genome.GetSize() > min_genome_size && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
    offspring_genome.GetMutationSteps().AddDeletionMutation(mut_line, offspring_genome[mut_line].GetSymbol());
    offspring_genome.Remove(mut_line);
    totalMutations++;
  }
  
  
  // Poisson Divide Deletions
  unsigned int num_poisson_del = m_organism->NumDividePoissonDel(ctx);
  for (unsigned int i=0; i<num_poisson_del; i++)
  {
    if (offspring_genome.GetSize() <= min_genome_size) break;
    if (totalMutations >= maxmut) break;
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
    offspring_genome.GetMutationSteps().AddDeletionMutation(mut_line, offspring_genome[mut_line].GetSymbol());
    offspring_genome.Remove(mut_line);
    totalMutations++;
  }
  
  
  // Divide Uniform Mutations
  if (m_organism->TestDivideUniform(ctx) && totalMutations < maxmut) {
    if (doUniformMutation(ctx, offspring_genome)) totalMutations++;
  }
  
  
  // Divide Mutations (per site)
  if (m_organism->GetDivMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), 
                                                  m_organism->GetDivMutProb() / mut_multiplier);
    // If we have lines to mutate...
    if (num_mut > 0 && totalMutations < maxmut) {
      for (int i = 0; i < num_mut && totalMutations < maxmut; i++) {
        int site = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
        char before_mutation = offspring_genome[site].GetSymbol();
        offspring_genome[site] = m_inst_set->GetRandomInst(ctx);
        offspring_genome.GetMutationSteps().AddSubstitutionMutation(site, before_mutation, offspring_genome[site].GetSymbol());
        totalMutations++;
      }
    }
  }
  
  // Insert Mutations (per site)
  if (m_organism->GetDivInsProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), m_organism->GetDivInsProb());
    
    // If would make creature too big, insert up to max_genome_size
    if (num_mut + offspring_genome.GetSize() > max_genome_size) {
      num_mut = max_genome_size - offspring_genome.GetSize();
    }
    
    // If we have lines to insert...
    if (num_mut > 0) {
      // Build a sorted list of the sites where mutations occured
      tArray<int> mut_sites(num_mut);
      for (int i = 0; i < num_mut; i++) mut_sites[i] = ctx.GetRandom().GetUInt(offspring_genome.GetSize() + 1);
      tArrayUtils::QSort(mut_sites);
      
      // Actually do the mutations (in reverse sort order)
      for (int i = mut_sites.GetSize() - 1; i >= 0; i--) {
        offspring_genome.Insert(mut_sites[i], m_inst_set->GetRandomInst(ctx));
        offspring_genome.GetMutationSteps().AddInsertionMutation(mut_sites[i], offspring_genome[mut_sites[i]].GetSymbol());
      }
      
      totalMutations += num_mut;
    }
  }
  
  
  // Delete Mutations (per site)
  if (m_organism->GetDivDelProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), m_organism->GetDivDelProb());
    
    // If would make creature too small, delete down to min_genome_size
    if (offspring_genome.GetSize() - num_mut < min_genome_size) {
      num_mut = offspring_genome.GetSize() - min_genome_size;
    }
    
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
      offspring_genome.GetMutationSteps().AddDeletionMutation(site, offspring_genome[site].GetSymbol());
      offspring_genome.Remove(site);
    }
    
    totalMutations += num_mut;
  }
  
  
  
  // Uniform Mutations (per site)
  if (m_organism->GetDivUniformProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), 
                                                  m_organism->GetDivUniformProb() / mut_multiplier);
    
    // If we have lines to mutate...
    if (num_mut > 0 && totalMutations < maxmut) {
      for (int i = 0; i < num_mut && totalMutations < maxmut; i++) {
        if (doUniformMutation(ctx, offspring_genome)) totalMutations++;
      }
    }
  }
  
  // Mutations in the parent's genome
  if (m_organism->GetParentMutProb() > 0 && totalMutations < maxmut) {
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (m_organism->TestParentMut(ctx)) {
        GetMemory()[i] = m_inst_set->GetRandomInst(ctx);
        totalMutations++; //Unlike the others we can't be sure this was done only on divide -- AWC 06/29/06
        
      }
    }
  }
  
  return totalMutations;
}


bool cHardwareBase::doUniformMutation(cAvidaContext& ctx, cSequence& genome)
{
  
  int mut = ctx.GetRandom().GetUInt((m_inst_set->GetSize() * 2) + 1);
  
  if (mut < m_inst_set->GetSize()) { // point
    int site = ctx.GetRandom().GetUInt(genome.GetSize());
    genome[site] = cInstruction(mut);
  } else if (mut == m_inst_set->GetSize()) { // delete
    int min_genome_size = m_world->GetConfig().MIN_GENOME_SIZE.Get();
    if (!min_genome_size || min_genome_size < MIN_GENOME_LENGTH) min_genome_size = MIN_GENOME_LENGTH;
    if (genome.GetSize() == min_genome_size) return false;
    int site = ctx.GetRandom().GetUInt(genome.GetSize());
    genome.Remove(site);
  } else { // insert
    int max_genome_size = m_world->GetConfig().MAX_GENOME_SIZE.Get();
    if (!max_genome_size || max_genome_size > MAX_GENOME_LENGTH) max_genome_size = MAX_GENOME_LENGTH;
    if (genome.GetSize() == max_genome_size) return false;
    int site = ctx.GetRandom().GetUInt(genome.GetSize() + 1);
    genome.Insert(site, cInstruction(mut - m_inst_set->GetSize() - 1));
  }
  
  return true;
}

void cHardwareBase::doUniformCopyMutation(cAvidaContext& ctx, cHeadCPU& head)
{
  int mut = ctx.GetRandom().GetUInt((m_inst_set->GetSize() * 2) + 1);
  
  if (mut < m_inst_set->GetSize()) head.SetInst(cInstruction(mut));
  else if (mut == m_inst_set->GetSize()) head.RemoveInst();
  else head.InsertInst(cInstruction(mut - m_inst_set->GetSize() - 1));
}



// Slip Mutations
// As if the read head jumped from one random position of the offspring
// to another random position and continued reading to the end.
// This can cause large deletions or tandem duplications.
// Unlucky organisms might exceed the allowed length (randomly) if these mutations occur.
void cHardwareBase::doSlipMutation(cAvidaContext& ctx, cSequence& genome, int from)
{
  cSequence genome_copy = cSequence(genome);
  
  // All combinations except beginning to past end allowed
  if (from < 0) from = ctx.GetRandom().GetInt(genome_copy.GetSize() + 1);
  int to = (from == 0) ? ctx.GetRandom().GetInt(genome_copy.GetSize()) : ctx.GetRandom().GetInt(genome_copy.GetSize() + 1);
  
  // Resize child genome
  int insertion_length = (from - to);
  genome.Resize(genome.GetSize() + insertion_length);
  
  // Fill insertion
  if (insertion_length > 0) {
    tArray<bool> copied_so_far(insertion_length);
    copied_so_far.SetAll(false);
    for (int i = 0; i < insertion_length; i++) {
      switch (m_world->GetConfig().SLIP_FILL_MODE.Get()) {
          //Duplication
        case 0:
          genome[from + i] = genome_copy[to + i];
          break;
          
          //Empty (nop-X)
        case 1:
          genome[from + i] = m_inst_set->GetInst("nop-X");
          break;
          
          //Random
        case 2:
          genome[from + i] = m_inst_set->GetRandomInst(ctx);
          break;
          
          //Scrambled order
        case 3:
        {
          int copy_index = m_world->GetRandom().GetInt(insertion_length - i);
          int test = 0;
          int passed = copy_index;
          while (passed >= 0) {
            if (copied_so_far[test]) {
              copy_index++; 
            } else { //this one hasn't been chosen, so we count it.
              passed--;
            }
            test++;
          }
          genome[from + i] = genome[to + copy_index];
          copied_so_far[copy_index] = true;
        }
          break;
          
          //Empty (nop-C)
        case 4:
          genome[from + i] = m_inst_set->GetInst("nop-C");
          break;
          
        default:
          m_world->GetDriver().RaiseException("Unknown SLIP_FILL_MODE\n");
      }
    }
  }
  
  // Deletion / remaining genome
  if (insertion_length < 0) insertion_length = 0;
  for (int i = insertion_length; i < genome_copy.GetSize() - to; i++) genome[from + i] = genome_copy[to + i];
  genome.GetMutationSteps().AddSlipMutation(from, to);
  
  if (m_world->GetVerbosity() >= VERBOSE_DETAILS) {
    cout << "SLIP MUTATION from " << from << " to " << to << endl;
    cout << "Parent: " << genome_copy.AsString()   << endl;
    cout << "Offspring: " << genome.AsString() << endl;
  }
}



/*
 Return the number of mutations that occur on divide.  AWC 06/29/06
 Limit the number of mutations that occur to be less than or equat to maxmut (defaults to INT_MAX)
 */
unsigned cHardwareBase::Divide_DoExactMutations(cAvidaContext& ctx, double mut_multiplier, const int pointmut)
{
  int maxmut = pointmut;
  int totalMutations = 0;
  cSequence& child_genome = m_organism->OffspringGenome().GetSequence();
  
  m_organism->GetPhenotype().SetDivType(mut_multiplier);
  
  // Divide Mutations
  if (totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize());
    child_genome[mut_line] = m_inst_set->GetRandomInst(ctx);
    totalMutations++;
  }
  
  // Divide Mutations (per site)
  if (m_organism->GetDivMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = pointmut;
    // If we have lines to mutate...
    if (num_mut > 0 && totalMutations < maxmut) {
      for (int i = 0; i < num_mut && totalMutations < maxmut; i++) {
        int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
        child_genome[site] = m_inst_set->GetRandomInst(ctx);
        totalMutations++;
        cerr << "Resampling here " << totalMutations << endl;
      }
    }
  }
  
  return totalMutations;
}


// test whether the offspring creature contains an advantageous mutation.
/*
 Return true iff only a reversion is performed -- returns false is sterilized regardless of whether or 
 not a reversion is performed.  AWC 06/29/06
 */
bool cHardwareBase::Divide_TestFitnessMeasures(cAvidaContext& ctx)
{
  cPhenotype & phenotype = m_organism->GetPhenotype();
  phenotype.CopyTrue() = ( m_organism->OffspringGenome() == m_organism->GetGenome() );
  phenotype.ChildFertile() = true;
	
  // Only continue if we're supposed to do a fitness test on divide...
  if (m_organism->GetTestOnDivide() == false) return false;
	
  // If this was a perfect copy, then we don't need to worry about any other
  // tests...  Theoretically, we need to worry about the parent changing,
  // but as long as the child is always compared to the original genotype,
  // this won't be an issue.
  if (phenotype.CopyTrue() == true) return false;
	
  const double parent_fitness = m_organism->GetTestFitness(ctx);
  const tArray<int>& parenttasks = phenotype.GetCurTaskCount();
  const double neut_min = parent_fitness * (1.0 - m_organism->GetNeutralMin());
  const double neut_max = parent_fitness * (1.0 + m_organism->GetNeutralMax());
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cCPUTestInfo test_info;
  test_info.UseRandomInputs();
  testcpu->TestGenome(ctx, test_info, m_organism->OffspringGenome());
  const double child_fitness = test_info.GetGenotypeFitness();
  delete testcpu;
  
  bool revert = false;
  bool sterilize = false;
  
  // If implicit mutations are turned off, make sure this won't spawn one.
  if (m_organism->GetSterilizeUnstable() == true) {
    if (test_info.GetMaxDepth() > 0) sterilize = true;
  }
  
  if (child_fitness == 0.0) {
    // Fatal mutation... test for reversion.
    if (ctx.GetRandom().P(m_organism->GetRevertFatal())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizeFatal())) sterilize = true;
  } else if (child_fitness < neut_min) {
    if (ctx.GetRandom().P(m_organism->GetRevertNeg())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizeNeg())) sterilize = true;
  } else if (child_fitness <= neut_max) {
    if (ctx.GetRandom().P(m_organism->GetRevertNeut())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizeNeut())) sterilize = true;
  } else {
    if (ctx.GetRandom().P(m_organism->GetRevertPos())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizePos())) sterilize = true;
  }
  
  int RorS = 0;	// 0 = neither, 1 = revert, 2 = sterilize
  if (ctx.GetRandom().P(m_organism->GetRevertTaskLoss()))
    RorS = 1;
  else if (ctx.GetRandom().P(m_organism->GetSterilizeTaskLoss()))
    RorS = 2;
  // check if child has lost any tasks parent had AND not gained any new tasks
  if (RorS) {
    const tArray<int>& childtasks = test_info.GetTestPhenotype().GetLastTaskCount();
    bool del = false;
    bool added = false;
    for (int i=0; i<childtasks.GetSize(); i++)
    {
      if (childtasks[i] > parenttasks[i]) {
        added = true;
        break;
      }
      else if (childtasks[i] < parenttasks[i])
        del = true;
    }
    if (RorS == 1) 
      revert = (del & !added);
    else 
      sterilize = (del & !added);
  }
  
  // Ideally, we won't have reversions and sterilizations turned on at the
  // same time, but if we do, give revert the priority.
  if (revert == true) {
    m_organism->OffspringGenome() = m_organism->GetGenome();
  }
  
  if (sterilize == true) {
    m_organism->GetPhenotype().ChildFertile() = false;
  }
  
  return (!sterilize) && revert;
}

// test whether the offspring creature contains an advantageous mutation.
/*
 Return true iff only a reversion is performed -- returns false is sterilized regardless of whether or 
 not a reversion is performed.  AWC 06/29/06
 */
bool cHardwareBase::Divide_TestFitnessMeasures1(cAvidaContext& ctx)
{
  cPhenotype & phenotype = m_organism->GetPhenotype();
  phenotype.CopyTrue() = (m_organism->OffspringGenome() == m_organism->GetGenome());
  phenotype.ChildFertile() = true;
	
  // Only continue if we're supposed to do a fitness test on divide...
  if (m_organism->GetTestOnDivide() == false) return false;
	
  // If this was a perfect copy, then we don't need to worry about any other
  // tests...  Theoretically, we need to worry about the parent changing,
  // but as long as the child is always compared to the original genotype,
  // this won't be an issue.
  if (phenotype.CopyTrue() == true) return false;
	
  const double parent_fitness = m_organism->GetTestFitness(ctx);
  const tArray<int>& parenttasks = phenotype.GetCurTaskCount();
  const double neut_min = parent_fitness * (1.0 - m_organism->GetNeutralMin());
  const double neut_max = parent_fitness * (1.0 + m_organism->GetNeutralMax());
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cCPUTestInfo test_info;
  test_info.UseRandomInputs();
  testcpu->TestGenome(ctx, test_info, m_organism->OffspringGenome());
  const double child_fitness = test_info.GetGenotypeFitness();
  delete testcpu;
  
  bool revert = false;
  bool sterilize = false;
  
  // If implicit mutations are turned off, make sure this won't spawn one.
  if (m_organism->GetSterilizeUnstable() > 0) {
    if (test_info.GetMaxDepth() > 0) sterilize = true;
  }
  
  if (m_organism->GetSterilizeUnstable() > 1 && !test_info.IsViable()) {
    sterilize = true;
  }
  
  if (child_fitness == 0.0) {
    // Fatal mutation... test for reversion.
    if (ctx.GetRandom().P(m_organism->GetRevertFatal())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizeFatal())) sterilize = true;
  } else if (child_fitness < neut_min) {
    if (ctx.GetRandom().P(m_organism->GetRevertNeg())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizeNeg())) sterilize = true;
  } else if (child_fitness <= neut_max) {
    if (ctx.GetRandom().P(m_organism->GetRevertNeut())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizeNeut())) sterilize = true;
  } else {
    if (ctx.GetRandom().P(m_organism->GetRevertPos())) revert = true;
    if (ctx.GetRandom().P(m_organism->GetSterilizePos())) sterilize = true;
  }
  
  int RorS = 0;	// 0 = neither, 1 = revert, 2 = sterilize
  if (ctx.GetRandom().P(m_organism->GetRevertTaskLoss()))
	  RorS = 1;
  else if (ctx.GetRandom().P(m_organism->GetSterilizeTaskLoss()))
	  RorS = 2;
  // check if child has lost any tasks parent had AND not gained any new tasks
  if (RorS) {
	  const tArray<int>& childtasks = test_info.GetTestPhenotype().GetLastTaskCount();
	  bool del = false;
	  bool added = false;
	  for (int i=0; i<childtasks.GetSize(); i++)
	  {
		  if (childtasks[i] > parenttasks[i]) {
			  added = true;
			  break;
		  }
		  else if (childtasks[i] < parenttasks[i])
			  del = true;
	  }
	  if (RorS == 1) 
		  revert = (del & !added);
	  else 
		  sterilize = (del & !added);
  }
  
  // Ideally, we won't have reversions and sterilizations turned on at the
  // same time, but if we do, give revert the priority.
  if (revert == true) {
	  m_organism->OffspringGenome() = m_organism->GetGenome();
  }
  
  if (sterilize == true) {
	  m_organism->GetPhenotype().ChildFertile() = false;
  }
  
  return (!sterilize) && revert;
}

int cHardwareBase::PointMutate(cAvidaContext& ctx, const double mut_rate)
{
  cCPUMemory& memory = GetMemory();
  const int num_muts = ctx.GetRandom().GetRandBinomial(memory.GetSize(), mut_rate);
  
  for (int i = 0; i < num_muts; i++) {
    const int pos = ctx.GetRandom().GetUInt(memory.GetSize());
    memory[pos] = m_inst_set->GetRandomInst(ctx);
    memory.SetFlagMutated(pos);
    memory.SetFlagPointMut(pos);
  }
  
  return num_muts;
}


cHeadCPU cHardwareBase::FindLabelFull(const cCodeLabel& label)
{
  assert(label.GetSize() > 0); // Trying to find label of 0 size!
  
  cHeadCPU temp_head(this);
  
  while (temp_head.InMemory()) {
    // If we are not in a label, jump to the next checkpoint...
    if (!m_inst_set->IsNop(temp_head.GetInst())) {
      temp_head.AbsJump(label.GetSize());
      continue;
    }
    
    // Otherwise, rewind to the begining of this label...
    
    while (!(temp_head.AtFront()) && m_inst_set->IsNop(temp_head.GetInst(-1)))
      temp_head.AbsJump(-1);
    
    // Calculate the size of the label being checked, and make sure they
    // are equal.
    
    int size = 0;
    bool label_match = true;
    do {
      // Check if the nop matches
      if (size < label.GetSize() && label[size] != m_inst_set->GetNopMod(temp_head.GetInst()))
        label_match = false;
      
      // Increment the current position and length calculation
      temp_head.AbsJump(1);
      size++;
      
      // While still within memory and the instruction is a nop
    } while (temp_head.InMemory() && m_inst_set->IsNop(temp_head.GetInst()));
    
    if (size != label.GetSize()) continue;
    
    // temp_head will point to the first non-nop instruction after the label, or the end of the memory space
    //   if this is a match, return this position
    if (label_match) return temp_head;
  }
  
  // The label does not exist in this creature.
  
  temp_head.AbsSet(-1);
  return temp_head;
}

tBuffer<int>& cHardwareBase::GetInputBuf() 
{ 
  return m_organism->GetInputBuf();
}

tBuffer<int>& cHardwareBase::GetOutputBuf() 
{ 
  return m_organism->GetOutputBuf(); 
}


bool cHardwareBase::Inst_Nop(cAvidaContext& ctx)          // Do Nothing.
{
  return true;
}


// @JEB Check implicit repro conditions -- meant to be called at the end of SingleProcess
void cHardwareBase::checkImplicitRepro(cAvidaContext& ctx, bool exec_last_inst)         
{  
  //Dividing a dead organism causes all kinds of problems
  if (m_organism->IsDead()) return;
  
  if( (m_world->GetConfig().IMPLICIT_REPRO_TIME.Get() && (m_organism->GetPhenotype().GetTimeUsed() >= m_world->GetConfig().IMPLICIT_REPRO_TIME.Get()))
     || (m_world->GetConfig().IMPLICIT_REPRO_CPU_CYCLES.Get() && (m_organism->GetPhenotype().GetCPUCyclesUsed() >= m_world->GetConfig().IMPLICIT_REPRO_CPU_CYCLES.Get()))
     || (m_world->GetConfig().IMPLICIT_REPRO_BONUS.Get() && (m_organism->GetPhenotype().GetCurBonus() >= m_world->GetConfig().IMPLICIT_REPRO_BONUS.Get()))
     || (m_world->GetConfig().IMPLICIT_REPRO_END.Get() && exec_last_inst)
     || (m_world->GetConfig().IMPLICIT_REPRO_ENERGY.Get() && (m_organism->GetPhenotype().GetStoredEnergy() >= m_world->GetConfig().IMPLICIT_REPRO_ENERGY.Get())) )
  {
    Inst_Repro(ctx);
  }
}

//This must be overridden by the specific CPU to function properly
bool cHardwareBase::Inst_Repro(cAvidaContext& ctx) 
{
  cout << "This hardware type does not have a =repro= instruction. IMPLICIT_REPRO conditions cannot be used!" << endl;
  exit(1);
  return false;
}


bool cHardwareBase::Inst_DoubleEnergyUsage(cAvidaContext& ctx)
{
  cPhenotype& phenotype = m_organism->GetPhenotype();
  phenotype.DoubleEnergyUsage();
  double newOrgMerit = phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy()  * phenotype.GetEnergyUsageRatio());
  m_organism->UpdateMerit(newOrgMerit);
  return true;
}

bool cHardwareBase::Inst_HalveEnergyUsage(cAvidaContext& ctx)
{
  cPhenotype& phenotype = m_organism->GetPhenotype();
  phenotype.HalveEnergyUsage();
  double newOrgMerit = phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy()  * phenotype.GetEnergyUsageRatio());
  m_organism->UpdateMerit(newOrgMerit);
  return true;
}

bool cHardwareBase::Inst_DefaultEnergyUsage(cAvidaContext& ctx)
{
  cPhenotype& phenotype = m_organism->GetPhenotype();
  phenotype.DefaultEnergyUsage();
  double newOrgMerit = phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy()  * phenotype.GetEnergyUsageRatio());
  m_organism->UpdateMerit(newOrgMerit);
  return true;
}


// This method will test to see if all costs have been paid associated
// with executing an instruction and only return true when that instruction
// should proceed.
bool cHardwareBase::SingleProcess_PayPreCosts(cAvidaContext& ctx, const cInstruction& cur_inst)
{
#if INSTRUCTION_COSTS
  if (m_world->GetConfig().ENERGY_ENABLED.Get() > 0) {
    // TODO:  Get rid of magic number. check avaliable energy first
    double energy_req = m_inst_energy_cost[cur_inst.GetOp()] * (m_organism->GetPhenotype().GetMerit().GetDouble() / 100.0); //compensate by factor of 100
		
    if (energy_req > 0.0) {
      if (m_organism->GetPhenotype().GetStoredEnergy() >= energy_req) {
				m_inst_energy_cost[cur_inst.GetOp()] = 0.0;
				// subtract energy used from current org energy.
        m_organism->GetPhenotype().ReduceEnergy(energy_req);  
        
        // tracking sleeping organisms
        if (m_inst_set->ShouldSleep(cur_inst)) m_organism->SetSleeping(true);
      } else {
        m_organism->GetPhenotype().SetToDie();
				return false; // no more, your died...  (evil laugh)
      }
    }
  }
	
	// If task switching costs need to be paid off...
	if (m_task_switching_cost > 0) { 
		m_task_switching_cost--;
		// update deme level stats
		cDeme* deme = m_organism->GetOrgInterface().GetDeme();
		if(deme != NULL) {
			deme->IncNumSwitchingPenalties(1);
		}
		return false;
	}
  
  // If first time cost hasn't been paid off...
  if (m_has_ft_costs && m_inst_ft_cost[cur_inst.GetOp()] > 0) {
    m_inst_ft_cost[cur_inst.GetOp()]--;       // dec cost
    return false;
  }
  
  // Next, look at the per use cost
  if (m_has_costs) {
    if (m_inst_cost > 1) { // Current cost being paid, decrement and return false
      m_inst_cost--;
      return false;
    }
    
    if (!m_inst_cost && m_inst_set->GetCost(cur_inst) > 1) {
      // no current cost, but there are costs active, and this instruction has a cost, setup the counter and return false
      m_inst_cost = m_inst_set->GetCost(cur_inst) - 1;
      return false;
    }
    
    // If we fall to here, reset the current cost count to zero
    m_inst_cost = 0;
  }
  
  if (m_world->GetConfig().ENERGY_ENABLED.Get() > 0) {
    m_inst_energy_cost[cur_inst.GetOp()] = m_inst_set->GetEnergyCost(cur_inst); // reset instruction energy cost
  }
#endif
  return true;
}


void cHardwareBase::SingleProcess_PayPostCosts(cAvidaContext& ctx, const cInstruction& cur_inst)
{
#if INSTRUCTION_COSTS
#endif
}


//! Called when the organism that owns this CPU has received a flash from a neighbor.
void cHardwareBase::ReceiveFlash()
{
  m_world->GetDriver().RaiseFatalException(1, "Method cHardwareBase::ReceiveFlash must be overriden.");
}

/*! Retrieve a fragment of this organism's genome that extends downstream from the read head.
 */
cSequence cHardwareBase::GetGenomeFragment(unsigned int downstream) {
	cHeadCPU tmp(GetHead(nHardware::HEAD_READ));
	cSequence fragment(downstream);
	for(; downstream>0; --downstream, tmp.Advance()) { 
		fragment.Append(tmp.GetInst());
	}
	return fragment;
}

/*! Insert a genome fragment at the current write head.
 */
void cHardwareBase::InsertGenomeFragment(const cSequence& fragment) {
	cHeadCPU& wh = GetHead(nHardware::HEAD_WRITE);
	wh.GetMemory().Insert(wh.GetPosition(), fragment);
	wh.Adjust();
}
