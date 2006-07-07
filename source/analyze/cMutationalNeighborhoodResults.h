/*
 *  cMutationalNeighborhoodResults.h
 *  Avida
 *
 *  Created by David Bryson on 6/21/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cMutationalNeighborhoodResults_h
#define cMutationalNeighborhoodResults_h

#ifndef cMutationalNeighborhood_h
#include "cMutationalNeighborhood.h"
#endif

class cMutationalNeighborhoodResults
{
private:
  cMutationalNeighborhood& m_src;

  cMutationalNeighborhoodResults(); // @not_implemented
  cMutationalNeighborhoodResults(const cMutationalNeighborhood&); // @not_implemented
  cMutationalNeighborhoodResults& operator=(const cMutationalNeighborhoodResults&); // @not_implemented
  
public:
  cMutationalNeighborhoodResults(cMutationalNeighborhood& src) : m_src(src)
  {
    pthread_rwlock_rdlock(&m_src.m_rwlock);
  }
  cMutationalNeighborhoodResults(cMutationalNeighborhood* src) : m_src(*src)
  {
    pthread_rwlock_rdlock(&m_src.m_rwlock);
  }
  
  ~cMutationalNeighborhoodResults()
  {
    pthread_rwlock_unlock(&m_src.m_rwlock);
  }
  
  inline void PrintStats(cDataFile& df, int update = -1) const { m_src.PrintStats(df, update); }
  
  inline int GetTargetTask() const { return m_src.GetTargetTask(); }
  
  inline const cGenome& GetBaseGenome() const { return m_src.GetBaseGenome(); }
  inline double GetBaseFitness() const { return m_src.GetBaseFitness(); }
  inline double GetBaseMerit() const { return m_src.GetBaseMerit(); }
  inline double GetBaseGestation() const { return m_src.GetBaseGestation(); }
  inline bool GetBaseTargetTask() const { return m_src.GetBaseTargetTask(); }
  
  inline int GetSingleTotal() const { return m_src.GetSingleTotal(); }
  
  inline double GetSingleAverageFitness() const { return m_src.GetSingleAverageFitness(); }
  inline double GetSingleAverageSqrFitness() const { return m_src.GetSingleAverageSqrFitness(); }
  inline const cGenome& GetSinglePeakGenome() const { return m_src.GetSinglePeakGenome(); }
  inline double GetSinglePeakFitness() const { return m_src.GetSinglePeakFitness(); }

  inline double GetSingleProbPos() const { return m_src.GetSingleProbPos(); }
  inline double GetSingleProbNeg() const { return m_src.GetSingleProbNeg(); }
  inline double GetSingleProbNeut() const { return m_src.GetSingleProbNeut(); }
  inline double GetSingleProbLethal() const { return m_src.GetSingleProbLethal(); }
  inline double GetSingleAverageSizePos() const { return m_src.GetSingleAverageSizePos(); }
  inline double GetSingleAverageSizeNeg() const { return m_src.GetSingleAverageSizeNeg(); }

  inline double GetSingleTotalEntropy() const { return m_src.GetSingleTotalEntropy(); }
  inline double GetSingleComplexity() const { return m_src.GetSingleComplexity(); }

  inline int GetSingleTargetTask() const { return m_src.GetSingleTargetTask(); }
  inline double GetSingleProbTargetTask() const { return m_src.GetSingleProbTargetTask(); }
  inline int GetSingleTask() const { return m_src.GetSingleProbTask(); }
  inline double GetSingleProbTask() const { return m_src.GetSingleProbTask(); }
  inline int GetSingleKnockout() const { return m_src.GetSingleKnockout(); }
  inline double GetSingleProbKnockout() const { return m_src.GetSingleProbKnockout(); }


  inline int GetDoubleTotal() const { return m_src.GetDoubleTotal(); }
  
  inline double GetDoubleAverageFitness() const { return m_src.GetDoubleAverageFitness(); }
  inline double GetDoubleAverageSqrFitness() const { return m_src.GetDoubleAverageSqrFitness(); }
  inline const cGenome& GetDoublePeakGenome() const { return m_src.GetDoublePeakGenome(); }
  inline double GetDoublePeakFitness() const { return m_src.GetDoublePeakFitness(); }

  inline double GetDoubleProbPos() const { return m_src.GetDoubleProbPos(); }
  inline double GetDoubleProbNeg() const { return m_src.GetDoubleProbNeg(); }
  inline double GetDoubleProbNeut() const { return m_src.GetDoubleProbNeut(); }
  inline double GetDoubleProbLethal() const { return m_src.GetDoubleProbLethal(); }
  inline double GetDoubleAverageSizePos() const { return m_src.GetDoubleAverageSizePos(); }
  inline double GetDoubleAverageSizeNeg() const { return m_src.GetDoubleAverageSizeNeg(); }

  inline double GetDoubleTotalEntropy() const { return m_src.GetDoubleTotalEntropy(); }
  inline double GetDoubleComplexity() const { return m_src.GetDoubleComplexity(); }

  inline int GetDoubleTargetTask() const { return m_src.GetDoubleTargetTask(); }
  inline double GetDoubleProbTargetTask() const { return m_src.GetDoubleProbTargetTask(); }
  inline int GetDoubleTargetTaskPos() const { return m_src.GetDoubleTargetTaskPos(); }
  inline double GetDoubleProbTargetTaskPos() const { return m_src.GetDoubleProbTargetTaskPos(); }
  inline int GetDoubleTargetTaskNeg() const { return m_src.GetDoubleTargetTaskNeg(); }
  inline double GetDoubleProbTargetTaskNeg() const { return m_src.GetDoubleProbTargetTaskNeg(); }
  inline int GetDoubleTargetTaskNeut() const { return m_src.GetDoubleTargetTaskNeut(); }
  inline double GetDoubleProbTargetTaskNeut() const { return m_src.GetDoubleProbTargetTaskNeut(); }
  inline int GetDoubleTargetTaskLethal() const { return m_src.GetDoubleTargetTaskLethal(); }
  inline double GetDoubleProbTargetTaskLethal() const { return m_src.GetDoubleProbTargetTaskLethal(); }
  inline int GetDoubleTask() const { return m_src.GetDoubleProbTask(); }
  inline double GetDoubleProbTask() const { return m_src.GetDoubleProbTask(); }
  inline int GetDoubleKnockout() const { return m_src.GetDoubleKnockout(); }
  inline double GetDoubleProbKnockout() const { return m_src.GetDoubleProbKnockout(); }  
};

#endif
