/*
 *  cPhenPlastGenotype.cpp
 *  Avida
 *
 *  Created by Matthew Rupp on 7/29/07.
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

#include "cPhenPlastGenotype.h"
#include <iostream>

cPhenPlastGenotype::cPhenPlastGenotype(const cGenome& in_genome, int num_trials, cWorld* world, cAvidaContext& ctx)
: m_genome(in_genome), m_num_trials(num_trials), m_world(world)
{
  cTestCPU* test_cpu = m_world->GetHardwareManager().CreateTestCPU();
  for (int k = 0; k < m_num_trials; k++){
    cCPUTestInfo test_info;
    test_info.UseRandomInputs(true);
    test_cpu->TestGenome(ctx, test_info, m_genome);
    
    //Is this a new phenotype?
    UniquePhenotypes::iterator uit = m_unique.find(&test_info.GetTestPhenotype());
    if (uit == m_unique.end()){  // Yes, make a new entry for it
      cPlasticPhenotype* new_phen = new cPlasticPhenotype(test_info, m_num_trials);
      m_unique.insert( static_cast<cPhenotype*>(new_phen) );
    } else{   // No, add an observation to existing entry, make sure it is equivalent
      assert( static_cast<cPlasticPhenotype*>((*uit))->AddObservation(test_info) );
    }
  }
  delete test_cpu;
}

cPhenPlastGenotype::~cPhenPlastGenotype()
{
  UniquePhenotypes::iterator it = m_unique.begin();
  while (it != m_unique.end()){
    delete *it;
    ++it;
  }
}


cPlasticPhenotype cPhenPlastGenotype::GetPlasticPhenotype(int num) const
{
  assert(num >= 0 && num < (int) m_unique.size());
  UniquePhenotypes::iterator it = m_unique.begin();
  for (int k = 0; k < num; k++, it++);
  return *static_cast<cPlasticPhenotype*>(*it);
}

