/*
 *  cPhenPlastUtil.cc
 *  Avida
 *
 *  Created by David on 6/18/10.
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

#include "cPhenPlastUtil.h"

#include "cBGGenotype.h"
#include "cPhenPlastGenotype.h"
#include "cPhenPlastSummary.h"


int cPhenPlastUtil::GetNumPhenotypes(cAvidaContext& ctx, cWorld* world, cBioGroup* bg)
{
  cPhenPlastSummary* ps = bg->GetData<cPhenPlastSummary>();
  if (!ps) {
    assert(dynamic_cast<cBGGenotype*>(bg));
    
    ps = TestPlasticity(ctx, world, ((cBGGenotype*)bg)->GetGenome());
    bg->AttachData(ps);
  }
  
  return ps->m_num_phenotypes;
}

double cPhenPlastUtil::GetPhenotypicEntropy(cAvidaContext& ctx, cWorld* world, cBioGroup* bg)
{
  cPhenPlastSummary* ps = bg->GetData<cPhenPlastSummary>();
  if (!ps) {
    assert(dynamic_cast<cBGGenotype*>(bg));
    
    ps = TestPlasticity(ctx, world, ((cBGGenotype*)bg)->GetGenome());
    bg->AttachData(ps);
  }
  
  return ps->m_phenotypic_entropy;
}

double cPhenPlastUtil::GetTaskProbability(cAvidaContext& ctx, cWorld* world, cBioGroup* bg, int task_id)
{
  cPhenPlastSummary* ps = bg->GetData<cPhenPlastSummary>();
  if (!ps) {
    assert(dynamic_cast<cBGGenotype*>(bg));
    
    ps = TestPlasticity(ctx, world, ((cBGGenotype*)bg)->GetGenome());
    bg->AttachData(ps);
  }
  
  return ps->m_task_probabilities[task_id];
}

const tArray<double>& cPhenPlastUtil::GetTaskProbabilities(cAvidaContext& ctx, cWorld* world, cBioGroup* bg)
{
  cPhenPlastSummary* ps = bg->GetData<cPhenPlastSummary>();
  if (!ps) {
    assert(dynamic_cast<cBGGenotype*>(bg));
    
    ps = TestPlasticity(ctx, world, ((cBGGenotype*)bg)->GetGenome());
    bg->AttachData(ps);
  }
  
  return ps->m_task_probabilities;
}

cPhenPlastSummary* cPhenPlastUtil::TestPlasticity(cAvidaContext& ctx, cWorld* world, const Genome& mg)
{
  cCPUTestInfo test_info;
  // @TODO - phenplastgenotype should take a metagenome and pull its own config setting
  cPhenPlastGenotype pp(mg, world->GetConfig().GENOTYPE_PHENPLAST_CALC.Get(), test_info, world, ctx);
  return new cPhenPlastSummary(pp);
}
