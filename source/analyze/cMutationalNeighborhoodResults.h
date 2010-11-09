/*
 *  cMutationalNeighborhoodResults.h
 *  Avida
 *
 *  Created by David Bryson on 6/21/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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
  
  
  
  
  
  inline int Get2SAggregateTotal() const { return m_src.Get2SAggregateTotal(); }
  
  inline double Get2SAggregateAverageFitness() const { return m_src.Get2SAggregateAverageFitness(); }
  inline double Get2SAggregateAverageSqrFitness() const { return m_src.Get2SAggregateAverageSqrFitness(); }
  inline const cGenome& Get2SAggregatePeakGenome() const { return m_src.Get2SAggregatePeakGenome(); }
  inline double Get2SAggregatePeakFitness() const { return m_src.Get2SAggregatePeakFitness(); }

  inline double Get2SAggregateProbBeneficial() const { return m_src.Get2SAggregateProbBeneficial(); }
  inline double Get2SAggregateProbDeleterious() const { return m_src.Get2SAggregateProbDeleterious(); }
  inline double Get2SAggregateProbNeutral() const { return m_src.Get2SAggregateProbNeutral(); }
  inline double Get2SAggregateProbLethal() const { return m_src.Get2SAggregateProbLethal(); }
  inline double Get2SAggregateAverageSizeBeneficial() const { return m_src.Get2SAggregateAverageSizeBeneficial(); }
  inline double Get2SAggregateAverageSizeDeleterious() const { return m_src.Get2SAggregateAverageSizeDeleterious(); }

//  inline double Get2SAggregateTotalEntropy() const { return m_src.Get2SAggregateTotalEntropy(); }
//  inline double Get2SAggregateComplexity() const { return m_src.Get2SAggregateComplexity(); }

  inline int Get2SAggregateTargetTask() const { return m_src.Get2SAggregateTargetTask(); }
  inline double Get2SAggregateProbTargetTask() const { return m_src.Get2SAggregateProbTargetTask(); }
  inline double Get2SAggregateAverageSizeTargetTask() const { return m_src.Get2SAggregateAverageSizeTargetTask(); }
  inline int Get2SAggregateTargetTaskBeneficial() const { return m_src.Get2SAggregateTargetTaskBeneficial(); }
  inline double Get2SAggregateProbTargetTaskBeneficial() const { return m_src.Get2SAggregateProbTargetTaskBeneficial(); }
  inline double Get2SAggregateAverageSizeTargetTaskBeneficial() const { return m_src.Get2SAggregateAverageSizeTargetTaskBeneficial(); }
  inline int Get2SAggregateTargetTaskDeleterious() const { return m_src.Get2SAggregateTargetTaskDeleterious(); }
  inline double Get2SAggregateProbTargetTaskDeleterious() const { return m_src.Get2SAggregateProbTargetTaskDeleterious(); }
  inline double Get2SAggregateAverageSizeTargetTaskDeleterious() const { return m_src.Get2SAggregateAverageSizeTargetTaskDeleterious(); }
  inline int Get2SAggregateTargetTaskNeutral() const { return m_src.Get2SAggregateTargetTaskNeutral(); }
  inline double Get2SAggregateProbTargetTaskNeutral() const { return m_src.Get2SAggregateProbTargetTaskNeutral(); }
  inline int Get2SAggregateTargetTaskLethal() const { return m_src.Get2SAggregateTargetTaskLethal(); }
  inline double Get2SAggregateProbTargetTaskLethal() const { return m_src.Get2SAggregateProbTargetTaskLethal(); }
  inline int Get2SAggregateTask() const { return m_src.Get2SAggregateTask(); }
  inline double Get2SAggregateProbTask() const { return m_src.Get2SAggregateProbTask(); }
  inline double Get2SAggregateAverageSizeTask() const { return m_src.Get2SAggregateAverageSizeTask(); }
  inline int Get2SAggregateKnockout() const { return m_src.Get2SAggregateKnockout(); }
  inline double Get2SAggregateProbKnockout() const { return m_src.Get2SAggregateProbKnockout(); }
  inline double Get2SAggregateAverageSizeKnockout() const { return m_src.Get2SAggregateAverageSizeKnockout(); }





  inline int Get2SPointTotal() const { return m_src.Get2SPointTotal(); }
  
  inline double Get2SPointAverageFitness() const { return m_src.Get2SPointAverageFitness(); }
  inline double Get2SPointAverageSqrFitness() const { return m_src.Get2SPointAverageSqrFitness(); }
  inline const cGenome& Get2SPointPeakGenome() const { return m_src.Get2SPointPeakGenome(); }
  inline double Get2SPointPeakFitness() const { return m_src.Get2SPointPeakFitness(); }
  
  inline double Get2SPointProbBeneficial() const { return m_src.Get2SPointProbBeneficial(); }
  inline double Get2SPointProbDeleterious() const { return m_src.Get2SPointProbDeleterious(); }
  inline double Get2SPointProbNeutral() const { return m_src.Get2SPointProbNeutral(); }
  inline double Get2SPointProbLethal() const { return m_src.Get2SPointProbLethal(); }
  inline double Get2SPointAverageSizeBeneficial() const { return m_src.Get2SPointAverageSizeBeneficial(); }
  inline double Get2SPointAverageSizeDeleterious() const { return m_src.Get2SPointAverageSizeDeleterious(); }
  
  inline double Get2SPointTotalEntropy() const { return m_src.Get2SPointTotalEntropy(); }
  inline double Get2SPointComplexity() const { return m_src.Get2SPointComplexity(); }
  
  inline int Get2SPointTargetTask() const { return m_src.Get2SPointTargetTask(); }
  inline double Get2SPointProbTargetTask() const { return m_src.Get2SPointProbTargetTask(); }
  inline double Get2SPointAverageSizeTargetTask() const { return m_src.Get2SPointAverageSizeTargetTask(); }
  inline int Get2SPointTargetTaskBeneficial() const { return m_src.Get2SPointTargetTaskBeneficial(); }
  inline double Get2SPointProbTargetTaskBeneficial() const { return m_src.Get2SPointProbTargetTaskBeneficial(); }
  inline double Get2SPointAverageSizeTargetTaskBeneficial() const { return m_src.Get2SPointAverageSizeTargetTaskBeneficial(); }
  inline int Get2SPointTargetTaskDeleterious() const { return m_src.Get2SPointTargetTaskDeleterious(); }
  inline double Get2SPointProbTargetTaskDeleterious() const { return m_src.Get2SPointProbTargetTaskDeleterious(); }
  inline double Get2SPointAverageSizeTargetTaskDeleterious() const { return m_src.Get2SPointAverageSizeTargetTaskDeleterious(); }
  inline int Get2SPointTargetTaskNeutral() const { return m_src.Get2SPointTargetTaskNeutral(); }
  inline double Get2SPointProbTargetTaskNeutral() const { return m_src.Get2SPointProbTargetTaskNeutral(); }
  inline int Get2SPointTargetTaskLethal() const { return m_src.Get2SPointTargetTaskLethal(); }
  inline double Get2SPointProbTargetTaskLethal() const { return m_src.Get2SPointProbTargetTaskLethal(); }
  inline int Get2SPointTask() const { return m_src.Get2SPointTask(); }
  inline double Get2SPointProbTask() const { return m_src.Get2SPointProbTask(); }
  inline double Get2SPointAverageSizeTask() const { return m_src.Get2SPointAverageSizeTask(); }
  inline int Get2SPointKnockout() const { return m_src.Get2SPointKnockout(); }
  inline double Get2SPointProbKnockout() const { return m_src.Get2SPointProbKnockout(); }
  inline double Get2SPointAverageSizeKnockout() const { return m_src.Get2SPointAverageSizeKnockout(); }





  inline int Get2SInsertTotal() const { return m_src.Get2SInsertTotal(); }
  
  inline double Get2SInsertAverageFitness() const { return m_src.Get2SInsertAverageFitness(); }
  inline double Get2SInsertAverageSqrFitness() const { return m_src.Get2SInsertAverageSqrFitness(); }
  inline const cGenome& Get2SInsertPeakGenome() const { return m_src.Get2SInsertPeakGenome(); }
  inline double Get2SInsertPeakFitness() const { return m_src.Get2SInsertPeakFitness(); }
  
  inline double Get2SInsertProbBeneficial() const { return m_src.Get2SInsertProbBeneficial(); }
  inline double Get2SInsertProbDeleterious() const { return m_src.Get2SInsertProbDeleterious(); }
  inline double Get2SInsertProbNeutral() const { return m_src.Get2SInsertProbNeutral(); }
  inline double Get2SInsertProbLethal() const { return m_src.Get2SInsertProbLethal(); }
  inline double Get2SInsertAverageSizeBeneficial() const { return m_src.Get2SInsertAverageSizeBeneficial(); }
  inline double Get2SInsertAverageSizeDeleterious() const { return m_src.Get2SInsertAverageSizeDeleterious(); }
  
  inline double Get2SInsertTotalEntropy() const { return m_src.Get2SInsertTotalEntropy(); }
  inline double Get2SInsertComplexity() const { return m_src.Get2SInsertComplexity(); }
  
  inline int Get2SInsertTargetTask() const { return m_src.Get2SInsertTargetTask(); }
  inline double Get2SInsertProbTargetTask() const { return m_src.Get2SInsertProbTargetTask(); }
  inline double Get2SInsertAverageSizeTargetTask() const { return m_src.Get2SInsertAverageSizeTargetTask(); }
  inline int Get2SInsertTargetTaskBeneficial() const { return m_src.Get2SInsertTargetTaskBeneficial(); }
  inline double Get2SInsertProbTargetTaskBeneficial() const { return m_src.Get2SInsertProbTargetTaskBeneficial(); }
  inline double Get2SInsertAverageSizeTargetTaskBeneficial() const { return m_src.Get2SInsertAverageSizeTargetTaskBeneficial(); }
  inline int Get2SInsertTargetTaskDeleterious() const { return m_src.Get2SInsertTargetTaskDeleterious(); }
  inline double Get2SInsertProbTargetTaskDeleterious() const { return m_src.Get2SInsertProbTargetTaskDeleterious(); }
  inline double Get2SInsertAverageSizeTargetTaskDeleterious() const { return m_src.Get2SInsertAverageSizeTargetTaskDeleterious(); }
  inline int Get2SInsertTargetTaskNeutral() const { return m_src.Get2SInsertTargetTaskNeutral(); }
  inline double Get2SInsertProbTargetTaskNeutral() const { return m_src.Get2SInsertProbTargetTaskNeutral(); }
  inline int Get2SInsertTargetTaskLethal() const { return m_src.Get2SInsertTargetTaskLethal(); }
  inline double Get2SInsertProbTargetTaskLethal() const { return m_src.Get2SInsertProbTargetTaskLethal(); }
  inline int Get2SInsertTask() const { return m_src.Get2SInsertTask(); }
  inline double Get2SInsertProbTask() const { return m_src.Get2SInsertProbTask(); }
  inline double Get2SInsertAverageSizeTask() const { return m_src.Get2SInsertAverageSizeTask(); }
  inline int Get2SInsertKnockout() const { return m_src.Get2SInsertKnockout(); }
  inline double Get2SInsertProbKnockout() const { return m_src.Get2SInsertProbKnockout(); }
  inline double Get2SInsertAverageSizeKnockout() const { return m_src.Get2SInsertAverageSizeKnockout(); }





  inline int Get2SDeleteTotal() const { return m_src.Get2SDeleteTotal(); }
  
  inline double Get2SDeleteAverageFitness() const { return m_src.Get2SDeleteAverageFitness(); }
  inline double Get2SDeleteAverageSqrFitness() const { return m_src.Get2SDeleteAverageSqrFitness(); }
  inline const cGenome& Get2SDeletePeakGenome() const { return m_src.Get2SDeletePeakGenome(); }
  inline double Get2SDeletePeakFitness() const { return m_src.Get2SDeletePeakFitness(); }
  
  inline double Get2SDeleteProbBeneficial() const { return m_src.Get2SDeleteProbBeneficial(); }
  inline double Get2SDeleteProbDeleterious() const { return m_src.Get2SDeleteProbDeleterious(); }
  inline double Get2SDeleteProbNeutral() const { return m_src.Get2SDeleteProbNeutral(); }
  inline double Get2SDeleteProbLethal() const { return m_src.Get2SDeleteProbLethal(); }
  inline double Get2SDeleteAverageSizeBeneficial() const { return m_src.Get2SDeleteAverageSizeBeneficial(); }
  inline double Get2SDeleteAverageSizeDeleterious() const { return m_src.Get2SDeleteAverageSizeDeleterious(); }
  
  inline double Get2SDeleteTotalEntropy() const { return m_src.Get2SDeleteTotalEntropy(); }
  inline double Get2SDeleteComplexity() const { return m_src.Get2SDeleteComplexity(); }
  
  inline int Get2SDeleteTargetTask() const { return m_src.Get2SDeleteTargetTask(); }
  inline double Get2SDeleteProbTargetTask() const { return m_src.Get2SDeleteProbTargetTask(); }
  inline double Get2SDeleteAverageSizeTargetTask() const { return m_src.Get2SDeleteAverageSizeTargetTask(); }
  inline int Get2SDeleteTargetTaskBeneficial() const { return m_src.Get2SDeleteTargetTaskBeneficial(); }
  inline double Get2SDeleteProbTargetTaskBeneficial() const { return m_src.Get2SDeleteProbTargetTaskBeneficial(); }
  inline double Get2SDeleteAverageSizeTargetTaskBeneficial() const { return m_src.Get2SDeleteAverageSizeTargetTaskBeneficial(); }
  inline int Get2SDeleteTargetTaskDeleterious() const { return m_src.Get2SDeleteTargetTaskDeleterious(); }
  inline double Get2SDeleteProbTargetTaskDeleterious() const { return m_src.Get2SDeleteProbTargetTaskDeleterious(); }
  inline double Get2SDeleteAverageSizeTargetTaskDeleterious() const { return m_src.Get2SDeleteAverageSizeTargetTaskDeleterious(); }
  inline int Get2SDeleteTargetTaskNeutral() const { return m_src.Get2SDeleteTargetTaskNeutral(); }
  inline double Get2SDeleteProbTargetTaskNeutral() const { return m_src.Get2SDeleteProbTargetTaskNeutral(); }
  inline int Get2SDeleteTargetTaskLethal() const { return m_src.Get2SDeleteTargetTaskLethal(); }
  inline double Get2SDeleteProbTargetTaskLethal() const { return m_src.Get2SDeleteProbTargetTaskLethal(); }
  inline int Get2SDeleteTask() const { return m_src.Get2SDeleteTask(); }
  inline double Get2SDeleteProbTask() const { return m_src.Get2SDeleteProbTask(); }
  inline double Get2SDeleteAverageSizeTask() const { return m_src.Get2SDeleteAverageSizeTask(); }
  inline int Get2SDeleteKnockout() const { return m_src.Get2SDeleteKnockout(); }
  inline double Get2SDeleteProbKnockout() const { return m_src.Get2SDeleteProbKnockout(); }
  inline double Get2SDeleteAverageSizeKnockout() const { return m_src.Get2SDeleteAverageSizeKnockout(); }





  inline int GetInsPntTotal() const { return m_src.GetInsPntTotal(); }
  
  inline double GetInsPntAverageFitness() const { return m_src.GetInsPntAverageFitness(); }
  inline double GetInsPntAverageSqrFitness() const { return m_src.GetInsPntAverageSqrFitness(); }
  inline const cGenome& GetInsPntPeakGenome() const { return m_src.GetInsPntPeakGenome(); }
  inline double GetInsPntPeakFitness() const { return m_src.GetInsPntPeakFitness(); }
  
  inline double GetInsPntProbBeneficial() const { return m_src.GetInsPntProbBeneficial(); }
  inline double GetInsPntProbDeleterious() const { return m_src.GetInsPntProbDeleterious(); }
  inline double GetInsPntProbNeutral() const { return m_src.GetInsPntProbNeutral(); }
  inline double GetInsPntProbLethal() const { return m_src.GetInsPntProbLethal(); }
  inline double GetInsPntAverageSizeBeneficial() const { return m_src.GetInsPntAverageSizeBeneficial(); }
  inline double GetInsPntAverageSizeDeleterious() const { return m_src.GetInsPntAverageSizeDeleterious(); }
  
  inline double GetInsPntTotalEntropy() const { return m_src.GetInsPntTotalEntropy(); }
  inline double GetInsPntComplexity() const { return m_src.GetInsPntComplexity(); }
  
  inline int GetInsPntTargetTask() const { return m_src.GetInsPntTargetTask(); }
  inline double GetInsPntProbTargetTask() const { return m_src.GetInsPntProbTargetTask(); }
  inline double GetInsPntAverageSizeTargetTask() const { return m_src.GetInsPntAverageSizeTargetTask(); }
  inline int GetInsPntTargetTaskBeneficial() const { return m_src.GetInsPntTargetTaskBeneficial(); }
  inline double GetInsPntProbTargetTaskBeneficial() const { return m_src.GetInsPntProbTargetTaskBeneficial(); }
  inline double GetInsPntAverageSizeTargetTaskBeneficial() const { return m_src.GetInsPntAverageSizeTargetTaskBeneficial(); }
  inline int GetInsPntTargetTaskDeleterious() const { return m_src.GetInsPntTargetTaskDeleterious(); }
  inline double GetInsPntProbTargetTaskDeleterious() const { return m_src.GetInsPntProbTargetTaskDeleterious(); }
  inline double GetInsPntAverageSizeTargetTaskDeleterious() const { return m_src.GetInsPntAverageSizeTargetTaskDeleterious(); }
  inline int GetInsPntTargetTaskNeutral() const { return m_src.GetInsPntTargetTaskNeutral(); }
  inline double GetInsPntProbTargetTaskNeutral() const { return m_src.GetInsPntProbTargetTaskNeutral(); }
  inline int GetInsPntTargetTaskLethal() const { return m_src.GetInsPntTargetTaskLethal(); }
  inline double GetInsPntProbTargetTaskLethal() const { return m_src.GetInsPntProbTargetTaskLethal(); }
  inline int GetInsPntTask() const { return m_src.GetInsPntTask(); }
  inline double GetInsPntProbTask() const { return m_src.GetInsPntProbTask(); }
  inline double GetInsPntAverageSizeTask() const { return m_src.GetInsPntAverageSizeTask(); }
  inline int GetInsPntKnockout() const { return m_src.GetInsPntKnockout(); }
  inline double GetInsPntProbKnockout() const { return m_src.GetInsPntProbKnockout(); }
  inline double GetInsPntAverageSizeKnockout() const { return m_src.GetInsPntAverageSizeKnockout(); }





  inline int GetInsDelTotal() const { return m_src.GetInsDelTotal(); }
  
  inline double GetInsDelAverageFitness() const { return m_src.GetInsDelAverageFitness(); }
  inline double GetInsDelAverageSqrFitness() const { return m_src.GetInsDelAverageSqrFitness(); }
  inline const cGenome& GetInsDelPeakGenome() const { return m_src.GetInsDelPeakGenome(); }
  inline double GetInsDelPeakFitness() const { return m_src.GetInsDelPeakFitness(); }
  
  inline double GetInsDelProbBeneficial() const { return m_src.GetInsDelProbBeneficial(); }
  inline double GetInsDelProbDeleterious() const { return m_src.GetInsDelProbDeleterious(); }
  inline double GetInsDelProbNeutral() const { return m_src.GetInsDelProbNeutral(); }
  inline double GetInsDelProbLethal() const { return m_src.GetInsDelProbLethal(); }
  inline double GetInsDelAverageSizeBeneficial() const { return m_src.GetInsDelAverageSizeBeneficial(); }
  inline double GetInsDelAverageSizeDeleterious() const { return m_src.GetInsDelAverageSizeDeleterious(); }
  
  inline double GetInsDelTotalEntropy() const { return m_src.GetInsDelTotalEntropy(); }
  inline double GetInsDelComplexity() const { return m_src.GetInsDelComplexity(); }
  
  inline int GetInsDelTargetTask() const { return m_src.GetInsDelTargetTask(); }
  inline double GetInsDelProbTargetTask() const { return m_src.GetInsDelProbTargetTask(); }
  inline double GetInsDelAverageSizeTargetTask() const { return m_src.GetInsDelAverageSizeTargetTask(); }
  inline int GetInsDelTargetTaskBeneficial() const { return m_src.GetInsDelTargetTaskBeneficial(); }
  inline double GetInsDelProbTargetTaskBeneficial() const { return m_src.GetInsDelProbTargetTaskBeneficial(); }
  inline double GetInsDelAverageSizeTargetTaskBeneficial() const { return m_src.GetInsDelAverageSizeTargetTaskBeneficial(); }
  inline int GetInsDelTargetTaskDeleterious() const { return m_src.GetInsDelTargetTaskDeleterious(); }
  inline double GetInsDelProbTargetTaskDeleterious() const { return m_src.GetInsDelProbTargetTaskDeleterious(); }
  inline double GetInsDelAverageSizeTargetTaskDeleterious() const { return m_src.GetInsDelAverageSizeTargetTaskDeleterious(); }
  inline int GetInsDelTargetTaskNeutral() const { return m_src.GetInsDelTargetTaskNeutral(); }
  inline double GetInsDelProbTargetTaskNeutral() const { return m_src.GetInsDelProbTargetTaskNeutral(); }
  inline int GetInsDelTargetTaskLethal() const { return m_src.GetInsDelTargetTaskLethal(); }
  inline double GetInsDelProbTargetTaskLethal() const { return m_src.GetInsDelProbTargetTaskLethal(); }
  inline int GetInsDelTask() const { return m_src.GetInsDelTask(); }
  inline double GetInsDelProbTask() const { return m_src.GetInsDelProbTask(); }
  inline double GetInsDelAverageSizeTask() const { return m_src.GetInsDelAverageSizeTask(); }
  inline int GetInsDelKnockout() const { return m_src.GetInsDelKnockout(); }
  inline double GetInsDelProbKnockout() const { return m_src.GetInsDelProbKnockout(); }
  inline double GetInsDelAverageSizeKnockout() const { return m_src.GetInsDelAverageSizeKnockout(); }





  inline int GetDelPntTotal() const { return m_src.GetDelPntTotal(); }
  
  inline double GetDelPntAverageFitness() const { return m_src.GetDelPntAverageFitness(); }
  inline double GetDelPntAverageSqrFitness() const { return m_src.GetDelPntAverageSqrFitness(); }
  inline const cGenome& GetDelPntPeakGenome() const { return m_src.GetDelPntPeakGenome(); }
  inline double GetDelPntPeakFitness() const { return m_src.GetDelPntPeakFitness(); }
  
  inline double GetDelPntProbBeneficial() const { return m_src.GetDelPntProbBeneficial(); }
  inline double GetDelPntProbDeleterious() const { return m_src.GetDelPntProbDeleterious(); }
  inline double GetDelPntProbNeutral() const { return m_src.GetDelPntProbNeutral(); }
  inline double GetDelPntProbLethal() const { return m_src.GetDelPntProbLethal(); }
  inline double GetDelPntAverageSizeBeneficial() const { return m_src.GetDelPntAverageSizeBeneficial(); }
  inline double GetDelPntAverageSizeDeleterious() const { return m_src.GetDelPntAverageSizeDeleterious(); }
  
  inline double GetDelPntTotalEntropy() const { return m_src.GetDelPntTotalEntropy(); }
  inline double GetDelPntComplexity() const { return m_src.GetDelPntComplexity(); }
  
  inline int GetDelPntTargetTask() const { return m_src.GetDelPntTargetTask(); }
  inline double GetDelPntProbTargetTask() const { return m_src.GetDelPntProbTargetTask(); }
  inline double GetDelPntAverageSizeTargetTask() const { return m_src.GetDelPntAverageSizeTargetTask(); }
  inline int GetDelPntTargetTaskBeneficial() const { return m_src.GetDelPntTargetTaskBeneficial(); }
  inline double GetDelPntProbTargetTaskBeneficial() const { return m_src.GetDelPntProbTargetTaskBeneficial(); }
  inline double GetDelPntAverageSizeTargetTaskBeneficial() const { return m_src.GetDelPntAverageSizeTargetTaskBeneficial(); }
  inline int GetDelPntTargetTaskDeleterious() const { return m_src.GetDelPntTargetTaskDeleterious(); }
  inline double GetDelPntProbTargetTaskDeleterious() const { return m_src.GetDelPntProbTargetTaskDeleterious(); }
  inline double GetDelPntAverageSizeTargetTaskDeleterious() const { return m_src.GetDelPntAverageSizeTargetTaskDeleterious(); }
  inline int GetDelPntTargetTaskNeutral() const { return m_src.GetDelPntTargetTaskNeutral(); }
  inline double GetDelPntProbTargetTaskNeutral() const { return m_src.GetDelPntProbTargetTaskNeutral(); }
  inline int GetDelPntTargetTaskLethal() const { return m_src.GetDelPntTargetTaskLethal(); }
  inline double GetDelPntProbTargetTaskLethal() const { return m_src.GetDelPntProbTargetTaskLethal(); }
  inline int GetDelPntTask() const { return m_src.GetDelPntTask(); }
  inline double GetDelPntProbTask() const { return m_src.GetDelPntProbTask(); }
  inline double GetDelPntAverageSizeTask() const { return m_src.GetDelPntAverageSizeTask(); }
  inline int GetDelPntKnockout() const { return m_src.GetDelPntKnockout(); }
  inline double GetDelPntProbKnockout() const { return m_src.GetDelPntProbKnockout(); }
  inline double GetDelPntAverageSizeKnockout() const { return m_src.GetDelPntAverageSizeKnockout(); }
};

#endif
