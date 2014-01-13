/*
 *  cMutationRates.cc
 *  Avida
 *
 *  Called "mutation_rates.cc" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cMutationRates.h"

#include "cWorld.h"
#include "cAvidaConfig.h"


void cMutationRates::Setup(cWorld* world)
{
  copy.mut_prob = world->GetConfig().COPY_MUT_PROB.Get();
  copy.ins_prob = world->GetConfig().COPY_INS_PROB.Get();
  copy.del_prob = world->GetConfig().COPY_DEL_PROB.Get();
  copy.uniform_prob = world->GetConfig().COPY_UNIFORM_PROB.Get();
  copy.slip_prob = world->GetConfig().COPY_SLIP_PROB.Get();
  
  divide.ins_prob = world->GetConfig().DIV_INS_PROB.Get();
  divide.del_prob = world->GetConfig().DIV_DEL_PROB.Get();
  divide.mut_prob = world->GetConfig().DIV_MUT_PROB.Get();
  divide.uniform_prob = world->GetConfig().DIV_UNIFORM_PROB.Get();
  divide.slip_prob = world->GetConfig().DIV_SLIP_PROB.Get();
  divide.trans_prob = world->GetConfig().DIV_TRANS_PROB.Get();
  divide.lgt_prob = world->GetConfig().DIV_LGT_PROB.Get();
  
  divide.divide_mut_prob = world->GetConfig().DIVIDE_MUT_PROB.Get();
  divide.divide_ins_prob = world->GetConfig().DIVIDE_INS_PROB.Get();
  divide.divide_del_prob = world->GetConfig().DIVIDE_DEL_PROB.Get();
  divide.divide_uniform_prob = world->GetConfig().DIVIDE_UNIFORM_PROB.Get();
  divide.divide_slip_prob = world->GetConfig().DIVIDE_SLIP_PROB.Get();
  divide.divide_trans_prob = world->GetConfig().DIVIDE_TRANS_PROB.Get();
  divide.divide_lgt_prob = world->GetConfig().DIVIDE_LGT_PROB.Get();
  
  divide.divide_poisson_mut_mean = world->GetConfig().DIVIDE_POISSON_MUT_MEAN.Get();
  divide.divide_poisson_ins_mean = world->GetConfig().DIVIDE_POISSON_INS_MEAN.Get();
  divide.divide_poisson_del_mean = world->GetConfig().DIVIDE_POISSON_DEL_MEAN.Get();
  divide.divide_poisson_slip_mean = world->GetConfig().DIVIDE_POISSON_SLIP_MEAN.Get();
  divide.divide_poisson_trans_mean = world->GetConfig().DIVIDE_POISSON_TRANS_MEAN.Get();
  divide.divide_poisson_lgt_mean = world->GetConfig().DIVIDE_POISSON_LGT_MEAN.Get();
  
  divide.parent_mut_prob = world->GetConfig().PARENT_MUT_PROB.Get();  
  divide.parent_ins_prob = world->GetConfig().PARENT_INS_PROB.Get();
  divide.parent_del_prob = world->GetConfig().PARENT_DEL_PROB.Get();
  
  point.ins_prob = world->GetConfig().POINT_INS_PROB.Get();
  point.del_prob = world->GetConfig().POINT_DEL_PROB.Get();
  point.mut_prob = world->GetConfig().POINT_MUT_PROB.Get();

  inject.ins_prob = world->GetConfig().INJECT_INS_PROB.Get();
  inject.del_prob = world->GetConfig().INJECT_DEL_PROB.Get();
  inject.mut_prob = world->GetConfig().INJECT_MUT_PROB.Get();
  
  meta.copy_mut_prob = world->GetConfig().META_COPY_MUT.Get();
  meta.standard_dev = world->GetConfig().META_STD_DEV.Get();

  update.death_prob = world->GetConfig().DEATH_PROB.Get();  
}

void cMutationRates::Clear()
{
  copy.mut_prob = 0.0;
  copy.ins_prob = 0.0;
  copy.del_prob = 0.0;
  copy.uniform_prob = 0.0;
  copy.slip_prob = 0.0;
  
  divide.ins_prob = 0.0;
  divide.del_prob = 0.0;
  divide.mut_prob = 0.0;
  divide.uniform_prob = 0.0;
  divide.slip_prob = 0.0;
  divide.trans_prob = 0.0;
  divide.lgt_prob = 0.0;
  
  divide.divide_mut_prob = 0.0;
  divide.divide_ins_prob = 0.0;
  divide.divide_del_prob = 0.0;
  divide.divide_uniform_prob = 0.0;
  divide.divide_slip_prob = 0.0;
  divide.divide_trans_prob = 0.0;
  divide.divide_lgt_prob = 0.0;
  
  divide.divide_poisson_mut_mean = 0.0;
  divide.divide_poisson_ins_mean = 0.0;
  divide.divide_poisson_del_mean = 0.0;
  divide.divide_poisson_slip_mean = 0.0;
  divide.divide_poisson_trans_mean = 0.0;
  divide.divide_poisson_lgt_mean = 0.0;
  
  divide.parent_mut_prob = 0.0;
  divide.parent_ins_prob = 0.0;
  divide.parent_del_prob = 0.0;

  point.ins_prob = 0.0;
  point.del_prob = 0.0;
  point.mut_prob = 0.0;
  
  inject.ins_prob = 0.0;
  inject.del_prob = 0.0;
  inject.mut_prob = 0.0;
  
  meta.copy_mut_prob = 0.0;
  meta.standard_dev = 0.0;

  update.death_prob = 0.0;
}

void cMutationRates::Copy(const cMutationRates& in_muts)
{
  copy = in_muts.copy;
  divide = in_muts.divide;
  point = in_muts.point;
  inject = in_muts.inject;
  meta = in_muts.meta;
  update = in_muts.update;
}
