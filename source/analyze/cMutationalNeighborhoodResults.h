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
    m_src.m_rwlock.ReadLock();
  }
  cMutationalNeighborhoodResults(cMutationalNeighborhood* src) : m_src(*src)
  {
    m_src.m_rwlock.ReadLock();
  }
  
  ~cMutationalNeighborhoodResults()
  {
    m_src.m_rwlock.ReadUnlock();
  }
  
  inline void PrintStats(cDataFile& df, int update = -1) const { m_src.PrintStats(df, update); }
  
  inline int GetTargetTask() const { return m_src.GetTargetTask(); }
  
  inline const cGenome& GetBaseGenome() const { return m_src.GetBaseGenome(); }
  inline double GetBaseFitness() const { return m_src.GetBaseFitness(); }
  inline double GetBaseMerit() const { return m_src.GetBaseMerit(); }
  inline double GetBaseGestation() const { return m_src.GetBaseGestation(); }
  inline bool GetBaseTargetTask() const { return m_src.GetBaseTargetTask(); }
  
  
  
  
  inline int Get1SAggregateTotal() const { return m_src.Get1SAggregateTotal(); }
  
  inline double Get1SAggregateAverageFitness() const { return m_src.Get1SAggregateAverageFitness(); }
  inline double Get1SAggregateAverageSqrFitness() const { return m_src.Get1SAggregateAverageSqrFitness(); }
  inline const cGenome& Get1SAggregatePeakGenome() const { return m_src.Get1SAggregatePeakGenome(); }
  inline double Get1SAggregatePeakFitness() const { return m_src.Get1SAggregatePeakFitness(); }

  inline double Get1SAggregateProbBeneficial() const { return m_src.Get1SAggregateProbBeneficial(); }
  inline double Get1SAggregateProbDeleterious() const { return m_src.Get1SAggregateProbDeleterious(); }
  inline double Get1SAggregateProbNeutral() const { return m_src.Get1SAggregateProbNeutral(); }
  inline double Get1SAggregateProbLethal() const { return m_src.Get1SAggregateProbLethal(); }
  inline double Get1SAggregateAverageSizeBeneficial() const { return m_src.Get1SAggregateAverageSizeBeneficial(); }
  inline double Get1SAggregateAverageSizeDeleterious() const { return m_src.Get1SAggregateAverageSizeDeleterious(); }

