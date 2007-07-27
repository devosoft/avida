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
      sStep& odata = m_onestep[cur_site];
      odata.peak_fitness = m_base_fitness;
      odata.peak_genome = m_base_genome;
      odata.site_count.Resize(m_base_genome.GetSize(), 0);

      // Setup Data Used in Two Step
      sStep& tdata = m_twostep[cur_site];
      tdata.peak_fitness = m_base_fitness;
      tdata.peak_genome = m_base_genome;
      tdata.site_count.Resize(m_base_genome.GetSize(), 0);

      // Do the processing, starting with One Step
      ProcessOneStepPoint(ctx, testcpu, test_info, cur_site);

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
  m_onestep.ResizeClear(m_base_genome.GetSize());
  m_twostep.ResizeClear(m_base_genome.GetSize());
  m_fitness.ResizeClear(m_base_genome.GetSize(), m_inst_set.GetSize());
  
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
  sStep& odata = m_onestep[cur_site];
  
  cGenome mod_genome(m_base_genome);
  
  // Loop through all the lines of genome, testing trying all combinations.
  int cur_inst = mod_genome[cur_site].GetOp();
  
  // Fill in unmutated entry in fitness table with base fitness
  m_fitness[cur_site][cur_inst] = m_base_fitness;
  
  // Loop through all instructions...
  for (int inst_num = 0; inst_num < inst_size; inst_num++) {
    if (cur_inst == inst_num) continue;
    
    mod_genome[cur_site].SetOp(inst_num);
    testcpu->TestGenome(ctx, test_info, mod_genome);
    
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
    
    m_fitness[cur_site][cur_inst] = test_fitness;

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

    ProcessTwoStepPoint(ctx, testcpu, test_info, cur_site, mod_genome);
  }
}

void cMutationalNeighborhood::ProcessTwoStepPoint(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                  int cur_site, cGenome& mod_genome)
{
  const int inst_size = m_inst_set.GetSize();
  sStep& tdata = m_twostep[cur_site];

  // Loop through remaining lines of genome, testing trying all combinations.
  for (int line_num = cur_site + 1; line_num < m_base_genome.GetSize(); line_num++) {
    int cur_inst = mod_genome[line_num].GetOp();
    
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) continue;
      
      mod_genome[line_num].SetOp(inst_num);
      testcpu->TestGenome(ctx, test_info, mod_genome);
      
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
      
      if (test_fitness >= m_neut_min) tdata.site_count[line_num]++;
      
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
          m_pending.Push(new sPendingTarget(cur_site, mod_genome[cur_site].GetOp()));
          m_pending.Push(new sPendingTarget(line_num, inst_num));
        }
      }
      
    }
    
    mod_genome[line_num].SetOp(cur_inst);
  }
}


