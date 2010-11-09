/*
 *  cMutationalNeighborhood.cc
 *  Avida
 *
 *  Created by David on 6/13/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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
#include "cEnvironment.h"
#include "cInstSet.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cWorld.h"
#include "tAnalyzeJob.h"

using namespace std;


cMutationalNeighborhood::cMutationalNeighborhood(cWorld* world, const cGenome& genome, int target)
  : m_world(world), m_initialized(false), m_inst_set(m_world->GetHardwareManager().GetInstSet(genome.GetInstSet()))
  , m_target(target), m_base_genome(genome)
{
  // Acquire write lock, to prevent any cMutationalNeighborhoodResults instances before computing
  m_rwlock.WriteLock();
}


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
      sStep& tpdata = m_twostep_point[cur_site];
      tpdata.peak_fitness = m_base_fitness;
      tpdata.peak_genome = m_base_genome;
      tpdata.site_count.Resize(m_base_genome.GetSize(), 0);

      sStep& tidata = m_twostep_insert[cur_site];
      tidata.peak_fitness = m_base_fitness;
      tidata.peak_genome = m_base_genome;
      tidata.site_count.Resize(m_base_genome.GetSize() + 2, 0);

      sStep& tddata = m_twostep_delete[cur_site];
      tddata.peak_fitness = m_base_fitness;
      tddata.peak_genome = m_base_genome;
      tddata.site_count.Resize(m_base_genome.GetSize(), 0);

      
      sStep& tipdata = m_insert_point[cur_site];
      tipdata.peak_fitness = m_base_fitness;
      tipdata.peak_genome = m_base_genome;
      tipdata.site_count.Resize(m_base_genome.GetSize() + 1, 0);
      
      sStep& tiddata = m_insert_delete[cur_site];
      tiddata.peak_fitness = m_base_fitness;
      tiddata.peak_genome = m_base_genome;
      tiddata.site_count.Resize(m_base_genome.GetSize() + 1, 0);
      
      sStep& tdpdata = m_delete_point[cur_site];
      tdpdata.peak_fitness = m_base_fitness;
      tdpdata.peak_genome = m_base_genome;
      tdpdata.site_count.Resize(m_base_genome.GetSize(), 0);
      
      
      // Do the processing, starting with One Step
      ProcessOneStepPoint(ctx, testcpu, test_info, cur_site);
      ProcessOneStepInsert(ctx, testcpu, test_info, cur_site);
      ProcessOneStepDelete(ctx, testcpu, test_info, cur_site);

      // Process the hanging insertion on the first cycle through (to balance execution time)
      if (cur_site == 0) {
        cur_site = m_base_genome.GetSize();
        
        sStep& oidata2 = m_onestep_insert[cur_site];
        oidata2.peak_fitness = m_base_fitness;
        oidata2.peak_genome = m_base_genome;
        oidata2.site_count.Resize(m_base_genome.GetSize() + 1, 0);
        
        sStep& tidata2 = m_twostep_insert[cur_site];
        tidata2.peak_fitness = m_base_fitness;
        tidata2.peak_genome = m_base_genome;
        tidata2.site_count.Resize(m_base_genome.GetSize() + 2, 0);
        
        sStep& tipdata2 = m_insert_point[cur_site];
        tipdata2.peak_fitness = m_base_fitness;
        tipdata2.peak_genome = m_base_genome;
        tipdata2.site_count.Resize(m_base_genome.GetSize() + 1, 0);
        
        sStep& tiddata2 = m_insert_delete[cur_site];
        tiddata2.peak_fitness = m_base_fitness;
        tiddata2.peak_genome = m_base_genome;
        tiddata2.site_count.Resize(m_base_genome.GetSize() + 1, 0);
        
        ProcessOneStepInsert(ctx, testcpu, test_info, cur_site); 
      }

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
  m_twostep_insert.ResizeClear(m_base_genome.GetSize() + 1);
  m_twostep_delete.ResizeClear(m_base_genome.GetSize());
  
  m_insert_point.ResizeClear(m_base_genome.GetSize() + 1);
  m_insert_delete.ResizeClear(m_base_genome.GetSize() + 1);
  m_delete_point.ResizeClear(m_base_genome.GetSize());
  
  m_fitness_point.ResizeClear(m_base_genome.GetSize(), m_inst_set.GetSize());
  m_fitness_insert.ResizeClear(m_base_genome.GetSize() + 1, m_inst_set.GetSize());
  m_fitness_delete.ResizeClear(m_base_genome.GetSize(), 1);
  
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
  cSequence& seq = mod_genome.GetSequence();
  
  // Loop through all the lines of genome, testing trying all combinations.
  int cur_inst = seq[cur_site].GetOp();
  
  // Fill in unmutated entry in fitness table with base fitness
  m_fitness_point[cur_site][cur_inst] = m_base_fitness;
  
  // Loop through all instructions...
  for (int inst_num = 0; inst_num < inst_size; inst_num++) {
    if (cur_inst == inst_num) continue;
    
    seq[cur_site].SetOp(inst_num);
    m_fitness_point[cur_site][inst_num] = ProcessOneStepGenome(ctx, testcpu, test_info, mod_genome, odata, cur_site);

    ProcessTwoStepPoint(ctx, testcpu, test_info, cur_site, mod_genome);
  }
}

void cMutationalNeighborhood::ProcessOneStepInsert(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site)
{
  const int inst_size = m_inst_set.GetSize();
  sStep& odata = m_onestep_insert[cur_site];
  
  cGenome mod_genome(m_base_genome);
  cSequence& seq = mod_genome.GetSequence();
  seq.Insert(cur_site, cInstruction(0));
  
  // Loop through all instructions...
  for (int inst_num = 0; inst_num < inst_size; inst_num++) {
    seq[cur_site].SetOp(inst_num);
    m_fitness_insert[cur_site][inst_num] = ProcessOneStepGenome(ctx, testcpu, test_info, mod_genome, odata, cur_site);
    
    ProcessTwoStepInsert(ctx, testcpu, test_info, cur_site, mod_genome);
    ProcessInsertPointCombo(ctx, testcpu, test_info, cur_site, mod_genome);
    ProcessInsertDeleteCombo(ctx, testcpu, test_info, cur_site, mod_genome);
  }  
}


void cMutationalNeighborhood::ProcessOneStepDelete(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site)
{
  sStep& odata = m_onestep_delete[cur_site];
  
  cGenome mod_genome(m_base_genome);
  cSequence& seq = mod_genome.GetSequence();
  seq.Remove(cur_site);

  m_fitness_delete[cur_site][0] = ProcessOneStepGenome(ctx, testcpu, test_info, mod_genome, odata, cur_site);
  ProcessTwoStepDelete(ctx, testcpu, test_info, cur_site, mod_genome);
  ProcessDeletePointCombo(ctx, testcpu, test_info, cur_site, mod_genome);
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
  cSequence& seq = mod_genome.GetSequence();
  sTwoStep& tdata = m_twostep_point[cur_site];
  sPendFit cur(m_fitness_point, cur_site, seq[cur_site].GetOp());

  // Loop through remaining lines of genome, testing trying all combinations.
  for (int line_num = cur_site + 1; line_num < m_base_genome.GetSize(); line_num++) {
    int cur_inst = seq[line_num].GetOp();
    
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) continue;
      
      seq[line_num].SetOp(inst_num);
      ProcessTwoStepGenome(ctx, testcpu, test_info, mod_genome, tdata, sPendFit(m_fitness_point, line_num, inst_num), cur);
    }
    
    seq[line_num].SetOp(cur_inst);
  }
}


void cMutationalNeighborhood::ProcessTwoStepInsert(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                   int cur_site, cGenome& mod_genome)
{
  const int inst_size = m_inst_set.GetSize();
  const int mod_size = mod_genome.GetSize();
  cSequence& seq = mod_genome.GetSequence();
  sTwoStep& tdata = m_twostep_insert[cur_site];
  sPendFit cur(m_fitness_insert, cur_site, seq[cur_site].GetOp());
  
  // Loop through all instructions...
  for (int line_num = cur_site + 1; line_num <= mod_size; line_num++) {
    seq.Insert(line_num, cInstruction(0));
    
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      seq[cur_site].SetOp(inst_num);
      ProcessTwoStepGenome(ctx, testcpu, test_info, mod_genome, tdata, sPendFit(m_fitness_insert, line_num - 1, inst_num), cur);
    }
    seq.Remove(line_num);
  }
}


void cMutationalNeighborhood::ProcessTwoStepDelete(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                   int cur_site, cGenome& mod_genome)
{
  const int mod_size = mod_genome.GetSize();
  cSequence& seq = mod_genome.GetSequence();
  sTwoStep& tdata = m_twostep_delete[cur_site];
  sPendFit cur(m_fitness_delete, cur_site, 0); // Delete 'inst' is always 0
  
  // Loop through all instructions...
  for (int line_num = cur_site; line_num < mod_size; line_num++) {
    int cur_inst = seq[line_num].GetOp();
    seq.Remove(line_num);
    ProcessTwoStepGenome(ctx, testcpu, test_info, mod_genome, tdata, sPendFit(m_fitness_delete, line_num + 1, 0), cur);
    seq.Insert(line_num, cInstruction(cur_inst));
  }
}


void cMutationalNeighborhood::ProcessInsertPointCombo(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                      int cur_site, cGenome& mod_genome)
{
  const int inst_size = m_inst_set.GetSize();
  cSequence& seq = mod_genome.GetSequence();
  sTwoStep& tdata = m_insert_point[cur_site];
  sPendFit cur(m_fitness_insert, cur_site, seq[cur_site].GetOp());
  
  // Loop through all lines of genome, testing trying all combinations.
  for (int line_num = 0; line_num < seq.GetSize(); line_num++) {
    if (line_num == cur_site) continue; // Skip the site of the insertion
    int actual = (line_num < cur_site) ? line_num : (line_num - 1); // if at or past insertion site, adjust pending target site
    
    int cur_inst = seq[line_num].GetOp();
    
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) continue;
      
      seq[line_num].SetOp(inst_num);
      ProcessTwoStepGenome(ctx, testcpu, test_info, mod_genome, tdata, sPendFit(m_fitness_point, actual, inst_num), cur);
    }
    
    seq[line_num].SetOp(cur_inst);
  }
}


void cMutationalNeighborhood::ProcessInsertDeleteCombo(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                       int cur_site, cGenome& mod_genome)
{
  cSequence& seq = mod_genome.GetSequence();
  sTwoStep& tdata = m_insert_delete[cur_site];
  sPendFit cur(m_fitness_insert, cur_site, seq[cur_site].GetOp());

  // Loop through all lines of genome, testing trying all combinations.
  for (int line_num = 0; line_num < seq.GetSize(); line_num++) {
    if (line_num == cur_site) continue; // Skip the site of the insertion
    int actual = (line_num < cur_site) ? line_num : (line_num - 1); // if at or past insertion site, adjust pending target site
    
    int cur_inst = seq[line_num].GetOp();
    seq.Remove(line_num);
    ProcessTwoStepGenome(ctx, testcpu, test_info, mod_genome, tdata, sPendFit(m_fitness_delete, actual, 0), cur);
    seq.Insert(line_num, cInstruction(cur_inst));
  }
}


void cMutationalNeighborhood::ProcessDeletePointCombo(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                      int cur_site, cGenome& mod_genome)
{
  const int inst_size = m_inst_set.GetSize();
  cSequence& seq = mod_genome.GetSequence();
  sTwoStep& tdata = m_delete_point[cur_site];
  sPendFit cur(m_fitness_delete, cur_site, 0); // Delete 'inst' is always 0
  
  // Loop through all lines of genome, testing trying all combinations.
  for (int line_num = 0; line_num < seq.GetSize(); line_num++) {
    int cur_inst = seq[line_num].GetOp();
    int actual = (line_num < cur_site) ? line_num : (line_num + 1); // if at or past deletion site, adjust pending target site
    
    // Loop through all instructions...
    for (int inst_num = 0; inst_num < inst_size; inst_num++) {
      if (cur_inst == inst_num) continue;
      
      seq[line_num].SetOp(inst_num);
      ProcessTwoStepGenome(ctx, testcpu, test_info, mod_genome, tdata, sPendFit(m_fitness_point, actual, inst_num), cur);
    }
    
    seq[line_num].SetOp(cur_inst);
  }
}


double cMutationalNeighborhood::ProcessTwoStepGenome(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info,
                                                     const cGenome& mod_genome, sTwoStep& tdata,
                                                     const sPendFit& cur, const sPendFit& oth)
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
  
  if (test_fitness >= m_neut_min) tdata.site_count[cur.site]++;
  
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
      tdata.pending.Push(new sPendFit(oth));
      tdata.pending.Push(new sPendFit(cur));
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
  
  m_ot.task_target = m_op.task_target + m_oi.task_target + m_od.task_target;
  m_ot.task_total = m_op.task_total + m_oi.task_total + m_od.task_total;
  m_ot.task_knockout = m_op.task_knockout + m_oi.task_knockout + m_od.task_knockout;

  m_ot.task_size_target = m_op.task_size_target + m_oi.task_size_target + m_od.task_size_target;
  m_ot.task_size_total = m_op.task_size_total + m_oi.task_size_total + m_od.task_size_total;
  m_ot.task_size_knockout = m_op.task_size_knockout + m_oi.task_size_knockout + m_od.task_size_knockout;


  
  
  m_tp.peak_fitness = m_base_fitness;
  m_tp.peak_genome = m_base_genome;
  m_tp.site_count.Resize(m_base_genome.GetSize(), 0);
  AggregateTwoStep(m_twostep_point, m_tp);
  
  m_ti.peak_fitness = m_base_fitness;
  m_ti.peak_genome = m_base_genome;
  m_ti.site_count.Resize(m_base_genome.GetSize() + 2, 0);
  AggregateTwoStep(m_twostep_insert, m_ti);
  
  m_td.peak_fitness = m_base_fitness;
  m_td.peak_genome = m_base_genome;
  m_td.site_count.Resize(m_base_genome.GetSize(), 0);
  AggregateTwoStep(m_twostep_delete, m_td);


  m_tip.peak_fitness = m_base_fitness;
  m_tip.peak_genome = m_base_genome;
  m_tip.site_count.Resize(m_base_genome.GetSize() + 1, 0);
  AggregateTwoStep(m_insert_point, m_tip);
  
  m_tid.peak_fitness = m_base_fitness;
  m_tid.peak_genome = m_base_genome;
  m_tid.site_count.Resize(m_base_genome.GetSize() + 1, 0);
  AggregateTwoStep(m_insert_delete, m_tid);
  
  m_tdp.peak_fitness = m_base_fitness;
  m_tdp.peak_genome = m_base_genome;
  m_tdp.site_count.Resize(m_base_genome.GetSize(), 0);
  AggregateTwoStep(m_delete_point, m_tdp);
  
  
  // Collect totals across all two step mutants
  m_tt.total = m_tp.total + m_ti.total + m_td.total + m_tip.total + m_tid.total + m_tdp.total;
  m_tt.total_fitness = m_tp.total_fitness + m_ti.total_fitness + m_td.total_fitness
                       + m_tip.total_fitness + m_tid.total_fitness + m_tdp.total_fitness;
  m_tt.total_sqr_fitness = m_tp.total_sqr_fitness + m_ti.total_sqr_fitness + m_td.total_sqr_fitness
                           + m_tip.total_sqr_fitness + m_tid.total_sqr_fitness + m_tdp.total_sqr_fitness;
  
  const double pftp = m_tp.peak_fitness;
  const double pfti = m_ti.peak_fitness;
  const double pftd = m_td.peak_fitness;
  const double pftip = m_tip.peak_fitness;
  const double pftid = m_tid.peak_fitness;
  const double pftdp = m_tdp.peak_fitness;
  
  if (pftp >= pfti && pftp >= pftd && pftp >= pftip && pftp >= pftid && pftp >= pftdp) {
    m_tt.peak_fitness = m_tp.peak_fitness;
    m_tt.peak_genome = m_tp.peak_genome;
  } else if (pfti >= pftd && pfti >= pftip && pfti >= pftid && pfti >= pftdp) {
    m_tt.peak_fitness = m_ti.peak_fitness;
    m_tt.peak_genome = m_ti.peak_genome;
  } else if (pftd >= pftip && pftd >= pftid && pftd >= pftdp) {
    m_tt.peak_fitness = m_td.peak_fitness;
    m_tt.peak_genome = m_td.peak_genome;
  } else if (pftip >= pftid && pftip >= pftdp) {
    m_tt.peak_fitness = m_tip.peak_fitness;
    m_tt.peak_genome = m_tip.peak_genome;
  } else if (pftid >= pftdp) {
    m_tt.peak_fitness = m_tid.peak_fitness;
    m_tt.peak_genome = m_tid.peak_genome;
  } else {
    m_tt.peak_fitness = m_tdp.peak_fitness;
    m_tt.peak_genome = m_tdp.peak_genome;
  }
  
  m_tt.pos = m_tp.pos + m_ti.pos + m_td.pos + m_tip.pos + m_tid.pos + m_tdp.pos;
  m_tt.neg = m_tp.neg + m_ti.neg + m_td.neg + m_tip.neg + m_tid.neg + m_tdp.neg;
  m_tt.neut = m_tp.neut + m_ti.neut + m_td.neut + m_tip.neut + m_tid.neut + m_tdp.neut;
  m_tt.dead = m_tp.dead + m_ti.dead + m_td.dead + m_tip.dead + m_tid.dead + m_tdp.dead;
  m_tt.size_pos = m_tp.size_pos + m_ti.size_pos + m_td.size_pos + m_tip.size_pos + m_tid.size_pos + m_tdp.size_pos;
  m_tt.size_neg = m_tp.size_neg + m_ti.size_neg + m_td.size_neg + m_tip.size_neg + m_tid.size_neg + m_tdp.size_neg;
  
  m_tt.task_target = m_tp.task_target + m_ti.task_target + m_td.task_target
                     + m_tip.task_target + m_tid.task_target + m_tdp.task_target;
  m_tt.task_target_pos = m_tp.task_target_pos + m_ti.task_target_pos + m_td.task_target_pos
                         + m_tip.task_target_pos + m_tid.task_target_pos + m_tdp.task_target_pos;
  m_tt.task_target_neg = m_tp.task_target_neg + m_ti.task_target_neg + m_td.task_target_neg
                         + m_tip.task_target_neg + m_tid.task_target_neg + m_tdp.task_target_neg;
  m_tt.task_target_neut = m_tp.task_target_neut + m_ti.task_target_neut + m_td.task_target_neut
                          + m_tip.task_target_neut + m_tid.task_target_neut + m_tdp.task_target_neut;
  m_tt.task_target_dead = m_tp.task_target_dead + m_ti.task_target_dead + m_td.task_target_dead
                          + m_tip.task_target_dead + m_tid.task_target_dead + m_tdp.task_target_dead;
  m_tt.task_total = m_tp.task_total + m_ti.task_total + m_td.task_total
                    + m_tip.task_total + m_tid.task_total + m_tdp.task_total;
  m_tt.task_knockout = m_tp.task_knockout + m_ti.task_knockout + m_td.task_knockout
                       + m_tip.task_knockout + m_tid.task_knockout + m_tdp.task_knockout;
  
  m_tt.task_size_target = m_tp.task_size_target + m_ti.task_size_target + m_td.task_size_target
                          + m_tip.task_size_target + m_tid.task_size_target + m_tdp.task_size_target;
  m_tt.task_size_target_pos = m_tp.task_size_target_pos + m_ti.task_size_target_pos + m_td.task_size_target_pos
                              + m_tip.task_size_target_pos + m_tid.task_size_target_pos + m_tdp.task_size_target_pos;
  m_tt.task_size_target_neg = m_tp.task_size_target_neg + m_ti.task_size_target_neg + m_td.task_size_target_neg
                              + m_tip.task_size_target_neg + m_tid.task_size_target_neg + m_tdp.task_size_target_neg;
  m_tt.task_size_total = m_tp.task_size_total + m_ti.task_size_total + m_td.task_size_total
                         + m_tip.task_size_total + m_tid.task_size_total + m_tdp.task_size_total;
  m_tt.task_size_knockout = m_tp.task_size_knockout + m_ti.task_size_knockout + m_td.task_size_knockout
                            + m_tip.task_size_knockout + m_tid.task_size_knockout + m_tdp.task_size_knockout;
  
  // Unlock data for reading
  m_rwlock.WriteUnlock();
  
  // Cleanup state information
  m_onestep_point.Resize(0);
  m_onestep_insert.Resize(0);
  m_onestep_delete.Resize(0);
  
  m_twostep_point.Resize(0);
  m_twostep_insert.Resize(0);
  m_twostep_delete.Resize(0);
  
  m_insert_point.Resize(0);
  m_insert_delete.Resize(0);
  m_delete_point.Resize(0);
  
  m_fitness_point.Resize(0, 0);
  m_fitness_insert.Resize(0, 0);
  m_fitness_delete.Resize(0, 0);
}

void cMutationalNeighborhood::AggregateOneStep(tArray<sStep>& steps, sOneStepAggregate& osa)
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


void cMutationalNeighborhood::AggregateTwoStep(tArray<sTwoStep>& steps, sTwoStepAggregate& tsa)
{
  sPendFit* pend = NULL;

  for (int i = 0; i < steps.GetSize(); i++) {
    sTwoStep& tdata = steps[i];
    tsa.total += tdata.total;
    tsa.total_fitness += tdata.total_fitness;
    tsa.total_sqr_fitness += tdata.total_sqr_fitness;
    tsa.pos += tdata.pos;
    tsa.neg += tdata.neg;
    tsa.neut += tdata.neut;
    tsa.dead += tdata.dead;
    tsa.size_pos += tdata.size_pos; 
    tsa.size_neg += tdata.size_neg; 
    
    if (tdata.peak_fitness > tsa.peak_fitness) {
      tsa.peak_genome = tdata.peak_genome;
      tsa.peak_fitness = tdata.peak_fitness;
    }
    
    
    for (int j = 0; j < tsa.site_count.GetSize(); j++) {
      tsa.site_count[j] += tdata.site_count[j];
    }
    
    tsa.task_target += tdata.task_target;
    tsa.task_total += tdata.task_total;
    tsa.task_knockout += tdata.task_knockout;
    
    tsa.task_size_target += tdata.task_size_target;
    tsa.task_size_total += tdata.task_size_total;
    tsa.task_size_knockout += tdata.task_size_knockout;
    
    while ((pend = tdata.pending.Pop())) {
      double fitness = pend->GetFitness();
      
      if (fitness == 0.0) {
        tsa.task_target_dead++;
      } else if (fitness < m_neut_min) {
        tsa.task_target_neg++;
        tsa.task_size_target_neg += fitness;
      } else if (fitness <= m_neut_max) {
        tsa.task_target_neut++;
      } else {
        tsa.task_target_pos++;
        tsa.task_size_target_pos += fitness;
      }
      
      delete pend;
    }
  }
  
  const double max_ent = log(static_cast<double>(m_inst_set.GetSize()));
  for (int i = 0; i < m_base_genome.GetSize(); i++) {
    // Per-site entropy is the log of the number of legal states for that
    // site.  Add one to account for the unmutated state.
    tsa.total_entropy += log(static_cast<double>(tsa.site_count[i] + 1)) / max_ent;
  }
  tsa.complexity = m_base_genome.GetSize() - tsa.total_entropy;
  
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
  
  df.Write(Get2SAggregateTotal(), "Total 2-Step Mutants");
  df.Write(Get2SAggregateProbBeneficial(), "2-Step Probability Beneficial");
  df.Write(Get2SAggregateProbDeleterious(), "2-Step Probability Deleterious");
  df.Write(Get2SAggregateProbNeutral(), "2-Step Probability Neutral");
  df.Write(Get2SAggregateProbLethal(), "2-Step Probability Lethal");
  df.Write(Get2SAggregateAverageSizeBeneficial(), "2-Step Average Beneficial Size");
  df.Write(Get2SAggregateAverageSizeDeleterious(), "2-Step Average Deleterious Size");
  df.Write(Get2SAggregatePeakFitness(), "2-Step Peak Fitness");
  df.Write(Get2SAggregateAverageFitness(), "2-Step Average Fitness");
  df.Write(Get2SAggregateAverageSqrFitness(), "2-Step Average Square Fitness");
//  df.Write(Get2SAggregateTotalEntropy(), "2-Step Total Entropy");
//  df.Write(Get2SAggregateComplexity(), "2-Step Total Complexity");
  df.Write(Get2SAggregateTargetTask(), "2-Step Confers Target Task");
  df.Write(Get2SAggregateProbTargetTask(), "2-Step Probability Confers Target Task");
  df.Write(Get2SAggregateAverageSizeTargetTask(), "2-Step Average Size of Target Task Conferral");
  df.Write(Get2SAggregateTargetTaskBeneficial(), "2-Step Confers Target - Previous Beneficial");
  df.Write(Get2SAggregateProbTargetTaskBeneficial(), "2-Step Prob. Confers Target - Previous Beneficial");
  df.Write(Get2SAggregateAverageSizeTargetTaskBeneficial(), "2-Step Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(Get2SAggregateTargetTaskDeleterious(), "2-Step Confers Target - Previous Deleterious");
  df.Write(Get2SAggregateProbTargetTaskDeleterious(), "2-Step Prob. Confers Target - Previous Deleterious");
  df.Write(Get2SAggregateAverageSizeTargetTaskDeleterious(), "2-Step Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(Get2SAggregateTargetTaskNeutral(), "2-Step Confers Target - Previous Neutral");
  df.Write(Get2SAggregateProbTargetTaskNeutral(), "2-Step Prob. Confers Target - Previous Neutral");
  df.Write(Get2SAggregateTargetTaskLethal(), "2-Step Confers Target - Previous Lethal");
  df.Write(Get2SAggregateProbTargetTaskLethal(), "2-Step Prob. Confers Target - Previous Lethal");
  df.Write(Get2SAggregateTask(), "2-Step Confers Any Task");
  df.Write(Get2SAggregateProbTask(), "2-Step Probability Confers Any Task");
  df.Write(Get2SAggregateAverageSizeTask(), "2-Step Average Size of Any Task Conferral");
  df.Write(Get2SAggregateKnockout(), "2-Step Knockout Task");
  df.Write(Get2SAggregateProbKnockout(), "2-Step Probability Knockout Task");
  df.Write(Get2SAggregateAverageSizeKnockout(), "2-Step Average Size of Task Knockout");
  
  df.Write(Get2SPointTotal(), "Total 2-Step Point Mutants");
  df.Write(Get2SPointProbBeneficial(), "2-Step Point Probability Beneficial");
  df.Write(Get2SPointProbDeleterious(), "2-Step Point Probability Deleterious");
  df.Write(Get2SPointProbNeutral(), "2-Step Point Probability Neutral");
  df.Write(Get2SPointProbLethal(), "2-Step Point Probability Lethal");
  df.Write(Get2SPointAverageSizeBeneficial(), "2-Step Point Average Beneficial Size");
  df.Write(Get2SPointAverageSizeDeleterious(), "2-Step Point Average Deleterious Size");
  df.Write(Get2SPointPeakFitness(), "2-Step Point Peak Fitness");
  df.Write(Get2SPointAverageFitness(), "2-Step Point Average Fitness");
  df.Write(Get2SPointAverageSqrFitness(), "2-Step Point Average Square Fitness");
  df.Write(Get2SPointTotalEntropy(), "2-Step Point Total Entropy");
  df.Write(Get2SPointComplexity(), "2-Step Point Total Complexity");
  df.Write(Get2SPointTargetTask(), "2-Step Point Confers Target Task");
  df.Write(Get2SPointProbTargetTask(), "2-Step Point Probability Confers Target Task");
  df.Write(Get2SPointAverageSizeTargetTask(), "2-Step Point Average Size of Target Task Conferral");
  df.Write(Get2SPointTargetTaskBeneficial(), "2-Step Point Confers Target - Previous Beneficial");
  df.Write(Get2SPointProbTargetTaskBeneficial(), "2-Step Point Prob. Confers Target - Previous Beneficial");
  df.Write(Get2SPointAverageSizeTargetTaskBeneficial(), "2-Step Point Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(Get2SPointTargetTaskDeleterious(), "2-Step Point Confers Target - Previous Deleterious");
  df.Write(Get2SPointProbTargetTaskDeleterious(), "2-Step Point Prob. Confers Target - Previous Deleterious");
  df.Write(Get2SPointAverageSizeTargetTaskDeleterious(), "2-Step Point Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(Get2SPointTargetTaskNeutral(), "2-Step Point Confers Target - Previous Neutral");
  df.Write(Get2SPointProbTargetTaskNeutral(), "2-Step Point Prob. Confers Target - Previous Neutral");
  df.Write(Get2SPointTargetTaskLethal(), "2-Step Point Confers Target - Previous Lethal");
  df.Write(Get2SPointProbTargetTaskLethal(), "2-Step Point Prob. Confers Target - Previous Lethal");
  df.Write(Get2SPointTask(), "2-Step Point Confers Any Task");
  df.Write(Get2SPointProbTask(), "2-Step Point Probability Confers Any Task");
  df.Write(Get2SPointAverageSizeTask(), "2-Step Point Average Size of Any Task Conferral");
  df.Write(Get2SPointKnockout(), "2-Step Point Knockout Task");
  df.Write(Get2SPointProbKnockout(), "2-Step Point Probability Knockout Task");
  df.Write(Get2SPointAverageSizeKnockout(), "2-Step Point Average Size of Task Knockout");
  
  df.Write(Get2SInsertTotal(), "Total 2-Step Insert Mutants");
  df.Write(Get2SInsertProbBeneficial(), "2-Step Insert Probability Beneficial");
  df.Write(Get2SInsertProbDeleterious(), "2-Step Insert Probability Deleterious");
  df.Write(Get2SInsertProbNeutral(), "2-Step Insert Probability Neutral");
  df.Write(Get2SInsertProbLethal(), "2-Step Insert Probability Lethal");
  df.Write(Get2SInsertAverageSizeBeneficial(), "2-Step Insert Average Beneficial Size");
  df.Write(Get2SInsertAverageSizeDeleterious(), "2-Step Insert Average Deleterious Size");
  df.Write(Get2SInsertPeakFitness(), "2-Step Insert Peak Fitness");
  df.Write(Get2SInsertAverageFitness(), "2-Step Insert Average Fitness");
  df.Write(Get2SInsertAverageSqrFitness(), "2-Step Insert Average Square Fitness");
  df.Write(Get2SInsertTotalEntropy(), "2-Step Insert Total Entropy");
  df.Write(Get2SInsertComplexity(), "2-Step Insert Total Complexity");
  df.Write(Get2SInsertTargetTask(), "2-Step Insert Confers Target Task");
  df.Write(Get2SInsertProbTargetTask(), "2-Step Insert Probability Confers Target Task");
  df.Write(Get2SInsertAverageSizeTargetTask(), "2-Step Insert Average Size of Target Task Conferral");
  df.Write(Get2SInsertTargetTaskBeneficial(), "2-Step Insert Confers Target - Previous Beneficial");
  df.Write(Get2SInsertProbTargetTaskBeneficial(), "2-Step Insert Prob. Confers Target - Previous Beneficial");
  df.Write(Get2SInsertAverageSizeTargetTaskBeneficial(), "2-Step Insert Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(Get2SInsertTargetTaskDeleterious(), "2-Step Insert Confers Target - Previous Deleterious");
  df.Write(Get2SInsertProbTargetTaskDeleterious(), "2-Step Insert Prob. Confers Target - Previous Deleterious");
  df.Write(Get2SInsertAverageSizeTargetTaskDeleterious(), "2-Step Insert Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(Get2SInsertTargetTaskNeutral(), "2-Step Insert Confers Target - Previous Neutral");
  df.Write(Get2SInsertProbTargetTaskNeutral(), "2-Step Insert Prob. Confers Target - Previous Neutral");
  df.Write(Get2SInsertTargetTaskLethal(), "2-Step Insert Confers Target - Previous Lethal");
  df.Write(Get2SInsertProbTargetTaskLethal(), "2-Step Insert Prob. Confers Target - Previous Lethal");
  df.Write(Get2SInsertTask(), "2-Step Insert Confers Any Task");
  df.Write(Get2SInsertProbTask(), "2-Step Insert Probability Confers Any Task");
  df.Write(Get2SInsertAverageSizeTask(), "2-Step Insert Average Size of Any Task Conferral");
  df.Write(Get2SInsertKnockout(), "2-Step Insert Knockout Task");
  df.Write(Get2SInsertProbKnockout(), "2-Step Insert Probability Knockout Task");
  df.Write(Get2SInsertAverageSizeKnockout(), "2-Step Insert Average Size of Task Knockout");
  
  df.Write(Get2SDeleteTotal(), "Total 2-Step Delete Mutants");
  df.Write(Get2SDeleteProbBeneficial(), "2-Step Delete Probability Beneficial");
  df.Write(Get2SDeleteProbDeleterious(), "2-Step Delete Probability Deleterious");
  df.Write(Get2SDeleteProbNeutral(), "2-Step Delete Probability Neutral");
  df.Write(Get2SDeleteProbLethal(), "2-Step Delete Probability Lethal");
  df.Write(Get2SDeleteAverageSizeBeneficial(), "2-Step Delete Average Beneficial Size");
  df.Write(Get2SDeleteAverageSizeDeleterious(), "2-Step Delete Average Deleterious Size");
  df.Write(Get2SDeletePeakFitness(), "2-Step Delete Peak Fitness");
  df.Write(Get2SDeleteAverageFitness(), "2-Step Delete Average Fitness");
  df.Write(Get2SDeleteAverageSqrFitness(), "2-Step Delete Average Square Fitness");
  df.Write(Get2SDeleteTotalEntropy(), "2-Step Delete Total Entropy");
  df.Write(Get2SDeleteComplexity(), "2-Step Delete Total Complexity");
  df.Write(Get2SDeleteTargetTask(), "2-Step Delete Confers Target Task");
  df.Write(Get2SDeleteProbTargetTask(), "2-Step Delete Probability Confers Target Task");
  df.Write(Get2SDeleteAverageSizeTargetTask(), "2-Step Delete Average Size of Target Task Conferral");
  df.Write(Get2SDeleteTargetTaskBeneficial(), "2-Step Delete Confers Target - Previous Beneficial");
  df.Write(Get2SDeleteProbTargetTaskBeneficial(), "2-Step Delete Prob. Confers Target - Previous Beneficial");
  df.Write(Get2SDeleteAverageSizeTargetTaskBeneficial(), "2-Step Delete Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(Get2SDeleteTargetTaskDeleterious(), "2-Step Delete Confers Target - Previous Deleterious");
  df.Write(Get2SDeleteProbTargetTaskDeleterious(), "2-Step Delete Prob. Confers Target - Previous Deleterious");
  df.Write(Get2SDeleteAverageSizeTargetTaskDeleterious(), "2-Step Delete Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(Get2SDeleteTargetTaskNeutral(), "2-Step Delete Confers Target - Previous Neutral");
  df.Write(Get2SDeleteProbTargetTaskNeutral(), "2-Step Delete Prob. Confers Target - Previous Neutral");
  df.Write(Get2SDeleteTargetTaskLethal(), "2-Step Delete Confers Target - Previous Lethal");
  df.Write(Get2SDeleteProbTargetTaskLethal(), "2-Step Delete Prob. Confers Target - Previous Lethal");
  df.Write(Get2SDeleteTask(), "2-Step Delete Confers Any Task");
  df.Write(Get2SDeleteProbTask(), "2-Step Delete Probability Confers Any Task");
  df.Write(Get2SDeleteAverageSizeTask(), "2-Step Delete Average Size of Any Task Conferral");
  df.Write(Get2SDeleteKnockout(), "2-Step Delete Knockout Task");
  df.Write(Get2SDeleteProbKnockout(), "2-Step Delete Probability Knockout Task");
  df.Write(Get2SDeleteAverageSizeKnockout(), "2-Step Delete Average Size of Task Knockout");
  
  df.Write(GetInsPntTotal(), "Total Insert/Point Mutants");
  df.Write(GetInsPntProbBeneficial(), "Insert/Point Probability Beneficial");
  df.Write(GetInsPntProbDeleterious(), "Insert/Point Probability Deleterious");
  df.Write(GetInsPntProbNeutral(), "Insert/Point Probability Neutral");
  df.Write(GetInsPntProbLethal(), "Insert/Point Probability Lethal");
  df.Write(GetInsPntAverageSizeBeneficial(), "Insert/Point Average Beneficial Size");
  df.Write(GetInsPntAverageSizeDeleterious(), "Insert/Point Average Deleterious Size");
  df.Write(GetInsPntPeakFitness(), "Insert/Point Peak Fitness");
  df.Write(GetInsPntAverageFitness(), "Insert/Point Average Fitness");
  df.Write(GetInsPntAverageSqrFitness(), "Insert/Point Average Square Fitness");
  df.Write(GetInsPntTotalEntropy(), "Insert/Point Total Entropy");
  df.Write(GetInsPntComplexity(), "Insert/Point Total Complexity");
  df.Write(GetInsPntTargetTask(), "Insert/Point Confers Target Task");
  df.Write(GetInsPntProbTargetTask(), "Insert/Point Probability Confers Target Task");
  df.Write(GetInsPntAverageSizeTargetTask(), "Insert/Point Average Size of Target Task Conferral");
  df.Write(GetInsPntTargetTaskBeneficial(), "Insert/Point Confers Target - Previous Beneficial");
  df.Write(GetInsPntProbTargetTaskBeneficial(), "Insert/Point Prob. Confers Target - Previous Beneficial");
  df.Write(GetInsPntAverageSizeTargetTaskBeneficial(), "Insert/Point Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(GetInsPntTargetTaskDeleterious(), "Insert/Point Confers Target - Previous Deleterious");
  df.Write(GetInsPntProbTargetTaskDeleterious(), "Insert/Point Prob. Confers Target - Previous Deleterious");
  df.Write(GetInsPntAverageSizeTargetTaskDeleterious(), "Insert/Point Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(GetInsPntTargetTaskNeutral(), "Insert/Point Confers Target - Previous Neutral");
  df.Write(GetInsPntProbTargetTaskNeutral(), "Insert/Point Prob. Confers Target - Previous Neutral");
  df.Write(GetInsPntTargetTaskLethal(), "Insert/Point Confers Target - Previous Lethal");
  df.Write(GetInsPntProbTargetTaskLethal(), "Insert/Point Prob. Confers Target - Previous Lethal");
  df.Write(GetInsPntTask(), "Insert/Point Confers Any Task");
  df.Write(GetInsPntProbTask(), "Insert/Point Probability Confers Any Task");
  df.Write(GetInsPntAverageSizeTask(), "Insert/Point Average Size of Any Task Conferral");
  df.Write(GetInsPntKnockout(), "Insert/Point Knockout Task");
  df.Write(GetInsPntProbKnockout(), "Insert/Point Probability Knockout Task");
  df.Write(GetInsPntAverageSizeKnockout(), "Insert/Point Average Size of Task Knockout");
  
  df.Write(GetInsDelTotal(), "Total Insert/Delete Mutants");
  df.Write(GetInsDelProbBeneficial(), "Insert/Delete Probability Beneficial");
  df.Write(GetInsDelProbDeleterious(), "Insert/Delete Probability Deleterious");
  df.Write(GetInsDelProbNeutral(), "Insert/Delete Probability Neutral");
  df.Write(GetInsDelProbLethal(), "Insert/Delete Probability Lethal");
  df.Write(GetInsDelAverageSizeBeneficial(), "Insert/Delete Average Beneficial Size");
  df.Write(GetInsDelAverageSizeDeleterious(), "Insert/Delete Average Deleterious Size");
  df.Write(GetInsDelPeakFitness(), "Insert/Delete Peak Fitness");
  df.Write(GetInsDelAverageFitness(), "Insert/Delete Average Fitness");
  df.Write(GetInsDelAverageSqrFitness(), "Insert/Delete Average Square Fitness");
  df.Write(GetInsDelTotalEntropy(), "Insert/Delete Total Entropy");
  df.Write(GetInsDelComplexity(), "Insert/Delete Total Complexity");
  df.Write(GetInsDelTargetTask(), "Insert/Delete Confers Target Task");
  df.Write(GetInsDelProbTargetTask(), "Insert/Delete Probability Confers Target Task");
  df.Write(GetInsDelAverageSizeTargetTask(), "Insert/Delete Average Size of Target Task Conferral");
  df.Write(GetInsDelTargetTaskBeneficial(), "Insert/Delete Confers Target - Previous Beneficial");
  df.Write(GetInsDelProbTargetTaskBeneficial(), "Insert/Delete Prob. Confers Target - Previous Beneficial");
  df.Write(GetInsDelAverageSizeTargetTaskBeneficial(), "Insert/Delete Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(GetInsDelTargetTaskDeleterious(), "Insert/Delete Confers Target - Previous Deleterious");
  df.Write(GetInsDelProbTargetTaskDeleterious(), "Insert/Delete Prob. Confers Target - Previous Deleterious");
  df.Write(GetInsDelAverageSizeTargetTaskDeleterious(), "Insert/Delete Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(GetInsDelTargetTaskNeutral(), "Insert/Delete Confers Target - Previous Neutral");
  df.Write(GetInsDelProbTargetTaskNeutral(), "Insert/Delete Prob. Confers Target - Previous Neutral");
  df.Write(GetInsDelTargetTaskLethal(), "Insert/Delete Confers Target - Previous Lethal");
  df.Write(GetInsDelProbTargetTaskLethal(), "Insert/Delete Prob. Confers Target - Previous Lethal");
  df.Write(GetInsDelTask(), "Insert/Delete Confers Any Task");
  df.Write(GetInsDelProbTask(), "Insert/Delete Probability Confers Any Task");
  df.Write(GetInsDelAverageSizeTask(), "Insert/Delete Average Size of Any Task Conferral");
  df.Write(GetInsDelKnockout(), "Insert/Delete Knockout Task");
  df.Write(GetInsDelProbKnockout(), "Insert/Delete Probability Knockout Task");
  df.Write(GetInsDelAverageSizeKnockout(), "Insert/Delete Average Size of Task Knockout");
  
  df.Write(GetDelPntTotal(), "Total Delete/Point Mutants");
  df.Write(GetDelPntProbBeneficial(), "Delete/Point Probability Beneficial");
  df.Write(GetDelPntProbDeleterious(), "Delete/Point Probability Deleterious");
  df.Write(GetDelPntProbNeutral(), "Delete/Point Probability Neutral");
  df.Write(GetDelPntProbLethal(), "Delete/Point Probability Lethal");
  df.Write(GetDelPntAverageSizeBeneficial(), "Delete/Point Average Beneficial Size");
  df.Write(GetDelPntAverageSizeDeleterious(), "Delete/Point Average Deleterious Size");
  df.Write(GetDelPntPeakFitness(), "Delete/Point Peak Fitness");
  df.Write(GetDelPntAverageFitness(), "Delete/Point Average Fitness");
  df.Write(GetDelPntAverageSqrFitness(), "Delete/Point Average Square Fitness");
  df.Write(GetDelPntTotalEntropy(), "Delete/Point Total Entropy");
  df.Write(GetDelPntComplexity(), "Delete/Point Total Complexity");
  df.Write(GetDelPntTargetTask(), "Delete/Point Confers Target Task");
  df.Write(GetDelPntProbTargetTask(), "Delete/Point Probability Confers Target Task");
  df.Write(GetDelPntAverageSizeTargetTask(), "Delete/Point Average Size of Target Task Conferral");
  df.Write(GetDelPntTargetTaskBeneficial(), "Delete/Point Confers Target - Previous Beneficial");
  df.Write(GetDelPntProbTargetTaskBeneficial(), "Delete/Point Prob. Confers Target - Previous Beneficial");
  df.Write(GetDelPntAverageSizeTargetTaskBeneficial(), "Delete/Point Ave. Size of Previous Beneficial in Target Conferral");
  df.Write(GetDelPntTargetTaskDeleterious(), "Delete/Point Confers Target - Previous Deleterious");
  df.Write(GetDelPntProbTargetTaskDeleterious(), "Delete/Point Prob. Confers Target - Previous Deleterious");
  df.Write(GetDelPntAverageSizeTargetTaskDeleterious(), "Delete/Point Ave. Size of Previous Deleterious in Target Conferral");
  df.Write(GetDelPntTargetTaskNeutral(), "Delete/Point Confers Target - Previous Neutral");
  df.Write(GetDelPntProbTargetTaskNeutral(), "Delete/Point Prob. Confers Target - Previous Neutral");
  df.Write(GetDelPntTargetTaskLethal(), "Delete/Point Confers Target - Previous Lethal");
  df.Write(GetDelPntProbTargetTaskLethal(), "Delete/Point Prob. Confers Target - Previous Lethal");
  df.Write(GetDelPntTask(), "Delete/Point Confers Any Task");
  df.Write(GetDelPntProbTask(), "Delete/Point Probability Confers Any Task");
  df.Write(GetDelPntAverageSizeTask(), "Delete/Point Average Size of Any Task Conferral");
  df.Write(GetDelPntKnockout(), "Delete/Point Knockout Task");
  df.Write(GetDelPntProbKnockout(), "Delete/Point Probability Knockout Task");
  df.Write(GetDelPntAverageSizeKnockout(), "Delete/Point Average Size of Task Knockout");
  
  df.Endl();
}
