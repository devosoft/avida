/*
 *  cPhenPlastGenotype.h
 *  Avida
 *
 *  Created by Matthew Rupp on 7/27/07.
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


#ifndef cPhenPlastGenotype_h
#define cPhenPlastGenotype_h

#include <set>
#include <utility>

#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cStringList_h
#include "cStringList.h"
#endif
#ifndef cStringUtil_h
#include "cStringUtil.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef cPlasticPhenotype_h
#include "cPlasticPhenotype.h"
#endif
#ifndef cPhenotype_h
#include "cPhenotype.h"
#endif
#ifndef cHardwareManager_h
#include "cHardwareManager.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif
#ifndef cEnvironment_h
#include "cEnvironment.h"
#endif
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

class cAvidaContext;
class cTestCPU;
class cWorld;
class cEnvironment;

/**
 * This class examines a genotype for evidence of phenotypic plasticity. 
**/
 

class cPhenPlastGenotype
{
  private:

    typedef set<cPhenotype*, cPhenotype::lt_phenotype  > UniquePhenotypes;  //Actually, these are cPlasticPhenotypes*
    tList<cPlasticPhenotype> m_plastic_phenotypes;  //This will store a list of our unique plastic phenotype pointers  
    cGenome m_genome;

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
    tArray<double> m_task_probabilities;
    
    
    
    void Process(cCPUTestInfo& test_info, cWorld* world, cAvidaContext& ctx);

  public:
    cPhenPlastGenotype(const cGenome& in_genome, int num_trails, cCPUTestInfo& test_info,  cWorld* world, cAvidaContext& ctx);
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
    tArray<double> GetTaskProbabilities() const { return m_task_probabilities; }
    double GetViableProbability() const { return m_viable_probability; }
};

#endif

