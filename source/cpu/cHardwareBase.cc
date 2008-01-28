/*
 *  cHardwareBase.cc
 *  Avida
 *../RSDN_FI_25ku_basemut_org50_1dist_fixlen_32205
 *  Called "hardware_base.cc" prior to 11/17/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "functions.h"


int cHardwareBase::GetExecutedSize(const int parent_size)
{
  int executed_size = 0;
  const cCPUMemory& memory = GetMemory();
  for (int i = 0; i < parent_size; i++) {
    if (memory.FlagExecuted(i)) executed_size++;
  }  
  return executed_size;
}

bool cHardwareBase::Divide_CheckViable(cAvidaContext& ctx, const int parent_size, const int child_size)
{
  // Make sure the organism is okay with dividing now...
  if (organism->Divide_CheckViable() == false) return false; // (divide fails)
  
  // Make sure that neither parent nor child will be below the minimum size.  
  const int genome_size = organism->GetGenome().GetSize();
  const double size_range = m_world->GetConfig().CHILD_SIZE_RANGE.Get();
  const int min_size = Max(MIN_CREATURE_SIZE, static_cast<int>(genome_size / size_range));
  const int max_size = Min(MAX_CREATURE_SIZE, static_cast<int>(genome_size * size_range));
  
  if (child_size < min_size || child_size > max_size) {
    organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Invalid offspring length (%d)", child_size));
    return false; // (divide fails)
  }
  if (parent_size < min_size || parent_size > max_size) {
    organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Invalid post-divide length (%d)",parent_size));
    return false; // (divide fails)
  }
  
  // Count the number of lines executed in the parent, and make sure the
  // specified fraction has been reached.
  
  const int executed_size = GetExecutedSize(parent_size);
  const int min_exe_lines = static_cast<int>(parent_size * m_world->GetConfig().MIN_EXE_LINES.Get());
  if (executed_size < min_exe_lines) {
    organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Too few executed lines (%d < %d)", executed_size, min_exe_lines));
    return false; // (divide fails)
  }
	
  const int copied_size = GetCopiedSize(parent_size, child_size);
  const int min_copied = static_cast<int>(child_size * m_world->GetConfig().MIN_COPIED_LINES.Get());
  if (copied_size < min_copied) {
    organism->Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
                    cStringUtil::Stringf("Too few copied commands (%d < %d)", copied_size, min_copied));
    return false; // (divide fails)
  }
  
  // Save the information we collected here...
  cPhenotype& phenotype = organism->GetPhenotype();
  phenotype.SetLinesExecuted(executed_size);
  phenotype.SetLinesCopied(copied_size);
  
  // Determine the fitness of this organism as compared to its parent...
  if (m_world->GetTestSterilize() && !phenotype.IsInjected()) {
    const int merit_base = phenotype.CalcSizeMerit();
    const double cur_fitness = merit_base * phenotype.GetCurBonus() / phenotype.GetTimeUsed();
    const double fitness_ratio = cur_fitness / phenotype.GetLastFitness();
    
    bool sterilize = false;
    
    if (fitness_ratio < nHardware::FITNESS_NEUTRAL_MIN) {
      if (ctx.GetRandom().P(organism->GetSterilizeNeg())) sterilize = true;
    } else if (fitness_ratio <= nHardware::FITNESS_NEUTRAL_MAX) {
      if (ctx.GetRandom().P(organism->GetSterilizeNeut())) sterilize = true;
    } else {
      if (ctx.GetRandom().P(organism->GetSterilizePos())) sterilize = true;
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
unsigned cHardwareBase::Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier, const int maxmut)
{
  int totalMutations = 0;
  //cerr << "Maxmut: " << maxmut << endl;
//  sCPUStats& cpu_stats = organism->CPUStats();
  cCPUMemory& child_genome = organism->ChildGenome();
  
  organism->GetPhenotype().SetDivType(mut_multiplier);
  
  // @JEB Divide Slip Mutations
  // As if the read head jumped from one random position of the child
  // to another random position and continued reading to the end.
  // This can cause large deletions or tandem duplications.
  // Unlucky organisms might exceed the allowed length (randomly) if these mutations occur.
  // Limited to once per divide and NOT COUNTED.
  if ( organism->TestDivideSlip(ctx) )
  {
    cGenome child_copy = cGenome(child_genome);
    
    //All combinations except beginning to past end allowed
    int from = ctx.GetRandom().GetInt(child_copy.GetSize()+1);
    int to = (from == 0) ? ctx.GetRandom().GetInt(child_copy.GetSize()) : ctx.GetRandom().GetInt(child_copy.GetSize()+1);
    
    //Resize child genome
    int insertion_length = (from-to);
    child_genome.Resize( child_genome.GetSize() + insertion_length );
    
    //Fill insertion
    if (insertion_length > 0)
    {
      tArray<bool> copied_so_far(insertion_length);
      copied_so_far.SetAll(false);
      for (int i=0; i < insertion_length; i++) 
      {
        switch (m_world->GetConfig().SLIP_FILL_MODE.Get())
        {
          case 0:
          child_genome[from+i] = child_copy[to+i];
          break;
          
          case 1:        
          child_genome[from+i] = m_inst_set->GetInst("nop-X");
          break;
          
          case 2:        
          child_genome[from+i] = m_inst_set->GetRandomInst(ctx);
          break;
          
          //Randomized order of instructions
          case 3:
          {
            int copy_index = m_world->GetRandom().GetInt(insertion_length-i);
            int test = 0;
            int passed = copy_index;
            while (passed >= 0)
            {
              if (copied_so_far[test]) 
              {
                copy_index++; 
              }
              else //this one hasn't been chosen, so we count it.
              {
                passed--;
              }
              test++;
            }
            child_genome[from+i] = child_genome[to+copy_index];
            copied_so_far[copy_index] = true;
          }
          break;
          
          default:
          cout << "Unknown SLIP_FILL_MODE\n";
          
          }
      }
    }
    
    //Deletion / remaining genome
    if (insertion_length < 0) insertion_length = 0;
    for (int i=insertion_length; i < child_copy.GetSize() - to; i++) 
    {
        child_genome[from+i] = child_copy[to+i];

    }

    
    if (m_world->GetVerbosity() >= VERBOSE_DETAILS) 
    {
      cout << "SLIP MUTATION from " << from << " to " << to << endl;
      cout << "Parent: " << child_copy.AsString()   << endl;
      cout << "Child : " << child_genome.AsString() << endl;
    }
  }
  
  // Divide Mutations
  if (organism->TestDivideMut(ctx) && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize());
    child_genome[mut_line] = m_inst_set->GetRandomInst(ctx);
//    ++cpu_stats.mut_stats.divide_mut_count;
    totalMutations++;
    //cerr << "Mutating HERE!!!! BAD!!!!!" << endl;
  }
  
  // Divide Insertions
  if (organism->TestDivideIns(ctx) && child_genome.GetSize() < MAX_CREATURE_SIZE && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
    child_genome.Insert(mut_line, m_inst_set->GetRandomInst(ctx));
//    ++cpu_stats.mut_stats.divide_insert_mut_count;
    totalMutations++;
  }
  
  // Divide Deletions
  if (organism->TestDivideDel(ctx) && child_genome.GetSize() > MIN_CREATURE_SIZE && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize());
    child_genome.Remove(mut_line);
//    ++cpu_stats.mut_stats.divide_delete_mut_count;
    totalMutations++;
  }
  
  // Divide Mutations (per site)
  if (organism->GetDivMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(), 
                                                  organism->GetDivMutProb() / mut_multiplier);
    // If we have lines to mutate...
    if (num_mut > 0 && totalMutations < maxmut) {
      for (int i = 0; i < num_mut && totalMutations < maxmut; i++) {
        int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
        child_genome[site] = m_inst_set->GetRandomInst(ctx);
//        ++cpu_stats.mut_stats.div_mut_count;
        totalMutations++;
      }
    }
  }
  

  
  // Need to come back and fix tese last two - per site instructions
  // Insert Mutations (per site)
  if (organism->GetInsMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(),
                                                  organism->GetInsMutProb());
    // If would make creature to big, insert up to MAX_CREATURE_SIZE
    if (num_mut + child_genome.GetSize() > MAX_CREATURE_SIZE) {
      num_mut = MAX_CREATURE_SIZE - child_genome.GetSize();
    }
    // If we have lines to insert...
    if (num_mut > 0) {
      // Build a list of the sites where mutations occured
      static int mut_sites[MAX_CREATURE_SIZE];
      for (int i = 0; i < num_mut; i++) {
        mut_sites[i] = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
      }
      // Sort the list
      qsort( (void*)mut_sites, num_mut, sizeof(int), &IntCompareFunction );
      // Actually do the mutations (in reverse sort order)
      for (int i = num_mut-1; i >= 0; i--) {
        child_genome.Insert(mut_sites[i], m_inst_set->GetRandomInst(ctx));
//        cpu_stats.mut_stats.insert_mut_count++;
        totalMutations++; //Unlike the others we can't be sure this was done only on divide -- AWC 06/29/06
      }
    }
  }
  
  
  // Delete Mutations (per site)
  if (organism->GetDelMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(),
                                                  organism->GetDelMutProb());
    // If would make creature too small, delete down to MIN_CREATURE_SIZE
    if (child_genome.GetSize() - num_mut < MIN_CREATURE_SIZE) {
      num_mut = child_genome.GetSize() - MIN_CREATURE_SIZE;
    }
    
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
      child_genome.Remove(site);
//      cpu_stats.mut_stats.delete_mut_count++;
      totalMutations++;
    }
  }
  
  
  
  // Uniform Mutations on Divide
  if (organism->GetUniformMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(), 
                                                  organism->GetUniformMutProb() / mut_multiplier);
    
    // If we have lines to mutate...
    if (num_mut > 0 && totalMutations < maxmut) {
      int mutrange = (m_inst_set->GetSize() * 2) + 1;
      for (int i = 0; i < num_mut && totalMutations < maxmut; i++) {
        int mut = ctx.GetRandom().GetUInt(mutrange);
        
        if (mut < m_inst_set->GetSize()) { // point
          int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
          child_genome[site] = cInstruction(mut);
        } else if (mut == m_inst_set->GetSize()) { // delete
          if (child_genome.GetSize() == MIN_CREATURE_SIZE) continue;
          int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
          child_genome.Remove(site);
        } else { // insert
          if (child_genome.GetSize() == MAX_CREATURE_SIZE) continue;
          int site = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
          child_genome.Insert(site, cInstruction(mut - m_inst_set->GetSize() - 1));
        }
                                               
        totalMutations++;
      }
    }
  }
  
  
  
  
  // Mutations in the parent's genome
  if (organism->GetParentMutProb() > 0 && totalMutations < maxmut) {
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestParentMut(ctx)) {
        GetMemory()[i] = m_inst_set->GetRandomInst(ctx);
//        cpu_stats.mut_stats.parent_mut_line_count++;
        totalMutations++; //Unlike the others we can't be sure this was done only on divide -- AWC 06/29/06
        
      }
    }
  }
  
  // Count up mutated lines
//  for (int i = 0; i < GetMemory().GetSize(); i++) {
//    if (GetMemory().FlagPointMut(i)) {
//      cpu_stats.mut_stats.point_mut_line_count++;
//    }
//  }
//  for (int i = 0; i < child_genome.GetSize(); i++) {
//    if (child_genome.FlagCopyMut(i)) {
//      cpu_stats.mut_stats.copy_mut_line_count++;
//    }
//  }
  
  return totalMutations;
}

/*
 Return the number of mutations that occur on divide.  AWC 06/29/06
 Limit the number of mutations that occur to be less than or equat to maxmut (defaults to INT_MAX)
 */
