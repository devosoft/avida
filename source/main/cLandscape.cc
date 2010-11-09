/*
 *  cLandscape.cc
 *  Avida
 *
 *  Called "landscape.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cLandscape.h"

#include "cCPUMemory.h"
#include "cDataFile.h"
#include "cEnvironment.h"
#include "cInstSet.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cStats.h"             // For GetUpdate in outputs...
#include "cTestCPU.h"
#include "cWorld.h"


cLandscape::cLandscape(cWorld* world, const cGenome& in_genome)
: m_world(world), trials(1), m_min_found(0), m_max_trials(0), site_count(NULL)
{
  Reset(in_genome);
}

cLandscape::~cLandscape()
{
  if (site_count != NULL) delete [] site_count;
}

void cLandscape::Reset(const cGenome& in_genome)
{
  base_genome       = in_genome;
  peak_genome       = in_genome;
  base_fitness    = 0.0;
  base_merit      = 0.0;
  base_gestation  = 0;
  peak_fitness    = 0.0;
  total_fitness   = 0.0;
  total_sqr_fitness = 0.0;
  distance        = 0;
  trials          = 0;
  
  total_count   = 0;
  dead_count    = 0;
  neg_count     = 0;
  neut_count    = 0;
  pos_count     = 0;
  
  pos_size	= 0;
  neg_size	= 0;
  
  total_epi_count   = 0;
  pos_epi_count	= 0;
  neg_epi_count	= 0; 
  no_epi_count	= 0; 
  dead_epi_count= 0; 
  
  pos_epi_size	= 0;
  neg_epi_size	= 0;
  no_epi_size	= 0;
  
  if (site_count) delete [] site_count;
  site_count = new int[base_genome.GetSize() + 1];
  for (int i = 0; i <= base_genome.GetSize(); i++) site_count[i] = 0;
  
  total_entropy = 0.0;
  complexity = 0.0;
  neut_min = 0.0;
  neut_max = 0.0;
  
  m_num_found = 0;
}

double cLandscape::ProcessGenome(cAvidaContext& ctx, cTestCPU* testcpu, cGenome& in_genome)
{
  testcpu->TestGenome(ctx, m_cpu_test_info, in_genome);
  
  double test_fitness = m_cpu_test_info.GetColonyFitness();
  
  total_fitness += test_fitness;
  total_sqr_fitness += test_fitness * test_fitness;
  total_count++;
  if (test_fitness == 0) {
    dead_count++;
  } else if (test_fitness < neut_min) {
    neg_count++;
    neg_size = neg_size + test_fitness  ;
  } else if (test_fitness <= neut_max) {
    neut_count++;
  } else {
    pos_count++;
    pos_size = pos_size + test_fitness  ;
    if (test_fitness > peak_fitness) {
      peak_fitness = test_fitness;
      peak_genome = in_genome;
    }
  }
  
  return test_fitness;
}

void cLandscape::ProcessBase(cAvidaContext& ctx, cTestCPU* testcpu)
{
  // Collect info on base creature.
  
  testcpu->TestGenome(ctx, m_cpu_test_info, base_genome);
  
  cPhenotype & phenotype = m_cpu_test_info.GetColonyOrganism()->GetPhenotype();
  base_fitness = m_cpu_test_info.GetColonyFitness();
  base_merit = phenotype.GetMerit().GetDouble();
  base_gestation = phenotype.GetGestationTime();
  
  peak_fitness = base_fitness;
  peak_genome = base_genome;
  
  neut_min = base_fitness * nHardware::FITNESS_NEUTRAL_MIN;
  neut_max = base_fitness * nHardware::FITNESS_NEUTRAL_MAX;
  
}

void cLandscape::Process(cAvidaContext& ctx)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  
  // Get the info about the base creature.
  ProcessBase(ctx, testcpu);
  
  // Now Process the new creature at the proper distance.
  Process_Body(ctx, testcpu, base_genome, distance, 0);

  delete testcpu;
  
  // Calculate the complexity...
  
  double max_ent = log((double) m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize());
  total_entropy = 0;
  for (int i = 0; i < base_genome.GetSize(); i++) {
    // Per-site entropy is the log of the number of legal states for that
    // site.  Add one to account for the unmutated state.
    total_entropy += (log((double) site_count[i] + 1) / max_ent);
  }
  complexity = base_genome.GetSize() - total_entropy;
  
  m_num_found = base_genome.GetSize() * (m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize() - 1);
}


// For distances greater than one, this needs to be called recursively.

void cLandscape::Process_Body(cAvidaContext& ctx, cTestCPU* testcpu, cGenome& cur_genome,
                              int cur_distance, int start_line)
{
  const int max_line = base_genome.GetSize() - cur_distance + 1;
  const int inst_size = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize();
  
  cGenome mg(cur_genome);
  cSequence& mod_genome = mg.GetSequence();
  
  // Loop through all the lines of genome, testing trying all combinations.
  for (int line_num = start_line; line_num < max_line; line_num++) {
    int cur_inst = base_genome.GetSequence()[line_num].GetOp();
    
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) continue;
      
      mod_genome[line_num].SetOp(inst_num);
      if (cur_distance <= 1) {
        ProcessGenome(ctx, testcpu, mg);
        if (m_cpu_test_info.GetColonyFitness() >= neut_min) site_count[line_num]++;
      } else {
        Process_Body(ctx, testcpu, mg, cur_distance - 1, line_num + 1);
      }
    }
    
    mod_genome[line_num].SetOp(cur_inst);
  }
  
}



void cLandscape::ProcessDump(cAvidaContext& ctx, cDataFile& df)
{
  df.WriteComment("Detailed dump of the per-site, per-instruction fitness");
  df.WriteComment("values for the entire single-step landscape.");
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  
  // Get the info about the base creature.
  ProcessBase(ctx, testcpu);
  const int max_line = base_genome.GetSize();
  const int inst_size = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize();
  
  cGenome mg(base_genome);
  cSequence& mod_genome = mg.GetSequence();
  
  // Loop through all the lines of genome, testing trying all combinations.
  for (int line_num = 0; line_num < max_line; line_num++) {
    int cur_inst = base_genome.GetSequence()[line_num].GetOp();
    df.Write(cur_inst, "Original Instruction");

    // Loop through all instructions...
    double fitness = 0.0;
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) {
        fitness = base_fitness;
      } else {
        mod_genome[line_num].SetOp(inst_num);
        fitness = ProcessGenome(ctx, testcpu, mg);
      }
      df.Write(fitness, "Mutation Fitness (instruction = column_number - 2)");
    }

    df.Endl();
    mod_genome[line_num].SetOp(cur_inst);
  }
  
  delete testcpu;
}



void cLandscape::ProcessDelete(cAvidaContext& ctx)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  // Get the info about the base creature.
  ProcessBase(ctx, testcpu);
  
  const int max_line = base_genome.GetSize();
  cGenome mg(base_genome);
  cCPUMemory mod_genome = mg.GetSequence();
  
  // Loop through all the lines of genome, testing all deletions.
  for (int line_num = 0; line_num < max_line; line_num++) {
    int cur_inst = base_genome.GetSequence()[line_num].GetOp();
    mod_genome.Remove(line_num);
    mg.SetSequence(mod_genome);
    ProcessGenome(ctx, testcpu, mg);
    if (m_cpu_test_info.GetColonyFitness() >= neut_min) site_count[line_num]++;
    mod_genome.Insert(line_num, cInstruction(cur_inst));
  }
  
  delete testcpu;
}

void cLandscape::ProcessInsert(cAvidaContext& ctx)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  // Get the info about the base creature.
  ProcessBase(ctx, testcpu);
  
  const int max_line = base_genome.GetSize();
  const int inst_size = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize();
  
  cGenome mg(base_genome);
  cCPUMemory mod_genome = mg.GetSequence();
  
  // Loop through all the lines of genome, testing all insertions.
  for (int line_num = 0; line_num <= max_line; line_num++) {
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      mod_genome.Insert(line_num, cInstruction(inst_num));
      mg.SetSequence(mod_genome);
      ProcessGenome(ctx, testcpu, mg);
      if (m_cpu_test_info.GetColonyFitness() >= neut_min) site_count[line_num]++;
      mod_genome.Remove(line_num);
    }
  }

  delete testcpu;
}

// Prediction for a landscape where n sites are _randomized_.
void cLandscape::PredictWProcess(cAvidaContext& ctx, cDataFile& df, int update)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  distance = 1;
  
  // Get the info about the base creature.
  ProcessBase(ctx, testcpu);
  if (base_fitness == 0.0) return;
  
  BuildFitnessChart(ctx, testcpu);
  const int genome_size = fitness_chart.GetNumRows();
  const int inst_size = fitness_chart.GetNumCols();
  const double min_neut_fitness = 0.99;
  const double max_neut_fitness = 1.01;
  
  // Loop through the entries printing them and doing additional
  // calculations.
  int total_pos_found = 0;
  int total_neut_found = 0;
  int total_neg_found = 0;
  int total_dead_found = 0;
  double max_fitness = 1.0;
  double total_fitness = 0.0;
  double total_sqr_fitness = 0.0;
  
  for (int row = 0; row < genome_size; row++) {
    double max_line_fitness = 1.0;
    for (int col = 0; col < inst_size; col++) {
      double & cur_fitness = fitness_chart(row, col);
      cur_fitness /= base_fitness;
      total_fitness += cur_fitness;
      total_sqr_fitness += cur_fitness * cur_fitness;
      if (cur_fitness > max_neut_fitness) total_pos_found++;
      else if (cur_fitness > min_neut_fitness) total_neut_found++;
      else if (cur_fitness > 0.0) total_neg_found++;
      
      if (cur_fitness > max_line_fitness) max_line_fitness = cur_fitness;
    }
    max_fitness *= max_line_fitness;
  }
  
  const int total_tests = genome_size * inst_size;
  total_dead_found = total_tests - total_pos_found - total_neut_found - total_neg_found;
  df.Write(update, "Update");
  df.Write(1, "Number of Mutations");
  df.Write((static_cast<double>(total_dead_found) / static_cast<double>(total_tests)), "Probability Lethal");
  df.Write((static_cast<double>(total_neg_found) / static_cast<double>(total_tests)), "Probability Deleterious");
  df.Write((static_cast<double>(total_neut_found) / static_cast<double>(total_tests)), "Probability Neutral");
  df.Write((static_cast<double>(total_pos_found) / static_cast<double>(total_tests)), "Probability Beneficial");
  df.Write(total_tests, "Total Tests");
  df.Write(total_neut_found + total_pos_found, "Total Neutral and Beneficial");
  df.Write(total_fitness / static_cast<double>(total_tests), "Average Fitness");
  df.Write(total_sqr_fitness / static_cast<double>(total_tests), "Average Square Fitness");
  df.Endl();
  
  // Sample the table out to 10 mutations
  const int max_muts = 10;
  const int min_found = 100;
  const int min_tests = 10000;
  const int max_tests = 1000000000;
  
  double test_fitness;
  for (int num_muts = 2; num_muts <= max_muts; num_muts++) {
    total_pos_found = 0;
    total_neut_found = 0;
    total_neg_found = 0;
    total_fitness = 0.0;
    total_sqr_fitness = 0.0;
    tArray<int> mut_lines(num_muts);
    
    int test_id = 0;
    while ((test_id < min_tests) ||
           (test_id < max_tests && (total_neut_found + total_pos_found) < min_found)) {
      ctx.GetRandom().Choose(genome_size, mut_lines);
      test_fitness = 1.0;
      for (int j = 0; j < num_muts && test_fitness != 0.0; j++) {	
        test_fitness *=
        fitness_chart(mut_lines[j], ctx.GetRandom().GetUInt(inst_size));
      }
      total_fitness += test_fitness;
      total_sqr_fitness += test_fitness * test_fitness;
      if (test_fitness > max_neut_fitness) total_pos_found++;
      else if (test_fitness > min_neut_fitness) total_neut_found++;
      else if (test_fitness > 0.0) total_neg_found++;
      
      test_id++;
    }
    
    total_dead_found = test_id - total_pos_found - total_neut_found - total_neg_found;
    df.Write(update, "Update");
    df.Write(num_muts, "Number of Mutations");
    df.Write((static_cast<double>(total_dead_found) / static_cast<double>(test_id)), "Probability Lethal");
    df.Write((static_cast<double>(total_neg_found) / static_cast<double>(test_id)), "Probability Deleterious");
    df.Write((static_cast<double>(total_neut_found) / static_cast<double>(test_id)), "Probability Neutral");
    df.Write((static_cast<double>(total_pos_found) / static_cast<double>(test_id)), "Probability Beneficial");
    df.Write(test_id, "Total Tests");
    df.Write(total_neut_found + total_pos_found, "Total Neutral and Beneficial");
    df.Write(total_fitness / static_cast<double>(test_id), "Average Fitness");
    df.Write(total_sqr_fitness / static_cast<double>(test_id), "Average Square Fitness");
    df.Endl();
    
    if (total_pos_found + total_neut_found < min_found / 2) break;
  }
  
  // Calculate the complexity...
  
  double max_ent = log(static_cast<double>(m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize()));
  total_entropy = 0;
  for (int i = 0; i < base_genome.GetSize(); i++) {
    total_entropy += (log(static_cast<double>(site_count[i] + 1)) / max_ent);
  }
  complexity = base_genome.GetSize() - total_entropy;
  
  delete testcpu;
}


// Prediction for a landscape where n sites are _mutated_.
void cLandscape::PredictNuProcess(cAvidaContext& ctx, cDataFile& df, int update)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  distance = 1;
  
  // Get the info about the base creature.
  ProcessBase(ctx, testcpu);
  if (base_fitness == 0.0) return;
  
  BuildFitnessChart(ctx, testcpu);
  const int genome_size = fitness_chart.GetNumRows();
  const int inst_size = fitness_chart.GetNumCols();
  const double min_neut_fitness = 0.99;
  const double max_neut_fitness = 1.01;
  
  // Loop through the entries printing them and doing additional
  // calculations.
  int total_pos_found = 0;
  int total_neut_found = 0;
  int total_neg_found = 0;
  int total_dead_found = 0;
  int total_live_found = 0;
  double max_fitness = 1.0;
  double max_found_fitness = 0.0;
  double total_fitness = 0.0;
  double total_sqr_fitness = 0.0;
  
  for (int row = 0; row < genome_size; row++) {
    double max_line_fitness = 1.0;
    int base_inst = base_genome.GetSequence()[row].GetOp();
    for (int col = 0; col < inst_size; col++) {
      if (col == base_inst) continue; // Only consider changes to line!
      double & cur_fitness = fitness_chart(row, col);
      cur_fitness /= base_fitness;
      total_fitness += cur_fitness;
      total_sqr_fitness += cur_fitness * cur_fitness;
      if (cur_fitness > max_neut_fitness) total_pos_found++;
      else if (cur_fitness > min_neut_fitness) total_neut_found++;
      else if (cur_fitness > 0.0) total_neg_found++;
      
      if (cur_fitness > max_line_fitness) max_line_fitness = cur_fitness;
    }
    max_fitness *= max_line_fitness;
    if (max_line_fitness > max_found_fitness) max_found_fitness = max_line_fitness;
  }
  
  const int total_tests = genome_size * inst_size;
  total_live_found = total_pos_found + total_neut_found + total_neg_found;
  total_dead_found = total_tests - total_live_found;
  df.Write(update, "Update");
  df.Write(1, "Number of Mutations");
  df.Write((static_cast<double>(total_dead_found) / static_cast<double>(total_tests)), "Probability Lethal");
  df.Write((static_cast<double>(total_neg_found) / static_cast<double>(total_tests)), "Probability Deleterious");
  df.Write((static_cast<double>(total_neut_found) / static_cast<double>(total_tests)), "Probability Neutral");
  df.Write((static_cast<double>(total_pos_found) / static_cast<double>(total_tests)), "Probability Beneficial");
  df.Write(total_tests, "Total Tests");
  df.Write(total_live_found, "Total Living Mutants");
  df.Write(total_fitness / static_cast<double>(total_tests), "Average Fitness");
  df.Write(total_sqr_fitness / static_cast<double>(total_tests), "Average Square Fitness");
  df.Write(max_found_fitness, "Maximum Fitness");
  df.Endl();
  
  // Sample the table out to 10 mutations
  const int max_muts = 10;
  const int min_found = 100;
  const int min_tests = 10000;
  const int max_tests = 1000000000;
  
  double test_fitness;
  for (int num_muts = 2; num_muts <= max_muts; num_muts++) {
    total_pos_found = 0;
    total_neut_found = 0;
    total_neg_found = 0;
    total_fitness = 0.0;
    total_sqr_fitness = 0.0;
    max_found_fitness = 0.0;
    tArray<int> mut_lines(num_muts);
    
    int test_id = 0;
    while ((test_id < min_tests) ||
           (test_id < max_tests && (total_neg_found + total_neut_found + total_pos_found) < min_found)) {
      ctx.GetRandom().Choose(genome_size, mut_lines);
      test_fitness = 1.0;
      for (int j = 0; j < num_muts && test_fitness != 0.0; j++) {	
        int base_inst = base_genome.GetSequence()[ mut_lines[j] ].GetOp();
        int mut_inst = ctx.GetRandom().GetUInt(inst_size);
        while (mut_inst == base_inst) mut_inst = ctx.GetRandom().GetUInt(inst_size);
        test_fitness *= fitness_chart(mut_lines[j], mut_inst);
        if (test_fitness == 0.0) break;
      }
      total_fitness += test_fitness;
      total_sqr_fitness += test_fitness * test_fitness;
      if (test_fitness > max_found_fitness) max_found_fitness = test_fitness;
      
      if (test_fitness > max_neut_fitness) total_pos_found++;
      else if (test_fitness > min_neut_fitness) total_neut_found++;
      else if (test_fitness > 0.0) total_neg_found++;
      
      test_id++;
    }
    
    total_live_found = total_neg_found + total_neut_found + total_pos_found;
    total_dead_found = test_id - total_live_found;
    
    df.Write(update, "Update");
    df.Write(num_muts, "Number of Mutations");
    df.Write((static_cast<double>(total_dead_found) / static_cast<double>(test_id)), "Probability Lethal");
    df.Write((static_cast<double>(total_neg_found) / static_cast<double>(test_id)), "Probability Deleterious");
    df.Write((static_cast<double>(total_neut_found) / static_cast<double>(test_id)), "Probability Neutral");
    df.Write((static_cast<double>(total_pos_found) / static_cast<double>(test_id)), "Probability Beneficial");
    df.Write(total_tests, "Total Tests");
    df.Write(total_live_found, "Total Living Mutants");
    df.Write(total_fitness / static_cast<double>(test_id), "Average Fitness");
    df.Write(total_sqr_fitness / static_cast<double>(test_id), "Average Square Fitness");
    df.Write(max_found_fitness, "Maximum Fitness");
    df.Endl();

    if (total_live_found < min_found / 2) break;
  }
  
  // Calculate the complexity...
  
  double max_ent = log(static_cast<double>(m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize()));
  total_entropy = 0;
  for (int i = 0; i < base_genome.GetSize(); i++) {
    total_entropy += (log(static_cast<double>(site_count[i] + 1)) / max_ent);
  }
  complexity = base_genome.GetSize() - total_entropy;
  
  delete testcpu;
}


void cLandscape::SampleProcess(cAvidaContext& ctx)
{
  distance = 1;
  
  cGenome mod_genome(base_genome);
  int genome_size = base_genome.GetSize();

  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet());
  
  ProcessBase(ctx, testcpu);
  
  // Set to default number of trials if trials has not been specified
  if (trials == 0) trials = inst_set.GetSize() - 1;
  
  // Loop through all the lines of genome, testing each line.
  for (int line_num = 0; line_num < genome_size; line_num++) {
    cInstruction cur_inst( base_genome.GetSequence()[line_num] );
    
    for (int i = 0; i < trials; i++) {
      // Choose the new instruction for that line...
      cInstruction new_inst(inst_set.GetRandomInst(ctx) );
      if (cur_inst == new_inst) { i--; continue; }
      
      // Make the change, and test it!
      mod_genome.GetSequence()[line_num] = new_inst;
      ProcessGenome(ctx, testcpu, mod_genome);
    }
    
    mod_genome.GetSequence()[line_num] = cur_inst;
  }
  
  delete testcpu;
}


void cLandscape::RandomProcess(cAvidaContext& ctx)
{
  cGenome mod_genome(base_genome);
  int genome_size = base_genome.GetSize();
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet());
  ProcessBase(ctx, testcpu);
  
  // Set to default number of trials if trials has not been specified
  if (trials == 0) trials = inst_set.GetSize() - 1;
  
  int mut_num;
  tArray<int> mut_lines(distance);
  
  // Loop through all the lines of genome, testing many combinations.
  int cur_trial = 0;
  int total_found = 0;
    
  for (cur_trial = 0; (cur_trial < trials) || (total_found < m_min_found && cur_trial < m_max_trials); cur_trial++) {    
    // Choose the lines to mutate...
    ctx.GetRandom().Choose(genome_size, mut_lines);
    
    // Choose the new instructions for those lines...
    for (mut_num = 0; mut_num < distance; mut_num++) {
      const cInstruction new_inst(inst_set.GetRandomInst(ctx));
      const cInstruction& cur_inst = base_genome.GetSequence()[ mut_lines[mut_num] ];
      if (cur_inst == new_inst) {
        mut_num--;
        continue;
      }
      
      mod_genome.GetSequence()[ mut_lines[mut_num] ] = new_inst;
    }
    
    // And test it!
    
    ProcessGenome(ctx, testcpu, mod_genome);
    
    
    // And reset the genome.
    for (mut_num = 0; mut_num < distance; mut_num++) {
      mod_genome.GetSequence()[ mut_lines[mut_num] ] = base_genome.GetSequence()[ mut_lines[mut_num] ];
    }
  }
  
  trials = cur_trial;

  delete testcpu;
  
  m_num_found = total_found;
}

void cLandscape::BuildFitnessChart(cAvidaContext& ctx, cTestCPU* testcpu)
{
  // First, resize the fitness_chart.
  const int max_line = base_genome.GetSize();
  const int inst_size = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize();
  fitness_chart.ResizeClear(max_line, inst_size);
  
  cGenome mod_genome(base_genome);
  
  // Loop through all the lines of genome, testing trying all combinations.
  for (int line_num = 0; line_num < max_line; line_num++) {
    int cur_inst = base_genome.GetSequence()[line_num].GetOp();
    
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) {
        fitness_chart(line_num, inst_num) = base_fitness;
        continue;
      }
      
      mod_genome.GetSequence()[line_num].SetOp(inst_num);
      ProcessGenome(ctx, testcpu, mod_genome);
      fitness_chart(line_num, inst_num) = m_cpu_test_info.GetColonyFitness();
    }
    
    mod_genome.GetSequence()[line_num].SetOp(cur_inst);
  }
}

void cLandscape::TestPairs(cAvidaContext& ctx)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet());
  
  ProcessBase(ctx, testcpu);
  if (base_fitness == 0.0) return;
  
  BuildFitnessChart(ctx, testcpu);
  
  cGenome mod_genome(base_genome);
  const int genome_size = base_genome.GetSize();
  
  tArray<int> mut_lines(2);
  tArray<cInstruction> mut_insts(2);
  
  // Loop through all the lines of genome, testing many combinations.
  for (int i = 0; i < trials; i++) {
    // Choose the lines to mutate...
    ctx.GetRandom().Choose(genome_size, mut_lines);
    
    // Choose the new instructions for those lines...
    for (int mut_num = 0; mut_num < 2; mut_num++) {
      const cInstruction new_inst( inst_set.GetRandomInst(ctx) );
      const cInstruction& cur_inst = base_genome.GetSequence()[ mut_lines[mut_num] ];
      if (cur_inst == new_inst) {
        mut_num--;
        continue;
      }
      
      mut_insts[mut_num] = new_inst;
    }
    
    TestMutPair(ctx, testcpu, mod_genome, mut_lines[0], mut_lines[1], mut_insts[0], mut_insts[1]);
  }
  delete testcpu;
}


void cLandscape::TestAllPairs(cAvidaContext& ctx)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  ProcessBase(ctx, testcpu);
  if (base_fitness == 0.0) return;
  
  BuildFitnessChart(ctx, testcpu);
  
  const int max_line = base_genome.GetSize();
  const int inst_size = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize();
  cGenome mod_genome(base_genome);
  cInstruction inst1, inst2;
  
  // Loop through all the lines of genome, testing trying all combinations.
  for (int line1_num = 0; line1_num < max_line - 1; line1_num++) {
    for (int line2_num = line1_num + 1; line2_num < max_line; line2_num++) {
      
      // Loop through all instructions...
      for (int inst1_num = 0; inst1_num < inst_size; inst1_num++) {
        inst1.SetOp(inst1_num);
        if (inst1 == base_genome.GetSequence()[line1_num]) continue;
        for (int inst2_num = 0; inst2_num < inst_size; inst2_num++) {
          inst2.SetOp(inst2_num);
          if (inst2 == base_genome.GetSequence()[line2_num]) continue;
          TestMutPair(ctx, testcpu, mod_genome, line1_num, line2_num, inst1, inst2);
        } // inst2_num loop
      } //inst1_num loop;
      
    } // line2_num loop
  } // line1_num loop.
  
  delete testcpu;
}


void cLandscape::HillClimb(cAvidaContext& ctx, cDataFile& df)
{
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cGenome cur_genome(base_genome);
  cGenome mg(base_genome);
  cCPUMemory mod_genome = mg.GetSequence();

  int gen = 0;
  
  const int inst_size = m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize();
  double pos_frac = 1.0;
  
  distance = 1;
  
  bool finished = false;
  while (finished == false) {
    if (pos_frac == 0.0) finished = true;
    
    // Search the landscape for the next best.
    Reset(cur_genome);
    const int max_line = cur_genome.GetSize();
    
    // Try all Mutations...
    Process(ctx);
    
    // Try Insertion Mutations.
    
    mod_genome = cur_genome.GetSequence();
    for (int line_num = 0; line_num <= max_line; line_num++) {
      // Loop through all instructions...
      for (int inst_num = 0; inst_num < inst_size; inst_num++) {
        mod_genome.Insert(line_num, cInstruction(inst_num));
        mg.SetSequence(mod_genome);
        ProcessGenome(ctx, testcpu, mg);
        mod_genome.Remove(line_num);
      }
    }
    
    // Try all deletion mutations.
    
    for (int line_num = 0; line_num < max_line; line_num++) {
      int cur_inst = cur_genome.GetSequence()[line_num].GetOp();
      mod_genome.Remove(line_num);
      mg.SetSequence(mod_genome);
      ProcessGenome(ctx, testcpu, mg);
      mod_genome.Insert(line_num, cInstruction(cur_inst));
    }
    
    pos_frac = GetProbPos();
    
    // Print the information on the current best.
    testcpu->TestGenome(ctx, m_cpu_test_info, cur_genome);
    cPhenotype& colony_phenotype = m_cpu_test_info.GetColonyOrganism()->GetPhenotype();
    df.Write(gen, "Generation");
    df.Write(colony_phenotype.GetMerit().GetDouble(), "Merit");
    df.Write(colony_phenotype.GetGestationTime(), "Gestation Time");
    df.Write(colony_phenotype.GetFitness(), "Fitness");
    df.Write(cur_genome.GetSize(), "Genome Length");
    df.Write(GetProbDead(), "Probability Lethal");
    df.Write(GetProbNeg(), "Probability Deleterious");
    df.Write(GetProbNeut(), "Probability Neutral");
    df.Write(GetProbPos(), "Probability Beneficial");
    df.Endl();
              
    // Move on to the peak genome found.
    cur_genome = GetPeakGenome();
    gen++;
  }

  delete testcpu;
}


double cLandscape::TestMutPair(cAvidaContext& ctx, cTestCPU* testcpu, cGenome& mod_genome, int line1, int line2,
                               const cInstruction& mut1, const cInstruction& mut2)
{
  mod_genome.GetSequence()[line1] = mut1;
  mod_genome.GetSequence()[line2] = mut2;
  testcpu->TestGenome(ctx, m_cpu_test_info, mod_genome);
  double combo_fitness = m_cpu_test_info.GetColonyFitness() / base_fitness;
  
  mod_genome.GetSequence()[line1] = base_genome.GetSequence()[line1];
  mod_genome.GetSequence()[line2] = base_genome.GetSequence()[line2];
  
  double mut1_fitness = fitness_chart(line1, mut1.GetOp()) / base_fitness;
  double mut2_fitness = fitness_chart(line2, mut2.GetOp()) / base_fitness;
  double mult_combo = mut1_fitness * mut2_fitness;
    
  total_epi_count++;
  if ((mut1_fitness == 0 || mut2_fitness == 0) && (combo_fitness == 0)) {
    dead_epi_count++;
  } else if (combo_fitness < mult_combo) {
    neg_epi_count++;
    neg_epi_size = neg_epi_size + combo_fitness;
  } else if (combo_fitness > mult_combo) {
    pos_epi_count++;
    pos_epi_size = pos_epi_size + combo_fitness;
  } else {
    no_epi_count++;
    no_epi_size = no_epi_size + combo_fitness;
  }
  
  return combo_fitness;
}


void cLandscape::PrintStats(cDataFile& df, int update)
{
  df.Write(update, "Update");
  df.Write(GetProbDead(), "Probability Lethal");
  df.Write(GetProbNeg(), "Probability Deleterious");
  df.Write(GetProbNeut(), "Probability Neutral");
  df.Write(GetProbPos(), "Probability Beneficial");
  df.Write(GetAvPosSize(), "Average Beneficial Size");
  df.Write(GetAvNegSize(), "Average Deleterious Size");
  df.Write(total_count, "Total Mutants");
  df.Write(distance, "Distance");
  df.Write(base_fitness, "Base Fitness");
  df.Write(base_merit, "Base Merit");
  df.Write(base_gestation, "Base Gestation");
  df.Write(peak_fitness, "Peak Fitness");
  df.Write(GetAveFitness(), "Average Fitness");
  df.Write(GetAveSqrFitness(), "Average Square Fitness");
  df.Write(total_entropy, "Total Entropy");
  df.Write(complexity, "Total Complexity");
  df.Write(GetProbEpiDead(), "Probability Lethal Epistasis");
  df.Write(GetProbEpiPos(), "Probability Synergistic Epistasis");
  df.Write(GetProbEpiNeg(), "Probability Antagonistic Epistasis");
  df.Write(GetProbNoEpi(), "Probability No Epistasis");
  df.Write(GetAvPosEpiSize(), "Average Synergistic Epistasis Size");
  df.Write(GetAvNegEpiSize(), "Average Antagonistic Epistasis Size");
  df.Write(GetAvNoEpiSize(), "Average Size - No Epistasis");
  df.Write(total_epi_count, "Total Epistasis Count");
  df.Endl();
}

void cLandscape::PrintEntropy(cDataFile& df)
{
  df.WriteComment("Entropy Data");
  double max_ent = log(static_cast<double>(m_world->GetHardwareManager().GetInstSet(base_genome.GetInstSet()).GetSize()));
  for (int j = 0; j < base_genome.GetSize(); j++) df.WriteAnonymous(log(static_cast<double>(site_count[j] + 1)) / max_ent);
  df.Endl();
}

void cLandscape::PrintSiteCount(cDataFile& df)
{
  df.WriteComment("Site Counts");
  for (int j = 0; j < base_genome.GetSize(); j++) df.WriteAnonymous(site_count[j]);
  df.Endl();
}