//  inline double Get1SAggregateTotalEntropy() const { return m_src.Get1SAggregateTotalEntropy(); }
//  inline double Get1SAggregateComplexity() const { return m_src.Get1SAggregateComplexity(); }

  inline int Get1SAggregateTargetTask() const { return m_src.Get1SAggregateTargetTask(); }
  inline double Get1SAggregateProbTargetTask() const { return m_src.Get1SAggregateProbTargetTask(); }
  inline double Get1SAggregateAverageSizeTargetTask() const { return m_src.Get1SAggregateAverageSizeTargetTask(); }
  inline int Get1SAggregateTask() const { return m_src.Get1SAggregateTask(); }
  inline double Get1SAggregateProbTask() const { return m_src.Get1SAggregateProbTask(); }
  inline double Get1SAggregateAverageSizeTask() const { return m_src.Get1SAggregateAverageSizeTask(); }
  inline int Get1SAggregateKnockout() const { return m_src.Get1SAggregateKnockout(); }
  inline double Get1SAggregateProbKnockout() const { return m_src.Get1SAggregateProbKnockout(); }
  inline double Get1SAggregateAverageSizeKnockout() const { return m_src.Get1SAggregateAverageSizeKnockout(); }

  
  
  

  inline int Get1SPointTotal() const { return m_src.Get1SPointTotal(); }
  
  inline double Get1SPointAverageFitness() const { return m_src.Get1SPointAverageFitness(); }
  inline double Get1SPointAverageSqrFitness() const { return m_src.Get1SPointAverageSqrFitness(); }
  inline const cGenome& Get1SPointPeakGenome() const { return m_src.Get1SPointPeakGenome(); }
  inline double Get1SPointPeakFitness() const { return m_src.Get1SPointPeakFitness(); }
  
  inline double Get1SPointProbBeneficial() const { return m_src.Get1SPointProbBeneficial(); }
  inline double Get1SPointProbDeleterious() const { return m_src.Get1SPointProbDeleterious(); }
  inline double Get1SPointProbNeutral() const { return m_src.Get1SPointProbNeutral(); }
  inline double Get1SPointProbLethal() const { return m_src.Get1SPointProbLethal(); }
  inline double Get1SPointAverageSizeBeneficial() const { return m_src.Get1SPointAverageSizeBeneficial(); }
  inline double Get1SPointAverageSizeDeleterious() const { return m_src.Get1SPointAverageSizeDeleterious(); }
  
  inline double Get1SPointTotalEntropy() const { return m_src.Get1SPointTotalEntropy(); }
  inline double Get1SPointComplexity() const { return m_src.Get1SPointComplexity(); }
  
  inline int Get1SPointTargetTask() const { return m_src.Get1SPointTargetTask(); }
  inline double Get1SPointProbTargetTask() const { return m_src.Get1SPointProbTargetTask(); }
  inline double Get1SPointAverageSizeTargetTask() const { return m_src.Get1SPointAverageSizeTargetTask(); }
  inline int Get1SPointTask() const { return m_src.Get1SPointTask(); }
  inline double Get1SPointProbTask() const { return m_src.Get1SPointProbTask(); }
  inline double Get1SPointAverageSizeTask() const { return m_src.Get1SPointAverageSizeTask(); }
  inline int Get1SPointKnockout() const { return m_src.Get1SPointKnockout(); }
  inline double Get1SPointProbKnockout() const { return m_src.Get1SPointProbKnockout(); }
  inline double Get1SPointAverageSizeKnockout() const { return m_src.Get1SPointAverageSizeKnockout(); }
  
  
  
  
  
  inline int Get1SInsertTotal() const { return m_src.Get1SInsertTotal(); }
  
  inline double Get1SInsertAverageFitness() const { return m_src.Get1SInsertAverageFitness(); }
  inline double Get1SInsertAverageSqrFitness() const { return m_src.Get1SInsertAverageSqrFitness(); }
  inline const cGenome& Get1SInsertPeakGenome() const { return m_src.Get1SInsertPeakGenome(); }
  inline double Get1SInsertPeakFitness() const { return m_src.Get1SInsertPeakFitness(); }
  
  inline double Get1SInsertProbBeneficial() const { return m_src.Get1SInsertProbBeneficial(); }
  inline double Get1SInsertProbDeleterious() const { return m_src.Get1SInsertProbDeleterious(); }
  inline double Get1SInsertProbNeutral() const { return m_src.Get1SInsertProbNeutral(); }
  inline double Get1SInsertProbLethal() const { return m_src.Get1SInsertProbLethal(); }
  inline double Get1SInsertAverageSizeBeneficial() const { return m_src.Get1SInsertAverageSizeBeneficial(); }
  inline double Get1SInsertAverageSizeDeleterious() const { return m_src.Get1SInsertAverageSizeDeleterious(); }
  
  inline double Get1SInsertTotalEntropy() const { return m_src.Get1SInsertTotalEntropy(); }
  inline double Get1SInsertComplexity() const { return m_src.Get1SInsertComplexity(); }
  
  inline int Get1SInsertTargetTask() const { return m_src.Get1SInsertTargetTask(); }
  inline double Get1SInsertProbTargetTask() const { return m_src.Get1SInsertProbTargetTask(); }
  inline double Get1SInsertAverageSizeTargetTask() const { return m_src.Get1SInsertAverageSizeTargetTask(); }
  inline int Get1SInsertTask() const { return m_src.Get1SInsertTask(); }
  inline double Get1SInsertProbTask() const { return m_src.Get1SInsertProbTask(); }
  inline double Get1SInsertAverageSizeTask() const { return m_src.Get1SInsertAverageSizeTask(); }
  inline int Get1SInsertKnockout() const { return m_src.Get1SInsertKnockout(); }
  inline double Get1SInsertProbKnockout() const { return m_src.Get1SInsertProbKnockout(); }
  inline double Get1SInsertAverageSizeKnockout() const { return m_src.Get1SInsertAverageSizeKnockout(); }
  
  
  
  
  
  inline int Get1SDeleteTotal() const { return m_src.Get1SDeleteTotal(); }
  
  inline double Get1SDeleteAverageFitness() const { return m_src.Get1SDeleteAverageFitness(); }
  inline double Get1SDeleteAverageSqrFitness() const { return m_src.Get1SDeleteAverageSqrFitness(); }
  inline const cGenome& Get1SDeletePeakGenome() const { return m_src.Get1SDeletePeakGenome(); }
  inline double Get1SDeletePeakFitness() const { return m_src.Get1SDeletePeakFitness(); }
  
  inline double Get1SDeleteProbBeneficial() const { return m_src.Get1SDeleteProbBeneficial(); }
  inline double Get1SDeleteProbDeleterious() const { return m_src.Get1SDeleteProbDeleterious(); }
  inline double Get1SDeleteProbNeutral() const { return m_src.Get1SDeleteProbNeutral(); }
  inline double Get1SDeleteProbLethal() const { return m_src.Get1SDeleteProbLethal(); }
  inline double Get1SDeleteAverageSizeBeneficial() const { return m_src.Get1SDeleteAverageSizeBeneficial(); }
  inline double Get1SDeleteAverageSizeDeleterious() const { return m_src.Get1SDeleteAverageSizeDeleterious(); }
  
  inline double Get1SDeleteTotalEntropy() const { return m_src.Get1SDeleteTotalEntropy(); }
  inline double Get1SDeleteComplexity() const { return m_src.Get1SDeleteComplexity(); }
  
  inline int Get1SDeleteTargetTask() const { return m_src.Get1SDeleteTargetTask(); }
  inline double Get1SDeleteProbTargetTask() const { return m_src.Get1SDeleteProbTargetTask(); }
  inline double Get1SDeleteAverageSizeTargetTask() const { return m_src.Get1SDeleteAverageSizeTargetTask(); }
  inline int Get1SDeleteTask() const { return m_src.Get1SDeleteTask(); }
  inline double Get1SDeleteProbTask() const { return m_src.Get1SDeleteProbTask(); }
  inline double Get1SDeleteAverageSizeTask() const { return m_src.Get1SDeleteAverageSizeTask(); }
  inline int Get1SDeleteKnockout() const { return m_src.Get1SDeleteKnockout(); }
  inline double Get1SDeleteProbKnockout() const { return m_src.Get1SDeleteProbKnockout(); }
  inline double Get1SDeleteAverageSizeKnockout() const { return m_src.Get1SDeleteAverageSizeKnockout(); }
  
  
  
  
  
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