unsigned cHardwareBase::Divide_DoExactMutations(cAvidaContext& ctx, double mut_multiplier, const int pointmut)
{
  int maxmut = pointmut;
  int totalMutations = 0;
  //cerr << "Maxmut: " << maxmut << endl;
//  sCPUStats& cpu_stats = organism->CPUStats();
  cCPUMemory& child_genome = organism->ChildGenome();
  
  organism->GetPhenotype().SetDivType(mut_multiplier);
  
  // Divide Mutations
  if (organism->TestDivideMut(ctx) && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize());
    child_genome[mut_line] = m_inst_set->GetRandomInst(ctx);
//    ++cpu_stats.mut_stats.divide_mut_count;
    totalMutations++;
    //cerr << "Mutating HERE!!!! BAD!!!!!" << endl;
  }
  
  // Divide Insertions
  if (organism->TestDivideIns(ctx) && child_genome.GetSize() < MAX_CREATURE_SIZE && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
    child_genome.Insert(mut_line, m_inst_set->GetRandomInst(ctx));
//    ++cpu_stats.mut_stats.divide_insert_mut_count;
    totalMutations++;
  }
  
  // Divide Deletions
  if (organism->TestDivideDel(ctx) && child_genome.GetSize() > MIN_CREATURE_SIZE && totalMutations < maxmut) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize());
    child_genome.Remove(mut_line);
