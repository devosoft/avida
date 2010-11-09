/*
 *  cModularityAnalysis.cc
 *  Avida
 *
 *  Created by David on 1/11/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#include "cModularityAnalysis.h"

#include "cAnalyzeGenotype.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "tDataCommandManager.h"
#include "tDataEntry.h"


void cModularityAnalysis::Initialize()
{
  tDataCommandManager<cAnalyzeGenotype>& dcm = cAnalyzeGenotype::GetDataCommandManager();
  // A basic macro to link a keyword to a description and Get and Set methods in cAnalyzeGenotype.
#define ADD_GDATA(KEYWORD, DESC, GET, COMP) \
  dcm.Add(KEYWORD, new tDataEntryProxy<cAnalyzeGenotype, cFlexVar ()> \
    (KEYWORD, DESC, &cModularityAnalysis::GET, COMP));
#define ADD_GDATA_IDX(KEYWORD, DESC, GET, COMP) \
  dcm.Add(KEYWORD, new tDataEntryProxy<cAnalyzeGenotype, cFlexVar (int)> \
    (KEYWORD, &cModularityAnalysis::DESC, &cModularityAnalysis::GET, COMP));

  ADD_GDATA("tasks_done", "Number of Tasks Performed", GetTasksDoneFor, 2);
  ADD_GDATA("insts_tasks", "Number of Instructions Involved in Tasks", GetInstsInvolvedInTasksFor, 1);
  ADD_GDATA("tasks_prop", "Proportion of Sites in Tasks", GetTaskProportionFor, 4);
  ADD_GDATA("ave_tasks_per_site", "Average Number of Tasks Per Site", GetAveTasksPerSiteFor, 6);
  ADD_GDATA("ave_sites_per_task", "Average Number of Sites Per Task", GetAveSitesPerTaskFor, 6);
  ADD_GDATA("ave_prop_nonoverlap", "Average Proportion of the Non-overlapping Region of a Task", GetTaskProportionFor, 4);
  ADD_GDATA_IDX("sites_per_task", DescSitesPerTask, GetSitesPerTaskFor, 1);
  ADD_GDATA_IDX("sites_inv_x_tasks", DescSitesInvolvedInXTasks, GetSitesInvolvedInXTasksFor, 1);
  ADD_GDATA_IDX("task_length", DescTaskLength, GetTaskLengthFor, 4);
  ADD_GDATA_IDX("ave_task_position", DescAveTaskPosition, GetAveTaskPositionFor, 4);  
}

void cModularityAnalysis::CalcFunctionalModularity(cAvidaContext& ctx)
{
  cTestCPU* testcpu = m_genotype->GetWorld()->GetHardwareManager().CreateTestCPU();
  cCPUTestInfo test_info = m_test_info;
  
  const cGenome& base_genome = m_genotype->GetGenome();
  const cSequence& base_seq = base_genome.GetSequence();

  // Calculate the stats for the genotype we're working with...
  testcpu->TestGenome(ctx, test_info, base_genome);  
  double base_fitness = test_info.GetColonyFitness();
    
  // Check if the organism does any tasks
  bool does_tasks = false;
  const tArray<int> base_tasks = test_info.GetColonyOrganism()->GetPhenotype().GetLastTaskCount();
  const int num_tasks = base_tasks.GetSize();
  for (int i = 0; i < num_tasks; i++) {
    if (base_tasks[i] > 0) {
      does_tasks = true;
      break;
    }
  }
  
  // Don't calculate the modularity if the organism doesn't reproduce. i.e. if the fitness is 0
  if (base_fitness > 0.0 && does_tasks) {
    // Set up the instruction set for mapping
    cInstSet& map_inst_set = m_genotype->GetWorld()->GetHardwareManager().GetInstSet(base_genome.GetInstSet());
    const cInstruction null_inst = map_inst_set.ActivateNullInst();

    // Genome for testing
    const int max_line = base_genome.GetSize();
    cGenome mod_genome(base_genome);
    cSequence& seq = mod_genome.GetSequence();
    
    // Create and initialize the modularity matrix
    tMatrix<int> mod_matrix(num_tasks, max_line);
    mod_matrix.SetAll(0);
    
    tArray<int> site_num_tasks(max_line, 0);  // number of tasks instruction is used in
    tArray<int> sites_per_task(num_tasks, 0); // number of sites involved in each task
    
    // Loop through all the lines of code, testing the removal of each.
    for (int line_num = 0; line_num < max_line; line_num++) {
      int cur_inst = base_seq[line_num].GetOp();
      
      seq[line_num] = null_inst;
      
      // Run the modified genome through the Test CPU
      testcpu->TestGenome(ctx, test_info, mod_genome);
      
      if (test_info.GetColonyFitness() > 0.0) {
        const tArray<int>& test_tasks = test_info.GetColonyOrganism()->GetPhenotype().GetLastTaskCount();
        
        for (int cur_task = 0; cur_task < num_tasks; cur_task++) {
          // This is done so that under 'binary' option it marks
          // the task as being influenced by the mutation iff
          // it is completely knocked out, not just decreased
          if (base_tasks[cur_task] && !test_tasks[cur_task]) {
            // If knocking out an instruction stops the expression of a particular task, mark that in the modularity matrix
            // and add it to two counts
            mod_matrix(cur_task, line_num) = 1;
            sites_per_task[cur_task]++;
            site_num_tasks[line_num]++;
          }
        }
      }
          
      // Reset the mod_genome back to the original sequence.
      seq[line_num].SetOp(cur_inst);
    }
    
    tArray<int> sites_inv_x_tasks(num_tasks + 1, 0);  // # of inst's involved in 0,1,2,3... tasks    
    tArray<double> ave_task_position(num_tasks, 0.0); // mean positions of the tasks in the genome
    tArray<int> task_length(num_tasks, 0);  // distance between first and last inst involved in a task
 
    int total_task = 0;           // total number of tasks done
    int total_inst = 0;           // total number of instructions involved in tasks
    int total_all = 0;            // sum of mod_matrix
    double sum_task_overlap = 0;  // sum of task overlap for for this genome


    // Calculate instruction and task totals
    for (int i = 0; i < num_tasks; i++) {
      total_all += sites_per_task[i];
      if (sites_per_task[i]) total_task++;
    }
    for (int i = 0; i < max_line; i++) {
      sites_inv_x_tasks[site_num_tasks[i]]++;
      if (site_num_tasks[i] != 0) total_inst++;
    }
    
    
    // Calculate average task overlap
    // first construct num_task x num_task matrix with number of sites overlapping
    tMatrix<int> task_overlap(num_tasks, num_tasks);
    task_overlap.SetAll(0);

    for (int i = 0; i < max_line; i++) {
      for (int j = 0; j < num_tasks; j++) {
        for (int k = j; k < num_tasks; k++) {
          if (mod_matrix(j, i) > 0 && mod_matrix(k, i) > 0) {
            task_overlap(j, k)++;
            if (j != k) task_overlap(k, j)++;
          }               
        }
      }
    }
    
    // go though the task_overlap matrix, add and average everything up. 
    if (total_task > 1) {
      for (int i = 0; i < num_tasks; i++) {
        if (task_overlap(i, i)) {
          int overlap_per_task = 0;
          for (int j = 0; j < num_tasks; j++) if (i != j) overlap_per_task += task_overlap(i,j);
          sum_task_overlap += (double)overlap_per_task / (task_overlap(i, i) * (total_task - 1));
        }
      }
    }
    
    
    // Calculate the first/last position of a task, the task "spread"
    // starting from the top look for the fist command that matters for a task
    for (int i = 0; i < num_tasks; i++) { 
      for (int j = 0; j < max_line; j++) {
        if (mod_matrix(i, j) > 0 && task_length[i] == 0) {
          task_length[i] = j;
          break;
        }
      }
    }
    
    // starting from the bottom look for the last command that matters for a task
    // and subtract it from the first to get the task length
    // add one in order to account for both the beginning and the end instruction
    for (int i = 0; i < num_tasks; i++) {
      for (int j = max_line - 1; j >= 0; j--) {
        if (mod_matrix(i, j) > 0) {
          task_length[i] = j - task_length[i] + 1;
          break;
        }
      }
    }
    
    
    // Calculate mean positions of the tasks
    tArray<int> task_position(num_tasks);
    for (int i = 0; i < num_tasks; i++) {
      task_position[i] = 0;
      for (int j = 0; j < max_line; j++) if (mod_matrix(i,j) > 0) task_position[i] += j;
    }
    for (int i = 0; i < num_tasks; i++) ave_task_position[i] = (double)task_position[i] / sites_per_task[i];
    
    
    cModularityData* mod_data = new cModularityData;
    mod_data->tasks_done = total_task;
    mod_data->insts_tasks = total_inst;
    mod_data->tasks_prop = (double)total_inst / max_line;
    mod_data->ave_tasks_per_site = (total_inst) ? (double)total_all / total_inst : 0.0;
    mod_data->ave_sites_per_task = (total_task) ? (double)total_all / total_task : 0.0;
    mod_data->ave_prop_nonoverlap = (total_task) ? sum_task_overlap / total_task : 0.0;
    mod_data->sites_per_task = sites_per_task;
    mod_data->sites_inv_x_tasks = sites_inv_x_tasks;
    mod_data->task_length = task_length;
    mod_data->ave_task_position = ave_task_position;
    m_genotype->SetGenotypeData(GD_MD_ID, mod_data);
  }
}

#ifdef DEBUG
#define GET_MD() \
  cAnalyzeGenotype::ReadToken* tok = genotype->GetReadToken(); \
  cModularityData* data = dynamic_cast<cModularityData*>(genotype->GetGenotypeData(tok, GD_MD_ID)); \
  delete tok;
#else
#define GET_MD() \
  cAnalyzeGenotype::ReadToken* tok = genotype->GetReadToken(); \
  cModularityData* data = static_cast<cModularityData*>(genotype->GetGenotypeData(tok, GD_MD_ID)); \
  delete tok;
#endif

cFlexVar cModularityAnalysis::GetTasksDoneFor(const cAnalyzeGenotype* genotype)
{
  GET_MD();
  if (data) return cFlexVar(data->tasks_done);
  return cFlexVar(0);
}

cFlexVar cModularityAnalysis::GetInstsInvolvedInTasksFor(const cAnalyzeGenotype* genotype)
{
  GET_MD();
  if (data) return cFlexVar(data->insts_tasks);
  return cFlexVar(0);
}

cFlexVar cModularityAnalysis::GetTaskProportionFor(const cAnalyzeGenotype* genotype)
{
  GET_MD();
  if (data) return cFlexVar(data->tasks_prop);
  return cFlexVar(0.0);
}

cFlexVar cModularityAnalysis::GetAveTasksPerSiteFor(const cAnalyzeGenotype* genotype)
{
  GET_MD();
  if (data) return cFlexVar(data->ave_tasks_per_site);
  return cFlexVar(0.0);
}

cFlexVar cModularityAnalysis::GetAveSitesPerTaskFor(const cAnalyzeGenotype* genotype)
{
  GET_MD();
  if (data) return cFlexVar(data->ave_sites_per_task);
  return cFlexVar(0.0);
}

cFlexVar cModularityAnalysis::GetPropNonoverlapFor(const cAnalyzeGenotype* genotype)
{
  GET_MD();
  if (data) return cFlexVar(data->ave_prop_nonoverlap);
  return cFlexVar(0.0);
}


cFlexVar cModularityAnalysis::GetSitesPerTaskFor(const cAnalyzeGenotype* genotype, int idx)
{
  GET_MD();
  if (data && idx >= 0 && idx < data->sites_per_task.GetSize()) return cFlexVar(data->sites_per_task[idx]);
  return cFlexVar(0);
}

cString cModularityAnalysis::DescSitesPerTask(const cAnalyzeGenotype* genotype, int idx)
{
  return cStringUtil::Stringf("Number of Sites Per Task %d", idx);
}


cFlexVar cModularityAnalysis::GetSitesInvolvedInXTasksFor(const cAnalyzeGenotype* genotype, int idx)
{
  GET_MD();
  if (data && idx >= 0 && idx < data->sites_per_task.GetSize()) return cFlexVar(data->sites_inv_x_tasks[idx]);
  return cFlexVar(0);
}

cString cModularityAnalysis::DescSitesInvolvedInXTasks(const cAnalyzeGenotype* genotype, int idx)
{
  return cStringUtil::Stringf("Number of Sites Involved in %d Tasks", idx);
}


cFlexVar cModularityAnalysis::GetTaskLengthFor(const cAnalyzeGenotype* genotype, int idx)
{
  GET_MD();
  if (data && idx >= 0 && idx < data->sites_per_task.GetSize()) return cFlexVar(data->task_length[idx]);
  return cFlexVar(0);
}

cString cModularityAnalysis::DescTaskLength(const cAnalyzeGenotype* genotype, int idx)
{
  return cStringUtil::Stringf("Task %d Length", idx);
}


cFlexVar cModularityAnalysis::GetAveTaskPositionFor(const cAnalyzeGenotype* genotype, int idx)
{
  GET_MD();
  if (data && idx >= 0 && idx < data->sites_per_task.GetSize()) return cFlexVar(data->ave_task_position[idx]);
  return cFlexVar(0.0);
}

cString cModularityAnalysis::DescAveTaskPosition(const cAnalyzeGenotype* genotype, int idx)
{
  return cStringUtil::Stringf("Task %d Position", idx);
}

#undef GET_MD


cModularityAnalysis::cModularityData::cModularityData()
: tasks_done(0), insts_tasks(0), tasks_prop(0.0), ave_tasks_per_site(0.0), ave_sites_per_task(0.0), ave_prop_nonoverlap(0.0)
{
}
