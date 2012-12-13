/*
 *  private/systematics/GenomeTestMetrics.cc
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/private/systematics/GenomeTestMetrics.h"

#include "avida/core/Genome.h"

#include "cAvidaContext.h"
#include "cHardwareManager.h"
#include "cPhenotype.h"
#include "cTestCPU.h"
#include "cWorld.h"

const Apto::String Avida::Systematics::GenomeTestMetrics::ObjectKey("Avida::Systematics::GenomeTestMetrics");



Avida::Systematics::GenomeTestMetrics::GenomeTestMetrics(cWorld* world, cAvidaContext& ctx, GroupPtr g)
{
  Apto::SmartPtr<cTestCPU> testcpu(world->GetHardwareManager().CreateTestCPU(ctx));
  
  cCPUTestInfo test_info;
  testcpu->TestGenome(ctx, test_info, Genome(g->Properties().Get("genome").StringValue()));
  
  m_is_viable = test_info.IsViable();
  
  cPhenotype& phenotype = test_info.GetTestPhenotype();
  m_fitness = test_info.GetGenotypeFitness();
  m_colony_fitness = test_info.GetColonyFitness();
  m_merit = phenotype.GetMerit().GetDouble();
  m_executed_size = phenotype.GetExecutedSize();
  m_copied_size = phenotype.GetCopiedSize();
  m_gestation_time = phenotype.GetGestationTime();
  m_task_counts = phenotype.GetLastTaskCount();
}


Avida::Systematics::GenomeTestMetrics::~GenomeTestMetrics() { ; }


bool Avida::Systematics::GenomeTestMetrics::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::Systematics::GenomeTestMetricsPtr Avida::Systematics::GenomeTestMetrics::GetMetrics(cWorld* world, cAvidaContext& ctx,
                                                                                           GroupPtr g)
{
  GenomeTestMetricsPtr metrics = g->GetData<GenomeTestMetrics>();
  if (!metrics && g->Properties().Has("genome")) {
    metrics = GenomeTestMetricsPtr(new GenomeTestMetrics(world, ctx, g));
    assert(metrics);
    g->AttachData(metrics);
  }

  return metrics;
}
