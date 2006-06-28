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
  
  struct sOneStep
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

    // state used in two step calculations
    tArray<double> fitness;
    tArray<int> cur_tasks;
    
    sOneStep() : total(0), total_fitness(0.0), total_sqr_fitness(0.0), peak_fitness(0.0), dead(0), neg(0), neut(0), pos(0),
      size_pos(0.0), size_neg(0.0), task_target(0), task_total(0), task_knockout(0) { ; }
  };
  tArray<sOneStep> m_onestep;
  
  struct sTwoStep
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
    int task_target_pos;
    int task_target_neg;
    int task_target_neut;
    int task_target_dead;
    int task_total;
    int task_knockout;
    
    sTwoStep() : total(0), total_fitness(0.0), total_sqr_fitness(0.0), peak_fitness(0.0), dead(0), neg(0), neut(0), pos(0),
      size_pos(0.0), size_neg(0.0), task_target(0), task_target_pos(0), task_target_neg(0), task_target_dead(0),
      task_total(0), task_knockout(0) { ; }
  };
  tArray<sTwoStep> m_twostep;
  
  const cInstSet& m_inst_set;
  
  // Base data
  // --------------------------------------------------------------------------
  cGenome m_base_genome;
  double m_base_fitness;
  double m_base_merit;
  double m_base_gestation;
  tArray<int> m_base_tasks;
  double m_neut_min;  // These two variables are a range around the base
  double m_neut_max;  //   fitness to be counted as neutral mutations.
  
  // Aggregated data
  // --------------------------------------------------------------------------
  int m_total;

  double m_total_fitness;
  double m_total_sqr_fitness;
  cGenome m_peak_genome;
  double m_peak_fitness;
  
  int m_dead;
  int m_neg;
  int m_neut;
  int m_pos;
  double m_size_pos; 
  double m_size_neg; 
  
  tArray<int> m_site_count;
  
  double m_total_entropy;
  double m_complexity;
  
  // Single step task totals
  int m_stask_target;
  int m_stask_total;
  int m_stask_knockout;
  
  // Two step task totals
  int m_ttask_target;
  int m_ttask_target_pos;
  int m_ttask_target_neg;
  int m_ttask_target_neut;
  int m_ttask_target_dead;
  int m_ttask_total;
  int m_ttask_knockout;


  void ProcessInitialize(cAvidaContext& ctx);
  void ProcessOneStep(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site);
  void ProcessTwoStep(cAvidaContext& ctx, cTestCPU* testcpu, cCPUTestInfo& test_info, int cur_site, cGenome& mod_genome);
  void ProcessComplete(cAvidaContext& ctx);
  
  cMutationalNeighborhood(); // @not_implemented
  cMutationalNeighborhood(const cMutationalNeighborhood&); // @not_implemented
  cMutationalNeighborhood& operator=(const cMutationalNeighborhood&); // @not_implemented
  
public:
  cMutationalNeighborhood(cWorld* world, const cGenome& genome, const cInstSet& inst_set)
  : m_world(world), m_initialized(false), m_inst_set(inst_set), m_base_genome(genome)
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
  void PrintStats(cDataFile& df, int update = -1);
  void PrintEntropy(cDataFile& df);
  void PrintSiteCount(cDataFile& df);
  
  inline const cGenome& GetBaseGenome() const { return m_base_genome; }
  inline double GetBaseFitness() const { return m_base_fitness; }
  
  inline int GetTotal() const { return m_total; }
  
  inline double GetAveFitness() const { return m_total_fitness / m_total; }
  inline double GetAveSqrFitness() const { return m_total_sqr_fitness / m_total; }
  inline const cGenome& GetPeakGenome() const { return m_peak_genome; }
  inline double GetPeakFitness() const { return m_peak_fitness; }
  
  inline double GetProbDead() const { return static_cast<double>(m_dead) / m_total; }
  inline double GetProbNeg()  const { return static_cast<double>(m_neg) / m_total; }
  inline double GetProbNeut() const { return static_cast<double>(m_neut) / m_total; }
  inline double GetProbPos()  const { return static_cast<double>(m_pos) / m_total; }
  inline double GetAvPosSize() const { if (m_pos == 0) return 0.0; else return m_size_pos / m_pos; }
  inline double GetAvNegSize() const { if (m_neg == 0) return 0.0; else return m_size_neg / m_neg; }
  
  inline double GetTotalEntropy() const { return m_total_entropy; }
  inline double GetComplexity() const { return m_complexity; }
  
  inline int GetSingleTargetTask() const { return m_stask_target; }
  inline double GetProbSingleTargetTask() const { return static_cast<double>(m_stask_target) / m_total; }
  inline int GetSingleTask() const { return m_stask_total; }
  inline double GetProbSingleTask() const { return static_cast<double>(m_stask_total) / m_total; }
  inline int GetSingleKnockout() const { return m_stask_knockout; }
  inline double GetProbSingleKnockout() const { return static_cast<double>(m_stask_knockout) / m_total; }

  inline int GetDoubleTargetTask() const { return m_ttask_target; }
  inline double GetProbDoubleTargetTask() const { return static_cast<double>(m_ttask_target) / m_total; }
  inline int GetDoubleTargetTaskPos() const { return m_ttask_target_pos; }
  inline double GetProbDoubleTargetTaskPos() const { return static_cast<double>(m_ttask_target_pos) / m_total; }
  inline int GetDoubleTargetTaskNeg() const { return m_ttask_target_neg; }
  inline double GetProbDoubleTargetTaskNeg() const { return static_cast<double>(m_ttask_target_neg) / m_total; }
  inline int GetDoubleTargetTaskNeut() const { return m_ttask_target_neut; }
  inline double GetProbDoubleTargetTaskNeut() const { return static_cast<double>(m_ttask_target_neut) / m_total; }
  inline int GetDoubleTargetTaskDead() const { return m_ttask_target_dead; }
  inline double GetProbDoubleTargetTaskDead() const { return static_cast<double>(m_ttask_target_dead) / m_total; }
  inline int GetDoubleTask() const { return m_stask_total; }
  inline double GetProbDoubleTask() const { return static_cast<double>(m_stask_total) / m_total; }
  inline int GetDoubleKnockout() const { return m_stask_knockout; }
  inline double GetProbDoubleKnockout() const { return static_cast<double>(m_stask_knockout) / m_total; }
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
