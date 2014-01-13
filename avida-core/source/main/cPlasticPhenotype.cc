/*
 *  cPlasticCPUTestInfo.cpp
 *  Avida
 *
 *  Created by Matthew Rupp on 7/27/07
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

#include "cPlasticPhenotype.h"
#include "cCPUMemory.h"
#include "cOrganism.h"
#include "cHardwareBase.h"

bool cPlasticPhenotype::AddObservation( cCPUTestInfo& test_info )
{
  cPhenotype& test_phenotype = test_info.GetTestPhenotype();
  if (cPhenotype::Compare(&test_phenotype, this) == 0 ) {   // Test if phenotypes are equal.
    if (m_num_observations == 0){
      m_env_inputs = test_info.GetTestCPUInputs();
      SetExecutedFlags(test_info);
      m_viable = test_info.IsViable();
    } 
    m_num_observations++;
    return true;
  }

  return false; //Wrong phenotype
}


void cPlasticPhenotype::SetExecutedFlags(cCPUTestInfo& test_info)
{
  cCPUMemory& cpu_memory = test_info.GetTestOrganism()->GetHardware().GetMemory();
  cString new_executed_flags;
  for (int i=0; i<cpu_memory.GetSize(); i++)
  {
    new_executed_flags += (cpu_memory.FlagExecuted(i)) ? "+" : "-";
  }
  m_executed_flags = new_executed_flags;
}
