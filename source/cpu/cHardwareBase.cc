/*
 *  cHardwareBase.cc
 *  Avida
 *
 *  Called "hardware_base.cc" prior to 11/17/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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
#include "cMutation.h"
#include "cMutationLib.h"
#include "nMutation.h"
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

#include "functions.h"


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

int cHardwareBase::GetExecutedSize(const int parent_size)
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
  // Make sure the organism is okay with dividing now...
  if (m_organism->Divide_CheckViable() == false) return false; // (divide fails)
  
  // Make sure that neither parent nor child will be below the minimum size.  
  const int genome_size = m_organism->GetGenome().GetSize();
  const double size_range = m_world->GetConfig().CHILD_SIZE_RANGE.Get();
  const int min_size = Max(MIN_CREATURE_SIZE, static_cast<int>(genome_size / size_range));
  const int max_size = Min(MAX_CREATURE_SIZE, static_cast<int>(genome_size * size_range));
  
  if (child_size < min_size || child_size > max_size) {
    m_organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Invalid offspring length (%d)", child_size));
    return false; // (divide fails)
  }
  if (parent_size < min_size || parent_size > max_size) {
    m_organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Invalid post-divide length (%d)",parent_size));
    return false; // (divide fails)
  }
  
  // Absolute minimum and maximum child/parent size limits -- @JEB
  const int max_genome_size = m_world->GetConfig().MAX_GENOME_SIZE.Get();
  const int min_genome_size = m_world->GetConfig().MIN_GENOME_SIZE.Get();
  if ( (min_genome_size && (child_size < min_genome_size)) || (max_genome_size && (child_size > max_genome_size)) ) {
    m_organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Invalid absolute offspring length (%d)",child_size));
    return false; // (divide fails)
  }
  
  if ( (min_genome_size && (parent_size < min_genome_size)) || (max_genome_size && (parent_size > max_genome_size)) ) {
    m_organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Invalid absolute post-divide length (%d)",parent_size));
    return false; // (divide fails)
  }
  
  // Count the number of lines executed in the parent, and make sure the
  // specified fraction has been reached.
  
  const int executed_size = GetExecutedSize(parent_size);
  const int min_exe_lines = static_cast<int>(parent_size * m_world->GetConfig().MIN_EXE_LINES.Get());
  if (executed_size < min_exe_lines) {
    m_organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Too few executed lines (%d < %d)", executed_size, min_exe_lines));
    return false; // (divide fails)
  }
  
  // Repro organisms mark their entire genomes as copied
  int copied_size = parent_size;
  if (!using_repro) {
    // Normal organisms check to see how much was copied
    copied_size = GetCopiedSize(parent_size, child_size); // Fails for REPRO organisms
    const int min_copied = static_cast<int>(child_size * m_world->GetConfig().MIN_COPIED_LINES.Get());
  
    if (copied_size < min_copied) {
      m_organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                      cStringUtil::Stringf("Too few copied commands (%d < %d)", copied_size, min_copied));
      return false; // (divide fails)
    }
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
    
    bool sterilize = false;
    
    if (fitness_ratio < nHardware::FITNESS_NEUTRAL_MIN) {
      if (ctx.GetRandom().P(m_organism->GetSterilizeNeg())) sterilize = true;
    } else if (fitness_ratio <= nHardware::FITNESS_NEUTRAL_MAX) {
      if (ctx.GetRandom().P(m_organism->GetSterilizeNeut())) sterilize = true;
    } else {
      if (ctx.GetRandom().P(m_organism->GetSterilizePos())) sterilize = true;
    }
    
    if (sterilize) {
      // Don't let this organism have this or any more children!
      phenotype.IsFertile() = false;
      return false;
    }    
  }
  
  return true; // (divide succeeds!)
}


/*
 Return the number of mutations that occur on divide.  AWC 06/29/06
 Limit the number of mutations that occur to be less than or equal to maxmut (defaults to INT_MAX)
 */
