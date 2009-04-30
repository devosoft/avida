/*
 *  cPhenPlastSummary
 *  Avida
 *
 *  Created by Matthew Rupp on 4/23/09.
 *
 */

#ifndef cPhenPlastSummary_h
#define cPhenPlastSummary_h

#ifndef cPhenPlastGenotype_h
#include "cPhenPlastGenotype.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cPhenPlastSummary{
  public:
    int     m_recalculate_trials;  
    int     m_num_phenotypes;
    double  m_min_fitness;
    double  m_max_fitness;
    double  m_avg_fitness;
    double  m_likely_fitness;
    double  m_phenotypic_entropy;
    double  m_likely_frequency;
    double  m_min_fit_frequency;
    double  m_max_fit_frequency;
    double m_viable_probability;
    tArray<double> m_task_probabilities;
  
  cPhenPlastSummary(const cPhenPlastGenotype& pp) 
  {
    m_recalculate_trials = pp.GetNumTrials();
    m_max_fitness = pp.GetMaximumFitness();
    m_avg_fitness = pp.GetAverageFitness();
    m_min_fitness = pp.GetMinimumFitness();
    m_phenotypic_entropy = pp.GetPhenotypicEntropy();
    m_likely_frequency  = pp.GetMaximumFrequency();
    m_max_fit_frequency = pp.GetMaximumFitnessFrequency();
    m_min_fit_frequency = pp.GetMinimumFitnessFrequency();
    m_likely_fitness = pp.GetLikelyFitness();
    m_num_phenotypes = pp.GetNumPhenotypes();
    m_task_probabilities = pp.GetTaskProbabilities();
    m_viable_probability = pp.GetViableProbability();
  }
  
  cPhenPlastSummary(const cPhenPlastSummary& ps){
    m_recalculate_trials = ps.m_recalculate_trials;
    m_num_phenotypes = ps.m_num_phenotypes;
    m_min_fitness = ps.m_min_fitness;
    m_max_fitness = ps.m_max_fitness;
    m_avg_fitness = ps.m_avg_fitness;
    m_likely_fitness = ps.m_likely_fitness;
    m_min_fit_frequency = ps.m_min_fit_frequency;
    m_max_fit_frequency = ps.m_max_fit_frequency;
    m_viable_probability = ps.m_viable_probability;
    m_task_probabilities = ps.m_task_probabilities;
  }
  
  
};

#endif
