/*
 *  cPlasticCPUTestInfo.cpp
 *  Avida
 *
 *  Created by Matthew Rupp on 7/27/07
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

#include "cPlasticPhenotype.h"
#include "cCPUMemory.h"
#include "cOrganism.h"
#include "cHardwareBase.h"

bool cPlasticPhenotype::AddObservation( cCPUTestInfo& test_info )
{
  cPhenotype& test_phenotype = test_info.GetTestPhenotype();
  if (test_phenotype == *this ){
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
