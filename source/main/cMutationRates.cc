/*
 *  cMutationRates.cc
 *  Avida
 *
 *  Called "mutation_rates.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
  
  divide.divide_mut_prob = world->GetConfig().DIVIDE_MUT_PROB.Get();
  divide.divide_ins_prob = world->GetConfig().DIVIDE_INS_PROB.Get();
  divide.divide_del_prob = world->GetConfig().DIVIDE_DEL_PROB.Get();
  
  divide.divide_poisson_mut_mean = world->GetConfig().DIVIDE_POISSON_MUT_MEAN.Get();
  divide.divide_poisson_ins_mean = world->GetConfig().DIVIDE_POISSON_INS_MEAN.Get();
  divide.divide_poisson_del_mean = world->GetConfig().DIVIDE_POISSON_DEL_MEAN.Get();
  divide.divide_poisson_slip_mean = world->GetConfig().DIVIDE_POISSON_SLIP_MEAN.Get();

  divide.divide_uniform_prob = world->GetConfig().DIVIDE_UNIFORM_PROB.Get();
  divide.divide_slip_prob = world->GetConfig().DIVIDE_SLIP_PROB.Get();
  
  divide.parent_mut_prob = world->GetConfig().PARENT_MUT_PROB.Get();  
  
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
  
  divide.divide_mut_prob = 0.0;
  divide.divide_ins_prob = 0.0;
  divide.divide_del_prob = 0.0;
  
  divide.divide_poisson_mut_mean = 0.0;
  divide.divide_poisson_ins_mean = 0.0;
  divide.divide_poisson_del_mean = 0.0; 
  divide.divide_poisson_slip_mean = 0.0; 

  divide.divide_uniform_prob = 0.0;
  divide.divide_slip_prob = 0.0;
  
  divide.parent_mut_prob = 0.0;

  inject.ins_prob = 0.0;
  inject.del_prob = 0.0;
  inject.mut_prob = 0.0;
  
  meta.copy_mut_prob = 0.0;
  meta.standard_dev = 0.0;

  update.death_prob = 0.0;
}

void cMutationRates::Copy(const cMutationRates& in_muts)
{
  copy.mut_prob = in_muts.copy.mut_prob;
  copy.ins_prob = in_muts.copy.ins_prob;
  copy.del_prob = in_muts.copy.del_prob;
  copy.uniform_prob = in_muts.copy.uniform_prob;
  copy.slip_prob = in_muts.copy.slip_prob;
  
  divide.ins_prob = in_muts.divide.ins_prob;
  divide.del_prob = in_muts.divide.del_prob;
  divide.mut_prob = in_muts.divide.mut_prob;
  divide.uniform_prob = in_muts.divide.uniform_prob;
  divide.slip_prob = in_muts.divide.slip_prob;
  
  divide.divide_mut_prob = in_muts.divide.divide_mut_prob;
  divide.divide_ins_prob = in_muts.divide.divide_ins_prob;
  divide.divide_del_prob = in_muts.divide.divide_del_prob;
  
  divide.divide_poisson_mut_mean = in_muts.divide.divide_poisson_mut_mean;
  divide.divide_poisson_ins_mean = in_muts.divide.divide_poisson_ins_mean;
  divide.divide_poisson_del_mean = in_muts.divide.divide_poisson_del_mean;
  divide.divide_poisson_slip_mean = in_muts.divide.divide_poisson_slip_mean;

  divide.divide_uniform_prob = in_muts.divide.divide_uniform_prob;
  divide.divide_slip_prob = in_muts.divide.divide_slip_prob;
  
  divide.parent_mut_prob = in_muts.divide.parent_mut_prob;
  
  inject.ins_prob = in_muts.inject.ins_prob;
  inject.del_prob = in_muts.inject.del_prob;
  inject.mut_prob = in_muts.inject.mut_prob;
  
  meta.copy_mut_prob = in_muts.meta.copy_mut_prob;
  meta.standard_dev = in_muts.meta.standard_dev;

  update.death_prob = in_muts.update.death_prob;
}
