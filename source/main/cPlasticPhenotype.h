/*
 *  cPlasticCPUTestInfo.h
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

#ifndef cPlasticCPUTestInfo
#define cPlasticCPUTestInfo


#include <cassert>

#ifndef cCPUTestInfo_h
#include "cCPUTestInfo.h"
#endif

#ifndef cPhenotype_h
#include "cPhenotype.h"
#endif

#ifndef tArray_h
#include "tArray.h"
#endif


#include <iostream>

class cPlasticPhenotype : public cPhenotype{

  private:
    int m_num_observations;
    int m_num_trials;
    tArray<int> m_env_inputs;
    
    //Information retrieved from test_info not available in phenotype
    cString m_executed_flags;
    int m_viable;
    
    void SetExecutedFlags(cCPUTestInfo& test_info);
    
  public:
      cPlasticPhenotype(cCPUTestInfo& test_info, int num_trials) : 
        cPhenotype(test_info.GetTestPhenotype()), m_num_observations(0), m_num_trials(num_trials) 
        { assert(m_num_trials > 0); AddObservation(test_info);}
    
    ~cPlasticPhenotype() { ; }
    
    //Modifiers
    bool AddObservation(  cCPUTestInfo& test_info );
    
    //Accessors
    int GetNumObservations()      const { return m_num_observations; }
    int GetNumTrials()            const { return m_num_trials; }
    double GetFrequency()         const { return static_cast<double>(m_num_observations) / m_num_trials; }
    tArray<int> GetEnvInputs()   const { return m_env_inputs; }
    int IsViable()               const { return m_viable; }
    cString GetExecutedFlags()    const { return m_executed_flags; }
  

};

#endif

