/*
 *  cMutationalNeighborhoodResults.h
 *  Avida
 *
 *  Created by David Bryson on 6/21/06.
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

  inline double GetSingleProbBeneficial() const { return m_src.GetSingleProbBeneficial(); }
  inline double GetSingleProbDeleterious() const { return m_src.GetSingleProbDeleterious(); }
  inline double GetSingleProbNeutral() const { return m_src.GetSingleProbNeutral(); }
  inline double GetSingleProbLethal() const { return m_src.GetSingleProbLethal(); }
  inline double GetSingleAverageSizeBeneficial() const { return m_src.GetSingleAverageSizeBeneficial(); }
  inline double GetSingleAverageSizeDeleterious() const { return m_src.GetSingleAverageSizeDeleterious(); }

  inline double GetSingleTotalEntropy() const { return m_src.GetSingleTotalEntropy(); }
  inline double GetSingleComplexity() const { return m_src.GetSingleComplexity(); }

  inline int GetSingleTargetTask() const { return m_src.GetSingleTargetTask(); }
  inline double GetSingleProbTargetTask() const { return m_src.GetSingleProbTargetTask(); }
  inline int GetSingleTask() const { return m_src.GetSingleTask(); }
  inline double GetSingleProbTask() const { return m_src.GetSingleProbTask(); }
  inline int GetSingleKnockout() const { return m_src.GetSingleKnockout(); }
  inline double GetSingleProbKnockout() const { return m_src.GetSingleProbKnockout(); }


  inline int GetDoubleTotal() const { return m_src.GetDoubleTotal(); }
  
  inline double GetDoubleAverageFitness() const { return m_src.GetDoubleAverageFitness(); }
  inline double GetDoubleAverageSqrFitness() const { return m_src.GetDoubleAverageSqrFitness(); }
  inline const cGenome& GetDoublePeakGenome() const { return m_src.GetDoublePeakGenome(); }
  inline double GetDoublePeakFitness() const { return m_src.GetDoublePeakFitness(); }

  inline double GetDoubleProbBeneficial() const { return m_src.GetDoubleProbBeneficial(); }
  inline double GetDoubleProbDeleterious() const { return m_src.GetDoubleProbDeleterious(); }
  inline double GetDoubleProbNeutral() const { return m_src.GetDoubleProbNeutral(); }
  inline double GetDoubleProbLethal() const { return m_src.GetDoubleProbLethal(); }
  inline double GetDoubleAverageSizeBeneficial() const { return m_src.GetDoubleAverageSizeBeneficial(); }
  inline double GetDoubleAverageSizeDeleterious() const { return m_src.GetDoubleAverageSizeDeleterious(); }

  inline double GetDoubleTotalEntropy() const { return m_src.GetDoubleTotalEntropy(); }
  inline double GetDoubleComplexity() const { return m_src.GetDoubleComplexity(); }

  inline int GetDoubleTargetTask() const { return m_src.GetDoubleTargetTask(); }
  inline double GetDoubleProbTargetTask() const { return m_src.GetDoubleProbTargetTask(); }
  inline int GetDoubleTargetTaskBeneficial() const { return m_src.GetDoubleTargetTaskBeneficial(); }
  inline double GetDoubleProbTargetTaskBeneficial() const { return m_src.GetDoubleProbTargetTaskBeneficial(); }
  inline int GetDoubleTargetTaskDeleterious() const { return m_src.GetDoubleTargetTaskDeleterious(); }
  inline double GetDoubleProbTargetTaskDeleterious() const { return m_src.GetDoubleProbTargetTaskDeleterious(); }
  inline int GetDoubleTargetTaskNeutral() const { return m_src.GetDoubleTargetTaskNeutral(); }
  inline double GetDoubleProbTargetTaskNeutral() const { return m_src.GetDoubleProbTargetTaskNeutral(); }
  inline int GetDoubleTargetTaskLethal() const { return m_src.GetDoubleTargetTaskLethal(); }
  inline double GetDoubleProbTargetTaskLethal() const { return m_src.GetDoubleProbTargetTaskLethal(); }
  inline int GetDoubleTask() const { return m_src.GetDoubleTask(); }
  inline double GetDoubleProbTask() const { return m_src.GetDoubleProbTask(); }
  inline int GetDoubleKnockout() const { return m_src.GetDoubleKnockout(); }
  inline double GetDoubleProbKnockout() const { return m_src.GetDoubleProbKnockout(); }  
};

#endif