//    ++cpu_stats.mut_stats.divide_delete_mut_count;
    totalMutations++;
  }
  
  // Divide Mutations (per site)
  if (organism->GetDivMutProb() > 0 && totalMutations < maxmut) {
    //int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(), 
    //                                              organism->GetDivMutProb() / mut_multiplier);
    int num_mut = pointmut;
    // If we have lines to mutate...
    if (num_mut > 0 && totalMutations < maxmut) {
      for (int i = 0; i < num_mut && totalMutations < maxmut; i++) {
        int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
        child_genome[site] = m_inst_set->GetRandomInst(ctx);
//        ++cpu_stats.mut_stats.div_mut_count;
        totalMutations++;
        //cerr << "OK to mutate here " << totalMutations << endl;
      }
    }
  }
  
  
  
  // Need to come back and fix tese last two - per site instructions
  // Insert Mutations (per site)
  if (organism->GetInsMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(),
                                                  organism->GetInsMutProb());
    
    // If would make creature to big, insert up to MAX_CREATURE_SIZE
    if (num_mut + child_genome.GetSize() > MAX_CREATURE_SIZE) {
      num_mut = MAX_CREATURE_SIZE - child_genome.GetSize();
    }
    // If we have lines to insert...
    if (num_mut > 0) {
      // Build a list of the sites where mutations occured
      static int mut_sites[MAX_CREATURE_SIZE];
      for (int i = 0; i < num_mut; i++) {
        mut_sites[i] = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
      }
      // Sort the list
      qsort( (void*)mut_sites, num_mut, sizeof(int), &IntCompareFunction );
      // Actually do the mutations (in reverse sort order)
      for (int i = num_mut-1; i >= 0; i--) {
        child_genome.Insert(mut_sites[i], m_inst_set->GetRandomInst(ctx));
//        cpu_stats.mut_stats.insert_mut_count++;
        totalMutations++; //Unlike the others we can't be sure this was done only on divide -- AWC 06/29/06
      }
    }
  }
  
  
  // Delete Mutations (per site)
  if (organism->GetDelMutProb() > 0 && totalMutations < maxmut) {
    int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(),
                                                  organism->GetDelMutProb());
    // If would make creature too small, delete down to MIN_CREATURE_SIZE
    if (child_genome.GetSize() - num_mut < MIN_CREATURE_SIZE) {
      num_mut = child_genome.GetSize() - MIN_CREATURE_SIZE;
    }
    
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
      child_genome.Remove(site);
