/*
 *  cMutationalNeighborhood.h
 *  Avida
 *
 *  Created by David on 6/13/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cMutationalNeighborhood_h
#define cMutationalNeighborhood_h

#ifndef cGenome_h
#include "cGenome.h"
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

#include <pthread.h>

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
  pthread_rwlock_t m_rwlock;
  pthread_mutex_t m_mutex;
  
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
    
    int dead;
    int neg;
    int neut;
    int pos;
    double size_pos;
    double size_neg;
    
    tArray<int> site_count;
    
    int task_target;
    int task_total;
    int task_knockout;

    
    sStep() : total(0), total_fitness(0.0), total_sqr_fitness(0.0), peak_fitness(0.0), dead(0), neg(0), neut(0), pos(0),
      size_pos(0.0), size_neg(0.0), task_target(0), task_total(0), task_knockout(0) { ; }
  };
  tArray<sStep> m_onestep;
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
  int m_o_total;

  double m_o_total_fitness;
  double m_o_total_sqr_fitness;
  cGenome m_o_peak_genome;
  double m_o_peak_fitness;
  
  int m_o_dead;
  int m_o_neg;
  int m_o_neut;
  int m_o_pos;
  double m_o_size_pos; 
  double m_o_size_neg; 
  
  tArray<int> m_o_site_count;
  
  double m_o_total_entropy;
  double m_o_complexity;
  
  int m_o_task_target;
  int m_o_task_total;
  int m_o_task_knockout;
  
  // Aggregated Two Step Data
  // --------------------------------------------------------------------------
  int m_t_total;

  double m_t_total_fitness;
  double m_t_total_sqr_fitness;
  cGenome m_t_peak_genome;
  double m_t_peak_fitness;
  
  int m_t_dead;
  int m_t_neg;
  int m_t_neut;
  int m_t_pos;
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


  void ProcessInitialize(cAvidaContext& ctx);
  void ProcessOneStep(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
  void ProcessTwoStep(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
  void ProcessComplete(cAvidaContext& ctx);
  
  cMutationalNeighborhood(); // @not_implemented
  cMutationalNeighborhood(const cMutationalNeighborhood&); // @not_implemented
  cMutationalNeighborhood& operator=(const cMutationalNeighborhood&); // @not_implemented
  
public:
  cMutationalNeighborhood(cWorld* world, const cGenome& genome, const cInstSet& inst_set, int target)
  : m_world(world), m_initialized(false), m_inst_set(inst_set), m_target(target), m_base_genome(genome)
  {
    pthread_rwlock_init(&m_rwlock, NULL);
    pthread_mutex_init(&m_mutex, NULL);
    
    // Acquire write lock, to prevent any Results instances before computing
    pthread_rwlock_wrlock(&m_rwlock);
  }
  ~cMutationalNeighborhood()
  {
    pthread_rwlock_destroy(&m_rwlock);
    pthread_mutex_destroy(&m_mutex);
  }
  
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

  inline int GetSingleTotal() const { return m_o_total; }
  
  inline double GetSingleAverageFitness() const { return m_o_total_fitness / m_o_total; }
  inline double GetSingleAverageSqrFitness() const { return m_o_total_sqr_fitness / m_o_total; }
  inline const cGenome& GetSinglePeakGenome() const { return m_o_peak_genome; }
  inline double GetSinglePeakFitness() const { return m_o_peak_fitness; }
  
  inline double GetSingleProbDead() const { return static_cast<double>(m_o_dead) / m_o_total; }
  inline double GetSingleProbNeg()  const { return static_cast<double>(m_o_neg) / m_o_total; }
  inline double GetSingleProbNeut() const { return static_cast<double>(m_o_neut) / m_o_total; }
  inline double GetSingleProbPos()  const { return static_cast<double>(m_o_pos) / m_o_total; }
  inline double GetSingleAverageSizePos() const { if (m_o_pos == 0) return 0.0; else return m_o_size_pos / m_o_pos; }
  inline double GetSingleAverageSizeNeg() const { if (m_o_neg == 0) return 0.0; else return m_o_size_neg / m_o_neg; }
  
  inline double GetSingleTotalEntropy() const { return m_o_total_entropy; }
  inline double GetSingleComplexity() const { return m_o_complexity; }

  inline int GetSingleTargetTask() const { return m_o_task_target; }
  inline double GetSingleProbTargetTask() const { return static_cast<double>(m_o_task_target) / m_o_total; }
  inline int GetSingleTask() const { return m_o_task_total; }
  inline double GetSingleProbTask() const { return static_cast<double>(m_o_task_total) / m_o_total; }
  inline int GetSingleKnockout() const { return m_o_task_knockout; }
  inline double GetSingleProbKnockout() const { return static_cast<double>(m_o_task_knockout) / m_o_total; }
  

  inline int GetDoubleTotal() const { return m_t_total; }
  
  inline double GetDoubleAverageFitness() const { return m_t_total_fitness / m_t_total; }
  inline double GetDoubleAverageSqrFitness() const { return m_t_total_sqr_fitness / m_t_total; }
  inline const cGenome& GetDoublePeakGenome() const { return m_t_peak_genome; }
  inline double GetDoublePeakFitness() const { return m_t_peak_fitness; }
  
  inline double GetDoubleProbDead() const { return static_cast<double>(m_t_dead) / m_t_total; }
  inline double GetDoubleProbNeg()  const { return static_cast<double>(m_t_neg) / m_t_total; }
  inline double GetDoubleProbNeut() const { return static_cast<double>(m_t_neut) / m_t_total; }
  inline double GetDoubleProbPos()  const { return static_cast<double>(m_t_pos) / m_t_total; }
  inline double GetDoubleAverageSizePos() const { if (m_t_pos == 0) return 0.0; else return m_t_size_pos / m_t_pos; }
  inline double GetDoubleAverageSizeNeg() const { if (m_t_neg == 0) return 0.0; else return m_t_size_neg / m_t_neg; }
  
  inline double GetDoubleTotalEntropy() const { return m_t_total_entropy; }
  inline double GetDoubleComplexity() const { return m_t_complexity; }

  inline int GetDoubleTargetTask() const { return m_t_task_target; }
  inline double GetDoubleProbTargetTask() const { return static_cast<double>(m_t_task_target) / m_t_total; }
  inline int GetDoubleTargetTaskPos() const { return m_t_task_target_pos; }
  inline double GetDoubleProbTargetTaskPos() const
  {
    if (m_t_task_target == 0) return 0.0; else return static_cast<double>(m_t_task_target_pos) / (2 * m_t_task_target);
  }
  inline int GetDoubleTargetTaskNeg() const { return m_t_task_target_neg; }
  inline double GetDoubleProbTargetTaskNeg() const
  {
    if (m_t_task_target == 0) return 0.0; else return static_cast<double>(m_t_task_target_neg) / (2 * m_t_task_target);
  }
  inline int GetDoubleTargetTaskNeut() const { return m_t_task_target_neut; }
  inline double GetDoubleProbTargetTaskNeut() const
  {
    if (m_t_task_target == 0) return 0.0; else return static_cast<double>(m_t_task_target_neut) / (2 * m_t_task_target);
  }
  inline int GetDoubleTargetTaskDead() const { return m_t_task_target_dead; }
  inline double GetDoubleProbTargetTaskDead() const
  {
    if (m_t_task_target == 0) return 0.0; else return static_cast<double>(m_t_task_target_dead) / (2 * m_t_task_target);
  }
  inline int GetDoubleTask() const { return m_t_task_total; }
  inline double GetDoubleProbTask() const { return static_cast<double>(m_t_task_total) / m_t_total; }
  inline int GetDoubleKnockout() const { return m_t_task_knockout; }
  inline double GetDoubleProbKnockout() const { return static_cast<double>(m_t_task_knockout) / m_t_total; }
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