void cMutationalNeighborhood::ProcessComplete(cAvidaContext& ctx)
{
  // Initialize values
  m_o_total = 0;
  m_o_total_sqr_fitness = 0.0;
  m_o_pos = 0;
  m_o_neg = 0;
  m_o_neut = 0;
  m_o_dead = 0;
  m_o_size_pos = 0.0;
  m_o_size_neg = 0.0;
  m_o_peak_fitness = m_base_fitness;
  m_o_peak_genome = m_base_genome;  
  m_o_site_count.Resize(m_base_genome.GetSize(), 0);
  m_o_total_entropy = 0;
  m_o_task_target = 0;
  m_o_task_total = 0;
  m_o_task_knockout = 0;
  m_o_task_size_target = 0.0;
  m_o_task_size_total = 0.0;
  m_o_task_size_knockout = 0.0;

  for (int i = 0; i < m_onestep.GetSize(); i++) {
    sStep& odata = m_onestep[i];
    m_o_total += odata.total;
    m_o_total_fitness += odata.total_fitness;
    m_o_total_sqr_fitness += odata.total_sqr_fitness;
    m_o_pos += odata.pos;
    m_o_neg += odata.neg;
    m_o_neut += odata.neut;
    m_o_dead += odata.dead;
    m_o_size_pos += odata.size_pos; 
    m_o_size_neg += odata.size_neg; 
  
    if (odata.peak_fitness > m_o_peak_fitness) {
      m_o_peak_genome = odata.peak_genome;
      m_o_peak_fitness = odata.peak_fitness;
    }
  
  
    for (int j = 0; j < m_o_site_count.GetSize(); j++) {
      m_o_site_count[j] += odata.site_count[j];
    }
      
    m_o_task_target += odata.task_target;
    m_o_task_total += odata.task_total;
    m_o_task_knockout += odata.task_knockout;

    m_o_task_size_target += odata.task_size_target;
    m_o_task_size_total += odata.task_size_total;
    m_o_task_size_knockout += odata.task_size_knockout;
  }
  
  const double max_ent = log(static_cast<double>(m_inst_set.GetSize()));
  for (int i = 0; i < m_base_genome.GetSize(); i++) {
    // Per-site entropy is the log of the number of legal states for that
    // site.  Add one to account for the unmutated state.
    m_o_total_entropy += log(static_cast<double>(m_o_site_count[i] + 1)) / max_ent;
  }
  m_o_complexity = m_base_genome.GetSize() - m_o_total_entropy;
  
  
  // Initialize values
  m_t_total = 0;
  m_t_total_sqr_fitness = 0.0;
  m_t_pos = 0;
  m_t_neg = 0;
  m_t_neut = 0;
  m_t_dead = 0;
  m_t_size_pos = 0.0;
  m_t_size_neg = 0.0;
  m_t_peak_fitness = m_base_fitness;
  m_t_peak_genome = m_base_genome;  
  m_t_site_count.Resize(m_base_genome.GetSize(), 0);
  m_t_total_entropy = 0;
  m_t_task_target = 0;
  m_t_task_target_pos = 0;
  m_t_task_target_neg = 0;
  m_t_task_target_neut = 0;
  m_t_task_target_dead = 0;
  m_t_task_total = 0;
  m_t_task_knockout = 0;
  m_t_task_size_target = 0.0;
  m_t_task_size_target_pos = 0.0;
  m_t_task_size_target_neg = 0.0;
  m_t_task_size_total = 0.0;
  m_t_task_size_knockout = 0.0;
  
  for (int i = 0; i < m_twostep.GetSize(); i++) {
    sStep& tdata = m_twostep[i];
    m_t_total += tdata.total;
    m_t_total_fitness += tdata.total_fitness;
    m_t_total_sqr_fitness += tdata.total_sqr_fitness;
    m_t_pos += tdata.pos;
    m_t_neg += tdata.neg;
    m_t_neut += tdata.neut;
    m_t_dead += tdata.dead;
    m_t_size_pos += tdata.size_pos; 
    m_t_size_neg += tdata.size_neg; 
  
    if (tdata.peak_fitness > m_t_peak_fitness) {
      m_t_peak_genome = tdata.peak_genome;
      m_t_peak_fitness = tdata.peak_fitness;
    }
  
  
    for (int j = 0; j < m_t_site_count.GetSize(); j++) {
      m_t_site_count[j] += tdata.site_count[j];
    }
      
    m_t_task_target += tdata.task_target;
    m_t_task_total += tdata.task_total;
    m_t_task_knockout += tdata.task_knockout;
    
    m_t_task_size_target += tdata.task_size_target;
    m_t_task_size_total += tdata.task_size_total;
    m_t_task_size_knockout += tdata.task_size_knockout;
  }

  for (int i = 0; i < m_base_genome.GetSize(); i++) {
    // Per-site entropy is the log of the number of legal states for that
    // site.  Add one to account for the unmutated state.
    m_t_total_entropy += log(static_cast<double>(m_t_site_count[i] + 1)) / max_ent;
  }
  m_t_complexity = m_base_genome.GetSize() - m_t_total_entropy;

  sPendingTarget* pend = NULL;
  while ((pend = m_pending.Pop())) {
    double fitness = m_fitness[pend->site][pend->inst];
    
    if (fitness == 0.0) {
      m_t_task_target_dead++;
    } else if (fitness < m_neut_min) {
      m_t_task_target_neg++;
      m_t_task_size_target_neg += fitness;
    } else if (fitness <= m_neut_max) {
      m_t_task_target_neut++;
    } else {
      m_t_task_target_pos++;
      m_t_task_size_target_pos += fitness;
    }
    
    delete pend;
  }
  
  m_rwlock.WriteUnlock();
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

  df.Write(GetSingleTotal(), "Total One Step Mutants");
  df.Write(GetSingleProbBeneficial(), "One Step Probability Beneficial");
  df.Write(GetSingleProbDeleterious(), "One Step Probability Deleterious");
  df.Write(GetSingleProbNeutral(), "One Step Probability Neutral");
  df.Write(GetSingleProbLethal(), "One Step Probability Lethal");
  df.Write(GetSingleAverageSizeBeneficial(), "One Step Average Beneficial Size");
  df.Write(GetSingleAverageSizeDeleterious(), "One Step Average Deleterious Size");
  df.Write(GetSinglePeakFitness(), "One Step Peak Fitness");
  df.Write(GetSingleAverageFitness(), "One Step Average Fitness");
  df.Write(GetSingleAverageSqrFitness(), "One Step Average Square Fitness");
  df.Write(GetSingleTotalEntropy(), "One Step Total Entropy");
  df.Write(GetSingleComplexity(), "One Step Total Complexity");
  df.Write(GetSingleTargetTask(), "One Step Confers Target Task");
  df.Write(GetSingleProbTargetTask(), "One Step Probability Confers Target Task");
  df.Write(GetSingleAverageSizeTargetTask(), "One Step Average Size of Target Task Conferral");
  df.Write(GetSingleTask(), "One Step Confers Any Task");
  df.Write(GetSingleProbTask(), "One Step Probability Confers Any Task");
  df.Write(GetSingleAverageSizeTask(), "One Step Average Size of Any Task Conferral");
  df.Write(GetSingleKnockout(), "One Step Knockout Task");
  df.Write(GetSingleProbKnockout(), "One Step Probability Knockout Task");
  df.Write(GetSingleAverageSizeKnockout(), "One Step Average Size of Task Knockout");

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
