/*
 *  cPlasticCPUTestInfo.cpp
 *  Avida
 *
 *  Created by Matthew Rupp on 7/27/07
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

#include "cPlasticPhenotype.h"


bool cPlasticPhenotype::AddObservation( const cPhenotype& in_phen, const tArray<int>& env_inputs )
{
  if (in_phen == *this){
    if (m_num_observations == 0)
      m_env_inputs.Resize(1, env_inputs.GetSize());
    else
      m_env_inputs.Resize(m_env_inputs.GetNumRows()+1, env_inputs.GetSize());
    m_env_inputs[m_env_inputs.GetNumRows()-1] = env_inputs;
    m_num_observations++;
    return true;
  }
  return false;  //Wrong phenotype
}

bool cPlasticPhenotype::AddObservation( cCPUTestInfo& test_info )
{
  tArray<int> env_inputs = test_info.GetTestCPUInputs();
  cPhenotype& test_phenotype = test_info.GetTestPhenotype();
  if (test_phenotype == *this ){
    if (m_num_observations == 0)
      m_env_inputs.Resize(1, env_inputs.GetSize());
    else
      m_env_inputs.Resize(m_env_inputs.GetNumRows()+1, env_inputs.GetSize());
    m_env_inputs[m_env_inputs.GetNumRows()-1] = env_inputs;
    m_num_observations++;
    return true;
  }
  return false; //Wrong phenotype
}
