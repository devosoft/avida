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
  inline void PrintEntropy(cDataFile& df) const { m_src.PrintEntropy(df); }
  inline void PrintSiteCount(cDataFile& df) const { m_src.PrintSiteCount(df); }
  
  inline const cGenome& GetBaseGenome() const { return m_src.GetBaseGenome(); }
  inline double GetBaseFitness() const { return m_src.GetBaseFitness(); }
  
  inline int GetTotal() const { return m_src.GetTotal(); }
  
  inline double GetAveFitness() const { return m_src.GetAveFitness(); }
  inline double GetAveSqrFitness() const { return m_src.GetAveSqrFitness(); }
  inline const cGenome& GetPeakGenome() const { return m_src.GetPeakGenome(); }
  inline double GetPeakFitness() const { return m_src.GetPeakFitness(); }

  inline double GetProbDead() const { return m_src.GetProbDead(); }
  inline double GetProbNeg() const { return m_src.GetProbNeg(); }
  inline double GetProbNeut() const { return m_src.GetProbNeut(); }
  inline double GetProbPos() const { return m_src.GetProbPos(); }
  inline double GetAvPosSize() const { return m_src.GetAvPosSize(); }
  inline double GetAvNegSize() const { return m_src.GetAvNegSize(); }

  inline double GetTotalEntropy() const { return m_src.GetTotalEntropy(); }
  inline double GetComplexity() const { return m_src.GetComplexity(); }

  inline int GetSingleTargetTask() const { return m_src.GetSingleTargetTask(); }
  inline double GetProbSingleTargetTask() const { return m_src.GetProbSingleTargetTask(); }
  inline int GetSingleTask() const { return m_src.GetProbSingleTask(); }
  inline double GetProbSingleTask() const { return m_src.GetProbSingleTask(); }
  inline int GetSingleKnockout() const { return m_src.GetSingleKnockout(); }
  inline double GetProbSingleKnockout() const { return m_src.GetProbSingleKnockout(); }

  inline int GetDoubleTargetTask() const { return m_src.GetDoubleTargetTask(); }
  inline double GetProbDoubleTargetTask() const { return m_src.GetProbDoubleTargetTask(); }
  inline int GetDoubleTargetTaskPos() const { return m_src.GetDoubleTargetTaskPos(); }
  inline double GetProbDoubleTargetTaskPos() const { return m_src.GetProbDoubleTargetTaskPos(); }
  inline int GetDoubleTargetTaskNeg() const { return m_src.GetDoubleTargetTaskNeg(); }
  inline double GetProbDoubleTargetTaskNeg() const { return m_src.GetProbDoubleTargetTaskNeg(); }
  inline int GetDoubleTargetTaskNeut() const { return m_src.GetDoubleTargetTaskNeut(); }
  inline double GetProbDoubleTargetTaskNeut() const { return m_src.GetProbDoubleTargetTaskNeut(); }
  inline int GetDoubleTargetTaskDead() const { return m_src.GetDoubleTargetTaskDead(); }
  inline double GetProbDoubleTargetTaskDead() const { return m_src.GetProbDoubleTargetTaskDead(); }
  inline int GetDoubleTask() const { return m_src.GetProbDoubleTask(); }
  inline double GetProbDoubleTask() const { return m_src.GetProbDoubleTask(); }
  inline int GetDoubleKnockout() const { return m_src.GetDoubleKnockout(); }
  inline double GetProbDoubleKnockout() const { return m_src.GetProbDoubleKnockout(); }  
};

#endif