//      cpu_stats.mut_stats.delete_mut_count++;
      totalMutations++;
    }
  }
  
  // Mutations in the parent's genome
  if (organism->GetParentMutProb() > 0 && totalMutations < maxmut) {
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestParentMut(ctx)) {
        GetMemory()[i] = m_inst_set->GetRandomInst(ctx);
//        cpu_stats.mut_stats.parent_mut_line_count++;
        totalMutations++; //Unlike the others we can't be sure this was done only on divide -- AWC 06/29/06
        
      }
    }
  }
  
  
  // Count up mutated lines
//  for (int i = 0; i < GetMemory().GetSize(); i++) {
//    if (GetMemory().FlagPointMut(i)) {
//      cpu_stats.mut_stats.point_mut_line_count++;
//    }
//  }
//  for (int i = 0; i < child_genome.GetSize(); i++) {
//    if (child_genome.FlagCopyMut(i)) {
//      cpu_stats.mut_stats.copy_mut_line_count++;
//    }
//  }
  
  return totalMutations;
}


// test whether the offspring creature contains an advantageous mutation.
/*
 Return true iff only a reversion is performed -- returns false is sterilized regardless of whether or 
 not a reversion is performed.  AWC 06/29/06
 */
bool cHardwareBase::Divide_TestFitnessMeasures(cAvidaContext& ctx)
{
  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.CopyTrue() = ( organism->ChildGenome() == organism->GetGenome() );
  phenotype.ChildFertile() = true;
	
  // Only continue if we're supposed to do a fitness test on divide...
  if (organism->GetTestOnDivide() == false) return false;
	
  // If this was a perfect copy, then we don't need to worry about any other
  // tests...  Theoretically, we need to worry about the parent changing,
  // but as long as the child is always compared to the original genotype,
  // this won't be an issue.
  if (phenotype.CopyTrue() == true) return false;
	
  const double parent_fitness = organism->GetTestFitness(ctx);
  const double neut_min = parent_fitness * (1.0 - organism->GetNeutralMin());//nHardware::FITNESS_NEUTRAL_MIN;
    const double neut_max = parent_fitness * (1.0 + organism->GetNeutralMax());//nHardware::FITNESS_NEUTRAL_MAX;
      
      cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
      cCPUTestInfo test_info;
      test_info.UseRandomInputs();
      testcpu->TestGenome(ctx, test_info, organism->ChildGenome());
      const double child_fitness = test_info.GetGenotypeFitness();
      delete testcpu;
      
      bool revert = false;
      bool sterilize = false;
      
      // If implicit mutations are turned off, make sure this won't spawn one.
      if (organism->GetFailImplicit() == true) {
        if (test_info.GetMaxDepth() > 0) sterilize = true;
      }
      
      if (child_fitness == 0.0) {
        // Fatal mutation... test for reversion.
        if (ctx.GetRandom().P(organism->GetRevertFatal())) revert = true;
        if (ctx.GetRandom().P(organism->GetSterilizeFatal())) sterilize = true;
      } else if (child_fitness < neut_min) {
        if (ctx.GetRandom().P(organism->GetRevertNeg())) revert = true;
        if (ctx.GetRandom().P(organism->GetSterilizeNeg())) sterilize = true;
      } else if (child_fitness <= neut_max) {
        if (ctx.GetRandom().P(organism->GetRevertNeut())) revert = true;
        if (ctx.GetRandom().P(organism->GetSterilizeNeut())) sterilize = true;
      } else {
        if (ctx.GetRandom().P(organism->GetRevertPos())) revert = true;
        if (ctx.GetRandom().P(organism->GetSterilizePos())) sterilize = true;
      }
      
      // Ideally, we won't have reversions and sterilizations turned on at the
      // same time, but if we do, give revert the priority.
      if (revert == true) {
        organism->ChildGenome() = organism->GetGenome();
      }
      
      if (sterilize == true) {
        organism->GetPhenotype().ChildFertile() = false;
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
//    organism->CPUStats().mut_stats.point_mut_count++;
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
  cLocalMutations& mut_info = organism->GetLocalMutations();
  const tList<cMutation>& mut_list = mut_info.GetMutationLib().GetMutationList(trigger);
	
  // If we have no mutations for this trigger, stop here.
  if (mut_list.GetSize() == 0) return false;
  bool has_mutation = false;
	
  // Determine what memory this mutation will be affecting.
  cCPUMemory& target_mem = (trigger == nMutation::TRIGGER_DIVIDE) ? organism->ChildGenome() : GetMemory();
	
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
  return organism->GetInputBuf();
}

tBuffer<int>& cHardwareBase::GetOutputBuf() 
{ 
  return organism->GetOutputBuf(); 
}


bool cHardwareBase::Inst_Nop(cAvidaContext& ctx)          // Do Nothing.
{
  return true;
}

// @JEB Check implicit repro conditions -- meant to be called at the end of SingleProcess
void cHardwareBase::CheckImplicitRepro(cAvidaContext& ctx, bool exec_last_inst)         
{  
  //Dividing a dead organism causes all kinds of problems
  if (organism->IsDead()) return;

  if( (m_world->GetConfig().IMPLICIT_REPRO_TIME.Get() && (organism->GetPhenotype().GetTimeUsed() >= m_world->GetConfig().IMPLICIT_REPRO_TIME.Get()))
   || (m_world->GetConfig().IMPLICIT_REPRO_CPU_CYCLES.Get() && (organism->GetPhenotype().GetCPUCyclesUsed() >= m_world->GetConfig().IMPLICIT_REPRO_CPU_CYCLES.Get()))
   || (m_world->GetConfig().IMPLICIT_REPRO_BONUS.Get() && (organism->GetPhenotype().GetCurBonus() >= m_world->GetConfig().IMPLICIT_REPRO_BONUS.Get()))
   || (m_world->GetConfig().IMPLICIT_REPRO_END.Get() && exec_last_inst ))
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

bool cHardwareBase::Inst_DoubleEnergyUsage(cAvidaContext& ctx) {
  double energy_req = inst_energy_cost[IP().GetNextInst().GetOp()]
                        * cMerit::EnergyToMerit(organism->GetPhenotype().GetStoredEnergy() * organism->GetPhenotype().GetEnergyUsageRatio() * 2.0, m_world)
                        / 100.0; //compensate by factor of 100

  return organism->GetPhenotype().DoubleEnergyUsage(energy_req);
}

bool cHardwareBase::Inst_HalfEnergyUsage(cAvidaContext& ctx) {
  organism->GetPhenotype().HalfEnergyUsage();
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
    double energy_req = inst_energy_cost[cur_inst.GetOp()] * (organism->GetPhenotype().GetMerit().GetDouble() / 100.0); //compensate by factor of 100

    if (energy_req > 0.0) { 
      int cellID = organism->GetCellID();
      if (organism->GetPhenotype().GetStoredEnergy() >= energy_req) {
        inst_energy_cost[cur_inst.GetOp()] = 0;
        // subtract energy used from current org energy.
        organism->GetPhenotype().ReduceEnergy(energy_req);  
        
        // tracking sleeping organisms
        cString instName = m_world->GetHardwareManager().GetInstSet().GetName(cur_inst);
        if( instName == cString("sleep") || instName == cString("sleep1") || instName == cString("sleep2") ||
            instName == cString("sleep3") || instName == cString("sleep4")) {
          cPopulation& pop = m_world->GetPopulation();
          if(m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1) {
            pop.AddBeginSleep(cellID,m_world->GetStats().GetUpdate());
          }
          pop.GetCell(cellID).GetOrganism()->SetSleeping(true);
          m_world->GetStats().incNumAsleep(pop.GetCell(cellID).GetDemeID());
        }
      } else {
        organism->GetPhenotype().SetToDie();
        return false;
      }
    }
  }

  // If first time cost hasn't been paid off...
  if (m_has_ft_costs && inst_ft_cost[cur_inst.GetOp()] > 0) {
    inst_ft_cost[cur_inst.GetOp()]--;       // dec cost
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
    inst_energy_cost[cur_inst.GetOp()] = m_inst_set->GetEnergyCost(cur_inst); // reset instruction energy cost
  }
#endif
  return true;
}

void cHardwareBase::ResetInstructionCosts()
{
  m_inst_cost = 0;

  const int num_inst_cost = m_inst_set->GetSize();

  if (m_has_ft_costs) {
    inst_ft_cost.Resize(num_inst_cost);
    for (int i = 0; i < num_inst_cost; i++) inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
  
  if (m_has_energy_costs) {
    inst_energy_cost.Resize(num_inst_cost);
    for (int i = 0; i < num_inst_cost; i++) inst_energy_cost[i] = m_inst_set->GetEnergyCost(cInstruction(i));
  }
}
