/*
 *  cMutationRates.cc
 *  Avida
 *
 *  Called "mutation_rates.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cMutationRates.h"

#include "cWorld.h"
#include "cAvidaConfig.h"


void cMutationRates::Setup(cWorld* world)
{
  exec.point_mut_prob = world->GetConfig().POINT_MUT_PROB.Get();
  copy.mut_prob = world->GetConfig().COPY_MUT_PROB.Get();
  divide.ins_prob = world->GetConfig().INS_MUT_PROB.Get();
  divide.del_prob = world->GetConfig().DEL_MUT_PROB.Get();
  divide.mut_prob = world->GetConfig().DIV_MUT_PROB.Get();
  divide.divide_mut_prob = world->GetConfig().DIVIDE_MUT_PROB.Get();
  divide.divide_ins_prob = world->GetConfig().DIVIDE_INS_PROB.Get();
  divide.divide_del_prob = world->GetConfig().DIVIDE_DEL_PROB.Get();
  divide.parent_mut_prob = world->GetConfig().PARENT_MUT_PROB.Get();  
  inject.ins_prob = world->GetConfig().INJECT_INS_PROB.Get();
  inject.del_prob = world->GetConfig().INJECT_DEL_PROB.Get();
  inject.mut_prob = world->GetConfig().INJECT_MUT_PROB.Get();
}

void cMutationRates::Clear()
{
  exec.point_mut_prob = 0.0;
  copy.mut_prob = 0.0;
  divide.ins_prob = 0.0;
  divide.del_prob = 0.0;
  divide.mut_prob = 0.0;
  divide.divide_mut_prob = 0.0;
  divide.divide_ins_prob = 0.0;
  divide.divide_del_prob = 0.0;
  divide.parent_mut_prob = 0.0;
  inject.ins_prob = 0.0;
  inject.del_prob = 0.0;
  inject.mut_prob = 0.0;
}

void cMutationRates::Copy(const cMutationRates& in_muts)
{
  exec.point_mut_prob = in_muts.exec.point_mut_prob;
  copy.mut_prob = in_muts.copy.mut_prob;
  divide.ins_prob = in_muts.divide.ins_prob;
  divide.del_prob = in_muts.divide.del_prob;
  divide.mut_prob = in_muts.divide.mut_prob;
  divide.divide_mut_prob = in_muts.divide.divide_mut_prob;
  divide.divide_ins_prob = in_muts.divide.divide_ins_prob;
  divide.divide_del_prob = in_muts.divide.divide_del_prob;
  divide.parent_mut_prob = in_muts.divide.parent_mut_prob;
  inject.ins_prob = in_muts.inject.ins_prob;
  inject.del_prob = in_muts.inject.del_prob;
  inject.mut_prob = in_muts.inject.mut_prob;
}
