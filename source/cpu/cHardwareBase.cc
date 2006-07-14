/*
 *  cHardwareBase.cc
 *  Avida
 *
 *  Created by David on 11/17/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#include "cHardwareBase.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cHardwareManager.h"
#include "cHeadCPU.h"
#include "cInstSet.h"
#include "cMutation.h"
#include "cMutationLib.h"
#include "nMutation.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cRandom.h"
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
*/
unsigned cHardwareBase::Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier)
{
  unsigned totalMutations = 0;

  sCPUStats& cpu_stats = organism->CPUStats();
  cCPUMemory& child_genome = organism->ChildGenome();
  
  organism->GetPhenotype().SetDivType(mut_multiplier);
  
  // Divide Mutations
  if (organism->TestDivideMut(ctx)) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize());
    child_genome[mut_line] = m_inst_set->GetRandomInst(ctx);
    totalMutations += ++cpu_stats.mut_stats.divide_mut_count;
  }
  
  // Divide Insertions
  if (organism->TestDivideIns(ctx) && child_genome.GetSize() < MAX_CREATURE_SIZE) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
    child_genome.Insert(mut_line, m_inst_set->GetRandomInst(ctx));
    totalMutations += ++cpu_stats.mut_stats.divide_insert_mut_count;
  }
  
  // Divide Deletions
  if (organism->TestDivideDel(ctx) && child_genome.GetSize() > MIN_CREATURE_SIZE) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize());
    child_genome.Remove(mut_line);
    totalMutations += ++cpu_stats.mut_stats.divide_delete_mut_count;
  }
  
  // Divide Mutations (per site)
  if (organism->GetDivMutProb() > 0) {
    int num_mut = ctx.GetRandom().GetRandBinomial(child_genome.GetSize(), 
                                                  organism->GetDivMutProb() / mut_multiplier);
    // If we have lines to mutate...
    if (num_mut > 0) {
      for (int i = 0; i < num_mut; i++) {
        int site = ctx.GetRandom().GetUInt(child_genome.GetSize());
        child_genome[site] = m_inst_set->GetRandomInst(ctx);
        cpu_stats.mut_stats.div_mut_count++;
      }
    }
    totalMutations += cpu_stats.mut_stats.div_mut_count;
  }
  
  
  // Insert Mutations (per site)
  if (organism->GetInsMutProb() > 0) {
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
        cpu_stats.mut_stats.insert_mut_count++;
	totalMutations++; //Unline the others we can't be sure this was done only on divide -- AWC 06/29/06
      }
    }
  }
  
  
  // Delete Mutations (per site)
  if (organism->GetDelMutProb() > 0) {
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
      cpu_stats.mut_stats.delete_mut_count++;
    }
  }
  
  // Mutations in the parent's genome
  if (organism->GetParentMutProb() > 0) {
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestParentMut(ctx)) {
        GetMemory()[i] = m_inst_set->GetRandomInst(ctx);
        cpu_stats.mut_stats.parent_mut_line_count++;
	totalMutations++; //Unline the others we can't be sure this was done only on divide -- AWC 06/29/06

      }
    }
  }
  
  
  // Count up mutated lines
  for (int i = 0; i < GetMemory().GetSize(); i++) {
    if (GetMemory().FlagPointMut(i)) {
      cpu_stats.mut_stats.point_mut_line_count++;
    }
  }
  for (int i = 0; i < child_genome.GetSize(); i++) {
    if (child_genome.FlagCopyMut(i)) {
      cpu_stats.mut_stats.copy_mut_line_count++;
    }
  }

  return totalMutations;
}


// test whether the offspring creature contains an advantageous mutation.
/*
  Return true iff only a reversion is performed -- returns false is steralized regardless of weather or 
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
	
  const double parent_fitness = organism->GetTestFitness();
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
    organism->CPUStats().mut_stats.point_mut_count++;
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


bool cHardwareBase::Inst_Nop(cAvidaContext& ctx)          // Do Nothing.
{
  return true;
}
