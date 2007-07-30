/*
 *  cMutationalNeighborhood.cc
 *  Avida
 *
 *  Created by David on 6/13/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "cMutationalNeighborhood.h"

#include "cAnalyze.h"
#include "cAnalyzeJobQueue.h"
#include "cCPUTestInfo.h"
#include "cCPUMemory.h"
#include "cEnvironment.h"
#include "cInstSet.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cTools.h"
#include "cWorld.h"
#include "tAnalyzeJob.h"

using namespace std;


void cMutationalNeighborhood::Process(cAvidaContext& ctx)
{
  m_mutex.Lock();
  if (m_initialized) {
    int cur_site = m_cur_site++;
    m_mutex.Unlock();

    if (cur_site < m_base_genome.GetSize()) {
      // Create test infrastructure
      cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
      cCPUTestInfo test_info;
      
      // Setup One Step Data
      sStep& opdata = m_onestep_point[cur_site];
      opdata.peak_fitness = m_base_fitness;
      opdata.peak_genome = m_base_genome;
      opdata.site_count.Resize(m_base_genome.GetSize(), 0);

      sStep& oidata = m_onestep_insert[cur_site];
      oidata.peak_fitness = m_base_fitness;
      oidata.peak_genome = m_base_genome;
      oidata.site_count.Resize(m_base_genome.GetSize() + 1, 0);

      sStep& oddata = m_onestep_delete[cur_site];
      oddata.peak_fitness = m_base_fitness;
      oddata.peak_genome = m_base_genome;
      oddata.site_count.Resize(m_base_genome.GetSize(), 0);
      
      // Setup Data Used in Two Step
      sStep& tdata = m_twostep_point[cur_site];
      tdata.peak_fitness = m_base_fitness;
      tdata.peak_genome = m_base_genome;
      tdata.site_count.Resize(m_base_genome.GetSize(), 0);

      // Do the processing, starting with One Step
      ProcessOneStepPoint(ctx, testcpu, test_info, cur_site);
      ProcessOneStepInsert(ctx, testcpu, test_info, cur_site);
      if (cur_site == (m_base_genome.GetSize() - 1)) {
        // Process the hanging insertion on the last cycle through
        ProcessOneStepInsert(ctx, testcpu, test_info, cur_site + 1); 
      }
      ProcessOneStepDelete(ctx, testcpu, test_info, cur_site);

      // Cleanup
      delete testcpu;
    }
  } else {
    ProcessInitialize(ctx);
    return;
  }
  
  m_mutex.Lock();
  if (++m_completed == m_base_genome.GetSize()) ProcessComplete(ctx); 
  m_mutex.Unlock();
}


void cMutationalNeighborhood::ProcessInitialize(cAvidaContext& ctx)
{
  // Generate base information
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cCPUTestInfo test_info;
  testcpu->TestGenome(ctx, test_info, m_base_genome);
  
  cPhenotype& phenotype = test_info.GetColonyOrganism()->GetPhenotype();
  m_base_fitness = test_info.GetColonyFitness();
  m_base_merit = phenotype.GetMerit().GetDouble();
  m_base_gestation = phenotype.GetGestationTime();
  m_base_tasks = phenotype.GetLastTaskCount();
  
  m_neut_min = m_base_fitness * nHardware::FITNESS_NEUTRAL_MIN;
  m_neut_max = m_base_fitness * nHardware::FITNESS_NEUTRAL_MAX;
  
  // If invalid target supplied, set to the last task
  if (m_target >= m_base_tasks.GetSize() || m_target < 0) m_target = m_base_tasks.GetSize() - 1;
  
  delete testcpu;

  // Setup state to begin processing
  m_onestep_point.ResizeClear(m_base_genome.GetSize());
  m_onestep_insert.ResizeClear(m_base_genome.GetSize() + 1);
  m_onestep_delete.ResizeClear(m_base_genome.GetSize());
  m_twostep_point.ResizeClear(m_base_genome.GetSize());
  m_fitness_point.ResizeClear(m_base_genome.GetSize(), m_inst_set.GetSize());
  m_fitness_insert.ResizeClear(m_base_genome.GetSize() + 1, m_inst_set.GetSize());
  m_fitness_delete.ResizeClear(m_base_genome.GetSize());
  
  m_cur_site = 0;
  m_completed = 0;
  m_initialized = true;
  
  // Unlock internal mutex (was locked on Process() entrance)
  //  - will allow workers to begin processing if job queue already active
  m_mutex.Unlock();
  
  // Load enough jobs to process all sites
  cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
  for (int i = 0; i < m_base_genome.GetSize(); i++)
    jobqueue.AddJob(new tAnalyzeJob<cMutationalNeighborhood>(this, &cMutationalNeighborhood::Process));
  
  jobqueue.Start();
}


void cMutationalNeighborhood::ProcessOneStepPoint(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site)
{
  const int inst_size = m_inst_set.GetSize();
  sStep& odata = m_onestep_point[cur_site];
  
  cGenome mod_genome(m_base_genome);
  
  // Loop through all the lines of genome, testing trying all combinations.
  int cur_inst = mod_genome[cur_site].GetOp();
  
  // Fill in unmutated entry in fitness table with base fitness
  m_fitness_point[cur_site][cur_inst] = m_base_fitness;
  
  // Loop through all instructions...
  for (int inst_num = 0; inst_num < inst_size; inst_num++) {
    if (cur_inst == inst_num) continue;
    
    mod_genome[cur_site].SetOp(inst_num);
    m_fitness_point[cur_site][inst_num] = ProcessOneStepGenome(ctx, testcpu, test_info, mod_genome, odata, cur_site);

    ProcessTwoStepPoint(ctx, testcpu, test_info, cur_site, mod_genome);
  }
}

void cMutationalNeighborhood::ProcessOneStepInsert(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site)
{
  const int inst_size = m_inst_set.GetSize();
  sStep& odata = m_onestep_insert[cur_site];
  
  cCPUMemory mod_genome(m_base_genome);
  mod_genome.Insert(cur_site, cInstruction(0));
  
  // Loop through all instructions...
  for (int inst_num = 0; inst_num < inst_size; inst_num++) {
    mod_genome[cur_site].SetOp(inst_num);
    m_fitness_insert[cur_site][inst_num] = ProcessOneStepGenome(ctx, testcpu, test_info, mod_genome, odata, cur_site);
    
    //ProcessTwoStepPoint(ctx, testcpu, test_info, cur_site, mod_genome);
  }  
}


void cMutationalNeighborhood::ProcessOneStepDelete(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site)
{
  sStep& odata = m_onestep_delete[cur_site];
  
  cCPUMemory mod_genome(m_base_genome);
  mod_genome.Remove(cur_site);
  m_fitness_delete[cur_site] = ProcessOneStepGenome(ctx, testcpu, test_info, mod_genome, odata, cur_site);
  //ProcessTwoStepPoint(ctx, testcpu, test_info, cur_site, mod_genome);
}


double cMutationalNeighborhood::ProcessOneStepGenome(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                     const cGenome& mod_genome, sStep& odata, int cur_site)
{
  // Run the modified genome through the Test CPU
  testcpu->TestGenome(ctx, test_info, mod_genome);
  
  // Collect the calculated fitness
  double test_fitness = test_info.GetColonyFitness();
  
  
  odata.total_fitness += test_fitness;
  odata.total_sqr_fitness += test_fitness * test_fitness;
  odata.total++;
  if (test_fitness == 0.0) {
    odata.dead++;
  } else if (test_fitness < m_neut_min) {
    odata.neg++;
    odata.size_neg += test_fitness;
  } else if (test_fitness <= m_neut_max) {
    odata.neut++;
  } else {
    odata.pos++;
    odata.size_pos += test_fitness;
    if (test_fitness > odata.peak_fitness) {
      odata.peak_fitness = test_fitness;
      odata.peak_genome = mod_genome;
    }
  }
  
  if (test_fitness >= m_neut_min) odata.site_count[cur_site]++;
  
  if (test_fitness != 0.0) { // Only count tasks if the organism is alive
    const tArray<int>& cur_tasks = test_info.GetColonyOrganism()->GetPhenotype().GetLastTaskCount();    
    bool knockout = false;
    bool anytask = false;
    for (int i = 0; i < m_base_tasks.GetSize(); i++) {
      if (m_base_tasks[i] && !cur_tasks[i]) knockout = true;
      else if (!m_base_tasks[i] && cur_tasks[i]) anytask = true;
    }
    if (knockout) {
      odata.task_knockout++;
      odata.task_size_knockout += test_fitness;
    }
    if (anytask) {
      odata.task_total++;
      odata.task_size_total += test_fitness;
    }
    if (m_base_tasks.GetSize() && !m_base_tasks[m_target] && cur_tasks[m_target]) {
      odata.task_target++;
      odata.task_size_target += test_fitness;
    }
  }
  
  return test_fitness;
}



void cMutationalNeighborhood::ProcessTwoStepPoint(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                  int cur_site, cGenome& mod_genome)
{
  const int inst_size = m_inst_set.GetSize();
  sTwoStep& tdata = m_twostep_point[cur_site];

  // Loop through remaining lines of genome, testing trying all combinations.
  for (int line_num = cur_site + 1; line_num < m_base_genome.GetSize(); line_num++) {
    int cur_inst = mod_genome[line_num].GetOp();
    
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) continue;
      
      mod_genome[line_num].SetOp(inst_num);
      ProcessTwoStepGenome(ctx, testcpu, test_info, mod_genome, tdata, line_num, cur_site);
    }
    
    mod_genome[line_num].SetOp(cur_inst);
  }
}


double cMutationalNeighborhood::ProcessTwoStepGenome(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                     const cGenome& mod_genome, sTwoStep& tdata, int cur_site, int oth_site)
{
  // Run the modified genome through the Test CPU
  testcpu->TestGenome(ctx, test_info, mod_genome);
  
  // Collect the calculated fitness
  double test_fitness = test_info.GetColonyFitness();
  
  tdata.total_fitness += test_fitness;
  tdata.total_sqr_fitness += test_fitness * test_fitness;
  tdata.total++;
  if (test_fitness == 0.0) {
    tdata.dead++;
  } else if (test_fitness < m_neut_min) {
    tdata.neg++;
    tdata.size_neg += test_fitness;
  } else if (test_fitness <= m_neut_max) {
    tdata.neut++;
  } else {
    tdata.pos++;
    tdata.size_pos += test_fitness;
    if (test_fitness > tdata.peak_fitness) {
      tdata.peak_fitness = test_fitness;
      tdata.peak_genome = mod_genome;
    }
  }
  
  if (test_fitness >= m_neut_min) tdata.site_count[cur_site]++;
  
  if (test_fitness != 0.0) { // Only count tasks if the organism is alive
    const tArray<int>& cur_tasks = test_info.GetColonyOrganism()->GetPhenotype().GetLastTaskCount();    
    bool knockout = false;
    bool anytask = false;
    for (int i = 0; i < m_base_tasks.GetSize(); i++) {
      if (m_base_tasks[i] && !cur_tasks[i]) knockout = true;
      else if (!m_base_tasks[i] && cur_tasks[i]) anytask = true;
    }
    if (knockout) {
      tdata.task_knockout++;
      tdata.task_size_knockout += test_fitness;
    }
    if (anytask) {
      tdata.task_total++;
      tdata.task_size_total += test_fitness;
    }
    if (m_base_tasks.GetSize() && !m_base_tasks[m_target] && cur_tasks[m_target]) {
      tdata.task_target++;
      tdata.task_size_target += test_fitness;
      
      // Push both instructions as possible first mutations, for post determination of first step fitness effect
      tdata.pending.Push(new sPendingTarget(oth_site, mod_genome[oth_site].GetOp()));
      tdata.pending.Push(new sPendingTarget(cur_site, mod_genome[cur_site].GetOp()));
    }
  }
  
  return test_fitness;
}

void cMutationalNeighborhood::ProcessComplete(cAvidaContext& ctx)
{
  m_op.peak_fitness = m_base_fitness;
  m_op.peak_genome = m_base_genome;
  m_op.site_count.Resize(m_base_genome.GetSize(), 0);
  AggregateOneStep(m_onestep_point, m_op);

  m_oi.peak_fitness = m_base_fitness;
  m_oi.peak_genome = m_base_genome;
  m_oi.site_count.Resize(m_base_genome.GetSize() + 1, 0);
  AggregateOneStep(m_onestep_insert, m_oi);

  m_od.peak_fitness = m_base_fitness;
  m_od.peak_genome = m_base_genome;
  m_od.site_count.Resize(m_base_genome.GetSize(), 0);
  AggregateOneStep(m_onestep_delete, m_od);
  
  
  // Collect totals across all one step mutants
  m_ot.total = m_op.total + m_oi.total + m_od.total;
  m_ot.total_fitness = m_op.total_fitness + m_oi.total_fitness + m_od.total_fitness;
  m_ot.total_sqr_fitness = m_op.total_sqr_fitness + m_oi.total_sqr_fitness + m_od.total_sqr_fitness;

  if (m_op.peak_fitness >= m_oi.peak_fitness && m_op.peak_fitness >= m_od.peak_fitness) {
    m_ot.peak_fitness = m_op.peak_fitness;
    m_ot.peak_genome = m_op.peak_genome;
  } else if (m_oi.peak_fitness >= m_od.peak_fitness) {
    m_ot.peak_fitness = m_oi.peak_fitness;
    m_ot.peak_genome = m_oi.peak_genome;
  } else {
    m_ot.peak_fitness = m_od.peak_fitness;
    m_ot.peak_genome = m_od.peak_genome;
  }
  
  m_ot.pos = m_op.pos + m_oi.pos + m_od.pos;
  m_ot.neg = m_op.neg + m_oi.neg + m_od.neg;
  m_ot.neut = m_op.neut + m_oi.neut + m_od.neut;
  m_ot.dead = m_op.dead + m_oi.dead + m_od.dead;
  m_ot.size_pos = m_op.size_pos + m_oi.size_pos + m_od.size_pos;
  m_ot.size_neg = m_op.size_neg + m_oi.size_neg + m_od.size_neg;
  
  // @TODO - total_entropy/complexity across all mutation classes?

  m_ot.task_target = m_op.task_target + m_oi.task_target + m_od.task_target;
  m_ot.task_total = m_op.task_total + m_oi.task_total + m_od.task_total;
  m_ot.task_knockout = m_op.task_knockout + m_oi.task_knockout + m_od.task_knockout;

  m_ot.task_size_target = m_op.task_size_target + m_oi.task_size_target + m_od.task_size_target;
  m_ot.task_size_total = m_op.task_size_total + m_oi.task_size_total + m_od.task_size_total;
  m_ot.task_size_knockout = m_op.task_size_knockout + m_oi.task_size_knockout + m_od.task_size_knockout;


  
  
  // Initialize values
  sPendingTarget* pend = NULL;
  
  m_tp.peak_fitness = m_base_fitness;
  m_tp.peak_genome = m_base_genome;  
  m_tp.site_count.Resize(m_base_genome.GetSize(), 0);
  
  for (int i = 0; i < m_twostep_point.GetSize(); i++) {
    sTwoStep& tdata = m_twostep_point[i];
    m_tp.total += tdata.total;
    m_tp.total_fitness += tdata.total_fitness;
    m_tp.total_sqr_fitness += tdata.total_sqr_fitness;
    m_tp.pos += tdata.pos;
    m_tp.neg += tdata.neg;
    m_tp.neut += tdata.neut;
    m_tp.dead += tdata.dead;
    m_tp.size_pos += tdata.size_pos; 
    m_tp.size_neg += tdata.size_neg; 
  
    if (tdata.peak_fitness > m_tp.peak_fitness) {
      m_tp.peak_genome = tdata.peak_genome;
      m_tp.peak_fitness = tdata.peak_fitness;
    }
  
  
    for (int j = 0; j < m_tp.site_count.GetSize(); j++) {
      m_tp.site_count[j] += tdata.site_count[j];
    }
      
    m_tp.task_target += tdata.task_target;
    m_tp.task_total += tdata.task_total;
    m_tp.task_knockout += tdata.task_knockout;
    
    m_tp.task_size_target += tdata.task_size_target;
    m_tp.task_size_total += tdata.task_size_total;
    m_tp.task_size_knockout += tdata.task_size_knockout;

    while ((pend = tdata.pending.Pop())) {
      double fitness = m_fitness_point[pend->site][pend->inst];
      
      if (fitness == 0.0) {
        m_tp.task_target_dead++;
      } else if (fitness < m_neut_min) {
        m_tp.task_target_neg++;
        m_tp.task_size_target_neg += fitness;
      } else if (fitness <= m_neut_max) {
        m_tp.task_target_neut++;
      } else {
        m_tp.task_target_pos++;
        m_tp.task_size_target_pos += fitness;
      }
      
      delete pend;
    }
  }

  const double max_ent = log(static_cast<double>(m_inst_set.GetSize()));
  for (int i = 0; i < m_base_genome.GetSize(); i++) {
    // Per-site entropy is the log of the number of legal states for that
    // site.  Add one to account for the unmutated state.
    m_tp.total_entropy += log(static_cast<double>(m_tp.site_count[i] + 1)) / max_ent;
  }
  m_tp.complexity = m_base_genome.GetSize() - m_tp.total_entropy;

  m_rwlock.WriteUnlock();
}

void cMutationalNeighborhood::AggregateOneStep(tArray<sStep>& steps, sOneStepAggregate osa)
{
  for (int i = 0; i < steps.GetSize(); i++) {
    sStep& odata = steps[i];
    osa.total += odata.total;
    osa.total_fitness += odata.total_fitness;
    osa.total_sqr_fitness += odata.total_sqr_fitness;
    osa.pos += odata.pos;
    osa.neg += odata.neg;
    osa.neut += odata.neut;
    osa.dead += odata.dead;
    osa.size_pos += odata.size_pos; 
    osa.size_neg += odata.size_neg; 
    
    if (odata.peak_fitness > osa.peak_fitness) {
      osa.peak_genome = odata.peak_genome;
      osa.peak_fitness = odata.peak_fitness;
    }
    
    
    for (int j = 0; j < osa.site_count.GetSize(); j++) {
      osa.site_count[j] += odata.site_count[j];
    }
    
    osa.task_target += odata.task_target;
    osa.task_total += odata.task_total;
    osa.task_knockout += odata.task_knockout;
    
    osa.task_size_target += odata.task_size_target;
    osa.task_size_total += odata.task_size_total;
    osa.task_size_knockout += odata.task_size_knockout;
  }
  
  const double max_ent = log(static_cast<double>(m_inst_set.GetSize()));
  for (int i = 0; i < m_base_genome.GetSize(); i++) {
    // Per-site entropy is the log of the number of legal states for that
    // site.  Add one to account for the unmutated state.
    osa.total_entropy += log(static_cast<double>(osa.site_count[i] + 1)) / max_ent;
  }
  osa.complexity = m_base_genome.GetSize() - osa.total_entropy;
}


void cMutationalNeighborhood::PrintStats(cDataFile& df, int update) const
{
  df.Write(update, "Update/Tree Depth");
  
  df.Write(GetTargetTask(), "Target Task");

  df.Write(GetBaseFitness(), "Base Fitness");
  df.Write(GetBaseMerit(), "Base Merit");
  df.Write(GetBaseGestation(), "Base Gestation");
  df.Write(GetBaseGenome().GetSize(), "Base Genome Length");
  df.Write(GetBaseTargetTask(), "Base Performs Target Task");

  df.Write(Get1SAggregateTotal(), "Total 1-Step Mutants");
  df.Write(Get1SAggregateProbBeneficial(), "1-Step Probability Beneficial");
  df.Write(Get1SAggregateProbDeleterious(), "1-Step Probability Deleterious");
  df.Write(Get1SAggregateProbNeutral(), "1-Step Probability Neutral");
  df.Write(Get1SAggregateProbLethal(), "1-Step Probability Lethal");
  df.Write(Get1SAggregateAverageSizeBeneficial(), "1-Step Average Beneficial Size");
  df.Write(Get1SAggregateAverageSizeDeleterious(), "1-Step Average Deleterious Size");
  df.Write(Get1SAggregatePeakFitness(), "1-Step Peak Fitness");
  df.Write(Get1SAggregateAverageFitness(), "1-Step Average Fitness");
  df.Write(Get1SAggregateAverageSqrFitness(), "1-Step Average Square Fitness");
//  df.Write(Get1SAggregateTotalEntropy(), "1-Step Total Entropy");
//  df.Write(Get1SAggregateComplexity(), "1-Step Total Complexity");
  df.Write(Get1SAggregateTargetTask(), "1-Step Confers Target Task");
  df.Write(Get1SAggregateProbTargetTask(), "1-Step Probability Confers Target Task");
  df.Write(Get1SAggregateAverageSizeTargetTask(), "1-Step Average Size of Target Task Conferral");
  df.Write(Get1SAggregateTask(), "1-Step Confers Any Task");
  df.Write(Get1SAggregateProbTask(), "1-Step Probability Confers Any Task");
  df.Write(Get1SAggregateAverageSizeTask(), "1-Step Average Size of Any Task Conferral");
  df.Write(Get1SAggregateKnockout(), "1-Step Knockout Task");
  df.Write(Get1SAggregateProbKnockout(), "1-Step Probability Knockout Task");
  df.Write(Get1SAggregateAverageSizeKnockout(), "1-Step Average Size of Task Knockout");
  
  df.Write(Get1SPointTotal(), "Total 1-Step Point Mutants");
  df.Write(Get1SPointProbBeneficial(), "1-Step Point Probability Beneficial");
  df.Write(Get1SPointProbDeleterious(), "1-Step Point Probability Deleterious");
  df.Write(Get1SPointProbNeutral(), "1-Step Point Probability Neutral");
  df.Write(Get1SPointProbLethal(), "1-Step Point Probability Lethal");
  df.Write(Get1SPointAverageSizeBeneficial(), "1-Step Point Average Beneficial Size");
  df.Write(Get1SPointAverageSizeDeleterious(), "1-Step Point Average Deleterious Size");
  df.Write(Get1SPointPeakFitness(), "1-Step Point Peak Fitness");
  df.Write(Get1SPointAverageFitness(), "1-Step Point Average Fitness");
  df.Write(Get1SPointAverageSqrFitness(), "1-Step Point Average Square Fitness");
  df.Write(Get1SPointTotalEntropy(), "1-Step Point Total Entropy");
  df.Write(Get1SPointComplexity(), "1-Step Point Total Complexity");
  df.Write(Get1SPointTargetTask(), "1-Step Point Confers Target Task");
  df.Write(Get1SPointProbTargetTask(), "1-Step Point Probability Confers Target Task");
  df.Write(Get1SPointAverageSizeTargetTask(), "1-Step Point Average Size of Target Task Conferral");
  df.Write(Get1SPointTask(), "1-Step Point Confers Any Task");
  df.Write(Get1SPointProbTask(), "1-Step Point Probability Confers Any Task");
  df.Write(Get1SPointAverageSizeTask(), "1-Step Point Average Size of Any Task Conferral");
  df.Write(Get1SPointKnockout(), "1-Step Point Knockout Task");
  df.Write(Get1SPointProbKnockout(), "1-Step Point Probability Knockout Task");
  df.Write(Get1SPointAverageSizeKnockout(), "1-Step Point Average Size of Task Knockout");

  df.Write(Get1SInsertTotal(), "Total 1-Step Insert Mutants");
  df.Write(Get1SInsertProbBeneficial(), "1-Step Insert Probability Beneficial");
  df.Write(Get1SInsertProbDeleterious(), "1-Step Insert Probability Deleterious");
  df.Write(Get1SInsertProbNeutral(), "1-Step Insert Probability Neutral");
  df.Write(Get1SInsertProbLethal(), "1-Step Insert Probability Lethal");
  df.Write(Get1SInsertAverageSizeBeneficial(), "1-Step Insert Average Beneficial Size");
  df.Write(Get1SInsertAverageSizeDeleterious(), "1-Step Insert Average Deleterious Size");
  df.Write(Get1SInsertPeakFitness(), "1-Step Insert Peak Fitness");
  df.Write(Get1SInsertAverageFitness(), "1-Step Insert Average Fitness");
  df.Write(Get1SInsertAverageSqrFitness(), "1-Step Insert Average Square Fitness");
  df.Write(Get1SInsertTotalEntropy(), "1-Step Insert Total Entropy");
  df.Write(Get1SInsertComplexity(), "1-Step Insert Total Complexity");
  df.Write(Get1SInsertTargetTask(), "1-Step Insert Confers Target Task");
  df.Write(Get1SInsertProbTargetTask(), "1-Step Insert Probability Confers Target Task");
  df.Write(Get1SInsertAverageSizeTargetTask(), "1-Step Insert Average Size of Target Task Conferral");
  df.Write(Get1SInsertTask(), "1-Step Insert Confers Any Task");
  df.Write(Get1SInsertProbTask(), "1-Step Insert Probability Confers Any Task");
  df.Write(Get1SInsertAverageSizeTask(), "1-Step Insert Average Size of Any Task Conferral");
  df.Write(Get1SInsertKnockout(), "1-Step Insert Knockout Task");
  df.Write(Get1SInsertProbKnockout(), "1-Step Insert Probability Knockout Task");
  df.Write(Get1SInsertAverageSizeKnockout(), "1-Step Insert Average Size of Task Knockout");
  
  df.Write(Get1SDeleteTotal(), "Total 1-Step Delete Mutants");
  df.Write(Get1SDeleteProbBeneficial(), "1-Step Delete Probability Beneficial");
  df.Write(Get1SDeleteProbDeleterious(), "1-Step Delete Probability Deleterious");
  df.Write(Get1SDeleteProbNeutral(), "1-Step Delete Probability Neutral");
  df.Write(Get1SDeleteProbLethal(), "1-Step Delete Probability Lethal");
  df.Write(Get1SDeleteAverageSizeBeneficial(), "1-Step Delete Average Beneficial Size");
  df.Write(Get1SDeleteAverageSizeDeleterious(), "1-Step Delete Average Deleterious Size");
  df.Write(Get1SDeletePeakFitness(), "1-Step Delete Peak Fitness");
  df.Write(Get1SDeleteAverageFitness(), "1-Step Delete Average Fitness");
  df.Write(Get1SDeleteAverageSqrFitness(), "1-Step Delete Average Square Fitness");
  df.Write(Get1SDeleteTotalEntropy(), "1-Step Delete Total Entropy");
  df.Write(Get1SDeleteComplexity(), "1-Step Delete Total Complexity");
  df.Write(Get1SDeleteTargetTask(), "1-Step Delete Confers Target Task");
  df.Write(Get1SDeleteProbTargetTask(), "1-Step Delete Probability Confers Target Task");
  df.Write(Get1SDeleteAverageSizeTargetTask(), "1-Step Delete Average Size of Target Task Conferral");
  df.Write(Get1SDeleteTask(), "1-Step Delete Confers Any Task");
  df.Write(Get1SDeleteProbTask(), "1-Step Delete Probability Confers Any Task");
  df.Write(Get1SDeleteAverageSizeTask(), "1-Step Delete Average Size of Any Task Conferral");
  df.Write(Get1SDeleteKnockout(), "1-Step Delete Knockout Task");
  df.Write(Get1SDeleteProbKnockout(), "1-Step Delete Probability Knockout Task");
  df.Write(Get1SDeleteAverageSizeKnockout(), "1-Step Delete Average Size of Task Knockout");
  
  df.Write(GetDoubleTotal(), "Total Two Step Mutants");
  df.Write(GetDoubleProbBeneficial(), "Two Step Probability Beneficial");
  df.Write(GetDoubleProbDeleterious(), "Two Step Probability Deleterious");
  df.Write(GetDoubleProbNeutral(), "Two Step Probability Neutral");
  df.Write(GetDoubleProbLethal(), "Two Step Probability Lethal");
  df.Write(GetDoubleAverageSizeBeneficial(), "Two Step Average Beneficial Size");
  df.Write(GetDoubleAverageSizeDeleterious(), "Two Step Average Deleterious Size");
  df.Write(GetDoublePeakFitness(), "Two Step Peak Fitness");
  df.Write(GetDoubleAverageFitness(), "Two Step Average Fitness");
  df.Write(GetDoubleAverageSqrFitness(), "Two Step Average Square Fitness");
  df.Write(GetDoubleTotalEntropy(), "Two Step Total Entropy");
  df.Write(GetDoubleComplexity(), "Two Step Total Complexity");
  df.Write(GetDoubleTargetTask(), "Two Step Confers Target Task");
  df.Write(GetDoubleProbTargetTask(), "Two Step Probability Confers Target Task");
  df.Write(GetDoubleAverageSizeTargetTask(), "Two Step Average Size of Target Task Conferral");
  df.Write(GetDoubleTargetTaskBeneficial(), "Two Step Confers Target - Previous Beneficial");
  df.Write(GetDoubleProbTargetTaskBeneficial(), "Two Step Prob. Confers Target - Previous Beneficial");
  df.Write(GetDoubleAverageSizeTargetTaskBeneficial(), "Two Step Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(GetDoubleTargetTaskDeleterious(), "Two Step Confers Target - Previous Deleterious");
  df.Write(GetDoubleProbTargetTaskDeleterious(), "Two Step Prob. Confers Target - Previous Deleterious");
  df.Write(GetDoubleAverageSizeTargetTaskDeleterious(), "Two Step Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(GetDoubleTargetTaskNeutral(), "Two Step Confers Target - Previous Neutral");
  df.Write(GetDoubleProbTargetTaskNeutral(), "Two Step Prob. Confers Target - Previous Neutral");
  df.Write(GetDoubleTargetTaskLethal(), "Two Step Confers Target - Previous Lethal");
  df.Write(GetDoubleProbTargetTaskLethal(), "Two Step Prob. Confers Target - Previous Lethal");
  df.Write(GetDoubleTask(), "Two Step Confers Any Task");
  df.Write(GetDoubleProbTask(), "Two Step Probability Confers Any Task");
  df.Write(GetDoubleAverageSizeTask(), "Two Step Average Size of Any Task Conferral");
  df.Write(GetDoubleKnockout(), "Two Step Knockout Task");
  df.Write(GetDoubleProbKnockout(), "Two Step Probability Knockout Task");
  df.Write(GetDoubleAverageSizeKnockout(), "Two Step Average Size of Task Knockout");
  
  df.Endl();
}
