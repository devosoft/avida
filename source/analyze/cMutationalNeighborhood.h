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
  // --------------------------------------------------------------------------
  cRWLock m_rwlock;
  cMutex m_mutex;
  
  bool m_initialized;
  int m_cur_site;
  int m_completed;
  
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
  tArray<sStep> m_twostep;

  tMatrix<double> m_fitness;
  
  struct sPendingTarget
  {
    int site;
    int inst;
    sPendingTarget(int in_site, int in_inst) : site(in_site), inst(in_inst) { ; }
  };
  tList<sPendingTarget> m_pending;
  
  const cInstSet& m_inst_set;  
  int m_target;
  
  
  // Base data
  // --------------------------------------------------------------------------
  cGenome m_base_genome;
  double m_base_fitness;
  double m_base_merit;
  double m_base_gestation;
  tArray<int> m_base_tasks;
  double m_neut_min;  // These two variables are a range around the base
  double m_neut_max;  //   fitness to be counted as neutral mutations.
  
  
  // Aggregated One Step Data
  // --------------------------------------------------------------------------
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
  
  sOneStepAggregate m_op;
  
  
  // Aggregated Two Step Data
  // --------------------------------------------------------------------------
  int m_t_total;

  double m_t_total_fitness;
  double m_t_total_sqr_fitness;
  cGenome m_t_peak_genome;
  double m_t_peak_fitness;
  
  int m_t_pos;
  int m_t_neg;
  int m_t_neut;
  int m_t_dead;
  double m_t_size_pos; 
  double m_t_size_neg; 
  
  tArray<int> m_t_site_count;
  
  double m_t_total_entropy;
  double m_t_complexity;

  int m_t_task_target;
  int m_t_task_target_pos;
  int m_t_task_target_neg;
  int m_t_task_target_neut;
  int m_t_task_target_dead;
  int m_t_task_total;
  int m_t_task_knockout;

  double m_t_task_size_target;
  double m_t_task_size_target_pos;
  double m_t_task_size_target_neg;
  double m_t_task_size_total;
  double m_t_task_size_knockout;
  

  void ProcessInitialize(cAvidaContext& ctx);
  void ProcessOneStepPoint(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
  void ProcessTwoStepPoint(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
//  void ProcessOneStepInsert(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
//  void ProcessTwoStepInsert(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
//  void ProcessOneStepDelete(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
//  void ProcessTwoStepDelete(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
//  void ProcessInDelPointCombo(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
  void ProcessComplete(cAvidaContext& ctx);
  
  void AggregateOneStep(tArray<sStep>& steps, sOneStepAggregate osa);
  
  cMutationalNeighborhood(); // @not_implemented
  cMutationalNeighborhood(const cMutationalNeighborhood&); // @not_implemented
  cMutationalNeighborhood& operator=(const cMutationalNeighborhood&); // @not_implemented
  
public:
  cMutationalNeighborhood(cWorld* world, const cGenome& genome, const cInstSet& inst_set, int target)
  : m_world(world), m_initialized(false), m_inst_set(inst_set), m_target(target), m_base_genome(genome)
  {
    // Acquire write lock, to prevent any Results instances before computing
	m_rwlock.WriteLock();
  }
  ~cMutationalNeighborhood() { ; }
  
  void Process(cAvidaContext& ctx);

  
// These methods can only be accessed via a cMutationalNeighborhoodResults object
private:
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

  inline int GetSingleTotal() const { return m_op.total; }
  
  inline double GetSingleAverageFitness() const { return m_op.total_fitness / m_op.total; }
  inline double GetSingleAverageSqrFitness() const { return m_op.total_sqr_fitness / m_op.total; }
  inline const cGenome& GetSinglePeakGenome() const { return m_op.peak_genome; }
  inline double GetSinglePeakFitness() const { return m_op.peak_fitness; }
  
  inline double GetSingleProbBeneficial()  const { return double(m_op.pos) / m_op.total; }
  inline double GetSingleProbDeleterious()  const { return double(m_op.neg) / m_op.total; }
  inline double GetSingleProbNeutral() const { return double(m_op.neut) / m_op.total; }
  inline double GetSingleProbLethal() const { return double(m_op.dead) / m_op.total; }
  inline double GetSingleAverageSizeBeneficial() const { if (m_op.pos == 0) return 0.0; else return m_op.size_pos / m_op.pos; }
  inline double GetSingleAverageSizeDeleterious() const { if (m_op.neg == 0) return 0.0; else return m_op.size_neg / m_op.neg; }
  
  inline double GetSingleTotalEntropy() const { return m_op.total_entropy; }
  inline double GetSingleComplexity() const { return m_op.complexity; }

  inline int GetSingleTargetTask() const { return m_op.task_target; }
  inline double GetSingleProbTargetTask() const { return double(m_op.task_target) / m_op.total; }
  inline double GetSingleAverageSizeTargetTask() const
  {
    if (m_op.task_target == 0) return 0.0; else return double(m_op.task_size_target) / m_op.task_target;
  }
  inline int GetSingleTask() const { return m_op.task_total; }
  inline double GetSingleProbTask() const { return double(m_op.task_total) / m_op.total; }
  inline double GetSingleAverageSizeTask() const
  {
    if (m_op.task_total == 0) return 0.0; else return double(m_op.task_size_total) / m_op.task_total;
  }
  inline int GetSingleKnockout() const { return m_op.task_knockout; }
  inline double GetSingleProbKnockout() const { return double(m_op.task_knockout) / m_op.total; }
  inline double GetSingleAverageSizeKnockout() const
  {
    if (m_op.task_knockout == 0) return 0.0; else return double(m_op.task_size_knockout) / m_op.task_knockout;
  }
  

  inline int GetDoubleTotal() const { return m_t_total; }
  
  inline double GetDoubleAverageFitness() const { return m_t_total_fitness / m_t_total; }
  inline double GetDoubleAverageSqrFitness() const { return m_t_total_sqr_fitness / m_t_total; }
  inline const cGenome& GetDoublePeakGenome() const { return m_t_peak_genome; }
  inline double GetDoublePeakFitness() const { return m_t_peak_fitness; }
  
  inline double GetDoubleProbBeneficial()  const { return double(m_t_pos) / m_t_total; }
  inline double GetDoubleProbDeleterious()  const { return double(m_t_neg) / m_t_total; }
  inline double GetDoubleProbNeutral() const { return double(m_t_neut) / m_t_total; }
  inline double GetDoubleProbLethal() const { return double(m_t_dead) / m_t_total; }
  inline double GetDoubleAverageSizeBeneficial() const { if (m_t_pos == 0) return 0.0; else return m_t_size_pos / m_t_pos; }
  inline double GetDoubleAverageSizeDeleterious() const { if (m_t_neg == 0) return 0.0; else return m_t_size_neg / m_t_neg; }
  
  inline double GetDoubleTotalEntropy() const { return m_t_total_entropy; }
  inline double GetDoubleComplexity() const { return m_t_complexity; }

  inline int GetDoubleTargetTask() const { return m_t_task_target; }
  inline double GetDoubleProbTargetTask() const { return double(m_t_task_target) / m_t_total; }
  inline double GetDoubleAverageSizeTargetTask() const
  {
    if (m_t_task_target == 0) return 0.0; else return double(m_t_task_size_target) / m_t_task_target;
  }
  inline int GetDoubleTargetTaskBeneficial() const { return m_t_task_target_pos; }
  inline double GetDoubleProbTargetTaskBeneficial() const
  {
    if (m_t_task_target == 0) return 0.0; else return double(m_t_task_target_pos) / (2 * m_t_task_target);
  }
  inline double GetDoubleAverageSizeTargetTaskBeneficial() const
  {
    if (m_t_task_target == 0) return 0.0; else return double(m_t_task_size_target_pos) / (2 * m_t_task_target);
  }
  inline int GetDoubleTargetTaskDeleterious() const { return m_t_task_target_neg; }
  inline double GetDoubleProbTargetTaskDeleterious() const
  {
    if (m_t_task_target == 0) return 0.0; else return double(m_t_task_target_neg) / (2 * m_t_task_target);
  }
  inline double GetDoubleAverageSizeTargetTaskDeleterious() const
  {
    if (m_t_task_target == 0) return 0.0; else return double(m_t_task_size_target_neg) / (2 * m_t_task_target);
  }
  inline int GetDoubleTargetTaskNeutral() const { return m_t_task_target_neut; }
  inline double GetDoubleProbTargetTaskNeutral() const
  {
    if (m_t_task_target == 0) return 0.0; else return double(m_t_task_target_neut) / (2 * m_t_task_target);
  }
  inline int GetDoubleTargetTaskLethal() const { return m_t_task_target_dead; }
  inline double GetDoubleProbTargetTaskLethal() const
  {
    if (m_t_task_target == 0) return 0.0; else return double(m_t_task_target_dead) / (2 * m_t_task_target);
  }
  inline int GetDoubleTask() const { return m_t_task_total; }
  inline double GetDoubleProbTask() const { return double(m_t_task_total) / m_t_total; }
  inline double GetDoubleAverageSizeTask() const
  {
    if (m_t_task_total == 0) return 0.0; else return double(m_t_task_size_total) / m_t_task_total;
  }
  inline int GetDoubleKnockout() const { return m_t_task_knockout; }
  inline double GetDoubleProbKnockout() const { return double(m_t_task_knockout) / m_t_total; }
  inline double GetDoubleAverageSizeKnockout() const
  {
    if (m_t_task_knockout == 0) return 0.0; else return double(m_t_task_size_knockout) / m_t_task_knockout;
  }
};


#ifdef ENABLE_UNIT_TESTS
namespace nMutationalNeighborhood {
  /**
  * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