int cHardwareBase::Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier, const int maxmut)
{
  int totalMutations = 0;
  cCPUMemory& offspring_genome = m_organism->ChildGenome();
  
  m_organism->GetPhenotype().SetDivType(mut_multiplier);
  
  // Divide Slip Mutations - NOT COUNTED.
  if (m_organism->TestDivideSlip(ctx)) doSlipMutation(ctx, offspring_genome);
    
  // Divide Mutations
  if (m_organism->TestDivideMut(ctx) && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
    offspring_genome[mut_line] = m_inst_set->GetRandomInst(ctx);
    totalMutations++;
  }
  
  // Divide Insertions
  if (m_organism->TestDivideIns(ctx) && offspring_genome.GetSize() < MAX_CREATURE_SIZE && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize() + 1);
    offspring_genome.Insert(mut_line, m_inst_set->GetRandomInst(ctx));
    totalMutations++;
  }
  
  // Divide Deletions
  if (m_organism->TestDivideDel(ctx) && offspring_genome.GetSize() > MIN_CREATURE_SIZE && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(offspring_genome.GetSize());
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
        offspring_genome[site] = m_inst_set->GetRandomInst(ctx);
        totalMutations++;
      }
    }
  }

  
  // Insert Mutations (per site)
  if (m_organism->GetDivInsProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), m_organism->GetDivInsProb());

    // If would make creature to big, insert up to MAX_CREATURE_SIZE
    if (num_mut + offspring_genome.GetSize() > MAX_CREATURE_SIZE) {
      num_mut = MAX_CREATURE_SIZE - offspring_genome.GetSize();
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
      }
      
      totalMutations += num_mut;
    }
  }
  
  
  // Delete Mutations (per site)
  if (m_organism->GetDivDelProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), m_organism->GetDivDelProb());
    
    // If would make creature too small, delete down to MIN_CREATURE_SIZE
    if (offspring_genome.GetSize() - num_mut < MIN_CREATURE_SIZE) {
      num_mut = offspring_genome.GetSize() - MIN_CREATURE_SIZE;
    }
    
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      offspring_genome.Remove(ctx.GetRandom().GetUInt(offspring_genome.GetSize()));
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
  
  
  // Slip Mutations (per site) - NOT COUNTED
  if (m_organism->GetDivSlipProb() > 0) {
    int num_mut = ctx.GetRandom().GetRandBinomial(offspring_genome.GetSize(), 
                                                  m_organism->GetDivSlipProb() / mut_multiplier);
    for (int i = 0; i < num_mut; i++) doSlipMutation(ctx, offspring_genome);
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


bool cHardwareBase::doUniformMutation(cAvidaContext& ctx, cCPUMemory& genome)
{
  int mut = ctx.GetRandom().GetUInt((m_inst_set->GetSize() * 2) + 1);
  
  if (mut < m_inst_set->GetSize()) { // point
    int site = ctx.GetRandom().GetUInt(genome.GetSize());
    genome[site] = cInstruction(mut);
  } else if (mut == m_inst_set->GetSize()) { // delete
    if (genome.GetSize() == MIN_CREATURE_SIZE) return false;
    int site = ctx.GetRandom().GetUInt(genome.GetSize());
    genome.Remove(site);
  } else { // insert
    if (genome.GetSize() == MAX_CREATURE_SIZE) return false;
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
void cHardwareBase::doSlipMutation(cAvidaContext& ctx, cCPUMemory& genome, int from)
{
  cGenome genome_copy = cGenome(genome);
  
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
        case 0:
          genome[from + i] = genome_copy[to + i];
          break;
          
        case 1:
          genome[from + i] = m_inst_set->GetInst("nop-X");
          break;
          
        case 2:
          genome[from + i] = m_inst_set->GetRandomInst(ctx);
          break;
          
          //Randomized order of instructions
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
          
        default:
          m_world->GetDriver().RaiseException("Unknown SLIP_FILL_MODE\n");
      }
    }
  }
  
  // Deletion / remaining genome
  if (insertion_length < 0) insertion_length = 0;
  for (int i = insertion_length; i < genome_copy.GetSize() - to; i++) genome[from + i] = genome_copy[to + i];
  
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
  cCPUMemory& child_genome = m_organism->ChildGenome();
  
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
  phenotype.CopyTrue() = ( m_organism->ChildGenome() == m_organism->GetGenome() );
  phenotype.ChildFertile() = true;
	
  // Only continue if we're supposed to do a fitness test on divide...
  if (m_organism->GetTestOnDivide() == false) return false;
	
  // If this was a perfect copy, then we don't need to worry about any other
  // tests...  Theoretically, we need to worry about the parent changing,
  // but as long as the child is always compared to the original genotype,
  // this won't be an issue.
  if (phenotype.CopyTrue() == true) return false;
	
  const double parent_fitness = m_organism->GetTestFitness(ctx);
  const double neut_min = parent_fitness * (1.0 - m_organism->GetNeutralMin());
    const double neut_max = parent_fitness * (1.0 + m_organism->GetNeutralMax());
      
      cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
      cCPUTestInfo test_info;
      test_info.UseRandomInputs();
      testcpu->TestGenome(ctx, test_info, m_organism->ChildGenome());
      const double child_fitness = test_info.GetGenotypeFitness();
      delete testcpu;
      
      bool revert = false;
      bool sterilize = false;
      
      // If implicit mutations are turned off, make sure this won't spawn one.
      if (m_organism->GetFailImplicit() == true) {
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
      
      // Ideally, we won't have reversions and sterilizations turned on at the
      // same time, but if we do, give revert the priority.
      if (revert == true) {
        m_organism->ChildGenome() = m_organism->GetGenome();
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
  phenotype.CopyTrue() = (m_organism->ChildGenome() == m_organism->GetGenome());
  phenotype.ChildFertile() = true;
	
  // Only continue if we're supposed to do a fitness test on divide...
  if (m_organism->GetTestOnDivide() == false) return false;
	
  // If this was a perfect copy, then we don't need to worry about any other
  // tests...  Theoretically, we need to worry about the parent changing,
  // but as long as the child is always compared to the original genotype,
  // this won't be an issue.
  if (phenotype.CopyTrue() == true) return false;
	
  const double parent_fitness = m_organism->GetTestFitness(ctx);
  const double neut_min = parent_fitness * (1.0 - m_organism->GetNeutralMin());
  const double neut_max = parent_fitness * (1.0 + m_organism->GetNeutralMax());
      
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cCPUTestInfo test_info;
  test_info.UseRandomInputs();
  testcpu->TestGenome(ctx, test_info, m_organism->ChildGenome());
  const double child_fitness = test_info.GetGenotypeFitness();
  delete testcpu;
  
  bool revert = false;
  bool sterilize = false;
    
  // If implicit mutations are turned off, make sure this won't spawn one.
  if (m_organism->GetFailImplicit() > 0) {
    if (test_info.GetMaxDepth() > 0) sterilize = true;
  }

  if (m_organism->GetFailImplicit() > 1 && !test_info.IsViable()) {
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
  
  // Ideally, we won't have reversions and sterilizations turned on at the
  // same time, but if we do, give revert the priority.
  if (revert == true) {
    m_organism->ChildGenome() = m_organism->GetGenome();
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

void cHardwareBase::TriggerMutations_Body(cAvidaContext& ctx, int type, cCPUMemory & target_memory, cHeadCPU& cur_head)
{
  const int pos = cur_head.GetPosition();
	
  switch (type) {
		case nMutation::TYPE_POINT:
			target_memory[pos] = m_inst_set->GetRandomInst(ctx);
			target_memory.SetFlagMutated(pos);
			break;
		case nMutation::TYPE_INSERT:
		case nMutation::TYPE_DELETE:
		case nMutation::TYPE_HEAD_INC:
		case nMutation::TYPE_HEAD_DEC:
		case nMutation::TYPE_TEMP:
		case nMutation::TYPE_KILL:
		default:
      m_world->GetDriver().RaiseException("Mutation type not implemented!");
			break;
  };
}


bool cHardwareBase::TriggerMutations_ScopeGenome(cAvidaContext& ctx, const cMutation* cur_mut,
                                                 cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate)
{
  // The rate we have stored indicates the probability that a single
  // mutation will occur anywhere in the genome.
  
  if (ctx.GetRandom().P(rate) == true) {
    // We must create a temporary head and use it to randomly determine the
    // position in the genome to be mutated.
    cHeadCPU tmp_head(cur_head);
    tmp_head.AbsSet(ctx.GetRandom().GetUInt(target_memory.GetSize()));
    TriggerMutations_Body(ctx, cur_mut->GetType(), target_memory, tmp_head);
    return true;
  }
  return false;
}

bool cHardwareBase::TriggerMutations_ScopeLocal(cAvidaContext& ctx, const cMutation* cur_mut,
                                                cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate)
{
  // The rate we have stored is the probability for a mutation at this single
  // position in the genome.
	
  if (ctx.GetRandom().P(rate) == true) {
    TriggerMutations_Body(ctx, cur_mut->GetType(), target_memory, cur_head);
    return true;
  }
  return false;
}

int cHardwareBase::TriggerMutations_ScopeGlobal(cAvidaContext& ctx, const cMutation * cur_mut,
                                                cCPUMemory & target_memory, cHeadCPU& cur_head, const double rate)
{
  // The probability we have stored is per-site, so we can pull a random
  // number from a binomial distribution to determine the number of mutations
  // that should occur.
	
  const int num_mut =
	ctx.GetRandom().GetRandBinomial(target_memory.GetSize(), rate);
	
  if (num_mut > 0) {
    for (int i = 0; i < num_mut; i++) {
      cHeadCPU tmp_head(cur_head);
      tmp_head.AbsSet(ctx.GetRandom().GetUInt(target_memory.GetSize()));
      TriggerMutations_Body(ctx, cur_mut->GetType(), target_memory, tmp_head);
    }
  }
	
  return num_mut;
}

// Trigger mutations of a specific type.  Outside triggers cannot specify
// a head since hardware types are not known.
bool cHardwareBase::TriggerMutations(cAvidaContext& ctx, int trigger)
{
  // Only update triggers should happen from the outside!
  assert(trigger == nMutation::TRIGGER_UPDATE);
	
  // Assume instruction pointer is the intended target (if one is even
  // needed!
	
  return TriggerMutations(ctx, trigger, IP());
}

bool cHardwareBase::TriggerMutations(cAvidaContext& ctx, int trigger, cHeadCPU& cur_head)
{
  // Collect information about mutations from the organism.
  cLocalMutations& mut_info = m_organism->GetLocalMutations();
  const tList<cMutation>& mut_list = mut_info.GetMutationLib().GetMutationList(trigger);
	
  // If we have no mutations for this trigger, stop here.
  if (mut_list.GetSize() == 0) return false;
  bool has_mutation = false;
	
  // Determine what memory this mutation will be affecting.
  cCPUMemory& target_mem = (trigger == nMutation::TRIGGER_DIVIDE) ? m_organism->ChildGenome() : GetMemory();
	
  // Loop through all mutations associated with this trigger and test them.
  tConstListIterator<cMutation> mut_it(mut_list);
	
  while (mut_it.Next() != NULL) {
    const cMutation* cur_mut = mut_it.Get();
    const int mut_id = cur_mut->GetID();
    const int scope = cur_mut->GetScope();
    const double rate = mut_info.GetRate(mut_id);
    switch (scope) {
			case nMutation::SCOPE_GENOME:
				if (TriggerMutations_ScopeGenome(ctx, cur_mut, target_mem, cur_head, rate)) {
					has_mutation = true;
					mut_info.IncCount(mut_id);
				}
				break;
			case nMutation::SCOPE_LOCAL:
			case nMutation::SCOPE_PROP:
				if (TriggerMutations_ScopeLocal(ctx, cur_mut, target_mem, cur_head, rate)) {
					has_mutation = true;
					mut_info.IncCount(mut_id);
				}
				break;
			case nMutation::SCOPE_GLOBAL:
			case nMutation::SCOPE_SPREAD:
				int num_muts = TriggerMutations_ScopeGlobal(ctx, cur_mut, target_mem, cur_head, rate);
				if (num_muts > 0) {
					has_mutation = true;
					mut_info.IncCount(mut_id, num_muts);
				}
          break;
    }
  }
	
  return has_mutation;
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
void cHardwareBase::CheckImplicitRepro(cAvidaContext& ctx, bool exec_last_inst)         
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
  m_organism->GetPhenotype().DoubleEnergyUsage();
  double newOrgMerit = cMerit::EnergyToMerit(m_organism->GetPhenotype().GetStoredEnergy()  * m_organism->GetPhenotype().GetEnergyUsageRatio(), m_world);
  m_organism->UpdateMerit(newOrgMerit);
  return true;
}

bool cHardwareBase::Inst_HalfEnergyUsage(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().HalfEnergyUsage();
  double newOrgMerit = cMerit::EnergyToMerit(m_organism->GetPhenotype().GetStoredEnergy()  * m_organism->GetPhenotype().GetEnergyUsageRatio(), m_world);
  m_organism->UpdateMerit(newOrgMerit);
  return true;
}

bool cHardwareBase::Inst_DefaultEnergyUsage(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().DefaultEnergyUsage();
  double newOrgMerit = cMerit::EnergyToMerit(m_organism->GetPhenotype().GetStoredEnergy()  * m_organism->GetPhenotype().GetEnergyUsageRatio(), m_world);
  m_organism->UpdateMerit(newOrgMerit);
  return true;
}

// This method will test to see if all costs have been paid associated
// with executing an instruction and only return true when that instruction
// should proceed.
bool cHardwareBase::SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction& cur_inst)
{
#if INSTRUCTION_COSTS

  if (m_world->GetConfig().ENERGY_ENABLED.Get() > 0) {
    // TODO:  Get rid of magic number. check avaliable energy first
    double energy_req = m_inst_energy_cost[cur_inst.GetOp()] * (m_organism->GetPhenotype().GetMerit().GetDouble() / 100.0); //compensate by factor of 100
    int cellID = m_organism->GetCellID();
    
    if((cellID != -1) && (energy_req > 0.0)) { // guard against running in the test cpu.

      if (m_organism->GetPhenotype().GetStoredEnergy() >= energy_req) {
        m_inst_energy_cost[cur_inst.GetOp()] = 0;
        // subtract energy used from current org energy.
        m_organism->GetPhenotype().ReduceEnergy(energy_req);  
        
        // tracking sleeping organisms
        cString instName = m_world->GetHardwareManager().GetInstSet().GetName(cur_inst);
        if( instName == cString("sleep") || instName == cString("sleep1") || instName == cString("sleep2") ||
            instName == cString("sleep3") || instName == cString("sleep4")) {
          cPopulation& pop = m_world->GetPopulation();
          if(m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1) {
            pop.AddBeginSleep(cellID,m_world->GetStats().GetUpdate());
          }
          m_organism->SetSleeping(true);
          m_organism->GetOrgInterface().GetDeme()->IncSleepingCount();
        }
      } else {
        m_organism->GetPhenotype().SetToDie();
        return false;
      }
    }
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


//! Called when the organism that owns this CPU has received a flash from a neighbor.
void cHardwareBase::ReceiveFlash() {
  m_world->GetDriver().RaiseFatalException(1, "Method cHardwareBase::ReceiveFlash must be overriden.");
}
