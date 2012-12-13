/*
 *  cPhenPlastGenotype.h
 *  Avida
 *
 *  Created by Matthew Rupp on 7/27/07.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef cPhenPlastGenotype_h
#define cPhenPlastGenotype_h

#include "avida/core/Genome.h"
#include "avida/core/WorldDriver.h"

#include "cCPUMemory.h"
#include "cPlasticPhenotype.h"
#include "cPhenotype.h"
#include "cHardwareManager.h"
#include "cWorld.h"
#include "cEnvironment.h"

#include <set>
#include <utility>

class cAvidaContext;
class cTestCPU;
class cWorld;
class cEnvironment;

using namespace Avida;


class cPhenPlastGenotype
{
private:

  typedef set<cPhenotype*, cPhenotype::PhenotypeCompare  > UniquePhenotypes;  //Actually, these are cPlasticPhenotypes*
  tList<cPlasticPhenotype> m_plastic_phenotypes;  //This will store a list of our unique plastic phenotype pointers  
  Genome m_genome;
  
  int m_num_trials;  
  UniquePhenotypes m_unique;
  cWorld* m_world;
    
  double m_max_fitness;
  double m_avg_fitness;
  double m_likely_fitness;
  double m_phenotypic_entropy;
  double m_max_freq;
  double m_max_fit_freq;
  double m_min_fit_freq;
  double m_min_fitness;
  double m_viable_probability;
  Apto::Array<double> m_task_probabilities;
    
    
  
  void Process(cCPUTestInfo& test_info, cWorld* world, cAvidaContext& ctx);
  
public:
  cPhenPlastGenotype(const Genome& in_genome, int num_trails, cCPUTestInfo& test_info,  cWorld* world, cAvidaContext& ctx);
  ~cPhenPlastGenotype();
    
  // Accessors
  int    GetNumPhenotypes() const     { return m_unique.size();  }
  int    GetNumTrials() const         { return m_num_trials;     }
  double GetMaximumFitness() const    { return m_max_fitness;    }
  double GetMinimumFitness() const    { return m_min_fitness;    }
  double GetAverageFitness() const    { return m_avg_fitness;    }
  double GetLikelyFitness()  const    { return m_likely_fitness; }
  double GetPhenotypicEntropy() const { return m_phenotypic_entropy; }
  double GetMaximumFrequency() const  { return m_max_freq; }
  double GetMaximumFitnessFrequency() const {return m_max_fit_freq;}
  double GetMinimumFitnessFrequency() const {return m_min_fit_freq;}
  const cPlasticPhenotype* GetPlasticPhenotype(int num) const;
  const cPlasticPhenotype* GetMostLikelyPhenotype() const;
  const cPlasticPhenotype* GetHighestFitnessPhenotype() const;
  Apto::Array<double> GetTaskProbabilities() const { return m_task_probabilities; }
  double GetViableProbability() const { return m_viable_probability; }
};

#endif

