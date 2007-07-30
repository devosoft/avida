/*
 *  cMutationalNeighborhood.h
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

#ifndef cMutationalNeighborhood_h
#define cMutationalNeighborhood_h

#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef cMutex_h
#include "cMutex.h"
#endif
#ifndef cRWLock_h
#include "cRWLock.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif
#ifndef tMatrix_h
#include "tMatrix.h"
#endif

class cAvidaContext;
class cCPUMemory;
class cCPUTestInfo;
class cDataFile;
class cInstSet;
class cInstruction;
class cTestCPU;
class cWorld;


class cMutationalNeighborhood
{
  friend class cMutationalNeighborhoodResults;
  
private:
  cWorld* m_world;
  
  // Internal state information
  // -----------------------------------------------------------------------------------------------------------------------
  cRWLock m_rwlock;
  cMutex m_mutex;
  
  bool m_initialized;
  int m_cur_site;
  int m_completed;
  
  const cInstSet& m_inst_set;  
  int m_target;
  
  
  
  // Base data
  // -----------------------------------------------------------------------------------------------------------------------
  cGenome m_base_genome;
  double m_base_fitness;
  double m_base_merit;
  double m_base_gestation;
  tArray<int> m_base_tasks;
  double m_neut_min;  // These two variables are a range around the base
  double m_neut_max;  //   fitness to be counted as neutral mutations.
  
  

  // One Step Per-Site Data
  // -----------------------------------------------------------------------------------------------------------------------
  struct sStep
  {
    int total;
    
    double total_fitness;
    double total_sqr_fitness;
    cGenome peak_genome;
    double peak_fitness;
    
    int pos;
    int neg;
    int neut;
    int dead;
    double size_pos;
    double size_neg;
    
    tArray<int> site_count;
    
    int task_target;
    int task_total;
    int task_knockout;
    
    double task_size_target;
    double task_size_total;
    double task_size_knockout;

    
    sStep() : total(0), total_fitness(0.0), total_sqr_fitness(0.0), peak_fitness(0.0), pos(0), neg(0), neut(0), dead(0),
      size_pos(0.0), size_neg(0.0), task_target(0), task_total(0), task_knockout(0), task_size_target(0.0),
      task_size_total(0.0), task_size_knockout(0.0) { ; }
  };
  tArray<sStep> m_onestep_point;
  tArray<sStep> m_onestep_insert;
  tArray<sStep> m_onestep_delete;
  

  // Two Step Per-Site Data
  // -----------------------------------------------------------------------------------------------------------------------
  
  // Based on sStep, the sTwoStep data structure adds a pending list to calculate fitness effects based single step
  // fitness values that may not have been calculated yet.  A pending list must be maintained for each site, as a
  // list in the main class would be subject to a race condition should two separate threads try to write to it
  // simultaneously.
  struct sPendingTarget
  {
    int site;
    int inst;
    sPendingTarget(int in_site, int in_inst) : site(in_site), inst(in_inst) { ; }
  };
  struct sTwoStep : public sStep
  {
    tList<sPendingTarget> pending;
    
    sTwoStep() : sStep() { ; }
  };  
  tArray<sTwoStep> m_twostep_point;
  tArray<sTwoStep> m_twostep_insert;
  tArray<sTwoStep> m_twostep_delete;


  // One Step Fitness Data
  // -----------------------------------------------------------------------------------------------------------------------
  tMatrix<double> m_fitness_point;
  tMatrix<double> m_fitness_insert;
  tArray<double> m_fitness_delete;
  


  // Aggregated One Step Data
  // -----------------------------------------------------------------------------------------------------------------------
  struct sOneStepAggregate
  {
    int total;
    
    double total_fitness;
    double total_sqr_fitness;
    cGenome peak_genome;
    double peak_fitness;
    
    int pos;
    int neg;
    int neut;
    int dead;
    double size_pos; 
    double size_neg; 
    
    tArray<int> site_count;
    
    double total_entropy;
    double complexity;
    
    int task_target;
    int task_total;
    int task_knockout;
    
    double task_size_target;
    double task_size_total;
    double task_size_knockout;
    
    sOneStepAggregate() : total(0), total_fitness(0.0), total_sqr_fitness(0.0), peak_fitness(0.0), pos(0), neg(0), neut(0),
      dead(0), size_pos(0.0), size_neg(0.0), total_entropy(0.0), complexity(0.0), task_target(0), task_total(0),
      task_knockout(0), task_size_target(0.0), task_size_total(0.0), task_size_knockout(0.0) { ; }
  };

  sOneStepAggregate m_op; // One Step Point Mutants
  sOneStepAggregate m_oi; // One Step Insert Mutants
  sOneStepAggregate m_od; // One Step Delete Mutants

  sOneStepAggregate m_ot; // One Step Total

  
  
  // Aggregated Two Step Data
  // -----------------------------------------------------------------------------------------------------------------------
  struct sTwoStepAggregate : public sOneStepAggregate
  {
    int task_target_pos;
    int task_target_neg;
    int task_target_neut;
    int task_target_dead;

    double task_size_target_pos;
    double task_size_target_neg;
    
    sTwoStepAggregate() : sOneStepAggregate(), task_target_pos(0), task_target_neg(0), task_target_neut(0),
      task_target_dead(0), task_size_target_pos(0.0), task_size_target_neg(0.0) { ; }
  };
  
  sTwoStepAggregate m_tp;
  sTwoStepAggregate m_ti;
  sTwoStepAggregate m_td;
  
  sTwoStepAggregate m_tt;

  
  
  cMutationalNeighborhood(); // @not_implemented
  cMutationalNeighborhood(const cMutationalNeighborhood&); // @not_implemented
  cMutationalNeighborhood& operator=(const cMutationalNeighborhood&); // @not_implemented
  
public:
  // Public Methods - Instantiate and Process Only.   All results must be read with a cMutationalNeighborhood object.
  // -----------------------------------------------------------------------------------------------------------------------
  cMutationalNeighborhood(cWorld* world, const cGenome& genome, const cInstSet& inst_set, int target)
    : m_world(world), m_initialized(false), m_inst_set(inst_set), m_target(target), m_base_genome(genome)
  {
    // Acquire write lock, to prevent any cMutationalNeighborhoodResults instances before computing
    m_rwlock.WriteLock();
  }
  ~cMutationalNeighborhood() { ; }
  
  void Process(cAvidaContext& ctx);


private:
  // Internal Calculation Methods
  // -----------------------------------------------------------------------------------------------------------------------
  void ProcessInitialize(cAvidaContext& ctx);
  
  void ProcessOneStepPoint(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
  void ProcessOneStepInsert(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
  void ProcessOneStepDelete(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
  double ProcessOneStepGenome(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, const cGenome& mod_genome,
                              sStep& odata, int cur_site);
  void AggregateOneStep(tArray<sStep>& steps, sOneStepAggregate osa);

  void ProcessTwoStepPoint(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
  void ProcessTwoStepInsert(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cCPUMemory& mod_genome);
  void ProcessTwoStepDelete(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cCPUMemory& mod_genome);
  //  void ProcessInDelPointCombo(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
  double ProcessTwoStepGenome(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, const cGenome& mod_genome,
                              sTwoStep& tdata, int cur_site, int oth_site);
  void AggregateTwoStep(tArray<sTwoStep>& steps, sTwoStepAggregate osa);
  
  void ProcessComplete(cAvidaContext& ctx);
  
  
  
  // cMutationalNeighborhoodResults Backing Methods
  // -----------------------------------------------------------------------------------------------------------------------
  void PrintStats(cDataFile& df, int update = -1) const;
  
  inline int GetTargetTask() const { return m_target; }

  inline const cGenome& GetBaseGenome() const { return m_base_genome; }
  inline double GetBaseFitness() const { return m_base_fitness; }
  inline double GetBaseMerit() const { return m_base_merit; }
  inline double GetBaseGestation() const { return m_base_gestation; }
  inline bool GetBaseTargetTask() const
  {
    if (m_base_tasks.GetSize()) return m_base_tasks[m_target]; else return false;
  }

  
  
  inline int Get1SAggregateTotal() const { return m_ot.total; }
  
  inline double Get1SAggregateAverageFitness() const { return m_ot.total_fitness / m_ot.total; }
  inline double Get1SAggregateAverageSqrFitness() const { return m_ot.total_sqr_fitness / m_ot.total; }
  inline const cGenome& Get1SAggregatePeakGenome() const { return m_ot.peak_genome; }
  inline double Get1SAggregatePeakFitness() const { return m_ot.peak_fitness; }
  
  inline double Get1SAggregateProbBeneficial()  const { return double(m_ot.pos) / m_ot.total; }
  inline double Get1SAggregateProbDeleterious()  const { return double(m_ot.neg) / m_ot.total; }
  inline double Get1SAggregateProbNeutral() const { return double(m_ot.neut) / m_ot.total; }
  inline double Get1SAggregateProbLethal() const { return double(m_ot.dead) / m_ot.total; }
  inline double Get1SAggregateAverageSizeBeneficial() const { if (m_ot.pos == 0) return 0.0; else return m_ot.size_pos / m_ot.pos; }
  inline double Get1SAggregateAverageSizeDeleterious() const { if (m_ot.neg == 0) return 0.0; else return m_ot.size_neg / m_ot.neg; }
  
  //inline double Get1SAggregateTotalEntropy() const { return m_ot.total_entropy; }
  //inline double Get1SAggregateComplexity() const { return m_ot.complexity; }
  
  inline int Get1SAggregateTargetTask() const { return m_ot.task_target; }
  inline double Get1SAggregateProbTargetTask() const { return double(m_ot.task_target) / m_ot.total; }
  inline double Get1SAggregateAverageSizeTargetTask() const
  {
    if (m_ot.task_target == 0) return 0.0; else return double(m_ot.task_size_target) / m_ot.task_target;
  }
  inline int Get1SAggregateTask() const { return m_ot.task_total; }
  inline double Get1SAggregateProbTask() const { return double(m_ot.task_total) / m_ot.total; }
  inline double Get1SAggregateAverageSizeTask() const
  {
    if (m_ot.task_total == 0) return 0.0; else return double(m_ot.task_size_total) / m_ot.task_total;
  }
  inline int Get1SAggregateKnockout() const { return m_ot.task_knockout; }
  inline double Get1SAggregateProbKnockout() const { return double(m_ot.task_knockout) / m_ot.total; }
  inline double Get1SAggregateAverageSizeKnockout() const
  {
    if (m_ot.task_knockout == 0) return 0.0; else return double(m_ot.task_size_knockout) / m_ot.task_knockout;
  }
  
  
  
  
  inline int Get1SPointTotal() const { return m_op.total; }
  
  inline double Get1SPointAverageFitness() const { return m_op.total_fitness / m_op.total; }
  inline double Get1SPointAverageSqrFitness() const { return m_op.total_sqr_fitness / m_op.total; }
  inline const cGenome& Get1SPointPeakGenome() const { return m_op.peak_genome; }
  inline double Get1SPointPeakFitness() const { return m_op.peak_fitness; }
  
  inline double Get1SPointProbBeneficial()  const { return double(m_op.pos) / m_op.total; }
  inline double Get1SPointProbDeleterious()  const { return double(m_op.neg) / m_op.total; }
  inline double Get1SPointProbNeutral() const { return double(m_op.neut) / m_op.total; }
  inline double Get1SPointProbLethal() const { return double(m_op.dead) / m_op.total; }
  inline double Get1SPointAverageSizeBeneficial() const { if (m_op.pos == 0) return 0.0; else return m_op.size_pos / m_op.pos; }
  inline double Get1SPointAverageSizeDeleterious() const { if (m_op.neg == 0) return 0.0; else return m_op.size_neg / m_op.neg; }
  
  inline double Get1SPointTotalEntropy() const { return m_op.total_entropy; }
  inline double Get1SPointComplexity() const { return m_op.complexity; }

  inline int Get1SPointTargetTask() const { return m_op.task_target; }
  inline double Get1SPointProbTargetTask() const { return double(m_op.task_target) / m_op.total; }
  inline double Get1SPointAverageSizeTargetTask() const
  {
    if (m_op.task_target == 0) return 0.0; else return double(m_op.task_size_target) / m_op.task_target;
  }
  inline int Get1SPointTask() const { return m_op.task_total; }
  inline double Get1SPointProbTask() const { return double(m_op.task_total) / m_op.total; }
  inline double Get1SPointAverageSizeTask() const
  {
    if (m_op.task_total == 0) return 0.0; else return double(m_op.task_size_total) / m_op.task_total;
  }
  inline int Get1SPointKnockout() const { return m_op.task_knockout; }
  inline double Get1SPointProbKnockout() const { return double(m_op.task_knockout) / m_op.total; }
  inline double Get1SPointAverageSizeKnockout() const
  {
    if (m_op.task_knockout == 0) return 0.0; else return double(m_op.task_size_knockout) / m_op.task_knockout;
  }
  

  
  
  inline int Get1SInsertTotal() const { return m_oi.total; }
  
  inline double Get1SInsertAverageFitness() const { return m_oi.total_fitness / m_oi.total; }
  inline double Get1SInsertAverageSqrFitness() const { return m_oi.total_sqr_fitness / m_oi.total; }
  inline const cGenome& Get1SInsertPeakGenome() const { return m_oi.peak_genome; }
  inline double Get1SInsertPeakFitness() const { return m_oi.peak_fitness; }
  
  inline double Get1SInsertProbBeneficial()  const { return double(m_oi.pos) / m_oi.total; }
  inline double Get1SInsertProbDeleterious()  const { return double(m_oi.neg) / m_oi.total; }
  inline double Get1SInsertProbNeutral() const { return double(m_oi.neut) / m_oi.total; }
  inline double Get1SInsertProbLethal() const { return double(m_oi.dead) / m_oi.total; }
  inline double Get1SInsertAverageSizeBeneficial() const { if (m_oi.pos == 0) return 0.0; else return m_oi.size_pos / m_oi.pos; }
  inline double Get1SInsertAverageSizeDeleterious() const { if (m_oi.neg == 0) return 0.0; else return m_oi.size_neg / m_oi.neg; }
  
  inline double Get1SInsertTotalEntropy() const { return m_oi.total_entropy; }
  inline double Get1SInsertComplexity() const { return m_oi.complexity; }
  
  inline int Get1SInsertTargetTask() const { return m_oi.task_target; }
  inline double Get1SInsertProbTargetTask() const { return double(m_oi.task_target) / m_oi.total; }
  inline double Get1SInsertAverageSizeTargetTask() const
  {
    if (m_oi.task_target == 0) return 0.0; else return double(m_oi.task_size_target) / m_oi.task_target;
  }
  inline int Get1SInsertTask() const { return m_oi.task_total; }
  inline double Get1SInsertProbTask() const { return double(m_oi.task_total) / m_oi.total; }
  inline double Get1SInsertAverageSizeTask() const
  {
    if (m_oi.task_total == 0) return 0.0; else return double(m_oi.task_size_total) / m_oi.task_total;
  }
  inline int Get1SInsertKnockout() const { return m_oi.task_knockout; }
  inline double Get1SInsertProbKnockout() const { return double(m_oi.task_knockout) / m_oi.total; }
  inline double Get1SInsertAverageSizeKnockout() const
  {
    if (m_oi.task_knockout == 0) return 0.0; else return double(m_oi.task_size_knockout) / m_oi.task_knockout;
  }
  
  
  
  
  inline int Get1SDeleteTotal() const { return m_od.total; }
  
  inline double Get1SDeleteAverageFitness() const { return m_od.total_fitness / m_od.total; }
  inline double Get1SDeleteAverageSqrFitness() const { return m_od.total_sqr_fitness / m_od.total; }
  inline const cGenome& Get1SDeletePeakGenome() const { return m_od.peak_genome; }
  inline double Get1SDeletePeakFitness() const { return m_od.peak_fitness; }
  
  inline double Get1SDeleteProbBeneficial()  const { return double(m_od.pos) / m_od.total; }
  inline double Get1SDeleteProbDeleterious()  const { return double(m_od.neg) / m_od.total; }
  inline double Get1SDeleteProbNeutral() const { return double(m_od.neut) / m_od.total; }
  inline double Get1SDeleteProbLethal() const { return double(m_od.dead) / m_od.total; }
  inline double Get1SDeleteAverageSizeBeneficial() const { if (m_od.pos == 0) return 0.0; else return m_od.size_pos / m_od.pos; }
  inline double Get1SDeleteAverageSizeDeleterious() const { if (m_od.neg == 0) return 0.0; else return m_od.size_neg / m_od.neg; }
  
  inline double Get1SDeleteTotalEntropy() const { return m_od.total_entropy; }
  inline double Get1SDeleteComplexity() const { return m_od.complexity; }
  
  inline int Get1SDeleteTargetTask() const { return m_od.task_target; }
  inline double Get1SDeleteProbTargetTask() const { return double(m_od.task_target) / m_od.total; }
  inline double Get1SDeleteAverageSizeTargetTask() const
  {
    if (m_od.task_target == 0) return 0.0; else return double(m_od.task_size_target) / m_od.task_target;
  }
  inline int Get1SDeleteTask() const { return m_od.task_total; }
  inline double Get1SDeleteProbTask() const { return double(m_od.task_total) / m_od.total; }
  inline double Get1SDeleteAverageSizeTask() const
  {
    if (m_od.task_total == 0) return 0.0; else return double(m_od.task_size_total) / m_od.task_total;
  }
  inline int Get1SDeleteKnockout() const { return m_od.task_knockout; }
  inline double Get1SDeleteProbKnockout() const { return double(m_od.task_knockout) / m_od.total; }
  inline double Get1SDeleteAverageSizeKnockout() const
  {
    if (m_od.task_knockout == 0) return 0.0; else return double(m_od.task_size_knockout) / m_od.task_knockout;
  }
  
  
  
  
  inline int Get2SAggregateTotal() const { return m_tt.total; }
  
  inline double Get2SAggregateAverageFitness() const { return m_tt.total_fitness / m_tt.total; }
  inline double Get2SAggregateAverageSqrFitness() const { return m_tt.total_sqr_fitness / m_tt.total; }
  inline const cGenome& Get2SAggregatePeakGenome() const { return m_tt.peak_genome; }
  inline double Get2SAggregatePeakFitness() const { return m_tt.peak_fitness; }
  
  inline double Get2SAggregateProbBeneficial()  const { return double(m_tt.pos) / m_tt.total; }
  inline double Get2SAggregateProbDeleterious()  const { return double(m_tt.neg) / m_tt.total; }
  inline double Get2SAggregateProbNeutral() const { return double(m_tt.neut) / m_tt.total; }
  inline double Get2SAggregateProbLethal() const { return double(m_tt.dead) / m_tt.total; }
  inline double Get2SAggregateAverageSizeBeneficial() const { if (m_tt.pos == 0) return 0.0; else return m_tt.size_pos / m_tt.pos; }
  inline double Get2SAggregateAverageSizeDeleterious() const { if (m_tt.neg == 0) return 0.0; else return m_tt.size_neg / m_tt.neg; }
  
//  inline double Get2SAggregateTotalEntropy() const { return m_tt.total_entropy; }
//  inline double Get2SAggregateComplexity() const { return m_tt.complexity; }

  inline int Get2SAggregateTargetTask() const { return m_tt.task_target; }
  inline double Get2SAggregateProbTargetTask() const { return double(m_tt.task_target) / m_tt.total; }
  inline double Get2SAggregateAverageSizeTargetTask() const
  {
    if (m_tt.task_target == 0) return 0.0; else return double(m_tt.task_size_target) / m_tt.task_target;
  }
  inline int Get2SAggregateTargetTaskBeneficial() const { return m_tt.task_target_pos; }
  inline double Get2SAggregateProbTargetTaskBeneficial() const
  {
    if (m_tt.task_target == 0) return 0.0; else return double(m_tt.task_target_pos) / (2 * m_tt.task_target);
  }
  inline double Get2SAggregateAverageSizeTargetTaskBeneficial() const
  {
    if (m_tt.task_target == 0) return 0.0; else return double(m_tt.task_size_target_pos) / (2 * m_tt.task_target);
  }
  inline int Get2SAggregateTargetTaskDeleterious() const { return m_tt.task_target_neg; }
  inline double Get2SAggregateProbTargetTaskDeleterious() const
  {
    if (m_tt.task_target == 0) return 0.0; else return double(m_tt.task_target_neg) / (2 * m_tt.task_target);
  }
  inline double Get2SAggregateAverageSizeTargetTaskDeleterious() const
  {
    if (m_tt.task_target == 0) return 0.0; else return double(m_tt.task_size_target_neg) / (2 * m_tt.task_target);
  }
  inline int Get2SAggregateTargetTaskNeutral() const { return m_tt.task_target_neut; }
  inline double Get2SAggregateProbTargetTaskNeutral() const
  {
    if (m_tt.task_target == 0) return 0.0; else return double(m_tt.task_target_neut) / (2 * m_tt.task_target);
  }
  inline int Get2SAggregateTargetTaskLethal() const { return m_tt.task_target_dead; }
  inline double Get2SAggregateProbTargetTaskLethal() const
  {
    if (m_tt.task_target == 0) return 0.0; else return double(m_tt.task_target_dead) / (2 * m_tt.task_target);
  }
  inline int Get2SAggregateTask() const { return m_tt.task_total; }
  inline double Get2SAggregateProbTask() const { return double(m_tt.task_total) / m_tt.total; }
  inline double Get2SAggregateAverageSizeTask() const
  {
    if (m_tt.task_total == 0) return 0.0; else return double(m_tt.task_size_total) / m_tt.task_total;
  }
  inline int Get2SAggregateKnockout() const { return m_tt.task_knockout; }
  inline double Get2SAggregateProbKnockout() const { return double(m_tt.task_knockout) / m_tt.total; }
  inline double Get2SAggregateAverageSizeKnockout() const
  {
    if (m_tt.task_knockout == 0) return 0.0; else return double(m_tt.task_size_knockout) / m_tt.task_knockout;
  }




  inline int Get2SPointTotal() const { return m_tp.total; }
  
  inline double Get2SPointAverageFitness() const { return m_tp.total_fitness / m_tp.total; }
  inline double Get2SPointAverageSqrFitness() const { return m_tp.total_sqr_fitness / m_tp.total; }
  inline const cGenome& Get2SPointPeakGenome() const { return m_tp.peak_genome; }
  inline double Get2SPointPeakFitness() const { return m_tp.peak_fitness; }
  
  inline double Get2SPointProbBeneficial()  const { return double(m_tp.pos) / m_tp.total; }
  inline double Get2SPointProbDeleterious()  const { return double(m_tp.neg) / m_tp.total; }
  inline double Get2SPointProbNeutral() const { return double(m_tp.neut) / m_tp.total; }
  inline double Get2SPointProbLethal() const { return double(m_tp.dead) / m_tp.total; }
  inline double Get2SPointAverageSizeBeneficial() const { if (m_tp.pos == 0) return 0.0; else return m_tp.size_pos / m_tp.pos; }
  inline double Get2SPointAverageSizeDeleterious() const { if (m_tp.neg == 0) return 0.0; else return m_tp.size_neg / m_tp.neg; }
  
  inline double Get2SPointTotalEntropy() const { return m_tp.total_entropy; }
  inline double Get2SPointComplexity() const { return m_tp.complexity; }
  
  inline int Get2SPointTargetTask() const { return m_tp.task_target; }
  inline double Get2SPointProbTargetTask() const { return double(m_tp.task_target) / m_tp.total; }
  inline double Get2SPointAverageSizeTargetTask() const
  {
    if (m_tp.task_target == 0) return 0.0; else return double(m_tp.task_size_target) / m_tp.task_target;
  }
  inline int Get2SPointTargetTaskBeneficial() const { return m_tp.task_target_pos; }
  inline double Get2SPointProbTargetTaskBeneficial() const
  {
    if (m_tp.task_target == 0) return 0.0; else return double(m_tp.task_target_pos) / (2 * m_tp.task_target);
  }
  inline double Get2SPointAverageSizeTargetTaskBeneficial() const
  {
    if (m_tp.task_target == 0) return 0.0; else return double(m_tp.task_size_target_pos) / (2 * m_tp.task_target);
  }
  inline int Get2SPointTargetTaskDeleterious() const { return m_tp.task_target_neg; }
  inline double Get2SPointProbTargetTaskDeleterious() const
  {
    if (m_tp.task_target == 0) return 0.0; else return double(m_tp.task_target_neg) / (2 * m_tp.task_target);
  }
  inline double Get2SPointAverageSizeTargetTaskDeleterious() const
  {
    if (m_tp.task_target == 0) return 0.0; else return double(m_tp.task_size_target_neg) / (2 * m_tp.task_target);
  }
  inline int Get2SPointTargetTaskNeutral() const { return m_tp.task_target_neut; }
  inline double Get2SPointProbTargetTaskNeutral() const
  {
    if (m_tp.task_target == 0) return 0.0; else return double(m_tp.task_target_neut) / (2 * m_tp.task_target);
  }
  inline int Get2SPointTargetTaskLethal() const { return m_tp.task_target_dead; }
  inline double Get2SPointProbTargetTaskLethal() const
  {
    if (m_tp.task_target == 0) return 0.0; else return double(m_tp.task_target_dead) / (2 * m_tp.task_target);
  }
  inline int Get2SPointTask() const { return m_tp.task_total; }
  inline double Get2SPointProbTask() const { return double(m_tp.task_total) / m_tp.total; }
  inline double Get2SPointAverageSizeTask() const
  {
    if (m_tp.task_total == 0) return 0.0; else return double(m_tp.task_size_total) / m_tp.task_total;
  }
  inline int Get2SPointKnockout() const { return m_tp.task_knockout; }
  inline double Get2SPointProbKnockout() const { return double(m_tp.task_knockout) / m_tp.total; }
  inline double Get2SPointAverageSizeKnockout() const
  {
    if (m_tp.task_knockout == 0) return 0.0; else return double(m_tp.task_size_knockout) / m_tp.task_knockout;
  }

  

  inline int Get2SInsertTotal() const { return m_ti.total; }
  
  inline double Get2SInsertAverageFitness() const { return m_ti.total_fitness / m_ti.total; }
  inline double Get2SInsertAverageSqrFitness() const { return m_ti.total_sqr_fitness / m_ti.total; }
  inline const cGenome& Get2SInsertPeakGenome() const { return m_ti.peak_genome; }
  inline double Get2SInsertPeakFitness() const { return m_ti.peak_fitness; }
  
  inline double Get2SInsertProbBeneficial()  const { return double(m_ti.pos) / m_ti.total; }
  inline double Get2SInsertProbDeleterious()  const { return double(m_ti.neg) / m_ti.total; }
  inline double Get2SInsertProbNeutral() const { return double(m_ti.neut) / m_ti.total; }
  inline double Get2SInsertProbLethal() const { return double(m_ti.dead) / m_ti.total; }
  inline double Get2SInsertAverageSizeBeneficial() const { if (m_ti.pos == 0) return 0.0; else return m_ti.size_pos / m_ti.pos; }
  inline double Get2SInsertAverageSizeDeleterious() const { if (m_ti.neg == 0) return 0.0; else return m_ti.size_neg / m_ti.neg; }
  
  inline double Get2SInsertTotalEntropy() const { return m_ti.total_entropy; }
  inline double Get2SInsertComplexity() const { return m_ti.complexity; }
  
  inline int Get2SInsertTargetTask() const { return m_ti.task_target; }
  inline double Get2SInsertProbTargetTask() const { return double(m_ti.task_target) / m_ti.total; }
  inline double Get2SInsertAverageSizeTargetTask() const
  {
    if (m_ti.task_target == 0) return 0.0; else return double(m_ti.task_size_target) / m_ti.task_target;
  }
  inline int Get2SInsertTargetTaskBeneficial() const { return m_ti.task_target_pos; }
  inline double Get2SInsertProbTargetTaskBeneficial() const
  {
    if (m_ti.task_target == 0) return 0.0; else return double(m_ti.task_target_pos) / (2 * m_ti.task_target);
  }
  inline double Get2SInsertAverageSizeTargetTaskBeneficial() const
  {
    if (m_ti.task_target == 0) return 0.0; else return double(m_ti.task_size_target_pos) / (2 * m_ti.task_target);
  }
  inline int Get2SInsertTargetTaskDeleterious() const { return m_ti.task_target_neg; }
  inline double Get2SInsertProbTargetTaskDeleterious() const
  {
    if (m_ti.task_target == 0) return 0.0; else return double(m_ti.task_target_neg) / (2 * m_ti.task_target);
  }
  inline double Get2SInsertAverageSizeTargetTaskDeleterious() const
  {
    if (m_ti.task_target == 0) return 0.0; else return double(m_ti.task_size_target_neg) / (2 * m_ti.task_target);
  }
  inline int Get2SInsertTargetTaskNeutral() const { return m_ti.task_target_neut; }
  inline double Get2SInsertProbTargetTaskNeutral() const
  {
    if (m_ti.task_target == 0) return 0.0; else return double(m_ti.task_target_neut) / (2 * m_ti.task_target);
  }
  inline int Get2SInsertTargetTaskLethal() const { return m_ti.task_target_dead; }
  inline double Get2SInsertProbTargetTaskLethal() const
  {
    if (m_ti.task_target == 0) return 0.0; else return double(m_ti.task_target_dead) / (2 * m_ti.task_target);
  }
  inline int Get2SInsertTask() const { return m_ti.task_total; }
  inline double Get2SInsertProbTask() const { return double(m_ti.task_total) / m_ti.total; }
  inline double Get2SInsertAverageSizeTask() const
  {
    if (m_ti.task_total == 0) return 0.0; else return double(m_ti.task_size_total) / m_ti.task_total;
  }
  inline int Get2SInsertKnockout() const { return m_ti.task_knockout; }
  inline double Get2SInsertProbKnockout() const { return double(m_ti.task_knockout) / m_ti.total; }
  inline double Get2SInsertAverageSizeKnockout() const
  {
    if (m_ti.task_knockout == 0) return 0.0; else return double(m_ti.task_size_knockout) / m_ti.task_knockout;
  }




  inline int Get2SDeleteTotal() const { return m_td.total; }
  
  inline double Get2SDeleteAverageFitness() const { return m_td.total_fitness / m_td.total; }
  inline double Get2SDeleteAverageSqrFitness() const { return m_td.total_sqr_fitness / m_td.total; }
  inline const cGenome& Get2SDeletePeakGenome() const { return m_td.peak_genome; }
  inline double Get2SDeletePeakFitness() const { return m_td.peak_fitness; }
  
  inline double Get2SDeleteProbBeneficial()  const { return double(m_td.pos) / m_td.total; }
  inline double Get2SDeleteProbDeleterious()  const { return double(m_td.neg) / m_td.total; }
  inline double Get2SDeleteProbNeutral() const { return double(m_td.neut) / m_td.total; }
  inline double Get2SDeleteProbLethal() const { return double(m_td.dead) / m_td.total; }
  inline double Get2SDeleteAverageSizeBeneficial() const { if (m_td.pos == 0) return 0.0; else return m_td.size_pos / m_td.pos; }
  inline double Get2SDeleteAverageSizeDeleterious() const { if (m_td.neg == 0) return 0.0; else return m_td.size_neg / m_td.neg; }
  
  inline double Get2SDeleteTotalEntropy() const { return m_td.total_entropy; }
  inline double Get2SDeleteComplexity() const { return m_td.complexity; }
  
  inline int Get2SDeleteTargetTask() const { return m_td.task_target; }
  inline double Get2SDeleteProbTargetTask() const { return double(m_td.task_target) / m_td.total; }
  inline double Get2SDeleteAverageSizeTargetTask() const
  {
    if (m_td.task_target == 0) return 0.0; else return double(m_td.task_size_target) / m_td.task_target;
  }
  inline int Get2SDeleteTargetTaskBeneficial() const { return m_td.task_target_pos; }
  inline double Get2SDeleteProbTargetTaskBeneficial() const
  {
    if (m_td.task_target == 0) return 0.0; else return double(m_td.task_target_pos) / (2 * m_td.task_target);
  }
  inline double Get2SDeleteAverageSizeTargetTaskBeneficial() const
  {
    if (m_td.task_target == 0) return 0.0; else return double(m_td.task_size_target_pos) / (2 * m_td.task_target);
  }
  inline int Get2SDeleteTargetTaskDeleterious() const { return m_td.task_target_neg; }
  inline double Get2SDeleteProbTargetTaskDeleterious() const
  {
    if (m_td.task_target == 0) return 0.0; else return double(m_td.task_target_neg) / (2 * m_td.task_target);
  }
  inline double Get2SDeleteAverageSizeTargetTaskDeleterious() const
  {
    if (m_td.task_target == 0) return 0.0; else return double(m_td.task_size_target_neg) / (2 * m_td.task_target);
  }
  inline int Get2SDeleteTargetTaskNeutral() const { return m_td.task_target_neut; }
  inline double Get2SDeleteProbTargetTaskNeutral() const
  {
    if (m_td.task_target == 0) return 0.0; else return double(m_td.task_target_neut) / (2 * m_td.task_target);
  }
  inline int Get2SDeleteTargetTaskLethal() const { return m_td.task_target_dead; }
  inline double Get2SDeleteProbTargetTaskLethal() const
  {
    if (m_td.task_target == 0) return 0.0; else return double(m_td.task_target_dead) / (2 * m_td.task_target);
  }
  inline int Get2SDeleteTask() const { return m_td.task_total; }
  inline double Get2SDeleteProbTask() const { return double(m_td.task_total) / m_td.total; }
  inline double Get2SDeleteAverageSizeTask() const
  {
    if (m_td.task_total == 0) return 0.0; else return double(m_td.task_size_total) / m_td.task_total;
  }
  inline int Get2SDeleteKnockout() const { return m_td.task_knockout; }
  inline double Get2SDeleteProbKnockout() const { return double(m_td.task_knockout) / m_td.total; }
  inline double Get2SDeleteAverageSizeKnockout() const
  {
    if (m_td.task_knockout == 0) return 0.0; else return double(m_td.task_size_knockout) / m_td.task_knockout;
  }
};


#endif
