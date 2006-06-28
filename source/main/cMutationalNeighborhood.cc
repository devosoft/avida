/*
 *  cMutationalNeighborhood.cc
 *  Avida
 *
 *  Created by David on 6/13/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
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
#include "cStats.h"             // For GetUpdate in outputs...
#include "cTestCPU.h"
#include "cTestUtil.h"
#include "cTools.h"
#include "cWorld.h"
#include "tAnalyzeJob.h"

using namespace std;


void cMutationalNeighborhood::Process(cAvidaContext& ctx)
{
  pthread_mutex_lock(&m_mutex);
  if (m_initialized) {
    int cur_site = m_cur_site++;
    pthread_mutex_unlock(&m_mutex);

    if (cur_site < m_base_genome.GetSize()) {
      // Create test infrastructure
      cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
      cCPUTestInfo test_info;
      
      // Setup One Step Data
      sOneStep& odata = m_onestep[cur_site];
      odata.peak_fitness = m_base_fitness;
      odata.peak_genome = m_base_genome;
      odata.fitness.Resize(m_inst_set.GetSize(), 0.0);
      odata.site_count.Resize(m_base_genome.GetSize(), 0);

      // Setup Data Used in Two Step
      sTwoStep& tdata = m_twostep[cur_site];
      tdata.peak_fitness = m_base_fitness;
      tdata.peak_genome = m_base_genome;
      tdata.site_count.Resize(m_base_genome.GetSize(), 0);

      // Do the processing, starting with One Step
      ProcessOneStep(ctx, testcpu, test_info, cur_site);

      // Cleanup
      delete testcpu;
    }
  } else {
    ProcessInitialize(ctx);
    return;
  }
  
  pthread_mutex_lock(&m_mutex);
  if (++m_completed == m_base_genome.GetSize()) ProcessComplete(ctx); 
  pthread_mutex_unlock(&m_mutex);
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
  
  delete testcpu;

  // Setup state to begin processing
  m_onestep.ResizeClear(m_base_genome.GetSize());
  m_twostep.ResizeClear(m_base_genome.GetSize());
  
  m_cur_site = 0;
  m_completed = 0;
  m_initialized = true;
  
  // Unlock internal mutex (was locked on Process() entrance)
  //  - will allow workers to begin processing if job queue already active
  pthread_mutex_unlock(&m_mutex);
  
  // Load enough jobs to process all sites
  cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
  for (int i = 0; i < m_base_genome.GetSize(); i++)
    jobqueue.AddJob(new tAnalyzeJob<cMutationalNeighborhood>(this, &cMutationalNeighborhood::Process));
  
  jobqueue.Start();
}


void cMutationalNeighborhood::ProcessOneStep(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site)
{
  const int inst_size = m_inst_set.GetSize();
  sOneStep& odata = m_onestep[cur_site];
  
  cGenome mod_genome(m_base_genome);
  
  // Loop through all the lines of genome, testing trying all combinations.
  int cur_inst = mod_genome[cur_site].GetOp();
  
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
    
    odata.fitness[cur_inst] = test_fitness;
    odata.cur_tasks = test_info.GetColonyOrganism()->GetPhenotype().GetLastTaskCount();
    
    // @TODO - calculate task values

    //ProcessTwoStep(ctx, testcpu, test_info, cur_site, mod_genome);
  }
}

void cMutationalNeighborhood::ProcessTwoStep(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome)
{
  const int inst_size = m_inst_set.GetSize();
  sOneStep& odata = m_onestep[cur_site];
  sTwoStep& tdata = m_twostep[cur_site];

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
      
      // @TODO - calculate task values
    }
    
    mod_genome[line_num].SetOp(cur_inst);
  }
}


void cMutationalNeighborhood::ProcessComplete(cAvidaContext& ctx)
{
  // Initialize values
  m_total = 0;
  m_total_sqr_fitness = 0.0;
  m_dead = 0;
  m_neg = 0;
  m_neut = 0;
  m_pos = 0;
  m_size_pos = 0.0;
  m_size_neg = 0.0;
  m_peak_fitness = m_base_fitness;
  m_peak_genome = m_base_genome;  
  m_site_count.Resize(m_base_genome.GetSize(), 0);

  for (int i = 0; i < m_onestep.GetSize(); i++) {
    sOneStep& odata = m_onestep[i];
    m_total += odata.total;
    m_total_fitness += odata.total_fitness;
    m_total_sqr_fitness += odata.total_sqr_fitness;
    m_dead += odata.dead;
    m_neg += odata.neg;
    m_neut += odata.neut;
    m_pos += odata.pos;
    m_size_pos += odata.size_pos; 
    m_size_neg += odata.size_neg; 
  
    if (odata.peak_fitness > m_peak_fitness) {
      m_peak_genome = odata.peak_genome;
      m_peak_fitness = odata.peak_fitness;
    }
  
  
    for (int j = 0; j < m_site_count.GetSize(); j++) {
      m_site_count[j] += odata.site_count[j];
    }
      
    // @TODO - aggregate task data
  }
  
  double max_ent = log(static_cast<double>(m_inst_set.GetSize()));
  m_total_entropy = 0;
  for (int i = 0; i < m_base_genome.GetSize(); i++) {
    // Per-site entropy is the log of the number of legal states for that
    // site.  Add one to account for the unmutated state.
    m_total_entropy += log(static_cast<double>(m_site_count[i] + 1)) / max_ent;
  }
  m_complexity = m_base_genome.GetSize() - m_total_entropy;
  
  
  // @TODO - aggregate two step task data

  
  pthread_rwlock_unlock(&m_rwlock);
}


void cMutationalNeighborhood::PrintStats(cDataFile& df, int update)
{
  df.Write(update, "Update");
  df.Write(m_base_fitness, "Base Fitness");
  df.Write(m_base_merit, "Base Merit");
  df.Write(m_base_gestation, "Base Gestation");
  df.Write(m_total, "Total One Step Mutants");
  df.Write(GetProbDead(), "One Step Probability Dead");
  df.Write(GetProbNeg(), "One Step Probability Deleterious");
  df.Write(GetProbNeut(), "One Step Probability Neutral");
  df.Write(GetProbPos(), "One Step Probability Positive");
  df.Write(GetAvPosSize(), "One Step Average Positive Size");
  df.Write(GetAvNegSize(), "One Step Average Negative Size");
  df.Write(m_peak_fitness, "One Step Peak Fitness");
  df.Write(GetAveFitness(), "One Step Average Fitness");
  df.Write(GetAveSqrFitness(), "One Step Average Square Fitness");
  df.Write(m_total_entropy, "One Step Total Entropy");
  df.Write(m_complexity, "One Step Total Complexity");
  df.Endl();
}

void cMutationalNeighborhood::PrintEntropy(cDataFile& df)
{
  double max_ent = log(static_cast<double>(m_inst_set.GetSize()));
  for (int j = 0; j < m_base_genome.GetSize(); j++) {
    df.Write(log(static_cast<double>(m_site_count[j] + 1)) / max_ent, " ");
  }
  df.Endl();
}

void cMutationalNeighborhood::PrintSiteCount(cDataFile& df)
{
  for (int j = 0; j < m_base_genome.GetSize(); j++) {
    df.Write(m_site_count[j], " ");
  }
  df.Endl();
}
