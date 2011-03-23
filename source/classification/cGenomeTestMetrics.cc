/*
 *  cGenomeTestMetrics.cc
 *  Avida
 *
 *  Created by David Bryson on 8/13/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#include "cGenomeTestMetrics.h"

#include "cAvidaContext.h"
#include "cBioGroup.h"
#include "cHardwareManager.h"
#include "cGenome.h"
#include "cPhenotype.h"
#include "cTestCPU.h"
#include "cWorld.h"
#include "tAutoRelease.h"


cGenomeTestMetrics::cGenomeTestMetrics(cAvidaContext& ctx, cBioGroup* bg)
{
  tAutoRelease<cTestCPU> testcpu(ctx.GetWorld()->GetHardwareManager().CreateTestCPU());
  
  cCPUTestInfo test_info;
  testcpu->TestGenome(ctx, test_info, cGenome(bg->GetProperty("genome").AsString()));
  
  m_is_viable = test_info.IsViable();
  
  cPhenotype& phenotype = test_info.GetTestPhenotype();
  m_fitness = test_info.GetGenotypeFitness();
  m_colony_fitness = test_info.GetColonyFitness();
  m_merit = phenotype.GetMerit().GetDouble();
  m_executed_size = phenotype.GetExecutedSize();
  m_copied_size = phenotype.GetCopiedSize();
  m_gestation_time = phenotype.GetGestationTime();  
}



cGenomeTestMetrics* cGenomeTestMetrics::GetMetrics(cAvidaContext& ctx, cBioGroup* bg)
{
  cGenomeTestMetrics* metrics = bg->GetData<cGenomeTestMetrics>();
  if (!metrics && bg->HasProperty("genome")) {
    metrics = new cGenomeTestMetrics(ctx, bg);
    assert(metrics);
    bg->AttachData(metrics);
  }

  return metrics;
}
